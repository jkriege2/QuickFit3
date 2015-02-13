/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfespimb040cameraview.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "lmcurve.h"
#include "qmoretextobject.h"
#include "datatable2.h"
#include "qfespimb040opticssetup.h"
#include <typeinfo>
#include "qfmathtools.h"
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QtGlobal>
#include <qfversion.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>

double fGauss( double t, const double *p )
{
    const double offset=p[0];
    const double A=p[1];
    const double avg=p[2];
    const double var=p[3]; 
    return offset+A*exp(-2.0*(t-avg)*(t-avg)/var);
}

double fSlit( double t, const double *p )
{
    const double offset=p[0];
    const double A=p[1];
    const double avg=p[2];
    const double d=p[3];
    const double x=M_PI*(t-avg)/d;
    const double s=qfSinc(x);
    return offset+A*s*s;
}

double fSigmoid( double t, const double *p )
{
    const double offset=p[0];
    const double A=p[1];
    const double avg=p[2];
    const double d=p[3];
    return offset+A/(1.0+exp(-1.0*(t-avg)/d));
}


#define LABEL_UPDATE_INTERVAL_MS 250
#define HISTOGRAM_UPDATE_INTERVAL_MS 250

#define MARGINAL_FIT_SIZE_FACTOR 3
#define MARGINAL_FIT_MINVALUES 100

QFESPIMB040CameraView::QFESPIMB040CameraView(QWidget* parent, int cameraID, QFCameraConfigComboBoxStartResume* stopresume, QFESPIMB040OpticsSetupBase* opticsSetup):
    QWidget(parent)
{
    setWindowTitle(tr("Preview Camera %1").arg(cameraID+1));
    setWindowIcon(QIcon(":/spimb040_logo.png"));
    this->cameraID=cameraID;

    chkCountsRangeAutoHigh=NULL;
    chkCountsRangeAutoLow=NULL;
    labImageStatisticsText="";
    maskEmpty=true;
    pixelWidth=1;
    pixelHeight=1;
    measureX[0]=measureX[1]=0;
    measureY[0]=measureY[1]=0;
    //measureFirst=true;
    m_stopresume=stopresume;
    this->opticsSetup=opticsSetup;

    MarginalLeftWidth=0;
    MarginalTopWidth=0;
    MarginalLeftPosition=0;
    MarginalTopPosition=0;

    // more variable initialisation
    imageStatisticsCalculated=false;
    currentlyRedrawing=false;

    //initialise image histogram data arrays
    histogram_n=255;
    histogram_x=(double*)qfMalloc(histogram_n*sizeof(double));
    histogram_y=(double*)qfMalloc(histogram_n*sizeof(double));
    for (unsigned int i=0; i<histogram_n; i++) {
        histogram_x[i]=i;
        histogram_y[i]=0;
    }

    // initialize image and mask
    rawImage.assign(100, 100, 0);
    image.assign(100, 100, 0);
    mask.assign(100, 100, false);
    for (uint32_t x=0; x<image.width(); x++) {
        for (uint32_t y=0; y<image.height(); y++) {
            double r=sqrt(x*x+y*y);
            rawImage(x,y)=round((cos(r/10)+1.1)*250);
        }
    }
    image.assign(rawImage);

    pltDataMarginalBottomN=image.width();
    pltDataMarginalLeftN=image.height();
    pltDataMarginalBottomX=(double*)qfCalloc(pltDataMarginalBottomN,sizeof(double));
    pltDataMarginalBottomY=(double*)qfCalloc(pltDataMarginalBottomN,sizeof(double));
    pltDataMarginalLeftX=(double*)qfCalloc(pltDataMarginalLeftN,sizeof(double));
    pltDataMarginalLeftY=(double*)qfCalloc(pltDataMarginalLeftN,sizeof(double));
    pltDataMarginalBottomFitN=qMax((uint32_t)MARGINAL_FIT_MINVALUES,pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR);
    pltDataMarginalLeftFitN=qMax((uint32_t)MARGINAL_FIT_MINVALUES,pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR);
    pltDataMarginalFitBottomY=(double*)qfCalloc(pltDataMarginalBottomFitN,sizeof(double));
    pltDataMarginalFitLeftY=(double*)qfCalloc(pltDataMarginalLeftFitN,sizeof(double));
    pltDataMarginalFitBottomX=(double*)qfCalloc(pltDataMarginalBottomFitN,sizeof(double));
    pltDataMarginalFitLeftX=(double*)qfCalloc(pltDataMarginalLeftFitN,sizeof(double));
    pltDataMarginalXPixelF=pltDataMarginalXPixel=0;
    pltDataMarginalYPixelF=pltDataMarginalYPixel=0;
    pltDataMarginalLeftYMin=0;
    pltDataMarginalLeftYMax=250;
    pltDataMarginalBottomYMin=0;
    pltDataMarginalBottomYMax=250;



    // create widgets and actions
    createMainWidgets();
    createActions();



    // display test images set above
    clearImage();

}

void QFESPIMB040CameraView::init(int cameraID, QFCameraConfigComboBoxStartResume* stopresume, QFESPIMB040OpticsSetupBase *opticsSetup) {
    setWindowTitle(tr("Preview Camera %1").arg(cameraID+1));
    m_stopresume=stopresume;
    this->opticsSetup=opticsSetup;
    this->cameraID=cameraID;
}

QFESPIMB040CameraView::~QFESPIMB040CameraView()
{
    qfFree(histogram_x);
    qfFree(histogram_y);
    qfFree(pltDataMarginalLeftX);
    qfFree(pltDataMarginalLeftY);
    qfFree(pltDataMarginalBottomX);
    qfFree(pltDataMarginalBottomY);
    qfFree(pltDataMarginalFitBottomY);
    qfFree(pltDataMarginalFitLeftY);
    qfFree(pltDataMarginalFitBottomX);
    qfFree(pltDataMarginalFitLeftX);
}

void QFESPIMB040CameraView::hideEvent(QHideEvent * event) {
    if (m_stopresume) m_stopresume->stop();
}

void QFESPIMB040CameraView::setPixelSize(double pixelWidth, double pixelHeight) {
    this->pixelHeight=pixelHeight;
    this->pixelWidth=pixelWidth;
}

