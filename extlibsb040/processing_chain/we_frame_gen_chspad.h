/*
*	Frame Generator for the CHSPAD 
*	
*	This is the second component in the working chain. It takes raw data and assembles frames.
*	It can be used with a raw file reader ow with usb acquisition
*	THIS IS A SINK/SOURCE
*	It should be capable of accepting dynmaic frame sizes
* It emits only correct raw data frames
*
*	-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
*                | # |
* <- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
*/

#ifndef __WE_FRAME_GEN_CHSPAD_H__
#define __WE_FRAME_GEN_CHSPAD_H__

#include <QWidget>
#include <QThread>
#include <QDebug>

#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal

#include "worker_thread.h"
#include "we_sink_source.h"
#include "qqueue.h"
#include "frame.h"
#include "helper.h"
#include "textcolor.h"

const unsigned int xres=4; //frames to attach in x dir
const unsigned int yres=128; //frames to attach in y dir

class we_frame_gen_chspad_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		void run()
		{
			unsigned int sub_frames_count=0;
			/** setup envionment */
			worker_element *parent=(worker_element*)this->parent();
			qfifo *fifo_snk_in =	(parent->get_intf_in())[0]->get_fifo();
			qfifo *fifo_snk_out =	(parent->get_intf_out())[0]->get_fifo();
			qfifo *fifo_src_in =	(parent->get_intf_in())[1]->get_fifo();
			qfifo *fifo_src_out =	(parent->get_intf_out())[1]->get_fifo();
			frame<uint32_t> **frames_snk = (parent->get_intf_in())[0]->get_frames();
			frame<uint32_t> **frames_src = (parent->get_intf_in())[1]->get_frames();
			
			if((fifo_snk_in==NULL)||(fifo_snk_out==NULL)||(fifo_src_in==NULL)||(fifo_src_out==NULL)||(frames_snk==NULL)||(frames_src==NULL))
			{
				CERR(fifo_snk_in)
				CERR(fifo_snk_out)
				CERR(fifo_src_in)
				CERR(fifo_src_out)
				CERR(frames_snk)
				CERR(frames_src)
				exit();
			}
			else
			{
				//snk side: fifo_snk_in, fifo_snk_out, frames_snk, 
				//src side: fifo_src_in, fifo_src_out, frames_src
				int last_frame=0;
				unsigned long int intensities[4]={0,0,0,0};
				unsigned long int photon_count=0;
				unsigned int frame_nr_current=0;
				int id_in=fifo_snk_in->pop(stop);/** get new frame for processing*/
				if(id_in<0)return;
				char *data=((char*)frames_snk[id_in]->data())+8;
				uint32_t frame_id=frames_snk[id_in]->data()[0]>>8;
				unsigned int frame_nr=frame_id>>9;
				std::cerr << "STILL OKAY" << std::endl;
				do{
					int id_out=fifo_src_in->pop(stop);/** get new frame for output*/
					if(id_out<0)break;
					frames_src[id_out]->clear();
					photon_count=0;
					intensities[0]=0;
					intensities[1]=0;
					intensities[2]=0;
					intensities[3]=0;
					do{
						data=((char*)frames_snk[id_in]->data())+8;
						frame_id=frames_snk[id_in]->data()[0]>>8;
						frame_nr=frame_id>>9;
						if(frame_nr != frame_nr_current){
							continue;
						}
						unsigned int xpos=(frame_id&0x3);
						unsigned int ypos=(frame_id%512)>>2;
						//std::cerr << "FRAME SEGEMNET" << frame_id << "," << frame_nr << "," << xpos << "," << ypos << std::endl;
						//Okay, this is slow now...
						for(unsigned int p=0;p<128/2;p++){
							uint32_t v=*((uint32_t*)data);
							//v&=0x0FFFU;
							frames_src[id_out]->set_pixel(xpos,ypos,(v&0x0FFFU)<<2);
							photon_count+=v&0x0FFFU;
							intensities[frame_id&0x3]+=v&0x0FFFU;
							xpos+=4;
							v=v>>12;
							frames_src[id_out]->set_pixel(xpos,ypos,(v&0x0FFFU)<<2);
							photon_count+=v&0x0FFFU;
							intensities[frame_id&0x3]+=v&0x0FFFU;
							xpos+=4;
							data+=3;
						}
						last_frame=frames_snk[id_in]->get_is_last();
						frames_src[id_out]->set_id(frame_nr);
						frames_src[id_out]->set_is_last(last_frame);
						fifo_snk_out->push(id_in);
						sub_frames_count++;
						id_in=fifo_snk_in->pop(stop);/** get new frame for processing*/
						if(id_in<0)break;
					}while((frame_nr == frame_nr_current)&&(last_frame==0));
					fifo_src_out->push(id_out);
					frames_processed++;
					frame_nr_current=frame_nr;
					std::cerr << "PROCESSED: " << frames_processed << "," << sub_frames_count << "," << frame_nr_current << ", PC="<< photon_count << "," << photon_count/(64*1024)<< " / ";
					std::cerr << intensities[0] << "," << intensities[1] << "," << intensities[2] << "," << intensities[3] << std::endl; 
					sub_frames_count=0;
				}while((last_frame==0)&&(*stop==false));
				std::cerr << "STOP=" << *stop << "LAST_FRAME" << last_frame<<std::endl;
				qDebug() << parent->objectName() << " DONE";
			}
		}
		
	public:
    we_frame_gen_chspad_thread(QObject *parent) : worker_thread(parent){}

		~we_frame_gen_chspad_thread() {}
};

class we_frame_gen_chspad : public we_sink_source
{ 
	Q_OBJECT 
	
	private:
		
	public:
		we_frame_gen_chspad(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			delete thread;
			thread = new we_frame_gen_chspad_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

		}
			
		~we_frame_gen_chspad() {}
};
#endif
