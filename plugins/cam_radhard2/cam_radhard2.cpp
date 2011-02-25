#include "cam_radhard2.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include <unistd.h>
#include <time.h>

QFExtensionCameraRadhard2::QFExtensionCameraRadhard2(QObject* parent):
    QObject(parent)
{
    conn=false;
    radhard2=NULL;
    subtractOne=true;
    iterations=100;
    divider=1;
    logService=NULL;
}

QFExtensionCameraRadhard2::~QFExtensionCameraRadhard2() {

}


void QFExtensionCameraRadhard2::deinit() {
	/* add code for cleanup here */
}

void QFExtensionCameraRadhard2::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionCameraRadhard2::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    loadSettings(NULL);
}

void QFExtensionCameraRadhard2::loadSettings(ProgramOptions* settingspo) {
    //QSettings& settings=*(settingspo->getQSettings());
    QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+".ini", QSettings::IniFormat);
    iterations=settings.value("radhard2/iterations", iterations).toInt();
    divider=settings.value("radhard2/divider", divider).toInt();
    subtractOne=settings.value("radhard2/subtract_one", subtractOne).toBool();
}

void QFExtensionCameraRadhard2::storeSettings(ProgramOptions* settingspo) {
    //QSettings& settings=*(settingspo->getQSettings());
    QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);
    settings.setValue("radhard2/iterations", iterations);
    settings.setValue("radhard2/divider", divider);
    settings.setValue("radhard2/subtract_one", subtractOne);
}

unsigned int QFExtensionCameraRadhard2::getCameraCount() {
    return 1;
}

void QFExtensionCameraRadhard2::showSettingsWidget(unsigned int camera, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/
    QDialog* dlg=new QDialog(parent);

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QFormLayout* formlayout=new QFormLayout(dlg);



    QSpinBox* spIterations=new QSpinBox(dlg);
    spIterations->setMinimum(0);
    spIterations->setMaximum(100000);
    spIterations->setValue(iterations);
    formlayout->addRow(tr("iterations:"), spIterations);
    connect(spIterations, SIGNAL(valueChanged(int)), this, SLOT(updateAcquisitionTime()));

    QSpinBox* spDivider=new QSpinBox(dlg);
    spDivider->setMinimum(0);
    spDivider->setMaximum(100000);
    spDivider->setValue(divider);
    formlayout->addRow(tr("divider:"), spDivider);
    connect(spDivider, SIGNAL(valueChanged(int)), this, SLOT(updateAcquisitionTime()));

    QLabel* l=new QLabel(dlg);
    formlayout->addRow(tr("acquisition time:"), l);
    connect(this, SIGNAL(displayAcquisitionTime(const QString&)), l, SLOT(setText(const QString&)));

    QCheckBox* chkSubtract1=new QCheckBox("", dlg);
    chkSubtract1->setChecked(subtractOne);
    formlayout->addRow(tr("subtract offset 1:"), chkSubtract1);


    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets
         iterations=spIterations->value();
         divider=spDivider->value();
         subtractOne=chkSubtract1->isChecked();
         storeSettings(NULL);
         if (isConnected(camera)) {
            sendIterations();
            sendDivider();
         }
    }
    delete dlg;
}

int QFExtensionCameraRadhard2::getImageWidth(unsigned int camera) {
    return 32;
}

int QFExtensionCameraRadhard2::getImageHeight(unsigned int camera) {
    return 32;
}

bool QFExtensionCameraRadhard2::isConnected(unsigned int camera) {
    return conn;
}

bool QFExtensionCameraRadhard2::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    sendIterations();
    sendDivider();

    if ((!conn) || (!radhard2)) {
        log_error(tr("Radhard2: not connected to hardware."));
        return false;
    }
    if (timestamp!=NULL) {
        *timestamp=(uint64_t)round(timer.get_time());
    }

    unsigned int result;
    int exposureT=getExposureTime(camera);

	// Ask for the pictures
    radhard2->SendCommand(CMD_START_ACQUISITION);
    radhard2->ReadData(&result);
	if(result != OPERATION_DONE) {
		log_error(tr("Radhard2: Received wrong ack constant after START_ACQUISITION command.\n"));
    }

    usleep(20000 + 2* int(exposureT));

	// Get the pictures
    radhard2->SendCommand(CMD_GET_MEMORY);
    radhard2->ReadData(data);

    if (subtractOne) {
        register int imagesize=getImageWidth(camera)*getImageHeight(camera);
        for(register int j=0; j<imagesize; j++) {
            data[j] =  data[j]-1;
        }
    }

    return true;
}

bool QFExtensionCameraRadhard2::connectDevice(unsigned int camera) {
    conn=false;
    if (radhard2) delete radhard2;
    radhard2=new Radhard2;
    //radhard2->OpenUsbDevice();
    radhard2->SendCommand(CMD_TEST_COMLINK, 0);
    unsigned int magicn=0;
    radhard2->ReadData(&magicn);
    log_text(tr("Radhard2: connection initialized\nRadhard2: read: MAGIC_NUMBER = %1\n").arg(QString::number(magicn, 16).toUpper()));
    if (magicn!=MAGIC_NUMBER) {
        log_error(tr("Radhard2: initialization failed (wrong magic number). Expected: MAGIC_NUMBER = %1\n").arg(QString::number(MAGIC_NUMBER, 16).toUpper()));
        return false;
    }
    sendDivider();
    sendIterations();
    timer.start();
    conn=true;
    return conn;
}

void QFExtensionCameraRadhard2::disconnectDevice(unsigned int camera) {
    if (radhard2) delete radhard2;
    radhard2=NULL;
    //radhard2->CloseUsbDevice();
    log_text(tr("Radhard2: disconnected\n"));
    conn = false;
}

double QFExtensionCameraRadhard2::getExposureTime(unsigned int camera) {
	return (double)( ((3840.0/1000000.0) + (20.0*(double)divider/1000000.0))*(double)iterations*1000.0 );
}


void QFExtensionCameraRadhard2::updateAcquisitionTime() {
	emit displayAcquisitionTime(tr("%1 usecs").arg(getExposureTime(0)));
}

void QFExtensionCameraRadhard2::sendDivider() {
    if (conn) {
        radhard2->SendCommand(CMD_SET_DIVIDER, divider);
        log_text(tr("Radhard2: setting DIVIDER = %1\n").arg(divider));
    }
}

void QFExtensionCameraRadhard2::sendIterations() {
    if (conn) {
        radhard2->SendCommand(CMD_SET_ITERATIONS, iterations);
        unsigned int result;
        radhard2->ReadData(&result);
        if(result == OPERATION_DONE) {
            log_text(tr("Radhard2: setting ITERATIONS = %1\n").arg(iterations));
        } else {
            log_error(tr("Radhard2: setting ITERATIONS = %1 failed! (result = %2)\n").arg(iterations).arg(QString::number(result, 16)));
        }
    }
}


void QFExtensionCameraRadhard2::log_indent() {
	if (logService) logService->log_indent();
	else if (services) services->log_indent();
}

void QFExtensionCameraRadhard2::log_unindent() {
	if (logService) logService->log_unindent();
	else if (services) services->log_unindent();
}

void QFExtensionCameraRadhard2::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFExtensionCameraRadhard2::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFExtensionCameraRadhard2::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}


//Q_EXPORT_PLUGIN2(cam_radhard2, QFExtensionCameraRadhard2)
Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCameraRadhard2)

// global variables for
