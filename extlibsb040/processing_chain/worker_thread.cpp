#include "worker_thread.h"
#include "worker_element.h"

worker_thread::worker_thread(QObject *parent):QThread(parent){
	frames_processed=0;
	stop=((worker_element*)parent)->get_breakCondition();
}


void worker_thread::run()
{
	/** setup envionment */
	worker_element *parent=(worker_element*)this->parent();
	qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
	qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
	frame<uint32_t> **frames = (parent->get_intf_in())[0]->get_frames();
	
	wet_pid
	
	int last_frame;
	do
	{
		int id=fifo_in->pop();
		/** do something with frame[id] */
		last_frame=frames[id]->get_is_last();
		fifo_out->push(id);
	}while((last_frame==0)&&(*stop==false));
//	qDebug() << parent->objectName() << "DONE";

}
