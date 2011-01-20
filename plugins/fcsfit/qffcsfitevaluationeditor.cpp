#include "qffcsfitevaluationeditor.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "../fcs/qfrdrfcsdatainterface.h"
#include <iostream>
#include "qffcsfitevaluation.h"
#include "tools.h"

QFFCSFitEvaluationEditor::QFFCSFitEvaluationEditor(QFPluginServices* services, QWidget* parent):
    QFEvaluationEditor(services, parent)
{
    cmbModel=NULL;
    dataEventsEnabled=true;
    m_parameterWidgetWidth=75;
    m_parameterCheckboxWidth=32;

    createWidgets();
}

QFFCSFitEvaluationEditor::~QFFCSFitEvaluationEditor()
{
    //dtor
}


void QFFCSFitEvaluationEditor::createWidgets() {
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
    cmbWeights->addItem(tr("from std. dev."));
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

    toolbar=new QToolBar("toolbar_fcs_fit", this);
    vbl->addWidget(toolbar);
    actSaveReport=new QAction(QIcon(":/fcs_fit_savereport.png"), tr("Save Report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actPrintReport=new QAction(QIcon(":/fcs_fit_printreport.png"), tr("Print Report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    QLabel* lPS=new QLabel(tr("   &Plot Style: "), toolbar);
    cmbPlotStyle=new QComboBox(toolbar);
    cmbPlotStyle->addItem(QIcon(":/fcsplot_points.png"), tr("points"));
    cmbPlotStyle->addItem(QIcon(":/fcsplot_lines.png"), tr("lines"));
    cmbPlotStyle->addItem(QIcon(":/fcsplot_pointslines.png"), tr("lines + points"));
    lPS->setBuddy(cmbPlotStyle);
    QLabel* lES=new QLabel(tr("   &Error Style: "), toolbar);
    cmbErrorStyle=new QComboBox(toolbar);
    cmbErrorStyle->addItem(QIcon(":/fcsplot_enone.png"), tr("none"));
    cmbErrorStyle->addItem(QIcon(":/fcsplot_elines.png"), tr("lines"));
    cmbErrorStyle->addItem(QIcon(":/fcsplot_ebars.png"), tr("bars"));
    cmbErrorStyle->addItem(QIcon(":/fcsplot_elinesbars.png"), tr("bars + lines"));
    lES->setBuddy(cmbErrorStyle);
    QWidget* sp1=new QWidget(this);
    sp1->setMinimumWidth(10);
    sp1->setMaximumWidth(10);
    chkGrid=new QCheckBox(tr("Show &Grid   "), toolbar);
    chkXLogScale=new QCheckBox(tr("&logarithmic tau   "), toolbar);
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
    vbl->setMargin(0);
    widgetResiduals->setLayout(vbl);

    pltResiduals=new JKQtPlotter(true, this);
    pltResiduals->resize(300,50);
    vbl->addWidget(pltResiduals,1);

    pltData->set_gridPrinting(true);
    pltData->addGridPrintingPlotter(0,1,pltResiduals);
    pltData->set_displayToolbar(false);
    pltResiduals->set_displayToolbar(false);
    pltResiduals->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [{\\mu}s]"));
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
    pltResiduals->setBorder(1,1,1,1);
    pltData->setBorder(1,1,1,1);
    pltResiduals->synchronizeToMaster(pltData, true, false);
    pltData->set_useAntiAliasingForSystem(true);
    pltData->set_useAntiAliasingForGraphs(true);
    pltResiduals->set_useAntiAliasingForSystem(true);
    pltResiduals->set_useAntiAliasingForGraphs(true);
    pltData->set_displayMousePosition(false);
    pltResiduals->set_displayMousePosition(false);
    pltData->set_keyFontSize(9);
    pltData->set_keyXMargin(2);
    pltData->set_keyYMargin(2);
    pltResiduals->set_keyFontSize(9);
    pltResiduals->set_keyXMargin(2);
    pltResiduals->set_keyYMargin(2);


    datacut=new DataCutSliders(this);
    vbl->addWidget(datacut,0);
    splitPlot->addWidget(widgetResiduals);

    splitFitStatistics=new QSplitter(this);
    splitPlot->addWidget(splitFitStatistics);
    tabResidulas=new QTabWidget(this);
    tabResidulas->setTabPosition(QTabWidget::West);
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
    pltResidualHistogram->getYAxis()->set_axisLabel(tr("frequency"));
    pltResidualHistogram->getYAxis()->set_labelFontSize(8);
    pltResidualHistogram->getYAxis()->set_tickLabelFontSize(8);
    pltResidualHistogram->setBorder(0,0,0,0);
    pltResidualHistogram->set_useAntiAliasingForSystem(true);
    pltResidualHistogram->set_useAntiAliasingForGraphs(true);
    pltResidualHistogram->set_displayMousePosition(false);
    pltResidualHistogram->set_keyFontSize(8);
    pltResidualHistogram->set_keyXMargin(1);
    pltResidualHistogram->set_keyYMargin(1);
    pltResidualHistogram->getYAxis()->set_minTicks(5);
    pltResidualHistogram->getYAxis()->set_minTicks(5);
    tabResidulas->addTab(pltResidualHistogram, tr("Histogram"));
    //layFitStat->addWidget(pltResidualHistogram, 10);
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
    fl->addRow(tr("run: "), spinRun);
    layModel->addLayout(fl, 0);

    labFitParameters=new QLabel(this);
    layModel->addWidget(labFitParameters);
    btnEditRanges=new QPushButton(tr("Edit &Ranges"), this);
    btnEditRanges->setCheckable(true);
    QHBoxLayout* hblfp=new QHBoxLayout(this);
    hblfp->addWidget(labFitParameters);
    hblfp->addStretch();
    hblfp->addWidget(btnEditRanges);
    layModel->addLayout(hblfp);



    scrollParameters=new QScrollArea(this);
    layModel->addWidget(scrollParameters, 100);
    layModel->setMargin(2);
    QWidget* widParameters=new QWidget(this);
    scrollParameters->setWidget(widParameters);
    scrollParameters->setWidgetResizable(true);
    layParameters=new QFormLayout(this);
    layParameters->setMargin(2);
    widParameters->setLayout(layParameters);


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
    toolbar->addAction(pltData->get_actSavePlot()); pltData->get_actSavePlot()->setIcon(QIcon(":/fcsplot_saveplot.png"));
    toolbar->addAction(pltData->get_actSaveData()); pltData->get_actSaveData()->setIcon(QIcon(":/fcsplot_savedata.png"));
    toolbar->addAction(pltData->get_actPrint()); pltData->get_actPrint()->setIcon(QIcon(":/fcsplot_print.png"));
    toolbar->addSeparator();
    toolbar->addAction(pltData->get_actZoomAll()); pltData->get_actZoomAll()->setIcon(QIcon(":/fcsplot_zoomall.png"));
    connect(pltData->get_actZoomAll(), SIGNAL(riggered()), pltResiduals, SLOT(zoomToFit()));
    toolbar->addSeparator();
    toolbar->addWidget(lPS);
    toolbar->addWidget(cmbPlotStyle);
    toolbar->addWidget(lES);
    toolbar->addWidget(cmbErrorStyle);
    toolbar->addWidget(sp1);
    toolbar->addWidget(chkGrid);
    toolbar->addWidget(chkXLogScale);
    toolbar->addSeparator();
    toolbar->addWidget(sp2);
    toolbar->addWidget(labMousePosition);

    connect(btnAlgorithmHelp, SIGNAL(clicked()), this, SLOT(displayFitAlgorithmHelp()));
    connect(btnModelHelp, SIGNAL(clicked()), this, SLOT(displayFitFunctionHelp()));
    connect(pltData, SIGNAL(zoomChangedLocally(double, double, double, double, QWidget*)), this, SLOT(zoomChangedLocally(double, double, double, double, QWidget*)));
    connect(pltData, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));
    connect(pltResiduals, SIGNAL(plotMouseMove(double, double)), this, SLOT(plotMouseMove(double, double)));

}


void QFFCSFitEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    const QFFCSFitEvaluation* fcs=qobject_cast<QFFCSFitEvaluation*>(current);

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
        cmbAlgorithm->setCurrentIndex(cmbAlgorithm->findData(current->getProperty("algorithm", cmbAlgorithm->itemData(0)).toString()));
        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        cmbModel->setCurrentIndex(cmbModel->findData(current->getProperty("model", cmbModel->itemData(0)).toString()));
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

    displayModel(true);
    replotData();
}

