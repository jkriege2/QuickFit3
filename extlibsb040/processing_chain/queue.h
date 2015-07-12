/*
 *	based on producer/consumer example by Andrae Muys
 *	http://www.cs.nmsu.edu/~jcook/Tools/pthreads/examples.html
 */

#ifndef __QUEUE__
#define __QUEUE__

#include "fifo.h"
#include "helper.h"

template <class T>
class dqueue
{
	private:
		unsigned int entries;
	public:
		T ** buffers;
		fifo *free,*full;
		
		dqueue(unsigned int entries)
		{
			full=new fifo(entries,(char*)"full");//fifo for full blocks
			free=new fifo(entries,(char*)"free");//fifo for free blocks
			
			this->entries=entries;
			
			buffers=(T**)malloc(sizeof(T*)*entries);
			for(unsigned int i=0;i<entries;i++)
			{
				buffers[i] =(T*) new T;
				free->push(i);
			}
		}
		
		~dqueue()
		{
			for(unsigned int i=0;i<entries;i++)
			{
				delete buffers[i];
			}
		}
		
		GET(entries)
		
		
};
#endif /*__QUEUE__*/