#include "qfespimb040cameraview.h"

#include <QtGui>

QFESPIMB040CameraView::QFESPIMB040CameraView(const QString& logfile, QFExtensionManager* extManager, QWidget* parent):
    QWidget(parent)
{
    setWindowIcon(QIcon(":/spimb040_logo.png"));


    // more variable initialisation
    imageStatisticsCalculated=false;

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




    // create widgets and actions
    createMainWidgets(logfile);
    createActions();



    // display test images set above
    clearImage();

}

QFESPIMB040CameraView::~QFESPIMB040CameraView()
{
    free(histogram_x);
    free(histogram_y);
}

void QFESPIMB040CameraView::createMainWidgets(const QString& logfile) {

    ///////////////////////////////////////////////////////////////
    // create main layout, toolbar and Splitters
    ///////////////////////////////////////////////////////////////
    QVBoxLayout* vbl=new QVBoxLayout(this);
    setLayout(vbl);
    vbl->setContentsMargins(0,0,0,0);
    toolbar=new QToolBar(tr("camera_toolbar"), this);
    vbl->addWidget(toolbar);
    splitVert=new QSplitter(Qt::Horizontal, this);
    splitHor=new QSplitter(Qt::Vertical, this);
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
    pltMain=new JKQTFastPlotter(wPltMain);
    pltMain->set_maintainAspectRatio(true);
    pltMain->setXRange(0,100);
    pltMain->setYRange(0,100);
    pltMain->set_xTickDistance(10);
    pltMain->set_yTickDistance(10);
    plteFrame=new JKQTFPimagePlot(pltMain, image.data(), JKQTFP_double, image.width(), image.height(), JKQTFP_GRAY);
    pltMain->addPlot(plteFrame);
    plteMask=new JKQTFPimageOverlayPlot(pltMain, mask.data(), mask.width(), mask.height());
    pltMain->addPlot(plteMask);
    connect(pltMain, SIGNAL(mouseMoved(double, double)), this, SLOT(imageMouseMoved(double, double)));
    connect(pltMain, SIGNAL(clicked(double, double)), this, SLOT(imageMouseClicked(double, double)));
    vbl->addWidget(pltMain);
    vbl->setStretchFactor(pltMain, 10);

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
    QHBoxLayout* hbl=new QHBoxLayout(w);
    spinCountsLower=new QDoubleSpinBox(w);
    spinCountsLower->setMaximum(0xEFFFFF);
    spinCountsLower->setMinimum(-0xEFFFFF);
    QLabel* l=new QLabel(tr("m&in. gray:"), w);
    l->setBuddy(spinCountsLower);
    hbl->addWidget(l);
    hbl->addWidget(spinCountsLower);
    spinCountsUpper=new QDoubleSpinBox(w);
    spinCountsUpper->setMaximum(0xEFFFFF);
    spinCountsUpper->setMinimum(-0xEFFFFF);
    // NOTE:
    // spinCountsLower and spinCountsUpper are connected/disconnected by a call to setCountsAutoscale():
    setCountsAutoscale(true);

    l=new QLabel(tr("m&ax. gray:"), w);
    l->setBuddy(spinCountsUpper);
    hbl->addWidget(l);
    hbl->addWidget(spinCountsUpper);
    chkCountsRangeAuto=new QCheckBox(tr("&auto"), w);
    connect(chkCountsRangeAuto, SIGNAL(clicked(bool)), this, SLOT(setCountsAutoscale(bool)));
    hbl->addWidget(chkCountsRangeAuto);
    hbl->addStretch(2);
    vbl->addLayout(hbl);


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

    actMaskEdit = new QAction(QIcon(":/spimb040/maskedit.png"), tr("&Edit mask (broken pixels)"), this);
    actMaskEdit->setCheckable(true);


    toolbar->addAction(actSaveRaw);
    toolbar->addSeparator();
    toolbar->addAction(actMaskLoad);
    toolbar->addAction(actMaskSave);
    toolbar->addAction(actMaskEdit);
    toolbar->addAction(actMaskClear);
    toolbar->addSeparator();

}


