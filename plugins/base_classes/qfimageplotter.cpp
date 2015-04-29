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



#include "qfimageplotter.h"
#include "qfrdrimagetoruninterface.h"
#include "qfrawdatarecord.h"
#include "qfplotter.h"
#include "qfimageparametergroupbox.h"
#include "qfoverlaystylecombobox.h"
#include "statistics_tools.h"

#define OverlayRectanglesAsImageOverlay true

QFImagePlotter::QFImagePlotter(bool datastore_internal, QWidget *parent, JKQTPdatastore *datast):
    QFPlotter(datastore_internal, parent, datast)
{
    initImFCSPlotter();
}


QFImagePlotter::QFImagePlotter(QWidget *parent) :
    QFPlotter(parent)
{
    initImFCSPlotter();
}

QFImagePlotter::~QFImagePlotter()
{
    if (plteImageData) qfFree(plteImageData);
}

double *QFImagePlotter::getData() const
{
    return plteImageData;
}

void QFImagePlotter::getDataAtBR(int idx, double &value)
{
    if (plteImageData && idx>0 && idx<plteImageSize) value=plteImageData[idx];
}

double QFImagePlotter::getDataAt(int idx, double defaultValue)
{
    if (plteImageData && idx>0 && idx<plteImageSize) return plteImageData[idx];
    return defaultValue;
}

int32_t QFImagePlotter::getDataSize() const
{
    return plteImageSize;
}

bool *QFImagePlotter::getExcluded() const
{
    return plteOverviewExcludedData;
}

bool *QFImagePlotter::getSelected() const
{
    return plteOverviewSelectedData;
}



void QFImagePlotter::connectTo(QFImageParameterGroupBox *paramGrp, QFOverlayStyleCombobox *overlayCmb)
{
    if (this->paramGrp) disconnect(this->paramGrp, SIGNAL(settingsChanged()), this, SLOT(updatePlot()));
    if (this->overlayCmb) disconnect(this->overlayCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot()));
    this->paramGrp=paramGrp;
    this->overlayCmb=overlayCmb;
    if (paramGrp) connect(paramGrp, SIGNAL(settingsChanged()), this, SLOT(updatePlot()));
    if (overlayCmb) connect(overlayCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot()));
}


void QFImagePlotter::updatePlot()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool draw=get_doDrawing();
    set_doDrawing(false);


    updateImage();
    updateOverlays();

    setCopyableData();
    set_doDrawing(draw);
    if (draw) update_plot();
    QApplication::restoreOverrideCursor();
}

