#include "qfrdrimagestack_dataeditor.h"
#include "qfrdrimagestack_data.h"
#include "qfrawdatapropertyeditor.h"

QFRDRImageStackDataEditor::QFRDRImageStackDataEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    dataMode=QFRDRImageStackDataEditor::dmFullHistogram;
    maskTools=new QFRDRImageMaskEditTools(this, QString("imagestackeditor/"));
    createWidgets();
    connect(maskTools, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
}

QFRDRImageStackDataEditor::~QFRDRImageStackDataEditor()
{
    //dtor
}

void QFRDRImageStackDataEditor::createWidgets() {
    QVBoxLayout* mainLay=new QVBoxLayout(this);
    setLayout(mainLay);

    QHBoxLayout* layTop=new QHBoxLayout(this);
    mainLay->addLayout(layTop);
    layTop->addWidget(new QLabel(tr("<b>image stack:</b>")));
    cmbImageStack=new QComboBox(this);
    layTop->addWidget(cmbImageStack);
    layTop->addStretch();

    spinBins=new QSpinBox(this);
    spinBins->setRange(10,100000);
    spinBins->setValue(256);
    layTop->addWidget(new QLabel(tr("histogram bins: ")));
    layTop->addWidget(spinBins);


    layTop=new QHBoxLayout(this);
    mainLay->addLayout(layTop);
    layTop->addWidget(new QLabel(tr("<b>channel mode:</b>")));
    cmbChannelMode=new QComboBox(this);
    cmbChannelMode->addItem(tr("single-channel"));
    cmbChannelMode->addItem(tr("3-channel RGB"));
    cmbChannelMode->setCurrentIndex(0);
    layTop->addWidget(cmbChannelMode);
    layTop->addWidget(new QLabel(tr("<b>channels:</b>")));
    cmbChannelR=new QComboBox(this);
    layTop->addWidget(cmbChannelR);
    cmbChannelG=new QComboBox(this);
    layTop->addWidget(cmbChannelG);
    cmbChannelB=new QComboBox(this);
    layTop->addWidget(cmbChannelB);
    layTop->addStretch();

    layTop->addWidget(new QLabel(tr("<b>mask color:</b>")));
    cmbMaskColor=new ColorComboBox(this);
    layTop->addWidget(cmbMaskColor);
    layTop->addStretch();


    toolbar=new QToolBar("", this);
    mainLay->addWidget(toolbar);

    pltImage=new JKQtPlotter(this);
    pltImage->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltImage->set_displayMousePosition(true);
    pltImage->set_displayToolbar(false);
    pltImage->get_plotter()->set_maintainAspectRatio(true);
    pltImage->get_plotter()->set_aspectRatio(1);
    pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
    pltImage->get_plotter()->set_axisAspectRatio(1);
    pltImage->setXY(0,0,1,1);
    pltImage->setAbsoluteXY(0,1,0,1);
    pltImage->get_plotter()->getXAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getYAxis()->set_axisMinWidth(1);
    pltImage->get_plotter()->getXAxis()->set_labelType(JKQTPCALTdefault);
    pltImage->get_plotter()->getYAxis()->set_labelType(JKQTPCALTdefault);
    pltImage->get_plotter()->getXAxis()->set_labelDigits(2);
    pltImage->get_plotter()->getYAxis()->set_labelDigits(2);
    pltImage->get_plotter()->getXAxis()->set_minTicks(5);
    //pltImage->get_plotter()->getXAxis()->addAxisTickLabel(0,"0");
    pltImage->get_plotter()->getYAxis()->set_minTicks(5);
    //pltImage->get_plotter()->getYAxis()->addAxisTickLabel(0,"0");

    pltImage->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltImage->get_plotter()->getXAxis()->set_axisLabel("");
    pltImage->get_plotter()->getYAxis()->set_axisLabel("");
    pltImage->get_plotter()->setGrid(false);
    pltImage->get_plotter()->set_useAntiAliasingForSystem(true);
    pltImage->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltImage->set_userActionCompositionMode(QPainter::CompositionMode_Xor);

    maskTools->registerPlotter(pltImage);



    pltData=new JKQtPlotter(this);
    pltData->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltData->get_plotter()->getXAxis()->set_minTicks(5);
    pltData->get_plotter()->getYAxis()->set_minTicks(5);

    pltData->get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    pltData->get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    pltData->get_plotter()->getXAxis()->set_axisLabel("");
    pltData->get_plotter()->getYAxis()->set_axisLabel("");
    pltData->get_plotter()->setGrid(true);
    pltData->get_plotter()->set_useAntiAliasingForSystem(true);
    pltData->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltData->set_userActionCompositionMode(QPainter::CompositionMode_Xor);


    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitter->addWidget(pltImage);
    splitter->addWidget(pltData);
    mainLay->addWidget(splitter, 1);
    QList<int> sizes;
    sizes<<round(splitter->width()/2.0)<<round(splitter->width()/2.0);
    splitter->setSizes(sizes);



    labDescription=new QLabel(this);
    mainLay->addWidget(labDescription);
    labFrame=new QLabel(this);
    mainLay->addWidget(labFrame);

    player=new QFPLayerControls(this);
    player->setVisible(false);
    mainLay->addWidget(labTAxis=new QLabel(tr("<b>time/z axis:</b>")));
    mainLay->addWidget(player);
    mainLay->addStretch();


    image=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0, 0, JKQTPMathImage::GRAY, pltImage->get_plotter());
    image->get_colorBarRightAxis()->set_labelType(JKQTPCALTdefault);
    image->get_colorBarRightAxis()->set_labelDigits(2);
    image->get_colorBarRightAxis()->set_minTicks(3);
    //pltImage->addGraph(image);

    imageRGB=new JKQTPRGBMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0, 0, pltImage->get_plotter());
    imageRGB->set_datatypeG(JKQTPMathImageBase::DoubleArray);
    imageRGB->set_datatypeB(JKQTPMathImageBase::DoubleArray);
    imageRGB->get_colorBarRightAxis()->set_labelType(JKQTPCALTdefault);
    imageRGB->get_colorBarRightAxis()->set_labelDigits(2);
    imageRGB->get_colorBarRightAxis()->set_minTicks(3);

    QColor ovlExCol=QColor("black");
    //ovlExCol.setAlphaF(0.5);
    plteOverviewExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, pltImage->get_plotter());
    plteOverviewExcluded->set_rectanglesAsImageOverlay(true);



    player->setSingleShot(true);

    toolbar->addAction(pltImage->get_plotter()->get_actSavePlot());
    toolbar->addAction(pltImage->get_plotter()->get_actPrint());
    toolbar->addAction(pltImage->get_plotter()->get_actCopyPixelImage());
    toolbar->addSeparator();
    toolbar->addAction(pltImage->get_plotter()->get_actZoomAll());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomIn());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomOut());
    toolbar->addSeparator();
    //toolbar->addAction(actEditMask);
    maskTools->registerPlotterMaskToolsToToolbar(toolbar);


    menuMask=propertyEditor->addMenu("&Mask", 0);
    /*menuMask->addAction(actClearMask);
    menuMask->addAction(actInvertMask);
    menuMask->addAction(actEditMask);
    menuMask->addSeparator();
    menuMask->addAction(actSaveMask);
    menuMask->addAction(actLoadMask);
    menuMask->addSeparator();
    menuMask->addAction(actCopyMask);
    menuMask->addAction(actPasteMask);*/
    maskTools->registerMaskToolsToMenu(menuMask);

};

void QFRDRImageStackDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    disconnect(cmbImageStack, SIGNAL(currentIndexChanged(int)), this, SLOT(stackChanged()));
    disconnect(player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));
    disconnect(cmbChannelMode, SIGNAL(currentIndexChanged(int)), this, SLOT(channelModeChanged()));
    //disconnect(pltImage, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)));

    if (old) {
        this->current=NULL;
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    maskTools->setRDR(current);
    QFRDRImageStackData* m=qobject_cast<QFRDRImageStackData*>(current);
    cmbImageStack->clear();
    this->current=current;
    connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    if (m) {
        for (int i=0; i<m->getImageStackCount(); i++) {
            QString n=m->getImageStackDescription(i);
            if (n.isEmpty()) n=tr("stack %1").arg(i+1);
            cmbImageStack->addItem(QIcon(":/image_stack/image.png"), n);
        }
    }


    cmbImageStack->setCurrentIndex(current->getProperty("imstack_invrimgdisp_image", 0).toInt());

    dataMode=QFRDRImageStackDataEditor::dmFullHistogram;

    player->setPosition(current->getProperty("imstack_invrimgdisp_playpos", 0).toInt());
    cmbMaskColor->setCurrentColor(QStringToQColor(current->getProperty("imstack_invrimgdisp_maskcolor", "gray").toString()));
    connect(cmbImageStack, SIGNAL(currentIndexChanged(int)), this, SLOT(stackChanged()));
    connect(player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));
    connect(cmbChannelMode, SIGNAL(currentIndexChanged(int)), this, SLOT(channelModeChanged()));
    //connect(pltImage, SIGNAL(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)), this, SLOT(plotMouseClicked(double,double,Qt::KeyboardModifiers,Qt::MouseButton)));
    channelModeChanged();
    stackChanged();
};

