#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{

    sX=100;
    sY=100;
    sZ=100;

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



    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(0,0);
    QList<int> widths;
    widths<<width()*3/4<<width()/4;
    ui->splitter->setSizes(widths);

    ui->cmbScene->setCurrentIndex(1);
    ui->cmbPalette->setColorPalette(JKQTPMathImageGRAY);
    ui->lutAlphaEditor->setAlphaMode(true);
    ui->spinAlphaLower->setRange(0,1);
    ui->spinAlphaUpper->setRange(0,1);
    ui->spinAlphaLower->setValue(ui->widget->getTransparentLowerThreshold());
    ui->spinAlphaUpper->setValue(ui->widget->getTransparentUpperThreshold());
    ui->spinAlphaLower->setValue(ui->widget2->getTransparentLowerThreshold());
    ui->spinAlphaUpper->setValue(ui->widget2->getTransparentUpperThreshold());
    ui->cmbAmbient->setCurrentColor(ui->widget2->ambientColor());
    ui->cmbBackground->setCurrentColor(ui->widget2->backgroundColor());
    connect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), ui->lutEditor, SLOT(setColorPalette(JKQTPMathImageColorPalette)));
    connect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget, SLOT(setLUT(QVector<int>)));
    connect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget, SLOT(setAlphaLUT(QVector<int>)));
    connect(ui->spinAlphaLower, SIGNAL(valueChanged(double)), ui->widget, SLOT(setTransparentLowerThreshold(double)));
    connect(ui->spinAlphaUpper, SIGNAL(valueChanged(double)), ui->widget, SLOT(setTransparentUpperThreshold(double)));
    connect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setLUT(QVector<int>)));
    connect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setAlphaLUT(QVector<int>)));
    connect(ui->spinAlphaLower, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentLowerThreshold(double)));
    connect(ui->spinAlphaUpper, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentUpperThreshold(double)));
    connect(ui->chkBound, SIGNAL(toggled(bool)), ui->widget2, SLOT(setShowBounds(bool)));
    connect(ui->cmbBackground, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setBackgroundColor(QColor)));
    connect(ui->cmbAmbient, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setAmbientColor(QColor)));
    update3DDataset();
}

MainWindow::~MainWindow()
{
    delete ui;
    free(data3D);
    free(data3D2);
    free(data3D3);
}

void MainWindow::update3DDataset()
{
    if (ui->cmbScene->currentIndex()==0) {
        ui->lutEditor->clearHistogram();
        ui->lutAlphaEditor->clearHistogram();
        ui->widget->setTeapot();
        ui->widget2->setTestGeometry();
    } else if (ui->cmbScene->currentIndex()==1) {
        ui->lutEditor->setHistogram(data3D, sX*sY*sZ);
        ui->lutAlphaEditor->setHistogram(data3D, sX*sY*sZ);
        ui->widget->addVolumeData(data3D, sX, sY, sZ);
        ui->widget2->setVolumeData(data3D, sX, sY, sZ);
    } else if (ui->cmbScene->currentIndex()==2) {
        ui->lutEditor->setHistogram(data3D2, sX*sY*sZ);
        ui->lutAlphaEditor->setHistogram(data3D2, sX*sY*sZ);
        ui->widget->addVolumeData(data3D2, sX, sY, sZ);
        ui->widget2->setVolumeData(data3D2, sX, sY, sZ);
    } else if (ui->cmbScene->currentIndex()==3) {
        ui->lutEditor->setHistogram(data3D3, sX*sY*sZ);
        ui->lutAlphaEditor->setHistogram(data3D3, sX*sY*sZ);
        ui->widget->addVolumeData(data3D3, sX, sY, sZ);
        ui->widget2->setVolumeData(data3D3, sX, sY, sZ);
    }
}
