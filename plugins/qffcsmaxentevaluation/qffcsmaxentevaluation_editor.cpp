/*
    Copyright (c) 2008-2014 Nikolas Schnellb�cher &
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


#include "qffcsmaxentevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qffcsmaxentevaluation_item.h"

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
#include "qfselectionlistdialog.h"
#include "csvtools.h"
#include "datatools.h"
////////////


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>

QFFCSMaxEntEvaluationEditor::QFFCSMaxEntEvaluationEditor(QFPluginServices *services, QFEvaluationPropertyEditor *propEditor, QWidget *parent):
    QFFCSByIndexAndModelEvaluationEditor(services, propEditor, parent, "fcsmaxentdistribution/")
{
    dataEventsEnabled=false;
    createWidgets();
    dataEventsEnabled=true;
}

QFFCSMaxEntEvaluationEditor::~QFFCSMaxEntEvaluationEditor()
{
}

void QFFCSMaxEntEvaluationEditor::getPlotData(QFRawDataRecord *record, int index, QList<QFGetPlotdataInterface::GetPlotDataItem> &plotdata, int option, const QString &optionName)
{
    Q_UNUSED(optionName);
    if (!current) return;
    if (!cmbModel) return;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    int model=eval->getCurrentModel();

    if ((!eval)||(!data)) {
        return;
    }


    int datacut_min=getUserMin(record, index);
    int datacut_max=getUserMax(record, index);


    try {

        if (data->getCorrelationN()>0) {

            /////////////////////////////////////////////////////////////////////////////////
            // retrieve data and tau-values from rawdata record
            /////////////////////////////////////////////////////////////////////////////////
            long N=data->getCorrelationN();
            double* tauvals=data->getCorrelationT();
            double* corrdata=NULL;

            if (index<0) {
                corrdata=data->getCorrelationMean();
            } else {
                if (index<(int)data->getCorrelationRuns()) {
                    corrdata=data->getCorrelationRun(index);
                } else {
                    corrdata=data->getCorrelationMean();
                }
            }

            bool weightsOK=false;
            double* weights=eval->allocWeights(&weightsOK, record, index, datacut_min, datacut_max, true);


            /////////////////////////////////////////////////////////////////////////////////
            // calculate fit statistics
            /////////////////////////////////////////////////////////////////////////////////
            double* modelVals=(double*)qfMalloc(N*sizeof(double));
            QVector<double> dist=eval->getDistribution(record, index, model);
            QVector<double> distTau=eval->getDistributionTaus(record, index, model);

            eval->evaluateModel(record, index, model, tauvals, modelVals, N, distTau.data(), dist.data(), dist.size());


            /////////////////////////////////////////////////////////////////////////////////
            // plot distribution
            /////////////////////////////////////////////////////////////////////////////////
            QVector<double> mem_tau=eval->getDistributionTaus(record, index, model);
            QVector<double> mem_D=eval->getDistributionDs(record, index, model);
            QVector<double> mem_dist=eval->getDistribution(record, index, model);




            if (option==0) {
                QFGetPlotdataInterface::GetPlotDataItem item;
                item.x=mem_tau;
                item.y=mem_dist;
                item.name=QString("\\verb{%1: p(\\tau_D})").arg(record->getName());
                plotdata.append(item);
            } else if (option==1) {
                QFGetPlotdataInterface::GetPlotDataItem item;
                item.x=mem_D;
                item.y=mem_dist;
                item.name=QString("\\verb{%1: p(D)}").arg(record->getName());
                plotdata.append(item);
            } else if (option==2) {
                QFGetPlotdataInterface::GetPlotDataItem item;
                item.x=arrayToVector(tauvals, N);
                item.y=arrayToVector(corrdata, N);
                if (weightsOK && weights) item.yerrors=arrayToVector(weights, N);
                item.name=QString("\\verb{%1: ACF}").arg(record->getName());
                plotdata.append(item);
            }

            qfFree(weights);

        }
    } catch(std::exception& E) {
        services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
    }
}

bool QFFCSMaxEntEvaluationEditor::getPlotDataSpecs(QStringList *optionNames, QList<QFGetPlotdataInterface::GetPlotPlotOptions> *listPlotOptions)
{
    if (optionNames) {
        *optionNames<<tr("MaxEnt tauD-distribution");
        *optionNames<<tr("MaxEnt D-distribution");
        *optionNames<<tr("autocorrelation data");
        *optionNames<<tr("autocorrelation curves + fit");
    }
    if (listPlotOptions) {
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("diffusion time $\\tau_D$ [\\mu s]"), tr("MaxEnt distribution $p(\\tau_D)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("diffusion coefficient $D$\\mumu}m^2/s]"), tr("MaxEnt distribution $p(D)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time $\\tau$ [s]"), tr("correlation curve $g(\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time $\\tau$ [s]"), tr("correlation curve $g(\\tau)$"), true, false);
    }
    return true;
}


void QFFCSMaxEntEvaluationEditor::createWidgets() {
    QWidget* wrange;
    QHBoxLayout* lrange;
    edtWxy=new QFDoubleEdit(this);
    edtWxy->setRange(1e-3,1e10);
    flAlgorithmParams->addRow(labWxy=new QLabel(tr("<i>w<sub>xy</sub></i> [nm] <i>= </i>")), edtWxy);


    edtRefIndx=new QFDoubleEdit(this);
    edtRefIndx->setRange(0,1000);
    flAlgorithmParams->addRow(labRefIndx=new QLabel(tr("<i>n = </i>")), edtRefIndx);
    edtRefIndx->setVisible(false);
    labRefIndx->setVisible(false);

    edtTheta=new QFDoubleEdit(this);
    edtTheta->setRange(0,180);
    flAlgorithmParams->addRow(labTheta=new QLabel(QLatin1String("<i>&Theta;</i> [\xB0] <i>= </i>")), edtTheta);
    edtTheta->setVisible(false);
    labTheta->setVisible(false);

    edtLambda=new QFDoubleEdit(this);
    edtLambda->setRange(0,10000);
    flAlgorithmParams->addRow(labLambda=new QLabel(tr("<i>&lambda;</i> [nm] <i>= </i>")), edtLambda);
    edtLambda->setVisible(false);
    labLambda->setVisible(false);


    edtAlpha=new QFDoubleEdit(this);
    edtAlpha->setRange(0, DBL_MAX);
    edtAlpha->setCheckBounds(true, false);
    edtAlpha->setDecimals(10);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&alpha; = </i>"), edtAlpha);

////////////////////////////////////////
    edtNdist=new QSpinBox(this);
    edtNdist->setRange(10, INT_MAX);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&Ndist = </i>"), edtNdist);

    cmbTauMode=new QComboBox(this);
    cmbTauMode->addItem(tr("correlation tau-axis"));
    cmbTauMode->addItem(tr("custom logarithmic tau-axis"));
    cmbTauMode->addItem(tr("custom linear tau-axis"));
    cmbTauMode->addItem(tr("custom logarithmic diffusion coefficient axis"));
    cmbTauMode->addItem(tr("custom linear diffusion coefficient axis"));
    cmbTauMode->setCurrentIndex(0);
    flAlgorithmParams->addRow(tr("dist. type:"), cmbTauMode);

    wrange=new QWidget(this);
    lrange=new QHBoxLayout();
    wrange->setLayout(lrange);
    lrange->setContentsMargins(0,0,0,0);
    edtTauMin=new QFDoubleEdit(wrange);
    edtTauMin->setLogScale(true);
    edtTauMin->setValue(0.01);
    edtTauMin->setCheckBounds(false, false);

    edtTauMax=new QFDoubleEdit(wrange);
    edtTauMin->setLogScale(true);
    edtTauMin->setValue(1000);
    edtTauMax->setCheckBounds(false, false);

    lrange->addWidget(edtTauMin);
    lrange->addWidget(new QLabel(" ... "));
    lrange->addWidget(edtTauMax);
    flAlgorithmParams->addRow(tr("dist. range:"), wrange);



    edtNumIter=new QSpinBox(this);
    edtNumIter->setRange(10, INT_MAX);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&Iterations/Step = </i>"), edtNumIter);

    wrange=new QWidget(this);
    lrange=new QHBoxLayout();
    wrange->setLayout(lrange);
    lrange->setContentsMargins(0,0,0,0);
    edtRange1Min=new QFDoubleEdit(wrange);
    edtRange1Min->setLogScale(true);
    edtRange1Min->setValue(1e-5);
    edtRange1Min->setCheckBounds(false, false);

    edtRange1Max=new QFDoubleEdit(wrange);
    edtRange1Min->setLogScale(true);
    edtRange1Min->setValue(1e-4);
    edtRange1Max->setCheckBounds(false, false);

    lrange->addWidget(edtRange1Min);
    lrange->addWidget(new QLabel(" ... "));
    lrange->addWidget(edtRange1Max);
    flAlgorithmParams->addRow(tr("sum range 1:"), wrange);
    flAlgorithmParams->addRow("", labRange1=new QLabel(this));
    labRange1->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);

    wrange=new QWidget(this);
    lrange=new QHBoxLayout();
    wrange->setLayout(lrange);
    lrange->setContentsMargins(0,0,0,0);
    edtRange2Min=new QFDoubleEdit(wrange);
    edtRange2Min->setLogScale(true);
    edtRange2Min->setValue(1e-3);
    edtRange2Min->setCheckBounds(false, false);
    edtRange2Max=new QFDoubleEdit(wrange);
    edtRange2Min->setLogScale(true);
    edtRange2Min->setValue(1e-2);
    edtRange2Max->setCheckBounds(false, false);
    lrange->addWidget(edtRange2Min);
    lrange->addWidget(new QLabel(" ... "));
    lrange->addWidget(edtRange2Max);

    flAlgorithmParams->addRow(tr("sum range 2:"), wrange);
    flAlgorithmParams->addRow("", labRange2=new QLabel(this));
    labRange2->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard);
    ////////////////////////////////////////




    cmbWeights=new QFFCSWeightingCombobox(this);
    cmbWeights->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    cmbWeights->setEditable(false);
    cmbWeights->setMaximumWidth(150);
    cmbWeights->setMinimumWidth(150);
    QLabel* l=new QLabel(tr("&Weight Model: "), this);
    l->setBuddy(cmbWeights);
    hblModel->addWidget(l);
    hblModel->addWidget(cmbWeights);
    hblModel->addStretch();




    tbPlot=new QToolBar(QString("tbPlot"), this);
    tbPlot->setIconSize(QSize(16,16));
    tbPlot->addAction(pltData->get_plotter()->get_actSavePlot());
    pltData->get_plotter()->get_actSavePlot()->setToolTip(tr("save the correlation and residuals plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlot->addAction(pltData->get_plotter()->get_actPrint());
    pltData->get_plotter()->get_actPrint()->setToolTip(tr("print the correlation and residuals plot"));
    tbPlot->addAction(pltData->get_plotter()->get_actCopyPixelImage());
    pltData->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the correlation and residuals plot as an image to the system clipboard"));
    tbPlot->addSeparator();
    tbPlot->addAction(pltData->get_plotter()->get_actZoomAll());
    tbPlot->addAction(pltData->get_plotter()->get_actZoomIn());
    tbPlot->addAction(pltData->get_plotter()->get_actZoomOut());
    tbPlot->addSeparator();


    toolbar->removeAction(pltData->get_plotter()->get_actSavePlot());
    toolbar->removeAction(pltData->get_plotter()->get_actPrint());
    toolbar->removeAction(pltData->get_plotter()->get_actCopyPixelImage());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomAll());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomIn());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomOut());
    plotLayout->insertWidget(0, tbPlot);


    pltDistribution=new QFPlotter(true, this);
    pltDistribution->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltDistribution->useExternalDatastore(pltData->getDatastore());
    pltDistribution->set_displayToolbar(false);
    pltDistribution->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    pltDistribution->getXAxis()->set_labelFontSize(11);
    pltDistribution->getXAxis()->set_tickLabelFontSize(10);
    pltDistribution->getXAxis()->set_logAxis(true);
    pltDistribution->getYAxis()->set_axisLabel(tr("MaxEnt distribution"));
    pltDistribution->getYAxis()->set_labelFontSize(11);
    pltDistribution->getYAxis()->set_tickLabelFontSize(10);
    pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    pltDistribution->get_plotter()->setBorder(1,1,1,1);
    pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltDistribution->set_displayMousePosition(false);
    pltDistribution->get_plotter()->set_keyFontSize(9);
    //pltDistribution->get_plotter()->set_keyXMargin(2);
    //pltDistribution->get_plotter()->set_keyYMargin(2);


    tbPlotDistribution=new QToolBar(QString("tbPlotDistribution"), this);
    tbPlotDistribution->setIconSize(QSize(16,16));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actSavePlot());
    pltDistribution->get_plotter()->get_actSavePlot()->setToolTip(tr("save the distribution plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actPrint());
    pltDistribution->get_plotter()->get_actPrint()->setToolTip(tr("print the distribution plot "));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actCopyPixelImage());
    pltDistribution->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the distribution plot as an image to the system clipboard"));
    tbPlotDistribution->addSeparator();
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomAll());
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomIn());
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomOut());
    cmbXAxisType=new QComboBox(this);//(tr("show D-axis"));
    cmbXAxisType->addItem(tr("correlation time tau"));
    cmbXAxisType->addItem(tr("FCS/DLS diffusion coefficient D"));
    chkShowRanges=new QCheckBox(tr("ranges"), this);
    tbPlotDistribution->addSeparator();
    tbPlotDistribution->addWidget(new QLabel("x-axis: "));
    tbPlotDistribution->addWidget(cmbXAxisType);
    tbPlotDistribution->addWidget(chkShowRanges);

    QWidget* wPltDist=new QWidget(this);
    QVBoxLayout* layPltDist=new QVBoxLayout();
    wPltDist->setLayout(layPltDist);
    layPltDist->setContentsMargins(0,0,0,0);
    layPltDist->addWidget(tbPlotDistribution);
    layPltDist->addWidget(pltDistribution);
    splitMorePLot->addWidget(wPltDist);

    splitMorePLot->setCollapsible(splitMorePLot->indexOf(pltDistribution), false);

    pltOverview=new QFRDRImageToRunPreview(this);
    pltOverview->setMaskEditable(true);
    pltOverview->setSelectionEditable(true);
    tabResidulas->addTab(pltOverview, tr("Overview Image"));

    actCopyAverageData=new QAction(QIcon(":/copy.png"), tr("&copy runs-average of MaxEnt distributions"), this);
    connect(actCopyAverageData, SIGNAL(triggered()), this, SLOT(copyAverageData()));
    menuResults->addSeparator();
    menuResults->addAction(actCopyAverageData);


    menuTools=propertyEditor->addOrFindMenu("&Tools", 0);
    menuTools->addAction(menuOverlays->menuAction());
    /////
    connect(pltDistribution, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMove(double,double)));
    connect(cmbXAxisType, SIGNAL(currentIndexChanged(int)), this, SLOT(chkShowDChanged()));
    connect(chkShowRanges, SIGNAL(toggled(bool)), this, SLOT(chkShowrangesChanged()));
    /////

}










void QFFCSMaxEntEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    QFFCSByIndexAndModelEvaluationEditor::connectWidgets(current, old);

    if (old) {

        disconnect(edtWxy, SIGNAL(valueChanged(double)), this, SLOT(wxyChanged(double)));
        disconnect(edtTheta, SIGNAL(valueChanged(double)), this, SLOT(thetaChanged(double)));
        disconnect(edtRefIndx, SIGNAL(valueChanged(double)), this, SLOT(nChanged(double)));
        disconnect(edtLambda, SIGNAL(valueChanged(double)), this, SLOT(lambdaChanged(double)));
        disconnect(edtTheta, SIGNAL(valueChanged(double)), this, SLOT(thetaChanged(double)));
        disconnect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));
        disconnect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        disconnect(edtNdist, SIGNAL(valueChanged(int)),this,SLOT(NdistChanged(int)));
        disconnect(edtNumIter, SIGNAL(valueChanged(int)),this,SLOT(NumIterChanged(int)));

        disconnect(edtRange1Min, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));
        disconnect(edtRange1Max, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));
        disconnect(edtRange2Min, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));
        disconnect(edtRange2Max, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));

        disconnect(cmbTauMode, SIGNAL(currentIndexChanged(int)), this, SLOT(tauModeChanged(int)));
        disconnect(edtTauMin, SIGNAL(valueChanged(double)),this,SLOT(tauMinMaxChanged()));
        disconnect(edtTauMax, SIGNAL(valueChanged(double)),this,SLOT(tauMinMaxChanged()));
    }

    QFFCSMaxEntEvaluationItem* item=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    //QFFCSMaxEntEvaluationItem* item_old=qobject_cast<QFFCSMaxEntEvaluationItem*>(old);
    if (item) {
        dataEventsEnabled=false;

        edtTheta->setValue(item->getTheta());
        connect(edtTheta, SIGNAL(valueChanged(double)), this, SLOT(thetaChanged(double)));
        edtRefIndx->setValue(item->getRefIndx());
        connect(edtRefIndx, SIGNAL(valueChanged(double)), this, SLOT(nChanged(double)));
        edtLambda->setValue(item->getLambda());
        connect(edtLambda, SIGNAL(valueChanged(double)), this, SLOT(lambdaChanged(double)));


        edtWxy->setValue(item->getWXY());
        connect(edtWxy, SIGNAL(valueChanged(double)), this, SLOT(wxyChanged(double)));

        edtAlpha->setValue(item->getAlpha());
        connect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));

        edtNdist->setValue(item->getNdist());
        connect(edtNdist, SIGNAL(valueChanged(int)), this, SLOT(NdistChanged(int)));

        edtNumIter->setValue(item->getNumIter());
        connect(edtNumIter, SIGNAL(valueChanged(int)), this, SLOT(NumIterChanged(int)));

        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));


        cmbXAxisType->setCurrentIndex(current->getProperty("show_daxis", 0).toInt());
        chkShowRanges->setChecked(current->getProperty("show_ranges", true).toBool());

        edtRange1Min->setValue(current->getProperty(QString("rdr%1_range1min").arg(item->getHighlightedRecord()?item->getHighlightedRecord()->getID():0), current->getProperty("range1min", 1e-5)).toDouble());
        edtRange1Max->setValue(current->getProperty(QString("rdr%1_range1max").arg(item->getHighlightedRecord()?item->getHighlightedRecord()->getID():0), current->getProperty("range1max", 1e-4)).toDouble());
        edtRange2Min->setValue(current->getProperty(QString("rdr%1_range2min").arg(item->getHighlightedRecord()?item->getHighlightedRecord()->getID():0), current->getProperty("range2min", 1e-3)).toDouble());
        edtRange2Max->setValue(current->getProperty(QString("rdr%1_range2max").arg(item->getHighlightedRecord()?item->getHighlightedRecord()->getID():0), current->getProperty("range2max", 1e-2)).toDouble());
        connect(edtRange1Min, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));
        connect(edtRange1Max, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));
        connect(edtRange2Min, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));
        connect(edtRange2Max, SIGNAL(valueChanged(double)),this,SLOT(sumRangesChanged()));

        edtTauMin->setValue(item->getTauMin());
        edtTauMax->setValue(item->getTauMax());
        cmbTauMode->setCurrentIndex(item->getTauMode());
        connect(cmbTauMode, SIGNAL(currentIndexChanged(int)), this, SLOT(tauModeChanged(int)));
        connect(edtTauMin, SIGNAL(valueChanged(double)),this,SLOT(tauMinMaxChanged()));
        connect(edtTauMax, SIGNAL(valueChanged(double)),this,SLOT(tauMinMaxChanged()));
        sumRangesChanged();

        dataEventsEnabled=true;


        if (item->propertyExists("PRESET_MODEL")) {
            cmbModel->setCurrentIndex(item->getProperty("PRESET_MODEL", cmbModel->currentIndex()).toInt());
            item->deleteProperty("PRESET_MODEL");
        }
    }

    /*
    /////
    connect(pltDistribution, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMove(double,double)));
    /////
    */
}