void QFFCSFitEvaluationEditor::resultsChanged() {
}

void QFFCSFitEvaluationEditor::readSettings() {
    if (cmbModel) {
        //splitPlot->restoreState(settings->getQSettings()->value("fcsfitevaleditor/splitter_plot", splitPlot->saveState()).toByteArray());
        cmbAlgorithm->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/algorithm", cmbAlgorithm->currentIndex()).toInt());
        //cmbModel->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/model", cmbModel->currentIndex()).toInt());
        //cmbWeights->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/weights", cmbWeights->currentIndex()).toInt());
        pltData->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltdata/");
        pltResiduals->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresiduals/");
        pltResidualHistogram->loadSettings(*settings->getQSettings(), "fcsfitevaleditor/pltresidualhistogram/");
        hlpAlgorithm->readSettings(*settings->getQSettings(), "fcsfitevaleditor/algorithm_");
        hlpFunction->readSettings(*settings->getQSettings(), "fcsfitevaleditor/function_");
        loadSplitter(*(settings->getQSettings()), splitPlot, "fcsfitevaleditor/splitter_plot");
        loadSplitter(*(settings->getQSettings()), splitModel, "fcsfitevaleditor/splitter_model");
        loadSplitter(*(settings->getQSettings()), splitFitStatistics, "fcsfitevaleditor/splitter_fitstatistics");
        m_parameterWidgetWidth=settings->getQSettings()->value("fcsfitevaleditor/parameterWidgetWidth", m_parameterWidgetWidth).toInt();
        m_parameterCheckboxWidth=settings->getQSettings()->value("fcsfitevaleditor/parameterCheckboxWidth", m_parameterCheckboxWidth).toInt();
        btnEditRanges->setChecked(settings->getQSettings()->value("fcsfitevaleditor/display_range_widgets", m_parameterWidgetWidth).toBool());
        spinResidualHistogramBins->setValue(settings->getQSettings()->value("fcsfitevaleditor/residual_histogram_bins", 10).toInt());
        tabResidulas->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/residual_toolbox_current", 0).toInt());
    }
}

