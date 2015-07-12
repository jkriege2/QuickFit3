/*
 *	based on producer/consumer example by Andrae Muys
 *	http://www.cs.nmsu.edu/~jcook/Tools/pthreads/examples.html
 */

#ifndef __QQUEUE_H__
#define __QQUEUE_H__

#include "qfifo.h"

template <class T>
class qqueue
{
	private:
		unsigned int entries;
	public:
		T ** buffers;
		qfifo *free,*full;
		
		qqueue(unsigned int entries)
		{
			full=new qfifo(entries,(char*)"full");//fifo for full blocks
			free=new qfifo(entries,(char*)"free");//fifo for free blocks
			
			this->entries=entries;
			
			buffers=(T**)malloc(sizeof(T*)*entries);
			for(unsigned int i=0;i<entries;i++)
			{
				buffers[i] =(T*) new T;
				free->push(i);
			}
		}
		
		~qqueue()
		{
			for(unsigned int i=0;i<entries;i++)
			{
				delete buffers[i];
			}
		}
		
};
#endif /*__QUEUE__*/