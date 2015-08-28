/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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
#include "statistics_tools.h"
#include "datatools.h"

inline float sqr(const float& x) { return x*x; }

QFRDRImagingFCSSimulationThread::QFRDRImagingFCSSimulationThread(QObject *parent) :
    QThread(parent),
    rng()
{
    saveTrajectores=false;
    trajectoresMaxSteps=1000;
    maxTrajectores=10;
    simspace_sizeinc=3;
    environmentMode=SIMENV_NORMAL;
    boundaryConditions=BOUNDARY_PERIODIC;
    saveMSD=false;
    msdMaxSteps=-1;
    canceled=false;
    brightnessG=100;
    brightnessR=100;
    brightnessG2=100;
    brightnessG3=100;
    brightnessR2=100;
    DG=DR=DRG=5;
    DG2=DR2=DRG2=15;
    DG3=100;
    FlowEeverywhere=true;
    VX=VY=0;
    psf_size_g=0.5;
    psf_size_r=0.55;
    pixel_size=0.4;
    frames=20000;
    frametime=500;
    sim_timestep=frametime;
    dualView=false;
    width=10;
    height=10;
    walkersG=10;
    walkersR=0;
    walkersRG=0;
    walkersG2=10;
    walkersG3=0;
    walkersR2=0;
    walkersRG2=0;
    background=100;
    backgroundNoise=2;
    onlyHalf_DG=false;
    onlyHalf_DG3=false;
    onlyHalf_DR=false;
    onlyHalf_DRG=false;
    onlyHalf_DG2=false;
    onlyHalf_DR2=false;
    onlyHalf_DRG2=false;
    crosstalk=5.0/100.0;
    warmup=50000;
    deltax=deltay=0;

    boundaryGridSpacing=100;
    boundaryGridJumpProbability=0.001;
    boundaryGridOnlyRight=false;

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

    msdNames.clear();

    filename=filename.replace("%timestep%", tr("%1uS").arg(frametime));
    filename=filename.replace("%sim_timestep%", tr("%1uS").arg(sim_timestep));
    filename=filename.replace("%type%", tr("2DRWsim"));
    filename=filename.replace("%D1r%", tr("%1").arg(DR));
    filename=filename.replace("%D1g%", tr("%1").arg(DG));
    filename=filename.replace("%D1gr%", tr("%1").arg(DRG));
    filename=filename.replace("%D1rg%", tr("%1").arg(DRG));
    filename=filename.replace("%D2r%", tr("%1").arg(DR2));
    filename=filename.replace("%D2g%", tr("%1").arg(DG2));
    filename=filename.replace("%D3g%", tr("%1").arg(DG3));
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

    QString tiffilename=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".tif";
    QString inifilename=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".configuration.ini";
    QString filename_msd=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".msd.dat";
    QString filename_t=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".trajectories.dat";
    QString filenameBackground=QFileInfo(filename).path()+"/"+QFileInfo(filename).completeBaseName()+".background.tif";
    QSettings config(inifilename, QSettings::IniFormat);
    config.setValue("acquisition/pixel_width", (double)pixel_size);
    config.setValue("acquisition/pixel_height", (double)pixel_size);
    config.setValue("acquisition/magnification", 1);
    config.setValue("acquisition/images", qlonglong(frames));
    config.setValue("acquisition/image_height", height);
    config.setValue("acquisition/image_width", width);
    config.setValue("acquisition/type", tr("simulation"));
    config.setValue("acquisition/frame_time", double(frametime*1.0e-6));
    config.setValue("acquisition/frame_rate", double(1.0/(frametime*1.0e-6)));
    config.setValue("acquisition/baseline_offset", 0);
    if (dualView) config.setValue("acquisition/dualview_mode", "h");
    else config.setValue("acquisition/dualview_mode", "none");
    int fileCount=2;
    config.setValue("files/count", fileCount);
    config.setValue("files/name0", QFileInfo(tiffilename).fileName());
    config.setValue("files/type0", "TIFF16");
    config.setValue("files/description0", tr("simulation result (image series)"));
    config.setValue("files/name1", QFileInfo(filenameBackground).fileName());
    config.setValue("files/type1", "TIFF16");
    config.setValue("files/description1", tr("background image series"));

    config.setValue("simulation/type", tr("2D random walk simulation"));
    if (boundaryConditions==0) config.setValue("simulation/boundaryConditions", tr("periodic"));
    else if (boundaryConditions==1) config.setValue("simulation/boundaryConditions", tr("random re-introduction"));
    config.setValue("simulation/boundaryConditions_id", boundaryConditions);

    config.setValue("simulation/environmentMode_id", environmentMode);
    if (environmentMode==SIMENV_NORMAL) {
        config.setValue("simulation/environmentMode", tr("free diffusion"));
    } else {
        config.setValue("simulation/environmentMode", tr("regular barrier grid"));
        config.setValue("simulation/boundaryGridSpacing", (double)boundaryGridSpacing);
        config.setValue("simulation/boundaryGridJumpProbability", (double)boundaryGridJumpProbability);
        config.setValue("simulation/boundaryGridOnlyRight", boundaryGridOnlyRight);
    }

    config.setValue("simulation/saveMSD", saveMSD);
    if (saveMSD) {
        config.setValue("simulation/msdMaxSteps", msdMaxSteps);
    }

    config.setValue("simulation/saveTrajectores", saveTrajectores);
    if (saveTrajectores) {
        config.setValue("simulation/maxTrajectories", maxTrajectores);
        config.setValue("simulation/maxTrajectorySteps", trajectoresMaxSteps);
    }

    config.setValue("simulation/warmup", (qlonglong)warmup);
    config.setValue("simulation/dualView", dualView);
    config.setValue("simulation/pixel_size", (double)pixel_size);
    config.setValue("simulation/psf_size_green", (double)psf_size_g);
    if (dualView) config.setValue("simulation/psf_size_red", (double)psf_size_r);
    if (dualView) config.setValue("simulation/crosstalk", (double)crosstalk);
    config.setValue("simulation/background", (double)background);
    config.setValue("simulation/backgroundNoise", (double)backgroundNoise);

    if (walkersG>0) config.setValue("simulation/DG", (double)DG);
    if (dualView && walkersR>0) config.setValue("simulation/DR", (double)DR);
    if (dualView && walkersRG>0) config.setValue("simulation/DRG", (double)DRG);
    if (walkersG2>0) config.setValue("simulation/DG2", (double)DG2);
    if (dualView && walkersR2>0) config.setValue("simulation/DR2", (double)DR2);
    if (dualView && walkersRG2>0) config.setValue("simulation/DRG2", (double)DRG2);
    if (walkersG3>0) config.setValue("simulation/DG3", (double)DG3);
    config.setValue("simulation/DeltaX", (double)deltax);
    config.setValue("simulation/DeltaY", (double)deltay);
    config.setValue("simulation/VX", (double)VX);
    config.setValue("simulation/VY", (double)VY);
    config.setValue("simulation/FlowEverywhere", FlowEeverywhere);
    if (walkersG>0) config.setValue("simulation/walkersG", walkersG);
    if (dualView && walkersR>0) config.setValue("simulation/walkersR", walkersR);
    if (dualView && walkersRG>0) config.setValue("simulation/walkersRG", walkersRG);
    if (walkersG2>0) config.setValue("simulation/walkersG2", walkersG2);
    if (dualView && walkersR2>0) config.setValue("simulation/walkersR2", walkersR2);
    if (dualView && walkersRG2>0) config.setValue("simulation/walkersRG2", walkersRG2);
    if (walkersG3>0) config.setValue("simulation/walkersG3", walkersG3);
    if (walkersG>0 || (dualView && walkersRG>0)) config.setValue("simulation/brightnessG", (double)brightnessG);
    if (dualView && (walkersR>0 || walkersRG>0)) config.setValue("simulation/brightnessR", (double)brightnessR);
    if ((dualView && walkersRG2>0) || walkersG2>0) config.setValue("simulation/brightnessG2", (double)brightnessG2);
    if (dualView && (walkersRG2>0 || walkersR2>0)) config.setValue("simulation/brightnessR2", (double)brightnessR2);
    if (walkersG3>0) config.setValue("simulation/brightnessG3", (double)brightnessG3);
    if (walkersG>0) config.setValue("simulation/onlyHalf_DG", onlyHalf_DG);
    if (walkersG2>0) config.setValue("simulation/onlyHalf_DG2", onlyHalf_DG2);
    if (walkersG3>0) config.setValue("simulation/onlyHalf_DG3", onlyHalf_DG3);
    if (dualView && walkersR>0) config.setValue("simulation/onlyHalf_DR", onlyHalf_DR);
    if (dualView && walkersR2>0) config.setValue("simulation/onlyHalf_DR2", onlyHalf_DR2);
    if (dualView && walkersRG>0) config.setValue("simulation/onlyHalf_DRG", onlyHalf_DRG);
    if (dualView && walkersRG2>0) config.setValue("simulation/onlyHalf_DRG2", onlyHalf_DRG2);

    int framesize=width*height;
    int realwidth=width;
    if (dualView) {
        realwidth=2*width;
        framesize=2*width*height;
    }
    const float nvar=backgroundNoise*backgroundNoise;

    TinyTIFFFile* tif=TinyTIFFWriter_open(tiffilename.toLatin1().data(), 16, realwidth, height);
    uint16_t* frame=(uint16_t*)qfMalloc(framesize*sizeof(uint16_t));
    float* framef=(float*)qfMalloc(framesize*sizeof(float));
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wg=createWalkers(walkersG, onlyHalf_DG);
    QList<QVector<QPair<float, float> > > wg_msd, wg_t;
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wr=createWalkers(walkersR, onlyHalf_DR);
    QList<QVector<QPair<float, float> > > wr_msd, wr_t;
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wrg=createWalkers(walkersRG, onlyHalf_DRG);
    QList<QVector<QPair<float, float> > > wrg_msd, wrg_t;
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wg2=createWalkers(walkersG2, onlyHalf_DG2);
    QList<QVector<QPair<float, float> > > wg2_msd, wg2_t;
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wg3=createWalkers(walkersG3, onlyHalf_DG3);
    QList<QVector<QPair<float, float> > > wg3_msd, wg3_t;
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wr2=createWalkers(walkersR2, onlyHalf_DR2);
    QList<QVector<QPair<float, float> > > wr2_msd, wr2_t;
    QVector<QFRDRImagingFCSSimulationThread::WalkerData> wrg2=createWalkers(walkersRG2, onlyHalf_DRG2);
    QList<QVector<QPair<float, float> > > wrg2_msd, wrg2_t;
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
        for (uint64_t wi=0; wi<warmup; wi++) {
            propagateWalkers(wg, DG,onlyHalf_DG);
            propagateWalkers(wg2, DG2,onlyHalf_DG2);
            propagateWalkers(wg3, DG3,onlyHalf_DG3);
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
            double t=0;
            double t0=0;
            for (currentFrame=0; currentFrame<frames; currentFrame++) {
                if (currentFrame%100==0) emit statusMessage(tr("running simulation (frame %1 / %2 )...").arg(currentFrame+1).arg(frames));
                for (register int i=0; i<framesize; i++) { frame[i]=0; }
                for (register int i=0; i<framesize; i++) { framef[i]=0; }
                t0=t;
                while (t<t0+frametime) {
                    bool savMSD=(saveMSD&&((msdMaxSteps<=1)||(t<float(msdMaxSteps)*sim_timestep)));
                    bool savTraj=(saveTrajectores&&((trajectoresMaxSteps<=1)||(t<float(trajectoresMaxSteps)*sim_timestep)));
                    propagateWalkers(wg, DG,onlyHalf_DG, savMSD?(&wg_msd):NULL, savTraj?(&wg_t):NULL);
                    const float psfgsig2=sqr(psf_size_g);
                    const float psfrsig2=sqr(psf_size_r);
                    for (register int i=0; i<wg.size(); i++) {
                        float* f=framef;
                        for (register int c=0; c<framesize; c++) {
                            const float x=float(c%realwidth)*pixel_size;
                            const float y=float(c/realwidth)*pixel_size;
                            const float d=sqr(wg[i].x-x)+sqr(wg[i].y-y);
                            if (d<5.0*psfgsig2) (*f)=(*f)+brightnessG*exp(-2.0*(d)/psfgsig2);
                            f++;
                        }
                    }
                    propagateWalkers(wg2, DG2,onlyHalf_DG2, savMSD?(&wg2_msd):NULL, savTraj?(&wg2_t):NULL);
                    for (register int i=0; i<wg2.size(); i++) {
                        float* f=framef;
                        for (register int c=0; c<framesize; c++) {
                            const float x=float(c%realwidth)*pixel_size;
                            const float y=float(c/realwidth)*pixel_size;
                            const float d=sqr(wg2[i].x-x)+sqr(wg2[i].y-y);
                            if (d<5.0*psfgsig2) (*f)=(*f)+brightnessG2*exp(-2.0*(d)/psfgsig2);
                            f++;
                        }
                    }
                    propagateWalkers(wg3, DG3,onlyHalf_DG3, savMSD?(&wg3_msd):NULL, savTraj?(&wg3_t):NULL);
                    for (register int i=0; i<wg3.size(); i++) {
                        float* f=framef;
                        for (register int c=0; c<framesize; c++) {
                            const float x=float(c%realwidth)*pixel_size;
                            const float y=float(c/realwidth)*pixel_size;
                            const float d=sqr(wg3[i].x-x)+sqr(wg3[i].y-y);
                            if (d<5.0*psfgsig2) (*f)=(*f)+brightnessG3*exp(-2.0*(d)/psfgsig2);
                            f++;
                        }
                    }
                    if (dualView) {
                        propagateWalkers(wr, DR,onlyHalf_DR, savMSD?(&wr_msd):NULL, savTraj?(&wr_t):NULL);
                        propagateWalkers(wrg, DRG,onlyHalf_DRG, savMSD?(&wrg_msd):NULL, savTraj?(&wrg_t):NULL);

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
                        propagateWalkers(wr2, DR2,onlyHalf_DR2, savMSD?(&wr2_msd):NULL, savTraj?(&wr2_t):NULL);
                        propagateWalkers(wrg2, DRG2,onlyHalf_DRG2, savMSD?(&wrg2_msd):NULL, savTraj?(&wrg2_t):NULL);
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
                    t=t+sim_timestep;
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

            if (saveMSD) {
                emit statusMessage(tr("calculating and saving MSDs ..."));

                QVector<uint64_t> tau;
                int S=1, P=8;
                while (tau.size()<=0 || tau.last()<frames/4) {
                    tau.resize(S*P);
                    statisticsAutocorrelateCreateMultiTau64(tau.data(), S, 2, P);
                    S++;
                }
                QVector<double> tauT;
                for (int i=0; i<tau.size(); i++) {
                    tauT.append(float(tau[i])*sim_timestep*1e-6);
                }

                QList<QVector<double> > msdout;
                QStringList columnHeaders;
                msdout.append(tauT);
                columnHeaders<<tr("lag time [s]");



                if (wg_msd.size()>0) {
                    msdNames<<"walker_g";
                    columnHeaders<<tr("MSD(walker_g) [micrometer^2]");
                    columnHeaders<<tr("MSDERROR(walker_g) [micrometer^2]");
                    calcMSD(msdout, wg_msd, tau);
                    wg_msd.clear();
                }
                if (wg2_msd.size()>0) {
                    msdNames<<"walker_{g2}";
                    columnHeaders<<tr("MSD(walker_{g2}) [micrometer^2]");
                    columnHeaders<<tr("MSDERROR(walker_{g2}) [micrometer^2]");
                    calcMSD(msdout, wg2_msd, tau);
                    wg2_msd.clear();
                }
                if (wg3_msd.size()>0) {
                    msdNames<<"walker_{g3}";
                    columnHeaders<<tr("MSD(walker_{g3}) [micrometer^2]");
                    columnHeaders<<tr("MSDERROR(walker_{g3}) [micrometer^2]");
                    calcMSD(msdout, wg3_msd, tau);
                    wg3_msd.clear();
                }
                if (wr_msd.size()>0) {
                    msdNames<<"walker_r";
                    columnHeaders<<tr("MSD(walker_r) [micrometer^2]");
                    columnHeaders<<tr("MSDERROR(walker_r) [micrometer^2]");
                    calcMSD(msdout, wr_msd, tau);
                    wr_msd.clear();
                }
                if (wr2_msd.size()>0) {
                    msdNames<<"walker_{r2}";
                    columnHeaders<<tr("MSD(walker_{r2}) [micrometer^2]");
                    columnHeaders<<tr("MSDERROR(walker_{r2}) [micrometer^2]");
                    calcMSD(msdout, wr2_msd, tau);
                    wr2_msd.clear();
                }
                if (wrg_msd.size()>0) {
                    msdNames<<"walker_rg";
                    columnHeaders<<tr("MSD(walker_rg) [micrometer^2]");
                    columnHeaders<<tr("MSDERROR(walker_rg) [micrometer^2]");
                    calcMSD(msdout, wrg_msd, tau);
                    wrg_msd.clear();
                }
                if (wrg2_msd.size()>0) {
                    msdNames<<"walker_{rg2}";
                    columnHeaders<<tr("MSD(walker_{rg2}) [micrometer^2]");
                    columnHeaders<<tr("MSDERROR(walker_{rg2}) [micrometer^2]");
                    calcMSD(msdout, wrg2_msd, tau);
                    wrg2_msd.clear();
                }

                saveStringToFile(filename_msd, toCSV(msdout, columnHeaders, QStringList(), '.', ", ", true, '\"', "#! ", 10));

                fileCount++;
                config.setValue("files/count", fileCount);
                config.setValue(QString("files/name%1").arg(fileCount-1), QFileInfo(filename_msd).fileName());
                config.setValue(QString("files/type%1").arg(fileCount-1), "CSV_MSD");
                config.setValue(QString("files/description%1").arg(fileCount-1), tr("simulation MSDs: %1").arg(msdNames.join("; ")));

            }


            if (saveTrajectores) {
                emit statusMessage(tr("saving trajectories ..."));

                int tmax=0;
                QList<QVector<double> > trajout;
                QStringList columnHeaders;



                if (wg_t.size()>0) {
                    trajNames<<tr("walkers G: %1").arg(wg_t.size());
                    saveTraj(trajout, wg_t, tmax,  columnHeaders, tr("walkers G"));
                    wg_t.clear();
                }
                if (wg2_t.size()>0) {
                    trajNames<<tr("walkers G2: %1").arg(wg2_t.size());
                    saveTraj(trajout, wg2_t, tmax,  columnHeaders, tr("walkers G2"));
                    wg2_t.clear();
                }
                if (wg3_t.size()>0) {
                    trajNames<<tr("walkers G3: %1").arg(wg3_t.size());
                    saveTraj(trajout, wg3_t, tmax,  columnHeaders, tr("walkers G3"));
                    wg3_t.clear();
                }
                if (wr_t.size()>0) {
                    trajNames<<tr("walkers R: %1").arg(wr_t.size());
                    saveTraj(trajout, wr_t, tmax,  columnHeaders, tr("walkers R"));
                    wr_t.clear();
                }
                if (wr2_t.size()>0) {
                    trajNames<<tr("walkers R2: %1").arg(wr2_t.size());
                    saveTraj(trajout, wr2_t, tmax,  columnHeaders, tr("walkers R2"));
                    wr2_t.clear();
                }
                if (wrg_t.size()>0) {
                    trajNames<<tr("walkers RG: %1").arg(wrg_t.size());
                    saveTraj(trajout, wrg_t, tmax,  columnHeaders, tr("walkers RG"));
                    wrg_t.clear();
                }
                if (wrg2_t.size()>0) {
                    trajNames<<tr("walkers RG2: %1").arg(wrg2_t.size());
                    saveTraj(trajout, wrg2_t, tmax,  columnHeaders, tr("walkers RG2"));
                    wrg2_t.clear();
                }

                QVector<double> tt;
                for (int i=0; i<tmax; i++) {
                    tt<<float(i)*sim_timestep*1e-6;
                }

                trajout.prepend(tt);
                columnHeaders.prepend(tr("time [s]"));

                saveStringToFile(filename_t, toCSV(trajout, columnHeaders, QStringList(), '.', ", ", true, '\"', "#! ", 8));

                fileCount++;
                config.setValue("files/count", fileCount);
                config.setValue(QString("files/name%1").arg(fileCount-1), QFileInfo(filename_t).fileName());
                config.setValue(QString("files/type%1").arg(fileCount-1), "CSV_TRAJECTORIES");
                config.setValue(QString("files/description%1").arg(fileCount-1), tr("simulation trajectories - %1").arg(trajNames.join("; ")));

            }

        }
        emit statusMessage(tr("simulation finished ..."));
        TinyTIFFWriter_close(tif);
    }
    qfFree(frame);
    qfFree(framef);
}


void QFRDRImagingFCSSimulationThread::calcMSD(QList<QVector<double> >& msdout, const QList<QVector<QPair<float, float> > >& wg_msd, const QVector<uint64_t>& tau)
{
    QList<QVector<float> > msdlst;
    int maxlen=0;
    for (int i=0; i<wg_msd.size(); i++) {
        QVector<float> x,y;
        int j=0;
        while (j<wg_msd[i].size()) {
            bool isNan=!QFFloatIsOK(wg_msd[i].at(j).first);
            if (!isNan) {
                x<<wg_msd[i].at(j).first;
                y<<wg_msd[i].at(j).second;
            }
            j++;
            if (j>=wg_msd[i].size() || isNan) {
                //qDebug()<<"msd of "<<x.size()<< y.size()<<" steps ";
                if (x.size()>10) {
                    QVector<float> msdtemp(tau.size(), 0.0);
                    //qDebug()<<"     start: maxlen="<<maxlen<<"   msdtemp: "<<msdtemp.size();
                    statisticsMSDMultiTau2D<float,double,float>(msdtemp.data(), x.data(), y.data(), qMin(x.size(), y.size()), tau.data(), tau.size());
                    while (msdtemp.last()<=0) {
                        msdtemp.removeLast();
                    }
                    maxlen=qMax(maxlen, msdtemp.size());
                    msdlst.append(msdtemp);
                    //qDebug()<<"     now:  maxlen="<<maxlen<<"   msdtemp: "<<msdtemp.size()<<"\n\n";
                }
                x.resize(0);
                y.resize(0);
            }
        }
    }
    msdout.append(QVector<double>());
    msdout.append(QVector<double>());
    for (int t=0; t<maxlen; t++) {
        QVector<float> m;
        for (int i=0; i<msdlst.size(); i++) {
            if (t<msdlst[i].size()) m<<msdlst[i].at(t);
        }
        double var=0;
        double mean=statisticsAverageVariance(var, m.data(), m.size());
        msdout[msdout.size()-2]<<(mean);
        msdout[msdout.size()-1]<<(sqrt(var));
    }
}

void QFRDRImagingFCSSimulationThread::saveTraj(QList<QVector<double> > &msdout, const QList<QVector<QPair<float, float> > > &wg_msd, int &tmax, QStringList &columnNames, const QString& wname)
{
    for (int i=0; i<wg_msd.size(); i++) {
        QVector<double> x,y;
        int j=0;
        while (j<wg_msd[i].size()) {
            bool isNan=!QFFloatIsOK(wg_msd[i].at(j).first);
            if (!isNan) {
                x<<wg_msd[i].at(j).first;
                y<<wg_msd[i].at(j).second;
            }
            j++;
            if (isNan) break;
        }
        msdout.append(x);
        msdout.append(y);
        tmax=qMax(tmax, x.size());
        columnNames<<tr("%1 #%2 X [micrometers]").arg(wname).arg(i+1);
        columnNames<<tr("%1 #%2 Y [micrometers]").arg(wname).arg(i+1);
    }
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

void QFRDRImagingFCSSimulationThread::propagateWalkers(QVector<QFRDRImagingFCSSimulationThread::WalkerData> &walkersv, float D, bool onlyHalfImage, QList<QVector<QPair<float, float> > > *msds, QList<QVector<QPair<float, float> > > *traj)
{
    const float vxfactor=VX*sim_timestep*1.0e-6;
    const float vyfactor=VY*sim_timestep*1.0e-6;
    const float Dfactor=sqrt(2.0*D*sim_timestep*1.0e-6);
    for (register int i=0; i<walkersv.size(); i++) {
        QFRDRImagingFCSSimulationThread::WalkerData& d=walkersv[i];

        register float v=(FlowEeverywhere || (d.x>float(width)*pixel_size/2.0))?1.0:0.0;
        register float dx=d.x+rng.randNorm(0.0, 1.0)*Dfactor+v*vxfactor;
        register float dy=d.y+rng.randNorm(0.0, 1.0)*Dfactor+v*vyfactor;


        if (environmentMode==SIMENV_GRIDBOUNDARIES && (!boundaryGridOnlyRight || (boundaryGridOnlyRight && d.x>float(width)/2.0*pixel_size))) {
            const int ddx=abs(floor((2.0*simspace_sizeinc+dx)/boundaryGridSpacing)-floor((2.0*simspace_sizeinc+d.x)/boundaryGridSpacing));
            const int ddy=abs(floor((2.0*simspace_sizeinc+dy)/boundaryGridSpacing)-floor((2.0*simspace_sizeinc+d.y)/boundaryGridSpacing));
            if (ddx!=0 || ddy!=0) {
                if (rng.rand()>boundaryGridJumpProbability/float(qMax(1,qMax(ddx,ddy)))) {
                    dx=d.x;
                    dy=d.y;
                } else {
                    //qDebug()<<"jump accepted for walker "<<i<<"  delta:"<<ddx<<ddy;
                }
            }
        }

        bool newpart=false;
        if (boundaryConditions==BOUNDARY_PERIODIC) { // periodic boundary conditions
            if (dx<-simspace_sizeinc) {newpart=true; dx=float(width)*pixel_size+simspace_sizeinc; }
            else if (dx>(width)*pixel_size+simspace_sizeinc) {newpart=true; dx=-simspace_sizeinc; }
            if (!onlyHalfImage) {
                if (dy<-simspace_sizeinc) {newpart=true; dy=float(height)*pixel_size+simspace_sizeinc; }
                else if (dy>float(height)*pixel_size+simspace_sizeinc) {newpart=true; dy=-simspace_sizeinc; }
            }
        } else if (boundaryConditions==BOUNDARY_REINTRODUCE) { // reintroduce parts that left the volume randomly on the boundary
            newpart=dx<-simspace_sizeinc;
            newpart=newpart||(dx>float(width)*pixel_size+simspace_sizeinc);
            newpart=newpart||(dy>float(height)*pixel_size+simspace_sizeinc);
            if (!onlyHalfImage) {
                newpart=newpart||(dy<-simspace_sizeinc);
            }
            if (newpart) {
                int b=rng.randInt()%4;
                if (b==0) { // top: y==float(height)*pixel_size+simspace_sizeinc
                    dx=rng.rand()*float(width)*pixel_size+simspace_sizeinc;
                    dy=float(height)*pixel_size+simspace_sizeinc;
                } else if (b==1) { // bottom: y==-simspace_sizeinc
                    dy=-simspace_sizeinc;
                    dx=rng.rand()*float(width)*pixel_size+simspace_sizeinc;
                } else if (b==2) { // top: x==float(width)*pixel_size+simspace_sizeinc
                    dx=float(width)*pixel_size+simspace_sizeinc;
                    dy=rng.rand()*float(height)*pixel_size+simspace_sizeinc;
                } else if (b==3) { // bottom: x==-simspace_sizeinc
                    dx=-simspace_sizeinc;
                    dy=rng.rand()*float(height)*pixel_size+simspace_sizeinc;
                }
            }
        }
        if (onlyHalfImage) { // limit walker to right image half!
            if (dy<float(height)*pixel_size/2.0) dy=float(height)*pixel_size/2.0;
        }

        d.x=dx;
        d.y=dy;

        if (msds) {
            while (i>=msds->size()) {
                msds->append(QVector<QPair<float, float> >());
            }
            if (newpart) (*msds)[i].append(qMakePair<float,float>(NAN,NAN));
            (*msds)[i].append(qMakePair<float,float>(d.x, d.y));
            //if (i==0) qDebug()<<(*msds)[i].last().first<<(*msds)[i].last().second;
        }
        if (traj && i<maxTrajectores) {
            while (i>=traj->size()) {
                traj->append(QVector<QPair<float, float> >());
            }
            if (newpart) (*traj)[i].append(qMakePair<float,float>(NAN,NAN));
            if ((*traj)[i].isEmpty() || QFFloatIsOK((*traj)[i].last().first)) (*traj)[i].append(qMakePair<float,float>(d.x, d.y));
            //if (i==0) qDebug()<<(*msds)[i].last().first<<(*msds)[i].last().second;
        }
     }
}

