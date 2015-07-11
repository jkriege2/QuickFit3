#ifndef __CORRELATOR_MULTITAU_H_INCLUDED__
#define __CORRELATOR_MULTITAU_H_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/*
*	Helper functions
*	x refers to the maximum value, 0 <= x_ < x
*/

template < typename T , typename R>
class correlatorjb;

template < typename T , typename R>
class block;

template < typename T , typename R>
class lag {
private:
	/**This is used to implemnt the flip flop.*/
	T *value_cur,*value_old;
	/**Next lag in chain.*/
	lag<T,R>    *next;
	/**Id of this lag in curent block.*/
	unsigned int id;
	
	block<T,R> *parent;
	T *null;
	
public:
	/**Result of correlation*/
	T *sum;
	/**Sum of the last two time steps. Used for connection to next block.*/
	T *acc;
	
	/**
	* Class Constructor.
	* @param id Id of the current lag in block. Range: 0..(x-1).
	* @param parent Currently unused. Might be used to update acc one klevel above...
	*/
	
	friend std::ostream& operator<<(std::ostream& os, const lag<T,R> &dt){
		os << "sum=" << dt.sum;
		return os;
	}
	
	void allocate()
	{
		value_cur = new T(*null);
		value_old = new T(*null);
		sum =       new T(*null);
		acc =       new T(*null);
	}
	
	void deallocate()
	{
		delete value_cur;
		delete value_old;
		delete sum;
		delete acc;
	}
	
	lag (unsigned int lag_id,block<T,R> *parent_block,T nullValue = 0U)
	{
		null = new T(nullValue);
		allocate();
		next=NULL;
		id=lag_id;
		parent = parent_block;
	}
	
	~lag (){
		deallocate();
	}
	
	/**
	* Set the next lag in chain. Used for creating linked lists.
	*@param next Next lag in chain.
	*/
	void set_next (lag *next_lag)
	{
		next=next_lag;
	}
	
	/**
	*	\brief reset lag
	*/
	void reset()
	{
		deallocate();
		allocate();
	}
	/**
	* Run the correlation for this lag for one time step. If this is the last lag in chain, acc is updated.
	* @param value_global Global value, used for all blocks and lags simultaniousely
	* @param value_local Local value, only used for first block.
	*/
	void run(T value_global, T value_local)
	{
		*sum += (value_global) * (value_local);
		if (next!=0)
			next->run(value_global,*value_cur);
		else
			*acc = ((*value_cur) + (*value_old));
		
		/** Implement the FlipFlop */
		*value_old = *value_cur;
		*value_cur = value_local;
	}
	
	/**
	* Print the result of the correlation for this lag.
	* @param pos Lag position in timing context.
	*/
	void print(unsigned int pos)
	{
		printf("%u %i\n",pos,sum);
	}
	
	T get_sum() {return *sum;}
	T get_value(){return *value_cur;}
};

/**
* Class block, implements one block of a multi-tau correlator.
* One blocks runs at the same clock.
*/
template <typename T,typename R>
class block
{
private:
	correlatorjb<T,R> *parent;
	lag<T,R> **lags;
	unsigned int lag_count;
	block<T,R> *next;
	/**Number of time steps processed*/
	/**Id of the current block. Used for printing purpose.*/
	unsigned int id;
	T *acc;
	T *sum,*sum_g;
	unsigned long ticks;
	T *null;
public:
	/**
	* Block Constructor. Uses dynamic allocation.
	*FIXME: Error handling is far from perfect.
	*@param id Id of the current block within a correlator. Should range from 0..(x-1).
	*@param lag_count Number of lags in this block.
	*/
	
	void allocate(){
		acc =   new T(*null);
		sum =   new T(*null);
		sum_g = new T(*null);
	}
	
	void deallocate(){
		delete acc;
		delete sum;
		delete sum_g;
	}
	
