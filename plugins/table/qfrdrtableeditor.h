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
#include "qfrawdataeditor.h"
#include "qenhancedtableview.h"
#include "qfrawdatapropertyeditor.h"
#include "tableresizedialog.h"
#include "qfrdrtabledelegate.h"
#include "qftools.h"


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
        void slSetColumnTitle();
        void slResize();
        void slCopy();
        void slPaste();
        void slCut();
        void slDelete();

    protected:
        /** \brief table view for the contents */
        QEnhancedTableView* tvMain;

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

        QAction* actCopy;
        QAction* actCut;
        QAction* actPaste;

        QToolBar* tbMain;
    private:
};

#endif // QFRDRTABLEEDITOR_H
