#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include <QDebug>
#include "cam_rh2v2.h"



QFExtensionCameraRh2v2::QFExtensionCameraRh2v2(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    bitfileMaster="";
    bitfileSlave="";
    autoflashbitfileMaster="";
    autoflashbitfileSlave="";
    autoflash=false;
    retries =10;
    retryDelay=1000;
  cameraSetting=(QFExtensionCameraRh2v2::cameraSettings*)calloc(2,sizeof(struct cameraSettings));

  cameraSetting[0].prefix=new QString("Radhard2");
  cameraSetting[0].settings_pc = new QSettings(QString(":/cam_rh2v2.pc.ini"), QSettings::IniFormat );/** read ini file */
  cameraSetting[0].pc=new processing_chain(cameraSetting[0].settings_pc,*cameraSetting[0].prefix+QString("_preview"));
  cameraSetting[0].xRes=32;
  cameraSetting[0].yRes=32;
  cameraSetting[0].pixelWidth=30;
  cameraSetting[0].pixelHeight=30;
  cameraSetting[0].exposureTime=0.000010;

  QObject::connect(cameraSetting[0].pc, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
  QObject::connect(cameraSetting[0].pc, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
  QObject::connect(cameraSetting[0].pc, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));

  cameraSetting[1].prefix=new QString("Multisensor");
  cameraSetting[1].settings_pc = new QSettings(QString(":/cam_rh2v2.pc.ini"), QSettings::IniFormat );/** read ini file */
  cameraSetting[1].pc=new processing_chain(cameraSetting[1].settings_pc,*cameraSetting[1].prefix+QString("_preview"));
  cameraSetting[1].xRes=128;
  cameraSetting[1].yRes=128;
  cameraSetting[1].pixelWidth=30;
  cameraSetting[1].pixelHeight=30;
  cameraSetting[1].exposureTime=0.0001;
	
  QObject::connect(cameraSetting[1].pc, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
  QObject::connect(cameraSetting[1].pc, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
  QObject::connect(cameraSetting[1].pc, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
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


bool QFExtensionCameraRh2v2::flashFPGA(QString bitfile, char fpga, QString& messageOut, int retries, int retryDelayMS) {
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
    QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);
    settings.setValue("radhard2/bitfileSlave", bitfileSlave);
    settings.setValue("radhard2/bitfileMaster", bitfileMaster);
    settings.setValue("radhard2/autoflashbitfileMaster", autoflashbitfileMaster);
    settings.setValue("radhard2/autoflashbitfileSlave", autoflashbitfileSlave);
    settings.setValue("radhard2/autoflash", autoflash);
    settings.setValue("radhard2/retries", retries);
    settings.setValue("radhard2/retryDelay", retryDelay);
}

QString& QFExtensionCameraRh2v2::findGroupByType(const QString &t, const unsigned int camera){
  QStringList groups = cameraSetting[camera].settings_pc->childGroups();
	QStringList::const_iterator constIterator;
	
	bool found=false;
	QString result;
	for (constIterator = groups.constBegin(); (constIterator != groups.constEnd()); ++constIterator){
    cameraSetting[camera].settings_pc->beginGroup(*constIterator);
    if(cameraSetting[camera].settings_pc->value("type").toString()==t)
		{
			found=true;
			result.append(*constIterator);
		}
    cameraSetting[camera].settings_pc->endGroup();
		if(found)break;
	}
	return result;
}


unsigned int QFExtensionCameraRh2v2::getCameraCount() {
    return 2;
	/* how man cameras may be accessed by your plugin (e.g. if you use one driver to access several cameras */
}

bool QFExtensionCameraRh2v2::reconfigure(unsigned int camera, const QSettings& settings, const QString& setName){
  cameraSetting[camera].settings_pc->setValue(findGroupByType("we_accumulator",camera)+"/config/count",settings.value(QString("rh2v2/acccnt")));
  cameraSetting[camera].settings_pc->setValue(findGroupByType("we_writer",camera)+"/config/duration",settings.value(QString("rh2v2/duration")));
  return cameraSetting[camera].pc->reconfigure(cameraSetting[camera].settings_pc,setName);
}


bool QFExtensionCameraRh2v2::reconfigure(unsigned int camera, const QSettings& settings, unsigned int set){
  cameraSetting[camera].settings_pc->setValue(findGroupByType("we_accumulator",camera)+"/config/count",settings.value(QString("rh2v2/acccnt")));
  cameraSetting[camera].settings_pc->setValue(findGroupByType("we_writer",camera)+"/config/duration",settings.value(QString("rh2v2/duration")));
  cameraSetting[camera].pc->reconfigure(cameraSetting[camera].settings_pc,set);
	return true;
}	

void QFExtensionCameraRh2v2::reconfigure2(unsigned int camera, const QSettings& settings, const QString& postfix){
  if(!reconfigure(camera,settings,*cameraSetting[camera].prefix+QString("_")+postfix)){
		QMessageBox msgBox;
    msgBox.setText("Unable to rebuilt chain! ("+*cameraSetting[camera].prefix+"_"+postfix+")");
    msgBox.setDetailedText("Perhaps the name does not exist?!?\nNo changes to the chain are applied.");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
	}
}

void QFExtensionCameraRh2v2::useCameraSettings(unsigned int camera, const QSettings& settings) {
  cameraSetting[camera].pc->stop();
	reconfigure2(camera,settings,"preview");
  cameraSetting[camera].pc->run();
}

void QFExtensionCameraRh2v2::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/


	/////////////////////////////////////////////////////////////////////////////////
	// if you want the settings dialog to be modal, you may uncomment the next lines
	// and add implementations
	/////////////////////////////////////////////////////////////////////////////////
	QDialog* dlg=new QDialog(parent);

	QVBoxLayout* lay=new QVBoxLayout(dlg);
	dlg->setLayout(lay);

	QFormLayout* formlayout=new QFormLayout(dlg);


	//  create your widgets here, do not to initialize them with the current settings
	// QWidget* widget=new QWidget(dlg);
	// lay->addRow(tr("Name"), widget);
	// lay->setValue(settings.value(QString("device/name%1").arg(camera), devfaultValue ).toInt());
	QSpinBox* sbAccCnt=new QSpinBox(dlg);
	sbAccCnt->setMinimum(1);
	sbAccCnt->setMaximum(16384);
	sbAccCnt->setValue(settings.value("rh2v2/acccnt", 1024).toUInt());
	formlayout->addRow(tr("Frame Accumulation Count"), sbAccCnt);

	QSpinBox* sbDuration=new QSpinBox(dlg);
	sbDuration->setMinimum(1);
	sbDuration->setMaximum(128*1024*16);
	sbDuration->setValue(settings.value("rh2v2/duration", 128*1024).toUInt());
	formlayout->addRow(tr("Number of frames to read-out ()"), sbDuration);
	
	lay->addLayout(formlayout);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, dlg);
	lay->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

	if ( dlg->exec()==QDialog::Accepted ) {
			//  read back values entered into the widgets and store in settings
			settings.setValue(QString("rh2v2/acccnt"),sbAccCnt->value());
			settings.setValue(QString("rh2v2/duration"),sbDuration->value());
	}
	delete dlg;
}

int QFExtensionCameraRh2v2::getImageWidth(unsigned int camera) {
  return cameraSetting[camera].xRes;
}

int QFExtensionCameraRh2v2::getImageHeight(unsigned int camera) {
  return cameraSetting[camera].yRes;
}

double QFExtensionCameraRh2v2::getPixelWidth(unsigned int camera) {
  return cameraSetting[camera].pixelWidth;
}

double QFExtensionCameraRh2v2::getPixelHeight(unsigned int camera) {
  return cameraSetting[camera].pixelHeight;
}
QString QFExtensionCameraRh2v2::getCameraName(unsigned int camera){
  return *cameraSetting[camera].prefix;
}

QString QFExtensionCameraRh2v2::getCameraSensorName(unsigned int camera){
  return QString("");
}

bool QFExtensionCameraRh2v2::isConnected(unsigned int camera) {
    return true;
}

bool QFExtensionCameraRh2v2::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    if (timestamp!=NULL) {
        *timestamp=(uint64_t) 0;
    }
	//TODO: the following should be checked prior to execution...
  if(cameraSetting[camera].pc->isRunning()){
    we_endpoint *we_ep = cameraSetting[camera].pc->find_first<we_endpoint>();
		if(we_ep!=NULL){
			return we_ep->get_frame(data);
		}
	}
	return false;
}

bool QFExtensionCameraRh2v2::connectDevice(unsigned int camera) {
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
                return false;
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
                return false;
            }

        }
    } else {
        if (autoflash) log_warning(tr("could not flash Radhard2 FPGAs, as bit file '%1' or '%2' does not exist!\n").arg(autoflashbitfileMaster).arg(autoflashbitfileSlave));
    }
    return true;
}

