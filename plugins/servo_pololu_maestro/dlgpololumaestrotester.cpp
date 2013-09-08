#include "dlgpololumaestrotester.h"
#include "ui_dlgpololumaestrotester.h"
#include "pololu_tools.h"

DlgPololuMaestroTester::DlgPololuMaestroTester(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPololuMaestroTester)
{
    serial=new QFBinarySerialProtocolHandler(&com, "popolu maestro");
    ui->setupUi(this);
    ui->edtPort->setText(ProgramOptions::getConfigValue("DlgPololuMaestroTester/port", "COM1").toString());
    ui->cmbBaudrate->setCurrentIndex(ProgramOptions::getConfigValue("DlgPololuMaestroTester/baudrate", 0).toInt());
    QTimer::singleShot(10, this, SLOT(updateRead()));
}

DlgPololuMaestroTester::~DlgPololuMaestroTester()
{
    ProgramOptions::setConfigValue("DlgPololuMaestroTester/port", ui->edtPort->text());
    ProgramOptions::setConfigValue("DlgPololuMaestroTester/baudrate", ui->cmbBaudrate->currentIndex());
    delete ui;
}

void DlgPololuMaestroTester::updateRead()
{
    if (com.isConnectionOpen()) {
        if (com.hasErrorOccured()) {
            ui->labMessage->setText(tr("<font color=\"red\">error: %1</font>").arg(com.getLastError().c_str()));
        } else {
            ui->labMessage->setText(tr("connected"));
            QByteArray res=serial->queryCommand(pololuMaestroBuildGetPosCommand(ui->spinChannel->value()), 2);
            ui->spinRead->setValue(pololuMaestroPosToMicroseconds(res));
        }
    } else {
        ui->labMessage->setText(tr("not connected"));
    }

    QTimer::singleShot(10, this, SLOT(updateRead()));
}

void DlgPololuMaestroTester::on_spinSet_valueChanged(int value)
{
    if (com.isConnectionOpen()) {
        serial->sendCommand(pololuMaestroBuildSetPosCommand(ui->spinChannel->value(), value));
    }
}

void DlgPololuMaestroTester::on_btnConnect_clicked()
{
    if (ui->btnConnect->isChecked()) {
        com.close();
        ui->btnConnect->setChecked(false);
        ui->btnConnect->setText(tr("&Connect"));
    } else {
        com.open(ui->edtPort->text().toLocal8Bit().data(), ui->cmbBaudrate->currentText().toInt());
        if (com.isConnectionOpen()) {
            ui->btnConnect->setChecked(true);
            ui->btnConnect->setText(tr("&Disconnect"));
        } else {
            ui->btnConnect->setChecked(false);
            ui->btnConnect->setText(tr("&Connect"));
        }

    }
}

void DlgPololuMaestroTester::on_cmbBaudrate_currentIndexChanged(int index)
{
    if (com.isConnectionOpen()) {
        com.close();
        com.open(ui->edtPort->text().toLocal8Bit().data(), ui->cmbBaudrate->currentText().toInt());
        if (com.isConnectionOpen()) {
            ui->btnConnect->setChecked(true);
            ui->btnConnect->setText(tr("&Disconnect"));
        } else {
            ui->btnConnect->setChecked(false);
            ui->btnConnect->setText(tr("&Connect"));
        }
    }
}
