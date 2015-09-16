#ifndef __WE_CORRELATOR_HW_H__
#define __WE_CORRELATOR_HW_H__

#include "worker_element.h"
#include "lut.h"
#include "helper.h"
#include "correlator_multitau.h"
#include "textcolor.h"
#include <iomanip>

const unsigned int b=14;
const unsigned int l=8;

class we_correlator_hw_thread : public worker_thread
{  
	Q_OBJECT
    protected:
        unsigned int height;//TODO
        uint64_t **hwc_dump;
        double **hwcResult;
        unsigned int *taus;
        double *tausD;

        void updateData(char* frameData){

            uint32_t id=((uint32_t*)frameData)[0]>>8;
            uint32_t *src=(uint32_t*)&frameData[8];
            uint32_t ticks=*((uint32_t*)&frameData[4])*1024;


            for(int x=0;x<32;x++){//a single frame contains 32 correlator results
                uint32_t *dst=(uint32_t*)hwc_dump[(id%(height))*32+x];
                for(int i=0;i<128;i++){//128 data words per correlator
                    if((*dst>*src)&&(i<112)){
                        //std::cerr << color(RESET,YELLOW,BLACK);
                        //std::cerr << "Possible overflow detected @ (" << id%height << "," << x << ")." << i << " d=" << *dst << "> s=" << *src << " (" << std::hex << ((id<<8) | 0xff) << std::dec << ")";
                        //std::cerr << color(RESET,WHITE,BLACK) << std::endl;
                        //dst[1]++;//overflow emulation
                    }
                    *dst=*src;
                    dst+=2;
                    src++;
                }
                src+=128; //skip delay registers
                correlatorjb<int,int>::normalize(hwc_dump[(id%(height))*32+x],ticks,b,l,hwcResult[(id%(height))*32+x]);
            }
		}
		
		void run()
		{
			/** setup envionment */
			worker_element *parent=(worker_element*)this->parent();
			qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
			qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
			frame<uint32_t> **frames = (parent->get_intf_in())[0]->get_frames();

            hwc_dump = (uint64_t **)malloc((height)*32 * sizeof(uint64_t*));
            for(int i=0;i<(height)*32;i++){
                hwc_dump[i]=(uint64_t*)calloc(128,sizeof(uint64_t)*2);
            }
            //hwcResult = (double **)malloc((height)*32 * sizeof(double*));
            //for(int i=0;i<(height)*32;i++){
            //    hwcResult[i]=(double*)calloc(14*8,sizeof(double));
            //}
            taus = static_cast<unsigned int *>(malloc(b*l*sizeof(unsigned int)));
            //tausD = static_cast<double *>(malloc(b*l*sizeof(double)));
            correlatorjb<int,int>::getTaus(b,l,taus);
            for(int i=0;i<b*l;i++){
                tausD[i]=taus[i];
            }

			
			int last_frame;
            qDebug() << parent->objectName() << " Starting...";
			do
			{
				int id=fifo_in->pop();
				
                updateData(frames[id]->dataC());
				last_frame=frames[id]->get_is_last();
				fifo_out->push(id);
                //qDebug()<<__FILE__<<" @ "<<__LINE__<<": HW-COR frame acquired ("<<")";
                emit(newData());
				frames_processed++;
			}while((last_frame==0)&&(*stop==false));
			qDebug() << parent->objectName() << " DONE";
		}

	public:
        we_correlator_hw_thread(QObject *parent) : worker_thread(parent), height(32){
        }

        ~we_correlator_hw_thread() {}
        SET(height);
        GETSET(hwcResult);
        GETSET(tausD);

	signals:
        void newData();
};

class we_correlator_hw : public worker_element
{ 
	Q_OBJECT 
	
	private:
		
	public:
		we_correlator_hw(QWidget *parent, unsigned int fifo_depth) : worker_element(parent,fifo_depth)
		{
			/** replace the thread */			
			delete thread;
			thread = new we_correlator_hw_thread(this);

			/** set interface properties */
			intf_in[0]->set_is_special(true); /** sink */
            connect((we_correlator_hw_thread*)thread, SIGNAL(newData()), this, SLOT(newDataAvailable()));

		}
		
        ~we_correlator_hw() {};
		
    int configure(const QSettings& settings)
		{	
            int height = settings.value("height",32).toInt();
            ((we_correlator_hw_thread*)thread)->set_height(height);
			return 0;
		}

    double **getResults(){return ((we_correlator_hw_thread*)thread)->get_hwcResult();};
    double *getTaus(){return ((we_correlator_hw_thread*)thread)->get_tausD();};

    void setData(double **data){((we_correlator_hw_thread*)thread)->set_hwcResult(data);};
    void setTaus(double *taus){((we_correlator_hw_thread*)thread)->set_tausD(taus);};


signals:
    void newData();
protected slots:
    void newDataAvailable(){emit newData();};
		
};



#endif
