/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "dlgpololumaestrotester.h"
#include "ui_dlgpololumaestrotester.h"
#include "pololu_tools.h"

DlgPololuMaestroTester::DlgPololuMaestroTester(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPololuMaestroTester)
{
    serial=new QFBinarySerialProtocolHandler(&com, "popolu maestro");
    ui->setupUi(this);
    ui->cmbPort->clear();
    std::vector<std::string> ports=QFSerialConnection::listPorts();
    for (size_t i=0; i<ports.size(); i++) {
        ui->cmbPort->addItem(ports[i].c_str());
    }


    ui->cmbPort->setEditText(ProgramOptions::getConfigValue("DlgPololuMaestroTester/port", "COM1").toString());
    ui->cmbBaudrate->setCurrentIndex(ProgramOptions::getConfigValue("DlgPololuMaestroTester/baudrate", 0).toInt());

    QTimer::singleShot(50, this, SLOT(updateRead()));
}

DlgPololuMaestroTester::~DlgPololuMaestroTester()
{
    ProgramOptions::setConfigValue("DlgPololuMaestroTester/port", ui->cmbPort->currentText());
    ProgramOptions::setConfigValue("DlgPololuMaestroTester/baudrate", ui->cmbBaudrate->currentIndex());
    delete ui;
    com.close();
    delete serial;
}

void DlgPololuMaestroTester::updateRead()
{
    if (com.isConnectionOpen()) {
        serial->sendCommand(pololuMaestroBuildSetPosCommand(ui->spinChannel->value(), ui->spinSet->value()));
        if (com.hasErrorOccured()) {
            ui->labMessage->setText(tr("<font color=\"red\">error: %1</font>").arg(com.getLastError().c_str()));
            com.resetError();
        } else {
            ui->w1->setEnabled(true);
            ui->w2->setEnabled(true);
            ui->w3->setEnabled(true);
            ui->labMessage->setText(tr("connected"));
            QByteArray res=serial->queryCommand(pololuMaestroBuildGetPosCommand(ui->spinChannel->value()), 2);
            ui->spinRead->setValue(pololuMaestroPosToMicroseconds(res));
        }
    } else {
        ui->labMessage->setText(tr("not connected, last error: %1").arg(com.getLastError().c_str()));
        ui->btnConnect->setChecked(false);
        ui->btnConnect->setText(tr("&Connect"));
        ui->w1->setEnabled(false);
        ui->w2->setEnabled(false);
        ui->w3->setEnabled(false);

    }

    QTimer::singleShot(50, this, SLOT(updateRead()));
}

void DlgPololuMaestroTester::on_spinSet_valueChanged(int value)
{
    if (com.isConnectionOpen()) {
        serial->sendCommand(pololuMaestroBuildSetPosCommand(ui->spinChannel->value(), value));
    }
}

void DlgPololuMaestroTester::on_btnConnect_clicked(bool checked)
{
    if (!checked) {
        com.close();
        ui->btnConnect->setChecked(false);
        ui->btnConnect->setText(tr("&Connect"));
        ui->w1->setEnabled(false);
        ui->w2->setEnabled(false);
        ui->w3->setEnabled(false);
        //qDebug()<<"close connection";
    } else {
        com.open(ui->cmbPort->currentText().toLocal8Bit().data(), ui->cmbBaudrate->currentText().toInt());
        //qDebug()<<"opening connection "<<com.isConnectionOpen();
        if (com.isConnectionOpen()) {
            ui->btnConnect->setChecked(true);
            ui->btnConnect->setText(tr("&Disconnect"));
            ui->labMessage->setText(tr("connected ..."));
            ui->w1->setEnabled(true);
            ui->w2->setEnabled(true);
            ui->w3->setEnabled(true);
        } else {
            ui->btnConnect->setChecked(false);
            ui->btnConnect->setText(tr("&Connect"));
            ui->labMessage->setText(tr("could not connect, reason: %1").arg(com.getLastError().c_str()));
            ui->w1->setEnabled(false);
            ui->w2->setEnabled(false);
            ui->w3->setEnabled(false);
        }

    }
}

void DlgPololuMaestroTester::on_cmbBaudrate_currentIndexChanged(int /*index*/)
{
    if (com.isConnectionOpen()) {
        com.close();
        com.open(ui->cmbPort->currentText().toLocal8Bit().data(), ui->cmbBaudrate->currentText().toInt());
        if (com.isConnectionOpen()) {
            ui->btnConnect->setChecked(true);
            ui->btnConnect->setText(tr("&Disconnect"));
        } else {
            ui->btnConnect->setChecked(false);
            ui->btnConnect->setText(tr("&Connect"));
        }
    }
}
