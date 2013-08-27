#! /bin/bash

function replace_in_all {
	echo "replace: " $1 
	for i in ./$FNAME/*.cpp ./$FNAME/*.h ./$FNAME/*.pro ./$FNAME/*.qrc $FNAME/help/*.html; do 
	  echo "  -> " $i
	  sed $1 $i > $i.temp 
	  mv $i.temp $i
	done
}

CLASSNAME=$1
TARGETNAME=$2
BASENAME=qfextensionlinearstage_imp

echo "This script will create a set of usable files from this template. 
In this course you will be asked for some information (e.g. class names ...)!

"

if [ "$CLASSNAME" == "" ]; then
	echo -n "class name for QFExtensionLinearStage implementation: "
	read -e CLASSNAME
else
	echo -n "class name for QFExtensionLinearStage implementation: " $CLASSNAME "
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
echo -n "file name for QFExtensionLinearStage implementation: " $FNAME ".*
"
mkdir -p $FNAME

for i in ${BASENAME}.*; do 
  cp -v "$i" "./$FNAME/${FNAME}${i/$BASENAME}"; 
done

mkdir -p $FNAME/translations
mkdir -p $FNAME/assets
mkdir -p $FNAME/help
mkdir -p $FNAME/help/pic
cp ftarget.html $FNAME/help/${TARGETNAME}.html
cp tutorial.html $FNAME/help/tutorial.html

replace_in_all 's/QFExtensionLinearStageImplementation/'$CLASSNAME'/g'
replace_in_all 's/target_id/'$TARGETNAME'/g'
replace_in_all 's/'$BASENAME'.png/'$FNAME'.png/g'
replace_in_all 's/'$BASENAME'.h/'$FNAME'.h/g'
replace_in_all 's/'$BASENAME'.cpp/'$FNAME'.cpp/g'
replace_in_all 's/'$BASENAME'.qrc/'$FNAME'.qrc/g'
replace_in_all 's/qf3ext_GROUPNAME/qf3ext_'$TARGETNAME'/g'
replace_in_all 's/HEADER_H/'$CLASSNAME_UC'_H/g'
