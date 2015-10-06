#!/bin/bash

function help {
	echo -e "SCRIPT TO DEPLOY QUICKFIT on WINDOWS (32/64bit)\n"
	echo -e "    (c) 2012-2013 by Jan W. Krieger\n"
	echo -e "  This script will create binary deployments of QuickFit. Either as"
	echo -e "  ZIP archive that the user has to unzip or as a windows installer"
	echo -e "  created with NSIS (http://nsis.sourceforge.net/Main_Page)."
	echo -e "  It also determines whether the build system is 32-bit or 64-bit "
	echo -e "  and configures the installer contents/filenames accordingly."
	echo -e "\ncommand line options:"
	echo -e "    -? --help        this help message"
	echo -e "    --nomake/--make  switch off/on a qmake|make|make install run" 
	echo -e "                     before deployment               (default: off)"
	echo -e "    --nozip/--zip    switch off/on creation of ZIP archives for " 
	echo -e "                     deployment                       (default: on)"
	echo -e "    --nonsis/--nsis  run NSIS compiler to generate installer before" 
	echo -e "                     script finishes                  (default: on)"
	echo -e "    --nodeldeploy    do not clean up at the end of the process" 
	echo -e "                     (delete deploy directory)        (default: on)"
	echo -e "    --nocreatedeploy create deploy directory at start of script" 
	echo -e "                                                      (default: on)"
}

function module_prepare_cleandeploy {
	rm -rf $1 &>/dev/null
	cp "quickfit3_win${BITDEPTH}_${SVNVER}_${1}.zip" "quickfit3_win${BITDEPTH}_${SVNVER}_${1}.zip.backup" &>/dev/null
	rm -f "quickfit3_win${BITDEPTH}_${SVNVER}_${1}.zip" &>/dev/null
}

function module_final_cleandeploy {
	rm -rf $1 &>/dev/null
	rm -f "quickfit3_win${BITDEPTH}_${SVNVER}_${1}.zip.backup" &>/dev/null
}

function main_final_cleandeploy {
	rm -rf $1 &>/dev/null
	rm -f "quickfit3_win${BITDEPTH}_${SVNVER}.zip.backup" &>/dev/null
}


