#include "qffcsmsdevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qffcsmsdevaluation_item.h"

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





#include <QtGui>
#include <QtCore>

QFFCSMSDEvaluationEditor::QFFCSMSDEvaluationEditor(QFPluginServices *services, QFEvaluationPropertyEditor *propEditor, QWidget *parent):
    QFFCSByIndexAndModelEvaluationEditor(services, propEditor, parent, "fcsmsdevaleditor/")
{
    dataEventsEnabled=false;
    createWidgets();
    dataEventsEnabled=true;
}

QFFCSMSDEvaluationEditor::~QFFCSMSDEvaluationEditor()
{
}

QWidget* QFFCSMSDEvaluationEditor::createSlopeWidgets(int i) {
    QWidget* w1=new QWidget(this);
    QHBoxLayout* lay1=new QHBoxLayout(w1);
    lay1->setContentsMargins(0,0,0,0);
    w1->setLayout(lay1);
    QWidget* w=new QWidget(this);
    QHBoxLayout* lay=new QHBoxLayout(w);
    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);
    numPre[i]=new QFDoubleEdit(this);
    numPre[i]->setRange(0, DBL_MAX);
    numPre[i]->setCheckBounds(true, false);
    numPre[i]->setDecimals(1);
    numPre[i]->setValue(6);
    connect(numPre[i], SIGNAL(valueChanged(double)), this, SLOT(theoryChanged()));

    numD[i]=new QFDoubleEdit(this);
    numD[i]->setRange(0, DBL_MAX);
    numD[i]->setCheckBounds(true, false);
    numD[i]->setDecimals(2);
    numD[i]->setValue(10);
    connect(numD[i], SIGNAL(valueChanged(double)), this, SLOT(theoryChanged()));

    numAlpha[i]=new QFDoubleEdit(this);
    numAlpha[i]->setRange(0, DBL_MAX);
    numAlpha[i]->setCheckBounds(true, false);
    numAlpha[i]->setDecimals(2);
    numAlpha[i]->setValue(1);
    connect(numAlpha[i], SIGNAL(valueChanged(double)), this, SLOT(theoryChanged()));

    lay1->addWidget(numPre[i]);
    lay1->addWidget(new QLabel("*"));
    lay1->addWidget(numD[i]);
    lay1->addWidget(new QLabel(tr("*t^")));
    lay1->addWidget(numAlpha[i]);
    chkSlope[i]=new QCheckBox(this);
    chkSlope[i]->setChecked(false);
    w1->setEnabled(false);
    connect(chkSlope[i], SIGNAL(toggled(bool)), w1, SLOT(setEnabled(bool)));
    connect(chkSlope[i], SIGNAL(toggled(bool)), this, SLOT(theoryChanged()));


    chkSlope[i]->setText(tr("MSD %1:").arg(i+1));

    //lay->addWidget(chkSlope[i]);
    lay->addWidget(w1);

    return w;
}


