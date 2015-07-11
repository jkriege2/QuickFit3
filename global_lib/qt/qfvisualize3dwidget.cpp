#include "qfvisualize3dwidget.h"
#include <qglbuilder.h>
#include <qglscenenode.h>
#include "qglteapot.h"
#include "qplane3d.h"
#include "qvector3d.h"
#include <float.h>
#include "../../../LIB/trunk/statistics_tools.h"


QFVisualize3DWidget::QFVisualize3DWidget(QWidget *parent) :
    QGLView(parent)
{
    scene=NULL;

    lastMaxDir=0;
    lastDirPos=true;
    currentVData=-1;

    camera()->setCenter(QVector3D(0,0,0));
    camera()->setEye(QVector3D(sizeX,sizeY,sizeZ));
    camera()->setFieldOfView(60);    setTeapot();
}

QFVisualize3DWidget::~QFVisualize3DWidget()
{
    clearVolumeData();
    if (scene) delete scene;
}

int QFVisualize3DWidget::addVolumeData(double *data, int sX, int sY, int sZ)
{
    //qDebug()<<"setVolumeData";
    VolumeData d;
    d.internalData=false;
    d.volumeData=data;
    d.sizeX=sX;
    d.sizeY=sY;
    d.sizeZ=sZ;
    d.transparentLowerThreshold=0.2;
    d.transparentUpperThreshold=DBL_MAX;
    d.vizMode=QFVisualize3DWidget::TextureRendering;

    vdata.append(d);
    currentVData=vdata.size()-1;
    updateVolumeVisualization();
    return vdata.size()-1;

}

void QFVisualize3DWidget::setVolumeData(double *data, int sX, int sY, int sZ)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].internalData=false;
    vdata[currentVData].volumeData=data;
    vdata[currentVData].sizeX=sX;
    vdata[currentVData].sizeY=sY;
    vdata[currentVData].sizeZ=sZ;

    updateVolumeVisualization();

}

void QFVisualize3DWidget::deleteVolumeData(int i)
{
    if (currentVData==i) {
        if (currentVData>0) currentVData--;
        if (currentVData==0) currentVData;
    }
    vdata.removeAt(i);
    if (vdata.size()<=0) if (currentVData>0) currentVData=-1;
    updateVolumeVisualization();
}

void QFVisualize3DWidget::setTeapot()
{
    //qDebug()<<"setTeapot";
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].clear();
    vdata[currentVData].showTeapot=true;
    updateVolumeVisualization();
}

void QFVisualize3DWidget::clearVolumeData()
{

    for (int i=0; i<vdata.size(); i++) {
        vdata[i].clear();
    }
    vdata.clear();
    updateVolumeVisualization();
}

void QFVisualize3DWidget::setCurrentVolumeData(int i)
{
    currentVData=i;
}




void QFVisualize3DWidget::addStackNodeZ(QGLBuilder& builder, QRgb* lut, int lutSize, double lmin, double lmax, bool posDir) {
    int z0=sizeZ-1;
    int z1=-1;
    int dz=-1;
    if (!posDir) {
        z0=0;
        z1=sizeZ;
        dz=+1;
    }
    qDebug()<<"nodesZ";
    for (int z=z0; z!=z1; z=z+dz) {
        //qDebug()<<"Z"<<z;
        QGLSceneNode* nodePlane=builder.newNode();
        nodePlane->setObjectName(QString("PlanePZ%1").arg(z));

        float xx=arrayX2WorldX(sizeX);
        float yy=arrayY2WorldY(sizeY);
        float zz=arrayZ2WorldZ(z);



        QGeometryData dataPlane;
        dataPlane.appendTexCoord(QVector2D(0,0));
        dataPlane.appendVertex(QVector3D(-xx,-yy,zz));
        dataPlane.appendTexCoord(QVector2D(1,0));
        dataPlane.appendVertex(QVector3D( xx,-yy,zz));
        dataPlane.appendTexCoord(QVector2D(1,1));
        dataPlane.appendVertex(QVector3D(xx,yy,zz));
        dataPlane.appendTexCoord(QVector2D(0,1));
        dataPlane.appendVertex(QVector3D(-xx,yy,zz));

        builder.addQuads(dataPlane);

        QImage imgPlane(QSize(sizeX, sizeY), QImage::Format_ARGB32);

        for (int y=0; y<sizeY; y++) {
            QRgb* line=(QRgb*)imgPlane.scanLine(y);
            for (int x=0; x<sizeX; x++) {
                line[x]=statisticsLookupLUT(volumeData[z*sizeX*sizeY+y*sizeX+x], lmin, lmax, lut, lutSize);
            }
        }


        QGLMaterial *mat = new QGLMaterial;
        mat->setColor(Qt::green);
        QGLTexture2D* tex=new QGLTexture2D;
        tex->setImage(imgPlane);
        mat->setTexture(tex);
        nodePlane->setMaterial(mat);
        nodePlane->setEffect(QGL::FlatReplaceTexture2D);

    }

}


