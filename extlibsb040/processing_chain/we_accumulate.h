/*!  
	\file we_accumulate.h
	\brief implements we_accumulate
	\author Jan Buchholz (to be blamed)
 */

/*!  
	\class we_accumulate
	\brief	ACCUMULATE
	
	This WE accumulates the frames arriving on the input and
	outputs the result.

	\section we_accumulate I/O ports
		THIS IS A SINK/SOURCE
		\verbatim
		-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
		               | # |
		<- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
		\endverbatim
	
	\section we_accumulate_config Config file options
		count: number of frames to be accumulated
*/

/*
*	Accumulate Frames
*	
*	THIS IS A SINK/SOURCE
*
*	-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
*                | # |
* <- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
*/

#ifndef __WE_ACCUMULATE_H__
#define __WE_ACCUMULATE_H__

#include "we_sink_source.h"
#include "worker_thread.h"

#include "../../../../LIB/trunk/frame.h"

//TODO: count and size of frames

class we_accumulate_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		int count;
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
				int last_frame=0;
				do
				{
					int id_out=fifo_src_in->pop(stop);/** get new frame for output*/
					if(id_out<0)break;
					frames_src[id_out]->clear();
					do
					{
						int id_in=fifo_snk_in->pop(stop);/** get new frame for accumulation*/
						if(id_in<0)break;
						last_frame=frames_snk[id_in]->get_is_last();
						(*frames_src[id_out])+=(*frames_snk[id_in]);
						fifo_snk_out->push(id_in);
					}while(((*frames_src[id_out]).get_accumulation_count()<count)&&(last_frame==0)&&(*stop==false));
					frames_src[id_out]->set_is_last(last_frame);
					fifo_src_out->push(id_out);/** frame for next we available */
					frames_processed++;
				}while((last_frame==0)&&(*stop==false));
			}
			qDebug() << parent->objectName() << " DONE";
		}
	public:
		we_accumulate_thread(QObject *parent) : worker_thread(parent){}
		GETSET(count)

		
};

class we_accumulate : public we_sink_source
{ 
	Q_OBJECT 
	
	public:
		we_accumulate(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			delete thread;
			thread = new we_accumulate_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

			intf_out[1]->set_inherit(intf_in[0]);
		}
		
        int configure(const QSettings& settings)
		{	
            int count = settings.value("count").toInt();
			((we_accumulate_thread*)thread)->set_count(count-1);/**< acc_count=x means 0..(x+1), so its kind of maximum now */
			return 0;
		}
		
};
#endif
