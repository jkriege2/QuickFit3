/*
 *	based on producer/consumer example by Andrae Muys
 *	http://www.cs.nmsu.edu/~jcook/Tools/pthreads/examples.html
 */

/*
 *	This is the Qt compliant version of the fifo class
 */

#ifndef __QFIFO_H__
#define __QFIFO_H__

#include <QWaitCondition>
#include <QMutex>

#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

#include <iostream>

class qfifo
{
	private:
		int *data;/**internal data*/
		unsigned int size;/**amount of internal data*/
		char * name;/**name for debugging */
		unsigned int depth,depth_max;/**statistics*/
		
		unsigned int head, tail;/**ring-buffer*/
		int full, empty;
		QMutex *mut;
		QWaitCondition *notFull;
		QWaitCondition *notEmpty;
		QWaitCondition *spaceAvailable;
	public:

		qfifo(unsigned int size,char * name)
		{
			this->data=(int*)malloc(size*sizeof(int));
			this->size=size;
			this->name=name;
			
			this->empty = 1;
			this->full = 0;
			this->head = 0;
			this->tail = 0;
			this->depth=0;
			this->depth_max=0;
			
			this->mut=new QMutex;
			this->notFull=new QWaitCondition;
			this->notEmpty=new QWaitCondition;
			
		}

		~qfifo()
		{
			free(this->data);
		}

		int push(int value, bool *breakCondition)
		{
			this->mut->lock();
			while (this->full) 
			{
//		#ifdef DEBUG_QFIFO_H
				printf ("(%s) %s: FIFO FULL [%i]\n",this->name,__FUNCTION__,value);
//		#endif
				if(*breakCondition)
				{
					this->mut->unlock();
					return(-1);
				}
				this->notFull->wait(this->mut);
			}
			//printf ("(%s) %s: PUSHING [%i]\n",this->name,__FUNCTION__,value);
			//std::cerr << "[" << this << "] " << head << "," << tail << "," << full << "," << empty << "," << depth << std::endl;
			this->data[this->tail] = value;
			this->tail++;
			if (this->tail == this->size)
				this->tail = 0;
			if (this->tail == this->head)
				this->full = 1;
			this->empty = 0;

			/** size calculations */
			this->depth = (tail>=head) ? this->tail-this->head : this->size-this->head+this->tail;
			if(this->full==1)this->depth=this->size;
			if(this->empty==1)this->depth=0;
			
			if(this->depth>this->depth_max)this->depth_max=this->depth;
			this->mut->unlock();
			this->notEmpty->wakeAll();

			return 0;
		}
		
		inline int push(int value){
			bool breakCondition=false;
			return push(value,&breakCondition);
		}
		
		/**
		* \brief  Get next value from FIFO (blocking)
		*
		* This function waits until a value is available and return it.
		*
		*
		* \param	breakCondition	pointer to a bool variable, when true it indicates to return immediatley, use wakeAll.
		* \return 								value from FIFO, -1 on *breakCondition=true
		*
		*/
		
		int pop(bool *breakCondition, unsigned int minimal_free_count=1)
		{
			this->mut->lock();
			while ((this->empty)&&(this->depth<minimal_free_count))/**< we wait for beeing not empty, but also for a minimal count of available blocks */
			{
		#ifdef DEBUG_QFIFO_H
			printf ("(%s) %s: FIFO EMPTY.\n",this->name,__FUNCTION__);
		#endif
				if(*breakCondition)
				{
					this->mut->unlock();
					return(-1);
				}
				this->notEmpty->wait(this->mut);
			}
			
			int value = this->data[this->head];
			//printf ("(%s) %s: POPPING [%i]\n",this->name,__FUNCTION__,value);
			//std::cerr << "[" << this << "] " << head << "," << tail << "," << full << "," << empty << "," << depth << std::endl;

			this->head++;
			if (this->head == this->size)
				this->head = 0;
			if (this->head == this->tail)
				this->empty = 1;
			this->full = 0;

			/** size calculations */
			this->depth = (tail>=head) ? this->tail-this->head : this->size-this->head+this->tail;
			if(this->full==1)this->depth=this->size;
			if(this->empty==1)this->depth=0;
			
			this->mut->unlock();
			this->notFull->wakeAll();
			
			return value;
		}
		
		inline int pop(unsigned int minimal_free_count=1){
			bool breakCondition=false;
			return pop(&breakCondition,minimal_free_count);
		}

		bool pop_nb(int * value, unsigned int minimal_free_count=1)
		{
			bool success=false;
			
			this->mut->lock();
			if((this->empty)||(this->depth < minimal_free_count))/**< just return if out creteria are not met */
			{
		#ifdef DEBUG_QFIFO_H
			printf ("(%s) %s: FIFO EMPTY. RETURNING NON BLOCKING\n",this->name,__FUNCTION__);
		#endif
			}
			else
			{
				success=true;
				*value = this->data[this->head];

				this->head++;
				if (this->head == this->size)
					this->head = 0;
				if (this->head == this->tail)
					this->empty = 1;
				this->full = 0;

				/** size calculations */
				this->depth = (tail>=head) ? (this->tail - this->head) : (this->size - this->head + this->tail);
				if(this->full==1)this->depth=this->size;
				if(this->empty==1)this->depth=0;
			}
			this->mut->unlock();
			this->notFull->wakeAll();

			return success;
		}

		GET(depth_max)
		GET(depth)
		GET(size)
		
		void resize(unsigned int size)
		{
			this->data=(int*)realloc(this->data,size*sizeof(int));
			this->size=size;
		}
		
		void clear()
		{
			while(this->empty==0)
				pop();
		}
		
		void wakeAll(){
			this->notEmpty->wakeAll();
			this->notFull->wakeAll();
		}

	};
#endif
