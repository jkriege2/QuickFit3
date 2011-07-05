#include "qffcsfitevaluationeditor.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include <iostream>
#include <cfloat>
#include "qffcsfitevaluation.h"
#include "tools.h"
#include "statistics_tools.h"
#include <QThread>
#include "dlgestimatefocalvolume.h"
#include "qmoretextobject.h"
#include "qmodernprogresswidget.h"







QFFCSFitEvaluationEditor::QFFCSFitEvaluationEditor(QFPluginServices* services, QWidget* parent):
    QFEvaluationEditor(services, parent)
{
    cmbModel=NULL;
    dataEventsEnabled=true;
    m_parameterWidgetWidth=75;
    m_parameterCheckboxWidth=32;
    fitStatisticsReport="";

    doFitThread=new QFFitAlgorithmThreadedFit(this);

    createWidgets();
}

QFFCSFitEvaluationEditor::~QFFCSFitEvaluationEditor()
{
    //dtor
}


void QFFCSFitEvaluationEditor::createWidgets() {
    dlgFitProgress = new dlgQFFitAlgorithmProgressDialog(this);
    dlgFitProgressReporter=new dlgQFFitAlgorithmProgressDialogReporter(dlgFitProgress);


    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    mainLayout->setContentsMargins(5,5,5,5);
    setLayout(mainLayout);


    QWidget* topWidget=new QWidget(this);
    QVBoxLayout* vbl=new QVBoxLayout(topWidget);
    vbl->setContentsMargins(0,0,0,0);
    topWidget->setLayout(vbl);

    QFormLayout* fl;//=new QFormLayout(this);
    QLabel* l;
    QHBoxLayout* hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    cmbAlgorithm=new QComboBox(this);
    l=new QLabel(tr("Fit &Algorithm: "), this);
    l->setBuddy(cmbAlgorithm);
    hbl->addWidget(l);
    cmbAlgorithm->setEditable(false);
    hbl->addWidget(cmbAlgorithm);
    btnConfigAlgorithm=new QPushButton(QIcon(":/fcs_fit_config.png"), tr("&Configure Algorithm"), this);
    btnConfigAlgorithm->setMaximumWidth(250);
    hbl->addWidget(btnConfigAlgorithm);
    btnAlgorithmHelp=new QPushButton(QIcon(":/fcs_fit_help.png"), tr("Algorithm &Help"), this);
    btnAlgorithmHelp->setMaximumWidth(250);
    hbl->addWidget(btnAlgorithmHelp);
    //fl->addRow(l, hbl);

    cmbWeights=new QComboBox(this);
    cmbWeights->setEditable(false);
    cmbWeights->addItem(tr("equal weights"));
    cmbWeights->addItem(tr("standard deviation"));
    cmbWeights->addItem(tr("per run errors"));
    cmbWeights->setMaximumWidth(150);
    cmbWeights->setMinimumWidth(150);
    l=new QLabel(tr("&Weight Model: "), this);
    l->setBuddy(cmbWeights);
    hbl->addSpacing(32);
    hbl->addWidget(l);
    hbl->addWidget(cmbWeights);
    hbl->addStretch();
    //fl->addRow(tr("&Weight Model:"), cmbWeights);
    //vbl->addLayout(fl);
    vbl->addLayout(hbl);


    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    cmbModel=new QComboBox(this);
    l=new QLabel(tr("Fit &Model:"), this);
    l->setBuddy(cmbModel);
    hbl->addWidget(l);
    cmbModel->setEditable(false);
    hbl->addWidget(cmbModel);
    btnModelHelp=new QPushButton(QIcon(":/fcs_fit_help.png"), tr("Model H&elp"), this);
    btnModelHelp->setMaximumWidth(250);
    hbl->addWidget(btnModelHelp);
    hbl->addStretch();
    vbl->addLayout(hbl);


    /*QFrame* frame=new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    frame->setMinimumHeight(5);
    frame->setMaximumHeight(5);
    vbl->addWidget(frame);*/

    QFont boldfont=font();
    boldfont.setBold(true);

    toolbar=new QToolBar("toolbar_fcs_fit", this);
    vbl->addWidget(toolbar);
    actSaveReport=new QAction(QIcon(":/fcs_fit_savereport.png"), tr("Save Report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actPrintReport=new QAction(QIcon(":/fcs_fit_printreport.png"), tr("Print Report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    QLabel* lPS=new QLabel(tr("   &Plots: "), toolbar);
    lPS->setFont(boldfont);
    cmbPlotStyle=new QComboBox(toolbar);
    cmbPlotStyle->addItem(QIcon(":/fcsplot_points.png"), tr("points"));
    cmbPlotStyle->addItem(QIcon(":/fcsplot_lines.png"), tr("lines"));
    cmbPlotStyle->addItem(QIcon(":/fcsplot_pointslines.png"), tr("lines + points"));
    lPS->setBuddy(cmbPlotStyle);
    QLabel* lES=new QLabel(tr("   &Errors: "), toolbar);
    lES->setFont(boldfont);
    cmbErrorStyle=new QComboBox(toolbar);
    cmbErrorStyle->addItem(QIcon(":/fcsplot_enone.png"), tr("none"));
    cmbErrorStyle->addItem(QIcon(":/fcsplot_elines.png"), tr("lines"));
    cmbErrorStyle->addItem(QIcon(":/fcsplot_ebars.png"), tr("bars"));
    cmbErrorStyle->addItem(QIcon(":/fcsplot_elinesbars.png"), tr("bars + lines"));
    lES->setBuddy(cmbErrorStyle);

    QLabel* lRS=new QLabel(tr("   &Residuals: "), toolbar);
    lRS->setFont(boldfont);
    cmbResidualStyle=new QComboBox(toolbar);
    cmbResidualStyle->addItem(QIcon(":/fcsplot_points.png"), tr("points"));
    cmbResidualStyle->addItem(QIcon(":/fcsplot_lines.png"), tr("lines"));
    cmbResidualStyle->addItem(QIcon(":/fcsplot_pointslines.png"), tr("lines + points"));
    lRS->setBuddy(cmbResidualStyle);

    chkWeightedResiduals=new QCheckBox(tr("&weighted  "), toolbar);

    QWidget* sp1=new QWidget(this);
    sp1->setMinimumWidth(10);
    sp1->setMaximumWidth(10);
    chkGrid=new QCheckBox(tr("&Grid   "), toolbar);
    chkXLogScale=new QCheckBox(tr("&log. tau   "), toolbar);
    chkXLogScale->setVisible(false);
    labMousePosition=new QLabel(this);
    QWidget* sp2=new QWidget(this);
    sp2->setMinimumWidth(10);
    sp2->setMaximumWidth(10);

    mainLayout->addWidget(topWidget, 0);

    splitModel=new QSplitter(this),
    splitModel->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(splitModel, 10);

    splitPlot=new QSplitter(this),
    splitPlot->setOrientation(Qt::Vertical);
    splitModel->addWidget(splitPlot);

    pltData=new JKQtPlotter(true, this);
    pltData->resize(300,200);
    splitPlot->addWidget(pltData);


    QWidget* widgetResiduals=new QWidget(this);
    vbl=new QVBoxLayout(widgetResiduals);
    vbl->setContentsMargins(0,0,0,0);
    vbl->setSpacing(1);
    widgetResiduals->setLayout(vbl);

    pltResiduals=new JKQtPlotter(true, this);
    pltResiduals->resize(300,50);
    vbl->addWidget(pltResiduals,1);

    pltData->get_plotter()->set_gridPrinting(true);
    pltData->get_plotter()->addGridPrintingPlotter(0,1,pltResiduals->get_plotter());
    pltData->set_displayToolbar(false);
    pltResiduals->set_displayToolbar(false);
    pltResiduals->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    pltResiduals->getXAxis()->set_labelFontSize(11);
    pltResiduals->getXAxis()->set_tickLabelFontSize(10);
    pltResiduals->getYAxis()->set_axisLabel(tr("residuals"));
    pltResiduals->getYAxis()->set_labelFontSize(11);
    pltResiduals->getYAxis()->set_tickLabelFontSize(10);
    pltData->getXAxis()->set_axisLabel("");
    pltData->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    pltData->getYAxis()->set_labelFontSize(11);
    pltData->getYAxis()->set_tickLabelFontSize(10);
    pltData->getXAxis()->set_drawMode1(JKQTPCADMticks);
    pltData->getXAxis()->set_tickLabelFontSize(10);
    pltResiduals->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    pltResiduals->get_plotter()->setBorder(1,1,1,1);
    pltData->get_plotter()->setBorder(1,1,1,1);
    pltResiduals->synchronizeToMaster(pltData, true, false);
    pltData->get_plotter()->set_useAntiAliasingForSystem(true);
    pltData->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltResiduals->get_plotter()->set_useAntiAliasingForSystem(true);
    pltResiduals->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltData->set_displayMousePosition(false);
    pltResiduals->set_displayMousePosition(false);
    pltData->get_plotter()->set_keyFontSize(9);
    pltData->get_plotter()->set_keyXMargin(2);
    pltData->get_plotter()->set_keyYMargin(2);
    pltResiduals->get_plotter()->set_keyFontSize(9);
    pltResiduals->get_plotter()->set_keyXMargin(2);
    pltResiduals->get_plotter()->set_keyYMargin(2);
    pltResiduals->useExternalDatastore(pltData->getDatastore());


    datacut=new DataCutSliders(this);
    vbl->addWidget(datacut,0);
    splitPlot->addWidget(widgetResiduals);

    splitFitStatistics=new QSplitter(this);
    splitPlot->addWidget(splitFitStatistics);
    tabResidulas=new QTabWidget(this);
    //tabResidulas->setTabPosition(QTabWidget::West);
    tabResidulas->setTabPosition(QTabWidget::North);
    tabResidulas->setTabShape(QTabWidget::Triangular);
    tabResidulas->setDocumentMode(true);
    splitFitStatistics->addWidget(tabResidulas);
    //QHBoxLayout* layFitStat=new QHBoxLayout(this);
    //layFitStat->setContentsMargins(0,0,0,0);
    //layFitStat->setMargin(0);
    //QWidget* wFitStat=new QWidget(this);
    //wFitStat->setLayout(layFitStat);
    //splitFitStatistics->addWidget(wFitStat);
    pltResidualHistogram=new JKQtPlotter(true, this);
    pltResidualHistogram->resize(200,200);
    pltResidualHistogram->set_displayToolbar(false);
    pltResidualHistogram->getXAxis()->set_axisLabel(tr("residuals"));
    pltResidualHistogram->getXAxis()->set_labelFontSize(8);
    pltResidualHistogram->getXAxis()->set_tickLabelFontSize(8);
    pltResidualHistogram->getXAxis()->set_minTicks(5);
    pltResidualHistogram->getYAxis()->set_axisLabel(tr("frequency"));
    pltResidualHistogram->getYAxis()->set_labelFontSize(8);
    pltResidualHistogram->getYAxis()->set_tickLabelFontSize(8);
    pltResidualHistogram->getYAxis()->set_minTicks(5);
    pltResidualHistogram->get_plotter()->setBorder(0,0,0,0);
    pltResidualHistogram->get_plotter()->set_useAntiAliasingForSystem(true);
    pltResidualHistogram->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltResidualHistogram->set_displayMousePosition(false);
    pltResidualHistogram->get_plotter()->set_keyFontSize(8);
    pltResidualHistogram->get_plotter()->set_keyXMargin(1);
    pltResidualHistogram->get_plotter()->set_keyYMargin(1);
    pltResidualHistogram->getYAxis()->set_minTicks(5);
    pltResidualHistogram->getYAxis()->set_minTicks(5);
    pltResidualHistogram->useExternalDatastore(pltData->getDatastore());
    tabResidulas->addTab(pltResidualHistogram, tr("Histogram"));


    pltResidualCorrelation=new JKQtPlotter(true, this);
    pltResidualCorrelation->resize(200,200);
    pltResidualCorrelation->set_displayToolbar(false);
    pltResidualCorrelation->getXAxis()->set_axisLabel(tr("lag [residual number]"));
    pltResidualCorrelation->getXAxis()->set_labelFontSize(8);
    pltResidualCorrelation->getXAxis()->set_tickLabelFontSize(8);
    pltResidualCorrelation->getXAxis()->set_minTicks(5);
    pltResidualCorrelation->getYAxis()->set_axisLabel(tr("autocorrelation"));
    pltResidualCorrelation->getYAxis()->set_labelFontSize(8);
    pltResidualCorrelation->getYAxis()->set_tickLabelFontSize(8);
    pltResidualCorrelation->getYAxis()->set_minTicks(3);
    pltResidualCorrelation->get_plotter()->setBorder(0,0,0,0);
    pltResidualCorrelation->get_plotter()->set_useAntiAliasingForSystem(true);
    pltResidualCorrelation->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltResidualCorrelation->set_displayMousePosition(false);
    pltResidualCorrelation->get_plotter()->set_keyFontSize(8);
    pltResidualCorrelation->get_plotter()->set_keyXMargin(1);
    pltResidualCorrelation->get_plotter()->set_keyYMargin(1);
    pltResidualCorrelation->getYAxis()->set_minTicks(5);
    pltResidualCorrelation->getYAxis()->set_minTicks(5);
    pltResidualCorrelation->useExternalDatastore(pltData->getDatastore());
    tabResidulas->addTab(pltResidualCorrelation, tr("Correlation"));

    layResidualAnalysis=new QFormLayout(this);
    layResidualAnalysis->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    QWidget* widResidualParameters=new QWidget(this);
    widResidualParameters->setLayout(layResidualAnalysis);
    //layFitStat->addLayout(layResidualAnalysis, 1);
    spinResidualHistogramBins=new QSpinBox(this);
    spinResidualHistogramBins->setRange(1,1000);
    layResidualAnalysis->addRow(tr("# bins:"), spinResidualHistogramBins);

    tabResidulas->addTab(widResidualParameters, tr("Parameters"));

    txtFitStatistics=new QTextEdit(this);
    txtFitStatistics->setReadOnly(true);
    splitFitStatistics->addWidget(txtFitStatistics);



    QWidget* modelWidget=new QWidget(this);
    QVBoxLayout* layModel=new QVBoxLayout(this);
    modelWidget->setLayout(layModel);

    fl=new QFormLayout(this);
    fl->setContentsMargins(9,0,0,0);
    fl->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    spinRun=new QSpinBox(this);
    spinRun->setMinimum(-1);
    spinRun->setMaximum(-1);
    spinRun->setSpecialValueText(tr("average"));
    spinRun->setMinimumWidth(100);
    labRun=new QLabel(this);
    QHBoxLayout* hblRun=new QHBoxLayout();
    hblRun->setContentsMargins(0,0,0,0);
    hblRun->addWidget(spinRun, 1);
    hblRun->addWidget(labRun, 2);
    fl->addRow(tr("run: "), hblRun);
    layModel->addLayout(fl, 0);

    labFitParameters=new QLabel(this);
    layModel->addWidget(labFitParameters);
    btnEditRanges=new QPushButton(tr("Edit &Ranges"), this);
    btnEditRanges->setCheckable(true);
    btnEditRanges->setChecked(false);
    QHBoxLayout* hblfp=new QHBoxLayout(this);
    hblfp->addWidget(labFitParameters);
    hblfp->addStretch();
    hblfp->addWidget(btnEditRanges);
    layModel->addLayout(hblfp);



    scrollParameters=new JKVerticalScrollArea(this);
    layModel->addWidget(scrollParameters, 100);
    layModel->setSpacing(2);
    QWidget* widParameters=new QWidget(this);
    scrollParameters->setWidget(widParameters);
    scrollParameters->setWidgetResizable(true);
    layParameters=new QGridLayout(this);
    layParameters->setSpacing(2);
    layParameters->setVerticalSpacing(1);
    layParameters->setHorizontalSpacing(0);
    layParameters->setMargin(4);
    widParameters->setLayout(layParameters);


    QGridLayout* layBtn=new QGridLayout(this);
    layBtn->setContentsMargins(0,0,0,0);
    btnFitCurrent=new QPushButton(QIcon(":/fcs_fit_fit.png"), tr("&Fit Current"), this);
    btnFitCurrent->setToolTip(tr("perform a fit for the currently displayed file and run"));
    layBtn->addWidget(btnFitCurrent, 0, 0);
    btnFitRunsCurrent=new QPushButton(QIcon(":/fcs_fit_fit.png"), tr("Fit All &Runs "), this);
    btnFitRunsCurrent->setToolTip(tr("perform a fit for all runs in the currently selected file "));
    layBtn->addWidget(btnFitRunsCurrent, 0, 1);
    btnFitAll=new QPushButton(QIcon(":/fcs_fit_fit.png"), tr("Fit All &Files (Current Run)"), this);
    btnFitAll->setToolTip(tr("perform a fit for all files, but fit in each file only the currently displayed run"));
    layBtn->addWidget(btnFitAll, 1, 0);
    btnFitRunsAll=new QPushButton(QIcon(":/fcs_fit_fit.png"), tr("Fit &Everything"), this);
    btnFitRunsAll->setToolTip(tr("perform a fit for all runs in all files"));
    layBtn->addWidget(btnFitRunsAll, 1, 1);
    btnResetCurrent=new QPushButton(tr("&Reset Current"), this);
    btnResetCurrent->setToolTip(tr("reset the currently displayed file (and run) to the initial parameters\nThis deletes all fit results stored for the current file."));
    layBtn->addWidget(btnResetCurrent, 2, 0);
    btnResetAll=new QPushButton(tr("&Reset All"), this);
    btnResetAll->setToolTip(tr("reset all loaded files to the initial parameters.\nThis deletes all fit results stored for all files file."));
    layBtn->addWidget(btnResetAll, 2, 1);
    btnCopyToInitial=new QPushButton(tr("Copy to &Initial"), this);
    btnCopyToInitial->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters, so they are used by files/runs that were not fit yet."));
    layBtn->addWidget(btnCopyToInitial, 3, 0);
    btnCopyToAll=new QPushButton(tr("&Copy to All"), this);
    btnCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters and also to all files."));
    layBtn->addWidget(btnCopyToAll, 3, 1);
    btnCopyToAllCurrentRun=new QPushButton(tr("&Copy to All (Current Run)"), this);
    btnCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters and also to all files, but only to the current run therein."));
    layBtn->addWidget(btnCopyToAllCurrentRun, 4, 0);

    btnCalibrateFocalVolume=new QPushButton(QIcon(":/fcs_focalvolume.png"), tr("Focal &Volume"), this);
    btnCalibrateFocalVolume->setToolTip(tr("estimate the focal volume"));
    layBtn->addWidget(btnCalibrateFocalVolume, 5, 0);

    btnLoadParameters=new QPushButton(QIcon(":/fcs_param_load.png"), tr("&Load Parameters"), this);
    btnLoadParameters->setToolTip(tr("load a FCS fit parameter set"));
    layBtn->addWidget(btnLoadParameters, 6, 0);
    btnSaveParameters=new QPushButton(QIcon(":/fcs_param_save.png"), tr("&Save Parameters"), this);
    btnSaveParameters->setToolTip(tr("save the current FCS fit parameter as a set"));
    layBtn->addWidget(btnSaveParameters, 6, 1);

    layModel->addLayout(layBtn);



    labFitResult=new QLabel(this);
    labFitResult->setWordWrap(true);
    layModel->addWidget(labFitResult);

    splitModel->addWidget(modelWidget);




    splitPlot->setCollapsible(0, false);
    splitPlot->setCollapsible(1, false);
    splitPlot->setStretchFactor(0,4);
    splitPlot->setStretchFactor(1,1);

    splitModel->setCollapsible(0, false);
    splitModel->setCollapsible(1, false);
    splitModel->setStretchFactor(0,4);
    splitModel->setStretchFactor(1,1);

    hlpAlgorithm=new QFHTMLHelpWindow(0);
    hlpAlgorithm->close();
    hlpFunction=new QFHTMLHelpWindow(0);
    hlpFunction->close();

    toolbar->addAction(actSaveReport);
    toolbar->addAction(actPrintReport);
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actSavePlot()); pltData->get_plotter()->get_actSavePlot()->setIcon(QIcon(":/fcsplot_saveplot.png"));
    toolbar->addAction(pltData->get_plotter()->get_actSaveData()); pltData->get_plotter()->get_actSaveData()->setIcon(QIcon(":/fcsplot_savedata.png"));
    toolbar->addAction(pltData->get_plotter()->get_actPrint()); pltData->get_plotter()->get_actPrint()->setIcon(QIcon(":/fcsplot_print.png"));
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actZoomAll()); pltData->get_plotter()->get_actZoomAll()->setIcon(QIcon(":/fcsplot_zoomall.png"));
    connect(pltData->get_plotter()->get_actZoomAll(), SIGNAL(triggered()), pltResiduals, SLOT(zoomToFit()));
    toolbar->addSeparator();
    toolbar->addWidget(lPS);
    toolbar->addWidget(cmbPlotStyle);
    toolbar->addWidget(lES);
    toolbar->addWidget(cmbErrorStyle);
    toolbar->addWidget(lRS);
    toolbar->addWidget(chkWeightedResiduals);
    toolbar->addWidget(cmbResidualStyle);
    toolbar->addWidget(sp1);
    toolbar->addWidget(chkGrid);
    toolbar->addWidget(chkXLogScale);
    toolbar->addSeparator();
    toolbar->addWidget(sp2);
    toolbar->addWidget(labMousePosition);

    connect(btnAlgorithmHelp, SIGNAL(clicked()), this, SLOT(displayFitAlgorithmHelp()));
    connect(btnConfigAlgorithm, SIGNAL(clicked()), this, SLOT(configFitAlgorithm()));
    connect(btnModelHelp, SIGNAL(clicked()), this, SLOT(displayFitFunctionHelp()));
    connect(pltData, SIGNAL(zoomChangedLocally(double, double, double, double, JKQtPlotter*)), this, SLOT(zoomChangedLocally(double, double, double, double, JKQtPlotter*)));
    connect(pltData, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResiduals, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualHistogram, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualCorrelation, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));

    connect(btnFitCurrent, SIGNAL(clicked()), this, SLOT(fitCurrent()));
    connect(btnFitAll, SIGNAL(clicked()), this, SLOT(fitAll()));
    connect(btnFitRunsAll, SIGNAL(clicked()), this, SLOT(fitRunsAll()));
    connect(btnFitRunsCurrent, SIGNAL(clicked()), this, SLOT(fitRunsCurrent()));
    connect(btnResetCurrent, SIGNAL(clicked()), this, SLOT(resetCurrent()));
    connect(btnResetAll, SIGNAL(clicked()), this, SLOT(resetAll()));
    connect(btnCopyToAll, SIGNAL(clicked()), this, SLOT(copyToAll()));
    connect(btnCopyToInitial, SIGNAL(clicked()), this, SLOT(copyToInitial()));
    connect(btnCopyToAllCurrentRun, SIGNAL(clicked()), this, SLOT(copyToAllCurrentRun()));
    connect(btnLoadParameters, SIGNAL(clicked()), this, SLOT(loadCurrentFitResults()));
    connect(btnSaveParameters, SIGNAL(clicked()), this, SLOT(saveCurrentFitResults()));
    connect(btnCalibrateFocalVolume, SIGNAL(clicked()), this, SLOT(calibrateFocalVolume()));

    connect(datacut, SIGNAL(copyUserMinToAll(int)), this, SLOT(copyUserMinToAll(int)));
    connect(datacut, SIGNAL(copyUserMaxToAll(int)), this, SLOT(copyUserMaxToAll(int)));

}


void QFFCSFitEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    QFFCSFitEvaluation* fcs=qobject_cast<QFFCSFitEvaluation*>(current);

    if (old!=NULL) {
        disconnect(old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        disconnect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        disconnect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(runChanged(int)));
        disconnect(cmbAlgorithm, SIGNAL(currentIndexChanged(int)), this, SLOT(algorithmChanged(int)));
        disconnect(cmbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
        disconnect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        disconnect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotStyleChanged(int)));
        disconnect(cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(errorStyleChanged(int)));
        disconnect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(chkGridToggled(bool)));
        disconnect(chkXLogScale, SIGNAL(toggled(bool)), this, SLOT(chkXLogScaleToggled(bool)));
        disconnect(cmbResidualStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(residualStyleChanged(int)));
        disconnect(chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(chkWeightedResidualsToggled(bool)));
        disconnect(spinResidualHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(residualHistogramBinsChanged(int)));

        cmbModel->clear();
        cmbAlgorithm->clear();
        spinRun->setMaximum(-1);
    }



    if (fcs) {

        dataEventsEnabled=false;
        chkXLogScale->setChecked(current->getProperty("plot_taulog", true).toBool());
        chkGrid->setChecked(current->getProperty("plot_grid", true).toBool());
        cmbPlotStyle->setCurrentIndex(current->getProperty("plot_style", 1).toInt());
        cmbErrorStyle->setCurrentIndex(current->getProperty("plot_errorstyle", 0).toInt());
        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        cmbResidualStyle->setCurrentIndex(current->getProperty("plot_residualsstyle", 0).toInt());
        chkWeightedResiduals->setChecked(current->getProperty("weighted_residuals", false).toBool());
        spinResidualHistogramBins->setValue(current->getProperty("plot_residualshistogrambins", 10).toInt());

        QStringList ff=fcs->getAvailableFitFunctions();
        for (int i=0; i<ff.size(); i++) {
            QString id=ff[i];
            cmbModel->addItem(fcs->getFitFunction(id)->name(), id);
        }
        ff=fcs->getAvailableFitAlgorithms();
        for (int i=0; i<ff.size(); i++) {
            QString id=ff[i];
            cmbAlgorithm->addItem(fcs->getFitAlgorithm(id)->name(), id);
        }


        if (fcs->getFitFunction()!=NULL) {
            cmbModel->setCurrentIndex(cmbModel->findData(fcs->getFitFunction()->id()));
        } else {
            cmbModel->setCurrentIndex(cmbModel->findData(-1));
        }
        if (fcs->getFitAlgorithm()!=NULL) {
            cmbAlgorithm->setCurrentIndex(cmbAlgorithm->findData(fcs->getFitAlgorithm()->id()));
        } else {
            cmbAlgorithm->setCurrentIndex(-1);
        }
        dataEventsEnabled=true;
    }

    connect(cmbAlgorithm, SIGNAL(currentIndexChanged(int)), this, SLOT(algorithmChanged(int)));
    connect(current, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
    connect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
    connect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(runChanged(int)));
    connect(cmbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
    connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
    connect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotStyleChanged(int)));
    connect(cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(errorStyleChanged(int)));
    connect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(chkGridToggled(bool)));
    connect(chkXLogScale, SIGNAL(toggled(bool)), this, SLOT(chkXLogScaleToggled(bool)));
    connect(cmbResidualStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(residualStyleChanged(int)));
    connect(chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(chkWeightedResidualsToggled(bool)));
    connect(spinResidualHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(residualHistogramBinsChanged(int)));

    displayModel(true);
    replotData();
}

