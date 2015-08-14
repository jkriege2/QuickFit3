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


#include "qfrdrtableeditor.h"
#include "qfrdrtablecolumnvaluesdialog.h"
#include "qfrdrtable.h"
#include "qfdlgcsvparameters.h"
#include "qfrdrtablesortdialog.h"
#include <QtAlgorithms>
#include "qfrdrtableparserfunctions.h"
#include "qfrdrtablecolumneditor.h"
#include "qfrdrtablecurvefitdialog.h"
#include "qfrdrtableregressiondialog.h"
#include "qfrdrtable2dhistogramdialog.h"
#include "dlgimporttable.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "qfrdrtablemulticolumneditor.h"
#include "qfrdrtablesetcellsdialog.h"
#include "qfrdrtablecolumnsboxplotdialog.h"

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

void QFRDRTableEditor::requestFit(int xCol, int yCol, int sigmaCol, int plot, int graph, QString /*function*/, QFRDRTable::GraphDataSelection sel, bool xlog, bool ylog)
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        //qDebug()<<"regression cX="<<xCol<<"  cY="<<yCol<<"  cW="<<sigmaCol<<"  plot="<<plot<<"  xlog="<<xlog<<"  ylog="<<ylog;
        QFRDRTableCurveFitDialog* dlg=new QFRDRTableCurveFitDialog(m, xCol, yCol, sigmaCol, sel, this, xlog, ylog, -1, plot, graph);
        if (dlg->exec()) {

        }
        delete dlg;
    }

}

void QFRDRTableEditor::requestRegression(int xCol, int yCol, int sigmaCol, int plot, int graph, QFRDRTable::GraphDataSelection sel, bool xlog, bool ylog)
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        //qDebug()<<"regression cX="<<xCol<<"  cY="<<yCol<<"  cW="<<sigmaCol<<"  plot="<<plot<<"  xlog="<<xlog<<"  ylog="<<ylog;
        QFRDRTableRegressionDialog* dlg=new QFRDRTableRegressionDialog(m, xCol, yCol, sigmaCol, sel, this, xlog, ylog, -1, plot, graph);
        if (dlg->exec()) {

        }
        delete dlg;
    }
}

void QFRDRTableEditor::requestRefit(int plot, int graph)
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        QString type=m->colgraphGetGraphProperty(plot, graph, "FIT_TYPE", "NONE").toString().toUpper().trimmed();
        if (type=="LEAST_SQUARES" || type=="FIT") {
            QFRDRTableCurveFitDialog* dlg=new QFRDRTableCurveFitDialog(m, plot, graph, this);
            if (dlg->exec()) {

            }
            delete dlg;
            m->colgraphSetDoEmitSignals(true);
            //m->emitRebuildPlotWidgets();
        } else if (type=="REGRESSION") {
            QFRDRTableRegressionDialog* dlg=new QFRDRTableRegressionDialog(m, plot, graph, this);
            if (dlg->exec()) {

            }
            delete dlg;
            m->colgraphSetDoEmitSignals(true);
        }
    }
}

