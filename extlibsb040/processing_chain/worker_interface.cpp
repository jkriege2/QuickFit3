#include "worker_interface.h"
#include "worker_element.h"

worker_interface::worker_interface(worker_element *parent, bool is_out, unsigned int fifo_depth)
{
	this->parent=parent;
	this->succ=NULL;
	this->pred=NULL;

	this->is_special=false;
	this->is_out=is_out;
	if(is_out)
		this->fifo = new qfifo(fifo_depth,0); /** only output fifos are initialized */
	else
		this->fifo=NULL;
	this->frames=NULL;
	this->is_connected=NULL;
	this->inherit=NULL; /** no properties will be inherited */
}

worker_interface::~worker_interface()
{
	if(is_out)
	{
		delete(this->fifo);
		if(is_special)
		{
//			std::cerr << qPrintable(parent->objectName()) << std::endl;
			cleanup< frame<uint32_t> >(this->frames);
			this->frames=NULL;
		}
	}

}

void worker_interface::connect_next(worker_interface *next)
{
//	std::cerr << __FUNCTION__ << ":" <<  qPrintable(parent->objectName()) << "(" << parent->find_interface(this) << ") ->" << qPrintable(next->parent->objectName()) << "(" << next->parent->find_interface(next) << ")" << std::endl;
	
    //std::cerr << "this->frames=" << this->frames << std::endl;
	if((next->is_connected))
	{
//		fprintf(stderr,"WARNING (%s): Interface already connected!\n",__FUNCTION__);
	}

	{
		if(this->is_out!=next->is_out)/**< only different types of interfaces can be connected*/
		{
			this->succ=next;
			this->succ->pred=this;
			this->succ->frames=this->frames;//FIXME: check this, frames are the same for in and out on the same interface
			if((this->fifo!=NULL)&&(this->succ->fifo!=NULL))/**< setup fifo depth for the chain */
				this->succ->fifo->resize(this->fifo->get_size());
			
			if(this->is_out)/** connect to next external, do nothing for internal connection */
			{
				this->succ->parent->connect_internal(this->succ);
				this->succ->fifo=fifo;
//				std::cerr << "worker_interface: Setting this->succ->frames=this->frames: " << this->frames << std::endl;
				this->succ->frames=this->frames;
				if(this->succ->is_special) /** next is sink: close the circle */
				{
					worker_interface *intf=this->succ;
					do
					{
						intf=intf->pred;
                        std::cerr<< __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << this << ":" << intf << std::endl;
					}while((intf!=NULL)&&((intf->is_special==false)||(intf->is_out==false)));/** looking for a sink */
					if((intf->is_special==false)||(intf->is_out==false))
					{
                        //std::cerr << "ERROR (" << __FILE__ << "," << __FUNCTION__ << "): no source found!" << std::endl;
					}
					else
					{
                        //fprintf(stderr,"connecting internal:\n");
						this->succ->parent->connect_internal(this->succ);/**< this is something like this->succ->connect_next(this->succ->succ)*/
						this->succ->succ->connect_next(intf->pred); /** close the circle */
                        //fprintf(stderr,"cicle closed..\n");
					}
				}
			}
		}
        else{
//			fprintf(stderr,"ERROR (%s): Connot connect interfaces of same kind!\n",__FUNCTION__);
        }
	}
}

int worker_interface::check()/** check if we are ready to run */
{
		if((is_connected==false)||(fifo==NULL)||(frames==NULL)||(pred==NULL)||(succ==NULL)||(parent==NULL))
		{
//			std::cerr << "[" << __FILE__ << "," << __FUNCTION__ << "," << __LINE__ << "] ERROR: sanity check failed!" << std::endl;
//			CERR(is_connected)
//			CERR(fifo)
//			CERR(frames)
//			CERR(pred)
//			CERR(succ)
//			CERR(parent)
		
			return (-1);
		}
                return 0;
}



/** setup single interface by means of frameproperty inheritance */
void worker_interface::setup(){
	if(inherit!=NULL){
		int xres=(inherit->get_frames())[0]->get_xres();
		int yres=(inherit->get_frames())[0]->get_yres();
		int fcnt=0;
		frame<uint32_t,32,32> **frames=inherit->get_frames();
		while(frames[fcnt]!=NULL){fcnt++;};
        //fprintf(stderr,"inherited xres=%i, yres=%i, fcnt=%i\n",xres,yres,fcnt);
		/** setup frames */
		if(this->frames==NULL){
			this->frames=(frame<uint32_t> **)calloc(fcnt+1,sizeof(frame<uint32_t>*));
			for(int i=0;i<fcnt;i++)
			{
				this->frames[i]=(frame<uint32_t> *)new frame<uint32_t>(xres,yres);
        //printf("%s: %p\n",__FUNCTION__,this->frames[i]);
			}
			this->frames[fcnt]=NULL;
//			this->set_frames(frames);
			//this->succ->frames=this->frames;
			//this->succ->succ->frames=this->frames;
		}else{
//			fprintf(stderr,"already set up!\nresizing...\n");
//			qDebug() << "Inheritance: " << this->inherit->pred->parent->objectName() << "(" << this->inherit->pred->frames[0]->get_xres() <<  ") -> "
//																	<< this->inherit->parent->objectName() << "(" << this->inherit->frames[0]->get_xres() <<  ") -> "
//																	<< this->parent->objectName() << "(" << this->frames[0]->get_xres() << ")";
			fcnt=0;
			do{
				this->frames[fcnt]->resize(xres,yres);
				fcnt++;
			}while(this->frames[fcnt]!=NULL);
//			qDebug() << "Inheritance: " << this->inherit->pred->parent->objectName() << "(" << this->inherit->pred->frames[0]->get_xres() <<  ") -> "
//																	<< this->inherit->parent->objectName() << "(" << this->inherit->frames[0]->get_xres() <<  ") -> "
//																	<< this->parent->objectName() << "(" << this->frames[0]->get_xres() << ")";
		}
	}
}