void QFFCSFitEvaluationEditor::resultsChanged() {
}

void QFFCSFitEvaluationEditor::readSettings() {
    if (cmbModel) {
        //cmbAlgorithm->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/algorithm", cmbAlgorithm->currentIndex()).toInt());
        //cmbModel->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/model", cmbModel->currentIndex()).toInt());
        //cmbWeights->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/weights", cmbWeights->currentIndex()).toInt());
        pltData->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltdata/");
        pltResiduals->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresiduals/");
        pltResidualHistogram->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresidualhistogram/");
        pltResidualCorrelation->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresidualcorrelation/");
        hlpAlgorithm->readSettings(*settings->getQSettings(), "fcsfitevaleditor/algorithm_");
        hlpFunction->readSettings(*settings->getQSettings(), "fcsfitevaleditor/function_");
        loadSplitter(*(settings->getQSettings()), splitPlot, "fcsfitevaleditor/splitter_plot");
        loadSplitter(*(settings->getQSettings()), splitModel, "fcsfitevaleditor/splitter_model");
        loadSplitter(*(settings->getQSettings()), splitFitStatistics, "fcsfitevaleditor/splitter_fitstatistics");
        m_parameterWidgetWidth=settings->getQSettings()->value("fcsfitevaleditor/parameterWidgetWidth", m_parameterWidgetWidth).toInt();
        m_parameterCheckboxWidth=settings->getQSettings()->value("fcsfitevaleditor/parameterCheckboxWidth", m_parameterCheckboxWidth).toInt();
        btnEditRanges->setChecked(settings->getQSettings()->value("fcsfitevaleditor/display_range_widgets", false).toBool());
        spinResidualHistogramBins->setValue(settings->getQSettings()->value("fcsfitevaleditor/residual_histogram_bins", 10).toInt());
        tabResidulas->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/residual_toolbox_current", 0).toInt());
        currentFPSSaveDir=settings->getQSettings()->value("fcsfitevaleditor/lastFPSDirectory", currentFPSSaveDir).toString();
    }
}

void QFFCSFitEvaluationEditor::writeSettings() {
    if (cmbModel) {
        //settings->getQSettings()->setValue("fcsfitevaleditor/splitter_plot", splitPlot->saveState());
        settings->getQSettings()->setValue("fcsfitevaleditor/algorithm", cmbAlgorithm->currentIndex());
        settings->getQSettings()->setValue("fcsfitevaleditor/model", cmbModel->currentIndex());
        settings->getQSettings()->setValue("fcsfitevaleditor/weights", cmbWeights->currentIndex());
        //pltData->saveSettings(*settings->getQSettings(), "fcsfitevaleditor/pltdata/");
        //pltResiduals->saveSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresiduals/");
        hlpAlgorithm->writeSettings(*settings->getQSettings(), "fcsfitevaleditor/algorithm_");
        hlpFunction->writeSettings(*settings->getQSettings(), "fcsfitevaleditor/function_");
        saveSplitter(*(settings->getQSettings()), splitPlot, "fcsfitevaleditor/splitter_plot");
        saveSplitter(*(settings->getQSettings()), splitModel, "fcsfitevaleditor/splitter_model");
        saveSplitter(*(settings->getQSettings()), splitFitStatistics, "fcsfitevaleditor/splitter_fitstatistics");
        settings->getQSettings()->setValue("fcsfitevaleditor/parameterWidgetWidth", m_parameterWidgetWidth);
        settings->getQSettings()->setValue("fcsfitevaleditor/parameterCheckboxWidth", m_parameterCheckboxWidth);
        settings->getQSettings()->setValue("fcsfitevaleditor/display_range_widgets", btnEditRanges->isChecked());
        settings->getQSettings()->setValue("fcsfitevaleditor/residual_histogram_bins", spinResidualHistogramBins->value());
        settings->getQSettings()->setValue("fcsfitevaleditor/residual_toolbox_current", tabResidulas->currentIndex());
        settings->getQSettings()->setValue("fcsfitevaleditor/lastFPSDirectory", currentFPSSaveDir);
    }
}

void QFFCSFitEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));
    bool modelChanged=false;
    //std::cout<<"QFFCSFitEvaluationEditor::highlightingChanged("<<formerRecord<<", "<<currentRecord<<")  eval="<<eval<<"   data="<<data<<"\n";
    if (data) {
        //labRecord->setText(tr("<b>current:</b> <i>%1</i>").arg(currentRecord->getName()) );
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));

        datacut->disableSliderSignals();
        datacut->set_min(0);
        datacut->set_max(data->getCorrelationN());
        QString run="avg";
        if (eval->getCurrentRun()>-1) run=QString::number(eval->getCurrentRun());
        datacut->set_userMin(currentRecord->getProperty(resultID+"_r"+run+"_datacut_min", 0).toInt());
        datacut->set_userMax(currentRecord->getProperty(resultID+"_r"+run+"_datacut_max", data->getCorrelationN()).toInt());
        datacut->enableSliderSignals();
        dataEventsEnabled=false;
        spinRun->setMaximum(data->getCorrelationRuns()-1);
        if (data->getCorrelationRuns()==1) spinRun->setMaximum(-1);
        spinRun->setValue(eval->getCurrentRun());//currentRecord->getProperty(resultID+"_selected_run", -1).toInt());
        if (data->getCorrelationRuns()>1) spinRun->setSuffix(QString(" / 0..%1").arg(data->getCorrelationRuns()-1));
        int newidx=cmbModel->findData(eval->getFitFunction()->id());
        if (newidx!=cmbModel->currentIndex()) modelChanged=true;
        cmbModel->setCurrentIndex(newidx);
        cmbWeights->setCurrentIndex(eval->getFitDataWeighting());
        dataEventsEnabled=true;

    }
    displayModel(modelChanged);
    replotData();
}

void QFFCSFitEvaluationEditor::displayModel(bool newWidget) {
    if (!current) return;
    if (!cmbModel) return;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();

    if (!ffunc) {
        /////////////////////////////////////////////////////////////////////////////////////////////
        // delete all fit parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<m_fitParameters.size(); i++) {
            if (m_fitParameters[i]) {
                m_fitParameters[i]->disableDatastore();
                disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
                disconnect(m_fitParameters[i], SIGNAL(valueChanged(QString, double)), this, SLOT(parameterValueChanged()));
                disconnect(m_fitParameters[i], SIGNAL(errorChanged(QString, double)), this, SLOT(parameterValueChanged()));
                disconnect(m_fitParameters[i], SIGNAL(fixChanged(QString, bool)), this, SLOT(parameterFixChanged()));
                disconnect(m_fitParameters[i], SIGNAL(rangeChanged(QString, double, double)), this, SLOT(parameterRangeChanged()));
                delete m_fitParameters[i];
            }
        }
        m_fitParameters.clear();
        return;
    }
    QFFitAlgorithm* falg=eval->getFitAlgorithm();

    if (newWidget) {
        /////////////////////////////////////////////////////////////////////////////////////////////
        // first delete all fit parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<m_fitParameters.size(); i++) {
            if (m_fitParameters[i]) {
                m_fitParameters[i]->disableDatastore();
                disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
                disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(unsetEditValues(bool)));
                disconnect(m_fitParameters[i], SIGNAL(valueChanged(QString, double)), this, SLOT(parameterValueChanged()));
                disconnect(m_fitParameters[i], SIGNAL(errorChanged(QString, double)), this, SLOT(parameterValueChanged()));
                disconnect(m_fitParameters[i], SIGNAL(fixChanged(QString, bool)), this, SLOT(parameterFixChanged()));
                disconnect(m_fitParameters[i], SIGNAL(rangeChanged(QString, double, double)), this, SLOT(parameterRangeChanged()));
                disconnect(m_fitParameters[i], SIGNAL(enterPressed(QString)), this, SLOT(fitCurrent()));
                delete m_fitParameters[i];
            }
        }
        // remove all widgets from layout
        QLayoutItem *child;
        while ((child = layParameters->takeAt(0)) != 0) {
            delete child;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////
        // create header widget
        /////////////////////////////////////////////////////////////////////////////////////////////
        QFFitParameterWidgetWrapper* header=new QFFitParameterWidgetWrapper(eval, layParameters, 0, "", QFFitParameterWidgetWrapper::Header, true, true, QFFitFunction::DisplayError, true, this, tr("parameter"));
        m_fitParameters.append(header);
        connect(btnEditRanges, SIGNAL(toggled(bool)), header, SLOT(setEditRange(bool)));
        connect(btnEditRanges, SIGNAL(toggled(bool)), header, SLOT(unsetEditValues(bool)));
        header->setEditRange(btnEditRanges->isChecked());
        header->unsetEditValues(!btnEditRanges->isChecked());

        /////////////////////////////////////////////////////////////////////////////////////////////
        // create new parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        btnCalibrateFocalVolume->setEnabled(false);
        bool has_particles=false;
        bool has_wxy=false;
        bool has_tauD=false;
        bool has_gamma=false;
        for (int i=0; i<ffunc->paramCount(); i++) {
            QString id=ffunc->getParameterID(i);
            if ((id.toLower()=="n_particle") || (id.toLower()=="1n_particle")) has_particles=true;
            if ((id.toLower()=="diff_tau1") || (id.toLower()=="diff_tau")) has_tauD=true;
            if ((id.toLower()=="focus_width")) has_wxy=true;
            if ((id.toLower()=="focus_struct_fac")) has_gamma=true;
            QFFitFunction::ParameterDescription d=ffunc->getDescription(i);
            QFFitParameterWidgetWrapper::WidgetType wtype=QFFitParameterWidgetWrapper::FloatEdit;
            if (d.type==QFFitFunction::IntNumber) wtype=QFFitParameterWidgetWrapper::IntSpinBox;
            if (d.type==QFFitFunction::IntCombo) wtype=QFFitParameterWidgetWrapper::IntDropDown;
            bool editable=d.userEditable;
            bool displayFix=d.userEditable;
            QFFitFunction::ErrorDisplayMode displayError=d.displayError;
            bool editRange=d.userEditable && d.userRangeEditable;
            if (!d.fit) {
                displayFix=false;
            }
            QString l=QString("<font size=\"+1\">%1</font>:").arg(d.label);
            if (!d.unit.isEmpty()) l=QString("<font size=\"+1\">%1 [%2]</font>:").arg(d.label).arg(d.unitLabel);
            l.replace("<sub>", "<sub><font size=\"+2\">", Qt::CaseInsensitive);
            l.replace("<sup>", "<sup><font size=\"+2\">", Qt::CaseInsensitive);
            l.replace("</sup>", "</font></sup>", Qt::CaseInsensitive);
            l.replace("</sub>", "</font></sub>", Qt::CaseInsensitive);
            QFFitParameterWidgetWrapper* fpw=new QFFitParameterWidgetWrapper(eval, layParameters, i+1, id, wtype, editable, displayFix, displayError, editRange, this, l, d.comboItems);
            fpw->setUnit(d.unit);
            fpw->setIncrement(d.inc);
            fpw->setWidgetWidth(m_parameterWidgetWidth);
            fpw->setRangeEnabled(true);
            fpw->setValueAbsoluteRange(d.absMinValue, d.absMaxValue);
            if (falg) fpw->setRangeEnabled(falg->get_supportsBoxConstraints());
            fpw->setToolTip(d.name);
            m_fitParameters.append(fpw);
            connect(btnEditRanges, SIGNAL(toggled(bool)), fpw, SLOT(unsetEditValues(bool)));
            connect(btnEditRanges, SIGNAL(toggled(bool)), fpw, SLOT(setEditRange(bool)));
            connect(fpw, SIGNAL(valueChanged(QString, double)), this, SLOT(parameterValueChanged()));
            connect(fpw, SIGNAL(errorChanged(QString, double)), this, SLOT(parameterValueChanged()));
            connect(fpw, SIGNAL(fixChanged(QString, bool)), this, SLOT(parameterFixChanged()));
            connect(fpw, SIGNAL(rangeChanged(QString, double, double)), this, SLOT(parameterRangeChanged()));
            connect(fpw, SIGNAL(enterPressed(QString)), this, SLOT(fitCurrent()));
            fpw->setEditRange(btnEditRanges->isChecked());
            fpw->unsetEditValues(btnEditRanges->isChecked());
        }
        btnCalibrateFocalVolume->setEnabled((has_tauD||(has_particles&&has_gamma))&&has_wxy);
        // add stretcher item in bottom row
        layParameters->addItem(new QSpacerItem(5,5, QSizePolicy::Minimum, QSizePolicy::Expanding), layParameters->rowCount(), 0);
    }

    if (eval->getCurrentRun()!=spinRun->value()) eval->setCurrentRun(eval->getCurrentRun());

    if (eval->hasFit()) {
        labFitParameters->setText(tr("<b><u>Local</u> Fit Parameters:</b>"));
        //labFitResult->setText(current->getHighlightedRecord()->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML"));
    } else {
        labFitParameters->setText(tr("<b><u>Global</u> Fit Parameters:</b>"));
        labFitResult->setText("");
    }

    updateParameterValues();
}

