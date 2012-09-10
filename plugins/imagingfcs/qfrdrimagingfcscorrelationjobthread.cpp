#include "qfrdrimagingfcscorrelationjobthread.h"
#include "qfrdrimagingfcscorrelationdialog.h"
#include "libtiff_tools.h"
#include "image_tools.h"
#include "statistics_tools.h"
#include <stdint.h>
#include "tinytiffwriter.h"
#include "qfimporterimageseries.h"
#include "qfimportermanager.h"
#include "qftools.h"
#include "lmcurve.h"
#include <typeinfo>

QMutex* QFRDRImagingFCSCorrelationJobThread::mutexFilename=NULL;

QFRDRImagingFCSCorrelationJobThread::QFRDRImagingFCSCorrelationJobThread(QFPluginServices *services, QObject *parent) :
    QThread(parent)
{
    if (!mutexFilename) mutexFilename=new QMutex();
    outLocale=QLocale::c();
    outLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    m_status=0;
    was_canceled=false;
    duration=0;
    backgroundImage=NULL;
    backgroundImageStd=NULL;
    fit_frames=NULL;
    fit_t=NULL;
    NFitFrames=0;
    pluginservices=services;
}

QFRDRImagingFCSCorrelationJobThread::~QFRDRImagingFCSCorrelationJobThread() {
    if (backgroundImage) free(backgroundImage);
    if (backgroundImageStd) free(backgroundImageStd);
}

double QFRDRImagingFCSCorrelationJobThread::durationMS() const {
    return duration;
}

double QFRDRImagingFCSCorrelationJobThread::durationS() const {
    return duration;
}

QStringList QFRDRImagingFCSCorrelationJobThread::getAddFiles() const {
    return addFiles;
}

IMFCSJob QFRDRImagingFCSCorrelationJobThread::getJob() const {
    return job;
}

QStringList QFRDRImagingFCSCorrelationJobThread::getImageFilterList(QFPluginServices* pluginservices)  {
    QStringList l;
    int i=0;
    QFImporterImageSeries* r=NULL;
    while ((r=getImageReader(i, pluginservices))!=NULL) {
        l.append(r->filter());
        delete r;
        i++;
    }
    return l;
}

QStringList QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList(QFPluginServices* pluginservices)  {
     QStringList l;
     int i=0;
     QFImporterImageSeries* r=NULL;
     while ((r=getImageReader(i, pluginservices))!=NULL) {
         l.append(r->formatName());
         delete r;
         i++;
     }
     return l;
}

QFImporterImageSeries* QFRDRImagingFCSCorrelationJobThread::getImageReader(int idx, QFPluginServices* pluginservices)  {
    QFImporterImageSeries* r=NULL;

    QStringList imp=pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();

    if (idx>=0 && idx<imp.size()) {
        r=dynamic_cast<QFImporterImageSeries*>(pluginservices->getImporterManager()->createImporter(imp[idx]));
    }

    return r;
}

int QFRDRImagingFCSCorrelationJobThread::getImageReaderCount(QFPluginServices* pluginservices) {
    QStringList imp=pluginservices->getImporterManager()->getImporters<QFImporterImageSeries*>();
    return imp.size();
}

int QFRDRImagingFCSCorrelationJobThread::status() const {
    return m_status;
}

void QFRDRImagingFCSCorrelationJobThread::cancel() {
    was_canceled=true;
    if (!isRunning()) {
        m_status=-1;
        emit messageChanged(tr("canceled by user"));
        emit statusChanged(m_status);
    }
}

void QFRDRImagingFCSCorrelationJobThread::init(IMFCSJob job) {
    this->job=job;
    m_status=0;
    was_canceled=false;
    addFiles.clear();
}

QString QFRDRImagingFCSCorrelationJobThread::replacePostfixSpecials(const QString& input, int counter) const {
    QString cnt="";
    if (counter>0) cnt=QString::number(counter);
    QString result=input;
    result=result.replace("%counter%", cnt, Qt::CaseInsensitive);
    result=result.replace("%s%", QString::number(job.S), Qt::CaseInsensitive);
    result=result.replace("%p%", QString::number(job.P), Qt::CaseInsensitive);
    result=result.replace("%m%", QString::number(job.m), Qt::CaseInsensitive);
    result=result.replace("%framerate%", QString::number(1.0/job.frameTime), Qt::CaseInsensitive);
    result=result.replace("%frametime%", QString::number(job.frameTime*1e6), Qt::CaseInsensitive);
    result=result.replace("%first%", QString("%1").arg(job.range_min,2,10,QChar('0')), Qt::CaseInsensitive);
    result=result.replace("%last%", QString("%1").arg(job.range_max,2,10,QChar('0')), Qt::CaseInsensitive);
    result=result.replace("%backoffset%", QString::number(job.backgroundOffset), Qt::CaseInsensitive);
    result=result.replace("%segments%", QString::number(job.segments), Qt::CaseInsensitive);
    result=result.replace("%backcorrectionid%", QString::number(job.backgroundCorrection), Qt::CaseInsensitive);
    result=result.replace("%correlatorid%", QString::number(job.correlator), Qt::CaseInsensitive);
    QString bpre="";
    if (job.binAverage) bpre="avg";
    if (job.interleaved_binning)  {
        result=result.replace("%binning%", bpre+QString("%1").arg(job.binning,2,10,QChar('0'))+"ib", Qt::CaseInsensitive);
    } else {
        result=result.replace("%binning%", bpre+QString("%1").arg(job.binning,2,10,QChar('0')), Qt::CaseInsensitive);
    }

    QString back="unknown";
    if (job.backgroundCorrection==0) back="none";
    if (job.backgroundCorrection==1) back="offset";
    if (job.backgroundCorrection==2) back="offsetmin";
    if (job.backgroundCorrection==3) back="imgoffset";
    QString corr="unknown";
    if (job.correlator==CORRELATOR_DIRECT) corr="direct";
    if (job.correlator==CORRELATOR_DIRECTAVG) corr="directavg";
    if (job.correlator==CORRELATOR_DIRECT_INT) corr="directint";
    if (job.correlator==CORRELATOR_DIRECTAVG_INT) corr="directavgint";
    if (job.correlator==CORRELATOR_MTAUALLMON) corr="mtauallmon";
    if (job.correlator==CORRELATOR_MTAUONEMON) corr="mtauonemon";
    QString bleach="none";
    if (job.bleach==BLEACH_REMOVEAVG) bleach="delavg";
    if (job.bleach==BLEACH_EXP) bleach="exp";

    result=result.replace("%backcorrection%", back, Qt::CaseInsensitive);
    result=result.replace("%correlator%", corr, Qt::CaseInsensitive);
    result=result.replace("%bleach%", bleach, Qt::CaseInsensitive);
    return result;
}

