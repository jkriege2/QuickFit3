#ifndef QFFITRESULTSEVALUATIONEDITORBASE_H
#define QFFITRESULTSEVALUATIONEDITORBASE_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qfplotter.h"
#include "tools.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QList>
#include <QPointer>
#include <QFormLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolBox>
#include <QTabWidget>
#include <QGridLayout>
#include <QTabBar>
#include <QTextDocument>
#include <QFileInfo>
#include <QTextDocumentWriter>
#include "qffitparameterwidget.h"
#include "jkverticalscrollarea.h"
#include "qvisiblehandlesplitter.h"


/*! \brief evaluation item editor base class for data fits using QFFitAlgorithm and QFFitFunction
    \ingroup qf3evaluationplugins

    This class implements some services that may be used in editors, when the QFEvaluationItem
    is derived from QFFitResultsEvaluation QFFitResultsByIndexEvaluation. Some of the functions
    only work when the QFEvaluationItem may be casted to QFFitResultsByIndexEvaluation or QFFitResultsEvaluation.
    If they are called with the wrong item, they will simply return without any effects! See the
    respective function documentation for hints on which functions you may use!

    In order to derive from this class you will HAVE to implement these purely virtual functions:
      - displayModel()
      - replotData()
    .

    Also your class should call these functions from this class in the respective implementations
    in the derived class, if they are provided:
      - readSettings()
      - writeSettings()
    .


    \section QFFitResultsEvaluationEditorBase_FitResultIniFiles Fit Result INI files
    Using saveCurrentFitResults() and loadCurrentFitResults(), it is possible to store the current set of
    fit values into a text file and to lateron reload them. The format of these files is:
    \code
[fit_function]
id=FITFUNCTION_ID

[fit_params]
PARAMID/value = PARMVALUE
PARAMID/error = PARMERROR
PARAMID/fix = PARMFIXSTATE
PARAMID/min = PARMVALUERANGEMIN
PARAMID/max = PARMVALUERANGEMAX

PARAMID1/value = PARMVALUE
PARAMID1/error = PARMERROR
PARAMID1/fix = PARMFIXSTATE
PARAMID1/min = PARMVALUERANGEMIN
PARAMID1/max = PARMVALUERANGEMAX

PARAMID2/value = PARMVALUE
PARAMID2/error = PARMERROR
PARAMID2/fix = PARMFIXSTATE
PARAMID2/min = PARMVALUERANGEMIN
PARAMID2/max = PARMVALUERANGEMAX
...
    \endcode



    \section QFFitResultsEvaluationEditorBase_Reports Reports
    This class implements basic services to save reports as PDF or PS file and to also print them.
    The reports themselves are generated in a function createReportDoc(), which you will have to
    overwrite in your derived class (this class contains an empty dummy implementation only!.
    You can call these slots for user interaction:
      - saveReport()
      - printReport()
    .

*/
class QFFitResultsEvaluationEditorBase : public QFEvaluationEditor {
        Q_OBJECT
    public:
        QFFitResultsEvaluationEditorBase(QString iniPrefix, QFPluginServices* services, QFEvaluationPropertyEditor *propEditor, QWidget *parent=NULL);
        virtual ~QFFitResultsEvaluationEditorBase();

        void setIniPrefix(QString p);
        QString iniPrefix() const;
        QString currentFPSSaveDir() const;
        void setCurrentFPSSaveDir(QString d);
        QString currentSaveDirectory() const;
        void setCurrentSaveDirectory(QString d);

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec)=0;
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec)=0;

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(int) with a viable choice for \a defaultMin */
        virtual int getUserMin()=0;
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(int) with a viable choice for \a defaultMax */
        virtual int getUserMax()=0;

        /** \brief get the lower datacut for the current record */
        virtual int getUserMin(int defaultMin=0);
        /** \brief get the upper datacut for the current record */
        virtual int getUserMax(int defaultMax=1);
        /** \brief get the lower datacut for the current record, the \a defaultMin should be read from editors current data cut widget */
        virtual int getUserMin(QFRawDataRecord* rec, int defaultMin);
        /** \brief get the upper datacut for the current record, the \a defaultMax should be read from editors current data cut widget */
        virtual int getUserMax(QFRawDataRecord* rec, int defaultMax);
    signals:

    public slots:

        /** \brief save current fit function and fit results to an INI file (see \Ref QFFitResultsEvaluationEditorBase_FitResultIniFiles for details on the format) */
        virtual void saveCurrentFitResults(QFRawDataRecord* rec);
        /** \brief set current fit function and fit results according to an INI file (see \Ref QFFitResultsEvaluationEditorBase_FitResultIniFiles for details on the format) */
        virtual void loadCurrentFitResults(QFRawDataRecord* rec);
        /** \brief save current fit function and fit results to an INI file (see \Ref QFFitResultsEvaluationEditorBase_FitResultIniFiles for details on the format) */
        virtual void saveCurrentFitResults();
        /** \brief set current fit function and fit results according to an INI file (see \Ref QFFitResultsEvaluationEditorBase_FitResultIniFiles for details on the format) */
        virtual void loadCurrentFitResults();

        /** \brief emitted when we should save a report */
        void saveReport();
        /** \brief emitted when we should print a report */
        void printReport();

        /** \brief set the lower datacut for the current record */
        virtual void setUserMin(int userMin);

        /** \brief set the upper datacut for the current record */
        virtual void setUserMax(int userMax);

        /** \brief set the upper and lower datacut for the current record */
        virtual void setUserMinMax(int userMin, int userMax);

    protected slots:
        /*! \brief display all data and parameters describing the current record

            \param modelChanged specifies whether the user selected model might have changed.
                                This could e.g. indicate that the fit parameter input widgets have
                                to be recreated.
        */
        virtual void displayModel(bool modelChanged)=0;

        /** \brief replot curves */
        virtual void replotData()=0;


        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();


        /** \brief reset current */
        virtual void resetCurrent();
        /** \brief reset all files */
        virtual void resetAll();
        /** \brief copy to all files, all runs */
        virtual void copyToAll(QFRawDataRecord* rec);
        /** \brief copy to initial values */
        virtual void copyToInitial(bool emitSignals=true, QFRawDataRecord* rec=NULL);


    protected:
        /** \brief create a report in a given QTextDocument object
         *
         *  For correct sizing of the plots: set the textWidth of the document before calling this function!
         */
        virtual void createReportDoc(QTextDocument* document);

        /** \brief last save/load directory in saveCurrentFitResults() and loadCurrentFitResults() */
        QString m_currentFPSSaveDir;

        /** \brief prefix used when writing data to the settings in readSettings() and writeSettings(). */
        QString m_iniPrefix;

        /** \brief current save directory */
        QString m_currentSaveDirectory;

        /** \brief button to print report */
        QAction* actPrintReport;
        /** \brief button to save report */
        QAction* actSaveReport;


};

#endif // QFFITRESULTSEVALUATIONEDITORBASE_H
