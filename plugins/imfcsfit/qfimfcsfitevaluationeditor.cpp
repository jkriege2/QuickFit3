#include "qfimfcsfitevaluationeditor.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include <iostream>
#include <cfloat>
#include "qfimfcsfitevaluation.h"
#include "tools.h"
#include "statistics_tools.h"
#include <QThread>
#include "qmoretextobject.h"
#include "qmodernprogresswidget.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"






QFImFCSFitEvaluationEditor::QFImFCSFitEvaluationEditor(QFPluginServices* services, QWidget* parent):
    QFFitResultsByIndexEvaluationEditorBase("imfcsfitevaleditor/", services, parent)
{
    cmbModel=NULL;
    dataEventsEnabled=true;
    m_parameterWidgetWidth=75;
    m_parameterCheckboxWidth=32;
    fitStatisticsReport="";


    createWidgets();
}

QFImFCSFitEvaluationEditor::~QFImFCSFitEvaluationEditor()
{
    //dtor
}


void QFImFCSFitEvaluationEditor::createWidgets() {
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
    btnConfigAlgorithm=new QPushButton(QIcon(":/fcsfit/fit_config.png"), tr("&Configure Algorithm"), this);
    btnConfigAlgorithm->setMaximumWidth(250);
    hbl->addWidget(btnConfigAlgorithm);
    btnAlgorithmHelp=new QPushButton(QIcon(":/fcsfit/fit_help.png"), tr("Algorithm &Help"), this);
    btnAlgorithmHelp->setMaximumWidth(250);
    hbl->addWidget(btnAlgorithmHelp);
    //fl->addRow(l, hbl);

    cmbWeights=new QComboBox(this);
    cmbWeights->setEditable(false);
    cmbWeights->addItem(tr("equal weights"));
    cmbWeights->addItem(tr("standard deviation"));
    cmbWeights->addItem(tr("per pixel errors"));
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
    btnModelHelp=new QPushButton(QIcon(":/fcsfit/fit_help.png"), tr("Model H&elp"), this);
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
    actSaveReport=new QAction(QIcon(":/fcsfit/fit_savereport.png"), tr("Save Report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actPrintReport=new QAction(QIcon(":/fcsfit/fit_printreport.png"), tr("Print Report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    QLabel* lPS=new QLabel(tr("   &Plots: "), toolbar);
    lPS->setFont(boldfont);
    cmbPlotStyle=new QComboBox(toolbar);
    cmbPlotStyle->addItem(QIcon(":/fcsfit/plot_points.png"), tr("points"));
    cmbPlotStyle->addItem(QIcon(":/fcsfit/plot_lines.png"), tr("lines"));
    cmbPlotStyle->addItem(QIcon(":/fcsfit/plot_pointslines.png"), tr("lines + points"));
    lPS->setBuddy(cmbPlotStyle);
    QLabel* lES=new QLabel(tr("   &Errors: "), toolbar);
    lES->setFont(boldfont);
    cmbErrorStyle=new QComboBox(toolbar);
    cmbErrorStyle->addItem(QIcon(":/fcsfit/plot_enone.png"), tr("none"));
    cmbErrorStyle->addItem(QIcon(":/fcsfit/plot_elines.png"), tr("lines"));
    cmbErrorStyle->addItem(QIcon(":/fcsfit/plot_ebars.png"), tr("bars"));
    cmbErrorStyle->addItem(QIcon(":/fcsfit/plot_elinesbars.png"), tr("bars + lines"));
    lES->setBuddy(cmbErrorStyle);

    QLabel* lRS=new QLabel(tr("   &Residuals: "), toolbar);
    lRS->setFont(boldfont);
    cmbResidualStyle=new QComboBox(toolbar);
    cmbResidualStyle->addItem(QIcon(":/fcsfit/plot_points.png"), tr("points"));
    cmbResidualStyle->addItem(QIcon(":/fcsfit/plot_lines.png"), tr("lines"));
    cmbResidualStyle->addItem(QIcon(":/fcsfit/plot_pointslines.png"), tr("lines + points"));
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

    splitModel=new QVisibleHandleSplitter(this),
    splitModel->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(splitModel, 10);

    splitPlot=new QVisibleHandleSplitter(this),
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

    splitFitStatistics=new QVisibleHandleSplitter(this);
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
    fl->addRow(tr("pixel: "), hblRun);
    layModel->addLayout(fl, 0);

    labFitParameters=new QLabel(this);
    layModel->addWidget(labFitParameters);
    /*btnEditRanges=new QPushButton(tr("Edit &Ranges"), this);
    btnEditRanges->setCheckable(true);
    btnEditRanges->setChecked(false);
    QHBoxLayout* hblfp=new QHBoxLayout(this);
    hblfp->addWidget(labFitParameters);
    hblfp->addStretch();
    hblfp->addWidget(btnEditRanges);
    layModel->addLayout(hblfp);*/
    tbEditRanges=new QTabBar(this);
    tbEditRanges->addTab(tr("Parameter Values"));
    tbEditRanges->addTab(tr("Parameter Ranges"));
    tbEditRanges->setShape(QTabBar::TriangularNorth);
    tbEditRanges->setDrawBase(false);
    tbEditRanges->setCurrentIndex(0);
    QVBoxLayout* hblfp=new QVBoxLayout(this);
    hblfp->addWidget(labFitParameters);
    hblfp->addWidget(tbEditRanges);
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
    btnFitCurrent=new QPushButton(QIcon(":/fcsfit/fit_fitcurrent.png"), tr("&Fit Current Pixel"), this);
    btnFitCurrent->setToolTip(tr("perform a fit for the currently displayed file and pixel"));
    layBtn->addWidget(btnFitCurrent, 0, 0);
    btnFitRunsCurrent=new QPushButton(QIcon(":/fcsfit/fit_fitallruns.png"), tr("Fit All &Pixels"), this);
    btnFitRunsCurrent->setToolTip(tr("perform a fit for all pixels in the currently selected file "));
    layBtn->addWidget(btnFitRunsCurrent, 0, 1);
    btnFitAll=new QPushButton(QIcon(":/fcsfit/fit_fitcurrentrunallfiles.png"), tr("Fit All &Files (Current Pixel)"), this);
    btnFitAll->setToolTip(tr("perform a fit for all files, but fit in each file only the currently displayed pixel"));
    layBtn->addWidget(btnFitAll, 1, 0);
    btnFitRunsAll=new QPushButton(QIcon(":/fcsfit/fit_fitall.png"), tr("Fit &Everything"), this);
    btnFitRunsAll->setToolTip(tr("perform a fit for all pixels in all files"));
    layBtn->addWidget(btnFitRunsAll, 1, 1);
    btnResetCurrent=new QPushButton(tr("&Reset Current Pixel"), this);
    btnResetCurrent->setToolTip(tr("reset the currently displayed file (and pixel) to the initial parameters\nThis deletes all fit results stored for the current file."));
    layBtn->addWidget(btnResetCurrent, 2, 0);
    btnResetAll=new QPushButton(tr("&Reset All"), this);
    btnResetAll->setToolTip(tr("reset all loaded files to the initial parameters.\nThis deletes all fit results stored for all files."));
    layBtn->addWidget(btnResetAll, 3, 1);

    btnResetAllRuns=new QPushButton(tr("&Reset All Pixels"), this);
    btnResetAllRuns->setToolTip(tr("reset all pixels to the initial parameters in the current file.\nThis deletes all fit results stored for all pixels in the current file."));
    layBtn->addWidget(btnResetAllRuns, 2, 1);

    btnCopyToInitial=new QPushButton(tr("Copy to &Initial"), this);
    btnCopyToInitial->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters,\n so they are used by files/pixels that were not fit yet."));
    layBtn->addWidget(btnCopyToInitial, 4, 0);
    btnCopyToAllRuns=new QPushButton(tr("&Copy to All Pixels"), this);
    btnCopyToAllRuns->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters\n and also to all runs in the current file."));
    layBtn->addWidget(btnCopyToAllRuns, 4, 1);
    btnCopyToAll=new QPushButton(tr("&Copy to All"), this);
    btnCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set\n of initial parameters and also to all files."));
    layBtn->addWidget(btnCopyToAll, 5, 1);
    btnCopyToAllCurrentRun=new QPushButton(tr("&Copy to All (Current Pixel)"), this);
    btnCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set of\n initial parameters and also to all files, but only to the current pixel therein."));
    layBtn->addWidget(btnCopyToAllCurrentRun, 5, 0);

    btnLoadParameters=new QPushButton(QIcon(":/fcsfit/param_load.png"), tr("&Load Parameters"), this);
    btnLoadParameters->setToolTip(tr("load a FCS fit parameter set from a file.\nThe parameter set files can be created using \"Save Parameters\""));
    layBtn->addWidget(btnLoadParameters, 7, 0);
    btnSaveParameters=new QPushButton(QIcon(":/fcsfit/param_save.png"), tr("&Save Parameters"), this);
    btnSaveParameters->setToolTip(tr("save the current FCS fit parameter as a set to a file\nfor later reuse with \"Load Parameters\""));
    layBtn->addWidget(btnSaveParameters, 7, 1);

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
    hlpAlgorithm->initFromPluginServices(services);
    hlpAlgorithm->close();
    hlpFunction=new QFHTMLHelpWindow(0);
    hlpFunction->initFromPluginServices(services);
    hlpFunction->close();

    toolbar->addAction(actSaveReport);
    toolbar->addAction(actPrintReport);
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actSavePlot()); pltData->get_plotter()->get_actSavePlot()->setIcon(QIcon(":/fcsfit/plot_saveplot.png"));
    toolbar->addAction(pltData->get_plotter()->get_actSaveData()); pltData->get_plotter()->get_actSaveData()->setIcon(QIcon(":/fcsfit/plot_savedata.png"));
    toolbar->addAction(pltData->get_plotter()->get_actPrint()); pltData->get_plotter()->get_actPrint()->setIcon(QIcon(":/fcsfit/plot_print.png"));
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actCopyPixelImage());
    toolbar->addAction(pltData->get_plotter()->get_actCopyData());
    toolbar->addAction(pltData->get_plotter()->get_actCopyMatlab());
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actZoomAll()); pltData->get_plotter()->get_actZoomAll()->setIcon(QIcon(":/fcsfit/plot_zoomall.png"));
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
    connect(btnResetAllRuns, SIGNAL(clicked()), this, SLOT(resetAllRuns()));
    connect(btnCopyToAllRuns, SIGNAL(clicked()), this, SLOT(copyToAllRuns()));
    connect(btnCopyToInitial, SIGNAL(clicked()), this, SLOT(copyToInitial()));
    connect(btnCopyToAllCurrentRun, SIGNAL(clicked()), this, SLOT(copyToAllCurrentRun()));
    connect(btnLoadParameters, SIGNAL(clicked()), this, SLOT(loadCurrentFitResults()));
    connect(btnSaveParameters, SIGNAL(clicked()), this, SLOT(saveCurrentFitResults()));
    connect(btnCalibrateFocalVolume, SIGNAL(clicked()), this, SLOT(calibrateFocalVolume()));

    connect(datacut, SIGNAL(copyUserMinToAll(int)), this, SLOT(copyUserMinToAll(int)));
    connect(datacut, SIGNAL(copyUserMaxToAll(int)), this, SLOT(copyUserMaxToAll(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAll(int,int)), this, SLOT(copyUserMinMaxToAll(int,int)));
    connect(datacut, SIGNAL(copyUserMinToAllRuns(int)), this, SLOT(copyUserMinToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMaxToAllRuns(int)), this, SLOT(copyUserMaxToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAllRuns(int,int)), this, SLOT(copyUserMinMaxToAllRuns(int,int)));

}


void QFImFCSFitEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    QFImFCSFitEvaluation* fcs=qobject_cast<QFImFCSFitEvaluation*>(current);

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
        spinResidualHistogramBins->setValue(current->getProperty("plot_residualshistogrambins", 25).toInt());

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


void QFImFCSFitEvaluationEditor::readSettings() {
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
        //btnEditRanges->setChecked(settings->getQSettings()->value("fcsfitevaleditor/display_range_widgets", false).toBool());
        tbEditRanges->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/display_range_widgets", 0).toInt());
        spinResidualHistogramBins->setValue(settings->getQSettings()->value("fcsfitevaleditor/residual_histogram_bins", 25).toInt());
        tabResidulas->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/residual_toolbox_current", 0).toInt());
        QFFitResultsEvaluationEditorBase::readSettings();

    }
}

void QFImFCSFitEvaluationEditor::writeSettings() {
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
        //settings->getQSettings()->setValue("fcsfitevaleditor/display_range_widgets", btnEditRanges->isChecked());
        settings->getQSettings()->setValue("fcsfitevaleditor/display_range_widgets", tbEditRanges->currentIndex());
        settings->getQSettings()->setValue("fcsfitevaleditor/residual_histogram_bins", spinResidualHistogramBins->value());
        settings->getQSettings()->setValue("fcsfitevaleditor/residual_toolbox_current", tabResidulas->currentIndex());
        QFFitResultsEvaluationEditorBase::writeSettings();
    }
}

void QFImFCSFitEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));
    bool modelChanged=false;
    std::cout<<"QFImFCSFitEvaluationEditor::highlightingChanged("<<formerRecord<<", "<<currentRecord<<")  eval="<<eval<<"   data="<<data<<"\n";
    if (data) {
        //labRecord->setText(tr("<b>current:</b> <i>%1</i>").arg(currentRecord->getName()) );
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));

        datacut->disableSliderSignals();
        datacut->set_min(0);
        datacut->set_max(data->getCorrelationN()-1);
        QString run="avg";
        if (eval->getCurrentIndex()>-1) run=QString::number(eval->getCurrentIndex());
        datacut->set_userMin(getUserMin(0));
        datacut->set_userMax(getUserMax(data->getCorrelationN()-1));
        datacut->enableSliderSignals();
        dataEventsEnabled=false;
        spinRun->setMaximum(data->getCorrelationRuns()-1);
        if (data->getCorrelationRuns()==1) spinRun->setMaximum(-1);
        spinRun->setValue(eval->getCurrentIndex());
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

void QFImFCSFitEvaluationEditor::displayModel(bool newWidget) {
    if (!current) return;
    if (!cmbModel) return;
    //QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();

    //qDebug()<<" **** displayModel()";

    if (!ffunc) {
        /////////////////////////////////////////////////////////////////////////////////////////////
        // delete all fit parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        //bool updEn=updatesEnabled();
        //setUpdatesEnabled(false);
        for (int i=0; i<m_fitParameters.size(); i++) {
            if (m_fitParameters[i]) {
                m_fitParameters[i]->disableDatastore();
                //disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
                disconnect(tbEditRanges, SIGNAL(currentChanged(int)), m_fitParameters[i], SLOT(setEditRange(int)));
                disconnect(m_fitParameters[i], SIGNAL(valueChanged(QString, double)), this, SLOT(parameterValueChanged()));
                disconnect(m_fitParameters[i], SIGNAL(errorChanged(QString, double)), this, SLOT(parameterValueChanged()));
                disconnect(m_fitParameters[i], SIGNAL(fixChanged(QString, bool)), this, SLOT(parameterFixChanged()));
                disconnect(m_fitParameters[i], SIGNAL(rangeChanged(QString, double, double)), this, SLOT(parameterRangeChanged()));
                delete m_fitParameters[i];
            }
        }
        m_fitParameters.clear();
        //setUpdatesEnabled(updEn);
        return;
    }
    QFFitAlgorithm* falg=eval->getFitAlgorithm();

    //bool updEn=updatesEnabled();
    //setUpdatesEnabled(false);

    if (newWidget) {
        /////////////////////////////////////////////////////////////////////////////////////////////
        // first delete all fit parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<m_fitParameters.size(); i++) {
            if (m_fitParameters[i]) {
                m_fitParameters[i]->disableDatastore();
                //disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
                //disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(unsetEditValues(bool)));
                disconnect(tbEditRanges, SIGNAL(currentChanged(int)), m_fitParameters[i], SLOT(setEditRange(int)));
                disconnect(tbEditRanges, SIGNAL(currentChanged(int)), m_fitParameters[i], SLOT(unsetEditValues(int)));
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
        //connect(btnEditRanges, SIGNAL(toggled(bool)), header, SLOT(setEditRange(bool)));
        //connect(btnEditRanges, SIGNAL(toggled(bool)), header, SLOT(unsetEditValues(bool)));
        connect(tbEditRanges, SIGNAL(currentChanged(int)), header, SLOT(setEditRange(int)));
        connect(tbEditRanges, SIGNAL(currentChanged(int)), header, SLOT(unsetEditValues(int)));
        //header->setEditRange(btnEditRanges->isChecked());
        //header->unsetEditValues(!btnEditRanges->isChecked());
        header->setEditRange(tbEditRanges->currentIndex());
        header->unsetEditValues(tbEditRanges->currentIndex());

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
            //connect(btnEditRanges, SIGNAL(toggled(bool)), fpw, SLOT(unsetEditValues(bool)));
            //connect(btnEditRanges, SIGNAL(toggled(bool)), fpw, SLOT(setEditRange(bool)));
            connect(tbEditRanges, SIGNAL(currentChanged(int)), fpw, SLOT(unsetEditValues(int)));
            connect(tbEditRanges, SIGNAL(currentChanged(int)), fpw, SLOT(setEditRange(int)));
            connect(fpw, SIGNAL(valueChanged(QString, double)), this, SLOT(parameterValueChanged()));
            connect(fpw, SIGNAL(errorChanged(QString, double)), this, SLOT(parameterValueChanged()));
            connect(fpw, SIGNAL(fixChanged(QString, bool)), this, SLOT(parameterFixChanged()));
            connect(fpw, SIGNAL(rangeChanged(QString, double, double)), this, SLOT(parameterRangeChanged()));
            connect(fpw, SIGNAL(enterPressed(QString)), this, SLOT(fitCurrent()));
            //fpw->setEditRange(btnEditRanges->isChecked());
            //fpw->unsetEditValues(btnEditRanges->isChecked());
            fpw->setEditRange(tbEditRanges->currentIndex());
            fpw->unsetEditValues(tbEditRanges->currentIndex());
        }
        btnCalibrateFocalVolume->setEnabled((has_tauD||(has_particles&&has_gamma))&&has_wxy);
        // add stretcher item in bottom row
        layParameters->addItem(new QSpacerItem(5,5, QSizePolicy::Minimum, QSizePolicy::Expanding), layParameters->rowCount(), 0);
    }

    if (eval->getCurrentIndex()!=spinRun->value()) eval->setCurrentIndex(eval->getCurrentIndex());

    if (eval->hasFit()) {
        labFitParameters->setText(tr("<b><u>Local</u> Fit Parameters:</b>"));
        //labFitResult->setText(current->getHighlightedRecord()->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML"));
    } else {
        labFitParameters->setText(tr("<b><u>Global</u> Fit Parameters:</b>"));
        labFitResult->setText("");
    }

    updateParameterValues();
    //setUpdatesEnabled(updEn);
}

