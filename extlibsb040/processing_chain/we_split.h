/*
*	SPLIT
*	
*	This component passes frames, and creates a copied trunk.
*
*	THIS IS A SOURCE
*
*	-> FIFO_IN[0]  --+->   FIFO_OUT[0]
*	                 |
*	                 |  / FIFO_OUT[1]
*	                 * |
*	                    \ FIFO_IN[1]
*/

#ifndef __WE_SPLIT_H__
#define __WE_SPLIT_H__

#include "worker_thread.h"
#include "worker_element.h"



class we_split_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		void run()
		{
			/** setup envionment */
			worker_element *parent=(worker_element*)this->parent();
			qfifo *fifo_src_in =	(parent->get_intf_in())[0]->get_fifo();
			qfifo *fifo_src_out =	(parent->get_intf_out())[0]->get_fifo();
			qfifo *fifo_cpy_in =	(parent->get_intf_in())[1]->get_fifo();
			qfifo *fifo_cpy_out =	(parent->get_intf_out())[1]->get_fifo();
			frame<uint32_t> **frames_src = (parent->get_intf_in())[0]->get_frames();
			frame<uint32_t> **frames_cpy = (parent->get_intf_in())[1]->get_frames();
			
			wet_pid
			
			if((fifo_src_in==NULL)||(fifo_src_out==NULL)||(fifo_cpy_in==NULL)||(fifo_cpy_out==NULL)||(frames_src==NULL)||(frames_cpy==NULL))
			{
				CERR(fifo_src_in)
				CERR(fifo_src_out)
				CERR(fifo_cpy_in)
				CERR(fifo_cpy_out)
				CERR(frames_src)
				CERR(frames_cpy)
				exit();
			}	
			else
			{
				int last_frame;
				do
				{
					int id_src=fifo_src_in->pop(stop);
					if(id_src<0)break;
					int id_cpy=fifo_cpy_in->pop(stop);
					if(id_cpy<0)break;
					
					(*frames_cpy[id_cpy])=(*frames_src[id_src]);/** copy frame content */
					last_frame=frames_src[id_src]->get_is_last();
					fifo_cpy_out->push(id_cpy);
					fifo_src_out->push(id_src);
					frames_processed++;
				}while((last_frame==0)&&(*stop==false));
			}
			qDebug() << parent->objectName() << "DONE";
		}
	public:
		we_split_thread(QObject *parent) : worker_thread(parent){}

		
};

class we_split : public worker_element
{ 
	Q_OBJECT 
	
	public:
		we_split(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : worker_element(parent,frame_count)
		{
			/** delte old thread */
			delete thread;
			thread = new we_split_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

			
			/** add new interfaces */
			intf_in=expand<worker_interface>(intf_in,1);
			intf_out=expand<worker_interface>(intf_out,1);
			
			intf_in[1]=new worker_interface(this,false,frame_count);
			intf_out[1]=new worker_interface(this,true,frame_count);
			
			/** connect new interfaces */
			intf_in[1]->connect_next(intf_out[1]);
			
			/** set interface properties */
			intf_out[1]->set_is_special(true); /** source */
			
			/** setup properties inheritance */
			intf_out[1]->set_inherit(intf_in[0]);
			
			/** setup frames */
/*			frame<uint32_t> **frames=(frame<uint32_t> **)calloc(frame_count+1,sizeof(frame<uint32_t>*));
			for(int i=0;i<frame_count;i++)
			{
				frames[i]=(frame<uint32_t> *)new frame<uint32_t>(frame_size_x,frame_size_y);
			}
			frames[frame_count]=NULL;
			intf_out[1]->set_frames(frames);*/
		}
			
		~we_split() {}
		
};
#endif
