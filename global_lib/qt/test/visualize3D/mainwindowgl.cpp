#include "mainwindowgl.h"
#include "ui_mainwindowgl.h"
#include <cmath>

MainWindowGL::MainWindowGL(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindowGL)
{

    sX=100;
    sY=100;
    sZ=100;
    ssX=5;
    ssY=5;
    ssZ=5;

    double wx=50;
    double wy=30;
    double wz=50;
    double wr=20;

    data3D=(double*)malloc(sX*sY*sZ*sizeof(double));
    data3D2=(double*)malloc(sX*sY*sZ*sizeof(double));
    data3D3=(double*)malloc(sX*sY*sZ*sizeof(double));
    for (int z=0; z<sZ; z++) {
        for (int y=0; y<sY; y++) {
            for (int x=0; x<sX; x++) {
                double xx=x-sX/2.0;
                double yy=y-sY/2.0;
                double zz=z-sZ/2.0;
                double rr=sqrt(xx*xx+yy*yy+zz*zz);
                //qDebug()<<xx;
                data3D[z*sX*sY+y*sX+x]=exp(-2.0*(xx*xx/wx/wx+yy*yy/wy/wy+zz*zz/wz/wz))*0.9*fabs(sin(2.0*M_PI*rr/wr));
                data3D2[z*sX*sY+y*sX+x]=fabs(sin(2.0*M_PI*rr/wr/2));
                data3D3[z*sX*sY+y*sX+x]=data3D2[z*sX*sY+y*sX+x];
                if (x>sX/2)data3D3[z*sX*sY+y*sX+x]=0;
                //if (z==0) data3D[z*sX*sY+y*sX+x]=1;
                //if (z==sZ-1) data3D[z*sX*sY+y*sX+x]=0.5;
            }
        }
    }

    data3D4=(double*)malloc(ssX*ssY*ssZ*sizeof(double));
    for (int z=0; z<ssZ; z++) {
        for (int y=0; y<ssY; y++) {
            for (int x=0; x<ssX; x++) {
                double xx=x-ssX/2.0;
                double yy=y-ssY/2.0;
                double zz=z-ssZ/2.0;
                double rr=sqrt(xx*xx+yy*yy+zz*zz);
                data3D4[z*ssX*ssY+y*ssX+x]=fabs(sin(2.0*M_PI*rr/wr/2));
                if (x>ssX/2)data3D4[z*ssX*ssY+y*ssX+x]=0;
            }
        }
    }



    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(0,0);
    QList<int> widths;
    widths<<width()*3/4<<width()/4;
    ui->splitter->setSizes(widths);


    ui->widget2->setCurrentVData(0);
    ui->cmbCurrentGeometry->setCurrentIndex(-1);

    mapLUT[0]=ui->lutEditor->getConfig();
    mapLUT[1]=ui->lutEditor->getConfig();
    mapLUTAlpha[0]=ui->lutAlphaEditor->getConfig();
    mapLUTAlpha[1]=ui->lutAlphaEditor->getConfig();
    mapDataSet[0]=1;
    mapDataSet[1]=4;
    mapColorPal[0]=JKQTPMathImageGRAY;
    mapColorPal[1]=JKQTPMathImageMATLAB;


    ui->lutAlphaEditor->setAlphaMode(true);
    ui->spinAlphaLower->setRange(0,1);
    ui->spinAlphaUpper->setRange(0,1);

    ui->cmbAmbient->setCurrentColor(ui->widget2->ambientColor());
    ui->cmbBackground->setCurrentColor(ui->widget2->backgroundColor());
    ui->cmbBlend->setCurrentIndex((int)ui->widget2->blendMode());
    ui->cmbInterpolation->setCurrentIndex((int)ui->widget2->interpolationMode());

    ui->spinRotX->setValue(ui->widget2->xRotation());
    ui->spinRotY->setValue(ui->widget2->yRotation());
    ui->spinRotZ->setValue(ui->widget2->zRotation());
    connect(ui->widget2, SIGNAL(xRotationChanged(double)), ui->spinRotX, SLOT(setValue(double)));
    connect(ui->spinRotX, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setXRotation(double)));
    connect(ui->widget2, SIGNAL(yRotationChanged(double)), ui->spinRotY, SLOT(setValue(double)));
    connect(ui->spinRotY, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setYRotation(double)));
    connect(ui->widget2, SIGNAL(zRotationChanged(double)), ui->spinRotZ, SLOT(setValue(double)));
    connect(ui->spinRotZ, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setZRotation(double)));

    connect(ui->cmbBackground, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setBackgroundColor(QColor)));
    connect(ui->cmbAmbient, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setAmbientColor(QColor)));
    connect(ui->cmbBlend, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setBlendMode(int)));
    connect(ui->cmbInterpolation, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setInterpolationMode(int)));

    setCurrent(0);
    ui->cmbCurrentGeometry->setCurrentIndex(0);

    connect(ui->cmbCurrentGeometry, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrent(int)));


    ui->cmbBackground->setCurrentColor(QColor("darkgrey"));
    ui->cmbScene->setCurrentIndex(5);
    ui->cmbInterpolation->setCurrentIndex(2);

}

MainWindowGL::~MainWindowGL()
{
    delete ui;
    free(data3D);
    free(data3D2);
    free(data3D3);
    free(data3D4);
}