	block(unsigned int block_id,unsigned int lags_per_block, T nullValue = 0U)
	{
		null = new T(nullValue);
		allocate();
		next=0;
		ticks=0;
		lag_count=lags_per_block;
		lags = static_cast<lag<T,R> **>(malloc(lag_count * sizeof(lag<T,R>*)));
		if (lags==0)
			printf("Error!");
		for(unsigned int x=0;x<lag_count;x++)
			lags[x]=new lag<T,R>(x,this,*null);
		for(unsigned int x=0;x<(lag_count-1);x++)
			lags[x]->set_next(lags[x+1]);
		id=block_id;
		parent=NULL;
	}
	
	/**
	* Class destructor.
	*/
	~block () {
		for(unsigned int x=0;x<lag_count;x++)
			delete lags[x];
		deallocate();
	}
	
	/**
	*	\brief reset block
	*/
	void reset()
	{
		deallocate();
		allocate();
		ticks=0;
		for(unsigned int x=0;x<lag_count;x++)
			lags[x]->reset();
	}
	
	
	/**
	* Run the correlation within this block for one time step.
	* @param value_global Global value, used for all blocks and lags simultaniousely
	* @param value_local Local value, only used for first block.
	*/
	void run(T value_global,T value_local){
		if(lag_count>0)
			lags[0]->run(value_global,value_local);
		if(next!=NULL){
			if ((ticks%2)==1){
				*acc += value_global;
				next->run(*acc,*(lags[lag_count-1]->acc));
			}else{
				*acc = value_global;
			}
		}
		ticks++;
		*sum += value_local;
		*sum_g += value_global;
	}
	
	/**
	* Set the next block in chain. Used for creating linked lists.
	*@param next Next block in chain.
	*/
	void set_next (block *next_block){
		next=next_block;
	}
	
	void set_parent(correlatorjb<T,R> *parent_correlator){
		parent=parent_correlator;
	}
	
	int get_lag_count(){return lag_count;}
	
	void fill_array_lin(T *array)
	{
		for(int x=0;x<lag_count;x++)
			for(int y=0;y<(1<<id);y++)
				array[x*(1<<id)+y]=lags[x]->get_sum();
	}
	
	T get_sum(unsigned int lag_number)
	{
		if((lag_count>0)&&(lag_number<lag_count))
			return *(lags[lag_number]->sum);
		else
			return T(0.0);
	}
	
	void fillArray(R *taus, T *raws, T *tau){
		for(unsigned int x=0;x<lag_count;x++){
			taus[x]=R(*tau);
			raws[x]=lags[x]->get_sum();
			*tau+=(1<<id);
		}
		
	}
	
	
	void fill_array(T **array,unsigned int pos, unsigned int *tau)
	{
		for(unsigned int x=0;x<lag_count;x++)
		{
			array[0][pos+x]=*tau;
			array[1][pos+x]=lags[x]->get_sum();
			array[2][pos+x]=lags[x]->get_value();
			*tau+=(1<<id);
		}
	}
	
	unsigned int get_ticks() { return ticks; }
	T get_sum(){return sum;}
};


template <typename T,typename R>
class correlatorjb_result
{
private:
	R *tau;
	R *val;
	T *raw;
	
	unsigned int block_count;
	unsigned int lag_count;
public:
	correlatorjb_result(unsigned int blocks_per_correlator,unsigned int lags_per_block){
		block_count = blocks_per_correlator;
		lag_count= lags_per_block;
		tau = new R[block_count*lag_count];
		val = new R[block_count*lag_count];
		raw = new T[block_count*lag_count];
	}
	~correlatorjb_result(){
		delete [] tau;
		delete [] val;
		delete [] raw;
	}
	
	friend std::ostream& operator<<(std::ostream& os, const correlatorjb_result<T,R> &r){
		for(unsigned int x=0;x<r.block_count*r.lag_count;x++){
			os << "tau=" << r.tau[x] << "val=" << r.val[x] << "raw=" << r.raw[x] << std::endl;
		}
		return os;
	}
	
	R *get_taus(){return tau;}
	R *get_vals(){return val;}
	T *get_raws(){return raw;}
	
};


/**
* Correlator class implementing a multi-tau correlator.
* This class is NOT intended to be optimized or fast,
* it tries to build a hardware equivalent of a parallel
* multi-tau correlator without beeing parallel. ;-)
* This is how it should look like (for a (3,2) invocation):
*
*
*/
template <typename T,typename R>
class correlatorjb
{
private:
	
