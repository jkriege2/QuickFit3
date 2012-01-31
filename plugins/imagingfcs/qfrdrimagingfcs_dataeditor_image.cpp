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



#define CLICK_UPDATE_TIMEOUT 500

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
    datahist=datahistsel=NULL;
    datasize=datasizesel=0;
    timUpdateAfterClick=new QTimer(this);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->setInterval(CLICK_UPDATE_TIMEOUT);
    timUpdateAfterClick->stop();
    connect(timUpdateAfterClick, SIGNAL(timeout()), this, SLOT(updateAfterClick()));
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
    //if (datahist) free(datahist);
    datahist=NULL;
    //if (datahistsel) free(datahistsel);
    datahistsel=NULL;
}




void QFRDRImagingFCSImageEditor::createWidgets() {
    QLabel* l;
    QGridLayout* lb=new QGridLayout(this);
    setLayout(lb);


    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.5);
    selectionColor=ovlSelCol;
    QColor ovlExCol=QColor("blue");
    ovlExCol.setAlphaF(0.5);
    excludedColor=ovlExCol;


    ///////////////////////////////////////////////////////////////
    // GROUPBOX: parameter selection group box
    ///////////////////////////////////////////////////////////////

    QGroupBox* grpTop=new QGroupBox(tr(" Parameter Image Plot Settings "), this);
    QGridLayout* topgrid=new QGridLayout(this);
    grpTop->setLayout(topgrid);

    int row=0;
    cmbResultGroup=new QComboBox(this);
    cmbResultGroup->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    topgrid->addWidget((l=new QLabel(tr("&result set:"))), row, 0);
    l->setBuddy(cmbResultGroup);
    topgrid->addWidget(cmbResultGroup, row, 1);

    cmbParameter=new QComboBox(this);
    cmbParameter->setSizeAdjustPolicy(QComboBox::AdjustToContents);
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
    cmbGofParameter->setSizeAdjustPolicy(QComboBox::AdjustToContents);
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







    ///////////////////////////////////////////////////////////////
    // GROUPBOX: visible plots
    ///////////////////////////////////////////////////////////////
    QGroupBox* grpVisiblePlots=new QGroupBox(tr(" visible plots "), this);
    QGridLayout* glVisPlots=new QGridLayout(this);
    grpVisiblePlots->setLayout(glVisPlots);
    chkOverviewVisible=new QCheckBox(tr("overview"), grpVisiblePlots);
    chkOverviewVisible->setChecked(true);
    chkGofVisible=new QCheckBox(tr("goodnes of fit"), grpVisiblePlots);
    chkGofVisible->setChecked(true);
    chkMaskVisible=new QCheckBox(tr("mask/selected"), grpVisiblePlots);
    chkMaskVisible->setChecked(false);
    glVisPlots->addWidget(chkOverviewVisible, 0,0);
    glVisPlots->addWidget(chkGofVisible, 0,1);
    glVisPlots->addWidget(chkMaskVisible, 1,0);
    vbl->addWidget(grpVisiblePlots);






    ///////////////////////////////////////////////////////////////
    // GROUPBOX: parameter image style
    ///////////////////////////////////////////////////////////////
    QGroupBox* wimg=new QGroupBox(tr(" parameter image style "), this);
    vbl->addWidget(wimg);
    QFormLayout* gli=new QFormLayout(this);
    wimg->setLayout(gli);

    cmbImageStyle=new QComboBox(wimg);
    cmbImageStyle->addItem(tr("masked image, black"));
    cmbImageStyle->addItem(tr("masked image, white"));
    cmbImageStyle->addItem(tr("masked image, grey"));
    cmbImageStyle->addItem(tr("image + overlay mask"));
    cmbImageStyle->addItem(tr("image only"));
    gli->addRow(tr("st&yle:"), cmbImageStyle);


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

    cmbOutOfRangeMode=new QComboBox(wimg);
    cmbOutOfRangeMode->addItem(tr("min/max color"));
    cmbOutOfRangeMode->addItem(tr("black"));
    cmbOutOfRangeMode->addItem(tr("white"));
    cmbOutOfRangeMode->addItem(tr("transparent"));
    cmbOutOfRangeMode->addItem(tr("red"));
    cmbOutOfRangeMode->addItem(tr("green"));
    cmbOutOfRangeMode->addItem(tr("blue"));
    gli->addRow(tr("out-&of-range mode:"), cmbOutOfRangeMode);

    chkDisplayImageOverlay=new QCheckBox(wimg);
    gli->addRow(tr("show &overlays:"), chkDisplayImageOverlay);

    cmbSelectionStyle=new QComboBox(wimg);
    QPixmap selPix(15,15);
    selPix.fill(ovlSelCol);
    cmbSelectionStyle->addItem(QIcon(selPix), tr("overlay image"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, ovlSelCol, ovlSelCol.lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("symbol"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("black"), QColor("black").lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("black symbol"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("white"), QColor("white").lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("white symbol"));
    selPix.fill(QColor(Qt::transparent));
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("grey"), QColor("grey").lighter());
    cmbSelectionStyle->addItem(QIcon(selPix), tr("grey symbol"));
    gli->addRow(tr("selection style:"), cmbSelectionStyle);





    ///////////////////////////////////////////////////////////////
    // GROUPBOX: mask options
    ///////////////////////////////////////////////////////////////
    QGroupBox* wmask=new QGroupBox(tr(" mask options "), this);
    vbl->addWidget(wmask);
    QGridLayout* glmask=new QGridLayout(this);
    wmask->setLayout(glmask);

    btnDontUse=new QPushButton(tr("&unmask selected"), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    glmask->addWidget(btnDontUse, 0, 0);
    btnUse=new QPushButton(tr("&mask selected"), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    glmask->addWidget(btnUse, 0, 1);
    btnMaskByIntensity=new QPushButton(tr("mask by &intensity"), w);
    glmask->addWidget(btnMaskByIntensity, 1, 1);
    connect(btnMaskByIntensity, SIGNAL(clicked()), this, SLOT(excludeByIntensity()));
    btnUseAll=new QPushButton(tr("&clear mask"), w);
    glmask->addWidget(btnUseAll, 1, 0);
    connect(btnUseAll, SIGNAL(clicked()), this, SLOT(includeAll()));







    ///////////////////////////////////////////////////////////////
    // GROUPBOX: correlation plot styles
    ///////////////////////////////////////////////////////////////
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

    gl->addRow((l=new QLabel(tr("average opt&ions:"))), cmbAverageStyle);
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







    ///////////////////////////////////////////////////////////////
    // GROUPBOX: overview image plot
    ///////////////////////////////////////////////////////////////
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
    pltOverview->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltOverview->get_plotter()->getYAxis()->set_axisMinWidth(1);

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

    plteOverviewSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltOverview->get_plotter());
    pltOverview->addGraph(plteOverviewSelected);


    plteOverviewExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltOverview->get_plotter());
    pltOverview->addGraph(plteOverviewExcluded);

    plteImageData=NULL;






    ///////////////////////////////////////////////////////////////
    // WIDGET: mask image plot
    ///////////////////////////////////////////////////////////////
    QWidget* wpltMask=new QWidget(this);
    wpltMask->setVisible(false);
    QVBoxLayout* lpltMask=new QVBoxLayout();
    wpltMask->setLayout(lpltMask);
    pltMask=new JKQtPlotter(wpltMask);
    lpltMask->addWidget(new QLabel(tr("Mask/Selected:")));
    lpltMask->addWidget(pltMask, 1);
    pltMask->set_zoomByDoubleAndRightMouseClick(false);
    pltMask->set_displayMousePosition(false);
    pltMask->set_displayToolbar(true);
    pltMask->get_plotter()->set_maintainAspectRatio(true);
    pltMask->get_plotter()->set_aspectRatio(1);
    pltMask->get_plotter()->set_maintainAxisAspectRatio(true);
    pltMask->get_plotter()->set_axisAspectRatio(1);
    pltMask->setXY(0,0,1,1);
    pltMask->setAbsoluteXY(0,1,0,1);
    pltMask->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltMask->get_plotter()->getYAxis()->set_axisMinWidth(1);

    pltMask->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltMask->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltMask->get_plotter()->getXAxis()->set_axisLabel("");
    pltMask->get_plotter()->getYAxis()->set_axisLabel("");
    pltMask->get_plotter()->setGrid(false);
    pltMask->get_plotter()->set_useAntiAliasingForSystem(true);
    pltMask->get_plotter()->set_useAntiAliasingForGraphs(true);
    connect(pltMask, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(imageZoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(pltMask, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));

    plteMask=new JKQTPOverlayImage(0,0,1,1,NULL, 0,0, QColor("black"), pltMask->get_plotter());
    plteMask->set_falseColor(QColor("white"));
    pltMask->addGraph(plteMask);

    plteMaskSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltMask->get_plotter());
    pltMask->addGraph(plteMaskSelected);










    ///////////////////////////////////////////////////////////////
    // WIDGET: parameter image plot
    ///////////////////////////////////////////////////////////////

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

    pltImage->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getYAxis()->set_axisMinWidth(1);
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








    ///////////////////////////////////////////////////////////////
    // WIDGET: goodnes of fit image plot
    ///////////////////////////////////////////////////////////////

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
    pltGofImage->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltGofImage->get_plotter()->getYAxis()->set_axisMinWidth(1);
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

    connect(pltOverview, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));
    connect(pltImage, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));
    connect(pltGofImage, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));






    ///////////////////////////////////////////////////////////////
    // WIDGET: correlation&residuals plot
    ///////////////////////////////////////////////////////////////

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
    splitterTop->addWidget(wpltMask);
    splitterTop->addWidget(wpltOverview);
    splitterTopBot->addWidget(splitterTop);
    /*splitterBotPlots=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterBotPlots->addWidget(plotter);
    splitterBotPlots->addWidget(wp);*/
    QVBoxLayout* layPlotsBot=new QVBoxLayout(this);
    QWidget* wPlotsBot=new QWidget(this);
    wPlotsBot->setLayout(layPlotsBot);
    layPlotsBot->setContentsMargins(0,0,0,0);
    layPlotsBot->addWidget(plotter,3);
    layPlotsBot->addWidget(wp,2);
    splitterBot=new QVisibleHandleSplitter(Qt::Horizontal, this);
    //splitterBot->addWidget(splitterBotPlots);
    splitterBot->addWidget(wPlotsBot);
    splitterBot->addWidget(tvParams);
    splitterTopBot->addWidget(splitterBot);

    vbl->addWidget(new QWidget(this), 1);








    ///////////////////////////////////////////////////////////////
    // GROUPBOX: buttons for data/report saving
    ///////////////////////////////////////////////////////////////

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
    histogram=new QFHistogramView(this);
    QWidget* widHist=histogram;
    chkExcludeExcludedRunsFromHistogram=new QCheckBox("", this);
    chkExcludeExcludedRunsFromHistogram->setToolTip(tr("if this option is activated the histograms are only calculated for those pixels that are not excluded."));
    histogram->addSettingsWidget(tr("mind excluded runs:"), chkExcludeExcludedRunsFromHistogram);






    //////////////////////////////////////////////////////////////////////////////////////////
    // PUT EVERYTHING TOGETHER
    //////////////////////////////////////////////////////////////////////////////////////////
    tabDisplay=new QTabWidget(this);
    tabDisplay->addTab(widACFs, tr("&Correlations"));
    tabDisplay->addTab(widHist, tr("&Histogram"));


    //lb->addWidget(grpTop, 1, 0, 1, 2);
    lb->addLayout(grdTop, 0,0);
    lb->addWidget(tabDisplay, 1,0);


    connect(chkOverviewVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));
    connect(chkGofVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));
    connect(chkMaskVisible, SIGNAL(toggled(bool)), this, SLOT(showHidePlots()));



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
            current->setQFProperty(QString("imfcs_imed_style_%1_%2").arg(egroup).arg(param), cmbImageStyle->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_oorc_%1_%2").arg(egroup).arg(param), cmbOutOfRangeMode->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), chkDisplayImageOverlay->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_ovstyle_%1_%2").arg(egroup).arg(param), cmbSelectionStyle->currentIndex(), false, false);
            current->setQFProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), chkImageAutoScale->isChecked(), false, false);
            if (!chkImageAutoScale->isChecked()) {
                current->setQFProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), edtColMin->value(), false, false);
                current->setQFProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), edtColMax->value(), false, false);
            }
            current->setQFProperty(QString("imfcs_imed_histbins_%1_%2").arg(egroup).arg(param), histogram->getBins(), false, false);
            current->setQFProperty(QString("imfcs_imed_histnorm_%1_%2").arg(egroup).arg(param), histogram->getNormalized(), false, false);
            current->setQFProperty(QString("imfcs_imed_histlog_%1_%2").arg(egroup).arg(param), histogram->getLog(), false, false);
            current->setQFProperty(QString("imfcs_imed_histex_%1_%2").arg(egroup).arg(param), chkExcludeExcludedRunsFromHistogram->isChecked(), false, false);
            current->setQFProperty(QString("imfcs_imed_histrauto_%1_%2").arg(egroup).arg(param), histogram->getAutorange(), false, false);
            if (!histogram->getAutorange()) {
                current->setQFProperty(QString("imfcs_imed_histrmin_%1_%2").arg(egroup).arg(param), histogram->getMin(), false, false);
                current->setQFProperty(QString("imfcs_imed_histrmax_%1_%2").arg(egroup).arg(param), histogram->getMax(), false, false);
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
            //histogram->connectParameterWidgets(false);
            plteImage->getDataMinMax(mi, ma);
            int d=current->getProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param),
                                       settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex())).toInt();
            if (d>=0) cmbColorbar->setCurrentIndex(d);
            else if (cmbColorbar->count()>0) cmbColorbar->setCurrentIndex(0);
            cmbImageStyle->setCurrentIndex(current->getProperty(QString("imfcs_imed_style_%1_%2").arg(egroup).arg(param), cmbImageStyle->currentIndex()).toInt());
            cmbOutOfRangeMode->setCurrentIndex(current->getProperty(QString("imfcs_imed_oorc_%1_%2").arg(egroup).arg(param), cmbOutOfRangeMode->currentIndex()).toInt());
            chkDisplayImageOverlay->setChecked(current->getProperty(QString("imfcs_imed_overlay_%1_%2").arg(egroup).arg(param), true).toBool());
            cmbSelectionStyle->setCurrentIndex(current->getProperty(QString("imfcs_imed_ovstyle_%1_%2").arg(egroup).arg(param), false).toBool());
            chkImageAutoScale->setChecked(current->getProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), true).toBool());
            if (!chkImageAutoScale->isChecked()) {
                edtColMin->setValue(current->getProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), mi).toDouble());
                edtColMax->setValue(current->getProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), ma).toDouble());
            }
            histogram->setBins(current->getProperty(QString("imfcs_imed_histbins_%1_%2").arg(egroup).arg(param), 25).toInt());
            histogram->setNormalized(current->getProperty(QString("imfcs_imed_histnorm_%1_%2").arg(egroup).arg(param), true).toBool());
            histogram->setLog(current->getProperty(QString("imfcs_imed_histlog_%1_%2").arg(egroup).arg(param), false).toBool());
            chkExcludeExcludedRunsFromHistogram->setChecked(current->getProperty(QString("imfcs_imed_histex_%1_%2").arg(egroup).arg(param), true).toBool());
            histogram->setAutorange(current->getProperty(QString("imfcs_imed_histrauto_%1_%2").arg(egroup).arg(param), true).toBool());
            if (!histogram->getAutorange()) {
                histogram->setMin(current->getProperty(QString("imfcs_imed_histrmin_%1_%2").arg(egroup).arg(param), 0).toDouble());
                histogram->setMax(current->getProperty(QString("imfcs_imed_histrmax_%1_%2").arg(egroup).arg(param), 10).toDouble());
            }

            connectParameterWidgets(true);

            pltImage->update_plot();
        }

    }
}