void QFImFCSFitEvaluationEditor::parameterValueChanged() {
    //bool updEn=updatesEnabled();
    //setUpdatesEnabled(false);
    QTime t;
    t.start();
    //qDebug()<<"QFImFCSFitEvaluationEditor::parameterValueChanged()";
    updateParameterValues();
    //qDebug()<<"QFImFCSFitEvaluationEditor::parameterValueChanged() ... half "<<t.elapsed()<<" ms";
    replotData();
    //qDebug()<<"QFImFCSFitEvaluationEditor::parameterValueChanged() ... done "<<t.elapsed()<<" ms";
    //setUpdatesEnabled(updEn);
}

void QFImFCSFitEvaluationEditor::parameterFixChanged() {
    updateParameterValues();
}

void QFImFCSFitEvaluationEditor::parameterRangeChanged() {
    //bool updEn=updatesEnabled();
    //setUpdatesEnabled(false);
    updateParameterValues();
    replotData();
    //setUpdatesEnabled(updEn);
}

void QFImFCSFitEvaluationEditor::updateParameterValues() {
    if (!current) return;
    if (!cmbModel) return;

    QTime t;
    t.start();

    //qDebug()<<"QFImFCSFitEvaluationEditor::updateParameterValues()";
    //QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
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
    //qDebug()<<"QFImFCSFitEvaluationEditor::updateParameterValues() ... done "<<t.elapsed()<<" ms";

}

