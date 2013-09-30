#include "qfrdrtableeditor.h"
#include "qfrdrtablecolumnvaluesdialog.h"
#include "qfrdrtable.h"
#include "dlgcsvparameters.h"
#include "qfrdrtablesortdialog.h"
#include <QtAlgorithms>
#include "qfrdrtableparserfunctions.h"
#include "qfrdrtablecolumneditor.h"
#include "qfrdrtablecurvefitdialog.h"
#include "qfrdrtableregressiondialog.h"
#include "qfrdrtable2dhistogramdialog.h"

QFRDRTableEditor::QFRDRTableEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    //std::cout<<"QFRDRTableEditor() ...\n";
    dlgMathExpression=NULL;
    createWidgets();
    //std::cout<<"QFRDRTableEditor() ... done\n";
}

QFRDRTableEditor::~QFRDRTableEditor()
{
    writeSettings();
}

void QFRDRTableEditor::requestFit(int xCol, int yCol, int sigmaCol, int plot, QString function, bool xlog, bool ylog)
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        //qDebug()<<"regression cX="<<xCol<<"  cY="<<yCol<<"  cW="<<sigmaCol<<"  plot="<<plot<<"  xlog="<<xlog<<"  ylog="<<ylog;
        QFRDRTableCurveFitDialog* dlg=new QFRDRTableCurveFitDialog(m, xCol, yCol, sigmaCol, this, xlog, ylog, -1, plot);
        if (dlg->exec()) {

        }
        delete dlg;
    }

}

void QFRDRTableEditor::requestRegression(int xCol, int yCol, int sigmaCol, int plot, bool xlog, bool ylog)
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        //qDebug()<<"regression cX="<<xCol<<"  cY="<<yCol<<"  cW="<<sigmaCol<<"  plot="<<plot<<"  xlog="<<xlog<<"  ylog="<<ylog;
        QFRDRTableRegressionDialog* dlg=new QFRDRTableRegressionDialog(m, xCol, yCol, sigmaCol, this, xlog, ylog, -1, plot);
        if (dlg->exec()) {

        }
        delete dlg;
    }
}