void QFRDRTableEditor::createWidgets() {
    QVBoxLayout* l=new QVBoxLayout();
    setLayout(l);

    tbMain=new QToolBar("tbtablemain", this);
    l->addWidget(tbMain);


    tvMain=new QFRDRTableEnhancedTableView(this);
    connect(tvMain, SIGNAL(editExpression(QModelIndex)), this, SLOT(editExpression(QModelIndex)));
    connect(tvMain, SIGNAL(keyPressed(int,Qt::KeyboardModifiers,QString)), this, SLOT(tableKeyPressed(int,Qt::KeyboardModifiers,QString)));
    connect(tvMain, SIGNAL(delPressed()), this, SLOT(slDelete()));
    connect(tvMain, SIGNAL(copyPressed()), this, SLOT(slCopy()));
    l->addWidget(tvMain, 2);

    tabQuick=new QFEnhancedTabWidget(this);
    l->addWidget(tabQuick, 1);
    tabQuick->setVisible(false);

    widPreScript=new QWidget(this);

    QVBoxLayout* layPreScript=new QVBoxLayout();
    QHBoxLayout* layPreScriptV=new QHBoxLayout();
    widPreScript->setLayout(layPreScriptV);
    edtPreScript=new QFEnhancedPlainTextEdit(this);
    labPreScriptOK=new QLabel(this);
    //labPreScriptTemplate=new QLabel(this);
    //labPreScriptTemplate->setWordWrap(true);
    labPreScriptHelp=new QLabel(this);
    labPreScriptHelp->setWordWrap(true);
    labPreScriptHelp->setTextFormat(Qt::RichText);
    scrollPreScriptHelp=new QScrollArea(this);
    scrollPreScriptHelp->setWidget(new QWidget());
    scrollPreScriptHelp->widget()->setLayout(layPreScript);
    layPreScript->addWidget(labPreScriptOK);
    btnPreScriptParserFHelp=new QPushButton(QIcon(":/lib/help.png"), tr("function &help"), this);
    btnPreScriptParserHelp=new QPushButton(QIcon(":/lib/help.png"), tr("parser &help"), this);
    QHBoxLayout* layScriptHelpButtons=new QHBoxLayout();
    layScriptHelpButtons->addWidget(btnPreScriptParserFHelp);
    layScriptHelpButtons->addWidget(btnPreScriptParserHelp);
    layPreScript->addLayout(layScriptHelpButtons);
    //layPreScript->addWidget(labPreScriptTemplate);
    layPreScript->addWidget(labPreScriptHelp, 1);
    scrollPreScriptHelp->setWidgetResizable(true);
    functionRef=new QFFunctionReferenceTool(NULL);
    functionRef->setCompleterFile(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/table_expression.txt");
    functionRef->setDefaultHelp(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+QString("/mathparser.html"));
    functionRef->setDefaultWordsMathExpression();
    functionRef->registerEditor(edtPreScript);
    functionRef->setLabHelp(labPreScriptHelp);
    functionRef->setCurrentHelpButton(btnPreScriptParserHelp);
    connect(btnPreScriptParserHelp, SIGNAL(clicked()), functionRef, SLOT(showDefaultHelp()));
    //functionRef->setLabTemplate(labPreScriptTemplate);
    functionRef->setDefaultHelp(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");
    layPreScriptV->addWidget(edtPreScript, 3);
    layPreScriptV->addWidget(scrollPreScriptHelp, 1);

    tabQuick->addTab(widPreScript, tr("Preevaluated Expressions"));
    tvQuickStat=new QEnhancedTableView(this);
    tmQuickStat=new QFTableModel(this);
    tvQuickStat->setModel(tmQuickStat);
    tabQuick->addTab(tvQuickStat, tr("Quick Statistics"));
    histQuick=new QFHistogramView(this);
    histQuick->setAutorange(true);
    histQuick->setNormalized(true);
    histQuick->setBins(35);
    tabQuick->addTab(histQuick, tr("Quick Histogram"));




    actLoadTable=new QAction(QIcon(":/table/table_open.png"), "load table", this);
    actLoadTable->setToolTip(tr("Load table ..."));
    actLoadTable->setShortcut(QKeySequence::Open);
    connect(actLoadTable, SIGNAL(triggered()), this, SLOT(slLoadTable()));
    connect(this, SIGNAL(enableActions(bool)), actLoadTable, SLOT(setEnabled(bool)));

    actImportTable=new QAction(QIcon(":/table/table_import.png"), "import data from file", this);
    actImportTable->setToolTip(tr("import data from file into the current table ..."));
    connect(actImportTable, SIGNAL(triggered()), this, SLOT(slImportFile()));
    connect(this, SIGNAL(enableActions(bool)), actImportTable, SLOT(setEnabled(bool)));



    actSaveTable=new QAction(QIcon(":/table/table_save.png"), "save table", this);
    actSaveTable->setToolTip(tr("Save table ..."));
    actSaveTable->setShortcut(QKeySequence::SaveAs);
    connect(actSaveTable, SIGNAL(triggered()), this, SLOT(slSaveTable()));

    actMakeEditable=new QAction(tr("make the table editable ..."), this);
    connect(actMakeEditable, SIGNAL(triggered()), this, SLOT(slMakeEditable()));
    connect(this, SIGNAL(enableActions(bool)), actMakeEditable, SLOT(setDisabled(bool)));

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

    actPasteNoEquations=new QAction(QIcon(":/table/paste.png"), tr("Paste without expressions"), this);
    connect(this, SIGNAL(enableActions(bool)), actPasteNoEquations, SLOT(setEnabled(bool)));
    connect(actPasteNoEquations, SIGNAL(triggered()), this, SLOT(slPasteNoEquations()));

    actImportFromClipboard=new QAction(tr("Import from Clipboard (extended Paste)"), this);
    connect(this, SIGNAL(enableActions(bool)), actImportFromClipboard, SLOT(setEnabled(bool)));
    connect(actImportFromClipboard, SIGNAL(triggered()), this, SLOT(slImportFromClipboard()));

    actSaveTableTemplate=new QAction(tr("save table template"), this);
    connect(actSaveTableTemplate, SIGNAL(triggered()), this, SLOT(slSaveTableTemplate()));
    actLoadTableTemplate=new QAction(tr("load table template"), this);
    connect(actLoadTableTemplate, SIGNAL(triggered()), this, SLOT(slLoadTableTemplate()));
    connect(this, SIGNAL(enableActions(bool)), actLoadTableTemplate, SLOT(setEnabled(bool)));
    actCopyTemplate=new QAction(tr("copy selected table column template"), this);
    connect(actCopyTemplate, SIGNAL(triggered()), this, SLOT(slCopyTableTemplate()));
    actPasteTemplate=new QAction(tr("paste selected table column  template"), this);
    connect(actPasteTemplate, SIGNAL(triggered()), this, SLOT(slPasteTableTemplate()));
    connect(this, SIGNAL(enableActions(bool)), actPasteTemplate, SLOT(setEnabled(bool)));

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

    actSetCellsValue=new QAction(tr("set value in cells"), this);
    actSetCellsValue->setToolTip(tr("set a new value in all currently selected cells ..."));
    connect(actSetCellsValue, SIGNAL(triggered()), this, SLOT(slSetCells()));
    connect(this, SIGNAL(enableActions(bool)), actSetCellsValue, SLOT(setEnabled(bool)));

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


    actSetDatatypeString=new QAction(QIcon(":/table/cell_string.png"), tr("... string"), this);
    actSetDatatypeString->setToolTip(tr("set the datatype of the currently selected cells to string"));
    connect(actSetDatatypeString, SIGNAL(triggered()), this, SLOT(slSetDatatypeString()));
    connect(this, SIGNAL(enableActions(bool)), actSetDatatypeString, SLOT(setEnabled(bool)));
    actSetDatatypeInteger=new QAction(QIcon(":/table/cell_integer.png"), tr("... integer"), this);
    actSetDatatypeInteger->setToolTip(tr("set the datatype of the currently selected cells to integer"));
    connect(actSetDatatypeInteger, SIGNAL(triggered()), this, SLOT(slSetDatatypeInteger()));
    connect(this, SIGNAL(enableActions(bool)), actSetDatatypeInteger, SLOT(setEnabled(bool)));
    actSetDatatypeDouble=new QAction(QIcon(":/table/cell_double.png"), tr("... floating-point number"), this);
    actSetDatatypeDouble->setToolTip(tr("set the datatype of the currently selected cells to floating-point number"));
    connect(actSetDatatypeDouble, SIGNAL(triggered()), this, SLOT(slSetDatatypeDouble()));
    connect(this, SIGNAL(enableActions(bool)), actSetDatatypeDouble, SLOT(setEnabled(bool)));
    actSetDatatypeDate=new QAction(QIcon(":/table/cell_date.png"), tr("... date"), this);
    actSetDatatypeDate->setToolTip(tr("set the datatype of the currently selected cells to date"));
    connect(actSetDatatypeDate, SIGNAL(triggered()), this, SLOT(slSetDatatypeDate()));
    connect(this, SIGNAL(enableActions(bool)), actSetDatatypeDate, SLOT(setEnabled(bool)));
    actSetDatatypeBool=new QAction(QIcon(":/table/cell_bool.png"), tr("... boolean"), this);
    actSetDatatypeBool->setToolTip(tr("set the datatype of the currently selected cells to boolean"));
    connect(actSetDatatypeBool, SIGNAL(triggered()), this, SLOT(slSetDatatypeBool()));
    connect(this, SIGNAL(enableActions(bool)), actSetDatatypeBool, SLOT(setEnabled(bool)));




    actSetColumnTitle=new QAction(QIcon(":/table/column_title.png"), tr("edit column properties"), this);
    actSetColumnTitle->setToolTip(tr("edit the column title and the clumn expression ..."));
    connect(actSetColumnTitle, SIGNAL(triggered()), this, SLOT(slEditColumnProperties()));
    connect(this, SIGNAL(enableActions(bool)), actSetColumnTitle, SLOT(setEnabled(bool)));

    actSHowMultiColEditor=new QAction(tr("show multi-column properties editor"), this);
    connect(actSHowMultiColEditor, SIGNAL(triggered()), this, SLOT(slShowMultiColumnEditor()));
    connect(this, SIGNAL(enableActions(bool)), actSHowMultiColEditor, SLOT(setEnabled(bool)));

    actSetColumnValues=new QAction("set column values (linear, ...)", this);
    actSetColumnValues->setToolTip(tr("init the current column with e.g. linearly increasing numbers ..."));
    connect(actSetColumnValues, SIGNAL(triggered()), this, SLOT(slSetColumnValues()));
    connect(this, SIGNAL(enableActions(bool)), actSetColumnValues, SLOT(setEnabled(bool)));

    actCalcCell=new QAction(QIcon(":/table/formula.png"), tr("evaluate math expression"), this);
    actCalcCell->setToolTip(tr("set the value of the selected columns by a freely defineable mathematical expression"));
    actCalcCell->setShortcut(tr("="));
    connect(actCalcCell, SIGNAL(triggered()), this, SLOT(slCalcCell()));
    connect(this, SIGNAL(enableActions(bool)), actCalcCell, SLOT(setEnabled(bool)));

    actClearExpression=new QAction(QIcon(":/table/formulaclear.png"), tr("clear math expression"), this);
    actClearExpression->setToolTip(tr("clear any expressions from the selected cells"));
    connect(actClearExpression, SIGNAL(triggered()), this, SLOT(slClearExpression()));
    connect(this, SIGNAL(enableActions(bool)), actClearExpression, SLOT(setEnabled(bool)));

    actRecalcAll=new QAction(QIcon(":/table/formularecalc.png"), tr("reevaluate all/selected math expression"), this);
    actRecalcAll->setToolTip(tr("reevaluate all math expressions ..."));
    connect(actRecalcAll, SIGNAL(triggered()), this, SLOT(slRecalcAll()));
    connect(this, SIGNAL(enableActions(bool)), actRecalcAll, SLOT(setEnabled(bool)));

    actExpressionSeedBeforeTableEval=new QAction(tr("use single seed for reevaluations"), this);
    actExpressionSeedBeforeTableEval->setCheckable(true);
    actExpressionSeedBeforeTableEval->setChecked(true);
    connect(actExpressionSeedBeforeTableEval, SIGNAL(triggered()), this, SLOT(slExpressionSeedBeforeTableEval()));
    connect(this, SIGNAL(enableActions(bool)), actExpressionSeedBeforeTableEval, SLOT(setEnabled(bool)));

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

    actUndo=new QAction(QIcon(":/lib/undo.png"), tr("undo"), this);
    actUndo->setShortcut(QKeySequence::Undo);
    connect(this, SIGNAL(enableActions(bool)), actUndo, SLOT(setEnabled(bool)));
    actRedo=new QAction(QIcon(":/lib/redo.png"), tr("redo"), this);
    actRedo->setShortcut(QKeySequence::Redo);
    connect(this, SIGNAL(enableActions(bool)), actRedo, SLOT(setEnabled(bool)));

    actQuickStat=new QAction(QIcon(":/table/quickstat.png"), tr("Quick Statistics"), this);
    actQuickStat->setCheckable(true);
    actQuickStat->setChecked(false);
    connect(actQuickStat, SIGNAL(toggled(bool)), this, SLOT(slQuickStat(bool)));

    actPreScript=new QAction(QIcon(":/table/prescript.png"), tr("Preevaluated Expressions"), this);
    actPreScript->setCheckable(true);
    actPreScript->setChecked(false);
    connect(actPreScript, SIGNAL(toggled(bool)), this, SLOT(slPreScript(bool)));

    actQuickHistogram=new QAction(QIcon(":/table/quickhist.png"), tr("Quick Histogram"), this);
    actQuickHistogram->setCheckable(true);
    actQuickHistogram->setChecked(false);
    connect(actQuickHistogram, SIGNAL(toggled(bool)), this, SLOT(slQuickHistogram(bool)));

    actAutosetColumnWidth=new QAction(tr("Autoset column &width"), this);
    connect(actAutosetColumnWidth, SIGNAL(triggered()), this, SLOT(slAutoSetColumnWidth()));

    actIndexedStat=new QAction(tr("insert indexed statistics for columns"), this);
    connect(actIndexedStat, SIGNAL(triggered()), this, SLOT(slInsertIndexedStat()));
    connect(this, SIGNAL(enableActions(bool)), actIndexedStat, SLOT(setEnabled(bool)));


    actColumnsBoxplots=new QAction(tr("calculate boxplots from columns"), this);
    connect(actColumnsBoxplots, SIGNAL(triggered()), this, SLOT(slColumnsBoxplots()));
    connect(this, SIGNAL(enableActions(bool)), actColumnsBoxplots, SLOT(setEnabled(bool)));


    tbMain->addAction(actLoadTable);
    tbMain->addAction(actImportTable);
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
    tbMain->addAction(actCalcCell);
    tbMain->addAction(actClearExpression);
    tbMain->addAction(actRecalcAll);
    tbMain->addAction(actSort);
    tbMain->addSeparator();
    tbMain->addAction(actPreScript);
    tbMain->addSeparator();
    tbMain->addAction(actQuickStat);
    tbMain->addAction(actQuickHistogram);



    tvMain->addAction(actUndo);
    tvMain->addAction(actRedo);
    tvMain->addAction(getSeparatorAction(this));
    tvMain->addAction(actCopy);
    tvMain->addAction(actCopyResults);
    tvMain->addAction(actCopyResultsNoHead);
    tvMain->addAction(actCut);
    tvMain->addAction(actPaste);
    tvMain->addAction(actPasteNoEquations);
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
    tvMain->addAction(actSHowMultiColEditor);
    tvMain->addAction(actSetDatatype);
    tvMain->addAction(actSetCellsValue);
    tvMain->addAction(getSeparatorAction(this));
    tvMain->addAction(actSetColumnValues);
    tvMain->addAction(actCalcCell);
    tvMain->addAction(actClearExpression);
    tvMain->addAction(actRecalcAll);
    tvMain->addAction(actExpressionSeedBeforeTableEval);
//    tvMain->addAction(getSeparatorAction(this));
//    tvMain->addAction(actHistogram);
//    tvMain->addAction(actHistogram2D);
//    tvMain->addAction(actColumnsBoxplots);
//    tvMain->addAction(actIndexedStat);
    tvMain->addAction(actSort);

    propertyEditor->setMenuBarVisible(true);
    QMenu* menuFile=propertyEditor->addMenu("&File", 0);
    menuFile->addAction(actLoadTable);
    menuFile->addAction(actImportTable);
    menuFile->addAction(actSaveTable);
    menuFile->addSeparator();
    menuFile->addAction(actMakeEditable);
    menuFile->addSeparator();
    menuFile->addAction(actLoadTableTemplate);
    menuFile->addAction(actSaveTableTemplate);
    menuFile->addSeparator();
    menuFile->addAction(tvMain->getActPrint());

    QMenu* menuEdit=propertyEditor->addMenu("&Edit", 0);
    menuEdit->addAction(actUndo);
    menuEdit->addAction(actRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(actCopy);
    menuEdit->addAction(actCut);
    menuEdit->addAction(actPaste);
    menuEdit->addAction(actPasteNoEquations);
    menuEdit->addAction(actImportFromClipboard);
    menuEdit->addSeparator();
    menuEdit->addAction(actCopyResults);
    menuEdit->addAction(actCopyResultsNoHead);
    menuEdit->addSeparator();
    menuEdit->addAction(actDelete);
    menuEdit->addAction(actClearExpression);
    menuEdit->addAction(actDeleteRow);
    menuEdit->addAction(actDeleteColumn);

    QMenu* menuTab=propertyEditor->addMenu("&Table", 0);
    menuTab->addAction(actCopyTemplate);
    menuTab->addAction(actPasteTemplate);
    menuTab->addSeparator();
    menuTab->addAction(actAppendRow);
    menuTab->addAction(actInsertRow);
    menuTab->addAction(actAppendColumn);
    menuTab->addAction(actInsertColumn);
    menuTab->addAction(actSHowMultiColEditor);
    menuTab->addSeparator();
    menuTab->addAction(actDeleteRow);
    menuTab->addAction(actDeleteColumn);
    menuTab->addAction(actRecalcAll);
    menuTab->addSeparator();
    menuTab->addAction(actSort);
    menuTab->addAction(actHistogram);
    menuTab->addAction(actHistogram2D);
    menuTab->addAction(actColumnsBoxplots);
    menuTab->addAction(actIndexedStat);
    menuTab->addSeparator();
    menuTab->addAction(actClear);
    menuTab->addAction(actResize);

    QMenu* menuColumns=propertyEditor->addMenu("C&olumns", 0);
    menuColumns->addAction(actSetColumnTitle);
    menuColumns->addAction(actSHowMultiColEditor);
    menuColumns->addAction(actSetColumnValues);
    menuColumns->addSeparator();
    menuColumns->addAction(actSort);
    menuColumns->addAction(actHistogram);
    menuColumns->addAction(actHistogram2D);
    menuColumns->addAction(actColumnsBoxplots);
    menuColumns->addAction(actIndexedStat);

    QMenu* menuCells=propertyEditor->addMenu("&Cells", 0);
    menuCells->addAction(actSetDatatype);
    menuDatatypes=menuCells->addMenu(QIcon(":/table/cell_types.png"), tr("set datatype to ..."));
    menuDatatypes->addAction(actSetDatatypeString);
    menuDatatypes->addAction(actSetDatatypeDouble);
    menuDatatypes->addAction(actSetDatatypeInteger);
    menuDatatypes->addAction(actSetDatatypeBool);
    menuDatatypes->addAction(actSetDatatypeDate);
    actMenuDatatypes=menuDatatypes->menuAction();
    tvMain->insertAction(actSetDatatype, actMenuDatatypes);
    tbMain->insertAction(actSetDatatype, actMenuDatatypes);
    menuCells->addAction(actSetCellsValue);
    menuCells->addSeparator();
    menuEdit->addAction(actDelete);
    menuCells->addSeparator();
    menuCells->addAction(actCalcCell);
    menuCells->addAction(actClearExpression);

    QMenu* menuTools=propertyEditor->addMenu("T&ools", 0);
    menuTools->addAction(actPreScript);
    menuTools->addAction(actQuickStat);
    menuTools->addAction(actQuickHistogram);
    menuTools->addAction(actHistogram);
    menuTools->addAction(actHistogram2D);
    menuTools->addAction(actColumnsBoxplots);
    menuTools->addAction(actIndexedStat);


    QMenu*menuView =propertyEditor->addMenu("&View", 0);
    menuView->addAction(actAutosetColumnWidth);

    QTimer::singleShot(10, this, SLOT(delayedStartSearch()));
}

void QFRDRTableEditor::delayedStartSearch()
{
    QStringList sl;
    sl<<QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"parserreference/";
    sl<<QFPluginServices::getInstance()->getMainHelpDirectory()+"/parserreference/";
    functionRef->startSearch(sl);
}

void QFRDRTableEditor::preScriptChanged()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        m->setParserPreScript(edtPreScript->text());
        QFMathParser mp;
        mp.addVariableDouble("thisrdr", 1);
        mp.addVariableDouble("columns", 1);
        mp.addVariableDouble("rows", 1);
        QFMathParser::qfmpNode* n=mp.parse(edtPreScript->text());
        if (mp.hasErrorOccured()) {
            labPreScriptOK->setText(tr("syntax check: <font color=\"red\">ERROR:<br>&nbsp;&nbsp;&nbsp;&nbsp;%1</font><br><hr>").arg(mp.getLastErrors().join("<br>&nbsp;&nbsp;&nbsp;&nbsp;")));
        } else {
            labPreScriptOK->setText(tr("syntax check: <font color=\"darkgreen\">OK</font><br><hr>"));
        }
        if (n) delete n;
    }
}



void QFRDRTableEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        QFRDRTable* m=qobject_cast<QFRDRTable*>(old);
        if (m && m->model()) {
            disconnect(actUndo, SIGNAL(triggered()), m->model(), SLOT(undo()));
            disconnect(m->model(), SIGNAL(undoAvailable(bool)), actUndo, SLOT(setEnabled(bool)));
            disconnect(actRedo, SIGNAL(triggered()), m->model(), SLOT(redo()));
            disconnect(m->model(), SIGNAL(redoAvailable(bool)), actRedo, SLOT(setEnabled(bool)));
            disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), this, SLOT(setActionsEnabled(bool)));
            disconnect(tvMain->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(slEditColumnProperties(int)));
            disconnect(tvMain->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged()));
            disconnect(edtPreScript, SIGNAL(textChanged()), this, SLOT(preScriptChanged()));
        }
        tmQuickStat->setReadonly(false);
        tmQuickStat->clear();
        tmQuickStat->setReadonly(true);
    }
    //std::cout<<"qobject_cast ... ";
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    //std::cout<<m<<" ... ";
    //if (m) std::cout<<m->model()<<" ... ";
    if (m && m->model()) {
        actExpressionSeedBeforeTableEval->setChecked(m->getQFProperty("actExpressionSeedBeforeTableEval", true).toBool());
        tvMain->setModel(m->model());
        edtPreScript->setText(m->getParserPreScript());
        connect(edtPreScript, SIGNAL(textChanged()), this, SLOT(preScriptChanged()));
        connect(m->model(), SIGNAL(notReadonlyChanged(bool)), this, SLOT(setActionsEnabled(bool)));
        connect(tvMain->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(slEditColumnProperties(int)));
        connect(actUndo, SIGNAL(triggered()), m->model(), SLOT(undo()));
        connect(m->model(), SIGNAL(undoAvailable(bool)), actUndo, SLOT(setEnabled(bool)));
        connect(actRedo, SIGNAL(triggered()), m->model(), SLOT(redo()));
        connect(m->model(), SIGNAL(redoAvailable(bool)), actRedo, SLOT(setEnabled(bool)));
        connect(tvMain->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged()));
        connect(m, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(updateScripts()));
        m->model()->setReadonly(m->model()->isReadonly());
        m->model()->emitUndoRedoSignals(true);
        actPreScript->setChecked(m->getParserPreScript().size()>0);
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

