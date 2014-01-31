#ifndef QFFCSMAXENTEVALUATIONEDITOR_H
#define QFFCSMAXENTEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>

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
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <QTextDocument>
#include "datacutslider.h"
#include "jkverticalscrollarea.h"
#include "qvisiblehandlesplitter.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qfsimplefitparameterswidget.h"
#include "qfusesresultsbyindexevaluationeditor.h"
#include "qffcsbyindexandmodelevaluationeditor.h"


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
#include "qfdoubleedit.h"
#include "qffcsweightingtools.h"
#include "qfdoubleedit.h"
#include "qfrdrimagetorunpreview.h"
////////////


/*! \brief editor for QFFCSMaxEntEvaluationItem
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFFCSMaxEntEvaluationEditor : public QFFCSByIndexAndModelEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSMaxEntEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSMaxEntEvaluationEditor();



    protected slots:
        void alphaChanged(double alpha);
        void wxyChanged(double wxy);
        void weightsChanged(int weights);
        void NdistChanged(int Ndist);
        void NumIterChanged(int NumIter);
        void chkShowDChanged();
        void chkShowrangesChanged();

        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();


        /** \brief activated when the highlighted record changed */
        virtual void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);

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

        virtual void sumRangesChanged();

    protected:       
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);
        /** \brief this may be overwritten to copy more than the enumerated fit parameters to another (the specified) record */
        virtual void copyMoreData(QFRawDataRecord* record, int index, int model);
        /** \brief this may be overwritten to copy more than the enumerated fit parameters to the initial values */
        virtual void copyMoreDataToInitial();

        /** \brief number edit to change alpha */
        QFDoubleEdit* edtAlpha;
        /** \brief number edit to change Ndist */
        QSpinBox* edtNdist;
        /** \brief number edit to change NumIter */
        QSpinBox* edtNumIter;
        /** \brief combobox to select a model for the weighting */
        QFFCSWeightingCombobox* cmbWeights;
        /** \brief ploter for the MaxEnt Distribution */
        QFPlotter* pltDistribution;
        QToolBar* tbPlot;
        QToolBar* tbPlotDistribution;

        QFDoubleEdit* edtRange1Min;
        QFDoubleEdit* edtRange1Max;
        QFDoubleEdit* edtRange2Min;
        QFDoubleEdit* edtRange2Max;
        QLabel* labRange1;
        QLabel* labRange2;

        QFDoubleEdit* edtWxy;
        QComboBox* cmbXAxisType;
        QCheckBox* chkShowRanges;
        QFRDRImageToRunPreview* pltOverview;

    private:
        void createWidgets();

};

#endif // QFFCSMAXENTEVALUATIONEDITOR_H
