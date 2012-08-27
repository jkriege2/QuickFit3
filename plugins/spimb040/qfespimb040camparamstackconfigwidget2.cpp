#include "qfespimb040camparamstackconfigwidget2.h"
#include "ui_qfespimb040camparamstackconfigwidget2.h"

#include "qfespimb040mainwindow.h"
#include "qfpluginservices.h"
#include "qfstagecombobox.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include <QtGui>
#include <QtCore>
#include "qfespimb040opticssetup.h"

QFESPIMB040CamParamStackConfigWidget2::QFESPIMB040CamParamStackConfigWidget2(QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* stageConfig, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040CamParamStackConfigWidget2)
{
    m_pluginServices=pluginServices;
    this->opticsSetup=stageConfig;
    this->acqDescription=acqDescription;
    this->expDescription=expDescription;
    ui->setupUi(this);

    if (stageConfig) {
        ui->cmbCam1Settings->init(configDirectory);
        ui->cmbCam1Settings->setStopResume(opticsSetup->getStopRelease(0));
        ui->cmbCam1Settings->connectTo(opticsSetup->cameraComboBox(0));
        //qDebug()<<configDirectory<<opticsSetup->cameraComboBox(0)<<opticsSetup->cameraComboBox(1);

        ui->cmbCam2Settings->init(configDirectory);
        ui->cmbCam2Settings->setStopResume(opticsSetup->getStopRelease(1));
        ui->cmbCam2Settings->connectTo(opticsSetup->cameraComboBox(1));
    }
    bindLineEdit(ui->edtPrefix1);
    bindLineEdit(ui->edtPrefix2);
    updateReplaces();

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
    settings.setValue(prefix+"paramer", ui->cmbParameter->currentIndex());
    settings.setValue(prefix+"mode", ui->cmbMode->currentIndex());
    settings.setValue(prefix+"images", ui->spinImages->value());

    settings.setValue(prefix+"lightpathidx", ui->cmbLightpath->currentIndex());
    settings.setValue(prefix+"lightpath", ui->chkLightpath->isChecked());
    settings.setValue(prefix+"savemeasurement", ui->chkSaveMeasurements->isChecked());
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
