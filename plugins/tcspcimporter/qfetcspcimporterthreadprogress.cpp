#include "qfetcspcimporterthreadprogress.h"
#include "ui_qfetcspcimporterthreadprogress.h"

QFETCSPCImporterThreadProgress::QFETCSPCImporterThreadProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFETCSPCImporterThreadProgress)
{
    ui->setupUi(this);
    m_wasCanceled=false;
    m_isDone=false;
    resetWidget();
    setStatus(0);
    setMessage("");
    setName("");
}

QFETCSPCImporterThreadProgress::~QFETCSPCImporterThreadProgress()
{
    delete ui;
}

void QFETCSPCImporterThreadProgress::resetWidget() {
    m_wasCanceled=false;
    m_isDone=false;
    setStatus(0);
    ui->labCanceling->setVisible(m_wasCanceled);
}

void QFETCSPCImporterThreadProgress::setStatus(int status) {
    m_status=status;
    switch(status) {
        case 0: ui->labStatus->setPixmap(QPixmap(":/tcspcimporter/thread_wait.png")); break;
        case 1: ui->labStatus->setPixmap(QPixmap(":/tcspcimporter/thread_run.png")); break;
        case 2: ui->labStatus->setPixmap(QPixmap(":/tcspcimporter/thread_ok.png")); break;
        case -1: ui->labStatus->setPixmap(QPixmap(":/tcspcimporter/thread_error.png")); break;
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
        ui->btnCancel->setIcon(QIcon(":/tcspcimporter/job_done.png"));*/
        ui->btnCancel->setEnabled(false);
        ui->labCanceling->setVisible(false);
    } else {
        m_isDone=false;
        ui->btnCancel->setText("&Cancel");
        ui->btnCancel->setIcon(QIcon(":/tcspcimporter/job_cancel.png"));
        ui->btnCancel->setEnabled(true);
    }

    if (status==2) {
        ui->progressBar->setValue(ui->progressBar->maximum());
    }
}

void QFETCSPCImporterThreadProgress::setName(QString name) {
    ui->labName->setText(name);
}

void QFETCSPCImporterThreadProgress::setMessage(QString message) {
    ui->labMessage->setText(message);
}

void QFETCSPCImporterThreadProgress::setProgress(int p) {
    ui->progressBar->setValue(p);
}

void QFETCSPCImporterThreadProgress::incProgress(int p) {
    ui->progressBar->setValue(ui->progressBar->value()+p);
}

void QFETCSPCImporterThreadProgress::setRange(int min, int max) {
    ui->progressBar->setRange(min, max);
}

void QFETCSPCImporterThreadProgress::on_btnCancel_clicked() {
   // if (m_isDone) {
        m_wasCanceled=true;
        ui->labCanceling->setVisible(m_wasCanceled);
        emit cancelClicked();
    /*} else {
        emit closeClicked();
    }*/
}

int QFETCSPCImporterThreadProgress::getRangeMax() const {
    return ui->progressBar->maximum();
}

int QFETCSPCImporterThreadProgress::getRangeMin() const {
    return ui->progressBar->minimum();
}

int QFETCSPCImporterThreadProgress::getProgress() const {
    return ui->progressBar->value();
}

bool QFETCSPCImporterThreadProgress::wasCanceled() const {
    return m_wasCanceled;
}

bool QFETCSPCImporterThreadProgress::isDone() const  {
    return m_isDone;
}

int QFETCSPCImporterThreadProgress::getStatus() const {
    return m_status;
}
