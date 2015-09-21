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
SVNVER=`git rev-list HEAD --count`
SVNVER=$((2100 + SVNVER))
echo -e "\n   SVN version: ${SVNVER}"
echo -e "\n\ndetermining compile date:"
COMPILEDATE=`date +%Y-%m-%d`
echo -e "\n   compile date: ${COMPILEDATE}"
echo -e "\n\ndetermining bit depth:"
BITDEPTH=`./quickfit3.exe --getbits`
echo -e "\n   bit depth: ${BITDEPTH}\n\n"

INSTALLER_BASENAME=quickfit3_win32${SPECIALS}_${SVNVER}
INSTALLER_INSTDIR="\$PROGRAMFILES32"
ZIPFILE=quickfit3_win32${SPECIALS}_${SVNVER}.zip
ZIPFILESPIM=quickfit3_win32${SPECIALS}_spimplugins_${SVNVER}.zip
ZIPFILEFCS=quickfit3_win32${SPECIALS}_fcsplugins_${SVNVER}.zip
ZIPFILEIMFCS=quickfit3_win32${SPECIALS}_imfcsplugins_${SVNVER}.zip
ZIPFILEMICROSCOPY=quickfit3_win32${SPECIALS}_microscopyplugins_${SVNVER}.zip
ZIPFILESPECIAL=quickfit3_win32${SPECIALS}_special_${SVNVER}.zip
if [ "${BITDEPTH}" == "64" ]; then
	INSTALLER_INSTDIR="\$PROGRAMFILES64"
	INSTALLER_BASENAME=quickfit3_win64${SPECIALS}_${SVNVER}
	ZIPFILE=quickfit3_win64${SPECIALS}_${SVNVER}.zip
	ZIPFILESPIM=quickfit3_win64${SPECIALS}_spimplugins_${SVNVER}.zip
	ZIPFILEFCS=quickfit3_win64${SPECIALS}_fcsplugins_${SVNVER}.zip
	ZIPFILEIMFCS=quickfit3_win64${SPECIALS}_imfcsplugins_${SVNVER}.zip
	ZIPFILESPECIAL=quickfit3_win64${SPECIALS}_special_${SVNVER}.zip
	ZIPFILEMICROSCOPY=quickfit3_win64${SPECIALS}_microscopyplugins_${SVNVER}.zip
fi
FCSPLUGINS=" fcs fccsfit fcsfit fcs_fitfuctions fcsmsdevaluation fitfunction_2ffcs fitfunction_dls fitfunction_fccs fitfunction_fcsdistribution fitfunction_spimfcs fitfunction_tirfcs importers_simpletcspcimporter photoncounts picoquantimporters qffcsmaxentevaluation tcspcimporter"
IMFCSPLUGINS=" imagingfcs imfccsfit imfcsfit numberandbrightness"
MICROSCOPYPLUGINS=" qfevalbeadscanpsf qfevalcolocalization qfevalcameracalibration fitfunctions_lightsheet spim_lightsheet_eval "
SPIMPLUGINS=" cam_testcamera stage_pi863 cam_andor spimb040 shutter_servo_arduino filterc_test cam_systemcam filterc_tmcl lights_b040laserbox lights_pccsled lights_coboltlaser stage_pi863_2 meas_b040resheater lights_coboltlaser servo_pololu_maestro shutter_relais_arduino"
SPECIALPLUGINS=""
if [ "${deployspecials}" == "1" ]; then
	SPECIALPLUGINS=
fi
REMOVEPLUGINS=" ${SPECIALPLUGINS} ${SPIMPLUGINS} ${FCSPLUGINS} ${IMFCSPLUGINS} ${MICROSCOPYPLUGINS} cam_radhard2 cam_rh2v2  b040_ffmcontrol alv_autocorrelator5000 multicontrol_stage qfe_acquisitiontest scanner2000_nicounter"
SPIMONLYQTMODULES=" QtScript4.dll QtScriptTools4.dll Qt5Script.dll Qt5ScriptTools.dll"
SPECIALONLYQTMODULES=""