void QFRDRTableEditor::slExpressionSeedBeforeTableEval()
{
    if (current) {
        current->setQFProperty("actExpressionSeedBeforeTableEval", actExpressionSeedBeforeTableEval->isChecked(), false, false);
    }
}

void QFRDRTableEditor::slSaveTable() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QString selectedFilter="";
            QString filter= m->getExportDialogFiletypes();
            QString fileName = qfGetSaveFileNameSet("table/slSaveTable/", this, m->getExportDialogTitle(), currentTableDir, filter, &selectedFilter);
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
                  <<tr("Semicolon Separated Values, Decimal Comma (*.txt *.csv *.dat)")
                  <<tr("Table XML file (*.qftxml)");
            QString fileName = qfGetOpenFileNameSet("table/slLoadTable/", this, tr("Load Table ..."), currentTableDir, filter.join(";;"), &selectedFilter);
            //std::cout<<"selectedFilter: "<<selectedFilter.toStdString()<<std::endl;
            if (!fileName.isNull()) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                int f=filter.indexOf(selectedFilter);

                if (f==filter.size()-1) {
                    m->model()->readXMLFile(fileName, 0, 0, false);
                } else {

                    QFDlgCSVParameters* csvDlg=new QFDlgCSVParameters(this, settings->getQSettings()->value("table/column_separator_save", ",").toString(),
                                                                  settings->getQSettings()->value("table/decimal_separator_save", ".").toString(),
                                                                  settings->getQSettings()->value("table/comment_start_save", "#").toString(),
                                                                  settings->getQSettings()->value("table/header_start_save", "#!").toString());
                    if (f==0) {
                        csvDlg->set_column_separator(',');
                        csvDlg->set_decimal_separator('.');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    } else if (f==1) {
                        csvDlg->set_column_separator(';');
                        csvDlg->set_decimal_separator('.');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    } else if (f==2) {
                        csvDlg->set_column_separator(';');
                        csvDlg->set_decimal_separator(',');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    }
                    csvDlg->setFileContents(fileName);
                    if (csvDlg->exec()==QDialog::Accepted) {
                        QMap<QString, QVariant> p;
                        settings->getQSettings()->setValue("table/column_separator_save", QString(csvDlg->get_column_separator()));
                        settings->getQSettings()->setValue("table/decimal_separator_save", QString(csvDlg->get_decimal_separator()));
                        settings->getQSettings()->setValue("table/comment_start_save", QString(csvDlg->get_comment_start()));
                        settings->getQSettings()->setValue("table/header_start_save", QString(csvDlg->get_header_start()));
                        m->model()->readCSV(fileName, csvDlg->get_column_separator(), csvDlg->get_decimal_separator(), csvDlg->get_header_start(), csvDlg->get_comment_start());
                    }
                    delete csvDlg;
                }
                QApplication::restoreOverrideCursor();
            }
        }
    }
}

