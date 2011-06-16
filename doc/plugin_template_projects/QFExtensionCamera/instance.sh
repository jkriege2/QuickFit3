#! /bin/bash

CLASSNAME=$1
BASENAME=qfextensioncamera

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
#echo $CLASSNAME
FNAME=`echo $CLASSNAME | tr A-Z a-z`
echo -n "file name for QFExtensionCamera implementation: " $FNAME ".*
"
mkdir -p $FNAME

for i in ${BASENAME}.*; do 
  cp -v "$i" "./$FNAME/${FNAME}${i/$BASENAME}"; 
done


#echo $FNAME