void QFFCSFitEvaluationEditor::parameterValueChanged() {
    updateParameterValues();
    replotData();
}

void QFFCSFitEvaluationEditor::parameterFixChanged() {
    updateParameterValues();
}

void QFFCSFitEvaluationEditor::parameterRangeChanged() {
    updateParameterValues();
    replotData();
}

void QFFCSFitEvaluationEditor::updateParameterValues() {
    if (!current) return;
    if (!cmbModel) return;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();

    if (!ffunc) return;

    double* fullParams=eval->allocFillParameters();
    double* errors=eval->allocFillParameterErrors();
    ffunc->calcParameter(fullParams, errors);


    for (int i=0; i<m_fitParameters.size(); i++) {
        if (m_fitParameters[i]) {
            //m_fitParameters[i]->reloadValues();
            QString id=m_fitParameters[i]->parameterID();
            int num=ffunc->getParameterNum(id);
            if (!id.isEmpty()) {
                if (num>=0) m_fitParameters[i]->setValue(fullParams[num], errors[num], false);
                bool visible=ffunc->isParameterVisible(ffunc->getParameterNum(id), fullParams);
                m_fitParameters[i]->setVisible(visible);
                //layParameters->labelForField(m_fitParameters[i])->setVisible(visible);
            }
        }
    }

    free(fullParams);
    free(errors);

    if (eval->hasFit()) labFitParameters->setText(tr("<b><u>Local</u> Fit Parameters:</b>"));
    else labFitParameters->setText(tr("<b><u>Global</u> Fit Parameters:</b>"));
}