void QFFCSMaxEntEvaluationEditor::readSettings() {
    QFFCSByIndexAndModelEvaluationEditor::readSettings();
};

void QFFCSMaxEntEvaluationEditor::writeSettings() {
    QFFCSByIndexAndModelEvaluationEditor::writeSettings();

}

void QFFCSMaxEntEvaluationEditor::modelChanged(int model)
{
    QFFCSByIndexAndModelEvaluationEditor::modelChanged(model);
    edtWxy->setVisible(model!=3);
    labWxy->setVisible(model!=3);
    edtLambda->setVisible(model==3);
    labLambda->setVisible(model==3);
    edtTheta->setVisible(model==3);
    labTheta->setVisible(model==3);
    edtRefIndx->setVisible(model==3);
    labRefIndx->setVisible(model==3);
}



void QFFCSMaxEntEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    //QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(currentRecord);

    disconnect(pltOverview, SIGNAL(currentRunChanged(int)), spinRun, SLOT(setValue(int)));
    disconnect(spinRun, SIGNAL(valueChanged(int)), pltOverview, SLOT(setCurrentRun(int)));
    if (data && eval) {
        bool oldde=dataEventsEnabled;
        dataEventsEnabled=false;

        if (dataImg) {
            pltOverview->setRDR(currentRecord);
            connect(pltOverview, SIGNAL(currentRunChanged(int)), spinRun, SLOT(setValue(int)));
            connect(spinRun, SIGNAL(valueChanged(int)), pltOverview, SLOT(setCurrentRun(int)));
            tabResidulas->setTabEnabled(tabResidulas->indexOf(pltOverview), true);
        } else {
            pltOverview->setRDR(0);
            tabResidulas->setTabEnabled(tabResidulas->indexOf(pltOverview), false);
        }

        edtWxy->setValue(eval->getWXY());
        edtLambda->setValue(eval->getLambda());
        edtRefIndx->setValue(eval->getRefIndx());
        edtTheta->setValue(eval->getTheta());
        edtAlpha->setValue(eval->getAlpha());
        edtTauMin->setValue(eval->getTauMin());
        edtTauMax->setValue(eval->getTauMax());
        cmbTauMode->setCurrentIndex(eval->getTauMode());
        cmbWeights->setCurrentWeight(eval->getFitDataWeighting());
        edtNdist->setRange(10,data->getCorrelationN()*10); //qMax(0,data->getCorrelationN())
        edtNdist->setValue(eval->getNdist());
        edtNdist->setSuffix(QString(" / %1").arg(data->getCorrelationN()));
        edtNumIter->setRange(1,10000); //qMax(0,data->getCorrelationN())
        edtNumIter->setValue(eval->getNumIter());
        edtRange1Min->setValue(current->getProperty(QString("rdr%1_range1min").arg(eval->getHighlightedRecord()?eval->getHighlightedRecord()->getID():0), current->getProperty("range1min", 1e-5)).toDouble());
        edtRange1Max->setValue(current->getProperty(QString("rdr%1_range1max").arg(eval->getHighlightedRecord()?eval->getHighlightedRecord()->getID():0), current->getProperty("range1max", 1e-4)).toDouble());
        edtRange2Min->setValue(current->getProperty(QString("rdr%1_range2min").arg(eval->getHighlightedRecord()?eval->getHighlightedRecord()->getID():0), current->getProperty("range2min", 1e-3)).toDouble());
        edtRange2Max->setValue(current->getProperty(QString("rdr%1_range2max").arg(eval->getHighlightedRecord()?eval->getHighlightedRecord()->getID():0), current->getProperty("range2max", 1e-2)).toDouble());
        sumRangesChanged();
        modelChanged(eval->getCurrentModel());
        dataEventsEnabled=oldde;
    }
    QFFCSByIndexAndModelEvaluationEditor::highlightingChanged(formerRecord, currentRecord);


}


void QFFCSMaxEntEvaluationEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    QFFCSWeightingTools* wdata=dynamic_cast<QFFCSWeightingTools*>(current.data());
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    JKQTPdatastore* dsdist=pltDistribution->getDatastore();

    if ((!eval)||(!data)) {
        pltData->clearGraphs();
        ds->clear();
        return;
    } else {

        //qDebug()<<" **** displayData()";
        QTime t, t1;
        t.start();
        t1.start();

        //bool updEn=updatesEnabled();
        //setUpdatesEnabled(false);

        pltResiduals->set_doDrawing(false);
        pltResiduals->set_emitSignals(false);
        pltResiduals->clearGraphs();
        pltData->set_doDrawing(false);
        pltData->set_emitSignals(false);
        pltData->clearGraphs();
        pltResidualHistogram->set_doDrawing(false);
        pltResidualHistogram->set_emitSignals(false);
        pltResidualHistogram->clearGraphs();
        pltResidualCorrelation->set_doDrawing(false);
        pltResidualCorrelation->set_emitSignals(false);
        pltResidualCorrelation->clearGraphs();
        pltDistribution->set_doDrawing(false);
        pltDistribution->set_emitSignals(false);
        pltDistribution->clearGraphs();
        dsres->clear();
        ds->clear();
        dsresh->clear();
        dsresc->clear();
        dsdist->clear();

        //qDebug()<<"   a "<<t.elapsed()<<" ms";
        t.start();

        pltResiduals->getXAxis()->set_logAxis(chkXLogScale->isChecked());
        pltData->getXAxis()->set_logAxis(chkXLogScale->isChecked());
        if (chkXLogScale->isChecked()) {
            pltData->getXAxis()->set_minorTicks(9);
            pltResiduals->getXAxis()->set_minorTicks(9);
        } else {
            pltData->getXAxis()->set_minorTicks(1);
            pltResiduals->getXAxis()->set_minorTicks(1);
        }



        pltResiduals->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltResiduals->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltResiduals->get_plotter()->set_showKey(chkKeyVisible->isChecked());
        pltData->get_plotter()->set_showKey(chkKeyVisible->isChecked());
        pltDistribution->get_plotter()->set_showKey(chkKeyVisible->isChecked());
        pltData->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltData->getYAxis()->set_drawGrid(chkGrid->isChecked());

        pltDistribution->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltDistribution->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualHistogram->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualHistogram->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualCorrelation->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltResidualCorrelation->getYAxis()->set_drawGrid(chkGrid->isChecked());



        pltData->getYAxis()->set_minTicks(5);
        pltResiduals->getYAxis()->set_minTicks(5);

        //qDebug()<<"   b "<<t.elapsed()<<" ms";
        t.start();

        //int errorStyle=cmbErrorStyle->currentIndex();
        int plotStyle=cmbPlotStyle->currentIndex();
        //int residualStyle=cmbResidualStyle->currentIndex();

        if (data->getCorrelationN()>0) {
            size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");

        //qDebug()<<record->getName()<<":\n"<<eval->getCurrentIndex()<<data->getCorrelationRuns()<<data->getCorrelationN();

            //////////////////////////////////////////////////////////////////////////////////
            // Plot average + error markers
            //////////////////////////////////////////////////////////////////////////////////
            size_t c_mean=0;
            QString graphName="";
            int c_std=-1;
            QString errorName="";
            if (eval->getCurrentIndex()<0) {
                c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                graphName=tr("\\verb{%1} average").arg(record->getName());
                //c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                //errorName=tr("stddev");
            } else {
                if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                    c_mean=ds->addColumn(data->getCorrelationRun(eval->getCurrentIndex()), data->getCorrelationN(), QString("run"+QString::number(eval->getCurrentIndex())));
                    graphName=tr("\\verb{%1} %2").arg(record->getName()).arg(data->getCorrelationRunName(eval->getCurrentIndex()));
                    /*if (eval->getFitDataWeighting()==QFFCSWeightingTools::RunErrorWeighting) {
                        c_std=ds->addColumn(data->getCorrelationRunError(eval->getCurrentIndex()), data->getCorrelationN(), "cperrunerror");
                        errorName=tr("per run");
                    } else {
                        c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                        errorName=tr("stddev");
                    }*/
                } else {
                    c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                    graphName=tr("\\verb{%1} average").arg(record->getName());
                    //c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                    //errorName=tr("stddev");
                }
            }
            if (wdata) {
                bool wok=false;
                double* weigm=wdata->allocWeights(&wok, record, eval->getCurrentIndex());
                if (wok && weigm && eval->getFitDataWeighting()!=QFFCSWeightingTools::EqualWeighting) {
                    errorName=wdata->dataWeightToName(eval->getFitDataWeighting(), tr("run"));
                    c_std=ds->addCopiedColumn(weigm, data->getCorrelationN(), QString("cerr_")+wdata->dataWeightToString(eval->getFitDataWeighting()));
                    qfFree(weigm);
                }
            }
            JKQTPerrorPlotstyle styl=cmbErrorStyle->getErrorStyle();

            //qDebug()<<"   c "<<t.elapsed()<<" ms";
            t.start();

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(pltData->get_plotter());
            QColor gcolor=pltData->get_plotter()->get_graphColor();
            g->set_color(gcolor);
            g->set_errorColor(gcolor.lighter());
            g->set_lineWidth(2);
            g->set_symbolSize(8);
            g->set_symbolWidth(1);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(graphName);
            g->set_yErrorStyle(styl);
            g->set_xErrorStyle(JKQTPnoError);
            g->set_datarange_start(datacut->get_userMin());
            g->set_datarange_end(datacut->get_userMax());
            // draw lines is default
            if (plotStyle==0) { // draw points
                g->set_drawLine(false);
                g->set_symbol(JKQTPcross);
            } else if (plotStyle==2) {
                g->set_symbol(JKQTPcross);
            }
            pltData->addGraph(g);
            //qDebug()<<"   d "<<t.elapsed()<<" ms";
            t.start();

            updateFitFunctions();
            //qDebug()<<"   e "<<t.elapsed()<<" ms";
            t.start();

            pltData->zoomToFit(true, true);
            pltResiduals->zoomToFit(false, true);
            pltDistribution->zoomToFit(true, true);

            pltResiduals->setX(pltData->getXMin(), pltData->getXMax());

            pltResidualHistogram->zoomToFit(true, true);
            pltResidualCorrelation->zoomToFit(true, true);
            //qDebug()<<"   f "<<t.elapsed()<<" ms";
            t.start();
        }


        pltResiduals->set_doDrawing(true);
        pltResiduals->set_emitSignals(true);
        pltData->set_doDrawing(true);
        pltData->set_emitSignals(true);
        pltResidualHistogram->set_doDrawing(true);
        pltResidualHistogram->set_emitSignals(true);
        pltResidualCorrelation->set_doDrawing(true);
        pltResidualCorrelation->set_emitSignals(true);
        pltDistribution->set_doDrawing(true);
        pltDistribution->set_emitSignals(true);
        //qDebug()<<"   g "<<t.elapsed()<<" ms";
        t.start();

        pltResiduals->update_plot();
        //qDebug()<<"   h "<<t.elapsed()<<" ms";
        t.start();
        pltData->update_plot();
        //qDebug()<<"   i "<<t.elapsed()<<" ms";
        t.start();
        pltResidualHistogram->update_plot();
        //qDebug()<<"   j "<<t.elapsed()<<" ms";
        t.start();
        pltResidualCorrelation->update_plot();
        //qDebug()<<"   k "<<t.elapsed()<<" ms";
        t.start();
        pltDistribution->update_plot();
        //qDebug()<<"   l "<<t.elapsed()<<" ms";
        t.start();
        //qDebug()<<"  displayData end  runtime = "<<t1.elapsed()<<" ms";

        //setUpdatesEnabled(updEn);

    }
}