	block<T,R> **blocks;/**Blocks running at different frquencies*/
	unsigned int block_count,lagCount;/**Number of blocks in current correlator setup*/
	correlatorjb<T,R> *next;/**One way links list, reference to the next block in chain.*/
	T *null,*sum,*sum_g;
	unsigned int ticks;
	T *acc_global,*acc_local;
	unsigned short int acc_cnt,acc_cnt_max;
public:
	typedef correlatorjb_result<T,R> result;
	
	/** 
	* Default CLass constructor. You must call init() afterwards
	*/
		
		/**
		* Initialize class. Should be calles by constructor.
		* FIXME: Error handling is still missing!
		* @param blocks_per_correlator Number of blocks to use
		* @param lags_per_block Number of lags per block
		* @param nullValue Default neutral element of the template input class (T)
		*/
		void init(unsigned int blocks_per_correlator,unsigned int lags_per_block, T nullValue){
			null = new T(nullValue);
			sum = new T(*null);
			sum_g = new T(*null);
			acc_global=new T(*null);
			acc_local=new T(*null);
			acc_cnt=0;
			acc_cnt_max=1;
			block_count=blocks_per_correlator;
			lagCount=lags_per_block;
			blocks = static_cast< block<T,R> ** > (malloc(block_count * sizeof(block<T,R>*)));
			if (blocks==NULL)
				printf("Error!");
			/**Create blocks*/
			for(unsigned int x=0;x<block_count;x++)
				blocks[x]=new block<T,R>(x,lags_per_block,*null);
			/**Create linked lists*/
			for(unsigned int x=0;x<(block_count-1);x++)
			{
				blocks[x]->set_next(blocks[x+1]);
				blocks[x]->set_parent(this);
			}
			for(unsigned int x=0;x<block_count;x++)
			{
				blocks[x]->set_parent(this);
			}
		}
		
		/**
		* Class constructor.
		* FIXME: Error handling is still missing!
		* @param blocks_per_correlator Number of blocks to use
		* @param lags_per_block Number of lags per block
		* @param nullValue Default neutral element of the template input class (T)
		*/
		correlatorjb(unsigned int blocks_per_correlator,unsigned int lags_per_block, T nullValue) :
			blocks(NULL),
			block_count(0),
			lagCount(0),
			next(NULL),
			ticks(0) {
				init(blocks_per_correlator,lags_per_block,nullValue);
		}
		
		correlatorjb():
			blocks(NULL),
			block_count(0),
			lagCount(0),
			next(NULL),
			ticks(0){
		}
		
		/**
		* Class Destructor
		*/
		~correlatorjb () {
			for(unsigned int x=0;x<block_count;x++)
				delete blocks[x];
		}
		
		/**
		*	\brief reset correlator state
		*	This resets the whole correlator to initial state
		*/
		void reset(){
			ticks = 0;
			sum = new T(*null);
			sum_g = new T(*null);
			/** reset all blocks */
			for(unsigned int x=0;x<block_count;x++)
				blocks[x]->reset();
		}
		
		/**
		* Run the correlator for one step.
		* @param value_global Global value, used for all blocks and lags simultaniousely
		* @param value_local Local value, only used for first block.
		*/
		void run(T value_global,T value_local){
			if(block_count>0)
			{
//				std::cerr<<value_global<<":"<<value_local<<" "<<std::endl;
				blocks[0]->run(value_global,value_local);
			}
			ticks++;
			*sum   += value_local;
			*sum_g += value_global;
//  			if(value_global>1){
// 				std::cerr<<"Warning: Global value >1"<<std::endl;
//  				exit(1);
//  			}
		}
		
		/**
		* Run the correlator with pre-accumulation.
		* @param value_global Global value, used for all blocks and lags simultaniousely
		* @param value_local Local value, only used for first block.
		* 
		* Runs the correlator for a single step. Beforehand the data is accumuted.
		*/
		void run_acc(T value_global,T value_local){
			*acc_global += value_global;
			*acc_local  += value_local;
			acc_cnt++;
			if(acc_cnt>=acc_cnt_max){
				run(*acc_global,*acc_local);
				*acc_global=*null;
				*acc_local=*null;
				acc_cnt=0;
			}
		}
		
