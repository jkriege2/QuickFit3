#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <unistd.h> //usleep

#include "../../../../LIB/trunk/qt/qqueue.h"
#include "../../../../LIB/trunk/frame.h"

#include "worker_element.h"

#ifdef __linux__
#include <sys/types.h>
#include <sys/syscall.h> 
#endif

#define wet_pid std::cerr << qPrintable(parent->objectName()) << ": pid=" << syscall(SYS_gettid) << std::endl;/**< prints the current PID, so you can find it in top -H */

class worker_thread : public QThread
{  
	Q_OBJECT
	
	friend class worker_element;
	
	protected:
		unsigned int frames_processed;
		bool *stop;
	public:
		worker_thread(QObject *parent);
		
        GETRST(frames_processed)
        GET(frames_processed)

	protected:
		void run();
		
	signals:
		void log_txt(QString);
		void log_wrn(QString);
		void log_err(QString);

};
#endif