void QFFCSMaxEntEvaluationEditor::updateFitFunctions() {

    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    int index=eval->getCurrentIndex();
    int model=eval->getCurrentModel();
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    JKQTPdatastore* dsdist=pltDistribution->getDatastore();


    if ((!eval)||(!data)) {
        return;
    }


    //qDebug()<<" **** updateFitFunctions";
    QTime t, t1;
    t.start();
    t1.start();

    int residualStyle=cmbResidualStyle->currentIndex();
    int residualHistogramBins=spinResidualHistogramBins->value();
    int datacut_min=datacut->get_userMin();
    int datacut_max=datacut->get_userMax();


    //bool updEn=updatesEnabled();
    //setUpdatesEnabled(false);
    try {

        if (data->getCorrelationN()>0) {
            int c_tau=ds->getColumnNum("tau");
            if (c_tau>=0) { // we only add a graph, if we have a column with tau values
                eval->set_doEmitPropertiesChanged(false);
                eval->set_doEmitResultsChanged(false);
                record->disableEmitResultsChanged();
                //qDebug()<<"    a "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // retrieve data and tau-values from rawdata record
                /////////////////////////////////////////////////////////////////////////////////
                long N=data->getCorrelationN();
                int runAvgWidth=11;
                double* tauvals=data->getCorrelationT();
                double* corrdata=NULL;

                if (index<0) {
                    corrdata=data->getCorrelationMean();
                } else {
                    if (index<(int)data->getCorrelationRuns()) {
                        corrdata=data->getCorrelationRun(index);
                    } else {
                        corrdata=data->getCorrelationMean();
                    }
                }

                double* weights=eval->allocWeights(NULL, record, index, datacut_min, datacut_max);

                int rangeMaxDatarange=getUserMax(record, index);
                int rangeMinDatarange=getUserMin(record, index);


                /////////////////////////////////////////////////////////////////////////////////
                // calculate fit statistics
                /////////////////////////////////////////////////////////////////////////////////
                record->disableEmitResultsChanged();
                double* modelVals=(double*)qfMalloc(N*sizeof(double));
                QVector<double> dist=eval->getDistribution(record, index, model);
                QVector<double> distTau=eval->getDistributionTaus(record, index, model);

                eval->evaluateModel(record, index, model, tauvals, modelVals, N, distTau.data(), dist.data(), dist.size());

                // CALCULATE FIT STATISTICS
                QFFitStatistics fit_stat=eval->calcFitStatistics(record, index, model, tauvals, modelVals, N, dist.size(), rangeMinDatarange, rangeMaxDatarange, runAvgWidth, residualHistogramBins);

                record->enableEmitResultsChanged();


                size_t c_fit = ds->addCopiedColumn(fit_stat.fitfunc,  "fit_model");
                //qDebug()<<"    f "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot fit model and additional function graphs
                /////////////////////////////////////////////////////////////////////////////////
                JKQTPxyLineGraph* g_fit=new JKQTPxyLineGraph(pltData->get_plotter());
                g_fit->set_drawLine(true);
                g_fit->set_title("fit function");
                g_fit->set_xColumn(c_tau);
                g_fit->set_yColumn(c_fit);
                g_fit->set_datarange_start(datacut->get_userMin());
                g_fit->set_datarange_end(datacut->get_userMax());
                pltData->addGraph(g_fit);
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_taures=c_tau;//dsres->addCopiedColumn(data->getCorrelationT(), N, "tau");
                size_t c_residuals=0;
                JKQTPxyLineGraph* g_residuals=new JKQTPxyLineGraph(pltResiduals->get_plotter());
                if (chkWeightedResiduals->isChecked()) {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals_weighted,  "residuals_weighted");
                    g_residuals->set_title("weighted residuals");
                } else {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals,  "residuals");
                    g_residuals->set_title("residuals");
                }
                g_residuals->set_xColumn(c_taures);
                g_residuals->set_yColumn(c_residuals);
                g_residuals->set_symbolSize(8);
                g_residuals->set_symbolWidth(1);
                g_residuals->set_datarange_start(datacut->get_userMin());
                g_residuals->set_datarange_end(datacut->get_userMax());
                g_residuals->set_drawLine(true);
                if (residualStyle==0) { // draw points
                    g_residuals->set_drawLine(false);
                    g_residuals->set_symbol(JKQTPcross);
                } else if (residualStyle==2) {
                    g_residuals->set_symbol(JKQTPcross);
                }
                pltResiduals->addGraph(g_residuals);
                //qDebug()<<"    h "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals running average
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_tauresra=dsres->addCopiedColumn(fit_stat.tau_runavg, "tau_resid_runavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg_weighted,  "residuals_runavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg,  "residuals_runavg");
                    g_residualsra->set_title("residuals, movAvg");
                }
                g_residualsra->set_xColumn(c_tauresra);
                g_residualsra->set_yColumn(c_residualsra);
                g_residualsra->set_symbol(JKQTPnoSymbol);
                g_residualsra->set_symbolSize(6);
                g_residualsra->set_symbolWidth(1);
                g_residualsra->set_color(g_residuals->get_color().darker());
                //g_residuals->set_datarange_start(datacut->get_userMin());
                //g_residuals->set_datarange_end(datacut->get_userMax());
                g_residualsra->set_drawLine(true);
                /*if (residualStyle==0) { // draw points
                    // always draw as lines
                    g_residualsra->set_symbol(JKQTPplus);
                } else if (residualStyle==2) {
                    g_residualsra->set_symbol(JKQTPplus);
                }*/
                pltResiduals->addGraph(g_residualsra);
                //qDebug()<<"    i "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals histogram
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualHistogramX=0;
                size_t c_residualHistogramY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fit_stat.rminw+fit_stat.residHistWBinWidth/2.0, fit_stat.rmaxw-fit_stat.residHistWBinWidth/2.0, "residualhist_weighted_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resWHistogram,  "residualhist_weighted_y");
                } else {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fit_stat.rmin+fit_stat.residHistBinWidth/2.0, fit_stat.rmax-fit_stat.residHistBinWidth/2.0, "residualhist_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resHistogram,  "residualhist_y");
                }
                JKQTPbarHorizontalGraph* g_residualsHistogram=new JKQTPbarHorizontalGraph(pltResidualHistogram->get_plotter());
                g_residualsHistogram->set_xColumn(c_residualHistogramX);
                g_residualsHistogram->set_yColumn(c_residualHistogramY);
                g_residualsHistogram->set_shift(0);
                g_residualsHistogram->set_width(1.0);
                pltResidualHistogram->addGraph(g_residualsHistogram);
                //qDebug()<<"    j "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals correlations
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualCorrelationX=dsresc->addLinearColumn(fit_stat.resN-1, 1, fit_stat.resN-1, "residualcorr_x");
                size_t c_residualCorrelationY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualCorrelationY=dsresc->addCopiedColumn(&(fit_stat.resWCorrelation[1]), fit_stat.resN-1, "residualcorr_weighted_y");
                } else {
                    c_residualCorrelationY=dsresh->addCopiedColumn(&(fit_stat.resCorrelation[1]), fit_stat.resN-1, "residualcorr_y");
                }
                JKQTPxyLineGraph* g_residualsCorrelation=new JKQTPxyLineGraph(pltResidualCorrelation->get_plotter());
                g_residualsCorrelation->set_xColumn(c_residualCorrelationX);
                g_residualsCorrelation->set_yColumn(c_residualCorrelationY);
                pltResidualCorrelation->addGraph(g_residualsCorrelation);
                //qDebug()<<"    k "<<t.elapsed()<<" ms";
                t.start();



                /////////////////////////////////////////////////////////////////////////////////
                // plot distribution
                /////////////////////////////////////////////////////////////////////////////////
                QVector<double> mem_tau=eval->getDistributionTaus(record, index, model);
                QVector<double> mem_D=eval->getDistributionDs(record, index, model);
                QVector<double> mem_dist=eval->getDistribution(record, index, model);

                int c_disttau=-1;
                int c_distD=-1;
                int c_dist=-1;
                if (mem_tau.size()>0 && mem_dist.size()>0) {
                    c_disttau=dsdist->addCopiedColumn(mem_tau.data(), mem_tau.size(), "maxent_tau");
                    c_distD=dsdist->addCopiedColumn(mem_D.data(), mem_D.size(), "maxent_D");
                    c_dist=dsdist->addCopiedColumn(mem_dist.data(), mem_dist.size(), "maxent_dist");;
                } else {
                    if (cmbXAxisType->currentIndex()==0) pltDistribution->setXY(pltData->getXMin(), pltData->getXMax(), pltData->getYMin(), pltData->getYMax());
                }

                if (chkShowRanges->isChecked()) {
                    JKQTPverticalRange* g_r1=new JKQTPverticalRange(pltDistribution->get_plotter());
                    g_r1->set_rangeMin(edtRange1Min->value());
                    g_r1->set_rangeMax(edtRange1Max->value());
                    //g_r1->set_plotCenterLine(false);
                    //g_r1->set_plotRangeLines(false);
                    g_r1->set_title(tr("range 1"));
                    //g_r1->set_fillRange(true);
                    QColor col=QColor("lightsalmon");
                    g_r1->set_color(col);
                    col.setAlphaF(0.5);
                    g_r1->set_fillColor(col);
                    pltDistribution->addGraph(g_r1);

                    JKQTPverticalRange* g_r2=new JKQTPverticalRange(pltDistribution->get_plotter());
                    g_r2->set_rangeMin(edtRange2Min->value());
                    g_r2->set_rangeMax(edtRange2Max->value());
                    //g_r2->set_plotCenterLine(false);
                    //g_r2->set_plotRangeLines(false);
                    g_r2->set_title(tr("range 2"));
                    //g_r2->set_fillRange(true);
                    col=QColor("lightskyblue");
                    g_r1->set_color(col);
                    col.setAlphaF(0.5);
                    g_r2->set_fillColor(col);
                    pltDistribution->addGraph(g_r2);
                }

                JKQTPxyLineGraph* g_dist=new JKQTPxyLineGraph(pltDistribution->get_plotter());
                g_dist->set_drawLine(true);
                g_dist->set_title("MaxEnt distribution");
                if (cmbXAxisType->currentIndex()==1) {
                    g_dist->set_xColumn(c_distD);
                    pltDistribution->getXAxis()->set_axisLabel(tr("diffusion coefficient $D$ [{\\mu}m^2/s]"));
                } else {
                    g_dist->set_xColumn(c_disttau);
                    pltDistribution->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
                }
                g_dist->set_yColumn(c_dist);
                g_dist->set_lineWidth(2);
                g_dist->set_symbolSize(8);
                g_dist->set_symbolWidth(1);
                int plotStyle=cmbPlotStyle->currentIndex();
                if (plotStyle==0) { // draw points
                    g_dist->set_drawLine(false);
                    g_dist->set_symbol(JKQTPcross);
                } else if (plotStyle==2) {
                    g_dist->set_symbol(JKQTPcross);
                }

                pltDistribution->addGraph(g_dist);
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();

                if (cmbXAxisType->currentIndex()==1) pltDistribution->zoomToFit();


                /////////////////////////////////////////////////////////////////////////////////
                // update display of fit results
                /////////////////////////////////////////////////////////////////////////////////
                QString txtFit="<font face=\"Arial\">";
                QString fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML");
                if (fitResult.isEmpty()) fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_message");

                if (!fitResult.isEmpty()) {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>%1</center></div><br>").arg(fitResult);
                } else {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>not fit yet</center></div><br>");
                }
                txtFit+=QString("<b>%1</b><center>").arg(tr("Fit Statistics:"));

                txtFit+=fit_stat.getAsHTMLTable();

                //qDebug()<<"    l "<<t.elapsed()<<" ms";
                t.start();

                txtFit+=QString("</center></font>");
                fitStatisticsReport=txtFit;
                txtFitStatistics->setHtml(txtFit);

                //qDebug()<<"    m_presignals "<<t.elapsed()<<" ms";
                eval->set_doEmitPropertiesChanged(true);
                eval->set_doEmitResultsChanged(true);
                //record->enableEmitResultsChanged();

                //qDebug()<<"    m "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // clean memory
                /////////////////////////////////////////////////////////////////////////////////
                qfFree(weights);
                fit_stat.free();

                //qDebug()<<"    n "<<t.elapsed()<<" ms";
                t.start();
            }
        }
    } catch(std::exception& E) {
        services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
    }
    //setUpdatesEnabled(updEn);
    //qDebug()<<"    updateFitFunctions end   runtime = "<<t1.elapsed()<<"ms";

}