void QFFCSFitEvaluationEditor::writeSettings() {
    if (cmbModel) {
        //settings->getQSettings()->setValue("fcsfitevaleditor/splitter_plot", splitPlot->saveState());
        settings->getQSettings()->setValue("fcsfitevaleditor/algorithm", cmbAlgorithm->currentIndex());
        //settings->getQSettings()->setValue("fcsfitevaleditor/model", cmbModel->currentIndex());
        //settings->getQSettings()->setValue("fcsfitevaleditor/weights", cmbWeights->currentIndex());
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
    }
}

void QFFCSFitEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    QFFCSFitEvaluation* fcs=qobject_cast<QFFCSFitEvaluation*>(current);
    QFRDRFCSDataInterface* data=dynamic_cast<QFRDRFCSDataInterface*>(currentRecord);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));
    bool modelChanged=false;
    if (data) {
        //labRecord->setText(tr("<b>current:</b> <i>%1</i>").arg(currentRecord->getName()) );
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(replotData()));

        datacut->disableSliderSignals();
        datacut->set_min(0);
        datacut->set_max(data->getCorrelationN());
        datacut->set_userMin(currentRecord->getProperty(resultID+"_datacut_min", 0).toInt());
        datacut->set_userMax(currentRecord->getProperty(resultID+"_datacut_max", data->getCorrelationN()).toInt());
        datacut->enableSliderSignals();
        dataEventsEnabled=false;
        spinRun->setMaximum(data->getCorrelationRuns()-1);
        spinRun->setValue(fcs->getCurrentRun());//currentRecord->getProperty(resultID+"_selected_run", -1).toInt());
        int newidx=cmbModel->findData(fcs->getFitFunction()->id());
        if (newidx!=cmbModel->currentIndex()) modelChanged=true;
        cmbModel->setCurrentIndex(newidx);
        dataEventsEnabled=true;

    }
    displayModel(modelChanged);
    replotData();
}