void QFRDRImagingFCSCorrelationJobThread::run() {
    QTime ptime;

    ptime.start();

    emit messageChanged(tr("loading data ..."));
    if ((job.fileFormat<0)||(job.fileFormat>=getImageReaderCount(pluginservices))) {
        m_status=-1;
        emit statusChanged(m_status);
        emit messageChanged(tr("file format not supported or given ... format given was: %1").arg(job.fileFormat));
        emit rangeChanged(0,100);
        emit progressChanged(0);
    } else if (!QFile::exists(job.filename)) {
        m_status=-1;
        emit statusChanged(m_status);
        emit messageChanged(tr("file does not exist ..."));
        emit rangeChanged(0,100);
        emit progressChanged(0);
    } else {
        m_status=1;
        emit statusChanged(m_status);
        emit rangeChanged(0,4780);


        reader=NULL;
        bool OK=false;
        if (job.fileFormat>=0 && job.fileFormat<getImageReaderCount(pluginservices)) {
            reader=getImageReader(job.fileFormat, pluginservices);
        }
        if (reader) {
            emit messageChanged(tr("opening %1 file ...").arg(reader->formatName()));
            OK=reader->open(job.filename);
            if (!OK) {
                m_status=-1; emit statusChanged(m_status);
                if (reader) messageChanged(tr("error opening file '%1': %2").arg(job.filename).arg(reader->lastError()));
                else emit messageChanged(tr("error opening file '%1'").arg(job.filename));
            } else {
                emit progressIncrement(10);
                emit messageChanged(tr("counting frames ..."));
                reader->setBinning(job.binning);
                reader->setInterleavedBinning(job.interleaved_binning);
                reader->setAverageBinning(job.binAverage);
                if (job.use_cropping) {
                    reader->setCropping(job.crop_x0, job.crop_x1, job.crop_y0, job.crop_y1);
                } else {
                    reader->unsetCropping();
                }
                first_frame=0;
                int32_t frame_count=reader->countFrames();
                if (job.range_min>0 && job.range_min<frame_count) first_frame=job.range_min;
                frames=frame_count-first_frame;
                if (job.range_max>(int64_t)first_frame && job.range_max<frame_count) frames=job.range_max-first_frame;
                double input_length=frames*job.frameTime;
                if (frames>0) {
                    frame_width=reader->frameWidth();
                    frame_height=reader->frameHeight();
                    average_frame=(float*)calloc(frame_width*frame_height, sizeof(float));
                    sqrsum_frame=(float*)calloc(frame_width*frame_height, sizeof(float));

                    baseline=job.backgroundOffset;

                    acf_tau=NULL;
                    acf=NULL;
                    acf_std=NULL;
                    acf_segments=NULL;
                    acf_N=0;
                    ccf_tau=NULL;
                    ccf1=NULL;
                    ccf2=NULL;
                    ccf3=NULL;
                    ccf4=NULL;
                    ccf1_std=NULL;
                    ccf2_std=NULL;
                    ccf3_std=NULL;
                    ccf4_std=NULL;
                    ccf1_segments=NULL;
                    ccf2_segments=NULL;
                    ccf3_segments=NULL;
                    ccf4_segments=NULL;
                    ccf_N=0;
                    dccf_tau=NULL;
                    dccf=NULL;
                    dccf_segments=NULL;
                    dccf_std=NULL;
                    dccf_N=0;
                    dccfframe_width=0;
                    dccfframe_height=0;
                    lastFrames=NULL;
                    firstFrames=NULL;
                    bleachOffset=NULL;
                    bleachAmplitude=NULL;
                    bleachTime=NULL;
                    fit_frames=NULL;
                    fit_t=NULL;
                    NFitFrames=0;

                    video_count=floor(frames/job.video_frames);
                    real_video_count=video_count;
                    video=(float*)calloc(frame_width*frame_height*video_count, sizeof(float));
                    bleachOffset=(float*)calloc(frame_width*frame_height, sizeof(float));
                    bleachAmplitude=(float*)calloc(frame_width*frame_height, sizeof(float));
                    bleachTime=(float*)calloc(frame_width*frame_height, sizeof(float));
                    bleachFitOK=(uint8_t*)calloc(frame_width*frame_height, sizeof(uint8_t));
                    firstFrames=(float*)calloc(frame_width*frame_height, sizeof(float));
                    lastFrames=(float*)calloc(frame_width*frame_height, sizeof(float));

                    ////////////////////////////////////////////////////////////////////////////////////////////
                    // CREATE FILENAMES FOR RESULTS AND MAKE SURE THE DIRECTORY FOR THE FILES EXISTS (mkpath() )
                    ////////////////////////////////////////////////////////////////////////////////////////////
                    QDir d=QFileInfo(job.filename).dir();

                    if (mutexFilename) mutexFilename->lock();
                    int counter=-1;
                    do {
                        outputFilenameBase=d.absoluteFilePath(replacePostfixSpecials(job.prefix)+QFileInfo(job.filename).completeBaseName()+replacePostfixSpecials(job.postfix, counter));
                        counter++;
                    } while (QFile::exists(outputFilenameBase+".evalsettings.txt"));
                    //qDebug()<<outputFilenameBase;
                    QString configFilename=outputFilenameBase+".evalsettings.txt";
                    QString averageFilename="";
                    QString stdFilename="";
                    QString backstdFilename="";
                    QString averageFilenameF="";
                    QString backgroundFilename="";
                    QString videoFilename="";
                    QString statisticsFilename="";
                    QString uncorrectedStatisticsFilename="";
                    QString backstatisticsFilename="";
                    QString acfFilename="";
                    QString ccfFilename="";
                    QString dccfFilename="";
                    QString acfFilenameBin="";
                    QString ccfFilenameBin="";
                    QString dccfFilenameBin="";
                    QString bleachOffsetFilename="";
                    QString bleachAmplitudeFilename="";
                    QString bleachTimeFilename="";
                    QString bleachFitSuccessFilename="";
                    QString firstFramesFilename="";
                    QString lastFramesFilename="";
                    QString bleachtimesFilename="";
                    QString bleachframesFilename="";
                    QString localFileDirectory=QFileInfo(d.absoluteFilePath(configFilename)).dir().absolutePath();

                    if (d.mkpath(localFileDirectory)) {
                        ////////////////////////////////////////////////////////////////////////////////////////////
                        // TOUCH OUTPUT FILE (.evalsettings.txt)
                        ////////////////////////////////////////////////////////////////////////////////////////////

                        touchFile(configFilename);

                        if (mutexFilename) mutexFilename->unlock();




                        ////////////////////////////////////////////////////////////////////////////////////////////
                        // RUN THE CORRELATION
                        ////////////////////////////////////////////////////////////////////////////////////////////
                        if (job.correlator==CORRELATOR_MTAUALLMON || job.correlator==CORRELATOR_MTAUONEMON) {
                            correlate_loadsingle();
                        } else {
                            correlate_loadall();
                        }





                        ////////////////////////////////////////////////////////////////////////////////////////////
                        // SAVE THE RESULTS
                        ////////////////////////////////////////////////////////////////////////////////////////////

                        //************** SAVE OVERVIEW IMAGE
                        if ((m_status==1) && !was_canceled && average_frame) {
                            emit messageChanged(tr("saving overview image ..."));
                            QString localFilename=averageFilename=outputFilenameBase+".overview.tif";
                            TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
                            if (tif) {
                                float avgMin=average_frame[0];
                                float avgMax=average_frame[0];
                                for (uint32_t i=0; i<frame_width*frame_height; i++) {
                                    avgMin=(average_frame[i]<avgMin)?average_frame[i]:avgMin;
                                    avgMax=(average_frame[i]>avgMax)?average_frame[i]:avgMax;
                                }
                                uint16_t* img=(uint16_t*)malloc(frame_width*frame_height*sizeof(uint16_t));
                                for (uint32_t i=0; i<frame_width*frame_height; i++) {
                                    img[i]=(uint16_t)round((double)(average_frame[i]-avgMin)*(double)0xFFFF/fabs(avgMax-avgMin));
                                }
                                TIFFTWriteUint16(tif, img, frame_width, frame_height);
                                free(img);
                                TIFFClose(tif);
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                averageFilename="";
                                emit messageChanged(tr("could not create overview image '%1'!").arg(localFilename));
                            }
                            localFilename=averageFilenameF=outputFilenameBase+".overview_float.tif";
                            tif = TIFFOpen(localFilename.toAscii().data(),"w");
                            if (tif) {

                                TIFFTWriteFloat(tif, average_frame, frame_width, frame_height);
                                TIFFClose(tif);
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                averageFilenameF="";
                                emit messageChanged(tr("could not create float overview image '%1'!").arg(localFilename));
                            }

                        }
                        emit progressIncrement(10);

                        //************** SAVE OVERVIEWSTD  IMAGE
                        if ((m_status==1) && !was_canceled && sqrsum_frame) {
                            emit messageChanged(tr("saving overview stddev image ..."));
                            QString localFilename=stdFilename=outputFilenameBase+".overviewstd.tif";
                            TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
                            if (tif) {
                                float* sd=(float*)calloc(frame_width*frame_height, sizeof(float));

                                if (frames>1) {
                                    for (int i=0; i<frame_width*frame_height; i++) {
                                        sd[i]=sqrt(double(frames)/double(frames-1)*(sqrsum_frame[i]-average_frame[i]*average_frame[i]));
                                    }
                                } else {
                                    for (int i=0; i<frame_width*frame_height; i++) {
                                        sd[i]=0;
                                    }

                                }

                                TIFFTWriteFloat(tif, sd, frame_width, frame_height);
                                TIFFClose(tif);
                                free(sd);
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                averageFilenameF="";
                                emit messageChanged(tr("could not create stddev overview image '%1'!").arg(localFilename));
                            }

                        }
                        emit progressIncrement(10);

                        //************** SAVE BACKGROUND IMAGE
                        if ((m_status==1) && !was_canceled && backgroundImage) {
                            bool doSave=false;
                            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                                if (backgroundImage[i]!=0) {
                                    doSave=true;
                                    break;
                                }
                            }
                            if (doSave) {
                                emit messageChanged(tr("saving background image ..."));
                                QString localFilename=backgroundFilename=outputFilenameBase+".background.tif";
                                TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, backgroundImage, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else {
                                    m_status=-1; emit statusChanged(m_status);
                                    backgroundFilename="";
                                    emit messageChanged(tr("could not create background image '%1'!").arg(localFilename));
                                }
                            }

                        }
                        //************** SAVE BACKGROUND STDDEV IMAGE
                        if ((m_status==1) && !was_canceled && backgroundImageStd) {
                            bool doSave=false;
                            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                                if (backgroundImageStd[i]!=0) {
                                    doSave=true;
                                    break;
                                }
                            }
                            if (doSave) {
                                emit messageChanged(tr("saving background S.D. image ..."));
                                QString localFilename=backstdFilename=outputFilenameBase+".backgroundstd.tif";
                                TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, backgroundImageStd, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else {
                                    m_status=-1; emit statusChanged(m_status);
                                    backstdFilename="";
                                    emit messageChanged(tr("could not create background S.D. image '%1'!").arg(localFilename));
                                }
                            }

                        }
                        emit progressIncrement(10);

                        //************** SAVE BLEACHING PARAMETERS IMAGE
                        if ((m_status==1) && !was_canceled ) {
                            if (job.bleach==BLEACH_EXP) {
                                emit messageChanged(tr("saving bleach parameter images ..."));
                                QString localFilename=bleachOffsetFilename=outputFilenameBase+".bleachoffset.tif";
                                bool error=false;
                                TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, bleachOffset, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=bleachAmplitudeFilename=outputFilenameBase+".bleachamplitude.tif";
                                tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, bleachAmplitude, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=bleachTimeFilename=outputFilenameBase+".bleachtime.tif";
                                tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, bleachTime, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=bleachFitSuccessFilename=outputFilenameBase+".bleachfitok.tif";
                                tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteUint8(tif, bleachFitOK, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=firstFramesFilename=outputFilenameBase+".firstframes.tif";
                                tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, firstFrames, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=lastFramesFilename=outputFilenameBase+".lastframes.tif";
                                tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, lastFrames, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                if (NFitFrames>0 && fit_t) {
                                    localFilename=bleachtimesFilename=outputFilenameBase+".bleachfittimes.txt";
                                    {
                                        QFile f(localFilename);
                                        if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                            QTextStream text(&f);
                                            text.setLocale(outLocale);
                                            for (int i=0; i<NFitFrames; i++) {
                                                text<<fit_t[i]<<"\n";
                                            }
                                            f.close();
                                        } else {
                                            error=true;
                                        }
                                    }
                                }

                                if (NFitFrames>0 && fit_frames) {
                                    localFilename=bleachframesFilename=outputFilenameBase+".bleachfitframes.tif";
                                    tif = TIFFOpen(localFilename.toAscii().data(),"w");
                                    if (tif) {
                                        for (int i=0; i<NFitFrames; i++) {
                                            TIFFTWriteFloat(tif, &(fit_frames[i*frame_width*frame_height]), frame_width, frame_height);
                                            TIFFWriteDirectory(tif);
                                        }
                                        TIFFClose(tif);
                                    } else error=true;
                                }


                                if (error) {
                                    m_status=-1; emit statusChanged(m_status);
                                    backgroundFilename="";
                                    emit messageChanged(tr("could not create background image '%1'!").arg(localFilename));
                                }
                            }
                        }
                        emit progressIncrement(10);

                        double videoAvgMin=0;
                        double videoAvgMax=(float)0xFFFF;
                        //************** SAVE VIDEO
                        if ((m_status==1) && !was_canceled && video && real_video_count>0 ) {
                            emit messageChanged(tr("saving video ..."));
                            QString localFilename=videoFilename=outputFilenameBase+".video.tif";
                            TinyTIFFFile* tif=TinyTIFFWriter_open(localFilename.toAscii().data(), 16, frame_width, frame_height);
                            //TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
                            if (tif) {
                                float avgMin=video[0];
                                float avgMax=video[0];
                                for (register uint32_t i=0; i<frame_width*frame_height*video_count; i++) {
                                    avgMin=(video[i]<avgMin)?video[i]:avgMin;
                                    avgMax=(video[i]>avgMax)?video[i]:avgMax;
                                }
                                uint16_t* img=(uint16_t*)malloc(frame_width*frame_height*sizeof(uint16_t));
                                for (register uint32_t c=0; c<real_video_count; c++) {
                                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                                        img[i]=(uint16_t)round((float)(video[c*frame_width*frame_height+i]-avgMin)*(float)0xFFFF/fabs(avgMax-avgMin));
                                    }
                                    TinyTIFFWriter_writeImage(tif, img);
                                    if (real_video_count/10>0) {
                                        if (c%(real_video_count/10)==0) {
                                            emit messageChanged(tr("saving video %1/%2...").arg(c+1).arg(video_count));
                                            progressIncrement(10);
                                        }
                                    }
                                }
                                if (real_video_count/10<=0) progressIncrement(100);
                                free(img);
                                TinyTIFFWriter_close(tif);
                                videoAvgMin = avgMin;
                                videoAvgMax = avgMax;
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create video '%1'!").arg(localFilename));
                            }

                        } else emit progressIncrement(100);

                        //************** SAVE STATISTICS
                        if ((m_status==1) && !was_canceled && job.statistics) {
                            emit messageChanged(tr("saving statistics ..."));
                            QString localFilename=statisticsFilename=outputFilenameBase+".statistics.dat";
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving statistics ..."));
                                int count=statistics_time.size();//qMin(statistics_time.size(), statistics_mean.size());
                                for (int i=0; i<count; i++) {
                                    text<<statistics_time[i];
                                    if (i<statistics_mean.size()) text<<", "<<statistics_mean[i];
                                    else text<<", 0";
                                    if (i<statistics_std.size()) text<<", "<<statistics_std[i];
                                    else text<<", 0";
                                    if (i<statistics_min.size()) text<<", "<<statistics_min[i];
                                    else text<<", 0";
                                    if (i<statistics_max.size()) text<<", "<<statistics_max[i];
                                    else text<<", 0";
                                    text<<"\n";
                                }
                                f.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create statistics file '%1': %2!").arg(localFilename).arg(f.errorString()));
                            }
                            localFilename=outputFilenameBase+".statistics.plt";
                            QFile f1(localFilename);
                            if (f1.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f1);
                                text.setLocale(outLocale);
                                text<<QString("set xlabel 'time [seconds]'\n");
                                text<<QString("set ylabel 'pixel grey value'\n");
                                text<<QString("set title \"Statistics '%1'\" noenhanced\n").arg(job.filename);
                                text<<QString("set style fill transparent solid 0.5 noborder\n");
                                text<<QString("set multiplot layout 1,2\n");
                                text<<QString("plot '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:4:5 title 'min/max' with filledcu, '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("unset multiplot\n");
                                text<<QString("pause -1\n");
                                text<<QString("set multiplot layout 2,2\n");
                                text<<QString("plot '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:3 title 'stddev' with lines lt 1\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:4 title 'min' with lines lt 1\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:5 title 'max' with lines lt 1\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("unset multiplot\n");
                                text<<QString("pause -1\n");
                                text<<QString("f(t)=a0+a1*exp(-t/t1)\n");
                                text<<QString("t1=50\n");
                                text<<QString("a0=0\n");
                                text<<QString("a1=500\n");
                                text<<QString("fit f(x) '%1' using 1:2 via a0, a1, t1\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("set label sprintf('1exp: a0=%f a1=%f t1=%f', a0, a1, t1) at graph 0.5,0.9\n");
                                text<<QString("plot '%1' using 1:2 title 'mean' with points, f(x) title 'exponential fit\n").arg(QFileInfo(statisticsFilename).fileName());
                                text<<QString("pause -1\n");
                                text<<QString("\n");
                                text<<QString("\n");
                                text<<QString("\n");
                                f1.close();
                            } else {
                                emit statusChanged(m_status);
                                emit messageChanged(tr("could not create statistics plot file '%1': %2!").arg(localFilename).arg(f1.errorString()));
                            }

                            localFilename=backstatisticsFilename=outputFilenameBase+".backstatistics.dat";
                            QFile fb(localFilename);
                            if (fb.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&fb);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving background statistics ..."));
                                int count=backstatistics_time.size();
                                for (int i=0; i<count; i++) {
                                    text<<backstatistics_time[i];
                                    if (i<backstatistics_mean.size()) text<<", "<<backstatistics_mean[i];
                                    else text<<", 0";
                                    if (i<backstatistics_std.size()) text<<", "<<backstatistics_std[i];
                                    else text<<", 0";
                                    if (i<backstatistics_min.size()) text<<", "<<backstatistics_min[i];
                                    else text<<", 0";
                                    if (i<backstatistics_max.size()) text<<", "<<backstatistics_max[i];
                                    else text<<", 0";
                                    text<<"\n";
                                }
                                fb.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create background statistics file '%1': %2!").arg(localFilename).arg(fb.errorString()));
                            }
                            localFilename=outputFilenameBase+".backstatistics.plt";
                            QFile fb1(localFilename);
                            if (fb1.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&fb1);
                                text.setLocale(outLocale);
                                text<<QString("set xlabel 'time [seconds]'\n");
                                text<<QString("set ylabel 'pixel grey value'\n");
                                text<<QString("set title \"Background Statistics '%1'\" noenhanced\n").arg(job.filename);
                                text<<QString("set style fill transparent solid 0.5 noborder\n");
                                text<<QString("set multiplot layout 1,2\n");
                                text<<QString("plot '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:4:5 title 'min/max' with filledcu, '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("unset multiplot\n");
                                text<<QString("pause -1\n");
                                text<<QString("set multiplot layout 2,2\n");
                                text<<QString("plot '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:3 title 'stddev' with lines lt 1\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:4 title 'min' with lines lt 1\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:5 title 'max' with lines lt 1\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("unset multiplot\n");
                                text<<QString("pause -1\n");
                                text<<QString("f(t)=a0+a1*exp(-t/t1)\n");
                                text<<QString("t1=50\n");
                                text<<QString("a0=0\n");
                                text<<QString("a1=500\n");
                                text<<QString("fit f(x) '%1' using 1:2 via a0, a1, t1\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("set label sprintf('1exp: a0=%f a1=%f t1=%f', a0, a1, t1) at graph 0.5,0.9\n");
                                text<<QString("plot '%1' using 1:2 title 'mean' with points, f(x) title 'exponential fit\n").arg(QFileInfo(backstatisticsFilename).fileName());
                                text<<QString("pause -1\n");
                                text<<QString("\n");
                                text<<QString("\n");
                                text<<QString("\n");
                                fb1.close();
                            } else {
                                emit statusChanged(m_status);
                                emit messageChanged(tr("could not create background statistics plot file '%1': %2!").arg(localFilename).arg(fb1.errorString()));
                            }

                            localFilename=uncorrectedStatisticsFilename=outputFilenameBase+".uncorrstatistics.dat";
                            QFile fu(localFilename);
                            if (fu.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&fu);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving background statistics ..."));
                                int count=statistics_uncorrected_time.size();
                                for (int i=0; i<count; i++) {
                                    text<<statistics_uncorrected_time[i];
                                    if (i<statistics_uncorrected_mean.size()) text<<", "<<statistics_uncorrected_mean[i];
                                    else text<<", 0";
                                    if (i<statistics_uncorrected_std.size()) text<<", "<<statistics_uncorrected_std[i];
                                    else text<<", 0";
                                    if (i<statistics_uncorrected_min.size()) text<<", "<<statistics_uncorrected_min[i];
                                    else text<<", 0";
                                    if (i<statistics_uncorrected_max.size()) text<<", "<<statistics_uncorrected_max[i];
                                    else text<<", 0";
                                    text<<"\n";
                                }
                                fu.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create uncorrected statistics file '%1': %2!").arg(localFilename).arg(fu.errorString()));
                            }
                            localFilename=outputFilenameBase+".uncorrstatistics.plt";
                            QFile fu1(localFilename);
                            if (fu1.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&fu1);
                                text.setLocale(outLocale);
                                text<<QString("set xlabel 'time [seconds]'\n");
                                text<<QString("set ylabel 'pixel grey value'\n");
                                text<<QString("set title \"Uncorrected Statistics '%1'\" noenhanced\n").arg(job.filename);
                                text<<QString("set style fill transparent solid 0.5 noborder\n");
                                text<<QString("set multiplot layout 1,2\n");
                                text<<QString("plot '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:4:5 title 'min/max' with filledcu, '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("unset multiplot\n");
                                text<<QString("pause -1\n");
                                text<<QString("set multiplot layout 2,2\n");
                                text<<QString("plot '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:3 title 'stddev' with lines lt 1\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:4 title 'min' with lines lt 1\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("plot '%1' using 1:5 title 'max' with lines lt 1\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("unset multiplot\n");
                                text<<QString("pause -1\n");
                                text<<QString("f(t)=a0+a1*exp(-t/t1)\n");
                                text<<QString("t1=50\n");
                                text<<QString("a0=0\n");
                                text<<QString("a1=500\n");
                                text<<QString("fit f(x) '%1' using 1:2 via a0, a1, t1\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("set label sprintf('1exp: a0=%f a1=%f t1=%f', a0, a1, t1) at graph 0.5,0.9\n");
                                text<<QString("plot '%1' using 1:2 title 'mean' with points, f(x) title 'exponential fit\n").arg(QFileInfo(uncorrectedStatisticsFilename).fileName());
                                text<<QString("pause -1\n");
                                text<<QString("\n");
                                text<<QString("\n");
                                text<<QString("\n");
                                fu1.close();
                            } else {
                                emit statusChanged(m_status);
                                emit messageChanged(tr("could not create uncorrected statistics plot file '%1': %2!").arg(localFilename).arg(fu1.errorString()));
                            }
                        }
                        emit progressIncrement(10);

                        //************** SAVE ACF
                        if ((m_status==1) && !was_canceled && job.acf && acf && acf_tau && acf_N>0) {
                            QString localFilename=acfFilename=outputFilenameBase+".autocorrelation.dat";
                            QString localFilename1=acfFilenameBin=outputFilenameBase+".autocorrelation.bin";

                            emit messageChanged(tr("saving autocorrelation ..."));
                            double* ccf[1]={acf};
                            double* ccferr[1]={acf_std};
                            double* ccfsegments[1]={acf_segments};
                            QString error;
                            //qDebug()<<"acf = "<<acf<<"  acf_std = "<<acf_std;
                            if (!saveCorrelationCSV(localFilename, acf_tau, ccf, ccferr, 1, acf_N, frame_width, frame_height, input_length, error,125)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create autocorrelation file '%1': %2!").arg(localFilename).arg(error));
                            }
                            if (!saveCorrelationBIN(localFilename1, acf_tau, ccf, ccferr, 1, acf_N, frame_width, frame_height, ccfsegments, error,125)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create binary autocorrelation file '%1': %2!").arg(localFilename1).arg(error));
                            }
                            if (QFile::exists(localFilename1)) addFiles.append(localFilename1);
                            else addFiles.append(localFilename);

                        }
                        emit progressIncrement(250);


                        //************** SAVE DCCF
                        if ((m_status==1) && !was_canceled && job.distanceCCF && dccf && dccf_tau && dccf_N>0) {
                            QString localFilename=dccfFilename=outputFilenameBase+".dccf.dat";
                            QString localFilename1=dccfFilenameBin=outputFilenameBase+".dccf.bin";

                            emit messageChanged(tr("saving distance distance crosscorrelation ..."));
                            double* ccf[1]={dccf};
                            double* ccferr[1]={dccf_std};
                            double* ccfsegments[1]={dccf_segments};
                            QString error;
                            if (!saveCorrelationCSV(localFilename, dccf_tau, ccf, ccferr, 1, dccf_N, frame_width, frame_height, input_length, error,125)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create distance crosscorrelation file '%1': %2!").arg(localFilename).arg(error));
                            }
                            if (!saveCorrelationBIN(localFilename1, dccf_tau, ccf, ccferr, 1, dccf_N, frame_width, frame_height, ccfsegments, error,125)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create binary distance crosscorrelation file '%1': %2!").arg(localFilename1).arg(error));
                            }
                            if (QFile::exists(localFilename1)) addFiles.append(localFilename1);
                            else addFiles.append(localFilename);


                        }
                        emit progressIncrement(250);


                        //************** SAVE CCF
                        if ((m_status==1) && !was_canceled && job.ccf && ccf_tau && ccf1 && ccf2 && ccf3 && ccf4 && ccf_N>0) {
                            QString localFilename=ccfFilename=outputFilenameBase+".crosscorrelation.dat";
                            QString localFilename1=ccfFilenameBin=outputFilenameBase+".crosscorrelation.bin";

                            emit messageChanged(tr("saving distance crosscorrelation ..."));
                            double* ccf[4]={ccf1, ccf2, ccf3, ccf4};
                            double* ccferr[4]={ccf1_std, ccf2_std, ccf3_std, ccf4_std};
                            double* ccfsegments[4]={ccf1_segments, ccf2_segments, ccf3_segments, ccf4_segments};
                            QString error;
                            if (!saveCorrelationCSV(localFilename, ccf_tau, ccf, ccferr, 4, ccf_N, frame_width, frame_height, input_length, error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create crosscorrelation file '%1': %2!").arg(localFilename).arg(error));
                            }
                            if (!saveCorrelationBIN(localFilename1, ccf_tau, ccf, ccferr, 4, ccf_N, frame_width, frame_height, ccfsegments, error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create binary crosscorrelation file '%1': %2!").arg(localFilename1).arg(error));
                            }
                            if (QFile::exists(localFilename1)) addFiles.append(localFilename1);
                            else addFiles.append(localFilename);

                        }
                        emit progressIncrement(10);

                        //************** SAVE SETTINGS
                        if ((m_status==1) && !was_canceled) {
                            emit messageChanged(tr("saving settings ..."));
                            QString& localFilename=configFilename;
                            QFile f(localFilename);
                            QDir d=QFileInfo(localFilename).absoluteDir();
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);

                                text<<"date/time                   : "<<QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss") << "\n";
                                text<<"input file                  : "<<d.relativeFilePath(job.filename) << "\n";
                                if (!job.descriptionFilename.isEmpty()) text<<"input description file      : " << d.relativeFilePath(job.descriptionFilename) << "\n";
                                if (!averageFilename.isEmpty())         text<<"overview image file         : " << d.relativeFilePath(averageFilename) << "\n";
                                if (!averageFilenameF.isEmpty())        text<<"overview image file real    : " << d.relativeFilePath(averageFilenameF) << "\n";
                                if (!stdFilename.isEmpty())             text<<"overview std image          : " << d.relativeFilePath(stdFilename) << "\n";
                                if (!backgroundFilename.isEmpty())      text<<"background image file       : " << d.relativeFilePath(backgroundFilename) << "\n";
                                if (!backstdFilename.isEmpty())         text<<"background stddev           : " << d.relativeFilePath(backstdFilename) << "\n";
                                if (!videoFilename.isEmpty())           text<<"video file                  : " << d.relativeFilePath(videoFilename) << "\n";
                                if (!statisticsFilename.isEmpty())      text<<"statistics file             : " << d.relativeFilePath(statisticsFilename) << "\n";
                                if (!backstatisticsFilename.isEmpty())         text<<"background statistics file  : " << d.relativeFilePath(backstatisticsFilename) << "\n";
                                if (!uncorrectedStatisticsFilename.isEmpty())  text<<"uncorrected statistics file : " << d.relativeFilePath(uncorrectedStatisticsFilename) << "\n";
                                if (!acfFilename.isEmpty())             text<<"autocorrelation file        : " << d.relativeFilePath(acfFilename) << "\n";
                                if (!acfFilenameBin.isEmpty())          text<<"bin. autocorrelation file   : " << d.relativeFilePath(acfFilenameBin) << "\n";
                                if (!ccfFilename.isEmpty())             text<<"crosscorrelation file       : " << d.relativeFilePath(ccfFilename) << "\n";
                                if (!ccfFilenameBin.isEmpty())          text<<"bin. crosscorrelation file  : " << d.relativeFilePath(ccfFilenameBin) << "\n";
                                if (!dccfFilename.isEmpty())            text<<"distance ccf file           : " << d.relativeFilePath(dccfFilename) << "\n";
                                if (!dccfFilenameBin.isEmpty())         text<<"bin. distance ccf file      : " << d.relativeFilePath(dccfFilenameBin) << "\n";
                                text<<"width                       : "<<outLocale.toString(frame_width) << "\n";
                                text<<"height                      : "<<outLocale.toString(frame_height) << "\n";
                                if (job.cameraSettingsGiven) {
                                    text<<"pixel width                 : "<<outLocale.toString(job.cameraPixelWidth) << "\n";
                                    text<<"pixel height                : "<<outLocale.toString(job.cameraPixelHeight) << "\n";
                                }
                                text<<"binning                     : "<<outLocale.toString(reader->getBinning()) << "\n";
                                text<<"interleaved binning         : "<< QString((reader->getInterleavedBinning())?"true":"false") << "\n";
                                text<<"averaging binning           : "<< QString((reader->getAverageBinning())?"true":"false") << "\n";
                                if (reader->getUseCropping()) {
                                    text<<"crop x0                     : "<<outLocale.toString(reader->getCropX0()) << "\n";
                                    text<<"crop x1                     : "<<outLocale.toString(reader->getCropX1()) << "\n";
                                    text<<"crop y0                     : "<<outLocale.toString(reader->getCropY0()) << "\n";
                                    text<<"crop y1                     : "<<outLocale.toString(reader->getCropY1()) << "\n";
                                }
                                text<<"frame count in file         : "<<outLocale.toString(frame_count) << "\n";
                                text<<"frame count                 : "<<outLocale.toString(frames) << "\n";
                                text<<"first frame                 : "<<outLocale.toString(first_frame) << "\n";
                                text<<"last frame                  : "<<outLocale.toString(first_frame+frames-1) << "\n";
                                text<<"correlator S                : "<<outLocale.toString(job.S) << "\n";
                                text<<"correlator m                : "<<outLocale.toString(job.m) << "\n";
                                text<<"correlator P                : "<<outLocale.toString(job.P) << "\n";
                                text<<"correlator type             : "<<outLocale.toString(job.correlator) << "\n";
                                text<<"correlator type name        : ";
                                switch(job.correlator) {
                                    case CORRELATOR_DIRECT:      text<<"direct\n"; break;
                                    case CORRELATOR_DIRECTAVG:   text<<"direct with averaging\n"; break;
                                    case CORRELATOR_DIRECT_INT:    text<<"direct, integer\n"; break;
                                    case CORRELATOR_DIRECTAVG_INT: text<<"direct with averaging, integer\n"; break;
                                    case CORRELATOR_MTAUALLMON:  text<<"multi-tau with monitors for all channels\n"; break;
                                    case CORRELATOR_MTAUONEMON:  text<<"multi-tau with a single monitor\n"; break;

                                    default: text<<"correlator type name        : unknown\n"; break;
                                }
                                text<<"smallest tau [s]            : "<<outLocale.toString(job.frameTime) << "\n";
                                text<<"baseline                    : "<<outLocale.toString(baseline) << "\n";
                                if (video && job.video) {
                                    text<<"video sum up                : "<<outLocale.toString(job.video_frames) << "\n";
                                    text<<"video frames                : "<<outLocale.toString(real_video_count) << "\n";
                                    text<<"video avgMin                : "<<outLocale.toString(videoAvgMin) << "\n";
                                    text<<"video avgMax                : "<<outLocale.toString(videoAvgMax) << "\n";
                                }
                                if (job.statistics) {
                                    text<<"statistics over             : "<<outLocale.toString(job.statistics_frames) << "\n";
                                }
                                if (job.distanceCCF) {
                                    text<<"DCCF Delta x                : "<<outLocale.toString(job.DCCFDeltaX) << "\n";
                                    text<<"DCCF Delta y                : "<<outLocale.toString(job.DCCFDeltaY) << "\n";
                                    text<<"DCCF frame width            : "<<outLocale.toString(dccfframe_width) << "\n";
                                    text<<"DCCF frame height           : "<<outLocale.toString(dccfframe_height) << "\n";

                                }
                                text<<"bleach correction           : ";
                                if (job.bleach==BLEACH_EXP) {
                                    text<<"remove mono-exponential f(t)=A*exp(-t/tau)\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
                                    //text<<"bleach decay constant [s]   : "<<outLocale.toString((double)job.bleachDecay*job.frameTime) << "\n";
                                    //text<<"bleach amplitude A          : "<<outLocale.toString((double)job.bleachA) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach time file            : "<<d.relativeFilePath(bleachTimeFilename) << "\n";
                                    text<<"bleach fit success          : "<<d.relativeFilePath(bleachFitSuccessFilename) << "\n";
                                    text<<"bleach timeseries t file    : "<<d.relativeFilePath(bleachtimesFilename) << "\n";
                                    text<<"bleach timeseries I file    : "<<d.relativeFilePath(bleachframesFilename) << "\n";
                                }  else if (job.bleach==BLEACH_NONE) {
                                        text<<"remove frame average\n";
                                        text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";

                                } else {
                                    text<<"none\n";
                                }
                                text<<"duration [s]                : "<<ptime.elapsed()/1000.0 << "\n";

                                f.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create settings file '%1': %2!").arg(localFilename).arg(f.errorString()));
                            }

                        }
                        emit progressIncrement(10);
                    } else {
                        if (mutexFilename) mutexFilename->unlock();
                        m_status=-1; emit statusChanged(m_status);
                        emit messageChanged(tr("could not create output subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                    }

                    if (video) free(video);
                    if (fit_frames) free(fit_frames);
                    if (fit_t) free(fit_t);
                    if (bleachOffset) free(bleachOffset);
                    if (bleachAmplitude) free(bleachAmplitude);
                    if (bleachTime) free(bleachTime);
                    if (bleachFitOK) free(bleachFitOK);
                    if (average_frame) free(average_frame);
                    if (sqrsum_frame) free(sqrsum_frame);
                    if (acf_tau) free(acf_tau);
                    if (acf) free(acf);
                    if (acf_segments) free(acf_segments);
                    if (acf_std) free(acf_std);
                    if (ccf_tau) free(ccf_tau);
                    if (ccf1) free(ccf1);
                    if (ccf2) free(ccf2);
                    if (ccf3) free(ccf3);
                    if (ccf4) free(ccf4);
                    if (ccf1_std) free(ccf1_std);
                    if (ccf2_std) free(ccf2_std);
                    if (ccf3_std) free(ccf3_std);
                    if (ccf4_std) free(ccf4_std);
                    if (ccf1_segments) free(ccf1_segments);
                    if (ccf2_segments) free(ccf2_segments);
                    if (ccf3_segments) free(ccf3_segments);
                    if (ccf4_segments) free(ccf4_segments);
                    if (dccf_tau) free(dccf_tau);
                    if (dccf) free(dccf);
                    if (dccf_std) free(dccf_std);
                    if (dccf_segments) free(dccf_segments);
                    if (m_status==1) {
                        if (was_canceled) {
                            m_status=-1; emit statusChanged(m_status);
                            duration=ptime.elapsed();
                            emit messageChanged(tr("canceled by user"));
                        } else {
                            m_status=2; emit statusChanged(m_status);
                            duration=ptime.elapsed();
                            emit messageChanged(tr("correlation ... done [duration = %1 s]").arg(duration/1.0e3));
                        }
                        emit statusChanged(m_status);
                    }
                } else {
                    m_status=-1; emit statusChanged(m_status);
                    duration=ptime.elapsed();
                    emit messageChanged(tr("no frames in file/selected"));
                }

                reader->close();
            }
            delete reader;
        } else {
            m_status=-1; emit statusChanged(m_status);
            duration=ptime.elapsed();
            emit messageChanged(tr("could not create image reader object"));
        }
    }


    duration=ptime.elapsed();
}


bool QFRDRImagingFCSCorrelationJobThread::saveCorrelationCSV(const QString &filename, double *corrTau, double **corrs, double** correrrs, uint32_t corrN, uint32_t N, uint32_t width, uint32_t height, double input_length, QString& error, int progress_steps) {
   /* qDebug()<<corrTau<<corrs<<correrrs<<corrN<<N<<width<<height<<input_length;
    for (uint32_t j=0; j<corrN; j++) {
        qDebug()<<"corrs["<<j<<"] = "<<corrs[j];
    }*/
    QFile f(filename);
    error="";
    int dataToWrite=width*height;
    int progressEvery=0;
    if (progress_steps>0) progressEvery=dataToWrite/progress_steps;
    int progress=0;
    int progem=0;
    bool ok=true;
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream text(&f);
        text.setLocale(outLocale);

        for (register uint32_t p=0; p<width*height; p++) {
            for (register uint32_t i=0; i<N; i++) {
                if (corrTau[i]<input_length)  {
                    text<<corrTau[i];
                    for (uint32_t j=0; j<corrN; j++) {
                       text<<", "<<(corrs[j])[p*N+i]-1.0;
                    }
                    if (correrrs && correrrs[0]) {
                        for (uint32_t j=0; j<corrN; j++) {
                           text<<", "<<(correrrs[j])[p*N+i]-1.0;
                        }
                    }
                    text<<"\n";
                } else break;
            }
            text<<"\n\n";
            progress++;
            if (progressEvery>0 && progress%progressEvery==0) { progem++; emit progressIncrement(1);}
        }
        f.close();
    } else {
        error=f.errorString();
        ok= false;
    }
    if (progressEvery>0 && progem<progress_steps)  { emit progressIncrement(progress_steps-progem); }

    return ok;
}

bool QFRDRImagingFCSCorrelationJobThread::saveCorrelationBIN(const QString &filename, double *corrTau, double **corrs, double** correrrs, uint32_t corrN, uint32_t N, uint32_t width, uint32_t height, double **corrSegments, QString& error, int progress_steps) {
    QFile f(filename);

    int dataToWrite=width*height*(job.segments+1);
    int progressEvery=0;
    if (progress_steps>0) progressEvery=dataToWrite/progress_steps;
    int progress=0;
    int progem=0;
    bool ok=true;
    if (f.open(QIODevice::WriteOnly)) {
        f.write("QF3.0imFCS"); // write header
        binfileWriteUint32(f, width);
        binfileWriteUint32(f, height);
        binfileWriteUint32(f, corrN);
        binfileWriteUint32(f, N);
        uint32_t sets=(correrrs==NULL || job.segments<=1)?1:2;
        binfileWriteUint32(f, sets);

        binfileWriteDoubleArray(f, corrTau, N);

        for (register uint32_t p=0; p<width*height; p++) {
            for (uint32_t j=0; j<corrN; j++) {
                binfileWriteDoubleArrayMinus1(f, &((corrs[j])[p*N]), N);
                /*for (register uint32_t i=0; i<N; i++) {
                    const double d=qToLittleEndian((corrs[j])[p*N+i]-1.0);  f.write((char*)(&d), sizeof(d));
                }*/
            }
            if (sets>1) {
                for (uint32_t j=0; j<corrN; j++) {
                    binfileWriteDoubleArray(f, &((correrrs[j])[p*N]), N);
                    /*for (register uint32_t i=0; i<N; i++) {
                        const double d=qToLittleEndian((correrrs[j])[p*N+i]-1.0);  f.write((char*)(&d), sizeof(d));
                    }*/
                }
            }

            progress++;
            if (progressEvery>0 && progress%progressEvery==0) {progem++; emit progressIncrement(1); }
        }

        if (job.segments>1 && corrSegments[0]) {
            binfileWriteUint32(f, job.segments);
            for (register uint32_t p=0; p<width*height; p++) {
                for (int seg=0; seg<job.segments; seg++) {
                    for (uint32_t j=0; j<corrN; j++) {
                        binfileWriteDoubleArrayMinus1(f, &((corrSegments[j])[seg*corrN*N+p*N]), N);
                        /*for (register uint32_t i=0; i<N; i++) {
                            const double d=qToLittleEndian((corrSegments[j])[seg*corrN*N+p*N+i]-1.0);  f.write((char*)(&d), sizeof(d));
                        }*/
                    }
                    progress++;
                    if (progressEvery>0 && progress%progressEvery==0) {progem++; emit progressIncrement(1); }
                }
            }
        }


        f.close();
    } else {
        ok= false;
    }
    if (progressEvery>0 && progem<progress_steps)  { emit progressIncrement(progress_steps-progem); }
    return ok;
}


void QFRDRImagingFCSCorrelationJobThread::correlate_series(float* image_series, uint32_t frame_width, uint32_t frame_height, uint32_t shiftX, uint32_t shiftY, uint64_t frames, double **ccf_tau_io, double **ccf_io, double **ccf_std_io, uint32_t &ccf_N, const QString& message, uint32_t increment_progress, double **ccf_segments_io) {
    ccf_N=job.S*job.P;
    double* ccf=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
    double* ccf_std=NULL;
    double* ccf_tau=(double*)calloc(ccf_N,sizeof(double));
    long* ccf_t=(long*)calloc(ccf_N,sizeof(long));
    if (ccf_segments_io && job.segments>1) *ccf_segments_io=(double*)calloc(ccf_N*frame_width*frame_height*job.segments,sizeof(double));

    statisticsAutocorrelateCreateMultiTau(ccf_t, job.S, job.m, job.P);

    for (uint32_t nn=0; nn<ccf_N*frame_width*frame_height; nn++) {
        ccf[nn]=1.0;
    }
    if (job.segments>1) {
        ccf_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
    }

    for (int32_t y=0; y<(int32_t)frame_height; y++) {
        for (int32_t x=0; x<(int32_t)frame_width; x++) {
            int32_t p=y*(int32_t)frame_width+x;
            int32_t x1=x+shiftX;
            int32_t y1=y+shiftY;
            int32_t p2=x1+y1*(int32_t)frame_width;


            if (job.segments<=1) {
                if (x1>=0 && x1<(int32_t)frame_width && y1>=0 && y1<(int32_t)frame_height) {
                    if (job.correlator==CORRELATOR_DIRECT) {
                        statisticsCrosscorrelateMultiTauSymmetricMemOptimized<float, double>(&(ccf[p*ccf_N]), &(image_series[p]), &(image_series[p2]), frames, ccf_t, ccf_N, frame_width*frame_height);
                    } else if (job.correlator==CORRELATOR_DIRECTAVG) {
                        statisticsCrosscorrelateMultiTauAvgSymmetric<float, double, double>(&(ccf[p*ccf_N]), &(image_series[p]), &(image_series[p2]), frames, job.S, job.m, job.P, frame_width*frame_height);
                    } else if (job.correlator==CORRELATOR_DIRECT_INT) {
                        statisticsCrosscorrelateMultiTauSymmetricMemOptimized<float,  uint64_t>(&(ccf[p*ccf_N]), &(image_series[p]), &(image_series[p2]), frames, ccf_t, ccf_N, frame_width*frame_height);
                    } else if (job.correlator==CORRELATOR_DIRECTAVG_INT) {
                        statisticsCrosscorrelateMultiTauAvgSymmetric<float, double, uint64_t>(&(ccf[p*ccf_N]), &(image_series[p]), &(image_series[p2]), frames, job.S, job.m, job.P, frame_width*frame_height);
                    }
                }
            } else {
                uint32_t segment_frames=frames/job.segments;
                double* cftemp=(double*)calloc(ccf_N,sizeof(double));
                double* sum=(double*)calloc(ccf_N,sizeof(double));
                double* sum2=(double*)calloc(ccf_N,sizeof(double));
                for (register uint32_t ct=0; ct<ccf_N; ct++) {
                    sum[ct]=0;
                    sum2[ct]=0;
                }
                for (int32_t seg=0; seg<job.segments; seg++) {
                    for (register uint32_t ct=0; ct<ccf_N; ct++) cftemp[ct]=0;
                    if (x1>=0 && x1<(int32_t)frame_width && y1>=0 && y1<(int32_t)frame_height) {
                        if (job.correlator==CORRELATOR_DIRECT) {
                            statisticsCrosscorrelateMultiTauSymmetricMemOptimized<float,  double>(cftemp, &(image_series[seg*segment_frames*frame_width*frame_height+p]), &(image_series[seg*segment_frames*frame_width*frame_height+p2]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                        } else if (job.correlator==CORRELATOR_DIRECTAVG) {
                            statisticsCrosscorrelateMultiTauAvgSymmetric<float, double, double>(cftemp, &(image_series[seg*segment_frames*frame_width*frame_height+p]), &(image_series[seg*segment_frames*frame_width*frame_height+p2]), segment_frames, job.S, job.m, job.P, frame_width*frame_height);
                        } else if (job.correlator==CORRELATOR_DIRECT_INT) {
                            statisticsCrosscorrelateMultiTauSymmetricMemOptimized<float,  uint64_t>(cftemp, &(image_series[seg*segment_frames*frame_width*frame_height+p]), &(image_series[seg*segment_frames*frame_width*frame_height+p2]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                        } else if (job.correlator==CORRELATOR_DIRECTAVG_INT) {
                            statisticsCrosscorrelateMultiTauAvgSymmetric<float, double, uint64_t>(cftemp, &(image_series[seg*segment_frames*frame_width*frame_height+p]), &(image_series[seg*segment_frames*frame_width*frame_height+p2]), segment_frames, job.S, job.m, job.P, frame_width*frame_height);
                        }
                        if (ccf_segments_io) {
                            for (register uint32_t ct=0; ct<ccf_N; ct++) {
                                (*ccf_segments_io)[seg*frame_width*frame_height*ccf_N + p*ccf_N + ct]=cftemp[ct];
                            }
                        }
                        for (register uint32_t ct=0; ct<ccf_N; ct++) {
                            sum[ct]  += cftemp[ct];
                            sum2[ct] += cftemp[ct]*cftemp[ct];
                        }
                    } else {
                        for (register uint32_t ct=0; ct<ccf_N; ct++) {
                            sum[ct]  += 1.0;
                            sum2[ct] += 1.0*1.0;
                        }
                    }
                }
                free(cftemp);
                double segs=job.segments;
                for (register uint32_t ct=0; ct<ccf_N; ct++) {
                    ccf[p*ccf_N+ct]=sum[0*ccf_N+ct]/segs;
                    ccf_std[p*ccf_N+ct]=sqrt((sum2[0*ccf_N+ct]-sum[0*ccf_N+ct]*sum[0*ccf_N+ct]/segs)/(segs-1.0));
                }
                free(sum);
                free(sum2);
            }

            if (m_status==1 && !was_canceled) {
                if (frame_width*frame_height<increment_progress) {
                    emit messageChanged(tr("%3 (%1/%2)...").arg(p+1).arg(frame_width*frame_height).arg(message)); emit progressIncrement(increment_progress/(frame_width*frame_height));
                } else if (p%(frame_width*frame_height/increment_progress)==0) {
                    emit messageChanged(tr("%3 (%1/%2)...").arg(p+1).arg(frame_width*frame_height).arg(message)); emit progressIncrement(1);
                }

            } else {
                break;
            }
        }
    }
    for (uint32_t i=0; i<ccf_N; i++) {
        ccf_tau[i]=(double)ccf_t[i]*job.frameTime;
    }
    free(ccf_t);


    *ccf_tau_io=ccf_tau;
    *ccf_io=ccf;
    *ccf_std_io=ccf_std;

}




void QFRDRImagingFCSCorrelationJobThread::correlate_loadall() {
    float* image_series=NULL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // AS NOT ENOUGH MEMORY MAY BE AVAILABLE; WE WAIT HERE UNTIL ENOUGH MEMORY IS AVAILABLE AND MAY BE ALLOCATED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    emit messageChanged(tr("allocating memory ..."));
    frames_min=0;
    frames_max=0;
    while ((image_series==NULL) && (!was_canceled)) {
        image_series=(float*)calloc((frames+2)*frame_width*frame_height,sizeof(float));
        if (!image_series) {
            emit messageChanged(tr("waiting for available memory"));
        }
        msleep(50);
    }
    if (image_series) {
        for (register uint64_t i=0; i<(frames+2)*frame_width*frame_height; i++) {
            image_series[i]=0;
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOW WE READ ALL FRAMES IN THE TIFF FILE INTO THE flot* ARRAY ALLOCATED BEFORE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    firstFrames=(float*)calloc(frame_width*frame_height,sizeof(float));
    lastFrames=(float*)calloc(frame_width*frame_height,sizeof(float));

    if (!was_canceled) {
        emit messageChanged(tr("reading frames ..."));

        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected;
        stat_state_uncorrected.sum=0;
        stat_state_uncorrected.sum2=0;
        stat_state_uncorrected.sframe_min=0;
        stat_state_uncorrected.sframe_max=0;
        stat_state_uncorrected.cnt=0;
        stat_state_uncorrected.statFirst=true;
        stat_state_uncorrected.video_frame=NULL;
        statistics_uncorrected_mean.clear();
        statistics_uncorrected_std.clear();
        statistics_uncorrected_min.clear();
        statistics_uncorrected_max.clear();
        statistics_uncorrected_time.clear();
        uint16_t video_frame_num=0;
        float fframes_min=0;
        float fframes_max=0;

        register uint32_t frame=0;
        //float sframe_min=0;
        //float sframe_max=0;
        float* frame_data=(float*)calloc(frame_width*frame_height,sizeof(float));
        for (register uint16 i=0; i<frame_width*frame_height; i++) {
            frame_data[i]=0;
            firstFrames[i]=0;
            lastFrames[i]=0;
        }
        bool OK=true;
        while (OK && (frame<first_frame)) {
            OK=reader->nextFrame();
            frame++;
        }
        frame=0;
        if (!OK) {
            m_status=-1; emit statusChanged(m_status);
            emit messageChanged(tr("error reading frame: %1").arg(reader->lastError()));
        } else {
            do {
                if (!reader->readFrameFloat(frame_data)) {
                    m_status=-1; emit statusChanged(m_status);
                    emit messageChanged(tr("error reading frame: %1").arg(reader->lastError()));
                } else {
                    contribute_to_statistics(stat_state_uncorrected, frame_data, frame_width, frame_height, frame, frames, NULL, NULL, NULL, video_frame_num, fframes_min, fframes_max, statistics_uncorrected_time, statistics_uncorrected_mean, statistics_uncorrected_std, statistics_uncorrected_min, statistics_uncorrected_max);

                    float frame_min=frame_data[0];
                    float frame_max=frame_data[0];

                    for (register uint16 i=0; i<frame_width*frame_height; i++) {
                        register float v=frame_data[i];
                        image_series[frame*frame_width*frame_height+i] = v;
                        frame_min=(v<frame_min)?v:frame_min;
                        frame_max=(v>frame_max)?v:frame_max;
                    }
                    if (frame==0) {
                        frames_min=frame_min;
                        frames_max=frame_max;
                    } else {
                        frames_min=(frame_min<frames_min)?frame_min:frames_min;
                        frames_max=(frame_max>frames_max)?frame_max:frames_max;
                    }
                    if (frame<job.bleachAvgFrames) {
                        for (register uint16 i=0; i<frame_width*frame_height; i++) {
                            firstFrames[i]=firstFrames[i]+(float)frame_data[i]/(float)job.bleachAvgFrames;
                        }
                    } else if (frame>frames-job.bleachAvgFrames) {
                        for (register uint16 i=0; i<frame_width*frame_height; i++) {
                            lastFrames[i]=lastFrames[i]+(float)frame_data[i]/(float)job.bleachAvgFrames;
                        }
                    }

                    if ((frames/500>0) && (frame%(frames/500)==0)) {
                        emit messageChanged(tr("reading frames (%1/%2)...").arg(frame).arg(frames));
                        emit progressIncrement(1);
                    }
                }
                frame++;
                if (was_canceled) break;
            } while (reader->nextFrame() && (m_status==1) && (frame<frames));
            if (frames/500<=0) emit progressIncrement(500);

        }
        free(frame_data);
    }

    if (m_status==1 && !was_canceled) {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // NOW WE CORRECT THE IMAGE FOR IT'S BASELINE (ACCORDING TO THE USER SETTINGS)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        calcBackgroundCorrection();
        NFitFrames=qMin((int64_t)300, (int64_t)frames);
        fit_t=(double*)calloc(NFitFrames,sizeof(double));
        fit_frames=(float*)calloc(NFitFrames*frame_width*frame_height,sizeof(float));
        if (m_status==1 && !was_canceled ) {
            emit messageChanged(tr("applying baseline correction..."));
            for (register uint32_t i=0; i<frame_width*frame_height*frames; i++) {
                image_series[i]=image_series[i]-baseline-backgroundImage[i%(frame_width*frame_height)];
            }
            for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                firstFrames[i]=firstFrames[i]-baseline-backgroundImage[i%(frame_width*frame_height)];
                lastFrames[i]=lastFrames[i]-baseline-backgroundImage[i%(frame_width*frame_height)];
            }
            for (int i=0; i<NFitFrames; i++) {
                int iidx=trunc((double)i/(double)(NFitFrames-1)*(double)frames);
                if (iidx<0) iidx=0;
                if (iidx>=frames) iidx=frames-1;
                fit_t[i]=(double)iidx+(double)job.bleachAvgFrames/2.0;
                //qDebug()<<i<<"/"<<NFitFrames<<":    "<<iidx<<"/"<<frames;
                for (int idx=0; idx<frame_width*frame_height; idx++) {
                    fit_frames[i*frame_width*frame_height+idx]=image_series[iidx*frame_width*frame_height+idx];
                }
                if (job.bleachAvgFrames>1) {
                    float sum=1;
                    for (int f=1; f<job.bleachAvgFrames; f++) {
                        if (iidx+f<frames) {
                            for (int idx=0; idx<frame_width*frame_height; idx++) {
                                fit_frames[i*frame_width*frame_height+idx]+=image_series[(iidx+f)*frame_width*frame_height+idx];
                            }
                            sum++;
                        }
                    }
                    for (int idx=0; idx<frame_width*frame_height; idx++) {
                        fit_frames[i*frame_width*frame_height+idx]/=sum;
                    }
                }
            }

        }
        QApplication::processEvents();
        QApplication::processEvents();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // NOW WE CORRECT THE IMAGE FOR BLEACHING
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (m_status==1 && !was_canceled ) {
            emit messageChanged(tr("calculating bleach correction parameters ..."));
            calcBleachCorrection(fit_frames, fit_t, NFitFrames);
            emit messageChanged(tr("applying bleach correction ..."));
            if (job.bleach==BLEACH_EXP) {
                for (register uint64_t t=0; t<frames; t++) {
                    for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                        uint64_t idx=t*frame_width*frame_height+i;
                        //image_series[idx]=image_series[idx]*firstFrames[i]/(bleachOffset[i]+bleachAmplitude[i]*exp(-1.0*(double)t/bleachTime[i]));
                        if (bleachFitOK[i]!=0) {
                            double expT=bleachAmplitude[i]*exp(-(double)t/bleachTime[i]);
                            double exp0=bleachAmplitude[i];
                            image_series[idx]=image_series[idx]/sqrt(expT/exp0)+exp0*(1.0-sqrt(expT/exp0));
                                              //*firstFrames[i]/(bleachOffset[i]+bleachAmplitude[i]*exp(-1.0*(double)t/bleachTime[i]));
                        }
                    }
                }
            } else if (job.bleach==BLEACH_REMOVEAVG) {
                for (register uint64_t t=0; t<frames; t++) {
                    double avg=0;
                    for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                        uint64_t idx=t*frame_width*frame_height+i;
                        avg=avg+image_series[idx]/ ((double)(frame_width*frame_height));
                    }
                    for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                        uint64_t idx=t*frame_width*frame_height+i;
                        image_series[idx]=image_series[idx]*firstFrames[i]/avg;
                    }
                }
            }
        }
    }

    if (m_status==1 && !was_canceled) {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // NOW WE CREATE THE VIDEO AND THE STATISTICS (from the corrected sequence!!!)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////



        emit messageChanged(tr("calculating statistics/video ..."));
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state;
        stat_state.sum=0;
        stat_state.sum2=0;
        stat_state.sframe_min=0;
        stat_state.sframe_max=0;
        stat_state.cnt=0;
        stat_state.statFirst=true;
        stat_state.video_frame=(float*)calloc(frame_width*frame_height, sizeof(float));
        for (int64_t i=0; i<frame_width*frame_height; i++)  {
            stat_state.video_frame[i]=0;
        }
        statistics_mean.clear();
        statistics_std.clear();
        statistics_min.clear();
        statistics_max.clear();
        statistics_time.clear();
        uint16_t video_frame_num=0;

        //bool statFirst=true;
        for(register uint32_t frame=0; frame<frames; frame++) {
            float* frame_data=&(image_series[frame*frame_width*frame_height]);
            contribute_to_statistics(stat_state, frame_data, frame_width, frame_height, frame, frames, &average_frame, &sqrsum_frame, &video, video_frame_num, frames_min, frames_max, statistics_time, statistics_mean, statistics_std, statistics_min, statistics_max);

            if (frames<500) {
                emit messageChanged(tr("calculating statistics/video (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(500/frames);
            } else if ((frames/500==0) || (frame%(frames/500)==0)) {
                emit messageChanged(tr("calculating statistics/video (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(2);
            }

            real_video_count=video_frame_num;


            if (was_canceled) break;
        }


        free(stat_state.video_frame);

    }

    if (m_status==1 && !was_canceled) {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // CALCULATE THE ACFs
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (job.acf) {
            emit messageChanged(tr("calculating autocorrelations ..."));

            correlate_series(image_series, frame_width, frame_height, 0,0, frames, &acf_tau, &acf, &acf_std, acf_N, tr("calculating autocorrelation"), 1000, &acf_segments);

        } else {
            emit progressIncrement(1000);
        }

    }

    if (m_status==1 && !was_canceled) {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // CALCULATE THE CCFs
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (job.ccf) {
            emit messageChanged(tr("calculating crosscorrelations ..."));

            correlate_series(image_series, frame_width, frame_height, -1,0, frames, &ccf_tau, &ccf1, &ccf1_std, ccf_N, tr("calculating left crosscorrelation"), 250, &ccf1_segments);
            correlate_series(image_series, frame_width, frame_height, +1,0, frames, &ccf_tau, &ccf2, &ccf2_std, ccf_N, tr("calculating right crosscorrelation"), 250, &ccf2_segments);
            correlate_series(image_series, frame_width, frame_height, 0,-1, frames, &ccf_tau, &ccf3, &ccf3_std, ccf_N, tr("calculating up crosscorrelation"), 250, &ccf3_segments);
            correlate_series(image_series, frame_width, frame_height, 0,+1, frames, &ccf_tau, &ccf4, &ccf4_std, ccf_N, tr("calculating down crosscorrelation"), 250, &ccf4_segments);

        } else {
            emit progressIncrement(1000);
        }
    }




    if (m_status==1 && !was_canceled) {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // CALCULATE THE DCCFs
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (job.distanceCCF) {
            emit messageChanged(tr("calculating distance crosscorrelations ..."));


            dccfframe_width=frame_width;//-abs(job.DCCFDeltaX);
            dccfframe_height=frame_height;//-abs(job.DCCFDeltaY);

            correlate_series(image_series, frame_width, frame_height, job.DCCFDeltaX, job.DCCFDeltaY, frames, &dccf_tau, &dccf, &dccf_std, dccf_N, tr("calculating left crosscorrelation"), 1000, &dccf_segments);

        } else {
            emit progressIncrement(1000);
        }
    }



    if (firstFrames) free(firstFrames);
    if (lastFrames) free(lastFrames);
    if (image_series) free(image_series);
    image_series=NULL;
}




void QFRDRImagingFCSCorrelationJobThread::contribute_to_correlations(QList<MultiTauCorrelator<double, double>* >& ccfjk, QList<correlatorjb<double, double>* >& ccfjb, float* frame_data, uint32_t frame_width, uint32_t frame_height, uint32_t shiftX, uint32_t shiftY, uint64_t frame, uint64_t segment_frames, double *ccf_tau, double *ccf, double *ccf_std, uint64_t ccf_N) {
    if (job.correlator==CORRELATOR_MTAUALLMON) {
        uint16_t i=0;
        for (register uint16 y=0; y<frame_height; y++) {
            for (register uint16 x=0; x<frame_width; x++) {

                const int32_t p=y*frame_width+x;
                const int32_t x1=x+shiftX;
                const int32_t y1=y+shiftY;
                const int32_t p1=y1*frame_width+x1;
                if ((x1>=0) && (x1<(int32_t)frame_width) && (y1>=0) && (y1<(int32_t)frame_height)) {

                    ccfjk[i]->crosscorrelate_step(frame_data[p], frame_data[p1]);

                    if ((frame%segment_frames)==(segment_frames-1)) {
                        ccfjk[i]->crossnormalize();
                        //qDebug()<<"normalize jk ("<<x<<", "<<y<<": "<<i<<") !";
                        double* corr1=ccfjk[i]->getCor();
                        for (register uint32_t tt=0; tt<ccf_N; tt++) {
                            register double v=corr1[tt];
                            ccf[i*ccf_N+tt]=ccf[i*ccf_N+tt]+v;
                            ccf_std[i*ccf_N+tt]=ccf_std[i*ccf_N+tt]+v*v;
                        }
                        delete ccfjk[i];
                        ccfjk[i]=new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime);
                    }
                }
                i++;
            }
        }
    } else {
        uint16_t i=0;
        for (register uint16 y=0; y<frame_height; y++) {
            for (register uint16 x=0; x<frame_width; x++) {

                const int32_t p=y*frame_width+x;
                const int32_t x1=x+shiftX;
                const int32_t y1=y+shiftY;
                const int32_t p1=y1*frame_width+x1;
                if ((x1>=0) && (x1<(int32_t)frame_width) && (y1>=0) && (y1<(int32_t)frame_height)) {

                    ccfjb[i]->run(frame_data[p], frame_data[p1]);

                    if ((frame%segment_frames)==(segment_frames-1)) {
                        double** corr1=ccfjb[i]->get_array_G();
                        //qDebug()<<"normalize jb ("<<x<<", "<<y<<": "<<i<<") !";
                        for (register uint32_t tt=0; tt<ccf_N; tt++) {
                            register double v=corr1[1][tt];
                            ccf[i*ccf_N+tt]=ccf[i*ccf_N+tt]+v;
                            ccf_std[i*ccf_N+tt]=ccf_std[i*ccf_N+tt]+v*v;
                        }
                        delete ccfjb[i];
                        ccfjb[i]=new correlatorjb<double, double>(job.S, job.P, 0.0);
                        free(corr1[0]);
                        free(corr1[1]);
                    }
                }
                i++;
            }
        }
    }

}

void QFRDRImagingFCSCorrelationJobThread::average_ccfs(double** acf, double** acf_std, uint32_t acf_N, uint32_t frame_width, uint32_t frame_height, uint32_t segments) {
    for (register uint64_t tt=0; tt<acf_N*frame_width*frame_height; tt++) {
        double sum=(*acf)[tt];
        double sum2=(*acf_std)[tt];
        (*acf)[tt]=sum/(double)segments;
        if (segments>1) {
            (*acf_std)[tt]=sqrt((sum2-sum*sum/(double)segments)/(double)(segments-1));
        } else {
            (*acf_std)[tt]=0;
        }
        if (was_canceled) break;
    }
    if (segments<=1) {
        free(*acf_std);
        *acf_std=NULL;
    }
}

void QFRDRImagingFCSCorrelationJobThread::prepare_ccfs(QList<MultiTauCorrelator<double, double> *> &acfjk, QList<correlatorjb<double, double> *> &acfjb, double **acf, double **acf_std, double **acf_t, uint32_t& acf_N, uint32_t frame_width, uint32_t frame_height, uint32_t segments) {
    if (job.correlator==CORRELATOR_MTAUALLMON) {
        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
            acfjk.append(new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime));
        }
        acf_N=acfjk[0]->getSlots();
    } else {
        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
            acfjb.append(new correlatorjb<double, double>(job.S, job.P, 0.0));
        }
        acf_N=job.S*job.P;
    }

    if (acf_N>0) {
        (*acf)=(double*)calloc(acf_N*frame_width*frame_height,sizeof(double));
        for (uint64_t nn=0; nn<acf_N*frame_width*frame_height; nn++) {
            (*acf)[nn]=1.0;
        }
        (*acf_std)=(double*)calloc(acf_N*frame_width*frame_height,sizeof(double));
        for (uint64_t nn=0; nn<acf_N*frame_width*frame_height; nn++) {
            (*acf_std)[nn]=1.0;
        }
        (*acf_t)=(double*)calloc(acf_N,sizeof(double));
        if (job.correlator==CORRELATOR_MTAUALLMON) {
            double* tau=acfjk[0]->getCorTau();
            for (uint64_t tt=0; tt<acf_N; tt++) {
                (*acf_t)[tt]=tau[tt];
            }
        } else {
            double** corr1=acfjb[0]->get_array_G();
            for (uint64_t tt=0; tt<acf_N; tt++) {
                (*acf_t)[tt]=job.frameTime*corr1[0][tt];
            }
            free(corr1[0]);
            free(corr1[1]);
        }
    }
}


void QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics(QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state& state, float* frame_data, uint16_t frame_width, uint16_t frame_height, uint32_t frame, uint32_t frames, float** average_frame, float** sqrsum_frame, float** video, uint16_t& video_frame_num, float& frames_min, float& frames_max, QVector<float>& statistics_time, QVector<float>& statistics_mean, QVector<float>& statistics_std, QVector<float>& statistics_min, QVector<float>& statistics_max, bool isBackground) {

    float frame_min=frame_data[0];
    float frame_max=frame_data[0];
    for (register uint16 i=0; i<frame_width*frame_height; i++) {
        register float v=frame_data[i];
        if (QFFloatIsOK(v)) {
            frame_min=(v<frame_min)?v:frame_min;
            frame_max=(v>frame_max)?v:frame_max;
            if (average_frame) (*average_frame)[i]=(*average_frame)[i]+(float)v/(float)frames;
            if (sqrsum_frame) (*sqrsum_frame)[i]=(*sqrsum_frame)[i]+(float)v*(float)v/(float)frames;
            state.sum+=v;
            state.sum2+=(v*v);
            if (state.video_frame) state.video_frame[i]=state.video_frame[i]+(float)v/(float)job.video_frames;
            state.cnt++;
        }
    }
    if (frame==0) {
        frames_min=frame_min;
        frames_max=frame_max;
    } else {
        frames_min=(frame_min<frames_min)?frame_min:frames_min;
        frames_max=(frame_max>frames_max)?frame_max:frames_max;
    }
    if (state.statFirst) {
        state.sframe_min=frame_min;
        state.sframe_max=frame_max;
        state.statFirst=false;
    } else {
        state.sframe_min=(frame_min<state.sframe_min)?frame_min:state.sframe_min;
        state.sframe_max=(frame_max>state.sframe_max)?frame_max:state.sframe_max;
    }
    if ((!isBackground && job.statistics && ((frame+1)%job.statistics_frames==0))
        || (isBackground && job.statistics && ((frame+1)%job.backstatistics_frames==0))){
        float N=state.cnt;//frame_width*frame_height*job.statistics_frames;
        if (N==0) N=2;
        statistics_time.append((float)frame*job.frameTime);
        statistics_mean.append(state.sum/N);
        statistics_min.append(state.sframe_min);
        statistics_max.append(state.sframe_max);
        if (!isBackground && job.statistics_frames>1)         statistics_std.append(sqrt((state.sum2-state.sum*state.sum/N)/(N-1.0)));
        else if (isBackground && job.backstatistics_frames>1) statistics_std.append(sqrt((state.sum2-state.sum*state.sum/N)/(N-1.0)));
        state.sum=0;
        state.sum2=0;
        state.sframe_min=0;
        state.sframe_max=0;
        state.statFirst=true;
        state.cnt=0;
    }
    if (!isBackground && job.video && state.video_frame && ((frame+1)%job.video_frames==0) && video){
        for (register uint32_t i=0; i<(uint32_t)frame_width*(uint32_t)frame_height; i++) {
            (*video)[video_frame_num*frame_width*frame_height+i]=state.video_frame[i];
            state.video_frame[i]=0;
        }
        video_frame_num++;
    }

}


void QFRDRImagingFCSCorrelationJobThread::correlate_loadsingle() {
    QList<MultiTauCorrelator<double, double>* > acfjk;
    QList<MultiTauCorrelator<double, double>* > ccf1jk;
    QList<MultiTauCorrelator<double, double>* > ccf2jk;
    QList<MultiTauCorrelator<double, double>* > ccf3jk;
    QList<MultiTauCorrelator<double, double>* > ccf4jk;
    QList<MultiTauCorrelator<double, double>* > dccfjk;

    QList<correlatorjb<double, double>* > acfjb;
    QList<correlatorjb<double, double>* > ccf1jb;
    QList<correlatorjb<double, double>* > ccf2jb;
    QList<correlatorjb<double, double>* > ccf3jb;
    QList<correlatorjb<double, double>* > ccf4jb;
    QList<correlatorjb<double, double>* > dccfjb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // AS NOT ENOUGH MEMORY MAY BE AVAILABLE; WE WAIT HERE UNTIL ENOUGH MEMORY IS AVAILABLE AND MAY BE ALLOCATED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    emit messageChanged(tr("allocating memory ..."));
    frames_min=0;
    frames_max=0;
    if (job.distanceCCF) {
        dccfframe_width=frame_width;//-abs(job.DCCFDeltaX);
        dccfframe_height=frame_height;//-abs(job.DCCFDeltaY);
    }
    firstFrames=(float*)calloc(frame_width*frame_height,sizeof(float));
    lastFrames=(float*)calloc(frame_width*frame_height,sizeof(float));
    NFitFrames=qMin((int64_t)300, (int64_t)frames);
    fit_t=(double*)calloc(NFitFrames,sizeof(double));
    fit_frames=(float*)calloc(NFitFrames*frame_width*frame_height,sizeof(float));


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOW WE READ ALL FRAMES IN THE TIFF FILE TO EXTRACT SOME BASIC STATISTICS INFORMATION ABOUT THE FRAMES (used for background correction)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled) {
        emit messageChanged(tr("reading frames ..."));
        register uint32_t frame=0;


        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected;
        stat_state_uncorrected.sum=0;
        stat_state_uncorrected.sum2=0;
        stat_state_uncorrected.sframe_min=0;
        stat_state_uncorrected.sframe_max=0;
        stat_state_uncorrected.cnt=0;
        stat_state_uncorrected.statFirst=true;
        stat_state_uncorrected.video_frame=NULL;
        statistics_uncorrected_mean.clear();
        statistics_uncorrected_std.clear();
        statistics_uncorrected_min.clear();
        statistics_uncorrected_max.clear();
        statistics_uncorrected_time.clear();
        uint16_t video_frame_num=0;
        float fframes_min=0;
        float fframes_max=0;

        float* frame_data=(float*)calloc(frame_width*frame_height,sizeof(float));
        for (int64_t i=0; i<frame_width*frame_height; i++)  {
            frame_data[i]=0;
        }
        bool OK=true;
        while (OK && (frame<first_frame)) {
            OK=reader->nextFrame();
            frame++;
        }
        frame=0;
        do {
            if (!reader->readFrameFloat(frame_data)) {
                m_status=-1; emit statusChanged(m_status);
                emit messageChanged(tr("error reading frame: %1").arg(reader->lastError()));
            } else {
                contribute_to_statistics(stat_state_uncorrected, frame_data, frame_width, frame_height, frame, frames, NULL, NULL, NULL, video_frame_num, fframes_min, fframes_max, statistics_uncorrected_time, statistics_uncorrected_mean, statistics_uncorrected_std, statistics_uncorrected_min, statistics_uncorrected_max);

                float frame_min=frame_data[0];
                float frame_max=frame_data[0];
                for (register uint16 i=0; i<frame_width*frame_height; i++) {
                    register float v=frame_data[i];
                    frame_min=(v<frame_min)?v:frame_min;
                    frame_max=(v>frame_max)?v:frame_max;
                }
                if (frame==0) {
                    frames_min=frame_min;
                    frames_max=frame_max;
                } else {
                    frames_min=(frame_min<frames_min)?frame_min:frames_min;
                    frames_max=(frame_max>frames_max)?frame_max:frames_max;
                }
                if (frame<job.bleachAvgFrames) {
                    for (register uint16 i=0; i<frame_width*frame_height; i++) {
                        firstFrames[i]=firstFrames[i]+(float)frame_data[i]/(float)job.bleachAvgFrames;
                    }
                } else if (frame>frames-job.bleachAvgFrames) {
                    for (register uint16 i=0; i<frame_width*frame_height; i++) {
                        lastFrames[i]=lastFrames[i]+(float)frame_data[i]/(float)job.bleachAvgFrames;
                    }
                }
            }

            if (frames<1000) {
                emit messageChanged(tr("reading frames (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(1000/frames);
            } else if (frame%(frames/1000)==0) {
                emit messageChanged(tr("reading frames (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(1);
            }
            frame++;
            if (was_canceled) break;
        } while (reader->nextFrame() && (m_status==1) && (frame<frames) && (!was_canceled));


        free(frame_data);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOW WE CALCULATE THE IMAGE BASELINE (ACCORDING TO THE USER SETTINGS). IT WILL BE APPLIED LATER!
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled && m_status==1) {
        emit messageChanged(tr("calculating background correction ..."));
        QApplication::processEvents();
        QApplication::processEvents();
        calcBackgroundCorrection();
        emit messageChanged(tr("calculating bleach correction ..."));
        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
            firstFrames[i]=firstFrames[i]-baseline-backgroundImage[i%(frame_width*frame_height)];
            lastFrames[i]=lastFrames[i]-baseline-backgroundImage[i%(frame_width*frame_height)];
        }
        if (job.bleach==BLEACH_EXP) {
            // for the exponential bleach correction, we need to do a bit more here, as we HAVE to extract
            // a subimage series for the fit, which is already background corrected!!!

            reader->reset();
            register uint32_t frame=0;
            bool OK=true;
            while (OK && (frame<first_frame)) {
                OK=reader->nextFrame();
                frame++;
            }
            frame=0;

            float* frame_data=(float*)malloc(frame_width*frame_height*sizeof(float));
            for (int i=0; i<NFitFrames; i++) {
                int iidx=trunc((double)i/(double)(NFitFrames-1)*(double)frames);
                if (iidx<0) iidx=0;
                if (iidx>=frames) iidx=frames-1;
                fit_t[i]=(double)iidx+(double)job.bleachAvgFrames/2.0;
                while (frame<iidx) {
                    reader->nextFrame();
                    frame++;
                }
                if (!reader->readFrameFloat(frame_data)) {
                    m_status=-1;
                }
                //qDebug()<<i<<"/"<<NFitFrames<<":    "<<iidx<<"/"<<frames;
                for (int idx=0; idx<frame_width*frame_height; idx++) {
                    fit_frames[i*frame_width*frame_height+idx]=frame_data[idx];
                }
                if (job.bleachAvgFrames>1) {
                    for (int f=1; f<job.bleachAvgFrames; f++) {
                        reader->nextFrame();
                        frame++;
                        if (!reader->readFrameFloat(frame_data)) { m_status=-1; }
                        for (int idx=0; idx<frame_width*frame_height; idx++) {
                            fit_frames[i*frame_width*frame_height+idx]+=frame_data[idx];
                        }
                    }
                    for (int idx=0; idx<frame_width*frame_height; idx++) {
                        fit_frames[i*frame_width*frame_height+idx]/=(float)job.bleachAvgFrames;
                    }
                }
                if (was_canceled || (m_status!=1)) break;
                if (i%(NFitFrames/100)==0) {
                    emit messageChanged(tr("calculating bleach correction ... frame %1/%2 ...").arg(i+1).arg(NFitFrames));
                }
            }
            free(frame_data);

            if (m_status!=1) {
                emit statusChanged(m_status);
                emit messageChanged(tr("error reading frame for bleach correction: %1").arg(reader->lastError()));
            }

        }
        if (m_status==1) calcBleachCorrection(fit_frames, fit_t, NFitFrames);
    }




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CALCULATE THE ACFs, CCFs AND DCCFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled && m_status==1) {
        emit messageChanged(tr("preparing correlations ..."));

        float* frame_data=(float*)malloc(frame_width*frame_height*sizeof(float));

        acf_N=0;
        if (job.acf) {
            prepare_ccfs(acfjk, acfjb, &acf, &acf_std, &acf_tau, acf_N, frame_width, frame_height, job.segments);
        }

        ccf_N=0;
        if (job.ccf) {
            prepare_ccfs(ccf1jk, ccf1jb, &ccf1, &ccf1_std, &ccf_tau, ccf_N, frame_width, frame_height, job.segments);
            prepare_ccfs(ccf2jk, ccf2jb, &ccf2, &ccf2_std, &ccf_tau, ccf_N, frame_width, frame_height, job.segments);
            prepare_ccfs(ccf3jk, ccf3jb, &ccf3, &ccf3_std, &ccf_tau, ccf_N, frame_width, frame_height, job.segments);
            prepare_ccfs(ccf4jk, ccf4jb, &ccf4, &ccf4_std, &ccf_tau, ccf_N, frame_width, frame_height, job.segments);

        }

        dccf_N=0;
        if (job.distanceCCF) {
            prepare_ccfs(dccfjk, dccfjb, &dccf, &dccf_std, &dccf_tau, dccf_N, dccfframe_width, dccfframe_height, job.segments);
        }

        reader->reset();
        register uint32_t frame=0;
        bool OK=true;
        while (OK && (frame<first_frame)) {
            OK=reader->nextFrame();
            frame++;
        }
        frame=0;

        uint16_t video_frame_num=0;

        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state;
        stat_state.sum=0;
        stat_state.sum2=0;
        stat_state.sframe_min=0;
        stat_state.sframe_max=0;
        stat_state.statFirst=true;
        stat_state.cnt=0;
        stat_state.video_frame=(float*)calloc(frame_width*frame_height, sizeof(float));
        for (int64_t i=0; i<frame_width*frame_height; i++)  {
            stat_state.video_frame[i]=0;
        }
        statistics_mean.clear();
        statistics_std.clear();
        statistics_min.clear();
        statistics_max.clear();
        statistics_time.clear();


        //bool videoFirst=true;

        uint32_t segment_frames=frames/job.segments;
        do {
            if (!reader->readFrameFloat(frame_data)) {
                m_status=-1; emit statusChanged(m_status);
                emit messageChanged(tr("error reading frame: %1").arg(reader->lastError()));
            } else {
                // APPLY BACKGROUND CORRECTION TO FRAME
                double avg=0;
                for (register uint16 i=0; i<frame_width*frame_height; i++) {
                    frame_data[i]=frame_data[i]-baseline-backgroundImage[i];
                    avg=avg+frame_data[i]/ ((double)(frame_width*frame_height));
                }

                if (job.bleach==BLEACH_EXP) {
                    for (register uint16 i=0; i<frame_width*frame_height; i++) {
                        if (bleachFitOK[i]!=0) {
                            frame_data[i]=frame_data[i]*firstFrames[i]/(bleachOffset[i]+bleachAmplitude[i]*exp(-1.0*(double)frame/bleachTime[i]));
                        }
                    }
                } else if (job.bleach==BLEACH_REMOVEAVG) {
                    for (register uint16 i=0; i<frame_width*frame_height; i++) {
                        frame_data[i]=frame_data[i]*firstFrames[i]/avg;
                    }
                }

                // CALCULATE STATISTICS AND VIDEO
                contribute_to_statistics(stat_state, frame_data, frame_width, frame_height, frame, frames, &average_frame, &sqrsum_frame, &video, video_frame_num, frames_min, frames_max, statistics_time, statistics_mean, statistics_std, statistics_min, statistics_max);




                // CALCULATE CORRELATIONS, we store the sum in acf and the sum of squares in acf_std
                if (job.acf && acf_N>0) {
                    contribute_to_correlations(acfjk, acfjb, frame_data, frame_width, frame_height, 0, 0, frame, segment_frames, acf_tau, acf, acf_std, acf_N);
                }
                if (job.ccf && ccf_N>0) {
                    contribute_to_correlations(ccf1jk, ccf1jb, frame_data, frame_width, frame_height, -1, 0, frame, segment_frames, ccf_tau, ccf1, ccf1_std, ccf_N);
                    contribute_to_correlations(ccf2jk, ccf2jb, frame_data, frame_width, frame_height, +1, 0, frame, segment_frames, ccf_tau, ccf2, ccf2_std, ccf_N);
                    contribute_to_correlations(ccf3jk, ccf3jb, frame_data, frame_width, frame_height, 0, -1, frame, segment_frames, ccf_tau, ccf3, ccf3_std, ccf_N);
                    contribute_to_correlations(ccf4jk, ccf4jb, frame_data, frame_width, frame_height, 0, +1, frame, segment_frames, ccf_tau, ccf4, ccf4_std, ccf_N);

                }
                if (job.distanceCCF && dccf_N>0) {
                    contribute_to_correlations(dccfjk, dccfjb, frame_data, frame_width, frame_height, job.DCCFDeltaX, job.DCCFDeltaY, frame, segment_frames, dccf_tau, dccf, dccf_std, dccf_N);
                }
            }

            if (frames<2000) {
                emit messageChanged(tr("calculating correlations (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(2000/frames);
            } else if (frame%(frames/2000)==0) {
                emit messageChanged(tr("calculating correlations (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(1);
            }
            frame++;
            if (was_canceled) break;
        } while (reader->nextFrame() && (m_status==1) && (frame<frames) && (!was_canceled));

        real_video_count=video_frame_num;

        // calculate avg + stddev from sum and square-sum, as calculated above
        if (job.acf && acf_N>0 && (m_status==1) && (!was_canceled) ) {
            emit messageChanged(tr("averaging ACF segments ..."));
            average_ccfs(&acf, &acf_std, acf_N, frame_width, frame_height, job.segments);
        }
        if (job.ccf && ccf_N>0 && (m_status==1) && (!was_canceled)) {
            emit messageChanged(tr("averaging CCF segments ..."));
            average_ccfs(&ccf1, &ccf1_std, ccf_N, frame_width, frame_height, job.segments);
            average_ccfs(&ccf2, &ccf2_std, ccf_N, frame_width, frame_height, job.segments);
            average_ccfs(&ccf3, &ccf3_std, ccf_N, frame_width, frame_height, job.segments);
            average_ccfs(&ccf4, &ccf4_std, ccf_N, frame_width, frame_height, job.segments);

        }
        if (job.distanceCCF && dccf_N>0 && (m_status==1) && (!was_canceled)) {
            emit messageChanged(tr("averaging distance CCF segments ..."));
            average_ccfs(&dccf, &dccf_std, dccf_N, dccfframe_width, dccfframe_height, job.segments);
        }
        free(frame_data);
        free(stat_state.video_frame);
    }

    qDeleteAll(acfjk);
    qDeleteAll(ccf1jk);
    qDeleteAll(ccf2jk);
    qDeleteAll(ccf3jk);
    qDeleteAll(ccf4jk);
    qDeleteAll(dccfjk);

    qDeleteAll(acfjb);
    qDeleteAll(ccf1jb);
    qDeleteAll(ccf2jb);
    qDeleteAll(ccf3jb);
    qDeleteAll(ccf4jb);
    qDeleteAll(dccfjb);


}



static double QFRDRImagingFCSCorrelationJobThread_fExpLin( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    return A-t/tau;

}


void QFRDRImagingFCSCorrelationJobThread::calcBleachCorrection(float* fit_frames, double* fit_t, int NFitFrames) {
    if (job.bleach==BLEACH_EXP) {
        if (fit_frames && fit_t && NFitFrames>2) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                lm_control_struct control=lm_control_double;
                control.maxcall=500;
                lm_status_struct status;

                double* fit_I=(double*)malloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    fit_I[jj]=log(fit_frames[jj*frame_width*frame_height+i]);
                }
                double par[2]={fit_I[0], fit_t[NFitFrames-2]/(fit_I[0]-fit_I[NFitFrames-2])};
                //qDebug()<<i<<": initA="<<par[0]<<" initTau="<<par[1];
                lmcurve_fit(2, par, NFitFrames, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpLin, &control, &status);
                //qDebug()<<i<<": A="<<par[0]<<" tau="<<par[1]<<"     norm="<<status.fnorm<<" feval="<<status.nfev<<" message="<<lm_shortmsg[status.info];

                bleachAmplitude[i]=exp(par[0]);
                bleachTime[i]=par[1];
                bleachOffset[i]=0;
                bleachFitOK[i]=1;
                double t1=fit_t[NFitFrames/2];
                double t2=fit_t[2*NFitFrames/3];
                if ( (!QFFloatIsOK(bleachAmplitude[i]))
                     || (!QFFloatIsOK(bleachOffset[i]))
                     || (!QFFloatIsOK(bleachTime[i]))
                     || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                     || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                     || (bleachTime[i]==0)
                     || (std::isinf(bleachTime[i]))
                     || (std::isinf(bleachAmplitude[i]))
                     || (bleachAmplitude[i]==0) ) {
                    bleachFitOK[i]=0;
                }
                free(fit_I);
                if (i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }
        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachOffset[i]=0;
                bleachTime[i]=1;
                bleachFitOK[i]=0;
            }
        }
    } else {
        for (uint32_t i=0; i<frame_width*frame_height; i++) {
            bleachAmplitude[i]=0;
            bleachOffset[i]=0;
            bleachTime[i]=1;
            bleachFitOK[i]=1;
        }
    }
}

void QFRDRImagingFCSCorrelationJobThread::calcBackgroundCorrection() {
    if (backgroundImage) free(backgroundImage);
    if (backgroundImageStd) free(backgroundImageStd);
    backgroundImage=(float*)malloc(frame_width*frame_height*sizeof(float));
    backgroundImageStd=(float*)malloc(frame_width*frame_height*sizeof(float));
    if (job.backgroundCorrection==0) {
        baseline=0;
    } else if (job.backgroundCorrection==1) {
        baseline=job.backgroundOffset;
        //frames_max=frames_max-baseline;
        //frames_min=frames_min-baseline;
    } else if (job.backgroundCorrection==2) {
        baseline=job.backgroundOffset+frames_min;
        //frames_max=frames_max-baseline;
        //frames_min=frames_min-baseline;
    } else if (job.backgroundCorrection==3) {
        baseline=job.backgroundOffset;
        //frames_max=frames_max-baseline;
        //frames_min=frames_min-baseline;
    }


    // reset background image to 0
    for (register uint64_t i=0; i<frame_width*frame_height; i++) {
        backgroundImage[i]=0;
        backgroundImageStd[i]=0;
    }


    QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state;
    stat_state.sum=0;
    stat_state.sum2=0;
    stat_state.sframe_min=0;
    stat_state.sframe_max=0;
    stat_state.cnt=0;
    stat_state.statFirst=true;
    stat_state.video_frame=NULL;
    backstatistics_mean.clear();
    backstatistics_std.clear();
    backstatistics_min.clear();
    backstatistics_max.clear();
    backstatistics_time.clear();
    uint16_t vidfnum=0;
    float bframes_min=0;
    float bframes_max=0;



    // if we should use a background file for correction, we read it and create an averaged frame from it.
    if (job.backgroundCorrection==3) {
        //qDebug()<<job.filenameBackground<<QFile::exists(job.filenameBackground);
        if (QFile::exists(job.filenameBackground)) {
            QFImporterImageSeries* reader=NULL;
            bool OK=false;
            if (job.fileFormat>=0 && job.fileFormat<getImageReaderCount(pluginservices)) {
                reader=getImageReader(job.fileFormat, pluginservices);
            }
            if (reader) {
                emit messageChanged(tr("opening %1 background file ...").arg(reader->formatName()));
                OK=reader->open(job.filenameBackground);
                if (!OK) {
                    m_status=-1; emit statusChanged(m_status);
                    if (reader) messageChanged(tr("error opening background file '%1': %2").arg(job.filename).arg(reader->lastError()));
                    else emit messageChanged(tr("error opening background file '%1'").arg(job.filename));
                } else {
                    reader->setBinning(job.binning);
                    reader->setInterleavedBinning(job.interleaved_binning);
                    reader->setAverageBinning(job.binAverage);
                    if (job.use_cropping) {
                        reader->setCropping(job.crop_x0, job.crop_x1, job.crop_y0, job.crop_y1);
                    } else {
                        reader->unsetCropping();
                    }
                    emit messageChanged(tr("reading frames in background file ..."));
                    uint16_t bframe_width=reader->frameWidth();
                    uint16_t bframe_height=reader->frameHeight();
                    uint32_t backFrames=reader->countFrames();

                    if ((bframe_width==frame_width)&&(bframe_height==frame_height))  {

                        float* frame_data=(float*)malloc(frame_width*frame_height*sizeof(float));
                        uint64_t frames=0;
                        do {
                            if (!reader->readFrameFloat(frame_data)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("error reading background frame: %1").arg(reader->lastError()));
                            } else {
                                for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                                    backgroundImage[i]=backgroundImage[i]+frame_data[i];
                                    backgroundImageStd[i]=backgroundImageStd[i]+frame_data[i]*frame_data[i];
                                }

                                contribute_to_statistics(stat_state, frame_data, frame_width, frame_height, frames, backFrames, NULL, NULL, NULL, vidfnum, bframes_min, bframes_max, backstatistics_time, backstatistics_mean, backstatistics_std, backstatistics_min, backstatistics_max, true);

                                if (frames%1000==0) {
                                    emit messageChanged(tr("reading frames in background file (%1)...").arg(frames));
                                }

                                frames++;
                            }
                        } while (reader->nextFrame() && (m_status==1) && (!was_canceled));

                        for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                            const double s2sum=backgroundImageStd[i];
                            const double ssum=backgroundImage[i];
                            const double NN=frames;
                            backgroundImageStd[i]=sqrt((s2sum-ssum*ssum/NN)/(NN-1.0));
                            backgroundImage[i]=ssum/(float)frames;
                        }

                        free(frame_data);
                    } else {
                        m_status=-1; emit statusChanged(m_status);
                        emit messageChanged(tr("background and data files have different framesize (data: %1x%2,  background:%3x%4)").arg(frame_width).arg(frame_height).arg(bframe_width).arg(bframe_height));
                    }

                    reader->close();
                }
                delete reader;
                reader=NULL;
            } else {
                m_status=-1; emit statusChanged(m_status);
                emit messageChanged(tr("could not create image reader object"));
            }
        }
    }

    emit progressIncrement(100);

}

