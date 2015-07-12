/*
Copyright (c) 2008-2015 Jan Buchholz, German Cancer Research Center

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 *	Frame class
 *	
 *	Basic structure: 
 *	image data is store line-wise: pos=y*xwidth+x
 */

#ifndef __FRAME_H__
#define __FRAME_H__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h> //uint32_t
#include <cmath>

#include "helper.h"
/**
 * \defgroup frame_image_tools templated single frame class frame
 * \ingroup image_tools
 *
 * special capabilities for JanB's processing chain and very basic frame manipulation
 */

/*! \brief this class provides basic capabilities to manage 2D images.
    \ingroup frame_image_tools

      - special capabilities for JanB's processing chain and very basic frame manipulation.
      - The class may manage internal and external memory for the pixels
    .

 */
template <typename T, int XRES=32U, int YRES=32U>
class frame
{
	protected:
        /** \brief indicates whether the memory in pixels is externally managed (\c true ). In that case this class won't free the memory!  [defeault: false] */
        bool is_external;
		unsigned int xres,yres;
        unsigned int accumulation_count;/**< just count number of successful += ops*/
		unsigned int is_last;
		uint32_t id,id2;
		unsigned int size;/**< size of date in byte */
		unsigned int size_effective;/**< effective size in bytes, used for ready, when not a full frame is read */

		T *pixels;/**< image data */
		float *axis;/**< defines the value for the x-axis */
		int range;/**< pixel value range, 1bit=2, 2bit=4... etc */

	public:
                unsigned int get_xres() const { return xres; }
                unsigned int get_yres() const { return yres; }
                unsigned int get_size() const { return size; }
                unsigned int get_size_effective() const { return size_effective; }
                int get_range() const { return range; }
                uint32_t get_id() const { return id; }
                uint32_t get_id2() const { return id2; }

                void set_size_effective(unsigned int v) { size_effective=v; }
                void set_range(int v)  { range=v; }
                void set_id(uint32_t v)  { id=v; }
                void set_id2(uint32_t v)  { id2=v; }




		void resize(unsigned int xres=32U,unsigned int yres=32U)
		{
			this->xres=xres;
			this->yres=yres;

            if(this->pixels!=NULL && !is_external)
				free(this->pixels);
			this->pixels=NULL;
			this->pixels=(T *)calloc((this->xres)*(this->yres),sizeof(T));
			
			if(this->axis!=NULL)
				free(this->axis);
			this->axis=NULL;
            this->axis=(__typeof__(axis))calloc((this->xres),sizeof(__typeof__(*axis)));
		
			this->accumulation_count=0;
		
			this->is_last=0;
		
			this->id=-1;
			this->size=xres*yres*sizeof(T);
			this->size_effective=this->size;
			this->range=-1; /**< automatic range */
            this->is_external=false;

		}

	frame(unsigned int xres=XRES,unsigned int yres=YRES)
	{
		this->xres=0;
		this->yres=0;

		this->pixels=NULL;
		this->axis=NULL;
        this->is_external=false;

		resize(xres,yres);
	}

    frame(T* data, unsigned int xres=XRES,unsigned int yres=YRES)
    {
        this->xres=0;
        this->yres=0;

        this->pixels=NULL;
        this->axis=NULL;

        resize(xres,yres);
        free(this->pixels);
        this->is_external=true;
        this->pixels=data;
    }

    /** \brief assigns the contents of another frame \a other to this frame
     *
     *  \note if the other frame uses external memory management, also this frame will use externl memory management
     *        and both will point to the same memory segment. In the oppositecase the pixels array of the other frame will
     *        simply be copied!
     */
	frame & operator=(frame & f) 
	{
		if((this->xres==f.xres)&&(this->yres==f.yres))
		{
            if (f.is_external) {
                this->pixels=f.pixels;
            } else {
                memcpy (this->data(),f.data(),f.get_size());/** copy pixel data*/
            }
			
			this->accumulation_count=f.accumulation_count;
			this->is_last=f.is_last;
			this->id=f.id;
			this->size_effective=f.size_effective;
			this->range=f.range;
            this->is_external=f.is_external;
		}
		else
			fprintf(stderr,"ERROR: (%s) Frames do not match!",__FUNCTION__);
		return *this;
	}
 

