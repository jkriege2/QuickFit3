#include "qfrdrtableeditor.h"
#include "qfrdrtable.h"
#include "dlgcsvparameters.h"


QFRDRTableEditor::QFRDRTableEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    //std::cout<<"QFRDRTableEditor() ...\n";
    createWidgets();
    //std::cout<<"QFRDRTableEditor() ... done\n";
}

QFRDRTableEditor::~QFRDRTableEditor()
{
    writeSettings();
}

void QFRDRTableEditor::createWidgets() {
    QVBoxLayout* l=new QVBoxLayout(this);
    setLayout(l);



    tvMain=new QEnhancedTableView(this);
    QFontMetrics fm(font());
    tvMain->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    tvMain->setContextMenuPolicy(Qt::ActionsContextMenu);
    tvMain->setItemDelegate(new QFRDRTableDelegate(tvMain));

    tbMain=new QToolBar("tbtablemain", this);
    l->addWidget(tbMain);

    actLoadTable=new QAction(QIcon(":/table/table_open.png"), "load table", this);
    actLoadTable->setToolTip(tr("Load table ..."));
    actLoadTable->setShortcut(QKeySequence::Open);
    connect(actLoadTable, SIGNAL(triggered()), this, SLOT(slLoadTable()));
    tbMain->addAction(actLoadTable);

    actSaveTable=new QAction(QIcon(":/table/table_save.png"), "save table", this);
    actSaveTable->setToolTip(tr("Save table ..."));
    actSaveTable->setShortcut(QKeySequence::SaveAs);
    connect(actSaveTable, SIGNAL(triggered()), this, SLOT(slSaveTable()));
    tbMain->addAction(actSaveTable);

    tbMain->addSeparator();

    actCopy=new QAction(QIcon(":/table/copy.png"), tr("Copy"), this);
    actCopy->setShortcut(QKeySequence::Copy);
    connect(actCopy, SIGNAL(triggered()), this, SLOT(slCopy()));
    tbMain->addAction(actCopy);
    actCut=new QAction(QIcon(":/table/cut.png"), tr("Cut"), this);
    actCut->setShortcut(QKeySequence::Cut);
    connect(actCut, SIGNAL(triggered()), this, SLOT(slCut()));
    tbMain->addAction(actCut);
    actPaste=new QAction(QIcon(":/table/paste.png"), tr("Paste"), this);
    actPaste->setShortcut(QKeySequence::Paste);
    connect(actPaste, SIGNAL(triggered()), this, SLOT(slPaste()));
    tbMain->addAction(actPaste);


    actCopyResults=new QAction(QIcon(":/lib/copy16.png"), tr("Copy selection for clipboard (for Excel ...)"), this);
    actCopyResultsNoHead=new QAction(QIcon(":/lib/copy16_nohead.png"), tr("Copy selection to clipboard (for Excel ...) without herader rows/columns"), this);
    connect(actCopyResults, SIGNAL(triggered()), tvMain, SLOT(copySelectionToExcel()));
    connect(actCopyResultsNoHead, SIGNAL(triggered()), tvMain, SLOT(copySelectionToExcelNoHead()));

    tbMain->addSeparator();

    actClear=new QAction(QIcon(":/table/table_clear.png"), "clear", this);
    actClear->setToolTip(tr("Clear table ..."));
    connect(actClear, SIGNAL(triggered()), this, SLOT(slClear()));


    tbMain->addSeparator();

    actResize=new QAction(QIcon(":/table/table_resize.png"), "resize table", this);
    actResize->setToolTip(tr("Resize the table to a new size"));
    actResize->setShortcut(tr("Ctrl+R"));
    connect(actResize, SIGNAL(triggered()), this, SLOT(slResize()));
    tbMain->addAction(actResize);

    actInsertRow=new QAction(QIcon(":/table/row_insert.png"), "insert row", this);
    actInsertRow->setToolTip(tr("Insert a row above of the currently selected cell ..."));
    connect(actInsertRow, SIGNAL(triggered()), this, SLOT(slInsertRow()));
    tbMain->addAction(actInsertRow);

    actAppendRow=new QAction(QIcon(":/table/row_append.png"), "append row", this);
    actAppendRow->setToolTip(tr("Append a row to the table ..."));
    actAppendRow->setShortcut(tr("Ctrl++"));
    connect(actAppendRow, SIGNAL(triggered()), this, SLOT(slAppendRow()));
    tbMain->addAction(actAppendRow);

    actDeleteRow=new QAction(QIcon(":/table/row_delete.png"), "delete row", this);
    actDeleteRow->setToolTip(tr("Delete the currenty selected row(s) ..."));
    connect(actDeleteRow, SIGNAL(triggered()), this, SLOT(slDeleteRow()));
    tbMain->addAction(actDeleteRow);

    actInsertColumn=new QAction(QIcon(":/table/column_insert.png"), "insert column", this);
    actInsertColumn->setToolTip(tr("Insert a column left of the currently selected cell ..."));
    connect(actInsertColumn, SIGNAL(triggered()), this, SLOT(slInsertColumn()));
    tbMain->addAction(actInsertColumn);

    actAppendColumn=new QAction(QIcon(":/table/column_append.png"), "append column", this);
    actAppendColumn->setToolTip(tr("Append a column to the table ..."));
    actAppendRow->setShortcut(tr("Ctrl+I"));
    connect(actAppendColumn, SIGNAL(triggered()), this, SLOT(slAppendColumn()));
    tbMain->addAction(actAppendColumn);

    actDeleteColumn=new QAction(QIcon(":/table/column_delete.png"), "delete column", this);
    actDeleteColumn->setToolTip(tr("Delete the currenty selected column(s) ..."));
    connect(actDeleteColumn, SIGNAL(triggered()), this, SLOT(slDeleteColumn()));
    tbMain->addAction(actDeleteColumn);

    tbMain->addSeparator();

    actDelete=new QAction(QIcon(":/table/cell_clear.png"), "delete contents", this);
    actDelete->setToolTip(tr("Delete contents from selected cells ..."));
    actDelete->setShortcut(QKeySequence::Delete);
    connect(actDelete, SIGNAL(triggered()), this, SLOT(slDelete()));
    tbMain->addAction(actDelete);

    tbMain->addSeparator();

    actSetDatatype=new QAction(QIcon(":/table/cell_type.png"), "set datatype", this);
    actSetDatatype->setToolTip(tr("set the datatype of the currently selected cells ..."));
    actSetDatatype->setShortcut(tr("Ctrl+T"));
    connect(actSetDatatype, SIGNAL(triggered()), this, SLOT(slSetDatatype()));
    tbMain->addAction(actSetDatatype);

    tbMain->addSeparator();

    actSetColumnTitle=new QAction(QIcon(":/table/column_title.png"), "set column title", this);
    actSetColumnTitle->setToolTip(tr("Set column title ..."));
    connect(actSetColumnTitle, SIGNAL(triggered()), this, SLOT(slSetColumnTitle()));
    tbMain->addAction(actSetColumnTitle);

    tvMain->addAction(actCopy);
    tvMain->addAction(actCut);
    tvMain->addAction(actPaste);
    QAction* actSep=new QAction(this);
    actSep->setSeparator(true);
    tvMain->addAction(actSep);
    tvMain->addAction(actCopyResults);
    tvMain->addAction(actCopyResultsNoHead);

    l->addWidget(tvMain);

    propertyEditor->setMenuBarVisible(true);
    QMenu* menuFile=propertyEditor->addMenu("&File", 0);
    menuFile->addAction(actLoadTable);
    menuFile->addAction(actSaveTable);
    QMenu* menuEdit=propertyEditor->addMenu("&Edit", 0);
    menuEdit->addAction(actCopy);
    menuEdit->addAction(actCut);
    menuEdit->addAction(actPaste);
    menuEdit->addSeparator();
    menuEdit->addAction(actCopyResults);
    menuEdit->addAction(actCopyResultsNoHead);
    menuEdit->addSeparator();
    menuEdit->addAction(actDelete);
    menuEdit->addAction(actSetDatatype);

    QMenu* menuTab=propertyEditor->addMenu("&Table", 0);
    menuTab->addAction(actAppendRow);
    menuTab->addAction(actInsertRow);
    menuTab->addAction(actAppendColumn);
    menuTab->addAction(actInsertColumn);
    menuTab->addSeparator();
    menuTab->addAction(actDeleteRow);
    menuTab->addAction(actDeleteColumn);
    menuTab->addSeparator();
    menuTab->addAction(actSetDatatype);
    menuTab->addAction(actSetColumnTitle);
    menuTab->addSeparator();
    menuTab->addAction(actClear);
    menuTab->addAction(actResize);
}

void QFRDRTableEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        QFRDRTable* m=qobject_cast<QFRDRTable*>(old);
        if (m && m->model()) {
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actLoadTable, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actClear, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actSetColumnTitle, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actAppendRow, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actAppendColumn, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actInsertRow, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actInsertColumn, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actDeleteColumn, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actDeleteRow, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actSetDatatype, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actResize, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actCut, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actPaste, SLOT(setEnabled(bool)));
        }
    }
    //std::cout<<"qobject_cast ... ";
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    //std::cout<<m<<" ... ";
    //if (m) std::cout<<m->model()<<" ... ";
    if (m && m->model()) {
        tvMain->setModel(m->model());
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actLoadTable, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actClear, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actSetColumnTitle, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actAppendRow, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actAppendColumn, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actInsertRow, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actInsertColumn, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actDeleteColumn, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actDeleteRow, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actSetDatatype, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actResize, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actCut, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), actPaste, SLOT(setEnabled(bool)));
        m->model()->setReadonly(m->model()->isReadonly());
    } else {
        tvMain->setModel(NULL);
    }
    //std::cout<<m<<" done\n";
}

void QFRDRTableEditor::rawDataChanged() {
}

void QFRDRTableEditor::readSettings() {
    if (settings) {
        //std::cout<<"QFRDRTableEditor::readSettings()\n";
        currentTableDir=settings->getQSettings()->value("rawtableeditor/currentTableDir", currentTableDir).toString();
    }
}

