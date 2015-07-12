#ifndef __WE_DISPLAY_H__
#define __WE_DISPLAY_H__

#include "worker_element.h"
#include "../../../../LIB/trunk/lut.h"
#include "../../../../LIB/trunk/helper.h"

#include <QPixmap>
#include <QPainter>

class we_display_thread : public worker_thread
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

			
			int xsize=1024,ysize=256,xres=32,yres=32;
			int i=0;
			QImage myQImage(xsize+2, ysize+2, QImage::Format_RGB32);
			QPainter painter(&myQImage);
			
			int last_frame;
			
			do
			{
				int id=fifo_in->pop();
				last_frame=frames[id]->get_is_last();
				int xmax=frames[id]->get_xres();
				int ymax=frames[id]->get_yres();
				for (unsigned int x=0; x<frames[id]->get_xres(); x++)
				{
					for (unsigned int y=0; y<frames[id]->get_yres(); y++)
					{
						unsigned int value=frames[id]->get_pixel(x,y);
						/*TODO: only for grayscale!*/
						QColor color;
						/*                rgb_value=value<<16 | value<<8 | value;
						color.setRgb((QRgb)rgb_value);*/
						//color.setHsvF((float)value/((float)(q->buffers[buffer])->acc_count()),1.0,1.0);
						
						//float gray=(float)value/((float)(frames[id]->get_accumulation_count()+1));
						
						color.setRgb(lut->lookup(0,value),lut->lookup(1,value),lut->lookup(2,value));
						painter.setPen(color);
						painter.fillRect(x*xsize/xmax+1, y*ysize/ymax+1,xsize/xmax,ysize/ymax,color);
					}
				}
				//print 'axis'
				QColor c;
				for (unsigned int x=0; x<frames[id]->get_xres(); x++){
					c.setRgb(0xFFFFFF*(x&0x1));
					painter.setPen(c);
					painter.fillRect(x*xsize/xmax+1,0,      xsize/xmax,1,c);
					painter.fillRect(x*xsize/xmax+1,ysize+1,xsize/xmax,1,c);
				}
				for (unsigned int y=0; y<frames[id]->get_yres(); y++){
					c.setRgb(0xFFFFFF*(y&0x1));
					painter.setPen(c);
					painter.fillRect(0,      y*ysize/ymax+1,1,ysize/ymax,c);
					painter.fillRect(xsize+1,y*ysize/ymax+1,1,ysize/ymax,c);
				}
				
				fifo_out->push(id);
				//std::cerr << "[" << __FUNCTION__ << "] " << fifo_in->get_depth() << std::endl;
				emit theImage(myQImage);
				frames_processed++;
				usleep(50000);
				i++;
			}while(last_frame==0);
			qDebug() << parent->objectName() << " DONE";
		}
	public:
		we_display_thread(QObject *parent) : worker_thread(parent){}
		~we_display_thread() {}
		GETSET(lut)
		
	signals:
		void theImage(const QImage &);
};

class we_display : public worker_element
{ 
	Q_OBJECT 
	
	private:
		
	public:
		QPixmap plot;
		
		we_display(QWidget *parent, unsigned int fifo_depth) : worker_element(parent,fifo_depth)
		{
			/** replace the thread */			
			delete thread;
			thread = new we_display_thread(this);

			/** set interface properties */
			intf_in[0]->set_is_special(true); /** sink */
			
			setFixedSize(1100, 1100);
			setWindowTitle("Display");
			connect(thread, SIGNAL(theImage(const QImage &)),this, SLOT(displayImage(const QImage &)));
			show();
		}
		
		void paintEvent(QPaintEvent *)
		{	
			QPainter painter(this);
			painter.drawPixmap(0, 0, plot);
		}
		
		~we_display() {};
		
	public slots:
		void displayImage(const QImage &image)
		{
			plot = QPixmap::fromImage(image);
			//show();
			update();
			raise();
			//activateWindow();
		}	
		
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
			((we_display_thread*)thread)->set_lut(lut);
			return 0;
		}
};



#endif
