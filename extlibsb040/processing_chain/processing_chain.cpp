#include "processing_chain.h"
#include <QEventLoop>

#ifdef DEBUG
#define QDEBUG(s) qDebug()<< __FILE__ << "@" << __LINE__ << "(" << __FUNCTION__ << "):" << s;
#else
#define QDEBUG(s)
#endif

worker_element * processing_chain::createByName(const QString &name,unsigned int fifo_depth,unsigned int frame_res_x, unsigned int frame_res_y){
	if (name=="we_reader_raw")
		return (worker_element*) new we_reader_raw(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_frame_analyzer")
		return (worker_element*) new we_frame_analyzer(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_frame_gen")
		return (worker_element*) new we_frame_gen(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_accumulate")
		return (worker_element*) new we_accumulate(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_split")
		return (worker_element*) new we_split(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_copy_nb")
		return (worker_element*) new we_copy_nb(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_display")
		return (worker_element*) new we_display(0,fifo_depth);
	else if (name=="we_display2")
		return (worker_element*) new we_display2(0,fifo_depth);
	else if (name=="we_frame_gen_rnd")
		return (worker_element*) new we_frame_gen_rnd(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_reader_usb_rh2")
		return (worker_element*) new we_reader_usb_rh2(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_roi")
		return (worker_element*) new we_roi(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_correlator")
		return (worker_element*) new we_correlator(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_bridge_outlet")
		return (worker_element*) new we_bridge_outlet(0,fifo_depth,frame_res_x,frame_res_y);
	else if (name=="we_null")
		return (worker_element*) new we_null(0,fifo_depth);
	else if (name=="we_display_graph")
		return (worker_element*) new we_display_graph(0,fifo_depth);
	else if (name=="we_endpoint")
		return (worker_element*) new we_endpoint(0);
	else if (name=="we_writer")
		return (worker_element*) new we_writer(0);
    else if (name=="we_frame_gen_chspad")
        return (worker_element*) new we_frame_gen_chspad(0,fifo_depth,frame_res_x,frame_res_y);
    else if (name=="we_correlator_hw")
        return (worker_element*) new we_correlator_hw(0,fifo_depth);
    return NULL;
}

int processing_chain::build_we_structure(QSettings& settings, QString current, int parent_id, int parent_slot, int slot, worker_element **array, int *array_pos, unsigned int set=0)
{
    QDEBUG("");
//	std::cerr << __FUNCTION__ << "(" << qPrintable(current) << "," << parent_id << "," << parent_slot << "," << slot << ")" << std::endl;
	int id=-1;
	
    QStringList groups = settings.childGroups();
	if(groups.indexOf(current)==-1)
		current.append(QString(":%1").arg(set));
	
    settings.beginGroup(current);
//	std::cerr << "INGROUP:" << settings->group().toLocal8Bit().data() << std::endl;
	

    QString type=settings.value("type").toString();
	if(type.isEmpty())
	{
//		std::cerr << "No type specified (" << qPrintable(current) << ")" << std::endl;
		return EXIT_FAILURE;
	}
    int fifo_depth = settings.value("fifo_depth",32).toInt();
    int frame_res_x = settings.value("frame_res_x",32).toInt();
    int frame_res_y = settings.value("frame_res_y",32).toInt();
	
	/** lets create the WEs */
	array[*array_pos]=createByName(type,fifo_depth,frame_res_x,frame_res_y);
	
	if(array[*array_pos]!=NULL)
	{
		array[*array_pos]->setObjectName(current);
		id=*array_pos;
		(*array_pos)++;
	}
    else{
//		std::cerr << "Failed to create we-object!" << std::endl;
    }

	/** connect */
	if(parent_id>=0)/** a parent does exists */
	{
		int user_slot=array[parent_id]->get_user_slot(parent_slot);/** this returns the output-slots that are not behind a sink, so that a sink is rerally a sink!*/
//		std::cerr << user_slot <<std::endl;
		array[parent_id]->connect_next(array[id],user_slot,slot);
	}
	/** setup WE */
    settings.beginGroup("config");
	array[id]->configure(settings);
    settings.endGroup();
	
	/** create siblings*/
	//  settings->beginWriteArray("out");
	//  for (int i = 0; i < 2; ++i) {
		//      settings->setArrayIndex(i);
		//      settings->setValue("next", "a");
		//  }
		//  settings->endArray();
        settings.endGroup();
		
		//	std::cerr << qPrintable(current+"/out") << "=" << qPrintable(settings->value(current+"/out").toString()) << std::endl;
		int i=0;
        QString value=settings.value(current+"/out/"+QString::number(i)+"/next").toString();
//		qDebug() << __LINE__ << value << current+"/out/"+QString::number(i)+"/next";
		if(value.isEmpty()==false)
		{
			do
			{
			build_we_structure(settings,value,id,i,0,array,array_pos,set);
			i++;
            value=settings.value(current+"/out/"+QString::number(i)+"/next").toString();
			}
			while(value.isEmpty()==false);
		}
	return 0;
}
	
processing_chain::processing_chain(QSettings& settings, unsigned int set):tag(-1){
  this->create(settings,set);
}

processing_chain::processing_chain(QSettings& settings, const QString& setName):tag(-1){
  this->create(settings,setName);
}

processing_chain::~processing_chain(){
	this->destroy();
}

void processing_chain::create(QSettings &settings, const QString& setName){
  int setNumber=getSetByName(settings,setName);
  if(setNumber<0)setNumber=0;
  create(settings,(unsigned int)setNumber);
}

void processing_chain::create(QSettings &settings, unsigned int set){
  //std::cerr<<__FILE__<< ":" << __FUNCTION__ << ":" << __LINE__ << ":" << "set=" << set << std::endl;
	this->set=set;
	running=false;
    we_count = settings.value("chain/elements",16).toInt();/** number of elements */
	we=(worker_element**)calloc(we_count,sizeof(worker_element*)); 
	
    QString current=settings.value("chain/root").toString();
	if(current.isEmpty())
	{
		return;
	}
	int i=0;
	build_we_structure(settings,current,-1,-1,-1,we,&i,set);
	we_count=i;
	
	//connect signals/slots
	for(int j=0;j<we_count;j++)
	{
		QObject::connect(we[j], SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
		QObject::connect(we[j], SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
		QObject::connect(we[j], SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
        QObject::connect(we[j], SIGNAL(finished(const QString&)), this, SLOT(finished(const QString&)));
	}
	
	we[0]->setup(0U);/** setup everything else (fifos...) */
}

void processing_chain::destroy(){
	for(unsigned int i=0;i<we_count;i++)
		delete we[i];
	free(we);
	we_count=0;
}

int processing_chain::getSetByName(QSettings &settings, const QString &setName)
{
    QDEBUG("setName");
    int set=-1;
    unsigned int sets=settings.value("chain/sets",1).toInt();
    for(unsigned int i=0;i<sets;i++){
        QString name=settings.value(QString("chain/set/%1/name").arg(i),"").toString();
        if(!name.isEmpty()){
            if(name==setName){
                set=i;
                break;
            }
        }
    }
	return set;
}

bool processing_chain::findGroupByType(const QString &type, QSettings &settings, QString &group, const QString &setName){
    int set = getSetByName(settings, setName);
    if(set<0){
        return false;
    }
    return findGroupByType(type, settings, group, set);
}


bool processing_chain::findGroupByType(const QString &type, QSettings &settings, QString &group, unsigned int set){
    QDEBUG(set);
    bool found=false;
    QStringList groups = settings.childGroups();
    QStringList::const_iterator constIterator;
    group.clear();
    for (constIterator = groups.constBegin(); (constIterator != groups.constEnd()); ++constIterator){
        settings.beginGroup(*constIterator);
        if(settings.value("type").toString()==type){
            if((*constIterator).endsWith(QString(":%1").arg(set))){
                found=true;
                group.append(*constIterator);
            }
        }
        settings.endGroup();
        if(found)break;
    }
    if(found==false){
        for (constIterator = groups.constBegin(); (constIterator != groups.constEnd()); ++constIterator){
            settings.beginGroup(*constIterator);
            if(settings.value("type").toString()==type){
                found=true;
                group.append(*constIterator);
            }
            settings.endGroup();
            if(found)break;
        }
    }
    return found;
}


bool processing_chain::reconfigure(QSettings& settings, unsigned int set){
    QDEBUG(set);
  //if(this->set!=set){
		this->destroy();
		this->create(settings,set);
  //}
  return true;
}

/*
 * returns true if setName is valid, and configuration is most likely to be changed (if set differs)
 */
bool processing_chain::reconfigure(QSettings& settings,const QString& setName){
    QDEBUG("");
	bool result=false;
    int setNumber=getSetByName(settings,setName);
	if(setNumber>=0){
		result=true;
		unsigned int set=(unsigned int)setNumber;
        QDEBUG(set);

                //if(this->set!=set){
			this->destroy();
			this->create(settings,set);
                //}
	}
	return result;
}

void processing_chain::run(){
    QDEBUG("");
	if(we!=NULL){
		if(we[0]!=NULL){
			we[0]->run_chain();
		}
	}
	this->running=true;
}

void processing_chain::stop(const QString& sender){
    QDEBUG(sender);
	this->running=checkRunningThread();//TODO: check where this variable is set wrong...
	if(this->running)
	{
		for(int i=0;i<we_count;i++)
		{
			we[i]->stop();
		}
        //wait for signal finishedAll to be emitted, see beow, not implemented well!
		QEventLoop loop;
        QObject::connect(this,SIGNAL(finishedAll(int)),&loop,SLOT(quit()));
		loop.exec();
	}
}

bool processing_chain::checkRunningThread(){
    QDEBUG(we_count);
	for(int i=0;i<we_count;i++)
	{
		if(we[i]->isRunning()){
			return true;
		}
	}
	return false;
}

void processing_chain::finished(const QString& sender){
    QDEBUG(sender);
// 	std::cerr << qPrintable(QObject::sender()->objectName()) << std::endl;
    this->stop(sender);
	//stop the rest..
	if(!checkRunningThread())
	{
		running=false;
        emit finishedAll(tag);
	}
}
