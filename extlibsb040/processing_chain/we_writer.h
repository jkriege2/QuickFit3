#ifndef __WE_WRITER_H__
#define __WE_WRITER_H__

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <unistd.h> //usleep

#include "worker_element.h"
#include "worker_thread.h"

#include "qqueue.h"
#include "frame.h"

class we_writer_thread : public worker_thread
{  
	Q_OBJECT

	protected:
		QString filename;
		int duration;
		
    void run()
			{
        qDebug()<<__FILE__ << __LINE__ <<" duration="<<duration;
				/** setup envionment */
				worker_element *parent=(worker_element*)this->parent();
				qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
				qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
				frame<uint32_t> **frames = (parent->get_intf_in())[0]->get_frames();
				
				int last_frame; /** last frame in chain? */

				QFile file(filename);
                                int count = 0;
                if (file.open(QIODevice::WriteOnly)){
				
				std::cerr << __FUNCTION__ << "[" << __LINE__ << "]" << "WARNING! Bad Size HACK!!!" << std::endl;
				

				int id;
				do
				{
					id=fifo_in->pop(stop);
					if(id<0)break;
					file.write((char*) frames[id]->data(),frames[id]->get_xres());
					//printf("Write %i\n",frames[id]->get_xres());
					last_frame=frames[id]->get_is_last();
					fifo_out->push(id);
					frames_processed++;
					count++;
				}while((last_frame==0)&&(count<=duration)&&(*stop==false));
				std::cerr << "WRITER DONE" << last_frame << ":" << count << ":" << duration << ":" << *stop << ":" << id<< std::endl;
}else{
                    qDebug()<< __FILE__ << __LINE__ << "Failed to open file:"<<filename;
                }
                file.close();
				log_txt(parent->objectName()+": thread finished.");
                qDebug()<<__FILE__ << __LINE__ <<" write count="<<count;
			}
	public:
		we_writer_thread(QObject *parent) : worker_thread(parent)	{}
		~we_writer_thread() {}
		SET(filename)
		SET(duration)
 };

class we_writer : public worker_element
{ 
   Q_OBJECT 

	private:

   public:
		we_writer(QWidget *parent) : worker_element(parent,0)
		{
			delete thread; /** replace old thread with the new one declared above */
			
			intf_in[0]->set_is_special(true);/** we are a sink */

			thread = new we_writer_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

		}
		
        int configure(const QSettings& settings)
		{	
            QString filename = settings.value("filename").toString();
            int duration=settings.value("duration").toInt();
			((we_writer_thread*)thread)->set_filename(filename);
			((we_writer_thread*)thread)->set_duration(duration);
			return 0;
		}
		
		~we_writer() {}
};
#endif
