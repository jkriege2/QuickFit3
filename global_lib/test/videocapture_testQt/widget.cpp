#include "widget.h"
#include "ui_widget.h"
#include <stdint.h>
#include <QDebug>
#include <QTimer>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    vid=new VideoCapture();
    std::vector<std::string> list=vid->getDeviceList();
    for (size_t i=0; i<list.size(); i++) {
        ui->cmbDevices->addItem(list[i].c_str());
    }
    ui->btnCapture->setEnabled(false);
    ui->btnCaptureContinuous->setEnabled(false);
    ui->btnConfig->setEnabled(false);
    ui->labStatus->setText(tr("disconnect"));
    dlg=NULL;
}

Widget::~Widget() {
    delete ui;
    delete vid;
}

void Widget::on_btnConnect_toggled(bool checked) {
    if (checked) {
        if (dlg) {
            dlg->close();
            dlg->deleteLater();
            dlg=NULL;
        }
        vid->disconnect();
        if (vid->connect(ui->cmbDevices->currentIndex())) {
            ui->cmbDevices->setEnabled(false);
            ui->btnCapture->setEnabled(true);
            ui->btnConfig->setEnabled(true);
            ui->btnCaptureContinuous->setEnabled(true);
            ui->btnConnect->setText(tr("&Disconnect"));
            ui->labStatus->setText(tr("connect, size = %1x%2").arg(vid->getVideoWidth()).arg(vid->getVideoHeight()));
            ui->btnCaptureContinuous->setChecked(true);
        } else {
            QMessageBox::critical(this, tr("Connection Error"), tr("could not connect to the specified device\n  error: %1").arg(vid->getLastError().c_str()));
            ui->btnConnect->setChecked(false);
            ui->cmbDevices->setEnabled(false);
            ui->btnConfig->setEnabled(false);
            ui->btnCaptureContinuous->setEnabled(false);
            ui->btnCapture->setEnabled(false);
            ui->btnConnect->setText(tr("C&onnect"));
            ui->labStatus->setText(tr("disconnect"));
        }
    } else {
        ui->cmbDevices->setEnabled(true);
        ui->btnCapture->setEnabled(false);
        ui->btnCaptureContinuous->setEnabled(false);
        ui->btnConfig->setEnabled(false);
        ui->btnConnect->setText(tr("C&onnect"));
        ui->labStatus->setText(tr("disconnect"));
    }
}

void Widget::on_btnCapture_clicked() {
    uint8_t* img=(unsigned char*)malloc(vid->getVideoWidth()*vid->getVideoHeight()*3*sizeof(uint8_t));

    if (vid->captureFrame(img)) {

        QImage qimg(vid->getVideoWidth(), vid->getVideoHeight(), QImage::Format_ARGB32);

        for (int y=0; y<vid->getVideoHeight(); y++) {
            uchar *line=qimg.scanLine(y);
            for (int x=0; x<vid->getVideoWidth(); x++) {
                line[x*4+2]=img[y*vid->getVideoWidth()*3+x*3+0];
                line[x*4+1]=img[y*vid->getVideoWidth()*3+x*3+1];
                line[x*4+0]=img[y*vid->getVideoWidth()*3+x*3+2];
                line[x*4+3]=0xFF;
            }
        }

        QPixmap pix;
        pix.convertFromImage(qimg);
        ui->label->setPixmap(pix);
        free(img);
    } else {
        qDebug()<<"could not capture frame,\n    reason: "<<vid->getLastError().c_str();
    }
}

void Widget::captureTimed() {
    uint8_t* img=(unsigned char*)malloc(vid->getVideoWidth()*vid->getVideoHeight()*3*sizeof(uint8_t));

    if (vid->captureFrame(img)) {

        QImage qimg(vid->getVideoWidth(), vid->getVideoHeight(), QImage::Format_ARGB32);

        for (int y=0; y<vid->getVideoHeight(); y++) {
            uchar *line=qimg.scanLine(y);
            for (int x=0; x<vid->getVideoWidth(); x++) {
                line[x*4+2]=img[y*vid->getVideoWidth()*3+x*3+0];
                line[x*4+1]=img[y*vid->getVideoWidth()*3+x*3+1];
                line[x*4+0]=img[y*vid->getVideoWidth()*3+x*3+2];
                line[x*4+3]=0xFF;
            }
        }

        QPixmap pix;
        pix.convertFromImage(qimg);
        ui->label->setPixmap(pix);
        free(img);
    } else {
        qDebug()<<"could not capture frame,\n    reason: "<<vid->getLastError().c_str();
    }
    if (ui->btnCaptureContinuous->isChecked() && ui->btnCaptureContinuous->isEnabled() && vid->isConnected()) {
        QTimer::singleShot(10, this, SLOT(captureTimed()));
    }
}

void Widget::on_btnCaptureContinuous_toggled(bool checked) {
    if (checked) captureTimed();
}

void Widget::on_btnConfig_clicked() {
    if (!dlg)  {
        dlg=new VideoCaptureUserPropertyWindow(this);
        dlg->init(vid);
    }

    dlg->show();
}
