#include "qfrdrimagingfcs_dataeditor_image.h"
#include "qfrdrimagingfcs_data.h"
#include <QDebug>
#include <math.h>
#include "jkqttools.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "datatable2.h"
#include "jkimage.h"
#include "qmoretextobject.h"
#include "statistics_tools.h"
#include "jkqtpelements.h"





QFRDRImagingFCSImageEditor::QFRDRImagingFCSImageEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    m_fitFunctions=services->getFitFunctionManager()->getModels("", this);
    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plteOverviewSize=0;
    plteImageData=NULL;
    plteGofImageData=NULL;
    plteImageSize=0;
    lastSavePath="";
    connectImageWidgetsCounter=0;
    connectParameterWidgetsCounter=0;
    createWidgets();
    //QTimer::singleShot(500, this, SLOT(debugInfo()));
}

QFRDRImagingFCSImageEditor::~QFRDRImagingFCSImageEditor()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    plteOverviewSelectedData=NULL;
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
    plteOverviewExcludedData=NULL;
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
    if (plteGofImageData) free(plteGofImageData);
    plteGofImageData=NULL;
}




void QFRDRImagingFCSImageEditor::createWidgets() {
    QLabel* l;
    QGridLayout* lb=new QGridLayout(this);
    setLayout(lb);



    QGroupBox* grpTop=new QGroupBox(tr(" Parameter Image Plot Settings "), this);
    QGridLayout* topgrid=new QGridLayout(this);
    grpTop->setLayout(topgrid);

    int row=0;
    cmbResultGroup=new QComboBox(this);
    topgrid->addWidget((l=new QLabel(tr("&result set:"))), row, 0);
    l->setBuddy(cmbResultGroup);
    topgrid->addWidget(cmbResultGroup, row, 1);

    cmbParameter=new QComboBox(this);
    topgrid->addWidget((labParameter=new QLabel(tr("&parameter:"))), row, 3);
    labParameter->setBuddy(cmbParameter);
    topgrid->addWidget(cmbParameter, row, 4);
    cmbParameterTransform=new QComboBox(this);
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/none.png"), tr("none"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/abs.png"), tr("abs"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/log.png"), tr("log"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/reci.png"), tr("reciprocal"));
    cmbParameterTransform->addItem(QIcon(":/imaging_fcs/sqrt.png"), tr("sqrt"));
    topgrid->addWidget((labParameterTransform=new QLabel(tr("    &transform:"))), row, 5);
    labParameterTransform->setBuddy(cmbParameterTransform);
    topgrid->addWidget(cmbParameterTransform, row, 6);

    row++;
    cmbGofParameter=new QComboBox(this);
    topgrid->addWidget((labGofParameter=new QLabel(tr("&goodness of fit:"))), row, 3);
    labGofParameter->setBuddy(cmbGofParameter);
    topgrid->addWidget(cmbGofParameter, row, 4);
    cmbGofParameterTransform=new QComboBox(this);
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/none.png"), tr("none"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/abs.png"), tr("abs"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/log.png"), tr("log"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/reci.png"), tr("reciprocal"));
    cmbGofParameterTransform->addItem(QIcon(":/imaging_fcs/sqrt.png"), tr("sqrt"));
    topgrid->addWidget((labGofParameterTransform=new QLabel(tr("    tr&ansform:"))), row, 5);
    labGofParameterTransform->setBuddy(cmbGofParameterTransform);
    topgrid->addWidget(cmbGofParameterTransform, row, 6);



    topgrid->addWidget(new QWidget(), 0, 2);
    topgrid->addWidget(new QWidget(), 0, 5);
    topgrid->addWidget(new QWidget(), 0, 7);
    topgrid->setColumnStretch(0, 0);
    topgrid->setColumnStretch(1, 0);
    topgrid->setColumnStretch(2, 1);
    topgrid->setColumnStretch(3, 0);
    topgrid->setColumnStretch(4, 0);
    topgrid->setColumnStretch(5, 0);
    topgrid->setColumnStretch(6, 0);
    topgrid->setColumnStretch(7, 1);


    QWidget* w=new QWidget(this);
    QVBoxLayout* vbl=new QVBoxLayout();
    w->setLayout(vbl);

    QGroupBox* wimg=new QGroupBox(tr(" parameter image style "), this);
    vbl->addWidget(wimg);
    QFormLayout* gli=new QFormLayout(this);
    wimg->setLayout(gli);
    cmbColorbar=new QComboBox(wimg);
    QStringList sl=JKQTFPimagePlot_getPalettes();
    for (int i=0; i<sl.size(); i++) {
        cmbColorbar->addItem(JKQTFPimagePlot_getPaletteIcon(i), sl[i]);
    }
    gli->addRow(tr("color &palette:"), cmbColorbar);

    chkImageAutoScale=new QCheckBox("auto", wimg);
    gli->addRow(tr("color &range:"), chkImageAutoScale);

    edtColMin=new JKDoubleEdit(wimg);
    edtColMin->setCheckBounds(false, false);
    edtColMax=new JKDoubleEdit(wimg);
    edtColMax->setCheckBounds(false, false);
    QHBoxLayout* coll=new QHBoxLayout();
    coll->addWidget(edtColMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtColMax,1);
    coll->setContentsMargins(0,0,0,0);
    gli->addRow(QString(""), coll);

    chkDisplayImageOverlay=new QCheckBox(wimg);
    gli->addRow(tr("show &overlays:"), chkDisplayImageOverlay);






    QGroupBox* wcp=new QGroupBox(tr(" correlation plot styles "), this);
    vbl->addWidget(wcp);
    QFormLayout* gl=new QFormLayout(this);
    wcp->setLayout(gl);
    chkDisplayAverage=new QCheckBox(w);
    chkDisplayAverage->setChecked(true);
    gl->addRow(tr("display &average:"), chkDisplayAverage);

    cmbAverageStyle=new QComboBox(w);
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbAverageErrorStyle=new QComboBox(w);
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));

    gl->addRow((l=new QLabel(tr("average &options:"))), cmbAverageStyle);
    gl->addRow("", cmbAverageErrorStyle);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageErrorStyle, SLOT(setEnabled(bool)));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));

    cmbRunStyle=new QComboBox(w);
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbRunErrorStyle=new QComboBox(w);
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));

    gl->addRow((labRunOptions=new QLabel(tr("pixel &options:"), w)), cmbRunStyle);
    labRunOptions->setBuddy(cmbRunStyle);
    gl->addRow("", cmbRunErrorStyle);

    chkLogTauAxis=new QCheckBox("", w);
    gl->addRow((l=new QLabel(tr("log &tau;-axis:"))), chkLogTauAxis);
    l->setTextFormat(Qt::RichText);

    chkDisplayResiduals=new QCheckBox(w);
    chkDisplayResiduals->setChecked(true);
    gl->addRow(tr("display &residuals:"), chkDisplayResiduals);

    chkKeys=new QCheckBox(w);
    chkKeys->setChecked(true);
    gl->addRow(tr("display &keys:"), chkKeys);

    connectImageWidgets(true);



    /*btnDontUse=new QPushButton(tr("exclude sel."), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    gl->addWidget(btnDontUse, row, 1);
    btnUse=new QPushButton(tr("include sel."), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    gl->addWidget(btnUse, row, 2);
    row++;*/




    QWidget* wpltOverview=new QWidget(this);
    QVBoxLayout* lpltOverview=new QVBoxLayout();
    wpltOverview->setLayout(lpltOverview);
    pltOverview=new JKQtPlotter(wpltOverview);
    lpltOverview->addWidget(new QLabel(tr("Overview:")));
    lpltOverview->addWidget(pltOverview, 1);
    pltOverview->set_zoomByDoubleAndRightMouseClick(false);
    pltOverview->set_displayMousePosition(false);
    pltOverview->set_displayToolbar(true);
    pltOverview->get_plotter()->set_maintainAspectRatio(true);
    pltOverview->get_plotter()->set_aspectRatio(1);
    pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
    pltOverview->get_plotter()->set_axisAspectRatio(1);
    pltOverview->setXY(0,0,1,1);
    pltOverview->setAbsoluteXY(0,1,0,1);

    pltOverview->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltOverview->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltOverview->get_plotter()->getXAxis()->set_axisLabel("");
    pltOverview->get_plotter()->getYAxis()->set_axisLabel("");
    pltOverview->get_plotter()->setGrid(false);
    pltOverview->get_plotter()->set_useAntiAliasingForSystem(true);
    pltOverview->get_plotter()->set_useAntiAliasingForGraphs(true);
    connect(pltOverview, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltOverview, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));

    plteOverview=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::UInt16Array, NULL, 0,0, JKQTPMathImage::GRAY, pltOverview->get_plotter());
    pltOverview->addGraph(plteOverview);

    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.5);
    plteOverviewSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltOverview->get_plotter());
    pltOverview->addGraph(plteOverviewSelected);


    QColor ovlExCol=QColor("blue");
    ovlExCol.setAlphaF(0.5);
    plteOverviewExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltOverview->get_plotter());
    pltOverview->addGraph(plteOverviewExcluded);

    plteImageData=NULL;













    QWidget* wpltImage=new QWidget(this);
    QVBoxLayout* lpltImage=new QVBoxLayout();
    wpltImage->setLayout(lpltImage);
    pltImage=new JKQtPlotter(wpltImage);
    lpltImage->addWidget((labParamImage=new QLabel(tr("Parameter Image:"))));
    lpltImage->addWidget(pltImage, 1);

    pltImage->set_zoomByDoubleAndRightMouseClick(false);
    pltImage->set_displayMousePosition(false);
    pltImage->set_displayToolbar(true);
    pltImage->get_plotter()->set_maintainAspectRatio(true);
    pltImage->get_plotter()->set_aspectRatio(1);
    pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltImage->get_plotter()->set_axisAspectRatio(1);
    pltImage->setXY(0,0,1,1);
    pltImage->setAbsoluteXY(0,1,0,1);

    pltImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltImage->get_plotter()->setGrid(false);
    pltImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    connect(pltImage, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));

    connect(pltImage, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));

    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::GRAY, pltImage->get_plotter());
    pltImage->addGraph(plteImage);


    plteImageSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltImage->get_plotter());
    //pltImage->addGraph(plteImageSelected);


    plteImageExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltImage->get_plotter());
    //pltImage->addGraph(plteImageExcluded);

    plteImageData=NULL;














    QWidget* wpltGofImage=new QWidget(this);
    QVBoxLayout* lpltGofImage=new QVBoxLayout();
    wpltGofImage->setLayout(lpltGofImage);
    pltGofImage=new JKQtPlotter(wpltGofImage);
    lpltGofImage->addWidget((labParamImage=new QLabel(tr("\"Goodnes of Fit\" Image:"))));
    lpltGofImage->addWidget(pltGofImage, 1);

    pltGofImage->set_zoomByDoubleAndRightMouseClick(false);
    pltGofImage->set_displayMousePosition(false);
    pltGofImage->set_displayToolbar(true);
    pltGofImage->get_plotter()->set_maintainAspectRatio(true);
    pltGofImage->get_plotter()->set_aspectRatio(1);
    pltGofImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltGofImage->get_plotter()->set_axisAspectRatio(1);
    pltGofImage->setXY(0,0,1,1);
    pltGofImage->setAbsoluteXY(0,1,0,1);

    pltGofImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltGofImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltGofImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltGofImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltGofImage->get_plotter()->setGrid(false);
    pltGofImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltGofImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    connect(pltGofImage, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));

    connect(pltGofImage, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));

    plteGofImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::GRAY, pltGofImage->get_plotter());
    pltGofImage->addGraph(plteGofImage);


    plteGofImageSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltGofImage->get_plotter());
    plteGofImageExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltGofImage->get_plotter());

    plteImageData=NULL;








    plotter = new JKQtPlotter(true, this);
    plotter->get_plotter()->set_showKey(true);
    plotterResid = new JKQtPlotter(true, this);
    plotterResid->get_plotter()->set_showKey(true);


    plotter->get_plotter()->set_gridPrinting(true);
    plotter->get_plotter()->addGridPrintingPlotter(0,1,plotterResid->get_plotter());
    plotter->set_displayToolbar(true);
    plotterResid->set_displayToolbar(false);
    plotterResid->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    plotterResid->getXAxis()->set_labelFontSize(11);
    plotterResid->getXAxis()->set_tickLabelFontSize(10);
    plotterResid->getYAxis()->set_axisLabel(tr("residuals"));
    plotterResid->getYAxis()->set_labelFontSize(11);
    plotterResid->getYAxis()->set_tickLabelFontSize(10);
    plotter->getXAxis()->set_axisLabel("");
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->getYAxis()->set_labelFontSize(11);
    plotter->getYAxis()->set_tickLabelFontSize(10);
    plotter->getXAxis()->set_drawMode1(JKQTPCADMticks);
    plotter->getXAxis()->set_tickLabelFontSize(10);
    plotterResid->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    plotterResid->get_plotter()->setBorder(1,1,1,1);
    plotter->get_plotter()->setBorder(1,1,1,1);
    plotterResid->synchronizeToMaster(plotter, true, false);
    plotter->get_plotter()->set_useAntiAliasingForSystem(true);
    plotter->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotterResid->get_plotter()->set_useAntiAliasingForSystem(true);
    plotterResid->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotter->set_displayMousePosition(false);
    plotterResid->set_displayMousePosition(false);
    plotter->get_plotter()->set_keyFontSize(9);
    plotter->get_plotter()->set_keyXMargin(2);
    plotter->get_plotter()->set_keyYMargin(2);
    plotterResid->get_plotter()->set_keyFontSize(9);
    plotterResid->get_plotter()->set_keyXMargin(2);
    plotterResid->get_plotter()->set_keyYMargin(2);
    plotterResid->useExternalDatastore(plotter->getDatastore());
    connect(plotter, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(acfZoomChangedLocally(double,double,double,double,JKQtPlotter*)));



    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout(this);
    lp->setContentsMargins(0,0,0,0);
    wp->setLayout(lp);
    lp->addWidget(plotterResid,1);
    sliders=new DataCutSliders(this);
    connect(sliders, SIGNAL(slidersChanged(int , int , int, int)), this, SLOT(slidersChanged(int, int, int, int)));
    lp->addWidget(sliders);


    tvParams=new QEnhancedTableView();
    tvParams->setItemDelegate(new QFHTMLDelegate(tvParams));
    tvParams->setAlternatingRowColors(true);
    QFontMetrics tvfm(font());
    tvParams->verticalHeader()->setDefaultSectionSize((int)round((double)tvfm.height()*1.5));
    tabFitvals=new QFTableModel(tvParams);
    tabFitvals->setReadonly(true);



    splitterTopBot=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterTop=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitterTop->addWidget(wpltImage);
    splitterTop->addWidget(wpltGofImage);
    splitterTop->addWidget(wpltOverview);
    splitterTopBot->addWidget(splitterTop);
    splitterBotPlots=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterBotPlots->addWidget(plotter);
    splitterBotPlots->addWidget(wp);
    splitterBot=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitterBot->addWidget(splitterBotPlots);
    splitterBot->addWidget(tvParams);
    splitterTopBot->addWidget(splitterBot);

    vbl->addWidget(new QWidget(this), 1);









    QGridLayout* grdTop=new QGridLayout(this);
    btnPrintReport = new QPushButton(QIcon(":/imaging_fcs/report_print.png"), tr("&Print report"), this);
    connect(btnPrintReport, SIGNAL(clicked()), this, SLOT(printReport()));
    btnSaveReport = new QPushButton(QIcon(":/imaging_fcs/report_save.png"), tr("&Save report"), this);
    connect(btnSaveReport, SIGNAL(clicked()), this, SLOT(saveReport()));
    btnSaveData = new QPushButton(QIcon(":/imaging_fcs/preview_savedata.png"), tr("Save &data"), this);
    connect(btnSaveData, SIGNAL(clicked()), this, SLOT(saveData()));
    grdTop->addWidget(grpTop, 0, 1, 3, 1);
    grdTop->addWidget(btnSaveData, 0, 0);
    grdTop->addWidget(btnSaveReport, 1, 0);
    grdTop->addWidget(btnPrintReport, 2, 0);
    grdTop->setColumnStretch(1,0);





    //////////////////////////////////////////////////////////////////////////////////////////
    // ACF TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widACFs=new QWidget(this);
    QGridLayout* layACFs=new QGridLayout();
    widACFs->setLayout(layACFs);
    layACFs->addWidget(splitterTopBot, 0, 0);
    layACFs->addWidget(w, 0, 1);
    layACFs->setColumnStretch(0,5);
    layACFs->setContentsMargins(0,0,0,0);



    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widHist=new QWidget(this);
    QGridLayout* layHist=new QGridLayout();
    widHist->setLayout(layHist);

    // HISTOGRAM SETTINGS/////////////////////////////////////////////////////////////////////
    grpHistogramSettings=new QGroupBox(tr("histogram style"), this);
    QFormLayout* flHistSet=new QFormLayout(grpHistogramSettings);
    grpHistogramSettings->setLayout(flHistSet);
    spinHistogramBins=new QSpinBox(this);
    spinHistogramBins->setRange(5,99999);
    spinHistogramBins->setValue(25);
    flHistSet->addRow(tr("# bins:"), spinHistogramBins);
    chkLogHistogram=new QCheckBox("", grpHistogramSettings);
    flHistSet->addRow(tr("log-scale:"), chkLogHistogram);
    chkNormalizedHistograms=new QCheckBox("", grpHistogramSettings);
    flHistSet->addRow(tr("normalized:"), chkNormalizedHistograms);
    chkExcludeExcludedRunsFromHistogram=new QCheckBox("", grpHistogramSettings);
    flHistSet->addRow(tr("mind excluded runs:"), chkExcludeExcludedRunsFromHistogram);
    chkExcludeExcludedRunsFromHistogram->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    chkHistogramRangeAuto=new QCheckBox("auto", grpHistogramSettings);
    flHistSet->addRow(tr("range:"), chkHistogramRangeAuto);
    edtHistogramMin=new JKDoubleEdit(wimg);
    edtHistogramMin->setCheckBounds(false, false);
    edtHistogramMin->setValue(0);
    edtHistogramMax=new JKDoubleEdit(wimg);
    edtHistogramMax->setCheckBounds(false, false);
    edtHistogramMax->setValue(10);
    coll=new QHBoxLayout();
    coll->addWidget(edtHistogramMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtHistogramMax,1);
    coll->setContentsMargins(0,0,0,0);
    flHistSet->addRow(QString(""), coll);

    // HISTOGRAM PLOTS ///////////////////////////////////////////////////////////////////////
    splitterHistogram=new QVisibleHandleSplitter(this);
    pltParamHistogram=new JKQtPlotter(this);
    tvHistogramParameters=new QEnhancedTableView(this);
    tabHistogramParameters=new QFTableModel(this);
    tabHistogramParameters->setReadonly(false);
    tabHistogramParameters->clear();
    tabHistogramParameters->appendColumn();
    tabHistogramParameters->setColumnTitle(0, tr("parameter"));
    tabHistogramParameters->appendColumn();
    tabHistogramParameters->setColumnTitle(1, tr("complete histogram"));
    tabHistogramParameters->appendColumn();
    tabHistogramParameters->setColumnTitle(2, tr("selection histogram"));
    for (int r=0; r<8; r++) tabHistogramParameters->appendRow();
    tabHistogramParameters->setCell(0, 0, tr("data points N"));
    tabHistogramParameters->setCell(1, 0, tr("average"));
    tabHistogramParameters->setCell(2, 0, tr("median"));
    tabHistogramParameters->setCell(3, 0, tr("std. dev. &sigma;"));
    tabHistogramParameters->setCell(4, 0, tr("min"));
    tabHistogramParameters->setCell(5, 0, tr("25% quantile"));
    tabHistogramParameters->setCell(6, 0, tr("75% quantile"));
    tabHistogramParameters->setCell(7, 0, tr("max"));
    tabHistogramParameters->setReadonly(true);

    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->setItemDelegate(new QFHTMLDelegate(tvParams));
    tvHistogramParameters->setAlternatingRowColors(true);
    tvHistogramParameters->verticalHeader()->setDefaultSectionSize((int)round((double)tvfm.height()*1.5));



    splitterHistogram->addWidget(pltParamHistogram);
    splitterHistogram->addWidget(tvHistogramParameters);
    layHist->addWidget(splitterHistogram, 0, 0);
    layHist->addWidget(grpHistogramSettings, 0, 1);
    layHist->setColumnStretch(0,5);
    layHist->setContentsMargins(0,0,0,0);








    //////////////////////////////////////////////////////////////////////////////////////////
    // PUT EVERYTHING TOGETHER
    //////////////////////////////////////////////////////////////////////////////////////////
    tabDisplay=new QTabWidget(this);
    tabDisplay->addTab(widACFs, tr("&Correlations"));
    tabDisplay->addTab(widHist, tr("&Histogram"));


    //lb->addWidget(grpTop, 1, 0, 1, 2);
    lb->addLayout(grdTop, 0,0);
    lb->addWidget(tabDisplay, 1,0);





    connectParameterWidgets();
};