void QFESPIMB040CameraView::createMainWidgets() {

    ///////////////////////////////////////////////////////////////
    // create main layout, toolbar and Splitters
    ///////////////////////////////////////////////////////////////
    QVBoxLayout* vbl=new QVBoxLayout();
    setLayout(vbl);
    vbl->setContentsMargins(0,0,0,0);
    toolbar=new QToolBar(tr("camera_toolbar"), this);
    vbl->addWidget(toolbar);
    splitVert=new QVisibleHandleSplitter(Qt::Horizontal, this);
    //splitHor=new QVisibleHandleSplitter(Qt::Vertical, this);
    //vbl->addWidget(splitHor);
    vbl->addWidget(splitVert);

    spinSaveSeriesFrames=new QSpinBox(this);
    spinSaveSeriesFrames->setRange(1,1000);
    spinSaveSeriesFrames->setValue(5);


    QWidget* wPltMain=new QWidget(this);
    vbl=new QVBoxLayout();
    vbl->setContentsMargins(0,0,0,0);
    vbl->setSpacing(0);

    ///////////////////////////////////////////////////////////////
    // create label for the mouse position
    ///////////////////////////////////////////////////////////////
    labCurrentPos=new QLabel(wPltMain);
    labCurrentPos->setAlignment(Qt::AlignHCenter);
    vbl->addWidget(labCurrentPos);
    wPltMain->setLayout(vbl);


    ///////////////////////////////////////////////////////////////
    // create main plotter
    ///////////////////////////////////////////////////////////////
    QGridLayout* layPlt=new QGridLayout();
    layPlt->setContentsMargins(0,0,0,0);
    pltMain=new JKQTFastPlotter(wPltMain);
    pltMain->set_maintainAspectRatio(true);
    pltMain->setXRange(0,100);
    pltMain->setYRange(0,100);
    pltMain->set_xTickDistance(10);
    pltMain->set_yTickDistance(10);
    pltMain->set_xAxisLabelVisible(false);
    pltMain->set_yAxisLabelVisible(false);
    pltMain->set_plotBorderLeft(30);
    pltMain->set_plotBorderBottom(20);
    plteFrame=new JKQTFPimagePlot(pltMain, image.data(), JKQTFP_double, image.width(), image.height(), JKQTFP_GRAY);
    pltMain->addPlot(plteFrame);
    plteMask=new JKQTFPimageOverlayPlot(pltMain, mask.data(), mask.width(), mask.height());
    pltMain->addPlot(plteMask);
    plteGrid=new JKQTFPQOverlayLinearGridPlot(pltMain);
    plteGrid->setVisible(false);
    pltMain->addPlot(plteGrid);
    plteMarginalPos=new JKQTFPVCrossPlot(pltMain, 1, &pltDataMarginalXPixelF, &pltDataMarginalYPixelF, QColor("red"));
    plteMarginalPos->set_crossWidth(15);
    plteMainDistance=new JKQTFPLinePlot(pltMain, 2, measureX, measureY, QColor("red"));
    pltMain->addPlot(plteMarginalPos);
    pltMain->set_dragLine(true);
    connect(pltMain, SIGNAL(mouseMoved(double, double)), this, SLOT(imageMouseMoved(double, double)));
    connect(pltMain, SIGNAL(clicked(double, double)), this, SLOT(imageMouseClicked(double, double)));
    connect(pltMain, SIGNAL(mouseDragged(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageMouseDragged(double,double,double,double,Qt::KeyboardModifiers)));
    layPlt->addWidget(pltMain, 0, 1);
    //vbl->setStretchFactor(pltMain, 10);
    ///////////////////////////////////////////////////////////////
    // create main marginal plotters
    ///////////////////////////////////////////////////////////////
    pltMarginalBottom=new JKQTFastPlotter(wPltMain);
    layPlt->addWidget(pltMarginalBottom, 1,1);
    pltMarginalBottom->setMaximumHeight(150);
    pltMarginalBottom->set_synchronizeX(pltMain);
    pltMarginalBottom->set_xAxisLabelVisible(false);
    pltMarginalBottom->set_yAxisLabelVisible(false);
    pltMarginalBottom->set_plotBorderTop(5);
    pltMarginalBottom->set_plotBorderBottom(5);
    plteMarginalBottom=new JKQTFPLinePlot(pltMarginalBottom, pltDataMarginalBottomN, pltDataMarginalBottomX, pltDataMarginalBottomY);
    plteMarginalFitBottom=new JKQTFPLinePlot(pltMarginalBottom, pltDataMarginalBottomN, pltDataMarginalBottomX, pltDataMarginalBottomY, QColor("blue"));
    pltMarginalBottom->addPlot(plteMarginalBottom);
    pltMarginalLeft=new JKQTFastPlotter(wPltMain);
    layPlt->addWidget(pltMarginalLeft, 0,0);
    pltMarginalLeft->setMaximumWidth(150);
    pltMarginalLeft->set_synchronizeY(pltMain);
    pltMarginalLeft->set_xAxisLabelVisible(false);
    pltMarginalLeft->set_yAxisLabelVisible(false);
    pltMarginalLeft->set_plotBorderLeft(5);
    pltMarginalLeft->set_plotBorderRight(5);
    plteMarginalLeft=new JKQTFPLinePlot(pltMarginalLeft, pltDataMarginalLeftN, pltDataMarginalLeftY, pltDataMarginalLeftX);
    plteMarginalFitLeft=new JKQTFPLinePlot(pltMarginalLeft, pltDataMarginalLeftN, pltDataMarginalLeftY, pltDataMarginalLeftX, QColor("blue"));
    pltMarginalLeft->addPlot(plteMarginalLeft);


    layPlt->setColumnStretch(0,2);
    layPlt->setColumnStretch(1,5);
    layPlt->setRowStretch(0,5);
    layPlt->setRowStretch(1,2);


    vbl->addLayout(layPlt, 10);
    tabSettings=new QTabWidget(this);
    //tabResults=new QTabWidget(this);

    splitVert->addWidget(wPltMain);
    splitVert->addWidget(tabSettings);
    //splitHor->addWidget(splitVert);
    //splitHor->addWidget(tabResults);


    ///////////////////////////////////////////////////////////////
    // create label for image acquisition data
    ///////////////////////////////////////////////////////////////
    labVideoSettings=new QLabel(wPltMain);
    labVideoSettings->setAlignment(Qt::AlignRight);
    vbl->addWidget(labVideoSettings);
    wPltMain->setLayout(vbl);




    /////////////////////////////////////////////////////////////////////
    // image statistics/histogram
    /////////////////////////////////////////////////////////////////////
    QWidget* w=new QWidget(this);
    vbl=new QVBoxLayout();
    w->setLayout(vbl);

    QHBoxLayout* stathbl=new QHBoxLayout();
    vbl->addLayout(stathbl);
    stathbl->setContentsMargins(0,0,0,0);
    chkImageStatisticsHistogram=new QCheckBox(tr("image histogram"), w);
    stathbl->addWidget(chkImageStatisticsHistogram);
    stathbl->addStretch();
    connect(chkImageStatisticsHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramChecked(bool)));


    pltCountsHistogram=new JKQTFastPlotter(w);
    QColor bt=QColor("blue");
    bt.setAlphaF(0.5);
    plteHistogramRange=new JKQTFPXRangePlot(pltCountsHistogram, 0, 255, bt, Qt::SolidLine, 1, Qt::SolidPattern);
    plteHistogram=new JKQTFPVBarPlot(pltCountsHistogram, histogram_n, histogram_x, histogram_y, QColor("red"), Qt::SolidLine, 2);
    pltCountsHistogram->addPlot(plteHistogramRange);
    pltCountsHistogram->addPlot(plteHistogram);
    pltCountsHistogram->set_xAxisLabel("grayvalue");
    pltCountsHistogram->set_yAxisLabel("# pixels");

    vbl->addWidget(pltCountsHistogram);
    QHBoxLayout* hbl;

    QGridLayout* gl=new QGridLayout();
    spinCountsLower=new QDoubleSpinBox(w);
    spinCountsLower->setMaximum(0xEFFFFF);
    spinCountsLower->setMinimum(-0xEFFFFF);
    QLabel* l=new QLabel(tr("m&in. gray:"), w);
    l->setBuddy(spinCountsLower);
    gl->addWidget(l, 0, 0);
    gl->addWidget(spinCountsLower,0,1);
    chkCountsRangeAutoLow=new QCheckBox(tr("&auto"), w);
    gl->addWidget(chkCountsRangeAutoLow,1,1);
    spinCountsUpper=new QDoubleSpinBox(w);
    spinCountsUpper->setMaximum(0xEFFFFF);
    spinCountsUpper->setMinimum(-0xEFFFFF);
    // NOTE:
    // spinCountsLower and spinCountsUpper are connected/disconnected by a call to setCountsAutoscale():
    setCountsAutoscale(true);

    l=new QLabel(tr("m&ax. gray:"), w);
    l->setBuddy(spinCountsUpper);
    gl->addWidget(l,0,2);
    gl->addWidget(spinCountsUpper,0,3);
    chkCountsRangeAutoHigh=new QCheckBox(tr("&auto"), w);

    gl->addWidget(chkCountsRangeAutoHigh,1,3);
    gl->addWidget(new QWidget(w),0,4);
    gl->setColumnStretch(4,1);
    vbl->addLayout(gl);

    connect(chkCountsRangeAutoLow, SIGNAL(clicked(bool)), this, SLOT(setCountsAutoscale(bool)));
    connect(chkCountsRangeAutoHigh, SIGNAL(clicked(bool)), this, SLOT(setCountsAutoscale(bool)));


    hbl=new QHBoxLayout();
    spinHistogramBins=new QSpinBox(w);
    spinHistogramBins->setMinimum(1);
    spinHistogramBins->setMaximum(0xEFFFFF);
    connect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(displayImageStatistics()));
    l=new QLabel(tr("# bins:"), w);
    l->setBuddy(spinHistogramBins);
    hbl->addWidget(l);
    hbl->addWidget(spinHistogramBins);
    chkHistogramBinsAuto=new QCheckBox(tr("&auto histogram bins"), w);
    connect(chkHistogramBinsAuto, SIGNAL(clicked(bool)), this, SLOT(setHistogramBinsAutoscale(bool)));
    hbl->addWidget(chkHistogramBinsAuto);
    hbl->addStretch(2);
    vbl->addLayout(hbl);
    chkHistogramLog=new QCheckBox(tr("&log scale historam"), w);
    connect(chkHistogramLog, SIGNAL(stateChanged(int)), this, SLOT(displayImageStatistics()));
    vbl->addWidget(chkHistogramLog);

    chkFindDefectivePixels=new QCheckBox(tr("&find defective pixels automatically"), w);
    connect(chkFindDefectivePixels, SIGNAL(stateChanged(int)), this, SLOT(displayImageStatistics()));
    vbl->addWidget(chkFindDefectivePixels);

    //labImageStatistics=new QLabel(w);
    labImageStatistics=new QFastTableLabel(w);
    labImageStatistics->setGrid(false);
    vbl->addWidget(labImageStatistics);

    //labMarginalFitResults=new QLabel(w);
    labMarginalFitResults=new QFastTableLabel(w);
    labMarginalFitResults->setGrid(false);
    vbl->addWidget(labMarginalFitResults);

    tabSettings->addTab(w, tr("&Statistics"));
    setCountsAutoscale(false);








    /////////////////////////////////////////////////////////////////////
    // image graph pane
    /////////////////////////////////////////////////////////////////////
    w=new QWidget(this);
    QGridLayout* gvbl=new QGridLayout();
    w->setLayout(gvbl);
    chkGraph=new QCheckBox(tr("enable graph display"), w);
    chkGraph->setChecked(false);
    connect(chkGraph, SIGNAL(toggled(bool)), this, SLOT(enableGraph(bool)));
    gvbl->addWidget(chkGraph, 0,0);
    btnClearGraph=new QPushButton(tr("clear"), w);
    gvbl->addWidget(btnClearGraph, 0,1);
    connect(btnClearGraph, SIGNAL(clicked()), this, SLOT(clearGraph()));
    gsplitter=new QVisibleHandleSplitter(Qt::Vertical, w);
    gvbl->addWidget(gsplitter,1,0,1,2);
    gvbl->setColumnStretch(0,1);
    gvbl->setRowStretch(1,1);

    pltGraph=new JKQTFastPlotter(gsplitter);
    plteGraphRange=new JKQTFPYRangePlot(pltGraph, 0,0,QColor("blue"),Qt::DotLine, 1, Qt::SolidPattern);
    plteGraphRange->setVisible(false);
    pltGraph->addPlot(plteGraphRange);
    plteGraph=new JKQTFPLinePlot(pltGraph, &plteGraphDataX, &plteGraphDataY);
    pltGraph->addPlot(plteGraph);

    QWidget* wgplot=new QWidget(w);
    gsplitter->addWidget(wgplot);
    QVBoxLayout* ggl=new QVBoxLayout();
    wgplot->setLayout(ggl);

    ggl->addWidget(pltGraph,1);
    labGraphMean=new QLabel(this);
    QFont f=font();
    f.setPointSizeF(1.3*f.pointSizeF());
    labGraphMean->setFont(f);
    labGraphMean->setWordWrap(true);
    ggl->addWidget(labGraphMean);

    QWidget* wgset=new QWidget(w);
    gsplitter->addWidget(wgset);
    QFormLayout* fl=new QFormLayout();
    wgset->setLayout(fl);

    cmbGraphParameter=new QComboBox(w);
    fl->addRow(tr("&parameter:"), cmbGraphParameter);
    cmbGraphParameter->addItem(tr("average intensity"));
    cmbGraphParameter->addItem(tr("maximum intensity"));
    cmbGraphParameter->addItem(tr("intensity: stddev"));
    cmbGraphParameter->addItem(tr("correlation coefficient"));
    cmbGraphParameter->addItem(tr("center correlation coefficient"));
    cmbGraphParameter->addItem(tr("left marginal: width"));
    cmbGraphParameter->addItem(tr("bottom marginal: width"));
    cmbGraphParameter->addItem(tr("left marginal: position"));
    cmbGraphParameter->addItem(tr("bottom marginal: position"));
    connect(cmbGraphParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(graphParameterChanged()));

    spinGraphWindow=new QDoubleSpinBox(w);
    fl->addRow(tr("time &window:"), spinGraphWindow);
    spinGraphWindow->setRange(0.1, 1e6);
    spinGraphWindow->setSuffix(tr(" s"));
    spinGraphWindow->setValue(60);

    tabSettings->addTab(w, tr("Graph"));
    gsplitter->setCollapsible(0,false);
    gsplitter->setCollapsible(1,false);







    /////////////////////////////////////////////////////////////////////
    // image settings pane
    /////////////////////////////////////////////////////////////////////
    w=new QWidget(this);
    fl=new QFormLayout();
    w->setLayout(fl);
    cmbColorscale=new QComboBox(w);
    cmbColorscale->setMaximumWidth(200);
    QStringList sl=JKQTFPimagePlot_getPalettes();
    for (int i=0; i<sl.size(); i++) {
        cmbColorscale->addItem(JKQTFPimagePlot_getPaletteIcon(i), sl[i]);
    }
    fl->addRow(tr("color &palette:"), cmbColorscale);
    cmbMaskColor=new ColorComboBox(this);
    cmbMaskColor->setMaximumWidth(200);
    fl->addRow(tr("&mask color:"), cmbMaskColor);
    connect(cmbColorscale, SIGNAL(currentIndexChanged(int)), this, SLOT(set_palette(int)));
    connect(cmbMaskColor, SIGNAL(activated(QColor)), plteMask, SLOT(set_color(QColor)));

    cmbRotation=new QComboBox(w);
    cmbRotation->addItem(tr("0 degrees"));
    cmbRotation->addItem(tr("90 degrees"));
    cmbRotation->addItem(tr("180 degrees"));
    cmbRotation->addItem(tr("270 degrees"));
    cmbRotation->setMaximumWidth(200);
    fl->addRow(tr("&rotation:"), cmbRotation);
    connect(cmbRotation, SIGNAL(currentIndexChanged(int)), this, SLOT(redrawFrameRecalc()));

    cmbImageTransformMode=new QComboBox(w);
    cmbImageTransformMode->setMaximumWidth(200);
    cmbImageTransformMode->addItem(tr("none"));
    cmbImageTransformMode->addItem(tr("top-bottom half"));
    cmbImageTransformMode->addItem(tr("left-right half"));
    cmbImageTransformMode->addItem(tr("equalize top/bottom intensity"));
    cmbImageTransformMode->addItem(tr("equalize left/right intensity"));
    fl->addRow(tr("frame &transform:"), cmbImageTransformMode);

    QHBoxLayout* glay=new QHBoxLayout();
    glay->setContentsMargins(0,0,0,0);
    chkGrid=new QCheckBox(w);
    chkGrid->setChecked(false);
    glay->addWidget(chkGrid);
    glay->addWidget(new QLabel(tr("     width: ")));
    spinGridWidth=new QSpinBox(w);
    spinGridWidth->setRange(1,10000);
    spinGridWidth->setSingleStep(10);
    spinGridWidth->setEnabled(false);
    glay->addWidget(spinGridWidth);
    glay->addStretch();
    fl->addRow(tr("grid:"), glay);
    cmbGridColor=new ColorComboBox(w);
    cmbGridColor->setMaximumWidth(200);
    cmbGridColor->setEnabled(false);
    fl->addRow(tr("&grid color:"), cmbGridColor);
    connect(chkGrid, SIGNAL(toggled(bool)), spinGridWidth, SLOT(setEnabled(bool)));
    connect(chkGrid, SIGNAL(toggled(bool)), cmbGridColor, SLOT(setEnabled(bool)));
    connect(chkGrid, SIGNAL(toggled(bool)), this, SLOT(updateGrid()));
    connect(spinGridWidth, SIGNAL(valueChanged(int)), this, SLOT(updateGrid()));
    connect(cmbGridColor, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGrid()));

    cmbMarginalPlots=new QComboBox(w);
    cmbMarginalPlots->addItem(tr("none"));
    cmbMarginalPlots->addItem(tr("marked line/column"));
    cmbMarginalPlots->addItem(tr("average"));
    cmbMarginalPlots->setMaximumWidth(200);
    fl->addRow(tr("&marginal plots:"), cmbMarginalPlots);
    connect(cmbMarginalPlots, SIGNAL(currentIndexChanged(int)), this, SLOT(redrawFrameRecalc()));


    cmbMarginalFitFunction=new QComboBox(w);
    cmbMarginalFitFunction->addItem(tr("none"));
    cmbMarginalFitFunction->addItem(tr("average+variance"));
    cmbMarginalFitFunction->addItem(tr("gaussian fit"));
    cmbMarginalFitFunction->addItem(tr("slit function fit"));
    cmbMarginalFitFunction->addItem(tr("sigmoidal function fit"));
    cmbMarginalFitFunction->setMaximumWidth(200);
    fl->addRow(tr("&marginal evaluation:"), cmbMarginalFitFunction);
    connect(cmbMarginalFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(redrawFrameRecalc()));


    spinTableFontsize=new QSpinBox(w);
    spinTableFontsize->setRange(1,32);
    spinTableFontsize->setValue(font().pointSize());
    spinTableFontsize->setSuffix(tr(" pt"));
    fl->addRow(tr("&table font size:"), spinTableFontsize);
    connect(spinTableFontsize, SIGNAL(valueChanged(int)), this, SLOT(setTableFontsize(int)));


    QFormLayout* flg;
    QGroupBox* grpAlign;
    grpAlign=new QGroupBox(tr(" Alignment Mode 1 Settings "), this);
    fl->addRow(grpAlign);
    flg=new QFormLayout();
    grpAlign->setLayout(flg);

    cmbAlign1ImageTransformMode=new QComboBox(grpAlign);
    for (int i=0; i<cmbImageTransformMode->count(); i++) {
        cmbAlign1ImageTransformMode->addItem(cmbImageTransformMode->itemIcon(i), cmbImageTransformMode->itemText(i), cmbImageTransformMode->itemData(i));
    }
    cmbAlign1ImageTransformMode->setCurrentIndex(cmbImageTransformMode->currentIndex());
    flg->addRow(tr("frame transform:"), cmbAlign1ImageTransformMode);


    cmbAlign1Colorscale=new QComboBox(grpAlign);;
    for (int i=0; i<cmbColorscale->count(); i++) {
        cmbAlign1Colorscale->addItem(cmbColorscale->itemIcon(i), cmbColorscale->itemText(i), cmbColorscale->itemData(i));
    }
    cmbAlign1Colorscale->setCurrentIndex(cmbColorscale->currentIndex());
    cmbAlign1Colorscale->setMaximumWidth(200);
    connect(cmbAlign1Colorscale, SIGNAL(currentIndexChanged(int)), this, SLOT(set_palette(int)));
    flg->addRow(tr("color palette:"), cmbAlign1Colorscale);

    cmbAlign1MarginalFitFunction=new QComboBox(grpAlign);
    for (int i=0; i<cmbMarginalFitFunction->count(); i++) {
        cmbAlign1MarginalFitFunction->addItem(cmbMarginalFitFunction->itemIcon(i), cmbMarginalFitFunction->itemText(i), cmbMarginalFitFunction->itemData(i));
    }
    cmbAlign1MarginalFitFunction->setCurrentIndex(cmbMarginalFitFunction->currentIndex());
    cmbAlign1MarginalFitFunction->setMaximumWidth(200);
    connect(cmbAlign1MarginalFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(redrawFrameRecalc()));
    flg->addRow(tr("marginal evaluation:"), cmbAlign1MarginalFitFunction);


    grpAlign=new QGroupBox(tr(" Alignment Mode 2 Settings "), this);
    fl->addRow(grpAlign);
    flg=new QFormLayout();
    grpAlign->setLayout(flg);



    cmbAlign2ImageTransformMode=new QComboBox(grpAlign);
    for (int i=0; i<cmbImageTransformMode->count(); i++) {
        cmbAlign2ImageTransformMode->addItem(cmbImageTransformMode->itemIcon(i), cmbImageTransformMode->itemText(i), cmbImageTransformMode->itemData(i));
    }
    cmbAlign2ImageTransformMode->setCurrentIndex(cmbImageTransformMode->currentIndex());
    flg->addRow(tr("frame transform:"), cmbAlign2ImageTransformMode);


    cmbAlign2Colorscale=new QComboBox(grpAlign);;
    for (int i=0; i<cmbColorscale->count(); i++) {
        cmbAlign2Colorscale->addItem(cmbColorscale->itemIcon(i), cmbColorscale->itemText(i), cmbColorscale->itemData(i));
    }
    cmbAlign2Colorscale->setCurrentIndex(cmbColorscale->currentIndex());
    cmbAlign2Colorscale->setMaximumWidth(200);
    connect(cmbAlign2Colorscale, SIGNAL(currentIndexChanged(int)), this, SLOT(set_palette(int)));
    flg->addRow(tr("color palette:"), cmbAlign2Colorscale);

    cmbAlign2MarginalFitFunction=new QComboBox(grpAlign);
    for (int i=0; i<cmbMarginalFitFunction->count(); i++) {
        cmbAlign2MarginalFitFunction->addItem(cmbMarginalFitFunction->itemIcon(i), cmbMarginalFitFunction->itemText(i), cmbMarginalFitFunction->itemData(i));
    }
    cmbAlign2MarginalFitFunction->setCurrentIndex(cmbMarginalFitFunction->currentIndex());
    cmbAlign2MarginalFitFunction->setMaximumWidth(200);
    connect(cmbAlign2MarginalFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(redrawFrameRecalc()));
    flg->addRow(tr("marginal evaluation:"), cmbAlign2MarginalFitFunction);

    tabSettings->addTab(w, tr("Settings"));
}



