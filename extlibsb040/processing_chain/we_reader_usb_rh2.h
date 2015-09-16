#ifndef __WE_READER_USB_RH2_H__
#define __WE_READER_USB_RH2_H__

#include "worker_thread.h"
#include "worker_element.h"
#include "qqueue.h"
#include "frame.h"
#include "helper.h"

#include <libusb-1.0/libusb.h>         // Main header file of the libusb library.

#define EP_OUT			4
//#define EP_IN			8
//#define ID_VENDOR		0x0547
//#define ID_PRODUCT		0x1002
#define USB_TIMEOUT		4096
#define MAX_USB_PACKET_SIZE	512

#define wet_debug(output) std::cerr << qPrintable(parent->objectName()) << ":" << output << std::endl;

class we_reader_usb_rh2_thread : public worker_thread
{  
	Q_OBJECT
	
protected:
	QString filename;
	unsigned int usb_vendor,usb_product,usb_endpoint;
	int roi_start,roi_size, delay;
	QStringList init;
    libusb_device_handle *usb_dev;
	


	void run()
	{
        if(*stop==true)qDebug()<<__FUNCTION__<<": stop already set false!";
		int len=0;
		/** setup envionment */
		worker_element *parent=(worker_element*)this->parent();
		qfifo *fifo_in =	(parent->get_intf_in())[0]->get_fifo();
		qfifo *fifo_out =	(parent->get_intf_out())[0]->get_fifo();
		frame<uint32_t> **frames = (parent->get_intf_out())[0]->get_frames();
		
		int ret;
		libusb_context *usb_ctx;
		ret=libusb_init(&usb_ctx);
		if(ret!=0){
			std::cerr << "ERROR libusb_init:" << libusb_error_name(ret) << std::endl;
		}
		
		libusb_set_debug(usb_ctx,3);
		
        usb_dev=libusb_open_device_with_vid_pid(usb_ctx,usb_vendor,usb_product);
		if(usb_dev==NULL){
            std::cerr << "ERROR: Failed to open device (" << std::hex << usb_vendor <<":"<<usb_product<<")."<< std::endl;
            return;
		}
		
		ret=libusb_set_configuration(usb_dev,1);
		if(ret!=0){
			std::cerr << "ERROR: libusb_set_configuration: " << libusb_error_name(ret) << std::endl;
		}
		
		ret=libusb_claim_interface(usb_dev,0);
		if(ret!=0){
			std::cerr << "ERROR: libusb_claim_interface: " << libusb_error_name(ret) << std::endl;
			exit(0);
		}
		
		unsigned char buffer[MAX_USB_PACKET_SIZE];
		usb_endpoint|=0x80;//in direction
		int transferred;
		ret=libusb_bulk_transfer(usb_dev,usb_endpoint,buffer,MAX_USB_PACKET_SIZE,&transferred,1000);
		if(ret<0){
			std::cerr << "ERROR: libusb_bulk_transfer: " << libusb_error_name(ret) << std::endl;
		}
		ret=libusb_bulk_transfer(usb_dev,usb_endpoint,buffer,MAX_USB_PACKET_SIZE,&transferred,1000);
		if(ret<0){
			std::cerr << "ERROR: libusb_bulk_transfer: " << libusb_error_name(ret) << std::endl;
		}
		ret=libusb_bulk_transfer(usb_dev,usb_endpoint,buffer,MAX_USB_PACKET_SIZE,&transferred,1000);
		if(ret<0){
			std::cerr << "ERROR: libusb_bulk_transfer: " << libusb_error_name(ret) << std::endl;
		}
		
		time_t start,end;
		unsigned long long data_written=0;

        bool ok;
		const unsigned int cmd_size=2;
			unsigned char *cmd=(unsigned char*)calloc(1,cmd_size);
		std::cerr<<"Sending configuration to FPGA firmware:" << std::endl;
		for (int i = 0; i < init.size(); ++i){
			*((uint16_t*)cmd)=(uint16_t)init.at(i).toUInt(&ok,16);
            std::cerr << std::hex << usb_product << ":" << std::hex << "0x" << *((uint16_t*)cmd) << std::dec << std::endl;
            ret=libusb_bulk_transfer(usb_dev, EP_OUT,cmd,cmd_size,&transferred,500);
            if(ret<0){
				std::cerr << "ERROR: libusb_bulk_transfer: " << libusb_error_name(ret) << std::endl;
            }
			if(transferred!=cmd_size){
                printf("Error sending command (%i) 0x%4.4x (len=%i, transferred=%i)\n",i,*((uint16_t*)cmd),cmd_size,transferred);
                goto done;
			}
		}
		time(&start);
		do
		{
			int id=fifo_in->pop(stop);
            if(id<0){
                qDebug()<<__FUNCTION__<<": id<0!";
                break;
            }
			do{
                ret=libusb_bulk_transfer(usb_dev, usb_endpoint,(unsigned char*)frames[id]->data(),frames[id]->get_size()/sizeof(uint32_t),&transferred,1000);//read usb data
				if(ret<0){
				 std::cerr << "ERROR: libusb_bulk_transfer: " << libusb_error_name(ret) << std::endl;
				}
				//if(len<(frames[id]->get_size()))printf("FRAMESIZE mismatch! (%d != %d)\n",len,frames[id]->get_size());
            }while((ret<0)&&(*stop==false));
            frames[id]->set_size_effective(transferred);
			fifo_out->push(id);
			frames_processed++;
		}while(*stop==false);
done:
        /** close usb device */
		ret=libusb_reset_device(usb_dev);
		if(ret!=0){
			std::cerr << "ERROR: libusb_reset_device: " << libusb_error_name(ret) << std::endl;
		}
		libusb_release_interface(usb_dev,0);
		if(ret!=0){
			std::cerr << "ERROR: libusb_release_interface: " << libusb_error_name(ret) << std::endl;
		}
		libusb_close(usb_dev);
		if(ret!=0){
			std::cerr << "ERROR: libusb_close: " << libusb_error_name(ret) << std::endl;
		}
		libusb_exit(usb_ctx);
		if(ret!=0){
			std::cerr << "ERROR: libusb_exit: " << libusb_error_name(ret) << std::endl;
		}				
        qDebug()<<"["<<parent->objectName()<<"]: "<<"DONE!";
	}
public:
    we_reader_usb_rh2_thread(QObject *parent) : worker_thread(parent),init(),usb_dev(NULL) {
		usb_vendor=0x0547;
		usb_product=0x1002;
		usb_endpoint=8;
		roi_start=-1;
		roi_size=-1;
	}
	~we_reader_usb_rh2_thread() {}

