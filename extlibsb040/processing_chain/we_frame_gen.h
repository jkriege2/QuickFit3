/*
*	Frame Analyzer
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

#ifndef __WE_FRAME_GEN_H__
#define __WE_FRAME_GEN_H__

#include <QWidget>
#include <QThread>
#include <QDebug>

#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal

#include "worker_thread.h"
#include "we_sink_source.h"
#include "../../../../LIB/trunk/qt/qqueue.h"
#include "../../../../LIB/trunk/frame.h"
#include "../../../../LIB/trunk/helper.h"
#include "../../../../LIB/trunk/textcolor.h"

class we_frame_gen_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
    unsigned int offset;

		void run()
		{
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
				int last_frame;
				do{
					int id_out=fifo_src_in->pop(stop);/** get new frame for output*/
					if(id_out<0)break;
					//std::cout << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "id_out=" << id_out << " frames_src=" << frames_src << " frames_src[id_out]=" << frames_src[id_out] << std::endl;
					frames_src[id_out]->clear();
					
					int id_in=fifo_snk_in->pop(stop);/** get new frame for processing*/
					if(id_in<0)break;
					//frames_src[id_out]->load_packed_uint32_t((uint32_t*)&(frames_snk[id_in]->data()[4]));/** convert data */
                    frames_src[id_out]->load_packed<uint32_t>((uint32_t*)&(frames_snk[id_in]->data()[offset]));
					last_frame=frames_snk[id_in]->get_is_last();
					frames_src[id_out]->set_id(frames_snk[id_in]->get_id());
					frames_src[id_out]->set_is_last(last_frame);
					fifo_snk_out->push(id_in);
					fifo_src_out->push(id_out);
					frames_processed++;
				}while((last_frame==0)&&(*stop==false));

				qDebug() << parent->objectName() << " DONE";
			}
		}
		
	public:
    we_frame_gen_thread(QObject *parent) : worker_thread(parent){
      offset=1;
    }

		~we_frame_gen_thread() {}

    SET(offset);
};

class we_frame_gen : public we_sink_source
{ 
	Q_OBJECT 
	
	private:
		
	public:
		we_frame_gen(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			delete thread;
			thread = new we_frame_gen_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

		}
			
		~we_frame_gen() {}

    int configure(const QSettings& settings)
    {
      unsigned int offset = settings.value("offset",1).toInt();
      ((we_frame_gen_thread*)thread)->set_offset(offset);
      return 0;
    }
		
};
#endif