void QFRDRTableEditor::createWidgets() {
    QVBoxLayout* l=new QVBoxLayout(this);
    setLayout(l);



    tvMain=new QFRDRTableEnhancedTableView(this);
    connect(tvMain, SIGNAL(editExpression(QModelIndex)), this, SLOT(editExpression(QModelIndex)));

    tbMain=new QToolBar("tbtablemain", this);
    l->addWidget(tbMain);

    actLoadTable=new QAction(QIcon(":/table/table_open.png"), "load table", this);
    actLoadTable->setToolTip(tr("Load table ..."));
    actLoadTable->setShortcut(QKeySequence::Open);
    connect(actLoadTable, SIGNAL(triggered()), this, SLOT(slLoadTable()));
    connect(this, SIGNAL(enableActions(bool)), actLoadTable, SLOT(setEnabled(bool)));

    actSaveTable=new QAction(QIcon(":/table/table_save.png"), "save table", this);
    actSaveTable->setToolTip(tr("Save table ..."));
    actSaveTable->setShortcut(QKeySequence::SaveAs);
    connect(actSaveTable, SIGNAL(triggered()), this, SLOT(slSaveTable()));

    actCopy=new QAction(QIcon(":/table/copy.png"), tr("Copy"), this);
    actCopy->setShortcut(QKeySequence::Copy);
    connect(actCopy, SIGNAL(triggered()), this, SLOT(slCopy()));
    actCut=new QAction(QIcon(":/table/cut.png"), tr("Cut"), this);
    actCut->setShortcut(QKeySequence::Cut);
    connect(this, SIGNAL(enableActions(bool)), actCut, SLOT(setEnabled(bool)));
    connect(actCut, SIGNAL(triggered()), this, SLOT(slCut()));
    actPaste=new QAction(QIcon(":/table/paste.png"), tr("Paste"), this);
    actPaste->setShortcut(QKeySequence::Paste);
    connect(this, SIGNAL(enableActions(bool)), actPaste, SLOT(setEnabled(bool)));
    connect(actPaste, SIGNAL(triggered()), this, SLOT(slPaste()));


    actCopyResults=new QAction(QIcon(":/lib/copy16.png"), tr("Copy selection for clipboard (for Excel ...)"), this);
    actCopyResultsNoHead=new QAction(QIcon(":/lib/copy16_nohead.png"), tr("Copy selection to clipboard (for Excel ...) without herader rows/columns"), this);
    connect(actCopyResults, SIGNAL(triggered()), tvMain, SLOT(copySelectionToExcel()));
    connect(actCopyResultsNoHead, SIGNAL(triggered()), tvMain, SLOT(copySelectionToExcelNoHead()));


    actClear=new QAction(QIcon(":/table/table_clear.png"), "clear", this);
    actClear->setToolTip(tr("Clear table ..."));
    connect(actClear, SIGNAL(triggered()), this, SLOT(slClear()));
    connect(this, SIGNAL(enableActions(bool)), actClear, SLOT(setEnabled(bool)));



    actResize=new QAction(QIcon(":/table/table_resize.png"), "resize table", this);
    actResize->setToolTip(tr("Resize the table to a new size"));
    actResize->setShortcut(tr("Ctrl+R"));
    connect(actResize, SIGNAL(triggered()), this, SLOT(slResize()));
    connect(this, SIGNAL(enableActions(bool)), actResize, SLOT(setEnabled(bool)));

    actInsertRow=new QAction(QIcon(":/table/row_insert.png"), "insert row", this);
    actInsertRow->setToolTip(tr("Insert a row above of the currently selected cell ..."));
    connect(actInsertRow, SIGNAL(triggered()), this, SLOT(slInsertRow()));
    connect(this, SIGNAL(enableActions(bool)), actInsertRow, SLOT(setEnabled(bool)));

    actAppendRow=new QAction(QIcon(":/table/row_append.png"), "append row", this);
    actAppendRow->setToolTip(tr("Append a row to the table ..."));
    actAppendRow->setShortcut(tr("Ctrl++"));
    connect(actAppendRow, SIGNAL(triggered()), this, SLOT(slAppendRow()));
    connect(this, SIGNAL(enableActions(bool)), actAppendRow, SLOT(setEnabled(bool)));

    actDeleteRow=new QAction(QIcon(":/table/row_delete.png"), "delete row", this);
    actDeleteRow->setToolTip(tr("Delete the currenty selected row(s) ..."));
    connect(actDeleteRow, SIGNAL(triggered()), this, SLOT(slDeleteRow()));
    connect(this, SIGNAL(enableActions(bool)), actDeleteRow, SLOT(setEnabled(bool)));

    actInsertColumn=new QAction(QIcon(":/table/column_insert.png"), "insert column", this);
    actInsertColumn->setToolTip(tr("Insert a column left of the currently selected cell ..."));
    connect(actInsertColumn, SIGNAL(triggered()), this, SLOT(slInsertColumn()));
    connect(this, SIGNAL(enableActions(bool)), actInsertColumn, SLOT(setEnabled(bool)));

    actAppendColumn=new QAction(QIcon(":/table/column_append.png"), "append column", this);
    actAppendColumn->setToolTip(tr("Append a column to the table ..."));
    actAppendColumn->setShortcut(tr("Ctrl+I"));
    connect(actAppendColumn, SIGNAL(triggered()), this, SLOT(slAppendColumn()));
    connect(this, SIGNAL(enableActions(bool)), actAppendColumn, SLOT(setEnabled(bool)));

    actDeleteColumn=new QAction(QIcon(":/table/column_delete.png"), "delete column", this);
    actDeleteColumn->setToolTip(tr("Delete the currenty selected column(s) ..."));
    connect(actDeleteColumn, SIGNAL(triggered()), this, SLOT(slDeleteColumn()));
    connect(this, SIGNAL(enableActions(bool)), actDeleteColumn, SLOT(setEnabled(bool)));

    actDelete=new QAction(QIcon(":/table/cell_clear.png"), "delete contents", this);
    actDelete->setToolTip(tr("Delete contents from selected cells ..."));
    actDelete->setShortcut(QKeySequence::Delete);
    connect(actDelete, SIGNAL(triggered()), this, SLOT(slDelete()));
    connect(this, SIGNAL(enableActions(bool)), actDelete, SLOT(setEnabled(bool)));

    actSetDatatype=new QAction(QIcon(":/table/cell_type.png"), "set datatype", this);
    actSetDatatype->setToolTip(tr("set the datatype of the currently selected cells ..."));
    actSetDatatype->setShortcut(tr("Ctrl+T"));
    connect(actSetDatatype, SIGNAL(triggered()), this, SLOT(slSetDatatype()));
    connect(this, SIGNAL(enableActions(bool)), actSetDatatype, SLOT(setEnabled(bool)));

    actSetColumnTitle=new QAction(QIcon(":/table/column_title.png"), "edit column properties", this);
    actSetColumnTitle->setToolTip(tr("edit the column title and the clumn expression ..."));
    connect(actSetColumnTitle, SIGNAL(triggered()), this, SLOT(slEditColumnProperties()));
    connect(this, SIGNAL(enableActions(bool)), actSetColumnTitle, SLOT(setEnabled(bool)));


    actSetColumnValues=new QAction("set column values (linear, ...)", this);
    actSetColumnValues->setToolTip(tr("init the current column with e.g. linearly increasing numbers ..."));
    connect(actSetColumnValues, SIGNAL(triggered()), this, SLOT(slSetColumnValues()));
    connect(this, SIGNAL(enableActions(bool)), actSetColumnValues, SLOT(setEnabled(bool)));

    actCalculateColumn=new QAction(QIcon(":/table/formula.png"), tr("evaluate math expression"), this);
    actCalculateColumn->setToolTip(tr("set the value of the selected columns by a freely defineable mathematical expression"));
    actCalculateColumn->setShortcut(tr("="));
    connect(actCalculateColumn, SIGNAL(triggered()), this, SLOT(slCalcColumn()));
    connect(this, SIGNAL(enableActions(bool)), actCalculateColumn, SLOT(setEnabled(bool)));

    actClearExpression=new QAction(QIcon(":/table/formulaclear.png"), tr("clear math expression"), this);
    actClearExpression->setToolTip(tr("clear any expressions from the selected cells"));
    connect(actClearExpression, SIGNAL(triggered()), this, SLOT(slClearExpression()));
    connect(this, SIGNAL(enableActions(bool)), actCalculateColumn, SLOT(setEnabled(bool)));

    actRecalcAll=new QAction(QIcon(":/table/formularecalc.png"), tr("reevaluate all/selected math expression"), this);
    actRecalcAll->setToolTip(tr("reevaluate all math expressions ..."));
    connect(actRecalcAll, SIGNAL(triggered()), this, SLOT(slRecalcAll()));
    connect(this, SIGNAL(enableActions(bool)), actRecalcAll, SLOT(setEnabled(bool)));

    actHistogram=new QAction(QIcon(":/table/histogram.png"), tr("calculate histogramn"), this);
    actHistogram->setToolTip(tr("calculate and insert histograms of the selected columns"));
    actHistogram->setVisible(true);
    connect(actHistogram, SIGNAL(triggered()), this, SLOT(slHistogram()));
    connect(this, SIGNAL(enableActions(bool)), actHistogram, SLOT(setEnabled(bool)));

    actHistogram2D=new QAction(tr("calculate 2D histogramn"), this);
    actHistogram2D->setToolTip(tr("calculate and insert 2D histograms of the selected columns"));
    actHistogram2D->setVisible(true);
    connect(actHistogram2D, SIGNAL(triggered()), this, SLOT(slHistogram2D()));
    connect(this, SIGNAL(enableActions(bool)), actHistogram2D, SLOT(setEnabled(bool)));

    actSort=new QAction(QIcon(":/table/sort_inc.png"), tr("sort selected cells"), this);
    actSort->setToolTip(tr("sort the selected cells"));
    connect(actSort, SIGNAL(triggered()), this, SLOT(slSort()));
    connect(this, SIGNAL(enableActions(bool)), actSort, SLOT(setEnabled(bool)));



    tbMain->addAction(actLoadTable);
    tbMain->addAction(actSaveTable);
    tbMain->addSeparator();
    tbMain->addAction(tvMain->getActPrint());
    tbMain->addSeparator();
    tbMain->addAction(actCopy);
    tbMain->addAction(actCut);
    tbMain->addAction(actPaste);
    tbMain->addSeparator();
    tbMain->addAction(actResize);
    tbMain->addAction(actInsertRow);
    tbMain->addAction(actAppendRow);
    tbMain->addAction(actDeleteRow);
    tbMain->addAction(actInsertColumn);
    tbMain->addAction(actAppendColumn);
    tbMain->addAction(actDeleteColumn);
    tbMain->addSeparator();
    tbMain->addAction(actDelete);
    tbMain->addSeparator();
    tbMain->addAction(actSetDatatype);
    tbMain->addAction(actSetColumnTitle);
    tbMain->addSeparator();
    tbMain->addAction(actCalculateColumn);
    tbMain->addAction(actClearExpression);
    tbMain->addAction(actRecalcAll);
    tbMain->addAction(actSort);



    tvMain->addAction(actCopy);
    tvMain->addAction(actCopyResults);
    tvMain->addAction(actCopyResultsNoHead);
    tvMain->addAction(actCut);
    tvMain->addAction(actPaste);
    tvMain->addAction(getSeparatorAction(this));
    tvMain->addAction(actDelete);
    tvMain->addAction(actDeleteRow);
    tvMain->addAction(actDeleteColumn);
    tvMain->addAction(getSeparatorAction(this));
    tvMain->addAction(actInsertRow);
    tvMain->addAction(actAppendRow);
    tvMain->addAction(actInsertColumn);
    tvMain->addAction(actAppendColumn);
    tvMain->addAction(actDeleteColumn);
    tvMain->addAction(getSeparatorAction(this));
    tvMain->addAction(actSetColumnTitle);
    tvMain->addAction(actSetDatatype);
    tvMain->addAction(getSeparatorAction(this));
    tvMain->addAction(actSetColumnValues);
    tvMain->addAction(actCalculateColumn);
    tvMain->addAction(actClearExpression);
    tvMain->addAction(actRecalcAll);
    tvMain->addAction(actHistogram);
    tvMain->addAction(actHistogram2D);
    tvMain->addAction(actSort);

    l->addWidget(tvMain);

    propertyEditor->setMenuBarVisible(true);
    QMenu* menuFile=propertyEditor->addMenu("&File", 0);
    menuFile->addAction(actLoadTable);
    menuFile->addAction(actSaveTable);
    menuFile->addSeparator();
    menuFile->addAction(tvMain->getActPrint());

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
    menuEdit->addAction(actClearExpression);
    menuEdit->addSeparator();
    menuEdit->addAction(actSetDatatype);
    menuEdit->addAction(actCalculateColumn);

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
    menuTab->addAction(actSetColumnValues);
    menuTab->addAction(actCalculateColumn);
    menuTab->addAction(actClearExpression);
    menuTab->addAction(actRecalcAll);
    menuTab->addAction(actHistogram);
    menuTab->addAction(actHistogram2D);
    menuTab->addAction(actSort);
    menuTab->addSeparator();
    menuTab->addAction(actClear);
    menuTab->addAction(actResize);


}

void QFRDRTableEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        QFRDRTable* m=qobject_cast<QFRDRTable*>(old);
        if (m && m->model()) {
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), this, SLOT(setActionsEnabled(bool)));
            disconnect(tvMain->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(slEditColumnProperties(int)));
        }
    }
    //std::cout<<"qobject_cast ... ";
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    //std::cout<<m<<" ... ";
    //if (m) std::cout<<m->model()<<" ... ";
    if (m && m->model()) {
        tvMain->setModel(m->model());
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), this, SLOT(setActionsEnabled(bool)));
        connect(tvMain->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(slEditColumnProperties(int)));
        m->model()->setReadonly(m->model()->isReadonly());
        setActionsEnabled(!m->model()->isReadonly());
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
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                int f=filter.split(";;").indexOf(selectedFilter);
                //std::cout<<"selectedFilter: "<<selectedFilter.toStdString()<<"   "<<m->getExportFiletypes().at(f).toStdString()<<std::endl;
                m->exportData(m->getExportFiletypes().at(f), fileName);
            }
            QApplication::restoreOverrideCursor();
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
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
                QApplication::restoreOverrideCursor();
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
                 QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                 m->model()->clear();
                 QApplication::restoreOverrideCursor();
             }
        }
    }
}


void QFRDRTableEditor::slAppendRow() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->appendRow();
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFRDRTableEditor::slAppendColumn() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->appendColumn();
            QApplication::restoreOverrideCursor();
        }
    }
}


void QFRDRTableEditor::slInsertRow() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
            QApplication::restoreOverrideCursor();
        }
    }
}


void QFRDRTableEditor::slInsertColumn() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFRDRTableEditor::slDeleteRow() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
                    m->model()->disableSignals();
                    for (int i=rl.size()-1; i>=0; i--) {
                        //std::cout<<"deleting row "<<rl[i]<<std::endl;
                        m->model()->deleteRow(rl[i]);
                    }
                    m->model()->enableSignals(true);
                }
            }
            QApplication::restoreOverrideCursor();
        }
    }
}


