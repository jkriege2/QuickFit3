SVNVER=$1
COMPILEDATE=$2


echo -e "\n\nCREATING RELEASE NOTES:\n\n"
sed "/%%RELEASE_NOTES%%/ {
  r ../application/releasenote.html
  d
}" releasenotes_template.html > releasenotes.~ht
sed "s/\\\$\\\$SVN\\\$\\\$/$SVNVER/g" releasenotes.~ht > releasenotes.~~h
cp -f releasenotes.~~h releasenotes.~ht
sed "s/\\\$\\\$COMPILEDATE\\\$\\\$/$COMPILEDATE/g" releasenotes.~ht > releasenotes.~~h
cp -f releasenotes.~~h releasenotes.~ht
cp releasenotes.~ht releasenotes.html
rm *.~*
