#include "qffcsbyindexandmodelevaluationeditor.h"
#include "qfusesresultsbyindexevaluation.h"
#include "qfusesresultsbyindexandmodelevaluation.h"


QFFCSByIndexAndModelEvaluationEditor::QFFCSByIndexAndModelEvaluationEditor(QFPluginServices *services, QFEvaluationPropertyEditor *propEditor, QWidget *parent, const QString& iniPrefix) :
    QFUsesResultsByIndexEvaluationEditor(services, propEditor, parent, iniPrefix)
{
    dataEventsEnabled=false;
    cmbModel=NULL;

    currentSaveDirectory="";

    createWidgets();

    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(NULL);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);

    // connect widgets
    //connect(ui->actEvaluateAll, SIGNAL(triggered()), this, SLOT(evaluateAll()));
    //connect(ui->actnEvaluateCurrent, SIGNAL(triggered()), this, SLOT(evaluateCurrent()));

    dataEventsEnabled=true;
}

QFFCSByIndexAndModelEvaluationEditor::~QFFCSByIndexAndModelEvaluationEditor()
{
    delete dlgEvaluationProgress;
}





void QFFCSByIndexAndModelEvaluationEditor::readSettings() {
    QFUsesResultsByIndexEvaluationEditor::readSettings();
    if (cmbModel && settings) {
        pltData->loadSettings(*settings->getQSettings(), m_iniPrefix+"pltdata/");
        pltResiduals->loadSettings(*settings->getQSettings(), m_iniPrefix+"pltresiduals/");
        pltResidualHistogram->loadSettings(*settings->getQSettings(), m_iniPrefix+"pltresidualhistogram/");
        pltResidualCorrelation->loadSettings(*settings->getQSettings(), m_iniPrefix+"pltresidualcorrelation/");
        loadSplitter(*(settings->getQSettings()), splitMorePLot, m_iniPrefix+"splitter_more_plot");
        loadSplitter(*(settings->getQSettings()), splitPlot, m_iniPrefix+"splitter_plot");
        loadSplitter(*(settings->getQSettings()), splitPlots, m_iniPrefix+"splitter_plots");
        loadSplitter(*(settings->getQSettings()), splitModel, m_iniPrefix+"splitter_model");
        loadSplitter(*(settings->getQSettings()), splitFitStatistics, m_iniPrefix+"splitter_fitstatistics");
        spinResidualHistogramBins->setValue(settings->getQSettings()->value(m_iniPrefix+"residual_histogram_bins", 25).toInt());
        tabResidulas->setCurrentIndex(settings->getQSettings()->value(m_iniPrefix+"residual_toolbox_current", 0).toInt());

    }

}

void QFFCSByIndexAndModelEvaluationEditor::writeSettings() {
    QFUsesResultsByIndexEvaluationEditor::writeSettings();
    if (cmbModel) {
        saveSplitter(*(settings->getQSettings()), splitMorePLot, m_iniPrefix+"splitter_more_plot");
        saveSplitter(*(settings->getQSettings()), splitPlot, m_iniPrefix+"splitter_plot");
        saveSplitter(*(settings->getQSettings()), splitPlots, m_iniPrefix+"splitter_plots");
        saveSplitter(*(settings->getQSettings()), splitModel, m_iniPrefix+"splitter_model");
        saveSplitter(*(settings->getQSettings()), splitFitStatistics, m_iniPrefix+"splitter_fitstatistics");
        settings->getQSettings()->setValue(m_iniPrefix+"residual_histogram_bins", spinResidualHistogramBins->value());
        settings->getQSettings()->setValue(m_iniPrefix+"residual_toolbox_current", tabResidulas->currentIndex());
    }
}

void QFFCSByIndexAndModelEvaluationEditor::connectDefaultWidgets(QFEvaluationItem *current, QFEvaluationItem *old)
{
    QFUsesResultsByIndexEvaluationEditor::connectDefaultWidgets(current, old);
}

