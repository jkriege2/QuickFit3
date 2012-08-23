#include "qfrdrtablesortdialog.h"
#include "ui_qfrdrtablesortdialog.h"

QFRDRTableSortDialog::QFRDRTableSortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableSortDialog)
{
    ui->setupUi(this);
}

QFRDRTableSortDialog::~QFRDRTableSortDialog()
{
    delete ui;
}

void QFRDRTableSortDialog::setColumns(QList<QPair<int, QString> > cols)
{
    ui->cmbColumn->clear();
    for (int i=0; i<cols.size(); i++) {
        ui->cmbColumn->addItem(QString("%1: %2").arg(cols[i].first+1).arg(cols[i].second), cols[i].first);
    }
}

void QFRDRTableSortDialog::setCurrentColumn(int col)
{
    for (int i=0; i<ui->cmbColumn->count(); i++) {
        if (ui->cmbColumn->itemData(i).toInt()==col) {
            ui->cmbColumn->setCurrentIndex(i);
        }
    }
}

bool QFRDRTableSortDialog::sortAscending() const
{
    return ui->cmbOrder->currentIndex()==0;
}

int QFRDRTableSortDialog::sortColumn() const
{
    return ui->cmbColumn->itemData(ui->cmbColumn->currentIndex()).toInt();
}