		void set_acc_cnt_max(unsigned short int acc_cnt_max){this->acc_cnt_max=acc_cnt_max;}
		
		unsigned int get_channel_count(){
			unsigned int channel_count=0;
			for(unsigned int x=0;x<block_count;x++)
				channel_count+=blocks[0]->get_lag_count()*(1<<x);
			return channel_count;
		}
		
		unsigned int get_lag_count(){
			return this->block_count*this->blocks[0]->get_lag_count();
		}
		
		T * get_array_lin(){
			unsigned int channel_count=this->get_channel_count();
			T *array = static_cast<T*>(malloc(channel_count * sizeof(T)));
			int pos=0;
			for(unsigned int x=0; x<block_count;x++){
				blocks[x]->fill_array_lin(&(array[pos]));
				pos+=blocks[0]->get_lag_count()*(1<<x);
			}
			return array;
		}
		
		R * get_array_lin_G(){
			T *array=this->get_array_lin();
			unsigned int channel_count=this->get_channel_count();
			R *result = static_cast<R*>(malloc(channel_count * sizeof(R)));
			
			R channel0(blocks[0]->get_sum(0));
			for(int x=0;x<(this->get_channel_count());x++)
			{
				result[x]=R(array[x]);//cast
				result[x]/=channel0 * channel0;
			}
			delete array;
			return result;
		}
		
		T ** get_array()
		{
			T **array_result=(T**)malloc(3 * sizeof(T*));
			array_result[0]=(T*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(T));;
			array_result[1]=(T*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(T));;
			array_result[2]=(T*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(T));;
			
			unsigned int tau=0;
			for(unsigned int x=0;x<block_count;x++)
				blocks[x]->fill_array(array_result,x*(blocks[0]->get_lag_count()),&tau);
			
			return array_result;
			
		}
		
		void getArray(R *taus,T *raws){
			T *tau = new T(0.0);
			for(unsigned int x=0;x<block_count;x++)
				blocks[x]->fillArray(&taus[x*lagCount],&raws[x*lagCount],tau);    
		}
		
		R ** get_array_G()
		{
			T **array=get_array();
			R **result=(R**)malloc(2 * sizeof(R*));
			result[0]=(R*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(R));;
			result[1]=(R*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(R));;
			
			T ticks=T(this->ticks);
			
			for(unsigned int x=0;x<(block_count*blocks[0]->get_lag_count());x++){
				result[0][x]=(R)array[0][x];
				unsigned int block=x/blocks[0]->get_lag_count();
				
				R sum=(R(*(this->sum))*R(ticks-result[0][x]))/R(ticks);
				// see "A distributed algorithm for multi-tau autocorrelation"                                                                                                                                                                                                         
				result[1][x]=(R(array[1][x])*R(ticks))/(R(1<<block)*sum*R(*(this->sum)));
				//std::cerr << "ticks=" << ticks << " result=" << result[1][x] << " sum=" << this->sum << " raw=" << array[1][x] << " tau=" << result[0][x] << std::endl;
				
				/* if(ticks>result[0][x]){ */
				/*   // see "A distributed algorithm for multi-tau autocorrelation" */
				/* 	result[1][x]=((R)array[1][x]*(R)ticks)/((R)(1<<block)*(R)(sum*this->sum)); */
				/* }else{ */
				/* 	result[1][x]=1.0; */
				/* } */
			}
			
			free(array[1]);
			free(array[0]);
			free(array);
			return result;
			
		}
		
