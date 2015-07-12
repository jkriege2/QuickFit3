// Basic Idea:
// We use these classes to build chains. 
// The data path is along such a chains.
// 
// Currently only a single path is allowed, splitter and merger might be implemented in the future.
// 
// Every working element has a thread performing the specific task. 
// 
// Data is transmitted using frames. The frames are located in an array created by the first worker (source).
// Those frames are handed over only by the id of the element. These ID are stored in fifos.
// 
// Every worker has a fifo for storing data for the next worker.
// The last worker (sink) needs to transmit the free frames back to the source.
// 
// It is also possible to build sinks that source ;-)
// The fifo chaining is done automatically using is_source variable;

// when connecting next worker_eleemtn make sure all properties are set before!

// before anything can be started, we need to fill the last fifo, so the source can do anything

//TODO: setup buffers when add is called.
//FIXME: every source should create its own buffers automatically, those pointers are propagated by a setup function

#ifndef __WORKER_ELEMENT_H__
#define __WORKER_ELEMENT_H__

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <unistd.h> //usleep
#include <QSettings>
#include <QTimer>


#include "qqueue.h"
#include "../../../../LIB/trunk/frame.h"
#include "../../../../LIB/trunk/helper.h"

#include "worker_thread.h"
#include "worker_interface.h"

// Connection scheme:
// simple: (AB)->(AB)->(AB)
// fifo->-A
// fifo-<-A
// setup steps: connect we in/out interfaces then build the chain

class worker_thread;
class worker_interface;

class worker_element : public QWidget
{ 
  Q_OBJECT 

 protected:
  QTimer *status_timer;
  worker_thread *thread;
  worker_interface **intf_in, **intf_out; /** interfaces */
  bool is_setup;
  bool is_run;
  unsigned int level;
  bool *breakCondition;
		
 public:
  worker_element(QWidget *parent,unsigned int);
  ~worker_element();		
  void connect_next(worker_element*, unsigned int, unsigned int);/**build double linked list, connect fifos*/
  void run(unsigned int);
  void setup(unsigned int);
  void setup_fifo(int value, unsigned int channel_out=0);
  worker_element* connect_internal(worker_interface*);
  int find_interface(worker_interface *intf);
  int get_user_slot(int);
  int check();
  virtual int configure(const QSettings&);
  virtual void stop();
  GET(intf_in);
  GET(intf_out);
  GET(breakCondition);
  bool isRunning();

		
 public slots:
  virtual void status_update();
  void run_chain();
  void logger_txt(QString s){emit log_txt(this->objectName()+": "+s);};
  void logger_wrn(QString s){emit log_wrn(this->objectName()+": "+s);};
  void logger_err(QString s){emit log_err(this->objectName()+": "+s);};
  void thread_finished(){if(status_timer!=NULL)status_timer->stop();emit finished(this->objectName());};
	
 signals:
  void log_txt(QString);
  void log_wrn(QString);
  void log_err(QString);
  void finished(const QString &sender);
		
};
#endif