void QFESPIMB040CameraView::loadSettings(ProgramOptions* settings, QString prefix) {
    jkloadSplitter((*settings->getQSettings()), splitHor,  prefix+"split_hor/");
    jkloadSplitter((*settings->getQSettings()), splitVert, prefix+"split_vert/");

    spinCountsLower->setValue((settings->getQSettings())->value(prefix+"histogram.min", 0).toInt());
    spinCountsUpper->setValue((settings->getQSettings())->value(prefix+"histogram.max", 255).toInt());
    bool b=(settings->getQSettings())->value(prefix+"histogram.auto", true).toBool();
    chkCountsRangeAuto->setChecked(b);
    setCountsAutoscale(b);
    b=(settings->getQSettings())->value(prefix+"histogram.log", false).toBool();
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

    lastImagepath=(settings->getQSettings())->value(prefix+"last_imagepath", lastImagepath).toString();
    lastMaskpath=(settings->getQSettings())->value(prefix+"last_maskpath", lastMaskpath).toString();
    lastImagefilter=(settings->getQSettings())->value(prefix+"last_imagefilter", lastImagefilter).toString();
}

void QFESPIMB040CameraView::storeSettings(ProgramOptions* settings, QString prefix) {
    jksaveSplitter((*settings->getQSettings()), splitHor,  prefix+"split_hor/");
    jksaveSplitter((*settings->getQSettings()), splitVert, prefix+"split_vert/");

    (settings->getQSettings())->setValue(prefix+"last_imagepath", lastImagepath);
    (settings->getQSettings())->setValue(prefix+"last_maskpath", lastMaskpath);
    (settings->getQSettings())->setValue(prefix+"last_imagefilter", lastImagefilter);

    (settings->getQSettings())->setValue(prefix+"histogram.min", spinCountsLower->value());
    (settings->getQSettings())->setValue(prefix+"histogram.max", spinCountsUpper->value());
    (settings->getQSettings())->setValue(prefix+"histogram.auto", chkCountsRangeAuto->isChecked());
    (settings->getQSettings())->setValue(prefix+"histogram.log", chkHistogramLog->isChecked());
    (settings->getQSettings())->setValue(prefix+"histogram.items", histogram_n);

    (settings->getQSettings())->setValue(prefix+"imagesettings.palette", cmbColorscale->currentIndex());
    (settings->getQSettings())->setValue(prefix+"imagesettings.mask_color", cmbMaskColor->currentIndex());
    (settings->getQSettings())->setValue(prefix+"imagesettings.rotation", cmbRotation->currentIndex());

}

void QFESPIMB040CameraView::imageMouseMoved(double x, double y) {
    uint32_t xx=floor(x);
    uint32_t yy=floor(y);

    if ((xx>=0) && (xx<image.width()) && (yy>=0) && (yy<image.height())) {
        QString s=QString::number(image(xx,yy));
        if (mask(xx, yy)) {
            s=s+tr(" <font color=\"red\">[broken pixel]</font>");
        }
        labCurrentPos->setText(tr("image(%1, %2) = %3").arg(xx).arg(yy).arg(s));
    } else {
        labCurrentPos->setText(tr(""));
    }
}

void QFESPIMB040CameraView::imageMouseClicked(double x, double y) {
    if (actMaskEdit->isChecked()) {
        uint32_t xx=floor(x);
        uint32_t yy=floor(y);

        uint32_t xxold=xx;
        uint32_t yyold=yy;

        switch(cmbRotation->currentIndex()) {
            case 1: yy=xxold; xx=image.width()-yyold-1; break;
            case 2: yy=image.height()-yyold-1; xx=image.width()-xxold-1; break;
            case 3: yy=image.height()-xxold-1; xx=yyold; break;
        };

        if ((xx>=0) && (xx<image.width()) && (yy>=0) && (yy<image.height())) {
            mask(xx, yy)=!mask(xx, yy);
        }
    }
    redrawFrameRecalc();
}


