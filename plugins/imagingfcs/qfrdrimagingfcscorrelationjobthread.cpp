#include "qfrdrimagingfcscorrelationjobthread.h"
#include "qfrdrimagereadertiff.h"
#include "multitau-correlator.h"
#include "correlator_multitau.h"
#include "libtiff_tools.h"
#include "image_tools.h"
#include "statistics_tools.h"
#include <stdint.h>

QFRDRImagingFCSCorrelationJobThread::QFRDRImagingFCSCorrelationJobThread(QObject *parent) :
    QThread(parent)
{
    outLocale=QLocale::c();
    outLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    m_status=0;
    was_canceled=false;
}

QStringList QFRDRImagingFCSCorrelationJobThread::getAddFiles() const {
    return addFiles;
}
Job QFRDRImagingFCSCorrelationJobThread::getJob() const {
    return job;
}

QStringList QFRDRImagingFCSCorrelationJobThread::getImageFilterList()  {
    QList<QFRDRImageReader*> list=getImageReaderList();
    QStringList l;
    for (int i=0; i<list.size(); i++) {
        l.append(list[i]->filter());
    }
    return l;
}

 QStringList QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList()  {
    QList<QFRDRImageReader*> list=getImageReaderList();
    QStringList l;
    for (int i=0; i<list.size(); i++) {
        l.append(list[i]->formatName());
    }
    return l;
}

 QList<QFRDRImageReader*> QFRDRImagingFCSCorrelationJobThread::getImageReaderList()  {
    static QList<QFRDRImageReader*> list;
    if (list.size()<=0) {
        list.clear();
        //////////////////////////////////////////////////////////////////////////////////////////
        // INSERT ADDITIONAL IMAGE FILTERS HERE!
        //////////////////////////////////////////////////////////////////////////////////////////
        list.append(new QFRDRImageReaderTIFF());
    }
    return list;
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

void QFRDRImagingFCSCorrelationJobThread::init(Job job) {
    this->job=job;
    m_status=0;
    was_canceled=false;
    addFiles.clear();
}

void QFRDRImagingFCSCorrelationJobThread::run() {
    emit messageChanged(tr("loading data ..."));
    if ((job.fileFormat<0)||(job.fileFormat>1)) {
        m_status=-1;
        emit statusChanged(m_status);
        emit messageChanged(tr("file format not supported or given ..."));
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
        emit rangeChanged(0,2200);


        reader=NULL;
        bool OK=false;
        if (job.fileFormat>=0 && job.fileFormat<getImageReaderList().size()) {
            reader=getImageReaderList().at(job.fileFormat);
        }
        if (reader) {
            emit messageChanged(tr("opening %1 file ...").arg(reader->formatName()));
            emit progressIncrement(10);
            OK=reader->open(job.filename);
            if (!OK) {
                m_status=-1; emit statusChanged(m_status);
                if (reader) emit messageChanged(reader->lastError());
                else emit messageChanged(tr("error opening file '%1'").arg(job.filename));
            } else {
                emit progressIncrement(10);
                emit messageChanged(tr("counting frames ..."));
                emit progressIncrement(10);
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

                    baseline=job.backgroundOffset;

                    acf_tau=NULL;
                    acf=NULL;
                    acf_std=NULL;
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
                    ccf_N=0;

                    video_count=floor(frames/job.video_frames);
                    real_video_count=video_count;
                    video=(float*)calloc(frame_width*frame_height*video_count, sizeof(float));




                    if (job.correlator>0) {
                        correlate_loadsingle();
                    } else {
                        correlate_loadall();
                    }

                    if ((m_status==1) && !was_canceled && average_frame) {
                        emit messageChanged(tr("saving overview image ..."));
                        QDir d=QFileInfo(job.filename).dir();
                        QString localFilename=QFileInfo(d.absoluteFilePath(job.prefix+QFileInfo(job.filename).completeBaseName()+".overview.tif")).absoluteFilePath();
                        QString localFileDirectory=QFileInfo(d.absoluteFilePath(localFilename)).dir().absolutePath();
                        //qDebug()<<d<<d.absolutePath()<<localFileDirectory;
                        if (d.mkpath(localFileDirectory)) {
                            TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
                            if (tif) {
                                float avgMin=average_frame[0];
                                float avgMax=average_frame[0];
                                for (uint32_t i=0; i<frame_width*frame_height; i++) {
                                    avgMin=(average_frame[i]<avgMin)?average_frame[i]:avgMin;
                                    avgMax=(average_frame[i]>avgMax)?average_frame[i]:avgMax;
                                }
                                uint8_t* img=(uint8_t*)malloc(frame_width*frame_height*sizeof(uint8_t));
                                for (uint32_t i=0; i<frame_width*frame_height; i++) {
                                    img[i]=(uint8_t)round((average_frame[i]-avgMin)*255.0/fabs(avgMax-avgMin));
                                }
                                TIFFTWriteUint8(tif, img, frame_width, frame_height);
                                free(img);
                                TIFFClose(tif);
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create overview image '%1'!").arg(localFilename));
                            }
                        } else {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("could not create overview image subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                        }
                    }
                    emit progressIncrement(10);

                    if ((m_status==1) && !was_canceled && video && real_video_count>0 ) {
                        emit messageChanged(tr("saving video ..."));
                        QDir d=QFileInfo(job.filename).dir();
                        QString localFilename=QFileInfo(d.absoluteFilePath(job.prefix+QFileInfo(job.filename).completeBaseName()+".video.tif")).absoluteFilePath();
                        QString localFileDirectory=QFileInfo(d.absoluteFilePath(localFilename)).dir().absolutePath();
                        if (d.mkpath(localFileDirectory)) {
                            TIFF* tif = TIFFOpen(localFilename.toAscii().data(),"w");
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
                                    TIFFTWriteUint16(tif, img, frame_width, frame_height);
                                    TIFFWriteDirectory(tif);
                                    if (c%5==0) emit messageChanged(tr("saving video %1/%2...").arg(c+1).arg(video_count));
                                }
                                free(img);
                                TIFFClose(tif);
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create video '%1'!").arg(localFilename));
                            }
                        } else {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("could not create video subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                        }
                    }
                    emit progressIncrement(50);

                    if ((m_status==1) && !was_canceled && job.statistics) {
                        emit messageChanged(tr("saving statistics ..."));
                        QDir d=QFileInfo(job.filename).dir();
                        QString localFilename=QFileInfo(d.absoluteFilePath(job.prefix+QFileInfo(job.filename).completeBaseName()+".statistics.dat")).absoluteFilePath();
                        QString localFileDirectory=QFileInfo(d.absoluteFilePath(localFilename)).dir().absolutePath();
                        if (d.mkpath(localFileDirectory)) {
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving statistics ..."));
                                int count=statistics_time.size();//qMin(statistics_time.size(), statistics_mean.size());
                                /*count=qMin(count, statistics_std.size());
                                count=qMin(count, statistics_min.size());
                                count=qMin(count, statistics_max.size());*/
                                for (int i=0; i<count; i++) {
                                    text<<statistics_time[i]<<", "<<statistics_mean[i]-baseline<<", "<<statistics_std[i]<<", "<<statistics_min[i]-baseline<<", "<<statistics_max[i]-baseline<<"\n";
                                }
                                f.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create statistics file '%1'!").arg(localFilename));
                            }
                        } else {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("could not create statistics subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                        }
                    }
                    emit progressIncrement(10);

                    if ((m_status==1) && !was_canceled && job.acf && acf && acf_tau && acf_N>0) {
                        QDir d(QFileInfo(job.filename).absoluteDir());
                        QString localFilename=QFileInfo(d.absoluteFilePath(job.prefix+QFileInfo(job.filename).completeBaseName()+".autocorrelation.dat")).absoluteFilePath();
                        QString localFileDirectory=QFileInfo(d.absoluteFilePath(localFilename)).dir().absolutePath();
                        if (d.mkpath(localFileDirectory)) {
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving autocorrelation ..."));

                                for (register uint32_t p=0; p<frame_width*frame_height; p++) {
                                    for (register uint32_t i=0; i<acf_N; i++) {
                                        if (acf_tau[i]<input_length)  {
                                            text<<acf_tau[i]<<", "<<acf[p*acf_N+i];
                                            if (acf_std) text<<", "<<acf_std[p*acf_N+i];
                                            text<<"\n";
                                        } else break;
                                    }
                                    text<<"\n\n";
                                }
                                addFiles.append(localFilename);
                                f.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create autocorrelation file '%1'!").arg(localFilename));
                            }
                        } else {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("could not create autocorrelation subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                        }
                    }
                    emit progressIncrement(10);


                    if ((m_status==1) && !was_canceled && job.ccf && ccf_tau && ccf1 && ccf2 && ccf3 && ccf4 && ccf_N>0) {
                        QDir d(QFileInfo(job.filename).absoluteDir());
                        QString localFilename=QFileInfo(d.absoluteFilePath(job.prefix+QFileInfo(job.filename).completeBaseName()+".crosscorrelation.dat")).absoluteFilePath();
                        QString localFileDirectory=QFileInfo(d.absoluteFilePath(localFilename)).dir().absolutePath();
                        if (d.mkpath(localFileDirectory)) {
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving crosscorrelation ..."));

                                for (register uint32_t p=0; p<frame_width*frame_height; p++) {
                                    for (register uint32_t i=0; i<ccf_N; i++) {
                                        if (ccf_tau[i]<input_length)  {
                                            text<<ccf_tau[i]<<", "<<ccf1[p*acf_N+i]<<", "<<ccf2[p*acf_N+i]<<", "<<ccf3[p*acf_N+i]<<", "<<ccf4[p*acf_N+i];
                                            if (ccf1_std && ccf2_std && ccf3_std && ccf4_std) text<<", "<<ccf1_std[p*acf_N+i]<<", "<<ccf2_std[p*acf_N+i]<<", "<<ccf3_std[p*acf_N+i]<<", "<<ccf4_std[p*acf_N+i];
                                            text<<"\n";
                                        } else break;
                                    }
                                    text<<"\n\n";
                                }
                                addFiles.append(localFilename);
                                f.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create crosscorrelation file '%1'!").arg(localFilename));
                            }
                        } else {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("could not create crosscorrelation subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                        }
                    }
                    emit progressIncrement(10);

                    if ((m_status==1) && !was_canceled) {
                        emit messageChanged(tr("saving settings ..."));
                        QDir d=QFileInfo(job.filename).dir();
                        QString localFilename=QFileInfo(d.absoluteFilePath(job.prefix+QFileInfo(job.filename).completeBaseName()+".evalsettings.txt")).absoluteFilePath();
                        QString localFileDirectory=QFileInfo(d.absoluteFilePath(localFilename)).dir().absolutePath();
                        if (d.mkpath(localFileDirectory)) {
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);

                                text<<"date/time                   : "<<QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss") << "\n";
                                text<<"width                       : "<<frame_width << "\n";
                                text<<"height                      : "<<frame_height << "\n";
                                text<<"frame count in file         : "<<frame_count << "\n";
                                text<<"frame count                 : "<<frames << "\n";
                                text<<"first frame                 : "<<first_frame << "\n";
                                text<<"last frame                  : "<<first_frame+frames-1 << "\n";
                                text<<"correlator S                : "<<job.S << "\n";
                                text<<"correlator m                : "<<job.m << "\n";
                                text<<"correlator P                : "<<job.P << "\n";
                                text<<"correlator type             : "<<job.correlator << "\n";
                                text<<"smallest tau [s]            : "<<job.frameTime << "\n";
                                text<<"baseline                    : "<<baseline << "\n";
                                if (video && job.video) {
                                    text<<"video sum up                : "<<job.video_frames << "\n";
                                    text<<"video frames                : "<<real_video_count << "\n";
                                }
                                if (job.statistics) {
                                    text<<"statistics over             : "<<job.statistics_frames << "\n";
                                }

                                f.close();
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create settings file '%1'!").arg(localFilename));
                            }
                        } else {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("could not create settings subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                        }
                    }
                    emit progressIncrement(10);

                    if (video) free(video);
                    if (average_frame) free(average_frame);
                    if (acf_tau) free(acf_tau);
                    if (acf) free(acf);
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
                    if (m_status==1) {
                        if (was_canceled) {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("canceled by user"));
                        } else {
                            m_status=2; emit statusChanged(m_status);
                            emit messageChanged(tr("correlation ... done "));
                        }
                        emit statusChanged(m_status);
                    }
                } else {
                    m_status=-1; emit statusChanged(m_status);
                    emit messageChanged(tr("no frames in file/selected"));
                }

                reader->close();
            }
        } else {
            m_status=-1; emit statusChanged(m_status);
            emit messageChanged(tr("could not create image reader object"));
        }
    }

    /*if (m_status==2) {
        job.processingOK=true;
        job.filesToAdd=addFiles;
    } else {
        job.processingOK=false;
        job.filesToAdd.clear();
    }*/
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOW WE READ ALL FRAMES IN THE TIFF FILE INTO THE flot* ARRAY ALLOCATED BEFORE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled) {
        emit messageChanged(tr("reading frames ..."));
        register uint32_t frame=0;
        float sum=0;
        float sum2=0;
        float sframe_min=0;
        float sframe_max=0;
        uint16_t* frame_data=(uint16_t*)malloc(frame_width*frame_height*sizeof(uint16_t));
        float* video_frame=(float*)calloc(frame_width*frame_height, sizeof(float));
        uint16_t video_frame_num=0;
        bool OK=true;
        while (OK && (frame<first_frame)) {
            OK=reader->nextFrame();
            frame++;
        }
        frame=0;
        statistics_mean.clear();
        statistics_std.clear();
        statistics_min.clear();
        statistics_max.clear();
        statistics_time.clear();
        do {
            if (!reader->readFrameUINT16(frame_data)) {
                m_status=-1; emit statusChanged(m_status);
                emit messageChanged(tr("error reading frame: %1").arg(reader->lastError()));
            } else {
                float frame_min=frame_data[0];
                float frame_max=frame_data[0];
                for (register uint16 i=0; i<frame_width*frame_height; i++) {
                    register uint16_t v=frame_data[i];
                    image_series[frame*frame_width*frame_height+i] = v;
                    frame_min=(v<frame_min)?v:frame_min;
                    frame_max=(v>frame_max)?v:frame_max;
                    average_frame[i]=average_frame[i]+(float)v/(float)frames;
                    sum+=v;
                    sum2+=(v*v);
                    video_frame[i]=video_frame[i]+(float)v/(float)job.video_frames;
                }
                if (frame==0) {
                    frames_min=frame_min;
                    frames_max=frame_max;
                } else {
                    frames_min=(frame_min<frames_min)?frame_min:frames_min;
                    frames_max=(frame_max>frames_max)?frame_max:frames_max;
                }
                if (frame%job.statistics_frames==0) {
                    sframe_min=frame_min;
                    sframe_max=frame_max;
                } else {
                    sframe_min=(frame_min<sframe_min)?frame_min:sframe_min;
                    sframe_max=(frame_max>sframe_max)?frame_max:sframe_max;
                }
                if (job.statistics && (frame%job.statistics_frames==0) && (frame>0)) {
                    float N=frame_width*frame_height*job.statistics_frames;
                    statistics_time.append((float)frame*job.frameTime);
                    statistics_mean.append(sum/N);
                    statistics_min.append(sframe_min);
                    statistics_max.append(sframe_max);
                    if (job.statistics_frames>1) statistics_std.append(sqrt((sum2-sum*sum/N)/(N-1.0)));
                    sum=0;
                    sum2=0;
                    sframe_min=0;
                    sframe_max=0;
                }
                if (job.video && (frame%job.video_frames==0) && (frame>0) && video){
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        video[video_frame_num*frame_width*frame_height+i]=video_frame[i];
                        video_frame[i]=0;
                    }
                    video_frame_num++;
                    //qDebug()<<video_frame_num;
                }
            }
            real_video_count=video_frame_num;

            if (frame%(frames/1000)==0) {
                emit messageChanged(tr("reading frames (%1/%2)...").arg(frame).arg(frames));
                emit progressIncrement(1);
            }
            frame++;
            if (was_canceled) break;
        } while (reader->nextFrame() && (m_status==1) && (frame<frames));


        free(frame_data);
        free(video_frame);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOW WE CORRECT THE IMAGE FOR IT'S BASELINE (ACCORDING TO THE USER SETTINGS)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled) calcBackgroundCorrection();
    emit messageChanged(tr("applying baseline correction..."));
    for (uint32_t i=0; i<frame_width*frame_height*frames; i++) {
        image_series[i]=image_series[i]-baseline;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CALCULATE THE ACFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (job.acf && job.correlator==0) {
        emit messageChanged(tr("calculating autocorrelations ..."));
        acf_N=job.S*job.P;
        acf=(double*)calloc(acf_N*frame_width*frame_height,sizeof(double));
        acf_tau=(double*)calloc(acf_N,sizeof(double));
        long* acf_t=(long*)calloc(acf_N*frame_width*frame_height,sizeof(long));
        statisticsAutocorrelateCreateMultiTau(acf_t, job.S, job.m, job.P);
        for (uint32_t p=0; p<frame_width*frame_height; p++) {
            statisticsAutocorrelateMultiTauSymmetric(&(acf[p*acf_N]), &(image_series[p]), frames, acf_t, acf_N, frame_width*frame_height);
            emit messageChanged(tr("calculating autocorrelations %1/%2 ...").arg(p+1).arg(frame_width*frame_height));
            if (frame_width*frame_height<500) emit progressIncrement(ceil(500/(frame_width*frame_height)));
            else if (p%(frame_width*frame_height/500)==0) emit progressIncrement(1);
        }
        for (uint32_t i=0; i<acf_N; i++) {
            acf_tau[i]=(double)acf_t[i]*job.frameTime;
        }
        free(acf_t);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CALCULATE THE CCFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (job.ccf && job.correlator==0) {
        emit messageChanged(tr("calculating crosscorrelations ..."));
        ccf_N=job.S*job.P;
        ccf1=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
        ccf2=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
        ccf3=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
        ccf4=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
        ccf_tau=(double*)calloc(ccf_N,sizeof(double));
        long* ccf_t=(long*)calloc(ccf_N*frame_width*frame_height,sizeof(long));
        statisticsAutocorrelateCreateMultiTau(ccf_t, job.S, job.m, job.P);
        for (uint32_t p=0; p<frame_width*frame_height; p++) {
            if ((int32_t)p-1>=0) statisticsCrosscorrelateMultiTauSymmetric(&(ccf1[p*acf_N]), &(image_series[p-1]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
            if ((int32_t)p+1<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(ccf2[p*acf_N]), &(image_series[p+1]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
            if ((int32_t)p-(int32_t)frame_width>=0) statisticsCrosscorrelateMultiTauSymmetric(&(ccf3[p*acf_N]), &(image_series[p-frame_width]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
            if ((int32_t)p+(int32_t)frame_width<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(ccf4[p*acf_N]), &(image_series[p+frame_width]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);

            emit messageChanged(tr("calculating crosscorrelations %1/%2 ...").arg(p+1).arg(frame_width*frame_height));
            if (frame_width*frame_height<500) emit progressIncrement(ceil(500/(frame_width*frame_height)));
            else if (p%(frame_width*frame_height/500)==0) emit progressIncrement(1);
        }
        for (uint32_t i=0; i<acf_N; i++) {
            ccf_tau[i]=(double)ccf_t[i]*job.frameTime;
        }
        free(ccf_t);
    }


    if (image_series) free(image_series);
    image_series=NULL;
}

void QFRDRImagingFCSCorrelationJobThread::correlate_loadsingle() {

}


void QFRDRImagingFCSCorrelationJobThread::calcBackgroundCorrection() {
    if (job.backgroundCorrection==0) {
        baseline=0;
    } else if (job.backgroundCorrection==1) {
        baseline=job.backgroundOffset;
        frames_max=frames_max-baseline;
        frames_min=frames_min-baseline;
    } else if (job.backgroundCorrection==2) {
        baseline=baseline+frames_min;
        frames_max=frames_max-baseline;
        frames_min=frames_min-baseline;
    }
}