void QFESPIMB040CameraView::createActions() {
    actSaveRaw = new QAction(QIcon(":/spimb040/saveraw.png"), tr("Save &raw image as ..."), this);
    actSaveRaw->setShortcut(QKeySequence::Save);
    connect(actSaveRaw, SIGNAL(triggered()), this, SLOT(saveRaw()));

    actSaveTransformed = new QAction(QIcon(":/spimb040/savetransformed.png"), tr("Save &transformed image as ..."), this);
    connect(actSaveTransformed, SIGNAL(triggered()), this, SLOT(saveTransformedImage()));
    actSaveMulti = new QAction(QIcon(":/spimb040/savemulti.png"), tr("Save &raw image as ..."), this);
    connect(actSaveMulti, SIGNAL(triggered()), this, SLOT(saveMulti()));
    actSaveMultiSeries = new QAction(QIcon(":/spimb040/savemultiseries.png"), tr("Save &raw image series as ..."), this);
    connect(actSaveMultiSeries, SIGNAL(triggered()), this, SLOT(saveMultiSeries()));


    actMaskClear = new QAction(QIcon(":/spimb040/maskclear.png"), tr("&Clear mask (broken pixels)"), this);
    connect(actMaskClear, SIGNAL(triggered()), this, SLOT(clearMask()));

    actMaskSave = new QAction(QIcon(":/spimb040/masksave.png"), tr("&Save mask (broken pixels)"), this);
    connect(actMaskSave, SIGNAL(triggered()), this, SLOT(saveMask()));

    actMaskLoad = new QAction(QIcon(":/spimb040/maskload.png"), tr("&Load mask (broken pixels)"), this);
    connect(actMaskLoad, SIGNAL(triggered()), this, SLOT(loadMask()));
    recentMaskFiles=new QFRecentFilesMenu(this);
    recentMaskFiles->setUseSystemFileIcons(false);
    recentMaskFiles->setDefaultIcon(QIcon(":/spimb040/maskfileicon.png"));
    recentMaskFiles->setAlwaysEnabled(true);
    connect(recentMaskFiles, SIGNAL(openRecentFile(QString)), this, SLOT(loadMask(QString)));
    actMaskLoad->setMenu(recentMaskFiles);

    actMaskHisto = new QAction(QIcon(":/spimb040/maskhisto.png"), tr("&Create mask by histogram"), this);
    connect(actMaskHisto, SIGNAL(triggered()), this, SLOT(histogramMask()));

    actPrintReport = new QAction(QIcon(":/spimb040/report_print.png"), tr("&Print a report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    actSaveReport = new QAction(QIcon(":/spimb040/report_save.png"), tr("&Save a report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actSaveData = new QAction(QIcon(":/spimb040/preview_savedata.png"), tr("&Save marginal and histogram data"), this);
    connect(actSaveData, SIGNAL(triggered()), this, SLOT(saveData()));


    actgMouse=new QActionGroup(this);
    actCursor=actgMouse->addAction(QIcon(":/spimb040/cursor.png"), tr("default mouse cursor"));
    actCursor->setCheckable(true);

    actMaskEdit = new QAction(QIcon(":/spimb040/maskedit.png"), tr("&Edit mask (broken pixels)"), actgMouse);
    actMaskEdit->setCheckable(true);

    actMeasure = new QAction(QIcon(":/spimb040/measure.png"), tr("&Measure distances"), actgMouse);
    actMeasure->setCheckable(true);

    actCursor->setChecked(true);



    actgAlignMode=new QActionGroup(this);
    actNormalMode=actgAlignMode->addAction(QIcon(":/spimb040/alignmode_normal.png"), tr("Preiew Mode"));
    actNormalMode->setCheckable(true);
    connect(actNormalMode, SIGNAL(toggled(bool)), this, SLOT(redrawFrameRecalc()));

    actAlign1Mode=actgAlignMode->addAction(QIcon(":/spimb040/alignmode_1.png"), tr("Align Mode 1"));
    actAlign1Mode->setCheckable(true);
    connect(actAlign1Mode, SIGNAL(toggled(bool)), this, SLOT(redrawFrameRecalc()));

    actAlign2Mode=actgAlignMode->addAction(QIcon(":/spimb040/alignmode_2.png"), tr("Align Mode 2"));
    actAlign2Mode->setCheckable(true);
    connect(actAlign2Mode, SIGNAL(toggled(bool)), this, SLOT(redrawFrameRecalc()));


    actNormalMode->setChecked(true);




    toolbar->addAction(actSaveRaw);
    toolbar->addAction(actSaveData);
    toolbar->addAction(actSaveTransformed);
    toolbar->addAction(actSaveMulti);
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel("<b>series:</b>  #="));
    toolbar->addWidget(spinSaveSeriesFrames);
    toolbar->addAction(actSaveMultiSeries);

    toolbar->addSeparator();
    toolbar->addAction(actSaveReport);
    toolbar->addAction(actPrintReport);
    toolbar->addSeparator();
    toolbar->addAction(actMaskLoad);
    toolbar->addAction(actMaskSave);
    toolbar->addAction(actMaskEdit);
    toolbar->addAction(actMaskHisto);
    toolbar->addAction(actMaskClear);
    toolbar->addSeparator();
    toolbar->addAction(actCursor);
    toolbar->addAction(actMeasure);
    toolbar->addSeparator();
    toolbar->addAction(actNormalMode);
    toolbar->addAction(actAlign1Mode);
    toolbar->addAction(actAlign2Mode);
    toolbar->addSeparator();

}


void QFESPIMB040CameraView::loadSettings(QSettings& settings, QString prefix) {
    jkloadWidgetGeometry(settings, this, prefix+"geometry/");
    //jkloadSplitter(settings, splitHor,  prefix+"split_hor/");
    jkloadSplitter(settings, splitVert, prefix+"split_vert/");
    recentMaskFiles->readSettings(settings, prefix+"recent_mask_files/");

    spinCountsLower->setValue(settings.value(prefix+"histogram.min", 0).toInt());
    spinCountsUpper->setValue(settings.value(prefix+"histogram.max", 255).toInt());
    chkCountsRangeAutoLow->setChecked(settings.value(prefix+"histogram.autolow", true).toBool());
    chkCountsRangeAutoHigh->setChecked(settings.value(prefix+"histogram.autohigh", true).toBool());
    setCountsAutoscale();
    bool b=settings.value(prefix+"histogram.log", false).toBool();
    chkHistogramLog->setChecked(b);
    histogram_n=settings.value(prefix+"histogram.items", histogram_n).toUInt();
    spinHistogramBins->setValue(histogram_n);
    // reallocate histogram and initialize
    if (histogram_x) qfFree(histogram_x);
    if (histogram_y) qfFree(histogram_y);
    histogram_x=(double*)qfMalloc(histogram_n*sizeof(double));
    histogram_y=(double*)qfMalloc(histogram_n*sizeof(double));
    for (unsigned int i=0; i<histogram_n; i++) {
        histogram_x[i]=i;
        histogram_y[i]=0;
    }
    b=settings.value(prefix+"histogram.items_auto", true).toBool();
    chkHistogramBinsAuto->setChecked(b);
    setHistogramBinsAutoscale(b);

    cmbColorscale->setCurrentIndex(settings.value(prefix+"imagesettings.palette", 6).toInt());
    cmbMaskColor->setCurrentIndex(settings.value(prefix+"imagesettings.mask_color", 0).toInt());
    cmbRotation->setCurrentIndex(settings.value(prefix+"imagesettings.rotation", 0).toInt());
    cmbMarginalPlots->setCurrentIndex(settings.value(prefix+"imagesettings.marginal", 1).toInt());
    cmbMarginalFitFunction->setCurrentIndex(settings.value(prefix+"imagesettings.marginal_fitfunction", 0).toInt());
    pltDataMarginalXPixel=settings.value(prefix+"imagesettings.marginal_xpixel", pltDataMarginalXPixel).toInt();
    pltDataMarginalYPixel=settings.value(prefix+"imagesettings.marginal_ypixel", pltDataMarginalYPixel).toInt();

    lastImagepath=settings.value(prefix+"last_imagepath", lastImagepath).toString();
    lastMaskpath=settings.value(prefix+"last_maskpath", lastMaskpath).toString();
    lastImagefilter=settings.value(prefix+"last_imagefilter", lastImagefilter).toString();
    lastMaskHistogramMode=settings.value(prefix+"last_mask_histogram_mode", lastMaskHistogramMode).toInt();
    lastMaskHistogramPixels=settings.value(prefix+"last_mask_histogram_pixels", lastMaskHistogramMode).toInt();
    chkImageStatisticsHistogram->setChecked(settings.value(prefix+"display_imagestatistics", chkImageStatisticsHistogram->isChecked()).toBool());

    chkGrid->setChecked(settings.value(prefix+"grid", false).toBool());
    spinGridWidth->setValue(settings.value(prefix+"grid_width", 32).toInt());
    spinTableFontsize->setValue(settings.value(prefix+"table_fontsize", font().pointSize()).toInt());
    spinSaveSeriesFrames->setValue(settings.value(prefix+"saver_series_frames", 5).toInt());
    cmbGridColor->setCurrentIndex(settings.value(prefix+"grid_color", 15).toInt());
    cmbImageTransformMode->setCurrentIndex(settings.value(prefix+"image_mode", 0).toInt());


    cmbAlign1Colorscale->setCurrentIndex(settings.value(prefix+"align1_imagesettings.palette", 8).toInt());
    cmbAlign2Colorscale->setCurrentIndex(settings.value(prefix+"align2_imagesettings.palette", 6).toInt());
    cmbAlign1MarginalFitFunction->setCurrentIndex(settings.value(prefix+"align1_imagesettings.marginal_fitfunction", 2).toInt());
    cmbAlign2MarginalFitFunction->setCurrentIndex(settings.value(prefix+"align2_imagesettings.marginal_fitfunction", 2).toInt());
    cmbAlign1ImageTransformMode->setCurrentIndex(settings.value(prefix+"align1_image_mode", 2).toInt());
    cmbAlign2ImageTransformMode->setCurrentIndex(settings.value(prefix+"align2_image_mode", 4).toInt());


    chkGraph->setChecked(settings.value(prefix+"graph", false).toBool());
    cmbGraphParameter->setCurrentIndex(settings.value(prefix+"graph_parameter", 0).toInt());
    spinGraphWindow->setValue(settings.value(prefix+"graph_window", 60).toDouble());
    jkloadSplitter(settings, gsplitter, prefix+"split_graph/");
}

void QFESPIMB040CameraView::loadSettings(QFManyFilesSettings &settings, QString prefix)
{
     jkloadWidgetGeometry(settings, this, prefix+"geometry/");
     //jkloadSplitter(settings, splitHor,  prefix+"split_hor/");
     jkloadSplitter(settings, splitVert, prefix+"split_vert/");
     recentMaskFiles->readSettings(settings, prefix+"recent_mask_files/");

     spinCountsLower->setValue(settings.value(prefix+"histogram.min", 0).toInt());
     spinCountsUpper->setValue(settings.value(prefix+"histogram.max", 255).toInt());
     chkCountsRangeAutoLow->setChecked(settings.value(prefix+"histogram.autolow", true).toBool());
     chkCountsRangeAutoHigh->setChecked(settings.value(prefix+"histogram.autohigh", true).toBool());
     setCountsAutoscale();
     bool b=settings.value(prefix+"histogram.log", false).toBool();
     chkHistogramLog->setChecked(b);
     histogram_n=settings.value(prefix+"histogram.items", histogram_n).toUInt();
     spinHistogramBins->setValue(histogram_n);
     // reallocate histogram and initialize
     if (histogram_x) qfFree(histogram_x);
     if (histogram_y) qfFree(histogram_y);
     histogram_x=(double*)qfMalloc(histogram_n*sizeof(double));
     histogram_y=(double*)qfMalloc(histogram_n*sizeof(double));
     for (unsigned int i=0; i<histogram_n; i++) {
         histogram_x[i]=i;
         histogram_y[i]=0;
     }
     b=settings.value(prefix+"histogram.items_auto", true).toBool();
     chkHistogramBinsAuto->setChecked(b);
     setHistogramBinsAutoscale(b);

     cmbColorscale->setCurrentIndex(settings.value(prefix+"imagesettings.palette", 6).toInt());
     cmbMaskColor->setCurrentIndex(settings.value(prefix+"imagesettings.mask_color", 0).toInt());
     cmbRotation->setCurrentIndex(settings.value(prefix+"imagesettings.rotation", 0).toInt());
     cmbMarginalPlots->setCurrentIndex(settings.value(prefix+"imagesettings.marginal", 1).toInt());
     cmbMarginalFitFunction->setCurrentIndex(settings.value(prefix+"imagesettings.marginal_fitfunction", 2).toInt());
     pltDataMarginalXPixel=settings.value(prefix+"imagesettings.marginal_xpixel", pltDataMarginalXPixel).toInt();
     pltDataMarginalYPixel=settings.value(prefix+"imagesettings.marginal_ypixel", pltDataMarginalYPixel).toInt();

     lastImagepath=settings.value(prefix+"last_imagepath", lastImagepath).toString();
     lastMaskpath=settings.value(prefix+"last_maskpath", lastMaskpath).toString();
     lastImagefilter=settings.value(prefix+"last_imagefilter", lastImagefilter).toString();
     lastMaskHistogramMode=settings.value(prefix+"last_mask_histogram_mode", lastMaskHistogramMode).toInt();
     lastMaskHistogramPixels=settings.value(prefix+"last_mask_histogram_pixels", lastMaskHistogramMode).toInt();
     chkImageStatisticsHistogram->setChecked(settings.value(prefix+"display_imagestatistics", chkImageStatisticsHistogram->isChecked()).toBool());

     chkGrid->setChecked(settings.value(prefix+"grid", false).toBool());
     spinGridWidth->setValue(settings.value(prefix+"grid_width", 32).toInt());
     spinTableFontsize->setValue(settings.value(prefix+"table_fontsize", font().pointSize()).toInt());
     spinSaveSeriesFrames->setValue(settings.value(prefix+"saver_series_frames", 5).toInt());

     cmbGridColor->setCurrentIndex(settings.value(prefix+"grid_color", 15).toInt());
     cmbImageTransformMode->setCurrentIndex(settings.value(prefix+"image_mode", 0).toInt());


     chkGraph->setChecked(settings.value(prefix+"graph", false).toBool());
     cmbGraphParameter->setCurrentIndex(settings.value(prefix+"graph_parameter", 0).toInt());
     spinGraphWindow->setValue(settings.value(prefix+"graph_window", 60).toDouble());


     cmbAlign1Colorscale->setCurrentIndex(settings.value(prefix+"align1_imagesettings.palette", 8).toInt());
     cmbAlign2Colorscale->setCurrentIndex(settings.value(prefix+"align2_imagesettings.palette", 6).toInt());
     cmbAlign1MarginalFitFunction->setCurrentIndex(settings.value(prefix+"align1_imagesettings.marginal_fitfunction", 2).toInt());
     cmbAlign2MarginalFitFunction->setCurrentIndex(settings.value(prefix+"align2_imagesettings.marginal_fitfunction", 2).toInt());
     cmbAlign1ImageTransformMode->setCurrentIndex(settings.value(prefix+"align1_image_mode", 2).toInt());
     cmbAlign2ImageTransformMode->setCurrentIndex(settings.value(prefix+"align2_image_mode", 0).toInt());


     jkloadSplitter(settings, gsplitter, prefix+"split_graph/");
}

void QFESPIMB040CameraView::storeSettings(QSettings& settings, QString prefix) {
    jksaveWidgetGeometry(settings, this, prefix+"geometry/");
    //jksaveSplitter(settings, splitHor,  prefix+"split_hor/");
    jksaveSplitter(settings, splitVert, prefix+"split_vert/");
    recentMaskFiles->storeSettings(settings, prefix+"recent_mask_files/");

    settings.setValue(prefix+"last_imagepath", lastImagepath);
    settings.setValue(prefix+"last_maskpath", lastMaskpath);
    settings.setValue(prefix+"last_imagefilter", lastImagefilter);

    settings.setValue(prefix+"histogram.min", spinCountsLower->value());
    settings.setValue(prefix+"histogram.max", spinCountsUpper->value());
    settings.setValue(prefix+"histogram.autolow", chkCountsRangeAutoLow->isChecked());
    settings.setValue(prefix+"histogram.autohigh", chkCountsRangeAutoHigh->isChecked());
    settings.setValue(prefix+"histogram.log", chkHistogramLog->isChecked());
    settings.setValue(prefix+"histogram.items", histogram_n);

    settings.setValue(prefix+"imagesettings.palette", cmbColorscale->currentIndex());
    settings.setValue(prefix+"imagesettings.mask_color", cmbMaskColor->currentIndex());
    settings.setValue(prefix+"imagesettings.rotation", cmbRotation->currentIndex());
    settings.setValue(prefix+"imagesettings.marginal", cmbMarginalPlots->currentIndex());
    settings.setValue(prefix+"imagesettings.marginal_fitfunction", cmbMarginalFitFunction->currentIndex());
    settings.setValue(prefix+"imagesettings.marginal_xpixel", pltDataMarginalXPixel);
    settings.setValue(prefix+"imagesettings.marginal_ypixel", pltDataMarginalYPixel);

    settings.setValue(prefix+"display_imagestatistics", chkImageStatisticsHistogram->isChecked());
    settings.setValue(prefix+"last_mask_histogram_mode", lastMaskHistogramMode);
    settings.setValue(prefix+"last_mask_histogram_pixels", lastMaskHistogramMode);

    settings.setValue(prefix+"grid", chkGrid->isChecked());
    settings.setValue(prefix+"grid_width", spinGridWidth->value());
    settings.setValue(prefix+"table_fontsize", spinTableFontsize->value());
    settings.setValue(prefix+"saver_series_frames", spinSaveSeriesFrames->value());
    settings.setValue(prefix+"grid_color", cmbGridColor->currentIndex());
    settings.setValue(prefix+"image_mode", cmbImageTransformMode->currentIndex());

    settings.setValue(prefix+"graph", chkGraph->isChecked());
    settings.setValue(prefix+"graph_parameter", cmbGraphParameter->currentIndex());
    settings.setValue(prefix+"graph_window", spinGraphWindow->value());





    settings.setValue(prefix+"align1_imagesettings.palette", cmbAlign1Colorscale->currentIndex());
    settings.setValue(prefix+"align2_imagesettings.palette", cmbAlign2Colorscale->currentIndex());
    settings.setValue(prefix+"align1_imagesettings.marginal_fitfunction", cmbAlign1MarginalFitFunction->currentIndex());
    settings.setValue(prefix+"align2_imagesettings.marginal_fitfunction", cmbAlign2MarginalFitFunction->currentIndex());
    settings.setValue(prefix+"align1_image_mode", cmbAlign1ImageTransformMode->currentIndex());
    settings.setValue(prefix+"align2_image_mode", cmbAlign2ImageTransformMode->currentIndex());



    jksaveSplitter(settings, gsplitter, prefix+"split_graph/");

}

void QFESPIMB040CameraView::storeSettings(QFManyFilesSettings &settings, QString prefix)
{
    jksaveWidgetGeometry(settings, this, prefix+"geometry/");
    //jksaveSplitter(settings, splitHor,  prefix+"split_hor/");
    jksaveSplitter(settings, splitVert, prefix+"split_vert/");
    recentMaskFiles->storeSettings(settings, prefix+"recent_mask_files/");

    settings.setValue(prefix+"last_imagepath", lastImagepath);
    settings.setValue(prefix+"last_maskpath", lastMaskpath);
    settings.setValue(prefix+"last_imagefilter", lastImagefilter);

    settings.setValue(prefix+"histogram.min", spinCountsLower->value());
    settings.setValue(prefix+"histogram.max", spinCountsUpper->value());
    settings.setValue(prefix+"histogram.autolow", chkCountsRangeAutoLow->isChecked());
    settings.setValue(prefix+"histogram.autohigh", chkCountsRangeAutoHigh->isChecked());
    settings.setValue(prefix+"histogram.log", chkHistogramLog->isChecked());
    settings.setValue(prefix+"histogram.items", histogram_n);

    settings.setValue(prefix+"imagesettings.palette", cmbColorscale->currentIndex());
    settings.setValue(prefix+"imagesettings.mask_color", cmbMaskColor->currentIndex());
    settings.setValue(prefix+"imagesettings.rotation", cmbRotation->currentIndex());
    settings.setValue(prefix+"imagesettings.marginal", cmbMarginalPlots->currentIndex());
    settings.setValue(prefix+"imagesettings.marginal_fitfunction", cmbMarginalFitFunction->currentIndex());
    settings.setValue(prefix+"imagesettings.marginal_xpixel", pltDataMarginalXPixel);
    settings.setValue(prefix+"imagesettings.marginal_ypixel", pltDataMarginalYPixel);

    settings.setValue(prefix+"display_imagestatistics", chkImageStatisticsHistogram->isChecked());
    settings.setValue(prefix+"last_mask_histogram_mode", lastMaskHistogramMode);
    settings.setValue(prefix+"last_mask_histogram_pixels", lastMaskHistogramMode);

    settings.setValue(prefix+"grid", chkGrid->isChecked());
    settings.setValue(prefix+"grid_width", spinGridWidth->value());
    settings.setValue(prefix+"table_fontsize", spinTableFontsize->value());
    settings.setValue(prefix+"saver_series_frames", spinSaveSeriesFrames->value());
    settings.setValue(prefix+"grid_color", cmbGridColor->currentIndex());
    settings.setValue(prefix+"image_mode", cmbImageTransformMode->currentIndex());

    settings.setValue(prefix+"graph", chkGraph->isChecked());
    settings.setValue(prefix+"graph_parameter", cmbGraphParameter->currentIndex());
    settings.setValue(prefix+"graph_window", spinGraphWindow->value());

    settings.setValue(prefix+"align1_imagesettings.palette", cmbAlign1Colorscale->currentIndex());
    settings.setValue(prefix+"align2_imagesettings.palette", cmbAlign2Colorscale->currentIndex());
    settings.setValue(prefix+"align1_imagesettings.marginal_fitfunction", cmbAlign1MarginalFitFunction->currentIndex());
    settings.setValue(prefix+"align2_imagesettings.marginal_fitfunction", cmbAlign2MarginalFitFunction->currentIndex());
    settings.setValue(prefix+"align1_image_mode", cmbAlign1ImageTransformMode->currentIndex());
    settings.setValue(prefix+"align2_image_mode", cmbAlign2ImageTransformMode->currentIndex());

    jksaveSplitter(settings, gsplitter, prefix+"split_graph/");
}

void QFESPIMB040CameraView::imageMouseMoved(double x, double y) {
    uint32_t xx=floor(x);
    uint32_t yy=floor(y);
    double pixelW=pixelWidth;
    double pixelH=pixelHeight;
    if (cmbRotation->currentIndex()%2==1) {
        pixelW=pixelHeight;
        pixelH=pixelWidth;
    }

    if (actMeasure->isChecked()) {
        if ((xx>=0) && (xx<image.width()) && (yy>=0) && (yy<image.height())) {
            QString s=QLocale::system().toString((qlonglong)image(xx,yy));
            if (mask(xx, yy)) {
                s=s+tr(" <font color=\"red\">[broken pixel]</font>");
            }
            double dx=measureX[0]-measureX[1];
            double dy=measureY[0]-measureY[1];
            double d=sqrt(dx*dx+dy*dy);
            double du=sqrt(dx*dx*pixelW*pixelW+dy*dy*pixelH*pixelH);
            double angle=atan(dy/dx)/M_PI*180.0;
            labCurrentPos->setText(tr("<b></b>img(%1, %2) = img(%4&mu;m, %5&mu;m) = %3&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>l</i><sub>meas</sub> = %6px = %7&mu;m&nbsp;&nbsp;&nbsp;&alpha;<sub>meas</sub> = %8").arg(xx).arg(yy).arg(s).arg(xx*pixelW).arg(yy*pixelH).arg(d).arg(du).arg(angle));
        } else {
            labCurrentPos->setText("");
        }
    } else {
        if ((xx>=0) && (xx<image.width()) && (yy>=0) && (yy<image.height())) {
            QString s=QLocale::system().toString((qlonglong)image(xx,yy));
            if (mask(xx, yy)) {
                s=s+tr(" <font color=\"red\">[broken pixel]</font>");
            }
            labCurrentPos->setText(tr("<b></b>image(%1, %2) = image(%4&mu;m, %5&mu;m) = %3").arg(xx).arg(yy).arg(s).arg(xx*pixelW).arg(yy*pixelH));
        } else {
            labCurrentPos->setText("");
        }
    }
}

void QFESPIMB040CameraView::imageMouseClicked(double x, double y) {
    uint32_t xx=floor(x);
    uint32_t yy=floor(y);

    uint32_t xxold=xx;
    uint32_t yyold=yy;

    switch(cmbRotation->currentIndex()) {
        case 1: yy=xxold; xx=image.width()-yyold-1; break;
        case 2: yy=image.height()-yyold-1; xx=image.width()-xxold-1; break;
        case 3: yy=image.height()-xxold-1; xx=yyold; break;
    };

    if (actMaskEdit->isChecked()) {
        if ((xx>=0) && (xx<image.width()) && (yy>=0) && (yy<image.height())) {
            mask(xx, yy)=!mask(xx, yy);
            maskEmpty=false;
        }
        redrawFrameRecalc();
    } else if (actMeasure->isChecked()) {
        /*if (measureFirst) {
            measureX[0]=(double)xx+0.5;
            measureY[0]=(double)yy+0.5;
        } else {
            measureX[1]=(double)xx+0.5;
            measureY[1]=(double)yy+0.5;
        }
        measureFirst = !measureFirst;
        redrawFrameRecalc();*/
    } else {
        if ((xx>=0) && (xx<image.width()) && (yy>=0) && (yy<image.height())) {
            pltDataMarginalXPixel=xx;
            pltDataMarginalYPixel=yy;
        }
        redrawFrameRecalc();
        //qDebug()<<pltDataMarginalXPixelF<<pltDataMarginalYPixelF;
    }
    imageMouseMoved(x,y);
}

void QFESPIMB040CameraView::imageMouseDragged(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers)
{
    uint32_t xx=floor(x1);
    uint32_t yy=floor(y1);
    uint32_t xx2=floor(x2);
    uint32_t yy2=floor(y2);

    uint32_t xxold=xx;
    uint32_t yyold=yy;
    uint32_t xxold2=xx2;
    uint32_t yyold2=yy2;

    switch(cmbRotation->currentIndex()) {
        case 1: yy=xxold; xx=image.width()-yyold-1; yy2=xxold2; xx2=image.width()-yyold2-1; break;
        case 2: yy=image.height()-yyold-1; xx=image.width()-xxold-1; yy2=image.height()-yyold2-1; xx2=image.width()-xxold2-1; break;
        case 3: yy=image.height()-xxold-1; xx=yyold;  yy2=image.height()-xxold2-1; xx2=yyold2; break;
    };

    //qDebug()<<"mouseDragged: "<<x1<<y1<<" -> "<<x2<<y2<<"   "<<actMeasure->isChecked()<<"   "<<modifiers;
    if (actMeasure->isChecked()) {
        measureX[0]=(double)xx+0.5;
        measureY[0]=(double)yy+0.5;
        measureX[1]=(double)xx2+0.5;
        measureY[1]=(double)yy2+0.5;
        imageMouseMoved(x2,y2);
    }
}


void QFESPIMB040CameraView::setCountsAutoscale(bool autoscale) {
    if (!chkCountsRangeAutoLow) return;
    if (chkCountsRangeAutoLow->isChecked()) {
        spinCountsLower->setEnabled(false);
        disconnect(spinCountsLower, SIGNAL(valueChanged(double)), this, SLOT(redrawFrameRecalc()));
    } else {
        spinCountsLower->setEnabled(true);
        connect(spinCountsLower, SIGNAL(valueChanged(double)), this, SLOT(redrawFrameRecalc()));
    }
    if (!chkCountsRangeAutoHigh) return;
    if (chkCountsRangeAutoHigh->isChecked()) {
        spinCountsUpper->setEnabled(false);
        disconnect(spinCountsUpper, SIGNAL(valueChanged(double)), this, SLOT(redrawFrameRecalc()));
    } else {
        spinCountsUpper->setEnabled(true);
        connect(spinCountsUpper, SIGNAL(valueChanged(double)), this, SLOT(redrawFrameRecalc()));
    }
}

void QFESPIMB040CameraView::setHistogramBinsAutoscale(bool autoscale) {
    if (autoscale) {
        spinHistogramBins->setEnabled(false);
        disconnect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(displayImageStatistics()));
    } else {
        spinHistogramBins->setEnabled(true);
        connect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(displayImageStatistics()));
    }
    displayImageStatistics();
}