/*void QFRDRImagingFCSImageEditor::replotOverlays() {
    pltOverview->set_doDrawing(false);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);
    pltMask->set_doDrawing(false);
    updateOverlaySettings();
    pltOverview->set_doDrawing(true);
    pltImage->set_doDrawing(true);
    pltGofImage->set_doDrawing(true);
    pltMask->set_doDrawing(true);
    if (replot) {
        pltOverview->update_plot();
        pltImage->update_plot();
        pltGofImage->update_plot();
        pltMask->update_plot();
    }
}*/

void QFRDRImagingFCSImageEditor::updateOverlaySettings() {
    if (!chkDisplayImageOverlay->isChecked()) {
        pltImage->deleteGraph(plteImageSelected, false);
        pltImage->deleteGraph(plteImageExcluded, false);
        pltGofImage->deleteGraph(plteGofImageSelected, false);
        pltGofImage->deleteGraph(plteGofImageExcluded, false);
        pltMask->deleteGraph(plteMaskSelected, false);
    } else {
        pltImage->addGraph(plteImageExcluded);
        pltImage->addGraph(plteImageSelected);
        pltGofImage->addGraph(plteGofImageSelected);
        pltGofImage->addGraph(plteGofImageExcluded);
        pltMask->addGraph(plteMaskSelected);
    }

    setSelectedOverlayStyle(plteOverviewSelected);
    setSelectedOverlayStyle(plteImageSelected);
    setSelectedOverlayStyle(plteGofImageSelected);
    setSelectedOverlayStyle(plteMaskSelected);

    if (cmbImageStyle->currentIndex()<cmbImageStyle->count()-1) {
        pltImage->deleteGraph(plteImageExcluded, false);
        pltImage->addGraph(plteImageExcluded);
    }
    if (cmbImageStyle->currentIndex()==cmbImageStyle->count()-1) {
        pltImage->deleteGraph(plteImageExcluded, false);
    }
    if (cmbImageStyle->currentIndex()==0) {
        plteImageExcluded->set_trueColor(QColor("black"));
    } else if (cmbImageStyle->currentIndex()==1) {
        plteImageExcluded->set_trueColor(QColor("white"));
    } else if (cmbImageStyle->currentIndex()==2) {
        plteImageExcluded->set_trueColor(QColor("grey"));
    } else if (cmbImageStyle->currentIndex()==3) {
        plteImageExcluded->set_trueColor(excludedColor);
    } else if (cmbImageStyle->currentIndex()==3) {
        plteImageExcluded->set_trueColor(Qt::transparent);
    }

    if (pltImage->containsGraph(plteImageSelected)) pltImage->moveGraphTop(plteImageSelected);
    saveImageSettings();

}

