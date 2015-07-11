all: example testWagner
	@ echo "Use 'make test' to test output and speed of MersenneTwister.h"
	@ echo "Or run the example program with './example'"

example: example.cpp MersenneTwister.h
	g++ -Wall -ansi -o example example.cpp

testWagner: testWagner.cpp MersenneTwister.h
	g++ -O3 -Wall -ansi -o testWagner testWagner.cpp

testOrig: testOrig.c mt19937ar.c
	gcc -O3 -o testOrig testOrig.c

testCokus: testCokus.c mt19937ar-cok.c
	gcc -O3 -o testCokus testCokus.c

testHinsch: testHinsch.cpp mtrand.h mtrand.cc
	g++ -O3 -o testHinsch testHinsch.cpp mtrand.cc

testStd: testStd.c
	gcc -O3 -o testStd testStd.c

test: testWagner testOrig testCokus testHinsch testStd
	@ echo "Testing output and speed of random number generators, please be patient..."
	./testWagner > testWagner.out
	./testOrig > testOrig.out
	./testCokus > testCokus.out
	./testHinsch > testHinsch.out
	./testStd > testStd.out
	./testResults.sh
	@ rm -f tmp*

clean:
	@ rm -f test*.out bug.out
	@ rm -f example testWagner testOrig testCokus testHinsch testStd
	@ rm -f tmp*
	@ rm -f state.data
	@ rm -f core
