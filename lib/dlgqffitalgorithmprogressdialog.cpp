#include "dlgqffitalgorithmprogressdialog.h"

#include <QTimer>

dlgQFFitAlgorithmProgressDialog::dlgQFFitAlgorithmProgressDialog(QWidget* parent):
    QDialog(parent)//, QFFitAlgorithmReporter()
{
    setupUi(this);
    m_canceled=false;
    m_display=false;
    m_displayDelay=500;
}

dlgQFFitAlgorithmProgressDialog::~dlgQFFitAlgorithmProgressDialog()
{
    //dtor
}

void dlgQFFitAlgorithmProgressDialog::setAllowCancel(bool enable) {
    btnCancel->setEnabled(enable);
}


void dlgQFFitAlgorithmProgressDialog::done() {
    m_display=false;
    close();
}

void dlgQFFitAlgorithmProgressDialog::doDisplay() {
    if (m_display) open();
}

void dlgQFFitAlgorithmProgressDialog::display() {
    m_canceled=false;
    m_display=true;
    labCancel->setText("");
    if (m_displayDelay>0) QTimer::singleShot(m_displayDelay, this, SLOT(doDisplay()));
    else doDisplay();
}

void dlgQFFitAlgorithmProgressDialog::on_btnCancel_clicked() {
    m_canceled=true;
    labCancel->setText(tr("canceled ... please wait ..."));
    btnCancel->setEnabled(false);
}


void dlgQFFitAlgorithmProgressDialog::reportStatus(const QString& message) {
    labStatus->setText(message);
}

void dlgQFFitAlgorithmProgressDialog::setProgressMax(int max) {
    prgFit->setRange(0,max);
}

void dlgQFFitAlgorithmProgressDialog::setProgress(int value) {
    prgFit->setValue(value);
}

void dlgQFFitAlgorithmProgressDialog::setProgressFull() {
    prgFit->setValue(prgFit->maximum());
}

void dlgQFFitAlgorithmProgressDialog::incProgress(int increment) {
    prgFit->setValue(prgFit->value()+increment);
}




void dlgQFFitAlgorithmProgressDialog::reportSuperStatus(const QString& message) {
    labSuperStatus->setText(message);
}

void dlgQFFitAlgorithmProgressDialog::setSuperProgressMax(int max) {
    prgOverall->setRange(0,max);
}

void dlgQFFitAlgorithmProgressDialog::setSuperProgress(int value) {
    prgOverall->setValue(value);
}

void dlgQFFitAlgorithmProgressDialog::incSuperProgress(int increment) {
    prgOverall->setValue(prgOverall->value()+increment);
}








dlgQFFitAlgorithmProgressDialogReporter::dlgQFFitAlgorithmProgressDialogReporter(dlgQFFitAlgorithmProgressDialog* parent):
    QObject(parent), QFFitAlgorithmReporter()
{
    m_parent=parent;
    connect(this, SIGNAL(s_reportStatus(const QString&)), parent, SLOT(reportStatus(const QString&)));
    connect(this, SIGNAL(s_setProgressMax(int)), parent, SLOT(setProgressMax(int)));
    connect(this, SIGNAL(s_setProgress(int)), parent, SLOT(setProgress(int)));
    connect(this, SIGNAL(s_setProgressFull()), parent, SLOT(setProgressFull()));
    connect(this, SIGNAL(s_incProgress(int)), parent, SLOT(incProgress(int)));
}


void dlgQFFitAlgorithmProgressDialogReporter::reportStatus(const QString& message) {
    emit s_reportStatus(message);
}

void dlgQFFitAlgorithmProgressDialogReporter::setProgressMax(int max) {
    emit s_setProgressMax(max);
}

void dlgQFFitAlgorithmProgressDialogReporter::setProgress(int value) {
    emit s_setProgress(value);
}

void dlgQFFitAlgorithmProgressDialogReporter::setProgressFull() {
    emit s_setProgressFull();
}

void dlgQFFitAlgorithmProgressDialogReporter::incProgress(int increment) {
    emit s_incProgress(increment);
}

bool dlgQFFitAlgorithmProgressDialogReporter::isCanceled() {
    return m_parent->isCanceled();
}


