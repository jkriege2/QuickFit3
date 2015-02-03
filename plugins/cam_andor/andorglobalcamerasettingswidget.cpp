/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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
    ui->labCamera->setText(tr("<u><b>camera #%1:</b></u>").arg(camera));
    setInfo("");
    m_emit=true;
}

AndorGlobalCameraSettingsWidget::~AndorGlobalCameraSettingsWidget()
{
    delete ui;
}

void AndorGlobalCameraSettingsWidget::setSettings(int fan_mode, bool cooling_on, bool cooling_wait, int temperature, int shutterMode) {
    m_emit=false;
    if (ui->cmbFanMode->currentIndex()!=fan_mode) ui->cmbFanMode->setCurrentIndex(fan_mode);
    if (ui->chkCooling->isChecked()!=cooling_on) ui->chkCooling->setChecked(cooling_on);
    if (ui->chkCoolingWait->isChecked()!=cooling_wait) ui->chkCoolingWait->setChecked(cooling_wait);
    if (ui->spinTemperature->value()!=temperature) ui->spinTemperature->setValue(temperature);
    if (ui->cmbShutter->currentIndex()!=shutterMode) ui->cmbShutter->setCurrentIndex(shutterMode);
    m_emit=true;
    int w=qMax(width(), sizeHint().width());
    int h=qMax(height(), sizeHint().height());
    resize(w,h);
}

void AndorGlobalCameraSettingsWidget::forceSettingsChanged() {
    emit settingsChanged(m_camera, ui->cmbFanMode->currentIndex(), ui->chkCooling->isChecked(), ui->chkCoolingWait->isChecked(), ui->spinTemperature->value(), ui->cmbShutter->currentIndex());
}

void AndorGlobalCameraSettingsWidget::showCurrentTemperature(int progress, float temperature) {
    QString drift="";
    if (progress==3) drift=tr("<small>(drift!)</small>");

    if (progress==-1) ui->labTemperature->setText(tr("<b>---</b> <small>(error!)</small>"));
    else ui->labTemperature->setText(tr("<b>%1 &deg;C</b> %2").arg(temperature).arg(drift));

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
    if (!ui->chkCooling->isChecked()) ui->labTemperaturePic->setPixmap(QPixmap(QString(":/cam_andor/thermometer.png")));
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
