#ifndef __WE_SINK_SOURCE_H__
#define __WE_SINK_SOURCE_H__

#include "worker_element.h"

class we_sink_source : public worker_element
{ 
	Q_OBJECT 
	
	public:
		we_sink_source(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : worker_element(parent,0)/**< we don't knwo the frame count yet */
		{
			/** delte old thread */
			delete thread;
			thread = NULL;
			
			/** add new interfaces */
//fprintf(stderr,"A)%p,%p,%p\n",intf_out[0],intf_out[1],intf_out[2]);
//fprintf(stderr,"B)%p,%p,%p\n",intf_in[0],intf_in[1],intf_in[2]);
			intf_in=expand<worker_interface>(intf_in,1);
			intf_out=expand<worker_interface>(intf_out,1);
//fprintf(stderr,"C)%p,%p,%p\n",intf_out[0],intf_out[1],intf_out[2]);
//fprintf(stderr,"D)%p,%p,%p\n",intf_in[0],intf_in[1],intf_in[2]);
			
			intf_in[1]=new worker_interface(this,false,frame_count);
			intf_out[1]=new worker_interface(this,true,frame_count);
			
			/** connect new interfaces */
			intf_in[1]->connect_next(intf_out[1]);
			
			/** set interface properties */
			intf_in[0]->set_is_special(true); /** sink */
			intf_out[1]->set_is_special(true); /** source */
			
			/** setup frames */
			frame<uint32_t> **frames=(frame<uint32_t> **)calloc(frame_count+1,sizeof(frame<uint32_t>*));
            for(unsigned int i=0;i<frame_count;i++)
			{
				frames[i]=(frame<uint32_t> *)new frame<uint32_t>(frame_size_x,frame_size_y);
			}
			frames[frame_count]=NULL;
			intf_out[1]->set_frames(frames);
		}
			
		~we_sink_source() {}
		
};
#endif
