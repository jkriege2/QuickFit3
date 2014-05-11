#include "qfrdrtableindexedstatdialog.h"
#include "ui_qfrdrtableindexedstatdialog.h"

QFRDRTableIndexedStatDialog::QFRDRTableIndexedStatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableIndexedStatDialog)
{
    ui->setupUi(this);
}

QFRDRTableIndexedStatDialog::~QFRDRTableIndexedStatDialog()
{
    delete ui;
}

void QFRDRTableIndexedStatDialog::setColumns(QList<QPair<int, QString> > cols)
{
    ui->cmbData->clear();
    ui->cmbIndex->clear();
    for (int i=0; i<cols.size(); i++) {
        ui->cmbData->addItem(QString("%1: %2").arg(cols[i].first+1).arg(cols[i].second), cols[i].first);
        ui->cmbIndex->addItem(QString("%1: %2").arg(cols[i].first+1).arg(cols[i].second), cols[i].first);
    }
}

int QFRDRTableIndexedStatDialog::indexColumn() const
{
    return ui->cmbIndex->currentIndex();
}

int QFRDRTableIndexedStatDialog::dataColumn() const
{
    return ui->cmbData->currentIndex();
}

bool QFRDRTableIndexedStatDialog::avg() const
{
    return ui->chkMean->isChecked();
}

bool QFRDRTableIndexedStatDialog::std() const
{
    return ui->chkStdDev->isChecked();
}

bool QFRDRTableIndexedStatDialog::median() const
{
    return ui->chkMedian->isChecked();
}

bool QFRDRTableIndexedStatDialog::q25() const
{
    return ui->chkQ25->isChecked();
}

bool QFRDRTableIndexedStatDialog::q75() const
{
    return ui->chkQ75->isChecked();
}

bool QFRDRTableIndexedStatDialog::min() const
{
    return ui->chkMin->isChecked();
}

bool QFRDRTableIndexedStatDialog::max() const
{
    return ui->chkMax->isChecked();
}