void QFImFCSFitEvaluationEditor::replotData() {


    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();

    if ((!eval)||(!data)) {
        pltData->clearGraphs();
        ds->clear();
        return;
    }

    //qDebug()<<" **** replotData()";
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
    dsres->clear();
    ds->clear();
    dsresh->clear();
    dsresc->clear();

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
                if (eval->getFitDataWeighting()==QFImFCSFitEvaluation::RunErrorWeighting) {
                    c_std=ds->addColumn(data->getCorrelationRunError(eval->getCurrentIndex()), data->getCorrelationN(), "cperpixelerror");
                    errorName=tr("per pixel");
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
    //qDebug()<<"  replotData end  runtime = "<<t1.elapsed()<<" ms";

    //setUpdatesEnabled(updEn);
}



void QFImFCSFitEvaluationEditor::updateFitFunctions() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    int run=eval->getCurrentIndex();
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    QFFitFunction* ffunc=eval->getFitFunction();
    //QFImFCSFitEvaluation::DataWeight weighting=eval->getFitDataWeighting();

    if (!ffunc) return;

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

                if (eval->getCurrentIndex()<0) {
                    corrdata=data->getCorrelationMean();
                } else {
                    if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                        corrdata=data->getCorrelationRun(eval->getCurrentIndex());
                    } else {
                        corrdata=data->getCorrelationMean();
                    }
                }

                double* weights=eval->allocWeights(NULL, record, eval->getCurrentIndex(), datacut_min, datacut_max);


                /////////////////////////////////////////////////////////////////////////////////
                // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
                // we are working with a complete set of parameters
                /////////////////////////////////////////////////////////////////////////////////
                double* fullParams=eval->allocFillParameters();
                double* errors=eval->allocFillParameterErrors();
                bool* paramsFix=eval->allocFillFix(record, eval->getCurrentIndex());
                ffunc->calcParameter(fullParams, errors);

                /////////////////////////////////////////////////////////////////////////////////
                // calculate fit statistics
                /////////////////////////////////////////////////////////////////////////////////
                record->disableEmitResultsChanged();
                QFFitFunction::FitStatistics fitResults=eval->calcFitStatistics(ffunc, N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins, record, run);
                record->enableEmitResultsChanged();


                size_t c_fit = ds->addCopiedColumn(fitResults.fitfunc, N, "fit_model");
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
                for (int i=0; i<(int)ffunc->getAdditionalPlotCount(fullParams); i++) {
                    double* params=eval->allocFillParameters();
                    QString name=ffunc->transformParametersForAdditionalPlot(i, params);
                    double* afitfunc=(double*)malloc(N*sizeof(double));
                    for (int j=0; j<N; j++) {
                        afitfunc[j]=ffunc->evaluate(tauvals[j], params);
                    }
                    size_t c_afit=ds->addCopiedColumn(afitfunc, N, QString("add_fit_model_%1").arg(i));
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
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_taures=c_tau;//dsres->addCopiedColumn(data->getCorrelationT(), N, "tau");
                size_t c_residuals=0;
                JKQTPxyLineGraph* g_residuals=new JKQTPxyLineGraph(pltResiduals->get_plotter());
                if (chkWeightedResiduals->isChecked()) {
                    c_residuals=dsres->addCopiedColumn(fitResults.residuals_weighted, N, "residuals_weighted");
                    g_residuals->set_title("weighted residuals");
                } else {
                    c_residuals=dsres->addCopiedColumn(fitResults.residuals, N, "residuals");
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
                size_t c_tauresra=dsres->addCopiedColumn(fitResults.tau_runavg, fitResults.runAvgN, "tau_resid_pixelavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(fitResults.residuals_runavg_weighted, fitResults.runAvgN, "residuals_pixelavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(fitResults.residuals_runavg, fitResults.runAvgN, "residuals_pixelavg");
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
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fitResults.rminw+fitResults.residHistWBinWidth/2.0, fitResults.rmaxw-fitResults.residHistWBinWidth/2.0, "residualhist_weighted_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fitResults.resWHistogram, residualHistogramBins, "residualhist_weighted_y");
                } else {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fitResults.rmin+fitResults.residHistBinWidth/2.0, fitResults.rmax-fitResults.residHistBinWidth/2.0, "residualhist_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fitResults.resHistogram, residualHistogramBins, "residualhist_y");
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
                size_t c_residualCorrelationX=dsresc->addLinearColumn(fitResults.resN-1, 1, fitResults.resN-1, "residualcorr_x");
                size_t c_residualCorrelationY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualCorrelationY=dsresc->addCopiedColumn(&(fitResults.resWCorrelation[1]), fitResults.resN-1, "residualcorr_weighted_y");
                } else {
                    c_residualCorrelationY=dsresh->addCopiedColumn(&(fitResults.resCorrelation[1]), fitResults.resN-1, "residualcorr_y");
                }
                JKQTPxyLineGraph* g_residualsCorrelation=new JKQTPxyLineGraph(pltResidualCorrelation->get_plotter());
                g_residualsCorrelation->set_xColumn(c_residualCorrelationX);
                g_residualsCorrelation->set_yColumn(c_residualCorrelationY);
                pltResidualCorrelation->addGraph(g_residualsCorrelation);
                //qDebug()<<"    k "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // update display of fit results
                /////////////////////////////////////////////////////////////////////////////////
                QString txtFit="<font face=\"Arial\">";
                QString fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML", eval->getCurrentIndex(), true);

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
                                "</tr>").arg(fitResults.residSqrSum).arg(fitResults.residWeightSqrSum);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&lang;E&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &lang;E&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fitResults.residAverage).arg(fitResults.residWeightAverage);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fitResults.residStdDev).arg(fitResults.residWeightStdDev);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">NP =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">NR =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fitResults.fitparamN).arg(fitResults.dataSize);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">DF =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\"></td><td align=\"left\" valign=\"bottom\"></td>"
                                "</tr>").arg(fitResults.degFreedom);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">TSS  =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">R<sup>2</sup> =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fitResults.TSS).arg(fitResults.Rsquared);
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
                free(fullParams);
                free(errors);
                free(weights);
                free(paramsFix);
                fitResults.free();

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

















