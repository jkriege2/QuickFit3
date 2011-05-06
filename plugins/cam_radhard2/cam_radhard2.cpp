#include "cam_radhard2.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include <unistd.h>
#include <time.h>

#define LOG_PREFIX "Radhard2:  "

QFExtensionCameraRadhard2::QFExtensionCameraRadhard2(QObject* parent):
    QObject(parent)
{
    conn=false;
    radhard2=NULL;
    subtractOne=true;
    iterations=100;
    divider=1;
    logService=NULL;
    edtBitfile=NULL;
    labFlashSuccess=NULL;
    autoflashbitfile="";
    spIterations=NULL;
    spDivider=NULL;
    autoflash=true;
    autoflashbitfile=bitfile="";

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
    services->log_global_text(tr("%2initializing extension '%1' ...\n").arg(getName()).arg(LOG_PREFIX));
    autoflashbitfile=bitfile=services->getOptions()->getAssetsDirectory()+"/plugins/extensions/"+getID()+"/radhard2_top_cell.bit";
    actProgramFPGA=new QAction(QIcon(":/cam_radhard2_flash.png"), tr("Flash Radhard2 FPGA"), this);
    connect(actProgramFPGA, SIGNAL(triggered()), this, SLOT(programFPGA()));
    QMenu* extm=services->getMenu("extensions");
    QMenu* subMenu=extm->addMenu(tr("Radhard 2 Tools"));
    if (extm) {
        subMenu->addAction(actProgramFPGA);
    }

    loadSettings(NULL);

    services->log_global_text(tr("%2initializing extension '%1' ... DONE\n").arg(getName()).arg(LOG_PREFIX));
}

void QFExtensionCameraRadhard2::programFPGA() {
    QDialog* dlg=new QDialog(NULL);

    QGridLayout* lay=new QGridLayout(dlg);
    dlg->setLayout(lay);

    edtBitfile=new QEnhancedLineEdit(dlg);
    QLabel* l=new QLabel(tr("&Bitfile:"), dlg);
    l->setBuddy(edtBitfile);
    JKStyledButton* btnSelect=new JKStyledButton(JKStyledButton::SelectFile, edtBitfile, dlg);
    edtBitfile->addButton(btnSelect);
    edtBitfile->setText(bitfile);

    QPushButton* btnFlash=new QPushButton(tr("&Flash"), dlg);
    connect(btnFlash, SIGNAL(clicked()), this, SLOT(programFPGAClicked()));

    labFlashSuccess=new QLabel(dlg);
    labFlashSuccess->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

    QPushButton* btnClose=new QPushButton(tr("&Close"), dlg);
    connect(btnClose, SIGNAL(clicked()), dlg, SLOT(accept()));

    lay->addWidget(l, 0, 0);
    lay->addWidget(edtBitfile, 0, 1, 1, 3);
    lay->addWidget(btnFlash, 1, 1, 1, 1);
    lay->addWidget(labFlashSuccess, 2, 1, 1, 3);
    lay->addWidget(btnClose, 3, 3);

    dlg->resize(500,200);
    dlg->exec();
    bitfile=edtBitfile->text();
    storeSettings(NULL);
    delete dlg;
    edtBitfile=NULL;
    labFlashSuccess=NULL;
}

void QFExtensionCameraRadhard2::programFPGAClicked() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (edtBitfile) {
        QString message;
        bool ok=flashFPGA(edtBitfile->text(), 'm', message);
        if (labFlashSuccess) labFlashSuccess->setText(message);
        if (!ok) {
            QMessageBox::critical(NULL, tr("Radhard2 driver"), tr("Could not program Radhard2 FPGA, see dialog for error message!"));
        }
    }
    QApplication::restoreOverrideCursor();
}

bool QFExtensionCameraRadhard2::flashFPGA(QString bitfile, char fpga, QString& messageOut, int retries) {
    messageOut="";
    int res=0;
    int i=0;
    while ((i<retries) && (res==0)) {
        char message[8192];
        res=flash_bitfile(bitfile.toAscii().data(), message, fpga);
        if (i>0) messageOut+="\n\n";
        messageOut += tr("try %1/%2:\n%3").arg(i+1).arg(retries).arg(message);
    }
    return res!=0;
}


