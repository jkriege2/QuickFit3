#include "qffcsmaxentevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qffcsmaxentevaluation_item.h"

#include <QtGui>
#include <QtCore>

QFFCSMaxEntEvaluationEditor::QFFCSMaxEntEvaluationEditor(QFPluginServices* services, QWidget* parent):
    QFEvaluationEditor(services, parent)
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

QFFCSMaxEntEvaluationEditor::~QFFCSMaxEntEvaluationEditor()
{
    delete dlgEvaluationProgress;
}


void QFFCSMaxEntEvaluationEditor::createWidgets() {

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
    cmbModel=new QComboBox(this);
    l=new QLabel(tr("&Physical Model:"), this);
    l->setBuddy(cmbModel);
    hbl->addWidget(l);
    cmbModel->setEditable(false);
    cmbModel->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    hbl->addWidget(cmbModel);
    hbl->addStretch();

    cmbWeights=new QComboBox(this);
    cmbWeights->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    cmbWeights->setEditable(false);
    cmbWeights->addItem(tr("equal weights"));
    cmbWeights->addItem(tr("standard deviation"));
    cmbWeights->addItem(tr("per run errors"));
    cmbWeights->setMaximumWidth(150);
    cmbWeights->setMinimumWidth(150);
    l=new QLabel(tr("&Weight Model: "), this);
    l->setBuddy(cmbWeights);
    hbl->addWidget(l);
    hbl->addWidget(cmbWeights);
    hbl->addStretch();
    //fl->addRow(tr("&Weight Model:"), cmbWeights);
    //vbl->addLayout(fl);
    vbl->addLayout(hbl);


    QFont boldfont=font();
    boldfont.setBold(true);

    toolbar=new QToolBar("toolbar_fcs_fit", this);
    vbl->addWidget(toolbar);
    actSaveReport=new QAction(QIcon(":/fcs_maxent/fit_savereport.png"), tr("Save Report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actPrintReport=new QAction(QIcon(":/fcs_maxent/fit_printreport.png"), tr("Print Report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    QLabel* lPS=new QLabel(tr("   &Plots: "), toolbar);
    lPS->setFont(boldfont);
    cmbPlotStyle=new QComboBox(toolbar);
    cmbPlotStyle->addItem(QIcon(":/fcs_maxent/plot_points.png"), tr("points"));
    cmbPlotStyle->addItem(QIcon(":/fcs_maxent/plot_lines.png"), tr("lines"));
    cmbPlotStyle->addItem(QIcon(":/fcs_maxent/plot_pointslines.png"), tr("lines + points"));
    lPS->setBuddy(cmbPlotStyle);
    QLabel* lES=new QLabel(tr("   &Errors: "), toolbar);
    lES->setFont(boldfont);
    cmbErrorStyle=new QComboBox(toolbar);
    cmbErrorStyle->addItem(QIcon(":/fcs_maxent/plot_enone.png"), tr("none"));
    cmbErrorStyle->addItem(QIcon(":/fcs_maxent/plot_elines.png"), tr("lines"));
    cmbErrorStyle->addItem(QIcon(":/fcs_maxent/plot_ebars.png"), tr("bars"));
    cmbErrorStyle->addItem(QIcon(":/fcs_maxent/plot_elinesbars.png"), tr("bars + lines"));
    lES->setBuddy(cmbErrorStyle);

    QLabel* lRS=new QLabel(tr("   &Residuals: "), toolbar);
    lRS->setFont(boldfont);
    cmbResidualStyle=new QComboBox(toolbar);
    cmbResidualStyle->addItem(QIcon(":/fcs_maxent/plot_points.png"), tr("points"));
    cmbResidualStyle->addItem(QIcon(":/fcs_maxent/plot_lines.png"), tr("lines"));
    cmbResidualStyle->addItem(QIcon(":/fcs_maxent/plot_pointslines.png"), tr("lines + points"));
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










void QFFCSMaxEntEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFFCSMaxEntEvaluationItem* item=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    QFFCSMaxEntEvaluationItem* item_old=qobject_cast<QFFCSMaxEntEvaluationItem*>(old);

    if (old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        disconnect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        disconnect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(runChanged(int)));
        disconnect(cmbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
        disconnect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        disconnect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotStyleChanged(int)));
        disconnect(cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(errorStyleChanged(int)));
        disconnect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(chkGridToggled(bool)));
        disconnect(chkXLogScale, SIGNAL(toggled(bool)), this, SLOT(chkXLogScaleToggled(bool)));
        disconnect(cmbResidualStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(residualStyleChanged(int)));
        disconnect(chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(chkWeightedResidualsToggled(bool)));
        disconnect(spinResidualHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(residualHistogramBinsChanged(int)));

        spinRun->setMaximum(-1);
    }



    if (item) {
        dataEventsEnabled=false;

        chkXLogScale->setChecked(current->getProperty("plot_taulog", true).toBool());
        chkGrid->setChecked(current->getProperty("plot_grid", true).toBool());
        cmbPlotStyle->setCurrentIndex(current->getProperty("plot_style", 1).toInt());
        cmbErrorStyle->setCurrentIndex(current->getProperty("plot_errorstyle", 0).toInt());
        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        cmbResidualStyle->setCurrentIndex(current->getProperty("plot_residualsstyle", 0).toInt());
        chkWeightedResiduals->setChecked(current->getProperty("weighted_residuals", false).toBool());
        spinResidualHistogramBins->setValue(current->getProperty("plot_residualshistogrambins", 25).toInt());

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
        connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        connect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(plotStyleChanged(int)));
        connect(cmbErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(errorStyleChanged(int)));
        connect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(chkGridToggled(bool)));
        connect(chkXLogScale, SIGNAL(toggled(bool)), this, SLOT(chkXLogScaleToggled(bool)));
        connect(cmbResidualStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(residualStyleChanged(int)));
        connect(chkWeightedResiduals, SIGNAL(toggled(bool)), this, SLOT(chkWeightedResidualsToggled(bool)));
        connect(spinResidualHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(residualHistogramBinsChanged(int)));

        dataEventsEnabled=true;
    }


    displayData();
    displayParameters();
}

void QFFCSMaxEntEvaluationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    displayData();
    displayParameters();
}

void QFFCSMaxEntEvaluationEditor::readSettings() {
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
};

void QFFCSMaxEntEvaluationEditor::writeSettings() {
    if (cmbModel) {
        saveSplitter(*(settings->getQSettings()), splitPlot, "fcsmaxentevaleditor/splitter_plot");
        saveSplitter(*(settings->getQSettings()), splitModel, "fcsmaxentevaleditor/splitter_model");
        saveSplitter(*(settings->getQSettings()), splitFitStatistics, "fcsmaxentevaleditor/splitter_fitstatistics");
        settings->getQSettings()->setValue("fcsmaxentevaleditor/residual_histogram_bins", spinResidualHistogramBins->value());
        settings->getQSettings()->setValue("fcsmaxentevaleditor/residual_toolbox_current", tabResidulas->currentIndex());
    }
}


int QFFCSMaxEntEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index, int defaultMin) {
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return defaultMin;
    const QString resultID=data->getEvaluationResultID(index);
    if (!rec) return defaultMin;


    return rec->getProperty(resultID+"_datacut_min", defaultMin).toInt();
}

int QFFCSMaxEntEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index, int defaultMax) {
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return defaultMax;
    const QString resultID=data->getEvaluationResultID(index);

    if (!rec) return defaultMax;

    return rec->getProperty(resultID+"_datacut_max", defaultMax).toInt();
}


int QFFCSMaxEntEvaluationEditor::getUserMin(QFRawDataRecord* rec) {
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return 0;
    return getUserMin(rec, data->getCurrentIndex());
}

int QFFCSMaxEntEvaluationEditor::getUserMax(QFRawDataRecord* rec) {
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return 0;
    return getUserMax(rec, data->getCurrentIndex());
}


int QFFCSMaxEntEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index) {
    return getUserMin(rec, index, datacut->get_userMin());
}

int QFFCSMaxEntEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index) {
    return getUserMax(rec, index, datacut->get_userMax());
}

void QFFCSMaxEntEvaluationEditor::setUserMin(int userMin) {
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
}

void QFFCSMaxEntEvaluationEditor::setUserMax(int userMax) {
    if (!current) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
}

void QFFCSMaxEntEvaluationEditor::setUserMinMax(int userMin, int userMax) {
    if (!current) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return;
    QFRawDataRecord* rdr=data->getHighlightedRecord();
    const QString resultID=data->getEvaluationResultID();
    rdr->disableEmitPropertiesChanged();
    rdr->setQFProperty(resultID+"_datacut_min", userMin, false, false);
    rdr->setQFProperty(resultID+"_datacut_max", userMax, false, false);
    rdr->enableEmitPropertiesChanged(true);
}

void QFFCSMaxEntEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
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
        cmbWeights->setCurrentIndex(eval->getCurrentWeights());

        dataEventsEnabled=true;
    }
    
    // ensure that data of new highlighted record is displayed
    displayParameters();
    displayData();
}

