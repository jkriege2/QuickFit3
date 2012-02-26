#ifndef QFIMFCSFITEVALUATIONEDITOR_H
#define QFIMFCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qt/jkqtplotter.h"
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
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <QTextDocument>
#include "datacutslider.h"
#include "qfhtmlhelpwindow.h"
#include "qffitparameterwidget.h"
#include "jkverticalscrollarea.h"
#include "dlgqffitalgorithmprogressdialog.h"
#include "qffitalgorithmthreaddedfit.h"
#include "qvisiblehandlesplitter.h"
#include "../base_classes/qffitresultsevaluationeditorbase.h"
#include "../base_classes/qffitresultsbyindexevaluationeditorbase.h"
#include "../interfaces/qfrdrimagetoruninterface.h"


/*! \brief editor class for imaging FCS least-square fits
    \ingroup qf3evalp_imfcsfit


*/
class QFImFCSFitEvaluationEditor : public QFFitResultsByIndexEvaluationEditorBase {
        Q_OBJECT
    public:
        /** Default constructor */
        QFImFCSFitEvaluationEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFImFCSFitEvaluationEditor();

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec, int index);
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec, int index);

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(,int) with a viable choice for \a defaultMin */
        virtual int getUserMin();
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(int) with a viable choice for \a defaultMax */
        virtual int getUserMax();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
        /** \brief used to reread all parameter widget values from the datastore */
        virtual void updateParameterValues();


    protected:
        /** \brief label displaying the current record */
        QLabel* labRecord;
        /** \brief combobox to select a fitting algorithm */
        QComboBox* cmbAlgorithm;
        /** \brief combobox to select a model */
        QComboBox* cmbModel;
        /** \brief combobox to select a model for the weighting */
        QComboBox* cmbWeights;
        /** \brief ploter for the data&curves */
        JKQtPlotter* pltData;
        /** \brief ploter for the residuals */
        JKQtPlotter* pltResiduals;
        /** \brief DataCutSliders */
        DataCutSliders* datacut;
        /** \brief Splitter between Plots  */
        QVisibleHandleSplitter* splitPlot;
        /** \brief Splitter between Plots and Model */
        QVisibleHandleSplitter* splitModel;
        /** \brief pushbutton to configure Fitting Algorithm */
        QPushButton* btnConfigAlgorithm;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QPushButton* btnAlgorithmHelp;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QPushButton* btnModelHelp;
        /** \brief QSpinBox that allows to select a special run */
        QSpinBox* spinRun;
        /** \brief label for the run name */
        QLabel* labRun;
        /** \brief help window for help on the current fit algorithm */
        QFHTMLHelpWindow* hlpAlgorithm;
        /** \brief help window for help on the current fit model function */
        QFHTMLHelpWindow* hlpFunction;
        /** \brief combobox to select a plotting style */
        QComboBox* cmbPlotStyle;
        /** \brief combobox to select a plotting style for the data errors */
        QComboBox* cmbErrorStyle;
        /** \brief combobox to select a plotting style for the residulas */
        QComboBox* cmbResidualStyle;
        /** \brief combobox to select a type for the residulas */
        QCheckBox* chkWeightedResiduals;
        /** \brief checkbox for x logscale */
        QCheckBox* chkXLogScale;
        /** \brief checkbox to display grid */
        QCheckBox* chkGrid;
        /** \brief button to print report */
        QAction* actPrintReport;
        /** \brief button to save report */
        QAction* actSaveReport;
        /** \brief toolbar above the plots */
        QToolBar* toolbar;
        /** \brief label to display the current position of the mouse cursor */
        QLabel* labMousePosition;
        /** \brief label that displays whether the current fit parameters are global or local */
        QLabel* labFitParameters;
        /** \brief widget to switch between editing fit parameter values and ranges  */
        QTabBar* tbEditRanges;
        //QPushButton* btnEditRanges;
        /** \brief plotter for residual distribution histogram */
        JKQtPlotter* pltResidualHistogram;
        /** \brief plotter for residual autocorrelation */
        JKQtPlotter* pltResidualCorrelation;
        /** \brief label for fit statistics */
        QTextEdit* txtFitStatistics;
        /** \brief label for fit statistics */
        QLabel* labFitResult;
        /** \brief spin box to set the number of bins in residuals histogram */
        QSpinBox* spinResidualHistogramBins;
        /** \brief splitter between residual statistics plotter pltResidualHistogram and fit results txtFitStatistics */
        QVisibleHandleSplitter* splitFitStatistics;
        /** \brief layout used to configure the residuals evaluation */
        QFormLayout* layResidualAnalysis;
        /** \brief toolbox to switch between residual histogram and parameters */
        QTabWidget* tabResidulas;
        /** \brief to fit current file */
        QPushButton* btnFitCurrent;
        /** \brief to fit all runs in current file */
        QPushButton* btnFitRunsCurrent;
        /** \brief to fit current run in all file */
        QPushButton* btnFitAll;
        /** \brief to fit all runs in all file */
        QPushButton* btnFitRunsAll;
        /** \brief reset current parameter set to default */
        QPushButton* btnResetCurrent;
        /** \brief reset all parameter sets to default */
        QPushButton* btnResetAll;
        /** \brief reset all runs in the current file to default */
        QPushButton* btnResetAllRuns;
        /** \brief copy current parameter set to default parameters */
        QPushButton* btnCopyToInitial;
        /** \brief copy current parameter set to all files & initial parameters */
        QPushButton* btnCopyToAll;
        /** \brief copy current parameter set to all runs in the current file & initial parameters */
        QPushButton* btnCopyToAllRuns;
        /** \brief copy current parameter set to all files (but therein just the current run & initial parameters */
        QPushButton* btnCopyToAllCurrentRun;
        /** \brief button used to calculate the focal volume */
        QPushButton* btnCalibrateFocalVolume;

        /** \brief button to save current parameter set */
        QPushButton* btnSaveParameters;
        /** \brief button to load current parameter set */
        QPushButton* btnLoadParameters;

        /** \brief scroll area for the fit parameters */
        JKVerticalScrollArea* scrollParameters;
        /** \brief layout that is used to display fit parameters */
        QGridLayout* layParameters;

        /** \brief fit progress dialog */
        dlgQFFitAlgorithmProgressDialog* dlgFitProgress;
        /** \brief reporter for dlgFitProgress */
        dlgQFFitAlgorithmProgressDialogReporter* dlgFitProgressReporter;

        /** \brief this list contains all currently displayed fitParameterWidgets */
        QList<QPointer<QFFitParameterWidgetWrapper> > m_fitParameters;

        /** \brief width of the widgets in m_fitParameters */
        int m_parameterWidgetWidth;
        /** \brief width of the checkboxes in the parameter widgets */
        int m_parameterCheckboxWidth;


        /** \brief when \c false the events that read the data from the widgets and write it to the QFEvaluationItem current are disabled */
        bool dataEventsEnabled;
        /** \brief stores the last fit statistics report, created in updateFitFunction() */
        QString fitStatisticsReport;


        /** \brief create a report in a given QTextDocument object
         *
         *  For correct sizing of the plots: set the textWidth of the document before calling this function!
         */
        virtual void createReportDoc(QTextDocument* document);

    protected slots:
        /** \brief executed when the mouse position over the plot changes */
        void plotMouseMove(double x, double y);
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);

        /** \brief display all data and parameters describing the current record */
        virtual void displayModel(bool newWidget);

        /** \brief replot curves */
        virtual void replotData();

        /*! \brief update the fit model function and the residuals in the graphs

            This function only adds the fit function plot objects and does NOT cause a replot of the graphs. It is called by
            replotData().
        */
        virtual void updateFitFunctions();

        /** \brief executed when the sliders values change */
        void slidersChanged(int userMin, int userMax, int min, int max);
        /** \brief emitted when the pltData rezooms */
        void zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender);

        /** \brief display fit function help */
        void displayFitFunctionHelp();
        /** \brief display fit algorithm help */
        void displayFitAlgorithmHelp();
        /** \brief configure the fitting algorithm */
        void configFitAlgorithm();
        /** \brief called when the current run changes */
        void runChanged(int run);
        /** \brief called when the fit model changes */
        void modelChanged(int model);
        /** \brief called when the fit algorithm changes */
        void algorithmChanged(int model);
        /** \brief called when the weights changes */
        void weightsChanged(int weight);
        /** \brief emitted when the x logscale checkbox changes its state */
        void chkXLogScaleToggled(bool checked);
        /** \brief emitted when the grid checkbox changes its state */
        void chkGridToggled(bool checked);
        /** \brief emitted when the plot style changes */
        void plotStyleChanged(int style);
        /** \brief emitted when the error plot style changes */
        void errorStyleChanged(int style);
        /** \brief emitted when the residual plot style changes */
        void residualStyleChanged(int style);
        /** \brief emitted when the weighted residuals checkbox changes its state */
        void chkWeightedResidualsToggled(bool checked);
        /** \brief called when number of residual histogram bins changes */
        void residualHistogramBinsChanged(int bins);
        /** \brief executed when a parameter is changed by the user */
        void parameterValueChanged();
        /** \brief executed when a parameter fix is changed by the user */
        void parameterFixChanged();
        /** \brief executed when a parameter range is changed by the user */
        void parameterRangeChanged();


        /*! \brief fit model to current data
         */
        void fitCurrent();
        /** \brief fit all files (current run) */
        void fitAll();
        /** \brief fit all files, all runs */
        void fitRunsAll();
        /** \brief fit all runs in current file */
        void fitRunsCurrent();


    private:
        /** \brief create all widgets on the form */
        void createWidgets();



    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */

        using QFFitResultsByIndexEvaluationEditorBase::getUserMin;
        using QFFitResultsByIndexEvaluationEditorBase::getUserMax;



};

#endif // QFIMFCSFITEVALUATIONEDITOR_H