void QFVisualize3DWidget::addStackNodeX(QGLBuilder &builder, QRgb *lut, int lutSize, double lmin, double lmax, bool posDir)
{
    int x0=sizeX-1;
    int x1=-1;
    int dx=-1;
    if (!posDir) {
        x0=0;
        x1=sizeX;
        dx=+1;
    }
    qDebug()<<"nodesX";
    for (int x=x0; x!=x1; x=x+dx) {
        //qDebug()<<"X"<<x;
        QGLSceneNode* nodePlane=builder.newNode();
        nodePlane->setObjectName(QString("PlanePX%1").arg(x));

        float xx=arrayX2WorldX(x);
        float yy=arrayY2WorldY(sizeY);
        float zz=arrayZ2WorldZ(sizeZ);



        QGeometryData dataPlane;
        dataPlane.appendTexCoord(QVector2D(0,0));
        dataPlane.appendVertex(QVector3D(xx,-yy,-zz));
        dataPlane.appendTexCoord(QVector2D(1,0));
        dataPlane.appendVertex(QVector3D( xx,yy,-zz));
        dataPlane.appendTexCoord(QVector2D(1,1));
        dataPlane.appendVertex(QVector3D(xx,yy,zz));
        dataPlane.appendTexCoord(QVector2D(0,1));
        dataPlane.appendVertex(QVector3D(xx,-yy,zz));

        builder.addQuads(dataPlane);

        QImage imgPlane(QSize(sizeY, sizeZ), QImage::Format_ARGB32);

        for (int z=0; z<sizeZ; z++) {
            QRgb* line=(QRgb*)imgPlane.scanLine(z);
            for (int y=0; y<sizeY; y++) {
                line[y]=statisticsLookupLUT(volumeData[z*sizeX*sizeY+y*sizeX+x], lmin, lmax, lut, lutSize);
            }
        }


        QGLMaterial *mat = new QGLMaterial;
        mat->setColor(Qt::green);
        QGLTexture2D* tex=new QGLTexture2D;
        tex->setImage(imgPlane);
        mat->setTexture(tex);
        nodePlane->setMaterial(mat);
        nodePlane->setEffect(QGL::FlatReplaceTexture2D);

    }
}