void QFRDRImagingFCSImageEditor::saveImageSettings() {
    if (current){
        QString egroup=filenameize(currentEvalGroup());
        QString param=filenameize(currentFitParameter());
        if (egroup.isEmpty() || param.isEmpty()) {
            writeSettings();
        } else {
            current->setQFProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param), cmbColorbar->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), chkDisplayImageOverlay->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), chkImageAutoScale->isChecked(), false, false);
            if (!chkImageAutoScale->isChecked()) {
                current->setQFProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), edtColMin->value(), false, false);
                current->setQFProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), edtColMax->value(), false, false);
            }
            current->setQFProperty(QString("imfcs_imed_histbins_%1_%2").arg(egroup).arg(param), spinHistogramBins->value(), false, false);
            current->setQFProperty(QString("imfcs_imed_histnorm_%1_%2").arg(egroup).arg(param), chkNormalizedHistograms->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_histlog_%1_%2").arg(egroup).arg(param), chkLogHistogram->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_histex_%1_%2").arg(egroup).arg(param), chkExcludeExcludedRunsFromHistogram->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_histrauto_%1_%2").arg(egroup).arg(param), chkHistogramRangeAuto->isChecked(), false, false);
            if (!chkHistogramRangeAuto->isChecked()) {
                current->setQFProperty(QString("imfcs_imed_histrmin_%1_%2").arg(egroup).arg(param), edtHistogramMin->value(), false, false);
                current->setQFProperty(QString("imfcs_imed_histrmax_%1_%2").arg(egroup).arg(param), edtHistogramMax->value(), false, false);
            }
        }

    }
}

