#!/bin/bash

ZIPFILE=quickfit3.zip
REMOVEPLUGINS="cam_radhard2 cam_testcamera stage_pi863 cam_andor spimb040"

rm -rf deploy
cp ${ZIPFILE} "${ZIPFILE}.backup"
rm ${ZIPFILE}

mkdir -p deploy

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

for f in *
do
    if [ $f != "deploy" ]; then
	  cp -rv $f ./deploy
	fi
done

if ! cd deploy; then echo "could not create ./deploy/"; exit 1; fi
rm ./globalconfig/*
rmdir ./globalconfig
rm ./qtplugins/qt*d4.dll

find -name ".svn" -type d -exec rm -rf {} \;
find -name "*.log" -exec rm -rf {} \;
find -name "*.autosave" -exec rm -rf {} \;
find -name "*.a" -exec rm -rf {} \;
find -name "*.backup" -exec rm -rf {} \;
find -name "*.ts" -exec rm -rf {} \;
find -name "*.cpt" -exec rm -rf {} \;
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
rm -rf deploy
rm "${ZIPFILE}.backup"