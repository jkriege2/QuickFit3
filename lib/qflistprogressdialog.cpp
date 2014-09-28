#include "qflistprogressdialog.h"
#include "ui_qflistprogressdialog.h"

QFListProgressDialog::QFListProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFListProgressDialog)
{
    m_canceled=false;
    ui->setupUi(this);
}

QFListProgressDialog::~QFListProgressDialog()
{
    delete ui;
}

void QFListProgressDialog::setRange(int rmin, int rmax)
{
    ui->progress->setRange(rmin, rmax);
    QApplication::processEvents();
    QApplication::processEvents();
}

void QFListProgressDialog::setMessage(const QString &message)
{
    ui->labMessage->setText(message);
    QApplication::processEvents();
    QApplication::processEvents();
}

void QFListProgressDialog::setMainMessage(const QString &message)
{
    ui->labMain->setText(message);
    QApplication::processEvents();
    QApplication::processEvents();
}

void QFListProgressDialog::addMessage(const QString &message)
{
    ui->list->addItem(message);
    ui->list->setCurrentRow(ui->list->count()-1);
    QApplication::processEvents();
    QApplication::processEvents();
}

void QFListProgressDialog::incProgress(int inc)
{
    ui->progress->setValue(ui->progress->value()+inc);
    QApplication::processEvents();
    QApplication::processEvents();
}

void QFListProgressDialog::setUserClose(bool enabled)
{
    ui->btnCancel->setVisible(!enabled);
    ui->btnClose->setVisible(enabled);
}

void QFListProgressDialog::setProgresComplete()
{
    ui->progress->setValue(ui->progress->maximum());
}

void QFListProgressDialog::on_btnCancel_clicked()
{
    this->m_canceled=true;
    QApplication::processEvents();
    QApplication::processEvents();
}

void QFListProgressDialog::on_btnClose_clicked()
{
    accept();
    deleteLater();
}

bool QFListProgressDialog::wasCanceled() const
{
    QApplication::processEvents();
    return m_canceled;
}

void QFListProgressDialog::setProgress(int value)
{
    ui->progress->setValue(value);
    QApplication::processEvents();
    QApplication::processEvents();
}