void QFRDRImageStackDataEditor::rawDataChanged() {
	// emitted when the raw data changes 
	QFRDRImageStackData* m=qobject_cast<QFRDRImageStackData*>(current);
    //labTest->setText(tr("loaded record with files<br><i>%1</i>").arg(m->getFiles().join("<br>")));
    displayImage();
    //qDebug()<<"QFRDRImageStackDataEditor::rawDataChanged()";
};

void QFRDRImageStackDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    QString prefix="image_stack/";
    player->setFPS(settings->getQSettings()->value(prefix+"player_fps", player->getFPS()).toDouble());
    player->setReplay(settings->getQSettings()->value(prefix+"player_replay", player->getReplay()).toBool());
    cmbChannelMode->setCurrentIndex(settings->getQSettings()->value(prefix+"imagemode", 0).toInt());
    spinBins->setValue(settings->getQSettings()->value(prefix+"bins", 200).toInt());
    loadSplitter(*(settings->getQSettings()), splitter, prefix);
}


void QFRDRImageStackDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
    QString prefix="image_stack/";
    settings->getQSettings()->setValue(prefix+"player_fps", player->getFPS());
    settings->getQSettings()->setValue(prefix+"player_replay", player->getReplay());
    settings->getQSettings()->setValue(prefix+"imagemode", cmbChannelMode->currentIndex());
    settings->getQSettings()->setValue(prefix+"bins", spinBins->value());
    saveSplitter(*(settings->getQSettings()), splitter, prefix);
}

void QFRDRImageStackDataEditor::addDataHistogram(double *data, bool* mask, int size, const QString &title, const QString &colX, const QString &colY, QColor col, double shift, double width) {
    JKQTPdatastore* dsData=pltData->getDatastore();

    double* histX=(double*)calloc(spinBins->value(), sizeof(double));
    double* histY=(double*)calloc(spinBins->value(), sizeof(double));

    if (mask) statisticsHistogramMasked(data, mask, size, histX, histY, spinBins->value(), false, false);
    else statisticsHistogram(data, size, histX, histY, spinBins->value(), false);
    size_t chx=dsData->addCopiedColumn(histX, spinBins->value(), colX);
    size_t chy=dsData->addCopiedColumn(histY, spinBins->value(), colY);

    JKQTPbarHorizontalGraph* plteHistogram=new JKQTPbarHorizontalGraph(pltData->get_plotter());
    plteHistogram->set_title(title);
    plteHistogram->set_xColumn(chx);
    plteHistogram->set_yColumn(chy);
    plteHistogram->set_shift(shift);
    plteHistogram->set_width(width);
    plteHistogram->set_fillColor(col);
    plteHistogram->set_color(col.darker());
    pltData->addGraph(plteHistogram);

    free(histX);
    free(histY);
}


