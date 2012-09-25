echo "--- MAKE INSTALL ---"

QTLIBPATH=/Users/langowsk/QtSDK/Desktop/Qt/4.8.1/gcc/lib/
QTLIBPATHCOPY=/Users/langowsk/QtSDK/Desktop/Qt/4.8.1/gcc/lib/
QTLIBS=(QtCore QtGui QtOpenGL QtSvg QtXml)
QTLIBVERSION=4

cd ..
make install
cd output

rm -r ./quickfit3.app/Contents/Frameworks/quickfit3lib.framework
rm -r ./quickfit3.app/Contents/Frameworks/quickfit3widgets.framework

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
cd plugins
find . -name "*.dylib" -exec install_name_tool -change quickfit3lib.framework/Versions/1/quickfit3lib @executable_path/../Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib {} \;
find . -name "*.dylib" -exec install_name_tool -change quickfit3widgets.framework/Versions/1/quickfit3widgets @executable_path/../Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets {} \;
cd ..





if [ "$1" != "--noqt" ]; then

echo "--- INSTALL_NAME_TOOL: Qt libs ---"
if [ -n "$1" ]; then
	QTLIBPATH=$1
fi
echo "     QTLIBPATH = ${QTLIBPATH}"
echo "  (1) copy Qt libs"
for f in "${QTLIBS[@]}"
do
	echo "        - ${f}.framework"
	cp -R ${QTLIBPATHCOPY}${f}.framework ./quickfit3.app/Contents/Frameworks/

       install_name_tool -id @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}
       install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/MacOS/quickfit3
       install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/quickfit3widgets.framework/Versions/1/quickfit3widgets
       install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} quickfit3.app/Contents/Frameworks/quickfit3lib.framework/Versions/1/quickfit3lib
       find . -name "*.dylib" -exec install_name_tool -change ${QTLIBPATH}${f}.framework/Versions/${QTLIBVERSION}/${f} @executable_path/../Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f} {} \;
       
       for ff in "${QTLIBS[@]}"
       do
           install_name_tool -change ${QTLIBPATH}${ff}.framework/Versions/${QTLIBVERSION}/${ff} @executable_path/../Frameworks/${ff}.framework/Versions/${QTLIBVERSION}/${ff} quickfit3.app/Contents/Frameworks/${f}.framework/Versions/${QTLIBVERSION}/${f}
       done
done

fi


#${QTLIBPATHCOPY}../bin/macdeployqt quickfit3.app -verbose=3 -no-strip