void QFRDRImagingFCSImageEditor::paletteChanged() {
    plteImage->set_palette(cmbColorbar->currentIndex());
    plteImage->set_autoImageRange(chkImageAutoScale->isChecked());
    switch(cmbOutOfRangeMode->currentIndex()) {
        case 0:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::LastPaletteColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::LastPaletteColor);
            break;
        default:
        case 1:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("black"));
            plteImage->set_rangeMaxFailColor(QColor("black"));
            break;
        case 2:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("white"));
            plteImage->set_rangeMaxFailColor(QColor("white"));
            break;
        case 3:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::Transparent);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::Transparent);
            break;
        case 4:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("red"));
            plteImage->set_rangeMaxFailColor(QColor("red"));
            break;
        case 5:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("green"));
            plteImage->set_rangeMaxFailColor(QColor("green"));
            break;
        case 6:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("blue"));
            plteImage->set_rangeMaxFailColor(QColor("blue"));
            break;
    }

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
    histogram->histogramSettingsChanged(false);
    updateHistogram();
    saveImageSettings();
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
    rawDataChanged();
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
    rawDataChanged();
}

void QFRDRImagingFCSImageEditor::excludeByIntensity() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        /*QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutAddRun(run);
        }
        */
        // TODO: DIALOG AND CODE!!!
        m->recalcCorrelations();
        rawDataChanged();
    }
}

void QFRDRImagingFCSImageEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        disconnect(old, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
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
        selected.clear();
    }

    fillParameterSet();
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
    replotSelection(true);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->stop();
    timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
}

void QFRDRImagingFCSImageEditor::updateAfterClick() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    replotData();
    //replotSelection(true);
    replotMask();
    updateSelectionHistogram(true);
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImageEditor::rawDataChanged() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    replotSelection(false);
    replotOverview();
    replotImage();
    replotData();
    replotMask();
    updateHistogram();
    QApplication::restoreOverrideCursor();
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
            plteImage->get_colorBarRightAxis()->set_minTicks(3);
            plteImage->get_colorBarTopAxis()->set_minTicks(5);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(3);
            plteGofImage->get_colorBarRightAxis()->set_minTicks(3);
            plteGofImage->get_colorBarTopAxis()->set_minTicks(5);
        } else {
            pltImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteImage->get_colorBarRightAxis()->set_minTicks(5);
            plteImage->get_colorBarTopAxis()->set_minTicks(3);
            pltGofImage->get_plotter()->getXAxis()->set_minTicks(7);
            plteGofImage->get_colorBarRightAxis()->set_minTicks(5);
            plteGofImage->get_colorBarTopAxis()->set_minTicks(3);
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

void QFRDRImagingFCSImageEditor::setSelectedOverlayStyle(JKQTPOverlayImageEnhanced* plot) {
    if (cmbSelectionStyle->currentIndex()==1) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("red"));
        plot->set_symbolSizeFactor(0.7);
    } else if (cmbSelectionStyle->currentIndex()==2) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("black"));
        plot->set_symbolSizeFactor(0.7);
    } else if (cmbSelectionStyle->currentIndex()==3) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("white"));
        plot->set_symbolSizeFactor(0.7);
    } else if (cmbSelectionStyle->currentIndex()==4) {
        plot->set_drawAsRectangles(false);
        plot->set_trueColor(QColor("grey"));
        plot->set_symbolSizeFactor(0.7);
    } else {
        plot->set_drawAsRectangles(true);
        plot->set_trueColor(selectionColor);
        plot->set_symbolSizeFactor(0.7);
    }
}