void QFExtensionCameraRadhard2::loadSettings(ProgramOptions* settingspo) {
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());
    //QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);
    //iterations=settings.value("radhard2/iterations", iterations).toInt();
    //divider=settings.value("radhard2/divider", divider).toInt();
    //subtractOne=settings.value("radhard2/subtract_one", subtractOne).toBool();
    bitfile=settings.value("radhard2/bitfile", bitfile).toString();
}

void QFExtensionCameraRadhard2::storeSettings(ProgramOptions* settingspo) {
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());
    //QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);
    //settings.setValue("radhard2/iterations", iterations);
    //settings.setValue("radhard2/divider", divider);
    //settings.setValue("radhard2/subtract_one", subtractOne);
    settings.setValue("radhard2/bitfile", bitfile);
}

unsigned int QFExtensionCameraRadhard2::getCameraCount() {
    return 1;
}

void QFExtensionCameraRadhard2::useCameraSettings(unsigned int camera, const QSettings& settings) {
    iterations=settings.value("radhard2/iterations", 1000).toUInt();
    divider=settings.value("radhard2/divider", 1).toUInt();
    subtractOne=settings.value("radhard2/subtract1", true).toBool();
    autoflash=settings.value("radhard2/autoflash", false).toBool();
    autoflashbitfile=settings.value("radhard2/autoflash_bitfile", "").toString();
    if (isConnected(camera)) {
        sendIterations();
        sendDivider();
     }
}

void QFExtensionCameraRadhard2::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/
    QDialog* dlg=new QDialog(parent);

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QFormLayout* formlayout=new QFormLayout(dlg);



    spIterations=new QSpinBox(dlg);
    spIterations->setMinimum(0);
    spIterations->setMaximum(100000);
    spIterations->setValue(settings.value("radhard2/iterations", 1000).toUInt());
    formlayout->addRow(tr("iterations:"), spIterations);
    connect(spIterations, SIGNAL(valueChanged(int)), this, SLOT(updateAcquisitionTime()));

    spDivider=new QSpinBox(dlg);
    spDivider->setMinimum(0);
    spDivider->setMaximum(100000);
    spDivider->setValue(settings.value("radhard2/divider", 1).toUInt());
    formlayout->addRow(tr("divider:"), spDivider);
    connect(spDivider, SIGNAL(valueChanged(int)), this, SLOT(updateAcquisitionTime()));

    QLabel* l=new QLabel(dlg);
    formlayout->addRow(tr("acquisition time:"), l);
    connect(this, SIGNAL(displayAcquisitionTime(const QString&)), l, SLOT(setText(const QString&)));

    QCheckBox* chkSubtract1=new QCheckBox("", dlg);
    chkSubtract1->setChecked(settings.value("radhard2/subtract1", true).toBool());
    formlayout->addRow(tr("subtract offset 1:"), chkSubtract1);


    QCheckBox* chkAutoFlash=new QCheckBox("", dlg);
    chkAutoFlash->setChecked(settings.value("radhard2/autoflash", true).toBool());
    formlayout->addRow(tr("autom. flash on connect:"), chkAutoFlash);

    QEnhancedLineEdit* edtBitfile=new QEnhancedLineEdit(dlg);
    formlayout->addRow(tr("bitfile for autoflash:"), edtBitfile);
    JKStyledButton* btnSelect=new JKStyledButton(JKStyledButton::SelectFile, edtBitfile, dlg);
    edtBitfile->addButton(btnSelect);
    edtBitfile->setText(settings.value("radhard2/autoflash_bitfile", bitfile).toString());
    connect(chkAutoFlash, SIGNAL(clicked(bool)), edtBitfile, SLOT(setEnabled(bool)));
    chkAutoFlash->setChecked(settings.value("radhard2/autoflash", bitfile).toBool());


    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets
         settings.setValue("radhard2/iterations", spIterations->value());
         settings.setValue("radhard2/divider", spDivider->value());
         settings.setValue("radhard2/subtract1", chkSubtract1->isChecked());
         settings.setValue("radhard2/autoflash", chkAutoFlash->isChecked());
         settings.setValue("radhard2/autoflash_bitfile", edtBitfile->text());
    }
    delete dlg;
    spIterations=NULL;
    spDivider=NULL;
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
        log_error(tr("not connected to hardware."));
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
		log_error(tr("Received wrong ACK constant after START_ACQUISITION (result was %1) command.\n").arg(8,16));
    }

    usleep(20000 + 2* int(exposureT));

	// Get the pictures
    radhard2->SendCommand(CMD_GET_MEMORY);
    radhard2->ReadImage(data);

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

    if (autoflash && QFile(autoflashbitfile).exists()) {
        log_text(tr("flashing Radhard2 FPGAs (bit file: %1)\n").arg(autoflashbitfile));
        QString flashMessage;
        bool ok=flashFPGA(autoflashbitfile, 'm', flashMessage);
        flashMessage.replace('\n', QString("\n%1  ").arg(LOG_PREFIX));
        if (ok) {
            log_text(tr("  %2\nflashing Radhard2 FPGAs (bit file: %1) ... DONE!\n").arg(autoflashbitfile).arg(flashMessage));
        } else {
            log_error(tr("  %2\nflashing Radhard2 FPGAs (bit file: %1) ... ERROR!\n").arg(autoflashbitfile).arg(flashMessage));
            return false;
        }
    } else {
        if (autoflash) log_warning(tr("could not flash Radhard2 FPGAs, as bit file '%1' does not exist!\n").arg(autoflashbitfile));
    }

    radhard2=new Radhard2;
    radhard2->SendCommand(CMD_TEST_COMLINK, 0);
    unsigned int magicn=0;
    radhard2->ReadData(&magicn);
    log_text(tr("connection initialized\n%2read: MAGIC_NUMBER = %1\n").arg(QString::number(magicn, 16).toUpper()).arg(LOG_PREFIX));
    if (magicn!=MAGIC_NUMBER) {
        log_error(tr("initialization failed (wrong magic number). Expected: MAGIC_NUMBER = %1\n").arg(QString::number(MAGIC_NUMBER, 16).toUpper()));
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
    log_text(tr("disconnected\n"));
    conn = false;
}

