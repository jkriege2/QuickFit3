#ifndef __WORKER_INTERFACE_H__
#define __WORKER_INTERFACE_H__

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <unistd.h> //usleep

#include "../../../../LIB/trunk/qt/qqueue.h"
#include "../../../../LIB/trunk/frame.h"
#include "../../../../LIB/trunk/helper.h"

#include "worker_element.h"
#include "worker_thread.h"

/* Connection scheme:
 * simple: (AB)->(AB)->(AB)
 * fifo->-A\
 * fifo-<-A/
 * setup steps: connect we in/out interfaces then build the chain
 */

class worker_element;

class worker_interface
{
	friend class worker_thread;
	friend class worker_element;
	
	protected:
		bool is_special;/** sink or source*/
		bool is_out;/** type of interface in/out */
		bool is_connected; /** is already connected to another interface */
		qfifo *fifo;
		frame<uint32_t> **frames;
		worker_interface *pred; /** predecessor */
		worker_interface *succ; /** successor */
		worker_element *parent; /** reference to parent object */
		worker_interface *inherit; /** reference to some sort of predecessor we inherit our frame properties from */
	public:
		worker_interface(worker_element *parent, bool is_out=false, unsigned int fifo_depth=0);
		~worker_interface();		
		void connect_next(worker_interface *next);
		void setup();
		int check();
		GETSET(is_special);
		GETSET(fifo);
		GETSET(frames);
		GETSET(inherit);
};
#endif