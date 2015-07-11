#include "fifo.h"

fifo::fifo(unsigned int size,char * name)
{
	this->data=(int*)malloc(size*sizeof(int));
	this->size=size;
	this->name=name;
	
	this->empty = 1;
	this->full = 0;
	this->head = 0;
	this->tail = 0;
	this->depth_max=0;
	this->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (this->mut, NULL);
	this->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (this->notFull, NULL);
	this->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (this->notEmpty, NULL);
}

fifo::~fifo()
{
	pthread_mutex_destroy (this->mut);
	free (this->mut);	
	pthread_cond_destroy (this->notFull);
	free (this->notFull);
	pthread_cond_destroy (this->notEmpty);
	free (this->notEmpty);
	
	free(this->data);
}

int fifo::push(int value)
{
	pthread_mutex_lock (this->mut);
	while (this->full) 
	{
#ifdef DEBUG 
		printf ("(%s) %s: FIFO FULL [%i]\n",this->name,__FUNCTION__,value);
#endif
		pthread_cond_wait (this->notFull,this->mut);
	}
	
	this->data[this->tail] = value;
	this->tail++;
	if (this->tail == this->size)
		this->tail = 0;
	if (this->tail == this->head)
		this->full = 1;
	this->empty = 0;
	
	this->depth = (tail>=head) ? this->tail-this->head : this->size-this->head+this->tail;
	if(this->depth>this->depth_max)this->depth_max=this->depth;
	
	pthread_mutex_unlock(this->mut);
	pthread_cond_signal(this->notEmpty);

	return 0;
}

int fifo::pop()
{
	pthread_mutex_lock (this->mut);
	while (this->empty) 
	{
#ifdef DEBUG
	printf ("(%s) %s: FIFO EMPTY.\n",this->name,__FUNCTION__);
#endif
		pthread_cond_wait (this->notEmpty, this->mut);
	}
	
	int value = this->data[this->head];

	this->head++;
	if (this->head == this->size)
		this->head = 0;
	if (this->head == this->tail)
		this->empty = 1;
	this->full = 0;

	pthread_mutex_unlock (this->mut);
	pthread_cond_signal (this->notFull);
	
	return value;
}

bool fifo::pop_nb(int * value)
{
	bool success=false;
	
	pthread_mutex_lock (this->mut);
	if(this->empty)
	{
#ifdef DEBUG
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
	}
	
	pthread_mutex_unlock (this->mut);
	pthread_cond_signal (this->notFull);

	return success;
}

unsigned int fifo::get_depth_max()
{
	return depth_max;
}

unsigned int fifo::get_depth()
{
	return depth;
}
