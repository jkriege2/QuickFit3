#ifndef __WE_ENDPOINT_H__
#define __WE_ENDPOINT_H__

#include <QWidget>
#include <QThread>
#include <QDebug>

#include "worker_element.h"
#include "worker_thread.h"

#include "../../../../LIB/trunk/qt/qqueue.h"
#include "../../../../LIB/trunk/frame.h"

#include <unistd.h>

class we_endpoint_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		void run(){
			do{
				sleep(2);
			}while(*stop==false);
		}
	public:
		we_endpoint_thread(QObject *parent) : worker_thread(parent){}
};

class we_endpoint : public worker_element
{ 
   Q_OBJECT 

	private:

   public:
		we_endpoint(QWidget *parent) : worker_element(parent,0)
		{
			delete thread; /** replace old thread with the new one declared above */
			thread = new we_endpoint_thread(this);//new QThread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

			intf_in[0]->set_is_special(true);/** we are a sink */
		}
		
		~we_endpoint() {}
		
		bool get_frame(uint32_t *buffer){
			bool result=true;
			qfifo *fifo_in =	(this->get_intf_in())[0]->get_fifo();
			qfifo *fifo_out =	(this->get_intf_out())[0]->get_fifo();
			frame<uint32_t> **frames = (this->get_intf_in())[0]->get_frames();
	
			int id;
            if(!fifo_in->pop_nb(&id)){
                return false;
            }
            if(buffer!=NULL){
                memcpy((void*)buffer,(void*)(frames[id]->data()),frames[id]->get_size());//effective?
            }

			fifo_out->push(id);
			return true;
		}
		unsigned int get_depth(){
			qfifo *fifo_in =	(this->get_intf_in())[0]->get_fifo();
			return fifo_in->get_depth();
		}
};
#endif