void QFESPIMB040CameraView::redrawFrame() {
    //std::cout<<">>>> redrawFrame()\n";
    pltMain->set_doDrawing(false);
    plteMask->set_color(cmbMaskColor->currentColor());

    int xtickdist=image.width();
    int ytickdist=image.height();

    if (cmbRotation->currentIndex()%2==1) {
        xtickdist=pow(10,floor(log10(image.height())));
        ytickdist=pow(10,floor(log10(image.width())));
        plteFrame->set_palette(cmbColorscaleCurrentIndex());
        plteFrame->set_xmax(image.height());
        plteFrame->set_ymax(image.width());
        pltMain->setXRange(0, image.height());
        pltMain->setYRange(0, image.width());
        if (!chkGrid->isChecked()) {
            pltMain->set_xTickDistance(xtickdist);
            pltMain->set_yTickDistance(ytickdist);
        }
        plteMask->set_xmax(image.height());
        plteMask->set_ymax(image.width());
        pltMain->set_aspectRatio((double)image.height()/(double)image.width());
    } else {
        xtickdist=pow(10,floor(log10(image.width())));
        ytickdist=pow(10,floor(log10(image.height())));
        plteFrame->set_palette(cmbColorscaleCurrentIndex());
        plteFrame->set_xmax(image.width());
        plteFrame->set_ymax(image.height());
        pltMain->setXRange(0, image.width());
        pltMain->setYRange(0, image.height());
        if (!chkGrid->isChecked()) {
            pltMain->set_xTickDistance(xtickdist);
            pltMain->set_yTickDistance(ytickdist);
        }
        plteMask->set_xmax(image.width());
        plteMask->set_ymax(image.height());
        pltMain->set_aspectRatio((double)image.width()/(double)image.height());

    }

    plteFrame->set_palette(cmbColorscaleCurrentIndex());
    plteFrame->set_rotation(cmbRotation->currentIndex());
    plteMask->set_rotation(cmbRotation->currentIndex());

    plteMask->set_data(mask.data(), mask.width(), mask.height());

    plteFrame->set_colorMin(spinCountsLower->value());
    plteFrame->set_colorMax(spinCountsUpper->value());
    if (typeid(QFESPIMB040CameraView_internalImageType)==typeid(uint32_t)) {
        plteFrame->set_image(image.data(), JKQTFP_uint32, image.width(), image.height());
    } else if (typeid(QFESPIMB040CameraView_internalImageType)==typeid(int64_t)) {
        plteFrame->set_image(image.data(), JKQTFP_int64, image.width(), image.height());
    } else if (typeid(QFESPIMB040CameraView_internalImageType)==typeid(double)) {
        plteFrame->set_image(image.data(), JKQTFP_double, image.width(), image.height());
    } else if (typeid(QFESPIMB040CameraView_internalImageType)==typeid(float)) {
        plteFrame->set_image(image.data(), JKQTFP_float, image.width(), image.height());
    }

    pltMain->deletePlot(plteMainDistance);
    if (actMeasure->isChecked()) pltMain->addPlot(plteMainDistance);

    pltMain->deletePlot(plteMarginalPos);
    if (cmbMarginalPlots->currentIndex()==1) pltMain->addPlot(plteMarginalPos);

    pltMain->set_doDrawing(true);
    pltMain->update_data_immediate();

    if (cmbMarginalPlots->currentIndex()!=0) { // single line marginal plot
        pltMarginalBottom->set_doDrawing(false);
        pltMarginalLeft->set_doDrawing(false);
        pltMarginalBottom->setVisible(true);
        pltMarginalLeft->setVisible(true);
        if (cmbRotation->currentIndex()%2==1) {
            // DISPLAY MARGINALS
            pltMarginalBottom->setXRange(0, image.height());
            pltMarginalBottom->setYRange(qMax(pltDataMarginalBottomYMin, spinCountsLower->value()), qMin(pltDataMarginalBottomYMax, spinCountsUpper->value()));
            pltMarginalLeft->setXRange(qMax(pltDataMarginalLeftYMin, spinCountsLower->value()), qMin(pltDataMarginalLeftYMax, spinCountsUpper->value()));
            pltMarginalLeft->setYRange(0, image.width());

            pltMarginalBottom->set_xTickDistance(xtickdist);
            pltMarginalLeft->set_yTickDistance(ytickdist);
            double bcsize= fabs(pltMarginalBottom->get_yMax()-pltMarginalBottom->get_yMin());
            //if (bcsize<1e-15); bcsize=1;
            double lcsize= fabs(pltMarginalLeft->get_xMax()-pltMarginalLeft->get_xMin());
            //if (lcsize<1e-15); lcsize=1;
            pltMarginalBottom->set_yTickDistance(pow(10,floor(log10(bcsize))));
            pltMarginalLeft->set_xTickDistance(2.5*pow(10,floor(log10(lcsize))));
        } else {
            // DISPLAY MARGINALS

            pltMarginalBottom->setXRange(0, image.width());
            pltMarginalBottom->setYRange(qMax(pltDataMarginalBottomYMin, spinCountsLower->value()), qMin(pltDataMarginalBottomYMax, spinCountsUpper->value()));
            pltMarginalLeft->setXRange(qMax(pltDataMarginalLeftYMin, spinCountsLower->value()), qMin(pltDataMarginalLeftYMax, spinCountsUpper->value()));
            pltMarginalLeft->setYRange(0, image.height());

            pltMarginalBottom->set_xTickDistance(xtickdist);
            pltMarginalLeft->set_yTickDistance(ytickdist);
            double bcsize= fabs(pltMarginalBottom->get_yMax()-pltMarginalBottom->get_yMin());
            //if (bcsize<1e-15); bcsize=1;
            double lcsize= fabs(pltMarginalLeft->get_xMax()-pltMarginalLeft->get_xMin());
            //if (lcsize<1e-15); lcsize=1;
            pltMarginalBottom->set_yTickDistance(pow(10,floor(log10(bcsize))));
            pltMarginalLeft->set_xTickDistance(2.5*pow(10,floor(log10(lcsize))));

        }
        plteMarginalBottom->set_data(pltDataMarginalBottomX, pltDataMarginalBottomY, pltDataMarginalBottomN);
        plteMarginalLeft->set_data(pltDataMarginalLeftY, pltDataMarginalLeftX, pltDataMarginalLeftN);

        pltMarginalLeft->deletePlot(plteMarginalFitLeft);
        pltMarginalBottom->deletePlot(plteMarginalFitBottom);
        if (cmbMarginalFitFunctionCrrentIndex()>0) {
            plteMarginalFitBottom->set_data(pltDataMarginalFitBottomX, pltDataMarginalFitBottomY, pltDataMarginalBottomFitN);
            plteMarginalFitLeft->set_data(pltDataMarginalFitLeftY, pltDataMarginalFitLeftX, pltDataMarginalLeftFitN);
            pltMarginalLeft->addPlot(plteMarginalFitLeft);
            pltMarginalBottom->addPlot(plteMarginalFitBottom);
        }
        pltMarginalBottom->set_doDrawing(true);
        pltMarginalLeft->set_doDrawing(true);
        pltMarginalBottom->update_plot();
        pltMarginalLeft->update_plot();
    } else {
        pltMarginalBottom->setVisible(false);
        pltMarginalLeft->setVisible(false);
    }
    //labMarginalFitResults->setText(marginalResults);
    labMarginalFitResults->setData(marginalResultsSimple);


}

