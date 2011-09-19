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
        case 1: ui->labStatus->setPixmap(QPixmap(":/imaging_fcs/thread_run.png")); break;
        case 2: ui->labStatus->setPixmap(QPixmap(":/imaging_fcs/thread_ok.png")); break;
        case -1: ui->labStatus->setPixmap(QPixmap(":/imaging_fcs/thread_error.png")); break;
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