void QFFCSMaxEntEvaluationEditor::displayParameters() {
    QFFCSByIndexAndModelEvaluationEditor::displayParameters();
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    QFSimpleFitParameterEnumeratorInterface* peval=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    bool hasEval=eval->hasResults(record);
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;
    edtNdist->setValue(eval->getNdist());
    edtNumIter->setValue(eval->getNumIter());
    dataEventsEnabled=oldde;

    datacut->setEnabled(!hasEval);
    edtNdist->setEnabled(!hasEval);
    cmbTauMode->setEnabled(!hasEval);
    edtTauMax->setEnabled(!hasEval);
    edtTauMin->setEnabled(!hasEval);
}





void QFFCSMaxEntEvaluationEditor::fitCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;



    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));

    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->display();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doFit(record, eval->getCurrentIndex(), eval->getCurrentModel(), getUserMin(record, eval->getCurrentIndex()), getUserMax(record, eval->getCurrentIndex()), 11, spinResidualHistogramBins->value());    
    dataEventsEnabled=oldde;
    eval->emitResultsChanged(record);
    record->emitResultsChanged();

    displayParameters();
    displayData();
    dlgEvaluationProgress->setValue(100);
    dlgEvaluationProgress->done();

    QApplication::restoreOverrideCursor();
}


