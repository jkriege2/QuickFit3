// testWagner.cpp
// Test output and speed of MersenneTwister.h
// Richard J. Wagner  15 May 2003

// This is free, unrestricted software void of any warranty.

#include <stdio.h>
#include <time.h>
#include <fstream>
#include "MersenneTwister.h"

void showrate( clock_t start, clock_t stop, int reps );

int main(void)
{
	printf( "Testing output and speed of MersenneTwister.h\n" );
	
	MTRand::uint32 oneSeed = 4357UL;
	MTRand::uint32 bigSeed[4] = { 0x123, 0x234, 0x345, 0x456 };
	
	printf( "\nTest of random integer generation:\n" );
	MTRand mtrand1( bigSeed, 4 );
	long i;
	for( i = 0; i < 1000; ++i )
	{
		printf( "%10lu ", mtrand1.randInt() );
		if( i % 5 == 4 ) printf("\n");
	}
	
	printf( "\nTest of random real number [0,1) generation:\n" );
	for( i = 0; i < 1000; ++i )
	{
		printf( "%10.8f ", mtrand1.randExc() );
		if( i % 5 == 4 ) printf("\n");
	}
	
	printf( "\nTest of random real number [0,1] generation:\n" );
	MTRand mtrand2( oneSeed );
	for( i = 0; i < 1000; ++i )
	{
		printf( "%10.8f ", mtrand2.rand() );
		if( i % 5 == 4 ) printf("\n");
	}
	
	
	printf( "\nTest of time to generate 300 million random integers:\n" );
	MTRand mtrand3( oneSeed );
	unsigned long junk;
	clock_t start = clock();
	for( i = 0; i < 300000000; ++i )
	{
		junk = mtrand3.randInt();
	}
	clock_t stop = clock();
	printf( "Time elapsed = " );
	printf( "%8.3f", double( stop - start ) / CLOCKS_PER_SEC );
	printf( " s\n" );
	
	printf( "\nTest of generation rates in various distributions:\n" );
	{
		printf( "  Integers in [0,2^32-1]      " );
		start = clock();
		for( i = 0; i < 50000000; ++i ) mtrand3.randInt();
		stop = clock();
		showrate(start,stop,50);
		
		printf( "  Integers in [0,100]         " );
		start = clock();
		for( i = 0; i < 50000000; ++i ) mtrand3.randInt(100);
		stop = clock();
		showrate(start,stop,50);
		
		printf( "  Reals in [0,1]              " );
		start = clock();
		for( i = 0; i < 50000000; ++i ) mtrand3.rand();
		stop = clock();
		showrate(start,stop,50);
		
		printf( "  Reals in [0,7]              " );
		start = clock();
		for( i = 0; i < 50000000; ++i ) mtrand3.rand(7.0);
		stop = clock();
		showrate(start,stop,50);
		
		printf( "  Reals in normal distribution" );
		start = clock();
		for( i = 0; i < 10000000; ++i ) mtrand3.randNorm(7.0,2.0);
		stop = clock();
		showrate(start,stop,10);
	}
	
	
	printf( "\nTests of functionality:\n" );
	
	// Array save/load test
	bool saveArrayFailure = false;
	MTRand mtrand4;
	unsigned long pass1[5], pass2[5];
	MTRand::uint32 saveArray[ MTRand::SAVE ];
	mtrand4.save( saveArray );
	for( i = 0; i < 5; ++i )
		pass1[i] = mtrand4.randInt();
	mtrand4.load( saveArray );
	for( i = 0; i < 5; ++i )
	{
		pass2[i] = mtrand4.randInt();
		if( pass2[i] != pass1[i] )
			saveArrayFailure = true;
	}
	if( saveArrayFailure )
		printf( "Error - Failed array save/load test\n" );
	else
		printf( "Passed array save/load test\n" );
	
	
	// Stream save/load test
	bool saveStreamFailure = false;
	std::ofstream dataOut( "state.data" );
	if( dataOut )
	{
		dataOut << mtrand4;  // comment out if compiler does not support
		dataOut.close();
	} 
	for( i = 0; i < 5; ++i )
		pass1[i] = mtrand4.randInt();
	std::ifstream dataIn( "state.data" );
	if( dataIn )
	{
		dataIn >> mtrand4;  // comment out if compiler does not support
		dataIn.close();
	}
	for( i = 0; i < 5; ++i )
	{
		pass2[i] = mtrand4.randInt();
		if( pass2[i] != pass1[i] )
			saveStreamFailure = true;
	}
	if( saveStreamFailure )
		printf( "Error - Failed stream save/load test\n" );
	else
		printf( "Passed stream save/load test\n" );
	
	
	// Integer range test
	MTRand mtrand5;
	bool integerRangeFailure = false;
	bool gotMax = false;
	for( i = 0; i < 10000; ++i )
	{
		int r = mtrand5.randInt(17);
		if( r < 0 || r > 17 )
			integerRangeFailure = true;
		if( r == 17 )
			gotMax = true;
	}
	if( !gotMax )
		integerRangeFailure = true;
	if( integerRangeFailure )
		printf( "Error - Failed integer range test\n" );
	else
		printf( "Passed integer range test\n" );
	
	
	// Float range test
	MTRand mtrand6;
	bool floatRangeFailure = false;
	for( i = 0; i < 10000; ++i )
	{
		float r = mtrand6.rand(0.3183);
		if( r < 0.0 || r > 0.3183 )
			floatRangeFailure = true;
	}
	if( floatRangeFailure )
		printf( "Error - Failed float range test\n" );
	else
		printf( "Passed float range test\n" );
	
	
	// Gaussian distribution test
	MTRand mtrand7;
	bool gaussianFailure = false;
	for( i = 0; i < 10000; ++i )
	{
		const double mean = 100.0;
		const double variance = 8.0;
		float r = mtrand7.randNorm( mean, variance );
		if( r < mean - 12.0 * variance || r > mean + 12.0 * variance )
			gaussianFailure = true;
	}
	if( gaussianFailure )
		printf( "Error - Failed Gaussian distribution test\n" );
	else
		printf( "Passed Gaussian distribution test\n" );
	
	
	// Auto-seed uniqueness test
	MTRand mtrand8a, mtrand8b, mtrand8c;
	double r8a = mtrand8a();
	double r8b = mtrand8b();
	double r8c = mtrand8c();
	if( r8a == r8b || r8a == r8c || r8b == r8c )
		printf( "Error - Failed auto-seed uniqueness test\n" );
	else
		printf( "Passed auto-seed uniqueness test\n" );
	
	return 0;
}

void showrate( clock_t start, clock_t stop, int reps )
{
	double time = double( stop - start ) / CLOCKS_PER_SEC;
	double rate = reps / time;
	printf( " %6.2f million per second\n", rate );
}