void QFFCSFitEvaluationEditor::displayModel(bool newWidget) {
    if (!current) return;
    if (!cmbModel) return;
    QFRDRFCSDataInterface* data=dynamic_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFFCSFitEvaluation* eval=dynamic_cast<QFFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();

    if (newWidget) {
        /////////////////////////////////////////////////////////////////////////////////////////////
        // first delete all fit parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<m_fitParameters.size(); i++) {
            if (m_fitParameters[i]) {
                m_fitParameters[i]->disableDatastore();
                QWidget* label=layParameters->labelForField(m_fitParameters[i]);
                layParameters->removeWidget(label);
                layParameters->removeWidget(m_fitParameters[i]);
                disconnect(btnEditRanges, SIGNAL(toggled(bool)), m_fitParameters[i], SLOT(setEditRange(bool)));
                disconnect(m_fitParameters[i], SIGNAL(valueChanged(QString, double)), this, SLOT(parameterValueChanged(QString, double)));
                disconnect(m_fitParameters[i], SIGNAL(fixChanged(QString, bool)), this, SLOT(parameterFixChanged(QString, bool)));
                disconnect(m_fitParameters[i], SIGNAL(rangeChanged(QString, double, double)), this, SLOT(parameterRangeChanged(QString, double, double)));
                delete m_fitParameters[i];
                delete label;
            }
        }
        m_fitParameters.clear();

        /////////////////////////////////////////////////////////////////////////////////////////////
        // create header widget
        /////////////////////////////////////////////////////////////////////////////////////////////
        QFFitParameterWidget* header=new QFFitParameterWidget(eval, "", QFFitParameterWidget::Header, true, true, true, true, this);
        layParameters->addRow("", header);
        m_fitParameters.append(header);
        connect(btnEditRanges, SIGNAL(toggled(bool)), header, SLOT(setEditRange(bool)));
        header->setEditRange(btnEditRanges->isChecked());

        /////////////////////////////////////////////////////////////////////////////////////////////
        // create new parameter widgets
        /////////////////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<ffunc->paramCount(); i++) {
            QString id=ffunc->getParameterID(i);
            QFFitFunction::ParameterDescription d=ffunc->getDescription(i);
            QFFitParameterWidget::WidgetType wtype=QFFitParameterWidget::FloatEdit;
            if (d.type==QFFitFunction::IntNumber) wtype=QFFitParameterWidget::IntSpinBox;
            bool editable=d.userEditable;
            bool displayFix=d.userEditable;
            bool displayError=d.displayError;
            bool editRange=d.userEditable && d.userRangeEditable;
            if (!d.fit) {
                displayFix=false;
            }
            QFFitParameterWidget* fpw=new QFFitParameterWidget(eval, id, wtype, editable, displayFix, displayError, editRange, this);
            fpw->setUnit(d.unit);
            fpw->setIncrement(d.inc);
            fpw->setWidgetWidth(m_parameterWidgetWidth, m_parameterCheckboxWidth);
            QLabel* l=new QLabel(QString("<font size=\"+1\">%1:</font>").arg(d.label), this);
            if (!d.unit.isEmpty()) l->setText(QString("<font size=\"+1\">%1 [%2]:</font>").arg(d.label).arg(d.unit));
            fpw->setToolTip(d.name);
            l->setToolTip(d.name);
            l->setTextFormat(Qt::RichText);
            layParameters->addRow(l, fpw);
            m_fitParameters.append(fpw);
            connect(btnEditRanges, SIGNAL(toggled(bool)), fpw, SLOT(setEditRange(bool)));
            connect(fpw, SIGNAL(valueChanged(QString, double)), this, SLOT(parameterValueChanged(QString, double)));
            connect(fpw, SIGNAL(fixChanged(QString, bool)), this, SLOT(parameterFixChanged(QString, bool)));
            connect(fpw, SIGNAL(rangeChanged(QString, double, double)), this, SLOT(parameterRangeChanged(QString, double, double)));
            fpw->setEditRange(btnEditRanges->isChecked());
        }
    }
    if (eval->hasFit()) labFitParameters->setText(tr("<u>Local Fit Parameters:</u>"));
    else labFitParameters->setText(tr("<u>Global Fit Parameters:</u>"));
    updateParameterValues();
}

