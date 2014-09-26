/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFFCSMSDEvaluationEditor_H
#define QFFCSMSDEvaluationEditor_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include "statistics_tools.h"
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
#include "qfrdrfcsdatainterface.h"
#include "qfrdrrunselection.h"
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
#include "qfrdrfcsdatainterface.h"
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
#include "qffcsmsdevaluation_item.h"
#include "qfrdrimagetorunpreview.h"




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


        virtual void getPlotData(QFRawDataRecord* rec, int index, QList<QFGetPlotdataInterface::GetPlotDataItem>& plotdata, int option);
        virtual bool getPlotDataSpecs(QStringList* optionNames=NULL, QList<QFGetPlotdataInterface::GetPlotPlotOptions>* listPlotOptions=NULL);


    protected slots:
        void fitWidthChanged(int width);
        void fitTypeChanged(int type);
        void fitRangeChanged(bool range);
        void weightsChanged(int weights);
        void updateDistributionResults();
        void updateDistribution();

        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void plotDistMouseMoved(double x, double y);


        /** \brief activated when the highlighted record changed */
        virtual void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
        void slidersDistChanged(int userMin, int userMax, int min, int max);
        void updateSliders();
        void theoryChanged();
        void fitMSD();

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

        void averageFirstFewLags();
        void getNFromFits();
        void copyAverageData();
        void fitAllMSD();

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
        QFFCSWeightingCombobox* cmbWeights;
        /** \brief ploter for the MaxEnt Distribution */
        QFPlotter* pltDistribution;
        QToolBar* tbPlot;
        QToolBar* tbPlotDistribution;
        QToolBar* tbPlotDistResults;
        QVisibleHandleSplitter* splitterDist;
        DataCutSliders* sliderDist;
        QFPlotter* pltDistResults;
        QComboBox* cmbDistResultsMode;
        QComboBox* cmbFitType;

        QFDoubleEdit* numPre[MSDTHEORYCOUNT];
        QFDoubleEdit* numD[MSDTHEORYCOUNT];
        QFDoubleEdit* numAlpha[MSDTHEORYCOUNT];
        QCheckBox* chkSlope[MSDTHEORYCOUNT];
        QToolButton* btnFitMSD[MSDTHEORYCOUNT];
        QSpinBox* spinFitWidth;
        QCheckBox* chkShowKeyDist;
        QCheckBox* chkShowKeyDistResults;
        QCheckBox* chkShowKey;
        QCheckBox* chkFitRange;

        QAction* actAverageFirstLags;
        QAction* actGetNFromFits;
        QAction* actCopyAverageData;
        QAction* actFitAllMSD;
        QAction* actUpdateCalculated;
        JKQTPoverlayVerticalRange* ovlDistRange;
        JKQTPoverlayLine* ovlModel;
        QFRDRImageToRunPreview* pltOverview;

    private:
        void createWidgets();
        QWidget *createSlopeWidgets(int i);

};

#endif // QFFCSMSDEvaluationEditor_H
