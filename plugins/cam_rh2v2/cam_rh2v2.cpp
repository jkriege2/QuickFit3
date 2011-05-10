#include <QtGui>
#include <QtPlugin>
#include <iostream>

#include "cam_rh2v2.h"



QFExtensionCameraRh2v2::QFExtensionCameraRh2v2(QObject* parent):
    QObject(parent)
{
	logService=NULL;
	settings_pc = new QSettings(QString(":/cam_rh2v2.pc.ini"), QSettings::IniFormat );/** read ini file */
	
	pc=new processing_chain(settings_pc,1);
	
	QObject::connect(pc, SIGNAL(log_txt(QString)), this, SLOT(logger_txt(QString)));
	QObject::connect(pc, SIGNAL(log_wrn(QString)), this, SLOT(logger_wrn(QString)));
	QObject::connect(pc, SIGNAL(log_err(QString)), this, SLOT(logger_err(QString)));
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
	services->log_global_text(tr("%1initializing extension '%2' ... [OK]\n").arg(LOG_PREFIX).arg(getName()));
}

void QFExtensionCameraRh2v2::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionCameraRh2v2::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

	}

QString& QFExtensionCameraRh2v2::findGroupByType(const QString &t){
	QStringList groups = settings_pc->childGroups();
	QStringList::const_iterator constIterator;
	
	bool found=false;
	QString result;
	for (constIterator = groups.constBegin(); (constIterator != groups.constEnd()); ++constIterator){
		settings_pc->beginGroup(*constIterator);
		if(settings_pc->value("type").toString()==t)
		{
			found=true;
			result.append(*constIterator);
		}
		settings_pc->endGroup();
		if(found)break;
	}
	return result;
}


unsigned int QFExtensionCameraRh2v2::getCameraCount() {
    return 1;
	/* how man cameras may be accessed by your plugin (e.g. if you use one driver to access several cameras */
}
void QFExtensionCameraRh2v2::useCameraSettings(unsigned int camera, const QSettings& settings) {
	//load settings from global settings object
	
	settings_pc->setValue(findGroupByType("we_writer")+"/config/filename",settings.value(QString("rh2v2/filename")));
	settings_pc->setValue(findGroupByType("we_accumulator")+"/config/count",settings.value(QString("rh2v2/acccnt")));
	
	log_text(settings_pc->value("writer/config/filename").toString());
	log_text(settings_pc->value("accumulator/config/count").toString());
	
	//TODO:for future use, must be called befor acquire, otherwise fifo empty->lock 
	//pc->run();


	//configure chain - maybe by recreating it...
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
	
	QEnhancedLineEdit* edtFilename=new QEnhancedLineEdit(dlg);
	formlayout->addRow(tr("output filename:"), edtFilename);
	JKStyledButton* btnSelect=new JKStyledButton(JKStyledButton::SelectFile, edtFilename, dlg);
	edtFilename->addButton(btnSelect);
	edtFilename->setText(settings.value("rh2v2/filename","/tmp/test.dat").toString());

	
	lay->addLayout(formlayout);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, dlg);
	lay->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

	if ( dlg->exec()==QDialog::Accepted ) {
			//  read back values entered into the widgets and store in settings
			settings.setValue(QString("rh2v2/acccnt"),sbAccCnt->value());
			settings.setValue(QString("rh2v2/duration"),sbDuration->value());
			settings.setValue(QString("rh2v2/filename"),edtFilename->text());
	}
	delete dlg;
}

int QFExtensionCameraRh2v2::getImageWidth(unsigned int camera) {
    return 32;
}

int QFExtensionCameraRh2v2::getImageHeight(unsigned int camera) {
    return 32;
}

bool QFExtensionCameraRh2v2::isConnected(unsigned int camera) {
    return true;
}

bool QFExtensionCameraRh2v2::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    if (timestamp!=NULL) {
        *timestamp=(uint64_t) 0;
    }
	//TODO: the following should be checked prior to execution...
	if(pc->isRunning()){
		we_endpoint *we_ep = pc->find_first<we_endpoint>();
		if(we_ep!=NULL){
			return we_ep->get_frame(data);
		}
	}
	return false;
}

bool QFExtensionCameraRh2v2::connectDevice(unsigned int camera) {
	pc->run();
	return true;
}

void QFExtensionCameraRh2v2::disconnectDevice(unsigned int camera) {
    /* disconnect from the given camera */
	pc->stop();
}

double QFExtensionCameraRh2v2::getExposureTime(unsigned int camera) {
    return 0.000010;
}

bool QFExtensionCameraRh2v2::startAcquisition(unsigned int camera, QString filenamePrefix) {
    return false;
}

bool QFExtensionCameraRh2v2::cancelAcquisition(unsigned int camera) {
    return true;
}

bool QFExtensionCameraRh2v2::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return false;
}

void QFExtensionCameraRh2v2::getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters) {
}

bool QFExtensionCameraRh2v2::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
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