function module_prepare {
	echo "PREPARE deploy_$1"
	mkdir ../deploy_$1
	mkdir ../deploy_$1/plugins
	mkdir ../deploy_$1/fitfunctionplugins
	mkdir ../deploy_$1/globalconfig_templates
	mkdir ../deploy_$1/source
	mkdir ../deploy_$1/assets
	mkdir ../deploy_$1/sdk
	mkdir ../deploy_$1/sdk/fitfunctions/
	mkdir ../deploy_$1/examples
	mkdir ../deploy_$1/assets/plugins
	mkdir ../deploy_$1/assets/plugins/help
	cp -rf ./sdk/fitfunctions/* ../deploy_$1/sdk/fitfunctions &>/dev/null
	cp -rf ./globalconfig_templates/* ../deploy_$1/globalconfig_templates &>/dev/null

	local PLUGINS=
	eval PLUGINS=\$$1
	echo "    --> PLUGINS: $PLUGINS"
	
	for f in $PLUGINS
	do
		if [ -d "./assets/plugins/${f}" ]; then
			echo "    --> MAKING DIR ../deploy_$1/assets/plugins/${f}"
			mkdir "../deploy_$1/assets/plugins/${f}" &>/dev/null
			cp -rf  "./assets/plugins/${f}" "../deploy_$1/assets/plugins/" &>/dev/null
		fi	
		if [ -d "./assets/plugins/help/${f}" ]; then
			echo "    --> MAKING DIR ../deploy_$1/assets/plugins/help/${f}"
			mkdir "../deploy_$1/assets/plugins/help/${f}" &>/dev/null
			cp -rf  "./assets/plugins/help/${f}" "../deploy_$1/assets/plugins/help/" &>/dev/null
		fi	
		if [ -d "./examples/${f}" ]; then
			echo "    --> MAKING DIR ../deploy_$1/examples/${f}"
			mkdir "../deploy_$1/examples/${f}" &>/dev/null
			cp -rf  "./examples/${f}" "../deploy_$1/examples/" &>/dev/null
		fi	
		find -name "${f}.*" -exec cp -rf "{}" "../deploy_$1/{}" \;
	done

}

function module_deploy_zip {
    cd deploy_$1
	echo "   --> quickfit3_win${BITDEPTH}_${SVNVER}_${1}.zip"
	zip -rv9 ../"quickfit3_win${BITDEPTH}_${SVNVER}_${1}.zip" *
	cd ..
}
function module_deploy_mainzip {
    cd $1
	echo "   --> ${ZIPFILE}"
	zip -rv9 ../${ZIPFILE} *
	cd ..
}

function module_deploy_nsis {

	echo "DEPLOY $1"
	if [ "$1" != "deploymain" ]; then
		cd deploy_$1
	else
		cd $1
	fi
	local LINSTALLER_FILES=
	local LUNINSTALLER_FILES=
	for f in `find . -type f`
	do
		fn=${f//\//\\\\}
		fn=${fn//.\\\\/}
		LINSTALLER_FILES="${LINSTALLER_FILES}\\
	File \"\\/oname=$fn\" \"$fn\""
		LUNINSTALLER_FILES="${LUNINSTALLER_FILES}\\
	Delete \"\$INSTDIR\\\\$fn\""
	done
	local LINSTALLER_DIRS=
	for f in `find . -type d`
	do
		fn=${f//\//\\\\}
		fn=${fn//.\\/}
		LINSTALLER_DIRS="${LINSTALLER_DIRS}\\
	CreateDirectory  \"\$INSTDIR\\$fn\""
		
	done
	cd ..
	
	local SEC_ID=$1
	local SEC_NAME=
	eval SEC_NAME=\${SECTIONNAME_$1}
	local SEC_MODE=
	eval SEC_MODE=\${SECTIONMODE_$1}
	local IN_SEC=
	eval IN_SEC=\${INSECTION_$1}
	
	if [ "$1" != "deploymain" ]; then
		echo "   --> section.ID= $SEC_ID"
		echo "   --> section.NAME= $SEC_NAME"
		echo "   --> section.MODE= $SEC_MODE"
		echo "   --> section.IN_SEC= $IN_SEC"
		
		echo "    step 1"
		if [ "$SEC_MODE" == "ro" ]; then
			sed "s/%%SEC_MODES%%/ IntOp \$0 \${SF_RO} | \${SF_SELECTED}\\
			SectionSetFlags \${sec_$SEC_ID} \$0\\
			%%SEC_MODES%%/" nsis_basicscript.~si > nsis_basicscript.~~s
			cp -f nsis_basicscript.~~s nsis_basicscript.~si
			ls -l *.~* &>/dev/null
		elif [ "$SEC_MODE" == "b" ]; then
			sed "s/%%SEC_MODES%%/ IntOp \$0 \${SF_BOLD} | \${SF_BOLD}\\
			SectionSetFlags \${sec_$SEC_ID} \$0\\
			%%SEC_MODES%%/" nsis_basicscript.~si > nsis_basicscript.~~s
			cp -f nsis_basicscript.~~s nsis_basicscript.~si
			ls -l *.~* &>/dev/null
		elif [ "$SEC_MODE" == "bs" ]; then
			sed "s/%%SEC_MODES%%/ IntOp \$0 \${SF_SELECTED} | \${SF_BOLD}\\
			SectionSetFlags \${sec_$SEC_ID} \$0\\
			%%SEC_MODES%%/" nsis_basicscript.~si > nsis_basicscript.~~s
			cp -f nsis_basicscript.~~s nsis_basicscript.~si
			ls -l *.~* &>/dev/null
		elif [ "$SEC_MODE" == "s" ]; then
			sed "s/%%SEC_MODES%%/ IntOp \$0 \${SF_SELECTED} | \${SF_SELECTED}\\
			SectionSetFlags \${sec_$SEC_ID} \$0\\
			%%SEC_MODES%%/" nsis_basicscript.~si > nsis_basicscript.~~s
			cp -f nsis_basicscript.~~s nsis_basicscript.~si
			ls -l *.~* &>/dev/null
		else
			sed "s/%%SEC_MODES%%/ SectionSetFlags \${sec_$SEC_ID} 0\\
			%%SEC_MODES%%/" nsis_basicscript.~si > nsis_basicscript.~~s
			cp -f nsis_basicscript.~~s nsis_basicscript.~si
			ls -l *.~* &>/dev/null
		fi
		
		cp nsis_sectiontemplate.nsi nsis_sectiontemplate$1.~ns
		echo "    step 2"
		sed "s/%%SEC_ID%%/$SEC_ID/" nsis_sectiontemplate$1.~ns > nsis_sectiontemplate$1.~~s
		cp -f nsis_sectiontemplate$1.~~s nsis_sectiontemplate$1.~ns
		ls -l *.~* &>/dev/null
		echo "    step 3"
		sed "s/%%SEC_NAME%%/$SEC_NAME/" nsis_sectiontemplate$1.~ns > nsis_sectiontemplate$1.~~s
		cp -f nsis_sectiontemplate$1.~~s nsis_sectiontemplate$1.~ns
		ls -l *.~* &>/dev/null
		echo "    step 4"
		sed "s/%%INSTALLER_DIRS%%/$LINSTALLER_DIRS/" nsis_sectiontemplate$1.~ns > nsis_sectiontemplate$1.~~s
		cp -f nsis_sectiontemplate$1.~~s nsis_sectiontemplate$1.~ns
		ls -l *.~* &>/dev/null
		echo "    step 5"
		sed "s/%%INSTALLER_FILES%%/$LINSTALLER_FILES/" nsis_sectiontemplate$1.~ns > nsis_sectiontemplate$1.~~s
		cp -f nsis_sectiontemplate$1.~~s nsis_sectiontemplate$1.~ns
		#cp nsis_sectiontemplate$1.~ns nsis_sectiontemplate$1_final.nsi
		ls -l *.~* &>/dev/null

		echo "    step 6"
		for s in $IN_SEC ; do
			sed "s/%%INSTALLER_INSECS%%/ SectionIn $s\\
			%%INSTALLER_INSECS%%/" nsis_sectiontemplate$1.~ns > nsis_sectiontemplate$1.~~s
			cp -f nsis_sectiontemplate$1.~~s nsis_sectiontemplate$1.~ns
			ls -l *.~* &>/dev/null
		done
		sed "s/%%INSTALLER_INSECS%%//" nsis_sectiontemplate$1.~ns > nsis_sectiontemplate$1.~~s
		cp -f nsis_sectiontemplate$1.~~s nsis_sectiontemplate$1.~ns
		ls -l *.~* &>/dev/null

		echo "    step 7"
		sed "s/%%UNINSTALLER_FILES%%/$LUNINSTALLER_FILES\\
		%%UNINSTALLER_FILES%%/" nsis_basicscript.~si > nsis_basicscript.~~s
		cp -f nsis_basicscript.~~s nsis_basicscript.~si
		ls -l *.~* &>/dev/null
		#local secfile=`cat nsis_sectiontemplate$1.~ns`
		echo "    step 8"
		sed "/%%ADDITIONAL_SECTIONS%%/r nsis_sectiontemplate$1.~ns" nsis_basicscript.~si > nsis_basicscript.~~s
		cp -f nsis_basicscript.~~s nsis_basicscript.~si
		ls -l *.~* &>/dev/null
		rm nsis_sectiontemplate$1.~*
	else
		echo "   --> main section!"
		echo "    step 1"
		sed "s/%%INSTALLER_DIRS%%/$LINSTALLER_DIRS/" nsis_basicscript.~si > nsis_basicscript.~~s
		cp -f nsis_basicscript.~~s nsis_basicscript.~si
		ls -l *.~* &>/dev/null
		echo "    step 2"
		sed "s/%%INSTALLER_FILES%%/$LINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
		cp -f nsis_basicscript.~~s nsis_basicscript.~si
		ls -l *.~* &>/dev/null
		echo "    step 3"
		sed "s/%%UNINSTALLER_FILES%%/$LUNINSTALLER_FILES\\
		%%UNINSTALLER_FILES%%/" nsis_basicscript.~si > nsis_basicscript.~~s
		cp -f nsis_basicscript.~~s nsis_basicscript.~si
		ls -l *.~* &>/dev/null
	fi
	
	#cp -v nsis_basicscript.~si "nsis_basicscript_step${1}.nsi" &>/dev/null
	
}

showhelp="0"
domake="0"
createZIP="0"
runNSIS="1"
delete_deploy="1"
create_deploy="1"

until [ -z "$1" ]  # Until all parameters used up . . .
do
    if [ "${1}" == "--help" ]; then
	    showhelp="1"
	elif [ "$1" == "-?" ]; then
	    showhelp="1"
	elif [ "$1" == "--nomake" ]; then
	    domake="0"
	elif [ "$1" == "--make" ]; then
	    domake="1"
	elif [ "$1" == "--nozip" ]; then
	    createZIP="0"
	elif [ "$1" == "--zip" ]; then
	    createZIP="1"
	elif [ "$1" == "--nsis" ]; then
	    runNSIS="1"
	elif [ "$1" == "--runnsis" ]; then
	    runNSIS="1"
	elif [ "$1" == "--nonsis" ]; then
	    runNSIS="0"
	elif [ "$1" == "--nodeldeploy" ]; then
	    delete_deploy="0"
	elif [ "$1" == "--nocreatedeploy" ]; then
	    create_deploy="0"
	else 
		echo "############################################################################"
		echo "# ERRROR: UNKOWN OPTION $1"
		echo "############################################################################"
		echo ""
		showhelp="1"
	fi
	shift;
done


if [ "$showhelp" != "0" ]; then
	help
else

	echo -e "DEPLOYING QUICKFIT\n  in order to deploy without recompiling, call with option --nomake"

	svn update
	svn update ../output
	echo -e "\n\ndetermining SVN version:"
	SVNVER=`./quickfit3.exe --gitversion`
	#`git rev-list HEAD --count`
	#SVNVER=$((2100 + SVNVER))
	echo -e "\n   SVN version: ${SVNVER}"
	echo -e "\n\ndetermining compile date:"
	COMPILEDATE=`./quickfit3.exe --compiledate`
	#`date +%Y-%m-%d`
	echo -e "\n   compile date: ${COMPILEDATE}"
	echo -e "\n\ndetermining bit depth:"
	BITDEPTH=`./quickfit3.exe --getbits`
	echo -e "\n   bit depth: ${BITDEPTH}\n\n"


	# Install-groups:
	# If you add more install groups, you'll have to define respective variables below
	# and add the required function calls throughout the script.
	#
	# Simply search for e.g. "microscopy" in this document and duplicate+modify the 
	# found lines! You will also have to add a new section to nsis_basicscript.nsi

	# These variables define names for ZIP-files for the single modules
	#
	INSTALLER_BASENAME=quickfit3_win${BITDEPTH}_${SVNVER}
	INSTALLER_INSTDIR="\$PROGRAMFILES${BITDEPTH}"
	ZIPFILE=quickfit3_win${BITDEPTH}_${SVNVER}.zip


	#####################################
	# define the installer sections:    #
	#####################################
	#
	# SECTIONNAME_PLUGINSLIST: description text of the installer section
	#                             Note: Don't use / here !!!
	# SECTIONMODE_PLUGINSLIST: mode of the installer section: "ro"=read-only(=mandatory)/selected
	#                                                         "b" =bold, unchecked/unselected
	#                                                         "bs"= bold, checked/selected
	#                                                         "s" = checked/selected
	# INSECTION_PLUGINSLIST: a list of the InstType number that this section should be part of. The default types are:
	#                                         1: "default: (imaging) FCS/FCCS + ALEX spFRET"
	#                                         2: "minimal (basic plugins only)"
	#                                         3: "full"
	#                                         4: "FCS/FCCS"
	#                                         5: "FCS/FCCS + ALEX spFRET"
	#                                         6: "(imaging) FCS/FCCS"
	#                        More types can be added to nsis_basicscript.nsi
	#                        NOte: sections 2 and 3 are set correctly by default!
	# PLUGINSLIST: 
	#      These arrays (space-deparated) define the different groups in the installer. 
	#      Note that if the DLL-name of your plugin differs from the plugin-id, you'll have to add BOTH, the DLL-basename and the plugin ID to the respective group
	#      All plugins that are part of no group and are not part of REMOVEPLUGINS will be installed in the main-group!
	#
	# Note: Don't forget to add PLUGINSLIST to the variable INSTALLER_SECTIONS
	SECTIONNAME_FCSPLUGINS="FCS and FCCS Plugins"
	SECTIONMODE_FCSPLUGINS="bs"
	INSECTION_FCSPLUGINS="1 4 5 6"
	FCSPLUGINS=" fcs qfrdrfcs qfevalfcsfit fccsfit fccs_fit fcsfit qffcsfitfuncs fcs_fitfuctions fcsmsdevaluation fitfunction_2ffcs fitfunction_dls fitfunction_fccs fitfunction_fcsdistribution fitfunction_spimfcs fitfunction_tirfcs fitfunctions_2ffcs fitfunctions_dls fitfunctions_fccs fitfunctions_fcsdistribution fitfunctions_spimfcs fitfunctions_tirfcs importers_simpletcspcimporter photoncounts importers_picoquant picoquantimporters qffcsmaxentevaluation fcs_maxent tcspcimporter  "


	SECTIONNAME_IMFCSPLUGINS="Imaging FCS and FCCS Plugins"
	SECTIONMODE_IMFCSPLUGINS="bs"
	INSECTION_IMFCSPLUGINS="1 6"
	IMFCSPLUGINS=" imagingfcs imaging_fcs imfccsfit imfccs_fit imfcsfit numberandbrightness number_and_brightness fitfunctions_spimfcs fitfunctions_tirfcs fitfunctions_2ffcs "


	SECTIONNAME_MICROSCOPYPLUGINS="Microscopy Characterization Plugins"
	SECTIONMODE_MICROSCOPYPLUGINS="s"
	INSECTION_MICROSCOPYPLUGINS="1 6"
	MICROSCOPYPLUGINS=" qfevalbeadscanpsf eval_beadscanpsf eval_cameracalibration eval_colocalization qfevalcolocalization qfevalcameracalibration fitfunctions_lightsheet spim_lightsheet_eval "


	SECTIONNAME_NITOOLS="National Instruments Hardware Control Plugins"
	SECTIONMODE_NITOOLS=""
	NITOOLS=" qfe_nidaqmxreader "


	SECTIONNAME_ALEXTOOLS="B040 ALEX-Tools for spFRET"
	SECTIONMODE_ALEXTOOLS=""
	INSECTION_ALEXTOOLS="1 5"
	ALEXTOOLS=" qfe_alexeval qfe_alexcontrol"


	SECTIONNAME_SPIMPLUGINS="B040 Microscope Instrument Control Plugins"
	SECTIONMODE_SPIMPLUGINS=""
	SPIMPLUGINS=" cam_testcamera stage_pi863 cam_andor spimb040 shutter_servo_arduino filterc_test cam_systemcam filterc_tmcl lights_b040laserbox lights_pccsled lights_coboltlaser stage_pi863_2 meas_b040resheater lights_coboltlaser servo_pololu_maestro shutter_relais_arduino  cam_server meas_spadmeasurement"


	# these plugins will be removed from the main-group. If the are not part of any group, these
	# plugins will not be part of the installer at all.
	REMOVEPLUGINS=" cam_radhard2 cam_rh2v2  b040_ffmcontrol alv_autocorrelator5000 multicontrol_stage qfe_acquisitiontest scanner2000_nicounter"

	INSTALLER_SECTIONS="FCSPLUGINS IMFCSPLUGINS MICROSCOPYPLUGINS NITOOLS ALEXTOOLS SPIMPLUGINS"

	for s in $INSTALLER_SECTIONS; do
		temp_ss=
		eval temp_ss=\$$s
		REMOVEPLUGINS="${REMOVEPLUGINS} $temp_ss"
	done
	#echo "REMOVEPLUGINS = $REMOVEPLUGINS"

	if [ "${create_deploy}" != "0" ]; then

		rm -rf deploymain &>/dev/null
		cp ${ZIPFILE} "${ZIPFILE}.backup" &>/dev/null
		rm ${ZIPFILE} &>/dev/null
		
		# for each install-group, clean the subdir 'deploy...' in which the files for the group are summarized
		# The first argument is the name for the new subdir (in /output) and the second parameter is the name
		# if the ZIP-file for this group (if ZIPs are activated)
		for s in $INSTALLER_SECTIONS; do
			module_prepare_cleandeploy $s
		done

		mkdir -p deploymain


		if [ "${domake}" != "0" ]; then
			cd ..
			#rm compiledate.h
			#rm svnversion.h
			#rm ./application/Makefile
			#rm ./application/Makefile.Release
			#rm ./application/Makefile.Debug
			qmake "CONFIG+=release" "CONFIG-=debug" quickfit3.pro
			make
			make install
			cd output
		fi

		PWDD=`pwd`
		echo "NOW IN  ${PWDD}";
		echo "PREPARING DEPLOY-DIRECTORY ./deploymain/"
		for f in *
		do
			if [ $f != "deploy" ] && [ $f != "deploymain" ]; then
			  if [[ "$f" != *setup.exe ]] && [[ "$f" != deploy* ]]; then
				if [[ "$f" != quickfit3_*.* ]] && [[ "$f" != Qt5*d.dll ]]; then
					echo "  ==> copy $f"
					cp -rf $f ./deploymain
				fi	
			  fi
			fi
		done

		if ! cd deploymain; then echo "could not create ./deploymain/"; exit 1; fi
		PWDDEP=`pwd`
		echo "NOW IN  ${PWDDEP}";
		
		echo "CLEANING DEPLOY-DIRECTORY ./deploymain/"
		rm ./globalconfig/* &>/dev/null
		rm -rf ./globalconfig &>/dev/null
		mkdir globalconfig &>/dev/null
		rm ./Qt*d4.dll &>/dev/null
		rm ./Qt5*d.dll &>/dev/null
		rm ./Qt*d.dll &>/dev/null
		rm ./test*.* &>/dev/null
		rm ./temp*.* &>/dev/null
		rm ./*temp*.* &>/dev/null
		rm ./untitled*.* &>/dev/null
		rm ./*.a &>/dev/null
		rm ./libEGLd.dll &>/dev/null
		rm ./libGLESv2d.dll &>/dev/null
		rm ./*.zip &>/dev/null
		rm ./qf3infotool.exe &>/dev/null
		rm ./qf3infotool*.* &>/dev/null
		rm ./qf3infotool &>/dev/null



		find -name "quickfit3_*.zip" -exec rm -rf {} \;
		find -name "*.sh" -exec rm -rf {} \;
		find -name ".svn" -type d -exec rm -rf {} \;
		find -name "*.log" -exec rm -rf {} \;
		find -name "*.prl" -exec rm -rf {} \;
		find -name "*.autosave" -exec rm -rf {} \;
		find -name "*.a" -exec rm -rf {} \;
		find -name "*.backup" -exec rm -rf {} \;
		find -name "*.ts" -exec rm -rf {} \;
		find -name "*.cpt" -exec rm -rf {} \;
		
		PWDD=`pwd`
		echo "NOW IN  ${PWDD}"
		#ls -alph
		if cd qtplugins; then
			PWDD=`pwd`
			echo "going to ./qtplugins/  ${PWDD}"
			find -name "*d4.dll" -exec rm -rf {} \;
			find -name "q*d.dll" -exec rm -rf {} \;
			find -name "dsengined.dll" -exec rm -rf {} \;
			find -name "eglvideonoded.dll" -exec rm -rf {} \;
			find -name "windowsprintersupportd.dll" -exec rm -rf {} \;
			find -name "*.a" -exec rm -rf {} \;
			cd $PWDDEP
			PWDD=`pwd`
			echo "returning from ./qtplugins/ ${PWDD}"
		else	
			echo "could not cd to ./qtplugins/ "
			cd $PWDDEP
		fi
		PWDD=`pwd`
		echo "NOW IN  ${PWDD}"

		
		# for each install-group, create a new subdir 'deploy...' in which the files for the group are summarized
		# The first argument is the name for the new subdir (in /output) and the second parameter is the name
		# if the ZIP-file for this group (if ZIPs are activated)
		for s in $INSTALLER_SECTIONS; do
			module_prepare $s
		done
		
		
		echo "CLEANING UNUSED PLUGINS FROM ./deploymain/"
			
		for f in $REMOVEPLUGINS
		do
			rm -rf  "./assets/plugins/${f}" &>/dev/null
			rm -rf  "./assets/plugins/help/${f}" &>/dev/null
			rm -rf  "./examples/${f}" &>/dev/null
			find -name "${f}.*" -exec rm -rf {} \;
		done

		
		rm *.sh &>/dev/null
		rm *.ini &>/dev/null
		rm *.nsi &>/dev/null
		rm *.~* &>/dev/null
		rm ATMCD32D.DLL &>/dev/null
		rm atmcd64d.DLL &>/dev/null
		cd plugins 
		rm ATMCD32D.DLL &>/dev/null
		rm atmcd64d.DLL &>/dev/null
		cd .. 
		rm *setup.exe &>/dev/null
		rm test*.* &>/dev/null
		rm untitled*.* &>/dev/null
		rm *.log &>/dev/null
		rm *.nsh &>/dev/null
		rm releasenotes_template.html &>/dev/null

		echo -e "\n\nCREATING RELEASE NOTES:\n\n"
		echo "  step 1"
		sed "/%%RELEASE_NOTES%%/ {
		  r ../../application/releasenote.html
		  d  
		}" ../releasenotes_template.html > releasenotes.~ht
		echo "  step 2"
		sed "s/\\\$\\\$SVN\\\$\\\$/$SVNVER/g" releasenotes.~ht > releasenotes.~~h
		cp -f releasenotes.~~h releasenotes.~ht
		echo "  step 3"
		sed "s/\\\$\\\$COMPILEDATE\\\$\\\$/$COMPILEDATE/g" releasenotes.~ht > releasenotes.~~h
		cp -f releasenotes.~~h releasenotes.~ht
		cp releasenotes.~ht releasenotes.html
		rm *.~*
		cp releasenotes.html ../releasenotes.html
		cd ..

	fi

	if [ "${createZIP}" != "0" ]; then
		# for each install-group, create a ZIP-file from the contenst of the subdir 'deploy...' 
		# in which the files for the group were summarized (see above).
		# The first argument is the name for the new subdir (in /output) and the second parameter is the name
		# if the ZIP-file for this group
		echo -e "\n\nCREATING ZIP ARCHIVES FOR DEPLOYMENT:\n\n"
		for s in $INSTALLER_SECTIONS; do
			module_deploy_zip $s
		done
		module_deploy_mainzip deploymain 
		
	fi


	echo -e "\n\nWRITING WINDOWS INSTALLER SCRIPT:\n\n"


	cp nsis_basicscript.nsi  nsis_basicscript.~si

	# for each install-group, add its files and data to the nsis_basicscript.nsi from the contenst of the subdir 'deploy...' 
	# in which the files for the group were summarized (see above).
	# The first argument is the name for the new subdir (in /output) and the second parameter is the 
	# *name* of the variable containing the files to be installed in the file nsis_basicscript.nsi
	# The third and fourth parameter are again the *names* of teh files to be uninstalled and of the directories
	# to be created by the installer.
	for s in $INSTALLER_SECTIONS; do
		module_deploy_nsis $s
	done
	module_deploy_nsis deploymain 

	ls -l *.~* &>/dev/null
	echo "COMPLETING INSTALLER SCRIPT"
	echo "   step 1"
	sed "s/%%INSTALLER_INSTDIR%%/$INSTALLER_INSTDIR/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 2"
	sed "s/%%INSTALLER_BASENAME%%/$INSTALLER_BASENAME/" nsis_basicscript.~si > nsis_basicscript.~~s 
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 3"
	sed "s/%%SVNVER%%/$SVNVER/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 4"
	sed "s/%%COMPILEDATE%%/$COMPILEDATE/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 5"
	sed "s/%%BITDEPTH%%/$BITDEPTH/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 6"
	sed "s/%%INSTALLER_DIRS%%//" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 7"
	sed "s/%%SEC_MODES%%//" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 8"
	sed "s/%%UNINSTALLER_FILES%%//" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 9"
	sed "s/%%INSTALLER_FILES%%//" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 10"
	sed "s/%%STORESPECIALVERSION%%/std/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 11"
	sed "s/%%ADDITIONAL_SECTIONS%%//" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null
	echo "   step 12"
	sed "s/%%INSTALLER_INSECS%%//" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~* &>/dev/null


	cp nsis_basicscript.~si ${INSTALLER_BASENAME}.nsi
	cp ${INSTALLER_BASENAME}.nsi ../${INSTALLER_BASENAME}_lastdeploy.nsi
	#rm nsis_basicscript.~si
	rm *.~*


	if [ "${runNSIS}" != "0" ]; then
		echo "RUNNING NSIS INSTALLER GENERATOR"
		makensis ${INSTALLER_BASENAME}.nsi
	fi

	if [ "${delete_deploy}" != "0" ]; then
		echo -e "\n\nCLEANUP:\n\n"

		
		# for each install-group, delete the subdir 'deploy...' 
		# and (if it was created) the backup-version of the ZIP-file.
		for s in $INSTALLER_SECTIONS; do
			module_final_cleandeploy $s
		done
		main_final_cleandeploy deploymain
	fi
fi