    /** \brief class destructor, frees only non-externally managed (\c is_external==false ) memory */
	~frame()
	{
		if (!is_external && pixels!=NULL) free(pixels);
		if (axis) free(axis);
		pixels=NULL;
		axis=NULL;
	}


	inline void set_is_last(int value)
	{
		this->is_last=value;
	}

	inline int get_is_last()
	{
		return this->is_last;
	}
	
	template < typename P >
    void load_packed(P * buf){
		const unsigned int bpw=sizeof(P)*8;
		P *ptr=buf;
        for(long y=0;y<(long)yres;y++){
            for(long x1=0;x1<(long)(xres/bpw);x1++){//iterate over words
                P value=*ptr++;
                for(long x2=bpw-1;x2>=0;x2--){//itearte over all bits
					set_pixel((xres-1)-(x1*bpw+x2),y,value&0x1);
					value>>=1;
				}
			}
		}
	}

    /** \brief adds another frame pixel-wise to this frame, if the sizes are the same! */
	frame<T,XRES,YRES>  & operator+=(const frame<T,XRES,YRES> & f)
	{
		if((f.xres==xres)&&(f.yres==yres))
		{
			for(int x=0;x<xres;x++)
				for(int y=0;y<yres;y++)
					this->pixels[x*(this->yres)+y]+=f.pixels[x*(this->yres)+y];
			accumulation_count++;
			this->range+=f.range-1;
		}
		return *this;
	}

    /** \brief reset al pixels to 0 */
	void clear()
	{
		for(int x=0;x<xres;x++)
			for(int y=0;y<yres;y++)
				this->pixels[x*(this->yres)+y]=0;
		accumulation_count=0;
	}

    /** \briefoutput all pixel contents to a stream */
	friend std::ostream& operator<<(std::ostream& out, const frame & f)
	{
		for(int y=0;y<f.yres;y++)
		{
			for(int x=0;x<f.xres;x++)
			{
				out << f.pixels[x*f.yres+y] << ' ';
			}
			out<<std::endl;
		}
		return out;
	}

    /** \brief output pixel values (cast to int) as space-separated list to a file stream */
	void to_stream(FILE * stream)
	{
		fprintf(stream,"%i:",id);
		for(int i=0;i<(xres*yres);i++)
		{
			fprintf(stream," %u",(int)pixels[i]);
		}
		fprintf(stream,"\n");
	}

    /** \brief return the value of a given pixel, or 0 if it does not exist (out of range)! */
    T get_pixel(unsigned int x, unsigned int y) const
	{
		if((x<this->xres)&&(y<this->yres))
			return this->pixels[x*(this->yres)+y];
		else
			return 0;
	}

    /** \brief set the value of a given pixel, if it exists */
    void set_pixel(unsigned int x, unsigned int y, T value)
	{
		if((x<this->xres)&&(y<this->yres))
			this->pixels[x*(this->yres)+y]=value;
	}
	
    float* get_axis(unsigned int x) const
	{
		return(axis[x]);
	}
	
        void set_axis(unsigned int x, float* value)
	{
		axis[x]=value;
	}

    /** \brief return a pointer to the array of pixels */
    T* get_pixels() const
	{
		return this->pixels;
	}

	unsigned int get_accumulation_count()
	{
		return accumulation_count;
	}
	
    /** \brief sum up all pixel values */
    T get_sum()
	{
        T sum=0;
		for(int i=0;i<(xres*yres);i++)
			sum+=pixels[i];
		return sum;
	}
	
    /** \brief set all pixels thatare lareger than \a max to the given \a value */
	void ceil_pixels(T max, T value)
	{
		for(int i=0;i<(xres*yres);i++)
			if(pixels[i]>max)pixels[i]=value;
	}
	
	
        //GET(xres)
        //GET(yres)
        //GETSET(id)
        //GETSET(id2)
        //GET(size)
        //GETSET(size_effective)
	T *data(){return this->pixels;}/** byte-wise access to pixels */
    char *dataC(){return (char*)this->pixels;}
        //GETSET(range)
};
#endif
