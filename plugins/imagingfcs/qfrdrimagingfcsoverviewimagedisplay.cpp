#include "qfrdrimagingfcsoverviewimagedisplay.h"
#include "qfrdrimagingfcs_data.h"
#include "statistics_tools.h"

#define sqr(x) ((x)*(x))

QFRDRImagingFCSOverviewImageDisplay::QFRDRImagingFCSOverviewImageDisplay(QWidget *parent) :
    QWidget(parent)
{
    current=NULL;
    histogram=NULL;
    selected_width=0;
    selected_height=0;
    plteOverviewSelectedData=NULL;
    plteOverviewSize=0;
    createWidgets();
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
    connect(pltImage, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(pltImage, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(pltImage, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));


    mainLay->addWidget(pltImage, 1);


    histogram=new QFHistogramView(tabMain);
    histogram->setBins(50);
    histogram->setAutorange(true);


    tabMain->addTab(wtabImag, tr("Image/Video"));
    tabMain->addTab(histogram, tr("Histogram"));



    labDescription=new QLabel(this);
    majorLay->addWidget(labDescription);
    labImageAvg=new QLabel(this);
    majorLay->addWidget(labImageAvg);

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

    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.3);
    plteSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, pltImage->get_plotter());
    plteSelected->set_rectanglesAsImageOverlay(false);
    pltImage->addGraph(plteSelected);


    player->setSingleShot(true);

    toolbar->addAction(pltImage->get_plotter()->get_actSavePlot());
    toolbar->addAction(pltImage->get_plotter()->get_actPrint());
    toolbar->addAction(pltImage->get_plotter()->get_actCopyPixelImage());
    toolbar->addSeparator();
    toolbar->addAction(pltImage->get_plotter()->get_actZoomAll());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomIn());
    toolbar->addAction(pltImage->get_plotter()->get_actZoomOut());

    ///////////////////////////////////////////////////////////////
    // TOOLBAR & ACTIONS: edit image plots
    ///////////////////////////////////////////////////////////////
    actImagesZoom=new QAction(QIcon(":/imaging_fcs/zoom.png"), tr("zoom"), this);
    actImagesZoom->setToolTip(tr("in this mode the user may zoom into a plot by drawing a rectangle (draging with the left mouse key)\nA click toggles the current selection/mask position."));
    actImagesZoom->setCheckable(true);
    actImagesDrawRectangle=new QAction(QIcon(":/imaging_fcs/draw_rectangle.png"), tr("rectangular selection"), this);
    actImagesDrawRectangle->setToolTip(tr("in this mode the user may draw a rectangle.<br>"
                                 "All pixels inside the rectangle will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawRectangle->setCheckable(true);

    actImagesDrawPoints=new QAction(QIcon(":/imaging_fcs/draw_point.png"), tr("point-wise selection"), this);
    actImagesDrawPoints->setToolTip(tr("in this mode the user may click on single points.<br>"
                                 "to add and remove them to/from the mask/selection. A click<br>"
                                 "will toggle the state of the clicked pixel<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawPoints->setCheckable(true);

    actImagesDrawCircle=new QAction(QIcon(":/imaging_fcs/draw_circle.png"), tr("circular selection"), this);
    actImagesDrawCircle->setToolTip(tr("in this mode the user may draw a circle.<br>"
                                 "All pixels inside the cirle will be selected<br>"
                                 "when the user releases the left mouse key. The point of first<br>"
                                 "click will be the center of the circle. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawCircle->setCheckable(true);
    actImagesDrawEllipse=new QAction(QIcon(":/imaging_fcs/draw_ellipse.png"), tr("elliptical selection"), this);
    actImagesDrawEllipse->setToolTip(tr("in this mode the user may draw a ellipse.<br>"
                                 "All pixels inside the ellipse will be selected<br>"
                                "when the user releases the left mouse key. The point of first<br>"
                                "click will be the center of the ellipse. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawEllipse->setCheckable(true);
    actImagesDrawLine=new QAction(QIcon(":/imaging_fcs/draw_line.png"), tr("line selection"), this);
    actImagesDrawLine->setToolTip(tr("in this mode the user may draw a line.<br>"
                                 "All pixels on the line will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: selection will be added to current selection</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesDrawLine->setCheckable(true);

    actImagesScribble=new QAction(QIcon(":/imaging_fcs/draw_scribble.png"), tr("scribble selection"), this);
    actImagesScribble->setToolTip(tr("in this mode the user may select/deselect pixels by.<br>"
                                 "keeping the left mouse button pressed and moving the mouse<br>"
                                 "over the image. Depending on the key pressed on the keyboard,<br>"
                                 "different actions are executed:<ul>"
                                 "<li>CTRL: the old selection/mask is not deleted, so the result is appended to it.</li>"
                                 "<li>SHIFT: selection will be removed from current selection</li>"
                                 "</ul>"));
    actImagesScribble->setCheckable(true);
    agImageSelectionActions=new QActionGroup(this);
    agImageSelectionActions->setExclusive(true);
    agImageSelectionActions->addAction(actImagesZoom);
    agImageSelectionActions->addAction(actImagesDrawPoints);
    agImageSelectionActions->addAction(actImagesDrawRectangle);
    agImageSelectionActions->addAction(actImagesScribble);
    agImageSelectionActions->addAction(actImagesDrawLine);
    agImageSelectionActions->addAction(actImagesDrawCircle);
    agImageSelectionActions->addAction(actImagesDrawEllipse);
    connect(agImageSelectionActions, SIGNAL(triggered(QAction*)), this, SLOT(setImageEditMode()));

    toolbar->addSeparator();
    toolbar->addAction(actImagesZoom);
    toolbar->addAction(actImagesDrawPoints);
    toolbar->addAction(actImagesDrawRectangle);
    toolbar->addAction(actImagesScribble);
    toolbar->addAction(actImagesDrawLine);
    toolbar->addAction(actImagesDrawCircle);
    toolbar->addAction(actImagesDrawEllipse);
}





void QFRDRImagingFCSOverviewImageDisplay::setImageEditMode() {
    if (actImagesZoom->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
    } else if (actImagesDrawPoints->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::ClickEvents);
    } else if (actImagesDrawRectangle->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::RectangleEvents);
    } else if (actImagesDrawLine->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::LineEvents);
    } else if (actImagesDrawCircle->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::CircleEvents);
    } else if (actImagesDrawEllipse->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::EllipseEvents);
    } else if (actImagesScribble->isChecked()) {
        pltImage->set_mouseActionMode(JKQtPlotter::ScribbleEvents);
    }
}



void QFRDRImagingFCSOverviewImageDisplay::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int width=image->get_Nx();
    int height=image->get_Ny();
    if (selected_width!=width || selected_height!=height) {
        selected.clear();
        selected_width=width;
        selected_height=height;
    }

    //qDebug()<<"imageClicked("<<x<<y<<modifiers<<")";

    int idx=yy*width+xx;
    if (xx>=0 && xx<width && yy>=0 && yy<height) {
        if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
            if (selected.contains(idx)) selected.remove(idx);
            else selected.insert(idx);
        } else if (modifiers==Qt::ShiftModifier) {
            selected.remove(idx);
        } else {
            if (!actImagesScribble->isChecked()) selected.clear();
            selected.insert(idx);
        }
        replotSelection(true);
    }
}

void QFRDRImagingFCSOverviewImageDisplay::imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool /*last*/) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    int xx=(int)floor(x);
    int yy=(int)floor(y);
    int width=image->get_Nx();
    int height=image->get_Ny();
    if (selected_width!=width || selected_height!=height) {
        selected.clear();
        selected_width=width;
        selected_height=height;
    }

    int idx=yy*width+xx;
    if (xx>=0 && xx<width && yy>=0 && yy<height) {

        if (first && modifiers==Qt::NoModifier) selected.clear();
        if (modifiers==Qt::ShiftModifier) {
            selected.remove(idx);
        } else {
            selected.insert(idx);
        }
        replotSelection(true);
    }
}