void QFVisualize3DWidget::addStackNodeY(QGLBuilder &builder, QRgb *lut, int lutSize, double lmin, double lmax, bool posDir)
{
    int y0=sizeY-1;
    int y1=-1;
    int dy=-1;
    if (!posDir) {
        y0=0;
        y1=sizeY;
        dy=+1;
    }
    qDebug()<<"nodesY";
    for (int y=y0; y!=y1; y=y+dy) {
        //qDebug()<<"Y"<<y;
        QGLSceneNode* nodePlane=builder.newNode();
        nodePlane->setObjectName(QString("PlanePY%1").arg(y));

        float xx=arrayX2WorldX(sizeX);
        float yy=arrayY2WorldY(y);
        float zz=arrayZ2WorldZ(sizeZ);



        QGeometryData dataPlane;
        dataPlane.appendTexCoord(QVector2D(0,0));
        dataPlane.appendVertex(QVector3D(-xx,yy,-zz));
        dataPlane.appendTexCoord(QVector2D(1,0));
        dataPlane.appendVertex(QVector3D(xx,yy,-zz));
        dataPlane.appendTexCoord(QVector2D(1,1));
        dataPlane.appendVertex(QVector3D(xx,yy,zz));
        dataPlane.appendTexCoord(QVector2D(0,1));
        dataPlane.appendVertex(QVector3D(-xx,yy,zz));

        builder.addQuads(dataPlane);

        QImage imgPlane(QSize(sizeX, sizeZ), QImage::Format_ARGB32);

        for (int z=0; z<sizeZ; z++) {
            QRgb* line=(QRgb*)imgPlane.scanLine(z);
            for (int x=0; x<sizeX; x++) {
                line[x]=statisticsLookupLUT(volumeData[z*sizeX*sizeY+y*sizeX+x], lmin, lmax, lut, lutSize);
            }
        }


        QGLMaterial *mat = new QGLMaterial;
        mat->setColor(Qt::green);
        QGLTexture2D* tex=new QGLTexture2D;
        tex->setImage(imgPlane);
        mat->setTexture(tex);
        nodePlane->setMaterial(mat);
        nodePlane->setEffect(QGL::FlatReplaceTexture2D);

    }
}

void QFVisualize3DWidget::updateVolumeVisualization(bool updateWidget)
{
    //qDebug()<<"updateVolumeVisualization volumeData="<<volumeData<<"  size = ("<<sizeX<<" x "<<sizeY<<" x "<<sizeZ<<")  scene="<<scene;
    QGLSceneNode * s=scene;
    scene=NULL;
    if (scene) delete scene;
    int lutSize=256;
    findMaxViewDir(lastMaxDir, lastDirPos);


    QGLBuilder builder;
    if (showTeapot) {
        builder << QGLTeapot();
    } else if (volumeData && sizeX>0 && sizeY>0 && sizeZ>0) {
        if (vizMode==QFVisualize3DWidget::TextureRendering) {
            //qDebug()<<"  bulding scene size = ("<<sizeX<<" x "<<sizeY<<" x "<<sizeZ<<")";

            if (this->lut.size()>0) {
                lutSize=this->lut.size();
            }
            QRgb* lut=(QRgb*)malloc(lutSize*sizeof(QRgb));
            //qDebug()<<"  malloc = "<<lutSize<<" = "<<lutSize*sizeof(QRgb)<<"bytes";

            double lmin=0, lmax=0;
            statisticsMinMax(volumeData, sizeX*sizeY*sizeZ, lmin, lmax);
            //qDebug()<<"  range = "<<lmin<<"..."<<lmax;
            //qDebug()<<"  transparentLowerThreshold = "<<transparentLowerThreshold;
            //qDebug()<<"  transparentUpperThreshold = "<<transparentUpperThreshold;
            for (int i=0; i<lutSize; i++) {
                const double val1=double(i)/double(lutSize-1);
                const double val=lmin+val1*(lmax-lmin);
                if (double(i)/double(lutSize-1)<transparentLowerThreshold) lut[i]=QColor(Qt::transparent).rgba();
                else if (double(i)/double(lutSize-1)>transparentUpperThreshold) lut[i]=QColor(Qt::transparent).rgba();
                else {
                    if (this->lut.size()==lutSize && this->lut_alpha.size()==lutSize) {
                        lut[i]=qRgba(qRed(this->lut[i]), qGreen(this->lut[i]), qBlue(this->lut[i]), this->lut_alpha[i]);
                        if (this->lut_alpha[i]==0) lut[i]=QColor(Qt::transparent).rgba();
                    } else if (this->lut.size()==lutSize) {
                        lut[i]=qRgba(qRed(this->lut[i]), qGreen(this->lut[i]), qBlue(this->lut[i]), val1*127.0+127.0);
                    } else {
                        lut[i]=qRgba(val1*255.0, val1*255.0, val1*255.0, val1*127.0+127.0);
                    }
                }
                //qDebug()<<QString("lut[%2] = 0x%1").arg(lut[i],8,16,QLatin1Char('0')).arg(i, 3, 10, QLatin1Char(' '));
            }

            //qDebug()<<"lastMaxDir="<<lastMaxDir<<"  lastDirPos="<<lastDirPos;


            switch(lastMaxDir) {
                case 0: // X-direction
                    addStackNodeX(builder, lut, lutSize, lmin, lmax, lastDirPos);
                    break;
                case 1: // Y-direction
                    addStackNodeY(builder, lut, lutSize, lmin, lmax, lastDirPos);
                    break;
                default:
                case 2: // Z-direction
                    addStackNodeZ(builder, lut, lutSize, lmin, lmax, lastDirPos);
                    break;
            }

            free(lut);
        }






    }
    scene = builder.finalizedSceneNode();

    //qDumpScene(scene);


    if (updateWidget) updateGL();
}