void QFFCSMSDEvaluationEditor::createWidgets() {

    flAlgorithmParams->addRow(tr("Theory:"), new QLabel(tr("<font size=\"+1\"><i>&lang;r<sup>2</sup>&rang; = f&middot;D&middot;t<sup>&alpha;<&sup></i> [&mu;m<sup>2</sup>]</size>")));

    for (int i=0; i<MSDTHEORYCOUNT; i++) {
        flAlgorithmParams->addRow(chkSlope[i], createSlopeWidgets(i));
    }
    //flAlgorithmParams->addRow(tr("MSD 2:"), createSlopeWidgets(1));
    //flAlgorithmParams->addRow(tr("MSD 3:"), createSlopeWidgets(2));
    //flAlgorithmParams->addRow(tr("MSD 4:"), createSlopeWidgets(3));

    spinFitWidth=new QSpinBox(this);
    spinFitWidth->setRange(5, INT_MAX);
    spinFitWidth->setValue(10);
    flAlgorithmParams->addRow(tr("fit width:"), spinFitWidth);


  /*  edtAlpha=new QFDoubleEdit(this);
    edtAlpha->setRange(0, DBL_MAX);
    edtAlpha->setCheckBounds(true, false);
    edtAlpha->setDecimals(10);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&alpha; = </i>"), edtAlpha);

////////////////////////////////////////
    edtNdist=new QSpinBox(this);
    edtNdist->setRange(10, INT_MAX);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&Ndist = </i>"), edtNdist);
    edtNumIter=new QSpinBox(this);
    edtNumIter->setRange(10, INT_MAX);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&Iterations/Step = </i>"), edtNumIter);
////////////////////////////////////////
*/



    cmbWeights=new QComboBox(this);
    cmbWeights->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    cmbWeights->setEditable(false);
    cmbWeights->addItem(tr("equal weights"));
    cmbWeights->addItem(tr("standard deviation"));
    cmbWeights->addItem(tr("per run errors"));
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
    chkShowKey=new QCheckBox(tr("show key"), this);
    tbPlot->addSeparator();
    tbPlot->addWidget(chkShowKey);
    connect(chkShowKey, SIGNAL(toggled(bool)), this, SLOT(displayData()));

    toolbar->removeAction(pltData->get_plotter()->get_actSavePlot());
    toolbar->removeAction(pltData->get_plotter()->get_actPrint());
    toolbar->removeAction(pltData->get_plotter()->get_actCopyPixelImage());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomAll());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomIn());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomOut());
    plotLayout->insertWidget(0, tbPlot);


    pltDistribution=new JKQtPlotter(true, this);
    pltDistribution->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltDistribution->useExternalDatastore(pltData->getDatastore());
    pltDistribution->set_displayToolbar(false);
    pltDistribution->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    pltDistribution->getXAxis()->set_labelFontSize(11);
    pltDistribution->getXAxis()->set_tickLabelFontSize(10);
    pltDistribution->getXAxis()->set_logAxis(true);
    pltDistribution->getYAxis()->set_axisLabel(tr("MSD $\\langle r^2(\\tau)\\rangle$ [\\mu m^2]"));
    pltDistribution->getYAxis()->set_labelFontSize(11);
    pltDistribution->getYAxis()->set_tickLabelFontSize(10);
    pltDistribution->getYAxis()->set_logAxis(true);
    pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    pltDistribution->get_plotter()->setBorder(1,1,1,1);
    pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltDistribution->set_displayMousePosition(false);
    pltDistribution->get_plotter()->set_keyFontSize(9);
    pltDistribution->get_plotter()->set_keyXMargin(2);
    pltDistribution->get_plotter()->set_keyYMargin(2);


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
    tbPlotDistribution->addSeparator();
    chkShowKeyDist=new QCheckBox(tr("show key"), this);
    chkShowKeyDist->setChecked(true);
    connect(chkShowKeyDist, SIGNAL(toggled(bool)), this, SLOT(updateDistribution()));
    tbPlotDistribution->addWidget(chkShowKeyDist);
    sliderDist=new DataCutSliders(this);


    pltDistResults=new JKQtPlotter(true, this);
    pltDistResults->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltDistResults->useExternalDatastore(pltData->getDatastore());
    pltDistResults->set_displayToolbar(false);
    pltDistResults->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    pltDistResults->getXAxis()->set_labelFontSize(11);
    pltDistResults->getXAxis()->set_tickLabelFontSize(10);
    pltDistResults->getXAxis()->set_logAxis(true);
    pltDistResults->getYAxis()->set_axisLabel("");//tr("MSD $\\langle r^2(\\tau)\\rangle$ [\\mu m^2]"));
    pltDistResults->getYAxis()->set_labelFontSize(11);
    pltDistResults->getYAxis()->set_tickLabelFontSize(10);
    pltDistResults->getYAxis()->set_logAxis(false);
    pltDistResults->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    pltDistResults->get_plotter()->setBorder(1,1,1,1);
    pltDistResults->get_plotter()->set_useAntiAliasingForSystem(true);
    pltDistResults->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltDistResults->get_plotter()->set_keyFontSize(9);
    pltDistResults->get_plotter()->set_keyXMargin(2);
    pltDistResults->useExternalDatastore(pltDistribution->getDatastore());
    pltDistResults->set_displayMousePosition(false);
    pltDistResults->synchronizeToMaster(pltDistribution, true, false);

    tbPlotDistResults=new QToolBar(QString("tbPlotDistributionResults"), this);
    tbPlotDistResults->setIconSize(QSize(16,16));
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actSavePlot());
    pltDistResults->get_plotter()->get_actSavePlot()->setToolTip(tr("save the distribution plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actPrint());
    pltDistResults->get_plotter()->get_actPrint()->setToolTip(tr("print the distribution plot "));
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actCopyPixelImage());
    pltDistResults->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the distribution plot as an image to the system clipboard"));
    tbPlotDistResults->addSeparator();
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actZoomAll());
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actZoomIn());
    tbPlotDistResults->addAction(pltDistResults->get_plotter()->get_actZoomOut());
    cmbDistResultsMode=new QComboBox(this);
    cmbDistResultsMode->addItem(tr("local alpha"));
    cmbDistResultsMode->addItem(tr("local D"));
    connect(cmbDistResultsMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDistributionResults()));
    tbPlotDistResults->addSeparator();
    tbPlotDistResults->addWidget(new QLabel(tr("   parameter: ")));
    tbPlotDistResults->addWidget(cmbDistResultsMode);
    chkShowKeyDistResults=new QCheckBox(tr("show key"), this);
    chkShowKeyDistResults->setChecked(true);
    connect(chkShowKeyDistResults, SIGNAL(toggled(bool)), this, SLOT(updateDistributionResults()));
    tbPlotDistResults->addSeparator();
    tbPlotDistResults->addWidget(chkShowKeyDistResults);

    splitterDist=new QVisibleHandleSplitter(Qt::Vertical, this);
    QWidget* w=new QWidget(this);
    QVBoxLayout* lay=new QVBoxLayout(w);
    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);
    lay->addWidget(tbPlotDistribution);
    lay->addWidget(pltDistribution, 1);
    splitterDist->addWidget(w);

    w=new QWidget(this);
    lay=new QVBoxLayout(w);
    lay->setContentsMargins(0,0,0,0);
    w->setLayout(lay);
    lay->addWidget(tbPlotDistResults);
    lay->addWidget(pltDistResults, 1);
    lay->addWidget(sliderDist);
    splitterDist->addWidget(w);

    QWidget* wPltDist=new QWidget(this);
    QVBoxLayout* layPltDist=new QVBoxLayout(this);
    wPltDist->setLayout(layPltDist);
    layPltDist->setContentsMargins(0,0,0,0);
    layPltDist->addWidget(splitterDist);
    splitMorePLot->addWidget(wPltDist);

    //splitMorePLot->setCollapsible(splitMorePLot->indexOf(pltDistribution), false);
    splitterDist->setCollapsible(splitterDist->indexOf(pltDistribution), false);
    splitterDist->setCollapsible(splitterDist->indexOf(pltDistResults), false);

    QAction* actFirst=menuParameters->actions().value(0, NULL);
    actAverageFirstFrames=new QAction(tr("&average first few frames for N"), this);
    connect(actAverageFirstFrames, SIGNAL(triggered()), this, SLOT(averageFirstFewFrames()));
    menuParameters->insertAction(actFirst, actAverageFirstFrames);
    menuParameters->insertSeparator(actFirst);

    /////
    connect(pltDistribution, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMove(double,double)));
    connect(pltDistribution, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltDistResults, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltDistResults, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    /////

}