void QFFCSByIndexAndModelEvaluationEditor::createWidgets() {

    QVBoxLayout* mainLayout=new QVBoxLayout();
    mainLayout->setContentsMargins(5,5,5,5);
    setLayout(mainLayout);


    QWidget* topWidget=new QWidget(this);
    QVBoxLayout* vbl=new QVBoxLayout();
    vbl->setContentsMargins(0,0,0,0);
    topWidget->setLayout(vbl);

    QFormLayout* fl;//=new QFormLayout();
    QLabel* l;
    QHBoxLayout* hbl;
    hblModel=new QHBoxLayout();
    hblModel->setContentsMargins(0,0,0,0);
    cmbModel=new QComboBox(this);
    l=new QLabel(tr("&Physical Model:"), this);
    l->setBuddy(cmbModel);
    hblModel->addWidget(l);
    cmbModel->setEditable(false);
    cmbModel->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    hblModel->addWidget(cmbModel);
    hblModel->addStretch();


    //fl->addRow(tr("&Weight Model:"), cmbWeights);
    //vbl->addLayout(fl);
    vbl->addLayout(hblModel);


    QFont boldfont=font();
    boldfont.setBold(true);

    toolbar=new QToolBar("toolbar_fcs_fit", this);
    vbl->addWidget(toolbar);
    QLabel* lPS=new QLabel(tr("   &Plots: "), toolbar);
    lPS->setFont(boldfont);
    cmbPlotStyle=new QComboBox(toolbar);
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_points.png"), tr("points"));
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_lines.png"), tr("lines"));
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_pointslines.png"), tr("lines + points"));
    lPS->setBuddy(cmbPlotStyle);
    QLabel* lES=new QLabel(tr("   &Errors: "), toolbar);
    lES->setFont(boldfont);
    cmbErrorStyle=new QComboBox(toolbar);
    cmbErrorStyle->addItem(QIcon(":/libqf3widgets/plot_enone.png"), tr("none"));
    cmbErrorStyle->addItem(QIcon(":/libqf3widgets/plot_elines.png"), tr("lines"));
    cmbErrorStyle->addItem(QIcon(":/libqf3widgets/plot_ebars.png"), tr("bars"));
    cmbErrorStyle->addItem(QIcon(":/libqf3widgets/plot_elinesbars.png"), tr("bars + lines"));
    lES->setBuddy(cmbErrorStyle);

    QLabel* lRS=new QLabel(tr("   &Residuals: "), toolbar);
    lRS->setFont(boldfont);
    cmbResidualStyle=new QComboBox(toolbar);
    cmbResidualStyle->addItem(QIcon(":/libqf3widgets/plot_points.png"), tr("points"));
    cmbResidualStyle->addItem(QIcon(":/libqf3widgets/plot_lines.png"), tr("lines"));
    cmbResidualStyle->addItem(QIcon(":/libqf3widgets/plot_pointslines.png"), tr("lines + points"));
    lRS->setBuddy(cmbResidualStyle);

    chkWeightedResiduals=new QCheckBox(tr("&weighted  "), toolbar);

    QWidget* sp1=new QWidget(this);
    sp1->setMinimumWidth(10);
    sp1->setMaximumWidth(10);
    chkGrid=new QCheckBox(tr("&Grids   "), toolbar);
    chkKeyVisible=new QCheckBox(tr("&Keys   "), toolbar);
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

    splitPlots=new QVisibleHandleSplitter(this),
    splitPlots->setOrientation(Qt::Vertical);
    splitPlot=new QVisibleHandleSplitter(this),
    splitPlot->setOrientation(Qt::Vertical);
    splitMorePLot=new QVisibleHandleSplitter(this),
    splitMorePLot->setOrientation(Qt::Horizontal);

    splitModel->addWidget(splitPlots);
    splitPlots->addWidget(splitMorePLot);
    splitMorePLot->addWidget(splitPlot);

    pltData=new QFPlotter(true, this);
    pltData->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltData->resize(300,200);
    plotLayout=new QVBoxLayout();
    wPlot=new QWidget(this);
    wPlot->setLayout(plotLayout);
    plotLayout->setContentsMargins(0,0,0,0);
    plotLayout->addWidget(pltData);



    splitPlot->addWidget(wPlot);


    QWidget* widgetResiduals=new QWidget(this);
    vbl=new QVBoxLayout();
    vbl->setContentsMargins(0,0,0,0);
    vbl->setSpacing(1);
    widgetResiduals->setLayout(vbl);

    pltResiduals=new QFPlotter(true, this);
    pltResiduals->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
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
    //pltData->get_plotter()->set_keyXMargin(2);
    //pltData->get_plotter()->set_keyYMargin(2);
    pltResiduals->get_plotter()->set_keyFontSize(9);
    //pltResiduals->get_plotter()->set_keyXMargin(2);
    //pltResiduals->get_plotter()->set_keyYMargin(2);
    pltResiduals->useExternalDatastore(pltData->getDatastore());
    pltResiduals->setMinimumHeight(75);


    datacut=new DataCutSliders(this);
    vbl->addWidget(datacut,0);
    splitPlot->addWidget(widgetResiduals);

    splitFitStatistics=new QVisibleHandleSplitter(this);
    splitPlots->addWidget(splitFitStatistics);
    tabResidulas=new QTabWidget(this);
    tabResidulas->setTabPosition(QTabWidget::North);
    tabResidulas->setTabShape(QTabWidget::Triangular);
    tabResidulas->setDocumentMode(true);
    splitFitStatistics->addWidget(tabResidulas);
    pltResidualHistogram=new QFPlotter(true, this);
    pltResidualHistogram->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
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


    pltResidualCorrelation=new QFPlotter(true, this);
    pltResidualCorrelation->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
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

    layResidualAnalysis=new QFormLayout();
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
    QVBoxLayout* layModel=new QVBoxLayout();
    modelWidget->setLayout(layModel);

    flAlgorithmParams=new QFormLayout();
    flAlgorithmParams->setContentsMargins(9,0,0,0);
    flAlgorithmParams->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
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
    flAlgorithmParams->addRow(tr("run: "), hblRun);
    layModel->addLayout(flAlgorithmParams, 0);



    labFitParameters=new QLabel(this);
    layModel->addWidget(labFitParameters);
    widFitParams=new QFSimpleFitParametersWidget(NULL, this);
    scrollParameters=new JKVerticalScrollArea(this);
    scrollParameters->setWidget(widFitParams);
    scrollParameters->setWidgetResizable(true);
    layModel->addWidget(scrollParameters, 1);

    QGridLayout* layBtn=new QGridLayout();
    layBtn->setContentsMargins(0,0,0,0);
    btnFitCurrent=createButtonAndActionShowText(actFitCurrent, QIcon(":/fcs_maxent/fit_fitcurrent.png"), tr("&Fit Current"), this);
    actFitCurrent->setToolTip(tr("perform a fit for the currently displayed file and run"));
    layBtn->addWidget(btnFitCurrent, 0, 0);
    btnFitRunsCurrent=createButtonAndActionShowText(actFitRunsCurrent, QIcon(":/fcs_maxent/fit_fitallruns.png"), tr("Fit All &Runs"), this);
    actFitRunsCurrent->setToolTip(tr("perform a fit for all runs in the currently selected file "));
    layBtn->addWidget(btnFitRunsCurrent, 0, 1);
    btnFitAll=createButtonAndActionShowText(actFitAll, QIcon(":/fcs_maxent/fit_fitcurrentrunallfiles.png"), tr("Fit All &Files (Current Run)"), this);
    actFitAll->setToolTip(tr("perform a fit for all files, but fit in each file only the currently displayed run"));
    layBtn->addWidget(btnFitAll, 1, 0);
    btnFitRunsAll=createButtonAndActionShowText(actFitRunsAll, QIcon(":/fcs_maxent/fit_fitall.png"), tr("Fit &Everything"), this);
    actFitRunsAll->setToolTip(tr("perform a fit for all runs in all files"));
    layBtn->addWidget(btnFitRunsAll, 1, 1);
    btnResetCurrent=createButtonAndActionShowText(actResetCurrent, tr("&Reset Current"), this);
    actResetCurrent->setToolTip(tr("reset the currently displayed file (and run) to the initial parameters\nThis deletes all fit results stored for the current file."));
    layBtn->addWidget(btnResetCurrent, 2, 0);
    btnResetAll=createButtonAndActionShowText(actResetAll, tr("&Reset All"), this);
    actResetAll->setToolTip(tr("reset all loaded files to the initial parameters.\nThis deletes all fit results stored for all files (and all runs therein)."));
    layBtn->addWidget(btnResetAll, 3, 1);

    btnResetAllRuns=createButtonAndActionShowText(actResetAllRuns, tr("&Reset All Runs"), this);
    actResetAllRuns->setToolTip(tr("reset all runs to the initial parameters in the current file.\nThis deletes all fit results stored for all runs in the current file."));
    layBtn->addWidget(btnResetAllRuns, 2, 1);

    btnCopyToInitial=createButtonAndActionShowText(actCopyToInitial, tr("Copy to &Initial"), this);
    actCopyToInitial->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters,\nso they are used by files/runs that were not fit yet."));
    layBtn->addWidget(btnCopyToInitial, 4, 0);
    btnCopyToAllRuns=createButtonAndActionShowText(actCopyToAllRuns, tr("&Copy to All Runs"), this);
    actCopyToAllRuns->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters\n and also to all runs in the current file."));
    layBtn->addWidget(btnCopyToAllRuns, 4, 1);
    btnCopyToAll=createButtonAndActionShowText(actCopyToAll, tr("&Copy to All"), this);
    actCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set\n of initial parameters and also to all files."));
    layBtn->addWidget(btnCopyToAll, 5, 1);
    btnCopyToAllCurrentRun=createButtonAndActionShowText(actCopyToAllCurrentRun, tr("&Copy to All (Current Run)"), this);
    actCopyToAllCurrentRun->setToolTip(tr("copy the currently displayed fit parameters to the set of\n initial parameters and also to all files, but only to the current run therein."));
    layBtn->addWidget(btnCopyToAllCurrentRun, 5, 0);

    layModel->addLayout(layBtn);



    labFitResult=new QLabel(this);
    labFitResult->setWordWrap(true);
    layModel->addWidget(labFitResult);

    splitModel->addWidget(modelWidget);




    splitPlot->setCollapsible(0, false);
    splitPlot->setCollapsible(1, false);
    splitPlot->setStretchFactor(0,4);
    splitPlot->setStretchFactor(1,1);
    splitMorePLot->setCollapsible(0, false);

    splitModel->setCollapsible(0, false);
    splitModel->setCollapsible(1, false);
    splitModel->setStretchFactor(0,4);
    splitModel->setStretchFactor(1,1);

    toolbar->addAction(actSaveReport);
    toolbar->addAction(actPrintReport);
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actSavePlot()); pltData->get_plotter()->get_actSavePlot()->setIcon(QIcon(":/fcs_maxent/plot_saveplot.png"));
    toolbar->addAction(pltData->get_plotter()->get_actSaveData()); pltData->get_plotter()->get_actSaveData()->setIcon(QIcon(":/fcs_maxent/plot_savedata.png"));
    toolbar->addAction(pltData->get_plotter()->get_actPrint()); pltData->get_plotter()->get_actPrint()->setIcon(QIcon(":/fcs_maxent/plot_print.png"));
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actCopyPixelImage());
    toolbar->addAction(pltData->get_plotter()->get_actCopyData());
    toolbar->addAction(pltData->get_plotter()->get_actCopyMatlab());
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_plotter()->get_actZoomAll()); pltData->get_plotter()->get_actZoomAll()->setIcon(QIcon(":/fcs_maxent/plot_zoomall.png"));
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
    toolbar->addWidget(chkKeyVisible);
    toolbar->addWidget(chkXLogScale);
    toolbar->addSeparator();
    toolbar->addWidget(sp2);
    toolbar->addWidget(labMousePosition);

    connect(pltData, SIGNAL(zoomChangedLocally(double, double, double, double, JKQtPlotter*)), this, SLOT(zoomChangedLocally(double, double, double, double, JKQtPlotter*)));
    connect(pltData, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResiduals, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualHistogram, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualCorrelation, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));

    connect(actFitCurrent, SIGNAL(triggered()), this, SLOT(fitCurrent()));
    connect(actFitAll, SIGNAL(triggered()), this, SLOT(fitRunsAll()));
    connect(actFitRunsAll, SIGNAL(triggered()), this, SLOT(fitAll()));
    connect(actFitRunsCurrent, SIGNAL(triggered()), this, SLOT(fitRunsCurrent()));
    connect(actResetCurrent, SIGNAL(triggered()), this, SLOT(resetCurrent()));
    connect(actResetAll, SIGNAL(triggered()), this, SLOT(resetAll()));
    connect(actCopyToAll, SIGNAL(triggered()), this, SLOT(copyToAll()));
    connect(actResetAllRuns, SIGNAL(triggered()), this, SLOT(resetAllRuns()));
    connect(actCopyToAllRuns, SIGNAL(triggered()), this, SLOT(copyToAllRuns()));
    connect(actCopyToInitial, SIGNAL(triggered()), this, SLOT(copyToInitial()));
    connect(actCopyToAllCurrentRun, SIGNAL(triggered()), this, SLOT(copyToAllCurrentRun()));

    connect(datacut, SIGNAL(copyUserMinToAll(int)), this, SLOT(copyUserMinToAll(int)));
    connect(datacut, SIGNAL(copyUserMaxToAll(int)), this, SLOT(copyUserMaxToAll(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAll(int,int)), this, SLOT(copyUserMinMaxToAll(int,int)));
    connect(datacut, SIGNAL(copyUserMinToAllRuns(int)), this, SLOT(copyUserMinToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMaxToAllRuns(int)), this, SLOT(copyUserMaxToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAllRuns(int,int)), this, SLOT(copyUserMinMaxToAllRuns(int,int)));


    menuResults=propertyEditor->addMenu("&Results", 0);
    menuResults->addAction(pltData->get_plotter()->get_actSaveData());
    menuResults->addAction(pltData->get_plotter()->get_actSavePlot());
    menuResults->addAction(pltData->get_plotter()->get_actPrint());
    menuResults->addSeparator();
    menuResults->addAction(pltData->get_plotter()->get_actCopyData());
    menuResults->addAction(pltData->get_plotter()->get_actCopyMatlab());
    menuResults->addAction(pltData->get_plotter()->get_actCopyPixelImage());
    pltData->get_plotter()->get_actCopyPixelImage()->setText("copy correlation plot");
    pltData->get_plotter()->get_actPrint()->setText("print correlation plot");
    pltData->get_plotter()->get_actSavePlot()->setText("save correlation plot");
    menuResults->addSeparator();
    menuResults->addAction(actSaveReport);
    menuResults->addAction(actPrintReport);

    menuParameters=propertyEditor->addMenu("&Parameters", 0);
    menuParameters->addAction(actResetCurrent);
    menuParameters->addAction(actResetAll);
    menuParameters->addAction(actResetAllRuns);
    menuParameters->addSeparator();
    menuParameters->addAction(actCopyToAll);
    menuParameters->addAction(actCopyToAllRuns);
    menuParameters->addAction(actCopyToInitial);
    menuParameters->addAction(actCopyToAllCurrentRun);

    //menuParameters->addAction();


    menuFit=propertyEditor->addMenu("&Fit", 0);
    menuFit->addAction(actFitCurrent);
    menuFit->addAction(actFitRunsCurrent);
    menuFit->addAction(actFitRunsAll);
    menuFit->addAction(actFitAll);


}

