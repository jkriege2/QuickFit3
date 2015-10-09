/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFFITRESULTSBYINDEXEVALUATIONEDITORWITHWIDGETS_H
#define QFFITRESULTSBYINDEXEVALUATIONEDITORWITHWIDGETS_H
#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qfplotter.h"
#include "cpptools.h"
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
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
//#include "qfhtmlhelpwindow.h"
#include "qffitparameterwidget.h"
#include "jkverticalscrollarea.h"
#include "dlgqffitalgorithmprogressdialog.h"
#include "qffitalgorithmthreaddedfit.h"
#include "qvisiblehandlesplitter.h"
#include "qffitresultsevaluationeditorbase.h"
#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qffitresultsbyindexevaluation.h"
#include "qffitresultsevaluation.h"
#include "qffitresultsbyindexevaluationeditorbase.h"
#include "qftools.h"
#include "qfevaluationpropertyeditor.h"
#include "qffitresultsbyindexevaluationfitthread.h"
#include "qfrdrrunselection.h"
#include "qffitfunctioncombobox.h"
#include "qfgetplotdatainterface.h"
/*! \brief evaluation item editor base class for data fits using QFFitAlgorithm and QFFitFunction where the
           QFRawDataRecord is a dataset with different runs (we can fit to each run). The QFEvaluationItem for
           this editor mus cast to QFFitResultsByIndexEvaluation.
    \ingroup qf3evaluationplugins


*/
class QFFitResultsByIndexEvaluationEditorWithWidgets : public QFFitResultsByIndexEvaluationEditorBase
{
        Q_OBJECT
    public:
        explicit QFFitResultsByIndexEvaluationEditorWithWidgets(QString iniPrefix, QFEvaluationPropertyEditor* propEditor, QFPluginServices* services, QWidget *parent = 0, bool hasMultiThreaded=false, bool multiThreadPriority=false, const QString& runName=QString("run"), bool useRunComboBox=false);


    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectDefaultWidgets(QFEvaluationItem* current, QFEvaluationItem* old, bool updatePlots);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
        /** \brief used to reread all parameter widget values from the datastore */
        virtual void updateParameterValues(QFRawDataRecord* rec);

        /** \brief return the datacutslider absolute maximum */
        virtual int getUserRangeMax(QFRawDataRecord* rec, int index)=0;
        /** \brief return the datacutslider absolute minimum */
        virtual int getUserRangeMin(QFRawDataRecord* rec, int index)=0;


    protected:
        virtual QString getPlotXLabel() const;
        virtual QString getPlotYLabel() const;
        virtual QString getFitName() const;
        virtual bool getPlotXLog() const;
        virtual bool getPlotYLog() const;

        void setGuessingEnabled(bool enabled=true);


        QString m_runName;
        /** \brief label displaying the current record */
        QLabel* labRecord;
        /** \brief combobox to select a fitting algorithm */
        QComboBox* cmbAlgorithm;
        /** \brief combobox to select a model */
        QFFitFunctionComboBox* cmbModel;

        QHBoxLayout* layAlgorithm;
        QHBoxLayout* layAfterAlgorithm;
        QHBoxLayout* layModel;
        QHBoxLayout* layAfterModel;

        /** \brief ploter for the data&curves */
        QFPlotter* pltData;
        /** \brief ploter for the residuals */
        QFPlotter* pltResiduals;
        /** \brief DataCutSliders */
        DataCutSliders* datacut;
        /** \brief Splitter between Plots  */
        QVisibleHandleSplitter* splitPlot;
        /** \brief Splitter between Plots and Model */
        QVisibleHandleSplitter* splitModel;

        /** \brief QSpinBox that allows to select a special run */
        QSpinBox* spinRun;
        QFEnhancedComboBox* cmbRun;
        /** \brief label for the run name */
        QLabel* labRun;
        QLabel* labRunLabel;

