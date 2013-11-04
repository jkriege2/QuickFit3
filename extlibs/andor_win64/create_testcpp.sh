#!/bin/sh
echo 'This script will read ATMCD32D.H and from this create a call to every function, declared in this header
in a file test.cpp. This one is then used to create a def file, also containing the @nn part of the dll
functions. calling dlltool finally working libandor.a is created!'

echo '
#include <stdio.h>

#ifdef __GNUC__
#  if(__GNUC__ > 3 || __GNUC__ ==3)
#	define _GNUC3_
#  endif
#endif

#ifdef _GNUC3_
#  include <iostream>
#  include <fstream>
   using namespace std;
#else
#  include <iostream.h>
#  include <fstream.h>
#endif

#include <stdlib.h>
#include <unistd.h>

#include "ATMCD32D.H"

INCLUDEMAIN {
CALLS_HERE
}
' > test.~cpp

cp ATMCD32D.H atmcd32d.~h
sed -n '/EXPNETTYPE unsigned int WINAPI/p' atmcd32d.~h > atmcd32d.~hh 
sed '/CALLS_HERE/ {
  r atmcd32d.~hh 
  d
}' test.~cpp > test.cpp


echo 'LIBRARY atmcd32d.dll
EXPORTS' > atmcd32d.def

dlltool -d atmcd32d.def --dllname atmcd32d.dll --output-lib libandor.a

cp test.cpp test1.cpp
sed 's/EXPNETTYPE unsigned int WINAPI /    /g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/[^\,^\(]*\*[^\,^\)]*/NULL/g' test1.cpp > test1.~cp 
 cp test1.~cp test1.cpp
sed 's/(void)/()/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/unsigned long[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/unsigned int[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/int[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/long[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/double[^,^)]*/0.0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/float[^,^)]*/0.0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/at_u32[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/BYTE[^,^)^(]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/short[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/DWORD[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/at_u64[^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/const//g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/unsigned char [^,^)]*/0/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/HANDLE[^,^)^(]*/NULL/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/HWND[^,^)]*/NULL/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/AT_DDGLiteChannelId[^,^)]*/AT_DDGLite_ChannelA/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/AT_VersionInfoId[^,^)]*/AT_SDKVersion/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/INCLUDEMAIN/int main()/g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp



g++ test1.cpp -o test1.exe -L. -landor 2> err.txt
cp err.txt  err_out.txt
sed -n '/.*__imp.*/p' err.txt > err.~tx
cp err.~tx err.txt
sed 's/.*__imp_//g' err.txt > err.~tx
cp err.~tx err.txt
sed 's/.*exit status.*//g' err.txt > err.~tx
cp err.~tx err.txt
sed 's/'\''//g' err.txt > err.~tx
cp err.~tx err.txt
sed 's/.*undefined reference.*//g' err.txt > err.~tx
cp err.~tx err.txt
sed 's/.*undefined reference.*//g' err.txt > err.~tx
cp err.~tx err.txt

sed '$r err.txt' <atmcd32d.def >atmcd32d.~def
cp atmcd32d.~def atmcd32d.def
dlltool -d atmcd32d.def --dllname atmcd32d.dll --output-lib libandor.a -k
#rm -f test.~cpp test1.cpp err.~tx  err.txt test1.exe atmcd32d.~h atmcd32d.~hh



