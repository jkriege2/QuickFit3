/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfespimb040camparamstackconfigwidget2.h"
#include "ui_qfespimb040camparamstackconfigwidget2.h"

#include "qfstyledbutton.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>
#include "qfespimb040opticssetup.h"
#include "qfcompleterfromfile.h"
#include <tiffio.h>
#include "libtiff_tools.h"

QFESPIMB040CamParamStackConfigWidget2::QFESPIMB040CamParamStackConfigWidget2(QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040CamParamStackConfigWidget2)
{
    this->log=log;
    this->acqTools=acqTools;
    m_pluginServices=pluginServices;
    this->opticsSetup=opticsSetup;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
    ui->setupUi(this);

    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/");
    QFCompleterFromFile* c1=new QFCompleterFromFile(this);
    c1->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/camparamstack_prefix1.txt");
    QFCompleterFromFile* c2=new QFCompleterFromFile(this);
    c2->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/camparamstack_prefix2.txt");
    ui->edtPrefix1->setCompleter(c1);
    ui->edtPrefix2->setCompleter(c2);
    ui->edtPrefix1->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtPrefix1, ui->edtPrefix1));
    ui->edtPrefix2->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtPrefix2, ui->edtPrefix2));

    if (opticsSetup) {
        ui->cmbCam1Settings->init(configDirectory);
        ui->cmbCam1Settings->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbCam1Settings->connectTo(opticsSetup->cameraComboBox(0));
        //qDebug()<<configDirectory<<opticsSetup->cameraComboBox(0)<<opticsSetup->cameraComboBox(1);

        ui->cmbCam2Settings->init(configDirectory);
        ui->cmbCam2Settings->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbCam2Settings->connectTo(opticsSetup->cameraComboBox(1));
    }
    updateReplaces();
    bindLineEdit(ui->edtPrefix1);
    bindLineEdit(ui->edtPrefix2);

}

QFESPIMB040CamParamStackConfigWidget2::~QFESPIMB040CamParamStackConfigWidget2()
{
    delete ui;
}


void QFESPIMB040CamParamStackConfigWidget2::loadSettings(QSettings& settings, QString prefix) {

    ui->chkUse1->setChecked(settings.value(prefix+"use1", true).toBool());
    ui->chkUse2->setChecked(settings.value(prefix+"use2", true).toBool());
    ui->edtPrefix1->setText(settings.value(prefix+"prefix1", "stack_cam1_%counter%").toString());
    ui->edtPrefix2->setText(settings.value(prefix+"prefix2", "stack_cam2_%counter%").toString());

    ui->cmbCam1Settings->setCurrentConfig(settings.value(prefix+"acqsettings1", "default").toString());
    ui->cmbCam2Settings->setCurrentConfig(settings.value(prefix+"acqsettings2", "default").toString());

    ui->spinImages->setValue(settings.value(prefix+"images", 1).toInt());

    ui->spinStart->setValue(settings.value(prefix+"start", 0).toDouble());
    ui->spinDelta->setValue(settings.value(prefix+"delta", 10).toDouble());
    ui->spinEnd->setValue(settings.value(prefix+"end", 100).toDouble());
    ui->cmbParameter->setCurrentIndex(settings.value(prefix+"parameter", 0).toInt());
    ui->cmbMode->setCurrentIndex(settings.value(prefix+"mode", 0).toInt());

    ui->chkLightpath->setChecked(settings.value(prefix+"lightpath", false).toBool());
    ui->cmbLightpath->setCurrentIndex(settings.value(prefix+"lightpathidx", -1).toInt());
    ui->chkPreviewMode->setChecked(settings.value(prefix+"chkPreviewMode", false).toBool());

    ui->chkSaveMeasurements->setChecked(settings.value(prefix+"savemeasurement", false).toBool());
    on_chkUse1_clicked(true);
    on_chkUse2_clicked(true);
}


void QFESPIMB040CamParamStackConfigWidget2::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"use1", ui->chkUse1->isChecked());
    settings.setValue(prefix+"use2", ui->chkUse2->isChecked());
    settings.setValue(prefix+"prefix1", ui->edtPrefix1->text());
    settings.setValue(prefix+"prefix2", ui->edtPrefix2->text());
    settings.setValue(prefix+"acqsettings1", ui->cmbCam1Settings->currentConfigName());
    settings.setValue(prefix+"acqsettings2", ui->cmbCam2Settings->currentConfigName());

    settings.setValue(prefix+"start", ui->spinStart->value());
    settings.setValue(prefix+"delta", ui->spinDelta->value());
    settings.setValue(prefix+"end", ui->spinEnd->value());
    settings.setValue(prefix+"parameter", ui->cmbParameter->currentIndex());
    settings.setValue(prefix+"mode", ui->cmbMode->currentIndex());
    settings.setValue(prefix+"images", ui->spinImages->value());

    settings.setValue(prefix+"lightpathidx", ui->cmbLightpath->currentIndex());
    settings.setValue(prefix+"lightpath", ui->chkLightpath->isChecked());
    settings.setValue(prefix+"chkPreviewMode", ui->chkPreviewMode->isChecked());
    settings.setValue(prefix+"lightpath", ui->chkLightpath->isChecked());
}