void QFVisualize3DWidget::setVisualizationMode(QFVisualize3DWidget::VisualizationMode mode)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;

    if (vdata[currentVData].vizMode!=mode) {
        vdata[currentVData].vizMode=mode;
        updateVolumeVisualization();
    }
}

void QFVisualize3DWidget::setLUT(const QVector<int> &lut)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].lut=lut;
    updateVolumeVisualization();
}

void QFVisualize3DWidget::setAlphaLUT(const QVector<int> &lut)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].lut_alpha=lut;
    updateVolumeVisualization();
    //qDebug()<<"alpha: "<<lut.first()<<" ... "<<lut.last();
}

void QFVisualize3DWidget::setTransparentLowerThreshold(double val)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].transparentLowerThreshold=val;
    updateVolumeVisualization();
}

void QFVisualize3DWidget::setTransparentUpperThreshold(double val)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].transparentUpperThreshold=val;
    updateVolumeVisualization();
}

void QFVisualize3DWidget::initializeGL(QGLPainter *painter)
{
    //qDebug()<<"initializeGL";
    //painter->setStandardEffect(QGL::LitMaterial);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

}

void QFVisualize3DWidget::paintGL(QGLPainter *painter)
{
    //qDebug()<<"paintGL view="<<(camera()->center()-camera()->eye());

    if (!showTeapot) {
        int maxDir=0;
        bool dirPos=true;
        findMaxViewDir(maxDir, dirPos);
        if (lastMaxDir!=maxDir || lastDirPos!=dirPos) {
            updateVolumeVisualization(false);
            //qDebug()<<lastMaxDir<<lastDirPos;
        }
    }

    if (scene) scene->draw(painter);
}

void QFVisualize3DWidget::findMaxViewDir(int &maxDir, bool &dirPos)
{
    QVector3D vd=camera()->center()-camera()->eye();
    QVector3D vabsd;
    vabsd.setX(fabs(vd.x()));
    vabsd.setY(fabs(vd.y()));
    vabsd.setZ(fabs(vd.z()));
    maxDir=0;
    dirPos=(vd.x()>0);
    //qDebug()<<vabsd.x()<<vabsd.y()<<vabsd.z();
    if (vabsd.x()>=vabsd.y() && vabsd.x()>=vabsd.z()) {
        maxDir=0;
        dirPos=(vd.x()>0);
    } else if (vabsd.y()>=vabsd.x() && vabsd.y()>=vabsd.z()) {
        maxDir=1;
        dirPos=(vd.y()>0);
    } else if (vabsd.z()>=vabsd.x() && vabsd.z()>=vabsd.y()) {
        maxDir=2;
        dirPos=(vd.z()>0);
    }
}



void QFVisualize3DWidget::VolumeData::clear()
{
    if (internalData && volumeData && sizeX>0 && sizeY>0 && sizeZ>0) {
        free(volumeData);
    }
    showTeapot=false;
    internalData=false;
    volumeData=NULL;
    sizeX=sizeY=sizeZ=0;
}
