/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    del->setUseCodeEditorFontForMultilineEdits(true);
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
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);

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
