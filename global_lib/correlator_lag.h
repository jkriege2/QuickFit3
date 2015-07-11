#ifndef __CORRELATOR_LAG_H_INCLUDED__
#define __CORRELATOR_LAG_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>

struct values
{
	int cur,old;
};

class lag {
	private:
		/**This is used to implemnt the flip flop.*/
		values				value;
		/**Next lag in chain.*/
		lag						*next;
		/**Id of this lag in curent block.*/
		unsigned int 	id;
  public:
		/**Result of correlation*/
		int sum;
		/**Sum of the last two time steps. Used for connection to next block.*/
		int acc;

				lag(unsigned int,void *);
	void	run (int,int);
	void	set_next (lag *);
	void	print(unsigned int);
	int		get_sum();
};

/**
 * Class Constructor. 
 * @param id Id of the current lag in block. Range: 0..(x-1).
 * @param parent Currently unused. Might be used to update acc one klevel above...
 */
lag::lag (unsigned int id,void *parent)
{
	value.old=0;
	value.cur=0;
	next=NULL;
	sum=0;
	acc=0;
	lag::id=id;
}

/**
 *Set the next lag in chain. Used for creating linked lists.
 *@param next Next lag in chain.
 */
void lag::set_next (lag *next)
{
	lag::next=next;
}

/**
 * Run the correlation for this lag for one time step. If this is the last lag in chain, acc is updated.
 * @param value_global Global value, used for all blocks and lags simultaniousely
 * @param value_local Local value, only used for first block.
 */
void lag::run(int value_global, int value_local)
{
	sum+=value_global*value_local;
	if (next!=0)
		next->run(value_global,value.cur);
	else
		acc=(value.cur+value.old);

	/**Implement the FlipFlop*/
	value.old=value.cur;
	value.cur=value_local;
}

/**
 * Print the result of the correlation for this lag.
 * @param pos Lag position in timing context.
 */
void lag::print(unsigned int pos)
{
	printf("%u %i\n",pos,sum);
}

int lag::get_sum()
{
	return sum;
}

#endif 
