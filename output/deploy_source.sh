#!/bin/bash

function help {
	echo -e "SCRIPT TO DEPLOY the QUICKFIT SOURCE-CODE\n"
	echo -e "    (c) 2012-2014 by Jan W. Krieger\n"
	echo -e "  This script relies on bash and subversion!\n"
	echo -e "\ncommand line options:"
	echo -e "    -? --help        this help message"
	echo -e "    --nozip/--zip    switch off/on creation of ZIP archives for " 
	echo -e "                     deployment                       (default: make_zip)"
	echo -e "    --deployqt/nodeployqt  salso create a ZIP-file of the Qt folder" 
	echo -e "                                                      (default: off)"
	echo -e "    --nodeldeploy    do not clean up at the end of the process" 
	echo -e "                     (delete deploy directory)        (default: delete)"
	echo -e "    --nocreatedeploy create deploy directory at start of script" 
	echo -e "                                                      (default: create)"
}

showhelp="0"
createZIP="1"
runNSIS="0"
deployspecials="0"
delete_deploy="1"
create_deploy="1"
SPECIALS=""
deploy_qt="0"

until [ -z "$1" ]  # Until all parameters used up . . .
do
    if [ "${1}" == "--help" ]; then
	    showhelp="1"
	fi
    if [ "$1" == "-?" ]; then
	    showhelp="1"
	fi
    if [ "$1" == "--nozip" ]; then
	    createZIP="0"
	fi
    if [ "$1" == "--zip" ]; then
	    createZIP="1"
	fi
    if [ "$1" == "--nodeployqt" ]; then
	    deploy_qt="0"
	fi
    if [ "$1" == "--deployqt" ]; then
	    deploy_qt="1"
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

echo -e "DEPLOYING QUICKFIT SOURCE CODE"

svn update
svn update ../output
echo -e "\n\ndetermining SVN version:"
SVNVER=`svnversion`

echo -e "\n   SVN version: ${SVNVER}"
echo -e "\n\ndetermining compile date:"
COMPILEDATE=`date +%Y-%m-%d`
echo -e "\n   compile date: ${COMPILEDATE}"
echo -e "\n\ndetermining bit depth:"
#echo '
##include <stdio.h>
#int main() {
#	int i=sizeof(void*)*8;
#	printf("%d\n", i);
#	return 0;
#}
#' > test~.cpp
#g++ -o a.out test~.cpp
#BITDEPTH=`./a.out`
#rm a.out 
#rm test~.cpp
BITDEPTH=`./quickfit3.exe --getbits`
echo -e "\n   bit depth: ${BITDEPTH}\n\n"

THISDIR=`pwd`

QF3REPOSITORY=https://www.dkfz.de/svn/B040/FCSTOOLS/trunk/QuickFit3
LIBREPOSITORY=https://www.dkfz.de/svn/B040/LIB
COMPILETXT2_REPOSITORY=https://www.dkfz.de/svn/B040/compile_quickfit3.txt
COMPILETXT1_REPOSITORY=https://www.dkfz.de/svn/B040/install_mingw64_and_qt.txt
COMPILEIMGS_REPOSITORY=https://www.dkfz.de/svn/B040/

INSTALLER_BASENAME=quickfit3_source${SPECIALS}_${SVNVER}
INSTALLER_INSTDIR="\$PROGRAMFILES32"
ZIPFILE=quickfit3_source${SPECIALS}_${SVNVER}.zip
CAIRO4QTZIPFILE=cairo4qtv2_source_${SVNVER}.zip
TINYTIFFZIPFILE=tinytiff_source_${SVNVER}.zip
SPECIALPLUGINS=""
if [ "${deployspecials}" == "1" ]; then
	SPECIALPLUGINS=
fi