void QFESPIMB040CameraView::redrawFrameRecalc(bool forceHisto) {
    if (currentlyRedrawing) return;
    bool updt=updatesEnabled();
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    currentlyRedrawing=true;
    QTime tim;
    tim.start();
    prepareImage();
    //qDebug()<<"redrawFrameRecalc(forceHisto="<<forceHisto<<")   prepareImage = "<<tim.elapsed()<<" ms";
    tim.start();
    displayImageStatistics(chkImageStatisticsHistogram->isChecked(), forceHisto);
    //qDebug()<<"redrawFrameRecalc(forceHisto="<<forceHisto<<")   displayImageStatistics = "<<tim.elapsed()<<" ms";
    tim.start();
    redrawFrame();
    //qDebug()<<"redrawFrameRecalc(forceHisto="<<forceHisto<<")   redrawFrame = "<<tim.elapsed()<<" ms";
    currentlyRedrawing=false;
    if (widVisible) setUpdatesEnabled(updt);
}

void QFESPIMB040CameraView::prepareImage() {
    marginalResults="";
    marginalResultsSimple="";
    //image.assign(rawImage);
    transformImage(image, rawImage);
    if (image.sizeDiffersFrom(mask)) {
        mask.assign(image.width(), image.height(), false);
    }

    if (pltDataMarginalXPixel<0) pltDataMarginalXPixel=0;
    if (pltDataMarginalXPixel>=image.width()) pltDataMarginalXPixel=image.width()-1;
    if (pltDataMarginalYPixel<0) pltDataMarginalYPixel=0;
    if (pltDataMarginalYPixel>=image.height()) pltDataMarginalYPixel=image.height()-1;


    if (cmbMarginalPlots->currentIndex()!=0) {
        bool changed=false;
        double pixelW=pixelWidth;
        double pixelH=pixelHeight;
        if (cmbRotation->currentIndex()%2==0) {
            if (pltDataMarginalBottomN!=image.width()) {
                pltDataMarginalBottomN=image.width();
                changed=true;
            }
            if (pltDataMarginalLeftN!=image.height()) {
                pltDataMarginalLeftN=image.height();
                changed=true;
            }
        } else {
            pixelW=pixelHeight;
            pixelH=pixelWidth;
            if (pltDataMarginalBottomN!=image.height()) {
                pltDataMarginalBottomN=image.height();
                changed=true;
            }
            if (pltDataMarginalLeftN!=image.width()) {
                pltDataMarginalLeftN=image.width();
                changed=true;
            }
        }
        if (changed) {
            pltDataMarginalBottomX=(double*)qfRealloc((void*)pltDataMarginalBottomX, pltDataMarginalBottomN*sizeof(double));
            pltDataMarginalBottomY=(double*)qfRealloc((void*)pltDataMarginalBottomY, pltDataMarginalBottomN*sizeof(double));
            pltDataMarginalLeftX=(double*)qfRealloc((void*)pltDataMarginalLeftX, pltDataMarginalLeftN*sizeof(double));
            pltDataMarginalLeftY=(double*)qfRealloc((void*)pltDataMarginalLeftY, pltDataMarginalLeftN*sizeof(double));

            pltDataMarginalBottomFitN=qMax((uint32_t)MARGINAL_FIT_MINVALUES,pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR);
            pltDataMarginalLeftFitN=qMax((uint32_t)MARGINAL_FIT_MINVALUES,pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR);
            pltDataMarginalFitBottomY=(double*)qfRealloc((void*)pltDataMarginalFitBottomY, pltDataMarginalBottomFitN*sizeof(double));
            pltDataMarginalFitLeftY=(double*)qfRealloc((void*)pltDataMarginalFitLeftY, pltDataMarginalLeftFitN*sizeof(double));
            pltDataMarginalFitBottomX=(double*)qfRealloc((void*)pltDataMarginalFitBottomX, pltDataMarginalBottomFitN*sizeof(double));
            pltDataMarginalFitLeftX=(double*)qfRealloc((void*)pltDataMarginalFitLeftX, pltDataMarginalLeftFitN*sizeof(double));
        }
        if (cmbRotation->currentIndex()==0) {
            // +x-
            // y
            // |
            //qDebug()<<image.width()<<pltDataMarginalBottomN<<image.height()<<pltDataMarginalLeftN;
            if (cmbMarginalPlots->currentIndex()==1) {
                image.copyLine(pltDataMarginalYPixel, pltDataMarginalBottomY, pltDataMarginalBottomX);
                image.copyColumn(pltDataMarginalXPixel, pltDataMarginalLeftY, pltDataMarginalLeftX);
            } else if (cmbMarginalPlots->currentIndex()==2) {
                image.copyLineAverage(pltDataMarginalBottomY, pltDataMarginalBottomX);
                image.copyColumnAverage(pltDataMarginalLeftY, pltDataMarginalLeftX);
            }
            pltDataMarginalXPixelF=pltDataMarginalXPixel+0.5;
            pltDataMarginalYPixelF=pltDataMarginalYPixel+0.5;

        } else if (cmbRotation->currentIndex()==1) {
            // |
            // x
            // +y-
            //qDebug()<<image.height()<<pltDataMarginalBottomN<<image.width()<<pltDataMarginalLeftN;
            if (cmbMarginalPlots->currentIndex()==1) {
                image.copyColumn(pltDataMarginalXPixel, pltDataMarginalBottomY, pltDataMarginalBottomX);
                image.copyLineReverse(pltDataMarginalYPixel, pltDataMarginalLeftY, pltDataMarginalLeftX);
            } else if (cmbMarginalPlots->currentIndex()==2) {
                //qDebug()<<"-";
                image.copyColumnAverage(pltDataMarginalBottomY, pltDataMarginalBottomX);
                //qDebug()<<"--";
                image.copyLineAverageReverse(pltDataMarginalLeftY, pltDataMarginalLeftX);
                //qDebug()<<"---";
            }
            pltDataMarginalXPixelF=pltDataMarginalYPixel+0.5;
            pltDataMarginalYPixelF=image.width()-1-pltDataMarginalXPixel+0.5;
        } else if (cmbRotation->currentIndex()==2) {
            //   |
            //   y
            // -x+
            //qDebug()<<image.width()<<pltDataMarginalBottomN<<image.height()<<pltDataMarginalLeftN;
            if (cmbMarginalPlots->currentIndex()==1) {
                image.copyLineReverse(pltDataMarginalYPixel, pltDataMarginalBottomY, pltDataMarginalBottomX);
                image.copyColumnReverse(pltDataMarginalXPixel, pltDataMarginalLeftY, pltDataMarginalLeftX);
            } else if (cmbMarginalPlots->currentIndex()==2) {
                image.copyLineAverageReverse(pltDataMarginalBottomY, pltDataMarginalBottomX);
                image.copyColumnAverageReverse(pltDataMarginalLeftY, pltDataMarginalLeftX);
            }
            pltDataMarginalXPixelF=image.width()-1-pltDataMarginalXPixel+0.5;
            pltDataMarginalYPixelF=image.height()-1-pltDataMarginalYPixel+0.5;
        } else if (cmbRotation->currentIndex()==3) {
            // -y+
            //   x
            //   |
            //qDebug()<<image.height()<<pltDataMarginalBottomN<<image.width()<<pltDataMarginalLeftN;
            if (cmbMarginalPlots->currentIndex()==1) {
                image.copyColumnReverse(pltDataMarginalXPixel, pltDataMarginalBottomY, pltDataMarginalBottomX);
                image.copyLine(pltDataMarginalYPixel, pltDataMarginalLeftY, pltDataMarginalLeftX);
            } else if (cmbMarginalPlots->currentIndex()==2) {
                image.copyColumnAverageReverse(pltDataMarginalBottomY, pltDataMarginalBottomX);
                image.copyLineAverage(pltDataMarginalLeftY, pltDataMarginalLeftX);
            }
            pltDataMarginalXPixelF=image.height()-1-pltDataMarginalYPixel+0.5;
            pltDataMarginalYPixelF=pltDataMarginalXPixel+0.5;
        }
        statisticsMinMax(pltDataMarginalBottomY, pltDataMarginalBottomN, pltDataMarginalBottomYMin, pltDataMarginalBottomYMax);
        statisticsMinMax(pltDataMarginalLeftY, pltDataMarginalLeftN, pltDataMarginalLeftYMin, pltDataMarginalLeftYMax);
        if (cmbMarginalFitFunctionCrrentIndex()>0) {
            // CALCULATE MARGINAL FITS
            marginalResults="";
            if (cmbMarginalFitFunctionCrrentIndex()==1) {
                marginalResults=tr("<b>Marginal Fits:</b><br><center><table border=\"0\" width=\"90%\">");
                double var=0;
                double avg=statisticsAverageVariance(var, pltDataMarginalLeftY, pltDataMarginalLeftX, pltDataMarginalLeftN);
                for (uint32_t i=0; i<pltDataMarginalLeftFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalLeftN/(double)pltDataMarginalLeftFitN;
                    pltDataMarginalFitLeftX[i]=x;
                    pltDataMarginalFitLeftY[i]=pltDataMarginalLeftYMin+(pltDataMarginalLeftYMax-pltDataMarginalLeftYMin)*exp(-0.5*(x-avg)*(x-avg)/var);
                }
                MarginalLeftWidth=sqrt(var);
                MarginalLeftPosition=avg;

                marginalResults+=tr("<tr><td width=\"20%\"><b>left:&nbsp;</b></td><td width=\"20%\">average = </td><td width=\"20%\">%1 px</td><td width=\"20%\">standard deviation = </td><td width=\"20%\">%2 px</td></tr>").arg(roundWithError(avg, sqrt(var), 1)).arg(roundError(sqrt(var), 1));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(avg*pixelH, sqrt(var)*pixelH, 1)).arg(roundError(sqrt(var)*pixelH, 1));
                marginalResultsSimple+=tr(">b20|20|20|20|20\n");
                marginalResultsSimple+=tr(" | average | | stddev |\n");
                marginalResultsSimple+=tr("left: | %1 px | %3 %5m |%2 px | %4 %5m\n").arg(roundWithError(avg, sqrt(var), 0)).arg(roundError(sqrt(var), 1)).arg(roundWithError(avg*pixelH, sqrt(var)*pixelH, 1)).arg(roundError(sqrt(var)*pixelH, 1)).arg(QChar(0xB5));
                var=0;
                avg=statisticsAverageVariance(var, pltDataMarginalBottomY, pltDataMarginalBottomX, pltDataMarginalBottomN);
                for (uint32_t i=0; i<pltDataMarginalBottomFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalBottomN/(double)pltDataMarginalBottomFitN;
                    pltDataMarginalFitBottomX[i]=x;
                    pltDataMarginalFitBottomY[i]=pltDataMarginalBottomYMin+(pltDataMarginalBottomYMax-pltDataMarginalBottomYMin)*exp(-0.5*(x-avg)*(x-avg)/var);
                }
                MarginalTopWidth=sqrt(var);
                MarginalTopPosition=avg;

                marginalResults+=tr("<tr><td><b>bottom:&nbsp;</b></td><td>average = </td><td>%1 px</td><td>standard deviation = </td><td>%2 px</td></tr>").arg(roundWithError(avg, sqrt(var), 1)).arg(roundError(sqrt(var), 1));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(avg*pixelW, sqrt(var)*pixelW, 1)).arg(roundError(sqrt(var)*pixelW, 1));
                marginalResults+=tr("</table></center>");

                marginalResultsSimple+=tr("bottom: | %1 px | %3 %5m |%2 px | %4 %5m").arg(roundWithError(avg, sqrt(var), 1)).arg(roundError(sqrt(var), 1)).arg(roundWithError(avg*pixelW, sqrt(var)*pixelW, 1)).arg(roundError(sqrt(var)*pixelW,1)).arg(QChar(0xB5));
            } else if (cmbMarginalFitFunctionCrrentIndex()==2) {
                marginalResults=tr("<b>Marginal Fits:</b><br><center><table border=\"0\" width=\"90%\">");
                double pout[4];
                int n_par = 4; // number of parameters
                int m_dat = pltDataMarginalLeftN; // number of data pairs
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalLeftY, pltDataMarginalLeftX, pltDataMarginalLeftN);
                if (sqrt(fabs(pout[3]))>pltDataMarginalLeftN/4.0) pout[3]/=10;
                pout[0]=pltDataMarginalLeftYMin;
                pout[1]=pltDataMarginalLeftYMax-pltDataMarginalLeftYMin;
                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalLeftX, pltDataMarginalLeftY, fGauss, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalLeftFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalLeftN/(double)pltDataMarginalLeftFitN;
                    pltDataMarginalFitLeftX[i]=x;
                    pltDataMarginalFitLeftY[i]=fGauss(x, pout);
                }
                MarginalLeftWidth=sqrt(fabs(pout[3]))*pixelH;
                MarginalLeftPosition=pout[2]*pixelH;

                marginalResultsSimple+=tr(">b20|20|20|20|20\n");
                marginalResults+=tr("<tr><td width=\"20%\"><b>left:&nbsp;</b></td><td width=\"20%\">average = </td><td width=\"20%\">%1 px</td><td width=\"20%\">&nbsp;&nbsp;1/e<sup>2</sup>-width = </td><td width=\"20%\">%2 px</td></tr>").arg(roundWithError(pout[2], sqrt(fabs(pout[3])), 2)).arg(roundError(sqrt(fabs(pout[3])), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelH, sqrt(fabs(pout[3]))*pixelH, 2)).arg(roundError(sqrt(fabs(pout[3]))*pixelH, 2));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>gauss</td></tr>").arg(status.fnorm);

                marginalResultsSimple+=tr("left:|average = |%1 px|1/e%3-radius = |%2 px\n").arg(roundWithError(pout[2], sqrt(fabs(pout[3])), 2)).arg(roundError(sqrt(fabs(pout[3])), 2)).arg(QChar(0xB2));
                marginalResultsSimple+=tr("||%1 %3m||%2 %3m\n").arg(roundWithError(pout[2]*pixelH, sqrt(fabs(pout[3]))*pixelH, 2)).arg(roundError(sqrt(fabs(pout[3]))*pixelH, 2)).arg(QChar(0xB5));
                marginalResultsSimple+=tr("|offset = |%1|amplitude = |%2\n").arg(pout[0]).arg(pout[1]);
                marginalResultsSimple+=tr("|%2%3 = |%1|func:|gauss\n").arg(status.fnorm).arg(QChar(0x3C7)).arg(QChar(0xB2));



                m_dat=pltDataMarginalBottomN;
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalBottomY, pltDataMarginalBottomX, pltDataMarginalBottomN);
                pout[0]=pltDataMarginalBottomYMin;
                pout[1]=pltDataMarginalBottomYMax-pltDataMarginalBottomYMin;
                control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalBottomX, pltDataMarginalBottomY, fGauss, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalBottomFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalBottomN/(double)pltDataMarginalBottomFitN;
                    pltDataMarginalFitBottomX[i]=x;
                    pltDataMarginalFitBottomY[i]=fGauss(x, pout);
                }
                MarginalTopWidth=sqrt(fabs(pout[3]))*pixelW;
                MarginalTopPosition=pout[2]*pixelW;

                marginalResults+=tr("<tr><td><b>bottom:&nbsp;</b></td><td>average = </td><td>%1 px</td><td>&nbsp;&nbsp;1/e<sup>2</sup>-width = </td><td>%2 px</td></tr>").arg(roundWithError(pout[2], sqrt(fabs(pout[3])), 2)).arg(roundError(sqrt(fabs(pout[3])), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelW, sqrt(fabs(pout[3]))*pixelW, 2)).arg(roundError(sqrt(fabs(pout[3]))*pixelW, 1));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func</td><td>gauss</td></tr>").arg(status.fnorm);
                marginalResults+=tr("</table></center>");

                marginalResultsSimple+=tr("bottom:|average = |%1 px|1/e%3-radius = |%2 px\n").arg(roundWithError(pout[2], sqrt(fabs(pout[3])), 2)).arg(roundError(sqrt(fabs(pout[3])), 2)).arg(QChar(0xB2));
                marginalResultsSimple+=tr("||%1 %3m||%2 %3m\n").arg(roundWithError(pout[2]*pixelW, sqrt(fabs(pout[3]))*pixelW, 2)).arg(roundError(sqrt(fabs(pout[3]))*pixelW, 2)).arg(QChar(0xB5));
                marginalResultsSimple+=tr("|offset = |%1|amplitude = |%2\n").arg(pout[0]).arg(pout[1]);
                marginalResultsSimple+=tr("|%2%3 = |%1|func:|gauss").arg(status.fnorm).arg(QChar(0x3C7)).arg(QChar(0xB2));

            } else if (cmbMarginalFitFunctionCrrentIndex()==3) {
                marginalResults=tr("<b>Marginal Fits:</b><br><center><table border=\"0\" width=\"90%\">");
                double pout[4];
                int n_par = 4; // number of parameters
                int m_dat = pltDataMarginalLeftN; // number of data pairs
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalLeftY, pltDataMarginalLeftX, pltDataMarginalLeftN);
                if (fabs(pout[3])>pltDataMarginalLeftN/10.0) pout[3]/=10;
                pout[0]=pltDataMarginalLeftYMin;
                pout[1]=pltDataMarginalLeftYMax-pltDataMarginalLeftYMin;
                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalLeftX, pltDataMarginalLeftY, fSlit, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalLeftFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalLeftN/(double)pltDataMarginalLeftFitN;
                    pltDataMarginalFitLeftX[i]=x;
                    pltDataMarginalFitLeftY[i]=fSlit(x, pout);
                }
                MarginalLeftWidth=fabs(pout[3])*pixelH;
                MarginalLeftPosition=pout[2]*pixelH;
                marginalResultsSimple+=tr(">b20|20|20|20|20\n");
                marginalResults+=tr("<tr><td width=\"20%\"><b>left:&nbsp;</b></td><td width=\"20%\">average = </td><td width=\"20%\">%1 px</td><td width=\"20%\">&nbsp;&nbsp;x<sub>1. Zero</sub> = </td><td width=\"20%\">%2 px</td></tr>").arg(roundWithError(pout[2], fabs(pout[3]), 1)).arg(roundError(fabs(pout[3]), 1));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelH, fabs(pout[3])*pixelH, 1)).arg(roundError(fabs(pout[3])*pixelH, 1));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>slit</td></tr>").arg(status.fnorm);

                marginalResultsSimple+=tr("left:|average = |%1 px|1. zero = |%2 px\n").arg(roundWithError(pout[2], (fabs(pout[3])), 1)).arg(roundError((fabs(pout[3])), 1));
                marginalResultsSimple+=tr("||%1 %3m||%2 %3m\n").arg(roundWithError(pout[2]*pixelH, (fabs(pout[3]))*pixelH, 1)).arg(roundError((fabs(pout[3]))*pixelH, 1)).arg(QChar(0xB5));
                marginalResultsSimple+=tr("|offset = |%1|amplitude = |%2\n").arg(pout[0]).arg(pout[1]);
                marginalResultsSimple+=tr("|%2%3 = |%1|func:|slit\n").arg(status.fnorm).arg(QChar(0x3C7)).arg(QChar(0xB2));


                m_dat=pltDataMarginalBottomN;
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalBottomY, pltDataMarginalBottomX, pltDataMarginalBottomN);
                if (fabs(pout[3])>pltDataMarginalLeftN/10.0) pout[3]/=10;
                pout[0]=pltDataMarginalBottomYMin;
                pout[1]=pltDataMarginalBottomYMax-pltDataMarginalBottomYMin;
                control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalBottomX, pltDataMarginalBottomY, fSlit, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalBottomFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalBottomN/(double)pltDataMarginalBottomFitN;
                    pltDataMarginalFitBottomX[i]=x;
                    pltDataMarginalFitBottomY[i]=fSlit(x, pout);
                }
                MarginalTopWidth=fabs(pout[3])*pixelW;
                MarginalTopPosition=pout[2]*pixelW;
                marginalResults+=tr("<tr><td><b>bottom:&nbsp;</b></td><td>average = </td><td>%1 px</td><td>&nbsp;&nbsp;x<sub>1. Zero</sub> = </td><td>%2 px</td></tr>").arg(roundWithError(pout[2], fabs(pout[3]), 1)).arg(roundError(fabs(pout[3]), 1));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelW, fabs(pout[3])*pixelW, 1)).arg(roundError(fabs(pout[3])*pixelW, 1));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>slit</td></tr>").arg(status.fnorm);
                marginalResults+=tr("</table></center>");

                marginalResultsSimple+=tr("bottom:|average = |%1 px|1. zero = |%2 px\n").arg(roundWithError(pout[2], (fabs(pout[3])), 1)).arg(roundError((fabs(pout[3])), 1));
                marginalResultsSimple+=tr("||%1 %3m||%2 %3m\n").arg(roundWithError(pout[2]*pixelW, (fabs(pout[3]))*pixelW, 1)).arg(roundError((fabs(pout[3]))*pixelW, 1)).arg(QChar(0xB5));
                marginalResultsSimple+=tr("|offset = |%1|amplitude = |%2\n").arg(pout[0]).arg(pout[1]);
                marginalResultsSimple+=tr("|%2%3 = |%1|func:|slit").arg(status.fnorm).arg(QChar(0x3C7)).arg(QChar(0xB2));
            } else if (cmbMarginalFitFunctionCrrentIndex()==4) {
                marginalResults=tr("<b>Marginal Fits:</b><br><center><table border=\"0\" width=\"90%\">");
                double pout[4];
                int n_par = 4; // number of parameters
                int m_dat = pltDataMarginalLeftN; // number of data pairs
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalLeftY, pltDataMarginalLeftX, pltDataMarginalLeftN);
                if (fabs(pout[3])>pltDataMarginalLeftN/10.0) pout[3]/=10;
                pout[0]=pltDataMarginalLeftYMin;
                pout[1]=pltDataMarginalLeftYMax-pltDataMarginalLeftYMin;
                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalLeftX, pltDataMarginalLeftY, fSigmoid, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalLeftFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalLeftN/(double)pltDataMarginalLeftFitN;
                    pltDataMarginalFitLeftX[i]=x;
                    pltDataMarginalFitLeftY[i]=fSigmoid(x, pout);
                }
                MarginalLeftWidth=fabs(pout[3])*pixelH;
                MarginalLeftPosition=pout[2]*pixelH;
                marginalResultsSimple+=tr(">b20|20|20|20|20\n");
                marginalResults+=tr("<tr><td width=\"20%\"><b>left:&nbsp;</b></td><td width=\"20%\">center = </td><td width=\"20%\">%1 px</td><td width=\"20%\">&nbsp;&nbsp;width = </td><td width=\"20%\">%2 px</td></tr>").arg(roundError(pout[2], 2)).arg(roundError(pout[3], 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundError(pout[2]*pixelH, 2)).arg(roundError(pout[3]*pixelH, 2));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>sigmoidal</td></tr>").arg(status.fnorm);

                marginalResultsSimple+=tr("left:|center = |%1 px|width = |%2 px\n").arg(roundWithError(pout[2], 2)).arg(roundError(pout[3], 2));
                marginalResultsSimple+=tr("||%1 %3m||%2 %3m\n").arg(roundWithError(pout[2]*pixelH, 2)).arg(roundError(pout[3]*pixelH, 2)).arg(QChar(0xB5));
                marginalResultsSimple+=tr("|offset = |%1|amplitude = |%2\n").arg(pout[0]).arg(pout[1]);
                marginalResultsSimple+=tr("|%2%3 = |%1|func:|sigmoidal\n").arg(status.fnorm).arg(QChar(0x3C7)).arg(QChar(0xB2));


                m_dat=pltDataMarginalBottomN;
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalBottomY, pltDataMarginalBottomX, pltDataMarginalBottomN);
                if (fabs(pout[3])>pltDataMarginalLeftN/10.0) pout[3]/=10;
                pout[0]=pltDataMarginalBottomYMin;
                pout[1]=pltDataMarginalBottomYMax-pltDataMarginalBottomYMin;
                control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalBottomX, pltDataMarginalBottomY, fSigmoid, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalBottomFitN; i++) {
                    double x=(double)i*(double)pltDataMarginalBottomN/(double)pltDataMarginalBottomFitN;
                    pltDataMarginalFitBottomX[i]=x;
                    pltDataMarginalFitBottomY[i]=fSigmoid(x, pout);
                }
                MarginalTopWidth=fabs(pout[3])*pixelW;
                MarginalTopPosition=pout[2]*pixelH;
                marginalResults+=tr("<tr><td><b>bottom:&nbsp;</b></td><td>average = </td><td>%1 px</td><td>&nbsp;&nbsp;x<sub>1. Zero</sub> = </td><td>%2 px</td></tr>").arg(roundError(pout[2], 2)).arg(roundError((pout[3]), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundError(pout[2]*pixelW, 2)).arg(roundError((pout[3])*pixelW, 2));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>sigmoidal</td></tr>").arg(status.fnorm);
                marginalResults+=tr("</table></center>");

                marginalResultsSimple+=tr("bottom:|average = |%1 px|1. zero = |%2 px\n").arg(roundError(pout[2],  2)).arg(roundError(((pout[3])), 2));
                marginalResultsSimple+=tr("||%1 %3m||%2 %3m\n").arg(roundError(pout[2]*pixelW, 2)).arg(roundError(((pout[3]))*pixelW, 2)).arg(QChar(0xB5));
                marginalResultsSimple+=tr("|offset = |%1|amplitude = |%2\n").arg(pout[0]).arg(pout[1]);
                marginalResultsSimple+=tr("|%2%3 = |%1|func:|sigmoidal").arg(status.fnorm).arg(QChar(0x3C7)).arg(QChar(0xB2));
            }
        }
    }
}