void QFImagePlotter::updateImage()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool draw=get_doDrawing();
    set_doDrawing(false);

    if (!plteImageData || plteImageSize<=0) {
        plteImage->set_data(NULL, 0, 0, JKQTPMathImageBase::DoubleArray);
        plteImage->get_colorBarRightAxis()->set_labelFontSize(8);
        plteImage->get_colorBarRightAxis()->set_axisLabel("");
        plteImage->get_colorBarTopAxis()->set_labelFontSize(8);
        plteImage->get_colorBarTopAxis()->set_axisLabel("");
    } else {
        double w=plteImageWidth;
        double h=plteImageHeight;
        if ((w<=0) || (h<=0)) {
            w=h=1;
        }
        setAbsoluteXY(0, w, 0, h);
        get_plotter()->set_maintainAspectRatio(keepAspect);
        get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        get_plotter()->set_maintainAxisAspectRatio(keepAspect);
        get_plotter()->set_axisAspectRatio(1.0*w/h);
        plteImage->get_colorBarRightAxis()->set_labelFontSize(8);
        plteImage->get_colorBarRightAxis()->set_axisLabel(plteImageLabel);
        plteImage->get_colorBarTopAxis()->set_labelFontSize(8);
        plteImage->get_colorBarTopAxis()->set_axisLabel(plteImageLabel);

        get_plotter()->getXAxis()->set_minTicks(3);
        get_plotter()->getYAxis()->set_minTicks(3);
        get_plotter()->getXAxis()->set_minorTicks(0);
        get_plotter()->getYAxis()->set_minorTicks(0);
        get_plotter()->getYAxis()->set_tickInsideLength(0);
        get_plotter()->getYAxis()->set_tickInsideLength(0);

        if (w>3*h) {
            get_plotter()->getXAxis()->set_minTicks(3);
            plteImage->get_colorBarRightAxis()->set_minTicks(3);
            plteImage->get_colorBarTopAxis()->set_minTicks(5);
        } else {
            get_plotter()->getXAxis()->set_minTicks(3);
            plteImage->get_colorBarRightAxis()->set_minTicks(5);
            plteImage->get_colorBarTopAxis()->set_minTicks(3);
        }
        setXY(0, w, 0, h);

        qDebug()<<plteImageData<<plteImageWidth<<plteImageHeight;

        plteImage->set_data(plteImageData, plteImageWidth, plteImageHeight, JKQTPMathImageBase::DoubleArray);
        plteImage->set_width(w);
        plteImage->set_height(h);
        if (paramGrp) {
            paramGrp->setSelectedImageStyle(plteImage, plteOverviewExcludedData, false);
        }


    }

    setCopyableData();
    set_doDrawing(draw);
    if (draw) update_plot();
    QApplication::restoreOverrideCursor();
}