void QFRDRTableEditor::slImportFile()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QString selectedFilter="";
            QStringList filter;
            filter<<tr("Comma Separated Values, Decimal Dot (*.txt *.csv *.dat)")
                  <<tr("Semicolon Separated Values, Decimal Dot (*.txt *.csv *.dat)")
                  <<tr("Semicolon Separated Values, Decimal Comma (*.txt *.csv *.dat)")
                  <<tr("Tab Separated Values, Decimal Dot (*.txt *.tsv *.dat *.csv)")
                  <<tr("Tab Separated Values, Decimal Comma (*.txt *.tsv *.dat *.csv)")
                  <<tr("Table XML file (*.qftxml)");
            QString fileName = qfGetOpenFileNameSet("table/slImportTable/", this, tr("Load Table ..."), currentTableDir, filter.join(";;"), &selectedFilter);
            //std::cout<<"selectedFilter: "<<selectedFilter.toStdString()<<std::endl;
            if (!fileName.isNull()) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                int f=filter.indexOf(selectedFilter);

                QFTablePluginModel* datamodel=new QFTablePluginModel(this);
                datamodel->setUndoEnabled(false);
                datamodel->setVerticalHeaderShowRowNumbers(true);
                bool ok=false;

                if (f==filter.size()-1) {
                    datamodel->readXMLFile(fileName, 0, 0, false);
                    ok=true;
                } else {

                    QFDlgCSVParameters* csvDlg=new QFDlgCSVParameters(this, settings->getQSettings()->value("table/column_separator_save", ",").toString(),
                                                                  settings->getQSettings()->value("table/decimal_separator_save", ".").toString(),
                                                                  settings->getQSettings()->value("table/comment_start_save", "#").toString(),
                                                                  settings->getQSettings()->value("table/header_start_save", "#!").toString());
                    if (f==0) {
                        csvDlg->set_column_separator(',');
                        csvDlg->set_decimal_separator('.');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    } else if (f==1) {
                        csvDlg->set_column_separator(';');
                        csvDlg->set_decimal_separator('.');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    } else if (f==2) {
                        csvDlg->set_column_separator(';');
                        csvDlg->set_decimal_separator(',');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    } else if (f==3) {
                        csvDlg->set_column_separator('\t');
                        csvDlg->set_decimal_separator('.');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    } else if (f==4) {
                        csvDlg->set_column_separator('\t');
                        csvDlg->set_decimal_separator(',');
                        csvDlg->set_comment_start('#');
                        csvDlg->set_header_start("#!");
                    }
                    csvDlg->setFileContents(fileName);
                    if (csvDlg->exec()==QDialog::Accepted) {
                        QMap<QString, QVariant> p;
                        settings->getQSettings()->setValue("table/column_separator_save", QString(csvDlg->get_column_separator()));
                        settings->getQSettings()->setValue("table/decimal_separator_save", QString(csvDlg->get_decimal_separator()));
                        settings->getQSettings()->setValue("table/comment_start_save", QString(csvDlg->get_comment_start()));
                        settings->getQSettings()->setValue("table/header_start_save", QString(csvDlg->get_header_start()));
                        datamodel->readCSV(fileName, csvDlg->get_column_separator(), csvDlg->get_decimal_separator(), csvDlg->get_header_start(), csvDlg->get_comment_start());
                        ok=true;
                    }
                    delete csvDlg;
                }




                QApplication::restoreOverrideCursor();
                if (ok) {
                    DlgIMportTable* dlg=new DlgIMportTable(this);
                    dlg->setModel(datamodel);

                    if (dlg->exec()) {
                        dlg->doImport(m->model(), tvMain->currentIndex().row(), tvMain->currentIndex().column());
                    }
                    delete dlg;
                }

                delete datamodel;
            }
        }
    }
}