void QFImFCSFitEvaluationEditor::chkXLogScaleToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    current->setQFProperty("plot_taulog", chkXLogScale->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::chkGridToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    current->setQFProperty("plot_grid", chkGrid->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::plotStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    current->setQFProperty("plot_style", cmbPlotStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::errorStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    current->setQFProperty("plot_errorstyle", cmbErrorStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::residualStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    current->setQFProperty("plot_residualsstyle", cmbResidualStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::chkWeightedResidualsToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    current->setQFProperty("weighted_residuals", chkWeightedResiduals->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::residualHistogramBinsChanged(int bins) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    current->setQFProperty("plot_residualshistogrambins", spinResidualHistogramBins->value(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}



void QFImFCSFitEvaluationEditor::displayFitFunctionHelp() {
    hlpFunction->clear();
    if (!current) return;
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    //QStringList sl;
    //sl<<":/";
    QString pid=cmbModel->itemData(cmbModel->currentIndex()).toString();
    int ppid=services->getFitFunctionManager()->getPluginForID(pid);
    //QString dll=services->getFitFunctionManager()->getPluginFilename(pid);
    //if (data->getFitFunction(ppid)->helpFile().isEmpty()) hlpFunction->updateHelp(data->getFitFunction(ppid)->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+data->getFitFunction(ppid)->id()+".html");
    //else hlpFunction->updateHelp(data->getFitFunction(ppid)->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+data->getFitFunction(ppid)->helpFile());
    //hlpFunction->show();
    QFFitFunction* function=data->getFitFunction(pid);
    QString help=services->getFitFunctionManager()->getPluginHelp(ppid, pid);
    if (QFile::exists(help) && function) {
        hlpFunction->updateHelp(help);
        hlpFunction->show();
    } else {
        QMessageBox::information(this, tr("FCS Fit"), tr("No Online-Help for this fit function available."));
    }
}

void QFImFCSFitEvaluationEditor::displayFitAlgorithmHelp() {
    hlpAlgorithm->clear();
    if (!current) return;
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    //QStringList sl;
    //sl<<":/";
    QString pid=cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString();
    int ppid=services->getFitAlgorithmManager()->getPluginForID(pid);
    //std::cout<<pid.toStdString()<<"   "<<ppid<<std::endl;
    //QString dll=services->getFitAlgorithmManager()->getPluginFilename(ppid);
    //QFFitAlgorithm* algorithm=data->getFitAlgorithm(pid);
    //if (algorithm) {
    //    if (algorithm->helpFile().isEmpty()) hlpAlgorithm->updateHelp(algorithm->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+algorithm->id()+".html");
    //    else hlpAlgorithm->updateHelp(algorithm->name(), services->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(dll).baseName()+QString("/")+algorithm->helpFile());
    //    hlpAlgorithm->show();
    //}
    QFFitAlgorithm* algorithm=data->getFitAlgorithm(pid);
    QString help=services->getFitAlgorithmManager()->getPluginHelp(ppid, pid);
    if (QFile::exists(help) && algorithm) {
        hlpAlgorithm->updateHelp(help);
        hlpAlgorithm->show();
    } else {
        QMessageBox::information(this, tr("FCS Fit"), tr("No Online-Help for this fit algorithm available."));
    }
}

void QFImFCSFitEvaluationEditor::configFitAlgorithm() {
    if (!current) return;
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
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

void QFImFCSFitEvaluationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (!data) return;
    if (!current->getHighlightedRecord()) return;
    setUserMinMax(userMin, userMax);
    replotData();
}



void QFImFCSFitEvaluationEditor::runChanged(int run) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;

    //qDebug()<<"runChanged";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTime t;

    t.start();
    QFRawDataRecord* currentRecord=current->getHighlightedRecord();
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();
    labRun->setText(QString("  (%1)").arg(fcs->getCorrelationRunName(run)));
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();

    data->setCurrentIndex(run);
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();

    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();

    datacut->disableSliderSignals();
    datacut->set_min(0);
    datacut->set_max(fcs->getCorrelationN());
    QString runn="avg";
    if (data->getCurrentIndex()>-1) runn=QString::number(data->getCurrentIndex());
    datacut->set_userMin(getUserMin(0));
    datacut->set_userMax(getUserMax(fcs->getCorrelationN()));
    datacut->enableSliderSignals();
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();

    displayModel(false);
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();
    replotData();
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();
    QApplication::restoreOverrideCursor();
    ////qDebug()<<"runChanged ... done";
}

void QFImFCSFitEvaluationEditor::modelChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    QString ff=cmbModel->itemData(cmbModel->currentIndex()).toString();
    data->setFitFunction(ff);
    displayModel(true);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::weightsChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    current->getHighlightedRecord()->setQFProperty("weights", cmbWeights->currentIndex(), false, false);
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (data) {
        if (cmbWeights->currentIndex()==0) data->setFitDataWeighting(QFImFCSFitEvaluation::EqualWeighting);
        else if (cmbWeights->currentIndex()==1) data->setFitDataWeighting(QFImFCSFitEvaluation::StdDevWeighting);
        else if (cmbWeights->currentIndex()==2) data->setFitDataWeighting(QFImFCSFitEvaluation::RunErrorWeighting);
        else data->setFitDataWeighting(QFImFCSFitEvaluation::EqualWeighting);
    }
    displayModel(true);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::algorithmChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    current->getHighlightedRecord()->setQFProperty("algorithm", cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString(), false, false);
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    QString alg=cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString();
    data->setFitAlgorithm(alg);
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (!dataEventsEnabled) return;
    if (sender==pltData) {
        pltResiduals->setX(newxmin, newxmax);
    }
}

void QFImFCSFitEvaluationEditor::plotMouseMove(double x, double y) {
    labMousePosition->setTextFormat(Qt::RichText);
    labMousePosition->setText(tr("cursor: (%1, %2)").arg(floattohtmlstr(x).c_str()).arg(floattohtmlstr(y).c_str()));
}





















void QFImFCSFitEvaluationEditor::fitCurrent() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (!eval || !dataImg) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;

    if (data->getCorrelationN()>0) {
        falg->setReporter(dlgFitProgressReporter);
        QString runname=tr("average");
        if (eval->getCurrentIndex()>=0) runname=QString("[%1, %2]").arg(dataImg->runToX(eval->getCurrentIndex())).arg(dataImg->runToY(eval->getCurrentIndex()));
        dlgFitProgress->reportSuperStatus(tr("fit '%1', pixel %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));
        dlgFitProgress->reportStatus("");
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setSuperProgressMax(100);
        dlgFitProgress->setProgress(0);
        dlgFitProgress->setSuperProgress(0);
        dlgFitProgress->setAllowCancel(true);
        dlgFitProgress->display();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        eval->doFit(record, eval->getCurrentIndex(), getUserMin(record, eval->getCurrentIndex(), datacut->get_userMin()), getUserMax(record, eval->getCurrentIndex(), datacut->get_userMax()), dlgFitProgressReporter);
        record->enableEmitResultsChanged(true);

        QTime tim;
        tim.start();

        //qDebug()<<"++++++ updating fit interface";
        dlgFitProgress->reportSuperStatus(tr("fit done ... updating user interface\n"));
        dlgFitProgress->reportStatus("");
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setSuperProgressMax(100);
        //qDebug()<<"++++++ before displayModel "<<tim.elapsed();
        displayModel(false);
        //qDebug()<<"++++++ before replotData "<<tim.elapsed();
        replotData();
        QApplication::restoreOverrideCursor();
        //qDebug()<<"++++++ before dlgFitProgress->done() "<<tim.elapsed();
        dlgFitProgress->done();
        falg->setReporter(NULL);
        //qDebug()<<"++++++ before processing events "<<tim.elapsed();
        QApplication::processEvents();
        //qDebug()<<"++++++ before emitting results changed "<<tim.elapsed();
        current->emitResultsChanged();
        //qDebug()<<"++++++ done "<<tim.elapsed();
    }
}

void QFImFCSFitEvaluationEditor::fitRunsCurrent() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (!eval || !dataImg) return;
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
            if (run>=0) runname=QString("[%1, %2]").arg(dataImg->runToX(run)).arg(dataImg->runToY(run));

            dlgFitProgress->reportSuperStatus(tr("fit '%1', pixel %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));

            //doFit(record, run);
            eval->doFit(record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()), dlgFitProgressReporter);

            dlgFitProgress->incSuperProgress();
            falg->setReporter(NULL);
            if (dlgFitProgress->isCanceled()) break;
        }
    }
    record->enableEmitResultsChanged(true);

    dlgFitProgress->reportSuperStatus(tr("fit done ... updating user interface\n"));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(100);
    QApplication::processEvents();

    current->emitResultsChanged();
    displayModel(false);
    replotData();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
}


void QFImFCSFitEvaluationEditor::fitAll() {
    if (!current) return;
    if (!cmbModel) return;
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(record);
    if (!eval || !dataImg) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!falg)) return;

    int run=eval->getCurrentIndex();
    QString runname=tr("average");
    if (run>=0) runname=QString("[%1, %2]").arg(dataImg->runToX(run)).arg(dataImg->runToY(run));

    dlgFitProgress->reportSuperStatus(tr("fit all files and current pixel (%3) therein<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()).arg(runname));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(10);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgFitProgress->setAllowCancel(true);
    dlgFitProgress->display();


    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();

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

                //doFit(record, run);
                eval->doFit(record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()), dlgFitProgressReporter);

                falg->setReporter(NULL);
                if (dlgFitProgress->isCanceled()) break;
            }
            dlgFitProgress->incSuperProgress();
            record->enableEmitResultsChanged(true);
        }
    }

    dlgFitProgress->reportSuperStatus(tr("fit done ... updating user interface\n"));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(100);
    QApplication::processEvents();

    current->emitResultsChanged();

    displayModel(false);
    replotData();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
}

