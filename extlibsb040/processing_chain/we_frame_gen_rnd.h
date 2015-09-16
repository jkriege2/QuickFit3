/**	\brief Random Frame Generator
 *  
 *	This class generates random frames
 *	TODO: depth, speed, 
 */

#ifndef __WE_FRAME_GEN_RND_H__
#define __WE_FRAME_GEN_RND_H__

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "worker_thread.h"
#include "worker_element.h"
#include "qqueue.h"
#include "frame.h"
#include "helper.h"
#include "prng.h"

#define EP_OUT			4
#define EP_IN			8
#define ID_VENDOR		0x0547
#define ID_PRODUCT		0x1002
#define USB_TIMEOUT		4096
#define MAX_USB_PACKET_SIZE	512

#define wet_debug(output) std::cerr << qPrintable(parent->objectName()) << ":" << #output << "=" << output << std::endl;



class we_frame_gen_rnd_thread : public worker_thread
{  
	Q_OBJECT

	protected:
		unsigned int depth;
		unsigned int count,frame_count;
		unsigned int delay;
		prng *rnd;
		
    void run()
			{
				/** setup envionment */
				worker_element *parent=(worker_element*)this->parent();
				qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
				qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
				frame<uint32_t> **frames = (parent->get_intf_out())[0]->get_frames();

				rnd=new prng_TT800();
				//rnd=new prng_mersenne_twister();
				//rnd=new prng_simple();
				
				#ifdef __linux__
                //std::cerr << qPrintable(parent->objectName()) << ": pid=" << getpid() << std::endl;/**< prints the current PID, so you can find it in top -H */
				#endif
				wet_debug(depth)
				wet_debug(count)
				wet_debug(frame_count)
				do
				{
					int id=fifo_in->pop(stop);
					if(id<0)break;
					
					int xres=frames[id]->get_xres();
					int yres=frames[id]->get_yres();
					for (unsigned int x=0; x<xres; x++)
					{
						for (unsigned int y=0; y<yres; y++)
						{
							frames[id]->set_pixel(x,y,(rnd->get_rand() % depth));
						}
					}
					if(count==frame_count)frames[id]->set_is_last(1);
					frames[id]->set_id(count);
					fifo_out->push(id);
					count++;
					/*usleep(delay);*/
					frames_processed++;
				}while((count<=frame_count)&&(*stop==false));
				wet_debug(depth)
				wet_debug(count)
				wet_debug(frame_count)

				std::cerr << qPrintable(parent->objectName()) << ": DONE" << std::endl;
			}
	public:
		we_frame_gen_rnd_thread(QObject *parent) : worker_thread(parent)
		{
			depth=1;
			count=0;
			frame_count=0;
			delay=0;
		}
		~we_frame_gen_rnd_thread() {}
		SET(depth)
		SET(frame_count)
		SET(delay)
		

 };

class we_frame_gen_rnd : public worker_element
{ 
   Q_OBJECT 
   
	private:

   public:
		we_frame_gen_rnd(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : worker_element(parent,frame_count)
		{
			/** replace the thread */
			delete thread;
			thread = new we_frame_gen_rnd_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));

			
			/** set interface properties */
			intf_out[0]->set_is_special(true); /** source */
			
			/** setup frames */
			frame<uint32_t> **frames=(frame<uint32_t> **)calloc(frame_count+1,sizeof(frame<uint32_t>*));
			for(int i=0;i<frame_count;i++)
			{
				frames[i]=(frame<uint32_t> *)new frame<uint32_t>(frame_size_x,frame_size_y);
			}
			frames[frame_count]=NULL;
			intf_out[0]->set_frames(frames);
		}
		
		~we_frame_gen_rnd() {}
		
        int configure(const QSettings& settings)
		{	
            ((we_frame_gen_rnd_thread*)thread)->set_depth(settings.value("depth",1).toInt());
            ((we_frame_gen_rnd_thread*)thread)->set_frame_count(settings.value("frames",0).toInt());
            ((we_frame_gen_rnd_thread*)thread)->set_delay(settings.value("delay",0).toInt());
			return 0;
		}
};
#endif