void QFRDRImagingFCSImageEditor::loadImageSettings() {
    if (current){
        QString egroup=filenameize(currentEvalGroup());
        QString param=filenameize(currentFitParameter());
        if (egroup.isEmpty() || param.isEmpty()) {
            writeSettings();
        } else if (settings) {
            double mi=0, ma=1;
            connectParameterWidgets(false);
            plteImage->getDataMinMax(mi, ma);
            int d=current->getProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param),
                                       settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex())).toInt();
            if (d>=0) cmbColorbar->setCurrentIndex(d);
            else if (cmbColorbar->count()>0) cmbColorbar->setCurrentIndex(0);
            chkDisplayImageOverlay->setChecked(current->getProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), false).toBool());
            chkImageAutoScale->setChecked(current->getProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), true).toBool());
            if (!chkImageAutoScale->isChecked()) {
                edtColMin->setValue(current->getProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), mi).toDouble());
                edtColMax->setValue(current->getProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), ma).toDouble());
            }
            spinHistogramBins->setValue(current->getProperty(QString("imfcs_imed_histbins_%1_%2").arg(egroup).arg(param), 25).toInt());
            chkNormalizedHistograms->setChecked(current->getProperty(QString("imfcs_imed_histnorm_%1_%2").arg(egroup).arg(param), true).toBool());
            chkLogHistogram->setChecked(current->getProperty(QString("imfcs_imed_histlog_%1_%2").arg(egroup).arg(param), false).toBool());
            chkExcludeExcludedRunsFromHistogram->setChecked(current->getProperty(QString("imfcs_imed_histex_%1_%2").arg(egroup).arg(param), true).toBool());
            chkHistogramRangeAuto->setChecked(current->getProperty(QString("imfcs_imed_histrauto_%1_%2").arg(egroup).arg(param), true).toBool());
            if (!chkHistogramRangeAuto->isChecked()) {
                edtHistogramMin->setValue(current->getProperty(QString("imfcs_imed_histrmin_%1_%2").arg(egroup).arg(param), 0).toDouble());
                edtHistogramMax->setValue(current->getProperty(QString("imfcs_imed_histrmax_%1_%2").arg(egroup).arg(param), 10).toDouble());
            }

            connectParameterWidgets(true);
            pltImage->update_plot();
        }

    }
}

void QFRDRImagingFCSImageEditor::displayOverlayChanged() {
    if (!chkDisplayImageOverlay->isChecked()) {
        pltImage->deleteGraph(plteImageSelected, false);
        pltImage->deleteGraph(plteImageExcluded, false);
        pltGofImage->deleteGraph(plteGofImageSelected, false);
        pltGofImage->deleteGraph(plteGofImageExcluded, false);
    } else {
        pltImage->addGraph(plteImageSelected);
        pltImage->addGraph(plteImageExcluded);
        pltGofImage->addGraph(plteGofImageSelected);
        pltGofImage->addGraph(plteGofImageExcluded);
    }
    saveImageSettings();
    //pltImage->update_data();
}

void QFRDRImagingFCSImageEditor::paletteChanged() {
    plteImage->set_palette(cmbColorbar->currentIndex());
    plteImage->set_autoImageRange(chkImageAutoScale->isChecked());
    edtColMin->setEnabled(!chkImageAutoScale->isChecked());
    edtColMax->setEnabled(!chkImageAutoScale->isChecked());
    if (!chkImageAutoScale->isChecked()) {
        plteImage->set_imageMin(edtColMin->value());
        plteImage->set_imageMax(edtColMax->value());
    } else {
        double mi=0, ma=0;
        plteImage->getDataMinMax(mi, ma);
        edtColMin->setValue(mi);
        edtColMax->setValue(ma);
    }
    saveImageSettings();
    pltImage->update_plot();
}

void QFRDRImagingFCSImageEditor::histogramSettingsChanged() {
    edtHistogramMin->setEnabled(!chkHistogramRangeAuto->isChecked());
    edtHistogramMax->setEnabled(!chkHistogramRangeAuto->isChecked());
    saveImageSettings();
    updateHistogram();
}

void QFRDRImagingFCSImageEditor::excludeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutAddRun(run);
        }
        m->recalcCorrelations();
    }
    replotData();
    updateHistogram();
}

void QFRDRImagingFCSImageEditor::includeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutRemoveRun(run);
        }
        m->recalcCorrelations();
    }
    replotData();
    updateHistogram();
}

void QFRDRImagingFCSImageEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        disconnect(current, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
        disconnect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        connect(current, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        sliders->disableSliderSignals();
        sliders->set_min(0);
        sliders->set_max(m->getCorrelationN());
        sliders->set_userMin(current->getProperty("imfcs_imed_datacut_min", 0).toInt());
        sliders->set_userMax(current->getProperty("imfcs_imed_datacut_max", m->getCorrelationN()).toInt());
        sliders->enableSliderSignals();
        selected.clear();
        selected.insert(0);
    } else {
//        runs.setCurrent(current);
        selected.clear();
    }

    fillParameterSet();

    //readSettings();
    //qDebug()<<"connectWidgets ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}


void QFRDRImagingFCSImageEditor::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int idx=m->xyToRun(xx, yy);

    if (modifiers==Qt::ControlModifier) {
        if (selected.contains(idx)) selected.remove(idx);
        else selected.insert(idx);
    } else {
        selected.clear();
        selected.insert(idx);
    }
    replotData();
    replotSelection(true);
    updateSelectionHistogram(true);
}

void QFRDRImagingFCSImageEditor::rawDataChanged() {
    replotSelection(false);
    replotOverview();
    replotImage();
    replotData();
    updateHistogram();
};

void QFRDRImagingFCSImageEditor::resultsChanged() {
    QString egroup=currentEvalGroup();
    QString fp=currentFitParameter();
    QString gfp=currentGofParameter();
    fillParameterSet();
    int grp=cmbResultGroup->findData(egroup);
    if (grp>=0) {
        cmbResultGroup->setCurrentIndex(grp);
        int p=cmbParameter->findData(fp);
        if (p>=0) {
            cmbParameter->setCurrentIndex(p);
        } else if (cmbParameter->count()>0) {
            cmbParameter->setCurrentIndex(0);
        }

        p=cmbGofParameter->findData(gfp);
        if (p>=0) {
            cmbGofParameter->setCurrentIndex(p);
        } else if (cmbGofParameter->count()>0) {
            cmbGofParameter->setCurrentIndex(0);
        }
    } else if (cmbResultGroup->count()>0) {
        cmbResultGroup->setCurrentIndex(0);
    }
};

void QFRDRImagingFCSImageEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    current->setQFProperty("imfcs_imed_datacut_min", userMin, false, false);
    current->setQFProperty("imfcs_imed_datacut_max", userMax, false, false);
    //replotData();

    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        plotterResid->clearGraphs();
        ds->clear();
        return;
    }

    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    plotterResid->set_doDrawing(false);
    plotterResid->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    plotter->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());
    plotterResid->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());
    plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(),false);
    plotterResid->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    plotterResid->setX(plotter->getXMin(), plotter->getXMax());
    plotterResid->zoomToFit(false, true, !chkLogTauAxis->isChecked(),false);

    QApplication::restoreOverrideCursor();
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotter->update_plot();
    plotterResid->set_doDrawing(true);
    plotterResid->set_emitSignals(true);
    plotterResid->update_plot();

}

void QFRDRImagingFCSImageEditor::replotImage() {
    //qDebug()<<"replotImage";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);



    if (!m) {
        plteImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteGofImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        //qDebug()<<"replotImage !m";
    } else {
        double w=m->getDataImageWidth();
        double h=m->getDataImageHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }
        //qDebug()<<w<<h;
        /*double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));*/

        pltImage->setAbsoluteXY(0, w, 0, h);
        pltImage->get_plotter()->set_maintainAspectRatio(true);
        pltImage->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        pltImage->get_plotter()->set_axisAspectRatio(1.0*w/h);
        pltGofImage->setAbsoluteXY(0, w, 0, h);
        pltGofImage->get_plotter()->set_maintainAspectRatio(true);
        pltGofImage->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltGofImage->get_plotter()->set_maintainAxisAspectRatio(true);
        pltGofImage->get_plotter()->set_axisAspectRatio(1.0*w/h);

        if (w>3*h) {
            pltImage->get_plotter()->getXAxis()->set_minTicks(3);
            plteImage->get_colorBarAxis()->set_minTicks(3);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(3);
            plteGofImage->get_colorBarAxis()->set_minTicks(3);
        } else {
            pltImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteImage->get_colorBarAxis()->set_minTicks(5);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteGofImage->get_colorBarAxis()->set_minTicks(5);
        }
        pltImage->setXY(0, w, 0, h);
        pltGofImage->setXY(0, w, 0, h);

        if (plteImageSize<m->getDataImageWidth()*m->getDataImageHeight()) {
            plteImageSize=m->getDataImageWidth()*m->getDataImageHeight();
            plteImageData=(double*)realloc(plteImageData, plteImageSize*sizeof(double));
            plteGofImageData=(double*)realloc(plteGofImageData, plteImageSize*sizeof(double));
        }

        readParameterImage(plteImageData, plteGofImageData, m->getDataImageWidth(), m->getDataImageHeight(), currentEvalGroup(), currentFitParameter(), currentFitParameterTransfrom(), currentGofParameter(), currentGofParameterTransfrom());

        plteImage->set_data(plteImageData, m->getDataImageWidth(), m->getDataImageHeight(), JKQTPMathImageBase::DoubleArray);
        plteImage->set_width(w);
        plteImage->set_height(h);
        plteImage->set_palette(cmbColorbar->currentIndex());

        plteGofImage->set_data(plteGofImageData, m->getDataImageWidth(), m->getDataImageHeight(), JKQTPMathImageBase::DoubleArray);
        plteGofImage->set_width(w);
        plteGofImage->set_height(h);

    }

    pltImage->set_doDrawing(true);
    pltImage->update_plot();
    pltGofImage->set_doDrawing(true);
    pltGofImage->update_plot();
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotImage ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::updateSelectionArrays() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //qDebug()<<"updateSelectionArrays";
    //pltOverview->set_doDrawing(false);


    if (m) {
        int siz=m->getDataImageWidth()*m->getDataImageHeight();
        if (plteOverviewSize<m->getDataImageWidth()*m->getDataImageHeight()) {
            plteOverviewSize=m->getDataImageWidth()*m->getDataImageHeight();
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSize*sizeof(bool));
            plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plteOverviewSize*sizeof(bool));
        }
        for (register int i=0; i<siz; i++) {
            int x=m->runToX(i);
            int y=m->runToY(i);
            int idx=y*m->getDataImageWidth()+x;
            plteOverviewSelectedData[idx]=selected.contains(i);
            plteOverviewExcludedData[idx]=m->leaveoutRun(i);
        }
    } else {
        for (register int i=0; i<plteOverviewSize; i++) {
            plteOverviewSelectedData[i]=false;
            plteOverviewExcludedData[i]=false;
        }
    }
    //qDebug()<<"updateSelectionArrays ... end";
}

