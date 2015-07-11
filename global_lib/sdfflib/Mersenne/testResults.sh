#!/bin/sh

# testResults.sh
# Examine the output from test of random number generators
# Explain the meaning in plain English

grep -A 200 "integer generation:" testWagner.out | tail -200 > tmpWagnerInt.out
grep -A 200 -F "[0,1) generation:" testWagner.out | tail -200 > tmpWagnerExc.out
grep -A 200 -F "[0,1] generation:" testWagner.out | tail -200 > tmpWagnerInc.out
grep -A 200 "integer generation:" testOrig.out | tail -200 > tmpOrigInt.out
grep -A 200 -F "[0,1) generation:" testOrig.out | tail -200 > tmpOrigExc.out
grep -A 200 -F "[0,1] generation:" testOrig.out | tail -200 > tmpOrigInc.out
grep -A 200 "genrand_int32()" mt19937ar.out | tail -200 > tmpRefInt.out
grep -A 200 -F "genrand_real2()" mt19937ar.out | tail -200 > tmpRefExc.out

rm -f tmpDiffLocal.out tmpDiffRef.out tmpError.out

diff --brief tmpWagnerInt.out tmpOrigInt.out | tee -a tmpDiffLocal.out
diff --brief tmpWagnerExc.out tmpOrigExc.out | tee -a tmpDiffLocal.out
diff --brief tmpWagnerInc.out tmpOrigInc.out | tee -a tmpDiffLocal.out

diff -b --brief tmpWagnerInt.out tmpRefInt.out | tee -a tmpDiffRef.out
diff -b --brief tmpWagnerExc.out tmpRefExc.out | tee -a tmpDiffRef.out

grep "Error" testWagner.out | tee tmpError.out

echo
echo "Rate of Mersenne Twister random integer generation:"
printf "    MersenneTwister.h (this C++ class) "
gawk '/Time elapsed/{ printf( "%4.1f million per second\n", 300 / $4 ) }' testWagner.out
printf "    Inventors' revised C version       "
gawk '/Time elapsed/{ printf( "%4.1f million per second\n", 300 / $4 ) }' testOrig.out
printf "    Cokus's optimized C version        "
gawk '/Time elapsed/{ printf( "%4.1f million per second\n", 300 / $4 ) }' testCokus.out
printf "    Hinsch's C++ class                 "
gawk '/Time elapsed/{ printf( "%4.1f million per second (non-standard seeding)\n", 300 / $4 ) }' testHinsch.out
printf "    Standard rand()                    "
gawk '/Time elapsed/{ printf( "%4.1f million per second (non-MT)\n", 300 / $4 ) }' testStd.out
echo

failLocal=false
failRef=false
if [ -s tmpDiffLocal.out ]; then
	failLocal=true
fi
if [ -s tmpDiffRef.out ] || [ -s tmpError.out ]; then
	failRef=true
fi
if [ $failLocal = "true" ] || [ $failRef = "true" ]; then
	if [ $failRef = "true" ]; then
		cat testWagner.out testOrig.out > bug.out
		echo "Failed tests - MersenneTwister.h generated incorrect output"
		echo "Results have been written to a file called 'bug.out'"
		echo "Please send a copy of 'bug.out' and system info to rjwagner@writeme.com"
	else
		echo "Inventors' revised C version failed"
		echo "MersenneTwister.h passed and is safe to use"
	fi
else
	echo "Passed all tests"
fi

exit 0
