/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qfrdroverviewimagedisplay.h"
#include "statistics_tools.h"
#include "qfrdroverviewimageinterface.h"
#include "qfrdrimagestackinterface.h"

#define sqr(x) ((x)*(x))

QFRDROverviewImageDisplay::QFRDROverviewImageDisplay(QWidget *parent) :
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

void QFRDROverviewImageDisplay::createWidgets() {
    QVBoxLayout* majorLay=new QVBoxLayout();
    setLayout(majorLay);

    QHBoxLayout* layTop=new QHBoxLayout();
    majorLay->addLayout(layTop);
    layTop->addWidget(new QLabel(tr("<b>image/video:</b>")));
    cmbImage=new QComboBox(this);
    layTop->addWidget(cmbImage);
    layTop->addStretch();

    tabMain=new QTabWidget(this);
    majorLay->addWidget(tabMain, 1);
    QWidget* wtabImag=new QWidget(tabMain);
    QVBoxLayout* mainLay=new QVBoxLayout();
    wtabImag->setLayout(mainLay);
    toolbar=new QToolBar("", this);
    mainLay->addWidget(toolbar);
    labValue=new QLabel("", this);
    mainLay->addWidget(labValue);

    pltImage=new QFPlotter(this);
    pltImage->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
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
    labStackPosition=new QLabel("", this);
    mainLay->addWidget(labStackPosition);
    labStackPosition->setAlignment(Qt::AlignRight);


    histogram=new QFHistogramView(tabMain);
    histogram->setHistogramXLabel(tr("intensity [A.U.]"));
    histogram->setBins(50);
    histogram->setAutorange(true);


    tabMain->addTab(wtabImag, tr("Image/Video"));
    tabMain->addTab(histogram, tr("Histogram"));



    labDescription=new QLabel(this);
    majorLay->addWidget(labDescription);
    labImageAvg=new QLabel(this);
    majorLay->addWidget(labImageAvg);

    chkRelaxedColorScale=new QCheckBox(tr("relaxed color scaling ( 5%/95% quantiles, not min/max)"), this);
    chkRelaxedColorScale->setVisible(true);
    connect(chkRelaxedColorScale, SIGNAL(toggled(bool)), this, SLOT(showFullScreen()));
    majorLay->addWidget(chkRelaxedColorScale);


    player=new QFPLayerControls(this);
    player->setVisible(false);
    majorLay->addWidget(player);

    chkHistVideo=new QCheckBox(tr("show histogram for video"), this);
    chkHistVideo->setVisible(false);
    majorLay->addWidget(chkHistVideo);

    majorLay->addStretch();




    image=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0, 0, JKQTPMathImageGRAY, pltImage->get_plotter());
    image->get_colorBarRightAxis()->set_labelType(JKQTPCALTdefault);
    image->get_colorBarRightAxis()->set_labelDigits(2);
    image->get_colorBarRightAxis()->set_minTicks(3);
    pltImage->addGraph(image);
    imageRGB=new JKQTPRGBMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0, 0, pltImage->get_plotter());
    imageRGB->get_colorBarRightAxis()->set_labelType(JKQTPCALTdefault);
    imageRGB->get_colorBarRightAxis()->set_labelDigits(2);
    imageRGB->get_colorBarRightAxis()->set_minTicks(3);
    imageRGB->set_visible(false);
    pltImage->addGraph(imageRGB);

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





void QFRDROverviewImageDisplay::setImageEditMode() {
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



void QFRDROverviewImageDisplay::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    //QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //if (!m) return;
    if (!current) return;
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

void QFRDROverviewImageDisplay::imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool /*last*/) {
    //QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //if (!m) return;
    if (!current) return;
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


void QFRDROverviewImageDisplay::imageRectangleFinished(double x, double y, double rwidth, double rheight, Qt::KeyboardModifiers modifiers) {
    //QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //if (!m) return;
    if (!current) return;
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
        //qDebug()<<"ctrl";
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                int idx=yy*width+xx;
                selected.insert(idx);
            }
        }
    } else if (modifiers==Qt::ShiftModifier) {
        //qDebug()<<"shift";
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                int idx=yy*width+xx;
                selected.remove(idx);
            }
        }
    } else {
        //qDebug()<<"other";
        selected.clear();
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                int idx=yy*width+xx;
                selected.insert(idx);
            }
        }
    }

    //qDebug()<<"imageRectangleFinished():  "<<selected_width<<selected_height<<width<<height;
    //qDebug()<<selected;
    replotSelection(true);
}