void QFRDRTableEditor::slDeleteColumn() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
                    QModelIndex cur=tvMain->currentIndex();
                    m->model()->disableSignals();
                    for (int i=rl.size()-1; i>=0; i--) {
                        //std::cout<<"deleting column "<<rl[i]<<std::endl;
                        m->model()->deleteColumn(rl[i]);
                    }
                    m->model()->enableSignals(true);
                    tvMain->selectionModel()->setCurrentIndex(cur, QItemSelectionModel::SelectCurrent);
                }
            }
            QApplication::restoreOverrideCursor();
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
                    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                    QVariant::Type t=QVariant::String;
                    switch (items.indexOf(item)) {
                        case 0: t=QVariant::String; break;
                        case 1: t=QVariant::Double; break;
                        case 2: t=QVariant::LongLong; break;
                        case 3: t=QVariant::DateTime; break;
                        case 4: t=QVariant::Bool; break;
                    }

                    QItemSelection sel=tvMain->selectionModel()->selection();
                    QModelIndex cur=tvMain->selectionModel()->currentIndex();
                    m->model()->disableSignals();
                    for (int i=0; i<l.size(); i++) {
                        m->model()->changeDatatype(l[i].row(), l[i].column(), t);
                    }
                    m->model()->enableSignals(true);
                    tvMain->selectionModel()->select(sel, QItemSelectionModel::Select);
                    tvMain->selectionModel()->setCurrentIndex(cur, QItemSelectionModel::Current);
                    QApplication::restoreOverrideCursor();
                }
            }
        }
    }
}

void QFRDRTableEditor::slEditColumnProperties(int col) {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (sm->hasSelection()) {
                quint16 c=sm->currentIndex().column();
                if (col>=0) c=col;
                QString t=m->model()->columnTitle(c);
                QVariant comment=m->model()->getColumnHeaderData(c, QFRDRTable::ColumnCommentRole);
                QVariant imwid=m->model()->getColumnHeaderData(c, QFRDRTable::ColumnImageWidth);
                QVariant exp=m->model()->getColumnHeaderData(c, QFRDRTable::ColumnExpressionRole);

                QFRDRTableColumnEditor* edt=new QFRDRTableColumnEditor(this);
                edt->setColumnTitle(t);
                edt->setComment(comment.toString());
                edt->setImageWidth(imwid.toInt());
                edt->setExpression(exp.isValid() && exp.canConvert(QVariant::String), exp.toString());

                if (edt->exec()) {
                     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                     m->model()->setColumnTitle(c, edt->getColumnTitle());
                     m->model()->setColumnHeaderData(c, QFRDRTable::ColumnCommentRole, edt->getComment());
                     m->model()->setColumnHeaderData(c, QFRDRTable::ColumnImageWidth, edt->getImageWidth());
                     if (edt->getExpressionEnabled()) {
                         m->model()->setColumnHeaderData(c, QFRDRTable::ColumnExpressionRole, edt->getExpression());
                         slRecalcAll();
                     } else {
                         m->model()->setColumnHeaderData(c, QFRDRTable::ColumnExpressionRole, QVariant());
                     }
                     QApplication::restoreOverrideCursor();
                 }
                delete edt;
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
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                m->model()->resize(dlg->newHeight(), dlg->newWidth());
                QApplication::restoreOverrideCursor();
            }
            delete dlg;
        }
    }
}

void QFRDRTableEditor::slCopy() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->copy(tvMain->selectionModel()->selectedIndexes());
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFRDRTableEditor::slPaste() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QModelIndex c=tvMain->selectionModel()->currentIndex();
            m->model()->paste(c.row(), c.column());
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFRDRTableEditor::slCut() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    slCopy();
    slDelete();
    QApplication::restoreOverrideCursor();
}

void QFRDRTableEditor::slDelete() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->disableSignals();
            m->model()->deleteCells(tvMain->selectionModel()->selectedIndexes());
            m->model()->enableSignals(true);
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFRDRTableEditor::slSetColumnValues() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QModelIndex ci=tvMain->currentIndex();
            if (ci.isValid()) {
                QFRDRTableColumnValuesDialog* dlg=new QFRDRTableColumnValuesDialog(this);
                QItemSelectionModel* smod=tvMain->selectionModel();
                if (smod->hasSelection()) {
                    smod->selectedColumns();
                    bool set=false;
                    for (int c=0; c<m->model()->columnCount(); c++) {
                        if (smod->isColumnSelected(c, QModelIndex())) {
                            dlg->setColumn(true);
                            dlg->setCount(m->model()->rowCount());
                            set=true;
                            break;
                        }
                    }
                    if (!set) {
                        for (int c=0; c<m->model()->rowCount(); c++) {
                            if (smod->isRowSelected(c, QModelIndex())) {
                                dlg->setColumn(false);
                                dlg->setCount(m->model()->columnCount());
                                set=true;
                                break;
                            }
                        }

                    }
                }

                if (dlg->exec()) {
                    QList<double> val=dlg->getValues();
                    if (val.size()>0) {
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                        QModelIndex cur=tvMain->selectionModel()->currentIndex();
                        m->model()->disableSignals();
                        if (dlg->addAsColumn()) {
                            for (int i=0; i<val.size(); i++) {
                                m->model()->setCellCreate(ci.row()+i, ci.column(), val[i]);
                            }
                        } else {
                            for (int i=0; i<val.size(); i++) {
                                m->model()->setCellCreate(ci.row(), ci.column()+i, val[i]);
                            }
                        }
                        m->model()->enableSignals(true);
                        tvMain->setCurrentIndex(cur);
                        QApplication::restoreOverrideCursor();
                    }
                }
                delete dlg;
            }
        }
    }
}