int QFFCSMSDEvaluationEditor::getMSDMin(QFRawDataRecord* rec, int index, int defaultMin) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return defaultMin;
    const QString resultID=data->getEvaluationResultID(index);
    if (!rec) return defaultMin;


    return rec->getProperty(resultID+"_msdcut_min", defaultMin).toInt();
}

int QFFCSMSDEvaluationEditor::getMSDMax(QFRawDataRecord* rec, int index, int defaultMax) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return defaultMax;
    const QString resultID=data->getEvaluationResultID(index);

    if (!rec) return defaultMax;

    return rec->getProperty(resultID+"_msdcut_max", defaultMax).toInt();
}


int QFFCSMSDEvaluationEditor::getMSDMin(QFRawDataRecord* rec) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMin(rec, data->getCurrentIndex());
}

int QFFCSMSDEvaluationEditor::getMSDMax(QFRawDataRecord* rec) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMax(rec, data->getCurrentIndex());
}


int QFFCSMSDEvaluationEditor::getMSDMin(QFRawDataRecord* rec, int index) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMin(rec, index, 0);
}

int QFFCSMSDEvaluationEditor::getMSDMax(QFRawDataRecord* rec, int index) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return 0;
    return getMSDMax(rec, index, data->getMSD(rec, index, data->getCurrentModel()).size());
}

void QFFCSMSDEvaluationEditor::setMSDMin(int MSDMin) {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex());
    rdr->setQFProperty(resultID+"_msdcut_min", MSDMin, false, false);
}

void QFFCSMSDEvaluationEditor::setMSDMax(int MSDMax) {
    if (!current) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex());
    rdr->setQFProperty(resultID+"_msdcut_max", MSDMax, false, false);
}


void QFFCSMSDEvaluationEditor::slidersDistChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!data) return;
    if (!current->getHighlightedRecord()) return;
    setMSDMax(userMax);
    setMSDMin(userMin);
    displayData();
}

void QFFCSMSDEvaluationEditor::updateSliders() {
    bool en=false;
    sliderDist->disableSliderSignals();
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    sliderDist->set_min(0);
    if (data && current->getHighlightedRecord()) {
        sliderDist->set_userMin(getMSDMin(current->getHighlightedRecord(), data->getCurrentIndex(), 0));
        QFFCSMSDEvaluationItem* item=qobject_cast<QFFCSMSDEvaluationItem*>(current);
        if (item) {
            QVector<double> msd=item->getMSD(current->getHighlightedRecord(), item->getCurrentIndex(), item->getCurrentModel());
            if (msd.size()>0) {
                en=true;
                sliderDist->set_max(msd.size());
                sliderDist->set_userMax(getMSDMax(current->getHighlightedRecord(), data->getCurrentIndex(), msd.size()));
            }
        }
    }

    sliderDist->setEnabled(en);
    sliderDist->enableSliderSignals();

}


void QFFCSMSDEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    QFFCSByIndexAndModelEvaluationEditor::connectWidgets(current, old);

    if (old) {
        //disconnect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));
        disconnect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        disconnect(sliderDist, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersDistChanged(int, int, int, int)));
        disconnect(spinFitWidth, SIGNAL(valueChanged(int)),this,SLOT(fitWidthChanged(int)));
        //disconnect(edtNumIter, SIGNAL(valueChanged(int)),this,SLOT(NumIterChanged(int)));
    }

    QFFCSMSDEvaluationItem* item=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    //QFFCSMSDEvaluationItem* item_old=qobject_cast<QFFCSMSDEvaluationItem*>(old);
    if (item) {
        dataEventsEnabled=false;

        for (int i=0; i<MSDTHEORYCOUNT; i++)     {
            chkSlope[i]->setChecked(item->getTheoryEnabled(i));
            numPre[i]->setValue(item->getTheoryPre(i));
            numD[i]->setValue(item->getTheoryD(i));
            numAlpha[i]->setValue(item->getTheoryAlpha(i));
        }

        /*edtAlpha->setValue(item->getAlpha());
        connect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));


        edtNumIter->setValue(item->getNumIter());
        connect(edtNumIter, SIGNAL(valueChanged(int)), this, SLOT(NumIterChanged(int)));*/

        spinFitWidth->setValue(item->getFitWidth());
        connect(spinFitWidth, SIGNAL(valueChanged(int)), this, SLOT(fitWidthChanged(int)));
        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        connect(sliderDist, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersDistChanged(int, int, int, int)));





        dataEventsEnabled=true;
    }

    /*
    /////
    connect(pltDistribution, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMove(double,double)));
    /////
    */
}


void QFFCSMSDEvaluationEditor::readSettings() {
    QFFCSByIndexAndModelEvaluationEditor::readSettings();
    chkShowKey->setChecked(settings->getQSettings()->value("fcsmsdevaleditor/showkey", true).toBool());
    chkShowKeyDist->setChecked(settings->getQSettings()->value("fcsmsdevaleditor/showkeydist", true).toBool());
    chkShowKeyDistResults->setChecked(settings->getQSettings()->value("fcsmsdevaleditor/showkeydistresults", true).toBool());
    cmbDistResultsMode->setCurrentIndex(settings->getQSettings()->value("fcsmsdevaleditor/distresultmode", true).toBool());

}