void QFImagePlotter::updateImage(double *data, bool *plteOverviewSelectedData, bool *plteOverviewExcludedData, int width, int height, const QString& label, bool deleteData, bool clearDatastore, QFRDRAnnotationInterface *annotations, int annotID, QColor annotColor)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool draw=get_doDrawing();
    set_doDrawing(false);

    if (clearDatastore) getDatastore()->clear();

    this->plteOverviewSelectedData=plteOverviewSelectedData;
    this->plteOverviewExcludedData=plteOverviewExcludedData;

    if (plteImageSize!=width*height) {
        plteImageSize=width*height;
        plteImageData=(double*)qfRealloc(plteImageData, plteImageSize*sizeof(double));
    }

    if (data && plteImageData) {
        memcpy(plteImageData, data, width*height*sizeof(double));
        if (deleteData) qfFree(data);
    }
    plteImageWidth=width;
    plteImageHeight=height;
    plteImageLabel=label;


    for (int i=0; i<plteAnnotations.size(); i++) {
        deleteGraph(plteAnnotations[i], true);
    }
    plteAnnotations.clear();



    updatePlot();
    setCopyableData();
    if (annotations) {
        if (annotID<0 || annotID>=annotations->annotGetCount()) {
            for (int i=0; i<annotations->annotGetCount(); i++) {
                if (annotations->annotGetType(i)) {
                    switch (annotations->annotGetType(i)) {
                        case QFRDRAnnotationInterface::annotKategoryIndex:{
                                QVector<double> ax,ay,ac, adat;
                                QVector<bool> aset;
                                adat=annotations->annotGetValues(i);
                                aset=annotations->annotGetValuesSet(i);
                                int j=0;
                                for (int y=0; y<height; y++) {
                                    for (int x=0; x<width; x++) {
                                        if (j<adat.size() && aset.value(j, false) && adat.value(j, 0)>0) {
                                            ax<<x;
                                            ay<<y;
                                            ac<<adat.value(j, 0);
                                        }
                                        j++;
                                    }
                                }
                                JKQTPxyParametrizedScatterGraph* plt=new JKQTPxyParametrizedScatterGraph(get_plotter());
                                int sx=getDatastore()->getColumnNum(QString("annotation%1_x").arg(i));
                                if (sx<0) sx=getDatastore()->addCopiedColumn(ax.data(), ax.size(), QString("annotation%1_x").arg(i));
                                else getDatastore()->getColumn(sx).copy(ax.data(), ax.size());
                                int sy=getDatastore()->getColumnNum(QString("annotation%1_y").arg(i));
                                if (sy<0) sy=getDatastore()->addCopiedColumn(ay.data(), ay.size(), QString("annotation%1_y").arg(i));
                                else getDatastore()->getColumn(sy).copy(ay.data(), ay.size());
                                int sc=getDatastore()->getColumnNum(QString("annotation%1_category").arg(i));
                                if (sc<0) sc=getDatastore()->addCopiedColumn(ac.data(), ac.size(), QString("annotation%1_category").arg(i));
                                else getDatastore()->getColumn(sc).copy(ac.data(), ac.size());
                                plt->set_symbolColumn(sc);
                                plt->set_color(annotColor);
                                plt->set_title(annotations->annotGetLabel(i));
                                plteAnnotations.append(plt);
                            }
                            break;
                        default: break;
                    }
                }
            }
        } else {
            int i=annotID;
            switch (annotations->annotGetType(i)) {
                case QFRDRAnnotationInterface::annotKategoryIndex:{
                        QVector<double> ax,ay,ac, adat;
                        QVector<bool> aset;
                        adat=annotations->annotGetValues(i);
                        aset=annotations->annotGetValuesSet(i);
                        int j=0;
                        for (int y=0; y<height; y++) {
                            for (int x=0; x<width; x++) {
                                if (j<adat.size() && aset.value(j, false) && adat.value(j, 0)>0) {
                                    ax<<x;
                                    ay<<y;
                                    ac<<adat.value(j, 0);
                                }
                                j++;
                            }
                        }
                        JKQTPxyParametrizedScatterGraph* plt=new JKQTPxyParametrizedScatterGraph(get_plotter());
                        int sx=getDatastore()->getColumnNum(QString("annotation%1_x").arg(i));
                        if (sx<0) sx=getDatastore()->addCopiedColumn(ax.data(), ax.size(), QString("annotation%1_x").arg(i));
                        else getDatastore()->getColumn(sx).copy(ax.data(), ax.size());
                        int sy=getDatastore()->getColumnNum(QString("annotation%1_y").arg(i));
                        if (sy<0) sy=getDatastore()->addCopiedColumn(ay.data(), ay.size(), QString("annotation%1_y").arg(i));
                        else getDatastore()->getColumn(sy).copy(ay.data(), ay.size());
                        int sc=getDatastore()->getColumnNum(QString("annotation%1_category").arg(i));
                        if (sc<0) sc=getDatastore()->addCopiedColumn(ac.data(), ac.size(), QString("annotation%1_category").arg(i));
                        else getDatastore()->getColumn(sc).copy(ac.data(), ac.size());
                        plt->set_symbolColumn(sc);
                        plt->set_color(annotColor);
                        plt->set_title(annotations->annotGetLabel(i));
                        plteAnnotations.append(plt);
                    }
                    break;
                default:
                    break;

            }
        }
    }
    set_doDrawing(draw);
    if (draw) update_plot();
    QApplication::restoreOverrideCursor();

}

void QFImagePlotter::updateOverlays(double *avgOut, double *sdOut)
{
    setDisplayOverlay(displayOverlay/*, displayMask*/, avgOut, sdOut);
}

void QFImagePlotter::updateOverlays(bool *plteOverviewSelectedData, bool *plteOverviewExcludedData, double *avgOut, double *sdOut)
{
    this->plteOverviewSelectedData=plteOverviewSelectedData;
    this->plteOverviewExcludedData=plteOverviewExcludedData;

    updateOverlays(avgOut, sdOut);
}

