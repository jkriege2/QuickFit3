
function help {
	echo -e "SCRIPT TO BUILD AN APP-BUNDLE of QUICKFIT on MacOS X (64bit)\n"
	echo -e "    (c) 2015 by Jan W. Krieger\n"
	echo -e "\ncommand line options:"
	echo -e "    -? --help        this help message"
	echo -e "    --nozip/--zip    switch off/on creation of ZIP archives for " 
	echo -e "                     deployment                       (default: on)"
	echo -e "    --simple         run in a simplified mode, which does not run "
	echo -e "                     make install and does not copy the assets.  "
	echo -e "                                                     (default: off)"
}

showhelp="0"
simple="0"
createZIP="1"

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
    if [ "$1" == "--simple" ]; then
	    simple="1"
		createZIP="0"
	fi
	shift;
done


if [ "$showhelp" != "0" ]; then
	help
	exit
fi


unset DYLD_PRINT_LIBRARIES

echo "--- MAKE INSTALL ---"

QT_INFO_LIBS=`qmake -query QT_INSTALL_LIBS`
QT_INFO_BIN=`qmake -query QT_INSTALL_BINS`
QT_INFO_PLUGINS=`qmake -query QT_INSTALL_PLUGINS`
QT_INFO_INSTALLDIR=`qmake -query QT_INSTALL_PREFIX`
QT_INFO_VERSION=`qmake -query QT_VERSION`
echo -e "\n\nbuilding for\n    Qt version ${QT_INFO_VERSION}\n       in ${QT_INFO_INSTALLDIR}\n\n"

QTLIBPATH="${QT_INFO_LIBS}/"
QTPLUGINPATH="${QT_INFO_PLUGINS}/"

QTLIBS=(QtCore QtWidgets QtOpenGL QtSvg QtXml QtPrintSupport QtNetwork QtMultimedia QtMacExtras QtGui QtConcurrent)
QTPLUGINS=(platforms imageformats iconengines printsupport bearer)
QTLIBVERSION=5

ASSETSPATH=./quickfit3.app/Contents/SharedSupport/
PLUGINSPATH=./quickfit3.app/Contents/PlugIns
QTPLUGINSPATH=./quickfit3.app/Contents/QtPlugIns



echo "--- determining QuickFit 3 version ---"

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
BITDEPTH=`./quickfit3.app/Contents/MacOs/quickfit3 --getbits`
echo -e "\n   bit depth: ${BITDEPTH}\n\n"

ZIPFILE=quickfit3_macx${BITDEPTH}_${SVNVER}.zip

DMGFILE=quickfit3_macx${BITDEPTH}_${SVNVER}.dmg

echo "--- make install of QuickFit 3 ---"
cd ..
make install -j32
cd output

echo "--- installing QF plugins and libs into bundle ---"
rm -r ./quickfit3.app/Contents/Frameworks/quickfit3lib.framework
rm -r ./quickfit3.app/Contents/Frameworks/quickfit3widgets.framework

