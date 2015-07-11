#ifndef __CORRELATOR_BLOCK_H_INCLUDED__
#define __CORRELATOR_BLOCK_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>

#include "correlator_lag.h"

/**
 * Class block, implements one block of a multi-tau correlator.
 * One blocks runs at the same clock.
 */
class block
{
	private:
		lag **lags;
		unsigned int lag_count;
		block *next;
		/**Number of time steps processed*/
		unsigned int ticks;
		/**Id of the current block. Used for printing purpose.*/
		unsigned int id;
	public:
		block(unsigned int,unsigned int);
		~block();
		void run(int,int);
		void print(unsigned int *);
		void set_next(block*);
		int get_lag_count();
		void fill_array_lin(int *array);
		int		get_sum(unsigned int lag_number);
		void fill_array(int **,unsigned int, unsigned int *);
};

/**
 *Block Constructor. Uses dynamic allocation.
 *FIXME: Error handling is far from perfect.
 *@param id Id of the current block within a correlator. Should range from 0..(x-1).
 *@param lag_count Number of lags in this block.
 */
block::block(unsigned int id,unsigned int lag_count)
{
	next=0;
	ticks=0;
	block::lag_count=lag_count;
	lags = (lag **)malloc(lag_count * sizeof(lag*));
	if (lags==0)
		printf("Error!");
	for(unsigned int x=0;x<lag_count;x++)
		lags[x]=new lag(x,this);
	for(unsigned int x=0;x<(lag_count-1);x++)
		lags[x]->set_next(lags[x+1]);

	block::id=id;
}

/**
 *Class destructor.
 */
block::~block () {
	for(unsigned int x=0;x<lag_count;x++)
		delete lags[x];
}

/**
 * Run the correlation within this block for one time step.
 * @param value_global Global value, used for all blocks and lags simultaniousely
 * @param value_local Local value, only used for first block.
 */
void block::run(int value_global,int value_local)
{
	if(lag_count>0)
		lags[0]->run(value_global,value_local);
	if((ticks%2)==0)
		if(next!=0)
			next->run(value_global,lags[lag_count-1]->acc);
	ticks++;
}

/**
 * Print the sums of all lags. 
 * TODO: Switch to cout streaming and operator<< overloading
 * @param pos Starting point for printing. Contains the next free position afterwards.
 */
void block::print(unsigned int *pos)
{
	for(unsigned int x=0;x<lag_count;x++)
	{
		for(int y=0;y<(1 << id);y++)
			lags[x]->print((*pos)++);
	}
}

/**
 *Set the next block in chain. Used for creating linked lists.
 *@param next Next block in chain.
 */
void block::set_next (block *next)
{
	block::next=next;
}

int block::get_lag_count()
{
	return lag_count;
}

void block::fill_array_lin(int *array)
{
	for(int x=0;x<lag_count;x++)
		for(int y=0;y<(1<<id);y++)
			array[x*(1<<id)+y]=lags[x]->get_sum();
}

int block::get_sum(unsigned int lag_number)
{
	if((lag_count>0)&&(lag_number<lag_count))
		return lags[lag_number]->sum;
}

void block::fill_array(int **array,unsigned int pos, unsigned int *tau)
{
	for(unsigned int x=0;x<lag_count;x++)
	{
		array[0][pos+x]=*tau;
		array[1][pos+x]=lags[x]->get_sum();
		*tau+=(1<<id);
	}
}

#endif
