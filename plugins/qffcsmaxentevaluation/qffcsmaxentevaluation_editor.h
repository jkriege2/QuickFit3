/*
    Copyright (c) 2008-2014 Nikolas Schnellbächer &
                            Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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


        virtual void getPlotData(QFRawDataRecord* record, int index, QList<QFGetPlotdataInterface::GetPlotDataItem>& plotdata, int option);
        virtual bool getPlotDataSpecs(QStringList* optionNames=NULL, QList<QFGetPlotdataInterface::GetPlotPlotOptions>* listPlotOptions=NULL);



    protected slots:
        void alphaChanged(double alpha);
        void wxyChanged(double wxy);
        void nChanged(double wxy);
        void lambdaChanged(double wxy);
        void thetaChanged(double wxy);
        void weightsChanged(int weights);
        void NdistChanged(int Ndist);
        void NumIterChanged(int NumIter);
        void chkShowDChanged();
        void chkShowrangesChanged();
        void tauModeChanged(int weights);
        void tauMinMaxChanged();

        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        virtual void modelChanged(int model);


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


        QFDoubleEdit* edtTauMin;
        QFDoubleEdit* edtTauMax;
        QComboBox* cmbTauMode;

        QFDoubleEdit* edtRange1Min;
        QFDoubleEdit* edtRange1Max;
        QFDoubleEdit* edtRange2Min;
        QFDoubleEdit* edtRange2Max;
        QLabel* labRange1;
        QLabel* labRange2;

        QFDoubleEdit* edtWxy;
        QLabel* labWxy;

        QFDoubleEdit* edtRefIndx;
        QLabel* labRefIndx;
        QFDoubleEdit* edtTheta;
        QLabel* labTheta;
        QFDoubleEdit* edtLambda;
        QLabel* labLambda;

        QComboBox* cmbXAxisType;
        QCheckBox* chkShowRanges;
        QFRDRImageToRunPreview* pltOverview;

        QMenu* menuTools;

    private:
        void createWidgets();

};

#endif // QFFCSMAXENTEVALUATIONEDITOR_H
