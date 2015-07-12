#include "mainwindowgl.h"
#include <QApplication>
#include "qfvisualize3dgleditor.h"
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*MainWindowGL w;
    w.show();*/


    int sX=100;
    int sY=100;
    int sZ=100;
    int ssX=5;
    int ssY=5;
    int ssZ=5;

    double wx=50;
    double wy=30;
    double wz=50;
    double wr=20;

    double* data3D=(double*)malloc(sX*sY*sZ*sizeof(double));
    double* data3D2=(double*)malloc(sX*sY*sZ*sizeof(double));
    double* data3D3=(double*)malloc(sX*sY*sZ*sizeof(double));
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
                data3D3[z*sX*sY+y*sX+x]=data3D2[z*sX*sY+y*sX+x]*5.0+2.0;
                if (x>sX/2)data3D3[z*sX*sY+y*sX+x]=0;
                //if (z==0) data3D[z*sX*sY+y*sX+x]=1;
                //if (z==sZ-1) data3D[z*sX*sY+y*sX+x]=0.5;
            }
        }
    }

    double* data3D4=(double*)malloc(ssX*ssY*ssZ*sizeof(double));
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

    data3D[0]=3;

    QFVisualize3DGLEditor edt;
    edt.setDefaultPalettes(JKQTPMathImageMATLAB, JKQTPMathImageGREEN);
    edt.addCopiedVolumeData(data3D, sX, sY, sZ, 0.01, 0.01, 0.01, "geometry I");
    edt.addCopiedVolumeData(data3D3, sX, sY, sZ, 0.02, 0.02, 0.02, "geometry II");

    QSettings set("viz3d.ini", QSettings::IniFormat);
    edt.readSettings(set);
    edt.setConfig(set.value("config", "").toString());

    edt.show();

    qDebug()<<(8*sizeof(void*))<<" bit binary\n\n";

    return a.exec();
}