void QFRDRTableEditor::slCalcColumn() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QModelIndex ci=tvMain->currentIndex();
            if (ci.isValid()) {
                if (!dlgMathExpression) dlgMathExpression=new QFRDRTableFormulaDialog(this);
                QItemSelectionModel* smod=tvMain->selectionModel();
                if (smod->hasSelection()) {
                    QModelIndexList idxs=smod->selectedIndexes();
                    QString exp=dlgMathExpression->getExpression();
                    bool first=true;
                    for (int i=0; i<idxs.size(); i++) {
                        QString lexp=m->model()->cellUserRole(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column()).toString();
                        if (!lexp.isEmpty()) {
                            if (first) {
                                exp=lexp;
                                break;
                            }
                        }
                    }
                    dlgMathExpression->setExpression(exp);
                    if (dlgMathExpression->exec()) {
                        if (idxs.size()>0) {
                            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                            QItemSelection sel=tvMain->selectionModel()->selection();
                            QModelIndex cur=tvMain->selectionModel()->currentIndex();
                            m->model()->disableSignals();


                            bool ok=true;
                            QFMathParser mp; // instanciate
                            addQFRDRTableFunctions(&mp);
                            mp.addVariableDouble("row", 1);
                            mp.addVariableDouble("col", 1);
                            mp.addVariableDouble("column", 1);
                            mp.addVariableDouble("columns", 1.0);
                            mp.addVariableDouble("rows", 1.0);


                            if (dlgMathExpression->getExpression().isEmpty())  {
                                for (int i=0; i<idxs.size(); i++) {
                                    m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column(), "");
                                }
                            } else {
                                QFMathParser::qfmpNode* n=NULL;
                                mp.resetErrors();
                                n=mp.parse(dlgMathExpression->getExpression());
                                if (mp.hasErrorOccured()) {
                                    QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while parsing the expression '%1':\n%2").arg(dlgMathExpression->getExpression()).arg(mp.getLastError()));
                                    ok=false;
                                }

                                if (ok && n) {
                                    for (int i=0; i<idxs.size(); i++) {
                                        ok=true;


                                        QVariant nv=evaluateExpression(mp, n, idxs[i], &ok, exp);
                                        if (ok) {
                                            m->model()->setCellCreate(idxs[i].row(), idxs[i].column(), nv);
                                        } else {
                                            m->model()->setCellCreate(idxs[i].row(), idxs[i].column(), QVariant());
                                        }
                                        m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column(), dlgMathExpression->getExpression());
                                        if (!ok) break;

                                    }
                                }
                                if (n) delete n;
                            }



                            m->model()->enableSignals(true);
                            tvMain->selectionModel()->select(sel, QItemSelectionModel::Select);
                            tvMain->selectionModel()->setCurrentIndex(cur, QItemSelectionModel::Current);
                            QApplication::restoreOverrideCursor();
                        }
                    }
                }

            }
        }
    }
}

void QFRDRTableEditor::slClearExpression()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* smod=tvMain->selectionModel();
            if (smod && smod->hasSelection()) {
                int answer = QMessageBox::question(this, tr("Clear Table Expressions"), tr("Are you sure that you want to clear all expressions from the selected table cells?"), QMessageBox::Yes | QMessageBox::No);
                if (answer == QMessageBox::Yes) {
                    QModelIndexList idxs=smod->selectedIndexes();
                    for (int i=0; i<idxs.size(); i++) {
                        m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column(), QVariant());
                    }
                }
            }
        }
    }

}

QVariant QFRDRTableEditor::evaluateExpression(QFMathParser& mp, QFMathParser::qfmpNode *n, QModelIndex cell, bool* ok, const QString& expression, bool columnMode)
{
   QVariant result;
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    //int row = cell.row();
    //int column = cell.column();
    if (m) {
        QString error="";
        bool rOK=true;
        mp.resetErrors();
        result=m->evaluateExpression(mp, n, cell, &rOK, expression, &error, columnMode);
        if (!rOK) {
            *ok= QMessageBox::critical(this, tr("QuickFit-table"), error,
                                      QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Ok)==QMessageBox::Ok;
            return QVariant();
        }
    }
    return result;


}

void QFRDRTableEditor::editExpression(const QModelIndex &index)
{
    tvMain->setCurrentIndex(index);
    slCalcColumn();
}

