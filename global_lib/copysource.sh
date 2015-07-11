#! /bin/sh

if [ $# -eq 0 ]
then
echo "$0 : You must supply a directory to copy to!"
exit 1
fi


rsync -ravz ./ --filter='+ */' --filter='+ *.cpp' --filter='+ *.h' --filter='+ *.c' --filter='+ *.sh' --filter='+ *.hpp' --filter='+ *.ini' --filter='+ *.depend' --filter='+ doc/' --filter='+ Doxyfile' --filter='+ doxyfile' --filter='+ *.dox' --filter='+ *.msk' --filter='+ *.cbp' --filter='+ *.pro' --filter='+ *.qrc' --filter='+ images/*' --filter='exclude *' $*

