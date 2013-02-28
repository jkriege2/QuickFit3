echo '
#include <stdio.h>
int main() {
    int i=sizeof(void*)*8;
    printf("%d\n", i);
	return 0;
}
' > test~.cpp
g++ -o a.out test~.cpp
BITDEPTH=`./a.out`
rm a.out test~.cpp

ZIPFILESPIM=quickfit3_spimplugins_${BITDEPTH}.zip
echo ${ZIPFILESPIM}