void QFImagePlotter::initImFCSPlotter()
{
    keepAspect=true;
    paramGrp=NULL;
    overlayCmb=NULL;
    current=NULL;
    plteImageData=NULL;
    plteImageSize=NULL;
    plteImageWidth=0;
    plteImageHeight=0;

    get_plotter()->set_plotLabelFontSize(10);
    set_displayMousePosition(false);
    set_displayToolbar(true);
    get_plotter()->set_maintainAspectRatio(keepAspect);
    get_plotter()->set_aspectRatio(1);
    get_plotter()->set_maintainAxisAspectRatio(keepAspect);
    get_plotter()->set_axisAspectRatio(1);
    setXY(0,0,1,1);
    setAbsoluteXY(0,1,0,1);

    get_plotter()->getXAxis()->set_axisMinWidth(1);
    get_plotter()->getYAxis()->set_axisMinWidth(1);
    get_plotter()->getXAxis()->set_tickLabelFontSize(8);
    get_plotter()->getYAxis()->set_tickLabelFontSize(8);
    get_plotter()->getXAxis()->set_axisLabel("");
    get_plotter()->getYAxis()->set_axisLabel("");
    get_plotter()->setGrid(false);
    get_plotter()->set_useAntiAliasingForSystem(true);
    get_plotter()->set_useAntiAliasingForGraphs(true);
    set_userActionCompositionMode(QPainter::CompositionMode_Xor);

    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImageGRAY, get_plotter());
    addGraph(plteImage);

    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.5);
    //selectionColor=ovlSelCol;
    QColor ovlExCol=QColor("blue");
    ovlExCol.setAlphaF(0.5);
    //excludedColor=ovlExCol;

    plteImageSelected=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlSelCol, get_plotter());
    plteImageSelected->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);


    plteImageExcluded=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0, 0, ovlExCol, get_plotter());
    plteImageExcluded->set_rectanglesAsImageOverlay(OverlayRectanglesAsImageOverlay);

    plteImageData=NULL;

    connect(this, SIGNAL(beforePlotScalingRecalculate()), this, SLOT(moveColorbarsAuto()));
}


void QFImagePlotter::moveColorbarsAuto()
{
        QFRDRImageToRunInterface* m=qobject_cast<QFRDRImageToRunInterface*>(current);
        bool rightVisible=false;
        if (m && plteImageHeight < plteImageWidth) { // wider than high, i.e. "landscape"
            rightVisible=!rightVisible;
        }
        if ((double)width()<=(double)height()) {
            plteImage->set_colorBarRightVisible(rightVisible);
            plteImage->set_colorBarTopVisible(!rightVisible);
        } else {
            plteImage->set_colorBarRightVisible(!rightVisible);
            plteImage->set_colorBarTopVisible(rightVisible);
        }
}

void QFImagePlotter::overlayStyleChanged()
{
    if (overlayCmb) overlayCmb->setSelectedOverlayStyle(plteImageSelected);
}


