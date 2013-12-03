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
        void requestFit(int xCol, int yCol, int sigmaCol, int plot, QString function, bool xlog=false, bool ylog=false);
        void requestRegression(int xCol, int yCol, int sigmaCol, int plot, bool xlog=false, bool ylog=false);
        void requestRefit(int plot);
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

        void slAppendRow();
        void slAppendColumn();
        void slInsertRow();
        void slInsertColumn();
        void slClear();
        void slDeleteRow();
        void slDeleteColumn();
        void slSetDatatype();
        void slSaveTable();
        void slLoadTable();
        void slEditColumnProperties(int col=-1);
        void slResize();
        void slCopy();
        void slPaste();
        void slCut();
        void slDelete();

        void slCopyTableTemplate();
        void slPasteTableTemplate();
        void slSaveTableTemplate();
        void slLoadTableTemplate();

        void slSetColumnValues();
        void slCalcCell();
        void slClearExpression();
        void slRecalcAll();
        void slSort();
        void slHistogram();
        void slHistogram2D();

        void setActionsEnabled(bool enabled);
        void setActionsDisabled(bool disabled);

        void editExpression(const QModelIndex& index);

        void tableKeyPressed(int key, Qt::KeyboardModifiers modifiers, QString text);
    signals:
        void enableActions(bool enabled);
        void disableActions(bool enabled);

    protected:
        /** \brief table view for the contents */
        QFRDRTableEnhancedTableView* tvMain;
        QFRDRTableFormulaDialog* dlgMathExpression;

        QString currentTableDir;


        QAction* actSaveTable;
        QAction* actLoadTable;
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

        QAction* actSetColumnValues;
        QAction* actCalculateColumn;
        QAction* actClearExpression;
        QAction* actRecalcAll;
        QAction* actSort;

        QAction* actCopy;
        QAction* actCut;
        QAction* actPaste;
        QAction* actHistogram;
        QAction * actHistogram2D;

        QAction* actSaveTableTemplate;
        QAction* actLoadTableTemplate;
        QAction* actCopyTemplate;
        QAction* actPasteTemplate;

        QAction* actUndo;
        QAction* actRedo;


        QToolBar* tbMain;

        QVariant evaluateExpression(QFMathParser& mp, QFMathParser::qfmpNode* node, QModelIndex cell, bool* ok, const QString& expression=QString("? ? ?"), bool columnMode=false);
    private:
};

#endif // QFRDRTABLEEDITOR_H
