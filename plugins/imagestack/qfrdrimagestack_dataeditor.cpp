#include "qfrdrimagestack_dataeditor.h"
#include "qfrdrimagestack_data.h"

QFRDRImageStackDataEditor::QFRDRImageStackDataEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    createWidgets();
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

    toolbar=new QToolBar("", this);
    mainLay->addWidget(toolbar);

    pltImage=new JKQtPlotter(this);
    pltImage->set_zoomByDoubleAndRightMouseClick(false);
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

    QHBoxLayout* layImage=new QHBoxLayout(this);
    layImage->setContentsMargins(0,0,0,0);
    layImage->addWidget(pltImage, 1);
    mainLay->addLayout(layImage, 1);



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

    player->setSingleShot(true);

    toolbar->addAction(pltImage->get_plotter()->get_actSavePlot());
    toolbar->addAction(pltImage->get_plotter()->get_actPrint());
    toolbar->addAction(pltImage->get_plotter()->get_actCopyPixelImage());
    toolbar->addSeparator();
    toolbar->addAction(pltImage->get_plotter()->get_actZoomAll());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomIn());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomOut());

};

void QFRDRImageStackDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    disconnect(cmbImageStack, SIGNAL(currentIndexChanged(int)), this, SLOT(stackChanged()));
    disconnect(player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));
    disconnect(cmbChannelMode, SIGNAL(currentIndexChanged(int)), this, SLOT(channelModeChanged()));

    if (old) {
        this->current=NULL;
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
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

    player->setPosition(current->getProperty("imstack_invrimgdisp_playpos", 0).toInt());
    connect(cmbImageStack, SIGNAL(currentIndexChanged(int)), this, SLOT(stackChanged()));
    connect(player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));
    connect(cmbChannelMode, SIGNAL(currentIndexChanged(int)), this, SLOT(channelModeChanged()));
    channelModeChanged();
    stackChanged();
};

void QFRDRImageStackDataEditor::rawDataChanged() {
	// emitted when the raw data changes 
	QFRDRImageStackData* m=qobject_cast<QFRDRImageStackData*>(current);
    //labTest->setText(tr("loaded record with files<br><i>%1</i>").arg(m->getFiles().join("<br>")));
};

void QFRDRImageStackDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    QString prefix="image_stack/";
    player->setFPS(settings->getQSettings()->value(prefix+"player_fps", player->getFPS()).toDouble());
    player->setReplay(settings->getQSettings()->value(prefix+"player_replay", player->getReplay()).toBool());
};


void QFRDRImageStackDataEditor::writeSettings() {
	// rwrite widget settings
    if (!settings) return;
    QString prefix="image_stack/";
    settings->getQSettings()->setValue(prefix+"player_fps", player->getFPS());
    settings->getQSettings()->setValue(prefix+"player_replay", player->getReplay());
};


void QFRDRImageStackDataEditor::showFrame(int frame, bool startPlayer) {
    bool dd=pltImage->get_doDrawing();
    pltImage->set_doDrawing(false);
    current->setQFProperty("imstack_invrimgdisp_playpos", player->getPosition(), false, false);

    QFRDRImageStackData* mv=qobject_cast<QFRDRImageStackData*>(current);
    if (mv && cmbImageStack->currentIndex()<mv->getImageStackCount()) {
        int idx=cmbImageStack->currentIndex();
        int width=mv->getImageStackWidth(idx);
        int height=mv->getImageStackHeight(idx);
        int channel=cmbChannelR->currentIndex();
        int channelG=cmbChannelG->currentIndex();
        int channelB=cmbChannelB->currentIndex();
        pltImage->clearGraphs(false);
        if (cmbChannelMode->currentIndex()==0) {
            double* ir=NULL;

            if (channel>=0 && channel<mv->getImageStackChannels(idx)) ir=mv->getImageStack(idx, frame, channel);
            image->set_width(width);
            image->set_height(height);
            //qDebug()<<"frame="<<frame<<"   "<<ir;

            image->set_data(ir, width, height, JKQTPMathImageBase::DoubleArray);
            pltImage->addGraph(image);
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
            pltImage->addGraph(imageRGB);
        }
        emit displayedFrame((double)frame*mv->getImageStackTUnitFactor(idx));
        labFrame->setText(tr("<b>frame:</b> %1/%2 (%3 %4)").arg(frame+1).arg(mv->getImageStackFrames(idx)).arg(frame*mv->getImageStackTUnitFactor(idx)).arg(mv->getImageStackTUnitName(idx)));
    }


    pltImage->set_doDrawing(dd);
    if (dd) {
        pltImage->update_plot();
        if (startPlayer) player->singleShotTimerStart();
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
    pltImage->zoomToFit();
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
    cmbChannelG->setCurrentIndex(current->getProperty("imstack_invrimgdisp_channelg", cmbChannelG->count()-1).toInt());
    cmbChannelB->setCurrentIndex(current->getProperty("imstack_invrimgdisp_channelb", cmbChannelG->count()-1).toInt());

    connect(cmbChannelR, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    connect(cmbChannelG, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    connect(cmbChannelB, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    displayImage();
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
