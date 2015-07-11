/*
 *	based on producer/consumer example by Andrae Muys
 *	http://www.cs.nmsu.edu/~jcook/Tools/pthreads/examples.html
 */

#ifndef __HISTOGRAM__
#define __HISTOGRAM__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h> //uint32_t
#include <cmath>
#include <list>

#define histogram_sqr(a) ((a)*(a))

using namespace std;

template <class T1>
class histogram_entry
{
	private:
		T1 value,count;
		
	public:
		histogram_entry(T1 value,T1 count = (T1) 1)
		{
			this->count=count;
			this->value=value;
		}
		
		~histogram_entry()
		{
		}
		
		void inc()  { count++;};
		T1 get_count() {return count;};
		T1 get_value() {return value;};
		
		bool operator<(const histogram_entry & h)
		{
			return (this->value<h.value);
		}
		
		bool operator==(const histogram_entry & h)
		{
			return (this->value==h.value);
		}
		
		friend std::ostream& operator<<(std::ostream & out, const histogram_entry & h)
		{	
			out << h.value << " " << h.count;
			return out;
		}
		
		
};


		
template <class T2>
class histogram
{
	private:
		list< histogram_entry<T2> > data;
    bool sorted;

	public:
		histogram(T2 * data,unsigned int size)
		{
                        for(unsigned int i=0;i<size;i++)
			{
				histogram_entry<T2> *entry;
				entry=new histogram_entry<T2>(data[i]);
				this->data.push_back(*entry);
                delete entry;
			}
            sorted=false;
		}
		
		~histogram()
		{
			//destroy data;
		}

		void sort()
		{
			data.sort();
            sorted=true;
		}

		friend std::ostream& operator<<(std::ostream & out, const histogram & h)
		{	
			typename list< histogram_entry<T2> >::const_iterator it;
			
			for (it=h.data.begin(); it!=h.data.end(); ++it)
				out << /*" " <<*/ *it << std::endl;
			out << std::endl;
			return out;
		}
				
        inline static bool uniqueness(histogram_entry<T2> & h1, histogram_entry<T2> & h2)
		{
			if(h1==h2)
			{
				h1.inc();
				h2.inc();
				return(true);
			}
			else
				return(false);
		};
		
        inline static bool uniqueness10(histogram_entry<T2> & h1, histogram_entry<T2> & h2)
		{
			if((h1.get_value()/12)==(h2.get_value()/12))
			{
				h1.inc();
				h2.inc();
				return(true);
			}
			else
				return(false);
		};
		
		void unique()
		{
			this->data.unique(uniqueness);
		}
		
		void create_histogram(unsigned int start, unsigned int stop, unsigned int steps)
		{
			this->data.unique(uniqueness10);
		}
		
		float get_mean()
		{
			typename list< histogram_entry<T2> >::iterator it;
			unsigned long long mean=0;
	
			for (it=this->data.begin(); it!=this->data.end(); ++it)
				mean+=(*it).get_value();
			printf("%llu\n",mean);
			return((float)mean/((float)this->data.size()));
		}

    float get_deviation()
    {
      typename list< histogram_entry<T2> >::iterator it,it1,it2;

      float sum=0,mean=this->get_mean(),size=this->data.size();

      for (it=data.begin(); it!=data.end(); it++)
        sum+=histogram_sqr((float)(*it).get_value()-mean);
      return sqrt(sum/size);
    }

    float median()
    {
      if(!sorted)sort();
      typename list< histogram_entry<T2> >::iterator it=this->data.begin();

      for (unsigned int i=0;i<data.size()/2;i++){
        it++;
      }
      return (*it).get_value();
    }
		
		float get_mean2(float percentage_cut_low, float percentage_cut_high)
		{
			typename list< histogram_entry<T2> >::iterator it,it1,it2;
			unsigned long long mean=0;

			float size=this->data.size();
			
			unsigned int cut_low =(unsigned int)floor(percentage_cut_low*size/100.0);
			unsigned int cut_high=(unsigned int)floor(percentage_cut_high*size/100.0);
			
	
			it1=this->data.begin();
			it2=this->data.end();

			for(int i=0;i<cut_low;i++)it1++;
			for(int i=0;i<cut_high;i++)it2--;
			
			for (it=it1; it!=it2; ++it)
				mean+=(*it).get_value();
			return((float)mean/(size-(float)(cut_low+cut_high)));
		}
		
		float get_deviation2(float percentage_cut_low, float percentage_cut_high)
		{
			typename list< histogram_entry<T2> >::iterator it,it1,it2;
	
			float sum=0,mean=this->get_mean2(percentage_cut_low,percentage_cut_high),size=this->data.size();
			
			unsigned int cut_low =(unsigned int)floor(percentage_cut_low*size/100.0);
			unsigned int cut_high=(unsigned int)floor(percentage_cut_high*size/100.0);
			
			size-=(float)(cut_low+cut_high);
	
            it1=this->data.begin();
			it2=this->data.end();

			for(int i=0;i<cut_low;i++)it1++;
            for(int i=0;i<cut_high;i++)it2--;

			for (it=it1; it!=it2; ++it)
                sum+=histogram_sqr((float)(*it).get_value()-mean);
			return sqrt(sum/size);
		}
		
};
#endif