        /** \brief combobox to select a plotting style */
        QComboBox* cmbPlotStyle;
        /** \brief combobox to select a plotting style for the data errors */
        JKQTPerrorPlotstyleComboBox* cmbErrorStyle;
        /** \brief combobox to select a plotting style for the residulas */
        QComboBox* cmbResidualStyle;
        /** \brief combobox to select a type for the residulas */
        QCheckBox* chkWeightedResiduals;
        /** \brief checkbox for x logscale */
        QCheckBox* chkXLogScale;
        /** \brief checkbox to display grid */
        QCheckBox* chkGrid;
        /** \brief checkbox to display key */
        QCheckBox* chkKey;
        /** \brief toolbar above the plots */
        QToolBar* toolbar;
        /** \brief label to display the current position of the mouse cursor */
        QLabel* labMousePosition;
        /** \brief label that displays whether the current fit parameters are global or local */
        QLabel* labFitParameters;
        /** \brief widget to switch between editing fit parameter values and ranges  */
        QTabBar* tbEditRanges;
        //QToolButton* btnEditRanges;
        /** \brief plotter for residual distribution histogram */
        QFPlotter* pltResidualHistogram;
        /** \brief plotter for residual autocorrelation */
        QFPlotter* pltResidualCorrelation;
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
        QAction* actFitCurrent;
        /** \brief to guess current file */
        QAction* actGuessCurrent;
        /** \brief to fit all runs in current file */
        QAction* actFitRunsCurrent;
        /** \brief to fit current run in all file */
        QAction* actFitAll;
        /** \brief the separator that follows the fits in menuFit */
        QAction* actFitSeparator;
        /** \brief to fit all runs in all file */
        QAction* actFitRunsAll;
        /** \brief reset current parameter set to default */
        QAction* actResetCurrent;
        /** \brief reset all parameter sets to default */
        QAction* actResetAll;
        /** \brief reset all runs in the current file to default */
        QAction* actResetAllRuns;
        /** \brief copy current parameter set to default parameters */
        QAction* actCopyToInitial;
        /** \brief copy current parameter set to all files & initial parameters */
        QAction* actCopyToAll;
        /** \brief copy current parameter set to all runs in the current file & initial parameters */
        QAction* actCopyToAllRuns;
        /** \brief copy current parameter set to all files (but therein just the current run & initial parameters */
        QAction* actCopyToAllCurrentRun;

        /** \brief button to save current parameter set */
        QAction* actSaveParameters;
        /** \brief button to load current parameter set */
        QAction* actLoadParameters;
        /** \brief pushbutton to configure Fitting Algorithm */
        QAction* actConfigAlgorithm;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QAction* actAlgorithmHelp;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QAction* actModelHelp;
        QAction* actModelSelector;
        QAction* actFitAllFilesThreaded;
        QAction* actFitAllThreaded;
        QAction* actFitAllRunsThreaded;
        QAction* actChi2Landscape;
        //QAction* actOverlayPlot;

        QMenu* menuFit;
        QMenu* menuParameters;
        QMenu* menuResults;
        QMenu* menuTools;



        QGridLayout* layButtons;

        /** \brief scroll area for the fit parameters */
        JKVerticalScrollArea* scrollParameters;
        /** \brief layout that is used to display fit parameters */
        QGridLayout* layParameters;
        /** \brief widget containing all fit parameter edit/display fields */
        QWidget* widParameters;

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

        bool m_multithreadPriority;
        bool m_hasMultithread;


        /** \brief when \c false the events that read the data from the widgets and write it to the QFEvaluationItem current are disabled */
        bool dataEventsEnabled;
        /** \brief stores the last fit statistics report, created in updateFitFunction() */
        QString fitStatisticsReport;


    protected:
        virtual void populateFitButtons(bool mulThreadEnabledInModel=true);
        virtual void fillRunCombo(QFFitResultsByIndexEvaluation *eval, QFRawDataRecord *rdr);
        int getCurrentRunFromWidget() const;