void QFFCSMSDEvaluationEditor::writeSettings() {
    QFFCSByIndexAndModelEvaluationEditor::writeSettings();
    settings->getQSettings()->setValue("fcsmsdevaleditor/showkey", chkShowKey->isChecked());
    settings->getQSettings()->setValue("fcsmsdevaleditor/showkeydist", chkShowKeyDist->isChecked());
    settings->getQSettings()->setValue("fcsmsdevaleditor/showkeydistresults", chkShowKeyDistResults->isChecked());
}



void QFFCSMSDEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    QFFCSByIndexAndModelEvaluationEditor::highlightingChanged(formerRecord, currentRecord);

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    //QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);

    if (data && eval) {
        bool oldde=dataEventsEnabled;
        dataEventsEnabled=false;
        //edtAlpha->setValue(eval->getAlpha());
        cmbWeights->setCurrentIndex(eval->getCurrentWeights());
        spinFitWidth->setValue(eval->getFitWidth());
        //edtNumIter->setRange(1,10000); //qMax(0,data->getCorrelationN())
        //edtNumIter->setValue(eval->getNumIter());
        for (int i=0; i<4; i++)     {
            chkSlope[i]->setChecked(eval->getTheoryEnabled(i));
            numPre[i]->setValue(eval->getTheoryPre(i));
            numD[i]->setValue(eval->getTheoryD(i));
            numAlpha[i]->setValue(eval->getTheoryAlpha(i));
        }

        updateSliders();

        dataEventsEnabled=oldde;
    }

}


void QFFCSMSDEvaluationEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);

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
        pltDistResults->set_doDrawing(false);
        pltDistResults->set_emitSignals(false);
        pltDistResults->clearGraphs();
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
        pltData->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltData->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltData->getYAxis()->set_minTicks(5);
        pltResiduals->getYAxis()->set_minTicks(5);
        pltData->get_plotter()->set_showKey(chkShowKey->isChecked());

        //qDebug()<<"   b "<<t.elapsed()<<" ms";
        t.start();

        int errorStyle=cmbErrorStyle->currentIndex();
        int plotStyle=cmbPlotStyle->currentIndex();
        //int residualStyle=cmbResidualStyle->currentIndex();

        if (data->getCorrelationN()>0) {
            size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");


            //////////////////////////////////////////////////////////////////////////////////
            // Plot average + error markers
            //////////////////////////////////////////////////////////////////////////////////
            size_t c_mean=0;
            QString graphName="";
            size_t c_std=0;
            QString errorName="";
            if (eval->getCurrentIndex()<0) {
                c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                graphName=tr("\\verb{%1} average").arg(record->getName());
                c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                errorName=tr("stddev");
            } else {
                if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                    c_mean=ds->addColumn(data->getCorrelationRun(eval->getCurrentIndex()), data->getCorrelationN(), QString("run"+QString::number(eval->getCurrentIndex())));
                    graphName=tr("\\verb{%1} %2").arg(record->getName()).arg(data->getCorrelationRunName(eval->getCurrentIndex()));
                    if (eval->getCurrentWeights()==2) {
                        c_std=ds->addColumn(data->getCorrelationRunError(eval->getCurrentIndex()), data->getCorrelationN(), "cperrunerror");
                        errorName=tr("per run");
                    } else {
                        c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                        errorName=tr("stddev");
                    }
                } else {
                    c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                    graphName=tr("\\verb{%1} average").arg(record->getName());
                    c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                    errorName=tr("stddev");
                }
            }
            JKQTPerrorPlotstyle styl=JKQTPnoError;
            switch (errorStyle) {
                case 1: styl=JKQTPerrorLines; break;
                case 2: styl=JKQTPerrorBars; break;
                case 3: styl=JKQTPerrorBarsLines; break;
            }
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
            pltDistResults->zoomToFit(false, true);

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
        pltDistResults->set_doDrawing(true);
        pltDistResults->set_emitSignals(true);
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
        pltDistResults->update_plot();
        //qDebug()<<"   m "<<t.elapsed()<<" ms";
        t.start();
        //qDebug()<<"  displayData end  runtime = "<<t1.elapsed()<<" ms";

        //setUpdatesEnabled(updEn);

    }
}