void QFESPIMB040CameraView::displayImageStatistics(bool withHistogram, bool forceHistogram) {
    if (!histogram_x || !histogram_y) return;

    // CALCULATE STATISTICS OF TRANSFORMED IMAGE
    double histogram_min=0;
    double histogram_max=0;
    double histogram_fmax=0;
    histogram_n=spinHistogramBins->value();
    if (!imageStatisticsCalculated) {
        if(chkFindDefectivePixels->isChecked()) {
          if(image.findDefectivePixels(mask.data()))maskEmpty=false;
        }
        if (withHistogram) {
            if (!maskEmpty) image.calcImageStatistics(mask.data(), &imageBrokenPixels, &imageSum, &imageMean, &imageStddev, &imageImin, &imageImax, &histogram_x, &histogram_y, &histogram_n, &histogram_min, &histogram_max, &histogram_fmax, chkHistogramBinsAuto->isChecked());
            else {
                imageBrokenPixels=0;
                image.calcImageStatistics(&imageSum, &imageMean, &imageStddev, &imageImin, &imageImax, &histogram_x, &histogram_y, &histogram_n, &histogram_min, &histogram_max, &histogram_fmax, chkHistogramBinsAuto->isChecked());
            }
        } else {
            if (!maskEmpty) image.calcImageStatistics(mask.data(), &imageBrokenPixels, &imageSum, &imageMean, &imageStddev, &imageImin, &imageImax);
            else {
                imageBrokenPixels=0;
                image.calcImageStatistics(&imageSum, &imageMean, &imageStddev, &imageImin, &imageImax);
            }
        }
    }
    imageStatisticsCalculated=true;
    QFESPIMB040CameraView_internalImageType cmin= spinCountsLower->value();
    QFESPIMB040CameraView_internalImageType cmax= spinCountsUpper->value();
    if (chkCountsRangeAutoLow->isChecked()) cmin=imageImin;
    if (chkCountsRangeAutoHigh->isChecked()) cmax=imageImax;
    double hrange=imageImax-imageImin;
    if (chkCountsRangeAutoLow->isChecked()) spinCountsLower->setValue(cmin);
    if (chkCountsRangeAutoHigh->isChecked()) spinCountsUpper->setValue(cmax);




    // update graph pane (if activated)
    if (chkGraph->isChecked()) {
        int cnt=0;
        if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(imageMean);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(imageImax);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(imageStddev);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(correlationCoefficient);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(correlationCoefficientCenter);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(MarginalLeftWidth);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(MarginalTopWidth);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(MarginalLeftPosition);
        } else if (cmbGraphParameter->currentIndex()==cnt++) {
            plteGraphDataX.append((double)graphTime.elapsed()/1000.0);
            plteGraphDataY.append(MarginalTopPosition);
        }

        updateGraph();
    }


    if (withHistogram && ((histogramUpdateTime.elapsed()>HISTOGRAM_UPDATE_INTERVAL_MS) || forceHistogram )) {

        // DISABLE UPDATING OF GRAPHS (we only want to do this once per function call!!!)
        pltCountsHistogram->set_doDrawing(false);

        histogramUpdateTime.start();
        spinHistogramBins->setValue(histogram_n);
        plteHistogram->set_data(histogram_x, histogram_y, histogram_n);

        bool logarithmic=false;
        double ymin, ymax, xmin, xmax;
        static bool last_log=false;
        if (chkHistogramLog->isChecked()) {
            logarithmic=true;
            ymin=0.9;
            ymax=histogram_fmax;
            pltCountsHistogram->set_yZeroTick(1);
        } else {
            ymin=0;
            ymax=histogram_fmax;
            logarithmic=false;
            pltCountsHistogram->set_yZeroTick(0);
        }
        xmin=histogram_min-0.01*(double)hrange;
        xmax=histogram_max+0.01*(double)hrange;

        double dt=pow(10,floor(log10(fabs(xmax))));
        pltCountsHistogram->set_xTickDistance(dt);
        dt=pow(10,floor(log10(fabs(ymax))));
        pltCountsHistogram->set_yTickDistance(dt);

        //if (last_log!=chkHistogramLog->isChecked()) {
            pltCountsHistogram->setYRange(ymin, ymax, logarithmic);
            pltCountsHistogram->setXRange(xmin, xmax, false);
        //} else {
            if (xmin<pltCountsHistogram->get_xMin()) pltCountsHistogram->setXRange(xmin, pltCountsHistogram->get_xMax(), false);
            if (xmax>pltCountsHistogram->get_xMax()) pltCountsHistogram->setXRange(pltCountsHistogram->get_xMin(), xmax, false);
            if (ymin<pltCountsHistogram->get_yMin()) pltCountsHistogram->setYRange(ymin, pltCountsHistogram->get_yMax(), logarithmic);
            if (ymax>pltCountsHistogram->get_yMax()) pltCountsHistogram->setYRange(ymin, pltCountsHistogram->get_yMax(), logarithmic);
        //}
        last_log=chkHistogramLog->isChecked();

        plteHistogramRange->set_xmin((double)cmin);
        plteHistogramRange->set_xmax((double)cmax);

        pltCountsHistogram->set_doDrawing(true);
        pltCountsHistogram->update_plot();
    }


    if ((labelUpdateTime.elapsed()>LABEL_UPDATE_INTERVAL_MS) || forceHistogram ) {
        QString correlation="";
        switch (cmbImageTransformModeCurrentIndex()) {
            case 1:
            case 2:
            case 3:
            case 4:
                correlation=tr("<tr><td>correlation =</td><td>%1</td><td></td></tr><tr><td>center corr. =</td><td>%2</td><td></td></tr>").arg(correlationCoefficient).arg(correlationCoefficientCenter);
                break;
            default: break;
        }
        labImageStatisticsText=tr("<b>Image Statistics:</b><br><center><table border=\"0\" width=\"90%\"><tr><td width=\"20%\">size = </td><td width=\"40%\">%1 &times; %2</td><td width=\"40%\">= %14 &times; %15 &mu;m<sup>2</sup></td></tr><tr><td>broken pixels = </td><td>%3</td><td></td></tr>%16<tr><td>&nbsp;</td><td></td><td></td></tr><tr><td></td><td><b># photons</b></td><td><b>count rate [kHz]</b></td></tr> <tr><td>sum = </td><td>%4</td><td>%5</td></tr> <tr><td>average = </td><td>%6 &plusmn; %7</td><td>%8 &plusmn; %9</td></tr> <tr><td>min ... max = </td><td>%10 ... %11</td><td>%12 ... %13</td></tr> </table></center>")
                        .arg(image.width()).arg(image.height()).arg(imageBrokenPixels).arg(floattohtmlstr(imageSum).c_str())
                        .arg(floattohtmlstr(imageSum/imageExposureTime/1000.0, 3).c_str())
                        .arg(roundWithError(imageMean, imageStddev, 1)).arg(roundError(imageStddev, 1))
                        .arg(roundWithError(imageMean/imageExposureTime/1000.0, imageStddev/imageExposureTime/1000.0, 1)).arg(roundError(imageStddev/imageExposureTime/1000.0, 1))
                        .arg(imageImin).arg(imageImax).arg(imageImin/imageExposureTime/1000.0).arg(imageImax/imageExposureTime/1000.0).arg((double)image.width()*pixelWidth).arg((double)image.height()*pixelHeight).arg(correlation);
        //labImageStatistics->setText(labImageStatisticsText);
        correlation="";
        switch (cmbImageTransformModeCurrentIndex()) {
            case 1:
            case 2:
            case 3:
            case 4:
                correlation=tr("correlation = |%1\ncenter corr. = |%2\n").arg(correlationCoefficient).arg(correlationCoefficientCenter);
                break;
            default:
                break;
        }
        QString s=tr(">b20|40|40\n"
                     "Image Statistics:\n"
                     "size = |%1 %18 %2|= %14 %18 %15 %16m%17\n"
                     "broken pixels = |%3\n%20"
                     "||\n"
                     "|# photons|count rate [kHz]\n"
                     "sum = |%4|%5\n"
                     "average = |%6 %19 %7|%8 %19 %9\n"
                     "min ... max = |%10 ... %11|%12 ... %13")
                  .arg(image.width()).arg(image.height()).arg(imageBrokenPixels).arg(imageSum)
                  .arg(roundError(imageSum/imageExposureTime/1000.0, 3))
                  .arg(roundWithError(imageMean, imageStddev, 1)).arg(roundError(imageStddev, 1))
                  .arg(roundWithError(imageMean/imageExposureTime/1000.0, imageStddev/imageExposureTime/1000.0, 1)).arg(roundError(imageStddev/imageExposureTime/1000.0, 1))
                  .arg(imageImin).arg(imageImax).arg(imageImin/imageExposureTime/1000.0).arg(imageImax/imageExposureTime/1000.0).arg((double)image.width()*pixelWidth).arg((double)image.height()*pixelHeight)
                  .arg(QChar(0xB5)).arg(QChar(0xB2)).arg('x').arg(QChar(0xB1)).arg(correlation);
        labImageStatistics->setData(s);
        labelUpdateTime.start();
    }
}

