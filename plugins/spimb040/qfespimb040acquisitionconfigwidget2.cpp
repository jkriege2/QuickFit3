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

#include "qfespimb040acquisitionconfigwidget2.h"
#include "ui_qfespimb040acquisitionconfigwidget2.h"
#include "qfcameraconfigcombobox.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>
#include "qfcompleterfromfile.h"
#include "qfstyledbutton.h"
#include "qfespimb040acquisitiondescription.h"

QFESPIMB040AcquisitionConfigWidget2::QFESPIMB040AcquisitionConfigWidget2(QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040AcquisitionConfigWidget2)
{
    this->acqTools=acqTools;
    this->log=log;
    m_pluginServices=pluginServices;
    this->opticsSetup=opticsSetup;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
    ui->setupUi(this);
    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/");
    QFCompleterFromFile* c1=new QFCompleterFromFile(this);
    c1->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/acquisition_prefix1.txt");
    QFCompleterFromFile* c2=new QFCompleterFromFile(this);
    c2->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/ext_spimb040/completers/acquisition_prefix2.txt");
    ui->edtPrefix1->setCompleter(c1);
    ui->edtPrefix2->setCompleter(c2);
    ui->edtPrefix1->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtPrefix1, ui->edtPrefix1));
    ui->edtPrefix2->addButton(new QFStyledButton(QFStyledButton::SelectFromCompleter, ui->edtPrefix2, ui->edtPrefix2));


    if (opticsSetup) {
        ui->cmbAcquisitionSettings1->init(configDirectory);
        ui->cmbAcquisitionSettings1->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbAcquisitionSettings1->connectTo(opticsSetup->cameraComboBox(0));
        ui->cmbPreviewSettings1_1->init(configDirectory);
        ui->cmbPreviewSettings1_1->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbPreviewSettings1_1->connectTo(opticsSetup->cameraComboBox(0));
        ui->cmbPreviewSettings1_2->init(configDirectory);
        ui->cmbPreviewSettings1_2->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbPreviewSettings1_2->connectTo(opticsSetup->cameraComboBox(0));
        ui->cmbPreviewSettings1_3->init(configDirectory);
        ui->cmbPreviewSettings1_3->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbPreviewSettings1_3->connectTo(opticsSetup->cameraComboBox(0));
        ui->cmbPreviewSettings1_4->init(configDirectory);
        ui->cmbPreviewSettings1_4->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbPreviewSettings1_4->connectTo(opticsSetup->cameraComboBox(0));
        ui->cmbPreviewSettings1_5->init(configDirectory);
        ui->cmbPreviewSettings1_5->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbPreviewSettings1_5->connectTo(opticsSetup->cameraComboBox(0));
        ui->cmbPreviewSettings1_6->init(configDirectory);
        ui->cmbPreviewSettings1_6->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbPreviewSettings1_6->connectTo(opticsSetup->cameraComboBox(0));

        ui->cmbAcquisitionSettings2->init(configDirectory);
        ui->cmbAcquisitionSettings2->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbAcquisitionSettings2->connectTo(opticsSetup->cameraComboBox(1));
        ui->cmbPreviewSettings2_1->init(configDirectory);
        ui->cmbPreviewSettings2_1->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbPreviewSettings2_1->connectTo(opticsSetup->cameraComboBox(1));
        ui->cmbPreviewSettings2_2->init(configDirectory);
        ui->cmbPreviewSettings2_2->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbPreviewSettings2_2->connectTo(opticsSetup->cameraComboBox(1));
        ui->cmbPreviewSettings2_3->init(configDirectory);
        ui->cmbPreviewSettings2_3->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbPreviewSettings2_3->connectTo(opticsSetup->cameraComboBox(1));
        ui->cmbPreviewSettings2_4->init(configDirectory);
        ui->cmbPreviewSettings2_4->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbPreviewSettings2_4->connectTo(opticsSetup->cameraComboBox(1));
        ui->cmbPreviewSettings2_5->init(configDirectory);
        ui->cmbPreviewSettings2_5->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbPreviewSettings2_5->connectTo(opticsSetup->cameraComboBox(1));
        ui->cmbPreviewSettings2_6->init(configDirectory);
        ui->cmbPreviewSettings2_6->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbPreviewSettings2_6->connectTo(opticsSetup->cameraComboBox(1));
    }

    updateReplaces();
    bindLineEdit(ui->edtPrefix1);
    bindLineEdit(ui->edtPrefix2);

}

QFESPIMB040AcquisitionConfigWidget2::~QFESPIMB040AcquisitionConfigWidget2()
{
    delete ui;
}


void QFESPIMB040AcquisitionConfigWidget2::loadSettings(QSettings& settings, QString prefix) {
    ui->chkOverview->setChecked(settings.value(prefix+"overview", true).toBool());
    ui->chkCloseMainShutter->setChecked(settings.value(prefix+"chkCloseMainShutter", false).toBool());
    ui->chkBackground->setChecked(settings.value(prefix+"background", true).toBool());
    ui->spinBackgroundFrames1->setValue(settings.value(prefix+"background_frames1", 1000).toInt());
    ui->spinBackgroundFrames2->setValue(settings.value(prefix+"background_frames2", 1000).toInt());
    ui->chkUse1->setChecked(settings.value(prefix+"use1", true).toBool());
    ui->chkUse2->setChecked(settings.value(prefix+"use2", true).toBool());
    ui->chkReconfigureAlways1->setChecked(settings.value(prefix+"chkReconfigureAlways1", true).toBool());
    ui->chkReconfigureAlways2->setChecked(settings.value(prefix+"chkReconfigureAlways2", true).toBool());
    ui->chkMeasurementsPreview->setChecked(settings.value(prefix+"chkMeasurementsPreview", true).toBool());
    ui->chkReadMeasurements->setChecked(settings.value(prefix+"chkReadMeasurements", true).toBool());
    ui->edtPrefix1->setText(settings.value(prefix+"prefix1", "stack_cam1_%counter%").toString());
    ui->edtPrefix2->setText(settings.value(prefix+"prefix2", "stack_cam2_%counter%").toString());
    ui->cmbAcquisitionSettings1->setCurrentConfig(settings.value(prefix+"acqsettings1", "default").toString());
    ui->cmbAcquisitionSettings2->setCurrentConfig(settings.value(prefix+"acqsettings2", "default").toString());
    ui->cmbPreviewSettings1_1->setCurrentConfig(settings.value(prefix+"prevsettings1", "default").toString());
    ui->cmbPreviewSettings2_1->setCurrentConfig(settings.value(prefix+"prevsettings2", "default").toString());
    ui->cmbPreviewSettings1_2->setCurrentConfig(settings.value(prefix+"prevsettings12", "default").toString());
    ui->cmbPreviewSettings2_2->setCurrentConfig(settings.value(prefix+"prevsettings22", "default").toString());
    ui->cmbPreviewSettings1_3->setCurrentConfig(settings.value(prefix+"prevsettings13", "default").toString());
    ui->cmbPreviewSettings2_3->setCurrentConfig(settings.value(prefix+"prevsettings23", "default").toString());
    ui->cmbPreviewSettings1_4->setCurrentConfig(settings.value(prefix+"prevsettings14", "default").toString());
    ui->cmbPreviewSettings2_4->setCurrentConfig(settings.value(prefix+"prevsettings24", "default").toString());
    ui->cmbPreviewSettings1_5->setCurrentConfig(settings.value(prefix+"prevsettings15", "default").toString());
    ui->cmbPreviewSettings2_5->setCurrentConfig(settings.value(prefix+"prevsettings25", "default").toString());
    ui->cmbPreviewSettings1_6->setCurrentConfig(settings.value(prefix+"prevsettings16", "default").toString());
    ui->cmbPreviewSettings2_6->setCurrentConfig(settings.value(prefix+"prevsettings26", "default").toString());

    //ui->chkLightpath->setChecked(settings.value(prefix+"lightpath", false).toBool());
    ui->cmbLightpath->setCurrentIndex(settings.value(prefix+"lightpathidx", -1).toInt());

    ui->chkLightpathPreview2->setChecked(settings.value(prefix+"lightpath_preview2", false).toBool());
    ui->cmbLightpathPreview2->setCurrentIndex(settings.value(prefix+"lightpathidx_preview2", -1).toInt());
    ui->chkLightpathPreview3->setChecked(settings.value(prefix+"lightpath_preview3", false).toBool());
    ui->cmbLightpathPreview3->setCurrentIndex(settings.value(prefix+"lightpathidx_preview3", -1).toInt());
    ui->chkLightpathPreview4->setChecked(settings.value(prefix+"lightpath_preview4", false).toBool());
    ui->cmbLightpathPreview4->setCurrentIndex(settings.value(prefix+"lightpathidx_preview4", -1).toInt());
    ui->chkLightpathPreview5->setChecked(settings.value(prefix+"lightpath_preview5", false).toBool());
    ui->cmbLightpathPreview5->setCurrentIndex(settings.value(prefix+"lightpathidx_preview5", -1).toInt());
    ui->chkLightpathPreview6->setChecked(settings.value(prefix+"lightpath_preview6", false).toBool());
    ui->cmbLightpathPreview6->setCurrentIndex(settings.value(prefix+"lightpathidx_preview6", -1).toInt());
    ui->chkNoBackground1->setChecked(settings.value(prefix+"chkNoBackground1", false).toBool());
    ui->chkNoPreview1->setChecked(settings.value(prefix+"chkNoPreview1", false).toBool());
    ui->chkNoBackground2->setChecked(settings.value(prefix+"chkNoBackground2", false).toBool());
    ui->chkNoPreview2->setChecked(settings.value(prefix+"chkNoPreview2", false).toBool());

    //qDebug()<<settings.value(prefix+"prevsettings1", "default").toString();
    on_chkUse1_toggled(ui->chkUse1->isChecked());
    on_chkUse2_toggled(ui->chkUse2->isChecked());
    on_chkBackground_toggled(ui->chkBackground->isChecked());
}


