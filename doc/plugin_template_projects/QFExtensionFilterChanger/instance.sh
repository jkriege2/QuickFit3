#! /bin/bash

function replace_in_all {
	echo "replace: " $1 
	for i in ./$DIRNAME/*.cpp ./$DIRNAME/*.h ./$DIRNAME/*.pro ./$DIRNAME/*.qrc ./$DIRNAME/help/*.html; do 
	  echo "  -> " $i
	  sed $1 $i > $i.temp | mv $i.temp $i
	done
}

CLASSNAME=$1
TARGETNAME=$2
BASENAME=qfextensiontestfilterchanger

echo "This script will create a set of usable files from this template. 
In this course you will be asked for some information (e.g. class names ...)!

"

if [ "$CLASSNAME" == "" ]; then
	echo -n "class name for QFExtension implementation: "
	read -e CLASSNAME
else
	echo -n "class name for QFExtension implementation: " $CLASSNAME "
"
fi

if [ "$TARGETNAME" == "" ]; then
	echo -n "name for plugin library (also pligin ID): "
	read -e TARGETNAME
else
	echo -n "name for plugin library (also pligin ID): " $TARGETNAME "
"
fi

CLASSNAME_UC=`echo $CLASSNAME | tr a-z A-Z`

FNAME=`echo $CLASSNAME | tr A-Z a-z`
echo -n "file name for QFExtension implementation: " $FNAME ".*
"
DIRNAME=`echo $TARGETNAME | tr A-Z a-z`
echo -n "file name for QFExtension implementation: " $DIRNAME ".*
"
mkdir -p $DIRNAME

for i in ${BASENAME}.*; do 
  cp -v "$i" "./$DIRNAME/${FNAME}${i/$BASENAME}"; 
done

mkdir -p $DIRNAME/translations
mkdir -p $FNAME/examples
mkdir -p $DIRNAME/assets
mkdir -p $DIRNAME/help
mkdir -p $DIRNAME/help/pic
cp ftarget.html $DIRNAME/help/${TARGETNAME}.html
cp project.pro $DIRNAME/${DIRNAME}.pro
cp tutorial.html $DIRNAME/help/tutorial.html

replace_in_all 's/QFExtensionTestFilterChanger/'$CLASSNAME'/g'
replace_in_all 's/filterc_test/'$TARGETNAME'/g'
replace_in_all 's/'$BASENAME'.png/'$FNAME'.png/g'
replace_in_all 's/'$BASENAME'.h/'$FNAME'.h/g'
replace_in_all 's/'$BASENAME'.cpp/'$FNAME'.cpp/g'
replace_in_all 's/'$BASENAME'.qrc/'$FNAME'.qrc/g'
replace_in_all 's/qf3ext_filterc_test/qf3ext_'$TARGETNAME'/g'
replace_in_all 's/QFEXTENSIONTESTFILTERCHANGER_H/'$CLASSNAME_UC'_H/g'
