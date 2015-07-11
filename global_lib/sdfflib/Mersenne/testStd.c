/* testStd.c                                  */
/* Test speed of the standard rand() function */
/* Richard J. Wagner  13 May 2003             */

/* This is free, unrestricted software void of any warranty. */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

long i;
unsigned long junk;
clock_t start;
clock_t stop;

int main(void)
{
	printf( "Testing speed of standard rand() function\n" );
	printf( "\nTest of time to generate 300 million random integers:\n" );
	srand( 4357U );
	start = clock();
	for( i = 0; i < 300000000; ++i )
	{
		junk = rand();
	}
	stop = clock();
	printf( "Time elapsed = " );
	printf( "%8.3f", (double)( stop - start ) / CLOCKS_PER_SEC );
	printf( " s\n" );
	
	return 0;
}

