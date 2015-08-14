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


#ifndef QFRDRTABLEEDITOR_H
#define QFRDRTABLEEDITOR_H

#include <QVBoxLayout>
#include <QWidget>
#include <QTableView>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include "qfrawdataeditor.h"
#include "qfrdrtableenhancedtableview.h"
#include "qfrawdatapropertyeditor.h"
#include "tableresizedialog.h"
#include "qfrdrtabledelegate.h"
#include "qftools.h"
#include "qfmathparser.h"
#include "qfrdrtableformuladialog.h"
#include "qfrdrtablehistogramdialog.h"
#include "qfenhancedtabwidget.h"
#include "qfhistogramview.h"
#include "qfenhancedplaintextedit.h"
#include "qfrdrtableindexedstatdialog.h"
class QFRDRTableMultiColumnEditor;

/*! \brief editor class for tables
    \ingroup qf3rdrdp_table
*/
class QFRDRTableEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRTableEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent=NULL);
        /** Default destructor */
        virtual ~QFRDRTableEditor();
    public slots:
        void requestFit(int xCol, int yCol, int sigmaCol, int plot, int graph, QString function, QFRDRTable::GraphDataSelection sel, bool xlog=false, bool ylog=false);
        void requestRegression(int xCol, int yCol, int sigmaCol, int plot, int graph, QFRDRTable::GraphDataSelection sel, bool xlog=false, bool ylog=false);
        void requestRefit(int plot, int graph);
    protected slots:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void slExpressionSeedBeforeTableEval();
        void slAppendRow();
        void slAppendColumn();
        void slInsertRow();
        void slInsertColumn();
        void slInsertIndexedStat();
        void slClear();
        void slDeleteRow();
        void slDeleteColumn();
        void slSetDatatype();
        void setDatatype(QVariant::Type t);
        void slSetDatatypeString();
        void slSetDatatypeDouble();
        void slSetDatatypeInteger();
        void slSetDatatypeBool();
        void slSetDatatypeDate();
        void slSaveTable();
        void slLoadTable();
        void slImportFile();
        void slImportFromClipboard();
        void slEditColumnProperties(int col=-1);
        void slResize();
        void slCopy();
        void slPaste();
        void slPasteNoEquations();
        void slCut();
        void slDelete();
        void slMakeEditable();
        void slSetCells();

        void slCopyTableTemplate();
        void slPasteTableTemplate();
        void slSaveTableTemplate();
        void slLoadTableTemplate();

        void slShowMultiColumnEditor();

        void slSetColumnValues();
        void slCalcCell();
        void slClearExpression();
        void slRecalcAll();
        void slSort();
        void slHistogram();
        void slHistogram2D();

        void slQuickStat(bool enabled);
        void slPreScript(bool enabled);

        void setQuickTabVisible();

        void setActionsEnabled(bool enabled);
        void setActionsDisabled(bool disabled);

        void editExpression(const QModelIndex& index);

        void tableKeyPressed(int key, Qt::KeyboardModifiers modifiers, QString text);

        void selectionChanged();
        void slQuickHistogram(bool enabled);
        void slAutoSetColumnWidth();
        void delayedStartSearch();
        void preScriptChanged();

        void slColumnsBoxplots();
        void updateScripts();
    signals:
        void enableActions(bool enabled);
        void disableActions(bool enabled);

    protected:
        /** \brief table view for the contents */
        QFRDRTableEnhancedTableView* tvMain;
        QFRDRTableFormulaDialog* dlgMathExpression;

        QString currentTableDir;

        QFEnhancedTabWidget* tabQuick;
        QEnhancedTableView* tvQuickStat;
        QFTableModel* tmQuickStat;
        QFHistogramView* histQuick;
        QWidget* widPreScript;
        QFEnhancedPlainTextEdit* edtPreScript;
        QFFunctionReferenceTool* functionRef;
        QLabel* labPreScriptHelp;
        QLabel* labPreScriptOK;
        QPushButton* btnPreScriptParserFHelp;
        QPushButton* btnPreScriptParserHelp;
        QScrollArea* scrollPreScriptHelp;


        QAction* actSaveTable;
        QAction* actSetCellsValue;
        QAction* actMakeEditable;
        QAction* actLoadTable;
        QAction* actImportTable;
        QAction* actAppendRow;
        QAction* actAppendColumn;
        QAction* actInsertRow;
        QAction* actInsertColumn;
        QAction* actClear;
        QAction* actDeleteRow;
        QAction* actDeleteColumn;
        QAction* actSetDatatype;
        QAction* actSetColumnTitle;
        QAction* actCopyResults;
        QAction* actCopyResultsNoHead;
        QAction* actResize;
        QAction* actDelete;
        QAction* actAutosetColumnWidth;
        QAction* actIndexedStat;
        QAction* actColumnsBoxplots;

        QAction* actSetDatatypeString;
        QAction* actSetDatatypeInteger;
        QAction* actSetDatatypeDouble;
        QAction* actSetDatatypeDate;
        QAction* actSetDatatypeBool;
        QMenu* menuDatatypes;
        QAction* actMenuDatatypes;

        QAction* actSetColumnValues;
        QAction* actCalcCell;
        QAction* actClearExpression;
        QAction* actRecalcAll;
        QAction* actSort;
        QAction* actExpressionSeedBeforeTableEval;

        QAction* actCopy;
        QAction* actCut;
        QAction* actPaste;
        QAction* actPasteNoEquations;
        QAction* actImportFromClipboard;
        QAction* actHistogram;
        QAction * actHistogram2D;

        QAction* actSaveTableTemplate;
        QAction* actLoadTableTemplate;
        QAction* actCopyTemplate;
        QAction* actPasteTemplate;



        QAction* actUndo;
        QAction* actRedo;

        QAction* actQuickStat;
        QAction* actQuickHistogram;
        QAction* actPreScript;
        QAction* actSHowMultiColEditor;

        QToolBar* tbMain;

        QPointer<QFRDRTableMultiColumnEditor> multicolEdit;

        QVariant evaluateExpression(QFMathParser& mp, QFMathParser::qfmpNode* node, QModelIndex cell, bool* ok, const QString& expression=QString("? ? ?"), bool columnMode=false);
    private:
};

#endif // QFRDRTABLEEDITOR_H
