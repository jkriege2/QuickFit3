/*
 *	based on producer/consumer example by Andrae Muys
 *	http://www.cs.nmsu.edu/~jcook/Tools/pthreads/examples.html
 */

#ifndef __STATISTICS__
#define __STATISTICS__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h> //uint32_t
#include <cmath>
#include <list>

using namespace std;

template <class T1>
class entry
{
	private:
		T1 value,count;
		
	public:
		entry(T1 value,T1 count = (T1) 1)
		{
			this->count=count;
			this->value=value;
		}
		
		~entry()
		{
		}
		
		void inc()  { count++;}
		T1 get_value() {return value;};

		
		bool operator<(const entry & h)
		{
			return (this->value<h.value);
		}
		
		bool operator==(const entry & h)
		{
			return (this->value==h.value);
		}
		
		friend std::ostream& operator<<(std::ostream & out, const entry & h)
		{	
			out << h.value << " " << h.count;
			return out;
		}
};

		template <class T2>
		bool uniqueness(entry<T2> & e1, entry<T2> & e2)
		{
			if(e1==e2)
			{
				e1.inc();
				e2.inc();
				return(true);
			}
			else
				return(false);
		};
		
template <class T2>
class distribution
{
	private:
		list< entry<T2> > *data;

	public:
		distribution(T2 * data,unsigned int size)
		{
			data = new list< entry<T2> >();
			for(int i=0;i<size;i++)
			{
				entry<T2> *e;
				e=new entry<T2>(data[i]);
				this->data->push_back(*e);
				delete e;
			}
		}
		
		distribution()
		{
			data = new list< entry<T2> >();
		}
		
		~distribution()
		{
			delete data;
		}
		
		void add(T2 value)
		{
			entry<T2> *e;
			e=new entry<T2>(value);
			this->data->push_back(*e);
			delete e;
		}

		void sort()
		{
			this->data->sort();
		}
		
	
		void unique()
		{
			data->unique(uniqueness<T2>);
		}

		friend std::ostream& operator<<(std::ostream & out, const distribution & h)
		{	
			typename list< entry<T2> >::const_iterator it;
			
			for (it=h.data->begin(); it!=h.data->end(); ++it)
				out << /*" " <<*/ *it << std::endl;
			out << std::endl;
			return out;
		}
		
		float get_mean()
		{
			typename list< entry<T2> >::iterator it;
			double mean=0;
	
			for (it=this->data->begin(); it!=this->data->end(); ++it)
				mean+=(*it).get_value();
			return((float) (mean/(double)this->data->size()));
		}
		
		float get_median()
		{
			typename list< entry<T2> >::iterator it=this->data->begin();
			float result;
			
			int size=data->size();
			for (int i=0;i<size/2-1; i++)it++;
			if((size%2)==0)
			{
				result=(*it).get_value();
				it++;
				result+=(*it).get_value();
				result/=2;
			}
			else
			{
				it++;
				result=(*it).get_value();
			}
			return result;
		}
		
		float get_min(float cut=0.0)
		{
			typename list< entry<T2> >::iterator it=this->data->begin();
			for(int i=0;i<(data->size()*cut);i++)
				it++;
			return (*it).get_value();
		}

		float get_max(float cut=0.0)
		{
			typename list< entry<T2> >::reverse_iterator it=this->data->rbegin();
			for(int i=0;i<(data->size()*cut);i++)
				it++;
			return (*it).get_value();
		}
};
#endif