if [ "${create_deploy}" != "0" ]; then

	#rm -rf deploy_source_dir
	#cp ${ZIPFILE} "${ZIPFILE}.backup"
	#rm ${ZIPFILE}
	mkdir -p deploy_source_dir
	
	cd deploy_source_dir
	LIB_FILES=""
    echo -e "NOW IN:\n"
	pwd 
	echo -e "\n\nCHECKING OUT readme files:\n"
    svn export --force ${COMPILETXT2_REPOSITORY} .
    svn export --force ${COMPILETXT1_REPOSITORY} .
    svn export --force ${COMPILEIMGS_REPOSITORY}build_quickfit3_config_qtcreator_debug.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}build_quickfit3_config_qtcreator_release.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}build_quickfit3_config_qtcreator_run.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}qtcreator_kit_qt32.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}qtcreator_kit_qt64.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}qtcreator_mingw32.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}qtcreator_mingw64.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}qtcreator_qt32.png .
    svn export --force ${COMPILEIMGS_REPOSITORY}qtcreator_qt64.png .
	rm -rf .svn
	
	echo -e "\n\nCHECKING OUT QuickFit3:\n"
    svn checkout --force ${QF3REPOSITORY} ./FCSTOOLS/trunk/QuickFit3
    svn checkout --force ${QF3REPOSITORY} ./FCSTOOLS/trunk/QuickFit3
    svn checkout --force ${QF3REPOSITORY} ./FCSTOOLS/trunk/QuickFit3
	echo -e "\n\nCHECKING OUT QuickFit3-LIB:\n"
    svn checkout --force ${LIBREPOSITORY} ./LIB
    svn checkout --force ${LIBREPOSITORY} ./LIB
    svn checkout --force ${LIBREPOSITORY} ./LIB
	
	echo -e "\n\nCREATING ${CAIRO4QTZIPFILE}:\n"
	mkdir cairo4qtv2
	cp -rf ./FCSTOOLS/trunk/QuickFit3/plugins/qfe_plotterexportercairo/qcairo*.* ./cairo4qtv2
	cp -rf ./FCSTOOLS/trunk/QuickFit3/plugins/qfe_plotterexportercairo/cairoQ*.* ./cairo4qtv2
	cp -rf ./FCSTOOLS/trunk/QuickFit3/gpl-3.0.txt ./cairo4qtv2
	cd cairo4qtv2
	zip -rv9 ../../${CAIRO4QTZIPFILE} *
	cd ..
	rm -rf cairo4qtv2
	
	echo -e "\n\nCREATING ${TINYTIFFZIPFILE}:\n"
	mkdir tinytiff
	mkdir tinytiff/test
	mkdir tinytiff/test/tinytiff_reader_test
	mkdir tinytiff/test/tinytiffwriter_test
	cp -rf ./LIB/trunk/tinytiff*.* ./tinytiff
	cp -rf ./LIB/trunk/lib_imexport.h ./tinytiff
	cp -rf ./LIB/trunk/libtiff_tools.* ./tinytiff
	cp -rf ./LIB/trunk/highrestimer.* ./tinytiff
	cp -rf ./LIB/trunk/tools.* ./tinytiff
	cp -rf ./LIB/trunk/test/tinytiffwriter_test/* ./tinytiff/test/tinytiffwriter_test/
	cp -rf ./LIB/trunk/test/tinytiff_reader_test/* ./tinytiff/test/tinytiff_reader_test/
	cp -rf ./FCSTOOLS/trunk/QuickFit3/gpl-3.0.txt ./tinytiff
	cd tinytiff
	zip -rv9 ../../${TINYTIFFZIPFILE} *
	cd ..
	rm -rf tinytiff
	
	echo -e "\n\nDELETING .svn:\n"
	for f in `find . -type d -name .svn`
	do
		rm -rf $f
	done
	
	rm ./FCSTOOLS/trunk/QuickFit3/extlibs/andor_win32/ATMCD32D.DLL
	rm ./FCSTOOLS/trunk/QuickFit3/extlibs/andor_win32/ATMCD32D.H
	rm ./FCSTOOLS/trunk/QuickFit3/extlibs/andor_win64/atmcd64d.DLL
	rm ./FCSTOOLS/trunk/QuickFit3/extlibs/andor_win64/ATMCD32D.H
	
	LIB_FILES=`../../tools/qf3sourcedeploy/qf3sourcedeploy ./FCSTOOLS`
	
	TEMP_VAR=`find ./LIB/trunk/test/multitau-correlator_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/jkserial_testQt -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/jkserial_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/jkmathparser_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/jkiniparser2_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/datatable2_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/statistics_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/tinytiff_reader_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/tinytiffwriter_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/test/videocapture_testQt -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/qt/test/jkfloatedit_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/qt/test/jkqtfastplotter_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/qt/test/jkqtmathtext_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/qt/test/jkqtplot_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/qt/test/qfasttablelabel_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/qt/test/QKeySequenceEdit_test -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	TEMP_VAR=`find ./LIB/trunk/doc -type f`
	LIB_FILES="${LIB_FILES} ${TEMP_VAR}"
	
	
	for f in `find ./LIB -type f`
	do
	    if [[ !( $LIB_FILES =~ $f ) ]]; then
			rm -rf $f
		fi
	done

	for f in `find . -type d -name .svn`
	do
		rm -rf $f
	done
	
	cp ./FCSTOOLS/trunk/QuickFit3/quickfit.inc.deploy ./FCSTOOLS/trunk/QuickFit3/quickfit.inc


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
	rm releasenotes.~~h releasenotes.~ht
	
	
	echo -e "\n\nCREATING README:\n\n"
	echo  "
-------------------------------------------------------------------
-- SOURCE RELEASE of QuickFit 3.0 
-------------------------------------------------------------------
--  date: $COMPILEDATE
--  SVN-version: $SVN
-------------------------------------------------------------------
-- http://www.dkfz.de/Macromol/quickfit/
-------------------------------------------------------------------

This repository contains all files, necessary to build QuickFit 3.0.

See the file compile_quickfit3.txt for information on compiling QF3
and install_mingw64_and_qt.txt for instructions on installing a suitable
64-bit MinGW compiler on a windows system.

" > readme.~txt
	sed "s/\\\$\\\$COMPILEDATE\\\$\\\$/$COMPILEDATE/g" readme.~txt > readme.~~txt
	cp -f readme.~~txt readme.~txt
	sed "s/\\\$\\\$SVN\\\$\\\$/$SVNVER/g" releasenotes.~ht > releasenotes.~~h
	cp -f readme.~~txt readme.~txt
	cp -f readme.~txt readme.txt
	rm readme.~~txt readme.~txt
	cd ..

fi

if [ "${createZIP}" != "0" ]; then
    cd deploy_source_dir
	echo -e "\n\nCREATING ZIP ARCHIVES FOR DEPLOYMENT:\n  output: ${ZIPFILE}\n\n"
	zip -rv9 ../${ZIPFILE} *
	cd ..	
fi

if [ "${deploy_qt}" != "0" ]; then
    QTPATH=`qmake -query QT_INSTALL_PREFIX`
	QTVERSION=`qmake -query QT_VERSION`
	ZIPFILEQT="qt-${QTVERSION}-win${BITDEPTH}-mingw.zip"
    cd $QTPATH
	echo -e "\n\nCREATING ZIP ARCHIVE OF QT:\n  output: ${ZIPFILEQT}\n  Qt path: ${QTPATH}\n  Qt version: ${QTVERSION}\n\n"
	zip -rv9 ${THISDIR}/${ZIPFILEQT} * -x *.o
	cd ${THISDIR}	
fi


if [ "${delete_deploy}" != "0" ]; then
	echo -e "\n\nCLEANUP:\n\n"

	rm -rf deploy_source_dir
	rm "${ZIPFILE}.backup"
fi