void QFRDRImagingFCSOverviewImageDisplay::imageRectangleFinished(double x, double y, double rwidth, double rheight, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    int width=image->get_Nx();
    int height=image->get_Ny();
    if (selected_width!=width || selected_height!=height) {
        selected.clear();
        selected_width=width;
        selected_height=height;
    }

    int xx1=qBound(0,(int)floor(x), width-1);
    int yy1=qBound(0,(int)floor(y), height-1);
    int xx2=qBound(0,(int)floor(x+rwidth), width-1);
    int yy2=qBound(0,(int)floor(y+rheight), height-1);

    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    //qDebug()<<"rect: "<<xx1<<yy1<<xx2<<yy2;
    //qDebug()<<selected;

    if (modifiers==Qt::ControlModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                int idx=yy*width+xx;
                selected.insert(idx);
            }
        }
    } else if (modifiers==Qt::ShiftModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                int idx=yy*width+xx;
                selected.remove(idx);
            }
        }
    } else {
        selected.clear();
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                int idx=yy*width+xx;
                selected.insert(idx);
            }
        }
    }
    //qDebug()<<selected;
    replotSelection(true);
}


void QFRDRImagingFCSOverviewImageDisplay::imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    int width=image->get_Nx();
    int height=image->get_Ny();
    if (selected_width!=width || selected_height!=height) {
        selected.clear();
        selected_width=width;
        selected_height=height;
    }


    int xx1=qBound(0,(int)floor(x-radiusX), width-1);
    int yy1=qBound(0,(int)floor(y-radiusY), height-1);
    int xx2=qBound(0,(int)floor(x+radiusX), width-1);
    int yy2=qBound(0,(int)floor(y+radiusY), height-1);

    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    //qDebug()<<"rect: "<<xx1<<yy1<<xx2<<yy2;
    //qDebug()<<selected;

    if (modifiers==Qt::ControlModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) {
                    int idx=yy*width+xx;
                    selected.insert(idx);
                }
            }
        }
    } else if (modifiers==Qt::ShiftModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) {
                    int idx=yy*width+xx;
                    selected.remove(idx);
                }
            }
        }
    } else {
        selected.clear();
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) {
                    int idx=yy*width+xx;
                    selected.insert(idx);
                }
            }
        }
    }
    //qDebug()<<selected;
    replotSelection(true);
}