        /** \brief to fit current file */
        QToolButton* btnFitCurrent;
        /** \brief to guess current file */
        QToolButton* btnGuessCurrent;
        /** \brief to fit all runs in current file */
        QToolButton* btnFitRunsCurrent;
        /** \brief to fit current run in all file */
        QToolButton* btnFitAll;
        /** \brief to fit all runs in all file */
        QToolButton* btnFitRunsAll;
        /** \brief reset current parameter set to default */
        QToolButton* btnResetCurrent;
        /** \brief reset all parameter sets to default */
        QToolButton* btnResetAll;
        /** \brief reset all runs in the current file to default */
        QToolButton* btnResetAllRuns;
        /** \brief copy current parameter set to default parameters */
        QToolButton* btnCopyToInitial;
        /** \brief copy current parameter set to all files & initial parameters */
        QToolButton* btnCopyToAll;
        /** \brief copy current parameter set to all runs in the current file & initial parameters */
        QToolButton* btnCopyToAllRuns;
        /** \brief copy current parameter set to all files (but therein just the current run & initial parameters */
        QToolButton* btnCopyToAllCurrentRun;
        /** \brief pushbutton to configure Fitting Algorithm */
        QToolButton* btnConfigAlgorithm;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QToolButton* btnAlgorithmHelp;
        /** \brief pushbutton to show help on Fitting Model */
        QToolButton* btnModelHelp;
        /** \brief pushbutton to select a Fitting Model */
        QToolButton* btnModelSelector;
        /** \brief button to save current parameter set */
        QToolButton* btnSaveParameters;
        /** \brief button to load current parameter set */
        QToolButton* btnLoadParameters;

        QToolButton* btnFirstRun;

    protected slots:
        /** \brief executed when the mouse position over the plot changes */
        void plotMouseMove(double x, double y);
        /** \brief activated when the highlighted record changed */
        virtual void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);

        /** \brief display all data and parameters describing the current record */
        virtual void displayModel(bool newWidget);

        /** \brief replot curves */
        virtual void replotData()=0;

        /*! \brief update the fit model function and the residuals in the graphs

            This function only adds the fit function plot objects and does NOT cause a replot of the graphs. It is called by
            replotData().
        */
        virtual void updateFitFunctions()=0;

        /** \brief executed when the sliders values change */
        void slidersChanged(int userMin, int userMax, int min, int max);
        /** \brief emitted when the pltData rezooms */
        void zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender);

        /** \brief display fit algorithm help */
        void displayFitAlgorithmHelp();
        /** \brief configure the fitting algorithm */
        void configFitAlgorithm();
        /** \brief called when the current run changes */
        void runChanged(int run);
        void runCmbChanged(int run);
        /** \brief called when the fit model changes */
        void modelChanged(int model);
        /** \brief called when the fit algorithm changes */
        void algorithmChanged(int model);

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
        /** \brief executed when the user presses enter in a parameter field */
        void parameterValueEnterPressed();
        /** \brief executed when a parameter fix is changed by the user */
        void parameterFixChanged();
        /** \brief executed when a parameter range is changed by the user */
        void parameterRangeChanged();

        virtual void plotChi2Landscape();

        virtual void gotoFirstRun();




        void chkKeyToggled(bool checked);
    public slots:
        /*! \brief fit model to current data
         */
        virtual void fitCurrent();
        /*! \brief guess model-parameters to current data
         */
        virtual void guessCurrent();
        /** \brief fit all files (current run) */
        virtual void fitAll();
        /** \brief fit all files, all runs */
        virtual void fitRunsAll();
        /** \brief fit all runs in current file */
        virtual void fitRunsCurrent();

        virtual void fitEverythingThreaded();
        virtual void fitAllRunsThreaded();
        virtual void fitAllFilesThreaded();

    private:
        /** \brief create all widgets on the form */
        void createWidgets(bool hasMultiThreaded, bool multiThreadPriority, bool useRunCombobox);
};

#endif // QFFITRESULTSBYINDEXEVALUATIONEDITORWITHWIDGETS_H