void QFFCSFitEvaluationEditor::parameterValueChanged(QString id, double value) {
    updateParameterValues();
    replotData();
}

void QFFCSFitEvaluationEditor::parameterFixChanged(QString id, bool fix) {
    updateParameterValues();
}

void QFFCSFitEvaluationEditor::parameterRangeChanged(QString id, double min, double max) {
    updateParameterValues();
}

void QFFCSFitEvaluationEditor::updateParameterValues() {
    if (!current) return;
    if (!cmbModel) return;
    QFRDRFCSDataInterface* data=dynamic_cast<QFRDRFCSDataInterface*>(current->getHighlightedRecord());
    QFFCSFitEvaluation* eval=dynamic_cast<QFFCSFitEvaluation*>(current);
    QFFitFunction* ffunc=eval->getFitFunction();

    if (!ffunc) return;

    double* values=eval->allocFillParameters();
    double* errors=eval->allocFillParameterErrors();
    ffunc->calcParameter(values, errors);


    for (int i=0; i<m_fitParameters.size(); i++) {
        if (m_fitParameters[i]) {
            //m_fitParameters[i]->reloadValues();
            QString id=m_fitParameters[i]->parameterID();
            int num=ffunc->getParameterNum(id);
            if (!id.isEmpty()) {
                if (num>=0) m_fitParameters[i]->setValue(values[num], errors[num], false);
                bool visible=ffunc->isParameterVisible(ffunc->getParameterNum(id), values);
                m_fitParameters[i]->setVisible(visible);
                layParameters->labelForField(m_fitParameters[i])->setVisible(visible);
            }
        }
    }

    free(values);
    free(errors);
}

void QFFCSFitEvaluationEditor::replotData() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=dynamic_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=dynamic_cast<QFFCSFitEvaluation*>(current);
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();

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
    dsres->clear();
    ds->clear();

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

    if (data->getCorrelationN()>0) {
        size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");


        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        size_t c_mean=0;
        QString graphName="";
        if (eval->getCurrentRun()<0) {
            c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
            graphName=tr("\\verb{%1} average").arg(record->getName());
        } else {
            if (eval->getCurrentRun()<data->getCorrelationRuns()) {
                c_mean=ds->addColumn(data->getCorrelationRun(eval->getCurrentRun()), data->getCorrelationN(), QString("run"+QString::number(eval->getCurrentRun())).toStdString());
                graphName=tr("\\verb{%2} run %1").arg(eval->getCurrentRun()).arg(record->getName());
            } else {
                c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                graphName=tr("\\verb{%1} average").arg(record->getName());
            }
        }
        size_t c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
        JKQTPerrorPlotstyle styl=JKQTPnoError;
        switch (errorStyle) {
            case 1: styl=JKQTPerrorLines; break;
            case 2: styl=JKQTPerrorBars; break;
            case 3: styl=JKQTPerrorBarsLines; break;
        }

        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(pltData);
        QColor gcolor=pltData->get_graphColor();
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
    }


    pltResiduals->set_doDrawing(true);
    pltResiduals->set_emitSignals(true);
    pltData->set_doDrawing(true);
    pltData->set_emitSignals(true);
    pltResiduals->update_plot();
    pltData->update_plot();
}