void QFRDRTableEditor::slRecalcAll()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QModelIndex cur=tvMain->currentIndex();
            QItemSelectionModel* smod=tvMain->selectionModel();
            QItemSelection sel=smod->selection();
            if (!smod->hasSelection()) {
                tvMain->selectAll();
            }
            QModelIndexList idxs=smod->selectedIndexes();

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->disableSignals();


            bool ok=true;
            QFMathParser mp, mpColumns; // instanciate
            addQFRDRTableFunctions(&mp);
            addQFRDRTableFunctions(&mpColumns, NULL, true);
            mp.addVariableDouble("row", 1);
            mp.addVariableDouble("col", 1);
            mp.addVariableDouble("column", 1);
            mp.addVariableDouble("columns", 1.0);
            mp.addVariableDouble("rows", 1.0);

            mpColumns.addVariableDouble("col", 1);
            mpColumns.addVariableDouble("columns", 1.0);
            mpColumns.addVariableDouble("rows", 1.0);

            QMap<QString, QFMathParser::qfmpNode*> nodes;
            QMap<QString, QFMathParser::qfmpNode*> cnodes;
            int changes=1;
            int iterations=0;
            int maxIterations=20;
            while (iterations<maxIterations && changes>0) {
                changes=0;

                // evaluate column expressions
                for (int i=0; i<m->model()->columnCount(); i++) {
                    QString lexp=m->model()->getColumnHeaderData(i, QFRDRTable::ColumnExpressionRole).toString();
                    if (!lexp.isEmpty()) {
                        QVariantList ov=m->model()->getColumnData(i, Qt::DisplayRole);

                        //qDebug()<<"     reeval0 col"<<i<<": "<<lexp<<"\n              "<<ov.size();
                        if (!cnodes.contains(lexp)) {

                            cnodes[lexp]=mpColumns.parse(lexp);
                            //qDebug()<<"     reeval1 col"<<i<<": parsing "<<lexp<<cnodes[lexp];
                            if (mp.hasErrorOccured()) {
                                QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while parsing the expression '%1':\n%2").arg(lexp).arg(mpColumns.getLastError()));
                                ok=false;
                            }
                        }
                        if (ok) {
                            //qDebug()<<"     reeval2 col"<<i<<": "<<cnodes[lexp]<<ok;
                            ok=(cnodes[lexp]!=NULL);
                            //qDebug()<<"     reeval3 col"<<i<<": "<<cnodes[lexp]<<ok;
                            if (ok) {
                                //qDebug()<<"     reeval4 col("<<i<<ov.size()<<")";
                                mpColumns.resetErrors();
                                QVariantList nv=evaluateExpression(mpColumns, cnodes[lexp], m->model()->index(0,i), &ok, lexp, true).toList();
                                //qDebug()<<"     reeval5 col("<<i<<n.size()<<")\n        <= "<<ok<<nv.size();
                                bool equalWithVariant=true;
                                //qDebug()<<"nv = "<<nv;
                                //qDebug()<<"ov = "<<ov;
                                for (int in=0; in<qMax(nv.size(),ov.size()); in++) {
                                    if (in<nv.size() && in<ov.size()) {
                                        //qDebug()<<nv[in]<<ov[in]<<nv[in].isValid()<<ov[in].isValid();
                                        if (nv[in]!=ov[in]) {
                                            equalWithVariant=false;
                                            //qDebug()<<"  unequal idx "<<i<<nv[i]<<ov[i];
                                            break;
                                        } else if (nv[in].isValid() && !ov[in].isValid()) {
                                            equalWithVariant=false;
                                            //qDebug()<<"  unequal idx "<<i<<nv[i]<<ov[i];
                                            break;
                                        }

                                    } else {
                                        if (in>=nv.size() && ov[in].isValid()) {
                                            //qDebug()<<"  unequal idx "<<i<<" no_nv "<<ov[i];
                                            equalWithVariant=false;
                                            break;
                                        }
                                    }
                                }
                                //qDebug()<<ok<<ov.size()<<nv.size()<<equalWithVariant;
                                if (ok && !equalWithVariant) {
                                    changes++;
                                    for (int r=0; r<qMax(nv.size(), m->model()->rowCount()); r++) {
                                        m->model()->setCellCreate(r, i, nv.value(r, QVariant()));
                                        m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, r, i, QVariant());
                                    }
                                }
                            }
                            //qDebug()<<"     reeval6 col"<<i<<ok;
                        }
                        if (!ok) break;
                    }
                }

                // evaluate cell expressions
                for (int i=0; i<idxs.size(); i++) {
                    QString lexp=m->model()->cellUserRole(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column()).toString();
                    if (!lexp.isEmpty()) {
                        QVariant ov=m->model()->cell(idxs[i].row(), idxs[i].column());
                        //qDebug()<<"     reeval "<<idxs[i].row()<<idxs[i].column()<<lexp;
                        if (!nodes.contains(lexp)) {

                            nodes[lexp]=mp.parse(lexp);
                            if (mp.hasErrorOccured()) {
                                QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while parsing the expression '%1':\n%2").arg(lexp).arg(mp.getLastError()));
                                ok=false;
                            }
                        }
                        if (ok) {
                            //qDebug()<<"     reeval "<<idxs[i].row()<<idxs[i].column()<<nodes[lexp]<<ok;
                            ok=nodes[lexp];
                            if (ok) {
                                mp.resetErrors();
                                QVariant nv=evaluateExpression(mp, nodes[lexp], idxs[i], &ok, lexp);
                                //qDebug()<<"     reeval("<<idxs[i].row()<<idxs[i].column()<<ov<<") <= "<<nv;
                                if (ok && ov!=nv) {
                                    changes++;
                                    m->model()->setCellCreate(idxs[i].row(), idxs[i].column(), nv);
                                    m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column(), lexp);
                                }
                            }
                        }
                        if (!ok) break;
                    }
                }
                if (!ok) break;
                iterations++;
                //qDebug()<<"** reeval: "<<iterations<<changes;
            }
            if (iterations>=maxIterations) {
                QMessageBox::critical(this, tr("QuickFit-table"), tr("Stopped reevaluating expressions after %1 iterations!\n  Changes were detected after %1 iterations, this might point to circular references in expression.\n  So results might not be reliable, rerun!").arg(maxIterations));
            }

            QMapIterator<QString, QFMathParser::qfmpNode*> it(nodes);
            while (it.hasNext()) {
                it.next();
                if (it.value()) delete it.value();
            }
            nodes.clear();

            m->model()->enableSignals(true);
            tvMain->selectionModel()->select(sel, QItemSelectionModel::Select);
            tvMain->selectionModel()->setCurrentIndex(cur, QItemSelectionModel::Current);
            QApplication::restoreOverrideCursor();
        }
    }
}

bool QFRDRTableEditorLessThan(const QPair<int, QVariant> &s1, const QPair<int, QVariant> &s2) {
    //qDebug()<<s1.second<<" < "<<s2.second<<"    strcmp: "<<(s1.second.toString().toLower() < s2.second.toString().toLower());

    if (s1.second.type()==QVariant::String && s2.second.type()==QVariant::String) {
        return s1.second.toString().toLower() < s2.second.toString().toLower();
    }
    if (s1.second.canConvert(QVariant::Double) && s2.second.canConvert(QVariant::Double)) {
        return s1.second.toDouble() < s2.second.toDouble();
    }
    if (s1.second.canConvert(QVariant::DateTime) && s2.second.canConvert(QVariant::DateTime)) {
        return s1.second.toDateTime() < s2.second.toDateTime();
    }
    return s1.second.toString().toLower() < s2.second.toString().toLower();
}