void QFRDRImageStackDataEditor::showFrame(int frame, bool startPlayer) {
    bool dd=pltImage->get_doDrawing();
    pltImage->set_doDrawing(false);
    bool ddd=pltData->get_doDrawing();
    pltData->set_doDrawing(false);
    current->setQFProperty("imstack_invrimgdisp_playpos", player->getPosition(), false, false);
    current->setQFProperty("imstack_invrimgdisp_maskcolor", cmbMaskColor->currentColor().name());
    pltImage->clearGraphs(false);
    pltData->clearGraphs(true);
    JKQTPdatastore* dsData=pltData->getDatastore();
    dsData->clear();

    QFRDRImageStackData* mv=qobject_cast<QFRDRImageStackData*>(current);
    if (mv && cmbImageStack->currentIndex()<mv->getImageStackCount()) {
        int idx=cmbImageStack->currentIndex();
        int width=mv->getImageStackWidth(idx);
        int height=mv->getImageStackHeight(idx);
        int channel=cmbChannelR->currentIndex();
        int channelG=cmbChannelG->currentIndex();
        int channelB=cmbChannelB->currentIndex();
        if (cmbChannelMode->currentIndex()==0) {
            double* ir=NULL;

            if (channel>=0 && channel<mv->getImageStackChannels(idx)) ir=mv->getImageStack(idx, frame, channel);
            image->set_width(width);
            image->set_height(height);
            //qDebug()<<"frame="<<frame<<"   "<<ir;

            image->set_data(ir, width, height, JKQTPMathImageBase::DoubleArray);

            if (mv->maskGet()) {
                double cmin=0;
                double cmax=0;
                statisticsMaskedMinMax(ir, mv->maskGet(), width*height, cmin, cmax, false);
                image->set_imageMin(cmin);
                image->set_imageMax(cmax);
                image->set_autoImageRange(false);
            } else {
                image->set_autoImageRange(true);
            }

            pltImage->addGraph(image);


            if (dataMode==QFRDRImageStackDataEditor::dmFullHistogram && ir) {
                addDataHistogram(ir, mv->maskGet(), width*height, tr("full histogram"), tr("histogram_x"), tr("histogram_y"));
            }
        } else {
            double* ir=NULL;
            double* ig=NULL;
            double* ib=NULL;


            if (channel>=0 && channel<mv->getImageStackChannels(idx)) ir=mv->getImageStack(idx, frame, channel);
            if (channelG>=0 && channelG<mv->getImageStackChannels(idx)) ig=mv->getImageStack(idx, frame, channelG);
            if (channelB>=0 && channelB<mv->getImageStackChannels(idx)) ib=mv->getImageStack(idx, frame, channelB);
            //qDebug()<<"frame="<<frame<<"   "<<ir<<ig<<ib;
            imageRGB->set_data(ir, ig, ib, width, height, JKQTPMathImageBase::DoubleArray);
            imageRGB->set_width(width);
            imageRGB->set_height(height);

            if (mv->maskGet()) {
                double cmin=0;
                double cmax=0;
                if (ir) {
                    statisticsMaskedMinMax(ir, mv->maskGet(), width*height, cmin, cmax, false);
                    imageRGB->set_imageMin(cmin);
                    imageRGB->set_imageMax(cmax);
                }

                if (ig) {
                    cmin=cmax=0;
                    statisticsMaskedMinMax(ig, mv->maskGet(), width*height, cmin, cmax, false);
                    imageRGB->set_imageMinG(cmin);
                    imageRGB->set_imageMaxG(cmax);
                }

                if (ib) {
                    cmin=cmax=0;
                    statisticsMaskedMinMax(ib, mv->maskGet(), width*height, cmin, cmax, false);
                    imageRGB->set_imageMinB(cmin);
                    imageRGB->set_imageMaxB(cmax);
                }

                imageRGB->set_autoImageRange(false);
            } else {
                imageRGB->set_autoImageRange(true);
            }


            pltImage->addGraph(imageRGB);
            if (dataMode==QFRDRImageStackDataEditor::dmFullHistogram) {
                if (ir) addDataHistogram(ir, mv->maskGet(), width*height, tr("red histogram"), tr("histogram_red_x"), tr("histogram_red_y"), QColor("darkred"), 0, 0.3);
                if (ig) addDataHistogram(ig, mv->maskGet(), width*height, tr("green histogram"), tr("histogram_green_x"), tr("histogram_green_y"), QColor("darkgreen"),0.33, 0.3);
                if (ib) addDataHistogram(ib, mv->maskGet(), width*height, tr("blue histogram"), tr("histogram_blue_x"), tr("histogram_blue_y"), QColor("darkblue"), 0.66, 0.3);
            }
        }

        plteOverviewExcluded->set_data(mv->maskGet(), mv->maskGetWidth(), mv->maskGetHeight());
        plteOverviewExcluded->set_width(width);
        plteOverviewExcluded->set_height(height);
        plteOverviewExcluded->set_trueColor(cmbMaskColor->currentColor());
        pltImage->addGraph(plteOverviewExcluded);

        emit displayedFrame((double)frame*mv->getImageStackTUnitFactor(idx));
        labFrame->setText(tr("<b>frame:</b> %1/%2 (%3 %4), masked: %5").arg(frame+1).arg(mv->getImageStackFrames(idx)).arg(frame*mv->getImageStackTUnitFactor(idx)).arg(mv->getImageStackTUnitName(idx)).arg(mv->maskGetCount()));
    }



    if (dd) {
        pltImage->update_plot();
        pltImage->set_doDrawing(dd);
        pltImage->update_plot();
        if (startPlayer) player->singleShotTimerStart();
    }
    if (ddd) {
        pltData->zoomToFit();
        pltData->set_doDrawing(ddd);
        pltData->update_plot();
    }
}