void QFFCSFitEvaluationEditor::updateFitFunctions() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=dynamic_cast<QFRDRFCSDataInterface*>(record);
    QFFCSFitEvaluation* eval=dynamic_cast<QFFCSFitEvaluation*>(current);
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    QFFitFunction* ffunc=eval->getFitFunction();

    if (!ffunc) return;

    if ((!eval)||(!data)) {
        return;
    }

    if (data->getCorrelationN()>0) {
        int c_tau=ds->getColumnNum("tau");
        if (c_tau>=0) { // we only add a graph, if we have a column with tau values
            long N=data->getCorrelationN();
            double* fitfunc=(double*)malloc(N*sizeof(double));
            double* residuals=(double*)malloc(N*sizeof(double));
            double* tauvals=data->getCorrelationT();
            double* corrdata=NULL;
            if (eval->getCurrentRun()<0) {
                corrdata=data->getCorrelationMean();
            } else {
                if (eval->getCurrentRun()<data->getCorrelationRuns()) {
                    corrdata=data->getCorrelationRun(eval->getCurrentRun());
                } else {
                    corrdata=data->getCorrelationMean();
                }
            }

            // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
            // we are working with a complete set of parameters
            double* values=eval->allocFillParameters();
            double* errors=eval->allocFillParameterErrors();
            ffunc->calcParameter(values, errors);

            double residSqrSum=0;

            for (int i=0; i<N; i++) {
                double value=ffunc->evaluate(tauvals[i], values);
                //di_fit->set(0, i, value);
                //di_resid->set(0,0,value)
                fitfunc[i]=value;
                residuals[i]=corrdata[i]-value;
                residSqrSum+=residuals[i]*residuals[i];
            }

            free(values);
            free(errors);

            size_t c_fit = ds->addCopiedColumn(fitfunc, N, "fit_model");
            free(fitfunc);
            size_t c_taures=dsres->addCopiedColumn(data->getCorrelationT(), N, "tau");
            size_t c_residuals=dsres->addCopiedColumn(residuals, N, "residuals");
            free(residuals);
            JKQTPxyLineGraph* g_fit=new JKQTPxyLineGraph(pltData);
            g_fit->set_drawLine(true);
            g_fit->set_title("fit function");
            g_fit->set_xColumn(c_tau);
            g_fit->set_yColumn(c_fit);
            g_fit->set_datarange_start(datacut->get_userMin());
            g_fit->set_datarange_end(datacut->get_userMax());
            pltData->addGraph(g_fit);

            JKQTPxyLineGraph* g_residuals=new JKQTPxyLineGraph(pltResiduals);
            g_residuals->set_drawLine(false);
            g_residuals->set_title("residuals");
            g_residuals->set_xColumn(c_taures);
            g_residuals->set_yColumn(c_residuals);
            g_residuals->set_symbolSize(8);
            g_residuals->set_symbolWidth(1);
            g_residuals->set_datarange_start(datacut->get_userMin());
            g_residuals->set_datarange_end(datacut->get_userMax());
            g_residuals->set_symbol(JKQTPcross);
            pltResiduals->addGraph(g_residuals);

            txtFitStatistics->setHtml(tr("<font size=\"+2\">&chi;<sup>2</sup></font> = %1").arg(residSqrSum));
        }
    }

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
        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter;
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(15,15,15,15,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setOutputFormat(QPrinter::PdfFormat);
        if (fi.suffix().toLower()=="ps") printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setOutputFileName(fn);
        QPainter painter;
        painter.begin(printer);
        pltData->draw(painter, printer->pageRect());
        painter.end();
        delete printer;
    }
}

void QFFCSFitEvaluationEditor::printReport() {
    QPrinter* p=new QPrinter;

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QPainter painter;
    painter.begin(p);
    pltData->draw(painter, p->pageRect());
    painter.end();

    delete p;
}