rm -rf ${PLUGINSPATH}
mkdir -p ${PLUGINSPATH}
cp -R ./plugins/* ${PLUGINSPATH}/

mkdir -p ${QTPLUGINSPATH}

mkdir -p ./quickfit3.app/Contents/Frameworks/quickfit3lib.framework
cp -R ./quickfit3lib.framework ./quickfit3.app/Contents/Frameworks/

mkdir -p ./quickfit3.app/Contents/Frameworks/quickfit3widgets.framework
cp -R ./quickfit3widgets.framework ./quickfit3.app/Contents/Frameworks/


if [ "$simple" == "0" ]; then
	echo "--- installing assets into bundle ---"
	rm -rf ${ASSETSPATH}/assets
	mkdir -p ${ASSETSPATH}/assets
	cp -R ./assets ${ASSETSPATH}

	echo "--- installing examples into bundle ---"
	rm -rf ${ASSETSPATH}/examples
	mkdir -p ${ASSETSPATH}/examples
	cp -R ./examples ${ASSETSPATH}

	echo "--- installing sdk into bundle ---"
	rm -rf ${ASSETSPATH}/sdk
	mkdir -p ${ASSETSPATH}/sdk
	cp -R ./sdk ${ASSETSPATH}

	echo "--- installing sources into bundle ---"
	rm -rf ${ASSETSPATH}/source
	mkdir -p ${ASSETSPATH}/source
	cp -R ./source ${ASSETSPATH}
fi

echo "--- INSTALL_NAME_TOOL: basics ---"
install_name_tool -id @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib quickfit3.app/Contents/Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib
install_name_tool -id @executable_path/../Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets quickfit3.app/Contents/Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets
install_name_tool -change quickfit3lib.framework/Versions/1/quickfit3lib @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib quickfit3.app/Contents/MacOS/quickfit3
install_name_tool -change quickfit3widgets.framework/Versions/1/quickfit3widgets @executable_path/../Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets quickfit3.app/Contents/MacOS/quickfit3
install_name_tool -change quickfit3lib.framework/Versions/1/quickfit3lib @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib quickfit3.app/Contents/Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets


echo "--- INSTALL_NAME_TOOL: plugins ---"

find ${PLUGINSPATH} -name "*.dylib" -exec install_name_tool -change quickfit3lib.framework/Versions/1/quickfit3lib @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib {} \;
find ${PLUGINSPATH} -name "*.dylib" -exec install_name_tool -change quickfit3widgets.framework/Versions/1/quickfit3widgets @executable_path/../Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets {} \;





if [ "$simple" == "0" ]; then
	echo "--- INSTALL_NAME_TOOL: Qt plugins ---"
	echo "     QTPLUGINPATH = ${QTPLUGINPATH}"
	echo "  (1) copy Qt plugins"
	for f in "${QTPLUGINS[@]}"
	do
			echo "        - ${f}"
			mkdir -p ${QTPLUGINSPATH}/${f}
			cp -R ${QTPLUGINPATH}${f}/*.dylib ${QTPLUGINSPATH}/${f}/
			rm -Rf ${QTPLUGINSPATH}${f}/*_debug.dylib
	done
fi

echo "--- INSTALL_NAME_TOOL: Qt libs ---"
if [ -n "$1" ]; then
		QTLIBPATH=$1
fi
echo "     QTLIBPATH = ${QTLIBPATH}"
echo "     PWD :"
pwd
echo "  (1) copy Qt libs"
for f in "${QTLIBS[@]}"
do
		echo "        - ${f}.framework"
		cp -R ${QTLIBPATH}${f}.framework ./quickfit3.app/Contents/Frameworks/
		#echo "removing ./quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}_debug"
		rm -rf ./quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}_debug
		rm -rf ./quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}_debug*
		#echo "removing ./quickfit3.app/Contents/Frameworks/${f}.framework/${f}_debug"
		rm -rf ./quickfit3.app/Contents/Frameworks/${f}.framework/${f}_debug
		rm -rf ./quickfit3.app/Contents/Frameworks/${f}.framework/${f}_debug*

	   install_name_tool -id @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}
	   install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/MacOS/quickfit3
	   install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets
	   install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib

	   find . -name "*.dylib" -exec install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} {} \;
	   if [ "$simple" == "0" ]; then
			find ${QTPLUGINSPATH} -name "*.dylib" -exec install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} {} \;
	   fi
	   find ${PLUGINSPATH} -name "*.dylib" -exec install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} {} \;

	   for ff in "${QTLIBS[@]}"
	   do
		   install_name_tool -change ${QTLIBPATH}${ff}.framework/Versions/${QTLIBVERSION}/${ff} @executable_path/../Frameworks/${ff}.framework/Versions/${QTLIBVERSION}/${ff} quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}
	   done
done


if [ "$createZIP" != "0" ]; then
	echo "--- preparing distribution directory ---"

	rm -Rf dist
	mkdir dist
	cp *.txt dist/
	cp -R quickfit3.app dist/

	echo "--- preparing release notes ---"

	. create_releasenotes.sh ${SVNVER} ${COMPILEDATE}
	cp releasenotes.html ./dist/quickfit3_releasenotes.html

	echo "--- creating DMG of app-bundle ---"

	rm -rf ./${DMGFILE}
	rm -rf ./${ZIPFILE}
	hdiutil create ./${DMGFILE} -srcfolder ./dist/ -ov

	echo "--- creating ZIP of app-bundle ---"

	cd dist
	zip -rv9 ../${ZIPFILE} *
	cd ..

	echo "--- CLEANUP ---"

	rm -rf dist
fi

echo "--- QuickFit 3 building bundle&distribution done! ---"
