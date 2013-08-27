#! /bin/bash

function replace_in_all {
	echo "replace: " $1 
	for i in ./$FNAME/*.cpp ./$FNAME/*.h ./$FNAME/*.pro ./$FNAME/*.qrc ./$FNAME/help/*.html; do 
	  echo "  -> " $i
	  sed $1 < $i > $i.temp 
	  mv $i.temp $i
	done
}

CLASSNAME=$1
TARGETNAME=$2
FITFUNCCLASSNAME=$3
FITFUNCNAME=$4
BASENAME=fitfunction_inst

echo "This script will create a set of usable files from this template. 
In this course you will be asked for some information (e.g. class names ...)!

"

if [ "$CLASSNAME" == "" ]; then
	echo -n "class name for QFFitFunction plugin implementation: "
	read -e CLASSNAME
else
	echo -n "class name for QFFitFunction plugin implementation: " $CLASSNAME "
"
fi

if [ "$TARGETNAME" == "" ]; then
	echo -n "name for plugin library (also plugin ID): "
	read -e TARGETNAME
else
	echo -n "name for plugin library (also plugin ID): " $TARGETNAME "
"
fi


if [ "$FITFUNCCLASSNAME" == "" ]; then
	echo -n "class name for first fit function: "
	read -e FITFUNCCLASSNAME
else
	echo -n "class name for first fit function: " $FITFUNCCLASSNAME "
"
fi


if [ "$FITFUNCNAME" == "" ]; then
	echo -n "ID for first fit function: "
	read -e FITFUNCNAME
else
	echo -n "ID for first fit function: " $FITFUNCNAME "
"
fi

CLASSNAME_UC=`echo $CLASSNAME | tr a-z A-Z`
CLASSNAMEFF_UC=`echo ${FITFUNCCLASSNAME} | tr a-z A-Z`

FNAME=`echo $CLASSNAME | tr A-Z a-z`
echo -n "file name for QFFitFunction plugin implementation: " $FNAME ".*
"
FNAMEFF=`echo $FITFUNCCLASSNAME | tr A-Z a-z`
echo -n "file name for QFFitFunction implementation: " $FNAMEFF ".*
"
mkdir -p $FNAME

for i in ${BASENAME}.*; do 
  cp -v "$i" "./$FNAME/${FNAME}${i/$BASENAME}"; 
done

for i in ${BASENAME}_f1.*; do 
  cp -v "$i" "./$FNAME/${FNAMEFF}${i/${BASENAME}_f1}"; 
done

mkdir -p $FNAME/translations
mkdir -p $FNAME/examples
mkdir -p $FNAME/help
mkdir -p $FNAME/help/pic
cp ftarget.html $FNAME/help/${TARGETNAME}.html
cp f1.html $FNAME/help/${FITFUNCNAME}.html
cp tutorial.html $FNAME/help/tutorial.html


replace_in_all 's/QFFitAlgorithmInst_F1/'$FITFUNCCLASSNAME'/g'
replace_in_all 's/QFFitAlgorithmInst/'$CLASSNAME'/g'
replace_in_all 's/target_id/'$TARGETNAME'/g'
replace_in_all 's/ff_id/'$FITFUNCNAME'/g'

replace_in_all 's/'$BASENAME'_f1.h/'$FNAMEFF'.h/g'
replace_in_all 's/'$BASENAME'_f1.cpp/'$FNAMEFF'.cpp/g'
replace_in_all 's/HEADER_F1_H/'$CLASSNAMEFF_UC'_H/g'

replace_in_all 's/'$BASENAME'.png/'$FNAME'.png/g'
replace_in_all 's/'$BASENAME'.h/'$FNAME'.h/g'
replace_in_all 's/'$BASENAME'.cpp/'$FNAME'.cpp/g'
replace_in_all 's/'$BASENAME'.qrc/'$FNAME'.qrc/g'
replace_in_all 's/'$BASENAME'.ui/'$FNAME'.ui/g'
replace_in_all 's/HEADER_H/'$CLASSNAME_UC'_H/g'

replace_in_all 's/doxygen_GROUPNAME/qf3fitfunp_'$TARGETNAME'/g'
replace_in_all 's/f1.html/'$FITFUNCNAME'.html/g'

rm $FNAME/*.temp