#include "qfrdrimagemaskedittools.h"
#include "programoptions.h"
#include "qftools.h"
#include <QClipboard>

#define sqr(x) ((x)*(x))

#define CLICK_UPDATE_TIMEOUT 500

QFRDRImageMaskEditTools::QFRDRImageMaskEditTools(QWidget *parentWidget, const QString& settingsPrefix):
    QObject(parentWidget)
{
    this->parentWidget=parentWidget;
    this->settingsPrefix=settingsPrefix;
    rdr=NULL;
    useDelay=false;

    actSaveMask=new QAction(QIcon(":/qfrdrmaskeditor/savemask.png"), tr("&save mask"), parentWidget);
    actSaveMask->setToolTip(tr("save the mask to harddisk"));
    connect(actSaveMask, SIGNAL(triggered()), this, SLOT(saveMask()));
    actLoadMask=new QAction(QIcon(":/qfrdrmaskeditor/loadmask.png"), tr("&load mask"), parentWidget);
    actLoadMask->setToolTip(tr("load a mask from harddisk"));
    connect(actLoadMask, SIGNAL(triggered()), this, SLOT(loadMask()));
    actCopyMask=new QAction(QIcon(":/qfrdrmaskeditor/copymask.png"), tr("&copy mask"), parentWidget);
    actCopyMask->setToolTip(tr("copy the mask to clipboard"));
    connect(actCopyMask, SIGNAL(triggered()), this, SLOT(copyMask()));
    actPasteMask=new QAction(QIcon(":/qfrdrmaskeditor/pastemask.png"), tr("&paste mask"), parentWidget);
    actPasteMask->setToolTip(tr("paste a mask from clipboard"));
    connect(actPasteMask, SIGNAL(triggered()), this, SLOT(pasteMask()));

    actClearMask=new QAction(QIcon(":/qfrdrmaskeditor/clearmask.png"), tr("&clear mask"), parentWidget);
    actClearMask->setToolTip(tr("clear the current mask"));
    connect(actClearMask, SIGNAL(triggered()), this, SLOT(clearMask()));
    actInvertMask=new QAction(QIcon(":/qfrdrmaskeditor/invertmask.png"), tr("&invert mask"), parentWidget);
    actInvertMask->setToolTip(tr("invert the current mask"));
    connect(actInvertMask, SIGNAL(triggered()), this, SLOT(invertMask()));

    actUndoMask=new QAction(QIcon(":/qfrdrmaskeditor/undomask.png"), tr("&undo"), parentWidget);
    actUndoMask->setToolTip(tr("undo last change to mask"));
    connect(actUndoMask, SIGNAL(triggered()), this, SLOT(undoMask()));
    actRedoMask=new QAction(QIcon(":/qfrdrmaskeditor/redomask.png"), tr("&redo"), parentWidget);
    actRedoMask->setToolTip(tr("redo peviously undone change to mask"));
    connect(actRedoMask, SIGNAL(triggered()), this, SLOT(redoMask()));





    actImagesZoom=new QAction(QIcon(":/qfrdrmaskeditor/zoom.png"), tr("zoom"), this);
    actImagesZoom->setToolTip(tr("in this mode the user may zoom into a plot by drawing a rectangle (draging with the left mouse key)\nA click toggles the current selection/mask position."));
    actImagesZoom->setCheckable(true);
    actImagesDrawPoints=new QAction(QIcon(":/qfrdrmaskeditor/draw_point.png"), tr("point-wise mask editing"), this);
    actImagesDrawPoints->setToolTip(tr("in this mode the user may click on single points.<br>"
                                 "to add and remove them to/from the mask. A click<br>"
                                 "will toggle the state of the clicked pixel<ul>"
                                 "<li>CTRL: pixel will be added to current mask</li>"
                                 "<li>SHIFT: pixel will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawPoints->setCheckable(true);
    actImagesDrawRectangle=new QAction(QIcon(":/imaging_fcs/draw_rectangle.png"), tr("rectangular mask editing"), this);
    actImagesDrawRectangle->setToolTip(tr("in this mode the user may draw a rectangle.<br>"
                                 "All pixels inside the rectangle will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawRectangle->setCheckable(true);

    actImagesDrawCircle=new QAction(QIcon(":/qfrdrmaskeditor/draw_circle.png"), tr("circular mask editing"), this);
    actImagesDrawCircle->setToolTip(tr("in this mode the user may draw a circle.<br>"
                                 "All pixels inside the cirle will be selected<br>"
                                 "when the user releases the left mouse key. The point of first<br>"
                                 "click will be the center of the circle. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawCircle->setCheckable(true);
    actImagesDrawEllipse=new QAction(QIcon(":/qfrdrmaskeditor/draw_ellipse.png"), tr("elliptical mask editing"), this);
    actImagesDrawEllipse->setToolTip(tr("in this mode the user may draw a ellipse.<br>"
                                 "All pixels inside the ellipse will be selected<br>"
                                "when the user releases the left mouse key. The point of first<br>"
                                "click will be the center of the ellipse. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawEllipse->setCheckable(true);
    actImagesDrawLine=new QAction(QIcon(":/qfrdrmaskeditor/draw_line.png"), tr("line mask editing"), this);
    actImagesDrawLine->setToolTip(tr("in this mode the user may draw a line.<br>"
                                 "All pixels on the line will be selected<br>"
                                 "when the user releases the left mouse key. You may<br>"
                                 "alter this function by pressing one of these keys:<ul>"
                                 "<li>CTRL: pixels will be added to current mask</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
                                 "</ul>"));
    actImagesDrawLine->setCheckable(true);

    actImagesScribble=new QAction(QIcon(":/qfrdrmaskeditor/draw_scribble.png"), tr("scribble mask editing"), this);
    actImagesScribble->setToolTip(tr("in this mode the user may select/deselect pixels by.<br>"
                                 "keeping the left mouse button pressed and moving the mouse<br>"
                                 "over the image. Depending on the key pressed on the keyboard,<br>"
                                 "different actions are executed:<ul>"
                                 "<li>CTRL: the old mask is not deleted, so the result is appended to it.</li>"
                                 "<li>SHIFT: pixels will be removed from current mask</li>"
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
    connect(agImageSelectionActions, SIGNAL(selected(QAction*)), this, SLOT(setImageEditMode()));

    actImagesZoom->setChecked(true);



    timUpdateAfterClick=new QTimer(this);
    timUpdateAfterClick->setSingleShot(true);
    timUpdateAfterClick->setInterval(CLICK_UPDATE_TIMEOUT);
    timUpdateAfterClick->stop();
    connect(timUpdateAfterClick, SIGNAL(timeout()), this, SLOT(updateAfterClick()));

    setRDR(rdr);

}

void QFRDRImageMaskEditTools::setRDR(QFRawDataRecord *rdr)
{
    if (this->rdr)  {
        this->rdr->setQFProperty(settingsPrefix+"QFRDRImageMaskEditTools_undostack", undos, false, false);
        this->rdr->setQFProperty(settingsPrefix+"QFRDRImageMaskEditTools_undopos", undoPos, false, false);
    }

    this->rdr=rdr;
    imagemask=qobject_cast<QFRDRImageMaskInterface*>(rdr);
    runselection=qobject_cast<QFRDRRunSelectionsInterface*>(rdr);
    actSaveMask->setEnabled(imagemask);
    actLoadMask->setEnabled(imagemask);
    actCopyMask->setEnabled(imagemask);
    actPasteMask->setEnabled(imagemask);
    actClearMask->setEnabled(imagemask);
    actInvertMask->setEnabled(imagemask);

    if (rdr)  {
        undos=rdr->getProperty(settingsPrefix+"QFRDRImageMaskEditTools_undostack",QStringList()).toStringList();
        undoPos=rdr->getProperty(settingsPrefix+"QFRDRImageMaskEditTools_undopos", -1).toInt();
        if (undos.size()<0) undoPos=-1;

        if (imagemask)  {
            undos.append(imagemask->maskToString());
            undoPos=undos.size()-1;
        }
    }
    updateUndoActions();

}


void QFRDRImageMaskEditTools::setLeaveout(QList<int> leaveout, bool clearOld)
{
    if (runselection) {
        QSet<int> set=leaveout.toSet();
        for (int i=0; i<runselection->leaveoutGetRunCount(); i++) {
            if (set.contains(i)) runselection->leaveoutAddRun(i);
            else if (clearOld) runselection->leaveoutRemoveRun(i);
        }
        signalMaskChanged(false);
    }

}

void QFRDRImageMaskEditTools::signalMaskChanged(bool delayed, bool updateUndoRedo)
{
    if (imagemask && updateUndoRedo)  {
        for (int i=undos.size()-1; i>undoPos; i--) {
            undos.removeAt(i);
        }
        undos.append(imagemask->maskToString());
        undoPos=undos.size()-1;
        //qDebug()<<"undos: size="<<undos.size()<<"pos="<<undoPos;
        updateUndoActions();
    }

    if (delayed && useDelay) {
        timUpdateAfterClick->setSingleShot(true);
        timUpdateAfterClick->stop();
        timUpdateAfterClick->start(CLICK_UPDATE_TIMEOUT);
    } else {
        //qDebug()<<"###### emitting QFRDRImageMaskEditTools::rawDataChanged()";
        emit rawDataChanged();
    }
}

void QFRDRImageMaskEditTools::updateUndoActions()
{
    actUndoMask->setEnabled(imagemask&&(undoPos>=0)&&(undoPos<undos.size()) );
    actRedoMask->setEnabled(imagemask&&(undoPos<undos.size()-1) );
}

void QFRDRImageMaskEditTools::updateAfterClick()
{
    //qDebug()<<"### emitting QFRDRImageMaskEditTools::rawDataChanged()";
    emit rawDataChanged();
}



void QFRDRImageMaskEditTools::loadMask() {
    if (!imagemask) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFRDRImageMaskEditTools/lastmaskdir", "").toString();
    QString filename= qfGetOpenFileName(parentWidget, tr("select mask file to open ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (QFile::exists(filename)) {
        if (imagemask) {
            imagemask->maskLoad(filename);
        }
        ProgramOptions::setConfigValue("QFRDRImageMaskEditTools/lastmaskdir", lastMaskDir);
    }

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::pasteMask()
{
    if (!imagemask) return;

    QClipboard* clipboard=QApplication::clipboard();

    const QMimeData* mime=clipboard->mimeData();
    if (mime->hasFormat("quickfit3/pixelselection")) {
        imagemask->maskLoadFromString(QString::fromUtf8(mime->data("quickfit3/pixelselection")));
    }

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::saveMask()
{
    if (!imagemask) return;
    QString lastMaskDir=ProgramOptions::getConfigValue("QFRDRImageMaskEditTools/lastmaskdir", "").toString();
    QString filename= qfGetSaveFileName(parentWidget, tr("save mask as ..."), lastMaskDir, tr("mask files (*.msk)"));
    if (!filename.isEmpty()) {
        imagemask->maskSave(filename);
        ProgramOptions::setConfigValue("QFRDRImageMaskEditTools/lastmaskdir", lastMaskDir);
    }
}

void QFRDRImageMaskEditTools::copyMask()
{
    if (!imagemask) return;
    QString mask=imagemask->maskToString();
    QClipboard* clipboard=QApplication::clipboard();
    QMimeData* mime=new QMimeData();
    mime->setText(mask);
    mime->setData("quickfit3/pixelselection", mask.toUtf8());
    clipboard->setMimeData(mime);
}

void QFRDRImageMaskEditTools::clearMask()
{
    if (!imagemask) return;
    imagemask->maskClear();

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::invertMask()
{
    //qDebug()<<"invertmask()   imagemask="<<imagemask;
    if (!imagemask) return;
    imagemask->maskInvert();

    signalMaskChanged(false);
}

void QFRDRImageMaskEditTools::undoMask()
{
    if (!imagemask) return;

    if (undoPos>=0) {
        QString mask=undos.value(undoPos, "");
        imagemask->maskClear();
        imagemask->maskLoadFromString(mask);
        undoPos--;
    }

    signalMaskChanged(false, false);
    updateUndoActions();
}

void QFRDRImageMaskEditTools::redoMask()
{
    if (!imagemask) return;

    if (undoPos+1<undos.size()) {
        QString mask=undos.value(undoPos+1, "");
        imagemask->maskClear();
        imagemask->maskLoadFromString(mask);
        undoPos++;
    }

    signalMaskChanged(false, false);
    updateUndoActions();
}

void QFRDRImageMaskEditTools::registerMaskToolsToMenu(QMenu *menu) const
{
    menu->addAction(actUndoMask);
    menu->addAction(actRedoMask);
    menu->addSeparator();
    menu->addAction(actLoadMask);
    menu->addAction(actSaveMask);
    menu->addAction(actCopyMask);
    menu->addAction(actPasteMask);
    menu->addSeparator();
    menu->addAction(actClearMask);
    menu->addAction(actInvertMask);

}

void QFRDRImageMaskEditTools::registerPlotter(JKQtPlotter *plot)
{
    plotters.removeAll(plot);
    plotters.append(plot);
    connect(plot, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    connect(plot, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    connect(plot, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));
}

void QFRDRImageMaskEditTools::unregisterPlotter(JKQtPlotter *plot)
{
    disconnect(plot, SIGNAL(userClickFinished(double,double,Qt::KeyboardModifiers)), this, SLOT(imageClicked(double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userScribbleClick(double,double,Qt::KeyboardModifiers, bool, bool)), this, SLOT(imageScribbled(double,double,Qt::KeyboardModifiers,bool,bool)));
    disconnect(plot, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userCircleFinished(double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageCircleFinished(double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageEllipseFinished(double,double,double,double,Qt::KeyboardModifiers)));
    disconnect(plot, SIGNAL(userLineFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(imageLineFinished(double,double,double,double,Qt::KeyboardModifiers)));
    plotters.removeAll(plot);
}

void QFRDRImageMaskEditTools::registerPlotterMaskToolsToToolbar(QToolBar *menu) const
{
    menu->addAction(actImagesZoom);
    menu->addAction(actImagesDrawPoints);
    menu->addAction(actImagesDrawRectangle);
    menu->addAction(actImagesDrawLine);
    menu->addAction(actImagesDrawCircle);
    menu->addAction(actImagesDrawEllipse);
    menu->addAction(actImagesScribble);
}

void QFRDRImageMaskEditTools::setUseDelay(bool use)
{
    useDelay=use;
}


void QFRDRImageMaskEditTools::imageClicked(double x, double y, Qt::KeyboardModifiers modifiers)
{
    if (!imagemask) return;
    int xx=(int)floor(x);
    int yy=(int)floor(y);

    if (xx>=0 && xx<imagemask->maskGetWidth() && yy>=0 && yy<imagemask->maskGetHeight()) {

        if (modifiers==Qt::ControlModifier && !actImagesScribble->isChecked()) {
            imagemask->maskToggle(xx,yy);
        } else if (modifiers==Qt::ShiftModifier) {
            imagemask->maskSet(xx,yy);
        } else {
            if (!actImagesScribble->isChecked()) imagemask->maskClear();
            imagemask->maskUnset(xx,yy);
        }
        signalMaskChanged(true);
    }
}

void QFRDRImageMaskEditTools::imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last)
{
    if (!imagemask) return;

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    if (xx>=0 && xx<imagemask->maskGetWidth() && yy>=0 && yy<imagemask->maskGetHeight()) {

        if (first && modifiers==Qt::NoModifier) imagemask->maskClear();
        if (modifiers==Qt::ShiftModifier) {
            imagemask->maskSet(xx,yy);
        } else {
            imagemask->maskUnset(xx,yy);
        }
        signalMaskChanged(true);
    }

}


void QFRDRImageMaskEditTools::imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers)
{
    //qDebug()<<"QFRDRImageMaskEditTools::imageRectangleFinished("<<x<<y<<width<<height<<")";
    if (!imagemask) return;
    int xx1=qBound(0,(int)floor(x), imagemask->maskGetWidth()-1);
    int yy1=qBound(0,(int)floor(y), imagemask->maskGetHeight()-1);
    int xx2=qBound(0,(int)floor(x+width), imagemask->maskGetWidth()-1);
    int yy2=qBound(0,(int)floor(y+height), imagemask->maskGetHeight()-1);

    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    if (modifiers==Qt::ControlModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                imagemask->maskUnset(xx,yy);
                //qDebug()<<"  u"<<xx<<yy;
            }
        }
    } else if (modifiers==Qt::ShiftModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                imagemask->maskSet(xx,yy);
                //qDebug()<<"  s"<<xx<<yy;
            }
        }
    } else {
        imagemask->maskClear();
        //qDebug()<<"  c";
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                imagemask->maskUnset(xx,yy);
                //qDebug()<<"  u"<<xx<<yy;
            }
        }
    }
    signalMaskChanged(true);
}

void QFRDRImageMaskEditTools::imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers)
{
    if (!imagemask) return;

    QLineF line(x1, y1, x2, y2);

    if (modifiers==Qt::ControlModifier) {
        for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
            QPointF p=line.pointAt(i);
            int xx=qBound(0,(int)floor(p.x()), imagemask->maskGetWidth()-1);
            int yy=qBound(0,(int)floor(p.y()), imagemask->maskGetHeight()-1);
            imagemask->maskUnset(xx,yy);
        }
    } else if (modifiers==Qt::ShiftModifier) {
        for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
            QPointF p=line.pointAt(i);
            int xx=qBound(0,(int)floor(p.x()), imagemask->maskGetWidth()-1);
            int yy=qBound(0,(int)floor(p.y()), imagemask->maskGetHeight()-1);
            imagemask->maskSet(xx,yy);
        }
    } else {
        imagemask->maskClear();
        for (double i=0; i<1.0; i=i+0.5/double(qMax(imagemask->maskGetWidth(), imagemask->maskGetHeight()))) {
            QPointF p=line.pointAt(i);
            int xx=qBound(0,(int)floor(p.x()), imagemask->maskGetWidth()-1);
            int yy=qBound(0,(int)floor(p.y()), imagemask->maskGetHeight()-1);
            imagemask->maskUnset(xx,yy);
        }
    }

    signalMaskChanged(true);
}

void QFRDRImageMaskEditTools::imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers)
{
    if (!imagemask) return;


    int xx1=qBound(0,(int)floor(x-radius), imagemask->maskGetWidth()-1);
    int yy1=qBound(0,(int)floor(y-radius), imagemask->maskGetHeight()-1);
    int xx2=qBound(0,(int)floor(x+radius), imagemask->maskGetWidth()-1);
    int yy2=qBound(0,(int)floor(y+radius), imagemask->maskGetHeight()-1);


    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);

    if (modifiers==Qt::ControlModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) imagemask->maskUnset(xx,yy);
            }
        }
    } else if (modifiers==Qt::ShiftModifier) {
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) imagemask->maskSet(xx,yy);
            }
        }
    } else {
        imagemask->maskClear();
        for (int yy=yy1; yy<=yy2; yy++) {
            for (int xx=xx1; xx<=xx2; xx++) {
                if (sqr(double(xx)-x+0.5)+sqr(double(yy)-y+0.5)<sqr(radius)) imagemask->maskUnset(xx,yy);
            }
        }
    }

    signalMaskChanged(true);
}

