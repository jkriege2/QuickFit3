/*
*	
*	THIS IS A SINK/SOURCE
*
*	-> FIFO_IN[0]  \ # / FIFO_OUT[1] ->
*                | # |
* <- FIFO_OUT[0] / # \ FIFO_IN[1]  <-
*/

#ifndef __WE_CORRELATOR_H__
#define __WE_CORRELATOR_H__

#include "we_sink_source.h"
#include "worker_thread.h"
#include "../../../../LIB/trunk/frame.h"
#include "../../../../LIB/trunk/correlator_multitau.h"

#include "../../../../LIB/trunk/qt/jkqtfastplotter.h"
#include <QGridLayout>

class we_correlator_thread : public worker_thread
{  
	Q_OBJECT
	
	protected:
		correlatorjb<long long int,float>	*correlator;
		unsigned int steps; /**< number of correlator steps*/
		JKQTFastPlotter *plotter;
		JKQTFPLinePlot *plot, *plot_mean;
		QVector<double> *values_x,*values_y,*values_y_mean;
		unsigned int count;/**< counts the correlation cycles */
		unsigned int reset_count; /**< steps between the correlator is reset */
		bool log_scale; /**< use a logarithmic scale */
		
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

			wet_pid
			
			int last_frame=0;
			int id_out;

			do
			{
				int i=0;
				do
				{
					int id_in=fifo_snk_in->pop();/**< get new frame for processing*/
					int value=frames_snk[id_in]->get_pixel(0,0);
					correlator->run(value,value);
					//std::cerr << value << std::endl;
					last_frame=frames_snk[id_in]->get_is_last();
					fifo_snk_out->push(id_in);
					frames_processed++;
				}while((i++<steps)&&(last_frame==0)&&(stop==false));
				
				/** lets get the results and push them out*/
				int id_out=fifo_src_in->pop();

				float **array=correlator->get_array_G();
				unsigned int channels=correlator->get_lag_count();
				values_x->clear();
				values_y->clear();
				for(int x=0;x<channels;x++)
				{
					values_x->push_back((double)(array[0][x]));
					values_y->push_back((double)(array[1][x]));
					//std::cout << array[0][x] << ":" << array[1][x] << std::endl;
				}
				fifo_src_out->push(id_out);
				plotter->setXRange(1.0,(double)array[0][channels-1],log_scale);
				plotter->setYRange(0.0,2.0);
				//std::cout << "WERT" << (double)array[0][channels-1] << std::endl;
				plotter->update_data();
				count++;
				if((count % reset_count)==0)correlator->reset();
				
			}while(last_frame==0&&(*stop==false));
				
			qDebug() << parent->objectName() << " DONE";
		}
	public:
		we_correlator_thread(QObject *parent) : worker_thread(parent)
		{
			count=0;
			reset_count=1;
			log_scale=true;
		}
		
		SET(correlator)
		SET(plotter)
		SET(plot)
		SET(values_x)
		SET(values_y)
		SET(values_y_mean)
		SET(steps)
		SET(reset_count)
		SET(log_scale)
};

class we_correlator : public we_sink_source
{ 
	Q_OBJECT 
	
	public:
		we_correlator(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : we_sink_source(parent,frame_count,frame_size_x,frame_size_y)
		{
			thread = new we_correlator_thread(this);
			
			QGridLayout *grid=new QGridLayout(this);
			/** instatiate plotter stuff */
			JKQTFastPlotter *plotter = new JKQTFastPlotter(this);
			QVector<double> *values_x=new QVector<double>();
			QVector<double> *values_y=new QVector<double>();
			QVector<double> *values_y_mean=new QVector<double>();
			JKQTFPLinePlot *plot = new JKQTFPLinePlot(plotter,values_x,values_y,QColor("blue"));
			plotter->addPlot(plot);
			
			
			/** setup thread */
			((we_correlator_thread*)thread)->set_plotter(plotter);
			((we_correlator_thread*)thread)->set_plot(plot);
			((we_correlator_thread*)thread)->set_values_x(values_x);
			((we_correlator_thread*)thread)->set_values_y(values_y);
			((we_correlator_thread*)thread)->set_values_y_mean(values_y_mean);
			
			grid->addWidget(plotter);
			this->setLayout(grid);
			show();

		}
		
        int configure(const QSettings& settings)
		{	
            unsigned int block_count=settings.value("block_count",0).toInt();
            unsigned int lag_count=settings.value("lag_count",0).toInt();
            unsigned int steps=settings.value("steps",0).toInt();

			correlatorjb<long long int,float>	*correlator;
            correlator=new correlatorjb<long long int,float>(block_count,lag_count,0ULL);
			
			((we_correlator_thread*)thread)->set_correlator(correlator);
			((we_correlator_thread*)thread)->set_steps(steps);
            ((we_correlator_thread*)thread)->set_reset_count(settings.value("reset_count",0).toInt());
            ((we_correlator_thread*)thread)->set_log_scale(settings.value("log_scale",true).toBool());
			
			return 0;
		}
};
#endif