void QFRDRImageStackDataEditor::displayImage() {
    if (!image) return;
    player->pause();
    current->setQFProperty("imstack_invrimgdisp_image", cmbImageStack->currentIndex(), false, false);
    QFRDRImageStackData* mv=qobject_cast<QFRDRImageStackData*>(current);
    player->setVisible(false);
    player->pause();
    pltImage->set_doDrawing(false);
    if (mv) {
        int idx=cmbImageStack->currentIndex();

        int width=mv->getImageStackWidth(idx);
        double rwidth=(double)width*mv->getImageStackXUnitFactor(idx);
        int height=mv->getImageStackHeight(idx);
        double rheight=(double)height*mv->getImageStackYUnitFactor(idx);
        int frames=mv->getImageStackFrames(idx);

        player->setRange(0, frames-1);
        //player->setPosition(0);
        player->setSingleShot(true);
        player->setVisible(true);
        pltImage->get_plotter()->setAbsoluteXY(0,rwidth,0,rheight);
        pltImage->get_plotter()->set_aspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_axisAspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_maintainAspectRatio(true);
        pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        pltImage->get_xAxis()->set_axisLabel(QString("%1 [%2]").arg(mv->getImageStackXName(idx)).arg(mv->getImageStackXUnitName(idx)));
        pltImage->get_yAxis()->set_axisLabel(QString("%1 [%2]").arg(mv->getImageStackYName(idx)).arg(mv->getImageStackYUnitName(idx)));
        labDescription->setText(tr("<b>image size:</b> %1 &times; %2 &times;pixels").arg(width).arg(height));

        showFrame(player->getPosition(), false);
    } else  {
        image->set_data(NULL, 0, 0, JKQTPMathImageBase::UInt16Array);
        player->setVisible(false);
        player->setRange(0,100);
    }
    pltImage->set_doDrawing(true);
    //pltImage->zoomToFit();
    pltImage->update_plot();
}

void QFRDRImageStackDataEditor::stackChanged() {
    QFRDRImageStackData* mv=qobject_cast<QFRDRImageStackData*>(current);
    int idx=cmbImageStack->currentIndex();
    disconnect(cmbChannelR, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    disconnect(cmbChannelG, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    disconnect(cmbChannelB, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    if (cmbChannelR->currentIndex()>=0) current->setQFProperty("imstack_invrimgdisp_channelr", cmbChannelR->currentIndex(), false, false);
    if (cmbChannelG->currentIndex()>=0) current->setQFProperty("imstack_invrimgdisp_channelg", cmbChannelG->currentIndex(), false, false);
    if (cmbChannelB->currentIndex()>=0) current->setQFProperty("imstack_invrimgdisp_channelb", cmbChannelB->currentIndex(), false, false);
    cmbChannelR->clear();
    cmbChannelG->clear();
    cmbChannelB->clear();
    if (mv) {
        for (int c=0; c<mv->getImageStackChannels(idx); c++) {
            cmbChannelR->addItem(mv->getImageStackChannelName(idx, c));
            cmbChannelG->addItem(mv->getImageStackChannelName(idx, c));
            cmbChannelB->addItem(mv->getImageStackChannelName(idx, c));
        }
        cmbChannelR->addItem(tr("--- none ---"));
        cmbChannelG->addItem(tr("--- none ---"));
        cmbChannelB->addItem(tr("--- none ---"));

        int frames=mv->getImageStackFrames(idx);
        player->setRange(0, frames-1);

    }
    cmbChannelR->setCurrentIndex(current->getProperty("imstack_invrimgdisp_channelr", 0).toInt());
    cmbChannelG->setCurrentIndex(current->getProperty("imstack_invrimgdisp_channelg", 1).toInt());
    cmbChannelB->setCurrentIndex(current->getProperty("imstack_invrimgdisp_channelb", 2).toInt());

    connect(cmbChannelR, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    connect(cmbChannelG, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    connect(cmbChannelB, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    displayImage();
    pltImage->zoomToFit();
    pltImage->update_plot();
}

void QFRDRImageStackDataEditor::channelModeChanged() {
    QFRDRImageStackData* mv=qobject_cast<QFRDRImageStackData*>(current);
    int idx=cmbImageStack->currentIndex();
    disconnect(cmbChannelR, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    if (cmbChannelMode->currentIndex()>=0) current->setQFProperty("imstack_invrimgdisp_channelmode", cmbChannelMode->currentIndex(), false, false);

    cmbChannelR->setVisible(true);
    cmbChannelG->setVisible(cmbChannelMode->currentIndex()==1);
    cmbChannelB->setVisible(cmbChannelMode->currentIndex()==1);

    connect(cmbChannelR, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    displayImage();
}



