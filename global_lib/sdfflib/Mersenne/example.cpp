// example.cpp
// Examples of random number generation with MersenneTwister.h
// Richard J. Wagner  15 May 2003

#include <iostream>
#include <fstream>
#include "MersenneTwister.h"

using namespace std;

int main(void)
{
	// A Mersenne Twister random number generator can be
	// declared with a simple
	
	MTRand mtrand1;
	
	// and used with
	
	double a = mtrand1();
	
	// or
	
	double b = mtrand1.rand();
	
	cout << "Two real numbers in the range [0,1]:  "
	     << a << ", " << b << endl;
	
	// Those calls produced the default of floating-point
	// numbers in the range 0 to 1 inclusive.  We can also
	// get integers in the range 0 to 2^32 - 1 (4294967295)
	// with
	
	unsigned long c = mtrand1.randInt();

	cout << "An integer in the range [0,"
	     << 0xffffffffUL << "]:  "
	     << c << endl;
	
	// Or get an integer in the range 0 to n (for n < 2^32)
	// with
	
	int d = mtrand1.randInt( 42 );
	
	cout << "An integer in the range [0,42]:  "
	     << d << endl;
	
	// We can get a real number in the range 0 to 1
	// exclusive of 1 with
	
	double e = mtrand1.randExc();
	
	cout << "A real number in the range [0,1):  "
	     << e << endl;
	
	// The functions rand() and randExc() can also have
	// ranges defined just like randInt()
	
	double f = mtrand1.rand( 2.5 );
	double g = mtrand1.randExc( 10.0 );
	
	cout << "A real numer in the range [0,2.5]:  "
	     << f << endl;
	cout << "And one in the range [0,10.0):  "
	     << g << endl;
	
	// Random number generators need a seed value to start
	// producing a sequence of random numbers.  We gave no
	// seed in our declaration of mtrand1, so one was
	// automatically generated from the system clock (or the
	// operating system's random number pool if available).
	// Alternatively we could provide our own seed. Each
	// seed uniquely determines the sequence of numbers that
	// will be produced.  We can replicate a sequence by
	// starting another generator with the same seed.
	
	MTRand mtrand2a( 1973 );  // makes new MTRand with given seed
	
	double h1 = mtrand2a();   // gets the first number generated
	
	MTRand mtrand2b( 1973 );  // makes an identical MTRand
	
	double h2 = mtrand2b();   // and gets the same number
	
	cout << "These two numbers are the same:  "
	     << h1 << ", " << h2 << endl;
	
	// We can also restart an existing MTRand with a new
	// seed
	
	mtrand2a.seed( 1776 );
	mtrand2b.seed( 1941 );
	
	double i1 = mtrand2a();
	double i2 = mtrand2b();
	
	cout << "Re-seeding gives different numbers:  "
	     << i1 << ", " << i2 << endl;
	
	// But there are only 2^32 possible seeds when we pass a
	// single 32-bit integer.  Since the seed dictates the
	// sequence, only 2^32 different random number sequences
	// will result.  For applications like Monte Carlo
	// simulation we might want many more.  We can seed with
	// an array of values rather than a single integer to
	// access the full 2^19937-1 possible sequences.
	
	MTRand::uint32 seed[ MTRand::N ];
	for( int s = 0; s < MTRand::N; ++s )
		seed[s] = 23 * s;  // fill with anything
	MTRand mtrand3( seed );
	
	double j1 = mtrand3();
	double j2 = mtrand3();
	double j3 = mtrand3();
	
	cout << "We seeded this sequence with 19968 bits:  "
	     << j1 << ", " << j2 << ", " << j3 << endl;
	
	// Again we will have the same sequence every time we
	// run the program.  Make the array with something that
	// will change to get unique sequences.  On a Linux
	// system, the default auto-initialization routine takes
	// a unique sequence from /dev/urandom.
	
	// For cryptography, also remember to hash the generated
	// random numbers.  Otherwise the internal state of
	// the generator can be learned after reading 624
	// values.
	
	// We might want to save the state of the generator at
	// an arbitrary point after seeding so a sequence
	// could be replicated.  An MTRand object can be saved
	// into an array or to a stream.  
	
	MTRand mtrand4;
	
	// The array must be of type uint32 and length SAVE.
	
	MTRand::uint32 randState[ MTRand::SAVE ];
	
	mtrand4.save( randState );
	
	// A stream is convenient for saving to a file.
	
	ofstream stateOut( "state.data" );
	if( stateOut )
	{
		stateOut << mtrand4;
		stateOut.close();
	}
	
	unsigned long k1 = mtrand4.randInt();
	unsigned long k2 = mtrand4.randInt();
	unsigned long k3 = mtrand4.randInt();
	
	cout << "A random sequence:       "
	     << k1 << ", " << k2 << ", " << k3 << endl;
	
	// And loading the saved state is as simple as
	
	mtrand4.load( randState );
	
	unsigned long k4 = mtrand4.randInt();
	unsigned long k5 = mtrand4.randInt();
	unsigned long k6 = mtrand4.randInt();
	
	cout << "Restored from an array:  "
	     << k4 << ", " << k5 << ", " << k6 << endl;
	
	ifstream stateIn( "state.data" );
	if( stateIn )
	{
		stateIn >> mtrand4;
		stateIn.close();
	}
	
	unsigned long k7 = mtrand4.randInt();
	unsigned long k8 = mtrand4.randInt();
	unsigned long k9 = mtrand4.randInt();
	
	cout << "Restored from a stream:  "
	     << k7 << ", " << k8 << ", " << k9 << endl;
	
	// In summary, the recommended common usage is
	
	MTRand mtrand5;  // automatically generate seed
	double l = mtrand5();               // real number in [0,1]
	double m = mtrand5.randExc(0.5);    // real number in [0,0.5)
	unsigned long n = mtrand5.randInt(10);  // integer in [0,10]
	
	// with the << and >> operators used for saving to and
	// loading from streams if needed.
	
	cout << "Your lucky number for today is "
	     << l + m * n << endl;
	
	return 0;
}

