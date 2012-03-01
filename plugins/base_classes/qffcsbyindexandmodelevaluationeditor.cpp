#include "qffcsbyindexandmodelevaluationeditor.h"
#include "qfusesresultsbyindexevaluation.h"
#include "qfusesresultsbyindexandmodelevaluation.h"


QFFCSByIndexAndModelEvaluationEditor::QFFCSByIndexAndModelEvaluationEditor(QFPluginServices *services, QWidget *parent) :
    QFUsesResultsByIndexEvaluationEditor(services, parent)
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
    //connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    //connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));

    dataEventsEnabled=true;
}

QFFCSByIndexAndModelEvaluationEditor::~QFFCSByIndexAndModelEvaluationEditor()
{
    delete dlgEvaluationProgress;
}

int QFFCSByIndexAndModelEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index, int defaultMin) {
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return defaultMin;
    const QString resultID=data->getEvaluationResultID(index, 0);
    if (!rec) return defaultMin;


    return rec->getProperty(resultID+"_datacut_min", defaultMin).toInt();
}

int QFFCSByIndexAndModelEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index, int defaultMax) {
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return defaultMax;
    const QString resultID=data->getEvaluationResultID(index, 0);

    if (!rec) return defaultMax;

    return rec->getProperty(resultID+"_datacut_max", defaultMax).toInt();
}


int QFFCSByIndexAndModelEvaluationEditor::getUserMin(QFRawDataRecord* rec) {
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return 0;
    return getUserMin(rec, data->getCurrentIndex());
}

int QFFCSByIndexAndModelEvaluationEditor::getUserMax(QFRawDataRecord* rec) {
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return 0;
    return getUserMax(rec, data->getCurrentIndex());
}


int QFFCSByIndexAndModelEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index) {
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return 0;
    return getUserMin(rec, index, 0);
}

int QFFCSByIndexAndModelEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index) {
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return 0;
    QFRDRFCSDataInterface* dintf=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    if (dintf) return getUserMax(rec, index, dintf->getCorrelationN()-1);
    return getUserMax(rec, index, 0);
}

void QFFCSByIndexAndModelEvaluationEditor::setUserMin(int userMin) {
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex(), 0);
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
}

void QFFCSByIndexAndModelEvaluationEditor::setUserMax(int userMax) {
    if (!current) return;
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex(), 0);
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
}

void QFFCSByIndexAndModelEvaluationEditor::setUserMinMax(int userMin, int userMax) {
    if (!current) return;
    QFUsesResultsByIndexAndModelEvaluation* data=qobject_cast<QFUsesResultsByIndexAndModelEvaluation*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=current->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID(data->getCurrentIndex(), 0);
    rdr->disableEmitPropertiesChanged();
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
    rdr->enableEmitPropertiesChanged(true);
}

void QFFCSByIndexAndModelEvaluationEditor::readSettings() {
    QFUsesResultsByIndexEvaluationEditor::readSettings();
    if (cmbModel && settings) {
        pltData->loadSettings(*settings->getQSettings(), "fcsmaxentevaleditor/pltdata/");
        pltResiduals->loadSettings(*settings->getQSettings(), "fcsmaxentevaleditor/pltresiduals/");
        pltResidualHistogram->loadSettings(*settings->getQSettings(), "fcsmaxentevaleditor/pltresidualhistogram/");
        pltResidualCorrelation->loadSettings(*settings->getQSettings(), "fcsmaxentevaleditor/pltresidualcorrelation/");
        loadSplitter(*(settings->getQSettings()), splitPlot, "fcsmaxentevaleditor/splitter_plot");
        loadSplitter(*(settings->getQSettings()), splitModel, "fcsmaxentevaleditor/splitter_model");
        loadSplitter(*(settings->getQSettings()), splitFitStatistics, "fcsmaxentevaleditor/splitter_fitstatistics");
        spinResidualHistogramBins->setValue(settings->getQSettings()->value("fcsmaxentevaleditor/residual_histogram_bins", 25).toInt());
        tabResidulas->setCurrentIndex(settings->getQSettings()->value("fcsmaxentevaleditor/residual_toolbox_current", 0).toInt());

    }

}