void QFRDRImagingFCSImageEditor::replotSelection(bool replot) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //qDebug()<<"replotSelection";
    pltOverview->set_doDrawing(false);
    pltImage->set_doDrawing(false);
    pltGofImage->set_doDrawing(false);
    pltMask->set_doDrawing(false);

    saveImageSettings();

    updateSelectionArrays();

    updateOverlaySettings();

    if (!m) {
        plteOverviewSelected->set_data(NULL, 1, 1);
        plteOverviewExcluded->set_data(NULL, 1, 1);
        plteImageSelected->set_data(NULL, 1, 1);
        plteImageExcluded->set_data(NULL, 1, 1);
        plteGofImageSelected->set_data(NULL, 1, 1);
        plteGofImageExcluded->set_data(NULL, 1, 1);
        plteMask->set_data(NULL, 1, 1);
        plteMaskSelected->set_data(NULL, 1, 1);
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

        plteMaskSelected->set_width(w);
        plteMaskSelected->set_height(h);
        plteMaskSelected->set_data(plteOverviewSelectedData, m->getDataImageWidth(), m->getDataImageHeight());

        plteMask->set_width(w);
        plteMask->set_height(h);
        plteMask->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());


    }

    pltOverview->set_doDrawing(true);
    pltImage->set_doDrawing(true);
    pltGofImage->set_doDrawing(true);
    pltMask->set_doDrawing(true);
    if (replot) {
        pltOverview->update_plot();
        pltImage->update_plot();
        pltGofImage->update_plot();
        pltMask->update_plot();
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
            plteOverview->get_colorBarRightAxis()->set_minTicks(3);
            plteOverview->get_colorBarTopAxis()->set_minTicks(5);
        } else {
            pltOverview->get_plotter()->getXAxis()->set_minTicks(7);
            plteOverview->get_colorBarRightAxis()->set_minTicks(5);
            plteOverview->get_colorBarTopAxis()->set_minTicks(3);
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

void QFRDRImagingFCSImageEditor::replotMask() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltMask->set_doDrawing(false);

    if (!m) {
        plteMask->set_data(NULL, 1, 1);
        plteMaskSelected->set_data(NULL, 1, 1);
    } else {
        double w=m->getDataImageWidth();
        double h=m->getDataImageHeight();
        if ((w==0) || (h==0)) {
            w=h=1;
        }

        pltMask->setAbsoluteXY(0, w, 0, h);
        pltMask->get_plotter()->set_maintainAspectRatio(true);
        pltMask->get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        pltMask->get_plotter()->set_maintainAxisAspectRatio(true);
        pltMask->get_plotter()->set_axisAspectRatio(1.0*w/h);

        pltMask->setXY(0, w, 0, h);

        if (plteOverviewExcludedData) plteMask->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());
        else plteMask->set_data(NULL, m->getDataImageWidth(), m->getDataImageHeight());
        plteMask->set_width(w);
        plteMask->set_height(h);
    }

    pltMask->set_doDrawing(true);
    pltMask->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImageEditor::replotData() {
     //qDebug()<<"replotData";
    QTime t;
    t.start();

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
    plotterResid->set_doDrawing(false);
    plotterResid->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
     //qDebug()<<"replotData   settings sliders: "<<t.elapsed(); t.start();
    plotter->clearGraphs();
    plotterResid->clearGraphs();
     //qDebug()<<"replotData   clearing graphs: "<<t.elapsed(); t.start();
    plotter->get_plotter()->set_showKey(chkKeys->isChecked());
    plotterResid->get_plotter()->set_showKey(chkKeys->isChecked());
     //qDebug()<<"replotData   setting show_key: "<<t.elapsed(); t.start();
    ds->clear();
     //qDebug()<<"replotData   ds->clear: "<<t.elapsed(); t.start();


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
         //qDebug()<<"replotData   add avg: "<<t.elapsed(); t.start();

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

                 //qDebug()<<"replotData   add graph "<<i<<": "<<t.elapsed(); t.start();

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
                 //qDebug()<<"replotData   search eval "<<i<<": "<<t.elapsed(); t.start();

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
                     //qDebug()<<"replotData   add fit graph "<<i<<": "<<t.elapsed(); t.start();
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
                     //qDebug()<<"replotData   add resid graph "<<i<<": "<<t.elapsed(); t.start();

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
                     //qDebug()<<"replotData   set table cells "<<i<<": "<<t.elapsed(); t.start();

                }

                free(corr);
                free(resid);
            }
        }
        tabFitvals->setReadonly(true);
        tvParams->setModel(tabFitvals);
        tvParams->resizeColumnsToContents();


         //qDebug()<<"replotData   add plots: "<<t.elapsed(); t.start();



        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(),false);
        plotterResid->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotterResid->setX(plotter->getXMin(), plotter->getXMax());
        plotterResid->zoomToFit(false, true, !chkLogTauAxis->isChecked(),false);
         //qDebug()<<"replotData   zoom: "<<t.elapsed(); t.start();

    }
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotterResid->set_doDrawing(true);
    plotterResid->set_emitSignals(true);
     //qDebug()<<"replotData   emit signals: "<<t.elapsed(); t.start();
    //QTime t;
    //t.start();
    plotter->update_plot();
     //qDebug()<<"replotData   update plots: "<<t.elapsed(); t.start();
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
    chkOverviewVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/overview_visible"), true).toBool());
    chkGofVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/gof_visible"), true).toBool());
    chkMaskVisible->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/mask_visible"), true).toBool());
    chkKeys->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_keys"), false).toBool());
    chkDisplayResiduals->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_resid"), true).toBool());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_avg"), true).toBool());
    cmbAverageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_style"), 0).toInt());
    cmbAverageErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_error_style"), 2).toInt());
    cmbColorbar->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), 0).toInt());
    cmbImageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/paramstyle"), 0).toInt());
    cmbOutOfRangeMode->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/outofrange_mode"), 1).toInt());
    chkDisplayImageOverlay->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlays"), true).toBool());
    cmbSelectionStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/image_overlay_style"), 0).toBool());
    cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_style"), 1).toInt());
    cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_error_style"), 1).toInt());
    //chkLogHistogram->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_histogram"), false).toBool());
    //chkNormalizedHistograms->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/normalized_histograms"), true).toBool());
    //chkExcludeExcludedRunsFromHistogram->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/exclude_in_histograms"), true).toBool());
    loadSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    loadSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    loadSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    //loadSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
    //loadSplitter(*(settings->getQSettings()), splitterHistogram, "imfcsimageeditor/splitterhistogramSizes");
    histogram->readSettings(*(settings->getQSettings()), "imfcsimageeditor/");
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
    settings->getQSettings()->setValue(QString("imfcsimageeditor/image_overlay_style"), cmbSelectionStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_resid"), chkDisplayResiduals->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_avg"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/paramstyle"), cmbImageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/outofrange_mode"), cmbOutOfRangeMode->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_style"), cmbAverageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_error_style"), cmbAverageErrorStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_style"), cmbRunStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    settings->getQSettings()->value(QString("imfcsimageeditor/overview_visible"), chkOverviewVisible->isChecked());
    settings->getQSettings()->value(QString("imfcsimageeditor/gof_visible"), chkGofVisible->isChecked());
    settings->getQSettings()->value(QString("imfcsimageeditor/mask_visible"), chkMaskVisible->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/log_histogram"), chkLogHistogram->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/normalized_histograms"), chkNormalizedHistograms->isChecked());
    //settings->getQSettings()->setValue(QString("imfcsimageeditor/exclude_in_histograms"), chkExcludeExcludedRunsFromHistogram->isChecked());
    saveSplitter(*(settings->getQSettings()), splitterTop, "imfcsimageeditor/splittertopSizes");
    saveSplitter(*(settings->getQSettings()), splitterBot, "imfcsimageeditor/splitterbotSizes");
    //saveSplitter(*(settings->getQSettings()), splitterBotPlots, "imfcsimageeditor/splitterbotplotsSizes");
    saveSplitter(*(settings->getQSettings()), splitterTopBot, "imfcsimageeditor/splittertopbotSizes");
    //saveSplitter(*(settings->getQSettings()), splitterHistogram, "imfcsimageeditor/splitterhistogramSizes");
    histogram->writeSettings(*(settings->getQSettings()), "imfcsimageeditor/");
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
    updateOverlaySettings();
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
            connect(chkLogTauAxis, SIGNAL(toggled(bool)), this, SLOT(replotData()));
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
        disconnect(chkLogTauAxis, SIGNAL(toggled(bool)), this, SLOT(replotData()));
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
            connect(cmbOutOfRangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
            connect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
            connect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
            connect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
            connect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(replotSelection()));
            connect(cmbImageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
            connect(cmbSelectionStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
            connect(chkExcludeExcludedRunsFromHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            histogram->connectParameterWidgets(connectTo);
            connect(histogram, SIGNAL(settingsChanged()), this, SLOT(saveImageSettings()));
        }
    } else {
        connectParameterWidgetsCounter++;
        disconnect(cmbResultGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterSetChanged()));
        disconnect(cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbGofParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(parameterChanged()));
        disconnect(cmbParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbGofParameterTransform, SIGNAL(currentIndexChanged(int)), this, SLOT(transformChanged()));
        disconnect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
        disconnect(cmbOutOfRangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(paletteChanged()));
        disconnect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(paletteChanged()));
        disconnect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(paletteChanged()));
        disconnect(cmbImageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
        disconnect(chkDisplayImageOverlay, SIGNAL(toggled(bool)), this, SLOT(replotSelection()));
        disconnect(cmbSelectionStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotSelection()));
        disconnect(chkExcludeExcludedRunsFromHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        histogram->connectParameterWidgets(connectTo);
        disconnect(histogram, SIGNAL(settingsChanged()), this, SLOT(saveImageSettings()));
    }

    //qDebug()<<"connectParameterWidgets ...  done ...  cmbResultGroup->isEnabled="<<cmbResultGroup->isEnabled()<<"  cmbResultGroup->currentIndex="<<cmbResultGroup->currentIndex()<<"  cmbResultGroup->count="<<cmbResultGroup->count();

}