void QFFCSFitEvaluationEditor::replotData() {


    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();

    if ((!eval)||(!data)) {
        pltData->clearGraphs();
        ds->clear();
        return;
    }

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
    dsres->clear();
    ds->clear();
    dsresh->clear();
    dsresc->clear();

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

    int errorStyle=cmbErrorStyle->currentIndex();
    int plotStyle=cmbPlotStyle->currentIndex();
    int residualStyle=cmbResidualStyle->currentIndex();

    if (data->getCorrelationN()>0) {
        size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");


        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        size_t c_mean=0;
        QString graphName="";
        size_t c_std=0;
        QString errorName="";
        if (eval->getCurrentRun()<0) {
            c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
            graphName=tr("\\verb{%1} average").arg(record->getName());
            c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
            errorName=tr("stddev");
        } else {
            if (eval->getCurrentRun()<(int)data->getCorrelationRuns()) {
                c_mean=ds->addColumn(data->getCorrelationRun(eval->getCurrentRun()), data->getCorrelationN(), QString("run"+QString::number(eval->getCurrentRun())).toStdString());
                graphName=tr("\\verb{%2} %3").arg(eval->getCurrentRun()).arg(record->getName()).arg(data->getCorrelationRunName(eval->getCurrentRun()));
                if (eval->getFitDataWeighting()==QFFCSFitEvaluation::RunErrorWeighting) {
                    c_std=ds->addColumn(data->getCorrelationRunError(eval->getCurrentRun()), data->getCorrelationN(), "cperrunerror");
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

        updateFitFunctions();

        pltData->zoomToFit(true, true);
        pltResiduals->zoomToFit(false, true);

        pltResiduals->setX(pltData->getXMin(), pltData->getXMax());

        pltResidualHistogram->zoomToFit(true, true);
        pltResidualCorrelation->zoomToFit(true, true);
    }


    pltResiduals->set_doDrawing(true);
    pltResiduals->set_emitSignals(true);
    pltData->set_doDrawing(true);
    pltData->set_emitSignals(true);
    pltResidualHistogram->set_doDrawing(true);
    pltResidualHistogram->set_emitSignals(true);
    pltResidualCorrelation->set_doDrawing(true);
    pltResidualCorrelation->set_emitSignals(true);
    pltResiduals->update_plot();
    pltData->update_plot();
    pltResidualHistogram->update_plot();
    pltResidualCorrelation->update_plot();
}

void QFFCSFitEvaluationEditor::updateFitFunctions() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFCSFitEvaluation::DataWeight weighting=eval->getFitDataWeighting();

    if (!ffunc) return;

    if ((!eval)||(!data)) {
        return;
    }

    int residualStyle=cmbResidualStyle->currentIndex();
    int residualHistogramBins=spinResidualHistogramBins->value();
    int datacut_min=datacut->get_userMin();
    int datacut_max=datacut->get_userMax();

    try {

        if (data->getCorrelationN()>0) {
            int c_tau=ds->getColumnNum("tau");
            if (c_tau>=0) { // we only add a graph, if we have a column with tau values
                /////////////////////////////////////////////////////////////////////////////////
                // retrieve data and tau-values from rawdata record
                /////////////////////////////////////////////////////////////////////////////////
                long N=data->getCorrelationN();
                int runAvgWidth=10;
                int runAvgStart=-runAvgWidth/2;
                double* fitfunc=(double*)calloc(N,sizeof(double));
                double* residuals=(double*)calloc(N,sizeof(double));
                double* residuals_weighted=(double*)calloc(N,sizeof(double));
                double* tauvals=data->getCorrelationT();
                double* corrdata=NULL;
                int runAvgMaxN=N;
                int runAvgN=0;
                double* tau_runavg=(double*)calloc(runAvgMaxN, sizeof(double));
                double* residuals_runavg=(double*)calloc(runAvgMaxN, sizeof(double));
                double* residuals_runavg_weighted=(double*)calloc(runAvgMaxN, sizeof(double));

                if (eval->getCurrentRun()<0) {
                    corrdata=data->getCorrelationMean();
                } else {
                    if (eval->getCurrentRun()<(int)data->getCorrelationRuns()) {
                        corrdata=data->getCorrelationRun(eval->getCurrentRun());
                    } else {
                        corrdata=data->getCorrelationMean();
                    }
                }

                double* weights=allocWeights(NULL, record, eval->getCurrentRun(), datacut_min, datacut_max);

                /////////////////////////////////////////////////////////////////////////////////
                // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
                // we are working with a complete set of parameters
                /////////////////////////////////////////////////////////////////////////////////
                double* fullParams=eval->allocFillParameters();
                double* errors=eval->allocFillParameterErrors();
                bool* paramsFix=eval->allocFillFix(record, eval->getCurrentRun());
                ffunc->calcParameter(fullParams, errors);
                int fitparamN=0;
                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (ffunc->isParameterVisible(i, fullParams) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                        fitparamN++;
                    }
                }
                int dataSize=datacut_max-datacut_min;
                int degFreedom=dataSize-fitparamN-1;

                /////////////////////////////////////////////////////////////////////////////////
                // calculate model function values, residuals and residual parameters/statistics
                /////////////////////////////////////////////////////////////////////////////////
                double residSqrSum=0;        // sum of squared residuals
                double residWeightSqrSum=0;  // sum of squared weighted residuals
                double residSum=0;           // sum of residuals
                double residWeightSum=0;     // sum of weightedresiduals

                double rmin=0;       // min of residuals
                double rmax=0;       // max of residuals
                double rminw=0;      // min of weighted residuals
                double rmaxw=0;      // max of weighted residuals
                bool hfirst=true;

                for (int i=0; i<N; i++) {
                    double value=ffunc->evaluate(tauvals[i], fullParams);
                    //di_fit->set(0, i, value);
                    //di_resid->set(0,0,value)
                    fitfunc[i]=value;
                    residuals[i]=corrdata[i]-value;
                    double res=residuals[i];
                    residuals_weighted[i]=res/weights[i];
                    if (fabs(weights[i])<1000*DBL_MIN) residuals_weighted[i]=0;
                    //std::cout<<"weights["<<i<<"]="<<weights[i]<<"\n";
                    double resw=residuals_weighted[i];
                    if ((i>=datacut_min)&&(i<=datacut_max)) {
                        residSqrSum+=res*res;
                        residWeightSqrSum+=resw*resw;
                        residSum+=res;
                        residWeightSum+=resw;

                        if (hfirst) {
                            rmin=res;
                            rmax=res;
                            rmaxw=resw;
                            rminw=resw;
                            hfirst=false;
                        } else {
                            if (res>rmax) rmax=res;
                            if (res<rmin) rmin=res;
                            if (resw>rmaxw) rmaxw=resw;
                            if (resw<rminw) rminw=resw;
                        }
                        //std::cout<<"res="<<res<<" resw="<<resw<<"    :    rmin="<<rmin<<"  rmax="<<rmax<<"    rminw="<<rminw<<"  rmaxw="<<rmaxw<<std::endl;
                        if ((i+runAvgStart>=datacut_min) && (i+runAvgStart+runAvgWidth<=datacut_max) && ((i-datacut_min)%runAvgWidth==0)) {
                            double s=0, sw=0;
                            for (int j=0; j<runAvgWidth; j++) {
                                s=s+residuals[i+j+runAvgStart];
                                sw=sw+residuals_weighted[i+j+runAvgStart];
                            }
                            tau_runavg[runAvgN]=tauvals[i];
                            residuals_runavg[runAvgN]=s/(double)runAvgWidth;
                            residuals_runavg_weighted[runAvgN]=sw/(double)runAvgWidth;
                            runAvgN++;
                        }
                    }
                }

                double residAverage=residSum/(double)N;
                double residWeightAverage=residWeightSum/(double)N;
                double residStdDev=sqrt(residSqrSum/(double)N-residSum*residSum/(double)N/(double)N);
                double residWeightStdDev=sqrt(residWeightSqrSum/(double)N-residWeightSum*residWeightSum/(double)N/(double)N);

                double residHistBinWidth=(rmax-rmin)/(double)residualHistogramBins;
                double residHistWBinWidth=(rmaxw-rminw)/(double)residualHistogramBins;
                //std::cout<<"rmin="<<rmin<<"  rmax="<<rmax<<"    rminw="<<rminw<<"  rmaxw="<<rmaxw<<std::endl;

                /////////////////////////////////////////////////////////////////////////////////
                // calculate residual histogram
                /////////////////////////////////////////////////////////////////////////////////
                double* resHistogram=(double*)calloc(residualHistogramBins, sizeof(double));
                double resHistogramCount=0;
                double* resWHistogram=(double*)calloc(residualHistogramBins, sizeof(double));
                double resWHistogramCount=0;
                for (int i=0; i<N; i++) {
                    if (residHistBinWidth>0) {
                        int idx=round((residuals[i]-rmin)/residHistBinWidth);
                        if ((idx>=0) && (idx<residualHistogramBins)) {
                            resHistogramCount++;
                            resHistogram[idx]++;
                        }
                    }
                    if (residHistWBinWidth>0) {
                        int idx=round((residuals_weighted[i]-rminw)/residHistWBinWidth);
                        if ((idx>=0) && (idx<residualHistogramBins)) {
                            resWHistogramCount++;
                            resWHistogram[idx]++;
                        }
                    }
                }
                if (resHistogramCount>0) for (int i=0; i<residualHistogramBins; i++) {
                    resHistogram[i]=resHistogram[i]/resHistogramCount;
                }
                if (resWHistogramCount>0) for (int i=0; i<residualHistogramBins; i++) {
                    resWHistogram[i]=resWHistogram[i]/resWHistogramCount;
                }


                /////////////////////////////////////////////////////////////////////////////////
                // calculate residual correlation
                /////////////////////////////////////////////////////////////////////////////////
                double* resCorrelation=statisticsAllocAutocorrelate(&(residuals[datacut_min]), datacut_max-datacut_min);
                double* resWCorrelation=statisticsAllocAutocorrelate(&(residuals_weighted[datacut_min]), datacut_max-datacut_min);
                int resN=ceil((double)(datacut_max-datacut_min)/2.0);


                size_t c_fit = ds->addCopiedColumn(fitfunc, N, "fit_model");

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
                for (int i=0; i<(int)ffunc->getAdditionalPlotCount(fullParams); i++) {
                    double* params=eval->allocFillParameters();
                    QString name=ffunc->transformParametersForAdditionalPlot(i, params);
                    double* afitfunc=(double*)malloc(N*sizeof(double));
                    for (int j=0; j<N; j++) {
                        afitfunc[j]=ffunc->evaluate(tauvals[j], params);
                    }
                    size_t c_afit=ds->addCopiedColumn(afitfunc, N, QString("add_fit_model_%1").arg(i).toStdString());
                    JKQTPxyLineGraph* g_afit=new JKQTPxyLineGraph(pltData->get_plotter());
                    g_afit->set_drawLine(true);
                    g_afit->set_title(name);
                    g_afit->set_xColumn(c_tau);
                    g_afit->set_yColumn(c_afit);
                    g_afit->set_datarange_start(datacut->get_userMin());
                    g_afit->set_datarange_end(datacut->get_userMax());
                    pltData->addGraph(g_afit);
                    free(params);
                    free(afitfunc);
                }
                pltData->addGraph(g_fit);


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_taures=c_tau;//dsres->addCopiedColumn(data->getCorrelationT(), N, "tau");
                size_t c_residuals=0;
                JKQTPxyLineGraph* g_residuals=new JKQTPxyLineGraph(pltResiduals->get_plotter());
                if (chkWeightedResiduals->isChecked()) {
                    c_residuals=dsres->addCopiedColumn(residuals_weighted, N, "residuals_weighted");
                    g_residuals->set_title("weighted residuals");
                } else {
                    c_residuals=dsres->addCopiedColumn(residuals, N, "residuals");
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


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals running average
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_tauresra=dsres->addCopiedColumn(tau_runavg, runAvgN, "tau_resid_runavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(residuals_runavg_weighted, runAvgN, "residuals_runavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(residuals_runavg, runAvgN, "residuals_runavg");
                    g_residualsra->set_title("residuals, movAvg");
                }
                g_residualsra->set_xColumn(c_tauresra);
                g_residualsra->set_yColumn(c_residualsra);
                g_residualsra->set_symbolSize(6);
                g_residualsra->set_symbolWidth(1);
                g_residualsra->set_color(g_residuals->get_color().darker());
                //g_residuals->set_datarange_start(datacut->get_userMin());
                //g_residuals->set_datarange_end(datacut->get_userMax());
                g_residualsra->set_drawLine(true);
                if (residualStyle==0) { // draw points
                    // always draw as lines
                    g_residualsra->set_symbol(JKQTPplus);
                } else if (residualStyle==2) {
                    g_residualsra->set_symbol(JKQTPplus);
                }
                pltResiduals->addGraph(g_residualsra);

                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals histogram
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualHistogramX=0;
                size_t c_residualHistogramY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, rminw+residHistWBinWidth/2.0, rmaxw-residHistWBinWidth/2.0, "residualhist_weighted_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(resWHistogram, residualHistogramBins, "residualhist_weighted_y");
                } else {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, rmin+residHistBinWidth/2.0, rmax-residHistBinWidth/2.0, "residualhist_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(resHistogram, residualHistogramBins, "residualhist_y");
                }
                JKQTPbarHorizontalGraph* g_residualsHistogram=new JKQTPbarHorizontalGraph(pltResidualHistogram->get_plotter());
                g_residualsHistogram->set_xColumn(c_residualHistogramX);
                g_residualsHistogram->set_yColumn(c_residualHistogramY);
                g_residualsHistogram->set_shift(0);
                g_residualsHistogram->set_width(1.0);
                pltResidualHistogram->addGraph(g_residualsHistogram);


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals correlations
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualCorrelationX=dsresc->addLinearColumn(resN-1, 1, resN-1, "residualcorr_x");
                size_t c_residualCorrelationY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualCorrelationY=dsresc->addCopiedColumn(&(resWCorrelation[1]), resN-1, "residualcorr_weighted_y");
                } else {
                    c_residualCorrelationY=dsresh->addCopiedColumn(&(resCorrelation[1]), resN-1, "residualcorr_y");
                }
                JKQTPxyLineGraph* g_residualsCorrelation=new JKQTPxyLineGraph(pltResidualCorrelation->get_plotter());
                g_residualsCorrelation->set_xColumn(c_residualCorrelationX);
                g_residualsCorrelation->set_yColumn(c_residualCorrelationY);
                pltResidualCorrelation->addGraph(g_residualsCorrelation);

                /////////////////////////////////////////////////////////////////////////////////
                // update display of fit results
                /////////////////////////////////////////////////////////////////////////////////
                QString txtFit="<font face=\"Arial\">";
                QString fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML");

                if (!fitResult.isEmpty()) {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>%1</center></div><br>").arg(fitResult);
                } else {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>not fit yet</center></div><br>");
                }
                txtFit+=QString("<b>%1</b><cebter>").arg(tr("Fit Statistics:"));
                txtFit+=QString("<table border=\"0\" width=\"95%\">");
                //txtFit+=QString("<tr><td align=\"right\"></td><td align=\"left\"></td><td align=\"right\"></td><td align=\"left\"></td></tr>");
                txtFit+=QString("<tr><td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> =</td><td align=\"left\" valign=\"bottom\">%1</td></td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<td><td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td></tr>").arg(residSqrSum).arg(residWeightSqrSum);
                txtFit+=QString("<tr><td align=\"right\" valign=\"bottom\">&lang;E&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td></td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<td><td align=\"right\" valign=\"bottom\"> &lang;E&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td></tr>").arg(residAverage).arg(residWeightAverage);
                txtFit+=QString("<tr><td align=\"right\" valign=\"bottom\">&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td></td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<td><td align=\"right\" valign=\"bottom\"> &radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td></tr>").arg(residStdDev).arg(residWeightStdDev);
                txtFit+=QString("<tr><td align=\"right\" valign=\"bottom\">NP=</td><td align=\"left\" valign=\"bottom\">%1</td></td><td><td align=\"right\" valign=\"bottom\">NR=</td><td align=\"left\" valign=\"bottom\">%2</td></tr>").arg(fitparamN).arg(dataSize);
                txtFit+=QString("<tr><td align=\"right\" valign=\"bottom\">DF=</td><td align=\"left\" valign=\"bottom\">%1</td><td align=\"right\" valign=\"bottom\"></td><td></td><td align=\"left\" valign=\"bottom\"></td></tr>").arg(degFreedom);

                txtFit+=QString("</table><br><font size=\"-1\"><i>Legend:</i>: &chi;<sup>2</sup>: sum error square, &lang;E&rang;: residual average, &radic;&lang;E2&rang;: residual stddev., <br>NP: number of fit parameters, NR: number of residuals, <b>DF: degrees of freedom </font>");
                txtFit+=QString("</center></font>");
                fitStatisticsReport=txtFit;
                txtFitStatistics->setHtml(txtFit);


                /////////////////////////////////////////////////////////////////////////////////
                // clean memory
                /////////////////////////////////////////////////////////////////////////////////
                free(fullParams);
                free(errors);
                free(fitfunc);
                free(residuals);
                free(tau_runavg);
                free(residuals_runavg);
                free(residuals_weighted);
                free(weights);
                free(resHistogram);
                free(resWHistogram);
                free(paramsFix);
                free(resWCorrelation);
                free(resCorrelation);
                free(residuals_runavg_weighted);
            }
        }
    } catch(std::exception& E) {
        services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
    }

}



