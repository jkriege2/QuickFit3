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
	echo -e "    --deployspecials deploy the specials together with the standard" 
	echo -e "                     installation                    (default: off)"
	echo -e "    --nodeldeploy    do not clean up at the end of the process" 
	echo -e "                     (delete deploy directory)        (default: on)"
	echo -e "    --nocreatedeploy create deploy directory at start of script" 
	echo -e "                                                      (default: on)"
}

function module_prepare_cleandeploy {
	rm -rf $1
	cp ${$2} "${$2}.backup"
	rm ${$2}
}

function module_final_cleandeploy {
	rm -rf $1
	rm ${$2}
}


function module_prepare {
	mkdir ../$1
	mkdir ../$1/plugins
	mkdir ../$1/fitfunctionplugins
	mkdir ../$1/globalconfig_templates
	mkdir ../$1/source
	mkdir ../$1/assets
	mkdir ../$1/sdk
	mkdir ../$1/sdk/fitfunctions/
	mkdir ../$1/examples
	mkdir ../$1/assets/plugins
	mkdir ../$1/assets/plugins/help
	cp -rf ./sdk/fitfunctions/* ../$1/sdk/fitfunctions
	cp -rf ./globalconfig_templates/* ../$1/globalconfig_templates

	for f in $3
	do
		cp -rf "../${f}" ../$1
	done
	
	for f in $2
	do
		mkdir "../$1/assets/plugins/${f}"
		mkdir "../$1/assets/plugins/help/${f}"
		cp -rf  "./assets/plugins/${f}" "../$1/assets/plugins/"
		cp -rf  "./examples/${f}" "../$1/examples/"
		cp -rf  "./assets/plugins/help/${f}" "../$1/assets/help/plugins/"
		find -name "${f}.*" -exec cp -rf "{}" "../$1/{}" \;
	done

}

function module_deploy_zip {
    cd $1
	echo -e " - ${2}\n"
	zip -rv9 ../$2 *
	cd ..
}

function module_deploy_nsis {

	cd $1
	local INSTALLER_FILES=
	local UNINSTALLER_FILES=
	for f in `find . -type f`
	do
		fn=${f//\//\\\\}
		fn=${fn//.\\\\/}
		INSTALLER_FILES="${INSTALLER_FILES}\\
	File \"\\/oname=$fn\" \"$fn\""
		UNINSTALLER_FILES="${UNINSTALLER_FILES}\\
	Delete \\/REBOOTOK \"\$INSTDIR\\\\$fn\""
	done
	local INSTALLER_DIRS=
	for f in `find . -type d`
	do
		fn=${f//\//\\\\}
		fn=${fn//.\\/}
		INSTALLER_DIRS="${INSTALLER_DIRS}\\
	CreateDirectory  \"\$INSTDIR\\$fn\""
		
	done
	cd ..
	
	sed "s/%%$2%%/$INSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~*
	sed "s/%%$3%%/$UNINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~*
	sed "s/%%$4%%/$INSTALLER_DIRS/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~*
	
	eval $2="'$INSTALLER_FILES'"
	eval $3="'$UNINSTALLER_FILES'"
	eval $4="'$INSTALLER_DIRS'"

}

showhelp="0"
domake="0"
createZIP="0"
runNSIS="1"
deployspecials="0"
delete_deploy="1"
create_deploy="1"
SPECIALS=""

until [ -z "$1" ]  # Until all parameters used up . . .
do
    if [ "${1}" == "--help" ]; then
	    showhelp="1"
	fi
    if [ "$1" == "-?" ]; then
	    showhelp="1"
	fi
    if [ "$1" == "--nomake" ]; then
	    domake="0"
	fi
    if [ "$1" == "--make" ]; then
	    domake="1"
	fi
    if [ "$1" == "--nozip" ]; then
	    createZIP="0"
	fi
    if [ "$1" == "--zip" ]; then
	    createZIP="1"
	fi
    if [ "$1" == "--nsis" ]; then
	    runNSIS="1"
	fi
    if [ "$1" == "--runnsis" ]; then
	    runNSIS="1"
	fi
    if [ "$1" == "--nonsis" ]; then
	    runNSIS="0"
	fi
    if [ "$1" == "--nodeldeploy" ]; then
	    delete_deploy="0"
	fi
    if [ "$1" == "--nocreatedeploy" ]; then
	    create_deploy="0"
	fi
    if [ "$1" == "--deployspecials" ]; then
	    deployspecials="1"
		SPECIALS="_withspecials"
	fi
	shift;
done


if [ "$showhelp" != "0" ]; then
	help
	exit
fi

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


INSTALLER_BASENAME=quickfit3_win${BITDEPTH}${SPECIALS}_${SVNVER}
INSTALLER_INSTDIR="\$PROGRAMFILES${BITDEPTH}"
ZIPFILE=quickfit3_win${BITDEPTH}${SPECIALS}_${SVNVER}.zip
ZIPFILESPIM=quickfit3_win${BITDEPTH}${SPECIALS}_spimplugins_${SVNVER}.zip
ZIPFILEFCS=quickfit3_win${BITDEPTH}${SPECIALS}_fcsplugins_${SVNVER}.zip
ZIPFILEIMFCS=quickfit3_win${BITDEPTH}${SPECIALS}_imfcsplugins_${SVNVER}.zip
ZIPFILEMICROSCOPY=quickfit3_win${BITDEPTH}${SPECIALS}_microscopyplugins_${SVNVER}.zip
ZIPFILENITOOLS=quickfit3_win${BITDEPTH}${SPECIALS}_nitools_${SVNVER}.zip
ZIPFILESPECIAL=quickfit3_win${BITDEPTH}${SPECIALS}_special_${SVNVER}.zip
#if [ "${BITDEPTH}" == "64" ]; then
#	INSTALLER_INSTDIR="\$PROGRAMFILES64"
#	INSTALLER_BASENAME=quickfit3_win64${SPECIALS}_${SVNVER}
#	ZIPFILE=quickfit3_win64${SPECIALS}_${SVNVER}.zip
#	ZIPFILESPIM=quickfit3_win64${SPECIALS}_spimplugins_${SVNVER}.zip
#	ZIPFILEFCS=quickfit3_win64${SPECIALS}_fcsplugins_${SVNVER}.zip
#	ZIPFILEIMFCS=quickfit3_win64${SPECIALS}_imfcsplugins_${SVNVER}.zip
#	ZIPFILESPECIAL=quickfit3_win64${SPECIALS}_special_${SVNVER}.zip
#	ZIPFILEMICROSCOPY=quickfit3_win64${SPECIALS}_microscopyplugins_${SVNVER}.zip
#fi
FCSPLUGINS=" fcs fccsfit fcsfit fcs_fitfuctions fcsmsdevaluation fitfunction_2ffcs fitfunction_dls fitfunction_fccs fitfunction_fcsdistribution fitfunction_spimfcs fitfunction_tirfcs importers_simpletcspcimporter photoncounts picoquantimporters qffcsmaxentevaluation tcspcimporter qfe_alexeval"
IMFCSPLUGINS=" imagingfcs imfccsfit imfcsfit numberandbrightness"
MICROSCOPYPLUGINS=" qfevalbeadscanpsf qfevalcolocalization qfevalcameracalibration fitfunctions_lightsheet spim_lightsheet_eval "
SPIMPLUGINS=" cam_testcamera stage_pi863 cam_andor spimb040 shutter_servo_arduino filterc_test cam_systemcam filterc_tmcl lights_b040laserbox lights_pccsled lights_coboltlaser stage_pi863_2 meas_b040resheater lights_coboltlaser servo_pololu_maestro shutter_relais_arduino"
NITOOLS=" qfe_nidaqmxreader plg_qfe_alexcontrol"
SPECIALPLUGINS=""
if [ "${deployspecials}" == "1" ]; then
	SPECIALPLUGINS=
fi
REMOVEPLUGINS=" ${SPECIALPLUGINS} ${SPIMPLUGINS} ${FCSPLUGINS} ${IMFCSPLUGINS} ${NITOOLS} ${MICROSCOPYPLUGINS} cam_radhard2 cam_rh2v2  b040_ffmcontrol alv_autocorrelator5000 multicontrol_stage qfe_acquisitiontest scanner2000_nicounter"
SPIMONLYQTMODULES=" QtScript4.dll QtScriptTools4.dll Qt5Script.dll Qt5ScriptTools.dll"
SPECIALONLYQTMODULES=""


if [ "${create_deploy}" != "0" ]; then

	rm -rf deploy
	cp ${ZIPFILE} "${ZIPFILE}.backup"
	rm ${ZIPFILE}
	

	module_prepare_cleandeploy deployspim ${ZIPFILESPIM}
	module_prepare_cleandeploy deployfcs ${ZIPFILEFCS}
	module_prepare_cleandeploy deployimfcs ${ZIPFILEIMFCS}
	module_prepare_cleandeploy deploymicroscopy ${ZIPFILEMICROSCOPY}
	module_prepare_cleandeploy deployspecial ${ZIPFILESPECIAL}
	module_prepare_cleandeploy deploynitools ${ZIPFILENITOOLS}

	mkdir -p deploy





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


	for f in *
	do
		if [ $f != "deploy" ]; then
		  if [[ "$f" != *setup.exe ]]; then
			if [[ "$f" != quickfit3_*.* ]]; then
				cp -rv $f ./deploy
			fi	
		  fi
		fi
	done

	if ! cd deploy; then echo "could not create ./deploy/"; exit 1; fi
	rm ./globalconfig/*
	rm -rf ./globalconfig
	mkdir globalconfig
	rm ./Qt*d4.dll
	rm ./Qt5*d.dll
	rm ./Qt*d.dll
	rm ./test*.*
	rm ./temp*.*
	rm ./*temp*.*
	rm ./untitled*.*
	rm ./*.a
	rm ./libEGLd.dll
	rm ./libGLESv2d.dll
	rm ./*.zip
	rm ./qf3infotool.exe
	rm ./qf3infotool*.*
	rm ./qf3infotool

	for f in $SPIMONLYQTMODULES
	do
		rm -rf  "./${f}"
	done

	for f in $SPECIALONLYQTMODULES
	do
		rm -rf  "./${f}"
	done

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
	
	cd qtplugins
	PWDD=`pwd`
	 echo "going to ./qtplugins/  ${PWDD}";
	find -name "*d4.dll" -exec rm -rf {} \;
	find -name "q*d.dll" -exec rm -rf {} \;
	find -name "dsengined.dll" -exec rm -rf {} \;
	find -name "eglvideonoded.dll" -exec rm -rf {} \;
	find -name "windowsprintersupportd.dll" -exec rm -rf {} \;
	find -name "*.a" -exec rm -rf {} \;
	cd ..
	PWDD=`pwd`
	echo "returning from ./qtplugins/ ${PWDD}";



	module_prepare deployspim $SPIMPLUGINS $SPIMONLYQTMODULES
	module_prepare deployfcs $FCSPLUGINS ""
	module_prepare deployimfcs $IMFCSPLUGINS ""
	module_prepare deploymicroscopy $MICROSCOPYPLUGINS ""
	module_prepare deployspecial $SPECIALPLUGINS $SPECIALONLYQTMODULES
	module_prepare deploynitools $NITOOLS ""
	

	for f in $REMOVEPLUGINS
	do
		rm -rf  "./assets/plugins/${f}"
		rm -rf  "./assets/plugins/help/${f}"
		find -name "${f}.*" -exec rm -rf {} \;
	done

	
	rm *.sh
	rm *.ini
	rm *.nsi
	rm *.~*
	rm ATMCD32D.DLL
	rm atmcd64d.DLL
	cd plugins
	rm ATMCD32D.DLL
	rm atmcd64d.DLL
	cd ..
	rm *setup.exe
	rm test*.*
	rm untitled*.*
	rm *.log
	rm *.nsh
	rm releasenotes_template.html

	echo -e "\n\nCREATING RELEASE NOTES:\n\n"
	sed "/%%RELEASE_NOTES%%/ {
	  r ../../application/releasenote.html
	  d  
	}" ../releasenotes_template.html > releasenotes.~ht
	sed "s/\\\$\\\$SVN\\\$\\\$/$SVNVER/g" releasenotes.~ht > releasenotes.~~h
	cp -f releasenotes.~~h releasenotes.~ht
	sed "s/\\\$\\\$COMPILEDATE\\\$\\\$/$COMPILEDATE/g" releasenotes.~ht > releasenotes.~~h
	cp -f releasenotes.~~h releasenotes.~ht
	cp releasenotes.~ht releasenotes.html
	rm *.~*
	cp releasenotes.html ../releasenotes.html
	cd ..

fi

if [ "${createZIP}" != "0" ]; then
	echo -e "\n\nCREATING ZIP ARCHIVES FOR DEPLOYMENT:\n\n"
	module_deploy_zip deploy ${ZIPFILE}
	module_deploy_zip deployspim ${ZIPFILESPIM}
	module_deploy_zip deployfcs ${ZIPFILEFCS}
	module_deploy_zip deployimfcs ${ZIPFILEIMFCS}
	module_deploy_zip deploymicroscopy ${ZIPFILEMICROSCOPY}
	module_deploy_zip deployspecial ${ZIPFILESPECIAL}
	module_deploy_zip deploynitools ${ZIPFILENITOOLS}
fi


echo -e "\n\nWRITING WINDOWS INSTALLER SCRIPT:\n\n"


cp nsis_basicscript.nsi > nsis_basicscript.~si

module_deploy_nsis deploy INSTALLER_FILES UNINSTALLER_FILES INSTALLER_DIRS
module_deploy_nsis deployspim SPIMINSTALLER_FILES SPIMUNINSTALLER_FILES SPIMINSTALLER_DIRS
module_deploy_nsis deployfcs FCSINSTALLER_FILES FCSUNINSTALLER_FILES FCSINSTALLER_DIRS
module_deploy_nsis deployimfcs IMFCSINSTALLER_FILES IMFCSUNINSTALLER_FILES IMFCSINSTALLER_DIRS
module_deploy_nsis deploymicroscopy MICROSCOPYINSTALLER_FILES MICROSCOPYUNINSTALLER_FILES MICROSCOPYINSTALLER_DIRS
module_deploy_nsis deploynitools NITOOLSINSTALLER_FILES NITOOLSUNINSTALLER_FILES NITOOLSINSTALLER_DIRS




sed "s/%%INSTALLER_INSTDIR%%/$INSTALLER_INSTDIR/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%INSTALLER_BASENAME%%/$INSTALLER_BASENAME/" nsis_basicscript.~si > nsis_basicscript.~~s 
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%SVNVER%%/$SVNVER/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%COMPILEDATE%%/$COMPILEDATE/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%BITDEPTH%%/$BITDEPTH/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%INSTALLER_DIRS%%/$INSTALLER_DIRS/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*


if [ "${deployspecials}" == "1" ]; then
	sed "s/%%STORESPECIALVERSION%%/special/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~*
else
	sed "s/%%STORESPECIALVERSION%%/std/" nsis_basicscript.~si > nsis_basicscript.~~s
	cp -f nsis_basicscript.~~s nsis_basicscript.~si
	ls -l *.~*
fi


cp nsis_basicscript.~si ${INSTALLER_BASENAME}.nsi
#rm nsis_basicscript.~si
rm *.~*


if [ "${runNSIS}" != "0" ]; then
	makensis ${INSTALLER_BASENAME}.nsi
fi

if [ "${delete_deploy}" != "0" ]; then
	echo -e "\n\nCLEANUP:\n\n"

	module_final_cleandeploy deploy "${ZIPFILE}.backup"
	module_final_cleandeploy deployspim "${ZIPFILESPIM}.backup"
	module_final_cleandeploy deployfcs "${ZIPFILEFCS}.backup"
	module_final_cleandeploy deployimfcs "${ZIPFILEIMFCS}.backup"
	module_final_cleandeploy deploymicroscopy "${ZIPFILEMICROSCOPY}.backup"
	module_final_cleandeploy deployspecial "${ZIPFILESPECIAL}.backup"
	module_final_cleandeploy deploynitools "${ZIPFILENITOOLS}.backup"
fi
