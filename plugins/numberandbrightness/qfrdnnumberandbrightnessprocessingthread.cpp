#include "qfrdnnumberandbrightnessprocessingthread.h"
#include "qfimporterimageseries.h"
#include "qfpluginservices.h"
#include "qfimportermanager.h"
#include "libtiff_tools.h"


QFRDRNnumberAndBrightnessProcessingThread::QFRDRNnumberAndBrightnessProcessingThread(int threadID, QObject *parent) :
    QThread(parent)
{
    m_canceled=false;
    this->threadID=threadID;
}

void QFRDRNnumberAndBrightnessProcessingThread::run()
{
    for (int j=0; j<jobs.size(); j++) {
        const Job& job=jobs[j];
        if (m_canceled) break;
        emit setJobProgress(0);
        emit setJobProgress(threadID, 0);
        bool jobDone=true;

        QFImporterImageSeries* r=NULL;

        QFImporter* imp=QFPluginServices::getInstance()->getImporterManager()->createImporter(job.readerID);
        r=dynamic_cast<QFImporterImageSeries*>(imp);
        if (r) {
            QFile fSet(job.outputname);
            QDir d=QFileInfo(job.outputname).absoluteDir();
            if (fSet.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream set(&fSet);
                set<<"date/time                   : "<<QDateTime::currentDateTime().toString(Qt::ISODate)<<"\n";
                set<<"input file                  : "<<d.relativeFilePath(job.inputfile)<<"\n";
                set<<"input background file       : "<<d.relativeFilePath(job.backgroundfile)<<"\n";

                int frame_width=0, frame_height=0;
                //qDebug()<<j<<"opening "<<job.inputfile;
                if (r->open(job.inputfile)) {
                    int width=frame_width=r->frameWidth();
                    int height=frame_height=r->frameHeight();
                    int framescnt=r->countFrames();
                    int expectedFrames=framescnt;
                    set<<"width                       : "<<width<<"\n";
                    set<<"height                      : "<<height<<"\n";
                    set<<"dualview mode               : "<<job.dvmode<<"\n";
                    double* frame=(double*)qfMalloc(width*height*sizeof(double));
                    double* fSum=(double*)qfMalloc(width*height*sizeof(double));
                    double* fSum2=(double*)qfMalloc(width*height*sizeof(double));
                    for (int i=0; i<width*height; i++) {
                        fSum[i]=0;
                        fSum2[i]=0;
                    }
                    double frames=0;
                    int f=0;
                    bool fok=true;
                    if (job.start>0) {
                        while (f<job.start && fok) {
                            fok=r->nextFrame();
                            f++;
                            if (m_canceled) break;
                        }
                    }
                    if (fok) {
                        do {
                            r->readFrameDouble(frame);
                            for (int i=0; i<width*height; i++) {
                                fSum[i]=fSum[i]+frame[i];
                                fSum2[i]=fSum2[i]+frame[i]*frame[i];
                            }
                            frames++;
                            f++;
                            if (m_canceled) break;
                            if (expectedFrames>25 && f%(expectedFrames/25)==0) {
                                emit setJobProgress(f/(expectedFrames/25));
                                emit setJobProgress(threadID, f/(expectedFrames/25));
                            }
                        } while (r->nextFrame() && (f<=job.end || job.end<0));
                    }
                    set<<"frame count                 : "<<int(frames)<<"\n";
                    set<<"frame count in file         : "<<int(framescnt)<<"\n";
                    if (job.start>=0) set<<"first frame                 : "<<job.start<<"\n";
                    else  set<<"first frame                 : "<<0<<"\n";
                    if (job.end>=0) set<<"last frame                  : "<<job.end<<"\n";
                    else set<<"last frame                  : "<<int(framescnt-1)<<"\n";
                    set<<"detector type               : "<<job.detectorMode<<"\n";
                    set<<"detector gain               : "<<CDoubleToQString(job.gain)<<"\n";
                    set<<"detector excess noise       : "<<CDoubleToQString(job.excessNoise)<<"\n";

                    for (int i=0; i<width*height; i++) {
                        fSum2[i]=sqrt((fSum2[i]-qfSqr(fSum[i])/frames)/(frames-1.0));
                        fSum[i]=fSum[i]/frames;
                    }

                    TIFF* tif;
                    QString fn;
                    tif=TIFFOpen((fn=job.basename+".overview_uncorrected.tif").toLocal8Bit().data(), "w");
                    if (tif) {
                        TIFFTWriteDoubleAsFloat(tif, fSum, width, height, job.pixel_size, job.pixel_size);
                        TIFFClose(tif);
                        set<<"uncorr. overview image file : "<<d.relativeFilePath(fn)<<"\n";
                    }

                    tif=TIFFOpen((fn=job.basename+".overviewstd_uncorrected.tif").toLocal8Bit().data(), "w");
                    if (tif) {
                        TIFFTWriteDoubleAsFloat(tif, fSum2, width, height, job.pixel_size, job.pixel_size);
                        TIFFClose(tif);
                        set<<"uncorr. overview std image  : "<<d.relativeFilePath(fn)<<"\n";
                    }
                    r->close();

                    if (frame) qfFree(frame);
                    if (fSum) qfFree(fSum);
                    if (fSum2) qfFree(fSum2);
                } else {
                    emit errorMessage(tr("Could not open file '%1'").arg(job.inputfile));
                }

                //qDebug()<<j<<"opening "<<job.backgroundfile;
                if (QFile::exists(job.backgroundfile) && r->open(job.backgroundfile)) {
                    int width=r->frameWidth();
                    int height=r->frameHeight();
                    if (width==frame_width && height==frame_height) {
                        double* frame=(double*)qfMalloc(width*height*sizeof(double));
                        double* fSum=(double*)qfMalloc(width*height*sizeof(double));
                        double* fSum2=(double*)qfMalloc(width*height*sizeof(double));
                        for (int i=0; i<width*height; i++) {
                            fSum[i]=0;
                            fSum2[i]=0;
                        }
                        double frames=0;
                        int f=0;
                        int expectedFrames=r->countFrames();
                        do {
                            r->readFrameDouble(frame);
                            for (int i=0; i<width*height; i++) {
                                fSum[i]=fSum[i]+frame[i];
                                fSum2[i]=fSum2[i]+frame[i]*frame[i];
                            }
                            frames++;
                            f++;
                            if (m_canceled) break;
                            if (expectedFrames>25 && f%(expectedFrames/25)==0) {
                                emit setJobProgress(50+f/(expectedFrames/25));
                                emit setJobProgress(threadID, 50+f/(expectedFrames/25));
                            }

                        } while (r->nextFrame() && (f<=job.end || job.end<0));

                        for (int i=0; i<width*height; i++) {
                            fSum2[i]=sqrt((fSum2[i]-qfSqr(fSum[i])/frames)/(frames-1.0));
                            fSum[i]=fSum[i]/frames;
                        }

                        TIFF* tif;
                        QString fn;
                        tif=TIFFOpen((fn=job.basename+".background.tif").toLocal8Bit().data(), "w");
                        if (tif) {
                            TIFFTWriteDoubleAsFloat(tif, fSum, width, height, job.pixel_size, job.pixel_size);
                            TIFFClose(tif);
                            set<<"background image file       : "<<d.relativeFilePath(fn)<<"\n";
                        }

                        tif=TIFFOpen((fn=job.basename+".backgroundstd.tif").toLocal8Bit().data(), "w");
                        if (tif) {
                            TIFFTWriteDoubleAsFloat(tif, fSum2, width, height, job.pixel_size, job.pixel_size);
                            TIFFClose(tif);
                            set<<"background stddev           : "<<d.relativeFilePath(fn)<<"\n";
                        }


                        if (frame) qfFree(frame);
                        if (fSum) qfFree(fSum);
                        if (fSum2) qfFree(fSum2);
                    } else {
                        emit errorMessage(tr("Background file '%1' and image stack '%2' do not have the same width and height!").arg(job.backgroundfile).arg(job.inputfile));
                    }
                    r->close();
                } else {
                    if (QFile::exists(job.backgroundfile)) emit errorMessage(tr("Could not open background file '%1'").arg(job.backgroundfile));
                }
                fSet.close();
            } else {
                jobDone=false;
            }

            delete r;
        } else {
            if (imp) delete imp;
        }



        emit setJobProgress(100);
        emit setJobProgress(threadID, 100);
        if (jobDone) emit finishedJob(job.outputname);
        else emit finishedJob("");

    }
    if (m_canceled) emit errorMessage(tr("Canceled by User!"));
}

void QFRDRNnumberAndBrightnessProcessingThread::addJob(const QFRDRNnumberAndBrightnessProcessingThread::Job &j)
{
    jobs.append(j);
}

void QFRDRNnumberAndBrightnessProcessingThread::cancel()
{
    m_canceled=true;
}
