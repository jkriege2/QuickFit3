#ifndef QFFCSMSDEvaluationEditor_H
#define QFFCSMSDEvaluationEditor_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include "statistics_tools.h"
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
#include "qffcsbyindexandmodelevaluationeditor.h"
#include "lmcurve.h"

/////////////
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include <iostream>
#include <cfloat>
#include "tools.h"
#include "statistics_tools.h"
#include <QThread>
#include "qmoretextobject.h"
#include "qmodernprogresswidget.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
////////////




/*! \brief editor for QFFCSMSDEvaluationItem
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFFCSMSDEvaluationEditor : public QFFCSByIndexAndModelEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSMSDEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSMSDEvaluationEditor();



    protected slots:
        void fitWidthChanged(int width);
        void weightsChanged(int weights);
        void updateDistributionResults();
        void updateDistribution();

        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();


        /** \brief activated when the highlighted record changed */
        virtual void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
        void slidersDistChanged(int userMin, int userMax, int min, int max);
        void updateSliders();
        void theoryChanged();

        /** \brief evaluate all files */
        virtual void fitRunsAll();
        /** \brief evaluate current file */
        virtual void fitCurrent();
        /** \brief display the current evaluation results */
        virtual void fitAll();

        virtual void fitRunsCurrent();

        virtual void displayData();
        virtual void updateFitFunctions();
        virtual void displayParameters();
        void distzoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender);

    protected:       
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);
        /** \brief this may be overwritten to copy more than the enumerated fit parameters to another (the specified) record */
        virtual void copyMoreData(QFRawDataRecord* record, int index, int model);
        /** \brief this may be overwritten to copy more than the enumerated fit parameters to the initial values */
        virtual void copyMoreDataToInitial();
        void setMSDMax(int MSDMax);
        void setMSDMin(int MSDMin);
        int getMSDMax(QFRawDataRecord *rec, int index);
        int getMSDMin(QFRawDataRecord *rec, int index);
        int getMSDMax(QFRawDataRecord *rec);
        int getMSDMin(QFRawDataRecord *rec);
        int getMSDMax(QFRawDataRecord *rec, int index, int defaultMax);
        int getMSDMin(QFRawDataRecord *rec, int index, int defaultMin);


        /** \brief combobox to select a model for the weighting */
        QComboBox* cmbWeights;
        /** \brief ploter for the MaxEnt Distribution */
        JKQtPlotter* pltDistribution;
        QToolBar* tbPlot;
        QToolBar* tbPlotDistribution;
        QToolBar* tbPlotDistResults;
        QVisibleHandleSplitter* splitterDist;
        DataCutSliders* sliderDist;
        JKQtPlotter* pltDistResults;
        QComboBox* cmbDistResultsMode;

        QFDoubleEdit* numPre[3];
        QFDoubleEdit* numD[3];
        QFDoubleEdit* numAlpha[3];
        QCheckBox* chkSlope[3];
        QSpinBox* spinFitWidth;
        QCheckBox* chkShowKeyDist;
        QCheckBox* chkShowKeyDistResults;
        QCheckBox* chkShowKey;

    private:
        void createWidgets();
        QWidget *createSlopeWidgets(int i);

};

#endif // QFFCSMSDEvaluationEditor_H