QString QFESPIMB040CamParamStackConfigWidget2::prefix1()  {
    updateReplaces();
    QString filename= ui->edtPrefix1->text();
    filename=transformFilename(filename);
    return filename;
}

QString QFESPIMB040CamParamStackConfigWidget2::prefix2()  {
    updateReplaces();
    QString filename= ui->edtPrefix2->text();
    filename=transformFilename(filename);
    return filename;
}

bool QFESPIMB040CamParamStackConfigWidget2::use1() const {
    return ui->chkUse1->isChecked();
}

bool QFESPIMB040CamParamStackConfigWidget2::use2() const {
    return ui->chkUse2->isChecked();
}

int QFESPIMB040CamParamStackConfigWidget2::images() const {
    return ui->spinImages->value();
}

int QFESPIMB040CamParamStackConfigWidget2::stackParameter() const {
    return ui->cmbParameter->currentIndex();
}

QString QFESPIMB040CamParamStackConfigWidget2::stackParameterName() const {
    return ui->cmbParameter->currentText();
}

int QFESPIMB040CamParamStackConfigWidget2::stackMode() const {
    return ui->cmbMode->currentIndex();
}

QString QFESPIMB040CamParamStackConfigWidget2::stackModeName() const {
    return ui->cmbMode->currentText();
}

double QFESPIMB040CamParamStackConfigWidget2::stackStart() const {
    return ui->spinStart->value();
}

double QFESPIMB040CamParamStackConfigWidget2::stackDelta() const {
    return ui->spinDelta->value();
}

double QFESPIMB040CamParamStackConfigWidget2::stackEnd() const {
    return ui->spinEnd->value();
}

QList<double> QFESPIMB040CamParamStackConfigWidget2::stack() const {
    QList<double> l;
    double start=ui->spinStart->value();
    double end=ui->spinEnd->value();
    double step=ui->spinDelta->value();
    double delta=end-start;

    if (ui->cmbMode->currentIndex()==0) {
        if (step*delta>0 && step>0) {
            for (double v=start; v<=end; v=v+step) {
                l.append(v);
            }
        } else if (step*delta>0 && step<0) {
            for (double v=start; v>=end; v=v+step) {
                l.append(v);
            }
        }
    } else if (ui->cmbMode->currentIndex()==1) {
        if (start>0 && ( (delta>0 && step>1.0))) {
            for (double v=start; v<=end; v=v*step) {
                l.append(v);
            }
        } else if (start>0 && ( (delta<0 && step<1.0))) {
            for (double v=start; v>=end; v=v*step) {
                l.append(v);
            }
        }
    } else if (ui->cmbMode->currentIndex()==2) {
        if (delta>0 && step>0) {
            double ls=step;
            int cnt=0;
            double v=start;
            do {
                l.append(v);
                cnt++;
                if (cnt>=8) {
                    ls=ls*2;
                    cnt=0;
                }
                v=v+ls;
            } while (v<end);
        }
    }

    return l;
}

bool QFESPIMB040CamParamStackConfigWidget2::lightpathActivated() const {
    return ui->chkLightpath->isChecked();
}

QString QFESPIMB040CamParamStackConfigWidget2::lightpathFilename() const {
    return ui->cmbLightpath->itemData(ui->cmbLightpath->currentIndex()).toString();
}

QString QFESPIMB040CamParamStackConfigWidget2::lightpath() const {
    return ui->cmbLightpath->currentText();
}

void QFESPIMB040CamParamStackConfigWidget2::on_btnAcquire_clicked() {
    emit doStack();
}

void QFESPIMB040CamParamStackConfigWidget2::on_spinStart_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040CamParamStackConfigWidget2::on_spinEnd_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040CamParamStackConfigWidget2::on_spinDelta_valueChanged(double value) {
    updateLabel();
}

void QFESPIMB040CamParamStackConfigWidget2::updateLabel() {
    QList<double> l=stack();
    QString s="";
    if (l.size()>0) {
        s=s+" (";
        for (int i=0; i<qMin(12, l.size()); i++) {
            if (i>0) s=s+"; ";
            s=s+QString::number(l[i]);
        }
        if (l.size()>12) s=s+" ...";
        s=s+")";
    }
    ui->labScan->setText(tr("%1 steps%2").arg(l.size()).arg(s));
}

void QFESPIMB040CamParamStackConfigWidget2::on_chkUse1_clicked(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->widScanParam1->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->edtPrefix1->setEnabled(ui->chkUse1->isChecked());
    ui->cmbCam1Settings->setEnabled(ui->chkUse1->isChecked());
}

