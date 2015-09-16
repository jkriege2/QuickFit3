#ifndef __WE_READER_RAW_H__
#define __WE_READER_RAW_H__

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <QString>
#include <unistd.h> //usleep

#include "worker_element.h"
#include "qqueue.h"
#include "frame.h"
#include "helper.h"

#define FRAMES_COUNT (256)
#define FRAMES_SIZE (1024)

#include <sys/types.h>


#define wet_debug(output) std::cerr << qPrintable(parent->objectName()) << ":" << output << std::endl;

class we_reader_raw_thread : public worker_thread
{  
	Q_OBJECT

	protected:
		QString filename;
		
    void run()
			{
				/** setup envionment */
				worker_element *parent=(worker_element*)this->parent();
				qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
				qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
				frame<uint32_t> **frames = (parent->get_intf_out())[0]->get_frames();

				wet_pid
				
				int result;
				FILE * f=fopen(qPrintable(filename),"r");
				do
				{
					int id=fifo_in->pop(stop);
					if(id<0)break;
					
					result = fread(frames[id]->data(),1,frames[id]->get_size(),f);
					frames[id]->set_range(2);
					if(result==0)frames[id]->set_is_last(1);
					fifo_out->push(id);
				}while((result>0)&&(*stop==false));
				
				fclose(f);
				
				std::cerr << qPrintable(parent->objectName()) << ": DONE";
			}
	public:
		we_reader_raw_thread(QObject *parent) : worker_thread(parent){}
		~we_reader_raw_thread() {}
		GETSET(filename)
 };

class we_reader_raw : public worker_element
{ 
   Q_OBJECT 
   
	private:

   public:
		we_reader_raw(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : worker_element(parent,frame_count)
		{
			/** replace the thread */
			delete thread;
			thread = new we_reader_raw_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

			
			/** set interface properties */
			intf_out[0]->set_is_special(true); /** source */
			
			/** setup frames */
			frame<uint32_t> **frames=(frame<uint32_t> **)calloc(frame_count+1,sizeof(frame<uint32_t>*));
            for(unsigned int i=0;i<frame_count;i++)
			{
				frames[i]=(frame<uint32_t> *)new frame<uint32_t>(frame_size_x,frame_size_y);
			}
			frames[frame_count]=NULL;
			intf_out[0]->set_frames(frames);
		}
		
		~we_reader_raw() {}
		
        int configure(const QSettings& settings)
		{	
            QString filename = settings.value("filename").toString();
			((we_reader_raw_thread*)thread)->set_filename(filename);
			return 0;
		}
};
#endif