void QFESPIMB040CameraView::clearMask() {
    if (m_stopresume) m_stopresume->stop();
    if (QMessageBox::question(this, tr("QuickFit SPIM Control"), tr("Do your really want to clear the mask?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes )==QMessageBox::No ) return;
    mask.setAll(false);
    maskEmpty=true;
    redrawFrameRecalc();
    if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::saveMask() {
    if (m_stopresume) m_stopresume->stop();
    QString fileName = qfGetSaveFileName(this, tr("Save Mask Data"),
                            lastMaskpath,
                            tr("Image Mask (*.msk)"));
    if (fileName.isEmpty()) return;
    lastMaskpath=QFileInfo(fileName).absolutePath();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!mask.save_nonzero_list(fileName.toStdString())) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("QuickFit SPIM Control"),
                          tr("Cannot write file %1.")
                          .arg(fileName));
        return;
    }
    QApplication::restoreOverrideCursor();
    if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::loadMask(const QString& fileName) {
    if (fileName.isEmpty()) return;
    lastMaskpath=QFileInfo(fileName).absolutePath();
    if (QMessageBox::question(this, tr("QuickFit SPIM Control"), tr("Do your really want to replace the mask with the file contents?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes )==QMessageBox::No ) return;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!mask.load_nonzero_list(fileName.toStdString())) {
        QMessageBox::warning(this, tr("QuickFit SPIM Control"),
                          tr("Cannot open file %1")
                          .arg(fileName));
    } else {
        recentMaskFiles->addRecentFile(fileName);
    }
    maskEmpty=false;
    redrawFrameRecalc();
    QApplication::restoreOverrideCursor();
    if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::loadMask() {
    if (m_stopresume) m_stopresume->stop();
    QString fileName = qfGetOpenFileName(this, tr("Load Mask Data"),
                            lastMaskpath,
                            tr("Image Mask (*.msk)"));
    loadMask(fileName);
}


void QFESPIMB040CameraView::histogramMask() {
    if (m_stopresume) m_stopresume->stop();
    if (QMessageBox::question(this, tr("QuickFit SPIM Control"), tr("Do your really want to replace the mask with a mask based on the current frame?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes )==QMessageBox::No ) return;

    QFESPIMB00HistogramMaskDialog* dlg=new QFESPIMB00HistogramMaskDialog(this);
    dlg->setMode(lastMaskHistogramMode);
    dlg->setPixels(lastMaskHistogramPixels, image.width()*image.height());
    if (dlg->exec()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        lastMaskHistogramMode=dlg->mode();
        lastMaskHistogramPixels=dlg->pixels();

        QFESPIMB040CameraView_internalImageType* d=image.dataCopy();
        uint32_t N=image.width()*image.height();
        statisticsSort(d, N);

        if (dlg->mode()==1) {
            mask.setAll(false);
        }
        uint32_t maxVal=d[N-1];
        if (N-dlg->pixels()>=0) maxVal=d[N-dlg->pixels()];
        for (long i=0; i<(long)N; i++) {
            if (image(i)>=maxVal) {
                mask(i)=true;
            }
        }

        qfFree(d);

        maskEmpty=false;
        redrawFrameRecalc();
        QApplication::restoreOverrideCursor();
    }
    delete dlg;
    if (m_stopresume) m_stopresume->resume();
}



void QFESPIMB040CameraView::saveRaw() {
    if (m_stopresume) m_stopresume->stop();
    saveJKImage(rawImage, tr("Save Raw Image ..."));
    if (m_stopresume) m_stopresume->resume();
}


void QFESPIMB040CameraView::saveMulti() {
    if (m_stopresume) m_stopresume->stop();
    //saveJKImage(rawImage, tr("Save Raw Image ..."));



    QString fileName = qfGetSaveFileName(this, tr("Save current image ..."),
                            lastImagepath,
                            "TIFF (*.tif)");

    if (fileName.isEmpty()) {
        if (m_stopresume) m_stopresume->resume();
        return;
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QuickFit SPIM Control: Save current image ..."),
                          tr("Cannot write file '%1':\nreason: %2.")
                          .arg(fileName)
                          .arg(file.errorString()));
        if (m_stopresume) m_stopresume->resume();
        return;
    }
    file.close();
    file.remove();
    lastImagepath=QFileInfo(fileName).absolutePath();
    QApplication::setOverrideCursor(Qt::WaitCursor);


    QFileInfo fi(fileName);
    QString fn=fi.absolutePath()+"/"+fi.completeBaseName();

    rawImage.save_tiffuint16(QString(fn+".tif").toStdString());
    rawImage.save_tiffuint32(QString(fn+"_uint32.tif").toStdString());
    rawImage.save_tifffloat(QString(fn+"_float.tif").toStdString());
    image.save_tiffuint16(QString(fn+"_transformed_uint16.tif").toStdString());
    image.save_tiffuint32(QString(fn+"_transformed.tif").toStdString());
    image.save_tifffloat(QString(fn+"_transformed_float.tif").toStdString());

    QImage imgo;
    JKQTFPimagePlot_array2image<QFESPIMB040CameraView_internalImageType>(image.data(), image.width(), image.height(), imgo, (JKQTFPColorPalette)cmbColorscaleCurrentIndex(), spinCountsLower->value(), spinCountsUpper->value());
    imgo.save(QString(fn+"_transformed.png"), "PNG");

    QImage imgo1;
    JKQTFPimagePlot_array2image<uint32_t>(rawImage.data(), rawImage.width(), rawImage.height(), imgo1, (JKQTFPColorPalette)cmbColorscaleCurrentIndex(), 0,0);
    imgo1.save(QString(fn+".png"), "PNG");

    QSettings setting(QString(fn+".configuration.ini"), QSettings::IniFormat);
    storeCameraConfig(setting);


    if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::saveMultiSeries()
{
    if (m_stopresume) m_stopresume->stop();
    QString fileName = qfGetSaveFileName(this, tr("Save current image ..."),
                            lastImagepath,
                            "TIFF (*.tif)");

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QuickFit SPIM Control: Save current image ..."),
                          tr("Cannot write file '%1':\nreason: %2.")
                          .arg(fileName)
                          .arg(file.errorString()));
        return;
    }
    file.close();
    file.remove();
    lastImagepath=QFileInfo(fileName).absolutePath();
    QApplication::setOverrideCursor(Qt::WaitCursor);


    if (!fileName.isEmpty()) saveMultiSeries(spinSaveSeriesFrames->value(), fileName);
    if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::saveMultiSeries(int frames, const QString fileName)
{
    //if (m_stopresume) m_stopresume->stop();
    //saveJKImage(rawImage, tr("Save Raw Image ..."));

    QFESPIMB040AcquisitionTools* acqTools=opticsSetup->getAcquisitionTools();

    acqTools->savePreviewMovie(cameraID, frames, fileName);



    //if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::storeCameraConfig(QSettings& setting) {
    QFExtensionCamera* cam=NULL;
    int camID=-1;
    if (opticsSetup) cam=opticsSetup->cameraComboBox(cameraID)->currentExtensionCamera();
    if (opticsSetup) camID=opticsSetup->cameraComboBox(cameraID)->currentCameraID();
    setting.setValue("acquisition/type", "preview");
    setting.setValue("acquisition/exposure", imageExposureTime);
    setting.setValue("acquisition/start_time", QDateTime::currentDateTime().toString(Qt::ISODate));
    setting.setValue("acquisition/image_width", rawImage.width());
    setting.setValue("acquisition/image_height", rawImage.height());
    setting.setValue("acquisition/pixel_width", pixelWidth);
    setting.setValue("acquisition/pixel_height", pixelHeight);
    setting.setValue("acquisition/transformation", cmbImageTransformMode->currentText());
    setting.setValue("acquisition/rotation", cmbRotation->currentIndex()*90);
    setting.setValue("acquisition/color_palette", cmbColorscale->currentText());
    if (opticsSetup) setting.setValue("acquisition/magnification", opticsSetup->getCameraMagnification(cameraID));
    if (cam) setting.setValue("acquisition/camera_model", cam->getCameraSensorName(camID));
    if (cam) setting.setValue("acquisition/sensor_model", cam->getCameraName(camID));

    setting.setValue("evaluation/imageBrokenPixels", imageBrokenPixels);
    setting.setValue("evaluation/imageSum", imageSum);
    setting.setValue("evaluation/imageMean", imageMean);
    setting.setValue("evaluation/imageStddev", imageStddev);
    setting.setValue("evaluation/imageImin", qlonglong(imageImin));
    setting.setValue("evaluation/imageImax", qlonglong(imageImax));
    setting.setValue("evaluation/correlationCoefficient", correlationCoefficient);
    setting.setValue("evaluation/centerCorrelationCoefficient", correlationCoefficientCenter);

    QMap<QString, QVariant> acqD;
    QString acquisitionDescriptionPrefix="setup/";
    if (opticsSetup) {
        acqD=opticsSetup->getSetup(cameraID);
    }
    if (acqD.size()>0) {
        QMapIterator<QString, QVariant> it(acqD);
        while (it.hasNext()) {
            it.next();
            setting.setValue(acquisitionDescriptionPrefix+it.key(), it.value());
        }
    }

    {
        QMapIterator<QString, QVariant> it(imageParameters);
        acquisitionDescriptionPrefix="acquisition/";
        while (it.hasNext()) {
            it.next();
            setting.setValue(acquisitionDescriptionPrefix+it.key(), it.value());
        }
    }
}

int QFESPIMB040CameraView::cmbImageTransformModeCurrentIndex() const
{
    if (actAlign1Mode->isChecked()) return cmbAlign1ImageTransformMode->currentIndex();
    if (actAlign2Mode->isChecked()) return cmbAlign2ImageTransformMode->currentIndex();
    return cmbImageTransformMode->currentIndex();
}

int QFESPIMB040CameraView::cmbColorscaleCurrentIndex() const
{
    if (actAlign1Mode->isChecked()) return cmbAlign1Colorscale->currentIndex();
    if (actAlign2Mode->isChecked()) return cmbAlign2Colorscale->currentIndex();
    return cmbColorscale->currentIndex();
}

int QFESPIMB040CameraView::cmbMarginalFitFunctionCrrentIndex() const
{
    if (actAlign1Mode->isChecked()) return cmbAlign1MarginalFitFunction->currentIndex();
    if (actAlign2Mode->isChecked()) return cmbAlign2MarginalFitFunction->currentIndex();
    return cmbMarginalFitFunction->currentIndex();
}

void QFESPIMB040CameraView::set_palette(int /*dummy*/)
{
    plteFrame->set_palette(cmbColorscaleCurrentIndex());
}

void QFESPIMB040CameraView::saveTransformedImage() {
    if (m_stopresume) m_stopresume->stop();
    saveJKImage(image, tr("Save Transformed Image ..."));

    if (m_stopresume) m_stopresume->resume();
}


void QFESPIMB040CameraView::displayImage(JKImage<uint32_t>& imageInput, double timeindex, double exposuretime, QMap<QString, QVariant> parameters) {
    rawImage.assign(imageInput);
    imageTimeindex=timeindex;
    imageExposureTime=exposuretime;
    imageStatisticsCalculated=false;
    imageParameters=parameters;
    redrawFrameRecalc(false);
}

void QFESPIMB040CameraView::displayImageComplete(JKImage<uint32_t>& imageInput, double timeindex, double exposuretime, QMap<QString, QVariant> parameters) {
    rawImage.assign(imageInput);
    imageTimeindex=timeindex;
    imageExposureTime=exposuretime;
    imageStatisticsCalculated=false;
    imageParameters=parameters;
    redrawFrameRecalc(true);
}

void QFESPIMB040CameraView::clearImage() {
    rawImage.assign(100, 100, 0);
    imageTimeindex=0;
    imageExposureTime=1;
    redrawFrameRecalc(true);
}

void QFESPIMB040CameraView::displayCameraConfig(QString camera, double framerate) {
    labVideoSettings->setText(tr("<div align=\"right\"><b>camera:</b> %1<br><b>framerate:</b> %2 fps&nbsp;&nbsp;<b>exposure:</b> %5ms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>size:</b> %3&times;%4").arg(camera).arg(roundError(framerate,2)).arg(rawImage.width()).arg(rawImage.height()).arg(imageExposureTime*1e3));
}


void QFESPIMB040CameraView::addUserAction(QAction* action) {
    toolbar->addAction(action);
}

void QFESPIMB040CameraView::deleteUserAction(QAction* action) {
    toolbar->removeAction(action);
}

void QFESPIMB040CameraView::histogramChecked(bool checked) {
}




void QFESPIMB040CameraView::createReportDoc(QTextDocument* document) {
    // make text object for QPicture available
    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

    // collect images from plots
    QPicture picMain; QSize sizeMain;
    QPainter* p=new QPainter(&picMain); pltMain->draw(p, QColor(Qt::white), &sizeMain); p->end(); delete p;
    QPicture picMarginLeft; QSize sizeMarginLeft;
    p=new QPainter(&picMarginLeft); pltMarginalLeft->draw(p, QColor(Qt::white), &sizeMarginLeft); p->end(); delete p;
    QPicture picMarginBottom; QSize sizeMarginBottom;
    p=new QPainter(&picMarginBottom); pltMarginalBottom->draw(p, QColor(Qt::white), &sizeMarginBottom); p->end(); delete p;
    QPicture picHist; QSize sizeHist;
    p=new QPainter(&picHist); pltCountsHistogram->draw(p, QColor(Qt::white), &sizeHist); p->end(); delete p;


    // we use this QTextCursor to write the document
    QTextCursor cursor(document);
    QFont f;
    f.setPointSize(7);;
    document->setDefaultFont(f);

    // here we define some generic formats
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(7);
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

    fHeading1.setFontPointSize(1.5*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);


    // insert heading
    cursor.insertText(QString("%1:").arg(windowTitle()), fHeading1);
    cursor.insertBlock();
//    cursor.movePosition(QTextCursor::End);

    cursor.insertHtml(labVideoSettings->text());
    double scale=1;
    double all_width=picMain.boundingRect().width()+picMarginLeft.boundingRect().width();
    double all_height=picMain.boundingRect().height()+picMarginBottom.boundingRect().height();
    double page_width=(double)document->textWidth()*0.95;
    double page_height=1.0*page_width;
    scale=page_width/all_width;
    if (scale*all_height>page_height) scale=page_height/all_height;
    int percentMain=round(100.0*(double)picMain.boundingRect().width()/all_width);
    if (percentMain>99) percentMain=99;
    if (percentMain<1) percentMain=1;

    cursor.insertBlock();
    QTextTableFormat tableFormat1;
    tableFormat1.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat1.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 100-percentMain);
    constraints << QTextLength(QTextLength::PercentageLength, percentMain);
    tableFormat1.setColumnWidthConstraints(constraints);
    QTextTable* table = cursor.insertTable(3, 2, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();


        double pixelW=pixelWidth;
        double pixelH=pixelHeight;
        if (cmbRotation->currentIndex()%2==1) {
            pixelW=pixelHeight;
            pixelH=pixelWidth;
        }

        if (actMeasure->isChecked()) {
            double dx=measureX[0]-measureX[1];
            double dy=measureY[0]-measureY[1];
            double d=sqrt(dx*dx+dy*dy);
            double du=sqrt(dx*dx*pixelW*pixelW+dy*dy*pixelH*pixelH);
            double angle=atan(dy/dx)/M_PI*180.0;
            tabCursor.insertHtml(tr("<i>l</i><sub>meas</sub> = %1px = %2&mu;m   &alpha;<sub>meas</sub> = %3").arg(d).arg(du).arg(angle));
        }


    }
    {
        QTextCursor tabCursor=table->cellAt(1, 1).firstCursorPosition();
        tabCursor.setBlockFormat(bfLeft);
        QPicture& pic=picMain;
        //double scale=document->textWidth()*0.78/pic.boundingRect().width();
        //if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, pic.boundingRect().topLeft(), QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.78);
    }
    {
        QTextCursor tabCursor=table->cellAt(1, 0).firstCursorPosition();
        tabCursor.setBlockFormat(bfRight);
        QPicture& pic=picMarginLeft;
        //double scale=document->textWidth()*0.18/pic.boundingRect().width();
        //if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, pic.boundingRect().topLeft(), QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.18);
    }
    {
        QTextCursor tabCursor=table->cellAt(2, 1).firstCursorPosition();
        tabCursor.setBlockFormat(bfLeft);
        QPicture& pic=picMarginBottom;
        //double scale=document->textWidth()*0.78/pic.boundingRect().width();
        //if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, pic.boundingRect().topLeft(), QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.78);
    }
    {
        QTextCursor tabCursor=table->cellAt(2, 0).firstCursorPosition();
        tabCursor.setBlockFormat(bfCenter);
        tabCursor.insertHtml(QString("<i>marginals:</i> %1<br>").arg(cmbMarginalPlots->currentText()));
        tabCursor.insertHtml(QString("<i>legend:</i> <font color=\"red\"> - data</font>, <font color=\"blue\"> - fit</font>"));
    }
    QApplication::processEvents();

    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.insertBlock();
    constraints.clear();
    constraints << QTextLength(QTextLength::PercentageLength, 35);
    constraints << QTextLength(QTextLength::PercentageLength, 65);
    tableFormat1.setColumnWidthConstraints(constraints);
    table = cursor.insertTable(1,2, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture& pic=picHist;
        double scale=document->textWidth()*0.33/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(QString("Histogram:\n"), fTextBold);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        //tabCursor.insertHtml(labImageStatistics->text());
        tabCursor.insertHtml(labImageStatisticsText);
        tabCursor.insertBlock();
        tabCursor.insertBlock();
        tabCursor.insertHtml(marginalResults);
    }
}

void QFESPIMB040CameraView::saveReport() {


    if (m_stopresume) m_stopresume->stop();
    QTextDocument* doc=new QTextDocument();
    createReportDoc(doc);
    qfSaveReport(doc, QString("QuickFit %1 CameraView Report").arg(qfInfoVersionFull()), QString("QFESPIMB040CameraView"), this);
    delete doc;

    if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::printReport() {
    if (m_stopresume) m_stopresume->stop();
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        if (m_stopresume) m_stopresume->resume();
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
    if (m_stopresume) m_stopresume->resume();
}

void QFESPIMB040CameraView::setTableFontsize(int size)
{
    QFont f=labImageStatistics->font();
    f.setPointSize( size);
    labImageStatistics->setFont(f);
    labMarginalFitResults->setFont(f);
}

void QFESPIMB040CameraView::saveData() {

    QStringList filters;
    filters<< tr("Comma-Separated value (*.dat *.csv)");
    filters<< tr("Semicolon-Separated value [German Excel] (*.txt)");
    filters<< tr("SYLK (*.slk)");

    QString selFilter=filters[0];

    if (m_stopresume) m_stopresume->stop();
    QString fileName = qfGetSaveFileName(this, tr("Save Data"),
                            lastImagepath,
                            filters.join(";;"), &selFilter);
    if (fileName.isEmpty()) return;
    lastImagepath=QFileInfo(fileName).absolutePath();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    datatable2 datat;
    uint32_t col=0;
    uint32_t rows=0;
    if (cmbMarginalPlots->currentIndex()>0) {
        col+=4;
        rows=qMax(rows, qMax(pltDataMarginalLeftN, pltDataMarginalBottomN));
        datat.resize(col, rows);
        datat.set_title(col-4, "marginal_left_x");
        datat.set_title(col-3, "marginal_left_data");
        for (long i=0; i<(long)pltDataMarginalLeftN; i++) {
            datat.set(col-4, i, pltDataMarginalLeftX[i]);
            datat.set(col-3, i, pltDataMarginalLeftY[i]);
        }
        datat.set_title(col-2, "marginal_bottom_x");
        datat.set_title(col-1, "marginal_bottom_data");
        for (long i=0; i<(long)pltDataMarginalBottomN; i++) {
            datat.set(col-2, i, pltDataMarginalBottomX[i]);
            datat.set(col-1, i, pltDataMarginalBottomY[i]);
        }
    }
    if (chkImageStatisticsHistogram->isChecked()) {
        col+=2;
        rows=qMax(rows, histogram_n);
        datat.resize(col, rows);
        datat.set_title(col-2, "histogram_x");
        datat.set_title(col-1, "histogram_data");
        for (long i=0; i<(long)histogram_n; i++) {
            datat.set(col-2, i, histogram_x[i]);
            datat.set(col-1, i, histogram_y[i]);
        }
    }

    if (selFilter==filters[0]) {
        datat.set_csv_options(false, ", ", '.');
        datat.save_csv(fileName.toStdString());
    } else if (selFilter==filters[1]) {
        datat.set_csv_options(false, "; ", ',');
        datat.save_csv(fileName.toStdString());
    } else if (selFilter==filters[2]) {
         datat.save_sylk(fileName.toStdString());
    }


    QApplication::restoreOverrideCursor();
    if (m_stopresume) m_stopresume->resume();



}

void QFESPIMB040CameraView::updateGrid() {
    pltMain->set_xTickDistance(spinGridWidth->value());
    pltMain->set_yTickDistance(spinGridWidth->value());
    plteGrid->set_width(spinGridWidth->value());
    plteGrid->set_color(cmbGridColor->currentColor());
    plteGrid->set_style(Qt::DashLine);
    plteGrid->set_lineWidth(1);
    plteGrid->setVisible(chkGrid->isChecked());
    //qDebug()<<pltMain->get_drawGrid();
}


void QFESPIMB040CameraView::transformImage(JKImage<QFESPIMB040CameraView_internalImageType>& out, const JKImage<uint32_t>& raw) {
    const int mode=cmbImageTransformModeCurrentIndex();
    if (mode==1 ||mode==3)  {
        if (mode==1) out.resize(raw.width(), (uint32_t)ceil((double)raw.height()/2.0));
        if (mode==3) out.resize(raw.width(), raw.height());

        QFESPIMB040CameraView_internalImageType* dout=out.data();
        const uint32_t* din=raw.data();
        register double amean=0, bmean=0, cnt=0;
        register double camean=0, cbmean=0, ccnt=0;
        /* calculate subtracted output image and grey value averages */
        for (register uint32_t y=0; y<raw.height(); y++) {
            for (register uint32_t x=0; x<raw.width(); x++) {
                register uint32_t i=y*raw.width()+x;
                register uint32_t i2=(y+raw.height()/2)*raw.width()+x;
                if (y<raw.height()/2) {
                    const QFESPIMB040CameraView_internalImageType a=din[i];
                    const QFESPIMB040CameraView_internalImageType b=din[i2];
                    if (mode==1) dout[y*out.width()+x]=a-b;
                    amean+=a;
                    bmean+=b;
                    cnt++;
                    if (x>raw.width()/4 && x<=3*raw.width()/4){
                        camean+=a;
                        cbmean+=b;
                        ccnt++;
                    }
                }
            }
        }
        amean=amean/(double)(cnt);
        bmean=bmean/(double)(cnt);
        camean=camean/(double)(ccnt);
        cbmean=cbmean/(double)(ccnt);

        if (mode==3) {
            double ascale=1;
            double bscale=1;
            if (amean>bmean) {
                 bscale=amean/bmean;
            } else if (amean<bmean) {
                ascale=bmean/amean;
            }
            for (register uint32_t y=0; y<out.height(); y++) {
                for (register uint32_t x=0; x<out.width(); x++) {
                    register uint32_t i=y*out.width()+x;
                    if (y<raw.height()/2) dout[i]=din[i]*ascale;
                    else dout[i]=din[i]*bscale;
                }
            }
        }
        /* calculate correlation coefficient */
        double corr=0, vara=0, varb=0;
        double ccorr=0, cvara=0, cvarb=0;
        for (register uint32_t y=0; y<raw.height(); y++) {
            for (register uint32_t x=0; x<raw.width(); x++) {
                register uint32_t i=y*out.width()+x;
                register uint32_t i2=(y+raw.height()/2)*raw.width()+x;
                if (y<raw.height()/2) {
                    const double a=din[i];
                    const double b=din[i2];
                    corr += (a-amean)*(b-bmean);
                    vara += (a-amean)*(a-amean);
                    varb += (b-bmean)*(b-bmean);
                    if (x>raw.width()/4 && x<=3*raw.width()/4){
                        ccorr += (a-camean)*(b-cbmean);
                        cvara += (a-camean)*(a-camean);
                        cvarb += (b-cbmean)*(b-cbmean);
                    }
                }
            }
        }
        correlationCoefficient=corr/sqrt(vara*varb);
        correlationCoefficientCenter=ccorr/sqrt(cvara*cvarb);
    } else if (mode==2 || mode==4)  {

        if (mode==2) out.resize((uint32_t)ceil((double)raw.width()/2.0), raw.height());
        if (mode==4) out.resize(raw.width(), raw.height());
        QFESPIMB040CameraView_internalImageType* dout=out.data();
        const uint32_t* din=raw.data();
        register double amean=0, bmean=0, cnt=0;
        register double camean=0, cbmean=0, ccnt=0;
        /* calculate subtracted output image and grey value averages */
        for (register uint32_t y=0; y<raw.height(); y++) {
            for (register uint32_t x=0; x<raw.width()/2; x++) {
                if (x<raw.width()/2) {
                    const QFESPIMB040CameraView_internalImageType a=din[y*raw.width()+x];
                    const QFESPIMB040CameraView_internalImageType b=din[y*raw.width()+x+raw.width()/2];
                    if (mode==2) dout[y*out.width()+x]=a-b;
                    amean+=a;
                    bmean+=b;
                    cnt++;
                    if (y>raw.height()/4 && y<=3*raw.height()/4){
                        camean+=a;
                        cbmean+=b;
                        ccnt++;
                    }
                }
            }
        }
        amean=amean/(double)(cnt);
        bmean=bmean/(double)(cnt);
        camean=camean/(double)(ccnt);
        cbmean=cbmean/(double)(ccnt);
        if (mode==4) {
            double ascale=1;
            double bscale=1;
            if (amean>bmean) {
                 bscale=amean/bmean;
            } else if (amean<bmean) {
                ascale=bmean/amean;
            }
            for (register uint32_t y=0; y<out.height(); y++) {
                for (register uint32_t x=0; x<out.width(); x++) {
                    register uint32_t i=y*out.width()+x;
                    if (x<raw.width()/2) dout[i]=din[i]*ascale;
                    else dout[i]=din[i]*bscale;
                }
            }
        }
        /* calculate correlation coefficient */
        double corr=0, vara=0, varb=0;
        double ccorr=0, cvara=0, cvarb=0;
        for (register uint32_t y=0; y<raw.height(); y++) {
            for (register uint32_t x=0; x<raw.width()/2; x++) {
                const double a=din[y*raw.width()+x];
                const double b=din[y*raw.width()+x+raw.width()/2];
                corr += (a-amean)*(b-bmean);
                vara += (a-amean)*(a-amean);
                varb += (b-bmean)*(b-bmean);
                if (x>out.width()/4 && x<=3*out.width()/4){
                    ccorr += (a-camean)*(b-cbmean);
                    cvara += (a-camean)*(a-camean);
                    cvarb += (b-cbmean)*(b-cbmean);
                }
            }
        }
        correlationCoefficient=corr/sqrt(vara*varb);
        correlationCoefficientCenter=ccorr/sqrt(cvara*cvarb);
    } else {
        out.assign(raw);
        correlationCoefficient=0;
        correlationCoefficientCenter=0;

    }


}

void QFESPIMB040CameraView::graphParameterChanged() {
    plteGraphDataX.clear();
    plteGraphDataY.clear();
    graphTime.start();
    updateGraph();
}

void QFESPIMB040CameraView::enableGraph(bool enabled) {
    if (enabled) {

    }
}

void QFESPIMB040CameraView::updateGraph() {
    pltGraph->set_doDrawing(false);

    while ((plteGraphDataX.size()>10) && (fabs(plteGraphDataX.last()-plteGraphDataX.first())>spinGraphWindow->value())) {
        plteGraphDataX.pop_front();
        plteGraphDataY.pop_front();
    }

    double min=0;
    double max=1;
    if (plteGraphDataX.size()>1) statisticsMinMax(plteGraphDataX.data(), plteGraphDataX.size(), min, max);
    pltGraph->setXRange(min, max);
    pltGraph->set_xTickDistance(1);
    if (min!=max) {
        if (min>max) qSwap(min, max);
        double p=floor(log10(max-min)-0.1);
        pltGraph->set_xTickDistance(pow(10.0, p));
        pltGraph->set_xZeroTick(floor(min/pow(10.0, p)-5)*pow(10.0, p));
        //qDebug()<<"X: min="<<min<<"  max="<<max<<"  delta="<<fabs(max-min)<<"  td="<<pow(10.0, p);
    }
    min=0;
    max=1;
    double mean=0, var=0;
    if (plteGraphDataY.size()>1) {
        statisticsMinMax(plteGraphDataY.data(), plteGraphDataY.size(), min, max);
        mean=statisticsAverageVariance(var, plteGraphDataY.data(), plteGraphDataY.size());
        plteGraphRange->setVisible(true);
        plteGraphRange->set_centerline(mean);
        plteGraphRange->set_ymin(mean-sqrt(var));
        plteGraphRange->set_ymax(mean+sqrt(var));
        labGraphMean->setTextFormat(Qt::RichText);
        labGraphMean->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        labGraphMean->setText(tr("graph (mean &plusmn; S.D.), [min...max]:&nbsp;&nbsp;&nbsp;( %1 &plusmn; %2 ),&nbsp;&nbsp;&nbsp;[ %3 ... %4 ]").arg(mean).arg(sqrt(var)).arg(min).arg(max));
    } else {
        plteGraphRange->setVisible(false);
        labGraphMean->setText("");
    }
    pltGraph->setYRange(min, max);
    pltGraph->set_yTickDistance(100);
    if (min!=max) {
        if (min>max) qSwap(min, max);
        double p=floor(log10(max-min)-0.1);
        //qDebug()<<min<<max<<max-min<<p<<pow(10.0, p);
        pltGraph->set_yTickDistance(pow(10.0, p));
        pltGraph->set_yZeroTick(floor(min/pow(10.0, p)-5)*pow(10.0, p));
        //qDebug()<<"Y: min="<<min<<"  max="<<max<<"  delta="<<fabs(max-min)<<"  td="<<pow(10.0, p);
    }
    pltGraph->set_xAxisLabel(tr("time [s]"));
    pltGraph->set_yAxisLabel(tr("intensity [AU]"));
    pltGraph->set_drawGrid(true);
    pltGraph->set_doDrawing(true);
    pltGraph->update_plot_immediate();
}

void QFESPIMB040CameraView::clearGraph() {
    plteGraphDataX.clear();
    plteGraphDataY.clear();
    graphTime.start();
    updateGraph();
}