void QFRDRTableEditor::writeSettings() {
    if (settings) {
        settings->getQSettings()->setValue("rawtableeditor/currentTableDir", currentTableDir);
        settings->getQSettings()->sync();
    }
}

void QFRDRTableEditor::slSaveTable() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QString selectedFilter="";
            QString filter= m->getExportDialogFiletypes();
            QString fileName = qfGetSaveFileName(this, m->getExportDialogTitle(), currentTableDir, filter, &selectedFilter);
            if ((!fileName.isEmpty())&&(!fileName.isNull())) {
                int f=filter.split(";;").indexOf(selectedFilter);
                //std::cout<<"selectedFilter: "<<selectedFilter.toStdString()<<"   "<<m->getExportFiletypes().at(f).toStdString()<<std::endl;
                m->exportData(m->getExportFiletypes().at(f), fileName);
            }
        }
    }
}

void QFRDRTableEditor::slLoadTable() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QString selectedFilter="";
            QStringList filter;
            filter<<tr("Comma Separated Values, Decimal Dot (*.txt *.csv *.dat)")
                  <<tr("Semicolon Separated Values, Decimal Dot (*.txt *.csv *.dat)")
                  <<tr("Semicolon Separated Values, Decimal Comma (*.txt *.csv *.dat)");
            QString fileName = qfGetOpenFileName(this, tr("Load Table ..."), currentTableDir, filter.join(";;"), &selectedFilter);
            //std::cout<<"selectedFilter: "<<selectedFilter.toStdString()<<std::endl;
            if (!fileName.isNull()) {
                int f=filter.indexOf(selectedFilter);

                dlgCSVParameters* csvDlg=new dlgCSVParameters(this, settings->getQSettings()->value("table/column_separator_save", ",").toString(),
                                                              settings->getQSettings()->value("table/decimal_separator_save", ".").toString(),
                                                              settings->getQSettings()->value("table/comment_start_save", "#").toString(),
                                                              settings->getQSettings()->value("table/header_start_save", "#!").toString());
                if (f==0) {
                    csvDlg->column_separator=',';
                    csvDlg->decimal_separator='.';
                    csvDlg->comment_start='#';
                    csvDlg->header_start="#!";
                } else if (f==1) {
                    csvDlg->column_separator=';';
                    csvDlg->decimal_separator='.';
                    csvDlg->comment_start='#';
                    csvDlg->header_start="#!";
                } else if (f==2) {
                    csvDlg->column_separator=';';
                    csvDlg->decimal_separator=',';
                    csvDlg->comment_start='#';
                    csvDlg->header_start="#!";
                }
                csvDlg->setFileContents(fileName);
                if (csvDlg->exec()==QDialog::Accepted) {
                    QMap<QString, QVariant> p;
                    settings->getQSettings()->setValue("table/column_separator_save", QString(csvDlg->column_separator));
                    settings->getQSettings()->setValue("table/decimal_separator_save", QString(csvDlg->decimal_separator));
                    settings->getQSettings()->setValue("table/comment_start_save", QString(csvDlg->comment_start));
                    settings->getQSettings()->setValue("table/header_start_save", QString(csvDlg->header_start));
                    m->model()->readCSV(fileName, csvDlg->column_separator, csvDlg->decimal_separator, csvDlg->header_start, csvDlg->comment_start);
                }
            }
        }
    }
}