void QFFCSMaxEntEvaluationEditor::fitRunsCurrent() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->display();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;

    // iterate through all records and all runs therein and do the fits
    //for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=eval->getHighlightedRecord();
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            dlgEvaluationProgress->setRange(0,data->getCorrelationRuns());
            for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
                dlgEvaluationProgress->setValue(idx);
            }
        }
        record->emitResultsChanged();
        QApplication::processEvents();
    //}
    dataEventsEnabled=oldde;
    eval->emitResultsChanged(record);
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->done();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMaxEntEvaluationEditor::fitAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->display();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            }
            record->emitResultsChanged();
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dataEventsEnabled=oldde;
    eval->emitResultsChanged();
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->done();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFFCSMaxEntEvaluationEditor::fitRunsAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->display();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            //for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                int idx=eval->getCurrentIndex();
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            //}
                record->emitResultsChanged();
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dataEventsEnabled=oldde;
    eval->emitResultsChanged();
    dlgEvaluationProgress->setValue(recs.size());
    dlgEvaluationProgress->done();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMaxEntEvaluationEditor::copyMoreData(QFRawDataRecord *record, int index, int model) {
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    eval->setFitValue(record, index, model, "maxent_alpha", eval->getAlpha());
    eval->setFitValue(record, index, model, "maxent_numiter", eval->getNumIter());
    eval->setFitValue(record, index, model, "maxent_Ndist", eval->getNdist());
}

void QFFCSMaxEntEvaluationEditor::copyMoreDataToInitial() {
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    eval->setInitFitValue("maxent_alpha", eval->getAlpha());
    eval->setInitFitValue("maxent_numiter", eval->getNumIter());
    eval->setInitFitValue("maxent_Ndist", eval->getNdist());
}




void QFFCSMaxEntEvaluationEditor::alphaChanged(double alpha) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setAlpha(alpha);
}

void QFFCSMaxEntEvaluationEditor::wxyChanged(double wxy) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setWXY(wxy);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::nChanged(double wxy)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setRefIndx(wxy);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::lambdaChanged(double wxy)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setLambda(wxy);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::thetaChanged(double wxy)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setTheta(wxy);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    displayData();
    QApplication::restoreOverrideCursor();
}
void QFFCSMaxEntEvaluationEditor::NdistChanged(int Ndist) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setNdist(Ndist);
}

void QFFCSMaxEntEvaluationEditor::NumIterChanged(int NumIter) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setNumIter(NumIter);
}


