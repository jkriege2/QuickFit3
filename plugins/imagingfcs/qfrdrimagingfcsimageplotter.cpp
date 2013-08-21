#include "qfrdrimagingfcsimageplotter.h"
#include "qfrdrimagetoruninterface.h"
#include "qfrawdatarecord.h"
#include "qfplotter.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcsimageparametergroupbox.h"
#include "qfrdrimagingfcsoverlaystylecombobox.h"
#include "statistics_tools.h"

#define OverlayRectanglesAsImageOverlay true

QFRDRImagingFCSImagePlotter::QFRDRImagingFCSImagePlotter(bool datastore_internal, QWidget *parent, JKQTPdatastore *datast):
    QFPlotter(datastore_internal, parent, datast)
{
    initImFCSPlotter();
}


QFRDRImagingFCSImagePlotter::QFRDRImagingFCSImagePlotter(QWidget *parent) :
    QFPlotter(parent)
{
    initImFCSPlotter();
}

QFRDRImagingFCSImagePlotter::~QFRDRImagingFCSImagePlotter()
{
    if (plteImageData) free(plteImageData);
}

double *QFRDRImagingFCSImagePlotter::getData() const
{
    return plteImageData;
}

void QFRDRImagingFCSImagePlotter::getDataAtBR(int idx, double &value)
{
    if (plteImageData && idx>0 && idx<plteImageSize) value=plteImageData[idx];
}

double QFRDRImagingFCSImagePlotter::getDataAt(int idx, double defaultValue)
{
    if (plteImageData && idx>0 && idx<plteImageSize) return plteImageData[idx];
    return defaultValue;
}

int32_t QFRDRImagingFCSImagePlotter::getDataSize() const
{
    return plteImageSize;
}

bool *QFRDRImagingFCSImagePlotter::getExcluded() const
{
    return plteOverviewExcludedData;
}

bool *QFRDRImagingFCSImagePlotter::getSelected() const
{
    return plteOverviewSelectedData;
}



void QFRDRImagingFCSImagePlotter::connectTo(QFRDRImagingFCSImageParameterGroupBox *paramGrp, QFRDRImagingFCSOverlayStyleCombobox *overlayCmb)
{
    if (this->paramGrp) disconnect(this->paramGrp, SIGNAL(settingsChanged()), this, SLOT(updatePlot()));
    if (this->overlayCmb) disconnect(this->overlayCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot()));
    this->paramGrp=paramGrp;
    this->overlayCmb=overlayCmb;
    if (paramGrp) connect(paramGrp, SIGNAL(settingsChanged()), this, SLOT(updatePlot()));
    if (overlayCmb) connect(overlayCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlot()));
}


void QFRDRImagingFCSImagePlotter::updatePlot()
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

void QFRDRImagingFCSImagePlotter::updateImage()
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
        get_plotter()->set_maintainAspectRatio(true);
        get_plotter()->set_aspectRatio(w/h);//qMax(0.01, qMin(100.0, w/h)));
        get_plotter()->set_maintainAxisAspectRatio(true);
        get_plotter()->set_axisAspectRatio(1.0*w/h);
        plteImage->get_colorBarRightAxis()->set_labelFontSize(8);
        plteImage->get_colorBarRightAxis()->set_axisLabel(plteImageLabel);
        plteImage->get_colorBarTopAxis()->set_labelFontSize(8);
        plteImage->get_colorBarTopAxis()->set_axisLabel(plteImageLabel);

        if (w>3*h) {
            get_plotter()->getXAxis()->set_minTicks(3);
            plteImage->get_colorBarRightAxis()->set_minTicks(3);
            plteImage->get_colorBarTopAxis()->set_minTicks(5);
        } else {
            get_plotter()->getXAxis()->set_minTicks(7);
            plteImage->get_colorBarRightAxis()->set_minTicks(5);
            plteImage->get_colorBarTopAxis()->set_minTicks(3);
        }
        setXY(0, w, 0, h);


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

