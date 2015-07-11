#include "mainwindow.h"
#include <QDebug>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    serial=new JKSerialConnection();
    tim=new QTimer(this);
    tim->setInterval(1);
    tim->setSingleShot(true);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete serial;
}


void MainWindow::on_btnConnect_clicked() {
    ui->labStatus->setText(tr("connecting ..."));
    serial->close();
    bool ok=serial->open(ui->edtPort->text().toStdString(), ui->spinBaud->value());
    if (ok)  {
        ui->labStatus->setText(tr("connected ..."));
        tim->singleShot(1, this, SLOT(readBytesFromSerial()));
    } else {
        ui->labStatus->setText(tr("<font color=\"red\">error connecting: %1</font>").arg(serial->getLastError().c_str()));
    }
}

void MainWindow::on_btnDisconnect_clicked() {
    tim->stop();
    serial->close();
    ui->labStatus->setText(tr("disconnected ..."));
}

void MainWindow::on_btnSendLB_clicked() {
    QString d=ui->edtText->text()+"\n";
    qDebug()<<"writing "<<ui->edtText->text()<<"<LB>";
    serial->write(d.toStdString());
}

void MainWindow::on_btnSendNoLB_clicked()  {
    QString d=ui->edtText->text();
    qDebug()<<"writing "<<ui->edtText->text();
    serial->write(d.toStdString());
}

void MainWindow::readBytesFromSerial() {
    if (!serial->isConnectionOpen()) return;
    char ch=0;
    while (serial->read_nowait(&ch)) {
        //qDebug()<<"recv"<<int(ch)<<ch;
        ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText()+ch);
        ch=0;
    }
    tim->singleShot(10, this, SLOT(readBytesFromSerial()));
}