void QFRDRTableEditor::slImportFromClipboard()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {

                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

                QFTablePluginModel* datamodel=new QFTablePluginModel(this);
                datamodel->setUndoEnabled(false);
                datamodel->setVerticalHeaderShowRowNumbers(true);
                datamodel->setReadonly(false);
                datamodel->clear();
                datamodel->paste();
                datamodel->setReadonly(true);
                bool ok=true;




                QApplication::restoreOverrideCursor();
                if (ok) {
                    DlgIMportTable* dlg=new DlgIMportTable(true, this);
                    dlg->setModel(datamodel);

                    if (dlg->exec()) {
                        dlg->doImport(m->model(), tvMain->currentIndex().row(), tvMain->currentIndex().column());
                    }
                    delete dlg;
                }

                delete datamodel;
        }
    }
}


void QFRDRTableEditor::slSaveTableTemplate()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QString selectedFilter="";
            QString filter= tr("Table Templates (*.ttmpl);;All files (*.*)");
            QString fileName = qfGetSaveFileNameSet("table/slSaveTableTemplate/", this, m->getExportDialogTitle(), currentTableDir, filter, &selectedFilter);
            if ((!fileName.isEmpty())&&(!fileName.isNull())) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                int f=filter.split(";;").indexOf(selectedFilter);
                m->model()->saveXML(fileName, QModelIndexList(), false, true);
            }
            QApplication::restoreOverrideCursor();
        }
    }

}

void QFRDRTableEditor::slLoadTableTemplate()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QString selectedFilter="";
            QStringList filter;
            filter<<tr("Table Templates (*.ttmpl)")
                  <<tr("All files (*.*)");
            QString fileName = qfGetOpenFileNameSet("table/slLoadTableTemplate/", this, tr("Load Table ..."), currentTableDir, filter.join(";;"), &selectedFilter);
            //std::cout<<"selectedFilter: "<<selectedFilter.toStdString()<<std::endl;
            if (!fileName.isNull()) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

                m->model()->readXMLFile(fileName, 0, 0, false, true);

                QApplication::restoreOverrideCursor();
            }
        }
    }
}

void QFRDRTableEditor::slShowMultiColumnEditor()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (!multicolEdit) {
            multicolEdit=new QFRDRTableMultiColumnEditor(m->model(), this);
        }
        multicolEdit->show();
        multicolEdit->raise();
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
                int i0=m->model()->rowCount();
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
                m->columnsInserted(0, 1, false);
            } else {
                QModelIndexList l=sm->selectedIndexes();
                int i0=m->model()->columnCount();
                for (int i=0; i<l.size(); i++) {
                    if (l[i].column()<i0) i0=l[i].column();
                }
                //std::cout<<"insertColumn("<<i0<<")\n";
                m->model()->insertColumn(i0);
                m->columnsInserted(i0, 1, false);
            }
            m->emitRebuildPlotWidgets();
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFRDRTableEditor::slInsertIndexedStat()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m && m->model()) {
        QItemSelectionModel* smod=tvMain->selectionModel();
        QList<int> sel;
        if (smod->hasSelection()) {
            QModelIndexList idxs=smod->selectedIndexes();
            for (int i=0; i<idxs.size(); i++) {
                if (!sel.contains(idxs[i].column())) {
                    sel.append(idxs[i].column());
                }
            }
            qSort(sel);
        } else {
            sel<<tvMain->currentIndex().column();
        }
        QFRDRTableIndexedStatDialog* dlg=new QFRDRTableIndexedStatDialog(this);
        dlg->setTable(m);
        dlg->setSelectedColumns(sel);
        if (dlg->exec()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->disableSignals();
            QStringList names;
            QString addToPre;
            QList<QFRDRTableIndexedStatDialog::DataProps> dp;
            QStringList ex=dlg->getExpressions(names, addToPre, dp);
            m->setParserPreScript(m->getParserPreScript()+"\n"+addToPre);
            int startCol=dlg->getResultStartColumn();
            if (startCol<0) startCol=m->model()->columnCount();
            for (int i=0; i<ex.size(); i++) {
                m->model()->setColumnTitleCreate(startCol+i, names.value(i, ""));
                m->model()->setColumnHeaderData(startCol+i, QFRDRTable::ColumnExpressionRole , ex.value(i, ""));
            }

            qDebug()<<dlg->createGraphs()<<dlg->addToExistingGraph()<<dlg->addNewGraphs()<<dp.size();

            if (dlg->createGraphs()) {
                m->colgraphSetDoEmitSignals(false);
                int g=-1;
                if (dlg->addToExistingGraph()) {
                    g=dlg->addToGraph();
                }
                for (int i=0; i<dp.size(); i++) {
                    if (dlg->addNewGraphs()) {
                        g=m->colgraphAddPlot(tr("Indexed Statistics For '%1'").arg(dp[i].dataname));
                    }
                    qDebug()<<i<<":  adding to graph g="<<g;
                    if (g>=0) {
                        if (dp[i].dataForBoxplotAvailable() || (!dp[i].dataForMeanStdAvailable() && !dp[i].dataForMedianNMADAvailable())) {
                            qDebug()<<"       boxplot";
                            m->colgraphAddBoxPlot(g, QFRDRColumnGraphsInterface::cgoVertical, startCol+dp[i].cid, startCol+dp[i].cmin, startCol+dp[i].cq25, startCol+dp[i].cmedian, startCol+dp[i].cmean, startCol+dp[i].cq75, startCol+dp[i].cmax, tr("Boxplot for %1").arg(dp[i].dataname));
                        } else if (dp[i].dataForMeanStdAvailable() && !dp[i].dataForMedianNMADAvailable()) {
                            m->colgraphAddErrorGraph(g, startCol+dp[i].cid, -1, startCol+dp[i].cmean, startCol+dp[i].cstd, QFRDRColumnGraphsInterface::cgtBars, tr("Mean \\pm Std.Dev. of %1").arg(dp[i].dataname));
                            qDebug()<<"       mean, nomedian";
                        } else if (!dp[i].dataForMeanStdAvailable() && dp[i].dataForMedianNMADAvailable()) {
                            qDebug()<<"       nomean, median";
                            m->colgraphAddErrorGraph(g, startCol+dp[i].cid, -1, startCol+dp[i].cmedian, startCol+dp[i].cnmad, QFRDRColumnGraphsInterface::cgtBars, tr("Median \\pm NMAD of %1").arg(dp[i].dataname));
                        } else if (dp[i].dataForMeanStdAvailable() && dp[i].dataForMedianNMADAvailable()) {
                            qDebug()<<"       mean, median";
                            m->colgraphAddErrorGraph(g, startCol+dp[i].cid, -1, startCol+dp[i].cmean, startCol+dp[i].cstd, QFRDRColumnGraphsInterface::cgtBars, tr("Median \\pm NMAD of %1").arg(dp[i].dataname));
                            m->colgraphAddErrorGraph(g, startCol+dp[i].cid, -1, startCol+dp[i].cmedian, startCol+dp[i].cnmad, QFRDRColumnGraphsInterface::cgtPoints, tr("Median \\pm NMAD of %1").arg(dp[i].dataname));
                        }

                    }
                }
                m->colgraphSetDoEmitSignals(true);
            }

            m->model()->enableSignals();

            slRecalcAll();
            updateScripts();
            QApplication::restoreOverrideCursor();

        }
        delete dlg;
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
                    QList<int> rl;
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
                answer = QMessageBox::question(this, tr("Delete row(s) ..."), tr("Are you sure that you want to delete the selected columns?"), QMessageBox::Yes | QMessageBox::No);
                if (answer == QMessageBox::Yes) {
                    QModelIndexList l=sm->selectedIndexes();
                    QList<int> rl;
                    for (int i=0; i<l.size(); i++) {
                        if (!rl.contains(l[i].column())) rl.append(l[i].column());
                    }
                    qSort(rl);
                    QModelIndex cur=tvMain->currentIndex();
                    m->model()->disableSignals();
                    for (int i=rl.size()-1; i>=0; i--) {
                        //std::cout<<"deleting column "<<rl[i]<<std::endl;
                        m->model()->deleteColumn(rl[i]);
                        m->columnsRemoved(rl[i], 1, false);
                    }
                    m->model()->enableSignals(true);
                    m->emitRebuildPlotWidgets();
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
                    QVariant::Type t=QVariant::String;
                    switch (items.indexOf(item)) {
                        case 0: t=QVariant::String; break;
                        case 1: t=QVariant::Double; break;
                        case 2: t=QVariant::LongLong; break;
                        case 3: t=QVariant::DateTime; break;
                        case 4: t=QVariant::Bool; break;
                    }

                    setDatatype(t);
                }
            }
        }
    }
}