void QFExtensionCameraRh2v2::disconnectDevice(unsigned int camera) {
    /* disconnect from the given camera */
  cameraSetting[camera].pc->stop();
}

double QFExtensionCameraRh2v2::getExposureTime(unsigned int camera) {
    return cameraSetting[camera].exposureTime;
}

bool QFExtensionCameraRh2v2::startAcquisition(unsigned int camera) {
    cameraSetting[camera].pc->run();
    return true;
}

void QFExtensionCameraRh2v2::cancelAcquisition(unsigned int camera) {
	
}

bool QFExtensionCameraRh2v2::prepareAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
  cameraSetting[camera].pc->stop();
  cameraSetting[camera].settings_pc->setValue(findGroupByType("we_writer",camera)+"/config/filename",filenamePrefix+".dat");
  //qDebug()<<"********"<<findGroupByType("we_writer",camera)+"/config/filename";
  reconfigure(camera,settings,*cameraSetting[camera].prefix+QString("_acquisition"));
  cameraSetting[camera].pc->reconfigure(cameraSetting[camera].settings_pc,*cameraSetting[camera].prefix+QString("_acquisition"));
	return true;
}

bool QFExtensionCameraRh2v2::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return cameraSetting[camera].pc->isRunning();
}

void QFExtensionCameraRh2v2::getAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::AcquititonFileDescription>* files, QMap<QString, QVariant>* parameters) {
    QFExtensionCamera::AcquititonFileDescription fd;
    fd.name=cameraSetting[camera].settings_pc->value(findGroupByType("we_writer",camera)+"/config/filename", "").toString();
    fd.type=QString("RADHARD2RAW");
    fd.description=tr("raw data from Radhard2");
    if (files) files->append(fd);


    (*parameters)["sequence_length"]=cameraSetting[camera].settings_pc->value(findGroupByType("we_writer",camera)+"/config/duration", "").toInt();
    (*parameters)["image_width"]=getImageWidth(camera);
    (*parameters)["image_height"]=getImageHeight(camera);
    (*parameters)["pixel_width"]=getPixelWidth(camera);
    (*parameters)["pixel_height"]=getPixelHeight(camera);
    (*parameters)["frame_time"]=1e-5;
}

bool QFExtensionCameraRh2v2::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

int QFExtensionCameraRh2v2::getAcquisitionProgress(unsigned int camera) {
    return 0;
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