void QFFCSFitEvaluationEditor::doFit(QFRawDataRecord* record, int run) {
    QApplication::processEvents();
    QApplication::processEvents();

    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;

    eval->restoreQFFitAlgorithmParameters(falg);

    QFFCSFitEvaluation::DataWeight weighting=eval->getFitDataWeighting();

    if (data->getCorrelationN()>0) {
        falg->setReporter(dlgFitProgressReporter);
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);
        dlgFitProgress->reportStatus(tr("setting up ..."));
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setProgress(0);

        services->log_text(tr("running fit with '%1' (%2) and model '%3' (%4) on raw data record '%5', run %6 ... \n").arg(falg->name()).arg(falg->id()).arg(ffunc->name()).arg(ffunc->id()).arg(record->getName()).arg(runname));

        long N=data->getCorrelationN();
        double* weights=NULL;
        double* taudata=data->getCorrelationT();
        double* corrdata=NULL;
        if (run<0) {
            corrdata=data->getCorrelationMean();
        } else {
            if (run<(int)data->getCorrelationRuns()) {
                corrdata=data->getCorrelationRun(run);
            } else {
                corrdata=data->getCorrelationMean();
            }
        }
        // we also have to care for the data cutting
        int cut_low=datacut->get_userMin();
        int cut_up=datacut->get_userMax();
        int cut_N=N-cut_low-(N-cut_up);
        if (cut_N<0) {
            cut_low=0;
            cut_up=ffunc->paramCount()-1;
            if (cut_up>=N) cut_up=N;
            cut_N=cut_up;
        }
        bool weightsOK=false;
        weights=allocWeights(&weightsOK, record, run, cut_low, cut_up);
        if (!weightsOK) services->log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));

        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=eval->allocFillParameters(record, run);
        double* initialparams=eval->allocFillParameters(record, run);
        double* errors=eval->allocFillParameterErrors(record, run);
        double* paramsMin=eval->allocFillParametersMin();
        double* paramsMax=eval->allocFillParametersMax();
        bool* paramsFix=eval->allocFillFix(record, run);



        try {

            ffunc->calcParameter(params, errors);
            ffunc->calcParameter(initialparams, errors);

            QString iparams="";
            QString oparams="";
            QString orparams="";

            for (int i=0; i<ffunc->paramCount(); i++) {
                if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                    if (!iparams.isEmpty()) iparams=iparams+";  ";
                    iparams=iparams+QString("%1 = %2").arg(ffunc->getDescription(i).id).arg(params[i]);
                }
                //printf("  fit: %s = %lf +/m %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
            }





            if (!dlgFitProgress->isCanceled()) {

                dlgFitProgress->reportStatus(tr("fitting ..."));
                dlgFitProgress->setProgressMax(100);
                dlgFitProgress->setProgress(0);
                doFitThread->init(falg, params, errors, &taudata[cut_low], &corrdata[cut_low], &weights[cut_low], cut_N, ffunc, initialparams, paramsFix, paramsMin, paramsMax);
                doFitThread->start(QThread::HighestPriority);
                while (!doFitThread->isFinished()) {
                    QApplication::processEvents();
                }
                dlgFitProgress->setProgressFull();
                dlgFitProgress->reportStatus(tr("calculating parameters, errors and storing data ..."));
            }
            //dlgFitProgress->setAllowCancel(false);


            if (!dlgFitProgress->isCanceled()) {
                QFFitAlgorithm::FitResult result=doFitThread->getResult();
                ffunc->calcParameter(params, errors);
                ffunc->sortParameter(params, errors);
                ffunc->calcParameter(params, errors);

                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                        if (!oparams.isEmpty()) oparams=oparams+";  ";
                        oparams=oparams+QString("%1 = %2+/-%3").arg(ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                    }
                    //printf("  fit: %s = %lf +/- %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                }

                // round errors and values
                for (int i=0; i<ffunc->paramCount(); i++) {
                    errors[i]=roundError(errors[i], 2);
                    params[i]=roundWithError(params[i], errors[i], 2);
                }
                eval->setFitResultValuesVisible(record, run, params, errors);

                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                        if (!orparams.isEmpty()) orparams=orparams+";  ";
                        orparams=orparams+QString("%1 = %2+/-%3").arg(ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                    }
                    //printf("  fit: %s = %lf +/- %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                }

                services->log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(doFitThread->getDeltaTime()).arg(result.fitOK?tr("success"):tr("no convergence")));
                services->log_text(tr("   - result-message: %1\n").arg(result.messageSimple));
                services->log_text(tr("   - initial params         (%1)\n").arg(iparams));
                services->log_text(tr("   - output params          (%1)\n").arg(oparams));
                services->log_text(tr("   - output params, rounded (%1)\n").arg(orparams));
                //labFitResult->setText(result.message);
/*
                eval->setFitResultValueInt(record, run, "fit_used_run", run);
                eval->setFitResultValueString(record, run, "fit_model_name", ffunc->id());
                eval->setFitResultValueString(record, run, "fitalg_name", falg->id());
                eval->setFitResultValue(record, run, "fitalg_runtime", doFitThread->getDeltaTime(), "msecs");
                eval->setFitResultValueBool(record, run, "fitalg_success", result.fitOK);
                eval->setFitResultValueString(record, run, "fitalg_message", result.messageSimple);
                eval->setFitResultValueString(record, run, "fitalg_messageHTML", result.message);
                eval->setFitResultValueInt(record, run, "fit_datapoints", cut_N);
                eval->setFitResultValueInt(record, run, "fit_cut_low", cut_low);
                eval->setFitResultValueInt(record, run, "fit_cut_up", cut_up);

*/
                record->disableEmitResultsChanged();
                record->resultsSetInteger(eval->getEvaluationResultID(ffunc->id(), run), "fit_used_run", run);
                record->resultsSetString(eval->getEvaluationResultID(ffunc->id(), run), "fit_model_name", ffunc->id());
                record->resultsSetString(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_name", falg->id());
                record->resultsSetNumber(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_runtime", doFitThread->getDeltaTime(), "msecs");
                record->resultsSetBoolean(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_success", result.fitOK);
                record->resultsSetString(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_message", result.messageSimple);
                record->resultsSetString(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_messageHTML", result.message);
                record->resultsSetInteger(eval->getEvaluationResultID(ffunc->id(), run), "fit_datapoints", cut_N);
                record->resultsSetInteger(eval->getEvaluationResultID(ffunc->id(), run), "fit_cut_low", cut_low);
                record->resultsSetInteger(eval->getEvaluationResultID(ffunc->id(), run), "fit_cut_up", cut_up);

                QMapIterator<QString, QVariant> it(result.params);
                while (it.hasNext()) {
                    it.next();
                    switch(it.value().type()) {
                        case QVariant::Double:
                            record->resultsSetNumber(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_"+it.key(), it.value().toDouble()); break;
                        case QVariant::Char:
                        case QVariant::Date:
                        case QVariant::DateTime:
                        case QVariant::String:
                            record->resultsSetString(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_"+it.key(), it.value().toString()); break;
                        case QVariant::Int:
                        case QVariant::LongLong:
                        case QVariant::UInt:
                        case QVariant::ULongLong:
                            record->resultsSetInteger(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_"+it.key(), it.value().toInt()); break;
                        case QVariant::Bool:
                            record->resultsSetBoolean(eval->getEvaluationResultID(ffunc->id(), run), "fitalg_"+it.key(), it.value().toBool()); break;
                        default: break;
                    }
                }
                record->enableEmitResultsChanged();
                emit resultsChanged();
            } else {
                services->log_warning(tr("   - fit canceled by user!!!\n"));
            }
        } catch(std::exception& E) {
            services->log_error(tr("error during fitting, error message: %1\n").arg(E.what()));
        }

        // clean temporary parameters
        //delete doFitThread;
        free(weights);
        free(params);
        free(initialparams);
        free(errors);
        free(paramsFix);
        free(paramsMax);
        free(paramsMin);

        //displayModel(false);
        //replotData();
        //QApplication::restoreOverrideCursor();
        //dlgFitProgress->done();
        falg->setReporter(NULL);
    }
}




void QFFCSFitEvaluationEditor::fitCurrent() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;

    if (data->getCorrelationN()>0) {
        falg->setReporter(dlgFitProgressReporter);
        QString runname=tr("average");
        if (eval->getCurrentRun()>=0) runname=QString::number(eval->getCurrentRun());
        dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));
        dlgFitProgress->reportStatus("");
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setSuperProgressMax(100);
        dlgFitProgress->setProgress(0);
        dlgFitProgress->setSuperProgress(0);
        dlgFitProgress->setAllowCancel(true);
        dlgFitProgress->display();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        doFit(record, eval->getCurrentRun());

        displayModel(false);
        replotData();
        QApplication::restoreOverrideCursor();
        dlgFitProgress->done();
        falg->setReporter(NULL);
    }
}

void QFFCSFitEvaluationEditor::fitRunsCurrent() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;


    dlgFitProgress->reportSuperStatus(tr("fit all runs<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(data->getCorrelationRuns());
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgFitProgress->setAllowCancel(true);
    dlgFitProgress->display();
    int runmax=data->getCorrelationRuns();
    if (runmax==1) runmax=0;
    for (int run=-1; run<runmax; run++) {
        if (data->getCorrelationN()>0) {
            falg->setReporter(dlgFitProgressReporter);
            QString runname=tr("average");
            if (run>=0) runname=QString::number(run);
            dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));

            doFit(record, run);

            dlgFitProgress->incSuperProgress();
            falg->setReporter(NULL);
            if (dlgFitProgress->isCanceled()) break;
        }
    }

    displayModel(false);
    replotData();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
}


void QFFCSFitEvaluationEditor::fitAll() {
    if (!current) return;
    if (!cmbModel) return;
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!falg)) return;

    int run=eval->getCurrentRun();
    QString runname=tr("average");
    if (run>=0) runname=QString::number(run);

    dlgFitProgress->reportSuperStatus(tr("fit all files and current run (%3) therein<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()).arg(runname));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(10);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgFitProgress->setAllowCancel(true);
    dlgFitProgress->display();


    QList<QFRawDataRecord*> recs=eval->getApplicableRecords();

    // count the records and runs to work on (for proper superProgress
    int items=recs.size();
    dlgFitProgress->setSuperProgressMax(items);

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        //std::cout<<"i="<<i<<"   run="<<run<<"   record="<<record<<"   data="<<data<<"\n";

        if (record && data) {
            //std::cout<<"   corrN="<<data->getCorrelationN()<<"   corrRuns="<<data->getCorrelationRuns()<<"\n";
            if ((data->getCorrelationN()>0)&&(run<(int)data->getCorrelationRuns())) {
                falg->setReporter(dlgFitProgressReporter);
                dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));

                doFit(record, run);

                falg->setReporter(NULL);
                if (dlgFitProgress->isCanceled()) break;
            }
            dlgFitProgress->incSuperProgress();
        }
    }

    displayModel(false);
    replotData();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
}

void QFFCSFitEvaluationEditor::fitRunsAll() {
    if (!current) return;
    if (!cmbModel) return;
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!falg)) return;

    dlgFitProgress->reportSuperStatus(tr("fit all files and all runs therein<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(10);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    dlgFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgFitProgress->display();


    QList<QFRawDataRecord*> recs=eval->getApplicableRecords();

    // count the records and runs to work on (for proper superProgress
    int items=0;
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);

        if (record && data) {
            int runmax=data->getCorrelationRuns();
            if (runmax==1) runmax=0;
            items=items+runmax+1;
        }
    }
    dlgFitProgress->setSuperProgressMax(items);

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);

        if (record && data) {
            int runmax=data->getCorrelationRuns();
            if (runmax==1) runmax=0;
            for (int run=-1; run<runmax; run++) {
                if (data->getCorrelationN()>0) {
                    falg->setReporter(dlgFitProgressReporter);
                    QString runname=tr("average");
                    if (run>=0) runname=QString::number(run);
                    dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));

                    doFit(record, run);

                    dlgFitProgress->incSuperProgress();
                    falg->setReporter(NULL);
                    if (dlgFitProgress->isCanceled()) break;
                }
            }
        }
    }

    displayModel(false);
    replotData();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
}