if [ "${create_deploy}" != "0" ]; then

	rm -rf deploy
	rm -rf deployspim
	rm -rf deployfcs
	rm -rf deployimfcs
	rm -rf deployspecial
	cp ${ZIPFILE} "${ZIPFILE}.backup"
	rm ${ZIPFILE}
	cp ${ZIPFILESPIM} "${ZIPFILESPIM}.backup"
	rm ${ZIPFILESPIM}
	cp ${ZIPFILEFCS} "${ZIPFILEFCS}.backup"
	rm ${ZIPFILEFCS}
	cp ${ZIPFILEIMFCS} "${ZIPFILEIMFCS}.backup"
	rm ${ZIPFILEIMFCS}
	cp ${ZIPFILEMICROSCOPY} "${ZIPFILEMICROSCOPY}.backup"
	rm ${ZIPFILEMICROSCOPY}
	cp ${ZIPFILESPECIAL} "${ZIPFILESPECIAL}.backup"
	rm ${ZIPFILESPECIAL}

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

	mkdir ../deployspim
	mkdir ../deployspim/plugins
	mkdir ../deployspim/fitfunctionplugins
	mkdir ../deployspim/globalconfig_templates
	mkdir ../deployspim/source
	mkdir ../deployspim/assets
	mkdir ../deployspim/sdk
	mkdir ../deployspim/sdk/fitfunctions/
	mkdir ../deployspim/examples
	mkdir ../deployspim/assets/plugins
	mkdir ../deployspim/assets/plugins/help

	mkdir ../deployfcs
	mkdir ../deployfcs/plugins
	mkdir ../deployfcs/fitfunctionplugins
	mkdir ../deployfcs/globalconfig_templates
	mkdir ../deployfcs/source
	mkdir ../deployfcs/assets
	mkdir ../deployfcs/sdk
	mkdir ../deployfcs/sdk/fitfunctions/
	mkdir ../deployfcs/examples
	mkdir ../deployfcs/assets/plugins
	mkdir ../deployfcs/assets/plugins/help

	mkdir ../deployimfcs
	mkdir ../deployimfcs/plugins
	mkdir ../deployimfcs/fitfunctionplugins
	mkdir ../deployimfcs/globalconfig_templates
	mkdir ../deployimfcs/source
	mkdir ../deployimfcs/assets
	mkdir ../deployimfcs/sdk
	mkdir ../deployimfcs/sdk/fitfunctions/
	mkdir ../deployimfcs/examples
	mkdir ../deployimfcs/assets/plugins
	mkdir ../deployimfcs/assets/plugins/help


	mkdir ../deploymicroscopy
	mkdir ../deploymicroscopy/plugins
	mkdir ../deploymicroscopy/fitfunctionplugins
	mkdir ../deploymicroscopy/globalconfig_templates
	mkdir ../deploymicroscopy/source
	mkdir ../deploymicroscopy/assets
	mkdir ../deploymicroscopy/sdk
	mkdir ../deploymicroscopy/sdk/fitfunctions/
	mkdir ../deploymicroscopy/examples
	mkdir ../deploymicroscopy/assets/plugins
	mkdir ../deploymicroscopy/assets/plugins/help

	mkdir ../deployspecial
	mkdir ../deployspecial/plugins
	mkdir ../deployspecial/fitfunctionplugins
	mkdir ../deployspecial/globalconfig_templates
	mkdir ../deployspecial/source
	mkdir ../deployspecial/assets
	mkdir ../deployspecial/examples
	mkdir ../deployspecial/assets/plugins
	mkdir ../deployspecial/assets/plugins/help
	mkdir ../deployspecial/sdk/fitfunctions/

	cp -rf ./sdk/fitfunctions/* ../deployfcs/sdk/fitfunctions
	cp -rf ./sdk/fitfunctions/* ../deployimfcs/sdk/fitfunctions
	cp -rf ./sdk/fitfunctions/* ../deployspim/sdk/fitfunctions
	cp -rf ./sdk/fitfunctions/* ../deployspecial/sdk/fitfunctions
	cp -rf ./sdk/fitfunctions/* ../deploymicroscopy/sdk/fitfunctions
	cp -rf ./globalconfig_templates/* ../deployfcs/globalconfig_templates
	cp -rf ./globalconfig_templates/* ../deployimfcs/globalconfig_templates
	cp -rf ./globalconfig_templates/* ../deployspim/globalconfig_templates
	cp -rf ./globalconfig_templates/* ../deployspecial/globalconfig_templates
	cp -rf ./globalconfig_templates/* ../deploymicroscopy/globalconfig_templates
	for f in $SPIMONLYQTMODULES
	do
		cp -rf "../${f}" ../deployspim
	done
	for f in $SPECIALONLYQTMODULES
	do
		cp -rf "../${f}" ../deployspecial
	done
	for f in $SPIMPLUGINS
	do
		mkdir "../deployspim/assets/plugins/${f}"
		mkdir "../deployspim/assets/plugins/help/${f}"
		cp -rf  "./assets/plugins/${f}" "../deployspim/assets/plugins/"
		cp -rf  "./examples/${f}" "../deployspim/examples/"
		cp -rf  "./assets/plugins/help/${f}" "../deployspim/assets/help/plugins/"
		find -name "${f}.*" -exec cp -rf "{}" "../deployspim/{}" \;
	done

	for f in $FCSPLUGINS
	do
		mkdir "../deployfcs/assets/plugins/${f}"
		mkdir "../deployfcs/assets/plugins/help/${f}"
		cp -rf  "./assets/plugins/${f}" "../deployfcs/assets/plugins/"
		cp -rf  "./examples/${f}" "../deployfcs/examples/"
		cp -rf  "./assets/plugins/help/${f}" "../deployfcs/assets/help/plugins/"
		find -name "${f}.*" -exec cp -rf "{}" "../deployfcs/{}" \;
	done

	for f in $IMFCSPLUGINS
	do
		mkdir "../deployimfcs/assets/plugins/${f}"
		mkdir "../deployimfcs/assets/plugins/help/${f}"
		cp -rf  "./assets/plugins/${f}" "../deployimfcs/assets/plugins/"
		cp -rf  "./examples/${f}" "../deployimfcs/examples/"
		cp -rf  "./assets/plugins/help/${f}" "../deployimfcs/assets/help/plugins/"
		find -name "${f}.*" -exec cp -rf "{}" "../deployimfcs/{}" \;
	done

	for f in $MICROSCOPYPLUGINS
	do
		mkdir "../deploymicroscopy/assets/plugins/${f}"
		mkdir "../deploymicroscopy/assets/plugins/help/${f}"
		cp -rf  "./assets/plugins/${f}" "../deploymicroscopy/assets/plugins/"
		cp -rf  "./examples/${f}" "../deploymicroscopy/examples/"
		cp -rf  "./assets/plugins/help/${f}" "../deploymicroscopy/assets/help/plugins/"
		find -name "${f}.*" -exec cp -rf "{}" "../deploymicroscopy/{}" \;
	done

	for f in $SPECIALPLUGINS
	do
		mkdir "../deployspecial/assets/plugins/${f}"
		mkdir "../deployspecial/assets/plugins/help/${f}"
		cp -rf  "./assets/plugins/${f}" "../deployspecial/assets/plugins/"
		cp -rf  "./examples/${f}" "../deployspecial/examples/"
		cp -rf  "./assets/plugins/help/${f}" "../deployspecial/assets/help/plugins/"
		find -name "${f}.*" -exec cp -rf "{}" "../deployspecial/{}" \;
	done

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
    cd deploy
	echo -e "\n\nCREATING ZIP ARCHIVES FOR DEPLOYMENT:\n\n"
	zip -rv9 ../${ZIPFILE} *
	cd ..
	cd deployspim
	zip -rv9 ../${ZIPFILESPIM} *
	cd ..
	cd deployfcs
	zip -rv9 ../${ZIPFILEFCS} *
	cd ..
	cd deployimfcs
	zip -rv9 ../${ZIPFILEIMFCS} *
	cd ..
	cd deploymicroscopy
	zip -rv9 ../${ZIPFILEMICROSCOPY} *
	cd ..
	cd deployspecial
	zip -rv9 ../${ZIPFILESPECIAL} *
	cd ..
fi


echo -e "\n\nWRITING WINDOWS INSTALLER SCRIPT:\n\n"

cd deploy
INSTALLER_FILES=
UNINSTALLER_FILES=
for f in `find . -type f`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\\\/}
    INSTALLER_FILES="${INSTALLER_FILES}\\
File \"\\/oname=$fn\" \"$fn\""
    UNINSTALLER_FILES="${UNINSTALLER_FILES}\\
Delete \\/REBOOTOK \"\$INSTDIR\\\\$fn\""
done
INSTALLER_DIRS=
for f in `find . -type d`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\/}
    INSTALLER_DIRS="${INSTALLER_DIRS}\\
CreateDirectory  \"\$INSTDIR\\$fn\""
   
done
cd ..

cd deployspim
SPIMINSTALLER_FILES=
SPIMUNINSTALLER_FILES=
for f in `find . -type f`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\\\/}
    SPIMINSTALLER_FILES="${SPIMINSTALLER_FILES}\\
File \"\\/oname=$fn\" \"$fn\""
    SPIMUNINSTALLER_FILES="${SPIMUNINSTALLER_FILES}\\
Delete \\/REBOOTOK \"\$INSTDIR\\\\$fn\""
done
SPIMINSTALLER_DIRS=
for f in `find . -type d`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\/}
    SPIMINSTALLER_DIRS="${SPIMINSTALLER_DIRS}\\
CreateDirectory  \"\$INSTDIR\\$fn\""
    
done
cd ..

cd deployfcs
FCSINSTALLER_FILES=
FCSUNINSTALLER_FILES=
for f in `find . -type f`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\\\/}
    FCSINSTALLER_FILES="${FCSINSTALLER_FILES}\\
File \"\\/oname=$fn\" \"$fn\""
    FCSUNINSTALLER_FILES="${FCSUNINSTALLER_FILES}\\
Delete \\/REBOOTOK \"\$INSTDIR\\\\$fn\""
done
FCSINSTALLER_DIRS=
for f in `find . -type d`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\/}
    FCSINSTALLER_DIRS="${FCSINSTALLER_DIRS}\\
CreateDirectory  \"\$INSTDIR\\$fn\""
    
done
cd ..

cd deployimfcs
IMFCSINSTALLER_FILES=
IMFCSUNINSTALLER_FILES=
for f in `find . -type f`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\\\/}
    IMFCSINSTALLER_FILES="${IMFCSINSTALLER_FILES}\\
File \"\\/oname=$fn\" \"$fn\""
    IMFCSUNINSTALLER_FILES="${IMFCSUNINSTALLER_FILES}\\
Delete \\/REBOOTOK \"\$INSTDIR\\\\$fn\""
done
IMFCSINSTALLER_DIRS=
for f in `find . -type d`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\/}
    IMFCSINSTALLER_DIRS="${IMFCSINSTALLER_DIRS}\\
CreateDirectory  \"\$INSTDIR\\$fn\""
    
done
cd ..

cd deploymicroscopy
MICROSCOPYINSTALLER_FILES=
MICROSCOPYUNINSTALLER_FILES=
for f in `find . -type f`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\\\/}
    MICROSCOPYINSTALLER_FILES="${MICROSCOPYINSTALLER_FILES}\\
File \"\\/oname=$fn\" \"$fn\""
    MICROSCOPYUNINSTALLER_FILES="${MICROSCOPYUNINSTALLER_FILES}\\
Delete \\/REBOOTOK \"\$INSTDIR\\\\$fn\""
done
MICROSCOPYINSTALLER_DIRS=
for f in `find . -type d`
do
    fn=${f//\//\\\\}
	fn=${fn//.\\/}
    MICROSCOPYINSTALLER_DIRS="${MICROSCOPYINSTALLER_DIRS}\\
CreateDirectory  \"\$INSTDIR\\$fn\""
    
done
cd ..

echo $SPIMINSTALLER_FILES
echo $SPIMUNINSTALLER_FILES
echo $SPIMINSTALLER_DIRS

sed "s/%%INSTALLER_FILES%%/$INSTALLER_FILES/" nsis_basicscript.nsi > nsis_basicscript.~si
ls -l *.~*
sed "s/%%UNINSTALLER_FILES%%/$UNINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%SPIMINSTALLER_FILES%%/$SPIMINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%SPIMUNINSTALLER_FILES%%/$SPIMUNINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%SPIMINSTALLER_DIRS%%/$SPIMINSTALLER_DIRS/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*

sed "s/%%FCSINSTALLER_FILES%%/$FCSINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%FCSUNINSTALLER_FILES%%/$FCSUNINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%FCSINSTALLER_DIRS%%/$FCSINSTALLER_DIRS/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%IMFCSINSTALLER_FILES%%/$IMFCSINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%IMFCSUNINSTALLER_FILES%%/$IMFCSUNINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%IMFCSINSTALLER_DIRS%%/$IMFCSINSTALLER_DIRS/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*

sed "s/%%MICROSCOPYINSTALLER_FILES%%/$MICROSCOPYINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%MICROSCOPYUNINSTALLER_FILES%%/$MICROSCOPYUNINSTALLER_FILES/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*
sed "s/%%MICROSCOPYINSTALLER_DIRS%%/$MICROSCOPYINSTALLER_DIRS/" nsis_basicscript.~si > nsis_basicscript.~~s
cp -f nsis_basicscript.~~s nsis_basicscript.~si
ls -l *.~*

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

	rm -rf deploy
	rm -rf deployspim
	rm -rf deployfcs
	rm -rf deployimfcs
	rm -rf deploymicroscopy
	rm -rf deployspecial
	rm "${ZIPFILE}.backup"
	rm "${ZIPFILESPIM}.backup"
	rm "${ZIPFILEFCS}.backup"
	rm "${ZIPFILEIMFCS}.backup"
	rm "${ZIPFILEMICROSCOPY}.backup"
	rm "${ZIPFILESPECIAL}.backup"
fi
