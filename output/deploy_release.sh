#!/bin/bash

echo -e "DEPLOYING QUICKFIT\n  in order to deploy without recompiling, call with option --nomake"

SVNVER=`svnversion`
#if [ -z $SVNVER]
#	SVNVER=newest
#fi
ZIPFILE=quickfit3_${SVNVER}.zip
ZIPFILESPIM=quickfit3_spimplugins_${SVNVER}.zip
SPIMPLUGINS=" cam_testcamera stage_pi863 cam_andor spimb040 shutter_servo_arduino filterc_test cam_systemcam filterc_tmcl lights_b040laserbox lights_pccsled"
REMOVEPLUGINS=" ${SPIMPLUGINS} cam_radhard2 cam_rh2v2 alv_autocorrelator5000 b040_ffmcontrol multicontrol_stage qfe_acquisitiontest scanner2000_nicounter"
SPIMONLYQTMODULES=" QtScript4.dll QtScriptTools4.dll"

rm -rf deploy
rm -rf deployspim
cp ${ZIPFILE} "${ZIPFILE}.backup"
rm ${ZIPFILE}
cp ${ZIPFILESPIM} "${ZIPFILESPIM}.backup"
rm ${ZIPFILESPIM}

mkdir -p deploy


if [ "$1" != "--nomake" ]; then
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
	  cp -rv $f ./deploy
	fi
done

if ! cd deploy; then echo "could not create ./deploy/"; exit 1; fi
rm ./globalconfig/*
rmdir ./globalconfig
rm ./qtplugins/q*d4.dll
rm ./Qt*d4.dll
rm

for f in $SPIMONLYQTMODULES
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

mkdir ../deployspim
mkdir ../deployspim/plugins
mkdir ../deployspim/globalconfig_templates
mkdir ../deployspim/source
mkdir ../deployspim/assets
mkdir ../deployspim/assets/plugins
mkdir ../deployspim/assets/plugins/help

cp -rf ./globalconfig_templates/* ../deployspim/globalconfig_templates
for f in $SPIMONLYQTMODULES
do
    cp -rf "../${f}" ../deployspim
done

for f in $SPIMPLUGINS
do
	mkdir "../deployspim/assets/plugins/${f}"
	mkdir "../deployspim/assets/plugins/help/${f}"
    cp -rf  "./assets/plugins/${f}" "../deployspim/assets/plugins/"
    cp -rf  "./assets/plugins/help/${f}" "../deployspim/assets/help/plugins/"
	find -name "${f}.*" -exec cp -rf "{}" "../deployspim/{}" \;
done

for f in $REMOVEPLUGINS
do
    rm -rf  "./assets/plugins/${f}"
    rm -rf  "./assets/plugins/help/${f}"
	find -name "${f}.*" -exec rm -rf {} \;
done

rm *.sh
rm *.ini
rm ATMCD32D.DLL

zip -rv9 ../${ZIPFILE} *
cd ..
cd deployspim
zip -rv9 ../${ZIPFILESPIM} *
cd ..

rm -rf deploy
rm -rf deployspim
rm "${ZIPFILE}.backup"
rm "${ZIPFILESPIM}.backup"