void QFRDRTableEditor::slSetCells()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (!sm->hasSelection()) {
            } else {
                QModelIndexList l=sm->selectedIndexes();
                QFRDRTableSetCellsDialog* dlg=new QFRDRTableSetCellsDialog(sm->currentIndex().data(Qt::EditRole), this);
                if (dlg->exec()) {
                    QVariant v=dlg->getValue();
                    m->model()->disableSignals();
                    m->model()->startMultiUndo();
                    for (int i=0; i<l.size(); i++) {
                        m->tableSetData(l[i].row(), l[i].column(), v);
                    }
                    m->model()->endMultiUndo();
                    m->model()->enableSignals(true);
                }
                delete dlg;
            }
        }
    }
}


void QFRDRTableEditor::setDatatype(QVariant::Type t)
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (sm->hasSelection()) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                QModelIndexList l=sm->selectedIndexes();
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

void QFRDRTableEditor::slSetDatatypeString()
{
    setDatatype(QVariant::String);
}

void QFRDRTableEditor::slSetDatatypeDouble()
{
    setDatatype(QVariant::Double);
}

void QFRDRTableEditor::slSetDatatypeInteger()
{
    setDatatype(QVariant::LongLong);
}

void QFRDRTableEditor::slSetDatatypeBool()
{
    setDatatype(QVariant::Bool);
}

void QFRDRTableEditor::slSetDatatypeDate()
{
    setDatatype(QVariant::DateTime);
}

void QFRDRTableEditor::slEditColumnProperties(int col) {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QItemSelectionModel* sm=tvMain->selectionModel();
            if (sm->hasSelection()) {
                int c=sm->currentIndex().column();
                if (col>=0) c=col;
                QString t=m->model()->columnTitle(c);
                QVariant comment=m->model()->getColumnHeaderData(c, QFRDRTable::ColumnCommentRole);
                QVariant imwid=m->model()->getColumnHeaderData(c, QFRDRTable::ColumnImageWidth);
                QVariant exp=m->model()->getColumnHeaderData(c, QFRDRTable::ColumnExpressionRole);

                QFRDRTableColumnEditor* edt=new QFRDRTableColumnEditor(m, m->model(), c, this);
                edt->setColumnTitle(t);
                edt->setComment(comment.toString());
                edt->setImageWidth(imwid.toInt());
                edt->setExpression(exp.isValid() && exp.canConvert(QVariant::String), exp.toString());
                int dres=edt->exec();
                if (dres>0) {
                     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                     m->model()->setColumnTitle(c, edt->getColumnTitle());
                     m->model()->setColumnHeaderData(c, QFRDRTable::ColumnCommentRole, edt->getComment());
                     m->model()->setColumnHeaderData(c, QFRDRTable::ColumnImageWidth, edt->getImageWidth());
                     if (edt->getExpressionEnabled()) {
                         m->model()->setColumnHeaderData(c, QFRDRTable::ColumnExpressionRole, edt->getExpression());
                         if (dres==1) slRecalcAll();
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

void QFRDRTableEditor::slPasteNoEquations()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QModelIndex c=tvMain->selectionModel()->currentIndex();
            QSet<int> dontImportRoles;
            dontImportRoles.insert(QFRDRTable::TableExpressionRole);
            dontImportRoles.insert(QFRDRTable::ColumnExpressionRole);
            m->model()->paste(c.row(), c.column(), dontImportRoles, dontImportRoles);
            QApplication::restoreOverrideCursor();
        }
    }
}

void QFRDRTableEditor::slCopyTableTemplate()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->copy(tvMain->selectionModel()->selectedIndexes(), false, true);
            QApplication::restoreOverrideCursor();
        }
    }

}

void QFRDRTableEditor::slPasteTableTemplate()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            if (QApplication::clipboard()->mimeData()&&(QApplication::clipboard()->mimeData()->hasFormat("quickfit3/qfrdrtable_template")||QApplication::clipboard()->mimeData()->hasFormat("quickfit3/qfrdrtable"))) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                QModelIndex c=tvMain->selectionModel()->currentIndex();
                m->model()->pasteHeaderTemplate(c.row(), c.column());
                QApplication::restoreOverrideCursor();
            } else {
                QMessageBox::critical(this, tr("Paste table template"), tr("No table template data in clipboard!"));
            }
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

