#include "qfrdrimagingfcsoverviewimagedisplay.h"
#include "qfrdrimagingfcs_data.h"

QFRDRImagingFCSOverviewImageDisplay::QFRDRImagingFCSOverviewImageDisplay(QWidget *parent) :
    QWidget(parent)
{
    current=NULL;
    histogram=NULL;
    createWidgets();
}

QFRDRImagingFCSOverviewImageDisplay::~QFRDRImagingFCSOverviewImageDisplay() {
    clearOverlays();
}

void QFRDRImagingFCSOverviewImageDisplay::connectWidgets(QFRawDataRecord *current, QFRawDataRecord *old) {
    disconnect(cmbImage, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    disconnect(player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));

    if (old) {
        this->current=NULL;
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    QFRDROverviewImageInterface* m=qobject_cast<QFRDROverviewImageInterface*>(current);
    cmbImage->clear();
    this->current=current;
    connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    if (m) {
        for (int i=0; i<m->getPreviewImageCount(); i++) {
            cmbImage->addItem(QIcon(":/imaging_fcs/image.png"), m->getPreviewImageName(i));
        }
    }

    QFRDRImageStackInterface* mv=qobject_cast<QFRDRImageStackInterface*>(current);
    if (mv) {
        for (int i=0; i<mv->getImageStackCount(); i++) {
            cmbImage->addItem(QIcon(":/imaging_fcs/video.png"), mv->getImageStackDescription(i));
        }
    }

    cmbImage->setCurrentIndex(current->getProperty("imfcs_invrimgdisp_image", 0).toInt());
    player->setPosition(current->getProperty("imfcs_invrimgdisp_playpos", 0).toInt());
    connect(cmbImage, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    connect(player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));
    displayImage();

}

void QFRDRImagingFCSOverviewImageDisplay::rawDataChanged()
{
}

void QFRDRImagingFCSOverviewImageDisplay::showFrame(int frame) {
    pltImage->set_doDrawing(false);
    current->setQFProperty("imfcs_invrimgdisp_playpos", player->getPosition(), false, false);

    QFRDROverviewImageInterface* m=qobject_cast<QFRDROverviewImageInterface*>(current);
    QFRDRImageStackInterface* mv=qobject_cast<QFRDRImageStackInterface*>(current);
    if (m && mv && cmbImage->currentIndex()-m->getPreviewImageCount()<mv->getImageStackCount()) {
        int idx=cmbImage->currentIndex()-m->getPreviewImageCount();
        int width=mv->getImageStackWidth(idx);
        int height=mv->getImageStackHeight(idx);
        image->set_data(mv->getImageStack(idx, frame), width, height, JKQTPMathImageBase::DoubleArray);
        emit displayedFrame((double)frame*mv->getImageStackTUnitFactor(idx));


        if (chkHistVideo->isChecked()) showHistograms(mv->getImageStack(idx, frame), width*height);
        histogram->setEnabled(chkHistVideo->isChecked());
    }


    pltImage->set_doDrawing(true);
    pltImage->update_plot();
    player->singleShotTimerStart();
}

void QFRDRImagingFCSOverviewImageDisplay::displayImage() {
    if (!image) return;
    player->pause();
    current->setQFProperty("imfcs_invrimgdisp_image", cmbImage->currentIndex(), false, false);
    QFRDROverviewImageInterface* m=qobject_cast<QFRDROverviewImageInterface*>(current);
    QFRDRImageStackInterface* mv=qobject_cast<QFRDRImageStackInterface*>(current);
    player->setVisible(false);
    chkHistVideo->setVisible(false);
    player->pause();
    pltImage->set_doDrawing(false);
    clearOverlays();
    if (m && cmbImage->currentIndex()<m->getPreviewImageCount()) {
        //qDebug()<<"1:    idx="<<cmbImage->currentIndex();
        int width=m->getPreviewImageWidth(cmbImage->currentIndex());
        int height=m->getPreviewImageHeight(cmbImage->currentIndex());
        //qDebug()<<"2:    w="<<width<<"   h="<<height;
        QList<QFRDROverviewImageInterface::OverviewImageGeoElement> overlayElements=m->getPreviewImageGeoElements(cmbImage->currentIndex());
        //qDebug()<<"3:    data="<<m->getPreviewImage(cmbImage->currentIndex());
        image->set_data(m->getPreviewImage(cmbImage->currentIndex()), width, height, JKQTPMathImageBase::DoubleArray);
        image->set_width(width);
        image->set_height(height);

        histogram->setEnabled(true);
        showHistograms(m->getPreviewImage(cmbImage->currentIndex()), width*height);

        //qDebug()<<"4";
        QList<QColor> cols;
        cols<<QColor("red")<<QColor("orange")<<QColor("green")<<QColor("deeppink");
        cols<<QColor("dodgerblue")<<QColor("mediumpurple")<<QColor("brown")<<QColor("salmon");

        for (int i=0; i<overlayElements.size(); i++) {
            if (overlayElements[i].type==QFRDROverviewImageInterface::PIGErectangle) {
                JKQTPgeoRectangle* elem=new JKQTPgeoRectangle(pltImage->get_plotter(), overlayElements[i].x, overlayElements[i].y, overlayElements[i].width, overlayElements[i].height, cols[i%8]);
                elem->set_bottomleftrectangle(overlayElements[i].x, overlayElements[i].y, overlayElements[i].width, overlayElements[i].height);
                elem->set_title(overlayElements[i].title);
                overlayGraphs.append(elem);
                pltImage->addGraph(elem);
            }
        }
        //qDebug()<<"5";

        pltImage->get_plotter()->setAbsoluteXY(0,width,0,height);
        pltImage->get_plotter()->set_aspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_axisAspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_maintainAspectRatio(true);
        pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        player->setVisible(false);
        chkHistVideo->setVisible(false);
        labDescription->setText(tr("<b>image size:</b> %1 &times; %2").arg(width).arg(height));
        //qDebug()<<"6";
    } else if (mv && cmbImage->currentIndex()-m->getPreviewImageCount()<mv->getImageStackCount()) {
        int idx=cmbImage->currentIndex()-m->getPreviewImageCount();
        int width=mv->getImageStackWidth(idx);
        double rwidth=(double)width*mv->getImageStackXUnitFactor(idx);
        int height=mv->getImageStackHeight(idx);
        double rheight=(double)height*mv->getImageStackYUnitFactor(idx);
        int frames=mv->getImageStackFrames(idx);
        player->setRange(0, frames-1);
        player->setPosition(0);
        player->setSingleShot(true);
        player->setVisible(true);
        chkHistVideo->setVisible(true);
        image->set_data(mv->getImageStack(idx, 0), width, height, JKQTPMathImageBase::DoubleArray);
        image->set_width(rwidth);
        image->set_height(rheight);

        if (chkHistVideo->isChecked()) showHistograms(mv->getImageStack(idx, 0), width*height);
        histogram->setEnabled(chkHistVideo->isChecked());

        pltImage->get_plotter()->setAbsoluteXY(0,rwidth,0,rheight);
        pltImage->get_plotter()->set_aspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_axisAspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_maintainAspectRatio(true);
        pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        labDescription->setText(tr("<b>image size:</b> %1 &times; %2 pixels = %3 &times; %4 %5&times;%6").arg(width).arg(height).arg(rwidth).arg(rheight).arg(mv->getImageStackXUnitName(idx)).arg(mv->getImageStackYUnitName(idx)));
        //player->play();
    } else  {
        image->set_data(NULL, 0, 0, JKQTPMathImageBase::UInt16Array);
        showHistograms(NULL, 0);
        histogram->setEnabled(false);

        player->setVisible(false);
        chkHistVideo->setVisible(false);
        player->setRange(0,100);
    }
    pltImage->set_doDrawing(true);
    pltImage->zoomToFit();
    pltImage->update_plot();
}

void QFRDRImagingFCSOverviewImageDisplay::showHistograms(double *data, int size)
{
    if (!histogram) return;
    if (!data && size<=0) {
        histogram->clear();
    } else if (histogram->histogramCount()<2) {
        histogram->clear();
        histogram->addHistogram( tr("full"), data, size);
        histogram->addHistogram( tr("selection"), NULL, 0);//m->getPreviewImage(cmbImage->currentIndex()), width*height);
    } else {
        histogram->setHistogram(0, tr("full"), data, size);
        histogram->setHistogram(1, tr("selection"), NULL, 0);//m->getPreviewImage(cmbImage->currentIndex()), width*height);
    }
    histogram->updateHistogram(true);
}


void QFRDRImagingFCSOverviewImageDisplay::calcExpFit() {
}

void QFRDRImagingFCSOverviewImageDisplay::readSettings(QSettings &settings, const QString &prefix) {
    player->setFPS(settings.value(prefix+"player_fps", player->getFPS()).toDouble());
    player->setReplay(settings.value(prefix+"player_replay", player->getReplay()).toBool());
    histogram->readSettings(settings, prefix+"histogram/");
    chkHistVideo->setChecked(settings.value(prefix+"videohistogram", chkHistVideo->isChecked()).toBool());
}

void QFRDRImagingFCSOverviewImageDisplay::writeSettings(QSettings &settings, const QString &prefix) {
    settings.setValue(prefix+"player_fps", player->getFPS());
    settings.setValue(prefix+"player_replay", player->getReplay());
    histogram->writeSettings(settings, prefix+"histogram/");
    settings.setValue(prefix+"videohistogram", chkHistVideo->isChecked());
}

void QFRDRImagingFCSOverviewImageDisplay::mouseMoved(double x, double y) {
    if (image) {
        labValue->setText(tr("Position: (%1, %2)        Value: %3").arg(x).arg(y).arg(image->getValueAt(x,y)));
    } else {
        labValue->setText("");
    }
}

void QFRDRImagingFCSOverviewImageDisplay::createWidgets() {
    QVBoxLayout* majorLay=new QVBoxLayout(this);
    setLayout(majorLay);

    QHBoxLayout* layTop=new QHBoxLayout(this);
    majorLay->addLayout(layTop);
    layTop->addWidget(new QLabel(tr("<b>image/video:</b>")));
    cmbImage=new QComboBox(this);
    layTop->addWidget(cmbImage);
    layTop->addStretch();

    tabMain=new QTabWidget(this);
    majorLay->addWidget(tabMain, 1);
    QWidget* wtabImag=new QWidget(tabMain);
    QVBoxLayout* mainLay=new QVBoxLayout(this);
    wtabImag->setLayout(mainLay);
    toolbar=new QToolBar("", this);
    mainLay->addWidget(toolbar);
    labValue=new QLabel("", this);
    mainLay->addWidget(labValue);

    pltImage=new JKQtPlotter(this);
    pltImage->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    pltImage->set_zoomByDoubleAndRightMouseClick(false);
    pltImage->set_displayMousePosition(false);
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
    connect(pltImage, SIGNAL(plotMouseMove(double,double)), this, SLOT(mouseMoved(double,double)));
    mainLay->addWidget(pltImage, 1);


    histogram=new QFHistogramView(tabMain);
    histogram->setBins(50);
    histogram->setAutorange(true);


    tabMain->addTab(wtabImag, tr("Image/Video"));
    tabMain->addTab(histogram, tr("Histogram"));



    labDescription=new QLabel(this);
    majorLay->addWidget(labDescription);

    player=new QFPLayerControls(this);
    player->setVisible(false);
    majorLay->addWidget(player);

    chkHistVideo=new QCheckBox(tr("show histogram for video"), this);
    chkHistVideo->setVisible(false);
    majorLay->addWidget(chkHistVideo);

    majorLay->addStretch();




    image=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0, 0, JKQTPMathImage::GRAY, pltImage->get_plotter());
    image->get_colorBarRightAxis()->set_labelType(JKQTPCALTdefault);
    image->get_colorBarRightAxis()->set_labelDigits(2);
    image->get_colorBarRightAxis()->set_minTicks(3);
    pltImage->addGraph(image);


    player->setSingleShot(true);

    toolbar->addAction(pltImage->get_plotter()->get_actSavePlot());
    toolbar->addAction(pltImage->get_plotter()->get_actPrint());
    toolbar->addAction(pltImage->get_plotter()->get_actCopyPixelImage());
    toolbar->addSeparator();
    toolbar->addAction(pltImage->get_plotter()->get_actZoomAll());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomIn());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomOut());

}

void QFRDRImagingFCSOverviewImageDisplay::clearOverlays() {
    for (int i=0; i<overlayGraphs.size(); i++) {
        pltImage->deleteGraph(overlayGraphs[i], true);
    }
    overlayGraphs.clear();
}
