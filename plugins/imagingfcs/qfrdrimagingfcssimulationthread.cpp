#include "qfrdrimagingfcssimulationthread.h"
#include <QApplication>
#include <QElapsedTimer>
#include "MersenneTwister.h"
#include <QFile>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QFileInfo>


inline double sqr(const double& x) { return x*x; }

QFRDRImagingFCSSimulationThread::QFRDRImagingFCSSimulationThread(QObject *parent) :
    QThread(parent),
    rng()
{
    canceled=false;
    brightnessG=100;
    brightnessR=100;
    DG=DR=DRG=5;
    VX=VY=0;
    psf_size=300;
    pixel_size=400;
    frames=10000;
    frametime=500;
    dualView=false;
    width=10;
    height=10;
    walkersG=10;
    walkersR=0;
    walkersRG=0;
}

int QFRDRImagingFCSSimulationThread::getCurrentFrame() const
{
    return currentFrame;
}

void QFRDRImagingFCSSimulationThread::cancel()
{
    canceled=true;
}

void QFRDRImagingFCSSimulationThread::waitForFinish()
{
    while (isRunning()) {
        QApplication::processEvents();
    }
}

void QFRDRImagingFCSSimulationThread::run()
{
    QElapsedTimer timer;
    timer.start();
    emit statusMessage(tr("initialising simulation ..."));

    if (filename.contains("%counter%")) {
        int cnt=0;
        QString fn=filename.replace("%counter%", QString::number(cnt));
        while (QFile::exists(fn)) {
            cnt++;
            fn=filename.replace("%counter%", QString::number(cnt));
        }
        filename=fn;
    }

    QString inifilename=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".configuration.ini";
    QSettings config(inifilename, QSettings::IniFormat);
    config.setValue("acquisition/pixel_width", pixel_size/1000.0);
    config.setValue("acquisition/pixel_height", pixel_size/1000.0);
    config.setValue("acquisition/magnification", 1);
    config.setValue("acquisition/images", qlonglong(frames));
    config.setValue("acquisition/image_height", height);
    config.setValue("acquisition/image_width", width);
    config.setValue("acquisition/type", tr("2D random walk simulation"));
    config.setValue("acquisition/frame_time", frametime*1.0e-6);
    config.setValue("files/count", 1);
    config.setValue("files/name0", QFileInfo(filename).fileName());
    config.setValue("files/type0", "TIFF16");
    config.setValue("files/description0", tr("simulation result"));

    TinyTIFFFile* tif=TinyTIFFWriter_open(filename.toLatin1().data(), 16, width, height);
    int framesize=width*height;
    int realwidth=width;
    if (dualView) {
        realwidth=2*width;
        framesize=2*width*height;
    }
    uint16_t* frame=(uint16_t*)malloc(framesize*sizeof(uint16_t));
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wg=createWalkers(walkersG);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wr=createWalkers(walkersR);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wrg=createWalkers(walkersRG);

    if (tif) {
        emit statusMessage(tr("running simulation ..."));
        for (currentFrame=0; currentFrame<frames; currentFrame++) {

            memset(frame, 0, framesize*sizeof(uint16_t));

            propagateWalkers(wg, DG);
            for (int i=0; i<wg.size(); i++) {
                for (int y=0; y<height; y++) {
                    for (int x=0; x<width; x++) {
                        frame[y*realwidth+x]=frame[y*realwidth+x]+brightnessG*exp(-2.0*(sqr(wg[i].x-x*pixel_size)+sqr(wg[i].y-y*pixel_size))/sqr(psf_size));
                    }
                }
            }
            if (dualView) {
                propagateWalkers(wr, DR);
                propagateWalkers(wrg, DRG);
            }



            TinyTIFFWriter_writeImage(tif, frame);
            if (timer.elapsed()>200) emit progress(currentFrame);
            if (canceled) break;
        }
        emit statusMessage(tr("simulation finished ..."));
        TinyTIFFWriter_close(tif);
    }
    free(frame);
}

QVector<QFRDRImagingFCSSimulationThread::WalkerData> QFRDRImagingFCSSimulationThread::createWalkers(int count)
{
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> res;
    for (int i=0; i<count; i++) {
        QFRDRImagingFCSSimulationThread::WalkerData d;
        d.x=rng.rand(pixel_size*double(width-1));
        d.y=rng.rand(pixel_size*double(height-1));
        res<<d;
    }
    return res;
}

void QFRDRImagingFCSSimulationThread::propagateWalkers(QVector<QFRDRImagingFCSSimulationThread::WalkerData> &walkersv, double D)
{
    for (int i=0; i<walkersv.size(); i++) {
        QFRDRImagingFCSSimulationThread::WalkerData d=walkersv[i];

        double dx=d.x+rng.randNorm(0.0, 2.0*D*frametime*1.0e-6)+VX*frametime*1.0e-6;
        double dy=d.y+rng.randNorm(0.0, 2.0*D*frametime*1.0e-6)+VY*frametime*1.0e-6;

        if ((d.x<0) || (d.x>double(width-1)*pixel_size) || (d.y<0) || (d.y>double(height-1)*pixel_size)) {
            dx=0;
            dy=0;
        } else {
            d.x=dx;
            d.y=dy;
        }

        walkersv[i]=d;
    }
}