void QFRDRImagingFCSImagePlotter::updateImage(double *data, bool *plteOverviewSelectedData, bool *plteOverviewExcludedData, int width, int height, const QString& label, bool deleteData, bool clearDatastore)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool draw=get_doDrawing();
    set_doDrawing(false);

    if (clearDatastore) getDatastore()->clear();

    this->plteOverviewSelectedData=plteOverviewSelectedData;
    this->plteOverviewExcludedData=plteOverviewExcludedData;

    if (plteImageSize!=width*height) {
        plteImageSize=width*height;
        plteImageData=(double*)realloc(plteImageData, plteImageSize*sizeof(double));
    }

    if (data && plteImageData) {
        memcpy(plteImageData, data, width*height*sizeof(double));
        if (deleteData) free(data);
    }
    plteImageWidth=width;
    plteImageHeight=height;
    plteImageLabel=label;



    updatePlot();
    set_doDrawing(draw);
    if (draw) update_plot();
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImagePlotter::updateOverlays(double *avgOut, double *sdOut)
{
    setDisplayOverlay(displayOverlay/*, displayMask*/, avgOut, sdOut);
}

void QFRDRImagingFCSImagePlotter::updateOverlays(bool *plteOverviewSelectedData, bool *plteOverviewExcludedData, double *avgOut, double *sdOut)
{
    this->plteOverviewSelectedData=plteOverviewSelectedData;
    this->plteOverviewExcludedData=plteOverviewExcludedData;

    updateOverlays(avgOut, sdOut);
}

void QFRDRImagingFCSImagePlotter::initImFCSPlotter()
{
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
    get_plotter()->set_maintainAspectRatio(true);
    get_plotter()->set_aspectRatio(1);
    get_plotter()->set_maintainAxisAspectRatio(true);
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

    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0, JKQTPMathImage::GRAY, get_plotter());
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


void QFRDRImagingFCSImagePlotter::moveColorbarsAuto()
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

void QFRDRImagingFCSImagePlotter::overlayStyleChanged()
{
    if (overlayCmb) overlayCmb->setSelectedOverlayStyle(plteImageSelected);
}


void QFRDRImagingFCSImagePlotter::setDisplayOverlay(bool displayOverlay, double *avgOut, double *sdOut)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool draw=get_doDrawing();
    set_doDrawing(false);

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
        bool *msk=(bool*)calloc(plteImageWidth*plteImageHeight, sizeof(bool));
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
        free(msk);

        if (avgOut) *avgOut=imgAvg;
        if (sdOut) *sdOut=sqrt(imgVar);
    }

    double w=plteImageWidth;
    double h=plteImageHeight;
    if ((w<=0) || (h<=0)) {
        w=h=1;
    }

    plteImageSelected->set_width(w);
    plteImageSelected->set_height(h);
    plteImageSelected->set_data(plteOverviewSelectedData, plteImageWidth, plteImageHeight);


    plteImageExcluded->set_width(w);
    plteImageExcluded->set_height(h);
    plteImageExcluded->set_data(plteOverviewExcludedData, plteImageWidth, plteImageHeight);

    emit saveImageSettings();

    //updateImage();

    set_doDrawing(draw);
    if (draw) update_plot();
    QApplication::restoreOverrideCursor();

}

void QFRDRImagingFCSImagePlotter::setCurrent(QFRawDataRecord *current)
{
    this->current=current;
}

void QFRDRImagingFCSImagePlotter::clearImage()
{
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
    plteImageSize=0;
    plteImageWidth=0;
    plteImageHeight=0;

    updatePlot();
}

void QFRDRImagingFCSImagePlotter::setCopyableData()
{
    getDatastore()->clear();
    if (plteImageData && plteOverviewSelectedData && plteOverviewExcludedData && plteImageSize>0) {
        getDatastore()->addCopiedColumn(plteImageData, plteImageSize, plteImageLabel);
        getDatastore()->addCopiedColumn(plteOverviewSelectedData, plteImageSize, tr("selection"));
        getDatastore()->addCopiedColumn(plteOverviewExcludedData, plteImageSize, tr("mask"));
    }
}