void QFImagePlotter::setDisplayOverlay(bool displayOverlay, double *avgOut, double *sdOut)
{
#ifdef DEBUG_TIMIMNG
    qDebug()<<"     QFImagePlotter::setDisplayOverlay";
    QElapsedTimer time;
    time.start();
#endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool draw=get_doDrawing();
    set_doDrawing(false);

#ifdef DEBUG_TIMIMNG
    qDebug()<<"     QFImagePlotter::setDisplayOverlay ... 1 = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif

    this->displayMask=true;
    if (paramGrp) displayMask=paramGrp->getImageStyleDisplayMask();
    this->displayOverlay=displayOverlay;
    if (!displayOverlay) {
        deleteGraph(plteImageSelected, false);
        deleteGraph(plteImageExcluded, false);
    } else {
        addGraph(plteImageExcluded);
        addGraph(plteImageSelected);
    }

    if (overlayCmb) overlayCmb->setSelectedOverlayStyle(plteImageSelected);

    if (displayMask) {
        deleteGraph(plteImageExcluded, false);
        addGraph(plteImageExcluded);
    } else {
        deleteGraph(plteImageExcluded, false);
    }

    if (paramGrp) paramGrp->setSelectedMaskStyle(plteImageExcluded);

    if (containsGraph(plteImageSelected)) moveGraphTop(plteImageSelected);

    if ((avgOut || sdOut)&&plteOverviewSelectedData&&plteOverviewExcludedData) {
        bool *msk=(bool*)qfCalloc(plteImageWidth*plteImageHeight, sizeof(bool));
        int cnt=0;
        for (int i=0; i<plteImageWidth*plteImageHeight; i++) {
            msk[i]=plteOverviewSelectedData[i]&&(!plteOverviewExcludedData[i]);
            if (msk[i]) cnt++;
        }
        double imgAvg=0;
        double imgVar=0;
        //imgAvg=statisticsAverageVarianceMasked(imgVar, plteOverviewSelectedData, plteImageData, qMin(plteOverviewSize, plteImageWidth*plteImageHeight));
        if (cnt>1){
            imgAvg=statisticsAverageVarianceMasked(imgVar, msk, plteImageData, plteImageWidth*plteImageHeight);
        } else {
            imgAvg=statisticsAverageVarianceMasked(imgVar, plteOverviewExcludedData, plteImageData, plteImageWidth*plteImageHeight, false);
        }
        qfFree(msk);

        if (avgOut) *avgOut=imgAvg;
        if (sdOut) *sdOut=sqrt(imgVar);
    }

    double w=plteImageWidth;
    double h=plteImageHeight;
    if ((w<=0) || (h<=0)) {
        w=h=1;
    }

#ifdef DEBUG_TIMIMNG
    qDebug()<<"     QFImagePlotter::setDisplayOverlay ... 2 = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
    plteImageSelected->set_width(w);
    plteImageSelected->set_height(h);
    plteImageSelected->set_data(plteOverviewSelectedData, plteImageWidth, plteImageHeight);


#ifdef DEBUG_TIMIMNG
    qDebug()<<"     QFImagePlotter::setDisplayOverlay ... 3 = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
    plteImageExcluded->set_width(w);
    plteImageExcluded->set_height(h);
    plteImageExcluded->set_data(plteOverviewExcludedData, plteImageWidth, plteImageHeight);

    emit saveImageSettings();

#ifdef DEBUG_TIMIMNG
    qDebug()<<"     QFImagePlotter::setDisplayOverlay ... 4 = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
    //updateImage();

    set_doDrawing(draw);

#ifdef DEBUG_TIMIMNG
    qDebug()<<"     QFImagePlotter::setDisplayOverlay ... 5 = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
    if (draw) update_plot();

#ifdef DEBUG_TIMIMNG
    qDebug()<<"     QFImagePlotter::setDisplayOverlay ... 6 = " <<time.nsecsElapsed()/1000<<" usecs = "<<(double)time.nsecsElapsed()/1000000.0<<" msecs";;
#endif
    QApplication::restoreOverrideCursor();

}

void QFImagePlotter::setCurrent(QFRawDataRecord *current)
{
    this->current=current;
}

void QFImagePlotter::clearImage()
{
    if (plteImageData) qfFree(plteImageData);
    plteImageData=NULL;
    plteImageSize=0;
    plteImageWidth=0;
    plteImageHeight=0;

    updatePlot();
}

void QFImagePlotter::setAspectRatio(bool keepAspect)
{
    if (this->keepAspect!=keepAspect) {
        this->keepAspect=keepAspect;
        updatePlot();
    }
}

void QFImagePlotter::setCopyableData()
{
    getDatastore()->clear();
    if (plteImageData && plteOverviewSelectedData && plteOverviewExcludedData && plteImageSize>0) {
        getDatastore()->addCopiedColumn(plteImageData, plteImageSize, plteImageLabel);
        getDatastore()->addCopiedColumn(plteOverviewSelectedData, plteImageSize, tr("selection"));
        getDatastore()->addCopiedColumn(plteOverviewExcludedData, plteImageSize, tr("mask"));
    }
}




