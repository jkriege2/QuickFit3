/*
*	Frame Analyzer
*	
*	This is the second component in the working chain. It takes raw data and assembles frames.
*	It can be used with a raw file reader or with usb acquisition
*	THIS IS A SINK/SOURCE
*	It should be capable of accepting dynmaic frame sizes
* It emits only correct raw data frames
*
*	-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
*                | # |
* <- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
*/

#ifndef __WE_FRAME_ANALYZER_H__
#define __WE_FRAME_ANALYZER_H__

#include <QWidget>
#include <QThread>
#include <QDebug>

#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal

#include "worker_thread.h"
#include "we_sink_source.h"

#include "qqueue.h"
#include "frame.h"
#include "helper.h"
#include "textcolor.h"

class we_frame_analyzer_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		unsigned int frame_size;
		
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
				int id_in[2];//ids of buffers
				int id_out;//id of current frame
				unsigned char *frame;
				
				unsigned int pos=0; /** next byte to read in current data buffer*/
				int buffer_count=0,candidate_found=0,error_count=0,error_frames=0;
				unsigned int id_expected=0;
				
				boost::crc_optimal<16, 0x1021, 0xFFFF, 0, true, false>  crc_ccitt1;
				
				id_in[0]=(fifo_snk_in)->pop(stop);//get first buffer
				id_in[1]=(fifo_snk_in)->pop(stop);//get second buffer
				id_out=(fifo_src_in)->pop(stop);//get final frame buffer

				std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "parent=" << parent << ", id_in[0]=" << id_in[0]  << ", id_in[1]=" << id_in[1]  << ", id_out=" << id_out << std::endl;
                std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "frame_src=" << frames_src << std::endl;
				if((id_in[0]>=0)&&(id_in[1]>=0)&&(id_out>=0)){
					frame=(unsigned char*) (frames_src[id_out])->data();//pointer to final frame data
					do
					{
						candidate_found=0;
						do
						{
							if(pos>=frames_snk[id_in[0]]->get_size_effective())
							{
								
								unsigned int tmp=id_in[0];
								
								pos=pos%frames_snk[id_in[0]]->get_size_effective();
								
								id_in[0]=id_in[1];
								id_in[1]=tmp;
								
								fifo_snk_out->push(id_in[1]);
								id_in[1]=fifo_snk_in->pop(stop);
								if(id_in[1]<0)break;
								if(frames_snk[id_in[1]]->get_is_last()==1)break;
								buffer_count++;
								
							}
							//printf("%2.2x\n",(unsigned int) buffer_usb[id_in[0]].data[pos]);
							if(((unsigned char*)frames_snk[id_in[0]]->data())[pos]==(unsigned char)0xff)
							{
								candidate_found=1;
							}
							else
							{
								pos++;
							}
						}while((candidate_found==0)&&(*stop==false));
						 // 			printf("cand: %i\n",pos);
						if((candidate_found==1)&&(*stop==false))
						{
							if((frames_snk[id_in[0]]->get_size_effective()-pos)<this->frame_size)
							{
								memcpy (&frame[0],&(((unsigned char*)frames_snk[id_in[0]]->data())[pos]),frames_snk[id_in[0]]->get_size_effective()-pos);
								memcpy (&frame[frames_snk[id_in[0]]->get_size_effective()-pos],&(((unsigned char*)frames_snk[id_in[1]]->data())[0]),this->frame_size-(frames_snk[id_in[0]]->get_size_effective()-pos));
							}
							else
								memcpy (frame,&((unsigned char*)frames_snk[id_in[0]]->data())[pos],this->frame_size);
							
							crc_ccitt1.process_bytes(frame,this->frame_size-2);
							uint16_t crc0=*((uint16_t*)&frame[this->frame_size-2]);
							int crc1=crc_ccitt1.checksum();
							crc_ccitt1.reset();
							if(crc0==crc1)
							{
								//  				printf("%i %8.8x CRC OK! (%4.4x)\n",buffer_count,pos,(unsigned int)crc0);
								unsigned int id=*((uint32_t*)frame)>>8;
								if(id_expected!=id)
								{
									error_frames+=id-id_expected;
                                    //textcolor(RESET,YELLOW,BLACK);
                                    //emit log_wrn(tr("%1 frames missing: %2 - %3 (%4)\n").arg(id-id_expected).arg(id_expected,8,16).arg(id-1,8,16).arg(error_frames));
                                    //printf("%d %d %d\n",id_expected,id-id_expected,error_frames);
									id_expected=id;
								}
								id_expected++;
								
								pos=(pos+this->frame_size);
								frames_src[id_out]->set_id(id);
								fifo_src_out->push(id_out);
								frames_processed++;
								id_out=fifo_src_in->pop(stop);
								if(id_out<0)break;
								frame=(unsigned char*)frames_src[id_out]->data();
							}
							else
							{
                                //textcolor(RESET,YELLOW,BLACK);
								//printf("[id=%6.6x] %i %8.8x CRC FAILED! (%4.4x <> %4.4x)\n",id_expected,buffer_count,pos,(unsigned int)crc0,(unsigned int)crc1);
								error_count++;
								pos++;
							}
						}
					}while((candidate_found==1)&&(*stop==false));
				}
				/** clean up */
				if(id_out>=0)fifo_src_out->push(id_out);
				if(id_in[0]>=0)fifo_snk_out->push(id_in[0]);
				if(id_in[1]>=0)fifo_snk_out->push(id_in[1]);
			}
			qDebug() << parent->objectName() << " DONE";
		}
		
	public:
		we_frame_analyzer_thread(QObject *parent) : worker_thread(parent)
		{
			this->frame_size=134;
		}
		
		virtual ~we_frame_analyzer_thread() {};
		SET(frame_size);
		
};

class we_frame_analyzer : public we_sink_source
{ 
	Q_OBJECT 
	
	private:
		
	public:
		we_frame_analyzer(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			thread = new we_frame_analyzer_thread(this);
			connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
			connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
			connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
			connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));


            std::cerr << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << "WE_FRAME_ANALYZER: intf_out[1]->get_frames()=" << intf_out[1]->get_frames() << std::endl;
		}

		~we_frame_analyzer(){}
		
        int configure(const QSettings& settings)
		{	
            int frame_size = settings.value("frame_size").toInt();
			((we_frame_analyzer_thread*)thread)->set_frame_size(frame_size);
			return 0;
		}
};
#endif