void QFFCSFitEvaluationEditor::resetCurrent() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    eval->resetAllFitResultsCurrentCurrentRun();
    updateParameterValues();
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::resetAll() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    eval->resetAllFitResults();
    updateParameterValues();
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::copyToAll() {

    if (!current) return;
    if (!cmbModel) return;
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    copyToInitial();

    QFFitFunction* ffunc=eval->getFitFunction();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double* params=eval->allocFillParameters();

    QList<QFRawDataRecord*> recs=eval->getApplicableRecords();

    for (int i=0; i<ffunc->paramCount(); i++) {
        QString id=ffunc->getParameterID(i);
        double value=eval->getFitValue(id);
        double error=eval->getFitError(id);
        bool fix=eval->getFitFix(id);
        if (ffunc->isParameterVisible(i, params)) {
            for (int i=0; i<recs.size(); i++) {
                QFRawDataRecord* record=recs[i];
                QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(record);
                if (fcs) {
                    int runmax=fcs->getCorrelationRuns();
                    if (runmax<=1) runmax=0;
                    for (int run=-1; run<runmax; run++) {
                        if (eval->hasFit(record, run)) {
                            eval->setFitResultValue(record, run, id, value, error);
                            eval->setFitResultFix(record, run, id, fix);
                        }
                    }
                }
            }
        };
    }

    free(params);
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::copyToAllCurrentRun() {

    if (!current) return;
    if (!cmbModel) return;
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    copyToInitial();

    QFFitFunction* ffunc=eval->getFitFunction();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double* params=eval->allocFillParameters();

    QList<QFRawDataRecord*> recs=eval->getApplicableRecords();

    for (int i=0; i<ffunc->paramCount(); i++) {
        QString id=ffunc->getParameterID(i);
        double value=eval->getFitValue(id);
        double error=eval->getFitError(id);
        bool fix=eval->getFitFix(id);
        if (ffunc->isParameterVisible(i, params)) {
            for (int i=0; i<recs.size(); i++) {
                QFRawDataRecord* record=recs[i];
                QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(record);
                if (fcs) {
                    int runmax=fcs->getCorrelationRuns();
                    if (runmax<=1) runmax=0;

                    int run=eval->getCurrentRun();

                    if (eval->hasFit(record, run)) {
                        eval->setFitResultValue(record, run, id, value, error);
                        eval->setFitResultFix(record, run, id, fix);
                    }

                }
            }
        };
    }

    free(params);
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::copyToInitial() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    if (!fcs) return;

    QFFitFunction* ffunc=eval->getFitFunction();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    double* params=eval->allocFillParameters();

    for (int i=0; i<ffunc->paramCount(); i++) {
        QString id=ffunc->getParameterID(i);
        double value=eval->getFitValue(id);
        double error=eval->getFitError(id);
        bool fix=eval->getFitFix(id);
        if (ffunc->isParameterVisible(i, params)) {
            eval->setInitFitFix(id, fix);
            eval->setInitFitValue(id, value, error);
        };
    }

    free(params);
    QApplication::restoreOverrideCursor();
}


void QFFCSFitEvaluationEditor::createReportDoc(QTextDocument* document) {
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
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!eval) return;
    if (!fcs) return;

    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* algorithm=eval->getFitAlgorithm();
    int run=eval->getCurrentRun();
    double* params=eval->allocFillParameters();

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
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    cursor.insertText(tr("FCS Fit Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(3, 4, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("file:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(0, 2).firstCursorPosition().insertText(tr("run:"), fTextBold);
    table->cellAt(0, 3).firstCursorPosition().insertText(QString("%1 [%2]").arg((eval->getCurrentRun()<0)?tr("average"):QString::number(eval->getCurrentRun())).arg(fcs->getCorrelationRunName(eval->getCurrentRun())), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("fit algorithm:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(algorithm->name(), fText);
    table->cellAt(1, 2).firstCursorPosition().insertText(tr("data range:"), fTextBold);
    table->cellAt(1, 3).firstCursorPosition().insertText(tr("%1 ... %2").arg(datacut->get_userMin()).arg(datacut->get_userMax()), fText);
    table->cellAt(2, 0).firstCursorPosition().insertText(tr("fit function:"), fTextBold);
    table->cellAt(2, 1).firstCursorPosition().insertText(ffunc->name(), fText);
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
    table->mergeCells(0,0,2,1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltData->get_plotter()->draw(*painter, QRect(0,0,pltData->width(),pltData->width()));
        delete painter;
        double scale=document->textWidth()*0.62/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation data, model and residuals:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();

    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltResidualHistogram->get_plotter()->draw(*painter, QRect(0,0,pltResidualHistogram->width(),pltResidualHistogram->width()));
        delete painter;
        double scale=document->textWidth()*0.3/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual histogram:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    {
        QTextCursor tabCursor=table->cellAt(1, 1).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltResidualCorrelation->get_plotter()->draw(*painter, QRect(0,0,pltResidualCorrelation->width(),pltResidualCorrelation->width()));
        delete painter;
        double scale=document->textWidth()*0.3/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual correlations:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    int fitParamCount=0;
    for (int i=0; i<ffunc->paramCount(); i++) {
        if (ffunc->isParameterVisible(i, params)) fitParamCount++;
    }
    QApplication::processEvents();

    cursor.insertBlock(); cursor.insertBlock();
    if (eval->hasFit()) cursor.insertText(tr("Model Parameters (fit results):"), fTextBold);
    else cursor.insertText(tr("Model Parameters (initial values):"), fTextBold);
    QTextTableFormat tableFormat2;
    tableFormat2.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat2.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    table = cursor.insertTable(ceil((double)fitParamCount/2.0)+1, 10, tableFormat2);
    QTextCursor tableCursor;
    QApplication::processEvents();

    tableCursor=table->cellAt(0, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 3).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();

    tableCursor=table->cellAt(0, 5).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 6).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 7).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();


    int rowStart=1;
    int colStart=0;
    for (int i=0; i<ffunc->paramCount(); i++) {
        QApplication::processEvents();
        QString id=ffunc->getParameterID(i);
        double error=roundError(eval->getFitError(id),2);
        double value=roundWithError(eval->getFitValue(id), error, 2);
        bool fix=eval->getFitFix(id);
        QFFitFunction::ParameterDescription d=ffunc->getDescription(id);
        if (ffunc->isParameterVisible(i, params)) {
            QString err="";
            if (d.displayError!=QFFitFunction::NoError) {
                err=QString("&plusmn;&nbsp;%1").arg(QString(floattohtmlstr(error, 5, true).c_str()));
            }
            QString range=QString("%1...%2").arg(QString(floattohtmlstr(d.minValue, 5, true).c_str())).arg(QString(floattohtmlstr(d.maxValue, 5, true).c_str()));

            tableCursor=table->cellAt(rowStart, colStart).firstCursorPosition();
            tableCursor.setBlockFormat(bfRight);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(d.label));
            tableCursor.insertText(" = ", fTextSmall);

            tableCursor=table->cellAt(rowStart, colStart+1).firstCursorPosition();
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
            tableCursor.setBlockFormat(bfRight);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString(floattohtmlstr(value, 5, true).c_str())));

            tableCursor=table->cellAt(rowStart, colStart+3).firstCursorPosition();
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(err));

            if (algorithm->get_supportsBoxConstraints()) {
                tableCursor=table->cellAt(rowStart, colStart+4).firstCursorPosition();
                tableCursor.setBlockCharFormat(fTextSmall);
                tableCursor.insertFragment(QTextDocumentFragment::fromHtml(range));
            }
            rowStart++;
        };
        if (rowStart>=table->rows()) {
            rowStart=1;
            colStart+=5;
        }
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.setBlockFormat(bfCenter);
    cursor.setBlockCharFormat(fTextSmall);
    cursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<i><u>legend:</u> <b>F</b>: fit parameter, <b>X</b>: fixed parameter, <b>C</b>: calculated parameter</i>")));
    QApplication::processEvents();

    cursor.setBlockFormat(bfLeft);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(); cursor.insertBlock();
    QString htmlBot=fitStatisticsReport;
    htmlBot.replace("width=\"95%\"", "");
    cursor.insertFragment(QTextDocumentFragment::fromHtml(htmlBot));

}

void QFFCSFitEvaluationEditor::saveReport() {
    QString fn="";
    if (fn.isEmpty()) {
        fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                    currentSaveDirectory,
                                    tr("PDF File (*.pdf);;PostScript File (*.ps)"));
        if (!fn.isEmpty()) currentSaveDirectory=extract_file_path(fn.toStdString()).c_str();
    }

    if (!fn.isEmpty()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QModernProgressDialog progress(tr("Exporting ..."), "", NULL);
        progress.setWindowModality(Qt::WindowModal);
        progress.setHasCancel(false);
        progress.setLabelText(tr("saving FCS Fit report <br> to '%1' ...").arg(fn));
        progress.open();

        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter();//QPrinter::HighResolution);
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(15,15,15,15,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setOutputFormat(QPrinter::PdfFormat);
        if (fi.suffix().toLower()=="ps") printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setOutputFileName(fn);
        QTextDocument* doc=new QTextDocument();
        doc->setTextWidth(printer->pageRect().size().width());
        createReportDoc(doc);
        doc->print(printer);
        //qDebug()<<doc->toHtml();
        delete doc;
        delete printer;
        progress.accept();
        QApplication::restoreOverrideCursor();
    }
}

void QFFCSFitEvaluationEditor::printReport() {
    QPrinter* p=new QPrinter();//QPrinter::HighResolution);

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QModernProgressDialog progress(tr("Printing ..."), "", NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.setHasCancel(false);
    progress.setLabelText(tr("printing FCS Fit report ..."));
    progress.open();
    QTextDocument* doc=new QTextDocument();
    doc->setTextWidth(p->pageRect().size().width());
    createReportDoc(doc);
    doc->print(p);
    delete p;
    progress.accept();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::chkXLogScaleToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_taulog", chkXLogScale->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::chkGridToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_grid", chkGrid->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::plotStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_style", cmbPlotStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::errorStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_errorstyle", cmbErrorStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::residualStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_residualsstyle", cmbResidualStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::chkWeightedResidualsToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("weighted_residuals", chkWeightedResiduals->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::residualHistogramBinsChanged(int bins) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_residualshistogrambins", spinResidualHistogramBins->value(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}



void QFFCSFitEvaluationEditor::displayFitFunctionHelp() {
    hlpFunction->clear();
    if (!current) return;
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    QStringList sl;
    sl<<":/";
    QString ppid=cmbModel->itemData(cmbModel->currentIndex()).toString();
    int pid=services->getFitFunctionManager()->getPluginForID(ppid);
    QString dll=services->getFitFunctionManager()->getFilename(pid);
    if (data->getFitFunction(ppid)->helpFile().isEmpty()) hlpFunction->updateHelp(data->getFitFunction(ppid)->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+data->getFitFunction(ppid)->id()+".html");
    else hlpFunction->updateHelp(data->getFitFunction(ppid)->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+data->getFitFunction(ppid)->helpFile());
    hlpFunction->show();
}

void QFFCSFitEvaluationEditor::displayFitAlgorithmHelp() {
    hlpAlgorithm->clear();
    if (!current) return;
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    QStringList sl;
    sl<<":/";
    QString pid=cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString();
    int ppid=services->getFitAlgorithmManager()->getPluginForID(pid);
    //std::cout<<pid.toStdString()<<"   "<<ppid<<std::endl;
    QString dll=services->getFitAlgorithmManager()->getFilename(ppid);
    QFFitAlgorithm* algorithm=data->getFitAlgorithm(pid);
    if (algorithm) {
        if (algorithm->helpFile().isEmpty()) hlpAlgorithm->updateHelp(algorithm->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+algorithm->id()+".html");
        else hlpAlgorithm->updateHelp(algorithm->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+algorithm->helpFile());
        hlpAlgorithm->show();
    }
}

void QFFCSFitEvaluationEditor::configFitAlgorithm() {
    if (!current) return;
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!data) return;
    QString pid=cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString();
    QFFitAlgorithm* algorithm=data->getFitAlgorithm(pid);
    if (algorithm) {
        data->restoreQFFitAlgorithmParameters(algorithm);
        if (algorithm->displayConfig()) {
            data->storeQFFitAlgorithmParameters(algorithm);
        }
    }
}
void QFFCSFitEvaluationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!data) return;
    if (!current->getHighlightedRecord()) return;
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QString run=QString::number(data->getCurrentRun());
    if (data->getCurrentRun()<0) run="avg";
    current->getHighlightedRecord()->setQFProperty(resultID+"_r"+run+"_datacut_min", userMin, false, false);
    current->getHighlightedRecord()->setQFProperty(resultID+"_r"+run+"_datacut_max", userMax, false, false);
    replotData();
}

