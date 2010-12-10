#include "qfrdrtableeditor.h"
#include "qfrdrtable.h"
#include "dlgcsvparameters.h"


QFRDRTableEditor::QFRDRTableEditor(QWidget* parent):
    QFRawDataEditor(parent)
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

    QHBoxLayout* hbl=new QHBoxLayout(this);
    l->addLayout(hbl);
    hbl->setContentsMargins(0,5,5,5);
    hbl->setSpacing(0);

    btnLoadTable=new QPushButton(QIcon(":/table_open.png"), "", this);
    btnLoadTable->setToolTip(tr("Load table ..."));
    btnLoadTable->resize(24,24);
    connect(btnLoadTable, SIGNAL(clicked()), this, SLOT(slLoadTable()));
    hbl->addWidget(btnLoadTable);

    btnSaveTable=new QPushButton(QIcon(":/table_save.png"), "", this);
    btnSaveTable->setToolTip(tr("Save table ...")); btnSaveTable->resize(24,24);
    connect(btnSaveTable, SIGNAL(clicked()), this, SLOT(slSaveTable()));
    hbl->addWidget(btnSaveTable);

    hbl->addSpacing(5);

    btnClear=new QPushButton(QIcon(":/table_clear.png"), "", this);
    btnClear->setToolTip(tr("Clear table ...")); btnClear->resize(24,24);
    connect(btnClear, SIGNAL(clicked()), this, SLOT(slClear()));
    hbl->addWidget(btnClear);

    btnInsertRow=new QPushButton(QIcon(":/row_insert.png"), "", this);
    btnInsertRow->setToolTip(tr("Insert a row above of the currently selected cell ...")); btnInsertRow->resize(24,24);
    connect(btnInsertRow, SIGNAL(clicked()), this, SLOT(slInsertRow()));
    hbl->addWidget(btnInsertRow);

    btnAppendRow=new QPushButton(QIcon(":/row_append.png"), "", this);
    btnAppendRow->setToolTip(tr("Append a row to the table ...")); btnAppendRow->resize(24,24);
    connect(btnAppendRow, SIGNAL(clicked()), this, SLOT(slAppendRow()));
    hbl->addWidget(btnAppendRow);

    btnDeleteRow=new QPushButton(QIcon(":/row_delete.png"), "", this);
    btnDeleteRow->setToolTip(tr("Delete the currenty selected row(s) ...")); btnDeleteRow->resize(24,24);
    connect(btnDeleteRow, SIGNAL(clicked()), this, SLOT(slDeleteRow()));
    hbl->addWidget(btnDeleteRow);

    btnInsertColumn=new QPushButton(QIcon(":/column_insert.png"), "", this);
    btnInsertColumn->setToolTip(tr("Insert a column left of the currently selected cell ...")); btnInsertColumn->resize(24,24);
    connect(btnInsertColumn, SIGNAL(clicked()), this, SLOT(slInsertColumn()));
    hbl->addWidget(btnInsertColumn);

    btnAppendColumn=new QPushButton(QIcon(":/column_append.png"), "", this);
    btnAppendColumn->setToolTip(tr("Append a column to the table ...")); btnAppendColumn->resize(24,24);
    connect(btnAppendColumn, SIGNAL(clicked()), this, SLOT(slAppendColumn()));
    hbl->addWidget(btnAppendColumn);

    btnDeleteColumn=new QPushButton(QIcon(":/column_delete.png"), "", this);
    btnDeleteColumn->setToolTip(tr("Delete the currenty selected column(s) ...")); btnDeleteColumn->resize(24,24);
    connect(btnDeleteColumn, SIGNAL(clicked()), this, SLOT(slDeleteColumn()));
    hbl->addWidget(btnDeleteColumn);

    hbl->addSpacing(5);

    btnSetDatatype=new QPushButton(QIcon(":/cell_type.png"), "", this);
    btnSetDatatype->setToolTip(tr("cell the datatype of the currently selected cells ...")); btnSetDatatype->resize(24,24);
    connect(btnSetDatatype, SIGNAL(clicked()), this, SLOT(slSetDatatype()));
    hbl->addWidget(btnSetDatatype);

    hbl->addSpacing(5);

    btnSetColumnTitle=new QPushButton(QIcon(":/column_title.png"), "", this);
    btnSetColumnTitle->setToolTip(tr("Set column title ...")); btnSetColumnTitle->resize(24,24);
    connect(btnSetColumnTitle, SIGNAL(clicked()), this, SLOT(slSetColumnTitle()));
    hbl->addWidget(btnSetColumnTitle);



    hbl->addStretch();
    tvMain=new QTableView(this);
    QFontMetrics fm(font());
    tvMain->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    l->addWidget(tvMain);
}

void QFRDRTableEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        QFRDRTable* m=qobject_cast<QFRDRTable*>(old);
        if (m && m->model()) {
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnLoadTable, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnClear, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnSetColumnTitle, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnAppendRow, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnAppendColumn, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnInsertRow, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnInsertColumn, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnDeleteColumn, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnDeleteRow, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnSetDatatype, SLOT(setEnabled(bool)));
        }
    }
    //std::cout<<"qobject_cast ... ";
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    //std::cout<<m<<" ... ";
    //if (m) std::cout<<m->model()<<" ... ";
    if (m && m->model()) {
        tvMain->setModel(m->model());
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnLoadTable, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnClear, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnSetColumnTitle, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnAppendRow, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnAppendColumn, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnInsertRow, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnInsertColumn, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnDeleteColumn, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnDeleteRow, SLOT(setEnabled(bool)));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), btnSetDatatype, SLOT(setEnabled(bool)));
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
            QString fileName = QFileDialog::getSaveFileName(this, m->getExportDialogTitle(), currentTableDir, filter, &selectedFilter);
            if ((!fileName.isEmpty())&&(!fileName.isNull())) {
                int f=filter.split(";;").indexOf(selectedFilter);
                std::cout<<"selectedFilter: "<<selectedFilter.toStdString()<<"   "<<m->getExportFiletypes().at(f).toStdString()<<std::endl;
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
            QString fileName = QFileDialog::getOpenFileName(this, tr("Load Table ..."), currentTableDir, filter.join(";;"), &selectedFilter);
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
                items << tr("string") << tr("double") << tr("integer") << tr("date") << tr("bool");
                bool ok;
                QString item = QInputDialog::getItem(this, tr("Change cell types ..."), tr("Select a new datatype:"), items, 0, false, &ok);
                if (ok) {
                    QVariant::Type t=QVariant::String;
                    switch (items.indexOf(item)) {
                        case 0: t=QVariant::String; break;
                        case 1: t=QVariant::Double; break;
                        case 2: t=QVariant::LongLong; break;
                        case 3: t=QVariant::Date; break;
                        case 4: t=QVariant::Bool; break;
                    }
                    QModelIndexList l=sm->selectedIndexes();
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





