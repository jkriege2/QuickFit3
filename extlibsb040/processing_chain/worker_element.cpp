#include "worker_element.h"
#include <limits>

worker_element::worker_element(QWidget *parent,unsigned int fifo_depth) : QWidget(parent)
{
	breakCondition = new bool(false);
	thread = new worker_thread(this);
	connect(thread, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
	connect(thread, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
	connect(thread, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
	connect(thread, SIGNAL(finished()),this,SLOT(thread_finished()));
	
	/** lets build the interfaces */
	intf_in=(worker_interface **)calloc(2,sizeof(worker_interface *));
//	fprintf(stderr,"Calloc(2,%i)=%p\n",(int)sizeof(worker_interface *),(void*)intf_in);
	intf_in[0]=new worker_interface(this,false,fifo_depth);
	intf_in[1]=NULL;
	
	intf_out=(worker_interface **)calloc(2,sizeof(worker_interface *));
//	fprintf(stderr,"Calloc(2,%i)=%p\n",(int)sizeof(worker_interface *),(void*)intf_out);
	intf_out[0]=new worker_interface(this,true,fifo_depth);
	intf_out[1]=NULL;
	
//	fprintf(stderr,"%p,%p,%p\n",intf_out[0],intf_out[1],intf_out[2]);
//	fprintf(stderr,"%p,%p,%p\n",intf_in[0],intf_in[1],intf_in[2]);
	
	/** connect interfaces */
	//FIXME: should be done on manual connection request!
	intf_in[0]->connect_next(intf_out[0]);/** okay, we need this connection to find the upstream source and the predecessor */
	
	/** set state */
	this->is_setup=false;
	this->is_run=false;
	
	/** setup timer for status */
  status_timer=NULL;
    //status_timer = new QTimer(this);
    //connect(status_timer, SIGNAL(timeout()), this, SLOT(status_update()));
    //status_timer->start(1000);
	
	this->level=INT_MAX;
}

worker_element::~worker_element()
{
    if (status_timer) delete status_timer ;
    delete thread;
	cleanup<worker_interface>(intf_in);
	cleanup<worker_interface>(intf_out);
}

void worker_element::connect_next(worker_element* next, unsigned int channel_out=0, unsigned int channel_in=0)/**build double linked list, connect fifos*/
{
	this->intf_out[channel_out]->setup();/** for inheritance of frame properties */
	this->intf_out[channel_out]->connect_next(next->intf_in[channel_in]);
}

worker_element* worker_element::connect_internal(worker_interface* intf)/** this function is called externally to request internal connection, returns pointer to successor*/
{
	worker_element *result=NULL;
	int id=0;
	
//	std::cerr << "connect_internal" << std::endl;
	
	id=find_id<worker_interface>(intf_in,intf);
//	std::cerr << "found i=" << id << ", intf_in[id]=" << intf_in[id] <<  std::endl;
	if(intf_in[id]==intf)
	{
		switch(id)
		{
			case 0:intf_in[id]->connect_next(intf_out[id]);break;/** default connection */
		}
	}
	return result;
}

void worker_element::run(unsigned int level)
{
	if(!thread->isRunning())
	{
		*breakCondition=false;
		this->is_run=true;
//		qDebug() << "RUN: (" << level << ") " << this->objectName();
		
		if (!thread->isRunning())
		{
			thread->start();
		}
		
		unsigned int i=0;
		while(intf_out[i]!=NULL)//run all following workers
	intf_out[i++]->succ->parent->run(level+1);
	}
}

int worker_element::find_interface(worker_interface *intf)
{
	int id_in,id_out;
	id_in=find_id<worker_interface>(intf_in,intf);
	id_out=find_id<worker_interface>(intf_out,intf);
	return (id_in>=0) ? id_in : id_out;
}

int worker_element::check()
{
	unsigned int i;
	int result=0;
	
	i=0;
	while(intf_in[i]!=NULL)
	{
//		std::cerr << "intf_in[" << i << "]" << std::endl;
		result+=intf_in[i]->check();
		i++;
	}
	
	i=0;
	while(intf_out[i]!=NULL)
	{
//		std::cerr << "intf_out[" << i << "]" << std::endl;
		result+=intf_out[i]->check();
		i++;
	}
	return result;	
}

void worker_element::setup(unsigned int level)
{
	if(this->level>=level)
	{
		check();
		this->is_setup=true;
		this->level=level;
//		qDebug() << "SETUP (" << level << ") " << this->objectName();
		unsigned int i=0;
		while(intf_out[i]!=NULL)//run all following workers
		{
			(intf_out[i]->pred->get_fifo())->clear();
			
			if(intf_out[i]->is_special)/** is source */
			{
				//fprintf(stderr,"%p,%p,%p\n",intf_out[0],intf_out[1],intf_out[2]);
				//fprintf(stderr,"%p,%p,%p\n",intf_in[0],intf_in[1],intf_in[2]);
				
				qfifo *fifo=intf_out[i]->pred->get_fifo();
				unsigned int fifo_depth=(intf_out[i]->get_fifo())->get_size();
				//std::cerr << "fifo_depth=" << fifo_depth << " " << (intf_out[i]->pred->get_fifo())->get_size() << std::endl;
				
				for(unsigned int j=0;j<fifo_depth;j++)
				{
					fifo->push(j);
				}
			}

			intf_out[i]->succ->parent->setup(level+1);
			i++;
		}
	
	}
}

void worker_element::setup_fifo(int value, unsigned int channel_out)
{
	if((this->intf_out[channel_out]->is_special==false)||(this->intf_out[channel_out]->is_out==false))
	{
//		std::cerr << "ERROR: you can't push into a non-source FIFO!" << std::endl;
		return;
	}
	qfifo *fifo = this->intf_out[channel_out]->pred->fifo;
	if(fifo==NULL)
	{
//		qDebug() <<  this->objectName();
//		std::cerr << "ERROR: FIFO is not initialized!" << std::endl;
		return;
	}
	fifo->push(value);
}

void worker_element::run_chain()
{
	run(0);
}

int worker_element::get_user_slot(int slot)/** this function returns the slot'est output interface id where the corrwesponding input interface is not a sink*/
{
	int i=0,j=0,result=-1;
	
	while(intf_out[i]!=NULL)
	{
		if(intf_out[i]->pred->get_is_special()==false)/** valid user interface */
		{
			if(j==slot)
			{
				result=i;
				break;
			}
			j++;
		}
		i++;
	}
	return result;
}

int worker_element::configure(const QSettings& settings)
{
	return 0;
}

void worker_element::status_update()
{
	int i=0;
	QString s="in:";
	//  std::cerr << qPrintable(this->objectName()) << ": in:";
	
	while(intf_in[i]!=NULL)
	{
		if(intf_in[i]->get_fifo()!=NULL)
			s+= QString("%1 %2").arg(i).arg((intf_in[i]->get_fifo())->get_depth());
		i++;
	}
	i=0;
	s+=" ; out:";
	while(intf_out[i]!=NULL)
	{
		if(intf_out[i]->get_fifo()!=NULL)
			s+= QString("%1 %2").arg(i).arg((intf_in[i]->get_fifo())->get_depth());
		//	std::cerr << " (" << i << ")" << (intf_out[i]->get_fifo())->get_depth();
			i++;
	}
    s+=QString(". fps: %1 @%2").arg(((float) (thread->get_frames_processed() * status_timer->interval())) / 1000.0).arg(thread->isRunning());
    s+=QString("fp: %1").arg(thread->get_frames_processed());
	logger_txt(s+"\n");
	
	//  std::cerr << ". fps:" << ((float) (thread->getrst_frames_processed() * status_timer->interval())) / 1000.0 << "@" << thread->isRunning() << std::endl;
}

void worker_element::stop(){
//	std::cerr << "break called:" << qPrintable(objectName()) << std::endl;
	if(thread->isRunning())
	{
		//set stop condition
		*breakCondition=true;
		//unblock fifos
		unsigned int i=0;
		while(intf_in[i]!=NULL)
		{
			(intf_in[i]->get_fifo())->wakeAll();
			i++;
		}
		i=0;
		while(intf_out[i]!=NULL)
		{
			(intf_out[i]->get_fifo())->wakeAll();
			i++;
		}
	}
}

bool worker_element::isRunning(){
	return thread->isRunning();
}