bool QFRDRTableEditorGreaterThan(const QPair<int, QVariant> &s1, const QPair<int, QVariant> &s2) {
    //qDebug()<<s1.second<<" > "<<s2.second<<"    strcmp: "<<(s1.second.toString().toLower() > s2.second.toString().toLower());
    if (s1.second.type()==QVariant::String && s2.second.type()==QVariant::String) {
        return s1.second.toString().toLower() > s2.second.toString().toLower();
    }
    if (s1.second.canConvert(QVariant::Double) && s2.second.canConvert(QVariant::Double)) {
        return s1.second.toDouble() > s2.second.toDouble();
    }
    if (s1.second.canConvert(QVariant::DateTime) && s2.second.canConvert(QVariant::DateTime)) {
        return s1.second.toDateTime() > s2.second.toDateTime();
    }
    return s1.second.toString().toLower() > s2.second.toString().toLower();
}


void QFRDRTableEditor::slSort() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QList<QPair<int, QString> > cols;
            QModelIndex ci=tvMain->currentIndex();
            if (ci.isValid()) {
                QFRDRTableSortDialog* dlg=new QFRDRTableSortDialog(this);
                QItemSelectionModel* smod=tvMain->selectionModel();
                if (smod->hasSelection()) {
                    QModelIndexList idxs=smod->selectedIndexes();
                    QList<QPair<int, QVariant> > rows;
                    int firstcol=0;
                    bool first=true;
                    QList<QVariant> idxData;
                    for (int i=0; i<idxs.size(); i++) {
                        if (first) {
                            firstcol=idxs[i].column();
                            first=false;
                        } else {
                            if (idxs[i].column()<firstcol) firstcol=idxs[i].column();
                        }
                        if (!rows.contains(qMakePair(idxs[i].row(), QVariant()))) rows.append(qMakePair(idxs[i].row(), QVariant()));
                        if (!cols.contains(qMakePair(idxs[i].column(), m->model()->headerData(idxs[i].column(), Qt::Horizontal).toString()))) cols.append(qMakePair(idxs[i].column(), m->model()->headerData(idxs[i].column(), Qt::Horizontal).toString()));
                        idxData.append(idxs[i].data());
                    }
                    dlg->setColumns(cols);
                    dlg->setCurrentColumn(firstcol);

                    if (dlg->exec()) {
                        if (idxs.size()>0) {
                            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                            int sortcol=dlg->sortColumn();
                            bool ascending=dlg->sortAscending();
                            //qDebug()<<"col: "<<sortcol<<"   ascending: "<<ascending;

                            for (int i=0; i<rows.size(); i++) {
                                rows[i].second=m->model()->cell(rows[i].first, sortcol);
                            }

                            QMap<int, int> rowmap;
                            if (ascending) {
                                qStableSort(rows.begin(), rows.end(), QFRDRTableEditorLessThan);
                            } else {
                                qStableSort(rows.begin(), rows.end(), QFRDRTableEditorGreaterThan);
                            }
                            for (int r=0; r<rows.size(); r++) {
                                rowmap[rows[r].first]=r;
                            }

                            QItemSelection sel=tvMain->selectionModel()->selection();
                            QModelIndex cur=tvMain->selectionModel()->currentIndex();
                            m->model()->disableSignals();

                            for (int i=0; i<idxs.size(); i++) {
                                m->model()->deleteCell(idxs[i].row(), idxs[i].column());
                            }

                            for (int i=0; i<idxs.size(); i++) {
                                m->model()->setCell(rowmap[idxs[i].row()], idxs[i].column(), idxData[i]);
                            }


                            m->model()->enableSignals(true);
                            tvMain->selectionModel()->select(sel, QItemSelectionModel::Select);
                            tvMain->selectionModel()->setCurrentIndex(cur, QItemSelectionModel::Current);
                            QApplication::restoreOverrideCursor();
                        }
                    }
                }

            }
        }
    }
}

void QFRDRTableEditor::slHistogram()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QMap<int, QList<double> > colData;
            //QMap<int, QString> colNames;
            QItemSelectionModel* smod=tvMain->selectionModel();
            int sel=-1;
            if (smod->hasSelection()) {
                QModelIndexList idxs=smod->selectedIndexes();
                for (int i=0; i<idxs.size(); i++) {
                    QVariant v=idxs[i].data();
                    bool okDouble=false;
                    double dv=v.toDouble(&okDouble);
                    if (v.type()==QVariant::Double || v.type()==QVariant::UInt || v.type()==QVariant::Int || v.type()==QVariant::ULongLong || v.type()==QVariant::LongLong || v.type()==QVariant::Bool || v.type()==QVariant::DateTime || v.type()==QVariant::Char || (v.type()==QVariant::String && okDouble)) {
                        colData[idxs[i].column()].append(dv);
                        //colNames[idxs[i].column()]=m->model()->headerData(idxs[i].column(), Qt::Horizontal).toString();
                    }
                    if (sel<0 && colData[idxs[i].column()].size()>0) sel=idxs[i].column();
                }
                if (colData.size()>0/* && colNames>0*/) {
                    QFRDRTableHistogramDialog* dlg=new QFRDRTableHistogramDialog(this);
                    dlg->setTable(m->model());
                    dlg->setSelectedColumn(sel);
                    dlg->init();
                    if (dlg->exec()) {
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

                        QItemSelection sel=tvMain->selectionModel()->selection();
                        QModelIndex cur=tvMain->selectionModel()->currentIndex();
                        m->model()->disableSignals();

                        QMap<int, QVector<double> > histData=dlg->getHistograms();
                        QMap<int, QString> histNames=dlg->getHeaderNames();

                        QMapIterator<int, QVector<double> > itD(histData);
                        while (itD.hasNext()) {
                            itD.next();
                            if (itD.key()>=m->model()->columnCount()) {
                                while (itD.key()>=m->model()->columnCount()) {
                                    m->model()->appendColumn();
                                }
                            } else {
                                m->model()->emptyColumn(itD.key());
                            }
                        }
                        QMapIterator<int, QVector<double> > itD1(histData);
                        while (itD1.hasNext()) {
                            itD1.next();
                            const QVector<double>& dat=itD1.value();
                            int col=itD1.key();
                            for (int i=0; i<dat.size(); i++) {
                                m->model()->setCellCreate(i, col, dat[i]);
                            }
                        }
                        QMapIterator<int, QString> itN(histNames);
                        while (itN.hasNext()) {
                            itN.next();
                            m->model()->setColumnTitleCreate(itN.key(), itN.value());
                        }



                        m->model()->enableSignals(true);
                        tvMain->selectionModel()->select(sel, QItemSelectionModel::Select);
                        tvMain->selectionModel()->setCurrentIndex(cur, QItemSelectionModel::Current);
                        QApplication::restoreOverrideCursor();
                    }
                    delete dlg;
                }
            }
        }
    }
}

