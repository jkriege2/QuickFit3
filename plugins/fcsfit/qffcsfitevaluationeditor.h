#ifndef QFFCSFITEVALUATIONEDITOR_H
#define QFFCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qt/jkqtplotter.h"
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
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "datacutslider.h"
#include "qfhtmlhelpwindow.h"
#include "qffitparameterwidget.h"
#include "jkverticalscrollarea.h"

/*! \brief editor class for FCS least-square fits
    \ingroup qf3evalp_fcsfit
*/
class QFFCSFitEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSFitEvaluationEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSFitEvaluationEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
        /** \brief used to reread all parameter widget values from the datastore */
        void updateParameterValues();
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
        QSplitter* splitPlot;
        /** \brief Splitter between Plots and Model */
        QSplitter* splitModel;
        /** \brief pushbutton to configure Fitting Algorithm */
        QPushButton* btnConfigAlgorithm;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QPushButton* btnAlgorithmHelp;
        /** \brief pushbutton to show help on Fitting Algorithm */
        QPushButton* btnModelHelp;
        /** \brief QSpinBox that allows to select a special run */
        QSpinBox* spinRun;
        /** \brief help window for help on the current fit algorithm */
        QFHTMLHelpWindow* hlpAlgorithm;
        /** \brief help window for help on the current fit model function */
        QFHTMLHelpWindow* hlpFunction;
        /** \brief combobox to select a plotting style */
        QComboBox* cmbPlotStyle;
        /** \brief combobox to select a plotting style for the data errors */
        QComboBox* cmbErrorStyle;
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
        /** \brief button to toggle editing ranges  */
        QPushButton* btnEditRanges;
        /** \brief plotter for residual distribution histogram */
        JKQtPlotter* pltResidualHistogram;
        /** \brief label for fit statistics */
        QTextEdit* txtFitStatistics;
        /** \brief label for fit statistics */
        QLabel* labFitResult;
        /** \brief spin box to set the number of bins in residuals histogram */
        QSpinBox* spinResidualHistogramBins;
        /** \brief splitter between residual statistics plotter pltResidualHistogram and fit results txtFitStatistics */
        QSplitter* splitFitStatistics;
        /** \brief layout used to configure the residuals evaluation */
        QFormLayout* layResidualAnalysis;
        /** \brief toolbox to switch between residual histogram and parameters */
        QTabWidget* tabResidulas;
        /** \button to fit current file */
        QPushButton* btnFitCurrent;

        /** \brief scroll area for the fit parameters */
        JKVerticalScrollArea* scrollParameters;
        /** \brief layout that is used to display fit parameters */
        QGridLayout* layParameters;

        /** \brief this list contains all currently displayed fitParameterWidgets */
        QList<QPointer<QFFitParameterWidget> > m_fitParameters;

        /** \brief width of the widgets in m_fitParameters */
        int m_parameterWidgetWidth;
        /** \brief width of the checkboxes in the parameter widgets */
        int m_parameterCheckboxWidth;


        /** \brief when \c false the events that read the data from the widgets and write it to the QFEvaluationItem current are disabled */
        bool dataEventsEnabled;
        /** \brief current save directory */
        QString currentSaveDirectory;


    protected slots:
        /** \brief executed when the mouse position over the plot changes */
        void plotMouseMove(double x, double y);
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);

        /** \brief display all data and parameters describing the current record */
        void displayModel(bool newWidget);

        /** \brief replot curves */
        void replotData();

        /*! \brief update the fit model function and the residuals in the graphs

            This function only adds the fit function plot objects and does NOT cause a replot of the graphs. It is called by
            replotData().
        */
        void updateFitFunctions();

        /*! \brief fit model to current data
         */
        void fitCurrent();

        /** \brief executed when the sliders values change */
        void slidersChanged(int userMin, int userMax, int min, int max);
        /** \brief emitted when the pltData rezooms */
        void zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, QWidget* sender);

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
        /** \brief emitted when we should save a report */
        void saveReport();
        /** \brief emitted when we should print a report */
        void printReport();
        /** \brief emitted when the x logscale checkbox changes its state */
        void chkXLogScaleToggled(bool checked);
        /** \brief emitted when the grid checkbox changes its state */
        void chkGridToggled(bool checked);
        /** \brief emitted when the plot style changes */
        void plotStyleChanged(int style);
        /** \brief emitted when the error plot style changes */
        void errorStyleChanged(int style);

        /** \brief executed when a parameter is changed by the user */
        void parameterValueChanged(QString id, double value);
        /** \brief executed when a parameter fix is changed by the user */
        void parameterFixChanged(QString id, bool fix);
        /** \brief executed when a parameter range is changed by the user */
        void parameterRangeChanged(QString id, double min, double max);


    private:
        /** \brief create all widgets on the form */
        void createWidgets();


};

#endif // QFFCSFITEVALUATIONEDITOR_H