double QFExtensionCameraRadhard2::getExposureTime(unsigned int camera) {
	return calcExposureTime(iterations, divider);
}

double QFExtensionCameraRadhard2::calcExposureTime(double piterations, double pdivider) {
	return (double)( ((3840.0/1000000.0) + (20.0*pdivider/1000000.0))*piterations*1000.0 );
}


void QFExtensionCameraRadhard2::updateAcquisitionTime() {
	emit displayAcquisitionTime(tr("%1 usecs").arg(calcExposureTime(spIterations->value(), spDivider->value())));
}

void QFExtensionCameraRadhard2::sendDivider() {
    if (conn) {
        radhard2->SendCommand(CMD_SET_DIVIDER, divider);
        //log_text(tr("setting DIVIDER = %1\n").arg(divider));
    }
}

void QFExtensionCameraRadhard2::sendIterations() {
    if (conn) {
        radhard2->SendCommand(CMD_SET_ITERATIONS, iterations);
        unsigned int result;
        radhard2->ReadData(&result);
        if(result == OPERATION_DONE) {
            //log_text(tr("setting ITERATIONS = %1\n").arg(iterations));
        } else {
            log_error(tr("setting ITERATIONS = %1 failed! (result = %2)\n").arg(iterations).arg(QString::number(result, 16)));
        }
    }
}


bool QFExtensionCameraRadhard2::startAcquisition(unsigned int camera, QString filenamePrefix) {
    return false;
}

bool QFExtensionCameraRadhard2::cancelAcquisition(unsigned int camera) {
    return true;
}

bool QFExtensionCameraRadhard2::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return false;
}

void QFExtensionCameraRadhard2::getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters) {
}

bool QFExtensionCameraRadhard2::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

void QFExtensionCameraRadhard2::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFExtensionCameraRadhard2::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFExtensionCameraRadhard2::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
	else if (services) services->log_error(LOG_PREFIX+message);
}


//Q_EXPORT_PLUGIN2(cam_radhard2, QFExtensionCameraRadhard2)
Q_EXPORT_PLUGIN2(TARGETNAME, QFExtensionCameraRadhard2)

// global variables for
