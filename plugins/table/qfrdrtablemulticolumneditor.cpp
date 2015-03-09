#include "qfrdrtablemulticolumneditor.h"
#include "ui_qfrdrtablemulticolumneditor.h"
#include "qfrdrtable.h"

QFRDRTableMultiColumnEditor::QFRDRTableMultiColumnEditor(QFTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableMultiColumnEditor)
{
    ui->setupUi(this);
    QFHTMLDelegate* del;
    ui->tableView->setItemDelegate(del=new QFHTMLDelegate(ui->tableView));
    del->setDisplayRichTextEditor(false);
    del->addMultilineEditColumn(1);
    hmodel.addProperty(QFRDRTable::ColumnExpressionRole, tr("expression"));
    hmodel.addProperty(QFRDRTable::ColumnImageWidth, tr("image width"));
    hmodel.addProperty(QFRDRTable::ColumnCommentRole, tr("comment"));
    hmodel.setModel(model);
    ui->tableView->verticalHeader()->setDefaultSectionSize(64);
    ui->tableView->setModel(&hmodel);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->setDefaultSectionSize(64);
    for (int i=0; i<hmodel.rowCount(); i++) {
        ui->tableView->setRowHeight(i, 64);
    }

}

QFRDRTableMultiColumnEditor::~QFRDRTableMultiColumnEditor()
{
    delete ui;
}

void QFRDRTableMultiColumnEditor::closeEvent(QCloseEvent *e)
{
    QDialog::closeEvent(e);
    e->accept();
    deleteLater();

}