int QFFCSByIndexAndModelEvaluationEditor::getUserMinAbsMax(QFRawDataRecord *rec, int index) {
    QFRDRFCSDataInterface* dintf=qobject_cast<QFRDRFCSDataInterface*>(rec);
    if (dintf) return dintf->getCorrelationN()-1;
    return 0;
}



void QFFCSByIndexAndModelEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {

    connectDefaultWidgets(current, old);

    // called when this widget should be connected to a new QFEvaluationItem

    QFUsesResultsByIndexAndModelEvaluation* item=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    QFSimpleFitParameterEnumeratorInterface* itemp=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    QFUsesResultsByIndexAndModelEvaluation* item_old=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(old);

    if (old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        disconnect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        disconnect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(runChanged(int)));
        disconnect(cmbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
        disconnect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotStyleChanged(int)));
        disconnect(cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(errorStyleChanged(int)));
        disconnect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(chkGridToggled(bool)));
        disconnect(chkKeyVisible, SIGNAL(toggled(bool)), this, SLOT(chkKeyVisibleToggled(bool)));
        disconnect(chkXLogScale, SIGNAL(toggled(bool)), this, SLOT(chkXLogScaleToggled(bool)));
        disconnect(cmbResidualStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(residualStyleChanged(int)));
        disconnect(chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(chkWeightedResidualsToggled(bool)));
        disconnect(spinResidualHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(residualHistogramBinsChanged(int)));
        disconnect(widFitParams, SIGNAL(parameterChanged(QString,double)), this, SLOT(fitParamChanged()));


        spinRun->setMaximum(-1);
        widFitParams->setParameterInterface(NULL);
    }



    if (item) {
        dataEventsEnabled=false;

        chkXLogScale->setChecked(current->getProperty("plot_taulog", true).toBool());
        chkGrid->setChecked(current->getProperty("plot_grid", true).toBool());
        chkKeyVisible->setChecked(current->getProperty("show_key", true).toBool());
        cmbPlotStyle->setCurrentIndex(current->getProperty("plot_style", 1).toInt());
        cmbErrorStyle->setCurrentIndex(current->getProperty("plot_errorstyle", 0).toInt());
        cmbResidualStyle->setCurrentIndex(current->getProperty("plot_residualsstyle", 0).toInt());
        chkWeightedResiduals->setChecked(current->getProperty("weighted_residuals", false).toBool());
        spinResidualHistogramBins->setValue(current->getProperty("plot_residualshistogrambins", 25).toInt());
        widFitParams->setParameterInterface(item);

        cmbModel->clear();


        for (int i=0; i<item->getModelCount(); i++) {
            cmbModel->addItem(item->getModelName(i));
        }

        /*
        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        */

        cmbModel->setCurrentIndex(item->getCurrentModel());
//        qDebug() << "THE CURRENT MODEL IS: ";
//        qDebug() << item->getCurrentModel();
        /* connect widgets and fill with data from item here */
        connect(item, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));

        connect(current, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        connect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        connect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(runChanged(int)));
        connect(cmbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
        connect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotStyleChanged(int)));
        connect(cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(errorStyleChanged(int)));
        connect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(chkGridToggled(bool)));
        connect(chkKeyVisible, SIGNAL(toggled(bool)), this, SLOT(chkKeyVisibleToggled(bool)));
        connect(chkXLogScale, SIGNAL(toggled(bool)), this, SLOT(chkXLogScaleToggled(bool)));
        connect(cmbResidualStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(residualStyleChanged(int)));
        connect(chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(chkWeightedResidualsToggled(bool)));
        connect(spinResidualHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(residualHistogramBinsChanged(int)));
        connect(widFitParams, SIGNAL(parameterChanged(QString,double)), this, SLOT(fitParamChanged()));


        /////
        //connect(cmbModel, SIGNAL(activated(int)),this,SLOT(modelChanged(int)));
        /////


        dataEventsEnabled=true;
    }


    displayData();
    displayParameters();
}

void QFFCSByIndexAndModelEvaluationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayData();
    displayParameters();
}


void QFFCSByIndexAndModelEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog

    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    //QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData())); // redisplay data, if data changed

        dataEventsEnabled=false;

        datacut->disableSliderSignals();
        datacut->set_min(0);
        datacut->set_max(data->getCorrelationN()-1);
        QString run="avg";
        if (eval->getCurrentIndex()>-1) run=QString::number(eval->getCurrentIndex());
        datacut->set_userMin(getUserMin(currentRecord, eval->getCurrentIndex(), 0));
        datacut->set_userMax(getUserMax(currentRecord, eval->getCurrentIndex(), data->getCorrelationN()-1));
        datacut->enableSliderSignals();
        spinRun->setMaximum(data->getCorrelationRuns()-1);
        if (data->getCorrelationRuns()==1) spinRun->setMaximum(-1);
        spinRun->setValue(eval->getCurrentIndex());
        if (data->getCorrelationRuns()>1) spinRun->setSuffix(QString(" / 0..%1").arg(data->getCorrelationRuns()-1));
        cmbModel->setCurrentIndex(eval->getCurrentModel());


        dataEventsEnabled=true;
    }

    // ensure that data of new highlighted record is displayed
    displayParameters();
    displayData();
}