void QFRDRImagingFCSOverviewImageDisplay::imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    int width=image->get_Nx();
    int height=image->get_Ny();
    if (selected_width!=width || selected_height!=height) {
        selected.clear();
        selected_width=width;
        selected_height=height;
    }


    int xx1=qBound(0,(int)floor(x-radius), width-1);
    int yy1=qBound(0,(int)floor(y-radius), height-1);
    int xx2=qBound(0,(int)floor(x+radius), width-1);
    int yy2=qBound(0,(int)floor(y+radius), height-1);


    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    //qDebug()<<"rect: "<<xx1<<yy1<<xx2<<yy2;
    //qDebug()<<selected;

    if (modifiers==Qt::ControlModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) {
                    int idx=yy*width+xx;
                    selected.insert(idx);
                }
            }
        }
    } else if (modifiers==Qt::ShiftModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) {
                    int idx=yy*width+xx;
                    selected.remove(idx);
                }
            }
        }
    } else {
        selected.clear();
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) {
                    int idx=yy*width+xx;
                    selected.insert(idx);
                }
            }
        }
    }
    replotSelection(true);
}

void QFRDRImagingFCSOverviewImageDisplay::imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    QLineF line(x1, y1, x2, y2);
    int width=image->get_Nx();
    int height=image->get_Ny();
    if (selected_width!=width || selected_height!=height) {
        selected.clear();
        selected_width=width;
        selected_height=height;
    }

    if (modifiers==Qt::ControlModifier) {
        for (double i=0; i<1.0; i=i+0.5/double(qMax(width, height))) {
            QPointF p=line.pointAt(i);
            int xx=qBound(0,(int)floor(p.x()), width-1);
            int yy=qBound(0,(int)floor(p.y()), height-1);
            int idx=yy*width+xx;
            selected.insert(idx);
        }
    } else if (modifiers==Qt::ShiftModifier) {
    for (double i=0; i<1.0; i=i+0.5/double(qMax(width, height))) {
            QPointF p=line.pointAt(i);
            int xx=qBound(0,(int)floor(p.x()), width-1);
            int yy=qBound(0,(int)floor(p.y()), height-1);
            int idx=yy*width+xx;
            selected.remove(idx);
        }
    } else {
        selected.clear();
        for (double i=0; i<1.0; i=i+0.5/double(qMax(width, height))) {
            QPointF p=line.pointAt(i);
            int xx=qBound(0,(int)floor(p.x()), width-1);
            int yy=qBound(0,(int)floor(p.y()), height-1);
            int idx=yy*width+xx;
            selected.insert(idx);
        }
    }
    replotSelection(true);
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

        if (selected_width!=width || selected_height!=height) {
            selected.clear();
            selected_width=width;
            selected_height=height;
            replotSelection(false);
        }

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

        if (selected_width!=width || selected_height!=height) {
            selected.clear();
            selected_width=width;
            selected_height=height;
            replotSelection(false);
        }

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


    int histCount=1;
    double* hs=NULL;
    if (selected.size()>0) {
        histCount++;
        hs=(double*)calloc(selected.size(),sizeof(double));
        QSetIterator<int32_t> i(selected);
        int c=0;
        while (i.hasNext()) {
             int idx=i.next();
             if (idx>0 && idx<size) {
                 hs[c]=data[idx];
             }
             c++;
        }
    }
    if (!data && size<=0) {
        histogram->clear();
    } else if (histogram->histogramCount()<histCount) {
        histogram->clear();
        double* d=(double*)calloc(size,sizeof(double));
        memcpy(d, data, sizeof(double)*size);
        histogram->addHistogram( tr("full"), d, size);
        if (selected.size()>0) histogram->addHistogram( tr("selection"), hs, selected.size());
    } else {
        double* d=(double*)calloc(size,sizeof(double));
        memcpy(d, data, sizeof(double)*size);
        histogram->setHistogram(0, tr("full"), d, size);
        if (selected.size()>0) histogram->setHistogram(1, tr("selection"), hs, selected.size());
    }
    histogram->updateHistogram(true);
}


