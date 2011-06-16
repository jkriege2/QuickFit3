#! /bin/bash

function replace_in_all {
	echo "replace: " $1 
	for i in ./$FNAME/*.cpp ./$FNAME/*.h ./$FNAME/*.pro ./$FNAME/*.qrc; do 
	  echo "  -> " $i
	  sed $1 $i > $i.temp | mv $i.temp $i
	done
}

CLASSNAME=$1
TARGETNAME=$2
BASENAME=qfextensioncamera_imp

echo "This script will create a set of usable files from this template. 
In this course you will be asked for some information (e.g. class names ...)!

"

if [ "$CLASSNAME" == "" ]; then
	echo -n "class name for QFExtensionCamera implementation: "
	read -e CLASSNAME
else
	echo -n "class name for QFExtensionCamera implementation: " $CLASSNAME "
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
echo -n "file name for QFExtensionCamera implementation: " $FNAME ".*
"
mkdir -p $FNAME

for i in ${BASENAME}.*; do 
  cp -v "$i" "./$FNAME/${FNAME}${i/$BASENAME}"; 
done


replace_in_all 's/QFExtensionCameraImplementation/'$CLASSNAME'/g'
replace_in_all 's/cam_mycamname/'$TARGETNAME'/g'
replace_in_all 's/'$BASENAME'.png/'$FNAME'.png/g'
replace_in_all 's/'$BASENAME'.h/'$FNAME'.h/g'
replace_in_all 's/'$BASENAME'.cpp/'$FNAME'.cpp/g'
replace_in_all 's/'$BASENAME'.qrc/'$FNAME'.qrc/g'
replace_in_all 's/qf3ext_GROUPNAME/qf3ext_cam_'$TARGETNAME'/g'
replace_in_all 's/QFEXTENSIONCAMERA_H/'$CLASSNAME_UC'_H/g'