void QFESPIMB040AcquisitionConfigWidget2::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"chkReadMeasurements", ui->chkReadMeasurements->isChecked());
    settings.setValue(prefix+"use1", ui->chkUse1->isChecked());
    settings.setValue(prefix+"use2", ui->chkUse2->isChecked());
    settings.setValue(prefix+"prefix1", ui->edtPrefix1->text());
    settings.setValue(prefix+"prefix2", ui->edtPrefix2->text());
    settings.setValue(prefix+"chkReconfigureAlways1", ui->chkReconfigureAlways1->isChecked());
    settings.setValue(prefix+"chkReconfigureAlways2", ui->chkReconfigureAlways2->isChecked());
    settings.setValue(prefix+"chkMeasurementsPreview", ui->chkMeasurementsPreview->isChecked());



    settings.setValue(prefix+"overview", ui->chkOverview->isChecked());
    settings.setValue(prefix+"chkCloseMainShutter", ui->chkCloseMainShutter->isChecked());
    settings.setValue(prefix+"background", ui->chkBackground->isChecked());
    settings.setValue(prefix+"background_frames1", ui->spinBackgroundFrames1->value());
    settings.setValue(prefix+"background_frames2", ui->spinBackgroundFrames2->value());
    settings.setValue(prefix+"acqsettings1", ui->cmbAcquisitionSettings1->currentConfigName());
    settings.setValue(prefix+"acqsettings2", ui->cmbAcquisitionSettings2->currentConfigName());
    settings.setValue(prefix+"prevsettings1", ui->cmbPreviewSettings1_1->currentConfigName());
    settings.setValue(prefix+"prevsettings2", ui->cmbPreviewSettings2_1->currentConfigName());
    settings.setValue(prefix+"prevsettings12", ui->cmbPreviewSettings1_2->currentConfigName());
    settings.setValue(prefix+"prevsettings22", ui->cmbPreviewSettings2_2->currentConfigName());
    settings.setValue(prefix+"prevsettings13", ui->cmbPreviewSettings1_3->currentConfigName());
    settings.setValue(prefix+"prevsettings23", ui->cmbPreviewSettings2_3->currentConfigName());
    settings.setValue(prefix+"prevsettings14", ui->cmbPreviewSettings1_4->currentConfigName());
    settings.setValue(prefix+"prevsettings24", ui->cmbPreviewSettings2_4->currentConfigName());
    settings.setValue(prefix+"prevsettings15", ui->cmbPreviewSettings1_5->currentConfigName());
    settings.setValue(prefix+"prevsettings25", ui->cmbPreviewSettings2_5->currentConfigName());
    settings.setValue(prefix+"prevsettings16", ui->cmbPreviewSettings1_6->currentConfigName());
    settings.setValue(prefix+"prevsettings26", ui->cmbPreviewSettings2_6->currentConfigName());

    settings.setValue(prefix+"lightpathidx", ui->cmbLightpath->currentIndex());
    //settings.setValue(prefix+"lightpath", ui->chkLightpath->isChecked());
    settings.setValue(prefix+"lightpathidx_preview2", ui->cmbLightpathPreview2->currentIndex());
    settings.setValue(prefix+"lightpath_preview2", ui->chkLightpathPreview2->isChecked());
    settings.setValue(prefix+"lightpathidx_preview3", ui->cmbLightpathPreview3->currentIndex());
    settings.setValue(prefix+"lightpath_preview3", ui->chkLightpathPreview3->isChecked());
    settings.setValue(prefix+"lightpathidx_preview4", ui->cmbLightpathPreview4->currentIndex());
    settings.setValue(prefix+"lightpath_preview4", ui->chkLightpathPreview4->isChecked());
    settings.setValue(prefix+"lightpathidx_preview5", ui->cmbLightpathPreview5->currentIndex());
    settings.setValue(prefix+"lightpath_preview5", ui->chkLightpathPreview5->isChecked());
    settings.setValue(prefix+"lightpathidx_preview6", ui->cmbLightpathPreview6->currentIndex());
    settings.setValue(prefix+"lightpath_preview6", ui->chkLightpathPreview6->isChecked());
    settings.setValue(prefix+"chkNoBackground1", ui->chkNoBackground1->isChecked());
    settings.setValue(prefix+"chkNoPreview1", ui->chkNoPreview1->isChecked());
    settings.setValue(prefix+"chkNoBackground2", ui->chkNoBackground2->isChecked());
    settings.setValue(prefix+"chkNoPreview2", ui->chkNoPreview2->isChecked());
}




QString QFESPIMB040AcquisitionConfigWidget2::prefix1()  {
    updateReplaces();
    QString filename= ui->edtPrefix1->text();
    filename=transformFilename(filename);
    return filename;
}

QString QFESPIMB040AcquisitionConfigWidget2::prefix2()  {
    updateReplaces();
    QString filename= ui->edtPrefix2->text();
    filename=transformFilename(filename);
    return filename;
}