void QFRDRImagingFCSImageEditor::imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender) {
    if (sender==pltImage) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
        pltMask->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltGofImage) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
        pltImage->setXY(newxmin, newxmax, newymin, newymax);
        pltMask->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltOverview) {
        pltGofImage->setXY(newxmin, newxmax, newymin, newymax);
        pltImage->setXY(newxmin, newxmax, newymin, newymax);
        pltMask->setXY(newxmin, newxmax, newymin, newymax);
    } else if (sender==pltMask) {
        pltOverview->setXY(newxmin, newxmax, newymin, newymax);
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

        int i=tabDisplay->currentIndex();
        tabDisplay->setCurrentIndex(0);
        tabDisplay->setCurrentIndex(1);
        tabDisplay->setCurrentIndex(i);


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
    int i=tabDisplay->currentIndex();
    tabDisplay->setCurrentIndex(0);
    tabDisplay->setCurrentIndex(1);
    tabDisplay->setCurrentIndex(i);
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
    QTextTable* table = cursor.insertTable(5, 2, tableFormat);
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
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>image style:</i> <b>%1</b></small>").arg(cmbImageStyle->currentText())));
        tabCursor=table->cellAt(4, 0).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>color range:</i> <b>%1 ... %2</b></small>").arg(edtColMin->value()).arg(edtColMax->value())));
        tabCursor=table->cellAt(4, 1).firstCursorPosition();
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<small><i>out-of-range mode:</i> <b>%1</b></small>").arg(cmbOutOfRangeMode->currentText())));
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.insertText(tr("Parameter Images:\n").arg(m->getName()), fHeading2);
    double w1=pltImage->width();
    double w2=(chkGofVisible->isChecked())?pltGofImage->width():0;
    double w3=(chkOverviewVisible->isChecked())?pltOverview->width():0;
    double w4=(chkMaskVisible->isChecked())?pltMask->width():0;

    double allwidth=w1+w2+w3+w4;
    table = cursor.insertTable(1, 4, tableFormat);
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
    if (chkGofVisible->isChecked()) {
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
    if (chkMaskVisible->isChecked()){
        QTextCursor tabCursor=table->cellAt(0, 2).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltMask->get_plotter()->draw(*painter, QRect(0,0,pltMask->width(),pltMask->height()));
        delete painter;
        double scale=document->textWidth()*w4/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("mask:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    QApplication::processEvents();
    if (chkOverviewVisible->isChecked()){
        QTextCursor tabCursor=table->cellAt(0, 3).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltOverview->get_plotter()->draw(*painter, QRect(0,0,pltOverview->width(),pltOverview->height()));
        delete painter;
        double scale=document->textWidth()*w3/allwidth*0.9/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("overview image:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    table = cursor.insertTable(2,1, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        plotter->get_plotter()->draw(*painter, QRect(0,0,plotter->width(),plotter->height()+plotterResid->height()));
        delete painter;
        double scale=0.6;//document->textWidth()*(double)plotter->width()/allwidth/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation curves:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);

        tabCursor=table->cellAt(1,0).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><font size=\"-6\">%1</font></center>").arg(tvParams->toHtml())));
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();



    histogram->writeReport(cursor, document);
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
    histogram->replotHistogram();
}

void QFRDRImagingFCSImageEditor::updateHistogram() {

    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    histogram->clear();

    if (plteImageData && (plteImageSize>=m->getDataImageWidth()*m->getDataImageHeight())) {
        int imageSize=m->getDataImageWidth()*m->getDataImageHeight();
        datahist=(double*)malloc(imageSize*sizeof(double));
        datasize=0;
        if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
            for (register int32_t i=0; i<imageSize; i++) {
                if (!m->leaveoutRun(i)) {
                    datahist[datasize]=plteImageData[i];
                    datasize++;
                }
            }
        } else  {
            for (register int32_t i=0; i<imageSize; i++) {
                datahist[datasize]=plteImageData[i];
                datasize++;
            }
        }
        histogram->addHistogram(tr("complete"), datahist, datasize);
    }

    updateSelectionHistogram(false);
    histogram->updateHistogram(true);

}

void QFRDRImagingFCSImageEditor::updateSelectionHistogram(bool replot) {


    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;



    if (plteImageData && (plteImageSize>=m->getDataImageWidth()*m->getDataImageHeight())) {
        int imageSize=m->getDataImageWidth()*m->getDataImageHeight();
        datahistsel=(double*)malloc(imageSize*sizeof(double));
        datasizesel=0;
        int32_t ii=0;
        if (chkExcludeExcludedRunsFromHistogram->isChecked()) {
            for (register int32_t i=0; i<imageSize; i++) {
                if (selected.contains(i) && !m->leaveoutRun(i)) {
                    datahistsel[datasizesel]=plteImageData[i];
                    datasizesel++;
                }
            }
        } else  {
            for (register int32_t i=0; i<imageSize; i++) {
                if (selected.contains(i)) {
                    datahistsel[datasizesel]=plteImageData[i];
                    datasizesel++;
                }
            }
        }
        if (datasizesel>2) {
            if (histogram->histogramCount()>1) {
                histogram->setHistogram(1, tr("selection"), datahistsel, datasizesel);
            } else {
                histogram->addHistogram(tr("selection"), datahistsel, datasizesel);
            }
        }
    }

    if (replot) histogram->updateHistogram(true, 1);

}

void QFRDRImagingFCSImageEditor::moveColorbarsAuto() {
    if ((double)pltImage->width()>(double)pltImage->height()) {
        plteImage->set_colorBarRightVisible(true);
        plteImage->set_colorBarTopVisible(false);
    } else {
        plteImage->set_colorBarRightVisible(false);
        plteImage->set_colorBarTopVisible(true);
    }
    if ((double)pltGofImage->width()>(double)pltGofImage->height()) {
        plteGofImage->set_colorBarRightVisible(true);
        plteGofImage->set_colorBarTopVisible(false);
    } else {
        plteGofImage->set_colorBarRightVisible(false);
        plteGofImage->set_colorBarTopVisible(true);
    }
    if ((double)pltOverview->width()>(double)pltOverview->height()) {
        plteOverview->set_colorBarRightVisible(true);
        plteOverview->set_colorBarTopVisible(false);
    } else {
        plteOverview->set_colorBarRightVisible(false);
        plteOverview->set_colorBarTopVisible(true);
    }

}

void QFRDRImagingFCSImageEditor::showHidePlots()  {
    bool changed=  (splitterTop->widget(1)->isVisible() != chkGofVisible->isChecked())
                 ||(splitterTop->widget(2)->isVisible() != chkMaskVisible->isChecked())
                 ||(splitterTop->widget(3)->isVisible() != chkOverviewVisible->isChecked());

    if (changed) {
        int oldcnt=1;
        if (splitterTop->widget(1)->isVisible()) oldcnt++;
        if (splitterTop->widget(2)->isVisible()) oldcnt++;
        if (splitterTop->widget(3)->isVisible()) oldcnt++;

        int cnt=1;
        if (chkGofVisible->isChecked()) cnt++;
        if (chkMaskVisible->isChecked()) cnt++;
        if (chkOverviewVisible->isChecked()) cnt++;

        splitterTop->widget(1)->setVisible(chkGofVisible->isChecked());
        splitterTop->widget(2)->setVisible(chkMaskVisible->isChecked());
        splitterTop->widget(3)->setVisible(chkOverviewVisible->isChecked());

        if (cnt!=oldcnt) {
            QList<int> list;
            int corrWidth=splitterTop->width()-(cnt-1)*splitterTop->handleWidth();
            for (int i=0; i<splitterTop->count(); i++) {
                if (splitterTop->widget(i)->isVisible()) list<<corrWidth/cnt;
                else list<<0;
            }
            splitterTop->setSizes(list);
        }
    }
}

void QFRDRImagingFCSImageEditor::includeAll() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        m->leaveoutClear();
        m->recalcCorrelations();
    }
    rawDataChanged();
}
