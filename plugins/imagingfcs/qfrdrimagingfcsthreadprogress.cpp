/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qfrdrimagingfcsthreadprogress.h"
#include "ui_qfrdrimagingfcsthreadprogress.h"

QFRDRImagingFCSThreadProgress::QFRDRImagingFCSThreadProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFRDRImagingFCSThreadProgress)
{
    ui->setupUi(this);
    m_wasCanceled=false;
    m_isDone=false;
    resetWidget();
    setStatus(0);
    setMessage("");
    setName("");
    timer=new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(337);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();
}

QFRDRImagingFCSThreadProgress::~QFRDRImagingFCSThreadProgress()
{
    delete ui;
}

void QFRDRImagingFCSThreadProgress::resetWidget() {
    m_wasCanceled=false;
    m_isDone=false;
    setStatus(0);
    ui->labCanceling->setVisible(m_wasCanceled);
}

void QFRDRImagingFCSThreadProgress::setStatus(int status) {
    m_status=status;
    switch(status) {
        /*case 0: ui->labStatus->setText(tr("waiting ...")); break;
        case 1: ui->labStatus->setText(tr("running ...")); break;
        case 2: ui->labStatus->setText(tr("done ...")); break;
        case -1: ui->labStatus->setText(tr("error ...")); break;
        default: ui->labStatus->setText(""); break;*/
        case 0: ui->labStatus->setPixmap(QPixmap(":/imaging_fcs/thread_wait.png")); break;
        case 1: {
            started.restart();

            ui->labStatus->setPixmap(QPixmap(":/imaging_fcs/thread_run.png"));
            } break;
        case 2: {
            qint64 ms=started.elapsed();
            runMins=ms/60000;
            runSecs=(ms-runMins*60000)/1000;
            ui->labStatus->setPixmap(QPixmap(":/imaging_fcs/thread_ok.png"));
            } break;

        case -1: {
            qint64 ms=started.elapsed();
            runMins=ms/60000;
            runSecs=(ms-runMins*60000)/1000;
            ui->labStatus->setPixmap(QPixmap(":/imaging_fcs/thread_error.png"));
            } break;
        default: ui->labStatus->setText(""); break;
    }

    QPalette pal=ui->labStatus->palette();
    if (status<0) {
        pal.setColor(QPalette::Active, QPalette::WindowText, QColor("darkred"));
    } else if (status==0) {
        pal.setColor(QPalette::Active, QPalette::WindowText, QColor("darkblue"));
    } else {
        pal.setColor(QPalette::Active, QPalette::WindowText, palette().color(QPalette::Active, QPalette::WindowText));
    }
    ui->labStatus->setPalette(pal);
    ui->labMessage->setPalette(pal);

    if ((status==2)||(status==-1)) {
        m_isDone=true;
        /*ui->btnCancel->setText("&Close");
        ui->btnCancel->setIcon(QIcon(":/imaging_fcs/job_done.png"));*/
        ui->btnCancel->setEnabled(false);
        ui->labCanceling->setVisible(false);
    } else {
        m_isDone=false;
        ui->btnCancel->setText("&Cancel");
        ui->btnCancel->setIcon(QIcon(":/imaging_fcs/job_cancel.png"));
        ui->btnCancel->setEnabled(true);
    }

    if (status==2) {
        ui->progressBar->setValue(ui->progressBar->maximum());
    }
}

void QFRDRImagingFCSThreadProgress::setName(QString name) {
    ui->labName->setText(name);
}

void QFRDRImagingFCSThreadProgress::setMessage(QString message) {
    ui->labMessage->setText(message);
}

void QFRDRImagingFCSThreadProgress::setProgress(int p) {
    ui->progressBar->setValue(p);
}

void QFRDRImagingFCSThreadProgress::incProgress(int p) {
    ui->progressBar->setValue(ui->progressBar->value()+p);
}

void QFRDRImagingFCSThreadProgress::setRange(int min, int max) {
    ui->progressBar->setRange(min, max);
}

void QFRDRImagingFCSThreadProgress::updateTime()
{
    if (m_status==1) {
        qint64 ms=started.elapsed();
        qint64 rrunMins=ms/60000;
        qint64 rrunSecs=(ms-rrunMins*60000)/1000;
        ui->labRunning->setText(tr("<small>runtime: %1:%2 min.</small>").arg(rrunMins,2,10,QLatin1Char('0')).arg(rrunSecs,2,10,QLatin1Char('0')));
    } else if (m_status==2 || m_status<0) {
        QString txt=tr("<small>runtime: %1:%2 min.</small>").arg(runMins,2,10,QLatin1Char('0')).arg(runSecs,2,10,QLatin1Char('0'));
        if (ui->labRunning->text()!=txt) ui->labRunning->setText(txt);
    } else {
        if (!ui->labRunning->text().isEmpty()) {
            ui->labRunning->setText("");
        }
    }

}

void QFRDRImagingFCSThreadProgress::on_btnCancel_clicked() {
   // if (m_isDone) {
        m_wasCanceled=true;
        ui->labCanceling->setVisible(m_wasCanceled);
        emit cancelClicked();
    /*} else {
        emit closeClicked();
    }*/
}

int QFRDRImagingFCSThreadProgress::getRangeMax() const {
    return ui->progressBar->maximum();
}

int QFRDRImagingFCSThreadProgress::getRangeMin() const {
    return ui->progressBar->minimum();
}

int QFRDRImagingFCSThreadProgress::getProgress() const {
    return ui->progressBar->value();
}

bool QFRDRImagingFCSThreadProgress::wasCanceled() const {
    return m_wasCanceled;
}

bool QFRDRImagingFCSThreadProgress::isDone() const  {
    return m_isDone;
}

int QFRDRImagingFCSThreadProgress::getStatus() const {
    return m_status;
}
