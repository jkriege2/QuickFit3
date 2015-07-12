/*
*	
*	THIS IS A SINK
*
* This WE will display graphs for all pixels it might find
* OK, currently we simply display the sum of all pixels [0..1024]
*/

#ifndef __WE_DISPLAY_GRAPH_H__
#define __WE_DISPLAY_GRAPH_H__

#include "we_sink_source.h"
#include "worker_thread.h"
#include "../../../../LIB/trunk/frame.h"
#include "../../../../LIB/trunk/correlator_multitau.h"

#include "../../../../LIB/trunk/qt/jkqtfastplotter.h"
#include <QGridLayout>

class we_display_graph_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		JKQTFastPlotter *plotter;
		JKQTFPLinePlot *plot;
		QVector<double> *values_y,*values_x;
        double min, max;
		
		void run()
		{
			worker_element *parent=(worker_element*)this->parent();
			qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
			qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
			frame<uint32_t> **frames = (parent->get_intf_in())[0]->get_frames();
			
			int last_frame=0;
			
			do
			{
				int id=fifo_in->pop(stop);/**< get new frame for processing*/
				if(id<0)break;
					
				last_frame=frames[id]->get_is_last();
				
				/** calculate sum */
				unsigned int sum=0;

				int xmax=frames[id]->get_xres();
				int ymax=frames[id]->get_yres();
				for (unsigned int x=0; x<frames[id]->get_xres(); x++)
				{
					for (unsigned int y=0; y<frames[id]->get_yres(); y++)
					{
						sum+=frames[id]->get_pixel(x,y);
					}
				}
				fifo_out->push(id);

				values_y->pop_front();
				values_y->push_back((double)sum);
                if(sum>max)max=sum;

				plotter->set_doDrawing(false);
				plotter->setXRange(0.0,1024.0,false);
				plotter->setYRange(0.0,max);
				plotter->set_xTickDistance(256);
				plotter->set_yTickDistance(max/4);
				plotter->set_doDrawing(true);
				//std::cout << "WERT" << (double)array[0][channels-1] << std::endl;
				plotter->update_data();
				frames_processed++;
			}while((last_frame==0)&&(*stop==false));
				
			qDebug() << parent->objectName() << " DONE";
		}
	public:
		we_display_graph_thread(QObject *parent) : worker_thread(parent)
		{
			min=0;
            max=1;
		}
		
		SET(plotter)
		SET(plot)
		SET(values_x)
		SET(values_y)

};

class we_display_graph : public worker_element
{ 
	Q_OBJECT 
	
	public:
		we_display_graph(QWidget *parent, unsigned int fifo_depth) : worker_element(parent,fifo_depth)
		{
			/** replace the thread */			
			delete thread;
			thread = new we_display_graph_thread(this);

			/** set interface properties */
			intf_in[0]->set_is_special(true); /** sink */
			
			/** visual interface buhh... ;-) */
			
			QGridLayout *grid=new QGridLayout(this);
			/** instatiate plotter stuff */
			JKQTFastPlotter *plotter = new JKQTFastPlotter(this);
			QVector<double> *values_x=new QVector<double>();
			QVector<double> *values_y=new QVector<double>();
			JKQTFPLinePlot *plot = new JKQTFPLinePlot(plotter,values_x,values_y,QColor("blue"));
			plotter->addPlot(plot);
			


			/** setup thread */
			((we_display_graph_thread*)thread)->set_plotter(plotter);
			((we_display_graph_thread*)thread)->set_plot(plot);
			((we_display_graph_thread*)thread)->set_values_x(values_x);
			((we_display_graph_thread*)thread)->set_values_y(values_y);
			
			grid->addWidget(plotter);
			this->setLayout(grid);
			resize(600, 600);
			show();

						/** fill in initial values */
			for(int x=0;x<1024;x++)
			{
				values_x->push_back((double)(x));
				values_y->push_back(0.0);
			}

			
		}
		
        int configure(const QSettings& settings)
		{	
			return 0;
		}
};
#endif