void QFRDRTableEditor::slClear() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
             QMessageBox::StandardButton answer;
             answer = QMessageBox::question(this, tr("Clear Table"), tr("Clearing the table will delete all its contents. Are you sure that you want to clear the table?"), QMessageBox::Yes | QMessageBox::No);
             if (answer == QMessageBox::Yes) {
                m->model()->clear();
             }
        }
    }
}


void QFRDRTableEditor::slAppendRow() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            m->model()->appendRow();
        }
    }
}

void QFRDRTableEditor::slAppendColumn() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            m->model()->appendColumn();
        }
    }
}


void QFRDRTableEditor::slInsertRow() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (!sm->hasSelection()) {
                m->model()->insertRow(0);
            } else {
                QModelIndexList l=sm->selectedIndexes();
                quint16 i0=m->model()->rowCount();
                for (int i=0; i<l.size(); i++) {
                    if (l[i].row()<i0) i0=l[i].row();
                }
                //std::cout<<"insertRow("<<i0<<")\n";
                m->model()->insertRow(i0);
            }
        }
    }
}


void QFRDRTableEditor::slInsertColumn() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (!sm->hasSelection()) {
                m->model()->insertColumn(0);
            } else {
                QModelIndexList l=sm->selectedIndexes();
                quint16 i0=m->model()->columnCount();
                for (int i=0; i<l.size(); i++) {
                    if (l[i].column()<i0) i0=l[i].column();
                }
                //std::cout<<"insertColumn("<<i0<<")\n";
                m->model()->insertColumn(i0);
            }
        }
    }
}

void QFRDRTableEditor::slDeleteRow() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (!sm->hasSelection()) {
            } else {
                QMessageBox::StandardButton answer;
                answer = QMessageBox::question(this, tr("Delete row(s) ..."), tr("Are you sure that you want to delete the selected rows?"), QMessageBox::Yes | QMessageBox::No);
                if (answer == QMessageBox::Yes) {
                    QModelIndexList l=sm->selectedIndexes();
                    QList<quint16> rl;
                    for (int i=0; i<l.size(); i++) {
                        if (!rl.contains(l[i].row())) rl.append(l[i].row());
                    }
                    qSort(rl);
                    for (int i=rl.size()-1; i>=0; i--) {
                        //std::cout<<"deleting row "<<rl[i]<<std::endl;
                        m->model()->deleteRow(rl[i]);
                    }
                }
            }
        }
    }
}


