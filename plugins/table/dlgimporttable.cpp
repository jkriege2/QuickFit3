#include "dlgimporttable.h"
#include "ui_dlgimporttable.h"
#include "programoptions.h"
#include "qfrdrtabledelegate.h"
#include<QItemSelectionModel>

DlgIMportTable::DlgIMportTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgIMportTable)
{
    ui->setupUi(this);
    ui->tableView->setItemDelegate(new QFRDRTableDelegate(ui->tableView));
    ui->tableView->selectAll();
    ProgramOptions::getConfigWindowGeometry(this, "DlgIMportTable");
}

DlgIMportTable::~DlgIMportTable()
{
    ProgramOptions::setConfigWindowGeometry(this, "DlgIMportTable");
    delete ui;
}

bool DlgIMportTable::importAll() const
{
    return ui->radImportAll->isChecked();
}

void DlgIMportTable::setModel(QAbstractItemModel *model)
{
    disconnect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionCHanged()));
    ui->tableView->setModel(model);
    ui->tableView->selectAll();
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionCHanged()));
    ui->radImportAll->setChecked(true);
}

QItemSelectionModel *DlgIMportTable::getSelection() const
{
    return ui->tableView->selectionModel();
}

int DlgIMportTable::importMode() const
{
    return ui->cmbImportWhere->currentIndex();
}

void DlgIMportTable::on_radImportAll_toggled(bool checked)
{
    if (checked) {
        ui->tableView->selectAll();
    }
}

void DlgIMportTable::selectionCHanged()
{
    bool all=true;
    for (int i=0; i<ui->tableView->model()->columnCount(); i++) {
        all=all&&ui->tableView->selectionModel()->isColumnSelected(i, QModelIndex());
        if (!all) break;
    }
    if (all) {
        for (int i=0; i<ui->tableView->model()->rowCount(); i++) {
            all=all&&ui->tableView->selectionModel()->isRowSelected(i, QModelIndex());
            if (!all) break;
        }
    }
    ui->radImportAll->setChecked(all);
    ui->radImportSelected->setChecked(!all);
}
