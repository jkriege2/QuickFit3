/*!  
	\file we_bridge_outlet.h
	\brief implements we_bridge_outlet
	\author Jan Buchholz (to be blamed)
 */

/*!  
	\class we_bridge_outlet
	\brief	BRIDGE OUTLET
	
	This is a normal bridge that waits until a a distinct amount of free buffers are available.
	If the buffers are available, the are filled continousely.
	Waiting for free buffers is non-blocking.
	With amount set to one, this is equally to we_copy_nb


	\section we_bridge_outlet_io I/O ports
		THIS IS A SINK/SOURCE
		\verbatim
		-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
		               | # |
		<- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
		\endverbatim
	
	\section we_bridge_outlet_config Config file options
		amount: count of frames to be transmitted consequtive
*/


#ifndef __WE_BRIDGE_OUTLET_H__
#define __WE_BRIDGE_OUTLET_H__

#include "we_sink_source.h"
#include "worker_thread.h"


class we_bridge_outlet_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		unsigned int amount; /**> number of free buffers we are waiting for */
		
    /** \copydoc worker_thread::run() */
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
			int last=0;
			do
			{
				int frames_left=amount;/**< number of frames that have to be transmitted */
				int id_in=fifo_snk_in->pop(stop);/**< get new frame for processing*/
				if(id_in<0)break;
				int id_out;
				
				last_frame=frames_snk[id_in]->get_is_last();

				if(fifo_src_in->pop_nb(&id_out,frames_left))/**< get new frame for output, non blocking*/
				{
					/** ok, we have the buffers available */
					(*frames_src[id_out])=(*frames_snk[id_in]);
					//if((frames_snk[id_in]->get_id()-last)>0)
					//	std::cerr << "DROPPED: " << frames_snk[id_in]->get_id()-last << std::endl;
					last=frames_snk[id_in]->get_id()+1;
					fifo_src_out->push(id_out);
					fifo_snk_out->push(id_in);
					frames_left--;
					/** copy frames blocking */
					while((frames_left>0)&&(last_frame==0)&&(*stop==false))
					{
						id_in=fifo_snk_in->pop(stop);
						if(id_in<0)break;
						id_out=fifo_src_in->pop(stop);
						if(id_out<0)break;
						(*frames_src[id_out])=(*frames_snk[id_in]);
						//std::cerr << "A" << frames_snk[id_in]->get_id() << std::endl;
						
						last=frames_snk[id_in]->get_id()+1;

						
						last_frame=frames_snk[id_in]->get_is_last();
						fifo_src_out->push(id_out);
						fifo_snk_out->push(id_in);
						frames_left--;
					}
					frames_processed++;
				}
				else
				{
					/** buffers are not aVAILABLE */
					fifo_snk_out->push(id_in);/**< release frame */
//					if(last_frame!=0)
//					{
						 /** blocking last_frame signal transmission! */
//						id_out=fifo_src_in->pop();
//						frames_src[id_out]->set_is_last(last_frame);
//						fifo_src_out->push(id_out);
//					}
				}
			}while((last_frame==0)&&(*stop==false));
			std::cerr << qPrintable(parent->objectName()) << ": DONE." << std::endl;
		}
	public:
		we_bridge_outlet_thread(QObject *parent) : worker_thread(parent)
		{
			amount=1;
		}
		SET(amount)

		
};

class we_bridge_outlet : public we_sink_source
{ 
	Q_OBJECT 
	
	public:
		we_bridge_outlet(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			delete thread;
			thread = new we_bridge_outlet_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

			/** setup properties inheritance */
			intf_out[1]->set_inherit(intf_in[0]);
			
		}
		
        int configure(const QSettings& settings)
		{	
            ((we_bridge_outlet_thread*)thread)->set_amount(settings.value("amount",1).toInt());
			return 0;
		}
		
};
#endif