void QFESPIMB040CameraView::setCountsAutoscale(bool autoscale) {
    if (autoscale) {
        spinCountsUpper->setEnabled(false);
        spinCountsLower->setEnabled(false);
        disconnect(spinCountsLower, SIGNAL(valueChanged(double)), this, SLOT(redrawFrameRecalc()));
        disconnect(spinCountsUpper, SIGNAL(valueChanged(double)), this, SLOT(redrawFrameRecalc()));
    } else {
        spinCountsUpper->setEnabled(true);
        spinCountsLower->setEnabled(true);
        connect(spinCountsLower, SIGNAL(valueChanged(double)), this, SLOT(redrawFrameRecalc()));
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
    pltMain->set_doDrawing(false);
    plteMask->set_color(cmbMaskColor->currentColor());

    if (cmbRotation->currentIndex()%2==1) {
        plteFrame->set_xmax(image.height());
        plteFrame->set_ymax(image.width());
        pltMain->setXRange(0, image.height());
        pltMain->setYRange(0, image.width());
        pltMain->set_xTickDistance(pow(10,floor(log10(image.height()))));
        pltMain->set_yTickDistance(pow(10,floor(log10(image.width()))));
        plteMask->set_xmax(image.height());
        plteMask->set_ymax(image.width());
        pltMain->set_aspectRatio((double)image.height()/(double)image.width());
    } else {
        plteFrame->set_xmax(image.width());
        plteFrame->set_ymax(image.height());
        pltMain->setXRange(0, image.width());
        pltMain->setYRange(0, image.height());
        pltMain->set_xTickDistance(pow(10,floor(log10(image.width()))));
        pltMain->set_yTickDistance(pow(10,floor(log10(image.height()))));
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
    plteFrame->set_image(image.data(), JKQTFP_double, image.width(), image.height());
    pltMain->set_doDrawing(true);
    pltMain->update_data();

}

void QFESPIMB040CameraView::redrawFrameRecalc() {
    prepareImage();
    displayImageStatistics();
    redrawFrame();
}

void QFESPIMB040CameraView::displayImageStatistics() {
    if (!histogram_x || !histogram_y) return;

    // DISABLE UPDATING OF GRAPHS (we only want to do this once per function call!!!)
    pltCountsHistogram->set_doDrawing(false);

    // CALCULATE STATISTICS OF TRANSFORMED IMAGE
    double histogram_min=0;
    double histogram_max=0;
    double histogram_fmax=0;
    histogram_n=spinHistogramBins->value();
    image.calcImageStatistics(mask.data(), &imageBrokenPixels, &imageSum, &imageMean, &imageStddev, &imageImin, &imageImax, &histogram_x, &histogram_y, &histogram_n, &histogram_min, &histogram_max, &histogram_fmax, chkHistogramBinsAuto->isChecked());
    imageStatisticsCalculated=true;
    double cmin= spinCountsLower->value();
    double cmax= spinCountsUpper->value();
    if (chkCountsRangeAuto->isChecked()) {
        cmin=imageImin;
        cmax=imageImax;
    }
    double hrange=imageImax-imageImin;


    spinHistogramBins->setValue(histogram_n);
    plteHistogram->set_data(histogram_x, histogram_y, histogram_n);
    double dt=pow(10,floor(log10(histogram_max-histogram_min)));
    pltCountsHistogram->set_xTickDistance(dt);
    dt=pow(10,floor(log10(histogram_fmax)));
    pltCountsHistogram->set_yTickDistance(dt);

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


    if ((histogramUpdateTime.elapsed()>1000) || (last_log!=chkHistogramLog->isChecked())) {
        pltCountsHistogram->setYRange(ymin, ymax, logarithmic);
        pltCountsHistogram->setXRange(xmin, xmax, false);
        histogramUpdateTime.start();
    } else {
        if (xmin<pltCountsHistogram->get_xMin()) pltCountsHistogram->setXRange(xmin, pltCountsHistogram->get_xMax(), false);
        if (xmax>pltCountsHistogram->get_xMax()) pltCountsHistogram->setXRange(pltCountsHistogram->get_xMin(), xmax, false);
        if (ymin<pltCountsHistogram->get_yMin()) pltCountsHistogram->setYRange(ymin, pltCountsHistogram->get_yMax(), logarithmic);
        if (ymax>pltCountsHistogram->get_yMax()) pltCountsHistogram->setYRange(ymin, pltCountsHistogram->get_yMax(), logarithmic);
    }
    last_log=chkHistogramLog->isChecked();

    plteHistogramRange->set_xmin((double)cmin);
    plteHistogramRange->set_xmax((double)cmax);

    if (chkCountsRangeAuto->isChecked()) {
        spinCountsLower->setValue(cmin);
        spinCountsUpper->setValue(cmax);
    }


    pltCountsHistogram->set_doDrawing(true);
    pltCountsHistogram->update_plot();

    QString s=tr("<b>Image Statistics:</b><br><center><table border=\"0\" width=\"90%\"><tr><td>size = </td><td>%1 &times; %2</td><td></td></tr><tr><td>broken pixels = </td><td>%3</td><td></td></tr><tr><td>&nbsp;</td><td></td><td></td></tr><tr><td></td><td><b># photons</b></td><td><b>count rate [kHz]</b></td></tr> <tr><td>sum = </td><td>%4</td><td>%5</td></tr> <tr><td>average = </td><td>%6 &plusmn; %7</td><td>%8 &plusmn; %9</td></tr> <tr><td>min ... max = </td><td>%10 ... %11</td><td>%12 ... %13</td></tr> </table></center>").arg(image.width()).arg(image.height()).arg(imageBrokenPixels).arg(floattohtmlstr(imageSum).c_str()).arg(floattohtmlstr(imageSum/imageExposureTime/1000.0).c_str()).arg(imageMean).arg(imageStddev).arg(imageMean/imageExposureTime/1000.0).arg(imageStddev/imageExposureTime/1000.0).arg(imageImin).arg(imageImax).arg(imageImin/imageExposureTime/1000.0).arg(imageImax/imageExposureTime/1000.0);
    labImageStatistics->setText(s);
}

void QFESPIMB040CameraView::clearMask() {
    if (QMessageBox::question(this, tr("QuickFit SPIM Control"), tr("Do your really want to clear the mask?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes )==QMessageBox::No ) return;
    mask.setAll(false);
    redrawFrameRecalc();
}

void QFESPIMB040CameraView::saveMask() {
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
}

void QFESPIMB040CameraView::loadMask() {
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
    redrawFrameRecalc();
    QApplication::restoreOverrideCursor();
}

void QFESPIMB040CameraView::prepareImage() {
    image.assign(rawImage);
    if (image.sizeDiffersFrom(mask)) {
        mask.assign(image.width(), image.height(), false);
    }
}

void QFESPIMB040CameraView::saveRaw() {
    QStringList imageFilters;
    imageFilters<<tr("16-Bit Grayscal TIFF (*.tif)");
    imageFilters<<tr("Float Grayscal TIFF (*.tif)");
    imageFilters<<tr("Comma Separated Values (*.dat)");
    imageFilters<<tr("Color Coded PNG (*.png)");
    imageFilters<<tr("Color Coded BMP (*.bmp)");
    imageFilters<<tr("Color Coded TIFF (*.tif)");


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Raw Image"),
                            lastImagepath,
                            imageFilters.join(";;"),&lastImagefilter);
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("QuickFit SPIM Control"),
                          tr("Cannot write file %1:\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));
        return;
    }
    file.close();
    lastImagepath=QFileInfo(fileName).absolutePath();
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
    }


    QApplication::restoreOverrideCursor();

}


void QFESPIMB040CameraView::displayImage(JKImage<uint32_t>& image, double timeindex, double exposuretime) {
    rawImage.assign(image);
    imageTimeindex=timeindex;
    imageExposureTime=exposuretime;
    redrawFrameRecalc();
}

void QFESPIMB040CameraView::clearImage() {
    rawImage.assign(100, 100, 0);
    imageTimeindex=0;
    imageExposureTime=1;
    redrawFrameRecalc();
}

void QFESPIMB040CameraView::displayCameraConfig(QString camera, double framerate) {
    labVideoSettings->setText(tr("<div align=\"right\"><b>camera:</b> %1<br><b>framerate:</b> %2 fps&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>size:</b> %3&times;%4</div>").arg(camera).arg(framerate).arg(rawImage.width()).arg(rawImage.height()));
}


void QFESPIMB040CameraView::addUserAction(QAction* action) {
    toolbar->addAction(action);
}

void QFESPIMB040CameraView::deleteUserAction(QAction* action) {
    toolbar->removeAction(action);
}