void QFFCSMaxEntEvaluationEditor::sumRangesChanged()
{

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (eval) {
        if (current->getHighlightedRecord()) {
            current->setQFProperty(QString("rdr%1_range1min").arg(current->getHighlightedRecord()?current->getHighlightedRecord()->getID():0), edtRange1Min->value());
            current->setQFProperty(QString("rdr%1_range1max").arg(current->getHighlightedRecord()?current->getHighlightedRecord()->getID():0), edtRange1Max->value());
            current->setQFProperty(QString("rdr%1_range2min").arg(current->getHighlightedRecord()?current->getHighlightedRecord()->getID():0), edtRange2Min->value());
            current->setQFProperty(QString("rdr%1_range2max").arg(current->getHighlightedRecord()?current->getHighlightedRecord()->getID():0), edtRange2Max->value());

            QVector<double> tau;
            if (cmbXAxisType->currentIndex()==1) tau=eval->getDistributionDs(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
            else tau=eval->getDistributionTaus(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
            QVector<double> dist=eval->getDistribution(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());


            double r1min=edtRange1Min->value();
            double r1max=edtRange1Max->value();
            if (r1min>r1max) qSwap(r1min, r1max);
            double r2min=edtRange2Min->value();
            double r2max=edtRange2Max->value();
            if (r2min>r2max) qSwap(r2min, r2max);
            double sum=0, cnt=0;
            double sumr1=0, sumr2=0;
            for (int i=0; i<qMin(tau.size(), dist.size()); i++) {
                double t=tau[i];
                double d=dist[i];
                sum=sum+d;
                if (r1min<=t && t<=r1max) sumr1+=d;
                if (r2min<=t && t<=r2max) sumr2+=d;
                cnt++;
            }

            labRange1->setText(QString::number(sumr1/sum*100.0)+QString(" %"));
            labRange2->setText(QString::number(sumr2/sum*100.0)+QString(" %"));
            QString param="";
            eval->setFitResultValue(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param="range1_sum", sumr1);
            eval->setFitResultGroup(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param, tr("distribution analysis"));
            eval->setFitResultValue(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param="range1_relcontents", sumr1/sum);
            eval->setFitResultGroup(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param, tr("distribution analysis"));
            eval->setFitResultValue(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param="range2_sum", sumr2);
            eval->setFitResultGroup(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param, tr("distribution analysis"));
            eval->setFitResultValue(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param="range2_relcontents", sumr2/sum);
            eval->setFitResultGroup(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param, tr("distribution analysis"));
            eval->setFitResultValue(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param="range_sumall", sum);
            eval->setFitResultGroup(current->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel(), param, tr("distribution analysis"));


        }
        current->setQFProperty(QString("range1min"), edtRange1Min->value());
        current->setQFProperty(QString("range1max"), edtRange1Max->value());
        current->setQFProperty(QString("range2min"), edtRange2Min->value());
        current->setQFProperty(QString("range2max"), edtRange2Max->value());

        if (dataEventsEnabled) displayData();

    }
}

void QFFCSMaxEntEvaluationEditor::copyAverageData()
{
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFRDRRunSelectionsInterface* rundata=qobject_cast<QFRDRRunSelectionsInterface*>(record);
    if ((!eval)||(!record)||(!data)) return;

    QStringList resultLabels;
    QList<QVariant> resultNames;
    QList<QColor> resultColors;
    QList<bool> selected;

    int runstart=0;
    int runend=data->getCorrelationRuns();
    if (data->getCorrelationRuns()==1) { runstart=-1; runend=0; }
    for (int run=runstart; run<runend; run++) {
        QString evalID=eval->getEvaluationResultID(run, eval->getCurrentModel());
        if (record->resultsExists(evalID, "maxent_distribution") && record->resultsExists(evalID, "maxent_tau")) {
            resultLabels.append(tr("run %1").arg(run));
            resultNames.append(run);
            if (!rundata || (rundata && !rundata->leaveoutRun(run))) {
                selected<<true;
                resultColors<<palette().color(QPalette::WindowText);
            } else {
                selected<<false;
                resultColors<<QColor("grey");
            }
        }
    }

    QFSelectionListDialog* dlg=new QFSelectionListDialog(this);
    dlg->init(resultLabels, resultNames, resultColors, *(ProgramOptions::getInstance()->getQSettings()), "fcsmaxentevaleditor/copysel/");
    dlg->setLabel(tr("Select the runs you want to average over (gray runs are excluded!)\nand then below the averaging options!"));
    dlg->selectItems(selected);

    QCheckBox* chkCopyRaw=new QCheckBox("", dlg);
    chkCopyRaw->setChecked(ProgramOptions::getInstance()->getQSettings()->value("fcsmaxentevaleditor/copysel/copyRaw", false).toBool());
    chkCopyRaw->setToolTip(tr("if this is activated, all the single run's data, used for the average and standard deviation will also be copied."));
    dlg->addWidget(tr("copy also single runs' data:"), chkCopyRaw);

    dlg->setWindowTitle(tr("Copy averaged MaxEnt data"));

    if (dlg->exec()) {
        ProgramOptions::getInstance()->getQSettings()->setValue("fcsmaxentevaleditor/copysel/copyRaw", chkCopyRaw->isChecked());
        dlg->writeList(*(ProgramOptions::getInstance()->getQSettings()), "fcsmaxentevaleditor/copysel/");

        QProgressDialog progress;
        progress.show();
        QList<QVariant> sel=dlg->getSelected();
        QList<double> tau, sum, sum2, Ds;
        double Ncnt=0;
        QString errorMessage="";
        progress.setRange(0, sel.size());
        for (int i=0; i<sel.size(); i++) {
            progress.setLabelText(tr("processing MSD data for copying ... run %1/%2").arg(i+1).arg(sel.size()));
            progress.setValue(i);
            QApplication::processEvents();
            Ncnt++;
            int run=sel[i].toInt();
            QString evalID=eval->getEvaluationResultID(run, eval->getCurrentModel());
            QVector<double> t=eval->getDistributionTaus(record, run, eval->getCurrentModel());
            QVector<double> d=eval->getDistributionDs(record, run, eval->getCurrentModel());
            QVector<double> mem=eval->getDistribution(record, run, eval->getCurrentModel());

            if (i==0) {
                sum=mem.toList();
                tau=t.toList();
                Ds=d.toList();
                for (int c=0; c<mem.size(); c++) {
                    sum2.append(mem[c]*mem[c]);
                }
            } else {
                if (tau.size()!=mem.size() || sum.size()!=mem.size() || sum2.size()!=mem.size()) {
                    errorMessage=tr("Can not average over runs: MaxEnt was calculated for run %1 with a different tau-scale!").arg(run);
                } else {
                    for (int c=0; c<mem.size(); c++) {
                        if (tau[c] != t[c]) {
                            errorMessage=tr("Can not average over runs: MaxEnt was calculated for run %1 with a different tau-scale!").arg(run);
                        } else {
                            sum[c]=sum[c]+mem[c];
                            sum2[c]=sum2[c]+mem[c]*mem[c];
                        }
                    }
                }
            }
            if (!errorMessage.isEmpty()) break;
            if (progress.wasCanceled()) break;
        }
        if (errorMessage.isEmpty() && !progress.wasCanceled()) {
            QList<QList<double> > data;
            QStringList colNames;
            colNames<<tr("tau [s]")<<tr("D [{\\mu}m^2/s]")<<tr("avg(MaxEnt)")<<tr("SD(MaxEnt)");
            data.append(tau);
            data.append(Ds);
            for (int i=0; i<sum.size(); i++) {
                double s=sum[i];
                double s2=sum2[i];
                sum[i]=s/Ncnt;
                if (Ncnt>=2) sum2[i]=sqrt((s2-s*s/Ncnt)/(Ncnt-1.0));
                else sum2[i]=0;
            }
            data.append(sum);
            data.append(sum2);

            if (chkCopyRaw->isChecked()) {
                for (int i=0; i<sel.size(); i++) {
                    int run=sel[i].toInt();
                    QVector<double> d=eval->getDistribution(record, run, eval->getCurrentModel());
                    colNames<<tr("maxent_run%1").arg(run);
                    data.append(d.toList());
                }
            }



            csvCopy(data, colNames);
        } else {
            QMessageBox::critical(this, tr("copy averaged MaxEnt data"), tr("ERROR:\n%1").arg(errorMessage), QMessageBox::Ok, QMessageBox::Ok);
        }

    }
    delete dlg;
}

void QFFCSMaxEntEvaluationEditor::chkShowDChanged() {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("show_daxis", cmbXAxisType->currentIndex(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::chkShowrangesChanged() {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("show_ranges", chkShowRanges->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::tauModeChanged(int weights)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);

    edtTauMin->setEnabled(weights>0 && data->hasResults(current->getHighlightedRecord(), data->getCurrentIndex(), data->getCurrentModel()));
    edtTauMax->setEnabled(edtTauMin->isEnabled());
    if (data) {
        data->setTauMode(weights);
    }
}

void QFFCSMaxEntEvaluationEditor::tauMinMaxChanged()
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);

    if (data)     {
        data->setTauMin(edtTauMin->value());
        data->setTauMax(edtTauMax->value());
    }

}

void QFFCSMaxEntEvaluationEditor::weightsChanged(int weights) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    data->setFitDataWeighting(weights);
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}






////////////////////////////////////////////////////////////////////////////////////////////////////
// REPORT GENERATION
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void QFFCSMaxEntEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if ((!eval)||(!record))return;    // ||(!data) vielleicht noch hinzufügen


    // we use this QTextCursor to write the document
    QTextCursor cursor(document);

    // here we define some generic formats
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);


    // insert heading
    cursor.insertText(tr("Evaluation Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(2, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(QString::number(record->getID()));
    cursor.movePosition(QTextCursor::End);

}


*/







void QFFCSMaxEntEvaluationEditor::createReportDoc(QTextDocument* document) {
    // make sure all widgets ahave the right size
    {   int trci=tabResidulas->currentIndex();
        for (int i=0;i<tabResidulas->count(); i++) {
            tabResidulas->setCurrentIndex(i);
        }
        tabResidulas->setCurrentIndex(trci);
    }

    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);



    if (!current) return;
    if (!cmbModel) return; ////////////
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(record);

    ////////////////////////////////
    //QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    ////////////////////////////////

    ///////////
    if (!record) return;
    //////////
    if (!eval) return;
    if (!fcs) return;

    /*
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* algorithm=eval->getFitAlgorithm();
    //int run=eval->getCurrentIndex();
    double* params=eval->allocFillParameters();
    */

    QTextCursor cursor(document);
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfLeftNB=bfLeft;
    bfLeftNB.setNonBreakableLines(true);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfRightNB=bfRight;
    bfRightNB.setNonBreakableLines(true);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);
    QTextBlockFormat bfCenterNB=bfCenter;
    bfCenterNB.setNonBreakableLines(true);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    cursor.insertText(tr("FCS Maxent Fit Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);



    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(3, 4, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("file:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(0, 2).firstCursorPosition().insertText(tr("run:"), fTextBold);
    table->cellAt(0, 3).firstCursorPosition().insertText(QString("%1 [%2]").arg((eval->getCurrentIndex()<0)?tr("average"):QString::number(eval->getCurrentIndex())).arg(fcs->getCorrelationRunName(eval->getCurrentIndex())), fText);
    //table->cellAt(1, 0).firstCursorPosition().insertText(tr("fit algorithm:"), fTextBold);
    //table->cellAt(1, 1).firstCursorPosition().insertText(algorithm->name(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("data range:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(tr("%1 ... %2 / %3 ... %4").arg(datacut->get_userMin()).arg(datacut->get_userMax()).arg(datacut->get_min()).arg(datacut->get_max()), fText);
    table->cellAt(2, 0).firstCursorPosition().insertText(tr("physical model: "), fTextBold);
    //table->cellAt(2, 1).firstCursorPosition().insertText(ffunc->name(), fText);
    table->cellAt(2, 1).firstCursorPosition().insertText("FCS 3D Diffusion with triplet", fText);
    table->cellAt(2, 2).firstCursorPosition().insertText(tr("data weighting:"), fTextBold);
    table->cellAt(2, 3).firstCursorPosition().insertText(cmbWeights->currentText(), fText);
    cursor.movePosition(QTextCursor::End);



    cursor.insertBlock(); cursor.insertBlock();
    cursor.insertText(tr("Plots:\n"), fTextBold);
    QTextTableFormat tableFormat1;
    tableFormat1.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat1.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 66);
    constraints << QTextLength(QTextLength::PercentageLength, 34);
    tableFormat1.setColumnWidthConstraints(constraints);

    table = cursor.insertTable(2, 2, tableFormat1);
    //table->mergeCells(0,0,2,1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltData->get_plotter()->draw(*painter, QRect(0,0,pltData->width(),pltData->width()));
        delete painter;
        double scale=document->textWidth()*0.45/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation data, model and residuals:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.45);
    }
    QApplication::processEvents();

    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltDistribution->get_plotter()->draw(*painter, QRect(0,0,pltDistribution->width(),pltDistribution->width()));
        delete painter;
        double scale=document->textWidth()*0.45/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("Maximum Entropy Distribution:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.45);
    }
    QApplication::processEvents();


    {
        QTextCursor tabCursor=table->cellAt(1, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltResidualHistogram->get_plotter()->draw(*painter, QRect(0,0,pltResidualHistogram->width(),pltResidualHistogram->width()));
        delete painter;
        double scale=document->textWidth()*0.45/pic.boundingRect().width();//0.3
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual histogram:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.45);
    }
    QApplication::processEvents();
    {
        QTextCursor tabCursor=table->cellAt(1, 1).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltResidualCorrelation->get_plotter()->draw(*painter, QRect(0,0,pltResidualCorrelation->width(),pltResidualCorrelation->width()));
        delete painter;
        double scale=document->textWidth()*0.45/pic.boundingRect().width();//0.3
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual correlations:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.45);
    }


    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////


    /*
    qDebug() << "WRITE DOCUMENT DEBUG: ";
    qDebug() << eval->getCurrentModel();
    qDebug() << eval->getParameterCount(eval->getCurrentModel());
    */


    int fitParamCount=0;
    /*
    for (int i=0; i<eval->getParameterCount(eval->getCurrentModel()); i++){ // ffunc->paramCount(); i++) {

        //if (ffunc->isParameterVisible(i, params)) fitParamCount++;
    }
    */
   fitParamCount=eval->getParameterCount(eval->getCurrentModel());


    QApplication::processEvents();

    cursor.insertBlock(); cursor.insertBlock();

    /*
    if (eval->hasFit()) cursor.insertText(tr("Model Parameters (fit results):"), fTextBold);
    else cursor.insertText(tr("Model Parameters (initial values):"), fTextBold);
    */
    cursor.insertText(tr("Model Parameters: "),fTextBold);

    QTextTableFormat tableFormat2;
    tableFormat2.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat2.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    table = cursor.insertTable(6, 4, tableFormat2);
    //table = cursor.insertTable(ceil((double)fitParamCount/2.0)+1, 12, tableFormat2);
    QTextCursor tableCursor;
    QApplication::processEvents();


    tableCursor=table->cellAt(0, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Algorithm Parameters"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("LocalParameters"), fTextBoldSmall);

    //if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 5).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();
/*
    tableCursor=table->cellAt(0, 6).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 8).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    //if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 11).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();
*/

    tableCursor=table->cellAt(1,0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr(" run =  "), fTextSmall);

    tableCursor=table->cellAt(2,0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr(" Alpha =  "), fTextSmall);

    tableCursor=table->cellAt(3, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr(" Ndist =  "), fTextSmall);

    tableCursor=table->cellAt(4, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr(" Iterations/Steps =  "), fTextSmall);

    //table->cellAt(0, 3).firstCursorPosition().insertText(QString("%1 [%2]").arg((eval->getCurrentIndex()<0)?tr("average"):QString::number(eval->getCurrentIndex())).arg(fcs->getCorrelationRunName(eval->getCurrentIndex())), fText);


    tableCursor=table->cellAt(1,1).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(QString("%1 [%2]").arg((eval->getCurrentIndex()<0)?tr("average"):QString::number(eval->getCurrentIndex())).arg(fcs->getCorrelationRunName(eval->getCurrentIndex())), fTextSmall);

    tableCursor=table->cellAt(2, 1).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr("%1").arg(eval->getAlpha()), fTextSmall);

    tableCursor=table->cellAt(3, 1).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr("%1").arg(eval->getNdist()), fTextSmall);

    tableCursor=table->cellAt(4, 1).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr("%1").arg(eval->getNumIter()), fTextSmall);

    tableCursor=table->cellAt(1, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\">&tau;<sub>trip</sub></font> [&mu;s] = ")));

    tableCursor=table->cellAt(2, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\">&theta;<sub>trip</sub></font> = ")));

    tableCursor=table->cellAt(3, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRightNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("Axial ratio <font size=\"-5\">&gamma;</font> = ")));

    tableCursor=table->cellAt(1, 3).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr("%1").arg(eval->getFitValue("trip_tau")), fTextSmall);

    tableCursor=table->cellAt(2, 3).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr("%1").arg(eval->getFitValue("trip_theta")), fTextSmall);

    tableCursor=table->cellAt(3, 3).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(tr("%1").arg(eval->getFitValue("focus_struct_fac")), fTextSmall);






    /*

    int rowStart=1;
    int colStart=0;
    for (int i=0; i<eval->getParameterCount(eval->getCurrentModel()); i++) {
        QApplication::processEvents();
        QString id=eval->getParameterID(eval->getCurrentModel(),i);
        double error=roundError(eval->getFitError(id),2);
        double value=roundWithError(eval->getFitValue(id), error, 2);
        QString value_string=floattohtmlstr(value, 5, true).c_str();
        bool fix=eval->getFitFix(id);
        QFFitFunction::ParameterDescription d=eval->getDescription(id); //ffunc->getDescription(id);

        QString range=QString("%1...%2").arg(QString(floattohtmlstr(d.minValue, 5, true).c_str())).arg(QString(floattohtmlstr(d.maxValue, 5, true).c_str()));
        if ((d.type==QFFitFunction::IntCombo)&&((int)value>=0)&&((int)value<d.comboItems.size())) {
            value_string="<i>"+d.comboItems[(int)value]+"</i>";
        }




        if (ffunc->isParameterVisible(i, params))
                {


            QString err="";

            if (d.displayError!=QFFitFunction::NoError) {
                err=QString("&plusmn;&nbsp;%1").arg(QString(floattohtmlstr(error, 5, true).c_str()));
            }


            tableCursor=table->cellAt(rowStart, colStart).firstCursorPosition();
            tableCursor.setBlockFormat(bfRightNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            //tableCursor.insertFragment(QTextDocumentFragment::fromHtml(d.label));
            tableCursor.insertText(" = ", fTextSmall);

            tableCursor=table->cellAt(rowStart, colStart+1).firstCursorPosition();
            tableCursor.setBlockFormat(bfLeftNB);
            tableCursor.setBlockCharFormat(fTextSmall);

            if (d.fit) {
                tableCursor.insertText(tr("F"), fTextSmall);
            }


            if (!d.userEditable) {
                tableCursor.insertText(tr("C"), fTextSmall);
            }

            if (fix) {
                tableCursor.insertText(tr("X"), fTextSmall);
            }


            tableCursor=table->cellAt(rowStart, colStart+2).firstCursorPosition();
            tableCursor.setBlockFormat(bfRightNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(value_string)));

            tableCursor=table->cellAt(rowStart, colStart+3).firstCursorPosition();
            tableCursor.setBlockFormat(bfLeftNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(err)));

            tableCursor=table->cellAt(rowStart, colStart+4).firstCursorPosition();
            tableCursor.setBlockFormat(bfLeftNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(d.unitLabel)));


            if (algorithm->get_supportsBoxConstraints()) {
                tableCursor=table->cellAt(rowStart, colStart+5).firstCursorPosition();
                tableCursor.setBlockFormat(bfLeftNB);
                tableCursor.setBlockCharFormat(fTextSmall);
                tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(range)));
            }

            rowStart++;
        };
        if (rowStart>=table->rows()) {
            rowStart=1;
            colStart+=6;
        }
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.setBlockFormat(bfCenterNB);
    cursor.setBlockCharFormat(fTextSmall);
    cursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<i><u>legend:</u> <b>F</b>: fit parameter, <b>X</b>: fixed parameter, <b>C</b>: calculated parameter</i>")));
    QApplication::processEvents();

    */
    cursor.setBlockFormat(bfLeft);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(); cursor.insertBlock();
    QString htmlBot=fitStatisticsReport;
    htmlBot.replace("width=\"95%\"", "");
    cursor.insertFragment(QTextDocumentFragment::fromHtml(htmlBot));


}