void QFFCSMaxEntEvaluationEditor::displayParameters() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    if (eval->hasResults(record)) {
        if (record->resultsExists(eval->getEvaluationResultID(), "evaluation_completed")) {
            //ui->labResults->setText(tr("<b>Results:</b><br>evaluation_completed = %1").arg(record->resultsGetAsString(eval->getEvaluationResultID(), "evaluation_completed")));
        } else {
            //ui->labResults->setText(tr("<b>NO RESULTS STORED</b>"));
        }
    } else {
        //ui->labResults->setText(tr("<b>NO EVALUATION DONE YET</b>"));
    }
}

void QFFCSMaxEntEvaluationEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);

    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();

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
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();


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
                // calculate fit statistics
                /////////////////////////////////////////////////////////////////////////////////
                record->disableEmitResultsChanged();
                QFFitStatistics fitResults=eval->calcFitStatistics(record, index, eval->getCurrentModel(), tauvals, N, datacut_min, datacut_max, runAvgWidth, residualHistogramBins);
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
                size_t c_tauresra=dsres->addCopiedColumn(fitResults.tau_runavg, fitResults.runAvgN, "tau_resid_runavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(fitResults.residuals_runavg_weighted, fitResults.runAvgN, "residuals_runavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(fitResults.residuals_runavg, fitResults.runAvgN, "residuals_runavg");
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
                QString fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML");

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
                free(weights);
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
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doEvaluation(record, eval->getCurrentIndex());

    displayParameters();
    displayData();
    dlgEvaluationProgress->setValue(100);

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
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


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
                eval->doEvaluation(record, idx);
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
                dlgEvaluationProgress->setValue(idx);
            }
        }
        QApplication::processEvents();
    //}
    dlgEvaluationProgress->setValue(recs.size());
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
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


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
                eval->doEvaluation(record, idx);
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            }
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setValue(recs.size());
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
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


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
                eval->doEvaluation(record, idx);
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            //}
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setValue(recs.size());
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
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

void QFFCSMaxEntEvaluationEditor::saveReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                currentSaveDirectory,
                                tr("PDF File (*.pdf);;PostScript File (*.ps)"));

    if (!fn.isEmpty()) {
        currentSaveDirectory=QFileInfo(fn).absolutePath();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter();
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
        delete doc;
        delete printer;
        QApplication::restoreOverrideCursor();
    }
}

void QFFCSMaxEntEvaluationEditor::printReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTextDocument* doc=new QTextDocument();
    doc->setTextWidth(p->pageRect().size().width());
    createReportDoc(doc);
    doc->print(p);
    delete p;
    delete doc;
    QApplication::restoreOverrideCursor();
}


void QFFCSMaxEntEvaluationEditor::plotMouseMove(double x, double y) {
    labMousePosition->setTextFormat(Qt::RichText);
    labMousePosition->setText(tr("cursor: (%1, %2)").arg(floattohtmlstr(x).c_str()).arg(floattohtmlstr(y).c_str()));
}

void QFFCSMaxEntEvaluationEditor::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (!dataEventsEnabled) return;
    if (sender==pltData) {
        pltResiduals->setX(newxmin, newxmax);
    }
}

void QFFCSMaxEntEvaluationEditor::modelChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    data->setCurrentModel(model);
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::weightsChanged(int weights) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    data->setCurrentWeights(weights);
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::runChanged(int run) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTime t;

    t.start();
    QFRawDataRecord* currentRecord=current->getHighlightedRecord();
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
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

void QFFCSMaxEntEvaluationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!data) return;
    if (!current->getHighlightedRecord()) return;
    setUserMinMax(userMin, userMax);
    displayData();
}



void QFFCSMaxEntEvaluationEditor::chkXLogScaleToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    current->setQFProperty("plot_taulog", chkXLogScale->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::chkGridToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    current->setQFProperty("plot_grid", chkGrid->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::plotStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    current->setQFProperty("plot_style", cmbPlotStyle->currentIndex(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::errorStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    current->setQFProperty("plot_errorstyle", cmbErrorStyle->currentIndex(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::residualStyleChanged(int style) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    current->setQFProperty("plot_residualsstyle", cmbResidualStyle->currentIndex(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::chkWeightedResidualsToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    current->setQFProperty("weighted_residuals", chkWeightedResiduals->isChecked(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::residualHistogramBinsChanged(int bins) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    current->setQFProperty("plot_residualshistogrambins", spinResidualHistogramBins->value(), false, false);
    displayData();
    QApplication::restoreOverrideCursor();
}
