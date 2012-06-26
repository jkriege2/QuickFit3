#include "dlgqfprogressdialog.h"
#include "ui_dlg_qfprogress.h"
#include <QTimer>

dlgQFProgressDialog::dlgQFProgressDialog(QWidget* parent):
    QDialog(parent), ui(new Ui::QFProgressDlg)
{
    ui->setupUi(this);
    m_canceled=false;
    m_display=false;
    m_displayDelay=500;
}

dlgQFProgressDialog::~dlgQFProgressDialog()
{
    delete ui;
}

void dlgQFProgressDialog::setAllowCancel(bool enable) {
    ui->btnCancel->setEnabled(enable);
}


void dlgQFProgressDialog::done() {
    m_display=false;
    close();
    ui->btnCancel->setEnabled(true);
}

void dlgQFProgressDialog::reportTask(const QString &message)
{
    ui->labTask->setText(message);
}

void dlgQFProgressDialog::doDisplay() {
    if (m_display) open();
}

void dlgQFProgressDialog::display() {
    m_canceled=false;
    m_display=true;
    ui->labCancel->setText("");
    if (m_displayDelay>0) QTimer::singleShot(m_displayDelay, this, SLOT(doDisplay()));
    else doDisplay();
}

void dlgQFProgressDialog::on_btnCancel_clicked() {
    m_canceled=true;
    ui->labCancel->setText(tr("canceled ... please wait ..."));
    ui->btnCancel->setEnabled(false);
}


void dlgQFProgressDialog::reportStatus(const QString& message) {
    ui->labStatus->setText(message);
}

void dlgQFProgressDialog::setProgressMax(int max) {
    ui->prgFit->setRange(0,max);
}

void dlgQFProgressDialog::setProgress(int value) {
    ui->prgFit->setValue(value);
}

void dlgQFProgressDialog::setProgressFull() {
    ui->prgFit->setValue(ui->prgFit->maximum());
}

void dlgQFProgressDialog::incProgress(int increment) {
    ui->prgFit->setValue(ui->prgFit->value()+increment);
}







dlgQFProgressDialogReporter::dlgQFProgressDialogReporter(dlgQFProgressDialog* parent):
    QObject(parent), QFFitAlgorithmReporter()
{
    m_parent=parent;
    connect(this, SIGNAL(s_reportStatus(const QString&)), parent, SLOT(reportStatus(const QString&)));
    connect(this, SIGNAL(s_setProgressMax(int)), parent, SLOT(setProgressMax(int)));
    connect(this, SIGNAL(s_setProgress(int)), parent, SLOT(setProgress(int)));
    connect(this, SIGNAL(s_setProgressFull()), parent, SLOT(setProgressFull()));
    connect(this, SIGNAL(s_incProgress(int)), parent, SLOT(incProgress(int)));
}


void dlgQFProgressDialogReporter::reportStatus(const QString& message) {
    emit s_reportStatus(message);
}

void dlgQFProgressDialogReporter::setProgressMax(int max) {
    emit s_setProgressMax(max);
}

void dlgQFProgressDialogReporter::setProgress(int value) {
    emit s_setProgress(value);
}

void dlgQFProgressDialogReporter::setProgressFull() {
    emit s_setProgressFull();
}

void dlgQFProgressDialogReporter::incProgress(int increment) {
    emit s_incProgress(increment);
}

bool dlgQFProgressDialogReporter::isCanceled() {
    return m_parent->isCanceled();
}