		/*!
		*		\brief Returns a nomalized array of values
		* 
		*		This function returns an array with thre resuklting values of the correlation.
		*		It is intended to be usable with cross- and auto-correlation.
		* 
		*/
		R ** get_array_G2()
		{
			T **array=get_array();
			R **result=(R**)malloc(2 * sizeof(R*));
			result[0]=(R*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(R));;
			result[1]=(R*)malloc(block_count*blocks[0]->get_lag_count() * sizeof(R));;
			
			for(int x=0;x<(block_count*blocks[0]->get_lag_count());x++)
			{
				result[0][x]=(R)array[0][x];
				unsigned int block=x/blocks[0]->get_lag_count();
				
				if(ticks>result[0][x])
					result[1][x]=((R)array[1][x]*(R)ticks)/((R)(1<<block)*(R)(((R)sum*R(sum_g)*((R)(ticks-result[0][x]))/(R)ticks)));// see "A distributed algorithm for multi-tau autocorrelation"
					else
						result[1][x]=1.0;
					
			}
			
			free(array[1]);
			free(array[0]);
			free(array);
			return result;
			
		}
		
		result *get_G(){
			//T **array=get_array();
			result *result_G = new result(block_count,blocks[0]->get_lag_count());
			R *taus = result_G->get_taus();
			R *vals = result_G->get_vals();
			T *raws = result_G->get_raws();
			getArray(taus,raws);
			
			for(unsigned int x=0;x<(block_count*blocks[0]->get_lag_count());x++)
			{
				// raws[x]=array[1][x];
				// taus[x]=R(array[0][x]);
				unsigned int block=x/blocks[0]->get_lag_count();
				R ticks_R = R(ticks);
				R sum_R = R(*sum);
				R sum_g_R= R(*sum_g);
				R num = R(raws[x]) * ticks_R * ticks_R;
				R den = R(1<<block) * sum_R * sum_g_R * (R(ticks)-taus[x]);
				vals[x]=(num/den);
			}
			
			//free(array[1]);
			//free(array[0]);
			//free(array);
			return result_G;
		}
		
		
		unsigned int get_ticks(){return ticks;}
		T get_sum(){return *sum;}

		/*!
		*		Compute \tau'
		*		@param l	lags per block
		*		@param k_	current linear position of correlator
		*/
		static unsigned int get_tau_(unsigned int l, unsigned int k_)
		{
			unsigned int result=0;
			for(unsigned int k=1;k<=k_;k++)
				result+=1<<((k-1)/l);
			return result;
		}
		
		/*!
		*		Fill array with tau values
		*		@param b		number of blocks
		*		@param l		number of lags within blocks
		*		@param taus	resulting values
		*/
		static void getTaus(unsigned int b, unsigned int l, unsigned int *taus){
			unsigned int k=b*l;
			for(unsigned int k_=0;k_<k;k_++){
					taus[k_]=get_tau_(l,k_);
			}
		}
		
		/*!
		*		Normalize array
		*		@param data		pointer to data array
		*		@param count	number of elements in array
		*		@param values	resulting normalized values
		*/
		template<typename D, typename V>
		static void normalize(D* data, unsigned int ticks, unsigned int count_global, unsigned int count_local, unsigned int b, unsigned int l, V* values, unsigned int *taus)
		{
			unsigned int *tau_=taus;
			V *value_=values;
			D *data_=data;

			for(unsigned int b_=0;b_<b;b_++)
			{
				for(unsigned int l_=0;l_<l;l_++)
				{
					V correction_factor=static_cast<V>(ticks-(*tau_))/static_cast<V>(ticks);
					*value_=(static_cast<V>(*data_)*static_cast<V>(ticks))/(static_cast<V>(1<<b_)*static_cast<V>(count_global*count_local)*correction_factor);
					data_++;
					value_++;
					tau_++;
				}
			}
			
		}

		template<typename D, typename V>
		static void normalize(D* data, unsigned int ticks, unsigned int b, unsigned int l, V* values, unsigned int *taus){
			normalize<D,V>(data, ticks, data[0], data[0], b, l, values, taus);
		}
		
		template<typename D, typename V>
		static void normalize(D* data, unsigned int ticks, unsigned int b, unsigned int l, V* values)
		{
            unsigned int *taus = static_cast<unsigned int *>(malloc(b*l*sizeof(unsigned int)));
			getTaus(b,l,taus);
            normalize<D,V>(data, ticks, data[0], data[0], b, l, values, taus);
			free(taus);
		}
};






#endif