void QFFCSMSDEvaluationEditor::updateFitFunctions() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
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
                double* modelVals=(double*)malloc(N*sizeof(double));
                QVector<double> dist=eval->getMSD(record, index, model);
                QVector<double> distTau=eval->getMSDTaus(record, index, model);

                eval->evaluateModel(record, index, model, tauvals, modelVals, N, distTau.data(), dist.data(), dist.size());

                // CALCULATE FIT STATISTICS
                QFFitStatistics fit_stat=eval->calcFitStatistics(record, index, model, tauvals, modelVals, N, dist.size(), rangeMinDatarange, rangeMaxDatarange, runAvgWidth, residualHistogramBins);

                record->enableEmitResultsChanged();


                size_t c_fit = ds->addCopiedColumn(fit_stat.fitfunc, N, "fit_model");
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

                double N_particle=eval->getFitValue("n_particle");
                if (N_particle>0) {
                    JKQTPgeoInfiniteLine* g_Nline=new JKQTPgeoInfiniteLine(pltData->get_plotter(), tauvals[datacut->get_userMin()], 1.0/N_particle, 1,0,QColor("orange"));
                    pltData->addGraph(g_Nline);
                }
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_taures=c_tau;//dsres->addCopiedColumn(data->getCorrelationT(), N, "tau");
                size_t c_residuals=0;
                JKQTPxyLineGraph* g_residuals=new JKQTPxyLineGraph(pltResiduals->get_plotter());
                if (chkWeightedResiduals->isChecked()) {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals_weighted, N, "residuals_weighted");
                    g_residuals->set_title("weighted residuals");
                } else {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals, N, "residuals");
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
                size_t c_tauresra=dsres->addCopiedColumn(fit_stat.tau_runavg, fit_stat.runAvgN, "tau_resid_runavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg_weighted, fit_stat.runAvgN, "residuals_runavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg, fit_stat.runAvgN, "residuals_runavg");
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
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resWHistogram, residualHistogramBins, "residualhist_weighted_y");
                } else {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fit_stat.rmin+fit_stat.residHistBinWidth/2.0, fit_stat.rmax-fit_stat.residHistBinWidth/2.0, "residualhist_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resHistogram, residualHistogramBins, "residualhist_y");
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



                updateDistribution();

                updateDistributionResults();





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
                txtFit+=QString("<b>%1</b><cebter>").arg(tr("Fit Statistics:"));
                txtFit+=QString("<table border=\"0\" width=\"95%\">");
                //txtFit+=QString("<tr><td align=\"right\"></td><td align=\"left\"></td><td align=\"right\"></td><td align=\"left\"></td></tr>");
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residSqrSum).arg(fit_stat.residWeightSqrSum);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&lang;E&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &lang;E&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residAverage).arg(fit_stat.residWeightAverage);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residStdDev).arg(fit_stat.residWeightStdDev);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">NP =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">NR =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.fitparamN).arg(fit_stat.dataSize);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">DF =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\"></td><td align=\"left\" valign=\"bottom\"></td>"
                                "</tr>").arg(fit_stat.degFreedom);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">TSS  =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">R<sup>2</sup> =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.TSS).arg(fit_stat.Rsquared);
                //qDebug()<<"    l "<<t.elapsed()<<" ms";
                t.start();

                txtFit+=QString("</table><br><font size=\"-1\"><i>Legend:</i>: &chi;<sup>2</sup>: sum error square, &lang;E&rang;: residual average, &radic;&lang;E2&rang;: residual stddev., <br>NP: number of fit parameters, NR: number of residuals, <br>DF: degrees of freedom, R<sup>2</sup>: coefficient of determination, <br>TSS: total sum of squares</font>");
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
                free(weights);
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

void QFFCSMSDEvaluationEditor::displayParameters() {
    QFFCSByIndexAndModelEvaluationEditor::displayParameters();
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    QFSimpleFitParameterEnumeratorInterface* peval=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;
    spinFitWidth->setValue(eval->getFitWidth());
    //edtNumIter->setValue(eval->getNumIter());
    dataEventsEnabled=oldde;


    if (eval->hasResults(record)) {
        datacut->setEnabled(false);
        //edtNdist->setEnabled(false);
    } else {
        datacut->setEnabled(true);
        //edtNdist->setEnabled(true);
    }

}

void QFFCSMSDEvaluationEditor::distzoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender) {
    if (!dataEventsEnabled) return;
    if (sender==pltDistribution) {
        disconnect(pltDistResults, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));

        pltDistResults->setX(newxmin, newxmax);
        pltDistResults->zoomToFit(false, true);
        connect(pltDistResults, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));

    }
    if (sender==pltDistResults) {
        disconnect(pltDistribution, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
        pltDistribution->setX(newxmin, newxmax);
        pltDistribution->zoomToFit(false, true);
        connect(pltDistribution, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(distzoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    }
}

void QFFCSMSDEvaluationEditor::averageFirstFewFrames() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if ((!eval)||(!record)||(!data)) return;

    int data_start=datacut->get_userMin();
    int data_end=datacut->get_userMax();

    bool ok=false;
    int points=QInputDialog::getInt(this, windowTitle(), tr("number of points to average"), ProgramOptions::getConfigValue("QFFCSMSDEvaluationEditor/avgruns_last", 5).toInt(), 1, data_end-data_start, 1, &ok);
    if (ok) {
        ProgramOptions::setConfigValue("QFFCSMSDEvaluationEditor/avgruns_last", points);
        double *d=NULL;
        if (eval->getCurrentIndex()<0) {
            d=data->getCorrelationMean();
        } else {
            if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                d=data->getCorrelationRun(eval->getCurrentIndex());
            } else {
                d=data->getCorrelationMean();
            }
        }
        if (d) {
            double avg=0;
            double cnt=0;
            for (int i=data_start; i<data_start+points; i++) {
                avg=avg+d[i];
                cnt++;
            }
            avg=avg/cnt;
            for (int i=0; i<eval->getParameterCount(eval->getCurrentModel()); i++) {
                QString pid=eval->getParameterID(eval->getCurrentModel(), i);
                //qDebug()<<pid<<1.0/avg;
                if (pid=="n_particle") {
                    eval->setFitValue(record, eval->getCurrentIndex(), eval->getCurrentModel(), pid, 1.0/avg);
                }
            }
        }

    }
    widFitParams->updateWidgetValues();
    fitParamChanged();

}




