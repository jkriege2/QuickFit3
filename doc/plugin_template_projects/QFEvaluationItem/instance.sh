#! /bin/bash

function replace_in_all {
	echo "replace: " $1 
	for i in ./$FNAME/*.cpp ./$FNAME/*.h ./$FNAME/*.ui ./$FNAME/*.pro ./$FNAME/*.qrc ./$FNAME/help/*.html ./$FNAME/help/*.ini; do 
	  echo "  -> " $i
	  sed $1 $i > $i.temp 
	  mv $i.temp $i
	done
}

CLASSNAME=$1
TARGETNAME=$2
BASENAME=qfeval

echo "This script will create a set of usable files from this template. 
In this course you will be asked for some information (e.g. class names ...)!

"

if [ "$CLASSNAME" == "" ]; then
	echo -n "class name for QFEvaluationItem implementation: "
	read -e CLASSNAME
else
	echo -n "class name for QFEvaluationItem implementation: " $CLASSNAME "
"
fi

if [ "$TARGETNAME" == "" ]; then
	echo -n "name for plugin library (also plugin ID): "
	read -e TARGETNAME
else
	echo -n "name for plugin library (also plugin ID): " $TARGETNAME "
"
fi

CLASSNAME_UC=`echo $CLASSNAME | tr a-z A-Z`

FNAME=`echo $TARGETNAME | tr A-Z a-z`
echo -n "file name for QFEvaluationItem implementation: " $FNAME ".*
"
mkdir -p $FNAME

for i in ${BASENAME}*.*; do 
  cp -v "$i" "./$FNAME/${FNAME}${i/$BASENAME}"; 
done
for i in target_id*.*; do 
  cp -v "$i" "./$FNAME/${TARGETNAME}${i/target_id}"; 
done
mkdir -p $FNAME/translations
mkdir -p $FNAME/examples
mkdir -p $FNAME/assets
mkdir -p $FNAME/help
mkdir -p $FNAME/help/pic
cp -v help.html $FNAME/help/${TARGETNAME}.html
cp -v tutorial.html $FNAME/help/tutorial.html
cp -v tutorials.ini $FNAME/help/tutorials.ini
mkdir -p $FNAME/images
mv $FNAME/*.png $FNAME/images/
mv $FNAME/*.svg $FNAME/images/
 
replace_in_all 's/target_id/'$TARGETNAME'/g'
replace_in_all 's/qf3evalp_GROUPNAME/qf3evalp_'$TARGETNAME'/g'

replace_in_all 's/'$BASENAME'.png/'$FNAME'.png/g'
replace_in_all 's/'$BASENAME'_small.png/'$FNAME'_small.png/g'
replace_in_all 's/'$BASENAME'_print.png/'$FNAME'_print.png/g'
replace_in_all 's/'$BASENAME'_save.png/'$FNAME'_save.png/g'
replace_in_all 's/'$BASENAME'_printreport.png/'$FNAME'_printreport.png/g'
replace_in_all 's/'$BASENAME'_savereport.png/'$FNAME'_savereport.png/g'
replace_in_all 's/'$BASENAME'_evaluate.png/'$FNAME'_evaluate.png/g'
replace_in_all 's/QFEVAL/'$CLASSNAME'/g'
replace_in_all 's/QFPEVAL/'$CLASSNAME'Plugin/g'
replace_in_all 's/'$BASENAME'.h/'$FNAME'.h/g'
replace_in_all 's/'$BASENAME'.cpp/'$FNAME'.cpp/g'
replace_in_all 's/'$BASENAME'.qrc/'$FNAME'.qrc/g'
replace_in_all 's/HEADER_H/'$CLASSNAME_UC'/g'
replace_in_all 's/'$BASENAME'_item.png/'$FNAME'_item.png/g'
replace_in_all 's/'$BASENAME'_item.h/'$FNAME'_item.h/g'
replace_in_all 's/'$BASENAME'_item.cpp/'$FNAME'_item.cpp/g'
replace_in_all 's/'$BASENAME'_item.qrc/'$FNAME'_item.qrc/g'
replace_in_all 's/'$BASENAME'_item.ui/'$FNAME'_item.ui/g'
replace_in_all 's/'$BASENAME'_editor.png/'$FNAME'_editor.png/g'
replace_in_all 's/'$BASENAME'_editor.h/'$FNAME'_editor.h/g'
replace_in_all 's/'$BASENAME'_editor.cpp/'$FNAME'_editor.cpp/g'
replace_in_all 's/'$BASENAME'_editor.qrc/'$FNAME'_editor.qrc/g'
replace_in_all 's/'$BASENAME'_editor.ui/'$FNAME'_editor.ui/g'
