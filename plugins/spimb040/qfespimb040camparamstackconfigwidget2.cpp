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

#define STAGE_INTERVAL_MS 1313

QFESPIMB040CamParamStackConfigWidget2::QFESPIMB040CamParamStackConfigWidget2(QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* stageConfig, QString configDirectory) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040CamParamStackConfigWidget2)
{
    m_pluginServices=pluginServices;
    this->opticsSetup=stageConfig;
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

    QTimer::singleShot(STAGE_INTERVAL_MS, this, SLOT(checkStage()));
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

    ui->spinStart->setValue(settings.value(prefix+"start", 0).toDouble());
    ui->spinDelta->setValue(settings.value(prefix+"delta", 10).toDouble());
    ui->spinEnd->setValue(settings.value(prefix+"end", 100).toDouble());
    ui->cmbParameter->setCurrentIndex(settings.value(prefix+"parameter", 0).toInt());
    ui->cmbMode->setCurrentIndex(settings.value(prefix+"mode", 0).toInt());

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
}


int QFESPIMB040CamParamStackConfigWidget2::counter() const {
    return ui->spinCounter->value();
}

void QFESPIMB040CamParamStackConfigWidget2::incCounter() {
    ui->spinCounter->setValue(ui->spinCounter->value()+1);
}

QString QFESPIMB040CamParamStackConfigWidget2::prefix1() const {
    QString filename= ui->edtPrefix1->text();
    filename=filename.replace("%counter%", QString("%1").arg((qlonglong)counter(), (int)3, (int)10, QChar('0')));
    return filename;
}

QString QFESPIMB040CamParamStackConfigWidget2::prefix2() const {
    QString filename= ui->edtPrefix2->text();
    filename=filename.replace("%counter%", QString("%1").arg((qlonglong)counter(), (int)3, (int)10, QChar('0')));
    return filename;
}

bool QFESPIMB040CamParamStackConfigWidget2::use1() const {
    return ui->chkUse1->isChecked();
}

bool QFESPIMB040CamParamStackConfigWidget2::use2() const {
    return ui->chkUse2->isChecked();
}

int QFESPIMB040CamParamStackConfigWidget2::stackParameter() const {
    return ui->cmbParameter->currentIndex();
}

int QFESPIMB040CamParamStackConfigWidget2::stackMode() const {
    return ui->cmbMode->currentIndex();
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