void QFFCSByIndexAndModelEvaluationEditor::resetCurrent() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    eval->resetAllFitResults(record, eval->getCurrentIndex(), eval->getCurrentModel());
    // ensure that data of new highlighted record is displayed
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::resetAll() {
    if (!current) return;
    //QFRawDataRecord* record=current->getHighlightedRecord();
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QPointer<QFRawDataRecord> > records=eval->getApplicableRecords();

    bool rc=eval->get_doEmitResultsChanged();
    eval->set_doEmitResultsChanged(false);
    foreach (QFRawDataRecord* record, records) {
        bool rec=record->isEmitResultsChangedEnabled();
        record->disableEmitResultsChanged();
        for (int idx=eval->getIndexMin(record); idx<=eval->getIndexMax(record); idx++) {
            eval->resetAllFitResults(record, idx, eval->getCurrentModel());
        }
        if (rec)record->enableEmitResultsChanged();
    }
    eval->set_doEmitResultsChanged(rc);
    if (rc) eval->emitResultsChanged();
    // ensure that data of new highlighted record is displayed
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::resetAllRuns() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool rc=eval->get_doEmitResultsChanged();
    eval->set_doEmitResultsChanged(false);
    bool rec=record->isEmitResultsChangedEnabled();
    record->disableEmitResultsChanged();
    for (int idx=eval->getIndexMin(record); idx<=eval->getIndexMax(record); idx++) {
        eval->resetAllFitResults(record, idx, eval->getCurrentModel());
    }
    if (rec)record->enableEmitResultsChanged();
    eval->set_doEmitResultsChanged(rc);
    if (rc) eval->emitResultsChanged(record);
    // ensure that data of new highlighted record is displayed
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFFCSByIndexAndModelEvaluationEditor::displayParameters() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    QFSimpleFitParameterEnumeratorInterface* peval=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    QStringList ids, labels;
    for (int i=0; i<peval->getParameterCount(eval->getCurrentModel()); i++) {
        ids.append(peval->getParameterID(eval->getCurrentModel(), i));
        labels.append(peval->getParameterName(eval->getCurrentModel(), i, true));
    }
    widFitParams->setParameters(ids, labels);
    widFitParams->updateWidgetValues();

    if (eval->hasResults(record)) {
        labFitParameters->setText(tr("<b>Local Parameters:</b>"));
    } else {
        labFitParameters->setText(tr("<b>Global/Initial Parameters:</b>"));
    }
}