void QFImFCSFitEvaluationEditor::fitRunsAll() {
    if (!current) return;
    if (!cmbModel) return;
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!falg)) return;

    dlgFitProgress->reportSuperStatus(tr("fit all files and all pixels therein<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(10);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    dlgFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgFitProgress->display();


    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();

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
            record->setResultsInitSize(qMax(1000, data->getCorrelationRuns()));
            record->setEvaluationIDMetadataInitSize(1000);
            int runmax=data->getCorrelationRuns();
            if (runmax==1) runmax=0;
            for (int run=-1; run<runmax; run++) {
                if (data->getCorrelationN()>0) {
                    falg->setReporter(dlgFitProgressReporter);
                    QString runname=tr("average");
                    if (run>=0) runname=QString::number(run);
                    dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));

                    //doFit(record, run);
                    eval->doFit(record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()), dlgFitProgressReporter);

                    dlgFitProgress->incSuperProgress();
                    falg->setReporter(NULL);
                    if (dlgFitProgress->isCanceled()) break;
                }
            }
            record->enableEmitResultsChanged(true);
        }
    }

    dlgFitProgress->reportSuperStatus(tr("fit done ... updating user interface\n"));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(100);
    QApplication::processEvents();

    current->emitResultsChanged();

    displayModel(false);
    replotData();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
}




int QFImFCSFitEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index) {
    return getUserMin(rec, index, datacut->get_userMin());
}

int QFImFCSFitEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index) {
    return getUserMax(rec, index, datacut->get_userMax());
}

int QFImFCSFitEvaluationEditor::getUserMin() {
    return getUserMin(datacut->get_userMin());
}

int QFImFCSFitEvaluationEditor::getUserMax() {
    return getUserMax(datacut->get_userMax());
}
























/////////////////////////////////////////////////////////////////////
// REPORT GENERATION
/////////////////////////////////////////////////////////////////////

void QFImFCSFitEvaluationEditor::createReportDoc(QTextDocument* document) {
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
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (!eval) return;
    if (!fcs) return;
    if (!dataImg) return;

    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* algorithm=eval->getFitAlgorithm();
    //int run=eval->getCurrentIndex();
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
    cursor.insertText(tr("FCS Fit Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(3, 4, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("file:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(0, 2).firstCursorPosition().insertText(tr("pixel:"), fTextBold);
    QString runname=tr("average");
    int run=eval->getCurrentIndex();
    if (run>=0) runname=QString("[%1, %2]").arg(dataImg->runToX(run)).arg(dataImg->runToY(run));

    table->cellAt(0, 3).firstCursorPosition().insertText(QString("%1").arg(runname), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("fit algorithm:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(algorithm->name(), fText);
    table->cellAt(1, 2).firstCursorPosition().insertText(tr("data range:"), fTextBold);
    table->cellAt(1, 3).firstCursorPosition().insertText(tr("%1 ... %2 / %3 ... %4").arg(datacut->get_userMin()).arg(datacut->get_userMax()).arg(datacut->get_min()).arg(datacut->get_max()), fText);
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
    table = cursor.insertTable(ceil((double)fitParamCount/2.0)+1, 12, tableFormat2);
    QTextCursor tableCursor;
    QApplication::processEvents();

    tableCursor=table->cellAt(0, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 5).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();

    tableCursor=table->cellAt(0, 6).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 7).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 11).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();


    int rowStart=1;
    int colStart=0;
    for (int i=0; i<ffunc->paramCount(); i++) {
        QApplication::processEvents();
        QString id=ffunc->getParameterID(i);
        double error=roundError(eval->getFitError(id),2);
        double value=roundWithError(eval->getFitValue(id), error, 2);
        QString value_string=floattohtmlstr(value, 5, true).c_str();
        bool fix=eval->getFitFix(id);
        QFFitFunction::ParameterDescription d=ffunc->getDescription(id);
        QString range=QString("%1...%2").arg(QString(floattohtmlstr(d.minValue, 5, true).c_str())).arg(QString(floattohtmlstr(d.maxValue, 5, true).c_str()));
        if ((d.type==QFFitFunction::IntCombo)&&((int)value>=0)&&((int)value<d.comboItems.size())) {
            value_string="<i>"+d.comboItems[(int)value]+"</i>";
        }
        if (ffunc->isParameterVisible(i, params)) {
            QString err="";
            if (d.displayError!=QFFitFunction::NoError) {
                err=QString("&plusmn;&nbsp;%1").arg(QString(floattohtmlstr(error, 5, true).c_str()));
            }

            tableCursor=table->cellAt(rowStart, colStart).firstCursorPosition();
            tableCursor.setBlockFormat(bfRightNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(d.label));
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

    cursor.setBlockFormat(bfLeft);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(); cursor.insertBlock();
    QString htmlBot=fitStatisticsReport;
    htmlBot.replace("width=\"95%\"", "");
    cursor.insertFragment(QTextDocumentFragment::fromHtml(htmlBot));

}
