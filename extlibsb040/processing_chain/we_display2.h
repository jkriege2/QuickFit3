#ifndef __WE_DISPLAY2_H__
#define __WE_DISPLAY2_H__

#include "worker_element.h"
#include "../../../../LIB/trunk/lut.h"
#include "../../../../LIB/trunk/helper.h"
#include "../../../../LIB/trunk/histogram.h"

#include <QPixmap>
#include <QPainter>

class we_display2_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		lookuptable *lut;
		void run()
		{
			/** setup envionment */
			worker_element *parent=(worker_element*)this->parent();
			qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
			qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
			frame<uint32_t> **frames = (parent->get_intf_in())[0]->get_frames();
			
			wet_pid

			
			int xsize=512,ysize=512,xres=32,yres=32;
			int i=0;
			QImage myQImage(xsize, ysize, QImage::Format_RGB32);
			QPainter painter(&myQImage);
			
			int last_frame;
			
			do
			{
				int id=fifo_in->pop();
				
				histogram<uint32_t> h((uint32_t*)(frames[id])->get_pixels(),1024);
        h.sort();
        float mean=h.get_mean2(0.0,5.0);
        float dev=h.get_deviation2(0.0,5.0);
        float min=mean-2.0*dev;
        if(min<0.0)min=0.0;
        float max=mean+2.0*dev;
        if(max>(float)frames[id]->get_accumulation_count())max=(float)(frames[id])->get_accumulation_count();
				last_frame=frames[id]->get_is_last();
                unsigned int xmax=frames[id]->get_xres();
                unsigned int ymax=frames[id]->get_yres();
				for (unsigned int x=0; x<xmax; x++)
				{
					for (unsigned int y=0; y<ymax; y++)
					{
						float value=(float)(frames[id]->get_pixel(x,y));
              
						if(value<min)value=min;
						if(value>max)value=max;
                
						float valueF=((float)value-min)/(max-min);
                
						QColor color;
						float r=sqrt(valueF);
						float g=pow(valueF,3);
						float b=sin(valueF*3.14159265*2);
						if(r<0.0)r=-r;
						if(g<0.0)g=-g;
						if(b<0.0)b=-b;
						color.setRgbF(valueF,valueF,valueF);
						painter.setPen(color);
						painter.fillRect(x*xsize/xres, y*ysize/yres,xsize/xres,ysize/yres,color);
					}
				}

				fifo_out->push(id);
				//std::cerr << "[" << __FUNCTION__ << "] " << fifo_in->get_depth() << std::endl;
				emit theImage(myQImage);
				frames_processed++;
				usleep(50000);
				i++;
			}while((last_frame==0)&&(*stop==false));
			qDebug() << parent->objectName() << " DONE";
		}
	public:
		we_display2_thread(QObject *parent) : worker_thread(parent){}
		~we_display2_thread() {}
		GETSET(lut)
		
	signals:
		void theImage(const QImage &);
};

class we_display2 : public worker_element
{ 
	Q_OBJECT 
	
	private:
		
	public:
		QPixmap plot;
		
		we_display2(QWidget *parent, unsigned int fifo_depth) : worker_element(parent,fifo_depth)
		{
			/** replace the thread */			
			delete thread;
			thread = new we_display2_thread(this);

			/** set interface properties */
			intf_in[0]->set_is_special(true); /** sink */
			
			setFixedSize(600, 600);
			setWindowTitle("Display");
			connect(thread, SIGNAL(theImage(const QImage &)),this, SLOT(displayImage(const QImage &)));
			show();
		}
		
		void paintEvent(QPaintEvent *)
		{	
			QPainter painter(this);
			painter.drawPixmap(0, 0, plot);
		}
		
		~we_display2() {};
		
        int configure(const QSettings& settings)
		{	
			lookuptable *lut;
            QString lut_type = settings.value("lut/type").toString();
            int lut_range = settings.value("lut/range").toInt();
            int r = settings.value("lut/r").toInt();
            int g = settings.value("lut/g").toInt();
            int b = settings.value("lut/b").toInt();
			
			if(lut_type=="gnuplot")
			{
				int rgbformulae[3];
				lut=new lookuptable(3,lut_range);

				rgbformulae[0]=r;
				rgbformulae[1]=g;
				rgbformulae[2]=b;
				
				lut->table_fill_rgbformulae(rgbformulae);
			}
			((we_display2_thread*)thread)->set_lut(lut);
			return 0;
		}
		
	public slots:
		void displayImage(const QImage &image)
		{
			plot = QPixmap::fromImage(image);
			//show();
			update();
			raise();
			//activateWindow();
		}	
		
		
};



#endif