void QFFCSByIndexAndModelEvaluationEditor::plotMouseMove(double x, double y) {
    labMousePosition->setTextFormat(Qt::RichText);
    labMousePosition->setText(tr("cursor: (%1, %2)").arg(floattohtmlstr(x).c_str()).arg(floattohtmlstr(y).c_str()));
}

void QFFCSByIndexAndModelEvaluationEditor::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (!dataEventsEnabled) return;
    if (sender==pltData) {
        pltResiduals->setX(newxmin, newxmax);
        pltResiduals->update_plot();
    }
}

void QFFCSByIndexAndModelEvaluationEditor::modelChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    data->setCurrentModel(model);
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFFCSByIndexAndModelEvaluationEditor::runChanged(int run) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTime t;

    t.start();
    QFRawDataRecord* currentRecord=current->getHighlightedRecord();
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);
    labRun->setText(QString("  (%1)").arg(fcs->getCorrelationRunName(run)));

    data->setCurrentIndex(run);


    datacut->disableSliderSignals();
    datacut->set_min(0);
    datacut->set_max(fcs->getCorrelationN());
    QString runn="avg";
    if (data->getCurrentIndex()>-1) runn=QString::number(data->getCurrentIndex());
    datacut->set_userMin(getUserMin(currentRecord, data->getCurrentIndex(), 0));
    datacut->set_userMax(getUserMax(currentRecord, data->getCurrentIndex(), fcs->getCorrelationN()));
    datacut->enableSliderSignals();

    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return;
    if (!current->getHighlightedRecord()) return;
    setUserMinMax(userMin, userMax);
    displayData();
}



