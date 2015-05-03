/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-06-22 12:12:54 +0200 (So, 22 Jun 2014) $  (revision $Rev: 3280 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qffitalgorithmerrorestimatemodewidget.h"
#include "ui_qffitalgorithmerrorestimatemodewidget.h"

QFFitAlgorithmErrorEstimateModeWidget::QFFitAlgorithmErrorEstimateModeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFFitAlgorithmErrorEstimateModeWidget)
{
    ui->setupUi(this);
    on_cmbMode_currentIndexChanged(ui->cmbMode->currentIndex());
}

QFFitAlgorithmErrorEstimateModeWidget::~QFFitAlgorithmErrorEstimateModeWidget()
{
    delete ui;
}

QFFitAlgorithm::FitParameterErrorEstimates QFFitAlgorithmErrorEstimateModeWidget::getMode() const
{
    return QFFitAlgorithm::FitParameterErrorEstimates(ui->cmbMode->currentIndex());
}

void QFFitAlgorithmErrorEstimateModeWidget::setMode(QFFitAlgorithm::FitParameterErrorEstimates mode)
{
    ui->cmbMode->setCurrentIndex(int(mode));
}

void QFFitAlgorithmErrorEstimateModeWidget::setBootstrappingRepeats(int repeats)
{
    ui->spinBSRepeats->setValue(repeats);
}

int QFFitAlgorithmErrorEstimateModeWidget::getBootstrappingRepeats() const
{
    return ui->spinBSRepeats->value();
}

void QFFitAlgorithmErrorEstimateModeWidget::setBootstrappingDistortion(double value)
{
    ui->spinBSDistortion->setValue(value*100.0);
}

double QFFitAlgorithmErrorEstimateModeWidget::getBootstrappingDistortion() const
{
    return ui->spinBSDistortion->value()/100.0;
}

void QFFitAlgorithmErrorEstimateModeWidget::setBootstrappingFraction(double value)
{
    ui->spinBSFraction->setValue(value*100.0);
}

double QFFitAlgorithmErrorEstimateModeWidget::getBootstrappingFraction() const
{
    return ui->spinBSFraction->value()/100.0;
}

QString QFFitAlgorithmErrorEstimateModeWidget::toString() const
{
    QString str=tr("mode: %1").arg(ui->cmbMode->currentText());
    if (ui->cmbMode->currentIndex()==1) {
        str.append(tr(", repeats: %1").arg(ui->spinBSRepeats->value()));
        str.append(tr(", fraction: %1 %").arg(ui->spinBSFraction->value()));
        str.append(tr(", distortion: %1 %").arg(ui->spinBSDistortion->value()));
    }
    return str;
}

void QFFitAlgorithmErrorEstimateModeWidget::saveSettings(QFProperties *props, const QString &prefix) const
{
    props->setQFProperty(prefix+"FIT_ERRORESTIMATE_MODE", ui->cmbMode->currentIndex(), false, false);
    props->setQFProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_REPEATS", ui->spinBSRepeats->value(), false, false);
    props->setQFProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_FRACTION", ui->spinBSFraction->value()/100.0, false, false);
    props->setQFProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_DISTORTION", ui->spinBSDistortion->value()/100.0, false, false);
}


void QFFitAlgorithmErrorEstimateModeWidget::readSettings(QFProperties *props, const QString &prefix)
{
    ui->cmbMode->setCurrentIndex(props->getProperty(prefix+"FIT_ERRORESTIMATE_MODE", 0).toInt());
    ui->spinBSRepeats->setValue(props->getProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_REPEATS", 50).toInt());
    ui->spinBSFraction->setValue(props->getProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_FRACTION", 0.6).toDouble()*100.0);
    ui->spinBSDistortion->setValue(props->getProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_DISTORTION", 0.02).toDouble()*100.0);
}

void QFFitAlgorithmErrorEstimateModeWidget::on_cmbMode_currentIndexChanged(int index)
{
    ui->labBSDist->setVisible(index==1);
    ui->labBSFrac->setVisible(index==1);
    ui->labBSRepeat->setVisible(index==1);
    ui->spinBSDistortion->setVisible(index==1);
    ui->spinBSFraction->setVisible(index==1);
    ui->spinBSRepeats->setVisible(index==1);
    emit parametersChanged();
    emit modeChanged(QFFitAlgorithm::FitParameterErrorEstimates(index));
}

void QFFitAlgorithmErrorEstimateModeWidget::on_spinBSRepeats_valueChanged(int /*value*/)
{
    emit parametersChanged();
}

void QFFitAlgorithmErrorEstimateModeWidget::on_spinBSDistortion_valueChanged(double /*value*/)
{
    emit parametersChanged();
}

void QFFitAlgorithmErrorEstimateModeWidget::on_spinBSFraction_valueChanged(double /*value*/)
{
    emit parametersChanged();
}