void QFESPIMB040CamParamStackConfigWidget2::on_chkUse2_clicked(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->widScanParam1->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->edtPrefix2->setEnabled(ui->chkUse2->isChecked());
    ui->cmbCam2Settings->setEnabled(ui->chkUse2->isChecked());
}

void QFESPIMB040CamParamStackConfigWidget2::on_cmbMode_currentIndexChanged(int index) {
    updateLabel();
}

QString QFESPIMB040CamParamStackConfigWidget2::currentConfigFilename(int camera) const {
    if (camera==0) return ui->cmbCam1Settings->currentConfigFilename();
    if (camera==1) return ui->cmbCam2Settings->currentConfigFilename();
    return "";
}

QString QFESPIMB040CamParamStackConfigWidget2::currentConfigName(int camera) const {
    if (camera==0) return ui->cmbCam1Settings->currentConfigName();
    if (camera==1) return ui->cmbCam2Settings->currentConfigName();
    return "";
}

void QFESPIMB040CamParamStackConfigWidget2::updateReplaces()
{
    setGlobalReplaces(opticsSetup, expDescription, acqDescription);
    setReplaceValue("acquisition1_name",  cleanStringForFilename(ui->cmbCam1Settings->currentConfigName()));
    setReplaceValue("acquisition2_name",  cleanStringForFilename(ui->cmbCam2Settings->currentConfigName()));
}

void QFESPIMB040CamParamStackConfigWidget2::lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes) {
    QString idx=ui->cmbLightpath->currentText();
    ui->cmbLightpath->clear();
    //qDebug()<<"QFESPIMB040CamParamStackConfigWidget2::lightpathesChanged "<<lightpathes.size();
    for (int i=0; i<lightpathes.size(); i++) {
        QTriple<QIcon, QString, QString> p=lightpathes[i];
        ui->cmbLightpath->addItem(p.first, p.second, p.third);
    }
    ui->cmbLightpath->setCurrentIndex(qMax(0, ui->cmbLightpath->findText(idx)));

}


bool QFESPIMB040CamParamStackConfigWidget2::saveMeasurements() const {
    return ui->chkSaveMeasurements->isChecked();
}

bool QFESPIMB040CamParamStackConfigWidget2::previewMode() const
{
    return ui->chkPreviewMode->isChecked();
}