void QFFCSMSDEvaluationEditor::fitCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;



    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));

    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doFit(record, eval->getCurrentIndex(), eval->getCurrentModel(), getUserMin(record, eval->getCurrentIndex()), getUserMax(record, eval->getCurrentIndex()), 11, spinResidualHistogramBins->value());    
    dataEventsEnabled=oldde;
    eval->emitResultsChanged(record);
    record->emitResultsChanged();


    updateSliders();
    displayParameters();
    displayData();
    dlgEvaluationProgress->setValue(100);

    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::fitRunsCurrent() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

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
    updateSliders();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::fitAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
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
    updateSliders();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFFCSMSDEvaluationEditor::fitRunsAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
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
    updateSliders();
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::copyMoreData(QFRawDataRecord *record, int index, int model) {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    //eval->setFitValue(record, index, model, "maxent_alpha", eval->getAlpha());
    //eval->setFitValue(record, index, model, "maxent_numiter", eval->getNumIter());
    //eval->setFitValue(record, index, model, "maxent_Ndist", eval->getNdist());
}

void QFFCSMSDEvaluationEditor::copyMoreDataToInitial() {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;

    /*eval->setInitFitValue("maxent_alpha", eval->getAlpha());
    eval->setInitFitValue("maxent_numiter", eval->getNumIter());
    eval->setInitFitValue("maxent_Ndist", eval->getNdist());*/
}

double fMSD( double t, const double *p )
{
    const double D=p[0];
    const double a=p[1];
    return 6.0*D*pow(t, a);
}