void QFFCSFitEvaluationEditor::chkXLogScaleToggled(bool checked) {
    if (!current) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_taulog", chkXLogScale->isChecked(), false, false);
    replotData();
}

void QFFCSFitEvaluationEditor::chkGridToggled(bool checked) {
    if (!current) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_grid", chkGrid->isChecked(), false, false);
    replotData();
}

void QFFCSFitEvaluationEditor::plotStyleChanged(int style) {
    if (!current) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_style", cmbPlotStyle->currentIndex(), false, false);
    replotData();
}

void QFFCSFitEvaluationEditor::errorStyleChanged(int style) {
    if (!current) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    current->setQFProperty("plot_errorstyle", cmbErrorStyle->currentIndex(), false, false);
    replotData();
}


void QFFCSFitEvaluationEditor::displayFitFunctionHelp() {
    hlpFunction->clear();
    if (!current) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    QStringList sl;
    sl<<":/";
    QString ppid=cmbModel->itemData(cmbModel->currentIndex()).toString();
    int pid=services->getFitFunctionManager()->getPluginForID(ppid);
    QString dll=services->getFitFunctionManager()->getFilename(pid);
    sl<<QFileInfo(dll).absolutePath()+QString("/help/")+QFileInfo(dll).completeBaseName()+QString("/");
    //std::cout<<sl[1].toStdString()<<std::endl;
    hlpFunction->setSearchPath(sl);
    if (data->getFitFunction(ppid)->helpFile().isEmpty()) hlpFunction->updateHelp(data->getFitFunction(ppid)->name(), data->getFitFunction(ppid)->id()+".html");
    else hlpFunction->updateHelp(data->getFitFunction(ppid)->name(), data->getFitFunction(ppid)->helpFile());
    hlpFunction->show();
}

void QFFCSFitEvaluationEditor::displayFitAlgorithmHelp() {
    hlpAlgorithm->clear();
    if (!current) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    QStringList sl;
    sl<<":/";
    QString pid=cmbModel->itemData(cmbModel->currentIndex()).toString();
    QString dll=services->getFitAlgorithmManager()->getFilename(pid);
    sl<<QFileInfo(dll).absolutePath()+QString("/help/")+QFileInfo(dll).completeBaseName()+QString("/");
    //std::cout<<sl[1].toStdString()<<std::endl;
    hlpAlgorithm->setSearchPath(sl);
    if (data->getFitAlgorithm(pid)->helpFile().isEmpty()) hlpAlgorithm->updateHelp(data->getFitAlgorithm(pid)->name(), data->getFitAlgorithm(pid)->id()+".html");
    else hlpAlgorithm->updateHelp(data->getFitAlgorithm(pid)->name(), data->getFitAlgorithm(pid)->helpFile());
    hlpAlgorithm->show();
}

void QFFCSFitEvaluationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    current->getHighlightedRecord()->setQFProperty(resultID+"_datacut_min", userMin, false, false);
    current->getHighlightedRecord()->setQFProperty(resultID+"_datacut_max", userMax, false, false);
    replotData();
}

void QFFCSFitEvaluationEditor::runChanged(int run) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    data->setCurrentRun(run);
    displayModel(false);
    replotData();
}

void QFFCSFitEvaluationEditor::modelChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSFitEvaluation* data=dynamic_cast<QFFCSFitEvaluation*>(current);
    QString ff=cmbModel->itemData(cmbModel->currentIndex()).toString();
    data->setFitFunction(ff);
    displayModel(true);
    replotData();
}

void QFFCSFitEvaluationEditor::weightsChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    current->getHighlightedRecord()->setQFProperty("weights", cmbWeights->currentIndex(), false, false);
}

void QFFCSFitEvaluationEditor::algorithmChanged(int model) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    current->getHighlightedRecord()->setQFProperty("algorithm", cmbAlgorithm->itemData(cmbAlgorithm->currentIndex()).toString(), false, false);
}

void QFFCSFitEvaluationEditor::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, QWidget* sender) {
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