void QFRDRImagingFCSOverviewImageDisplay::updateSelectionArrays() {
    double* d=(double*)image->get_data();
    int Nx=image->get_Nx();
    int Ny=image->get_Ny();
    if (d && Nx>0 && Ny>0) {
        if (selected_width!=Nx || selected_height!=Ny) {
            selected.clear();
            selected_width=Nx;
            selected_height=Ny;
        }
        int siz=selected_width*selected_height;
        if (plteOverviewSize<siz) {
            plteOverviewSize=siz;
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSize*sizeof(bool));
        }
        for (register int i=0; i<siz; i++) {
            plteOverviewSelectedData[i]=selected.contains(i);
        }
    } else {
        for (register int i=0; i<plteOverviewSize; i++) {
            plteOverviewSelectedData[i]=false;
        }
    }
}

void QFRDRImagingFCSOverviewImageDisplay::replotSelection(bool replot) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);

    updateSelectionArrays();


    if (!m) {
        plteSelected->set_data(NULL, 1, 1);
        labImageAvg->clear();
    } else {
        //uint16_t* ov=m->getDataImagePreview();
        double w=selected_width;
        double h=selected_height;
        if ((w==0) || (h==0)) {
            w=h=1;
        }

        double ovrAvg=0;
        double ovrVar=0;
        ovrAvg=statisticsAverageVarianceMasked(ovrVar, plteOverviewSelectedData, (double*)image->get_data(), qMin(plteOverviewSize, image->get_Nx()*image->get_Ny()));

        plteSelected->set_width(w);
        plteSelected->set_height(h);
        plteSelected->set_data(plteOverviewSelectedData, selected_width, selected_height);

        labImageAvg->setTextFormat(Qt::RichText);
        labImageAvg->setText(tr("avg&plusmn;SD(param img) = %1 &plusmn; %2").arg(ovrAvg).arg(sqrt(ovrVar)));


    }

    if (replot) displayImage();
    QApplication::restoreOverrideCursor();
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


void QFRDRImagingFCSOverviewImageDisplay::clearOverlays() {
    for (int i=0; i<overlayGraphs.size(); i++) {
        pltImage->deleteGraph(overlayGraphs[i], true);
    }
    overlayGraphs.clear();
}
