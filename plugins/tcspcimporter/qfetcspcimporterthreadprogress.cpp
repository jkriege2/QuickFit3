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
    timer=new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(337);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();

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
        case 1: {
                started.restart();
                ui->labStatus->setPixmap(QPixmap(":/tcspcimporter/thread_run.png")); break;
            }
        case 2: {
                qint64 ms=started.elapsed();
                runMins=ms/60000;
                runSecs=(ms-runMins*60000)/1000;
                ui->labStatus->setPixmap(QPixmap(":/tcspcimporter/thread_ok.png")); break;
            }
        case -1:  {
                qint64 ms=started.elapsed();
                runMins=ms/60000;
                runSecs=(ms-runMins*60000)/1000;
                ui->labStatus->setPixmap(QPixmap(":/tcspcimporter/thread_error.png")); break;
            }
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

void QFETCSPCImporterThreadProgress::updateTime()
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