void MainWindowGL::update3DDataset()
{
    double dX=-1;
    double dY=-1;
    double dZ=-1;
    if (ui->cmbCurrentGeometry->currentIndex()==1) {
        dX=4.0/double(sX);
        dY=4.0/double(sY);
        dZ=4.0/double(sZ);
    }
    if (ui->cmbScene->currentIndex()==0) {
        ui->lutEditor->clearHistogram();
        ui->lutAlphaEditor->clearHistogram();
        ui->widget2->setTestGeometry();
    } else if (ui->cmbScene->currentIndex()==1) {
        ui->lutEditor->setHistogram(data3D, sX*sY*sZ);
        ui->lutAlphaEditor->setHistogram(data3D, sX*sY*sZ);
        ui->widget2->setCopiedVolumeData(data3D, sX, sY, sZ, QFVisualize3DGLWidget::TextureRendering, dX, dY, dZ);
    } else if (ui->cmbScene->currentIndex()==2) {
        ui->lutEditor->setHistogram(data3D2, sX*sY*sZ);
        ui->lutAlphaEditor->setHistogram(data3D2, sX*sY*sZ);
        ui->widget2->setCopiedVolumeData(data3D2, sX, sY, sZ, QFVisualize3DGLWidget::TextureRendering, dX, dY, dZ);
    } else if (ui->cmbScene->currentIndex()==3) {
        ui->lutEditor->setHistogram(data3D3, sX*sY*sZ);
        ui->lutAlphaEditor->setHistogram(data3D3, sX*sY*sZ);
        ui->widget2->setCopiedVolumeData(data3D3, sX, sY, sZ, QFVisualize3DGLWidget::TextureRendering, dX, dY, dZ);
    } else if (ui->cmbScene->currentIndex()==4) {
        ui->lutEditor->clearHistogram();
        ui->lutAlphaEditor->clearHistogram();
        ui->widget2->clearCurrentVolumeData();
    } else if (ui->cmbScene->currentIndex()==5) {
        ui->lutEditor->setHistogram(data3D4, ssX*ssY*ssZ);
        ui->lutAlphaEditor->setHistogram(data3D4, ssX*ssY*ssZ);
        ui->widget2->setCopiedVolumeData(data3D4, ssX, ssY, ssZ, QFVisualize3DGLWidget::TextureRendering, dX, dY, dZ);
    }
}

void MainWindowGL::setCurrent(int item)
{
    disconnect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), ui->lutEditor, SLOT(setColorPalette(JKQTPMathImageColorPalette)));
    disconnect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setLUT(QVector<int>)));
    disconnect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setAlphaLUT(QVector<int>)));
    disconnect(ui->spinAlphaLower, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentLowerThreshold(double)));
    disconnect(ui->spinAlphaUpper, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentUpperThreshold(double)));
    disconnect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(writeSettings()));
    disconnect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(writeSettings()));
    disconnect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(writeSettings()));
    disconnect(ui->cmbScene, SIGNAL(currentIndexChanged(int)), this, SLOT(writeSettings()));
    disconnect(ui->cmbScene, SIGNAL(currentIndexChanged(int)), this, SLOT(update3DDataset()));
    disconnect(ui->chkVisible, SIGNAL(toggled(bool)), ui->widget2, SLOT(setGeoVisible(bool)));

    while (item>=ui->widget2->getVolumeDataCount()) {
        ui->widget2->addVolumeData(NULL, 0, 0, 0);
        mapDataSet[ui->widget2->getVolumeDataCount()-1]=4;
    }
    ui->widget2->setCurrentVData(item);

    ui->cmbScene->setCurrentIndex(mapDataSet.value(item, 4));
    ui->cmbPalette->setColorPalette(mapColorPal.value(item, JKQTPMathImageGRAY));
    ui->lutEditor->setColorPalette(mapColorPal.value(item, JKQTPMathImageGRAY));
    ui->lutEditor->setConfig(mapLUT.value(item, ui->lutEditor->getDefaultConfig()));
    ui->lutAlphaEditor->setConfig(mapLUTAlpha.value(item, ui->lutAlphaEditor->getDefaultConfig()));

    ui->spinAlphaLower->setValue(ui->widget2->getTransparentLowerThreshold());
    ui->spinAlphaUpper->setValue(ui->widget2->getTransparentUpperThreshold());
    ui->chkVisible->setChecked(ui->widget2->getGeoVisible());


    connect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), ui->lutEditor, SLOT(setColorPalette(JKQTPMathImageColorPalette)));
    connect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setLUT(QVector<int>)));
    connect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setAlphaLUT(QVector<int>)));
    connect(ui->spinAlphaLower, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentLowerThreshold(double)));
    connect(ui->spinAlphaUpper, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentUpperThreshold(double)));
    connect(ui->chkBound, SIGNAL(toggled(bool)), ui->widget2, SLOT(setShowBounds(bool)));
    connect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(writeSettings()));
    connect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(writeSettings()));
    connect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(writeSettings()));
    connect(ui->cmbScene, SIGNAL(currentIndexChanged(int)), this, SLOT(writeSettings()));
    connect(ui->cmbScene, SIGNAL(currentIndexChanged(int)), this, SLOT(update3DDataset()));
    connect(ui->chkVisible, SIGNAL(toggled(bool)), ui->widget2, SLOT(setGeoVisible(bool)));

    update3DDataset();
}

void MainWindowGL::writeSettings()
{
    int item=ui->cmbCurrentGeometry->currentIndex();
    if (item>=0) {
        mapDataSet[item]=ui->cmbScene->currentIndex();
        mapColorPal[item]=ui->cmbPalette->currentColorPalette();
        mapLUT[item]=ui->lutEditor->getConfig();
        mapLUTAlpha[item]=ui->lutAlphaEditor->getConfig();
    }
}