void QFFCSMSDEvaluationEditor::updateDistributionResults() {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;
    QVector<double> distTau=eval->getMSDTaus(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    QVector<double> dist=eval->getMSD(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    int data_start=sliderDist->get_userMin();
    int data_end=sliderDist->get_userMax();

    JKQTPdatastore* dsdist=pltDistResults->get_plotter()->getDatastore();

    bool updt=pltDistResults->get_doDrawing();
    pltDistResults->set_doDrawing(false);
    pltDistResults->clearGraphs();
    pltDistResults->get_plotter()->set_showKey(chkShowKeyDistResults->isChecked());

    dsdist->deleteAllColumns("msdfit_tau");
    dsdist->deleteAllColumns("msdfit_D");
    dsdist->deleteAllColumns("msdfit_alpha");

    int wid=spinFitWidth->value();

    QVector<double> fitTau;
    QVector<double> fitD, fitA;
    if (distTau.size()>1 && dist.size()>1 && spinFitWidth->value()>=3) {
        for (int i=0; i<distTau.size(); i+=qMax(1,spinFitWidth->value()/5)) {
        //for (int i=data_start; i<data_end; i+=qMax(1,spinFitWidth->value()/5)) {
            double* t=(double*)calloc(spinFitWidth->value(), sizeof(double));
            double* d=(double*)calloc(spinFitWidth->value(), sizeof(double));
            int cnt=0;
            for (int j=i; j<qMin(distTau.size(), i+spinFitWidth->value()); j++) {
                t[cnt]=distTau[j];
                d[cnt]=dist[j];
                cnt++;
            }

            if (cnt>3) {
                //qDebug()<<"fit "<<cnt<<" datapoints ...";
                fitTau.append(t[cnt/2]);

                double pout[2];
                int n_par = 2; // number of parameters
                int m_dat = cnt; // number of data pairs
                pout[0]=eval->getTheoryD(0);
                pout[1]=eval->getTheoryAlpha(0);
                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, t, d, fMSD, &control, &status );

                fitD.append(pout[0]);
                fitA.append(pout[1]);
                //qDebug()<<"    fit results  D="<<pout[0]<<"  a="<<pout[1];
            }

            free(t);
            free(d);
        }
        int c_msdtau=dsdist->addCopiedColumn(fitTau.data(), fitTau.size(), "msdfit_tau");
        int c_msdD=dsdist->addCopiedColumn(fitD.data(), fitD.size(), "msdfit_D");
        int c_msdA=dsdist->addCopiedColumn(fitA.data(), fitA.size(), "msdfit_alpha");

        double t_start=distTau.value(data_start, distTau.first());
        double t_end=distTau.value(data_end, distTau.last());
        //qDebug()<<"t_start="<<t_start<<"   t_end="<<t_end;
        //qDebug()<<"init: data_start="<<data_start<<"   data_end="<<data_end;

        data_start=0;
        data_end=fitTau.size();
        for (int i=0; i<fitTau.size(); i++) {
            //qDebug()<<"   "<<i<<": "<<fitTau[i];
            if (fitTau[i]<=t_start) data_start=i;
            if (fitTau[i]<=t_end) data_end=i;
        }

        //qDebug()<<"found: data_start="<<data_start<<"   data_end="<<data_end;

        if (cmbDistResultsMode->currentIndex()==0) {
            JKQTPxyLineGraph* g_msdfit=new JKQTPxyLineGraph(pltDistribution->get_plotter());
            g_msdfit->set_drawLine(true);
            g_msdfit->set_title("local \\alpha");
            g_msdfit->set_xColumn(c_msdtau);
            g_msdfit->set_yColumn(c_msdA);
            g_msdfit->set_datarange_start(data_start);
            g_msdfit->set_datarange_end(data_end);
            g_msdfit->set_symbol(JKQTPcross);
            g_msdfit->set_symbolSize(7);
            g_msdfit->set_color(QColor("red"));
            pltDistResults->addGraph(g_msdfit);
            pltDistResults->getYAxis()->set_axisLabel("local \\alpha");
        } else {
            JKQTPxyLineGraph* g_msdfit=new JKQTPxyLineGraph(pltDistribution->get_plotter());
            g_msdfit->set_drawLine(true);
            g_msdfit->set_title("local $D$");
            g_msdfit->set_xColumn(c_msdtau);
            g_msdfit->set_yColumn(c_msdD);
            g_msdfit->set_datarange_start(data_start);
            g_msdfit->set_datarange_end(data_end);
            g_msdfit->set_symbol(JKQTPcross);
            g_msdfit->set_symbolSize(7);
            g_msdfit->set_color(QColor("red"));
            pltDistResults->addGraph(g_msdfit);
            pltDistResults->getYAxis()->set_axisLabel("local $D$ [\\mu m^2/s]");
        }

    }

    // ensure that distribution and this plot are synched on x-axis
    pltDistribution->zoomToFit();
    pltDistResults->setX(pltDistribution->getXMin(), pltDistribution->getXMax());
    // zoom y-axis
    pltDistResults->zoomToFit(false, true);
    if (updt) {
        // ensure replot, if we have to
        pltDistResults->set_doDrawing(true);
        pltDistResults->update_plot();
    }
}

void QFFCSMSDEvaluationEditor::updateDistribution() {
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!eval) return;
    QVector<double> distTau=eval->getMSDTaus(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    QVector<double> dist=eval->getMSD(eval->getHighlightedRecord(), eval->getCurrentIndex(), eval->getCurrentModel());
    int data_start=sliderDist->get_userMin();
    int data_end=sliderDist->get_userMax();

    JKQTPdatastore* dsdist=pltDistribution->get_plotter()->getDatastore();


    bool updt=pltDistribution->get_doDrawing();
    pltDistribution->set_doDrawing(false);
    pltDistribution->clearGraphs();

    /////////////////////////////////////////////////////////////////////////////////
    // plot distribution
    /////////////////////////////////////////////////////////////////////////////////
    int c_disttau=-1;
    int c_dist=-1;
    dsdist->deleteAllColumns("msd_tau");
    dsdist->deleteAllColumns("msd");
    if (distTau.size()>0 && dist.size()>0) {
        c_disttau=dsdist->addCopiedColumn(distTau.data(), distTau.size(), "msd_tau");
        c_dist=dsdist->addCopiedColumn(dist.data(), dist.size(), "msd");
    } else {
        pltDistribution->setXY(pltData->getXMin(), pltData->getXMax(), pltData->getYMin(), pltData->getYMax());
    }
    JKQTPxyLineGraph* g_dist=new JKQTPxyLineGraph(pltDistribution->get_plotter());
    g_dist->set_drawLine(true);
    g_dist->set_title("MSD \\langle r^2(\\tau)\\rangle");
    g_dist->set_xColumn(c_disttau);
    g_dist->set_yColumn(c_dist);
    g_dist->set_datarange_start(data_start);
    g_dist->set_datarange_end(data_end);
    pltDistribution->addGraph(g_dist);
    QColor cb("white");
    cb.setAlphaF(0.5);
    pltDistribution->get_plotter()->set_keyBackgroundColor(cb);
    pltDistribution->get_plotter()->set_showKey(chkShowKeyDist->isChecked());



    /////////////////////////////////////////////////////////////////////////////////
    // plot theory curves
    /////////////////////////////////////////////////////////////////////////////////
    for (int i=0; i<MSDTHEORYCOUNT; i++)     {
        QVector<double> theo;
        if (chkSlope[i]->isChecked()) {
            const double D=numPre[i]->value()*numD[i]->value();
            const double a=numAlpha[i]->value();
            for (int x=0; x<distTau.size(); x++) {
                theo.append(D*pow(distTau[x], a));
            }
            dsdist->deleteAllColumns(QString("theory%1").arg(i+1));
            int c_theo=dsdist->addCopiedColumn(theo.data(), theo.size(), QString("theory%1").arg(i+1));
            JKQTPxyLineGraph* g_dist=new JKQTPxyLineGraph(pltDistribution->get_plotter());
            g_dist->set_drawLine(true);
            g_dist->set_title(tr("MSD %1, D=%2µm²/s, \\alpha=%3").arg(i+1).arg(numD[i]->value()).arg(numAlpha[i]->value()));
            g_dist->set_xColumn(c_disttau);
            g_dist->set_yColumn(c_theo);
            g_dist->set_datarange_start(sliderDist->get_userMin());
            g_dist->set_datarange_end(sliderDist->get_userMax());
            g_dist->set_lineWidth(1);
            g_dist->set_style(Qt::DashLine);

            pltDistribution->addGraph(g_dist);
        }
    }

    if (updt) {
        pltDistribution->zoomToFit();
        pltDistribution->set_doDrawing(true);
        pltDistribution->update_plot();
    }

}


void QFFCSMSDEvaluationEditor::fitWidthChanged(int width) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);

    bool rc=data->get_doEmitResultsChanged();
    bool pc=data->get_doEmitPropertiesChanged();
    data->set_doEmitResultsChanged(false);
    data->set_doEmitPropertiesChanged(false);
    if (data) data->setFitWidth(width);
    data->set_doEmitResultsChanged(rc);
    data->set_doEmitPropertiesChanged(pc);
    updateDistributionResults();
}




void QFFCSMSDEvaluationEditor::weightsChanged(int weights) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    data->setCurrentWeights(weights);
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMSDEvaluationEditor::theoryChanged() {
    QFFCSMSDEvaluationItem* data=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if (!dataEventsEnabled) return;
    if (!data) return;

    bool rc=data->get_doEmitResultsChanged();
    bool pc=data->get_doEmitPropertiesChanged();
    data->set_doEmitResultsChanged(false);
    data->set_doEmitPropertiesChanged(false);
    for (int i=0; i<MSDTHEORYCOUNT; i++)     {
        data->setTheory(i, chkSlope[i]->isChecked(), numPre[i]->value(), numD[i]->value(), numAlpha[i]->value());
    }
    data->set_doEmitResultsChanged(rc);
    data->set_doEmitPropertiesChanged(pc);

    updateDistribution();
}





