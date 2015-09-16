/*
*	Select region of interest
*	
*	THIS IS A SINK/SOURCE
*
*	-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
*                | # |
* <- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
*/

#ifndef __WE_ROI_H__
#define __WE_ROI_H__

#include "we_sink_source.h"
#include "worker_thread.h"
#include "helper.h"

class we_roi_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		unsigned int offset_x,offset_y;
		
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

			wet_pid

			
			int last_frame=0;
			int id_out;

			do
			{
				
				int id_in=fifo_snk_in->pop();/**< get new frame for processing*/
				id_out=fifo_src_in->pop();/**< get new output frame */
				
				//std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << id_in << "," << id_out << "," << std::endl;
				
				if(	((offset_x+frames_src[id_out]->get_xres())<=frames_snk[id_in]->get_xres())&&
						((offset_y+frames_src[id_out]->get_yres())<=frames_snk[id_in]->get_yres()))
					for(unsigned int y=0;y<frames_src[id_out]->get_yres();y++)
					{
						for(unsigned int x=0;x<frames_src[id_out]->get_xres();x++)
						{
							frames_src[id_out]->set_pixel(x,y,frames_snk[id_in]->get_pixel(offset_x+x,offset_y+y));
						}
					}

				last_frame=frames_snk[id_in]->get_is_last();
				frames_src[id_out]->set_is_last(last_frame);
				frames_src[id_out]->set_id(frames_snk[id_in]->get_id());
				fifo_src_out->push(id_out);
				fifo_snk_out->push(id_in);
			}while((last_frame==0)&&(*stop==false));
			
			qDebug() << parent->objectName() << " DONE";
		}
	public:
		we_roi_thread(QObject *parent) : worker_thread(parent)
		{
			offset_x=0;
			offset_y=0;
		}
		SET(offset_x)
		SET(offset_y)
		

		
};

class we_roi : public we_sink_source
{ 
	Q_OBJECT 
	
	public:
		we_roi(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			thread = new we_roi_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

		}
		
        int configure(const QSettings& settings)

		{	
            ((we_roi_thread*)thread)->set_offset_x(settings.value("offset_x",0).toInt());
            ((we_roi_thread*)thread)->set_offset_y(settings.value("offset_y",0).toInt());
			return 0;
		}

		
};
#endif
