#ifndef __PRNG_H__
#define __PRNG_H__

#ifndef __linux__ 
#error This class can be used with linux only
#endif


#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<time.h>

class prng
{
	protected:
	public:
                void fill_random(void *p,int size)
		{
			FILE *urandom;
			int result;
			
			if(urandom = fopen("/dev/urandom", "r")) 
			{
				result=fread(p,size, 1, urandom);
				fclose(urandom);
			}
			else 
			{
			std::cerr << "ERROR [" << __FILE__ << "," << __FUNCTION__ << "," << __LINE__ << "]: Unable to open /dev/urandom" << std::endl;
			}
		}
		
		virtual ~prng(void){};
		/** \brief returns random number [0..get_max()] */
		virtual unsigned int get_rand(){return 0;};
		/** \brief returns random number [0..1) */
		virtual float get_rand_f(){return 0.0;};
		/** \brief return the maximum random value to be returned */
		virtual unsigned int get_max(){return 0;};
		
		
};

class prng_simple: public prng
{
	public:
		prng_simple(){srand(time(NULL));}
		~prng_simple(){};
		virtual inline unsigned int get_rand() {return rand();}
		virtual inline float get_rand_f() {return (float) ((double)rand()/((double)(RAND_MAX)+1.0));}
		virtual inline unsigned int get_max() {return RAND_MAX;}
};

class prng_mersenne_twister: public prng
{

	protected:

			unsigned *y;
			int index;

	public:
		virtual unsigned int get_rand()
		{
			const int N = 624;
			const int M = 397;
		
			const unsigned A[2] = { 0, 0x9908b0df };
			const unsigned HI = 0x80000000;
			const unsigned LO = 0x7fffffff;

			if (index >= N) {
				unsigned h;
				for (int k=0 ; k<N-M ; ++k) {
					h = (y[k] & HI) | (y[k+1] & LO);
					y[k] = y[k+M] ^ (h >> 1) ^ A[h & 1];
				}
				for (int k=N-M ; k<N-1 ; ++k) {
					h = (y[k] & HI) | (y[k+1] & LO);
					y[k] = y[k+(M-N)] ^ (h >> 1) ^ A[h & 1];
				}
				h = (y[N-1] & HI) | (y[0] & LO);
				y[N-1] = y[M-1] ^ (h >> 1) ^ A[h & 1];
				index = 0;
			}

			unsigned e = y[index++];

			e ^= (e >> 11);
			e ^= (e << 7) & 0x9d2c5680;
			e ^= (e << 15) & 0xefc60000;
			e ^= (e >> 18);
			return e;
		}
		
		prng_mersenne_twister(void)
		{
			const int N = 624;
			y=(unsigned *) malloc(sizeof(unsigned)*N);
			if(y==NULL)
			{
				std::cerr << "ERROR [" << __FILE__ << "," << __FUNCTION__ << "," << __LINE__ << "]: Failed to allocate memory!" << std::endl;
			}
			else
			{
				fill_random((void*)y,sizeof(unsigned)*N);
				index=N+1;
			}
		}
		
		~prng_mersenne_twister()
		{
			free(y);
		}
		
		virtual unsigned int get_max() {return 0xffffffff;}
};

class prng_TT800: public prng
{

	protected:

			unsigned *y;
			int index;

	public:
		virtual unsigned int get_rand()
		{
			const int N = 25;
			const int M = 7;
			const unsigned A[2] = { 0, 0x8ebfd028 };

			if (index >= N) {
				int k; 
				for (k=0 ; k<N-M ; ++k)
					y[k] = y[k+M] ^ (y[k] >> 1) ^ A[y[k] & 1];
				for (; k<N ; ++k)
					y[k] = y[k+(M-N)] ^ (y[k] >> 1) ^ A[y[k] & 1];
				index = 0;
			}

			unsigned e = y[index++];
			e ^= (e << 7) & 0x2b5b2500;
			e ^= (e << 15) & 0xdb8b0000;
			e ^= (e >> 16);
			return e;
		}
		
		prng_TT800(void)
		{
			const int N = 25;
			y=(unsigned *) malloc(sizeof(unsigned)*N);
			if(y==NULL)
			{
				std::cerr << "ERROR [" << __FILE__ << "," << __FUNCTION__ << "," << __LINE__ << "]: Failed to allocate memory!" << std::endl;
			}
			else
			{
				fill_random((void*)y,sizeof(unsigned)*N);
				index=N+1;
			}
		}
		
		~prng_TT800()
		{
			free(y);
		}
		
		virtual unsigned int get_max() {return 0xffffffff;}
};


#endif
