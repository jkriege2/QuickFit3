#include "qffitresultsbyindexevaluationeditorwithwidgets.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include <iostream>
#include <cfloat>
#include "tools.h"
#include "statistics_tools.h"
#include <QThread>
#include "qmoretextobject.h"
#include "qmodernprogresswidget.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "dlgqfprogressdialog.h"

QFFitResultsByIndexEvaluationEditorWithWidgets::QFFitResultsByIndexEvaluationEditorWithWidgets(QString iniPrefix, QFEvaluationPropertyEditor* propEditor, QFPluginServices* services, QWidget *parent, bool hasMultiThreaded, bool multiThreadPriority) :
    QFFitResultsByIndexEvaluationEditorBase(iniPrefix, propEditor, services, parent)
{
    cmbModel=NULL;
    dataEventsEnabled=true;
    m_parameterWidgetWidth=75;
    m_parameterCheckboxWidth=32;
    m_multithreadPriority=multiThreadPriority;
    m_hasMultithread=hasMultiThreaded;
    fitStatisticsReport="";


    createWidgets(hasMultiThreaded, multiThreadPriority);

}

void QFFitResultsByIndexEvaluationEditorWithWidgets::createWidgets(bool hasMultiThreaded, bool multiThreadPriority) {



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
    QHBoxLayout* hbl=NULL;
    layAlgorithm=new QHBoxLayout(this);
    layAlgorithm->setContentsMargins(0,0,0,0);
    cmbAlgorithm=new QComboBox(this);
    l=new QLabel(tr("Fit &Algorithm: "), this);
    l->setBuddy(cmbAlgorithm);
    layAlgorithm->addWidget(l);
    cmbAlgorithm->setEditable(false);
    layAlgorithm->addWidget(cmbAlgorithm);
    btnConfigAlgorithm=createButtonAndActionShowText(actConfigAlgorithm, QIcon(":/fcsfit/fit_config.png"), tr("&Configure Algorithm"), this);
    btnConfigAlgorithm->setMaximumWidth(250);
    layAlgorithm->addWidget(btnConfigAlgorithm);
    btnAlgorithmHelp=createButtonAndActionShowText(actAlgorithmHelp, QIcon(":/fcsfit/fit_help.png"), tr("Algorithm &Help"), this);
    btnAlgorithmHelp->setMaximumWidth(250);
    layAlgorithm->addWidget(btnAlgorithmHelp);
    layAlgorithm->addStretch();



    vbl->addLayout(layAlgorithm);


    layModel=new QHBoxLayout(this);
    layModel->setContentsMargins(0,0,0,0);
    cmbModel=new QComboBox(this);
    l=new QLabel(tr("Fit &Model:"), this);
    l->setBuddy(cmbModel);
    layModel->addWidget(l);
    cmbModel->setEditable(false);
    layModel->addWidget(cmbModel);
    btnModelHelp=createButtonAndActionShowText(actModelHelp, QIcon(":/fcsfit/fit_help.png"), tr("Model H&elp"), this);
    btnModelHelp->setMaximumWidth(250);
    layModel->addWidget(btnModelHelp);
    layModel->addStretch();
    vbl->addLayout(layModel);


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
    /*actSaveReport=new QAction(QIcon(":/fcsfit/fit_savereport.png"), tr("Save Report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actPrintReport=new QAction(QIcon(":/fcsfit/fit_printreport.png"), tr("Print Report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));*/
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
    cmbErrorStyle->addItem(QIcon(":/libqf3widgets/plot_epoly.png"), tr("polygons"));
    cmbErrorStyle->addItem(QIcon(":/libqf3widgets/plot_epolybars.png"), tr("polygons + bars"));
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
    pltData->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltData->resize(300,200);
    splitPlot->addWidget(pltData);


    QWidget* widgetResiduals=new QWidget(this);
    vbl=new QVBoxLayout(widgetResiduals);
    vbl->setContentsMargins(0,0,0,0);
    vbl->setSpacing(1);
    widgetResiduals->setLayout(vbl);

    pltResiduals=new JKQtPlotter(true, this);
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
    pltData->get_plotter()->set_keyXMargin(2);
    pltData->get_plotter()->set_keyYMargin(2);
    pltResiduals->get_plotter()->set_keyFontSize(9);
    pltResiduals->get_plotter()->set_keyXMargin(2);
    pltResiduals->get_plotter()->set_keyYMargin(2);
    pltResiduals->useExternalDatastore(pltData->getDatastore());
    pltResiduals->setMinimumHeight(75);
    pltData->setMinimumHeight(75);


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


    pltResidualCorrelation=new JKQtPlotter(true, this);
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
    /*btnEditRanges=createButtonAndActionShowText(, tr("Edit &Ranges"), this);
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


    layButtons=new QGridLayout(this);
    layButtons->setContentsMargins(0,0,0,0);
    btnFitCurrent=createButtonAndActionShowText(actFitCurrent, QIcon(":/fcsfit/fit_fitcurrent.png"), tr("&Fit Current"), this);
    actFitCurrent->setToolTip(tr("perform a fit for the currently displayed file and run"));
    layButtons->addWidget(btnFitCurrent, 0, 0);
    btnFitRunsCurrent=createButtonAndActionShowText(actFitRunsCurrent, QIcon(":/fcsfit/fit_fitallruns.png"), tr("Fit All &Runs"), this);
    actFitRunsCurrent->setToolTip(tr("perform a fit for all runs in the currently selected file "));
    layButtons->addWidget(btnFitRunsCurrent, 0, 1);
    btnFitAll=createButtonAndActionShowText(actFitAll, QIcon(":/fcsfit/fit_fitcurrentrunallfiles.png"), tr("Fit All &Files (Current Run)"), this);
    actFitAll->setToolTip(tr("perform a fit for all files, but fit in each file only the currently displayed run"));
    layButtons->addWidget(btnFitAll, 1, 0);
    btnFitRunsAll=createButtonAndActionShowText(actFitRunsAll, QIcon(":/fcsfit/fit_fitall.png"), tr("Fit &Everything"), this);
    actFitRunsAll->setToolTip(tr("perform a fit for all runs in all files"));
    layButtons->addWidget(btnFitRunsAll, 1, 1);
    btnResetCurrent=createButtonAndActionShowText(actResetCurrent, tr("&Reset Current"), this);
    actResetCurrent->setToolTip(tr("reset the currently displayed file (and run) to the initial parameters\nThis deletes all fit results stored for the current file."));
    layButtons->addWidget(btnResetCurrent, 2, 0);
    btnResetAll=createButtonAndActionShowText(actResetAll, tr("&Reset All"), this);
    actResetAll->setToolTip(tr("reset all loaded files to the initial parameters.\nThis deletes all fit results stored for all files file."));
    layButtons->addWidget(btnResetAll, 3, 1);

    btnResetAllRuns=createButtonAndActionShowText(actResetAllRuns, tr("&Reset All Runs"), this);
    actResetAllRuns->setToolTip(tr("reset all runs to the initial parameters in the current file.\nThis deletes all fit results stored for all runs in the current file."));
    layButtons->addWidget(btnResetAllRuns, 2, 1);

    btnCopyToInitial=createButtonAndActionShowText(actCopyToInitial, tr("Copy to &Initial"), this);
    actCopyToInitial->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters,\n so they are used by files/runs that were not fit yet."));
    layButtons->addWidget(btnCopyToInitial, 4, 0);
    btnCopyToAllRuns=createButtonAndActionShowText(actCopyToAllRuns, tr("&Copy to All Runs"), this);
    actCopyToAllRuns->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters\n and also to all runs in the current file."));
    layButtons->addWidget(btnCopyToAllRuns, 4, 1);
    btnCopyToAll=createButtonAndActionShowText(actCopyToAll, tr("&Copy to All"), this);
    actCopyToAll->setToolTip(tr("copy the currently displayed fit parameters to the set\n of initial parameters and also to all files."));
    layButtons->addWidget(btnCopyToAll, 5, 1);
    btnCopyToAllCurrentRun=createButtonAndActionShowText(actCopyToAllCurrentRun, tr("&Copy to All (Current Run)"), this);
    actCopyToAllCurrentRun->setToolTip(tr("copy the currently displayed fit parameters to the set of\n initial parameters and also to all files, but only to the current run therein."));
    layButtons->addWidget(btnCopyToAllCurrentRun, 5, 0);


    btnLoadParameters=createButtonAndActionShowText(actLoadParameters, QIcon(":/fcsfit/param_load.png"), tr("&Load Parameters"), this);
    actLoadParameters->setToolTip(tr("load a FCS fit parameter set from a file.\nThe parameter set files can be created using \"Save Parameters\""));
    layButtons->addWidget(btnLoadParameters, 7, 0);
    btnSaveParameters=createButtonAndActionShowText(actSaveParameters, QIcon(":/fcsfit/param_save.png"), tr("&Save Parameters"), this);
    actSaveParameters->setToolTip(tr("save the current FCS fit parameter as a set to a file\nfor later reuse with \"Load Parameters\""));
    layButtons->addWidget(btnSaveParameters, 7, 1);

    layModel->addLayout(layButtons);

    btnFitAll->setFocusPolicy(Qt::StrongFocus);
    btnFitRunsAll->setFocusPolicy(Qt::StrongFocus);
    btnFitRunsCurrent->setFocusPolicy(Qt::StrongFocus);
    btnFitCurrent->setFocusPolicy(Qt::StrongFocus);


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

    //hlpAlgorithm=new QFHTMLHelpWindow(0);
    //hlpAlgorithm->initFromPluginServices(services);
    //hlpAlgorithm->close();
    //hlpFunction=new QFHTMLHelpWindow(0);
    //hlpFunction->initFromPluginServices(services);
    //hlpFunction->close();

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

    connect(actAlgorithmHelp, SIGNAL(triggered()), this, SLOT(displayFitAlgorithmHelp()));
    connect(actConfigAlgorithm, SIGNAL(triggered()), this, SLOT(configFitAlgorithm()));
    connect(actModelHelp, SIGNAL(triggered()), this, SLOT(displayFitFunctionHelp()));
    connect(pltData, SIGNAL(zoomChangedLocally(double, double, double, double, JKQtPlotter*)), this, SLOT(zoomChangedLocally(double, double, double, double, JKQtPlotter*)));
    connect(pltData, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResiduals, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualHistogram, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResidualCorrelation, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));

    connect(actFitCurrent, SIGNAL(triggered()), this, SLOT(fitCurrent()));
    connect(actFitAll, SIGNAL(triggered()), this, SLOT(fitAll()));
    connect(actFitRunsAll, SIGNAL(triggered()), this, SLOT(fitRunsAll()));
    connect(actFitRunsCurrent, SIGNAL(triggered()), this, SLOT(fitRunsCurrent()));
    connect(actResetCurrent, SIGNAL(triggered()), this, SLOT(resetCurrent()));
    connect(actResetAll, SIGNAL(triggered()), this, SLOT(resetAll()));
    connect(actCopyToAll, SIGNAL(triggered()), this, SLOT(copyToAll()));
    connect(actResetAllRuns, SIGNAL(triggered()), this, SLOT(resetAllRuns()));
    connect(actCopyToAllRuns, SIGNAL(triggered()), this, SLOT(copyToAllRuns()));
    connect(actCopyToInitial, SIGNAL(triggered()), this, SLOT(copyToInitial()));
    connect(actCopyToAllCurrentRun, SIGNAL(triggered()), this, SLOT(copyToAllCurrentRun()));
    connect(actLoadParameters, SIGNAL(triggered()), this, SLOT(loadCurrentFitResults()));
    connect(actSaveParameters, SIGNAL(triggered()), this, SLOT(saveCurrentFitResults()));

    connect(datacut, SIGNAL(copyUserMinToAll(int)), this, SLOT(copyUserMinToAll(int)));
    connect(datacut, SIGNAL(copyUserMaxToAll(int)), this, SLOT(copyUserMaxToAll(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAll(int,int)), this, SLOT(copyUserMinMaxToAll(int,int)));
    connect(datacut, SIGNAL(copyUserMinToAllRuns(int)), this, SLOT(copyUserMinToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMaxToAllRuns(int)), this, SLOT(copyUserMaxToAllRuns(int)));
    connect(datacut, SIGNAL(copyUserMinMaxToAllRuns(int,int)), this, SLOT(copyUserMinMaxToAllRuns(int,int)));



    menuParameters=propertyEditor->addMenu("&Parameters", 0);
    menuParameters->addAction(actResetCurrent);
    menuParameters->addAction(actResetAll);
    menuParameters->addAction(actResetAllRuns);
    menuParameters->addSeparator();
    menuParameters->addAction(actCopyToAll);
    menuParameters->addAction(actCopyToAllRuns);
    menuParameters->addAction(actCopyToInitial);
    menuParameters->addAction(actCopyToAllCurrentRun);
    menuParameters->addSeparator();
    menuParameters->addAction(actLoadParameters);
    menuParameters->addAction(actSaveParameters);
    //menuParameters->addAction();


    menuFit=propertyEditor->addMenu("&Fit", 0);
    menuFit->addAction(actFitCurrent);
    menuFit->addAction(actFitRunsCurrent);
    menuFit->addAction(actFitRunsAll);
    menuFit->addAction(actFitAll);
    actFitSeparator=menuFit->addSeparator();
    menuFit->addAction(actConfigAlgorithm);
    menuFit->addAction(actAlgorithmHelp);
    menuFit->addAction(actModelHelp);

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


    propertyEditor->getHelpMenu()->addAction(actAlgorithmHelp);
    propertyEditor->getHelpMenu()->addAction(actModelHelp);



    actFitAllFilesThreaded=new QAction(QIcon(":/fcsfit/fit_fitcurrentrunallfiles.png"), tr("Fit All &Files (this run, MT)"), this);
    actFitAllFilesThreaded->setToolTip(tr("multi-threaded: perform a fit for all files, but fit in each file only the currently displayed run"));
    connect (actFitAllFilesThreaded, SIGNAL(triggered()), this, SLOT(fitAllFilesThreaded()));

    actFitAllThreaded=new QAction(QIcon(":/imfcsfit/fit_fitall.png"), tr("Fit Everything (MT)"), this);
    actFitAllThreaded->setToolTip(tr("multi-threaded: perform a fit for all files, and all runs therein (everything)"));
    connect (actFitAllThreaded, SIGNAL(triggered()), this, SLOT(fitEverythingThreaded()));

    actFitAllRunsThreaded=new QAction(QIcon(":/imfcsfit/fit_fitallruns.png"), tr("Fit All Runs (MT)"), this);
    actFitAllRunsThreaded->setToolTip(tr("multi-threaded: perform a fit for all runs, in the current file"));
    connect (actFitAllRunsThreaded, SIGNAL(triggered()), this, SLOT(fitAllRunsThreaded()));

    if (hasMultiThreaded) {
        populateFitButtons();

        menuFit->insertSeparator(actFitRunsCurrent);
        menuFit->insertAction(actFitRunsCurrent, actFitAllRunsThreaded);
        menuFit->insertAction(actFitRunsCurrent, actFitAllFilesThreaded);
        menuFit->insertAction(actFitRunsCurrent, actFitAllThreaded);
        menuFit->insertSeparator(actFitRunsCurrent);
    }
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::connectDefaultWidgets(QFEvaluationItem *current, QFEvaluationItem *old, bool updatePlots) {
    QFFitResultsByIndexEvaluation* fcs=qobject_cast<QFFitResultsByIndexEvaluation*>(current);

    if (old!=NULL) {
        disconnect(old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        disconnect(datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        disconnect(spinRun, SIGNAL(valueChanged(int)), this, SLOT(runChanged(int)));
        disconnect(cmbAlgorithm, SIGNAL(currentIndexChanged(int)), this, SLOT(algorithmChanged(int)));
        disconnect(cmbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(modelChanged(int)));
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
        cmbPlotStyle->setCurrentIndex(current->getProperty("plot_style", 0).toInt());
        cmbErrorStyle->setCurrentIndex(current->getProperty("plot_errorstyle", 2).toInt());
        cmbResidualStyle->setCurrentIndex(current->getProperty("plot_residualsstyle", 0).toInt());
        chkWeightedResiduals->setChecked(current->getProperty("weighted_residuals", false).toBool());
        spinResidualHistogramBins->setValue(current->getProperty("plot_residualshistogrambins", 25).toInt());

        QStringList ff=fcs->getAvailableFitFunctions();
        for (int i=0; i<ff.size(); i++) {
            QString id=ff[i];
            if (fcs->getFitFunction(id)) cmbModel->addItem(fcs->getFitFunction(id)->name(), id);
            else  cmbModel->addItem(tr("unknown fit function"), id);
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
        algorithmChanged(cmbAlgorithm->currentIndex());
        dataEventsEnabled=true;
    }

    connect(cmbAlgorithm, SIGNAL(currentIndexChanged(int)), this, SLOT(algorithmChanged(int)));
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

    if (updatePlots) {
        displayModel(true);
        replotData();
    }
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::writeSettings() {
    if (cmbModel && current) {
        settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/algorithm").arg(current->getType()).arg(current->getID()), cmbAlgorithm->currentIndex());
        settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/model").arg(current->getType()).arg(current->getID()), cmbModel->currentIndex());
        //hlpAlgorithm->writeSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/algorithm_").arg(current->getType()).arg(current->getID()));
        //hlpFunction->writeSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/function_").arg(current->getType()).arg(current->getID()));
        saveSplitter(*(settings->getQSettings()), splitPlot, QString("fitevaleditor_%1%2/splitter_plot").arg(current->getType()).arg(current->getID()));
        saveSplitter(*(settings->getQSettings()), splitModel, QString("fitevaleditor_%1%2/splitter_model").arg(current->getType()).arg(current->getID()));
        saveSplitter(*(settings->getQSettings()), splitFitStatistics, QString("fitevaleditor_%1%2/splitter_fitstatistics").arg(current->getType()).arg(current->getID()));
        settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/parameterWidgetWidth").arg(current->getType()).arg(current->getID()), m_parameterWidgetWidth);
        settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/parameterCheckboxWidth").arg(current->getType()).arg(current->getID()), m_parameterCheckboxWidth);
        settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/display_range_widgets").arg(current->getType()).arg(current->getID()), tbEditRanges->currentIndex());
        settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/residual_histogram_bins").arg(current->getType()).arg(current->getID()), spinResidualHistogramBins->value());
        settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/residual_toolbox_current").arg(current->getType()).arg(current->getID()), tabResidulas->currentIndex());
        QFFitResultsEvaluationEditorBase::writeSettings();
    }
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::readSettings() {
    if (cmbModel && current) {
        pltData->loadSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/pltdata/"));
        pltResiduals->loadSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/pltresiduals/").arg(current->getType()).arg(current->getID()));
        pltResidualHistogram->loadSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/pltresidualhistogram/").arg(current->getType()).arg(current->getID()));
        pltResidualCorrelation->loadSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/pltresidualcorrelation/").arg(current->getType()).arg(current->getID()));
        //hlpAlgorithm->readSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/algorithm_").arg(current->getType()).arg(current->getID()));
        //hlpFunction->readSettings(*settings->getQSettings(), QString("fitevaleditor_%1%2/function_").arg(current->getType()).arg(current->getID()));
        loadSplitter(*(settings->getQSettings()), splitPlot, QString("fitevaleditor_%1%2/splitter_plot").arg(current->getType()).arg(current->getID()));
        loadSplitter(*(settings->getQSettings()), splitModel, QString("fitevaleditor_%1%2/splitter_model").arg(current->getType()).arg(current->getID()));
        loadSplitter(*(settings->getQSettings()), splitFitStatistics, QString("fitevaleditor_%1%2/splitter_fitstatistics").arg(current->getType()).arg(current->getID()));
        m_parameterWidgetWidth=settings->getQSettings()->value(QString("fitevaleditor_%1%2/parameterWidgetWidth").arg(current->getType()).arg(current->getID()), m_parameterWidgetWidth).toInt();
        m_parameterCheckboxWidth=settings->getQSettings()->value(QString("fitevaleditor_%1%2/parameterCheckboxWidth").arg(current->getType()).arg(current->getID()), m_parameterCheckboxWidth).toInt();
        tbEditRanges->setCurrentIndex(settings->getQSettings()->value(QString("fitevaleditor_%1%2/display_range_widgets").arg(current->getType()).arg(current->getID()), 0).toInt());
        spinResidualHistogramBins->setValue(settings->getQSettings()->value(QString("fitevaleditor_%1%2/residual_histogram_bins").arg(current->getType()).arg(current->getID()), 25).toInt());
        tabResidulas->setCurrentIndex(settings->getQSettings()->value(QString("fitevaleditor_%1%2/residual_toolbox_current").arg(current->getType()).arg(current->getID()), 0).toInt());
        QFFitResultsEvaluationEditorBase::readSettings();

    }
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::updateParameterValues()
{
    if (!current) return;
    if (!cmbModel) return;

    QTime t;
    t.start();

    //qDebug()<<"QFFCSFitEvaluationEditor::updateParameterValues()";
    //QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
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
    //qDebug()<<"QFFitResultsByIndexEvaluationEditor::updateParameterValues() ... done "<<t.elapsed()<<" ms";

}

void QFFitResultsByIndexEvaluationEditorWithWidgets::populateFitButtons(bool mulThreadEnabledInModel)
{
    removeAllActions(btnFitAll); btnFitAll->setDefaultAction(NULL);
    removeAllActions(btnFitRunsAll); btnFitRunsAll->setDefaultAction(NULL);
    removeAllActions(btnFitRunsCurrent); btnFitRunsCurrent->setDefaultAction(NULL);
    if (m_hasMultithread && mulThreadEnabledInModel) {
        btnFitAll->setPopupMode(QToolButton::MenuButtonPopup);
        btnFitRunsAll->setPopupMode(QToolButton::MenuButtonPopup);
        btnFitRunsCurrent->setPopupMode(QToolButton::MenuButtonPopup);

        if (m_multithreadPriority) {
            btnFitAll->setDefaultAction(actFitAllFilesThreaded);
            btnFitRunsAll->setDefaultAction(actFitAllThreaded);
            btnFitRunsCurrent->setDefaultAction(actFitAllRunsThreaded);
            btnFitAll->addAction(actFitAll);
            btnFitRunsAll->addAction(actFitRunsAll);
            btnFitRunsCurrent->addAction(actFitRunsCurrent);

        } else {
            btnFitAll->setDefaultAction(actFitAll);
            btnFitRunsAll->setDefaultAction(actFitRunsAll);
            btnFitRunsCurrent->setDefaultAction(actFitRunsCurrent);
            btnFitAll->addAction(actFitAllFilesThreaded);
            btnFitRunsAll->addAction(actFitAllThreaded);
            btnFitRunsCurrent->addAction(actFitAllRunsThreaded);
        }
    } else {
        btnFitAll->setPopupMode(QToolButton::DelayedPopup);
        btnFitRunsAll->setPopupMode(QToolButton::DelayedPopup);
        btnFitRunsCurrent->setPopupMode(QToolButton::DelayedPopup);
        btnFitAll->setDefaultAction(actFitAll);
        btnFitRunsAll->setDefaultAction(actFitRunsAll);
        btnFitRunsCurrent->setDefaultAction(actFitRunsCurrent);
    }

}

void QFFitResultsByIndexEvaluationEditorWithWidgets::plotMouseMove(double x, double y) {
    labMousePosition->setTextFormat(Qt::RichText);
    labMousePosition->setText(tr("cursor: (%1, %2)").arg(floattohtmlstr(x).c_str()).arg(floattohtmlstr(y).c_str()));
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::highlightingChanged(QFRawDataRecord *formerRecord, QFRawDataRecord *currentRecord)
{
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));
    bool modelChanged=false;
    if (currentRecord) {
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));

        datacut->disableSliderSignals();
        datacut->set_min(0);
        datacut->set_max(getUserRangeMax(currentRecord, eval->getCurrentIndex()));
        QString run="avg";
        if (eval->getCurrentIndex()>-1) run=QString::number(eval->getCurrentIndex());
        datacut->set_userMin(getUserMin(0));
        datacut->set_userMax(getUserMax(getUserRangeMax(currentRecord, eval->getCurrentIndex())));
        datacut->enableSliderSignals();
        dataEventsEnabled=false;
        spinRun->setMaximum(eval->getIndexMax(currentRecord));
        spinRun->setMinimum(eval->getIndexMin(currentRecord));
        //if (data->getCorrelationRuns()==1) spinRun->setMaximum(-1);
        spinRun->setValue(eval->getCurrentIndex());//currentRecord->getProperty(resultID+"_selected_run", -1).toInt());
        //if (data->getCorrelationRuns()>1)
        spinRun->setSuffix(QString(" / %2..%1").arg(eval->getIndexMax(currentRecord)).arg(eval->getIndexMin(currentRecord)));
        int newidx=cmbModel->findData(eval->getFitFunction()->id());
        if (newidx!=cmbModel->currentIndex()) modelChanged=true;
        cmbModel->setCurrentIndex(newidx);
        dataEventsEnabled=true;

    }
    displayModel(modelChanged);
    replotData();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::displayModel(bool newWidget) {
    if (!current) return;
    if (!cmbModel) return;
    //QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
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
                //disconnect(actEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
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
                //disconnect(actEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
                //disconnect(actEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(unsetEditValues(bool)));
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
        connect(tbEditRanges, SIGNAL(currentChanged(int)), header, SLOT(setEditRange(int)));
        connect(tbEditRanges, SIGNAL(currentChanged(int)), header, SLOT(unsetEditValues(int)));
        header->setEditRange(tbEditRanges->currentIndex());
        header->unsetEditValues(tbEditRanges->currentIndex());

        /////////////////////////////////////////////////////////////////////////////////////////////
        // create new parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////

        for (int i=0; i<ffunc->paramCount(); i++) {
            QString id=ffunc->getParameterID(i);

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

void QFFitResultsByIndexEvaluationEditorWithWidgets::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    if (!current->getHighlightedRecord()) return;
    setUserMinMax(userMin, userMax);
    replotData();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender) {
    if (!dataEventsEnabled) return;
    if (sender==pltData) {
        pltResiduals->setX(newxmin, newxmax);
        pltResiduals->update_plot();
    }
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::displayFitFunctionHelp()
{
    //hlpFunction->clear();
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
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
        QFPluginServices::getInstance()->displayHelpWindow(help);
        //hlpFunction->updateHelp(help);
        //hlpFunction->show();
    } else {
        QMessageBox::information(this, tr("FCS Fit"), tr("No Online-Help for this fit function available."));
    }
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::displayFitAlgorithmHelp()
{
    //hlpAlgorithm->clear();
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
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
        QFPluginServices::getInstance()->displayHelpWindow(help);
        //hlpAlgorithm->updateHelp(help);
        //hlpAlgorithm->show();
    } else {
        QMessageBox::information(this, tr("FCS Fit"), tr("No Online-Help for this fit algorithm available."));
    }
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::configFitAlgorithm()
{
    if (!current) return;
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
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

void QFFitResultsByIndexEvaluationEditorWithWidgets::runChanged(int run)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;

    //qDebug()<<"runChanged";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTime t;

    t.start();
    QFRawDataRecord* currentRecord=current->getHighlightedRecord();
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();
    if (data) labRun->setText(QString("  (%1)").arg(data->getIndexName(currentRecord, run)));
    else labRun->setText("");
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();

    data->setCurrentIndex(run);
    ////qDebug()<<t.elapsed()<<" ms";
    t.start();

    ////qDebug()<<t.elapsed()<<" ms";
    t.start();

    datacut->disableSliderSignals();
    datacut->set_min(getUserRangeMin(currentRecord, data->getCurrentIndex()));
    datacut->set_max(getUserRangeMax(currentRecord, data->getCurrentIndex()));
    datacut->set_userMin(getUserMin(getUserRangeMin(currentRecord, data->getCurrentIndex())));
    datacut->set_userMax(getUserMax(getUserRangeMax(currentRecord, data->getCurrentIndex())));
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

void QFFitResultsByIndexEvaluationEditorWithWidgets::modelChanged(int model)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    QString ff=cmbModel->itemData(cmbModel->currentIndex()).toString();
    data->setFitFunction(ff);
    displayModel(true);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::algorithmChanged(int model)
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    current->getHighlightedRecord()->setQFProperty("algorithm", cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString(), false, false);
    QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    QString alg=cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString();
    data->setFitAlgorithm(alg);
    populateFitButtons(data->getFitAlgorithm()->isThreadSafe());
    actFitAllThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());
    actFitAllFilesThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());
    actFitAllRunsThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::chkXLogScaleToggled(bool checked) {
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    current->setQFProperty("plot_taulog", chkXLogScale->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::chkGridToggled(bool checked)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    current->setQFProperty("plot_grid", chkGrid->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();

}

void QFFitResultsByIndexEvaluationEditorWithWidgets::plotStyleChanged(int style)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    current->setQFProperty("plot_style", cmbPlotStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::errorStyleChanged(int style)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    current->setQFProperty("plot_errorstyle", cmbErrorStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::residualStyleChanged(int style)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    current->setQFProperty("plot_residualsstyle", cmbResidualStyle->currentIndex(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::chkWeightedResidualsToggled(bool checked)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    current->setQFProperty("weighted_residuals", chkWeightedResiduals->isChecked(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::residualHistogramBinsChanged(int bins)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //QFFitResultsByIndexEvaluation* data=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    current->setQFProperty("plot_residualshistogrambins", spinResidualHistogramBins->value(), false, false);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::parameterValueChanged() {
    updateParameterValues();
    replotData();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::parameterValueEnterPressed() {

}

void QFFitResultsByIndexEvaluationEditorWithWidgets::parameterFixChanged() {
    updateParameterValues();
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::parameterRangeChanged() {
    updateParameterValues();
    replotData();
}



void QFFitResultsByIndexEvaluationEditorWithWidgets::fitCurrent() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!falg)) return;

    falg->setReporter(dlgFitProgressReporter);
    QString runname=tr("average");
    if (eval->getCurrentIndex()>=0) runname=QString::number(eval->getCurrentIndex());
    dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \n").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(100);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    dlgFitProgress->setAllowCancel(true);
    dlgFitProgress->display();
    QApplication::processEvents();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    eval->doFit(record, eval->getCurrentIndex(), getUserMin(record, eval->getCurrentIndex(), datacut->get_userMin()), getUserMax(record, eval->getCurrentIndex(), datacut->get_userMax()), dlgFitProgressReporter, ProgramOptions::getConfigValue(eval->getType()+"/log", false).toBool());
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

void QFFitResultsByIndexEvaluationEditorWithWidgets::fitRunsCurrent() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!eval)||(!falg)) return;


    int runmax=eval->getIndexMax(record);
    int runmin=eval->getIndexMin(record);
    dlgFitProgress->reportSuperStatus(tr("fit all runs<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(runmax-runmin);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgFitProgress->setAllowCancel(true);
    dlgFitProgress->display();
    QApplication::processEvents();
    QTime time;
    time.start();
    for (int run=runmin; run<=runmax; run++) {
        bool doall=!current->getProperty("leaveoutMasked", false).toBool();
        if (doall || (!doall && rsel && !rsel->leaveoutRun(run))) {
            falg->setReporter(dlgFitProgressReporter);
            QString runname=tr("average");
            if (run>=0) runname=QString::number(run);
            double runtime=double(time.elapsed())/1.0e3;
            double timeperfit=runtime/double(run-runmin);
            double estimatedRuntime=double(runmax-runmin)*timeperfit;
            double remaining=estimatedRuntime-runtime;
            dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \nruntime: %5:%6       remaining: %7:%8 [min:secs]       %9 fits/sec").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));

            //doFit(record, run);
            eval->doFit(record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()), dlgFitProgressReporter, ProgramOptions::getConfigValue(eval->getType()+"/log", false).toBool());

            dlgFitProgress->incSuperProgress();
            QApplication::processEvents();
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


void QFFitResultsByIndexEvaluationEditorWithWidgets::fitAll() {
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if ((!ffunc)||(!falg)) return;

    int run=eval->getCurrentIndex();
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
    QApplication::processEvents();


    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();

    // count the records and runs to work on (for proper superProgress
    int items=recs.size();
    dlgFitProgress->setSuperProgressMax(items);

    // iterate through all records and all runs therein and do the fits
    int jobsDone=0;
    QTime time;
    time.start();
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);
        //std::cout<<"i="<<i<<"   run="<<run<<"   record="<<record<<"   data="<<data<<"\n";
        int runmax=eval->getIndexMax(record);
        int runmin=eval->getIndexMin(record);

        if (record) {
            //std::cout<<"   corrN="<<data->getCorrelationN()<<"   corrRuns="<<data->getCorrelationRuns()<<"\n";
            bool doall=!current->getProperty("leaveoutMasked", false).toBool();
            if (run<=runmax && (doall || (!doall && rsel && !rsel->leaveoutRun(run)))) {
                falg->setReporter(dlgFitProgressReporter);
                double runtime=double(time.elapsed())/1.0e3;
                double timeperfit=runtime/double(jobsDone);
                double estimatedRuntime=double(items)*timeperfit;
                double remaining=estimatedRuntime-runtime;
                dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \nruntime: %5:%6       remaining: %7:%8 [min:secs]       %9 fits/sec").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(uint(int(runtime)%60)),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));

                //doFit(record, run);
                eval->doFit(record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()), dlgFitProgressReporter, ProgramOptions::getConfigValue(eval->getType()+"/log", false).toBool());

                falg->setReporter(NULL);
                QApplication::processEvents();
                if (dlgFitProgress->isCanceled()) break;
                jobsDone++;
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

void QFFitResultsByIndexEvaluationEditorWithWidgets::fitRunsAll() {
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
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
    QApplication::processEvents();

    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();

    // count the records and runs to work on (for proper superProgress
    int items=0;
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];

        if (record ) {
            int runmax=eval->getIndexMax(record);
            int runmin=eval->getIndexMin(record);
            items=items+runmax-runmin+1;
        }
    }
    dlgFitProgress->setSuperProgressMax(items);

    // iterate through all records and all runs therein and do the fits
    QTime time;
    time.start();
    int jobsDone=0;
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);

        if (record ) {
            record->setResultsInitSize(qMax(1000, items));
            record->setEvaluationIDMetadataInitSize(1000);
            int runmax=eval->getIndexMax(record);
            int runmin=eval->getIndexMin(record);
            for (int run=runmin; run<=runmax; run++) {
                bool doall=!current->getProperty("leaveoutMasked", false).toBool();
                if (run<=runmax && (doall || (!doall && rsel && !rsel->leaveoutRun(run)))) {
                    falg->setReporter(dlgFitProgressReporter);
                    QString runname=tr("average");
                    if (run>=0) runname=QString::number(run);
                    double runtime=double(time.elapsed())/1.0e3;
                    double timeperfit=runtime/double(jobsDone);
                    double estimatedRuntime=double(items)*timeperfit;
                    double remaining=estimatedRuntime-runtime;
                    dlgFitProgress->reportSuperStatus(tr("fit '%1', run %3<br>using model '%2'<br>and algorithm '%4' \nruntime: %5:%6       remaining: %7:%8 [min:secs]       %9 fits/sec").arg(record->getName()).arg(ffunc->name()).arg(runname).arg(falg->name()).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));

                    //doFit(record, run);
                    eval->doFit(record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()), dlgFitProgressReporter, ProgramOptions::getConfigValue(eval->getType()+"/log", false).toBool());

                    dlgFitProgress->incSuperProgress();
                    QApplication::processEvents();
                    falg->setReporter(NULL);
                    if (dlgFitProgress->isCanceled()) break;
                    jobsDone++;
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









void QFFitResultsByIndexEvaluationEditorWithWidgets::fitEverythingThreaded() {
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg->isThreadSafe()) {
        fitRunsAll();
        return;
    }
    if ((!ffunc)||(!falg)) return;

    dlgQFProgressDialog* dlgTFitProgress=new dlgQFProgressDialog(this);
    dlgTFitProgress->reportTask(tr("fit all files and all runs therein<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()));
    dlgTFitProgress->setProgressMax(100);
    dlgTFitProgress->setProgress(0);
    dlgTFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgTFitProgress->display();


    dlgTFitProgress->reportStatus("creating thread objects ...");
    QApplication::processEvents();
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    QList<QFFitResultsByIndexEvaluationFitThread*> threads;
    int threadcount=qMax(2,ProgramOptions::getInstance()->getMaxThreads());
    if (ProgramOptions::getConfigValue(eval->getType()+"/overrule_threads", false).toBool()) {
        threadcount=qMax(2,ProgramOptions::getConfigValue(eval->getType()+"/thrads", 1).toInt());
    }
    for (int i=0; i<threadcount; i++) {
        QFFitResultsByIndexEvaluationFitThread* t=new QFFitResultsByIndexEvaluationFitThread(true, this);
        connect (t, SIGNAL(sigLogError(QString)), this, SLOT(log_error(QString)));
        connect (t, SIGNAL(sigLogText(QString)), this, SLOT(log_text(QString)));
        connect (t, SIGNAL(sigLogWarning(QString)), this, SLOT(log_warning(QString)));
        threads.append(t);
    }


    // enqueue the jobs in the several threads and finally make sure they know when to stop
    dlgTFitProgress->reportStatus("distributing jobs ...");
    QApplication::processEvents();
    int items=0;
    int thread=0;
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);

        if (record ) {
            record->disableEmitResultsChanged();
            int runmax=eval->getIndexMax(record);
            int runmin=eval->getIndexMin(record);
            items=items+runmax-runmin+1;
            for (int run=runmin; run<=runmax; run++) {
                bool doall=!current->getProperty("leaveoutMasked", false).toBool();
                if (run<=runmax && (doall || (!doall && rsel && !rsel->leaveoutRun(run)))) {
                    threads[thread]->addJob(eval, record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()));
                    thread++;
                    if (thread>=threadcount) thread=0;
                }
            }
        }
        QApplication::processEvents();
    }
    dlgTFitProgress->setProgressMax(items+5);


    // start all threads and wait for them to finish
    for (int i=0; i<threadcount; i++) {
        threads[i]->start();
    }
    bool finished=false;
    bool canceled=false;
    QTime time;
    time.start();
    while (!finished) {


        // check whether all threads have finished and collect progress info
        finished=true;
        int jobsDone=0;
        for (int i=0; i<threadcount; i++) {
            finished=finished&&threads[i]->isFinished();
            jobsDone=jobsDone+threads[i]->getJobsDone();
        }
        dlgTFitProgress->setProgress(jobsDone);
        if (!canceled) {
            double runtime=double(time.elapsed())/1.0e3;
            double timeperfit=runtime/double(jobsDone);
            double estimatedRuntime=double(items)*timeperfit;
            double remaining=estimatedRuntime-runtime;
            dlgTFitProgress->reportStatus(tr("processing fits in %3 threads ... %1/%2 done\nruntime: %4:%5       remaining: %6:%7 [min:secs]       %8 fits/sec").arg(jobsDone).arg(items).arg(threadcount).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));
        }
        QApplication::processEvents();

        // check for user canceled
        if (!canceled && dlgTFitProgress->isCanceled()) {
            dlgTFitProgress->reportStatus("sending all threads the CANCEL signal");
            for (int i=0; i<threadcount; i++) {
                threads[i]->cancel(false);
            }
            canceled=true;
        }
    }


    // free memory
    for (int i=0; i<threadcount; i++) {
        delete threads[i];
    }

    dlgTFitProgress->reportStatus(tr("fit done ... updating user interface\n"));
    dlgTFitProgress->setProgress(items+2);
    QApplication::processEvents();

    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        if (record ) {
            record->enableEmitResultsChanged(true);
        }
    }
    current->emitResultsChanged();

    dlgTFitProgress->setProgress(items+3);
    QApplication::processEvents();
    displayModel(false);
    replotData();
    dlgTFitProgress->setProgress(items+5);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
    dlgTFitProgress->done();
    delete dlgTFitProgress;
}


void QFFitResultsByIndexEvaluationEditorWithWidgets::fitAllRunsThreaded() {
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg->isThreadSafe()) {
        fitRunsCurrent();
        return;
    }
    if ((!ffunc)||(!falg)) return;

    dlgQFProgressDialog* dlgTFitProgress=new dlgQFProgressDialog(this);
    dlgTFitProgress->reportTask(tr("fit all runs in the current file<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()));
    dlgTFitProgress->setProgressMax(100);
    dlgTFitProgress->setProgress(0);
    dlgTFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgTFitProgress->display();


    dlgTFitProgress->reportStatus("creating thread objects ...");
    QApplication::processEvents();
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    QList<QFFitResultsByIndexEvaluationFitThread*> threads;
    int threadcount=qMax(2,ProgramOptions::getInstance()->getMaxThreads());
    if (ProgramOptions::getConfigValue(eval->getType()+"/overrule_threads", false).toBool()) {
        threadcount=qMax(2,ProgramOptions::getConfigValue(eval->getType()+"/thrads", 1).toInt());
    }
    for (int i=0; i<threadcount; i++) {
        threads.append(new QFFitResultsByIndexEvaluationFitThread(true, this));
    }


    // enqueue the jobs in the several threads and finally make sure they know when to stop
    dlgTFitProgress->reportStatus("distributing jobs ...");
    QApplication::processEvents();
    int items=0;
    int thread=0;
    //for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=eval->getHighlightedRecord();
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);

        if (record ) {
            record->disableEmitResultsChanged();
            int runmax=eval->getIndexMax(record);
            int runmin=eval->getIndexMin(record);
            items=items+runmax-runmin+1;
            for (int run=runmin; run<=runmax; run++) {
                bool doall=!current->getProperty("LEAVEOUTMASKED", false).toBool();
                qDebug()<<doall;
                if (run<=runmax && (doall || (!doall && rsel && !rsel->leaveoutRun(run)))) {
                    threads[thread]->addJob(eval, record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()));
                    thread++;
                    if (thread>=threadcount) thread=0;
                }
            }
        }
    //}
    dlgTFitProgress->setProgressMax(items+5);


    // start all threads and wait for them to finish
    for (int i=0; i<threadcount; i++) {
        threads[i]->start();
    }
    bool finished=false;
    bool canceled=false;
    QTime time;
    time.start();
    while (!finished) {


        // check whether all threads have finished and collect progress info
        finished=true;
        int jobsDone=0;
        for (int i=0; i<threadcount; i++) {
            finished=finished&&threads[i]->isFinished();
            jobsDone=jobsDone+threads[i]->getJobsDone();
        }
        dlgTFitProgress->setProgress(jobsDone);
        if (!canceled) {
            double runtime=double(time.elapsed())/1.0e3;
            double timeperfit=runtime/double(jobsDone);
            double estimatedRuntime=double(items)*timeperfit;
            double remaining=estimatedRuntime-runtime;
            dlgTFitProgress->reportStatus(tr("processing fits in %3 threads ... %1/%2 done\nruntime: %4:%5       remaining: %6:%7 [min:secs]       %8 fits/sec").arg(jobsDone).arg(items).arg(threadcount).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));
        }
        QApplication::processEvents();

        // check for user canceled
        if (!canceled && dlgTFitProgress->isCanceled()) {
            dlgTFitProgress->reportStatus("sending all threads the CANCEL signal");
            for (int i=0; i<threadcount; i++) {
                threads[i]->cancel(false);
            }
            canceled=true;
        }
    }


    // free memory
    for (int i=0; i<threadcount; i++) {
        delete threads[i];
    }

    dlgTFitProgress->reportStatus(tr("fit done ... updating user interface\n"));
    dlgTFitProgress->setProgress(items+2);
    QApplication::processEvents();

    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        if (record ) {
            record->enableEmitResultsChanged(true);
        }
    }
    current->emitResultsChanged();

    dlgTFitProgress->setProgress(items+3);
    QApplication::processEvents();
    displayModel(false);
    replotData();
    dlgTFitProgress->setProgress(items+5);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
    dlgTFitProgress->done();
    delete dlgTFitProgress;
}

void QFFitResultsByIndexEvaluationEditorWithWidgets::fitAllFilesThreaded()
{
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg->isThreadSafe()) {
        fitAll();
        return;
    }
    if ((!ffunc)||(!falg)) return;

    dlgQFProgressDialog* dlgTFitProgress=new dlgQFProgressDialog(this);
    dlgTFitProgress->reportTask(tr("fit the current run in all files<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()));
    dlgTFitProgress->setProgressMax(100);
    dlgTFitProgress->setProgress(0);
    dlgTFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgTFitProgress->display();


    dlgTFitProgress->reportStatus("creating thread objects ...");
    QApplication::processEvents();
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    QList<QFFitResultsByIndexEvaluationFitThread*> threads;
    int threadcount=qMax(2,ProgramOptions::getInstance()->getMaxThreads());
    if (ProgramOptions::getConfigValue(eval->getType()+"/overrule_threads", false).toBool()) {
        threadcount=qMax(2,ProgramOptions::getConfigValue(eval->getType()+"/thrads", 1).toInt());
    }
    for (int i=0; i<threadcount; i++) {
        threads.append(new QFFitResultsByIndexEvaluationFitThread(true, this));
    }


    // enqueue the jobs in the several threads and finally make sure they know when to stop
    dlgTFitProgress->reportStatus("distributing jobs ...");
    QApplication::processEvents();
    int items=0;
    int thread=0;
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);

        if (record ) {
            record->disableEmitResultsChanged();
            items=items+1;
            int run=eval->getCurrentIndex();
            bool doall=!current->getProperty("leaveoutMasked", false).toBool();
            if (doall || (!doall && rsel && !rsel->leaveoutRun(run))) {
                threads[thread]->addJob(eval, record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()));
                thread++;
                if (thread>=threadcount) thread=0;
            }
        }
    }
    dlgTFitProgress->setProgressMax(items+5);


    // start all threads and wait for them to finish
    for (int i=0; i<threadcount; i++) {
        threads[i]->start();
    }
    bool finished=false;
    bool canceled=false;
    QTime time;
    time.start();
    while (!finished) {


        // check whether all threads have finished and collect progress info
        finished=true;
        int jobsDone=0;
        for (int i=0; i<threadcount; i++) {
            finished=finished&&threads[i]->isFinished();
            jobsDone=jobsDone+threads[i]->getJobsDone();
        }
        dlgTFitProgress->setProgress(jobsDone);
        if (!canceled) {
            double runtime=double(time.elapsed())/1.0e3;
            double timeperfit=runtime/double(jobsDone);
            double estimatedRuntime=double(items)*timeperfit;
            double remaining=estimatedRuntime-runtime;
            dlgTFitProgress->reportStatus(tr("processing fits in %3 threads ... %1/%2 done\nruntime: %4:%5       remaining: %6:%7 [min:secs]       %8 fits/sec").arg(jobsDone).arg(items).arg(threadcount).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));
        }
        QApplication::processEvents();

        // check for user canceled
        if (!canceled && dlgTFitProgress->isCanceled()) {
            dlgTFitProgress->reportStatus("sending all threads the CANCEL signal");
            for (int i=0; i<threadcount; i++) {
                threads[i]->cancel(false);
            }
            canceled=true;
        }
    }


    // free memory
    for (int i=0; i<threadcount; i++) {
        delete threads[i];
    }

    dlgTFitProgress->reportStatus(tr("fit done ... updating user interface\n"));
    dlgTFitProgress->setProgress(items+2);
    QApplication::processEvents();

    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        if (record ) {
            record->enableEmitResultsChanged(true);
        }
    }
    current->emitResultsChanged();

    dlgTFitProgress->setProgress(items+3);
    QApplication::processEvents();
    displayModel(false);
    replotData();
    dlgTFitProgress->setProgress(items+5);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
    dlgTFitProgress->done();
    delete dlgTFitProgress;
}
