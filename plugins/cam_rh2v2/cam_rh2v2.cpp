#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include <QDebug>
#include "cam_rh2v2.h"
#include "cam_rh2v2_cfgdlg.h"
#include "jkqtfastplotter.h"

#ifdef DEBUG
#define QDEBUG(s) qDebug()<<"><"<<__FUNCTION__<<"@"<<__LINE__<<s;
#else
#define QDEBUG(s)
#endif

QFExtensionCameraRh2v2::QFExtensionCameraRh2v2(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    services=NULL;
    bitfileMaster="";
    bitfileSlave="";
    autoflashbitfileMaster="";
    autoflashbitfileSlave="";
    autoflash=false;
    retries =10;
    retryDelay=1000;
    connected=false;
  cameraSetting=(QFExtensionCameraRh2v2::cameraSettings*)calloc(2,sizeof(struct cameraSettings));

  cameraSetting[0].prefix=new QString("Radhard2");
  cameraSetting[0].raw.settings = new QSettings(QString(":/cam_rh2v2.raw.ini"), QSettings::IniFormat );/** read ini file */
  cameraSetting[0].raw.pc=new processing_chain(*(cameraSetting[0].raw.settings),*cameraSetting[0].prefix+QString("_preview"));
  cameraSetting[0].raw.pc->setTag(0);
  cameraSetting[0].raw.enabled = true;
  cameraSetting[0].cor.settings = new QSettings(QString(":/cam_rh2v2.cor.ini"), QSettings::IniFormat );/** read ini file */
  cameraSetting[0].cor.pc = new processing_chain(*(cameraSetting[0].cor.settings),*cameraSetting[0].prefix+QString("_preview"));
  cameraSetting[0].cor.pc->setTag(0);
  cameraSetting[0].cor.enabled=true;
  cameraSetting[0].xRes=32;
  cameraSetting[0].yRes=32;
  cameraSetting[0].pixelWidth=30;
  cameraSetting[0].pixelHeight=30;
  cameraSetting[0].exposureTime=0.000010;
  cameraSetting[0].cordlg=new cam_rh2v2_cordlg(NULL);
  cameraSetting[0].cordlg->setCamera(0);

  QObject::connect(cameraSetting[0].raw.pc, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
  QObject::connect(cameraSetting[0].raw.pc, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
  QObject::connect(cameraSetting[0].raw.pc, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
  QObject::connect(cameraSetting[0].cor.pc, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
  QObject::connect(cameraSetting[0].cor.pc, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
  QObject::connect(cameraSetting[0].cor.pc, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
  QObject::connect(cameraSetting[0].raw.pc, SIGNAL(finishedAll(int)), this, SLOT(acquisitionFinished(int)));
  QObject::connect(cameraSetting[0].cor.pc, SIGNAL(finishedAll(int)), this, SLOT(acquisitionFinished(int)));
  QObject::connect(cameraSetting[0].cordlg,SIGNAL(reconfigure(int)), this, SLOT(reconfigureDialog(int)));


  cameraSetting[1].prefix=new QString("Multisensor");
  cameraSetting[1].raw.settings = new QSettings(QString(":/cam_rh2v2.raw.ini"), QSettings::IniFormat );/** read ini file */
  cameraSetting[1].raw.pc=new processing_chain(*(cameraSetting[1].raw.settings),*cameraSetting[1].prefix+QString("_preview"));
  cameraSetting[1].raw.pc->setTag(1);
  cameraSetting[1].raw.enabled = true;
  cameraSetting[1].cor.settings = NULL;
  cameraSetting[1].cor.pc = NULL;
  cameraSetting[1].cor.enabled = false;
  cameraSetting[1].xRes=128;
  cameraSetting[1].yRes=128;
  cameraSetting[1].pixelWidth=30;
  cameraSetting[1].pixelHeight=30;
  cameraSetting[1].exposureTime=0.0001;
  cameraSetting[1].cordlg=new cam_rh2v2_cordlg(NULL);
  cameraSetting[1].cordlg->setCamera(1);

  QObject::connect(cameraSetting[1].raw.pc, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
  QObject::connect(cameraSetting[1].raw.pc, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
  QObject::connect(cameraSetting[1].raw.pc, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
  QObject::connect(cameraSetting[1].raw.pc, SIGNAL(finishedAll(int)), this, SLOT(acquisitionFinished(int)));
  QObject::connect(cameraSetting[1].cordlg,SIGNAL(reconfigure(int)), this, SLOT(reconfigureDialog(int)));

}

QFExtensionCameraRh2v2::~QFExtensionCameraRh2v2() {

}


void QFExtensionCameraRh2v2::deinit() {
	/* add code for cleanup here */
}

void QFExtensionCameraRh2v2::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionCameraRh2v2::initExtension() {
    QDEBUG("")
    services->log_global_text(tr("%2initializing extension '%1' ...\n").arg(getName()).arg(LOG_PREFIX));
    bitfileMaster=autoflashbitfileMaster=services->getOptions()->getAssetsDirectory()+"/plugins/"+getID()+"/radhard2_top_cell_master.bit";
    bitfileSlave=autoflashbitfileSlave=services->getOptions()->getAssetsDirectory()+"/plugins/"+getID()+"/radhard2_top_cell_slave.bit";
    actProgramFPGA=new QAction(QIcon(":/cam_radhard2/flash.png"), tr("Flash FPGAs"), this);
    connect(actProgramFPGA, SIGNAL(triggered()), this, SLOT(programFPGA()));
    QMenu* extm=services->getMenu("extensions");
    if (extm) {
        QMenu* subMenu=extm->addMenu(QIcon(":/cam_radhard2_logo.png"), tr("Radhard 2 V2 Tools"));
        subMenu->addAction(actProgramFPGA);
    }

    loadSettings(NULL);

    services->log_global_text(tr("%2initializing extension '%1' ... DONE\n").arg(getName()).arg(LOG_PREFIX));
}



void QFExtensionCameraRh2v2::programFPGA() {
    QDEBUG("")
    QFRadhard2FlashtoolV2* dlg=new QFRadhard2FlashtoolV2(this, NULL);
    loadSettings(NULL);

    dlg->setBitfileMaster(bitfileMaster);
    dlg->setBitfileSlave(bitfileSlave);
    dlg->setAutoBitfileMaster(autoflashbitfileMaster);
    dlg->setAutoBitfileSlave(autoflashbitfileSlave);
    dlg->setAutoFlash(autoflash);
    dlg->setRetries(retries);
    dlg->setRetryDelayMS(retryDelay);

    dlg->exec();

    bitfileMaster=dlg->bitfileMaster();
    bitfileSlave=dlg->bitfileSlave();
    autoflashbitfileMaster=dlg->autoBitfileMaster();
    autoflashbitfileSlave=dlg->autoBitfileSlave();
    retries=dlg->retries();
    autoflash=dlg->autoflash();
    retryDelay=dlg->retryDelayMS();

    storeSettings(NULL);
    delete dlg;
}

void QFExtensionCameraRh2v2::acquisitionFinished(int camera){
    QDEBUG("")
    if(!(camera<0)){
        if(cameraSetting[camera].raw.pc->isRunning()){
            cameraSetting[camera].raw.pc->stop(QString("UI"));
        }
        if(cameraSetting[camera].cor.pc!=NULL){
            if(cameraSetting[camera].cor.pc->isRunning()){
                sleep(2);
                cameraSetting[camera].cor.pc->stop(QString("UI"));
            }
        }
    }
}


bool QFExtensionCameraRh2v2::flashFPGA(QString bitfile, char fpga, QString& messageOut, int retries, int retryDelayMS) {
    QDEBUG("")
    messageOut="";
    int res=0;
    int i=0;
    QString name="master";
    if (fpga=='S' || fpga=='s') name="slave";
    while ((i<retries) && (res==0)) {
        char message[8192];
        res=flash_bitfile(bitfile.toAscii().data(), message, fpga);
        if (i>0) messageOut+="\n\n";
        messageOut += tr("try %4 %1/%2:\n%3").arg(i+1).arg(retries).arg(message).arg(name);
        i++;
        QTime time;
        time.start();
        while (time.elapsed()<retryDelayMS) {
            QApplication::processEvents();
        }
    }
    return res!=0;
}

void QFExtensionCameraRh2v2::loadSettings(ProgramOptions* settingspo) {
    QDEBUG("")
    QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);
    bitfileSlave=settings.value("radhard2/bitfileSlave", bitfileSlave).toString();
    bitfileMaster=settings.value("radhard2/bitfileMaster", bitfileMaster).toString();
    autoflashbitfileMaster=settings.value("radhard2/autoflashbitfileMaster", autoflashbitfileMaster).toString();
    autoflashbitfileSlave=settings.value("radhard2/autoflashbitfileSlave", autoflashbitfileSlave).toString();
    autoflash=settings.value("radhard2/autoflash", autoflash).toBool();
    retries=settings.value("radhard2/retries", retries).toInt();
    retryDelay=settings.value("radhard2/retryDelay", retryDelay).toInt();

}

void QFExtensionCameraRh2v2::storeSettings(ProgramOptions* settingspo) {
    QDEBUG("")
    QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);
    settings.setValue("radhard2/bitfileSlave", bitfileSlave);
    settings.setValue("radhard2/bitfileMaster", bitfileMaster);
    settings.setValue("radhard2/autoflashbitfileMaster", autoflashbitfileMaster);
    settings.setValue("radhard2/autoflashbitfileSlave", autoflashbitfileSlave);
    settings.setValue("radhard2/autoflash", autoflash);
    settings.setValue("radhard2/retries", retries);
    settings.setValue("radhard2/retryDelay", retryDelay);
}

bool QFExtensionCameraRh2v2::findGroupByType(const QString &t, const unsigned int camera, QString &group){
    QDEBUG("")
    QStringList groups = cameraSetting[camera].raw.settings->childGroups();
	QStringList::const_iterator constIterator;
    group.clear();
	bool found=false;
	for (constIterator = groups.constBegin(); (constIterator != groups.constEnd()); ++constIterator){
        cameraSetting[camera].raw.settings->beginGroup(*constIterator);
        if(cameraSetting[camera].raw.settings->value("type").toString()==t){
            found=true;
            group.append(*constIterator);
        }
        cameraSetting[camera].raw.settings->endGroup();
        if(found)break;
    }
    return found;
}

bool QFExtensionCameraRh2v2::findGroupByType(const QString &t, QSettings &settings, QString &group){
    QDEBUG("")
    QStringList groups = settings.childGroups();
    QStringList::const_iterator constIterator;
    group.clear();
    bool found=false;
    for (constIterator = groups.constBegin(); (constIterator != groups.constEnd()); ++constIterator){
        settings.beginGroup(*constIterator);
        if(settings.value("type").toString()==t){
            found=true;
            group.append(*constIterator);
        }
        settings.endGroup();
        if(found)break;
    }
    return found;
}


unsigned int QFExtensionCameraRh2v2::getCameraCount() const {
    return 2;
	/* how man cameras may be accessed by your plugin (e.g. if you use one driver to access several cameras */
}

bool QFExtensionCameraRh2v2::setConfiguration(unsigned int camera, const QSettings& settings, const QString& setName){
    QDEBUG("")
    int setRaw = cameraSetting[camera].raw.pc->getSetByName(*(cameraSetting[camera].raw.settings),setName);
    if(setRaw<0){
        qDebug()<<"ERROR: Set number failed!";
        return false;
    }
    QString we;
    bool result=true;
    bool res;

    int duration = settings.value(QString("rh2v2/duration")).toInt();
    int binning = settings.value(QString("rh2v2/binning")).toUInt();
    int roi=settings.value(QString("rh2v2/ROIend")).toUInt()-settings.value(QString("rh2v2/ROIstart")).toUInt()+1;
    int frameSize=4+2+4*roi;
    float frameTime=settings.value(QString("rh2v2/frameTime")).toFloat();
    int delayCounter=ceil(frameTime*480.0/10.0-roi*4.0);
    if(delayCounter<0){
        delayCounter=0;
    }
    QStringList cfg;
    cfg << QString("0x") + QString::number(0x1000+delayCounter,16);
    cfg << QString("0x") + QString::number(0x2000+settings.value(QString("rh2v2/ROIstart")).toUInt(),16);
    cfg << QString("0x") + QString::number(0x3000+settings.value(QString("rh2v2/ROIend")).toUInt(),16);
    cfg << QString("0x") + QString::number(0x4000+(binning-1)/2,16);
    cfg << QString("0x") + QString::number(0x0001,16);//START


    res = cameraSetting[camera].raw.pc->findGroupByType("we_accumulate", *(cameraSetting[camera].raw.settings), we, setRaw);
    result&=res;
    if(res==true){
        cameraSetting[camera].raw.settings->setValue(we+"/config/count",settings.value(QString("rh2v2/acccnt")));
    }

    res = cameraSetting[camera].raw.pc->findGroupByType("we_writer", *(cameraSetting[camera].raw.settings), we, setRaw);
    result&=res;
    if(res==true){
        cameraSetting[camera].raw.settings->setValue(we+"/config/duration",duration*binning);
    }

    res = cameraSetting[camera].raw.pc->findGroupByType(QString("we_frame_analyzer"),*(cameraSetting[camera].raw.settings), we, setRaw);
    result&=res;
    if(res==true){
        cameraSetting[camera].raw.settings->setValue(we+QString("/frame_res_x"),frameSize);
        cameraSetting[camera].raw.settings->setValue(we+QString("/config/frame_size"),frameSize);
    }

    res = cameraSetting[camera].raw.pc->findGroupByType(QString("we_frame_gen"),*(cameraSetting[camera].raw.settings), we, setRaw);
    result&=res;
    if(res==true){
        cameraSetting[camera].raw.settings->setValue(we+QString("/frame_res_y"),roi);
    }

    res = cameraSetting[camera].raw.pc->findGroupByType(QString("we_reader_usb_rh2"),*(cameraSetting[camera].raw.settings),we, setRaw);
    result&=res;
    if(res==true){
        cameraSetting[camera].raw.settings->setValue(we+"/config/init",cfg);
    }

    cameraSetting[camera].yRes=roi;
    cameraSetting[camera].exposureTime=settings.value(QString("rh2v2/acccnt")).toFloat()*frameTime*0.000001;

    //Correlator hardware
    if(cameraSetting[camera].cor.settings!=NULL){
        cameraSetting[camera].cor.enabled=settings.value("rh2v2/doCorrelation",true).toBool();

        int setCor = cameraSetting[camera].raw.pc->getSetByName(*(cameraSetting[camera].cor.settings),setName);
        if(setCor<0){
            return result;
        }
        QStringList cfg_cor;
        uint32_t creCnt = duration*roi/(1024*32);
        cfg_cor << QString("0x") + QString::number(0xF800+roi-1,16);//CorrPU: roi
        cfg_cor << QString("0x") + QString::number(0xF102,16);//CRE: shot
        cfg_cor << QString("0x") + QString::number(0xF400+( creCnt     &0xff),16);//CRE: counter(0)
        cfg_cor << QString("0x") + QString::number(0xF500+((creCnt>> 8)&0xff),16);//CRE: counter(1)
        cfg_cor << QString("0x") + QString::number(0xF600+((creCnt>>16)&0xff),16);//CRE: counter(2)
        cfg_cor << QString("0x") + QString::number(0xF700+((creCnt>>24)&0xff),16);//CRE: counter(3)
        cfg_cor << QString("0x") + QString::number(0xF001,16);//CRE: run
        res = cameraSetting[camera].raw.pc->findGroupByType(QString("we_reader_usb_rh2"),*(cameraSetting[camera].cor.settings),we, setCor);
        result&=res;
        if(res==true){
            cameraSetting[camera].cor.settings->setValue(we+"/config/init",cfg_cor);
        }
    }

    if(result!=true)qDebug()<<__FILE__<<"@"<<__LINE__<<":"<<"Something failed!!!";
    return result;
}

bool QFExtensionCameraRh2v2::reconfigure(unsigned int camera, const QSettings& settings, const QString& setName){
    QDEBUG("")
    setConfiguration(camera,settings,setName);
    bool result=true;
    result&=cameraSetting[camera].raw.pc->reconfigure(*(cameraSetting[camera].raw.settings),setName);
    if(cameraSetting[camera].cor.pc!=NULL){
        result&=cameraSetting[camera].cor.pc->reconfigure(*(cameraSetting[camera].cor.settings),setName);
    }
    return result;
}

void QFExtensionCameraRh2v2::reconfigure2(unsigned int camera, const QSettings& settings, const QString& postfix){
    QDEBUG("")
  if(!reconfigure(camera,settings,*cameraSetting[camera].prefix+QString("_")+postfix)){
		QMessageBox msgBox;
        msgBox.setText("Unable to rebuilt chain! ("+*cameraSetting[camera].prefix+"_"+postfix+")");
        msgBox.setDetailedText("Perhaps the name does not exist?!?\nNo changes to the chain are applied.");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
	}
    QStringList init=cameraSetting[camera].raw.settings->allKeys();
}


void QFExtensionCameraRh2v2::reconfigureDialog(int camera){
    if(cameraSetting[camera].cor.pc!=NULL){
        if(cameraSetting[camera].cor.pc->isRunning()){
            we_correlator_hw *we_chw = cameraSetting[camera].cor.pc->find_first<we_correlator_hw>();
            if(we_chw!=NULL){
                double **results = cameraSetting[camera].cordlg->getData();
                double *taus = cameraSetting[camera].cordlg->getTaus();
                we_chw->setData(results);
                we_chw->setTaus(taus);
                JKQTFPLinePlot **plots=cameraSetting[camera].cordlg->getPlots();
                for(int i=0;i<1024;i++){
                    plots[i]->set_data(&(taus[1]),&(results[i][1]),14*8-1);
                }
                for(int i=0;i<cameraSetting[camera].cordlg->getPlotterCount();i++){
                    JKQTFastPlotter *plotter = cameraSetting[camera].cordlg->getPlotter(i);
                    plotter->set_doDrawing(false);
                    plotter->setXRange(taus[1],taus[14*8-1],true);
                    plotter->set_doDrawing(true);
                    QObject::connect(we_chw, SIGNAL(newData()), plotter, SLOT(update_data()));
                }
            }else{
                qDebug()<<"WARNING: 'we_correlator_hw' not found!";
            }
        }
    }
}


void QFExtensionCameraRh2v2::useCameraSettings(unsigned int camera, const QSettings& settings) {
    QDEBUG("")
    cameraSetting[camera].raw.pc->stop(QString("UI"));
    if(cameraSetting[camera].cor.pc!=NULL){
        cameraSetting[camera].cor.pc->stop(QString("UI"));
    }

    flashFPGA(camera);

    reconfigure2(camera,settings,"preview");
    cameraSetting[camera].raw.pc->run();
    if(cameraSetting[camera].cor.pc!=NULL){
        cameraSetting[camera].cordlg->show();
        cameraSetting[camera].cor.pc->run();
        if(cameraSetting[camera].cor.pc->isRunning()){
            reconfigureDialog(camera);
            we_reader_usb_rh2 *we_ru = cameraSetting[camera].cor.pc->find_first<we_reader_usb_rh2>();
            if(we_ru!=NULL){
                QObject::connect(cameraSetting[camera].cordlg, SIGNAL(btnReset()), we_ru, SLOT(resetCor()));
                QObject::connect(cameraSetting[camera].cordlg, SIGNAL(sndCmd(uint16_t)), we_ru, SLOT(sndCmd(uint16_t)));
            }
        }
    }
}

void QFExtensionCameraRh2v2::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
    QDEBUG("")

    cam_rh2v2_cfgdlg* dlg=new cam_rh2v2_cfgdlg(parent);

    dlg->setEditMode();
    dlg->setAccumulationCount(settings.value("rh2v2/acccnt", 1024).toUInt());
    dlg->setFrameCount(settings.value("rh2v2/duration", 128*1024).toUInt());
    dlg->setRoiFirst(settings.value("rh2v2/ROIstart", 0).toUInt());
    dlg->setRoiLast(settings.value("rh2v2/ROIend", 31).toUInt());
    dlg->setFrameTime(settings.value("rh2v2/frameTime", 10.0).toFloat());
    dlg->setBinning(settings.value("rh2v2/binning",1).toUInt());
    dlg->setCorrEnable(settings.value("rh2v2/doCorrelation",true).toBool());
    dlg->unsetEditMode();

	if ( dlg->exec()==QDialog::Accepted ) {
            settings.setValue(QString("rh2v2/acccnt"),dlg->getAccumulationCount());
            settings.setValue(QString("rh2v2/duration"),dlg->getFrameCount());
            settings.setValue(QString("rh2v2/ROIstart"),dlg->getRoiFirst());
            settings.setValue(QString("rh2v2/ROIend"),dlg->getRoiLast());
            settings.setValue(QString("rh2v2/frameTime"),dlg->getFrameTime());
            settings.setValue(QString("rh2v2/binning"),dlg->getBinning());
            settings.setValue(QString("rh2v2/doCorrelation"),dlg->getCorrEnable());
    }
	delete dlg;
}

int QFExtensionCameraRh2v2::getCameraImageWidth(unsigned int camera) {
  return cameraSetting[camera].yRes;
}

int QFExtensionCameraRh2v2::getImageCameraHeight(unsigned int camera) {
  return cameraSetting[camera].xRes;
}

double QFExtensionCameraRh2v2::getCameraPixelWidth(unsigned int camera) {
  return cameraSetting[camera].pixelWidth;
}

double QFExtensionCameraRh2v2::getCameraPixelHeight(unsigned int camera) {
  return cameraSetting[camera].pixelHeight;
}
QString QFExtensionCameraRh2v2::getCameraName(unsigned int camera){
  return *cameraSetting[camera].prefix;
}

QString QFExtensionCameraRh2v2::getCameraSensorName(unsigned int camera){
  return QString("");
}

bool QFExtensionCameraRh2v2::isCameraConnected(unsigned int camera) {
    return connected;
}

bool QFExtensionCameraRh2v2::acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* timestamp, QMap<QString, QVariant>* parameters) {
    if (timestamp!=NULL) {
        *timestamp=(uint64_t) 0;
    }
    //TODO: the following should be checked prior to execution...
    if(cameraSetting[camera].raw.pc->isRunning()){
        we_endpoint *we_ep = cameraSetting[camera].raw.pc->find_first<we_endpoint>();
        if(we_ep!=NULL){
            return we_ep->get_frame(data);
        }
    }
    return false;
}

bool QFExtensionCameraRh2v2::flashFPGA(unsigned int camera){
    QDEBUG("")
    //quick'n'dirty
    QString flashResult;
    bool ret;
    ret=flashFPGA(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/"+getID()+QString("/fpga1_top.bit"),'M',flashResult,retries,retryDelay);
    if(ret==false){
        log_error(flashResult);
    }else{
        log_text(flashResult);
    }
    sleep(1);
    ret=flashFPGA(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/"+getID()+QString("/fpga2_top.bit"),'S',flashResult,retries, retryDelay);
    if(ret==false){
        log_error(flashResult);
    }else{
        log_text(flashResult);
    }
    sleep(1);
    return true;
}

bool QFExtensionCameraRh2v2::connectCameraDevice(unsigned int camera) {
    QDEBUG("")
    if (autoflash && (QFile(autoflashbitfileMaster).exists() || QFile(autoflashbitfileSlave).exists())) {
        QString flashMessage;
        if (QFile(autoflashbitfileMaster).exists()) {
            log_text(tr("flashing Radhard2 Master FPGA (%1)\n").arg(autoflashbitfileMaster));
            bool ok=flashFPGA(autoflashbitfileMaster, 'm', flashMessage, retries, retryDelay);
            flashMessage.replace('\n', QString("\n%1  ").arg(LOG_PREFIX));
            if (ok) {
                log_text(tr("  %3\nflashing Radhard2 Master FPGA (%1) ... DONE!\n").arg(autoflashbitfileMaster).arg(flashMessage));
            } else {
                log_error(tr("  %3\nflashing Radhard2 Master FPGA (%1) ... ERROR!\n").arg(autoflashbitfileMaster).arg(flashMessage));
                return connected=false;
            }
        }
        if (QFile(autoflashbitfileSlave).exists()) {
            log_text(tr("flashing Radhard2 Slave FPGA (%1)\n").arg(autoflashbitfileSlave));
            bool ok=flashFPGA(autoflashbitfileSlave, 's', flashMessage, retries, retryDelay);
            flashMessage.replace('\n', QString("\n%1  ").arg(LOG_PREFIX));
            if (ok) {
                log_text(tr("  %3\nflashing Radhard2 Slave FPGA (%1) ... DONE!\n").arg(autoflashbitfileSlave).arg(flashMessage));
            } else {
                log_error(tr("  %3\nflashing Radhard2 Slave FPGA (%1) ... ERROR!\n").arg(autoflashbitfileSlave).arg(flashMessage));
                return connected=false;
            }

        }
    } else {
        if (autoflash) log_warning(tr("could not flash Radhard2 FPGAs, as bit file '%1' or '%2' does not exist!\n").arg(autoflashbitfileMaster).arg(autoflashbitfileSlave));
    }
    return connected=true;
}

void QFExtensionCameraRh2v2::disconnectCameraDevice(unsigned int camera) {
    QDEBUG("")
    /* disconnect from the given camera */
  cameraSetting[camera].raw.pc->stop(QString("UI"));
  if(cameraSetting[camera].cor.pc!=NULL){
      cameraSetting[camera].cor.pc->stop(QString("UI"));
  }
  connected=false;
}

double QFExtensionCameraRh2v2::getCameraExposureTime(unsigned int camera) {
    return cameraSetting[camera].exposureTime;
}

bool QFExtensionCameraRh2v2::startCameraAcquisition(unsigned int camera) {
    QDEBUG("")
    if(cameraSetting[camera].cor.enabled==true){
        if(cameraSetting[camera].cor.pc!=NULL){
            cameraSetting[camera].cor.pc->run();
            sleep(2);/*start correlation prior to acquisition*/
        }
    }
    cameraSetting[camera].raw.pc->run();
    return true;
}

void QFExtensionCameraRh2v2::cancelCameraAcquisition(unsigned int camera) {
    acquisitionFinished(camera);
}

bool QFExtensionCameraRh2v2::prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
    QDEBUG("")
    cameraSetting[camera].raw.pc->stop(QString("UI"));
    if(cameraSetting[camera].cor.pc!=NULL){
        cameraSetting[camera].cor.pc->stop(QString("UI"));
    }
    flashFPGA(camera);
    QString we;
    bool result=true,res;

    res=cameraSetting[camera].raw.pc->findGroupByType("we_writer",*(cameraSetting[camera].raw.settings),we,*cameraSetting[camera].prefix+QString("_acquisition"));
    result&=res;
    if(res==true){
        cameraSetting[camera].raw.settings->setValue(we+"/config/filename",filenamePrefix+".raw.dat");
    }
    if(cameraSetting[camera].cor.pc!=NULL){
        res=cameraSetting[camera].cor.pc->findGroupByType("we_writer",*(cameraSetting[camera].cor.settings),we,*cameraSetting[camera].prefix+QString("_acquisition"));
        result&=res;
        if(res==true){
            cameraSetting[camera].cor.settings->setValue(we+"/config/filename",filenamePrefix+".cor.dat");
        }
    }
    reconfigure2(camera,settings,"acquisition");
    return result;
}

bool QFExtensionCameraRh2v2::isCameraAcquisitionRunning(unsigned int camera, double* percentageDone) {
    bool result=false;
    result|=cameraSetting[camera].raw.pc->isRunning();
    if(cameraSetting[camera].cor.pc!=NULL){
        result|=cameraSetting[camera].cor.pc->isRunning();
    }
    return result;
}

void QFExtensionCameraRh2v2::getCameraAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::CameraAcquititonFileDescription>* files, QMap<QString, QVariant>* parameters) {
    QFExtensionCamera::CameraAcquititonFileDescription fd;
    QString we;
    bool res;

    res=cameraSetting[camera].raw.pc->findGroupByType("we_writer",*(cameraSetting[camera].raw.settings),we,*cameraSetting[camera].prefix+QString("_acquisition"));
    if(res==true){
        fd.name=cameraSetting[camera].raw.settings->value(we+"/config/filename", "").toString();
        fd.type=QString("RADHARD2RAW");
        fd.description=tr("raw data from Radhard2");
        if (files) files->append(fd);
    }

    if((cameraSetting[camera].cor.enabled==true)&&(cameraSetting[camera].cor.pc!=NULL)){
        res=cameraSetting[camera].cor.pc->findGroupByType("we_writer",*(cameraSetting[camera].cor.settings),we,*cameraSetting[camera].prefix+QString("_acquisition"));
        if(res==true){
            fd.name=cameraSetting[camera].cor.settings->value(we+"/config/filename", "").toString();
            fd.type=QString("RADHARD2COR");
            fd.description=tr("cor data from Radhard2");
            if (files) files->append(fd);
        }
    }

    (*parameters)["sequence_length"]=cameraSetting[camera].raw.settings->value(we+"/config/duration", "").toInt();
    (*parameters)["image_width"]=getCameraImageWidth(camera);
    (*parameters)["image_height"]=getImageCameraHeight(camera);
    (*parameters)["pixel_width"]=getCameraPixelWidth(camera);
    (*parameters)["pixel_height"]=getCameraPixelHeight(camera);
    (*parameters)["frame_time"]=1e-5;
}

bool QFExtensionCameraRh2v2::getCameraAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

int QFExtensionCameraRh2v2::getCameraAcquisitionProgress(unsigned int camera) {
    return 0;
}

bool QFExtensionCameraRh2v2::isCameraSettingChangable(QFExtensionCamera::CameraSetting which) const  {
    return false;
}

void QFExtensionCameraRh2v2::changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value)  {

}

QVariant QFExtensionCameraRh2v2::getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which) const  {
    return QVariant();
}

QVariant QFExtensionCameraRh2v2::getCurrentCameraSetting(int camera, QFExtensionCamera::CameraSetting which) const
{
    return QVariant();
}

void QFExtensionCameraRh2v2::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFExtensionCameraRh2v2::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFExtensionCameraRh2v2::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}


Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCameraRh2v2)