void QFFCSByIndexAndModelEvaluationEditor::chkXLogScaleToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("plot_taulog", chkXLogScale->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::chkGridToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("plot_grid", chkGrid->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::chkKeyVisibleToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("show_key", chkKeyVisible->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}
void QFFCSByIndexAndModelEvaluationEditor::plotStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("plot_style", cmbPlotStyle->currentIndex(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::errorStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("plot_errorstyle", cmbErrorStyle->currentIndex(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::residualStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("plot_residualsstyle", cmbResidualStyle->currentIndex(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::chkWeightedResidualsToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("weighted_residuals", chkWeightedResiduals->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::residualHistogramBinsChanged(int bins) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    current->setQFProperty("plot_residualshistogrambins", spinResidualHistogramBins->value(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::fitParamChanged() {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSByIndexAndModelEvaluationEditor::copyToInitial() {
    if (!current) return;
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    QFSimpleFitParameterEnumeratorInterface* itemp=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    int model=eval->getCurrentModel();
    if (!eval || !itemp) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //bool erc=eval->get_doEmitResultsChanged();
    bool epc=eval->get_doEmitPropertiesChanged();
    //eval->set_doEmitResultsChanged(false);
    eval->set_doEmitPropertiesChanged(false);

    for (int i=0; i<itemp->getParameterCount(model); i++) {
        QString id=itemp->getParameterID(model, i);
        double value=eval->getFitValue(id);
        double error=eval->getFitError(id);
        bool fix=eval->getFitFix(id);
        eval->setInitFitValue(id, value, error, fix);
    }
    copyMoreDataToInitial();


    //eval->set_doEmitResultsChanged(erc);
    eval->set_doEmitPropertiesChanged(epc);
    //if (erc) eval->emitResultsChanged();
    if (epc) eval->emitPropertiesChanged();
    QApplication::restoreOverrideCursor();
}

void QFFCSByIndexAndModelEvaluationEditor::copyToAll() {
    if (!current) return;
    //QFRawDataRecord* record=current->getHighlightedRecord();
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    QFSimpleFitParameterEnumeratorInterface* itemp=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    int model=eval->getCurrentModel();
    if (!eval || !itemp) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QPointer<QFRawDataRecord> > records=eval->getApplicableRecords();

    bool rc=eval->get_doEmitResultsChanged();
    eval->set_doEmitResultsChanged(false);
    foreach (QFRawDataRecord* record, records) {
        bool rec=record->isEmitResultsChangedEnabled();
        record->disableEmitResultsChanged();
        for (int idx=eval->getIndexMin(record); idx<=eval->getIndexMax(record); idx++) {
            //eval->resetAllFitResults(record, idx, eval->getCurrentModel());
            if (eval->hasResults(record, idx, model)) {
                for (int i=0; i<itemp->getParameterCount(model); i++) {
                    QString id=itemp->getParameterID(model, i);
                    double value=eval->getFitValue(id);
                    double error=eval->getFitError(id);
                    bool fix=eval->getFitFix(id);
                    eval->setFitValue(record, idx, model, id, value);
                    eval->setFitFix(record, idx, model, id, fix);
                    eval->setFitError(record, idx, model, id, error);
                }
                copyMoreData(record, idx, model);
            }
        }
        if (rec)record->enableEmitResultsChanged();
    }
    eval->set_doEmitResultsChanged(rc);
    if (rc) eval->emitResultsChanged();
    // ensure that data of new highlighted record is displayed
    QApplication::restoreOverrideCursor();

    if (QMessageBox::question(this, tr("MaxEnt Fit"), tr("Copy the current data also to the initial values?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        copyToInitial();
    }
}

void QFFCSByIndexAndModelEvaluationEditor::copyToAllRuns() {
    if (!current) return;
    //QFRawDataRecord* record=current->getHighlightedRecord();
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    QFSimpleFitParameterEnumeratorInterface* itemp=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    int model=eval->getCurrentModel();
    if (!eval || !itemp) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QList<QPointer<QFRawDataRecord> > records=eval->getApplicableRecords();

    bool rc=eval->get_doEmitResultsChanged();
    eval->set_doEmitResultsChanged(false);
    //foreach (QFRawDataRecord* record, records) {
    QFRawDataRecord* record=eval->getHighlightedRecord();
        bool rec=record->isEmitResultsChangedEnabled();
        record->disableEmitResultsChanged();
        for (int idx=eval->getIndexMin(record); idx<=eval->getIndexMax(record); idx++) {
            //eval->resetAllFitResults(record, idx, eval->getCurrentModel());
            if (eval->hasResults(record, idx, model)) {
                for (int i=0; i<itemp->getParameterCount(model); i++) {
                    QString id=itemp->getParameterID(model, i);
                    double value=eval->getFitValue(id);
                    double error=eval->getFitError(id);
                    bool fix=eval->getFitFix(id);
                    eval->setFitValue(record, idx, model, id, value);
                    eval->setFitFix(record, idx, model, id, fix);
                    eval->setFitError(record, idx, model, id, error);
                }
                copyMoreData(record, idx, model);
            }
        }
        if (rec)record->enableEmitResultsChanged();
    //}
    eval->set_doEmitResultsChanged(rc);
    if (rc) eval->emitResultsChanged();
    // ensure that data of new highlighted record is displayed
    QApplication::restoreOverrideCursor();

    if (QMessageBox::question(this, tr("MaxEnt Fit"), tr("Copy the current data also to the initial values?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        copyToInitial();
    }
}

void QFFCSByIndexAndModelEvaluationEditor::copyToAllCurrentRun() {
    if (!current) return;
    //QFRawDataRecord* record=current->getHighlightedRecord();
    QFUsesResultsByIndexAndModelEvaluation* eval=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    QFSimpleFitParameterEnumeratorInterface* itemp=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    int model=eval->getCurrentModel();
    if (!eval || !itemp) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QPointer<QFRawDataRecord> > records=eval->getApplicableRecords();

    bool rc=eval->get_doEmitResultsChanged();
    eval->set_doEmitResultsChanged(false);
    foreach (QFRawDataRecord* record, records) {
        bool rec=record->isEmitResultsChangedEnabled();
        record->disableEmitResultsChanged();
        //for (int idx=eval->getIndexMin(record); idx<=eval->getIndexMax(record); idx++) {
        int idx=eval->getCurrentIndex();
        if (eval->getIndexMin(record)<=idx && idx<=eval->getIndexMax(record)) {
            //eval->resetAllFitResults(record, idx, eval->getCurrentModel());
            if (eval->hasResults(record, idx, model)) {
                for (int i=0; i<itemp->getParameterCount(model); i++) {
                    QString id=itemp->getParameterID(model, i);
                    double value=eval->getFitValue(id);
                    double error=eval->getFitError(id);
                    bool fix=eval->getFitFix(id);
                    eval->setFitValue(record, idx, model, id, value);
                    eval->setFitFix(record, idx, model, id, fix);
                    eval->setFitError(record, idx, model, id, error);
                }
                copyMoreData(record, idx, model);
            }
        }
        if (rec)record->enableEmitResultsChanged();
    }
    eval->set_doEmitResultsChanged(rc);
    if (rc) eval->emitResultsChanged();
    // ensure that data of new highlighted record is displayed
    QApplication::restoreOverrideCursor();

    if (QMessageBox::question(this, tr("MaxEnt Fit"), tr("Copy the current data also to the initial values?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
        copyToInitial();
    }
}

void QFFCSByIndexAndModelEvaluationEditor::copyMoreData(QFRawDataRecord *record, int index, int model) {
    // may be overwritten by derived classes
}

void QFFCSByIndexAndModelEvaluationEditor::copyMoreDataToInitial() {
    // may be overwritten by derived classes
}
