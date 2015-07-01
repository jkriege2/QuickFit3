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
#include "qftools.h"


inline float sqr(const float& x) { return x*x; }

QFRDRImagingFCSSimulationThread::QFRDRImagingFCSSimulationThread(QObject *parent) :
    QThread(parent),
    rng()
{
    canceled=false;
    brightnessG=100;
    brightnessR=100;
    brightnessG2=100;
    brightnessR2=100;
    DG=DR=DRG=5;
    DG2=DR2=DRG2=15;
    FlowEeverywhere=true;
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
    walkersG2=10;
    walkersR2=0;
    walkersRG2=0;
    background=100;
    backgroundNoise=2;
    crosstalk=5.0/100.0;
    warmup=10000;
    deltax=deltay=0;
    onlyHalf_DG=onlyHalf_DR=onlyHalf_DRG=onlyHalf_DG2=onlyHalf_DR2=onlyHalf_DRG2=false;

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


    filename=filename.replace("%timestep%", tr("%1uS").arg(frametime));
    filename=filename.replace("%type%", tr("2DRWsim"));
    filename=filename.replace("%D1r%", tr("%1").arg(DR));
    filename=filename.replace("%D1g%", tr("%1").arg(DG));
    filename=filename.replace("%D1gr%", tr("%1").arg(DRG));
    filename=filename.replace("%D1rg%", tr("%1").arg(DRG));
    filename=filename.replace("%D2r%", tr("%1").arg(DR2));
    filename=filename.replace("%D2g%", tr("%1").arg(DG2));
    filename=filename.replace("%D2gr%", tr("%1").arg(DRG2));
    filename=filename.replace("%D2rg%", tr("%1").arg(DRG2));
    if (dualView) {
        if (deltax!=0 || deltay!=0) filename=filename.replace("%shift%", tr("shift_%1_%2").arg(deltax).arg(deltay));
        else filename=filename.replace("%shift%", tr("noshift"));
        filename=filename.replace("%dvmode%", tr("lrdualview"));
    } else {
        filename=filename.replace("%dvmode%", tr("nodualview"));
    }
    if (VX!=0 || VY!=0) filename=filename.replace("%flow%", tr("flow_%1_%2").arg(VX).arg(VY));
    else filename=filename.replace("%flow%", tr("noflow"));




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
    config.setValue("files/description0", tr("simulation result (image series)"));
    config.setValue("files/name1", QFileInfo(filenameBackground).fileName());
    config.setValue("files/type1", "TIFF16");
    config.setValue("files/description1", tr("background image series"));

    config.setValue("simulation/type", tr("2D random walk simulation"));
    config.setValue("simulation/DG", DG);
    config.setValue("simulation/DR", DR);
    config.setValue("simulation/DRG", DRG);
    config.setValue("simulation/DG2", DG2);
    config.setValue("simulation/DR2", DR2);
    config.setValue("simulation/DRG2", DRG2);
    config.setValue("simulation/DeltaX", deltax);
    config.setValue("simulation/DeltaY", deltay);
    config.setValue("simulation/VX", VX);
    config.setValue("simulation/VY", VY);
    config.setValue("simulation/FlowEeverywhere", FlowEeverywhere);
    config.setValue("simulation/walkersG", walkersG);
    config.setValue("simulation/walkersR", walkersR);
    config.setValue("simulation/walkersRG", walkersRG);
    config.setValue("simulation/walkersG2", walkersG2);
    config.setValue("simulation/walkersR2", walkersR2);
    config.setValue("simulation/walkersRG2", walkersRG2);
    config.setValue("simulation/dualView", dualView);
    config.setValue("simulation/brightnessG", brightnessG);
    config.setValue("simulation/brightnessR", brightnessR);
    config.setValue("simulation/brightnessG2", brightnessG2);
    config.setValue("simulation/brightnessR2", brightnessR2);
    config.setValue("simulation/pixel_size", pixel_size);
    config.setValue("simulation/psf_size_green", psf_size_g);
    config.setValue("simulation/psf_size_red", psf_size_r);
    config.setValue("simulation/crosstalk", crosstalk);
    config.setValue("simulation/warmup", (qlonglong)warmup);
    config.setValue("simulation/background", background);
    config.setValue("simulation/backgroundNoise", backgroundNoise);
    config.setValue("simulation/onlyHalf_DG", onlyHalf_DG);
    config.setValue("simulation/onlyHalf_DG2", onlyHalf_DG2);
    config.setValue("simulation/onlyHalf_DR", onlyHalf_DR);
    config.setValue("simulation/onlyHalf_DR2", onlyHalf_DR2);
    config.setValue("simulation/onlyHalf_DRG", onlyHalf_DRG);
    config.setValue("simulation/onlyHalf_DRG2", onlyHalf_DRG2);

    int framesize=width*height;
    int realwidth=width;
    if (dualView) {
        realwidth=2*width;
        framesize=2*width*height;
    }
    const float nvar=backgroundNoise*backgroundNoise;

    TinyTIFFFile* tif=TinyTIFFWriter_open(filename.toLatin1().data(), 16, realwidth, height);
    uint16_t* frame=(uint16_t*)qfMalloc(framesize*sizeof(uint16_t));
    float* framef=(float*)qfMalloc(framesize*sizeof(float));
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wg=createWalkers(walkersG, onlyHalf_DG);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wr=createWalkers(walkersR, onlyHalf_DR);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wrg=createWalkers(walkersRG, onlyHalf_DRG);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wg2=createWalkers(walkersG2, onlyHalf_DG2);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wr2=createWalkers(walkersR2, onlyHalf_DR2);
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wrg2=createWalkers(walkersRG2, onlyHalf_DRG2);
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
            propagateWalkers(wg, DG,onlyHalf_DG);
            propagateWalkers(wg2, DG2,onlyHalf_DG2);
            if (dualView) {
                propagateWalkers(wr, DR,onlyHalf_DR);
                propagateWalkers(wrg, DRG,onlyHalf_DRG);
                propagateWalkers(wr2, DR2,onlyHalf_DR2);
                propagateWalkers(wrg2, DRG2,onlyHalf_DRG2);
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
                for (register int i=0; i<framesize; i++) {
                    frame[i]=0;
                    framef[i]=0;
                }

                //memset(frame, 0, framesize*sizeof(uint16_t));
                propagateWalkers(wg, DG,onlyHalf_DG);
                const float psfgsig2=sqr(psf_size_g);
                const float psfrsig2=sqr(psf_size_r);
                for (register int i=0; i<wg.size(); i++) {
                    //if (i==0) qDebug()<<wg[i].x<<", "<<wg[i].y;
                    float* f=framef;
                    for (register int c=0; c<framesize; c++) {
                        const float x=float(c%realwidth)*pixel_size;
                        const float y=float(c/realwidth)*pixel_size;
                        const float d=sqr(wg[i].x-x)+sqr(wg[i].y-y);
                        if (d<5.0*psfgsig2) (*f)=(*f)+brightnessG*exp(-2.0*(d)/psfgsig2);
                        f++;
                    }
//                    for (register int y=0; y<height; y++) {
//                        for (register int x=0; x<width; x++) {
//                            frame[y*realwidth+x]=frame[y*realwidth+x]+brightnessG*exp(-2.0*(sqr(wg[i].x-float(x)*pixel_size)+sqr(wg[i].y-float(y)*pixel_size))/psfgsig2);
//                        }
//                    }
                }
                propagateWalkers(wg2, DG2,onlyHalf_DG2);
                for (register int i=0; i<wg2.size(); i++) {
                    //if (i==0) qDebug()<<wg[i].x<<", "<<wg[i].y;
                    float* f=framef;
                    for (register int c=0; c<framesize; c++) {
                        const float x=float(c%realwidth)*pixel_size;
                        const float y=float(c/realwidth)*pixel_size;
                        const float d=sqr(wg2[i].x-x)+sqr(wg2[i].y-y);
                        if (d<5.0*psfgsig2) (*f)=(*f)+brightnessG2*exp(-2.0*(d)/psfgsig2);
                        f++;
                    }
//                    for (register int y=0; y<height; y++) {
//                        for (register int x=0; x<width; x++) {
//                            frame[y*realwidth+x]=frame[y*realwidth+x]+brightnessG2*exp(-2.0*(sqr(wg2[i].x-float(x)*pixel_size)+sqr(wg2[i].y-float(y)*pixel_size))/psfgsig2);
//                        }
//                    }
                }
                if (dualView) {
                    propagateWalkers(wr, DR,onlyHalf_DR);
                    propagateWalkers(wrg, DRG,onlyHalf_DRG);

                    for (register int i=0; i<wr.size(); i++) {
                        for (register int y=0; y<height; y++) {
                            for (register int x=0; x<width; x++) {
                                framef[y*realwidth+x+width]=framef[y*realwidth+x+width]+brightnessR*exp(-2.0*(sqr(wr[i].x-float(x)*pixel_size-deltax)+sqr(wr[i].y-float(y)*pixel_size-deltay))/psfrsig2);
                            }
                        }
                    }
                    for (register int i=0; i<wrg.size(); i++) {
                        for (register int y=0; y<height; y++) {
                            for (register int x=0; x<width; x++) {
                                framef[y*realwidth+x+width]=framef[y*realwidth+x+width]+brightnessR*exp(-2.0*(sqr(wrg[i].x-float(x)*pixel_size-deltax)+sqr(wrg[i].y-float(y)*pixel_size-deltay))/psfrsig2);
                                framef[y*realwidth+x]=framef[y*realwidth+x]+brightnessG*exp(-2.0*(sqr(wrg[i].x-float(x)*pixel_size-deltax)+sqr(wrg[i].y-float(y)*pixel_size-deltay))/psfgsig2);
                            }
                        }
                    }
                    propagateWalkers(wr2, DR2,onlyHalf_DR2);
                    propagateWalkers(wrg2, DRG2,onlyHalf_DRG2);
                    for (register int i=0; i<wr2.size(); i++) {
                        for (register int y=0; y<height; y++) {
                            for (register int x=0; x<width; x++) {
                                framef[y*realwidth+x+width]=framef[y*realwidth+x+width]+brightnessR2*exp(-2.0*(sqr(wr2[i].x-float(x)*pixel_size-deltax)+sqr(wr2[i].y-float(y)*pixel_size-deltay))/psfrsig2);
                            }
                        }
                    }
                    for (register int i=0; i<wrg2.size(); i++) {
                        for (register int y=0; y<height; y++) {
                            for (register int x=0; x<width; x++) {
                                framef[y*realwidth+x+width]=framef[y*realwidth+x+width]+brightnessR2*exp(-2.0*(sqr(wrg2[i].x-float(x)*pixel_size-deltax)+sqr(wrg2[i].y-float(y)*pixel_size-deltay))/psfrsig2);
                                framef[y*realwidth+x]=framef[y*realwidth+x]+brightnessG2*exp(-2.0*(sqr(wrg2[i].x-float(x)*pixel_size-deltax)+sqr(wrg2[i].y-float(y)*pixel_size-deltay))/psfgsig2);
                            }
                        }
                    }
                    if (crosstalk>0) {
                        for (register int y=0; y<height; y++) {
                            for (register int x=0; x<width; x++) {
                                framef[y*realwidth+x+width]=framef[y*realwidth+x+width]+crosstalk*framef[y*realwidth+x];
                            }
                        }
                    }
                }


                if (nvar<=0 && background!=0) {
                    for (int i=0; i<framesize; i++) {
                        framef[i]=framef[i]+background;
                    }
                } else if (nvar>0) {
                    for (int i=0; i<framesize; i++) {
                        framef[i]=framef[i]+(rng.randNorm(background, nvar));
                    }
                }

                for (int i=0; i<framesize; i++) {
                    frame[i]=qMax(uint16_t(0),(uint16_t)round(framef[i]));
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
    qfFree(frame);
    qfFree(framef);
}

QVector<QFRDRImagingFCSSimulationThread::WalkerData> QFRDRImagingFCSSimulationThread::createWalkers(int count, bool onlyHalfImage)
{
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> res;
    for (int i=0; i<count; i++) {
        QFRDRImagingFCSSimulationThread::WalkerData d;
        d.x=rng.rand(pixel_size*float(width-1));
        if (onlyHalfImage) {
            d.y=rng.rand(pixel_size*float(height-1)/2.0);
        } else {
            d.y=rng.rand(pixel_size*float(height-1));
        }
        res<<d;
    }
    return res;
}

void QFRDRImagingFCSSimulationThread::propagateWalkers(QVector<QFRDRImagingFCSSimulationThread::WalkerData> &walkersv, float D, bool onlyHalfImage)
{
    const float vxfactor=VX*frametime*1.0e-6;
    const float vyfactor=VY*frametime*1.0e-6;
    const float Dfactor=sqrt(2.0*D*frametime*1.0e-6);
    for (register int i=0; i<walkersv.size(); i++) {
        QFRDRImagingFCSSimulationThread::WalkerData& d=walkersv[i];

        register float v=(FlowEeverywhere || (d.x>float(width)*pixel_size/2.0))?1.0:0.0;
        register float dx=d.x+rng.randNorm(0.0, 1.0)*Dfactor+v*vxfactor;
        register float dy=d.y+rng.randNorm(0.0, 1.0)*Dfactor+v*vyfactor;


        if (dx<-4.0*pixel_size) dx=float(width+3)*pixel_size;
        if (dx>(width+3)*pixel_size) dx=-4.0*pixel_size;
        if (onlyHalfImage) {
            if (dy<float(height)*pixel_size/2.0) dy=float(height)*pixel_size/2.0;
            if (dy>float(height+3)*pixel_size) dy=float(height+3)*pixel_size;
        } else {
            if (dy<-4.0*pixel_size) dy=float(height+3)*pixel_size;
            if (dy>float(height+3)*pixel_size) dy=-4.0*pixel_size;
        }

        d.x=dx;
        d.y=dy;

        //walkersv[i]=d;
    }
}
