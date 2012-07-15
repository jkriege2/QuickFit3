#!/bin/sh
echo -e "This script will read NIDAQmx.H and from this create a call to every function, declared in this header\n"\
        "in a file test.cpp. This one is then used to create a def file, also containing the @nn part of the dll\n"\
        "functions. calling dlltool finally working libNIDAQmx.a is created!\n\n"

cp NIDAQmx.h ni.h
echo -e "\033[0;33m1. creating necessary files \033[0m\n"
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

#include "ni.h"

INCLUDEMAIN {
CALLS_HERE
}
' > test.~cpp

cp ni.h ni.~h
sed -n '/int32 __CFUNC/p' ni.~h > ni.~hh 
sed '/CALLS_HERE/ {
  r ni.~hh 
  d
}' test.~cpp > test1.cpp


echo 'LIBRARY nicaiu.dll
EXPORTS' > ni.def

echo -e "\033[0;33m2. running DLLTOOL\033[0m\n"
dlltool -d ni.def --dllname nicaiu.dll --output-lib libNIDAQmx_first.a

sed 's/int32 __CFUNC_C /    /g' test1.cpp > test1.~cp
cp test1.~cp test1.cpp
sed 's/int32 __CFUNC /    /g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/char[^,^)^\[]*\[\]/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/TaskHandle[^,^)]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/CalHandle calHandle/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/CalHandle \*calHandle/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/CalHandle[^,^)]/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/DAQmxEveryNSamplesEventCallbackPtr[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/DAQmxDoneEventCallbackPtr[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/DAQmxSignalEventCallbackPtr[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/, \.\.\./, 0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp


sed 's/unsigned char[\s]*\*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/char \*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/char \*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/void \*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/float32 \*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/float64 \*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/float32\*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/float64\*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp

sed 's/char [^,^)^\[]*\[\]/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/void [^,^)^\[]*\[\]/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/float32 [^,^)^\[]*\[\]/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/float64 [^,^)^\[]*\[\]/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp

sed 's/float32[^,^)]*/0.0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/float64[^,^)]*/0.0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/[\w\s^,^)]*\*[^,^)]*/NULL/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp

sed 's/int8[^,^)]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/uInt8[^,^)^(]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/int16[^,^)]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/uInt16[^,^)^(]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/int32[^,^)]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/uInt32[^,^)^(]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/int64[^,^)]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/uInt64[^,^)^(]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp
sed 's/bool32[^,^)^(]*/0/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp

sed 's/const//g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp

sed 's/INCLUDEMAIN/int main()/g' test1.cpp > test1.~cp 
cp test1.~cp test1.cpp

echo -e "\033[0;33m3. running GCC \033[0m\n"
g++ test1.cpp -o test1.exe -L. -lNIDAQmx_first 2> err.txt
cp err.txt err_backup.txt
echo -e "\033[0;33m4. parsing output \033[0m\n"
sed 's/[^\n]*undefined reference to[^\n]*'\`'//g' err.txt > err.~tx 
cp err.~tx err.txt
sed 's/'\''//g' err.txt > err.~tx 
cp err.~tx err.txt
sed 's/[^\n]*exit status[^\n]*//g' err.txt > err.~tx 
cp err.~tx err.txt

sed '$r err.txt' <ni.def >ni.~def
mv ni.~def ni.def
echo -e "\033[0;33m5. running DLLTOOL again \033[0m\n"
dlltool -d ni.def --dllname nicaiu.dll --output-lib libNIDAQmx.a -k
rm -f test.~cpp test1.cpp test1.~cp  err.~tx  err.txt test1.exe ni.~h ni.~hh ni.h err_backup.txt libNIDAQmx_first.a



