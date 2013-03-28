#ifndef QFIMFCCSFITEVALUATIONEDITOR_H
#define QFIMFCCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include "qffitfunctionplottools.h"
#include "jkqtplotter.h"
namespace Ui {
    class QFImFCCSFitEvaluationEditor;
}

/*! \brief editor for QFImFCCSFitEvaluationItem
    \ingroup qf3rdrdp_imfccsfit

    The main part of this editor is a QTableView which allows to edit the fit parameters of several files.
    A special item delegate is used to create special editing widgets for the values and to select fit functions and RDRs.
*/
class QFImFCCSFitEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFImFCCSFitEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFImFCCSFitEvaluationEditor();
    protected slots:
        void zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender);
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        /** \brief display fit algorithm help */
        void showFitAlgorithmHelp();
        /** \brief configure the fitting algorithm */
        void configFitAlgorithm();
        /** \brief called when the fit algorithm changes */
        void fitAlgorithmChanged(int model);

        void setCurrentRun(int run);
    protected:
        /** \brief the user interface object (using QtDesigner) */
        Ui::QFImFCCSFitEvaluationEditor *ui;
        
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

        /** ßbrief ensures that the parameter table model is displayed properly */
        void ensureCorrectParamaterModelDisplay();

        void fileChanged(int num, QFRawDataRecord* file);

        void plotMouseMoved(double x, double y);

    
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

#endif // QFIMFCCSFITEVALUATIONEDITOR_H
