// testHinsch.cpp
// Test output and speed of mtrand.h
// Richard J. Wagner  15 May 2003

// This is free, unrestricted software void of any warranty.

#include <stdio.h>
#include <time.h>
#include "mtrand.h"

int main(void)
{
	printf( "Testing output and speed of mtrand.h\n" );
	printf( "\nTest of random integer generation:\n" );
	MTRand mtrand1;
	long i;
	for( i = 0; i < 1000; ++i )
	{
		printf( "%10lu ", mtrand1() );
		if( i % 5 == 4 ) printf("\n");
	}
	
	printf( "\nTest of time to generate 300 million random integers:\n" );
	MTRand mtrand4;
	unsigned long junk;
	clock_t start = clock();
	for( i = 0; i < 300000000; ++i )
	{
		junk = mtrand4();
	}
	clock_t stop = clock();
	printf( "Time elapsed = " );
	printf( "%8.3f", double( stop - start ) / CLOCKS_PER_SEC );
	printf( " s\n" );
	
	return 0;
}