bool QFESPIMB040AcquisitionConfigWidget2::use1() const {
    return ui->chkUse1->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget2::use2() const {
    return ui->chkUse2->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget2::overview() const {
    return ui->chkOverview->isChecked();
}



int QFESPIMB040AcquisitionConfigWidget2::frames1() const {
    return ui->spinFrames->value();
}

int QFESPIMB040AcquisitionConfigWidget2::frames2() const {
    return ui->spinFrames2->value();
}

QVariant QFESPIMB040AcquisitionConfigWidget2::getCameraSetting1(QFExtensionCamera::CameraSetting which) const
{
    if (which==QFExtensionCamera::CamSetNumberFrames) return frames1();
    else return camset1.value(which, QVariant());
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraSetting1(QFExtensionCamera::CameraSetting which, QVariant value)
{
    if (which==QFExtensionCamera::CamSetNumberFrames) ui->spinFrames->setValue(value.toInt());
    else camset1[which]=value;
}

QVariant QFESPIMB040AcquisitionConfigWidget2::getCameraSetting2(QFExtensionCamera::CameraSetting which) const
{
    if (which==QFExtensionCamera::CamSetNumberFrames) return frames2();
    else return camset2.value(which, QVariant());
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraSetting2(QFExtensionCamera::CameraSetting which, QVariant value)
{
    if (which==QFExtensionCamera::CamSetNumberFrames) ui->spinFrames2->setValue(value.toInt());
    else camset2[which]=value;
}




QVariant QFESPIMB040AcquisitionConfigWidget2::getCameraSetting1(const QString& which) const
{
    return camsetstr1.value(which, QVariant());
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraSetting1(const QString& which, QVariant value)
{
    camsetstr1[which]=value;
}

QVariant QFESPIMB040AcquisitionConfigWidget2::getCameraSetting2(const QString& which) const
{
    return camsetstr2.value(which, QVariant());
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraSetting2(const QString& which, QVariant value)
{
    camsetstr2[which]=value;
}



void QFESPIMB040AcquisitionConfigWidget2::clearCameraSettings1()
{
    camset1.clear();
    camsetstr1.clear();
    ui->spinFrames->setValue(ui->spinFrames->minimum());
}

QMap<QFExtensionCamera::CameraSetting, QVariant> QFESPIMB040AcquisitionConfigWidget2::getCameraSettings1() const
{
    QMap<QFExtensionCamera::CameraSetting, QVariant> c=camset1;
    if (frames1()>0) c[QFExtensionCamera::CamSetNumberFrames]=frames1();
    return c;
}
QMap<QFExtensionCamera::CameraSetting, QVariant> QFESPIMB040AcquisitionConfigWidget2::getCameraSettings2() const
{
    QMap<QFExtensionCamera::CameraSetting, QVariant> c=camset2;
    if (frames2()>0) c[QFExtensionCamera::CamSetNumberFrames]=frames2();
    return c;
}

QMap<QString, QVariant> QFESPIMB040AcquisitionConfigWidget2::getCameraSettingsStr1() const
{
    return camsetstr1;
}
QMap<QString, QVariant> QFESPIMB040AcquisitionConfigWidget2::getCameraSettingsStr2() const
{
    return camsetstr2;
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraConfigAcq(int camera, const QString &id)
{
    if (camera==0) ui->cmbAcquisitionSettings1->setCurrentConfig(id);
    else if (camera==1) ui->cmbAcquisitionSettings2->setCurrentConfig(id);
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraConfigAcq(int camera, int id)
{
    if (camera==0) ui->cmbAcquisitionSettings1->setCurrentConfig(id);
    else if (camera==1) ui->cmbAcquisitionSettings2->setCurrentConfig(id);
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraConfigOverview(int camera, int preview, const QString &id)
{
    if (camera==0) {
        if (preview==0) ui->cmbPreviewSettings1_1->setCurrentConfig(id);
        else if (preview==1) ui->cmbPreviewSettings1_2->setCurrentConfig(id);
        else if (preview==2) ui->cmbPreviewSettings1_3->setCurrentConfig(id);
        else if (preview==3) ui->cmbPreviewSettings1_4->setCurrentConfig(id);
        else if (preview==4) ui->cmbPreviewSettings1_5->setCurrentConfig(id);
        else if (preview==5) ui->cmbPreviewSettings1_6->setCurrentConfig(id);
    } else if (camera==1) {
        if (preview==0) ui->cmbPreviewSettings2_1->setCurrentConfig(id);
        else if (preview==1) ui->cmbPreviewSettings2_2->setCurrentConfig(id);
        else if (preview==2) ui->cmbPreviewSettings2_3->setCurrentConfig(id);
        else if (preview==3) ui->cmbPreviewSettings2_4->setCurrentConfig(id);
        else if (preview==4) ui->cmbPreviewSettings2_5->setCurrentConfig(id);
        else if (preview==5) ui->cmbPreviewSettings2_6->setCurrentConfig(id);
    }
}

void QFESPIMB040AcquisitionConfigWidget2::setCameraConfigOverview(int camera, int preview, int id)
{
    if (camera==0) {
        if (preview==0) ui->cmbPreviewSettings1_1->setCurrentConfig(id);
        else if (preview==1) ui->cmbPreviewSettings1_2->setCurrentConfig(id);
        else if (preview==2) ui->cmbPreviewSettings1_3->setCurrentConfig(id);
        else if (preview==3) ui->cmbPreviewSettings1_4->setCurrentConfig(id);
        else if (preview==4) ui->cmbPreviewSettings1_5->setCurrentConfig(id);
        else if (preview==5) ui->cmbPreviewSettings1_6->setCurrentConfig(id);
    } else if (camera==1) {
        if (preview==0) ui->cmbPreviewSettings2_1->setCurrentConfig(id);
        else if (preview==1) ui->cmbPreviewSettings2_2->setCurrentConfig(id);
        else if (preview==2) ui->cmbPreviewSettings2_3->setCurrentConfig(id);
        else if (preview==3) ui->cmbPreviewSettings2_4->setCurrentConfig(id);
        else if (preview==4) ui->cmbPreviewSettings2_5->setCurrentConfig(id);
        else if (preview==5) ui->cmbPreviewSettings2_6->setCurrentConfig(id);
    }
}

void QFESPIMB040AcquisitionConfigWidget2::clearCameraSettings2()
{
    camset2.clear();
    camsetstr2.clear();
    ui->spinFrames2->setValue(ui->spinFrames2->minimum());
}



bool QFESPIMB040AcquisitionConfigWidget2::lightpathActivated() const {
    return true; //ui->chkLightpath->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget2::lightpathActivatedPreview2() const {
    return ui->chkLightpathPreview2->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget2::lightpathActivatedPreview3() const {
    return ui->chkLightpathPreview3->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget2::lightpathActivatedPreview4() const {
    return ui->chkLightpathPreview4->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget2::lightpathActivatedPreview5() const {
    return ui->chkLightpathPreview5->isChecked();
}

bool QFESPIMB040AcquisitionConfigWidget2::lightpathActivatedPreview6() const {
    return ui->chkLightpathPreview6->isChecked();
}
QString QFESPIMB040AcquisitionConfigWidget2::lightpathFilename() const {
    return ui->cmbLightpath->itemData(ui->cmbLightpath->currentIndex()).toString();
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpath() const {
    return ui->cmbLightpath->currentText();
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpathFilenamePreview2() const {
    return ui->cmbLightpathPreview2->itemData(ui->cmbLightpathPreview2->currentIndex()).toString();
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpathPreview2() const {
    return ui->cmbLightpathPreview2->currentText();
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpathFilenamePreview3() const {
    return ui->cmbLightpathPreview3->itemData(ui->cmbLightpathPreview3->currentIndex()).toString();
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpathFilenamePreview4() const {
    return ui->cmbLightpathPreview4->itemData(ui->cmbLightpathPreview4->currentIndex()).toString();
}
QString QFESPIMB040AcquisitionConfigWidget2::lightpathFilenamePreview5() const {
    return ui->cmbLightpathPreview5->itemData(ui->cmbLightpathPreview5->currentIndex()).toString();
}
QString QFESPIMB040AcquisitionConfigWidget2::lightpathFilenamePreview6() const {
    return ui->cmbLightpathPreview6->itemData(ui->cmbLightpathPreview6->currentIndex()).toString();
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpathPreview3() const {
    return ui->cmbLightpathPreview3->currentText();
}
QString QFESPIMB040AcquisitionConfigWidget2::lightpathPreview4() const {
    return ui->cmbLightpathPreview4->currentText();
}
QString QFESPIMB040AcquisitionConfigWidget2::lightpathPreview5() const {
    return ui->cmbLightpathPreview5->currentText();
}
QString QFESPIMB040AcquisitionConfigWidget2::lightpathPreview6() const {
    return ui->cmbLightpathPreview6->currentText();
}
void QFESPIMB040AcquisitionConfigWidget2::on_btnAcquire_clicked() {

    emit doAcquisition();
    QApplication::processEvents();
    performAcquisition();
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcquireNext_clicked()
{
    acqDescription->nextCell();
    on_btnAcquire_clicked();
}

void QFESPIMB040AcquisitionConfigWidget2::on_chkUse1_toggled(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->btnAcquireNext->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkOverview->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkBackground->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->edtPrefix1->setEnabled(ui->chkUse1->isChecked());
    ui->widCam2Settings1->setEnabled(ui->chkUse1->isChecked());

    updateBackgroundWidgets();
}

void QFESPIMB040AcquisitionConfigWidget2::on_chkUse2_toggled(bool enabled) {
    ui->btnAcquire->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->btnAcquireNext->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkOverview->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->chkBackground->setEnabled(ui->chkUse1->isChecked() || ui->chkUse2->isChecked());
    ui->edtPrefix2->setEnabled(ui->chkUse2->isChecked());
    ui->widCam2Settings2->setEnabled(ui->chkUse2->isChecked());

    updateBackgroundWidgets();
}

void QFESPIMB040AcquisitionConfigWidget2::updateBackgroundWidgets() {
    bool updt=updatesEnabled() && isVisible();
    if (updt) setUpdatesEnabled(false);
    ui->cmbPreviewSettings1_1->setEnabled(ui->chkOverview->isChecked());
    ui->cmbPreviewSettings2_1->setEnabled(ui->chkOverview->isChecked());
    ui->cmbPreviewSettings1_2->setEnabled(ui->chkLightpathPreview2->isChecked());
    ui->cmbPreviewSettings2_2->setEnabled(ui->chkLightpathPreview2->isChecked());
    ui->cmbPreviewSettings1_3->setEnabled(ui->chkLightpathPreview3->isChecked());
    ui->cmbPreviewSettings2_3->setEnabled(ui->chkLightpathPreview3->isChecked());
    ui->cmbPreviewSettings1_4->setEnabled(ui->chkLightpathPreview4->isChecked());
    ui->cmbPreviewSettings2_4->setEnabled(ui->chkLightpathPreview4->isChecked());
    ui->cmbPreviewSettings1_5->setEnabled(ui->chkLightpathPreview5->isChecked());
    ui->cmbPreviewSettings2_5->setEnabled(ui->chkLightpathPreview5->isChecked());
    ui->cmbPreviewSettings1_6->setEnabled(ui->chkLightpathPreview6->isChecked());
    ui->cmbPreviewSettings2_6->setEnabled(ui->chkLightpathPreview6->isChecked());
    ui->spinBackgroundFrames1->setEnabled(ui->chkBackground->isChecked());
    ui->spinBackgroundFrames2->setEnabled(ui->chkBackground->isChecked());
    ui->labPreviewFrames1->setEnabled(ui->chkBackground->isChecked());
    ui->labPreviewFrames2->setEnabled(ui->chkBackground->isChecked());
    if (updt) setUpdatesEnabled(updt);
}

void QFESPIMB040AcquisitionConfigWidget2::on_chkBackground_toggled(bool enabled) {
    on_chkUse1_toggled(ui->chkUse1->isChecked());
    on_chkUse2_toggled(ui->chkUse2->isChecked());
}

QString QFESPIMB040AcquisitionConfigWidget2::currentConfigFilename(int camera) const {
    if (camera==0) return ui->cmbAcquisitionSettings1->currentConfigFilename();
    if (camera==1) return ui->cmbAcquisitionSettings2->currentConfigFilename();
    return "";
}

QString QFESPIMB040AcquisitionConfigWidget2::currentConfigName(int camera) const {
    if (camera==0) return ui->cmbAcquisitionSettings1->currentConfigName();
    if (camera==1) return ui->cmbAcquisitionSettings2->currentConfigName();
    return "";
}

QString QFESPIMB040AcquisitionConfigWidget2::currentPreviewConfigFilename(int camera, int preview) const {
    if (preview==0) {
        if (camera==0) return ui->cmbPreviewSettings1_1->currentConfigFilename();
        if (camera==1) return ui->cmbPreviewSettings2_1->currentConfigFilename();
    }
    if (preview==1) {
        if (camera==0) return ui->cmbPreviewSettings1_2->currentConfigFilename();
        if (camera==1) return ui->cmbPreviewSettings2_2->currentConfigFilename();
    }
    if (preview==2) {
        if (camera==0) return ui->cmbPreviewSettings1_3->currentConfigFilename();
        if (camera==1) return ui->cmbPreviewSettings2_3->currentConfigFilename();
    }
    if (preview==3) {
        if (camera==0) return ui->cmbPreviewSettings1_4->currentConfigFilename();
        if (camera==1) return ui->cmbPreviewSettings2_4->currentConfigFilename();
    }
    if (preview==4) {
        if (camera==0) return ui->cmbPreviewSettings1_5->currentConfigFilename();
        if (camera==1) return ui->cmbPreviewSettings2_5->currentConfigFilename();
    }
    if (preview==5) {
        if (camera==0) return ui->cmbPreviewSettings1_6->currentConfigFilename();
        if (camera==1) return ui->cmbPreviewSettings2_6->currentConfigFilename();
    }
    return "";
}

QString QFESPIMB040AcquisitionConfigWidget2::currentPreviewConfigName(int camera, int preview) const {
    if (preview==0) {
        if (camera==0) return ui->cmbPreviewSettings1_1->currentConfigName();
        if (camera==1) return ui->cmbPreviewSettings2_1->currentConfigName();
    }
    if (preview==1) {
        if (camera==0) return ui->cmbPreviewSettings1_2->currentConfigName();
        if (camera==1) return ui->cmbPreviewSettings2_2->currentConfigName();
    }
    if (preview==2) {
        if (camera==0) return ui->cmbPreviewSettings1_3->currentConfigName();
        if (camera==1) return ui->cmbPreviewSettings2_3->currentConfigName();
    }
    if (preview==3) {
        if (camera==0) return ui->cmbPreviewSettings1_4->currentConfigName();
        if (camera==1) return ui->cmbPreviewSettings2_4->currentConfigName();
    }
    if (preview==5) {
        if (camera==0) return ui->cmbPreviewSettings1_5->currentConfigName();
        if (camera==1) return ui->cmbPreviewSettings2_5->currentConfigName();
    }
    if (preview==6) {
        if (camera==0) return ui->cmbPreviewSettings1_6->currentConfigName();
        if (camera==1) return ui->cmbPreviewSettings2_6->currentConfigName();
    }
    return "";
}

int QFESPIMB040AcquisitionConfigWidget2::currentBackgroundFrames(int camera) const {
    if (camera==0) return ui->spinBackgroundFrames1->value();
    if (camera==1) return ui->spinBackgroundFrames2->value();
    return 1;
}

bool QFESPIMB040AcquisitionConfigWidget2::saveBackground() const {
    return ui->chkBackground->isChecked();
}

void QFESPIMB040AcquisitionConfigWidget2::lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes) {
    QString idx=ui->cmbLightpath->currentText();
    QString idx2=ui->cmbLightpathPreview2->currentText();
    QString idx3=ui->cmbLightpathPreview3->currentText();
    QString idx4=ui->cmbLightpathPreview4->currentText();
    QString idx5=ui->cmbLightpathPreview5->currentText();
    QString idx6=ui->cmbLightpathPreview6->currentText();
    ui->cmbLightpath->clear();
    ui->cmbLightpathPreview2->clear();
    ui->cmbLightpathPreview3->clear();
    ui->cmbLightpathPreview4->clear();
    ui->cmbLightpathPreview5->clear();
    ui->cmbLightpathPreview6->clear();
    for (int i=0; i<lightpathes.size(); i++) {
        QTriple<QIcon, QString, QString> p=lightpathes[i];
        ui->cmbLightpath->addItem(p.first, p.second, p.third);
        ui->cmbLightpathPreview2->addItem(p.first, p.second, p.third);
        ui->cmbLightpathPreview3->addItem(p.first, p.second, p.third);
        ui->cmbLightpathPreview4->addItem(p.first, p.second, p.third);
        ui->cmbLightpathPreview5->addItem(p.first, p.second, p.third);
        ui->cmbLightpathPreview6->addItem(p.first, p.second, p.third);
    }
    ui->cmbLightpath->setCurrentIndex(qMax(0, ui->cmbLightpath->findText(idx)));
    ui->cmbLightpathPreview2->setCurrentIndex(qMax(0, ui->cmbLightpathPreview2->findText(idx2)));
    ui->cmbLightpathPreview3->setCurrentIndex(qMax(0, ui->cmbLightpathPreview3->findText(idx3)));
    ui->cmbLightpathPreview4->setCurrentIndex(qMax(0, ui->cmbLightpathPreview4->findText(idx4)));
    ui->cmbLightpathPreview5->setCurrentIndex(qMax(0, ui->cmbLightpathPreview5->findText(idx5)));
    ui->cmbLightpathPreview6->setCurrentIndex(qMax(0, ui->cmbLightpathPreview6->findText(idx6)));
}


void QFESPIMB040AcquisitionConfigWidget2::updateReplaces()
{
    setGlobalReplaces(opticsSetup, expDescription, acqDescription);
    setReplaceValue("acquisition1_name",  cleanStringForFilename(ui->cmbAcquisitionSettings1->currentConfigName()));
    setReplaceValue("acquisition2_name",  cleanStringForFilename(ui->cmbAcquisitionSettings2->currentConfigName()));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnReset1_clicked()
{
    ui->spinFrames->setValue(ui->spinFrames->minimum());
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnReset2_clicked()
{
    ui->spinFrames2->setValue(ui->spinFrames2->minimum());
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1Test_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbAcquisitionSettings1->currentConfigFilename(), lightpathFilenamePreview(0));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1Test_released()
{
    opticsSetup->resetCameraPreview(0);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview1_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbPreviewSettings1_1->currentConfigFilename(), lightpathFilenamePreview(0));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview1_released()
{
    opticsSetup->resetCameraPreview(0);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview2_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbPreviewSettings1_2->currentConfigFilename(), lightpathFilenamePreview(1));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview2_released()
{
    opticsSetup->resetCameraPreview(0);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview3_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbPreviewSettings1_3->currentConfigFilename(), lightpathFilenamePreview(2));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview3_released()
{
    opticsSetup->resetCameraPreview(0);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview4_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbPreviewSettings1_4->currentConfigFilename(), lightpathFilenamePreview(3));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview4_released()
{
    opticsSetup->resetCameraPreview(0);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview5_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbPreviewSettings1_5->currentConfigFilename(), lightpathFilenamePreview(4));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview5_released()
{
    opticsSetup->resetCameraPreview(0);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview6_pressed()
{
    opticsSetup->overrideCameraPreview(0, ui->cmbPreviewSettings1_6->currentConfigFilename(), lightpathFilenamePreview(5));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq1TestPreview6_released()
{
    opticsSetup->resetCameraPreview(0);
}






void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2Test_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbAcquisitionSettings2->currentConfigFilename(), lightpathFilenamePreview(1));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2Test_released()
{
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview1_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbPreviewSettings2_1->currentConfigFilename(), lightpathFilenamePreview(1));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview1_released()
{
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview2_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbPreviewSettings2_2->currentConfigFilename(), lightpathFilenamePreview(1));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview2_released()
{
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview3_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbPreviewSettings2_3->currentConfigFilename(), lightpathFilenamePreview(2));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview3_released()
{
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview4_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbPreviewSettings2_4->currentConfigFilename(), lightpathFilenamePreview(3));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview4_released()
{
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview5_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbPreviewSettings2_5->currentConfigFilename(), lightpathFilenamePreview(4));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview5_released()
{
    opticsSetup->resetCameraPreview(1);
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview6_pressed()
{
    opticsSetup->overrideCameraPreview(1, ui->cmbPreviewSettings2_6->currentConfigFilename(), lightpathFilenamePreview(5));
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnAcq2TestPreview6_released()
{
    opticsSetup->resetCameraPreview(1);
}




int QFESPIMB040AcquisitionConfigWidget2::repeats() const
{
    return ui->spinRepeat->value();
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpathPreview(int preview) {
    if (preview==0) {
        return lightpath();
    } else if (preview==1) {
        return lightpathPreview2();
    } else if (preview==2) {
        return lightpathPreview3();
    } else if (preview==3) {
        return lightpathPreview4();
    } else if (preview==4) {
        return lightpathPreview5();
    } else if (preview==5) {
        return lightpathPreview6();
    }
    return "";
}

QString QFESPIMB040AcquisitionConfigWidget2::lightpathFilenamePreview(int preview) {
    if (preview==0) {
        return lightpathFilename();
    } else if (preview==1) {
        return lightpathFilenamePreview2();
    } else if (preview==2) {
        return lightpathFilenamePreview3();
    } else if (preview==3) {
        return lightpathFilenamePreview4();
    } else if (preview==4) {
        return lightpathFilenamePreview5();
    } else if (preview==5) {
        return lightpathFilenamePreview6();
    }
    return "";
}

bool QFESPIMB040AcquisitionConfigWidget2::lightpathActivatedPreview(int preview)
{
    if (preview==0) {
        return ui->chkOverview->isChecked();
    } else if (preview==1) {
        return lightpathActivatedPreview2();
    } else if (preview==2) {
        return lightpathActivatedPreview3();
    } else if (preview==3) {
        return lightpathActivatedPreview4();
    } else if (preview==4) {
        return lightpathActivatedPreview5();
    } else if (preview==5) {
        return lightpathActivatedPreview6();
    }
    return false;
}

int QFESPIMB040AcquisitionConfigWidget2::previewCount()
{
    return 6;
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnSaveTemplate_clicked()
{
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
    QDir().mkpath(ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040AcquisitionConfigWidget2/lasttemplatedir", ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/").toString();
    QString filename=qfGetSaveFileName(this, tr("save as template ..."), dir, tr("image acquisition configuration (*.iac)"))    ;
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
            storeSettings(set, "image_series_settings/");
            dir=QFileInfo(filename).absolutePath();
        }
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040AcquisitionConfigWidget2/lasttemplatedir", dir);
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
}

void QFESPIMB040AcquisitionConfigWidget2::on_btnLoadTemplate_clicked()
{
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->stop();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->stop();
    QDir().mkpath(ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/");
    QString dir=ProgramOptions::getInstance()->getQSettings()->value("QFESPIMB040AcquisitionConfigWidget2/lasttemplatedir", ProgramOptions::getInstance()->getHomeQFDirectory()+"/acq_templates/").toString();
    QString filename=qfGetOpenFileName(this, tr("open template ..."), dir, tr("image acquisition configuration (*.iac)"))    ;
    if (!filename.isEmpty()) {
        QSettings set(filename, QSettings::IniFormat);
        loadSettings(set, "image_series_settings/");
        dir=QFileInfo(filename).absolutePath();
    }
    ProgramOptions::getInstance()->getQSettings()->setValue("QFESPIMB040AcquisitionConfigWidget2/lasttemplatedir", dir);
    if (opticsSetup->getStopRelease(0)) opticsSetup->getStopRelease(0)->resume();
    if (opticsSetup->getStopRelease(1)) opticsSetup->getStopRelease(1)->resume();
}



#define ACQUISITION_ERROR(message) \
    log->log_error(QString("  - ")+(message)+QString("\n")); \
    QMessageBox::critical(this, tr("B040SPIM: Image Stack Acquisition Error"), (message));


void QFESPIMB040AcquisitionConfigWidget2::performAcquisition()
{
    if (!(use1() || use2())) return;

    QDateTime startDateTime=QDateTime::currentDateTime();
    QList<QFESPIMB040OpticsSetupBase::measuredValues> measured;

    bool ok=true;
    log->log_text(tr("starting image series acquisition:\n"));
    opticsSetup->lockLightpath();

    int repeatCnt=0;
    bool userCanceled=false;


    bool measureDuringAcquisitions=ui->chkReadMeasurements->isChecked();
    if (measureDuringAcquisitions)
        log->log_text(tr("  - saving measurements image series acquisition:\n"));
    else
        log->log_text(tr("  - don't save measurements during image series acquisition:\n"));

    QProgressListDialog progress(tr("Image Series Acquisition"), tr("&Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    //progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.addItem(tr("performing run %1/%2").arg(1).arg(repeats()));
    progress.addItem(tr("initializing cameras"));
    progress.addItem(tr("preparing cameras"));
    if (saveBackground()) progress.addItem(tr("acquire background images"));
    progress.addItem(tr("acquire overview images before acquisition"));
    progress.addItem(tr("acquire image series"));
    progress.addItem(tr("acquire overview images after acquisition"));
    progress.addItem(tr("clean up"));
    progress.setHasProgressBar(true);
    progress.show();
    progress.defineIcon(QProgressListWidget::statusUser, QIcon(":/spimb040/clock.png"));
    progress.setItemStatus(0, QProgressListWidget::statusUser);


    while (ok && repeatCnt<repeats() && !userCanceled) {
        progress.reset();
        progress.start();
        progress.setItemText(0, tr("performing run %1/%2").arg(repeatCnt+1).arg(repeats()));


        //////////////////////////////////////////////////////////////////////////////////////
        // switch off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()){
            log->log_text(tr("  - switch main shutter on!\n"));
            opticsSetup->setMainIlluminationShutter(false, false);
        }


        progress.setLabelText(tr("setting up acquisition %1/%2 ...").arg(repeatCnt+1).arg(repeats()));
        ok=true;
        //////////////////////////////////////////////////////////////////////////////////////
        // collect common acquisition data
        //////////////////////////////////////////////////////////////////////////////////////
        QMap<QString, QVariant> acquisitionDescription1, acquisitionDescription2;
        QMap<QString, QVariant> backgroundDescription1, backgroundDescription2;
        acquisitionDescription1["type"]="acquisition";
        acquisitionDescription2["type"]="acquisition";
        QList<QFExtensionCamera::CameraAcquititonFileDescription> moreFiles1, moreFiles2;
        QList<QFExtensionCamera::CameraAcquititonFileDescription> backgroundFiles1, backgroundFiles2;


        //////////////////////////////////////////////////////////////////////////////////////
        // SET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        opticsSetup->lockLighpathCombobox();
        QString oldLightpath=opticsSetup->getCurrentLightpathFilename();
        QString lightpathName=lightpath();
        if (lightpathActivated()) {
            if (!QFile::exists(lightpathFilename())) {
                ACQUISITION_ERROR(tr("  - acquisition lighpath configuration '%1' does not exist!\n").arg(lightpath()));
                opticsSetup->unlockLightpath();
                return;

            } else {
                log->log_text(tr("  - setting acquisition lightpath settings '%1' ...\n").arg(lightpath()));
                opticsSetup->loadLightpathConfig(lightpathFilename(), true);
                log->log_text(tr("  - setting acquisition lightpath settings '%1' ... DONE\n").arg(lightpath()));
            }
        }

        QApplication::processEvents();
        if (progress.wasCanceled())  {
            userCanceled=true;
            ok=false;
        }


        //////////////////////////////////////////////////////////////////////////////////////
        // lock cameras for use by this routine
        //////////////////////////////////////////////////////////////////////////////////////
        bool useCam1=false;
        QFExtension* extension1=NULL;
        QFExtensionCamera* ecamera1=NULL;
        int camera1=0;
        QString acquisitionSettingsFilename1="", previewSettingsFilename1="";
        QString acquisitionPrefix1=prefix1();
        QString acquisitionPrefix2=prefix2();
        progress.nextItem();
        QApplication::processEvents();
        if (progress.wasCanceled())  {
            userCanceled=true;
            ok=false;
        }



        //int backgroundFrames1=1;
        if (use1()) {
            if (!QDir().mkpath(QFileInfo(acquisitionPrefix1+".txt").absolutePath())) {
                ok=false;
                ACQUISITION_ERROR(tr("  - could not create directory '%1'!\n").arg(QFileInfo(acquisitionPrefix1+".txt").absolutePath()));
            }


            useCam1=opticsSetup->lockCamera(0,&extension1, &ecamera1, &camera1, &previewSettingsFilename1);
            if (QFile::exists(currentConfigFilename(0))) acquisitionSettingsFilename1=currentConfigFilename(0);
            if (QFile::exists(currentPreviewConfigFilename(0))) previewSettingsFilename1=currentPreviewConfigFilename(0);
            //backgroundFrames1=currentBackgroundFrames(0);
        }

        bool useCam2=false;
        QFExtension* extension2=NULL;
        QFExtensionCamera* ecamera2=NULL;
        QString acquisitionSettingsFilename2="", previewSettingsFilename2="";
        int camera2=0;
        //int backgroundFrames2=1;
        if (ok && use2()) {
            if (!QDir().mkpath(QFileInfo(acquisitionPrefix2+".txt").absolutePath())) {
                ok=false;
                ACQUISITION_ERROR(tr("  - could not create directory '%1'!\n").arg(QFileInfo(acquisitionPrefix2+".txt").absolutePath()));
            }
            useCam2=opticsSetup->lockCamera(1,&extension2, &ecamera2, &camera2, &previewSettingsFilename2);
            if (QFile::exists(currentConfigFilename(1))) acquisitionSettingsFilename2=currentConfigFilename(1);
            if (QFile::exists(currentPreviewConfigFilename(1))) previewSettingsFilename2=currentPreviewConfigFilename(1);
            //backgroundFrames2=currentBackgroundFrames(1);
        }


        progress.setValue(0);


        if (ok && useCam1) log->log_text(tr("  - prefix 1: '%1'\n").arg(acquisitionPrefix1));
        if (ok && useCam2) log->log_text(tr("  - prefix 2: '%1'\n").arg(acquisitionPrefix2));
        if (ok && useCam1) log->log_text(tr("  - acquisition settings 1: '%1'\n").arg(acquisitionSettingsFilename1));
        if (ok && useCam2) log->log_text(tr("  - acquisition settings 2: '%1'\n").arg(acquisitionSettingsFilename2));

        // set order of overview images (basically: lightpath 0 as last image ... so we do not have to reset the lightpath
        QList<int> prevs;
        for (int ovrImg=1; ovrImg<previewCount(); ovrImg++) {
            prevs<<ovrImg;
        }
        prevs<<0;

        QString currentCamConfig1="";
        QString currentCamConfig2="";

        //////////////////////////////////////////////////////////////////////////////////////
        // acquire background images
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && saveBackground()) {
            progress.nextItem();
            progress.setLabelText(tr("acquiring background frames from cameras ..."));
            QApplication::processEvents();
            if (progress.wasCanceled())  {
                userCanceled=true;
                ok=false;
            }

            //////////////////////////////////////////////////////////////////////////////////////
            // switch off light
            //////////////////////////////////////////////////////////////////////////////////////
            if (opticsSetup->isMainIlluminationShutterAvailable()) {
                log->log_text(tr("  - switch main shutter off!\n"));
                ok=ok&opticsSetup->setMainIlluminationShutter(false, true);
                if (!ok) {
                    ACQUISITION_ERROR(tr("  - could not switch main shutter off!\n"));
                }
            }
            //////////////////////////////////////////////////////////////////////////////////////
            // acquire background series
            //////////////////////////////////////////////////////////////////////////////////////
            QMap<QFExtensionCamera::CameraSetting, QVariant> camset1, camset2;
            camset1=getCameraSettings1();
            camset2=getCameraSettings2();
            QMap<QString, QVariant> camsetstr1, camsetstr2;
            camsetstr1=getCameraSettingsStr1();
            camsetstr2=getCameraSettingsStr2();
            camset1[QFExtensionCamera::CamSetNumberFrames]=currentBackgroundFrames(0);
            camset2[QFExtensionCamera::CamSetNumberFrames]=currentBackgroundFrames(1);
            ok = acqTools->acquireSeries(lightpathName, "", tr("background image series"), useCam1&&(ui->chkBackground->isChecked())&&(!ui->chkNoBackground1->isChecked()), extension1, ecamera1, camera1, acquisitionPrefix1+"_background", acquisitionSettingsFilename1, backgroundDescription1, backgroundFiles1, useCam2&&(ui->chkBackground->isChecked())&&(!ui->chkNoBackground2->isChecked()), extension2, ecamera2, camera2, acquisitionPrefix2+"_background", acquisitionSettingsFilename2, backgroundDescription2, backgroundFiles2, camset1, camset2, camsetstr1, camsetstr2, NULL, &progress, &userCanceled, measureDuringAcquisitions);
            if (!ok) {
                ACQUISITION_ERROR(tr("  - error acquiring background image series!\n"));
            } else {
                log->log_text(tr("  - acquired background image series!\n"));
            }


            currentCamConfig1="";
            currentCamConfig2="";

            for (int ovrImgI=0; ovrImgI<prevs.size(); ovrImgI++) {
                int ovrImg=prevs[ovrImgI];
                if (ok && lightpathActivatedPreview(ovrImg)) {
                    if (ok && useCam1 && (!ui->chkNoBackground1->isChecked())&&(ui->chkBackground->isChecked())&&(!ui->chkNoPreview1->isChecked())) {
                        progress.setLabelText(tr("acquiring overview background image from camera 1, lightpath %1 ...").arg(ovrImg+1));
                        QApplication::processEvents();
                        QString acquisitionPrefix=acquisitionPrefix1+QString("_overview_backgroundlp%1.tif").arg(ovrImg+1);
                        QString prefix=QString("overview_background_lightpath%1").arg(ovrImg+1);
                        if (ovrImg==0) {
                            acquisitionPrefix=acquisitionPrefix1+QString("_overview_background.tif");
                            prefix=QString("overview_background");
                        }
                        bool newCamSetting=ui->chkReconfigureAlways1->isChecked()||(currentCamConfig1!=currentPreviewConfigFilename(0,ovrImg));
                        currentCamConfig1=currentPreviewConfigFilename(0,ovrImg);
                        ok=acqTools->acquireImageWithLightpath("", "none", extension1, ecamera1, camera1, newCamSetting?currentCamConfig1:QString(), acquisitionPrefix, prefix, tr("overview background with lightpath %1 for camera 1").arg(ovrImg+1), moreFiles1, acquisitionDescription1, false, 1, ui->chkMeasurementsPreview->isChecked());
                        if (!ok) {
                            ACQUISITION_ERROR(tr("  - error acquiring overview background image from camera 1, lightpath %1!\n").arg(ovrImg+1));
                        } else {
                            log->log_text(tr("  - acquired overview background image from camera 1, lightpath %1 (reconfigured camera: %2)!\n").arg(ovrImg+1).arg(newCamSetting));
                        }

                    }
                    if (ok && useCam2 && (!ui->chkNoBackground2->isChecked()) &&(ui->chkBackground->isChecked())&& (!ui->chkNoPreview2->isChecked())) {
                        progress.setLabelText(tr("acquiring overview background image from camera 2, lightpath %1 ...").arg(ovrImg+1));
                        QApplication::processEvents();
                        QString acquisitionPrefix=acquisitionPrefix1+QString("_overview_backgroundlp%1.tif").arg(ovrImg+1);
                        QString prefix=QString("overview_background_lightpath%1").arg(ovrImg+1);
                        if (ovrImg==0) {
                            acquisitionPrefix=acquisitionPrefix1+QString("_overview_background.tif");
                            prefix=QString("overview_background");
                        }
                        bool newCamSetting=ui->chkReconfigureAlways2->isChecked()||(currentCamConfig2!=currentPreviewConfigFilename(1,ovrImg));
                        currentCamConfig2=currentPreviewConfigFilename(1,ovrImg);
                        ok=acqTools->acquireImageWithLightpath("", "none", extension2, ecamera2, camera2, newCamSetting?currentCamConfig2:QString(), acquisitionPrefix, prefix, tr("overview background with lightpath %1 for camera 2").arg(ovrImg+1), moreFiles2, acquisitionDescription2, false, 1, ui->chkMeasurementsPreview->isChecked());
                        if (!ok) {
                            ACQUISITION_ERROR(tr("  - error acquiring overview background image from camera 2, lightpath %1!\n").arg(ovrImg+1));
                        } else {
                            log->log_text(tr("  - acquired overview background image from camera 2, lightpath %1 (reconfigured camera: %2)!\n").arg(ovrImg+1).arg(newCamSetting));
                        }
                    }
                }
            }

        }

        //////////////////////////////////////////////////////////////////////////////////////
        // switch on light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()) {
            log->log_text(tr("  - switch main shutter back on!\n"));
            ok=ok&opticsSetup->setMainIlluminationShutter(true, true);
        }



        //////////////////////////////////////////////////////////////////////////////////////
        // acquire overview images
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();
        QApplication::processEvents();
        if (progress.wasCanceled())  {
            userCanceled=true;
            ok=false;
        }
        currentCamConfig1="";
        currentCamConfig2="";
        for (int ovrImgI=0; ovrImgI<prevs.size(); ovrImgI++) {
            int ovrImg=prevs[ovrImgI];
            if (ok && lightpathActivatedPreview(ovrImg)) {
                if (ok && useCam1 && (!ui->chkNoPreview1->isChecked())) {
                    progress.setLabelText(tr("acquiring overview image from camera 1, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix1+QString("_overviewlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix1+QString("_overview.tif");
                        prefix=QString("overview");
                    }
                    bool newCamSetting=ui->chkReconfigureAlways1->isChecked()||(currentCamConfig1!=currentPreviewConfigFilename(0,ovrImg));
                    currentCamConfig1=currentPreviewConfigFilename(0,ovrImg);
                    ok=acqTools->acquireImageWithLightpath(lightpathFilenamePreview(ovrImg), lightpathPreview(ovrImg), extension1, ecamera1, camera1, newCamSetting?currentCamConfig1:QString(), acquisitionPrefix, prefix, tr("overview before acquisition with lightpath %1 for camera 1").arg(ovrImg+1), moreFiles1, acquisitionDescription1, ui->chkCloseMainShutter->isChecked(), 1, ui->chkMeasurementsPreview->isChecked());
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 1, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log->log_text(tr("  - acquired overview image from camera 1, lightpath %1 (reconfigured camera: %2)!\n").arg(ovrImg+1).arg(newCamSetting));
                    }

                }
                if (ok && useCam2 && (!ui->chkNoPreview2->isChecked())) {
                    progress.setLabelText(tr("acquiring overview image from camera 2, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix1+QString("_overviewlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix1+QString("_overview.tif");
                        prefix=QString("overview");
                    }
                    bool newCamSetting=ui->chkReconfigureAlways2->isChecked()||(currentCamConfig2!=currentPreviewConfigFilename(1,ovrImg));
                    currentCamConfig2=currentPreviewConfigFilename(1,ovrImg);
                    ok=acqTools->acquireImageWithLightpath(lightpathFilenamePreview(ovrImg), lightpathPreview(ovrImg), extension2, ecamera2, camera2, newCamSetting?currentCamConfig2:QString(), acquisitionPrefix, prefix, tr("overview before acquisition with lightpath %1 for camera 2").arg(ovrImg+1), moreFiles2, acquisitionDescription2, ui->chkCloseMainShutter->isChecked(), 1, ui->chkMeasurementsPreview->isChecked());
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 2, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log->log_text(tr("  - acquired overview image from camera 2, lightpath %1 (reconfigured camera: %2)!\n").arg(ovrImg+1).arg(newCamSetting));
                    }
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////
        // switch off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()) {
            log->log_text(tr("  - switch main shutter off!\n"));
            ok=ok&opticsSetup->setMainIlluminationShutter(false, false);
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // SET ACQUISITION LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        if (lightpathActivated()) {
            if (!QFile::exists(lightpathFilename())) {
                ACQUISITION_ERROR(tr("  - acquisition lighpath configuration '%1' does not exist!\n").arg(lightpath()));
                opticsSetup->unlockLightpath();
                return;
            } else {
                log->log_text(tr("  - setting acquisition lightpath settings '%1' ...\n").arg(lightpath()));
                opticsSetup->loadLightpathConfig(lightpathFilename(), true);
                log->log_text(tr("  - setting acquisition lightpath settings '%1' ... DONE\n").arg(lightpath()));
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////
        // switch on light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()) {
            log->log_text(tr("  - switch main shutter back on!\n"));
            ok=ok&opticsSetup->setMainIlluminationShutter(true, true);
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // acquire image series
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();
        QApplication::processEvents();
        if (progress.wasCanceled())  {
            userCanceled=true;
            ok=false;
        }
        QMap<QFExtensionCamera::CameraSetting, QVariant> camset1, camset2;
        camset1=getCameraSettings1();
        camset2=getCameraSettings2();
        QMap<QString, QVariant> camsetstr1, camsetstr2;
        camsetstr1=getCameraSettingsStr1();
        camsetstr2=getCameraSettingsStr2();
        ok = acqTools->acquireSeries(lightpathName, "acquisition", tr("image series"), useCam1, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionSettingsFilename1, acquisitionDescription1, moreFiles1, useCam2, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionSettingsFilename2, acquisitionDescription2, moreFiles2, camset1, camset2, camsetstr1, camsetstr2, &measured, &progress, &userCanceled, measureDuringAcquisitions);
        if (!ok) {
            ACQUISITION_ERROR(tr("  - error acquiring images!\n"));
        } else {
            log->log_text(tr("  - acquired image series!\n"));
        }


        //////////////////////////////////////////////////////////////////////////////////////
        // acquire overview images
        //////////////////////////////////////////////////////////////////////////////////////
        progress.nextItem();
        QApplication::processEvents();
        if (progress.wasCanceled())  {
            userCanceled=true;
            ok=false;
        }
        prevs.clear();
        for (int ovrImg=0; ovrImg<previewCount(); ovrImg++) {
            prevs<<ovrImg;
        }
        currentCamConfig1="";
        currentCamConfig2="";
        for (int ovrImgI=0; ovrImgI<prevs.size(); ovrImgI++) {
            int ovrImg=prevs[ovrImgI];
            if (ok && lightpathActivatedPreview(ovrImg)) {
                if (ok && useCam1 && (!ui->chkNoPreview1->isChecked())) {
                    progress.setLabelText(tr("acquiring overview image from camera 1, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix1+QString("_overview_afterlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_after_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix1+QString("_overview_after.tif");
                        prefix=QString("overview_after");
                    }
                    bool newCamSetting=ui->chkReconfigureAlways1->isChecked()||(currentCamConfig1!=currentPreviewConfigFilename(0,ovrImg));
                    currentCamConfig1=currentPreviewConfigFilename(0,ovrImg);
                    ok=acqTools->acquireImageWithLightpath(lightpathFilenamePreview(ovrImg), lightpathPreview(ovrImg), extension1, ecamera1, camera1, newCamSetting?currentCamConfig1:QString(), acquisitionPrefix, prefix, tr("overview after acquisition with lightpath %1 for camera 1").arg(ovrImg+1), moreFiles1, acquisitionDescription1, ui->chkCloseMainShutter->isChecked(), 1, ui->chkMeasurementsPreview->isChecked());
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 1, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log->log_text(tr("  - acquired overview image from camera 1, lightpath %1 (reconfigured camera: %2)!\n").arg(ovrImg+1).arg(newCamSetting));
                    }

                }
                if (ok && useCam2 && (!ui->chkNoPreview2->isChecked())) {
                    progress.setLabelText(tr("acquiring overview image from camera 2, lightpath %1 ...").arg(ovrImg+1));
                    QApplication::processEvents();
                    QString acquisitionPrefix=acquisitionPrefix2+QString("_overview_afterlp%1.tif").arg(ovrImg+1);
                    QString prefix=QString("overview_after_lightpath%1").arg(ovrImg+1);
                    if (ovrImg==0) {
                        acquisitionPrefix=acquisitionPrefix2+QString("_overview_after.tif");
                        prefix=QString("overview_after");
                    }
                    bool newCamSetting=ui->chkReconfigureAlways2->isChecked()||(currentCamConfig2!=currentPreviewConfigFilename(1,ovrImg));
                    currentCamConfig2=currentPreviewConfigFilename(1,ovrImg);
                    ok=acqTools->acquireImageWithLightpath(lightpathFilenamePreview(ovrImg), lightpathPreview(ovrImg), extension2, ecamera2, camera2, newCamSetting?currentCamConfig2:QString(), acquisitionPrefix, prefix, tr("overview after acquisition with lightpath %1 for camera 2").arg(ovrImg+1), moreFiles2, acquisitionDescription2, ui->chkCloseMainShutter->isChecked(), 1, ui->chkMeasurementsPreview->isChecked());
                    if (!ok) {
                        ACQUISITION_ERROR(tr("  - error acquiring overview image from camera 2, lightpath %1!\n").arg(ovrImg+1));
                    } else {
                        log->log_text(tr("  - acquired overview image from camera 2, lightpath %1 (reconfigured camera: %2)!\n").arg(ovrImg+1).arg(newCamSetting));
                    }
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // switch off light
        //////////////////////////////////////////////////////////////////////////////////////
        if (opticsSetup->isMainIlluminationShutterAvailable()) {
            log->log_text(tr("  - switch main shutter off!\n"));
            ok=ok&opticsSetup->setMainIlluminationShutter(false, false);
        }

        progress.nextItem();
        QApplication::processEvents();
        if (progress.wasCanceled())  {
            userCanceled=true;
            ok=false;
        }
        //////////////////////////////////////////////////////////////////////////////////////
        // RESET LIGHTPATH
        //////////////////////////////////////////////////////////////////////////////////////
        if (lightpathActivated()) {
            opticsSetup->loadLightpathConfig(oldLightpath, false);
            log->log_text(tr("  - resetting acquisition lightpath ...\n"));
        }
        opticsSetup->unlockLighpathCombobox();

        //////////////////////////////////////////////////////////////////////////////////////
        // write acquisition data
        //////////////////////////////////////////////////////////////////////////////////////
        if (ok && useCam1) {
            log->log_text(tr("  - writing acquisition description 1 ..."));
            // add the background stuff to the description, files list
            for (int i=0; i<backgroundFiles1.size(); i++) {
                QFExtensionCamera::CameraAcquititonFileDescription d=backgroundFiles1[i];
                d.description="background "+d.description;
                moreFiles1.append(d);
            }
            QMapIterator<QString, QVariant> it(backgroundDescription1);
            while (it.hasNext()) {
                it.next();
                if ( (!acquisitionDescription1.contains(it.key())) || (acquisitionDescription1.value(it.key(), it.value())!=it.value()) ) {
                    acquisitionDescription1["background/"+it.key()]=it.value();
                }
            }

            if (measured.size()>0) {
                QString MeasurementsFilename=acqTools->saveMeasuredData(acquisitionPrefix1, measured);
                if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                    QFExtensionCamera::CameraAcquititonFileDescription d;
                    d.name=MeasurementsFilename;
                    d.description="measureable properties of setup";
                    d.type="CSV";
                    moreFiles1.append(d);
                }
            }

            acqTools->saveAcquisitionDescription(0, extension1, ecamera1, camera1, acquisitionPrefix1, acquisitionDescription1, moreFiles1, startDateTime, false);
            log->log_text(tr(" DONE!\n"));
        }
        if (ok && useCam2) {
            log->log_text(tr("  - writing acquisition description 2 ..."));
            // add the background stuff to the description, files list
            for (int i=0; i<backgroundFiles2.size(); i++) {
                QFExtensionCamera::CameraAcquititonFileDescription d=backgroundFiles2[i];
                d.description="background "+d.description;
                moreFiles2.append(d);
            }
            QMapIterator<QString, QVariant> it(backgroundDescription2);
            while (it.hasNext()) {
                it.next();
                if ( (!acquisitionDescription2.contains(it.key())) || (acquisitionDescription2.value(it.key(), it.value())!=it.value()) ) {
                    acquisitionDescription2["background/"+it.key()]=it.value();
                }
            }
            if (measureDuringAcquisitions) {
                QString MeasurementsFilename=acqTools->saveMeasuredData(acquisitionPrefix2, measured);
                if (!MeasurementsFilename.isEmpty() && QFile::exists(MeasurementsFilename)) {
                    QFExtensionCamera::CameraAcquititonFileDescription d;
                    d.name=MeasurementsFilename;
                    d.description="measureable properties of setup";
                    d.type="CSV";
                    moreFiles2.append(d);
                }
            }

            acqTools->saveAcquisitionDescription(1, extension2, ecamera2, camera2, acquisitionPrefix2, acquisitionDescription2, moreFiles2, startDateTime, false);
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
        repeatCnt++;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // switch on light
    //////////////////////////////////////////////////////////////////////////////////////
    if (opticsSetup->isMainIlluminationShutterAvailable()) {
        log->log_text(tr("  - switch main shutter back on!\n"));
        ok=ok&opticsSetup->setMainIlluminationShutter(true, true);
    }

    opticsSetup->unlockLightpath();
    log->log_text(tr("image series acquisition DONE!\n"));
    opticsSetup->ensureLightpath();
}