void QFRDRImagingFCSImageEditor::replotSelection(bool replot) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //qDebug()<<"replotSelection";
    pltOverview->set_doDrawing(false);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);
    updateSelectionArrays();

    if (!chkDisplayImageOverlay->isChecked()) {
        pltImage->deleteGraph(plteImageSelected, false);
        pltImage->deleteGraph(plteImageExcluded, false);
        pltGofImage->deleteGraph(plteGofImageSelected, false);
        pltGofImage->deleteGraph(plteGofImageExcluded, false);
    } else {
        pltImage->addGraph(plteImageSelected);
        pltImage->addGraph(plteImageExcluded);
        pltGofImage->addGraph(plteGofImageSelected);
        pltGofImage->addGraph(plteGofImageExcluded);
    }

    if (!m) {
        plteOverviewSelected->set_data(NULL, 1, 1);
        plteOverviewExcluded->set_data(NULL, 1, 1);
        plteImageSelected->set_data(NULL, 1, 1);
        plteImageExcluded->set_data(NULL, 1, 1);
        plteGofImageSelected->set_data(NULL, 1, 1);
        plteGofImageExcluded->set_data(NULL, 1, 1);
    } else {
        //uint16_t* ov=m->getDataImagePreview();
        double w=m->getDataImageWidth();
        double h=m->getDataImageHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }

        plteOverviewSelected->set_width(w);
        plteOverviewSelected->set_height(h);
        plteOverviewSelected->set_data(plteOverviewSelectedData, m->getDataImageWidth(), m->getDataImageHeight());

        plteOverviewExcluded->set_width(w);
        plteOverviewExcluded->set_height(h);
        plteOverviewExcluded->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());

        plteImageSelected->set_width(w);
        plteImageSelected->set_height(h);
        plteImageSelected->set_data(plteOverviewSelectedData, m->getDataImageWidth(), m->getDataImageHeight());

        plteImageExcluded->set_width(w);
        plteImageExcluded->set_height(h);
        plteImageExcluded->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());

        plteGofImageSelected->set_width(w);
        plteGofImageSelected->set_height(h);
        plteGofImageSelected->set_data(plteOverviewSelectedData, m->getDataImageWidth(), m->getDataImageHeight());

        plteGofImageExcluded->set_width(w);
        plteGofImageExcluded->set_height(h);
        plteGofImageExcluded->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());


    }

    pltOverview->set_doDrawing(true);
    pltImage->set_doDrawing(true);
    pltGofImage->set_doDrawing(true);
    if (replot) {
        pltOverview->update_plot();
        pltImage->update_plot();
        pltGofImage->update_plot();
    }
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotSelection ... end";
}

void QFRDRImagingFCSImageEditor::replotOverview() {
    //qDebug()<<"replotOverview";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltOverview->set_doDrawing(false);

    if (!m) {
        plteOverview->set_data(NULL, 1, 1, JKQTPMathImageBase::UInt16Array);
        plteOverviewSelected->set_data(NULL, 1, 1);
        plteOverviewExcluded->set_data(NULL, 1, 1);
    } else {
        //uint16_t* ov=m->getDataImagePreview();
        double w=m->getDataImageWidth();
        double h=m->getDataImageHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }

        //qDebug()<<"replotOverview()  w="<<w<<"   h="<<h;
        pltOverview->setAbsoluteXY(0, w, 0, h);
        pltOverview->get_plotter()->set_maintainAspectRatio(true);
        pltOverview->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltOverview->get_plotter()->set_maintainAxisAspectRatio(true);
        pltOverview->get_plotter()->set_axisAspectRatio(1.0*w/h);


        if (w>3*h) {
            pltOverview->get_plotter()->getXAxis()->set_minTicks(3);
            plteOverview->get_colorBarAxis()->set_minTicks(3);
        } else {
            pltOverview->get_plotter()->getXAxis()->set_minTicks(7);
            plteOverview->get_colorBarAxis()->set_minTicks(5);
        }
        pltOverview->setXY(0, w, 0, h);

        if (m->getDataImagePreview()) plteOverview->set_data(m->getDataImagePreview(), m->getDataImageWidth(), m->getDataImageHeight(), JKQTPMathImageBase::UInt16Array);
        else plteOverview->set_data(NULL, m->getDataImageWidth(), m->getDataImageHeight(), JKQTPMathImageBase::UInt16Array);
        plteOverview->set_width(w);
        plteOverview->set_height(h);
    }

    pltOverview->set_doDrawing(true);
    pltOverview->update_plot();
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotOverview ... done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::replotData() {
    //qDebug()<<"replotData";
    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    plotterResid->setVisible(chkDisplayResiduals->isChecked());

    if (!m) {
        plotter->clearGraphs();
        plotterResid->clearGraphs();
        ds->clear();
        //qDebug()<<"replotData ... done (!m) ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    plotter->clearGraphs();
    plotterResid->clearGraphs();
    plotter->get_plotter()->set_showKey(chkKeys->isChecked());
    plotterResid->get_plotter()->set_showKey(chkKeys->isChecked());
    ds->clear();


    if (m->getCorrelationN()>0) {


        JKQTPerrorPlotstyle runerrorstyle=JKQTPnoError;
        switch (cmbRunErrorStyle->currentIndex()) {
            case 1: runerrorstyle=JKQTPerrorBars; break;
            case 2: runerrorstyle=JKQTPerrorLines; break;
            case 3: runerrorstyle=JKQTPerrorBarsLines; break;
        }
        bool runLine=(cmbRunStyle->currentIndex()!=1);
        JKQTPgraphSymbols runSymbol=JKQTPcross;
        if (cmbRunStyle->currentIndex()==0) runSymbol=JKQTPnoSymbol;

        JKQTPerrorPlotstyle avgerrorstyle=JKQTPnoError;
        switch (cmbAverageErrorStyle->currentIndex()) {
            case 1: avgerrorstyle=JKQTPerrorBars; break;
            case 2: avgerrorstyle=JKQTPerrorLines; break;
            case 3: avgerrorstyle=JKQTPerrorBarsLines; break;
        }
        bool avgLine=(cmbAverageStyle->currentIndex()!=1);
        JKQTPgraphSymbols avgSymbol=JKQTPcross;
        if (cmbAverageStyle->currentIndex()==0) avgSymbol=JKQTPnoSymbol;



        size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");


        //QTime t;
        //t.start();
        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        if (chkDisplayAverage->isChecked()) {
            size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), "cmean");
            size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), "cstddev");

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
            //g->set_color(QColor("blue"));
            g->set_errorColor(g->get_color().lighter());
            g->set_lineWidth(2);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(tr("average"));
            g->set_yErrorStyle(avgerrorstyle);
            g->set_xErrorStyle(JKQTPnoError);
            g->set_datarange_start(sliders->get_userMin());
            g->set_datarange_end(sliders->get_userMax());
            g->set_drawLine(avgLine);
            g->set_symbolSize(5);
            g->set_symbol(avgSymbol);
            plotter->addGraph(g);
        }

       //////////////////////////////////////////////////////////////////////////////////
       // Plot ALL RUNS (left out runs in gray)
       // also evaluates and plots the fit function (if possible) + the residuals
       //////////////////////////////////////////////////////////////////////////////////
        tvParams->setModel(NULL);
        tabFitvals->setReadonly(false);
        tabFitvals->clear();
        tabFitvals->appendColumn();
        tabFitvals->setColumnTitle(0, tr("parameter"));
        for (int i=0; i<m->getCorrelationRuns(); i++) {
            if (selected.contains(i)) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("pixel error %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                g->set_lineWidth(1);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_drawLine(runLine);
                g->set_symbol(runSymbol);
                g->set_title(tr("run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                g->set_datarange_start(sliders->get_userMin());
                g->set_datarange_end(sliders->get_userMax());
                g->set_yErrorColumn(c_rune);
                g->set_yErrorStyle(runerrorstyle);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_symbolSize(5);
                g->set_errorWidth(1);
                if (m->leaveoutRun(i)) {
                    g->set_color(QColor("grey"));
                }
                g->set_errorColor(g->get_color().lighter());
                plotter->addGraph(g);


                double* corr=(double*)calloc(m->getCorrelationN(), sizeof(double));
                double* resid=(double*)calloc(m->getCorrelationN(), sizeof(double));
                QStringList names, units, unitlabels, namelabels;
                QList<double> values, errors;
                QList<bool> fix;

                // search for the evaluationID that matches the current group(has to be in evals)  and run (i)
                QStringList evals=current->resultsCalcEvaluationsInGroup(currentEvalGroup());
                QString en="";
                for (register int ev=0; ev<evals.size(); ev++) {
                    //en=evals[i];
                    if (current->resultsGetEvaluationGroupIndex(evals[ev])==i) {
                        en=evals[ev];
                        break;
                    }
                }

                // try to evaluate the fit function. If it succeeds, add plots and store the parameters & description to the display model!
                if (evaluateFitFunction(m->getCorrelationT(), corr, m->getCorrelationN(), names, namelabels, values, errors, fix, units, unitlabels, en)) {
                    double* acf=m->getCorrelationRun(i);
                    for (int nn=0; nn< m->getCorrelationN(); nn++) {
                        resid[nn]=corr[nn]-acf[nn];
                    }
                    size_t c_fit=ds->addCopiedColumn(corr, m->getCorrelationN(), QString("fit to pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    size_t c_resid=ds->addCopiedColumn(resid, m->getCorrelationN(), QString("residuals for pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    JKQTPxyLineGraph* gfit=new JKQTPxyLineGraph();
                    gfit->set_lineWidth(1.5);
                    gfit->set_xColumn(c_tau);
                    gfit->set_yColumn(c_fit);
                    gfit->set_drawLine(true);
                    gfit->set_style(Qt::DotLine);
                    gfit->set_symbol(JKQTPnoSymbol);
                    gfit->set_title(tr("fit to run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                    gfit->set_datarange_start(sliders->get_userMin());
                    gfit->set_datarange_end(sliders->get_userMax());
                    gfit->set_symbolSize(5);
                    gfit->set_color(g->get_color());
                    plotter->addGraph(gfit);
                    JKQTPxyLineGraph* gr=new JKQTPxyLineGraph();
                    gr->set_lineWidth(1.5);
                    gr->set_xColumn(c_tau);
                    gr->set_yColumn(c_resid);
                    gr->set_drawLine(true);
                    gr->set_style(Qt::DotLine);
                    gr->set_symbol(JKQTPnoSymbol);
                    gr->set_title(tr("residuals for run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                    gr->set_datarange_start(sliders->get_userMin());
                    gr->set_datarange_end(sliders->get_userMax());
                    gr->set_symbolSize(5);
                    gr->set_color(g->get_color());
                    plotterResid->addGraph(gr);

                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    int col=tabFitvals->columnCount()-1;
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, tr("error"));
                    tabFitvals->appendColumn();
                    tabFitvals->setColumnTitle(tabFitvals->columnCount()-1, QString(""));
                    for (int ii=0; ii<names.size(); ii++) {
                        int row=tabFitvals->getAddRow(0, namelabels[ii]);
                        tabFitvals->setCellEditRole(row, 0, names[ii]);
                        tabFitvals->setCell(row, col, values[ii]);
                        tabFitvals->setCell(row, col+1, errors[ii]);
                        tabFitvals->setCell(row, col+2, unitlabels[ii]);
                        tabFitvals->setCellEditRole(row, col+2, units[ii]);
                        QBrush c=QBrush(g->get_color().lighter(170));
                        tabFitvals->setCellBackgroundRole(row, col, c);
                        tabFitvals->setCellBackgroundRole(row, col+1, c);
                        tabFitvals->setCellBackgroundRole(row, col+2, c);
                        if (fix[ii]) tabFitvals->setCellCheckedRole(row, col, Qt::Checked);
                        else tabFitvals->setCellCheckedRole(row, col, Qt::Unchecked);
                    }

                }

                free(corr);
                free(resid);
            }
        }
        tabFitvals->setReadonly(true);
        tvParams->setModel(tabFitvals);
        tvParams->resizeColumnsToContents();





        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(),false);
        plotterResid->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotterResid->setX(plotter->getXMin(), plotter->getXMax());
        plotterResid->zoomToFit(false, true, !chkLogTauAxis->isChecked(),false);

    }
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    //QTime t;
    //t.start();
    plotter->update_plot();
    //qDebug()<<"plotting in "<<t.elapsed()<<" ms";
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotData ... done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
};


void QFRDRImagingFCSImageEditor::readSettings() {
    if (!settings) return;
    connectParameterWidgets(false);
    connectImageWidgets(false);
    lastSavePath=settings->getQSettings()->value(QString("imfcsimageeditor/last_save_path"), lastSavePath).toString();
    plotter->loadSettings(*(settings->getQSettings()), QString("imfcsimageeditor/corrplot"));
    chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_tau_axis"), true).toBool());
    chkKeys->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_keys"), false).toBool());
    chkDisplayResiduals->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_resid"), true).toBool());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_avg"), true).toBool());
    cmbAverageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_style"), 0).toInt());
    cmbAverageErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_error_style"), 2).toInt());
    cmbColorbar->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), 0).toInt());
    chkDisplayImageOverlay->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlays"), false).toBool());
    cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_style"), 1).toInt());
    cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_error_style"), 1).toInt());
    //chkLogHistogram->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_histogram"), false).toBool());
    //chkNormalizedHistograms->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/normalized_histograms"), true).toBool());
    //chkExcludeExcludedRunsFromHistogram->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/exclude_in_histograms"), true).toBool());
    loadSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    loadSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    loadSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    loadSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
    loadSplitter(*(settings->getQSettings()), splitterHistogram, "imfcsimageeditor/splitterhistogramSizes");
    connectParameterWidgets(true);
    rawDataChanged();
    connectImageWidgets(true);
}


