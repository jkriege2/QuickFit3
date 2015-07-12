/*
*	COPY Non-Blocking
*	
*	This component copies data from one side to the other. Frames are Added to queue in non-blocking way
*
*	THIS IS A SINK/SOURCE
*
*	-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
*                | # |
* <- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
*/

#ifndef __WE_COPY_NB_H__
#define __WE_COPY_NB_H__

#include "we_sink_source.h"
#include "worker_thread.h"

class we_copy_nb_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
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
				
				int id_in=fifo_snk_in->pop();/** get new frame for processing*/
				if(fifo_src_in->pop_nb(&id_out))/** get new frame for output, non blocking*/
				{
					(*frames_src[id_out])=(*frames_snk[id_in]);
					fifo_src_out->push(id_out);
				}
				last_frame=frames_snk[id_in]->get_is_last();
				fifo_snk_out->push(id_in);
			}while((last_frame==0)&&(*stop==false));
			/** blocking signal transmission! */
			id_out=fifo_src_in->pop();
			frames_src[id_out]->set_is_last(last_frame);
			fifo_src_out->push(id_out);
			
			qDebug() << parent->objectName() << " DONE";
		}
	public:
		we_copy_nb_thread(QObject *parent) : worker_thread(parent){}

		
};

class we_copy_nb : public we_sink_source
{ 
	Q_OBJECT 
	
	public:
		we_copy_nb(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			delete thread;
			thread = new we_copy_nb_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

		}
		
};
#endif
