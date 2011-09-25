#include "qfespimb040cameraview.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "lmcurve.h"
#include "qmoretextobject.h"
#include "datatable2.h"

#include <QtGui>
#include <QtCore>

double fGauss( double t, const double *p )
{
    double offset=p[0];
    double A=p[1];
    double avg=p[2];
    double var=p[3];
    return offset+A*exp(-2.0*(t-avg)*(t-avg)/var);
}

double fSlit( double t, const double *p )
{
    double offset=p[0];
    double A=p[1];
    double avg=p[2];
    double d=p[3];
    double x=M_PI*(t-avg)/d;
    double s=sin(x)/x;
    return offset+A*s*s;
}

#define LABEL_UPDATE_INTERVAL_MS 50
#define HISTOGRAM_UPDATE_INTERVAL_MS 50
#define MARGINAL_FIT_SIZE_FACTOR 3

QFESPIMB040CameraView::QFESPIMB040CameraView(int cameraID, QFPluginServices* pluginServices, QFCameraConfigComboBoxStartResume* stopresume, QWidget* parent):
    QWidget(parent)
{
    setWindowTitle(tr("Preview Camera %1").arg(cameraID+1));
    setWindowIcon(QIcon(":/spimb040_logo.png"));

    chkCountsRangeAutoHigh=NULL;
    chkCountsRangeAutoLow=NULL;
    maskEmpty=true;
    pixelWidth=1;
    pixelHeight=1;
    measureX[0]=measureX[1]=0;
    measureY[0]=measureY[1]=0;
    measureFirst=true;
    m_stopresume=stopresume;


    // more variable initialisation
    imageStatisticsCalculated=false;
    currentlyRedrawing=false;
    m_pluginServices=pluginServices;

    //initialise image histogram data arrays
    histogram_n=255;
    histogram_x=(double*)malloc(histogram_n*sizeof(double));
    histogram_y=(double*)malloc(histogram_n*sizeof(double));
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
    pltDataMarginalBottomX=(double*)calloc(pltDataMarginalBottomN,sizeof(double));
    pltDataMarginalBottomY=(double*)calloc(pltDataMarginalBottomN,sizeof(double));
    pltDataMarginalLeftX=(double*)calloc(pltDataMarginalLeftN,sizeof(double));
    pltDataMarginalLeftY=(double*)calloc(pltDataMarginalLeftN,sizeof(double));
    pltDataMarginalFitBottomY=(double*)calloc(pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR,sizeof(double));
    pltDataMarginalFitLeftY=(double*)calloc(pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR,sizeof(double));
    pltDataMarginalFitBottomX=(double*)calloc(pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR,sizeof(double));
    pltDataMarginalFitLeftX=(double*)calloc(pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR,sizeof(double));
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

QFESPIMB040CameraView::~QFESPIMB040CameraView()
{
    free(histogram_x);
    free(histogram_y);
    free(pltDataMarginalLeftX);
    free(pltDataMarginalLeftY);
    free(pltDataMarginalBottomX);
    free(pltDataMarginalBottomY);
    free(pltDataMarginalFitBottomY);
    free(pltDataMarginalFitLeftY);
    free(pltDataMarginalFitBottomX);
    free(pltDataMarginalFitLeftX);
}

void QFESPIMB040CameraView::setPixelSize(double pixelWidth, double pixelHeight) {
    this->pixelHeight=pixelHeight;
    this->pixelWidth=pixelWidth;
}

void QFESPIMB040CameraView::createMainWidgets() {

    ///////////////////////////////////////////////////////////////
    // create main layout, toolbar and Splitters
    ///////////////////////////////////////////////////////////////
    QVBoxLayout* vbl=new QVBoxLayout(this);
    setLayout(vbl);
    vbl->setContentsMargins(0,0,0,0);
    toolbar=new QToolBar(tr("camera_toolbar"), this);
    vbl->addWidget(toolbar);
    splitVert=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitHor=new QVisibleHandleSplitter(Qt::Vertical, this);
    vbl->addWidget(splitHor);


    QWidget* wPltMain=new QWidget(this);
    vbl=new QVBoxLayout(wPltMain);
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
    QGridLayout* layPlt=new QGridLayout(wPltMain);
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
    plteMarginalPos=new JKQTFPVCrossPlot(pltMain, 1, &pltDataMarginalXPixelF, &pltDataMarginalYPixelF, QColor("red"));
    plteMarginalPos->set_crossWidth(15);
    plteMainDistance=new JKQTFPLinePlot(pltMain, 2, measureX, measureY, QColor("red"));
    pltMain->addPlot(plteMarginalPos);
    connect(pltMain, SIGNAL(mouseMoved(double, double)), this, SLOT(imageMouseMoved(double, double)));
    connect(pltMain, SIGNAL(clicked(double, double)), this, SLOT(imageMouseClicked(double, double)));
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
    tabResults=new QTabWidget(this);

    splitVert->addWidget(wPltMain);
    splitVert->addWidget(tabSettings);
    splitHor->addWidget(splitVert);
    splitHor->addWidget(tabResults);


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
    vbl=new QVBoxLayout(w);
    w->setLayout(vbl);

    QHBoxLayout* stathbl=new QHBoxLayout(w);
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

    QGridLayout* gl=new QGridLayout(w);
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


    hbl=new QHBoxLayout(w);
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

    labImageStatistics=new QLabel(w);
    vbl->addWidget(labImageStatistics);

    labMarginalFitResults=new QLabel(w);
    vbl->addWidget(labMarginalFitResults);

    tabSettings->addTab(w, tr("&Statistics"));
    setCountsAutoscale(false);








    /////////////////////////////////////////////////////////////////////
    // image settings pane
    /////////////////////////////////////////////////////////////////////
    w=new QWidget(this);
    QFormLayout* fl=new QFormLayout(w);
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
    connect(cmbColorscale, SIGNAL(currentIndexChanged(int)), plteFrame, SLOT(set_palette(int)));
    connect(cmbMaskColor, SIGNAL(activated(QColor)), plteMask, SLOT(set_color(QColor)));

    cmbRotation=new QComboBox(w);
    cmbRotation->addItem(tr("0 degrees"));
    cmbRotation->addItem(tr("90 degrees"));
    cmbRotation->addItem(tr("180 degrees"));
    cmbRotation->addItem(tr("270 degrees"));
    cmbRotation->setMaximumWidth(100);
    fl->addRow(tr("&rotation:"), cmbRotation);
    connect(cmbRotation, SIGNAL(currentIndexChanged(int)), this, SLOT(redrawFrameRecalc()));

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
    cmbMarginalFitFunction->setMaximumWidth(200);
    fl->addRow(tr("&marginal evluation:"), cmbMarginalFitFunction);
    connect(cmbMarginalFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(redrawFrameRecalc()));



    tabSettings->addTab(w, tr("Settings"));
}

void QFESPIMB040CameraView::createActions() {
    actSaveRaw = new QAction(QIcon(":/spimb040/saveraw.png"), tr("Save &raw image as ..."), this);
    actSaveRaw->setShortcut(QKeySequence::Save);
    connect(actSaveRaw, SIGNAL(triggered()), this, SLOT(saveRaw()));

    actMaskClear = new QAction(QIcon(":/spimb040/maskclear.png"), tr("&Clear mask (broken pixels)"), this);
    connect(actMaskClear, SIGNAL(triggered()), this, SLOT(clearMask()));

    actMaskSave = new QAction(QIcon(":/spimb040/masksave.png"), tr("&Save mask (broken pixels)"), this);
    connect(actMaskSave, SIGNAL(triggered()), this, SLOT(saveMask()));

    actMaskLoad = new QAction(QIcon(":/spimb040/maskload.png"), tr("&Load mask (broken pixels)"), this);
    connect(actMaskLoad, SIGNAL(triggered()), this, SLOT(loadMask()));

    actPrintReport = new QAction(QIcon(":/spimb040/report_print.png"), tr("&Print a report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    actSaveReport = new QAction(QIcon(":/spimb040/report_save.png"), tr("&Save a report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actSaveData = new QAction(QIcon(":/spimb040/preview_savedata.png"), tr("&Save marginal and histogram data"), this);
    connect(actSaveData, SIGNAL(triggered()), this, SLOT(saveData()));


    QActionGroup* actgMouse=new QActionGroup(this);
    actCursor=actgMouse->addAction(QIcon(":/spimb040/cursor.png"), tr("default mouse cursor"));
    actCursor->setCheckable(true);

    actMaskEdit = new QAction(QIcon(":/spimb040/maskedit.png"), tr("&Edit mask (broken pixels)"), actgMouse);
    actMaskEdit->setCheckable(true);

    actMeasure = new QAction(QIcon(":/spimb040/measure.png"), tr("&Measure distances"), actgMouse);
    actMeasure->setCheckable(true);

    actCursor->setChecked(true);

    toolbar->addAction(actSaveRaw);
    toolbar->addAction(actSaveData);
    toolbar->addAction(actSaveReport);
    toolbar->addAction(actPrintReport);
    toolbar->addSeparator();
    toolbar->addAction(actMaskLoad);
    toolbar->addAction(actMaskSave);
    toolbar->addAction(actMaskEdit);
    toolbar->addAction(actMaskClear);
    toolbar->addSeparator();
    toolbar->addAction(actCursor);
    toolbar->addAction(actMeasure);
    toolbar->addSeparator();

}


void QFESPIMB040CameraView::loadSettings(ProgramOptions* settings, QString prefix) {
    jkloadWidgetGeometry((*settings->getQSettings()), this, prefix+"geometry/");
    jkloadSplitter((*settings->getQSettings()), splitHor,  prefix+"split_hor/");
    jkloadSplitter((*settings->getQSettings()), splitVert, prefix+"split_vert/");

    spinCountsLower->setValue((settings->getQSettings())->value(prefix+"histogram.min", 0).toInt());
    spinCountsUpper->setValue((settings->getQSettings())->value(prefix+"histogram.max", 255).toInt());
    chkCountsRangeAutoLow->setChecked((settings->getQSettings())->value(prefix+"histogram.autolow", true).toBool());
    chkCountsRangeAutoHigh->setChecked((settings->getQSettings())->value(prefix+"histogram.autohigh", true).toBool());
    setCountsAutoscale();
    bool b=(settings->getQSettings())->value(prefix+"histogram.log", false).toBool();
    chkHistogramLog->setChecked(b);
    histogram_n=(settings->getQSettings())->value(prefix+"histogram.items", histogram_n).toUInt();
    spinHistogramBins->setValue(histogram_n);
    // reallocate histogram and initialize
    if (histogram_x) free(histogram_x);
    if (histogram_y) free(histogram_y);
    histogram_x=(double*)malloc(histogram_n*sizeof(double));
    histogram_y=(double*)malloc(histogram_n*sizeof(double));
    for (unsigned int i=0; i<histogram_n; i++) {
        histogram_x[i]=i;
        histogram_y[i]=0;
    }
    b=(settings->getQSettings())->value(prefix+"histogram.items_auto", true).toBool();
    chkHistogramBinsAuto->setChecked(b);
    setHistogramBinsAutoscale(b);

    cmbColorscale->setCurrentIndex((settings->getQSettings())->value(prefix+"imagesettings.palette", 6).toInt());
    cmbMaskColor->setCurrentIndex((settings->getQSettings())->value(prefix+"imagesettings.mask_color", 0).toInt());
    cmbRotation->setCurrentIndex((settings->getQSettings())->value(prefix+"imagesettings.rotation", 0).toInt());
    cmbMarginalPlots->setCurrentIndex((settings->getQSettings())->value(prefix+"imagesettings.marginal", 1).toInt());
    cmbMarginalFitFunction->setCurrentIndex((settings->getQSettings())->value(prefix+"imagesettings.marginal_fitfunction", 0).toInt());
    pltDataMarginalXPixel=(settings->getQSettings())->value(prefix+"imagesettings.marginal_xpixel", pltDataMarginalXPixel).toInt();
    pltDataMarginalYPixel=(settings->getQSettings())->value(prefix+"imagesettings.marginal_ypixel", pltDataMarginalYPixel).toInt();

    lastImagepath=(settings->getQSettings())->value(prefix+"last_imagepath", lastImagepath).toString();
    lastMaskpath=(settings->getQSettings())->value(prefix+"last_maskpath", lastMaskpath).toString();
    lastImagefilter=(settings->getQSettings())->value(prefix+"last_imagefilter", lastImagefilter).toString();

    chkImageStatisticsHistogram->setChecked((settings->getQSettings())->value(prefix+"display_imagestatistics", chkImageStatisticsHistogram->isChecked()).toBool());
}

void QFESPIMB040CameraView::storeSettings(ProgramOptions* settings, QString prefix) {
    jksaveWidgetGeometry((*settings->getQSettings()), this, prefix+"geometry/");
    jksaveSplitter((*settings->getQSettings()), splitHor,  prefix+"split_hor/");
    jksaveSplitter((*settings->getQSettings()), splitVert, prefix+"split_vert/");

    (settings->getQSettings())->setValue(prefix+"last_imagepath", lastImagepath);
    (settings->getQSettings())->setValue(prefix+"last_maskpath", lastMaskpath);
    (settings->getQSettings())->setValue(prefix+"last_imagefilter", lastImagefilter);

    (settings->getQSettings())->setValue(prefix+"histogram.min", spinCountsLower->value());
    (settings->getQSettings())->setValue(prefix+"histogram.max", spinCountsUpper->value());
    (settings->getQSettings())->setValue(prefix+"histogram.autolow", chkCountsRangeAutoLow->isChecked());
    (settings->getQSettings())->setValue(prefix+"histogram.autohigh", chkCountsRangeAutoHigh->isChecked());
    (settings->getQSettings())->setValue(prefix+"histogram.log", chkHistogramLog->isChecked());
    (settings->getQSettings())->setValue(prefix+"histogram.items", histogram_n);

    (settings->getQSettings())->setValue(prefix+"imagesettings.palette", cmbColorscale->currentIndex());
    (settings->getQSettings())->setValue(prefix+"imagesettings.mask_color", cmbMaskColor->currentIndex());
    (settings->getQSettings())->setValue(prefix+"imagesettings.rotation", cmbRotation->currentIndex());
    (settings->getQSettings())->setValue(prefix+"imagesettings.marginal", cmbMarginalPlots->currentIndex());
    (settings->getQSettings())->setValue(prefix+"imagesettings.marginal_fitfunction", cmbMarginalFitFunction->currentIndex());
    (settings->getQSettings())->setValue(prefix+"imagesettings.marginal_xpixel", pltDataMarginalXPixel);
    (settings->getQSettings())->setValue(prefix+"imagesettings.marginal_ypixel", pltDataMarginalYPixel);

    (settings->getQSettings())->setValue(prefix+"display_imagestatistics", chkImageStatisticsHistogram->isChecked());


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
            QString s=QString::number(image(xx,yy));
            if (mask(xx, yy)) {
                s=s+tr(" <font color=\"red\">[broken pixel]</font>");
            }
            double dx=measureX[0]-measureX[1];
            double dy=measureY[0]-measureY[1];
            double d=sqrt(dx*dx+dy*dy);
            double du=sqrt(dx*dx*pixelW*pixelW+dy*dy*pixelH*pixelH);
            labCurrentPos->setText(tr("<b></b>image(%1, %2) = image(%4&mu;m, %5&mu;m) = %3&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;distance = %6px = %7&mu;m").arg(xx).arg(yy).arg(s).arg(xx*pixelW).arg(yy*pixelH).arg(d).arg(du));
        } else {
            labCurrentPos->setText("");
        }
    } else {
        if ((xx>=0) && (xx<image.width()) && (yy>=0) && (yy<image.height())) {
            QString s=QString::number(image(xx,yy));
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
        if (measureFirst) {
            measureX[0]=(double)xx+0.5;
            measureY[0]=(double)yy+0.5;
        } else {
            measureX[1]=(double)xx+0.5;
            measureY[1]=(double)yy+0.5;
        }
        measureFirst = !measureFirst;
        redrawFrameRecalc();
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
        plteFrame->set_xmax(image.height());
        plteFrame->set_ymax(image.width());
        pltMain->setXRange(0, image.height());
        pltMain->setYRange(0, image.width());
        pltMain->set_xTickDistance(xtickdist);
        pltMain->set_yTickDistance(ytickdist);
        plteMask->set_xmax(image.height());
        plteMask->set_ymax(image.width());
        pltMain->set_aspectRatio((double)image.height()/(double)image.width());
    } else {
        xtickdist=pow(10,floor(log10(image.width())));
        ytickdist=pow(10,floor(log10(image.height())));
        plteFrame->set_xmax(image.width());
        plteFrame->set_ymax(image.height());
        pltMain->setXRange(0, image.width());
        pltMain->setYRange(0, image.height());
        pltMain->set_xTickDistance(xtickdist);
        pltMain->set_yTickDistance(ytickdist);
        plteMask->set_xmax(image.width());
        plteMask->set_ymax(image.height());
        pltMain->set_aspectRatio((double)image.width()/(double)image.height());

    }

    plteFrame->set_rotation(cmbRotation->currentIndex());
    plteMask->set_rotation(cmbRotation->currentIndex());

    plteMask->set_data(mask.data(), mask.width(), mask.height());

    plteFrame->set_colorMin(spinCountsLower->value());
    plteFrame->set_colorMax(spinCountsUpper->value());
    //plteFrame->set_image(image, JKQTFP_double, image.width(), image.height());
    #if (QFESPIMB040CameraView_internalImageType==uint32_t)
        plteFrame->set_image(image.data(), JKQTFP_uint32, image.width(), image.height());
    #elif (QFESPIMB040CameraView_internalImageType==double)
        plteFrame->set_image(image.data(), JKQTFP_double, image.width(), image.height());
    #elif (QFESPIMB040CameraView_internalImageType==float)
        plteFrame->set_image(image.data(), JKQTFP_float, image.width(), image.height());
    #endif

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
        if (cmbMarginalFitFunction->currentIndex()>0) {
            plteMarginalFitBottom->set_data(pltDataMarginalFitBottomX, pltDataMarginalFitBottomY, pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR);
            plteMarginalFitLeft->set_data(pltDataMarginalFitLeftY, pltDataMarginalFitLeftX, pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR);
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
    labMarginalFitResults->setText(marginalResults);


}

void QFESPIMB040CameraView::redrawFrameRecalc(bool forceHisto) {
    if (currentlyRedrawing) return;
    currentlyRedrawing=true;
    prepareImage();
    displayImageStatistics(chkImageStatisticsHistogram->isChecked(), forceHisto);
    redrawFrame();
    currentlyRedrawing=false;
}

void QFESPIMB040CameraView::prepareImage() {
    marginalResults="";
    image.assign(rawImage);
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
            pltDataMarginalBottomX=(double*)realloc((void*)pltDataMarginalBottomX, pltDataMarginalBottomN*sizeof(double));
            pltDataMarginalBottomY=(double*)realloc((void*)pltDataMarginalBottomY, pltDataMarginalBottomN*sizeof(double));
            pltDataMarginalLeftX=(double*)realloc((void*)pltDataMarginalLeftX, pltDataMarginalLeftN*sizeof(double));
            pltDataMarginalLeftY=(double*)realloc((void*)pltDataMarginalLeftY, pltDataMarginalLeftN*sizeof(double));
            pltDataMarginalFitBottomY=(double*)realloc((void*)pltDataMarginalFitBottomY, pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR*sizeof(double));
            pltDataMarginalFitLeftY=(double*)realloc((void*)pltDataMarginalFitLeftY, pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR*sizeof(double));
            pltDataMarginalFitBottomX=(double*)realloc((void*)pltDataMarginalFitBottomX, pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR*sizeof(double));
            pltDataMarginalFitLeftX=(double*)realloc((void*)pltDataMarginalFitLeftX, pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR*sizeof(double));
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
            pltDataMarginalYPixelF=pltDataMarginalXPixel;
        }
        statisticsMinMax(pltDataMarginalBottomY, pltDataMarginalBottomN, pltDataMarginalBottomYMin, pltDataMarginalBottomYMax);
        statisticsMinMax(pltDataMarginalLeftY, pltDataMarginalLeftN, pltDataMarginalLeftYMin, pltDataMarginalLeftYMax);
        if (cmbMarginalFitFunction->currentIndex()>0) {
            // CALCULATE MARGINAL FITS
            marginalResults="";
            if (cmbMarginalFitFunction->currentIndex()==1) {
                marginalResults=tr("<b>Marginal Fits:</b><br><center><table border=\"0\" width=\"90%\">");
                double var=0;
                double avg=statisticsAverageVariance(var, pltDataMarginalLeftY, pltDataMarginalLeftX, pltDataMarginalLeftN);
                for (uint32_t i=0; i<pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR; i++) {
                    double x=(double)i/(double)MARGINAL_FIT_SIZE_FACTOR;
                    pltDataMarginalFitLeftX[i]=x;
                    pltDataMarginalFitLeftY[i]=pltDataMarginalLeftYMin+(pltDataMarginalLeftYMax-pltDataMarginalLeftYMin)*exp(-0.5*(x-avg)*(x-avg)/var);
                }
                marginalResults+=tr("<tr><td width=\"20%\"><b>left:&nbsp;</b></td><td width=\"20%\">average = </td><td width=\"20%\">%1 px</td><td width=\"20%\">standard deviation = </td><td width=\"20%\">%2 px</td></tr>").arg(roundWithError(avg, sqrt(var), 2)).arg(roundError(sqrt(var), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(avg*pixelH, sqrt(var)*pixelH, 2)).arg(roundError(sqrt(var)*pixelH, 2));
                var=0;
                avg=statisticsAverageVariance(var, pltDataMarginalBottomY, pltDataMarginalBottomX, pltDataMarginalBottomN);
                for (uint32_t i=0; i<pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR; i++) {
                    double x=(double)i/(double)MARGINAL_FIT_SIZE_FACTOR;
                    pltDataMarginalFitBottomX[i]=x;
                    pltDataMarginalFitBottomY[i]=pltDataMarginalBottomYMin+(pltDataMarginalBottomYMax-pltDataMarginalBottomYMin)*exp(-0.5*(x-avg)*(x-avg)/var);
                }
                marginalResults+=tr("<tr><td><b>bottom:&nbsp;</b></td><td>average = </td><td>%1 px</td><td>standard deviation = </td><td>%2 px</td></tr>").arg(roundWithError(avg, sqrt(var), 2)).arg(roundError(sqrt(var), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(avg*pixelW, sqrt(var)*pixelW, 2)).arg(roundError(sqrt(var)*pixelW, 2));
                marginalResults+=tr("</table></center>");
            } else if (cmbMarginalFitFunction->currentIndex()==2) {
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
                for (uint32_t i=0; i<pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR; i++) {
                    double x=(double)i/(double)MARGINAL_FIT_SIZE_FACTOR;
                    pltDataMarginalFitLeftX[i]=x;
                    pltDataMarginalFitLeftY[i]=fGauss(x, pout);
                }
                marginalResults+=tr("<tr><td width=\"20%\"><b>left:&nbsp;</b></td><td width=\"20%\">average = </td><td width=\"20%\">%1 px</td><td width=\"20%\">&nbsp;&nbsp;1/e<sup>2</sup>-width = </td><td width=\"20%\">%2 px</td></tr>").arg(roundWithError(pout[2], sqrt(fabs(pout[3])), 2)).arg(roundError(sqrt(fabs(pout[3])), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelH, sqrt(fabs(pout[3]))*pixelH, 2)).arg(roundError(sqrt(fabs(pout[3]))*pixelH, 2));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>gauss</td></tr>").arg(status.fnorm);



                m_dat=pltDataMarginalBottomN;
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalBottomY, pltDataMarginalBottomX, pltDataMarginalBottomN);
                pout[0]=pltDataMarginalBottomYMin;
                pout[1]=pltDataMarginalBottomYMax-pltDataMarginalBottomYMin;
                control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalBottomX, pltDataMarginalBottomY, fGauss, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR; i++) {
                    double x=(double)i/(double)MARGINAL_FIT_SIZE_FACTOR;
                    pltDataMarginalFitBottomX[i]=x;
                    pltDataMarginalFitBottomY[i]=fGauss(x, pout);
                }
                marginalResults+=tr("<tr><td><b>bottom:&nbsp;</b></td><td>average = </td><td>%1 px</td><td>&nbsp;&nbsp;1/e<sup>2</sup>-width = </td><td>%2 px</td></tr>").arg(roundWithError(pout[2], sqrt(fabs(pout[3])), 2)).arg(roundError(sqrt(fabs(pout[3])), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelW, sqrt(fabs(pout[3]))*pixelW, 2)).arg(roundError(sqrt(fabs(pout[3]))*pixelW, 2));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func</td><td>gauss</td></tr>").arg(status.fnorm);


                marginalResults+=tr("</table></center>");
            } else if (cmbMarginalFitFunction->currentIndex()==3) {
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
                for (uint32_t i=0; i<pltDataMarginalLeftN*MARGINAL_FIT_SIZE_FACTOR; i++) {
                    double x=(double)i/(double)MARGINAL_FIT_SIZE_FACTOR;
                    pltDataMarginalFitLeftX[i]=x;
                    pltDataMarginalFitLeftY[i]=fSlit(x, pout);
                }
                marginalResults+=tr("<tr><td width=\"20%\"><b>left:&nbsp;</b></td><td width=\"20%\">average = </td><td width=\"20%\">%1 px</td><td width=\"20%\">&nbsp;&nbsp;x<sub>1. Zero</sub> = </td><td width=\"20%\">%2 px</td></tr>").arg(roundWithError(pout[2], fabs(pout[3]), 2)).arg(roundError(fabs(pout[3]), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelH, fabs(pout[3])*pixelH, 2)).arg(roundError(fabs(pout[3])*pixelH, 2));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>slit</td></tr>").arg(status.fnorm);



                m_dat=pltDataMarginalBottomN;
                pout[2]=statisticsAverageVariance(pout[3], pltDataMarginalBottomY, pltDataMarginalBottomX, pltDataMarginalBottomN);
                if (fabs(pout[3])>pltDataMarginalLeftN/10.0) pout[3]/=10;
                pout[0]=pltDataMarginalBottomYMin;
                pout[1]=pltDataMarginalBottomYMax-pltDataMarginalBottomYMin;
                control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, m_dat, pltDataMarginalBottomX, pltDataMarginalBottomY, fSlit, &control, &status );
                for (uint32_t i=0; i<pltDataMarginalBottomN*MARGINAL_FIT_SIZE_FACTOR; i++) {
                    double x=(double)i/(double)MARGINAL_FIT_SIZE_FACTOR;
                    pltDataMarginalFitBottomX[i]=x;
                    pltDataMarginalFitBottomY[i]=fSlit(x, pout);
                }
                marginalResults+=tr("<tr><td><b>bottom:&nbsp;</b></td><td>average = </td><td>%1 px</td><td>&nbsp;&nbsp;x<sub>1. Zero</sub> = </td><td>%2 px</td></tr>").arg(roundWithError(pout[2], fabs(pout[3]), 2)).arg(roundError(fabs(pout[3]), 2));
                marginalResults+=tr("<tr><td></td><td></td><td>%1 &mu;m</td><td></td><td>%2 &mu;m</td></tr>").arg(roundWithError(pout[2]*pixelW, fabs(pout[3])*pixelW, 2)).arg(roundError(fabs(pout[3])*pixelW, 2));
                marginalResults+=tr("<tr><td><b></b></td><td>offset = </td><td>%1</td><td>&nbsp;&nbsp;amplitude = </td><td>%2</td></tr>").arg(pout[0]).arg(pout[1]);
                marginalResults+=tr("<tr><td><b></b></td><td>&chi;<sup>2</sub> = </td><td>%1</td><td>&nbsp;&nbsp;func:</td><td>slit</td></tr>").arg(status.fnorm);


                marginalResults+=tr("</table></center>");
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
        QString s=tr("<b>Image Statistics:</b><br><center><table border=\"0\" width=\"90%\"><tr><td width=\"20%\">size = </td><td width=\"40%\">%1 &times; %2</td><td width=\"40%\">= %14 &times; %15 &mu;m<sup>2</sup></td></tr><tr><td>broken pixels = </td><td>%3</td><td></td></tr><tr><td>&nbsp;</td><td></td><td></td></tr><tr><td></td><td><b># photons</b></td><td><b>count rate [kHz]</b></td></tr> <tr><td>sum = </td><td>%4</td><td>%5</td></tr> <tr><td>average = </td><td>%6 &plusmn; %7</td><td>%8 &plusmn; %9</td></tr> <tr><td>min ... max = </td><td>%10 ... %11</td><td>%12 ... %13</td></tr> </table></center>")
                        .arg(image.width()).arg(image.height()).arg(imageBrokenPixels).arg(floattohtmlstr(imageSum).c_str())
                        .arg(floattohtmlstr(imageSum/imageExposureTime/1000.0).c_str())
                        .arg(roundWithError(imageMean, imageStddev, 2)).arg(roundError(imageStddev, 2))
                        .arg(roundWithError(imageMean/imageExposureTime/1000.0, imageStddev/imageExposureTime/1000.0, 2)).arg(roundError(imageStddev/imageExposureTime/1000.0, 2))
                        .arg(imageImin).arg(imageImax).arg(imageImin/imageExposureTime/1000.0).arg(imageImax/imageExposureTime/1000.0).arg((double)image.width()*pixelWidth).arg((double)image.height()*pixelHeight);
        labImageStatistics->setText(s);
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Mask Data"),
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

void QFESPIMB040CameraView::loadMask() {
    if (m_stopresume) m_stopresume->stop();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Mask Data"),
                            lastMaskpath,
                            tr("Image Mask (*.msk)"));
    if (fileName.isEmpty()) return;
    lastMaskpath=QFileInfo(fileName).absolutePath();
    if (QMessageBox::question(this, tr("QuickFit SPIM Control"), tr("Do your really want to replace the mask with the file contents?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes )==QMessageBox::No ) return;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!mask.load_nonzero_list(fileName.toStdString())) {
        QMessageBox::warning(this, tr("QuickFit SPIM Control"),
                          tr("Cannot open file %1")
                          .arg(fileName));
    }
    maskEmpty=false;
    redrawFrameRecalc();
    QApplication::restoreOverrideCursor();
    if (m_stopresume) m_stopresume->resume();
}


void QFESPIMB040CameraView::saveRaw() {
    if (m_stopresume) m_stopresume->stop();
    QStringList imageFilters;
    imageFilters<<tr("16-Bit Grayscal TIFF (*.tif *.tiff16)");
    imageFilters<<tr("Float Grayscal TIFF (*.tif *.tiff)");
    imageFilters<<tr("Comma Separated Values (*.dat)");
    imageFilters<<tr("Color Coded PNG (*.png)");
    imageFilters<<tr("Color Coded BMP (*.bmp)");
    imageFilters<<tr("Color Coded TIFF (*.tif)");

    QString imFilter=lastImagefilter;


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Raw Image"),
                            lastImagepath,
                            imageFilters.join(";;"),&imFilter);

    /*QFileDialog dialog(this, tr("Save Raw Image as ..."), lastImagepath);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters(imageFilters);
    dialog.setNameFilter(imFilter);
    if (dialog.exec() != QDialog::Accepted) return ;
    imFilter = dialog.selectedNameFilter();
    QString fileName = dialog.selectedFiles()[0];*/

    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QuickFit SPIM Control"),
                          tr("Cannot write file %1:\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));
        if (m_stopresume) m_stopresume->resume();
        return;
    }
    file.close();
    file.remove();
    lastImagepath=QFileInfo(fileName).absolutePath();
    lastImagefilter=imFilter;
    QApplication::setOverrideCursor(Qt::WaitCursor);


    if (lastImagefilter==imageFilters[0]) {
        rawImage.save_tiffuint16(fileName.toStdString());
    } else if (lastImagefilter==imageFilters[1]) {
        rawImage.save_tifffloat(fileName.toStdString());
    } else if (lastImagefilter==imageFilters[2]) {
        rawImage.save_csv(fileName.toStdString());
    } else if (lastImagefilter==imageFilters[3]) {
        QImage img;
        JKQTFPimagePlot_array2image<uint32_t>(rawImage.data(), rawImage.width(), rawImage.height(), img, (JKQTFPColorPalette)cmbColorscale->currentIndex(), spinCountsLower->value(), spinCountsUpper->value());
        img.save(fileName, "PNG");
    } else if (lastImagefilter==imageFilters[4]) {
        QImage img;
        JKQTFPimagePlot_array2image<uint32_t>(rawImage.data(), rawImage.width(), rawImage.height(), img, (JKQTFPColorPalette)cmbColorscale->currentIndex(), spinCountsLower->value(), spinCountsUpper->value());
        img.save(fileName, "BMP");
    } else if (lastImagefilter==imageFilters[5]) {
        QImage img;
        JKQTFPimagePlot_array2image<uint32_t>(rawImage.data(), rawImage.width(), rawImage.height(), img, (JKQTFPColorPalette)cmbColorscale->currentIndex(), spinCountsLower->value(), spinCountsUpper->value());
        img.save(fileName, "TIFF");
    } else {
            rawImage.save_tiffuint16(fileName.toStdString());

    }


    QApplication::restoreOverrideCursor();
    if (m_stopresume) m_stopresume->resume();

}


void QFESPIMB040CameraView::displayImage(JKImage<uint32_t>& image, double timeindex, double exposuretime) {
    rawImage.assign(image);
    imageTimeindex=timeindex;
    imageExposureTime=exposuretime;
    imageStatisticsCalculated=false;
    redrawFrameRecalc(false);
}

void QFESPIMB040CameraView::displayImageComplete(JKImage<uint32_t>& image, double timeindex, double exposuretime) {
    rawImage.assign(image);
    imageTimeindex=timeindex;
    imageExposureTime=exposuretime;
    imageStatisticsCalculated=false;
    redrawFrameRecalc(true);
}

void QFESPIMB040CameraView::clearImage() {
    rawImage.assign(100, 100, 0);
    imageTimeindex=0;
    imageExposureTime=1;
    redrawFrameRecalc(true);
}

void QFESPIMB040CameraView::displayCameraConfig(QString camera, double framerate) {
    labVideoSettings->setText(tr("<div align=\"right\"><b>camera:</b> %1<br><b>framerate:</b> %2 fps&nbsp;&nbsp;<b>exposure:</b> %5ms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>size:</b> %3&times;%4").arg(camera).arg(framerate).arg(rawImage.width()).arg(rawImage.height()).arg(imageExposureTime*1e3));
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
            tabCursor.insertHtml(tr("distance = %1px = %2&mu;m").arg(d).arg(du));
        }


    }
    {
        QTextCursor tabCursor=table->cellAt(1, 1).firstCursorPosition();
        tabCursor.setBlockFormat(bfLeft);
        QPicture& pic=picMain;
        //double scale=document->textWidth()*0.78/pic.boundingRect().width();
        //if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, pic.boundingRect().topLeft(), QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    {
        QTextCursor tabCursor=table->cellAt(1, 0).firstCursorPosition();
        tabCursor.setBlockFormat(bfRight);
        QPicture& pic=picMarginLeft;
        //double scale=document->textWidth()*0.18/pic.boundingRect().width();
        //if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, pic.boundingRect().topLeft(), QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
    }
    {
        QTextCursor tabCursor=table->cellAt(2, 1).firstCursorPosition();
        tabCursor.setBlockFormat(bfLeft);
        QPicture& pic=picMarginBottom;
        //double scale=document->textWidth()*0.78/pic.boundingRect().width();
        //if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, pic.boundingRect().topLeft(), QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
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
        tabCursor.insertHtml(labImageStatistics->text());
        tabCursor.insertBlock();
        tabCursor.insertBlock();
        tabCursor.insertHtml(labMarginalFitResults->text());
    }
}

void QFESPIMB040CameraView::saveReport() {
    if (m_stopresume) m_stopresume->stop();
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                lastImagepath,
                                tr("PDF File (*.pdf);;PostScript File (*.ps)"));

    if (!fn.isEmpty()) {
        lastImagepath=QFileInfo(fn).absolutePath();
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

void QFESPIMB040CameraView::saveData() {

    QStringList filters;
    filters<< tr("Comma-Separated value (*.dat *.csv)");
    filters<< tr("Semicolon-Separated value [German Excel] (*.txt)");
    filters<< tr("SYLK (*.slk)");

    QString selFilter=filters[0];

    if (m_stopresume) m_stopresume->stop();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Data"),
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
        for (int i=0; i<pltDataMarginalLeftN; i++) {
            datat.set(col-4, i, pltDataMarginalLeftX[i]);
            datat.set(col-3, i, pltDataMarginalLeftY[i]);
        }
        datat.set_title(col-2, "marginal_bottom_x");
        datat.set_title(col-1, "marginal_bottom_data");
        for (int i=0; i<pltDataMarginalBottomN; i++) {
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
        for (int i=0; i<histogram_n; i++) {
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
