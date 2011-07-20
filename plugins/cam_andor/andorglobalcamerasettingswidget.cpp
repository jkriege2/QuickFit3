#include "andorglobalcamerasettingswidget.h"
#include "ui_andorglobalcamerasettingswidget.h"
#include <cmath>

AndorGlobalCameraSettingsWidget::AndorGlobalCameraSettingsWidget(int camera, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AndorGlobalCameraSettingsWidget)
{
    m_emit=false;
    m_camera=camera;
    ui->setupUi(this);
    ui->labCamera->setText(tr("<u><b>camer #1:</b></u>").arg(camera));
    setInfo("");
    m_emit=true;
}

AndorGlobalCameraSettingsWidget::~AndorGlobalCameraSettingsWidget()
{
    delete ui;
}

void AndorGlobalCameraSettingsWidget::setSettings(int fan_mode, bool cooling_on, int temperature, int shutterMode) {
    m_emit=false;
    ui->cmbFanMode->setCurrentIndex(fan_mode);
    ui->chkCooling->setChecked(cooling_on);
    ui->spinTemperature->setValue(temperature);
    ui->cmbShutter->setCurrentIndex(shutterMode);
    m_emit=true;
}

void AndorGlobalCameraSettingsWidget::forceSettingsChanged() {
    emit settingsChanged(m_camera, ui->cmbFanMode->currentIndex(), ui->chkCooling->isChecked(), ui->spinTemperature->value(), ui->cmbShutter->currentIndex());
}

void AndorGlobalCameraSettingsWidget::showCurrentTemperature(int progress, float temperature) {
    QString drift="";
    if (progress==3) drift=tr("<small>(drift!)</small>");

    if (progress==-1) ui->labTemperature->setText(tr("<b>---</b> <small>(error!)</small>"));
    else ui->labTemperature->setText(tr("<b>%1 °C</b> %2").arg(temperature).arg(drift));

    int direction=0;
    if (ui->spinTemperature->value()>(int)round(temperature)) direction=1;
    if (ui->spinTemperature->value()<(int)round(temperature)) direction=-1;

    if (progress==-2) ui->labTemperaturePic->setPixmap(QPixmap(QString(":/cam_andor/thermometer_off.png")));
    else if (progress==-1) ui->labTemperaturePic->setPixmap(QPixmap(QString(":/cam_andor/thermometer_error.png")));
    else if (progress==0) ui->labTemperaturePic->setPixmap(QPixmap(QString(":/cam_andor/thermometer_stabilized.png")));
    else {
        if (direction>0) ui->labTemperaturePic->setPixmap(QPixmap(QString(":/cam_andor/thermometer_rising.png")));
        else if (direction<0) ui->labTemperaturePic->setPixmap(QPixmap(QString(":/cam_andor/thermometer_falling.png")));
        else ui->labTemperaturePic->setPixmap(QPixmap(QString(":/cam_andor/thermometer.png")));
    }
}

void AndorGlobalCameraSettingsWidget::valuesChanged() {
    if (m_emit) forceSettingsChanged();
}

void AndorGlobalCameraSettingsWidget::setRange(int min, int max) {
    ui->spinTemperature->setRange(min, max);
}

void AndorGlobalCameraSettingsWidget::setInfo(const QString& info) {
    if (!info.isEmpty()) {
        ui->labInfo->setVisible(true);
        ui->labInfoLabel->setVisible(true);
        ui->labInfo->setText(info);
    } else {
        ui->labInfo->setVisible(false);
        ui->labInfoLabel->setVisible(false);
    }
}