void QFRDRImagingFCSImageEditor::writeSettings() {
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfcsimageeditor/last_save_path"), lastSavePath);
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/autoscale"), chkImageAutoScale->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/colmin"), edtColMin->value());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/colmax"), edtColMax->value());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/log_tau_axis"), chkLogTauAxis->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_keys"), chkKeys->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/image_overlays"), chkDisplayImageOverlay->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_resid"), chkDisplayResiduals->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_avg"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_style"), cmbAverageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_error_style"), cmbAverageErrorStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_style"), cmbRunStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/log_histogram"), chkLogHistogram->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/normalized_histograms"), chkNormalizedHistograms->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/exclude_in_histograms"), chkExcludeExcludedRunsFromHistogram->isChecked());
    saveSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    saveSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    saveSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
    saveSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    saveSplitter(*(settings->getQSettings()), splitterHistogram, "imfcsimageeditor/splitterhistogramSizes");
}


void QFRDRImagingFCSImageEditor::parameterSetChanged() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    int grp=cmbResultGroup->currentIndex();
    if ((!m)||(grp<0)) {
        labParameter->setEnabled(false);
        cmbParameter->setEnabled(false);
        labParameterTransform->setEnabled(false);
        cmbParameterTransform->setEnabled(false);
        labGofParameter->setEnabled(false);
        cmbGofParameter->setEnabled(false);
        labGofParameterTransform->setEnabled(false);
        cmbGofParameterTransform->setEnabled(false);
        clearImage();
        cmbParameter->clear();
        cmbGofParameter->clear();
        //cmbParameter->setCurrentIndex(-1);
    } else {
        connectParameterWidgets(false);
        m->setQFProperty("imfcs_imed_evalgroup", grp, false, false);
        labParameter->setEnabled(true);
        cmbParameter->setEnabled(true);
        labParameterTransform->setEnabled(true);
        cmbParameterTransform->setEnabled(true);
        labGofParameter->setEnabled(true);
        cmbGofParameter->setEnabled(true);
        labGofParameterTransform->setEnabled(true);
        cmbGofParameterTransform->setEnabled(true);
        //QStringList egroup=m->resultsCalcEvalGroups();
        QString egroup=currentEvalGroup();
        QList<QPair<QString, QString> > params=m->resultsCalcNamesAndLabels("", "fit results", egroup);
        cmbParameter->clear();
        cmbGofParameter->clear();
        for (int i=0; i<params.size(); i++) {
            cmbParameter->addItem(tr("fit result: %1").arg(params[i].first), params[i].second);
            cmbGofParameter->addItem(tr("fit result: %1").arg(params[i].first), params[i].second);
        }
        params=m->resultsCalcNamesAndLabels("", "fit properties", egroup);
        for (int i=0; i<params.size(); i++) {
            cmbParameter->addItem(tr("fit prop.: %1").arg(params[i].first), params[i].second);
            cmbGofParameter->insertItem(0, tr("fit prop.: %1").arg(params[i].first), params[i].second);
        }
        int d=cmbParameter->findData(current->getProperty(QString("imfcs_imed_param_%1").arg(filenameize(egroup)), ""));
        if (d>=0) cmbParameter->setCurrentIndex(d);
        d=cmbGofParameter->findData(current->getProperty(QString("imfcs_imed_gofparam_%1").arg(filenameize(egroup)), "fitalg_error_sum"));
        if (d>=0) cmbGofParameter->setCurrentIndex(d);
        cmbGofParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), 0).toInt());
        cmbParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), 0).toInt());
        connectParameterWidgets(true);

    }
    //qDebug()<<"parameterSetChanged ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
    parameterChanged();
    replotData();
    //updateHistogram();
    //qDebug()<<"parameterSetChanged ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();
}

void QFRDRImagingFCSImageEditor::parameterChanged() {
    if (!current) return;
    QString egroup=currentEvalGroup();

    connectParameterWidgets(false);
    cmbGofParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), 0).toInt());
    cmbParameterTransform->setCurrentIndex(current->getProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), 0).toInt());

    current->setQFProperty(QString("imfcs_imed_param_%1").arg(filenameize(egroup)), cmbParameter->itemData(cmbParameter->currentIndex()).toString(), false, false);
    current->setQFProperty(QString("imfcs_imed_gofparam_%1").arg(filenameize(egroup)), cmbGofParameter->itemData(cmbGofParameter->currentIndex()).toString(), false, false);
    connectParameterWidgets(true);

    transformChanged();
}

void QFRDRImagingFCSImageEditor::transformChanged() {
    QString egroup=currentEvalGroup();
    current->setQFProperty(QString("imfcs_imed_gofparamtrans_%1_%2").arg(filenameize(egroup)).arg(cmbGofParameter->currentIndex()), cmbGofParameterTransform->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_paramtrans_%1_%2").arg(filenameize(egroup)).arg(cmbParameter->currentIndex()), cmbParameterTransform->currentIndex(), false, false);
    connectParameterWidgets(false);
    replotOverview();
    replotImage();
    loadImageSettings();
    paletteChanged();
    displayOverlayChanged();
    connectParameterWidgets(true);
    updateHistogram();
}


void QFRDRImagingFCSImageEditor::clearImage() {
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
    plteImageSize=0;
    if (plteGofImageData) free(plteGofImageData);
    plteGofImageData=NULL;
}

void QFRDRImagingFCSImageEditor::fillParameterSet() {
    //qDebug()<<"fillParameterSet";

    if (!current) {
        //cmbResultGroup->setEnabled(false);
        cmbResultGroup->clear();
        //cmbResultGroup->setCurrentIndex(-1);
    } else {
        connectParameterWidgets(false);
        //cmbResultGroup->setEnabled(true);
        cmbResultGroup->clear();

        // fill the list of available fit result groups
        QStringList egroups=current->resultsCalcEvalGroups();
        //qDebug()<<"egroups="<<egroups;
        for (int i=0; i<egroups.size(); i++) {
            cmbResultGroup->addItem(QString("%1").arg(current->resultsGetLabelForEvaluationGroup(egroups[i])), egroups[i]);
            //qDebug()<<"egroup["<<i<<"/"<<cmbResultGroup->count()<<"]: "<<egroups[i];
        }
        int d=cmbResultGroup->findData(current->getProperty("imfcs_imed_evalgroup", "").toString());
        if (d>=0) cmbResultGroup->setCurrentIndex(d);
        else if (cmbResultGroup->count()>0) cmbResultGroup->setCurrentIndex(0);
        connectParameterWidgets(true);

    }
    // update the list of available fit parameters
    parameterSetChanged(); // this also replots !!!
    //qDebug()<<"fillParameterSet ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;

}

void QFRDRImagingFCSImageEditor::debugInfo() {
    //qDebug()<<"fillParameterSet ... done   cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();;
    QTimer::singleShot(500, this, SLOT(debugInfo()));
}

void QFRDRImagingFCSImageEditor::connectImageWidgets(bool connectTo) {
    if (connectTo) {
        connectImageWidgetsCounter--;
        if (connectImageWidgetsCounter<=0) {
            connectImageWidgetsCounter=0;
            connect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));
            connect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
            connect(chkLogTauAxis, SIGNAL(clicked()), this, SLOT(replotData()));
            connect(chkDisplayResiduals, SIGNAL(toggled(bool)), this, SLOT(replotData()));
            connect(chkKeys, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        }
    } else {
        connectImageWidgetsCounter++;
        disconnect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
        disconnect(chkLogTauAxis, SIGNAL(clicked()), this, SLOT(replotData()));
        disconnect(chkDisplayResiduals, SIGNAL(toggled(bool)), this, SLOT(replotData()));
        disconnect(chkKeys, SIGNAL(toggled(bool)), this, SLOT(replotData()));
    }
}

