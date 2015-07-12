/*
 * TODO: - property inheritance for interfaces
 *       - automatic property exporter (generated editor) and check for existence
 *       - macros for creating properties?!?
 * 
 *
 * 
 */

#ifndef __PROCESSING_CHAIN_H__
#define __PROCESSING_CHAIN_H__

 #include "we_writer.h"
 #include "we_frame_analyzer.h"
 #include "we_reader_raw.h"
 #include "we_frame_gen.h"
 #include "we_copy_nb.h"
 #include "we_display.h"
 #include "we_display2.h"
 #include "we_accumulate.h"
 #include "we_split.h"
 #include "we_frame_gen_rnd.h"
 #include "we_reader_usb_rh2.h"
 #include "we_roi.h"
 #include "we_correlator.h"
 #include "we_null.h"
 #include "we_bridge_outlet.h"
 #include "we_display_graph.h"
 #include "we_endpoint.h"
#include "we_frame_gen_chspad.h"
#include "we_correlator_hw.h"

 
 #include <QWaitCondition>
 #include <QMutex>
 
/*!
		\brief recursive function to build chain structure
		
		This function walks through the configuration file recursively and creates the structure of the WEs
		
		@param settings			pointer to the ini-file
		@param current			name of the current element
		@param parent_id		array index for the parent WE
		@param parent_slot	interface id of the parent
		@param slot 				input interface id for current WE
		@param array				array of WEs
		@param array_pos		pointing to the next free element in the WE array
*/

class processing_chain : public QObject{     
	Q_OBJECT
	Q_CLASSINFO("Author", "Jan Buchholz")
	
	private:
		int we_count;
		worker_element **we; 
		bool running;
		int set;
		QMutex *mutex;
		QWaitCondition *waitCondition;
        int tag;
		
        int build_we_structure(QSettings &settings, QString current, int parent_id, int parent_slot, int slot, worker_element **array, int *array_pos, unsigned int);
		worker_element * createByName(const QString &,unsigned int,unsigned int, unsigned int);
    void create(QSettings &settings, unsigned int set);
    void create(QSettings &settings, const QString&);

		void destroy();
		
	
	public:
    processing_chain(QSettings&, unsigned int set);
    processing_chain(QSettings&, const QString&);
    ~processing_chain();
		
		//void configure(QSettings *settings, unsigned int set);

        void stop(const QString &sender);
    int getSetByName(QSettings& settings, const QString &setName);
    bool reconfigure(QSettings& settings, unsigned int set);
    bool reconfigure(QSettings&, const QString&);

		template <class T>
		T* find_first(){
			T* result=NULL;
			for(int i=0;i<we_count;i++){
				result=dynamic_cast<T*>(we[i]);
				if(result!=NULL)
					break;
			}
			return result;
		}
		
		bool isRunning(){return running;}
		bool checkRunningThread();

        void setTag(int value){tag=value;}
        int getTag(){return tag;}

        bool findGroupByType(const QString &type,QSettings &settings, QString &group, unsigned int set);
        bool findGroupByType(const QString &type, QSettings &settings, QString &group, const QString &setName);
signals:
		void log_txt(QString);
		void log_wrn(QString);
		void log_err(QString);	
        void finishedAll(int tag);

	protected slots:
		void logger_txt(QString message){log_txt(message);}
		void logger_wrn(QString message){log_wrn(message);}
		void logger_err(QString message){log_err(message);}
        void finished(const QString &sender);
		
	public slots:
		void run();
	
};
#endif