void QFRDRImageMaskEditTools::imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers)
{
    if (!imagemask) return;

    int xx1=qBound(0,(int)floor(x-radiusX), imagemask->maskGetWidth()-1);
    int yy1=qBound(0,(int)floor(y-radiusY), imagemask->maskGetHeight()-1);
    int xx2=qBound(0,(int)floor(x+radiusX), imagemask->maskGetWidth()-1);
    int yy2=qBound(0,(int)floor(y+radiusY), imagemask->maskGetHeight()-1);

    if (xx1>xx2) qSwap(xx1, xx2);
    if (yy1>yy2) qSwap(yy1, yy2);


        if (modifiers==Qt::ControlModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) imagemask->maskUnset(xx,yy);
                }
            }
        } else if (modifiers==Qt::ShiftModifier) {
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) imagemask->maskSet(xx,yy);
                }
            }
        } else {
            imagemask->maskClear();
            for (int yy=yy1; yy<=yy2; yy++) {
                for (int xx=xx1; xx<=xx2; xx++) {
                    if (sqr(double(xx)-x+0.5)/sqr(radiusX)+sqr(double(yy)-y+0.5)/sqr(radiusY)<1.0) imagemask->maskUnset(xx,yy);
                }
            }
        }

    signalMaskChanged(true);
}



void QFRDRImageMaskEditTools::setImageEditMode() {
    for (int i=0; i<plotters.size(); i++) {
        if (plotters[i]) {
            if (actImagesZoom->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
            } else if (actImagesDrawPoints->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::ClickEvents);
            } else if (actImagesDrawRectangle->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::RectangleEvents);
            } else if (actImagesDrawLine->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::LineEvents);
            } else if (actImagesDrawCircle->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::CircleEvents);
            } else if (actImagesDrawEllipse->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::EllipseEvents);
            } else if (actImagesScribble->isChecked()) {
                plotters[i]->set_mouseActionMode(JKQtPlotter::ScribbleEvents);
            }
        }
    }


}
