unset DYLD_PRINT_LIBRARIES

echo "--- MAKE INSTALL ---"

QTLIBPATH=/Applications/Qt/5.4/clang_64/lib/
QTPLUGINPATH=/Applications/Qt/5.4/clang_64/plugins/

QTLIBS=(QtCore QtWidgets QtOpenGL QtSvg QtXml QtPrintSupport QtNetwork QtMultimedia QtMacExtras QtGui QtConcurrent)
QTPLUGINS=(platforms imageformats iconengines printsupport bearer)
QTLIBVERSION=5

ASSETSPATH=./quickfit3.app/Contents/SharedSupport/
PLUGINSPATH=./quickfit3.app/Contents/PlugIns
QTPLUGINSPATH=./quickfit3.app/Contents/QtPlugIns




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


echo "--- INSTALL_NAME_TOOL: basics ---"
install_name_tool -id @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib quickfit3.app/Contents/Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib
install_name_tool -id @executable_path/../Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets quickfit3.app/Contents/Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets
install_name_tool -change quickfit3lib.framework/Versions/1/quickfit3lib @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib quickfit3.app/Contents/MacOS/quickfit3
install_name_tool -change quickfit3widgets.framework/Versions/1/quickfit3widgets @executable_path/../Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets quickfit3.app/Contents/MacOS/quickfit3
install_name_tool -change quickfit3lib.framework/Versions/1/quickfit3lib @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib quickfit3.app/Contents/Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets


echo "--- INSTALL_NAME_TOOL: plugins ---"

find ${PLUGINSPATH} -name "*.dylib" -exec install_name_tool -change quickfit3lib.framework/Versions/1/quickfit3lib @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib {} \;
find ${PLUGINSPATH} -name "*.dylib" -exec install_name_tool -change quickfit3widgets.framework/Versions/1/quickfit3widgets @executable_path/../Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets {} \;





if [ "$1" != "--noqt" ]; then



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

           install_name_tool -id @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}
           install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/MacOS/quickfit3
           install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets
           install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib

           find . -name "*.dylib" -exec install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} {} \;
           find ${QTPLUGINSPATH} -name "*.dylib" -exec install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} {} \;
           find ${PLUGINSPATH} -name "*.dylib" -exec install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} {} \;

           for ff in "${QTLIBS[@]}"
           do
               install_name_tool -change ${QTLIBPATH}${ff}.framework/Versions/${QTLIBVERSION}/${ff} @executable_path/../Frameworks/${ff}.framework/Versions/${QTLIBVERSION}/${ff} quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}
           done
    done

fi

echo "--- QuickFit 3 quick-bundle done! ---"
