/* testCokus.c                              */
/* Test output and speed of mt19937ar-cok.c */
/* Richard J. Wagner  13 May 2003           */

/* This is free, unrestricted software void of any warranty. */

#include <stdio.h>
#include <time.h>
#include "mt19937ar-cok.c"

long i;
unsigned long junk;
clock_t start;
clock_t stop;

int main(void)
{
	unsigned long oneSeed = 4357UL;
	unsigned long bigSeed[4] = { 0x123, 0x234, 0x345, 0x456 };
	
	printf( "Testing output and speed of mt19937ar-cok.c\n" );
	printf( "\nTest of random integer generation:\n" );
	init_by_array( bigSeed, 4 );
	for( i = 0; i < 1000; ++i )
	{
		printf( "%10lu ", genrand_int32() );
		if( i % 5 == 4 ) printf("\n");
	}
	
	printf( "\nTest of random real number [0,1) generation:\n" );
	for( i = 0; i < 1000; ++i )
	{
		printf( "%10.8f ", genrand_real2() );
		if( i % 5 == 4 ) printf("\n");
	}
	
	printf( "\nTest of random real number [0,1] generation:\n" );
	init_genrand( oneSeed );
	for( i = 0; i < 1000; ++i )
	{
		printf( "%10.8f ", genrand_real1() );
		if( i % 5 == 4 ) printf("\n");
	}
	
	
	printf( "\nTest of time to generate 300 million random integers:\n" );
	init_genrand( oneSeed );
	start = clock();
	for( i = 0; i < 300000000; ++i )
	{
		junk = genrand_int32();
	}
	stop = clock();
	printf( "Time elapsed = " );
	printf( "%8.3f", (double)( stop - start ) / CLOCKS_PER_SEC );
	printf( " s\n" );
	
	return 0;
}

