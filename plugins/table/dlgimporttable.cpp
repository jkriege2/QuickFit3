#include "dlgimporttable.h"
#include "ui_dlgimporttable.h"
#include "programoptions.h"
#include "qfrdrtabledelegate.h"
#include<QItemSelectionModel>
#include "qftablepluginmodel.h"

DlgIMportTable::DlgIMportTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgIMportTable)
{
    ui->setupUi(this);
    ui->btnPasteAgain->setVisible(false);
    ui->tableView->setItemDelegate(new QFRDRTableDelegate(ui->tableView));
    ui->tableView->selectAll();
    ProgramOptions::getConfigWindowGeometry(this, "DlgIMportTable");
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

DlgIMportTable::DlgIMportTable(bool pastAgainEnabled, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgIMportTable)
{
    ui->setupUi(this);
    ui->btnPasteAgain->setVisible(pastAgainEnabled);
    ui->tableView->setItemDelegate(new QFRDRTableDelegate(ui->tableView));
    ui->tableView->selectAll();
    ui->cmbImportWhere->setCurrentIndex(2);
    ProgramOptions::getConfigWindowGeometry(this, "DlgIMportTable");
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);

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

bool DlgIMportTable::importExpressions() const
{
    return ui->chkImportExressions->isChecked();
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

void DlgIMportTable::doImport(QFTablePluginModel *model_out, int currentRow, int currentCol)
{
    QAbstractTableModel *datamodel=qobject_cast<QAbstractTableModel*>(ui->tableView->model());
    if (datamodel) {
        QSet<int> excludedRoles, excludedHeaderRoles;
        if (!this->importExpressions()) {
            excludedRoles<<QFRDRTable::TableExpressionRole;
            excludedHeaderRoles<<QFRDRTable::ColumnExpressionRole;
        }
        QList<int> coll;
        QList<int> rowl;
        if (!this->importAll()) {
            QSet<int> cols;
            QSet<int> rows;
            QModelIndexList idxs=this->getSelection()->selectedIndexes();
            for (int i=0; i<idxs.size(); i++) {
                if (!cols.contains(idxs[i].column())) {
                    cols.insert(idxs[i].column());
                }
                if (!rows.contains(idxs[i].row())) {
                    rows.insert(idxs[i].row());
                }
            }
            coll=cols.toList();
            rowl=rows.toList();
            qSort(coll);
            qSort(rowl);
        } else {
            for (int i=0; i<datamodel->columnCount(); i++) {
                coll.append(i);
            }
            for (int i=0; i<datamodel->rowCount(); i++) {
                rowl.append(i);
            }
        }
        int startR=0;
        int startC=-1;
        if (this->importMode()==0) {
            startC=model_out->columnCount();
        } else if (this->importMode()==1) {
            startC=0;
        } else if (this->importMode()==2) {
            startR=currentRow;
            startC=currentCol;
        }
        //qDebug()<<startR<<startC<<coll<<rowl;
        if (startR>=0 && startC>=0) {
            QFTableModel::copyColumnHeaderMode copyHeader=QFTableModel::copyHeaderAskUser;
            if (this->importAll()) {
                model_out->startMultiUndo();
                for (int c=0; c<datamodel->columnCount(); c++) {
                    model_out->copyColumnFromModel(datamodel, c, startC+c, startR, -1, -1, &copyHeader, excludedRoles, excludedHeaderRoles);
                }
                model_out->endMultiUndoAndReset();
            } else {
                QModelIndexList idxs=this->getSelection()->selectedIndexes();
                QList<QFTableModel::cellToCopy> cells;
                for (int id=0; id<idxs.size(); id++) {
                    int c=idxs[id].column();
                    int r=idxs[id].row();
                    int ch=startC+coll.indexOf(c);
                    int rh=startR+rowl.indexOf(r);
                    if (c>=0 && r>=0 && ch>=0 && rh>=0) {
                        cells.append(QFTableModel::cellToCopy(r,c,rh,ch));
                    }
                }
                model_out->copyCellsFromModelCreate(datamodel, cells, &copyHeader, excludedRoles, excludedHeaderRoles);
            }
        }
    }
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

void DlgIMportTable::on_btnPasteAgain_clicked()
{
    QFTablePluginModel* tab=qobject_cast<QFTablePluginModel*>(ui->tableView->model());
    if (tab) {
        bool ro=tab->isReadonly();
        tab->setReadonly(false);
        tab->clear();
        tab->paste();
        tab->setReadonly(ro);
    }
}