void QFRDRImagingFCSImageEditor::connectParameterWidgets(bool connectTo) {
    if (connectTo) {
        connectParameterWidgetsCounter--;
        if (connectParameterWidgetsCounter<=0) {
            connectParameterWidgetsCounter=0;
            connect(cmbResultGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterSetChanged()));
            connect(cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
            connect(cmbGofParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
            connect(cmbParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
            connect(cmbGofParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
            connect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
            connect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
            connect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
            connect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
            connect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(displayOverlayChanged()));
            connect(chkLogHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(chkNormalizedHistograms, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(chkExcludeExcludedRunsFromHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
            connect(chkHistogramRangeAuto, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(edtHistogramMin, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
            connect(edtHistogramMax, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        }
    } else {
        connectParameterWidgetsCounter++;
        disconnect(cmbResultGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterSetChanged()));
        disconnect(cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbGofParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbGofParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
        disconnect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
        disconnect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(displayOverlayChanged()));
        disconnect(chkLogHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkNormalizedHistograms, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkExcludeExcludedRunsFromHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkHistogramRangeAuto, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMin, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMax, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
    }

    //qDebug()<<"connectParameterWidgets ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();

}

void QFRDRImagingFCSImageEditor::imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==pltImage) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltGofImage) {
            pltOverview->setXY(newxmin, newxmax, newymin, newymax);
            pltImage->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltOverview) {
            pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
            pltImage->setXY(newxmin, newxmax, newymin, newymax);
    }
}
void QFRDRImagingFCSImageEditor::acfZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==plotter) {
        plotterResid->setX(newxmin, newxmax);
        //qDebug()<<newxmin << newxmax;
    }
}




QString QFRDRImagingFCSImageEditor::currentEvalGroup() const {
    int grp=cmbResultGroup->currentIndex();
    if (grp<0) return "";
    return cmbResultGroup->itemData(grp).toString();

}

QString QFRDRImagingFCSImageEditor::currentFitParameter() const {
    if (cmbParameter->currentIndex()<0) return "";
    return cmbParameter->itemData(cmbParameter->currentIndex()).toString();
}

QString QFRDRImagingFCSImageEditor::currentGofParameter() const {
    if (cmbGofParameter->currentIndex()<0) return "";
    return cmbGofParameter->itemData(cmbGofParameter->currentIndex()).toString();
}

QFRDRImagingFCSImageEditor::ImageTransforms QFRDRImagingFCSImageEditor::currentFitParameterTransfrom() const {
    //return QFRDRImagingFCSImageEditor::itNone;
    if (cmbParameterTransform->currentIndex()<0) return QFRDRImagingFCSImageEditor::itNone;
    return (QFRDRImagingFCSImageEditor::ImageTransforms)cmbParameterTransform->currentIndex();

}

QFRDRImagingFCSImageEditor::ImageTransforms QFRDRImagingFCSImageEditor::currentGofParameterTransfrom() const {
    //return QFRDRImagingFCSImageEditor::itSqrt;
    if (cmbGofParameterTransform->currentIndex()<0) return QFRDRImagingFCSImageEditor::itNone;
    return (QFRDRImagingFCSImageEditor::ImageTransforms)cmbGofParameterTransform->currentIndex();
}






void QFRDRImagingFCSImageEditor::readParameterImage(double* image, double* gof_image, uint16_t width, uint16_t height, QString evalGroup, QString fitParam, QFRDRImagingFCSImageEditor::ImageTransforms tranFitParam, QString gofParam, QFRDRImagingFCSImageEditor::ImageTransforms tranGofParam) {
    uint16_t arraysize=width*height;
    for (register uint16_t i=0; i<arraysize; i++) {
        image[i]=NAN;
        gof_image[i]=0;
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if ( (!m) || evalGroup.isEmpty() || fitParam.isEmpty() ) return;

    QStringList evals=current->resultsCalcEvaluationsInGroup(evalGroup);
    for (register int i=0; i<evals.size(); i++) {
        const QString& en=evals[i];
        int grpIdx=current->resultsGetEvaluationGroupIndex(en);
        int x=m->runToX(grpIdx);
        int y=m->runToY(grpIdx);
        if (x>=0 && x<width && y>=0 && y<height) {
            if (current->resultsExists(en, fitParam)) {
                image[y*width+x]=current->resultsGetAsDouble(en, fitParam);
                switch(tranFitParam) {
                    case QFRDRImagingFCSImageEditor::itAbs: {
                            image[y*width+x]=fabs(image[y*width+x]);
                        } break;
                    case QFRDRImagingFCSImageEditor::itLog: {
                            if (image[y*width+x]>0) image[y*width+x]=log(image[y*width+x])/log(10.0);
                            else  image[y*width+x]=NAN;

                        } break;
                    case QFRDRImagingFCSImageEditor::itReciprocal: {
                            if (image[y*width+x]!=0) image[y*width+x]=1.0/image[y*width+x];
                            else  image[y*width+x]=NAN;
                        } break;
                    case QFRDRImagingFCSImageEditor::itSqrt: {
                            if (image[y*width+x]>=0) image[y*width+x]=sqrt(image[y*width+x]);
                            else  image[y*width+x]=NAN;
                        } break;
                    default: break;
                }
            }
            if (current->resultsExists(en, gofParam)) {
                gof_image[y*width+x]=current->resultsGetAsDouble(en, gofParam);
                switch(tranGofParam) {
                    case QFRDRImagingFCSImageEditor::itAbs: {
                            gof_image[y*width+x]=fabs(gof_image[y*width+x]);
                        } break;
                    case QFRDRImagingFCSImageEditor::itLog: {
                            if (gof_image[y*width+x]>0) gof_image[y*width+x]=log(gof_image[y*width+x])/log(10.0);
                            else  gof_image[y*width+x]=NAN;

                        } break;
                    case QFRDRImagingFCSImageEditor::itReciprocal: {
                            if (gof_image[y*width+x]!=0) gof_image[y*width+x]=1.0/gof_image[y*width+x];
                            else  gof_image[y*width+x]=NAN;
                        } break;
                    case QFRDRImagingFCSImageEditor::itSqrt: {
                            if (gof_image[y*width+x]>=0) gof_image[y*width+x]=sqrt(gof_image[y*width+x]);
                            else  gof_image[y*width+x]=NAN;
                        } break;
                    default: break;
                }
            }
        }
    }

}

bool QFRDRImagingFCSImageEditor::evaluateFitFunction(const double* tau, double* fit, uint32_t N, QStringList& names, QStringList& namelabels, QList<double>& values, QList<double>& errors, QList<bool>& fix, QStringList& units, QStringList& unitlabels, QString evaluation) {
    QString fitfunc=current->resultsGetAsString(evaluation, "fit_model_name");
    QFFitFunction* ff=m_fitFunctions.value(fitfunc, NULL);
    //qDebug()<<evaluation<<fitfunc<<m_fitFunctions.size()<<ff;
    if (!ff) return false;
    double* params=(double*)calloc(ff->paramCount(),sizeof(double));
    double* errs=(double*)calloc(ff->paramCount(),sizeof(double));
    bool* fixs=(bool*)calloc(ff->paramCount(),sizeof(bool));
    names.clear();
    namelabels.clear();
    values.clear();
    errors.clear();
    fix.clear();
    units.clear();
    unitlabels.clear();

    QStringList pids;
    for (int i=0; i<ff->paramCount(); i++) {
        QString id=ff->getParameterID(i);
        pids.append(id);
        errs[i]=0;
        fixs[i]=false;
        params[i]=ff->getDescription(id).initialValue;
        if (current->resultsExists(evaluation, "fitparam_"+id)) {
            params[i]=current->resultsGetAsDouble(evaluation, "fitparam_"+id);
            errs[i]=current->resultsGetErrorAsDouble(evaluation, "fitparam_"+id);
        }
        if (current->resultsExists(evaluation, "fitparam_"+id+"_fix")) {
            fixs[i]=current->resultsGetAsBoolean(evaluation, "fitparam_"+id+"_fix");
        }
    }
    for (int i=0; i<ff->paramCount(); i++) {
        QString id=ff->getParameterID(i);
        if (ff->isParameterVisible(i, params)) {
            namelabels.append(ff->getDescription(id).label);
            unitlabels.append(ff->getDescription(id).unitLabel);
            names.append(ff->getDescription(id).name);
            units.append(ff->getDescription(id).unit);
            values.append(params[i]);
            errors.append(errs[i]);
            fix.append(fixs[i]);
        }
    }

    if (tau && fit) {
        for (unsigned int i=0; i<N; i++) {
            fit[i]=ff->evaluate(tau[i], params);
        }
    }

    free(params);
    free(errs);
    free(fixs);
    return true;
}

void QFRDRImagingFCSImageEditor::saveReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                lastSavePath,
                                tr("PDF File (*.pdf);;PostScript File (*.ps)"));

    if (!fn.isEmpty()) {
        lastSavePath=QFileInfo(fn).absolutePath();
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


void QFRDRImagingFCSImageEditor::printReport() {
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

void QFRDRImagingFCSImageEditor::saveData() {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    if (m) {
        QStringList filters;
        filters<< tr("Comma-Separated value (*.dat *.csv)");
        filters<< tr("Semicolon-Separated value [German Excel] (*.txt)");
        filters<< tr("SYLK (*.slk)");
        filters<< tr("float-TIFF (*.tif)");
        filters<< tr("16-bit-TIFF (*.tif *.tif16)");
        filters<< tr("color-coded PNG image (*.png)");

        QString selFilter=filters[0];

        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Data"),
                                lastSavePath,
                                filters.join(";;"), &selFilter);
        if (fileName.isEmpty()) return;
        lastSavePath=QFileInfo(fileName).absolutePath();
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QString ext=QFileInfo(fileName).suffix();
        QString base=QFileInfo(fileName).completeBaseName();
        QString path=QFileInfo(fileName).absolutePath();
        if (path.right(1)!="/") path=path+"/";

        QString fileNameParam=path+base+".param."+ext;
        QString fileNameGof=path+base+".gof."+ext;

        bool saveParam=true;
        bool saveGof=true;
        if (QFile::exists(fileNameParam)) {
            saveParam=(QMessageBox::question(this, tr("imFCS: Save Parameter Image"), tr("The file '%1' already exists.\n Overwrite?").arg(fileNameParam), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
        }
        if (QFile::exists(fileNameGof)) {
            saveGof=(QMessageBox::question(this, tr("imFCS: Save Parameter Image"), tr("The file '%1' already exists.\n Overwrite?").arg(fileNameGof), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);
        }

        JKImage<double> image(m->getDataImageWidth(), m->getDataImageHeight());
        JKImage<double> gof_image(m->getDataImageWidth(), m->getDataImageHeight());
        readParameterImage(image.data(), gof_image.data(), m->getDataImageWidth(), m->getDataImageHeight(), currentEvalGroup(), currentFitParameter(), QFRDRImagingFCSImageEditor::itNone, currentGofParameter(), QFRDRImagingFCSImageEditor::itNone);

        if (selFilter==filters[0]) {
            if (saveParam) image.save_csv(fileNameParam.toStdString(), ", ", '.');
            if (saveGof) gof_image.save_csv(fileNameGof.toStdString(), ", ", '.');
        } else if (selFilter==filters[1]) {
            if (saveParam) image.save_csv(fileNameParam.toStdString(), "; ", ',');
            if (saveGof) gof_image.save_csv(fileNameGof.toStdString(), "; ", ',');
        } else if (selFilter==filters[2]) {
            if (saveParam)  image.save_sylk(fileNameParam.toStdString());
            if (saveGof) gof_image.save_sylk(fileNameGof.toStdString());
        } else if (selFilter==filters[3]) {
            if (saveParam) image.save_tifffloat(fileNameParam.toStdString());
            if (saveGof) gof_image.save_tifffloat(fileNameGof.toStdString());
        } else if (selFilter==filters[4]) {
            if (saveParam) image.save_tiffuint16(fileNameParam.toStdString());
            if (saveGof) gof_image.save_tiffuint16(fileNameGof.toStdString());
        } else {
            if (saveParam) plteImage->drawImage().save(fileNameParam, "PNG");
            if (saveGof) plteGofImage->drawImage().save(fileNameGof, "PNG");
        }


        QApplication::restoreOverrideCursor();

    }
}


void QFRDRImagingFCSImageEditor::createReportDoc(QTextDocument* document) {

    // make sure all widgets ahave the right size
    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;


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
    fHeading1.setFontPointSize(1.4*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    QTextCharFormat fHeading2=fText;
    fHeading2.setFontPointSize(1.2*fText.fontPointSize());
    fHeading2.setFontWeight(QFont::Bold);
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    cursor.insertText(tr("imagingFCS Report: %1\n").arg(m->getName()), fHeading1);
    cursor.movePosition(QTextCursor::End);

    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTableFormat tableFormat1=tableFormat;
    tableFormat1.setWidth(QTextLength(QTextLength::VariableLength, 1));
    QTextTable* table = cursor.insertTable(4, 2, tableFormat);
    table->mergeCells(0,0,1,2);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>result set:</i> <b>%1</b></small>").arg(cmbResultGroup->currentText())));
        tabCursor=table->cellAt(1, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>parameter:</i> <b>%1</b></small>").arg(cmbParameter->currentText())));
        tabCursor=table->cellAt(1, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>transform:</i> <b>%1</b></small>").arg(cmbParameterTransform->currentText())));
        tabCursor=table->cellAt(2, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>goodnes of fit:</i> <b>%1</b></small>").arg(cmbGofParameter->currentText())));
        tabCursor=table->cellAt(2, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>transform:</i> <b>%1</b></small>").arg(cmbGofParameterTransform->currentText())));
        tabCursor=table->cellAt(3, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>color palette:</i> <b>%1</b></small>").arg(cmbColorbar->currentText())));
        tabCursor=table->cellAt(3, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>color range:</i> <b>%1 ... %2</b></small>").arg(edtColMin->value()).arg(edtColMax->value())));
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.insertText(tr("Parameter Images:\n").arg(m->getName()), fHeading2);
    double w1=pltImage->width();
    double w2=pltGofImage->width();
    double w3=pltOverview->width();
    double allwidth=w1+w2+w3;
    table = cursor.insertTable(1, 3, tableFormat);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltImage->get_plotter()->draw(*painter, QRect(0,0,pltImage->width(),pltImage->height()));
        delete painter;
        double scale=document->textWidth()*w1/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("parameter image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltGofImage->get_plotter()->draw(*painter, QRect(0,0,pltGofImage->width(),pltGofImage->height()));
        delete painter;
        double scale=document->textWidth()*w2/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("goodnes of fit image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    {
        QTextCursor tabCursor=table->cellAt(0, 2).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltOverview->get_plotter()->draw(*painter, QRect(0,0,pltOverview->width(),pltOverview->height()));
        delete painter;
        double scale=document->textWidth()*w3/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("parameter image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    table = cursor.insertTable(1, 2, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        plotter->get_plotter()->draw(*painter, QRect(0,0,plotter->width(),plotter->height()+plotterResid->height()));
        delete painter;
        double scale=document->textWidth()*(double)plotter->width()/allwidth/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation curves:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);

        tabCursor=table->cellAt(0, 1).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><font size=\"-4\">%1</font></center>").arg(tvParams->toHtml())));
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();



    w1=qMax(1,pltParamHistogram->width());
    w2=qMax(1,tvHistogramParameters->width());
    allwidth=qMax(1.0,w1+w2);
    cursor.insertBlock();
    cursor.insertText(tr("Parameter Histograms:\n").arg(m->getName()), fHeading2);
    table = cursor.insertTable(1, 2, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltParamHistogram->get_plotter()->draw(*painter, QRect(0,0,pltParamHistogram->width(),pltParamHistogram->height()));
        delete painter;
        double scale=document->textWidth()*w1/allwidth/pic.boundingRect().width();
        if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);

        tabCursor=table->cellAt(0, 1).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><font size=\"-4\">%1</font></center>").arg(tvHistogramParameters->toHtml())));
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();


    cursor.insertBlock();
    cursor.insertText(tr("Data Source:\n").arg(m->getName()), fHeading2);
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(tr("files:\n"), fTextBoldSmall);
    QStringList f=m->getFiles();
    QStringList t=m->getFilesTypes();
    for (int i=0; i<f.size(); i++) {
        QString typ="";
        if (i<t.size()) {
            if (!t[i].isEmpty()) {
                typ=QString("[%1]").arg(t[i]);
            }
        }
        cursor.insertText(tr("%1 %2\n").arg(f[i]).arg(typ), fTextSmall);

    }

    if (!m->getDescription().isEmpty()) {
        cursor.insertText(tr("description: "), fTextBoldSmall);
        cursor.insertText(m->getDescription(), fTextSmall);
    }
    cursor.movePosition(QTextCursor::End);

}


void QFRDRImagingFCSImageEditor::replotHistogram() {
    pltParamHistogram->set_doDrawing(false);
    pltParamHistogram->getYAxis()->set_logAxis(chkLogHistogram->isChecked());
    pltParamHistogram->zoomToFit(true, true, false, !chkLogHistogram->isChecked());
    pltParamHistogram->set_doDrawing(true);
    pltParamHistogram->update_plot();
}

void QFRDRImagingFCSImageEditor::updateHistogram() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    edtHistogramMin->setEnabled(!chkHistogramRangeAuto->isChecked());
    edtHistogramMax->setEnabled(!chkHistogramRangeAuto->isChecked());

    //qDebug()<<"updateHistogram";
    pltParamHistogram->set_doDrawing(false);
    tvHistogramParameters->setModel(NULL);
    tabHistogramParameters->setReadonly(false);

    pltParamHistogram->clearGraphs(true);
    JKQTPdatastore* ds=pltParamHistogram->getDatastore();
    ds->clear();

    for (int r=0; r<tabHistogramParameters->rowCount(); r++) {
        tabHistogramParameters->setCell(r, 1, QVariant());
    }

    //qDebug()<<"***** updateHistogram()   "<<plteImageData<<plteImageSize;

    if (plteImageData && (plteImageSize>=m->getDataImageWidth()*m->getDataImageHeight())) {
        int imageSize=m->getDataImageWidth()*m->getDataImageHeight();
        double* datahist=(double*)malloc(imageSize*sizeof(double));
        int32_t datasize=0;
        double mmin=edtHistogramMin->value();
        double mmax=edtHistogramMax->value();
        // && (plteImageData[i]>=mmin) && (plteImageData[i]<=mmax)
        if (chkHistogramRangeAuto->isChecked()) {
            int32_t ii=0;
            if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (!m->leaveoutRun(i)) {
                        datahist[ii]=plteImageData[i];
                        ii++;
                        datasize++;
                    }
                }
            } else  {
                for (register int32_t i=0; i<imageSize; i++) {
                    datahist[i]=plteImageData[i];
                    datasize++;
                }
            }
        } else {
            int32_t ii=0;
            if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (!m->leaveoutRun(i) && (plteImageData[i]>=mmin) && (plteImageData[i]<=mmax)) {
                        datahist[ii]=plteImageData[i];
                        ii++;
                        datasize++;
                    }
                }
            } else  {
                for (register int32_t i=0; i<imageSize; i++) {
                    if ((plteImageData[i]>=mmin) && (plteImageData[i]<=mmax)) {
                        datahist[ii]=plteImageData[i];
                        ii++;
                        datasize++;
                    }
                }
            }
        }


        statisticsSort(datahist, datasize);
        double dmean, dstd, dmin, dmax, dmedian, dq25, dq75;
        dmean=statisticsAverageVariance(dstd, datahist, datasize);
        dstd=sqrt(dstd);
        dmin=statisticsSortedMin(datahist, datasize);
        dmax=statisticsSortedMax(datahist, datasize);
        dmedian=statisticsSortedMedian(datahist, datasize);
        dq25=statisticsSortedQuantile(datahist, datasize, 0.25);
        dq75=statisticsSortedQuantile(datahist, datasize, 0.75);
        tabHistogramParameters->setCell(0, 1, datasize);
        tabHistogramParameters->setCell(1, 1, dmean);
        tabHistogramParameters->setCell(2, 1, dmedian);
        tabHistogramParameters->setCell(3, 1, dstd);
        tabHistogramParameters->setCell(4, 1, dmin);
        tabHistogramParameters->setCell(5, 1, dq25);
        tabHistogramParameters->setCell(6, 1, dq75);
        tabHistogramParameters->setCell(7, 1, dmax);

        if (chkHistogramRangeAuto->isChecked()) {
            connectImageWidgets(false);
            edtHistogramMin->setValue(dmin);
            edtHistogramMax->setValue(dmax);
            connectImageWidgets(true);
        }

        long histBins=spinHistogramBins->value();
        double* histX=(double*)malloc(histBins*sizeof(double));
        double* histY=(double*)malloc(histBins*sizeof(double));


        if (chkHistogramRangeAuto->isChecked()) {
            statisticsHistogram<double, double>(datahist, datasize, histX, histY, histBins, chkNormalizedHistograms->isChecked());
        } else {
            statisticsHistogramRanged<double, double>(datahist, datasize, edtHistogramMin->value(), edtHistogramMax->value(), histX, histY, histBins, chkNormalizedHistograms->isChecked());
        }

        mainHistogramMax=statisticsMax(histY, histBins);
        double barY=mainHistogramMax*1.1;

        size_t pltcPHHistogramX=ds->addCopiedColumn(histX, histBins, "histX");
        size_t pltcPHHistogramY=ds->addCopiedColumn(histY, histBins, "histY");
        size_t pltcPHBarY=ds->addCopiedColumn(&barY, 1, "barY");
        size_t pltcPHBarMean=ds->addCopiedColumn(&dmean, 1, "mean");;
        /*size_t pltcPHBarStd=*/ds->addCopiedColumn(&dstd, 1, "stddev");;
        size_t pltcPHBarMedian=ds->addCopiedColumn(&dmedian, 1, "median");;
        size_t pltcPHBarMin=ds->addCopiedColumn(&dmin, 1, "min");;
        size_t pltcPHBarMax=ds->addCopiedColumn(&dmax, 1, "max");;
        size_t pltcPHBarQ25=ds->addCopiedColumn(&dq25, 1, "quant25");;
        size_t pltcPHBarQ75=ds->addCopiedColumn(&dq75, 1, "quant75");;

        JKQTPboxplotHorizontalGraph* plteParamHistogramBoxplot=new JKQTPboxplotHorizontalGraph(pltParamHistogram->get_plotter());
        plteParamHistogramBoxplot->set_boxWidth(mainHistogramMax*0.08);
        plteParamHistogramBoxplot->set_maxColumn(pltcPHBarMax);
        plteParamHistogramBoxplot->set_minColumn(pltcPHBarMin);
        plteParamHistogramBoxplot->set_medianColumn(pltcPHBarMedian);
        plteParamHistogramBoxplot->set_meanColumn(pltcPHBarMean);
        plteParamHistogramBoxplot->set_percentile25Column(pltcPHBarQ25);
        plteParamHistogramBoxplot->set_percentile75Column(pltcPHBarQ75);
        plteParamHistogramBoxplot->set_posColumn(pltcPHBarY);
        plteParamHistogramBoxplot->set_color(QColor("blue"));
        plteParamHistogramBoxplot->set_title(tr("boxplot (complete)"));
        pltParamHistogram->addGraph(plteParamHistogramBoxplot);

        JKQTPbarHorizontalGraph* plteParamHistogram=new JKQTPbarHorizontalGraph(pltParamHistogram->get_plotter());
        plteParamHistogram->set_fillColor(plteParamHistogramBoxplot->get_color());
        plteParamHistogram->set_xColumn(pltcPHHistogramX);
        plteParamHistogram->set_yColumn(pltcPHHistogramY);
        plteParamHistogram->set_width(1);
        if (!chkHistogramRangeAuto->isChecked()) {
            plteParamHistogram->set_width(0.5);
            plteParamHistogram->set_shift(0);
        }
        plteParamHistogram->set_title(tr("histogram (complete)"));
        pltParamHistogram->addGraph(plteParamHistogram);

        free(histX);
        free(histY);
        free(datahist);
    }


    updateSelectionHistogram(false);


    tabHistogramParameters->setReadonly(true);
    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->resizeColumnsToContents();
    replotHistogram();
    //qDebug()<<"updateHistogram ... end";
}

void QFRDRImagingFCSImageEditor::updateSelectionHistogram(bool replot) {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    edtHistogramMin->setEnabled(!chkHistogramRangeAuto->isChecked());
    edtHistogramMax->setEnabled(!chkHistogramRangeAuto->isChecked());
    //qDebug()<<"updateSelectionHistogram";
    JKQTPdatastore* ds=pltParamHistogram->getDatastore();

    if (replot) {
        tvHistogramParameters->setModel(NULL);
        tabHistogramParameters->setReadonly(false);
        pltParamHistogram->set_doDrawing(false);
        if (pltParamHistogram->getGraphCount()>2) {
            for (int i=pltParamHistogram->getGraphCount()-1; i>=2; i--) {
                pltParamHistogram->deleteGraph(i, true);
            }
        }
        ds->deleteAllColumns("sel_histX");
        ds->deleteAllColumns("sel_histY");
        ds->deleteAllColumns("sel_barY");
        ds->deleteAllColumns("sel_mean");
        ds->deleteAllColumns("sel_stddev");
        ds->deleteAllColumns("sel_median");
        ds->deleteAllColumns("sel_min");
        ds->deleteAllColumns("sel_max");
        ds->deleteAllColumns("sel_quant25");
        ds->deleteAllColumns("sel_quant75");
    }


    for (int r=0; r<tabHistogramParameters->rowCount(); r++) {
        tabHistogramParameters->setCell(r, 2, QVariant());
    }



    if (plteImageData && (plteImageSize>=m->getDataImageWidth()*m->getDataImageHeight()) && (selected.size()>1)) {
        int imageSize=m->getDataImageWidth()*m->getDataImageHeight();
        int32_t datasize=0;
        double* datahist=(double*)malloc(imageSize*sizeof(double));

        int32_t ii=0;
        if (chkHistogramRangeAuto->isChecked()) {
            if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (selected.contains(i) && !m->leaveoutRun(i)) {
                        datahist[ii]=plteImageData[i];
                        ii++;
                        datasize++;
                    }
                }
            } else  {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (selected.contains(i)) {
                        datahist[ii]=plteImageData[i];
                        ii++;
                        datasize++;
                    }
                }
            }
        } else {
            double mmin=edtHistogramMin->value();
            double mmax=edtHistogramMax->value();
            // && (plteImageData[i]>=mmin) && (plteImageData[i]<=mmax)
            if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (selected.contains(i) && (plteImageData[i]>=mmin) && (plteImageData[i]<=mmax) && !m->leaveoutRun(i)) {
                        datahist[ii]=plteImageData[i];
                        ii++;
                        datasize++;
                    }
                }
            } else  {
                for (register int32_t i=0; i<imageSize; i++) {
                    if (selected.contains(i) && (plteImageData[i]>=mmin) && (plteImageData[i]<=mmax)) {
                        datahist[ii]=plteImageData[i];
                        ii++;
                        datasize++;
                    }
                }
            }
        }


        statisticsSort(datahist, datasize);
        double dmean, dstd, dmin, dmax, dmedian, dq25, dq75;
        dmean=statisticsAverageVariance(dstd, datahist, datasize);
        dstd=sqrt(dstd);
        dmin=statisticsSortedMin(datahist, datasize);
        dmax=statisticsSortedMax(datahist, datasize);
        dmedian=statisticsSortedMedian(datahist, datasize);
        dq25=statisticsSortedQuantile(datahist, datasize, 0.25);
        dq75=statisticsSortedQuantile(datahist, datasize, 0.75);
        tabHistogramParameters->setCell(0, 2, datasize);
        tabHistogramParameters->setCell(1, 2, dmean);
        tabHistogramParameters->setCell(2, 2, dmedian);
        tabHistogramParameters->setCell(3, 2, dstd);
        tabHistogramParameters->setCell(4, 2, dmin);
        tabHistogramParameters->setCell(5, 2, dq25);
        tabHistogramParameters->setCell(6, 2, dq75);
        tabHistogramParameters->setCell(7, 2, dmax);



        long histBins=spinHistogramBins->value();
        double* histX=(double*)malloc(histBins*sizeof(double));
        double* histY=(double*)malloc(histBins*sizeof(double));

        if (chkHistogramRangeAuto->isChecked()) {
            statisticsHistogram<double, double>(datahist, datasize, histX, histY, histBins, chkNormalizedHistograms->isChecked());
        } else {
            statisticsHistogramRanged<double, double>(datahist, datasize, edtHistogramMin->value(), edtHistogramMax->value(), histX, histY, histBins, chkNormalizedHistograms->isChecked());
        }

        double barY=mainHistogramMax*1.2;

        size_t pltcPHHistogramX=ds->addCopiedColumn(histX, histBins, "sel_histX");
        size_t pltcPHHistogramY=ds->addCopiedColumn(histY, histBins, "sel_histY");
        size_t pltcPHBarY=ds->addCopiedColumn(&barY, 1, "sel_barY");
        size_t pltcPHBarMean=ds->addCopiedColumn(&dmean, 1, "sel_mean");
        /*size_t pltcPHBarStd=*/ds->addCopiedColumn(&dstd, 1, "sel_stddev");
        size_t pltcPHBarMedian=ds->addCopiedColumn(&dmedian, 1, "sel_median");
        size_t pltcPHBarMin=ds->addCopiedColumn(&dmin, 1, "sel_min");
        size_t pltcPHBarMax=ds->addCopiedColumn(&dmax, 1, "sel_max");
        size_t pltcPHBarQ25=ds->addCopiedColumn(&dq25, 1, "sel_quant25");
        size_t pltcPHBarQ75=ds->addCopiedColumn(&dq75, 1, "sel_quant75");

        JKQTPboxplotHorizontalGraph* plteParamHistogramBoxplot=new JKQTPboxplotHorizontalGraph(pltParamHistogram->get_plotter());
        plteParamHistogramBoxplot->set_boxWidth(mainHistogramMax*0.08);
        plteParamHistogramBoxplot->set_maxColumn(pltcPHBarMax);
        plteParamHistogramBoxplot->set_minColumn(pltcPHBarMin);
        plteParamHistogramBoxplot->set_medianColumn(pltcPHBarMedian);
        plteParamHistogramBoxplot->set_meanColumn(pltcPHBarMean);
        plteParamHistogramBoxplot->set_percentile25Column(pltcPHBarQ25);
        plteParamHistogramBoxplot->set_percentile75Column(pltcPHBarQ75);
        plteParamHistogramBoxplot->set_posColumn(pltcPHBarY);
        plteParamHistogramBoxplot->set_color(QColor("red"));
        plteParamHistogramBoxplot->set_title(tr("boxplot (selection)"));
        pltParamHistogram->addGraph(plteParamHistogramBoxplot);

        JKQTPbarHorizontalGraph* plteParamHistogram=new JKQTPbarHorizontalGraph(pltParamHistogram->get_plotter());
        QColor col=plteParamHistogramBoxplot->get_color();
        col.setAlphaF(0.3);
        plteParamHistogram->set_fillColor(col);
        plteParamHistogram->set_xColumn(pltcPHHistogramX);
        plteParamHistogram->set_yColumn(pltcPHHistogramY);
        plteParamHistogram->set_title(tr("histogram (selection)"));
        plteParamHistogram->set_width(1);
        if (!chkHistogramRangeAuto->isChecked()) {
            plteParamHistogram->set_width(0.5);
            plteParamHistogram->set_shift(0.5);
        }
        pltParamHistogram->addGraph(plteParamHistogram);

        free(histX);
        free(histY);
        free(datahist);
    }





    if (replot) {
        tabHistogramParameters->setReadonly(true);
        tvHistogramParameters->setModel(tabHistogramParameters);
        tvHistogramParameters->resizeColumnsToContents();
        replotHistogram();
    }
    //qDebug()<<"updateSelectionHistogram ... end";
}
