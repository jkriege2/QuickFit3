/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "DialogReconstructMarkerRecords.h"
#include "ui_DialogReconstructMarkerRecords.h"
#include <QMessageBox>

DialogReconstructMarkerRecords::DialogReconstructMarkerRecords(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogReconstructMarkerRecords)
{
    ui->setupUi(this);
    setWindowTitle("Reconstruct Marker Records");
    setInfoText(0,0);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)),this,SLOT(setLabelTextChannel(QString)));
    ui->radioButtonReconstruct->toggle();
}

DialogReconstructMarkerRecords::~DialogReconstructMarkerRecords()
{
    delete ui;
}

double DialogReconstructMarkerRecords::getPeriod() { return ui->doubleSpinBoxPeriod->value()*1e-6;}

void DialogReconstructMarkerRecords::setPeriod(double period) {
    ui->doubleSpinBoxPeriod->setValue(period*1e6);
    ui->doubleSpinBoxDonorPeriod->setValue(period*1e6);
}

double DialogReconstructMarkerRecords::getFullDonorPeriod() { return ui->doubleSpinBoxDonorPeriod->value()*1e-6;}

double DialogReconstructMarkerRecords::getFullStartTime() { return ui->doubleSpinBoxStartTime->value()*1e-6;}

void DialogReconstructMarkerRecords::setFullStartTime(double startTime) { ui->doubleSpinBoxStartTime->setValue(startTime*1e6);}

double DialogReconstructMarkerRecords::getFullOffset() { return ui->doubleSpinBoxFullOffset->value()*1e-6;}

double DialogReconstructMarkerRecords::getOffset() { return ui->doubleSpinBoxOffset->value()*1e-6;}

void DialogReconstructMarkerRecords::setOffset(double offset)
{
    ui->doubleSpinBoxOffset->setValue(offset*1e6);
    ui->doubleSpinBoxFullOffset->setValue(offset*1e6);
}

ReconstructionMethod DialogReconstructMarkerRecords::getChannel()
{
    if (ui->radioButtonFullConstruct->isChecked() ) {
        return FullConstruction;
    } else if(ui->radioButtonFRET->isChecked()){
        return SingleWavelengthFRET;
    } else {
        return ((ReconstructionMethod) ui->comboBox->currentIndex());
    }
}

void DialogReconstructMarkerRecords::setChannel(ReconstructionMethod channel)
{
    if(channel == FullConstruction ) {
        ui->comboBox->setCurrentIndex(0);
        ui->radioButtonFullConstruct->setChecked(true);
    } else if(channel==SingleWavelengthFRET) {
        ui->radioButtonFRET->setChecked(true);
    } else {
        if(channel == ReconstructFromDonor ) ui->comboBox->setCurrentIndex(0);
        if(channel == ReconstructFromAcceptor ) ui->comboBox->setCurrentIndex(1);
        ui->radioButtonReconstruct->setChecked(true);
    }
}

void DialogReconstructMarkerRecords::setInfoText(int nEventsD, int nEventsA, QString additionalInfo)
{
    nMarkerEventsD=nEventsD;
    nMarkerEventsA=nEventsA;
    QString str;
    if(nMarkerEventsD==0&&nMarkerEventsA==0)
        str=tr("donor and acceptor");
    else {
        if(nMarkerEventsD>nMarkerEventsA*1.2) str=tr("acceptor");
        if(nMarkerEventsA>nMarkerEventsD*1.2) str=tr("donor");
        else str=tr("only few");
    }
    ui->labelInfo->setText(tr("Found %1 donor and %2 acceptor markers.\n"
                              "It seems that %3 markers are missing.\n"
                              "Please choose if you want to reconstruct the marker"
                              "times from the other channel"
                              "\nor fully reconstruct marker times from a given period\n"
                              "Note that the offset times should include excitation gaps.\n%4"
                              ).arg(QString::number(nMarkerEventsD)
                              ).arg(QString::number(nMarkerEventsA)
                              ).arg(str).arg(additionalInfo));
}



void DialogReconstructMarkerRecords::on_radioButtonFullConstruct_toggled(bool checked)
{
    ui->comboBox->setDisabled(checked);
    ui->doubleSpinBoxOffset->setDisabled(checked);
    ui->doubleSpinBoxPeriod->setDisabled(checked);
}

void DialogReconstructMarkerRecords::on_radioButtonFRET_toggled(bool checked)
{
    ui->comboBox->setDisabled(checked);
    ui->doubleSpinBoxOffset->setDisabled(checked);
    ui->doubleSpinBoxPeriod->setDisabled(checked);
    ui->doubleSpinBoxFullOffset->setDisabled(checked);
    ui->doubleSpinBoxDonorPeriod->setDisabled(checked);
    ui->doubleSpinBoxStartTime->setDisabled(checked);
}

void DialogReconstructMarkerRecords::on_radioButtonReconstruct_toggled(bool checked)
{
    ui->doubleSpinBoxFullOffset->setDisabled(checked);
    ui->doubleSpinBoxDonorPeriod->setDisabled(checked);
    ui->doubleSpinBoxStartTime->setDisabled(checked);
}

void DialogReconstructMarkerRecords::setLabelTextChannel(QString channel)
{
    if(checkPossibleReconstruction()) {
        ui->labelExcPeriod->setText("Period of\n" + channel + " excitation");
    }
}

bool DialogReconstructMarkerRecords::checkPossibleReconstruction()
{
    if( ((ui->comboBox->currentIndex()==0) && (nMarkerEventsD==0) ) || // donor
        ((ui->comboBox->currentIndex()==1) && (nMarkerEventsA==0) )   )// acceptor
    {
        QMessageBox::warning(this,tr("Warning"),tr("You selected reconstruction from the %1 channel.\nThere are no events in the %1 marker channel.\nReconstruction is not possible.").arg(ui->comboBox->currentText()));
        if( (ui->comboBox->currentIndex()==0))
            ui->comboBox->setCurrentIndex(1);
        else if((ui->comboBox->currentIndex()==1))
            ui->comboBox->setCurrentIndex(0);
        return false;
    }
    return true;
}