void QFRDRTableEditor::slDeleteColumn() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (!sm->hasSelection()) {
            } else {
                QMessageBox::StandardButton answer;
                answer = QMessageBox::question(this, tr("Delete row(s) ..."), tr("Are you sure that you want to delete the selected rows?"), QMessageBox::Yes | QMessageBox::No);
                if (answer == QMessageBox::Yes) {
                    QModelIndexList l=sm->selectedIndexes();
                    QList<quint16> rl;
                    for (int i=0; i<l.size(); i++) {
                        if (!rl.contains(l[i].column())) rl.append(l[i].column());
                    }
                    qSort(rl);
                    for (int i=rl.size()-1; i>=0; i--) {
                        //std::cout<<"deleting column "<<rl[i]<<std::endl;
                        m->model()->deleteColumn(rl[i]);
                    }
                }
            }
        }
    }
}

void QFRDRTableEditor::slSetDatatype() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (!sm->hasSelection()) {
            } else {
                QStringList items;
                items << tr("text") << tr("floating point number") << tr("integer number") << tr("date & time") << tr("boolean value");
                QModelIndexList l=sm->selectedIndexes();
                int type=-1;
                bool first=true;
                for (int i=0; i<l.size(); i++) {
                    int t=-1;
                    if (l[i].data().isValid()) {
                        switch(l[i].data().type()) {
                            case QVariant::String: t=0; break;
                            case QVariant::Double: t=1; break;
                            case QVariant::LongLong:
                            case QVariant::Int:
                            case QVariant::ULongLong:
                            case QVariant::UInt:
                                t=2; break;
                            case QVariant::Date:
                            case QVariant::Time:
                            case QVariant::DateTime:
                                t=3; break;
                            case QVariant::Bool:
                                t=4; break;
                            default: break;
                        }

                        if (first) {
                            type=t;
                            first=false;
                        } else {
                            if (type!=t) {
                                type=-1;
                                break;
                            }
                        }
                    }
                }
                bool ok;
                QString item = QInputDialog::getItem(this, tr("Change cell types ..."), tr("Select a new datatype:"), items, type, false, &ok);
                if (ok && items.indexOf(item)>=0) {
                    QVariant::Type t=QVariant::String;
                    switch (items.indexOf(item)) {
                        case 0: t=QVariant::String; break;
                        case 1: t=QVariant::Double; break;
                        case 2: t=QVariant::LongLong; break;
                        case 3: t=QVariant::DateTime; break;
                        case 4: t=QVariant::Bool; break;
                    }

                    for (int i=0; i<l.size(); i++) {
                        m->model()->changeDatatype(l[i].row(), l[i].column(), t);
                    }
                }
            }
        }
    }
}

void QFRDRTableEditor::slSetColumnTitle() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (sm->hasSelection()) {
                quint16 c=sm->currentIndex().column();
                QString t=m->model()->columnTitle(c);
                bool ok;
                QString text = QInputDialog::getText(this, tr("Change Column Title ..."), tr("New column title:"), QLineEdit::Normal, t, &ok);
                 if (ok) {
                     m->model()->setColumnTitle(c, text);
                 }
            }
        }
    }
}

void QFRDRTableEditor::slResize() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            TableResizeDialog* dlg=new TableResizeDialog(this);
            dlg->init(m->model()->columnCount(), m->model()->rowCount());
            if (dlg->exec()==QDialog::Accepted) {
                m->model()->resize(dlg->newHeight(), dlg->newWidth());
            }
            delete dlg;
        }
    }
}

void QFRDRTableEditor::slCopy() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            m->model()->copy(tvMain->selectionModel()->selectedIndexes());
        }
    }
}

void QFRDRTableEditor::slPaste() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QModelIndex c=tvMain->selectionModel()->currentIndex();
            m->model()->paste(c.row(), c.column());
        }
    }
}

void QFRDRTableEditor::slCut() {
    slCopy();
    slDelete();
}

void QFRDRTableEditor::slDelete() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            m->model()->deleteCells(tvMain->selectionModel()->selectedIndexes());
        }
    }
}





