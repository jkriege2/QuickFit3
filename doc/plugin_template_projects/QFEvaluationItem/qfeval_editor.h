#ifndef HEADER_HEDITOR_H
#define HEADER_HEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>

namespace Ui {
    class QFEVALEditor;
}

/*! \brief editor for QFEVALItem
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFEVALEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEVALEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEVALEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
    protected:
        /** \brief the user interface object (using QtDesigner) */
        Ui::QFEVALEditor *ui;
        
        /** \brief progress dialog used by the fits */
        QProgressDialog* dlgEvaluationProgress;
        
        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        
        /*! \brief a flag that indicates whether we are currently updating the display data 
        
            This may be used to distinguish between events emitted by the user, changing something (in a widget, ...) or
            automtically by this class reloading some display ...
        */
        bool updatingData;

        /*! \brief execute the evaulation

            \param record the record to do the evaluation for
        */
        void doEvaluation(QFRawDataRecord* record);
        
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);

    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
    
        /** \brief evaluate all files */
        void evaluateAll();
        /** \brief evaluate current file */
        void evaluateCurrent();
        /** \brief display the data from the current raw data record */
        void displayData();
        /** \brief display the current evaluation results */
        void displayEvaluation();
        /** \brief save a report of the evaluation results */
        void saveReport();
        /** \brief print a report of the evaluation results */
        void printReport();


};

#endif // HEADER_HEDITOR_H