void QFRDROverviewImageDisplay::imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers) {
    //QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //if (!m) return;
    if (!current) return;

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

void QFRDROverviewImageDisplay::imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers) {
    //QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //if (!m) return;
    if (!current) return;

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

void QFRDROverviewImageDisplay::imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers) {
    //QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    //if (!m) return;
    if (!current) return;

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































QFRDROverviewImageDisplay::~QFRDROverviewImageDisplay() {
    clearOverlays();
}

void QFRDROverviewImageDisplay::connectWidgets(QFRawDataRecord *current, QFRawDataRecord *old) {
    disconnect(cmbImage, SIGNAL(currentIndexChanged(int)), this, SLOT(displayImage()));
    disconnect(player, SIGNAL(showFrame(int)), this, SLOT(showFrame(int)));

    if (old) {
        this->current=NULL;
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    QFRDRAdditionalImagesInterface* m=qobject_cast<QFRDRAdditionalImagesInterface*>(current);
    cmbImage->clear();
    this->current=current;
    if (m&&current) {
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        if (m) {
            for (int i=0; i<m->getAdditionalImagesCount(); i++) {
                cmbImage->addItem(QIcon(":/imaging_fcs/image.png"), m->getAdditionalImagesName(i));
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
    }
    displayImage();

}

void QFRDROverviewImageDisplay::rawDataChanged()
{
}

void QFRDROverviewImageDisplay::showFrame(int frame) {
    tabMain->setTabEnabled(1,false);
    pltImage->set_doDrawing(false);
    current->setQFProperty("imfcs_invrimgdisp_playpos", player->getPosition(), false, false);
    pltImage->getDatastore()->clear();

    QFRDRAdditionalImagesInterface* m=qobject_cast<QFRDRAdditionalImagesInterface*>(current);
    QFRDRImageStackInterface* mv=qobject_cast<QFRDRImageStackInterface*>(current);
    if (m && mv && cmbImage->currentIndex()-m->getAdditionalImagesCount()>=0 && cmbImage->currentIndex()-m->getAdditionalImagesCount()<mv->getImageStackCount()) {
        int idx=cmbImage->currentIndex()-m->getAdditionalImagesCount();
        int width=mv->getImageStackWidth(idx);
        int height=mv->getImageStackHeight(idx);
        double* data=mv->getImageStack(idx, frame, 0);
        image->set_data(data, width, height, JKQTPMathImageBase::DoubleArray);
        image->set_visible(true);
        imageRGB->set_visible(false);
        if (mv->getImageStackChannels(idx)<=1) {
            imageRGB->set_visible(false);
            image->set_visible(true);
        } else {
            imageRGB->set_data(mv->getImageStack(idx, frame, 1), mv->getImageStack(idx, frame, 0), mv->getImageStack(idx, frame, 2), width, height, JKQTPMathImageBase::DoubleArray);
            imageRGB->set_visible(true);
            image->set_visible(false);
        }

        if (chkRelaxedColorScale->isChecked()) {
            if (mv->getImageStackChannels(idx)>1) {
                double* datas=duplicateArray(mv->getImageStack(idx, frame, 1), width*height);
                statisticsSort(datas, width*height);
                double qmi=statisticsSortedQuantile(datas, width*height, 0.05);
                double qma=statisticsSortedQuantile(datas, width*height, 0.95);
                imageRGB->set_autoImageRange(false);
                imageRGB->set_imageMin(qmi);
                imageRGB->set_imageMax(qma);
                qfFree(datas);

                datas=duplicateArray(mv->getImageStack(idx, frame, 0), width*height);
                statisticsSort(datas, width*height);
                qmi=statisticsSortedQuantile(datas, width*height, 0.05);
                qma=statisticsSortedQuantile(datas, width*height, 0.95);
                imageRGB->set_autoImageRange(false);
                imageRGB->set_imageMinG(qmi);
                imageRGB->set_imageMaxG(qma);
                qfFree(datas);

                datas=duplicateArray(mv->getImageStack(idx, frame, 2), width*height);
                statisticsSort(datas, width*height);
                qmi=statisticsSortedQuantile(datas, width*height, 0.05);
                qma=statisticsSortedQuantile(datas, width*height, 0.95);
                imageRGB->set_autoImageRange(false);
                imageRGB->set_imageMinB(qmi);
                imageRGB->set_imageMaxB(qma);
                qfFree(datas);

            } else {
                double* datas=duplicateArray(data, width*height);
                statisticsSort(datas, width*height);
                double qmi=statisticsSortedQuantile(datas, width*height, 0.05);
                double qma=statisticsSortedQuantile(datas, width*height, 0.95);
                image->set_autoImageRange(false);
                image->set_imageMin(qmi);
                image->set_imageMax(qma);
                qfFree(datas);
            }
        } else {
            image->set_autoImageRange(true);
            imageRGB->set_autoImageRange(true);
        }

        emit displayedFrame((double)frame*mv->getImageStackTUnitFactor(idx));

        if (selected_width!=width || selected_height!=height) {
            selected.clear();
            selected_width=width;
            selected_height=height;
            replotSelection(false);
        }

        int scnt=0;
        for (int ii=0; ii<selected_width*selected_height; ii++)  {
            if (plteOverviewSelectedData[ii]) scnt++;
        }
        //qDebug()<<idx<<":   "<<mv->getImageStack(idx, frame)<<width<<height<<"   "<<plteOverviewSelectedData<<selected_width<<selected_height<<"    "<<pltImage->getGraphCount()<<plteSelected->get_visible()<<scnt;

        pltImage->getDatastore()->addCopiedColumn(mv->getImageStack(idx, frame), width*height, mv->getImageStackDescription(idx));
        pltImage->getDatastore()->addCopiedColumn(plteOverviewSelectedData, selected_width*selected_height, tr("mask"));

        if (chkHistVideo->isChecked()) showHistograms(mv->getImageStack(idx, frame), width*height);
        histogram->setEnabled(chkHistVideo->isChecked());
        histogram->setHistogramXLabel(tr("intensity [A.U. or kHz]"));

        labStackPosition->setText(mv->getImageStackTimepointName(idx, frame));
    }


    pltImage->set_doDrawing(true);
    pltImage->update_plot();
    player->singleShotTimerStart();
}

void QFRDROverviewImageDisplay::displayImage() {
    tabMain->setTabEnabled(1,true);
    if (!image) return;
    if (!current) return;
    player->pause();
    current->setQFProperty("imfcs_invrimgdisp_image", cmbImage->currentIndex(), false, false);
    QFRDRAdditionalImagesInterface* m=qobject_cast<QFRDRAdditionalImagesInterface*>(current);
    QFRDRImageStackInterface* mv=qobject_cast<QFRDRImageStackInterface*>(current);
    player->setVisible(false);
    chkHistVideo->setVisible(false);
    player->pause();
    pltImage->set_doDrawing(false);
    clearOverlays();
    updateSelectionArrays();
    if (m && cmbImage->currentIndex()<m->getAdditionalImagesCount()) {
        QString images="";
        for (int jj=0; jj<cmbImage->currentIndex(); jj++) {
            images+=cmbImage->itemText(jj)+" / ";
        }
        //qDebug()<<"1:    idx="<<cmbImage->currentIndex()<<images;
        int width=m->getAdditionalImagesWidth(cmbImage->currentIndex());
        int height=m->getAdditionalImagesHeight(cmbImage->currentIndex());
        //qDebug()<<"2:    w="<<width<<"   h="<<height;
        QList<QFRDRAdditionalImagesInterface::AdditionalImagesGeoElement> overlayElements=m->getAdditionalImagesAnnotations(cmbImage->currentIndex());
        //qDebug()<<"3:    data="<<m->getOverviewImage(cmbImage->currentIndex());
        if (m->getAdditionalImage(cmbImage->currentIndex())) {
            image->set_data(m->getAdditionalImage(cmbImage->currentIndex()), width, height, JKQTPMathImageBase::DoubleArray);
            image->set_width(width);
            image->set_height(height);
            image->set_visible(true);
            imageRGB->set_visible(false);

            if (selected_width!=width || selected_height!=height) {
                selected.clear();
                selected_width=width;
                selected_height=height;
                replotSelection(false);
            }

            pltImage->getDatastore()->addCopiedColumn(m->getAdditionalImage(cmbImage->currentIndex()), width*height, cmbImage->currentText());
            pltImage->getDatastore()->addCopiedColumn(plteOverviewSelectedData, selected_width*selected_height, tr("mask"));

            histogram->setEnabled(true);
            showHistograms(m->getAdditionalImage(cmbImage->currentIndex()), width*height);

            //qDebug()<<"4";
            QList<QColor> cols;
            cols<<QColor("red")<<QColor("orange")<<QColor("green")<<QColor("deeppink");
            cols<<QColor("dodgerblue")<<QColor("mediumpurple")<<QColor("brown")<<QColor("salmon");

            for (int i=0; i<overlayElements.size(); i++) {
                if (overlayElements[i].type==QFRDRAdditionalImagesInterface::PIGErectangle) {
                    JKQTPgeoRectangle* elem=new JKQTPgeoRectangle(pltImage->get_plotter(), overlayElements[i].x, overlayElements[i].y, overlayElements[i].width, overlayElements[i].height, cols[i%8]);
                    elem->set_bottomleftrectangle(overlayElements[i].x, overlayElements[i].y, overlayElements[i].width, overlayElements[i].height);
                    elem->set_title(overlayElements[i].title);
                    overlayGraphs.append(elem);
                    pltImage->addGraph(elem);
                }
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
        labStackPosition->setText("");
        //qDebug()<<"6";
    } else if (mv && cmbImage->currentIndex()-m->getAdditionalImagesCount()<mv->getImageStackCount()) {
        int idx=cmbImage->currentIndex()-m->getAdditionalImagesCount();
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
        image->set_data(mv->getImageStack(idx, 0, 0), width, height, JKQTPMathImageBase::DoubleArray);
        image->set_width(rwidth);
        image->set_height(rheight);
        image->set_visible(true);
        imageRGB->set_visible(false);
        if (mv->getImageStackChannels(idx)>1) {
            imageRGB->set_data(mv->getImageStack(idx, 0, 1), mv->getImageStack(idx, 0, 0), mv->getImageStack(idx, 0, 3), width, height, JKQTPMathImageBase::DoubleArray);
            imageRGB->set_width(rwidth);
            imageRGB->set_height(rheight);
            image->set_visible(false);
            imageRGB->set_visible(true);
        }

        if (selected_width!=width || selected_height!=height) {
            selected.clear();
            selected_width=width;
            selected_height=height;
            replotSelection(false);
        }

        pltImage->getDatastore()->addCopiedColumn(mv->getImageStack(idx, 0), width*height, cmbImage->currentText());
        pltImage->getDatastore()->addCopiedColumn(plteOverviewSelectedData, selected_width*selected_height, tr("mask"));

        histogram->setEnabled(chkHistVideo->isChecked());
        if (chkHistVideo->isChecked()) showHistograms(mv->getImageStack(idx, 0), width*height);

        pltImage->get_plotter()->setAbsoluteXY(0,rwidth,0,rheight);
        pltImage->get_plotter()->set_aspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_axisAspectRatio(double(width)/double(height));
        pltImage->get_plotter()->set_maintainAspectRatio(true);
        pltImage->get_plotter()->set_maintainAxisAspectRatio(true);
        labDescription->setText(tr("<b>image size:</b> %1 &times; %2 pixels = %3 &times; %4 %5&times;%6").arg(width).arg(height).arg(rwidth).arg(rheight).arg(mv->getImageStackXUnitName(idx)).arg(mv->getImageStackYUnitName(idx)));
        //player->play();
    } else  {
        image->set_data(NULL, 0, 0, JKQTPMathImageBase::UInt16Array);
        image->set_visible(true);
        imageRGB->set_visible(false);
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

void QFRDROverviewImageDisplay::showHistograms(double *data, int size)
{
    if (!histogram) return;


    int histCount=1;
    double* hs=NULL;
    if (selected.size()>0) {
        histCount++;
        hs=(double*)qfCalloc(selected.size(),sizeof(double));
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
        //double* d=(double*)qfCalloc(size,sizeof(double));
        //memcpy(d, data, sizeof(double)*size);
        histogram->addHistogram( tr("full"), data, size, false);
        if (selected.size()>0) histogram->addHistogram( tr("selection"), hs, selected.size(), false);
    } else {
        //double* d=(double*)qfCalloc(size,sizeof(double));
        //memcpy(d, data, sizeof(double)*size);
        histogram->setHistogram(0, tr("full"), data, size, false);
        if (selected.size()>0) histogram->setHistogram(1, tr("selection"), hs, selected.size(), false);
    }
    histogram->updateHistogram(true);
    if (hs) qfFree(hs);
}


void QFRDROverviewImageDisplay::updateSelectionArrays() {
    double* d=(double*)image->get_data();
    int Nx=image->get_Nx();
    int Ny=image->get_Ny();
    //qDebug()<<"updateSelectionArrays():   "<<d<<Nx<<Ny<<selected_width<<selected_height;
    if (d && Nx>0 && Ny>0) {
        if (selected_width!=Nx || selected_height!=Ny) {
            selected.clear();
            selected_width=Nx;
            selected_height=Ny;
        }
        int siz=selected_width*selected_height;
        if (plteOverviewSize!=siz) {
            plteOverviewSize=siz;
            plteOverviewSelectedData=(bool*)qfRealloc(plteOverviewSelectedData, plteOverviewSize*sizeof(bool));
        }
        for (register int i=0; i<siz; i++) {
            plteOverviewSelectedData[i]=selected.contains(i);
        }
    } else {
        for (register int i=0; i<plteOverviewSize; i++) {
            plteOverviewSelectedData[i]=false;
        }
    }
    //qDebug()<<"updateSelectionArrays():   "<<d<<Nx<<Ny<<plteOverviewSelectedData<<selected_width<<selected_height;
}

void QFRDROverviewImageDisplay::replotSelection(bool replot) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRAdditionalImagesInterface* m=qobject_cast<QFRDRAdditionalImagesInterface*>(current);
    QFRDRImageStackInterface* mv=qobject_cast<QFRDRImageStackInterface*>(current);

    updateSelectionArrays();


    if (!current) {
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
        ovrAvg=statisticsAverageVarianceMasked(ovrVar, plteOverviewSelectedData, (double*)image->get_data(), qMin(plteOverviewSize, (int32_t)(image->get_Nx()*image->get_Ny())));

        plteSelected->set_width(w);
        plteSelected->set_height(h);
        plteSelected->set_data(plteOverviewSelectedData, selected_width, selected_height);

        //qDebug()<<"replotSelection("<<replot<<"):   "<<plteOverviewSelectedData<<selected_width<<selected_height;

        labImageAvg->setTextFormat(Qt::RichText);
        labImageAvg->setText(tr("avg&plusmn;SD(param img) = %1 &plusmn; %2").arg(ovrAvg).arg(sqrt(ovrVar)));


    }

    if (replot) {
        if (cmbImage->currentIndex()<m->getAdditionalImagesCount()) displayImage();
        else showFrame(player->getPosition());
    }
    QApplication::restoreOverrideCursor();
}



void QFRDROverviewImageDisplay::readSettings(QSettings &settings, const QString &prefix) {
    player->setFPS(settings.value(prefix+"player_fps", player->getFPS()).toDouble());
    player->setReplay(settings.value(prefix+"player_replay", player->getReplay()).toBool());
    histogram->readSettings(settings, prefix+"histogram/");
    chkHistVideo->setChecked(settings.value(prefix+"videohistogram", chkHistVideo->isChecked()).toBool());
    chkRelaxedColorScale->setChecked(settings.value(prefix+"chkRelaxedColorScale", chkRelaxedColorScale->isChecked()).toBool());
}

void QFRDROverviewImageDisplay::writeSettings(QSettings &settings, const QString &prefix) {
    settings.setValue(prefix+"player_fps", player->getFPS());
    settings.setValue(prefix+"player_replay", player->getReplay());
    histogram->writeSettings(settings, prefix+"histogram/");
    settings.setValue(prefix+"videohistogram", chkHistVideo->isChecked());
    settings.setValue(prefix+"chkRelaxedColorScale", false);
}

void QFRDROverviewImageDisplay::mouseMoved(double x, double y) {
    if (image) {
        if (image->get_visible()) {
            labValue->setText(tr("Position: (%1, %2)        Value: %3").arg(x).arg(y).arg(image->getValueAt(x,y)));
        } else if (imageRGB->get_visible()){
            labValue->setText(tr("Position: (%1, %2)        Value: (%3, %4, %5)").arg(x).arg(y).arg(imageRGB->getValueAt(x,y,1)).arg(imageRGB->getValueAt(x,y,0)).arg(imageRGB->getValueAt(x,y,3)));
        }
    } else {
        labValue->setText("");
    }
}


void QFRDROverviewImageDisplay::clearOverlays() {
    for (int i=0; i<overlayGraphs.size(); i++) {
        pltImage->deleteGraph(overlayGraphs[i], true);
    }
    overlayGraphs.clear();
}
