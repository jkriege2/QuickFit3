#include "qfrdrimagingfcssimulationthread.h"
#include <QApplication>
#include <QElapsedTimer>
#include "MersenneTwister.h"
#include <QFile>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QFileInfo>
#include <QDebug>

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
    psf_size_g=500;
    psf_size_r=550;
    pixel_size=400;
    frames=20000;
    frametime=500;
    dualView=false;
    width=10;
    height=10;
    walkersG=10;
    walkersR=0;
    walkersRG=0;
    background=100;
    backgroundNoise=2;
    crosstalk=5.0/100.0;
    warmup=10000;

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
        QString fn=filename;
        fn=fn.replace("%counter%", QString::number(cnt));
        while (QFile::exists(fn)) {
            cnt++;
            fn=filename;
            fn=fn.replace("%counter%", QString::number(cnt));
        }
        filename=fn;
    }

    QString inifilename=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".configuration.ini";
    QString filenameBackground=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".background.tif";
    QSettings config(inifilename, QSettings::IniFormat);
    config.setValue("acquisition/pixel_width", pixel_size);
    config.setValue("acquisition/pixel_height", pixel_size);
    config.setValue("acquisition/magnification", 1);
    config.setValue("acquisition/images", qlonglong(frames));
    config.setValue("acquisition/image_height", height);
    config.setValue("acquisition/image_width", width);
    config.setValue("acquisition/type", tr("simulation"));
    config.setValue("acquisition/frame_time", frametime*1.0e-6);
    config.setValue("acquisition/frame_rate", 1/(frametime*1.0e-6));
    config.setValue("acquisition/baseline_offset", 0);
    if (dualView) config.setValue("acquisition/dualview_mode", "h");
    else config.setValue("acquisition/dualview_mode", "none");
    config.setValue("files/count", 2);
    config.setValue("files/name0", QFileInfo(filename).fileName());
    config.setValue("files/type0", "TIFF16");
    config.setValue("files/description0", tr("simulation result"));
    config.setValue("files/name1", QFileInfo(filenameBackground).fileName());
    config.setValue("files/type1", "TIFF16");
    config.setValue("files/description1", tr("background"));

    config.setValue("simulation/type", tr("2D random walk simulation"));
    config.setValue("simulation/DG", DG);
    config.setValue("simulation/DR", DR);
    config.setValue("simulation/DRG", DRG);
    config.setValue("simulation/VX", VX);
    config.setValue("simulation/VY", VY);
    config.setValue("simulation/walkersG", walkersG);
    config.setValue("simulation/walkersR", walkersR);
    config.setValue("simulation/walkersRG", walkersRG);
    config.setValue("simulation/dualView", dualView);
    config.setValue("simulation/brightnessG", brightnessG);
    config.setValue("simulation/brightnessR", brightnessR);
    config.setValue("simulation/pixel_size", pixel_size);
    config.setValue("simulation/psf_size_green", psf_size_g);
    config.setValue("simulation/psf_size_red", psf_size_r);
    config.setValue("simulation/crosstalk", crosstalk);
    config.setValue("simulation/warmup", (qlonglong)warmup);
    config.setValue("simulation/background", background);
    config.setValue("simulation/backgroundNoise", backgroundNoise);

    int framesize=width*height;
    int realwidth=width;
    if (dualView) {
        realwidth=2*width;
        framesize=2*width*height;
    }
    TinyTIFFFile* tif=TinyTIFFWriter_open(filename.toLatin1().data(), 16, realwidth, height);
    uint16_t* frame=(uint16_t*)malloc(framesize*sizeof(uint16_t));
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wg=createWalkers(walkersG);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wr=createWalkers(walkersR);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wrg=createWalkers(walkersRG);
    TinyTIFFFile* tifBack=TinyTIFFWriter_open(filenameBackground.toLatin1().data(), 16, realwidth, height);
    if (tifBack) {
        emit statusMessage(tr("creating background files ..."));
        for (currentFrame=0; currentFrame<50; currentFrame++) {

            //memset(frame, 0, framesize*sizeof(uint16_t));
            for (int i=0; i<framesize; i++) {
                frame[i]=round(rng.randNorm(background, backgroundNoise*backgroundNoise));
            }
            TinyTIFFWriter_writeImage(tifBack, frame);
        }
         TinyTIFFWriter_close(tifBack);
    }

    if (tif) {
        emit statusMessage(tr("warming up simulation ..."));
        for (int wi=0; wi<warmup; wi++) {
            propagateWalkers(wg, DG);
            if (dualView) {
                propagateWalkers(wr, DR);
                propagateWalkers(wrg, DRG);
            }
            if (timer.elapsed()>200) {
                emit progress(currentFrame+warmup);
                timer.start();
            }
            if (canceled) break;
        }
        emit statusMessage(tr("running simulation ..."));
        if (!canceled) {
            for (currentFrame=0; currentFrame<frames; currentFrame++) {
                for (int i=0; i<framesize; i++) {
                    frame[i]=0;
                }

                //memset(frame, 0, framesize*sizeof(uint16_t));
                propagateWalkers(wg, DG);
                for (int i=0; i<wg.size(); i++) {
                    //if (i==0) qDebug()<<wg[i].x<<", "<<wg[i].y;
                    for (int y=0; y<height; y++) {
                        for (int x=0; x<width; x++) {
                            frame[y*realwidth+x]=frame[y*realwidth+x]+brightnessG*exp(-2.0*(sqr(wg[i].x-double(x)*pixel_size)+sqr(wg[i].y-double(y)*pixel_size))/sqr(psf_size_g));
                        }
                    }
                }
                if (dualView) {
                    propagateWalkers(wr, DR);
                    propagateWalkers(wrg, DRG);
                    for (int i=0; i<wr.size(); i++) {
                        for (int y=0; y<height; y++) {
                            for (int x=0; x<width; x++) {
                                frame[y*realwidth+x+width]=frame[y*realwidth+x+width]+brightnessR*exp(-2.0*(sqr(wr[i].x-double(x)*pixel_size)+sqr(wr[i].y-double(y)*pixel_size))/sqr(psf_size_r));
                            }
                        }
                    }
                    for (int i=0; i<wrg.size(); i++) {
                        for (int y=0; y<height; y++) {
                            for (int x=0; x<width; x++) {
                                frame[y*realwidth+x+width]=frame[y*realwidth+x+width]+brightnessR*exp(-2.0*(sqr(wrg[i].x-double(x)*pixel_size)+sqr(wrg[i].y-double(y)*pixel_size))/sqr(psf_size_r));
                                frame[y*realwidth+x]=frame[y*realwidth+x]+brightnessG*exp(-2.0*(sqr(wrg[i].x-double(x)*pixel_size)+sqr(wrg[i].y-double(y)*pixel_size))/sqr(psf_size_g));
                            }
                        }
                    }
                    if (crosstalk>0) {
                        for (int y=0; y<height; y++) {
                            for (int x=0; x<width; x++) {
                                frame[y*realwidth+x+width]=frame[y*realwidth+x+width]+crosstalk*frame[y*realwidth+x];
                            }
                        }
                    }
                }

                for (int i=0; i<framesize; i++) {
                    frame[i]=frame[i]+round(rng.randNorm(background, backgroundNoise*backgroundNoise));
                }


                TinyTIFFWriter_writeImage(tif, frame);
                if (timer.elapsed()>200) {
                    emit progress(currentFrame+warmup);
                    timer.start();
                }
                if (canceled) break;
            }
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

        double dx=d.x+rng.randNorm(0.0, 1.0)*sqrt(2.0*D*frametime*1.0e-6)+VX*frametime*1.0e-6;
        double dy=d.y+rng.randNorm(0.0, 1.0)*sqrt(2.0*D*frametime*1.0e-6)+VY*frametime*1.0e-6;

        /*if ((d.x<0) || (d.x>double(width-1)*pixel_size) || (d.y<0) || (d.y>double(height-1)*pixel_size)) {
            dx=0;
            dy=0;
        } else {
            d.x=dx;
            d.y=dy;
        }*/
        if (dx<-4.0*pixel_size) dx=double(width+3)*pixel_size;
        if (dx>(width+3)*pixel_size) dx=-4.0*pixel_size;
        if (dy<-4.0*pixel_size) dy=double(height+3)*pixel_size;
        if (dy>(height+3)*pixel_size) dy=-4.0*pixel_size;

        d.x=dx;
        d.y=dy;

        walkersv[i]=d;
    }
}