void QFFCSFitEvaluationEditor::copyUserMinToAll(int userMin) {
    if (!current) return;
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QFRawDataRecord*> recs=current->getProject()->getRawDataList();
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    for (int i=0; i<recs.size(); i++) {
        if (current->isApplicable(recs[i])) {
            QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(recs[i]);
            recs[i]->setQFProperty(resultID+"_ravg_datacut_min", userMin, false, false);

            for (int r=0; r<(int)fcs->getCorrelationRuns(); r++) {
                QString run=QString::number(r);
                if (!((recs[i]==current->getHighlightedRecord())&&(r==data->getCurrentRun()))) {
                    recs[i]->setQFProperty(resultID+"_r"+run+"_datacut_min", userMin, false, false);
                    //recs[i]->setQFProperty(resultID+"_r"+run+"_datacut_max", userMax, false, false);
                }
            }

        }
    }
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::copyUserMaxToAll(int userMax) {
    if (!current) return;
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    if (!data) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QFRawDataRecord*> recs=current->getProject()->getRawDataList();
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    for (int i=0; i<recs.size(); i++) {
        if (current->isApplicable(recs[i])) {
            QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(recs[i]);
            recs[i]->setQFProperty(resultID+"_ravg_datacut_max", userMax, false, false);

            for (int r=0; r<(int)fcs->getCorrelationRuns(); r++) {
                QString run=QString::number(r);
                if (!((recs[i]==current->getHighlightedRecord())&&(r==data->getCurrentRun()))) {
                    //recs[i]->setQFProperty(resultID+"_r"+run+"_datacut_min", userMin, false, false);
                    recs[i]->setQFProperty(resultID+"_r"+run+"_datacut_max", userMax, false, false);
                }
            }

        }
    }
    QApplication::restoreOverrideCursor();
}


void QFFCSFitEvaluationEditor::runChanged(int run) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;


    QFRawDataRecord* currentRecord=current->getHighlightedRecord();
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);

    labRun->setText(QString("  (%1)").arg(fcs->getCorrelationRunName(run)));

    data->setCurrentRun(run);

    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();

    datacut->disableSliderSignals();
    datacut->set_min(0);
    datacut->set_max(fcs->getCorrelationN());
    QString runn="avg";
    if (data->getCurrentRun()>-1) runn=QString::number(data->getCurrentRun());
    datacut->set_userMin(currentRecord->getProperty(resultID+"_r"+runn+"_datacut_min", 0).toInt());
    datacut->set_userMax(currentRecord->getProperty(resultID+"_r"+runn+"_datacut_max", fcs->getCorrelationN()).toInt());
    datacut->enableSliderSignals();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    displayModel(false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::modelChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    QString ff=cmbModel->itemData(cmbModel->currentIndex()).toString();
    data->setFitFunction(ff);
    displayModel(true);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::weightsChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    current->getHighlightedRecord()->setQFProperty("weights", cmbWeights->currentIndex(), false, false);
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    if (data) {
        if (cmbWeights->currentIndex()==0) data->setFitDataWeighting(QFFCSFitEvaluation::EqualWeighting);
        else if (cmbWeights->currentIndex()==1) data->setFitDataWeighting(QFFCSFitEvaluation::StdDevWeighting);
        else if (cmbWeights->currentIndex()==2) data->setFitDataWeighting(QFFCSFitEvaluation::RunErrorWeighting);
        else data->setFitDataWeighting(QFFCSFitEvaluation::EqualWeighting);
    }
    displayModel(true);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::algorithmChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    current->getHighlightedRecord()->setQFProperty("algorithm", cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString(), false, false);
    QFFCSFitEvaluation* data=qobject_cast<QFFCSFitEvaluation*>(current);
    QString alg=cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString();
    data->setFitAlgorithm(alg);
    QApplication::restoreOverrideCursor();
}

void QFFCSFitEvaluationEditor::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (!dataEventsEnabled) return;
    if (sender==pltData) {
        pltResiduals->setX(newxmin, newxmax);
    }/* else {
        plotter->setX(newxmin, newxmax);
    }*/
}

void QFFCSFitEvaluationEditor::plotMouseMove(double x, double y) {
    labMousePosition->setTextFormat(Qt::RichText);
    labMousePosition->setText(tr("cursor: (%1, %2)").arg(floattohtmlstr(x).c_str()).arg(floattohtmlstr(y).c_str()));
}

double* QFFCSFitEvaluationEditor::allocWeights(bool* weightsOKK, QFRawDataRecord* record_in, int run_in, int data_start, int data_end) {
    if (weightsOKK) *weightsOKK=false;
    if (!current) return NULL;
    if (!cmbModel) return NULL;
    QFRawDataRecord* record=record_in;
    if (!record_in) record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    QFFitFunction* ffunc=eval->getFitFunction();
    int run=run_in;
    if (run<=-100) run=eval->getCurrentRun();

    int N=data->getCorrelationN();

    QFFCSFitEvaluation::DataWeight weighting=eval->getFitDataWeighting();
    double* weights=(double*)malloc(N*sizeof(double));
    bool weightsOK=false;
    if (weighting==QFFCSFitEvaluation::StdDevWeighting) {
        double* std=data->getCorrelationStdDev();
        weightsOK=true;
        for (int i=0; i<N; i++) {
            weights[i]=std[i];
            if ((data_start>=0) && (data_end>=0)) {
                if ((i>=data_start)&&(i<=data_end)) {
                    if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                        weightsOK=false;
                        break;
                    }
                };
            } else {
                if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                    weightsOK=false;
                    break;
                };
            }
        }
    }
    if (weighting==QFFCSFitEvaluation::RunErrorWeighting) {
        double* std=data->getCorrelationRunError(run);
        weightsOK=true;
        for (int i=0; i<N; i++) {
            weights[i]=std[i];
            if ((data_start>=0) && (data_end>=0)) {
                if ((i>=data_start)&&(i<=data_end)) {
                    if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                        weightsOK=false;
                        break;
                    }
                };
            } else {
                if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                    weightsOK=false;
                    break;
                };
            }
        }
    }
    if (!weightsOK) {
        for (int i=0; i<N; i++) weights[i]=1;
        if (weighting==QFFCSFitEvaluation::EqualWeighting) weightsOK=true;
    }
    if (weightsOKK) *weightsOKK=weightsOK;
    return weights;
}

void QFFCSFitEvaluationEditor::saveCurrentFitResults() {
    if (!current) return;
    if (!cmbModel) return;
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();
    if (!ffunc || !eval) return;


    QString filter= tr("FCS Fit Parameter Set (*.fps)");
    QString selectedFilter=filter;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save FCS Fit Parameter Set as ..."), currentFPSSaveDir, filter, &selectedFilter);
    if ((!fileName.isEmpty())&&(!fileName.isNull())) {
        currentFPSSaveDir=QFileInfo(fileName).absolutePath();
        bool ok=true;
        if (QFile::exists(fileName)) {
            int ret = QMessageBox::question(this, tr("Save FCS Fit Parameter Set as  ..."),
                            tr("A Configuration with the name '%1' already exists.\n"
                               "Do you want to overwrite?").arg(fileName),
                            QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
            if (ret==QMessageBox::No) ok=false;
        }
        if (ok) {
            QSettings settings(fileName, QSettings::IniFormat);
            settings.setValue("fit_function/id", eval->getFitFunction()->id());

            QStringList pids=ffunc->getParameterIDs();
            double* fullParams=eval->allocFillParameters();
            double* errors=eval->allocFillParameterErrors();
            bool* paramsFix=eval->allocFillFix();
            double* paramsMin=eval->allocFillParametersMin();
            double* paramsMax=eval->allocFillParametersMax();
            //ffunc->calcParameter(fullParams, errors);
            for (int i=0; i<pids.size(); i++) {
                QString id=pids[i];
                int num=ffunc->getParameterNum(id);
                if (!id.isEmpty()) {
                    bool visible=ffunc->isParameterVisible(num, fullParams);
                    QFFitFunction::ParameterDescription d=ffunc->getDescription(id);
                    if (!d.userEditable) visible=false;

                    if (visible) {
                        settings.setValue("fit_params/"+id+"/value", eval->getFitValue(id));
                        if (d.displayError==QFFitFunction::EditError) settings.setValue("fit_params/"+id+"/error", eval->getFitError(id));
                        if (d.fit) settings.setValue("fit_params/"+id+"/fix", eval->getFitFix(id));
                        if (d.userRangeEditable) {
                           settings.setValue("fit_params/"+id+"/min", eval->getFitMin(id));
                           settings.setValue("fit_params/"+id+"/max", eval->getFitMax(id));
                        }
                    }
                }
            }
            free(fullParams);
            free(errors);
            free(paramsFix);
            free(paramsMin);
            free(paramsMax);
        }
    }
}

void QFFCSFitEvaluationEditor::loadCurrentFitResults() {
    if (!current) return;
    if (!cmbModel) return;
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();
    if (!ffunc || !eval) return;

    QString filter= tr("FCS Fit Parameter Set (*.fps)");
    QString selectedFilter=filter;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load FCS Fit Parameter Set ..."), currentFPSSaveDir, filter, &selectedFilter);
    if ((!fileName.isEmpty())&&(!fileName.isNull())) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        currentFPSSaveDir=QFileInfo(fileName).absolutePath();
        QSettings settings(fileName, QSettings::IniFormat);
        QString ffuncname=settings.value("fit_function/id", eval->getFitFunction()->id()).toString();
        eval->setFitFunction(ffuncname);
        settings.beginGroup("fit_params");
        QStringList keys = settings.allKeys();
        for (int i=0; i<keys.size(); i++) {
            if (keys[i].endsWith("/error")) {
                QString paramname=keys[i].left(keys[i].length()-6);
                eval->setFitError(paramname, settings.value(keys[i]).toDouble());
            } else if (keys[i].endsWith("/value")) {
                QString paramname=keys[i].left(keys[i].length()-6);
                eval->setFitValue(paramname, settings.value(keys[i]).toDouble());
            } else if (keys[i].endsWith("/fix")) {
                QString paramname=keys[i].left(keys[i].length()-4);
                eval->setFitFix(paramname, settings.value(keys[i]).toBool());
            } else if (keys[i].endsWith("/min")) {
                QString paramname=keys[i].left(keys[i].length()-4);
                eval->setFitMin(paramname, settings.value(keys[i]).toDouble());
            } else if (keys[i].endsWith("/max")) {
                QString paramname=keys[i].left(keys[i].length()-4);
                eval->setFitMax(paramname, settings.value(keys[i]).toDouble());
            }
        }
        displayModel(true);
        replotData();
        QApplication::restoreOverrideCursor();

    }
}

void QFFCSFitEvaluationEditor::calibrateFocalVolume() {
    double particles=1;
    double tauD=10;
    double particles_error=0;
    double tauD_error=0;
    double wxy=0, wxy_error=0;
    double gamma=6, gamma_error=1;
    bool has_tauD=false;
    bool has_nparticles=false;
    bool has_gamma=false;

    if (!current) return;
    if (!cmbModel) return;
    QFFCSFitEvaluation* eval=qobject_cast<QFFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();
    if (!ffunc || !eval) return;

    //read input parameters
    for (int i=0; i<ffunc->paramCount(); i++) {
        QString id=ffunc->getParameterID(i);
        QFFitFunction::ParameterDescription d=ffunc->getDescription(i);
        if (id.toLower()=="n_particle") {
            particles=eval->getFitValue(id);
            particles_error=eval->getFitError(id);
            has_nparticles=true;
        }
        if ((id.toLower()=="1n_particle") && (!has_nparticles)) {
            particles=1.0/eval->getFitValue(id);
            particles_error=fabs(eval->getFitError(id)/(eval->getFitValue(id)*eval->getFitValue(id)));
            has_nparticles=true;
        }
        if ((id.toLower()=="focus_struct_fac") && (!has_nparticles)) {
            gamma=eval->getFitValue(id);
            gamma_error=eval->getFitError(id);
            has_gamma=true;
        }
        if ((id.toLower()=="diff_tau1") || (id.toLower()=="diff_tau")) {
            double factor=1;
            if (d.unit=="msec") { factor=1000; }
            if (d.unit=="ms") { factor=1000; }
            if (d.unit=="msecs") { factor=1000; }
            if (d.unit=="milliseconds") { factor=1000; }
            if (d.unit=="sec") { factor=1000000; }
            if (d.unit=="s") { factor=1000000; }
            if (d.unit=="secs") { factor=1000000; }
            if (d.unit=="seconds") { factor=1000000; }
            tauD=factor*eval->getFitValue(id);
            tauD_error=factor*eval->getFitError(id);
            has_tauD=true;
        }
    }

    dlgEstimateFocalVolume* dlg=new dlgEstimateFocalVolume(settings, this);
    dlg->init(particles,  particles_error, has_nparticles, tauD, tauD_error, has_tauD, gamma, gamma_error, has_gamma);

    bool ok= ( dlg->exec() == QDialog::Accepted);
    wxy=dlg->get_wxy();
    wxy_error=dlg->get_wxyerror();

    delete dlg;
    //write back output parameters
    if (ok) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        for (int i=0; i<ffunc->paramCount(); i++) {
            QString id=ffunc->getParameterID(i);
            QFFitFunction::ParameterDescription d=ffunc->getDescription(i);
            if (id.toLower()=="focus_width") {

                double factor=1;
                if (d.unit=="micron") { factor=1000; }
                if (d.unit=="µm") { factor=1000; }
                if (d.unit=="um") { factor=1000; }
                if (d.unit=="microns") { factor=1000; }
                if (d.unit=="m") { factor=1000000; }
                if (d.unit=="meter") { factor=1000000; }
                if (d.unit=="meters") { factor=1000000; }
                eval->setFitValue(id, wxy/factor);
                eval->setFitError(id, wxy_error/factor);
            }
        }
        displayModel(true);
        replotData();
        QApplication::restoreOverrideCursor();
    }
}