void QFRDRTableEditor::slMakeEditable()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            bool ok=QMessageBox::warning(this, tr("make table RDR editable ..."), tr("You are about to make this table editable.\nCurrently the table data is read from an external file. If you use this function, the table contents will no longer be read from the file, but a possibly changed version will be stored in the project.\n\nDo you really want to do this?\n\n[Alternatively you can create a new editable table RDR and load an external file into it.]"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes;

            if (ok) {
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                m->model()->disableSignals();
                m->setQFProperty("DONT_READWRITE_FILE", true, false, false);
                m->model()->setReadonly(false);
                setActionsEnabled(true);
                QApplication::restoreOverrideCursor();
            }
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


void QFRDRTableEditor::slCalcCell() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            QModelIndex ci=tvMain->currentIndex();
            if (ci.isValid()) {
                QItemSelectionModel* smod=tvMain->selectionModel();
                QModelIndex cur=tvMain->currentIndex();
                if (smod->hasSelection()) {
                    if (!dlgMathExpression) dlgMathExpression=new QFRDRTableFormulaDialog(m, m->model(), smod->currentIndex().column(), smod->currentIndex().row(), this);
                    else dlgMathExpression->init(m->model(), cur.column(), cur.row());
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
                    dlgMathExpression->init(m->model(), cur.column(), cur.row());
                    if (dlgMathExpression->exec()) {
                        if (idxs.size()>0) {
                            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                            QItemSelection sel=tvMain->selectionModel()->selection();
                            QModelIndex cur=tvMain->selectionModel()->currentIndex();
                            m->model()->disableSignals();


                            bool ok=true;
                            QFMathParser mp; // instanciate
                            mp.addVariableDouble("thisrdr", m->getID());
                            mp.addVariableDouble("columns", m->model()->columnCount());
                            mp.addVariableDouble("rows", m->model()->rowCount());


                            if (dlgMathExpression->getExpression().isEmpty())  {
                                for (int i=0; i<idxs.size(); i++) {
                                    m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column(), "");
                                }
                            } else {
                                QFMathParser::qfmpNode* n=NULL;
                                mp.resetErrors();
                                if (m->getParserPreScript().size()>0) {
                                    QFMathParser::qfmpNode* npre=mp.parse(m->getParserPreScript());
                                    if (mp.hasErrorOccured()) {
                                        QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while parsing the prescript expression '%1':\n%2").arg(m->getParserPreScript()).arg(mp.getLastError()));
                                        ok=false;
                                    }
                                    if (ok && npre) {
                                        npre->evaluate();
                                        if (mp.hasErrorOccured()) {
                                            QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while evaluating the prescript expression '%1':\n%2").arg(m->getParserPreScript()).arg(mp.getLastError()));
                                            ok=false;
                                        }
                                    }
                                    if (npre) delete npre;
                                    mp.resetErrors();
                                }
                                addQFRDRTableFunctions(&mp);

                                mp.addVariableDouble("row", 0);
                                mp.addVariableDouble("col", 0);
                                mp.addVariableDouble("column", 1);

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
                m->model()->disableSignals();
                m->model()->startMultiUndo();
                int answer = QMessageBox::question(this, tr("Clear Table Expressions"), tr("Are you sure that you want to clear all expressions from the selected table cells?"), QMessageBox::Yes | QMessageBox::No);
                if (answer == QMessageBox::Yes) {
                    QModelIndexList idxs=smod->selectedIndexes();
                    QMap<int, int> colcnt;
                    for (int i=0; i<idxs.size(); i++) {
                        m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column(), QVariant());
                        if (colcnt.contains(idxs[i].column())) colcnt[idxs[i].column()]=colcnt[idxs[i].column()]+1;
                        else colcnt[idxs[i].column()]=1;
                    }
                    QMapIterator<int, int> it(colcnt);
                    int clearCol=-1;
                    while (it.hasNext()) {
                        it.next();
                        QString lexp=m->model()->getColumnHeaderData(it.key(), QFRDRTable::ColumnExpressionRole).toString();
                        if (!lexp.isEmpty()) {
                            if (it.value()>=m->model()->rowCount()) {
                                if (clearCol==-1) {
                                    clearCol= QMessageBox::question(this, tr("Clear Column Expressions"), tr("You selected a complete column. Do you want to remove the associated column expression?"), QMessageBox::Yes | QMessageBox::No);
                                }
                                if (clearCol==QMessageBox::Yes) {
                                    m->model()->setColumnHeaderData(it.key(), QFRDRTable::ColumnExpressionRole, QVariant());
                                }
                            }
                        }
                    }
                }
                m->model()->enableSignals(true);
                m->model()->endMultiUndo();
                if (QMessageBox::question(this, tr("Clear Expressions"), tr("You removed expressions from the table.\n  Do you want to recalculate All expressions?"), QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes) {
                    slRecalcAll();
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
    slCalcCell();
}

void QFRDRTableEditor::tableKeyPressed(int key, Qt::KeyboardModifiers modifiers, QString /*text*/)
{
    //qDebug()<<"key="<<key<<"   mods="<<modifiers<<"   text="<<text;
    if (key==Qt::Key_Delete && modifiers==Qt::NoModifier) {
        slDelete();
    }
}

void QFRDRTableEditor::selectionChanged()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
            //////////////////////////////////////////////////////////////////////////////////////////////
            // QUICK_STATISTICS!!!
            //////////////////////////////////////////////////////////////////////////////////////////////
            if (actQuickStat->isChecked() || actQuickHistogram->isChecked()) {
                if (actQuickStat->isChecked()) {
                    tmQuickStat->setReadonly(false);
                    tmQuickStat->disableSignals();
                    tmQuickStat->clear();
                }
                if (actQuickHistogram->isChecked()) {
                    histQuick->clear();
                }

                QModelIndexList idxs=tvMain->selectionModel()->selectedIndexes();

                QList<int> cols;
                for (int i=0; i<idxs.size(); i++) {
                    if (!cols.contains(idxs[i].column())) cols.append(idxs[i].column());
                }
                qSort(cols);

                //qDebug()<<"quickstat: "<<idxs.size()<<cols;

                for (int i=0; i<cols.size(); i++) {
                    QVector<double> data;
                    for (int j=0; j<idxs.size(); j++) {
                        if (idxs[j].column()==cols[i]) {
                            QVariant v=m->model()->cell(idxs[j].row(), idxs[j].column());
                            if (v.canConvert(QVariant::Double)) data.append(v.toDouble());
                        }
                    }
                    if (actQuickStat->isChecked() && data.size()>0) {
                        int r=tmQuickStat->rowCount();
                        tmQuickStat->setColumnTitleCreate(0, tr("column"));
                        tmQuickStat->setColumnTitleCreate(1, tr("mean"));
                        tmQuickStat->setColumnTitleCreate(2, tr("std. dev."));
                        tmQuickStat->setColumnTitleCreate(3, tr("min"));
                        tmQuickStat->setColumnTitleCreate(4, tr("q25"));
                        tmQuickStat->setColumnTitleCreate(5, tr("median"));
                        tmQuickStat->setColumnTitleCreate(6, tr("q75"));
                        tmQuickStat->setColumnTitleCreate(7, tr("max"));
                        tmQuickStat->setColumnTitleCreate(8, tr("# items"));
                        tmQuickStat->setCellCreate(r, 0, m->model()->headerData(cols[i], Qt::Horizontal));
                        tmQuickStat->setCellCreate(r, 1, qfstatisticsAverage(data));
                        tmQuickStat->setCellCreate(r, 2, qfstatisticsStd(data));
                        qSort(data);
                        tmQuickStat->setCellCreate(r, 3, qfstatisticsSortedMin(data));
                        tmQuickStat->setCellCreate(r, 4, qfstatisticsSortedQuantile(data, 0.25));
                        tmQuickStat->setCellCreate(r, 5, qfstatisticsSortedMedian(data));
                        tmQuickStat->setCellCreate(r, 6, qfstatisticsSortedQuantile(data, 0.75));
                        tmQuickStat->setCellCreate(r, 7, qfstatisticsSortedMax(data));
                        tmQuickStat->setCellCreate(r, 8, qfstatisticsCount(data));
                    }
                    if (actQuickHistogram->isChecked() && data.size()>0) {
                        histQuick->addCopiedHistogram( m->model()->headerData(cols[i], Qt::Horizontal).toString(), data.data(), data.size());
                    }

                }

                if (actQuickStat->isChecked()) {
                    tmQuickStat->setReadonly(true);
                    tmQuickStat->enableSignals(true);
                    tvQuickStat->resizeColumnsToContents();
                }
                if (actQuickHistogram->isChecked()) {
                    histQuick->updateHistogram(true);
                }
            }
        }
    }
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
            QModelIndexList idxs;//=smod->selectedIndexes();
            QStringList exprs;
            for (int c=0; c<m->model()->columnCount(); c++) {
                for (int r=0; r<m->model()->rowCount(); r++) {
                    QModelIndex idx=m->model()->index(r, c);
                    QString lexp=m->model()->cellUserRole(QFRDRTable::TableExpressionRole, r, c).toString();
                    if (!lexp.isEmpty()) {
                        idxs.append(idx);
                        exprs.append(lexp);
                    }
                }
            }

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->disableSignals();
            m->model()->startMultiUndo();
            QProgressDialog dialog(this);
            dialog.setLabelText(tr("evaluating expressions"));


            bool ok=true;
            QFMathParser mp, mpColumns; // instanciate
            uint32_t seed1=mp.get_rng()->randInt();
            uint32_t seed2=mp.get_rng()->randInt();
            if (actExpressionSeedBeforeTableEval->isChecked()) {
                mp.get_rng()->seed(seed1);
                mpColumns.get_rng()->seed(seed2);
            }
            mpColumns.addVariableDouble("thisrdr", m->getID());
            mpColumns.addVariableDouble("columns", m->model()->columnCount());
            mpColumns.addVariableDouble("rows", m->model()->rowCount());
            mp.addVariableDouble("thisrdr", m->getID());
            mp.addVariableDouble("columns", m->model()->columnCount());
            mp.addVariableDouble("rows", m->model()->rowCount());

            if (m->getParserPreScript().size()>0) {
                QFMathParser::qfmpNode* npre=mp.parse(m->getParserPreScript());
                QFMathParser::qfmpNode* nprec=mpColumns.parse(m->getParserPreScript());
                if (mp.hasErrorOccured()) {
                    QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while parsing the prescript expression '%1':\n%2").arg(m->getParserPreScript()).arg(mp.getLastError()));
                    ok=false;
                }
                if (ok && npre) {
                    npre->evaluate();
                    if (mp.hasErrorOccured()) {
                        QMessageBox::critical(this, tr("QuickFit-table"), tr("An error occured while evaluating the prescript expression '%1':\n%2").arg(m->getParserPreScript()).arg(mp.getLastError()));
                        ok=false;
                    }
                }
                if (ok && nprec) {
                    nprec->evaluate();
                }
                if (npre) delete npre;
                if (nprec) delete nprec;
                mp.resetErrors();
                mpColumns.resetErrors();
            }

            addQFRDRTableFunctions(&mp);
            addQFRDRTableFunctions(&mpColumns, NULL, true);
            mp.addVariableDouble("row", 1);
            mp.addVariableDouble("col", 1);
            mp.addVariableDouble("column", 1);
            mp.addVariableDouble("thisrdr", m->getID());
            mp.addVariableDouble("columns", 1.0);
            mp.addVariableDouble("rows", 1.0);

            mpColumns.addVariableDouble("col", 1);
            mpColumns.addVariableDouble("thisrdr", m->getID());
            mpColumns.addVariableDouble("columns", 1.0);
            mpColumns.addVariableDouble("rows", 1.0);

            QMap<QString, QFMathParser::qfmpNode*> nodes;
            QMap<QString, QFMathParser::qfmpNode*> cnodes;
            int changes=1;
            int iterations=0;
            int maxIterations=10;
            dialog.setRange(0,maxIterations);
            dialog.show();
            while (iterations<maxIterations && changes>0 && !dialog.wasCanceled()) {
                changes=0;
                dialog.setValue(iterations);
                QApplication::processEvents();
                // evaluate column expressions
                int i=0;
                int delta=1;
                int itend=m->model()->columnCount();
                if (iterations%2==1) {
                    i=m->model()->columnCount()-1;
                    delta=-1;
                    itend=-1;
                }




                //for (int i=0; i<m->model()->columnCount(); i++) {
                if (m->model()->columnCount()>0) while (i!=itend) {
                    if (actExpressionSeedBeforeTableEval->isChecked()) {
                        mpColumns.get_rng()->seed(seed2+i);
                    }
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

                                QVariant nvv=evaluateExpression(mpColumns, cnodes[lexp], m->model()->index(0,i), &ok, lexp, true);
                                QVariantList nv=nvv.toList();
                                if (nv.isEmpty()) nv<<nvv;
                                //qDebug()<<"     reeval5 col("<<i<<n.size()<<")\n        <= "<<ok<<nv.size();
                                bool equalWithVariant=true;
                                //qDebug()<<"nv = "<<nv;
                                //qDebug()<<"ov = "<<ov;
                                for (int in=0; in<qMax(nv.size(),ov.size()); in++) {
                                    if (in<nv.size() && in<ov.size()) {
                                        //qDebug()<<nv[in]<<ov[in]<<nv[in].isValid()<<ov[in].isValid();
                                        if (nv[in]!=ov[in]) {
                                            if (nv[in].type()==QVariant::Double && ov[in].type()==QVariant::Double) {
                                                double nd=nv[in].toDouble();
                                                double od=ov[in].toDouble();
                                                if (std::isnan(nd) && std::isnan(od)) {
                                                    // equals
                                                } else if (std::isinf(nd) && std::isinf(od)) {
                                                    // equals
                                                } else {
                                                    equalWithVariant=false;
                                                }
                                            } else {
                                                equalWithVariant=false;
                                            }
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
                                    if (!equalWithVariant) break;
                                }
                                //qDebug()<<ok<<ov.size()<<nv.size()<<equalWithVariant;
                                if (ok && !equalWithVariant) {
                                    changes++;
                                    m->model()->setColumnCreate(i, nv);
                                    m->model()->setCellsUserRoleCreate(QFRDRTable::TableExpressionRole, 0, qMax(nv.size()-1, m->model()->rowCount()-1), i, i, QVariant());
                                    /*for (int r=0; r<qMax(nv.size(), m->model()->rowCount()); r++) {
                                        m->model()->setCellCreate(r, i, nv.value(r, QVariant()));
                                        m->model()->setCellUserRoleCreate(QFRDRTable::TableExpressionRole, r, i, QVariant());
                                    }*/
                                }
                            }
                            //qDebug()<<"     reeval6 col"<<i<<ok;
                        }
                        if (!ok) break;
                    }

                    if (i%5==0) {
                        QApplication::processEvents();
                        if (dialog.wasCanceled()) break;
                    }
                    i+=delta;
                }

                // evaluate cell expressions
                for (int i=0; i<idxs.size(); i++) {
                    if (actExpressionSeedBeforeTableEval->isChecked()) {
                        mp.get_rng()->seed(seed1+i);
                    }
                    QString lexp=exprs.value(i, "");//m->model()->cellUserRole(QFRDRTable::TableExpressionRole, idxs[i].row(), idxs[i].column()).toString();
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
                    if (i%50==0) {
                        QApplication::processEvents();
                        if (dialog.wasCanceled()) break;
                    }

                }
                QApplication::processEvents();
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

            m->model()->endMultiUndo();
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


void QFRDRTableEditor::slColumnsBoxplots()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m && m->model()) {
        QItemSelectionModel* smod=tvMain->selectionModel();
        QList<int> sel;
        if (smod->hasSelection()) {
            QModelIndexList idxs=smod->selectedIndexes();
            for (int i=0; i<idxs.size(); i++) {
                if (!sel.contains(idxs[i].column())) {
                    sel.append(idxs[i].column());
                }
            }
            qSort(sel);
        } else {
            sel<<tvMain->currentIndex().column();
        }
        QFRDRTableColumnsBoxplotDialog* dlg=new QFRDRTableColumnsBoxplotDialog(this);
        dlg->setTable(m);
        dlg->setSelectedColumns(sel);
        if (dlg->exec()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            m->model()->disableSignals();
            QStringList names;
            QString addToPre;
            QStringList ex=dlg->getExpressions(names, addToPre);
            m->setParserPreScript(m->getParserPreScript()+"\n"+addToPre);
            int startCol=dlg->getResultStartColumn();
            if (startCol<0) startCol=m->model()->columnCount();
            for (int i=0; i<ex.size(); i++) {
                m->model()->setColumnTitleCreate(startCol+i, names.value(i, ""));
                m->model()->setColumnHeaderData(startCol+i, QFRDRTable::ColumnExpressionRole , ex.value(i, ""));
            }

            m->colgraphSetDoEmitSignals(false);
            int g=dlg->addToGraph();
            if (dlg->addNewGraph() || g>= m->colgraphGetPlotCount()) {
                g=m->colgraphAddPlot(tr("Boxplot"));
            }
            if (g>=0) {
                if (dlg->getPlotBoxplot()) {
                    m->colgraphAddBoxPlot(g, QFRDRColumnGraphsInterface::cgoVertical, startCol, startCol+3, startCol+4, startCol+6, startCol+5, startCol+7, startCol+8, tr("Boxplot"));
                }
                if (dlg->getPlotSymbols()) {
                    m->colgraphAddErrorGraph(g, startCol, -1, startCol+3, startCol+4, QFRDRColumnGraphsInterface::cgtPoints, tr("Mean \\pm Std.Dev."));
                }
                if (dlg->getPlot2Symbols()) {
                    m->colgraphAddErrorGraph(g, startCol, -1, startCol+3, startCol+5, QFRDRColumnGraphsInterface::cgtPoints, tr("Mean \\pm Std.Dev."));
                    m->colgraphAddErrorGraph(g, startCol, -1, startCol+4, startCol+6, QFRDRColumnGraphsInterface::cgtPoints, tr("Median \\pm NMAD"));
                }

            }

            m->colgraphSetDoEmitSignals(true);
            m->model()->enableSignals();
            slRecalcAll();
            updateScripts();
            QApplication::restoreOverrideCursor();

        }
        delete dlg;
    }
}

void QFRDRTableEditor::updateScripts()
{
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (edtPreScript->toPlainText()!=m->getParserPreScript()) edtPreScript->setText(m->getParserPreScript());
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

void QFRDRTableEditor::slQuickStat(bool enabled)
{
    setQuickTabVisible();
    if (enabled) {
        tabQuick->setCurrentWidget(tvQuickStat);
        selectionChanged();
    }
}

void QFRDRTableEditor::slPreScript(bool enabled)
{
    setQuickTabVisible();
    if (enabled) {
        tabQuick->setCurrentWidget(widPreScript);
        selectionChanged();
    }
}

void QFRDRTableEditor::slQuickHistogram(bool enabled)
{
    setQuickTabVisible();
    if (enabled) {
        tabQuick->setCurrentWidget(histQuick);
        selectionChanged();
    }
}

void QFRDRTableEditor::slAutoSetColumnWidth()
{
    tvMain->resizeColumnsToContents();
}

void QFRDRTableEditor::setQuickTabVisible()
{
    tabQuick->setVisible(actQuickStat->isChecked() || actQuickHistogram->isChecked() || actPreScript->isChecked());
    tvQuickStat->setVisible(actQuickStat->isChecked());
    histQuick->setVisible(actQuickHistogram->isChecked());
    tabQuick->setTabEnabled(tabQuick->indexOf(tvQuickStat), actQuickStat->isChecked());
    tabQuick->setTabEnabled(tabQuick->indexOf(histQuick), actQuickHistogram->isChecked());
    tabQuick->setTabEnabled(tabQuick->indexOf(widPreScript), actPreScript->isChecked());
    show();
    repaint();
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