    int sendCommand(uint16_t cmd,int timeout=500){
        int transferred,ret;
        std::cerr << std::hex << usb_product << ":" << std::hex << "0x" << cmd << std::dec << std::endl;
        ret=libusb_bulk_transfer(usb_dev,EP_OUT,(unsigned char*)&cmd,sizeof(cmd),&transferred,timeout);
        if(ret<0){
            return ret;
        }
        if(transferred!=sizeof(cmd)){
            return 2-transferred;
        }
        return 0;
    }

	GETSET(filename)
	SET(usb_vendor);
	SET(usb_product);
	SET(usb_endpoint);
	SET(init);
};

class we_reader_usb_rh2 : public worker_element
{ 
	Q_OBJECT 
	
private:
	
public:
	we_reader_usb_rh2(QWidget *parent, unsigned int frame_count, unsigned int frame_size_x, unsigned int frame_size_y) : worker_element(parent,frame_count)
	{
		/** replace the thread */
		delete thread;
		thread = new we_reader_usb_rh2_thread(this);
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
	
	~we_reader_usb_rh2() {}
	
    int configure(const QSettings& settings)
	{	
        int usb_vendor = settings.value("vendor",0x0547U).toInt();
		((we_reader_usb_rh2_thread*)thread)->set_usb_vendor(usb_vendor);
        int usb_product = settings.value("product",0x1002U).toInt();
		((we_reader_usb_rh2_thread*)thread)->set_usb_product(usb_product);
        int usb_endpoint = settings.value("endpoint",0x8U).toInt();
		((we_reader_usb_rh2_thread*)thread)->set_usb_endpoint(usb_endpoint);
		
        QStringList init=settings.value("init").toStringList();
		((we_reader_usb_rh2_thread*)thread)->set_init(init);
        qDebug()<<"Setting up we_reader_usb_rh2"<< init.size();
		return 0;
	}

    int sendCommand(uint16_t cmd,int timeout=500){
        return ((we_reader_usb_rh2_thread*)thread)->sendCommand(cmd,timeout);
    }

public slots:
    void resetCor(){
        sendCommand(0xF001);
    }

    void sndCmd(uint16_t cmd){
        sendCommand(cmd);
    }
};
#endif