void QFFCSByIndexAndModelEvaluationEditor::writeSettings() {
    QFUsesResultsByIndexEvaluationEditor::writeSettings();
    if (cmbModel) {
        saveSplitter(*(settings->getQSettings()), splitPlot, "fcsmaxentevaleditor/splitter_plot");
        saveSplitter(*(settings->getQSettings()), splitModel, "fcsmaxentevaleditor/splitter_model");
        saveSplitter(*(settings->getQSettings()), splitFitStatistics, "fcsmaxentevaleditor/splitter_fitstatistics");
        settings->getQSettings()->setValue("fcsmaxentevaleditor/residual_histogram_bins", spinResidualHistogramBins->value());
        settings->getQSettings()->setValue("fcsmaxentevaleditor/residual_toolbox_current", tabResidulas->currentIndex());
    }
}

void QFFCSByIndexAndModelEvaluationEditor::createWidgets() {

    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    mainLayout->setContentsMargins(5,5,5,5);
    setLayout(mainLayout);


    QWidget* topWidget=new QWidget(this);
    QVBoxLayout* vbl=new QVBoxLayout(topWidget);
    vbl->setContentsMargins(0,0,0,0);
    topWidget->setLayout(vbl);

    QFormLayout* fl;//=new QFormLayout(this);
    QLabel* l;
    QHBoxLayout* hbl;
    hblModel=new QHBoxLayout(this);
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
    tabResidulas->setTabPosition(QTabWidget::North);
    tabResidulas->setTabShape(QTabWidget::Triangular);
    tabResidulas->setDocumentMode(true);
    splitFitStatistics->addWidget(tabResidulas);
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

    flAlgorithmParams=new QFormLayout(this);
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

    QGridLayout* layBtn=new QGridLayout(this);
    layBtn->setContentsMargins(0,0,0,0);
    btnFitCurrent=new QPushButton(QIcon(":/fcs_maxent/fit_fitcurrent.png"), tr("&Fit Current"), this);
    btnFitCurrent->setToolTip(tr("perform a fit for the currently displayed file and run"));
    layBtn->addWidget(btnFitCurrent, 0, 0);
    btnFitRunsCurrent=new QPushButton(QIcon(":/fcs_maxent/fit_fitallruns.png"), tr("Fit All &Runs"), this);
    btnFitRunsCurrent->setToolTip(tr("perform a fit for all runs in the currently selected file "));
    layBtn->addWidget(btnFitRunsCurrent, 0, 1);
    btnFitAll=new QPushButton(QIcon(":/fcs_maxent/fit_fitcurrentrunallfiles.png"), tr("Fit All &Files (Current Run)"), this);
    btnFitAll->setToolTip(tr("perform a fit for all files, but fit in each file only the currently displayed run"));
    layBtn->addWidget(btnFitAll, 1, 0);
    btnFitRunsAll=new QPushButton(QIcon(":/fcs_maxent/fit_fitall.png"), tr("Fit &Everything"), this);
    btnFitRunsAll->setToolTip(tr("perform a fit for all runs in all files"));
    layBtn->addWidget(btnFitRunsAll, 1, 1);
    btnResetCurrent=new QPushButton(tr("&Reset Current"), this);
    btnResetCurrent->setToolTip(tr("reset the currently displayed file (and run) to the initial parameters\nThis deletes all fit results stored for the current file."));
    layBtn->addWidget(btnResetCurrent, 2, 0);
    btnResetAll=new QPushButton(tr("&Reset All"), this);
    btnResetAll->setToolTip(tr("reset all loaded files to the initial parameters.\nThis deletes all fit results stored for all files file."));
    layBtn->addWidget(btnResetAll, 3, 1);

    btnResetAllRuns=new QPushButton(tr("&Reset All Runs"), this);
    btnResetAllRuns->setToolTip(tr("reset all runs to the initial parameters in the current file.\nThis deletes all fit results stored for all runs in the current file."));
    layBtn->addWidget(btnResetAllRuns, 2, 1);

    btnCopyToInitial=new QPushButton(tr("Copy to &Initial"), this);
    btnCopyToInitial->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters,\n so they are used by files/runs that were not fit yet."));
    layBtn->addWidget(btnCopyToInitial, 4, 0);
    btnCopyToAllRuns=new QPushButton(tr("&Copy to All Runs"), this);
    btnCopyToAllRuns->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters\n and also to all runs in the current file."));
    layBtn->addWidget(btnCopyToAllRuns, 4, 1);
    btnCopyToAll=new QPushButton(tr("&Copy to All"), this);
    btnCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set\n of initial parameters and also to all files."));
    layBtn->addWidget(btnCopyToAll, 5, 1);
    btnCopyToAllCurrentRun=new QPushButton(tr("&Copy to All (Current Run)"), this);
    btnCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set of\n initial parameters and also to all files, but only to the current run therein."));
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
    toolbar->addWidget(chkXLogScale);
    toolbar->addSeparator();
    toolbar->addWidget(sp2);
    toolbar->addWidget(labMousePosition);

    connect(pltData, SIGNAL(zoomChangedLocally(double, double, double, double, JKQtPlotter*)), this, SLOT(zoomChangedLocally(double, double, double, double, JKQtPlotter*)));
    connect(pltData, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResiduals, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualHistogram, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualCorrelation, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));

    connect(btnFitCurrent, SIGNAL(clicked()), this, SLOT(fitCurrent()));
    connect(btnFitAll, SIGNAL(clicked()), this, SLOT(fitRunsAll()));
    connect(btnFitRunsAll, SIGNAL(clicked()), this, SLOT(fitRunsAll()));
    connect(btnFitRunsCurrent, SIGNAL(clicked()), this, SLOT(fitRunsCurrent()));
    connect(btnResetCurrent, SIGNAL(clicked()), this, SLOT(resetCurrent()));
    connect(btnResetAll, SIGNAL(clicked()), this, SLOT(resetAll()));
    connect(btnCopyToAll, SIGNAL(clicked()), this, SLOT(copyToAll()));
    connect(btnResetAllRuns, SIGNAL(clicked()), this, SLOT(resetAllRuns()));
    connect(btnCopyToAllRuns, SIGNAL(clicked()), this, SLOT(copyToAllRuns()));
    connect(btnCopyToInitial, SIGNAL(clicked()), this, SLOT(copyToInitial()));
    connect(btnCopyToAllCurrentRun, SIGNAL(clicked()), this, SLOT(copyToAllCurrentRun()));

    connect(datacut, SIGNAL(copyUserMinToAll(int)), this, SLOT(copyUserMinToAll(int)));
    connect(datacut, SIGNAL(copyUserMaxToAll(int)), this, SLOT(copyUserMaxToAll(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAll(int,int)), this, SLOT(copyUserMinMaxToAll(int,int)));
    connect(datacut, SIGNAL(copyUserMinToAllRuns(int)), this, SLOT(copyUserMinToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMaxToAllRuns(int)), this, SLOT(copyUserMaxToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAllRuns(int,int)), this, SLOT(copyUserMinMaxToAllRuns(int,int)));
}



void QFFCSByIndexAndModelEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
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


        cmbModel->setCurrentIndex(item->getCurrentModel());
        /* connect widgets and fill with data from item here */
        connect(item, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));

        connect(current, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        connect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        connect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(runChanged(int)));
        connect(cmbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
        connect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotStyleChanged(int)));
        connect(cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(errorStyleChanged(int)));
        connect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(chkGridToggled(bool)));
        connect(chkXLogScale, SIGNAL(toggled(bool)), this, SLOT(chkXLogScaleToggled(bool)));
        connect(cmbResidualStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(residualStyleChanged(int)));
        connect(chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(chkWeightedResidualsToggled(bool)));
        connect(spinResidualHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(residualHistogramBinsChanged(int)));
        connect(widFitParams, SIGNAL(parameterChanged(QString,double)), this, SLOT(fitParamChanged()));

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
        datacut->set_userMin(getUserMin(0));
        datacut->set_userMax(getUserMax(currentRecord, data->getCorrelationN()-1));
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