////////////////////////////////////////////////////////////////////////////////////////////////////
// REPORT GENERATION
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void QFFCSMSDEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
    if ((!eval)||(!record))return;    // ||(!data) vielleicht noch hinzufÃ¼gen


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







void QFFCSMSDEvaluationEditor::createReportDoc(QTextDocument* document) {
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
    QFFCSMSDEvaluationItem* eval=qobject_cast<QFFCSMSDEvaluationItem*>(current);
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
    cursor.insertText(tr("FCS Mean Squared Displacement Fit Report:\n\n"), fHeading1);
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
    table->cellAt(2, 1).firstCursorPosition().insertText(eval->getModelName(eval->getCurrentModel()), fText);
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

    table = cursor.insertTable(4, 2, tableFormat1);
    table->mergeCells(0,0,2,1);
    table->mergeCells(2,1,2,1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltData->get_plotter()->draw(*painter, QRect(0,0,pltData->width(),pltData->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation data, model and residuals:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();

    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltDistribution->get_plotter()->draw(*painter, QRect(0,0,pltDistribution->width(),pltDistribution->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("Mean Squared Displacement (MSD):\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    int old_mode=cmbDistResultsMode->currentIndex();
    {
        QTextCursor tabCursor=table->cellAt(1, 1).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        cmbDistResultsMode->setCurrentIndex(0);
        QApplication::processEvents();
        QApplication::processEvents();
        pltDistResults->get_plotter()->draw(*painter, QRect(0,0,pltDistribution->width(),pltDistribution->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("local anomality:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();

    {
        QTextCursor tabCursor=table->cellAt(2, 1).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        cmbDistResultsMode->setCurrentIndex(1);
        QApplication::processEvents();
        QApplication::processEvents();
        pltDistResults->get_plotter()->draw(*painter, QRect(0,0,pltDistribution->width(),pltDistribution->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width(); //0.62
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("local diffusion coefficient:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    cmbDistResultsMode->setCurrentIndex(old_mode);
    QApplication::processEvents();


    {
        QTextCursor tabCursor=table->cellAt(2, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltResidualHistogram->get_plotter()->draw(*painter, QRect(0,0,pltResidualHistogram->width(),pltResidualHistogram->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width();//0.3
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual histogram:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    {
        QTextCursor tabCursor=table->cellAt(3,0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltResidualCorrelation->get_plotter()->draw(*painter, QRect(0,0,pltResidualCorrelation->width(),pltResidualCorrelation->width()));
        delete painter;
        double scale=document->textWidth()*0.4/pic.boundingRect().width();//0.3
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual correlations:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
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
    table = cursor.insertTable(qMax(6,2+eval->getParameterCount(eval->getCurrentModel())), 4, tableFormat2);
    //table = cursor.insertTable(ceil((double)fitParamCount/2.0)+1, 12, tableFormat2);
    QTextCursor tableCursor;
    QApplication::processEvents();


    tableCursor=table->cellAt(0, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Evaluation Parameters"), fTextBoldSmall);

    tableCursor=table->cellAt(0, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(labFitParameters->text(), fTextBoldSmall);

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
    tableCursor.insertText(tr(" fit width =  "), fTextSmall);

    int k=0;
    for (int i=0; i<3; i++) {
        if (chkSlope[i]->isChecked()) {
            tableCursor=table->cellAt(3+k, 0).firstCursorPosition();
            tableCursor.setBlockFormat(bfRightNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertText(tr(" theory %1 =  ").arg(i+1), fTextSmall);

            tableCursor=table->cellAt(3+k, 1).firstCursorPosition();
            tableCursor.setBlockFormat(bfCenterNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\"> %1 &middot; %2 &middot; &tau;<sup>%3</sup> [&mu;m<sup>2</sup>/s]</font>").arg(numPre[i]->value()).arg(numD[i]->value()).arg(numAlpha[i]->value())));

            k++;
        }
    }

    k=0;
    for (int i=0; i<eval->getParameterCount(eval->getCurrentModel()); i++) {
        tableCursor=table->cellAt(1+k, 2).firstCursorPosition();
        tableCursor.setBlockFormat(bfRightNB);
        tableCursor.setBlockCharFormat(fTextSmall);
        tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\"> %1 =  </font>").arg(eval->getParameterName(eval->getCurrentModel(), i, true))));

        tableCursor=table->cellAt(1+k, 3).firstCursorPosition();
        tableCursor.setBlockFormat(bfCenterNB);
        tableCursor.setBlockCharFormat(fTextSmall);
        tableCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<font size=\"-1\"> %1</font>").arg(eval->getFitValue(eval->getParameterID(eval->getCurrentModel(), i)))));
        k++;
    }


    tableCursor=table->cellAt(1,1).firstCursorPosition();
    tableCursor.setBlockFormat(bfCenterNB);
    tableCursor.setBlockCharFormat(fTextSmall);
    tableCursor.insertText(QString("%1 [%2]").arg((eval->getCurrentIndex()<0)?tr("average"):QString::number(eval->getCurrentIndex())).arg(fcs->getCorrelationRunName(eval->getCurrentIndex())), fTextSmall);


    cursor.setBlockFormat(bfLeft);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(); cursor.insertBlock();
    QString htmlBot=fitStatisticsReport;
    htmlBot.replace("width=\"95%\"", "");
    cursor.insertFragment(QTextDocumentFragment::fromHtml(htmlBot));


}








