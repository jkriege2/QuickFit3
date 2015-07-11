/*
 *	based on producer/consumer example by Andrae Muys
 *	http://www.cs.nmsu.edu/~jcook/Tools/pthreads/examples.html
 */

#ifndef __FIFO__
#define __FIFO__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

class fifo
{
	private:
		int *data;/**internal data*/
		unsigned int size;/**amount of internal data*/
		char * name;
		unsigned int depth,depth_max;
		
		unsigned int head, tail;
		int full, empty;
		pthread_mutex_t *mut;
		pthread_cond_t *notFull, *notEmpty;
		
	public:
		fifo(unsigned int,char*);
		~fifo();
		int push(int);
		int pop();
		bool pop_nb(int *);
		unsigned int get_depth_max();
		unsigned int get_depth();
};
#endif