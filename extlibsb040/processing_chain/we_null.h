// this is a /dev/null for frames

#ifndef __WE_NULL_H__
#define __WE_NULL_H__

#include "worker_element.h"

class we_null : public worker_element
{ 
	Q_OBJECT 
	
	public:
		we_null(QWidget *parent, unsigned int fifo_depth) : worker_element(parent,fifo_depth)
		{
			/** set interface properties */
			intf_in[0]->set_is_special(true); /** sink */
		}

	~we_null() {};
};

#endif