void QFESPIMB040CamParamStackConfigWidget2::on_btnSaveTemplate_clicked()
{
    QDir().mkpath(ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040CamParamStackConfigWidget2/lasttemplatedir", ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/").toString();
    QString filename=qfGetSaveFileName(this, tr("save as template ..."), dir, tr("camera parameter stack configuration (*.cpsc)"))    ;
    if (!filename.isEmpty()) {
        bool ok=true;
        if (QFile::exists(filename)) {
            ok=false;
            if (QMessageBox::question(this, tr("save as template ..."), tr("The file\n  '%1'\nalready exists. Overwrite?").arg(filename), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes) {
                ok=true;
            }
        }
        if (ok) {
            QSettings set(filename, QSettings::IniFormat);
            storeSettings(set, "camera_param_stack_settings/");
            dir=QFileInfo(filename).absolutePath();
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040CamParamStackConfigWidget2/lasttemplatedir", dir);
}

void QFESPIMB040CamParamStackConfigWidget2::on_btnLoadTemplate_clicked()
{
    QDir().mkpath(ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040CamParamStackConfigWidget2/lasttemplatedir", ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open template ..."), dir, tr("camera parameter stack configuration (*.cpsc)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);
        loadSettings(set, "camera_param_stack_settings/");
        dir=QFileInfo(filename).absolutePath();
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040CamParamStackConfigWidget2/lasttemplatedir", dir);
}















#define CAMPARAMSTACK_ERROR(message) \
    log->log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Parameter Stack Acquisition Error"), (message));


void QFESPIMB040CamParamStackConfigWidget2::performStack()
{
    if (!(this->use1() || this->use2())) {
        QMessageBox::critical(this, tr("B040SPIM: Parameter Stack Acquisition"), tr("Cannot start image acquisition: No camera selected!"));
        return;
    }

    QDateTime startDateTime=QDateTime::currentDateTime();
    QList<QFESPIMB040OpticsSetupBase::measuredValues> measured;


    bool ok=true;

    //////////////////////////////////////////////////////////////////////////////////////
    // get array of values
    //////////////////////////////////////////////////////////////////////////////////////
    QList<double> scanVals=this->stack();
    if (scanVals.isEmpty()) {
        QMessageBox::critical(this, tr("B040SPIM: Parameter Stack Acquisition"), tr("No values to scan selected!"));
        return;
    }
    int images=scanVals.size()*this->images();


    log->log_text(tr("starting parameter stack acquisition:\n"));
    log->log_text(tr("  - scan parameter: %1\n").arg(this->stackParameterName()));
    log->log_text(tr("  - scan mode: %1\n").arg(this->stackModeName()));
    log->log_text(tr("  - scan start: %1\n").arg(this->stackStart()));
    log->log_text(tr("  - scan end: %1\n").arg(this->stackEnd()));
    log->log_text(tr("  - scan delta: %1\n").arg(this->stackDelta()));
    opticsSetup->lockLightpath();


    //////////////////////////////////////////////////////////////////////////////////////
    // LOCK/INIT CAMERA 1
    //////////////////////////////////////////////////////////////////////////////////////
    bool useCam1=false;
    QFExtension* extension1=NULL;
    QFExtensionCamera* ecamera1=NULL;
    int camera1=0;
    QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
    QString acquisitionPrefix1=this->prefix1();
    QString TIFFFIlename1=acquisitionPrefix1+".tif";
    TIFF* tiff1=NULL;
    if (this->use1()) {
        if (!(useCam1=opticsSetup->lockCamera(0, &extension1, &ecamera1, &camera1, &previewSettingsFilename1))) {
            CAMPARAMSTACK_ERROR(tr("error locking camer 1!\n"));
        }
    }
    if (QFile::exists(this->currentConfigFilename(0))) acquisitionSettingsFilename1=this->currentConfigFilename(0);

    //////////////////////////////////////////////////////////////////////////////////////
    // LOCK/INIT CAMERA 2
    //////////////////////////////////////////////////////////////////////////////////////
    bool useCam2=false;
    QFExtension* extension2=NULL;
    QFExtensionCamera* ecamera2=NULL;
    QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
    QString acquisitionPrefix2=this->prefix2();
    QString TIFFFIlename2=acquisitionPrefix2+".tif";
    TIFF* tiff2=NULL;
    int camera2=0;
    if (this->use2()) {
        if(!(useCam2=opticsSetup->lockCamera(1, &extension2, &ecamera2, &camera2, &previewSettingsFilename2))) {
            CAMPARAMSTACK_ERROR(tr("error locking camer 2!\n"));
        }
    }
    if (QFile::exists(this->currentConfigFilename(1))) acquisitionSettingsFilename2=this->currentConfigFilename(1);

    if (ok && !useCam1 && !useCam2) {
        CAMPARAMSTACK_ERROR(tr("Cannot start image acquisition: No camera selected, or both cameras not usable!"));
        opticsSetup->unlockLightpath();
        ok=false;
    }


    //////////////////////////////////////////////////////////////////////////////////////
    // CHECK WHETHER HARDWRAE SUPPORTS CHANGING THE SELECTED PARAMETER
    //////////////////////////////////////////////////////////////////////////////////////
    int stackParamI=this->stackParameter();
    QFExtensionCamera::CameraSetting stackParam=QFExtensionCamera::CameraSetting(stackParamI);
    if (ok) {
        if (stackParam>=0 && stackParam<=QFExtensionCamera::CamSetMaxParam) {
            if (useCam1 && !ecamera1->isCameraSettingChangable(stackParam)) {
                ok=false;
                CAMPARAMSTACK_ERROR(tr("camera 1 does not support changing the %1!\n").arg(this->stackParameterName()));
            }
            if (ok && useCam2 && !ecamera2->isCameraSettingChangable(stackParam)) {
                ok=false;
                CAMPARAMSTACK_ERROR(tr("camera 2 does not support changing the %1!\n").arg(this->stackParameterName()));
            }
        } else {
            ok=false;
            CAMPARAMSTACK_ERROR(tr("invalid stack parameter %1 '%2'\n").arg(stackParam).arg(this->stackParameterName()));
        }
    }



    if (ok) {
        if (ok && useCam1) log->log_text(tr("  - storing files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log->log_text(tr("  - storing files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        QProgressListDialog progress(tr("Parameter Stack Acquisition"), tr("&Cancel"), 0, 100, this);
        progress.addItem(tr("preparation"));
        progress.addItem(tr("acquisition"));
        progress.addItem(tr("cleanup"));
        progress.setWindowModality(Qt::WindowModal);
        progress.setValue(0);
        progress.start();

        //////////////////////////////////////////////////////////////////////////////////////
        // SET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        opticsSetup->lockLighpathCombobox();
        QString oldLightpath=opticsSetup->getCurrentLightpathFilename();
        QString oldLightpathName=opticsSetup->getCurrentLightpath();
        QString lightpathName="unknown";
        if (this->lightpathActivated()) {
            if (!QFile::exists(this->lightpathFilename())) {
                CAMPARAMSTACK_ERROR(tr("  - acquisition lighpath configuration '%1' does not exist!\n").arg(this->lightpath()));
                opticsSetup->unlockLightpath();
                return;

            } else {
                log->log_text(tr("  - setting acquisition lightpath settings '%1' ...\n").arg(this->lightpath()));
                opticsSetup->loadLightpathConfig(this->lightpathFilename(), true, &lightpathName);
                log->log_text(tr("  - setting acquisition lightpath settings '%1' ... DONE\n").arg(this->lightpath()));
            }
        }


        //////////////////////////////////////////////////////////////////////////////////////
        // switch on light
        //////////////////////////////////////////////////////////////////////////////////////
        bool formerMainShutterState=opticsSetup->getMainIlluminationShutter();
        if (opticsSetup->isMainIlluminationShutterAvailable()){
            log->log_text(tr("  - switch main shutter on!\n"));
            opticsSetup->setMainIlluminationShutter(true, true);
        }

        QList<QVariant> realValues1, realValues2;
        QTime timAcquisition=QTime::currentTime();
        QDateTime timStart;
        double duration=0;


        progress.nextItem();
        if (this->previewMode()) {
            log->log_text(tr("acquiring stack in PREVIEW MODE ...\n"));


            //////////////////////////////////////////////////////////////////////////////////////
            // OPEN OUTPUT TIFF FILES
            //////////////////////////////////////////////////////////////////////////////////////
            progress.setLabelText(tr("opening output files ..."));
            QApplication::processEvents();
            if (ok && useCam1) {
                QDir().mkpath(QFileInfo(TIFFFIlename1.toAscii().data()).absolutePath());
                tiff1=TIFFOpen(TIFFFIlename1.toAscii().data(), "w");
                if (!tiff1) {
                    ok=false;
                    CAMPARAMSTACK_ERROR(tr("error opening TIFF file (camera 1) '%1'!").arg(TIFFFIlename1));
                }
            }
            if (ok && useCam2) {
                QDir().mkpath(QFileInfo(TIFFFIlename2.toAscii().data()).absolutePath());
                tiff2=TIFFOpen(TIFFFIlename2.toAscii().data(), "w");
                if (!tiff2) {
                    ok=false;
                    CAMPARAMSTACK_ERROR(tr("error opening TIFF file (camera 2) '%1'!").arg(TIFFFIlename2));
                }
            }



            if (progress.wasCanceled()) {
                log->log_warning(tr("canceled by user!\n"));
                ok=false;
            }



            //////////////////////////////////////////////////////////////////////////////////////
            // CHANGE PARAM, ACQUIRE IMAGE, CHANGE PARAM, ACQUIRE IMAGE, ...
            //    images are stored in TIFF files using libtiff and they are (possibly) downscaled to 16-bit
            //////////////////////////////////////////////////////////////////////////////////////
            timAcquisition=QTime::currentTime();
            duration=0;
            if (ok) {
                progress.setLabelText(tr("acquiring images ..."));
                bool running=ok;
                //double newPos=stageStart;
                int stackIdx=0;
                int width1=0, height1=0;
                uint32_t* buffer1=NULL;
                int width2=0, height2=0;
                uint32_t* buffer2=NULL;
                int imageCnt=0;

                measured.append(opticsSetup->getMeasuredValues());

                while (running && (stackIdx<scanVals.size())) {

                    if (ok && useCam1) {

                        //////////////////////////////////////////////////////////////////////////////////////
                        // PREPARE CAMERA 1
                        //////////////////////////////////////////////////////////////////////////////////////
                        if (ok && useCam1) {
                            progress.setLabelText(tr("preparing camera 1 ..."));
                            QApplication::processEvents();

                            QString tmpName1=QDir::temp().absoluteFilePath("qf3spimb040_cam1tmpsettings.ini");

                            QTemporaryFile file1;
                            if (file1.open()) {
                                 tmpName1=file1.fileName();
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);

                            if (useCam1) QFile::copy(acquisitionSettingsFilename1, tmpName1);

                            QSettings settings(tmpName1, QSettings::IniFormat);

                            //QSettings settings(acquisitionSettingsFilename1, QSettings::IniFormat);

                            ecamera1->changeCameraSetting(settings, stackParam, scanVals[stackIdx]);
                            ecamera1->useCameraSettings(camera1, settings);
                            realValues1.append(ecamera1->getCameraCurrentSetting(camera1, stackParam));
                            //realValues1.append(ecamera1->getExposureTime(camera1));
                            log->log_text(tr("  - prepared camer 1!\n"));
                            width1=ecamera1->getCameraImageWidth(camera1);
                            height1=ecamera1->getCameraImageHeight(camera1);
                            buffer1=(uint32_t*)qfRealloc(buffer1, width1*height1*sizeof(uint32_t));
                            if (!buffer1) {
                                ok=false;
                                CAMPARAMSTACK_ERROR(tr("could not allocate image buffer for camera 1!\n"));
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                        }
                    }

                    if (ok && useCam2) {

                        //////////////////////////////////////////////////////////////////////////////////////
                        // PREPARE CAMERA 1
                        //////////////////////////////////////////////////////////////////////////////////////
                        if (ok && useCam2) {
                            progress.setLabelText(tr("preparing camera 2 ..."));
                            QApplication::processEvents();
                            QString tmpName1=QDir::temp().absoluteFilePath("qf3spimb040_cam2tmpsettings.ini");

                            QTemporaryFile file1;
                            if (file1.open()) {
                                 tmpName1=file1.fileName();
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);

                            if (useCam2) QFile::copy(acquisitionSettingsFilename2, tmpName1);

                            QSettings settings(tmpName1, QSettings::IniFormat);

                            ecamera2->changeCameraSetting(settings, stackParam, scanVals[stackIdx]);
                            ecamera2->useCameraSettings(camera2, settings);
                            realValues2.append(ecamera2->getCameraCurrentSetting(camera2, stackParam));
                            //realValues2.append(ecamera2->getExposureTime(camera2));
                            log->log_text(tr("  - prepared camer 2!\n"));
                            width2=ecamera2->getCameraImageWidth(camera2);
                            height2=ecamera2->getCameraImageHeight(camera2);
                            buffer2=(uint32_t*)qfRealloc(buffer2, width2*height2*sizeof(uint32_t));
                            if (!buffer2) {
                                ok=false;
                                CAMPARAMSTACK_ERROR(tr("could not allocate image buffer for camera 2!\n"));
                            }
                            if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                        }
                    }

                    QApplication::processEvents();
                    if (progress.wasCanceled()) {
                        running=false;
                        log->log_warning(tr("  - acquisition canceled by user!\n"));
                    } else {
                        for (int img=0; img<this->images(); img++) {
                            log->log_text(tr("acquiring images (%1/%2) ...\n").arg(imageCnt+1).arg(images));
                            progress.setLabelText(tr("acquiring images (%1/%2) ...").arg(imageCnt+1).arg(images));
                            QApplication::processEvents();
                            if (progress.wasCanceled()) {
                                running=false;
                                log->log_warning(tr("  - acquisition canceled by user!\n"));
                                break;
                            }
                            if (stackIdx==0) {
                                timAcquisition.start();
                                timStart=QDateTime::currentDateTime();
                            }
                            if (useCam1) {
                                if (ecamera1->acquireOnCamera(camera1, buffer1)) {
                                    TIFFTWriteUint16from32(tiff1, buffer1, width1, height1, false);
                                    TIFFWriteDirectory(tiff1);
                                } else {
                                    ok=false;
                                    CAMPARAMSTACK_ERROR(tr("error acquiring image %1/%2 on camera 1!\n").arg(imageCnt+1).arg(images));
                                }
                            }
                            //QApplication::processEvents();
                            if (useCam2) {
                                if (ecamera2->acquireOnCamera(camera2, buffer2)) {
                                    TIFFTWriteUint16from32(tiff2, buffer2, width2, height2, false);
                                    TIFFWriteDirectory(tiff2);
                                } else {
                                    ok=false;
                                    CAMPARAMSTACK_ERROR(tr("error acquiring image %1/%2 on camera 2!\n").arg(imageCnt+1).arg(images));
                                }
                            }
                            imageCnt++;
                        }
                        if (this->saveMeasurements()) measured.append(opticsSetup->getMeasuredValues());
                        //QApplication::processEvents();
                    }
                    if (!ok) running=false;

                    stackIdx++;
                    progress.setValue((int)round((double)stackIdx/(double)scanVals.size()*100.0));
                    QApplication::processEvents();
                }
                duration=timAcquisition.elapsed()/1000.0;
                if (buffer1) qfFree(buffer1);
                if (buffer2) qfFree(buffer2);
                buffer1=buffer2=NULL;

            }
        } else {
            log->log_text(tr("acquiring stack in ACQUISITION MODE ...\n"));

            if (progress.wasCanceled()) {
                log->log_warning(tr("canceled by user!\n"));
                ok=false;
            }
            QMap<QString, QVariant> acquisitionDescription1;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> moreFiles1;
            QMap<QString, QVariant> acquisitionDescription2;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> moreFiles2;

            //////////////////////////////////////////////////////////////////////////////////////
            // CHANGE PARAM, ACQUIRE IMAGE, CHANGE PARAM, ACQUIRE IMAGE, ...
            //    images are stored in TIFF files using libtiff and they are (possibly) downscaled to 16-bit
            //////////////////////////////////////////////////////////////////////////////////////
            timAcquisition=QTime::currentTime();
            duration=0;
            if (ok) {
                progress.setLabelText(tr("acquiring images ..."));
                bool running=ok;
                //double newPos=stageStart;
                int stackIdx=0;
                int width1=0, height1=0;
                uint32_t* buffer1=NULL;
                int width2=0, height2=0;
                uint32_t* buffer2=NULL;
                int imageCnt=0;
                int frames=this->images();

                measured.append(opticsSetup->getMeasuredValues());

                while (running && (stackIdx<scanVals.size())) {
                    QString tmpName1=QDir::temp().absoluteFilePath("qf3spimb040_cam1tmpsettings.ini");

                    QTemporaryFile file1;
                    if (file1.open()) {
                         tmpName1=file1.fileName();
                    }
                    if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                    if (useCam1) QFile::copy(acquisitionSettingsFilename1, tmpName1);
                    QSettings settings1(tmpName1, QSettings::IniFormat);
                    if (frames>0 && ecamera1->isCameraSettingChangable(QFExtensionCamera::CamSetNumberFrames)) ecamera1->changeCameraSetting(settings1, QFExtensionCamera::CamSetNumberFrames, frames);
                    ecamera1->changeCameraSetting(settings1, stackParam, scanVals[stackIdx]);






                    QString tmpName2=QDir::temp().absoluteFilePath("qf3spimb040_cam2tmpsettings.ini");

                    QTemporaryFile file2;
                    if (file2.open()) {
                         tmpName2=file2.fileName();
                    }
                    if (QFile::exists(tmpName2)) QFile::remove(tmpName2);

                    if (useCam2) QFile::copy(acquisitionSettingsFilename2, tmpName2);

                    QSettings settings2(tmpName2, QSettings::IniFormat);

                    if (frames>0 && ecamera2->isCameraSettingChangable(QFExtensionCamera::CamSetNumberFrames)) ecamera2->changeCameraSetting(settings2, QFExtensionCamera::CamSetNumberFrames, frames);
                    ecamera2->changeCameraSetting(settings2, stackParam, scanVals[stackIdx]);




                    //////////////////////////////////////////////////////////////////////////////////////
                    // acquire image series
                    //////////////////////////////////////////////////////////////////////////////////////
                    ok = acqTools->acquireSeries(lightpathName, QString("camstack%1").arg(stackIdx,4,10,QLatin1Char('0')), tr("cam parameter stack"), useCam1, extension1, ecamera1, camera1, acquisitionPrefix1+QString("__sidx%1").arg(stackIdx,4,10,QLatin1Char('0')), settings1, acquisitionDescription1, moreFiles1, useCam2, extension2, ecamera2, camera2, acquisitionPrefix2+QString("__sidx%1").arg(stackIdx,4,10,QLatin1Char('0')), settings2, acquisitionDescription2, moreFiles2, &measured, &progress, NULL);
                    if (!ok) {
                        if (frames>1) {
                            CAMPARAMSTACK_ERROR(tr("error acquiring image (%1...%2)/%3 !\n").arg(imageCnt+1).arg(imageCnt+1+frames).arg(images));
                        } else {
                            CAMPARAMSTACK_ERROR(tr("error acquiring image %1/%2 !\n").arg(imageCnt+1).arg(images));
                        }
                    }

                    if (QFile::exists(tmpName1)) QFile::remove(tmpName1);
                    if (QFile::exists(tmpName2)) QFile::remove(tmpName2);

                    if (!ok) running=false;

                    stackIdx++;
                    imageCnt+=frames;
                    progress.setValue((int)round((double)stackIdx/(double)scanVals.size()*100.0));
                    QApplication::processEvents();

                    if (progress.wasCanceled()) {
                        running=false;
                        log->log_error(tr("CANCELED BY USER!\n"));
                    }
                }
                duration=timAcquisition.elapsed()/1000.0;
                if (buffer1) qfFree(buffer1);
                if (buffer2) qfFree(buffer2);
                buffer1=buffer2=NULL;

            }
        }
        progress.setValue(100);
        measured.append(opticsSetup->getMeasuredValues());
        progress.nextItem();

        //////////////////////////////////////////////////////////////////////////////////////
        // collect lightpath data common to all cameras
        //////////////////////////////////////////////////////////////////////////////////////
        QMap<QString, QVariant> acquisitionDescription;
        opticsSetup->saveLightpathConfig(acquisitionDescription, "", "lightpath/");

        //////////////////////////////////////////////////////////////////////////////////////
        // switch on/off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()){
            log->log_text(tr("  - switch main shutter %1!\n").arg((formerMainShutterState)?tr("on"):tr("off")));
            opticsSetup->setMainIlluminationShutter(formerMainShutterState, true);
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // RESET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        if (this->lightpathActivated()) {
            opticsSetup->loadLightpathConfig(oldLightpath, false);
            log->log_text(tr("  - resetting to old lightpath settings (%1) ...\n").arg(oldLightpath));//Name));
        }
        opticsSetup->unlockLighpathCombobox();

        //////////////////////////////////////////////////////////////////////////////////////
        // close tiff files and free buffers
        //////////////////////////////////////////////////////////////////////////////////////
        progress.setLabelText(tr("closing output files ..."));
        QApplication::processEvents();
        if (tiff1) TIFFClose(tiff1);
        if (tiff2) TIFFClose(tiff2);
        tiff1=tiff2=NULL;

        //////////////////////////////////////////////////////////////////////////////////////
        // collect acquisition data common to all cameras
        //////////////////////////////////////////////////////////////////////////////////////
        QString scanCSV;
        if (ok) {
            acquisitionDescription["type"]="parameter stack";
            acquisitionDescription["stack_parameter"]=this->stackParameterName();
            acquisitionDescription["stack_mode"]=this->stackModeName();
            acquisitionDescription["stack_start"]=this->stackStart();
            acquisitionDescription["stack_delta"]=this->stackDelta();
            acquisitionDescription["stack_end"]=this->stackEnd();
            acquisitionDescription["sequence_overall_length"]=images;
            acquisitionDescription["images_per_step"]=this->images();
            acquisitionDescription["stack_length"]=scanVals.size();
            acquisitionDescription["start_time"]=timStart;
            acquisitionDescription["duration"]=duration;
            acquisitionDescription["stack_value_count"]=scanVals.size();
            acquisitionDescription["stack_values"]=CDoubleListToQString(scanVals);
            if (useCam1) acquisitionDescription["real_stack_values/camera1"]=realValues1;
            if (useCam2) acquisitionDescription["real_stack_values/camera2"]=realValues2;



            QTextStream pf(&scanCSV);
            pf<<"# number,   ideal value";
            if (useCam1) pf<<",   real value camera1";
            if (useCam2) pf<<",   real value camera2";
            pf<<"\n";
            for (int i=0; i<qMin(scanVals.size(), qMax(realValues1.size(), realValues2.size())); i++) {
                pf<<i<<", "<<CDoubleToQString(scanVals[i]);
                if (useCam1 && i<realValues1.size()) pf<<", "<<CDoubleToQString(realValues1[i].toDouble());
                if (useCam2 && i<realValues2.size()) pf<<", "<<CDoubleToQString(realValues2[i].toDouble());
                pf<<"\n";
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // write image stack properties to files, also collects camera specific information
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && useCam1) {
            QMap<QString, QVariant> acquisitionDescription1=acquisitionDescription;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            QFExtensionCamera::CameraAcquititonFileDescription d;
            d.name=TIFFFIlename1;
            d.description="image stack from camera 1";
            d.type="TIFF16";
            files.append(d);


            QString ParamValuesFilename=acquisitionPrefix1+".param.dat";
            QFile posFile(ParamValuesFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(scanCSV.toAscii().data());
                posFile.close();
                d.name=ParamValuesFilename;
                d.description="parameter values";
                d.type="CSV";
                files.append(d);
            } else {
                log->log_error(tr("  - could not write parameter values file '%1' for camera 1: %2 ...").arg(ParamValuesFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=acqTools->saveMeasuredData(acquisitionPrefix1, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }

            log->log_text(tr("  - writing acquisition description 1 ..."));
            acqTools->savePreviewDescription(0, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription1, files, startDateTime);
            log->log_text(tr(" DONE!\n"));
        }
        if (ok && useCam2) {
            QMap<QString, QVariant> acquisitionDescription2=acquisitionDescription;
            QList<QFExtensionCamera::CameraAcquititonFileDescription> files;
            QFExtensionCamera::CameraAcquititonFileDescription d;
            d.name=TIFFFIlename1;
            d.description="image stack from camera 2";
            d.type="TIFF16";
            files.append(d);


            QString ParamValuesFilename=acquisitionPrefix2+".param.dat";
            QFile posFile(ParamValuesFilename);
            if (posFile.open(QIODevice::WriteOnly)) {
                posFile.write(scanCSV.toAscii().data());
                posFile.close();
                d.name=ParamValuesFilename;
                d.description="parameter values";
                d.type="CSV";
                files.append(d);
            } else {
                log->log_error(tr("  - could not write parameter values file '%1' for camera 2: %2 ...").arg(ParamValuesFilename).arg(posFile.errorString()));
            }

            QString MeasurementsFilename=acqTools->saveMeasuredData(acquisitionPrefix2, measured);
            if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                QFExtensionCamera::CameraAcquititonFileDescription d;
                d.name=MeasurementsFilename;
                d.description="measureable properties of setup";
                d.type="CSV";
                files.append(d);
            }


            log->log_text(tr("  - writing acquisition description 2 ..."));
            acqTools->savePreviewDescription(1, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription2, files, startDateTime);
            log->log_text(tr(" DONE!\n"));
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // release cameras
        //////////////////////////////////////////////////////////////////////////////////////
        if (useCam1) {
            opticsSetup->releaseCamera(0);
            log->log_text(tr("  - released camera 1!\n"));
        }
        if (useCam2) {
            opticsSetup->releaseCamera(1);
            log->log_text(tr("  - released camera 2!\n"));
        }

        if (ok && useCam1) log->log_text(tr("  - stored files with prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log->log_text(tr("  - stored files with prefix 2: '%1'\n").arg(acquisitionPrefix2));

        if (ok) log->log_text(tr("image stack acquisition DONE!\n"));
    }
    opticsSetup->unlockLightpath();
    opticsSetup->ensureLightpath();
}