void QFRDRTableEditor::slHistogram2D()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QMap<int, QList<double> > colData;
            //QMap<int, QString> colNames;
            QItemSelectionModel* smod=tvMain->selectionModel();
            int sel=-1;
            int selY=-1;
            if (smod->hasSelection()) {
                QModelIndexList idxs=smod->selectedIndexes();
                for (int i=0; i<idxs.size(); i++) {
                    QVariant v=idxs[i].data();
                    bool okDouble=false;
                    double dv=v.toDouble(&okDouble);
                    if (v.type()==QVariant::Double || v.type()==QVariant::UInt || v.type()==QVariant::Int || v.type()==QVariant::ULongLong || v.type()==QVariant::LongLong || v.type()==QVariant::Bool || v.type()==QVariant::DateTime || v.type()==QVariant::Char || (v.type()==QVariant::String && okDouble)) {
                        colData[idxs[i].column()].append(dv);
                        //colNames[idxs[i].column()]=m->model()->headerData(idxs[i].column(), Qt::Horizontal).toString();
                    }
                    if (sel<0 && colData[idxs[i].column()].size()>0) sel=idxs[i].column();
                    if (selY<0 && sel<idxs[i].column() && colData[idxs[i].column()].size()>0) selY=idxs[i].column();
                }
                if (colData.size()>0/* && colNames>0*/) {
                    QFRDRTable2DHistogramDialog* dlg=new QFRDRTable2DHistogramDialog(this);
                    dlg->setTable(m->model());
                    dlg->setSelectedColumn(sel, selY);
                    dlg->init();
                    if (dlg->exec()) {
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

                        QItemSelection sel=tvMain->selectionModel()->selection();
                        QModelIndex cur=tvMain->selectionModel()->currentIndex();
                        m->model()->disableSignals();

                        QMap<int, QVector<double> > histData=dlg->getHistograms();
                        QMap<int, QVector<QVariant> > histData2=dlg->getHistograms2();
                        QMap<int, QString> histNames=dlg->getHeaderNames();
                        QMap<int, QMap<int, QVariant> > extraData=dlg->getExtraData();

                        QMapIterator<int, QVector<double> > itD(histData);
                        while (itD.hasNext()) {
                            itD.next();
                            if (itD.key()>=m->model()->columnCount()) {
                                while (itD.key()>=m->model()->columnCount()) {
                                    m->model()->appendColumn();
                                }
                            } else {
                                m->model()->emptyColumn(itD.key());
                            }
                        }
                        QMapIterator<int, QVector<QVariant> > itD2(histData2);
                        while (itD2.hasNext()) {
                            itD2.next();
                            if (itD2.key()>=m->model()->columnCount()) {
                                while (itD2.key()>=m->model()->columnCount()) {
                                    m->model()->appendColumn();
                                }
                            } else {
                                m->model()->emptyColumn(itD2.key());
                            }
                        }
                        QMapIterator<int, QVector<double> > itD1(histData);
                        while (itD1.hasNext()) {
                            itD1.next();
                            const QVector<double>& dat=itD1.value();
                            int col=itD1.key();
                            for (int i=0; i<dat.size(); i++) {
                                m->model()->setCellCreate(i, col, dat[i]);
                            }
                        }
                        QMapIterator<int, QVector<QVariant> > itD21(histData2);
                        while (itD21.hasNext()) {
                            itD21.next();
                            const QVector<QVariant>& dat=itD21.value();
                            int col=itD21.key();
                            for (int i=0; i<dat.size(); i++) {
                                m->model()->setCellCreate(i, col, dat[i]);
                            }
                        }
                        QMapIterator<int, QString> itN(histNames);
                        while (itN.hasNext()) {
                            itN.next();
                            m->model()->setColumnTitleCreate(itN.key(), itN.value());
                        }

                        QMapIterator<int, QMap<int, QVariant> > itED(extraData);
                        while (itED.hasNext()) {
                            itED.next();
                            const QMap<int, QVariant>& dat=itED.value();
                            int col=itED.key();
                            QMapIterator<int, QVariant> i(dat);
                            while (i.hasNext()) {
                                i.next();
                                m->model()->setColumnHeaderData(col, i.key(), i.value());
                            }
                        }


                        m->model()->enableSignals(true);
                        tvMain->selectionModel()->select(sel, QItemSelectionModel::Select);
                        tvMain->selectionModel()->setCurrentIndex(cur, QItemSelectionModel::Current);
                        QApplication::restoreOverrideCursor();
                    }
                    delete dlg;
                }
            }
        }
    }
}

void QFRDRTableEditor::setActionsEnabled(bool enabled) {
    emit disableActions(!enabled);
    emit enableActions(enabled);
}

void QFRDRTableEditor::setActionsDisabled(bool disabled)
{
    emit disableActions(disabled);
    emit enableActions(!disabled);
}







