#ifndef QFFCSBYINDEXANDMODELEVALUATIONEDITOR_H
#define QFFCSBYINDEXANDMODELEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>

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
#include "jkverticalscrollarea.h"
#include "qvisiblehandlesplitter.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qfsimplefitparameterswidget.h"
#include "qfusesresultsbyindexevaluationeditor.h"
#include "qfrdrfcsdatainterface.h"

class QFFCSByIndexAndModelEvaluationEditor : public QFUsesResultsByIndexEvaluationEditor {
        Q_OBJECT
    public:
        explicit QFFCSByIndexAndModelEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget *parent = 0, const QString& iniPrefix=QString("/QFFCSByIndexAndModelEvaluationEditor"));
        ~QFFCSByIndexAndModelEvaluationEditor();



        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
    public slots:




    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);

        /** \brief evaluate in all files the current run */
        virtual void fitRunsAll()=0;
        /** \brief evaluate current file and run */
        virtual void fitCurrent()=0;
        /** \brief evaluate all runs in all files */
        virtual void fitAll()=0;
        /** \brief evaluate all runs in current files */
        virtual void fitRunsCurrent()=0;

        /** \brief reset current results */
        virtual void resetCurrent();
        /** \brief reset all runs in all files */
        virtual void resetAll();
        /** \brief reset all runs in current files */
        virtual void resetAllRuns();

        /** \brief copy the current fit parameters to the initial settings */
        virtual void  copyToInitial();
        /** \brief copy current values to all records and all runs therein */
        virtual void copyToAll();

        /** \brief copy current values to all runs in the current record */
        virtual void copyToAllRuns();

        /** \brief copy current values to all records and the current run therein */
        virtual void copyToAllCurrentRun();

        /** \brief this may be overwritten to copy more than the enumerated fit parameters to another (the specified) record */
        virtual void copyMoreData(QFRawDataRecord* record, int index, int model);
        /** \brief this may be overwritten to copy more than the enumerated fit parameters to the initial values */
        virtual void copyMoreDataToInitial();

        void plotMouseMove(double x, double y);
        void zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender);
        void runChanged(int run);
        void modelChanged(int model);
        void slidersChanged(int userMin, int userMax, int min, int max);
        void residualHistogramBinsChanged(int bins);
        void chkWeightedResidualsToggled(bool checked);
        void residualStyleChanged(int style);
        void errorStyleChanged(int style);
        void plotStyleChanged(int style);
        void chkGridToggled(bool checked);
        void chkXLogScaleToggled(bool checked);
        void fitParamChanged();

        /** \brief display the current evaluation results */
        virtual void displayParameters();
        virtual void displayData()=0;
        virtual void updateFitFunctions()=0;

    protected:

        void createWidgets();
        virtual int getUserMinAbsMax(QFRawDataRecord *rec, int index);

        /** \brief label displaying the current record */
        QLabel* labRecord;
        /** \brief combobox to select a model */
        QComboBox* cmbModel;
        /** \brief ploter for the data&curves */
        JKQtPlotter* pltData;
        /** \brief ploter for the residuals */
        JKQtPlotter* pltResiduals;
        /** \brief DataCutSliders */
        DataCutSliders* datacut;
        /** \brief Splitter between Plots  */
        QVisibleHandleSplitter* splitPlot;
        QVisibleHandleSplitter* splitPlots;
        QVBoxLayout* plotLayout;
        QWidget* wPlot;
        /** \brief splitter between pltData and possibly more plots */
        QVisibleHandleSplitter* splitMorePLot;

        /** \brief Splitter between Plots and Model */
        QVisibleHandleSplitter* splitModel;
        /** \brief QSpinBox that allows to select a special run */
        QSpinBox* spinRun;
        /** \brief label for the run name */
        QLabel* labRun;
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
        /** \brief toolbar above the plots */
        QToolBar* toolbar;
        /** \brief label to display the current position of the mouse cursor */
        QLabel* labMousePosition;
        /** \brief label that displays whether the current fit parameters are global or local */
        QLabel* labFitParameters;
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
        /** \brief widget for fit parameters  */
        QFSimpleFitParametersWidget* widFitParams;
        /** \brief scroll area for widFitParams */
        JKVerticalScrollArea* scrollParameters;

        QFormLayout* flAlgorithmParams;
        QHBoxLayout* hblModel;


        /** \brief when \c false the events that read the data from the widgets and write it to the QFEvaluationItem current are disabled */
        bool dataEventsEnabled;
        /** \brief stores the last fit statistics report, created in updateFitFunction() */
        QString fitStatisticsReport;






        /** \brief progress dialog used by the fits */
        QProgressDialog* dlgEvaluationProgress;

        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        
};

#endif // QFFCSBYINDEXANDMODELEVALUATIONEDITOR_H
