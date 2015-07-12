#ifndef __LUT_H__
#define __LUT_H__

#include <stdint.h> //uint32_t
#include <cmath>

#define PI    3.14159265358979323846f 

class lookuptable
{
	private:
		uint8_t **table;
		unsigned int channels;
		unsigned int range_in;

	public:
		lookuptable(unsigned int channels,unsigned int range_in)
		{
			this->channels=channels;
			this->range_in=range_in;

			this->table=(uint8_t **)calloc(this->channels,sizeof(uint8_t *));
			for(unsigned int i=0; i<this->channels; i++)
			{
				this->table[i]=(uint8_t *)calloc(this->range_in,sizeof(uint8_t));
			}
		}

		~lookuptable()
		{
			for(unsigned int i=0; i<this->channels; i++)
			{
				free(this->table[i]);
			}
			free(this->table);
		}
		
		float rgbformulae_custom(int formula, float value)
		{
			float result;
			
			if(value<0.0)value=0.0;
			if(value>1.0)value=1.0;
			
			switch(abs(formula))
			{
				case  0:
					result=0.0;
					break;
				case 1:
					result=value*12.0; 
					if(result>1.0)result=0.0;
					break;
				case 2:
					result=value*12.0;
					break;
			}
			
			if(result<0.0)result=0.0;
			if(result>1.0)result=1.0;
			if(formula<0.0)result=1.0-result;
			
			return(result);
		}
		

		float gnuplot_rgbformulae(int formula, float value)
		{
			float result;
			
			if(value<0.0)value=0.0;
			if(value>1.0)value=1.0;
			
			switch(abs(formula))
			{
				case  0:result=0.0; break;
				case  1:result=0.5; break;
				case  2:result=1.0; break;
				case  3:result=value; break;
				case  4:result=value*value; break;
				case  5:result=pow(value,3); break;
				case  6:result=pow(value,4); break;
				case  7:result=sqrt(value); break;
				case  8:result=sqrt(sqrt(value)); break;
				case  9:result=sin(PI/2.0*value); break;
				case 10: result=cos(PI/2.0*value); break;
				case 11: result=abs(value-0.5); break;
				case 12: result=pow((2.0*value-1.0),2.0); break;
				case 13: result=sin(PI*value); break;
				case 14: result=abs(cos(PI*value)); break;
				case 15: result=sin(2.0*PI*value); break;
				case 16: result=cos(2.0*PI*value); break;
				case 17: result=abs(sin(2.0*PI*value)); break;
				case 18: result=abs(cos(2.0*PI*value)); break;
				case 19: result=abs(sin(4.0*PI*value)); break;
				case 20: result=abs(cos(4.0*PI*value)); break;
				case 21: result=3.0*value; break;
				case 22: result=3.0*value-1.0; break; 
				case 23: result=3.0*value-2.0; break;
				case 24: result=abs(3.0*value-1.0); break;
				case 25: result=abs(3.0*value-2.0); break;
				case 26: result=(3.0*value-1.0)/2.0; break;
				case 27: result=(3.0*value-2.0)/2.0; break;
				case 28: result=abs((3.0*value-1.0)/2.0); break;
				case 29: result=abs((3.0*value-2.0)/2.0); break;
				case 30: result=value/0.32-0.78125; break;
				case 31: result=2.0*value-0.84; break;
				case 32: result=4.0*value; break;
				case 33: result=abs(2.0*value - 0.5); break;
				case 34: result=2.0*value; break;
				case 35: result=2.0*value - 0.5; break;
				case 36: result=2.0*value - 1.0; break;
			}
			
			if(result<0.0)result=0.0;
			if(result>1.0)result=1.0;
			if(formula<0.0)result=1.0-result;
			
			return(result);
		}
		
		void table_fill_rgbformulae(int *formula)
		{
			for(unsigned int i=0;i<this->range_in;i++)
			{
				for(unsigned int c=0;c<this->channels;c++)
				{
					this->table[c][i]=255.0*gnuplot_rgbformulae(formula[c],(float)i / (float)this->range_in);
				}
			}
		}

		void table_fill_rgbformulae_custom(int *formula)
		{
			for(unsigned int i=0;i<this->range_in;i++)
			{
				for(unsigned int c=0;c<this->channels;c++)
				{
					this->table[c][i]=255.0*rgbformulae_custom(formula[c],(float)i / (float)this->range_in);
					fprintf(stderr,"%3u ",this->table[c][i]);
				}
				fprintf(stderr,"\n");
			}
		}

		
		
		inline uint8_t lookup(unsigned int channel, unsigned int value)
		{
			return this->table[channel][value];
		}
};

#endif