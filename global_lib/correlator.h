#ifndef __CORRELATOR_H_INCLUDED__
#define __CORRELATOR_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>

#include "correlator_block.h"

/**
 * Correlator class implementing a multi-tau correlator. 
 * This class is NOT intended to be optimized or fast,
 * it tries to build a hardware equivalent of a parallel 
 * multi-tau correlator without beeing parallel. ;-)
 * This is how it should look like (for a (3,2) invocation):
 *
 *
 */
class correlator
{
	private:
		block **blocks;/**Blocks running at different frquencies*/
		unsigned int block_count;/**Number of blocks in current correlator setup*/
		correlator *next;/**One way links list, reference to the next block in chain.*/
	public:
		correlator(unsigned int,unsigned int);
		~correlator();
		void run(int,int);
		void print(unsigned int*);
		unsigned int		get_channel_count();
		int *get_array_lin();
		float *get_array_lin_G();
		int **get_array();
		float **get_array_G();
};

/**
 * Class Constructor, everything is allocted at runtime
 * FIXME: Error handling is still missing!
 * @param block_count Number of blocks to use
 * @param lag_count Number of lags per block
 */
correlator::correlator(unsigned int block_count,unsigned int lag_count)
{
	next=0;
	correlator::block_count=block_count;
	blocks = (block **)malloc(block_count * sizeof(block*));
	if (blocks==0)
		printf("Error!");
	/**Create blocks*/
	for(unsigned int x=0;x<block_count;x++)
		blocks[x]=new block(x,lag_count);
	/**Create linked lists*/
	for(unsigned int x=0;x<(block_count-1);x++)
		blocks[x]->set_next(blocks[x+1]);
}

/**
 * Class Destructor
 */
correlator::~correlator () {
	for(unsigned int x=0;x<block_count;x++)
		delete blocks[x];
}

/**
 * Run the correlator for one step.
 * @param value_global Global value, used for all blocks and lags simultaniousely
 * @param value_local Local value, only used for first block.
 */
void correlator::run(int value_global,int value_local)
{
	if(block_count>0)
		blocks[0]->run(value_global,value_local);
}

/**
 * Print the sums of all lags. 
 * TODO: Switch to cout streaming and operator<< overloading
 * @param pos Starting point for printing. Contains the next free position afterwards.
 */
void correlator::print(unsigned int *pos)
{
	for(unsigned int x=0;x<block_count;x++)
		blocks[x]->print(pos);
}

unsigned int correlator::get_channel_count()
{
	unsigned int channel_count=0;
	for(unsigned int x=0;x<block_count;x++)
		channel_count+=blocks[0]->get_lag_count()*(1<<x);
	return channel_count;
}


int * correlator::get_array_lin()
{
	//int channel0=blocks[0]->get_channel(0);
	unsigned int channel_count=this->get_channel_count();
	int *array = (int*)malloc(channel_count * sizeof(int));
	
	int pos=0;
	for(unsigned int x;x<block_count;x++)
	{
		blocks[x]->fill_array_lin(&(array[pos]));
		pos+=blocks[0]->get_lag_count()*(1<<x);
	}
	
	return array;
}

float * correlator::get_array_lin_G()
{
	int *array=this->get_array_lin();
	unsigned int channel_count=this->get_channel_count();
	float *result = (float*)malloc(channel_count * sizeof(float));
	
	float channel0=blocks[0]->get_sum(0);
	for(int x=0;x<(this->get_channel_count());x++)
	{
		result[x]=((float)array[x])/(channel0*channel0);
	}
	delete array;
	return result;
}

int **correlator::get_array()
{
	int **result=(int**)malloc(2 * sizeof(int*));
	result[0]=(int*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(int));;
	result[1]=(int*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(int));;
	
	unsigned int tau=0;
	for(int x=0;x<block_count;x++)
		blocks[x]->fill_array(result,x*(blocks[0]->get_lag_count()),&tau);
	
	return result;
	
}

float **correlator::get_array_G()
{
	int **array=get_array();
	float **result=(float**)malloc(2 * sizeof(float*));
	result[0]=(float*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(float));;
	result[1]=(float*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(float));;

	float channel0=blocks[0]->get_sum(0);
	for(int x=0;x<(block_count*blocks[0]->get_lag_count());x++)
	{
		
		result[0][x]=(float)array[0][x];
		result[1][x]=((float)array[1][x])/(channel0*channel0);
	}
	
	free(array[1]);
	free(array[0]);
	free(array);
	return result;
	
}


#endif