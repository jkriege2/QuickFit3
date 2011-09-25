#include "qfrdrimagingfcscorrelationjobthread.h"
#include "qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagereadertiff.h"
//#include "qfrdrimagereaderrh.h"
#include "multitau-correlator.h"
#include "correlator_multitau.h"
#include "libtiff_tools.h"
#include "image_tools.h"
#include "statistics_tools.h"
#include <stdint.h>
#include "tinytiffwriter.h"

QFRDRImagingFCSCorrelationJobThread::QFRDRImagingFCSCorrelationJobThread(QObject *parent) :
    QThread(parent)
{
    outLocale=QLocale::c();
    outLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    m_status=0;
    was_canceled=false;
    duration=0;
    backgroundImage=NULL;
}

QFRDRImagingFCSCorrelationJobThread::~QFRDRImagingFCSCorrelationJobThread() {
    if (backgroundImage) free(backgroundImage);
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
Job QFRDRImagingFCSCorrelationJobThread::getJob() const {
    return job;
}

QStringList QFRDRImagingFCSCorrelationJobThread::getImageFilterList()  {
    QStringList l;
    int i=0;
    QFRDRImageReader* r=NULL;
    while ((r=getImageReader(i))!=NULL) {
        l.append(r->filter());
        delete r;
        i++;
    }
    return l;
}

QStringList QFRDRImagingFCSCorrelationJobThread::getImageFormatNameList()  {
     QStringList l;
     int i=0;
     QFRDRImageReader* r=NULL;
     while ((r=getImageReader(i))!=NULL) {
         l.append(r->formatName());
         delete r;
         i++;
     }
     return l;
}

QFRDRImageReader* QFRDRImagingFCSCorrelationJobThread::getImageReader(int idx)  {
    QFRDRImageReader* r=NULL;
    //////////////////////////////////////////////////////////////////////////////////////////
    // INSERT ADDITIONAL IMAGE FILTERS HERE!
    //////////////////////////////////////////////////////////////////////////////////////////
    if (idx==0) r=new QFRDRImageReaderTIFF();
    //if (idx==1) r=new QFRDRImageReaderRH();

    return r;
}

int QFRDRImagingFCSCorrelationJobThread::getImageReaderCount() {
    int i=0;
    QFRDRImageReader* r=NULL;
    while ((r=getImageReader(i))!=NULL) {
        delete r;
        i++;
    }
    return i;
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
    result=result.replace("%first%", QString::number(job.range_min), Qt::CaseInsensitive);
    result=result.replace("%last%", QString::number(job.range_max), Qt::CaseInsensitive);
    result=result.replace("%backoffset%", QString::number(job.backgroundOffset), Qt::CaseInsensitive);
    result=result.replace("%segments%", QString::number(job.segments), Qt::CaseInsensitive);
    result=result.replace("%backcorrectionid%", QString::number(job.backgroundCorrection), Qt::CaseInsensitive);
    result=result.replace("%correlatorid%", QString::number(job.correlator), Qt::CaseInsensitive);

    QString back="unknown";
    if (job.backgroundCorrection==0) back="none";
    if (job.backgroundCorrection==1) back="offset";
    if (job.backgroundCorrection==2) back="offsetmin";
    QString corr="unknown";
    if (job.correlator==0) corr="direct";
    if (job.correlator==1) corr="mtauallmon";
    if (job.correlator==2) corr="mtauonemon";

    result=result.replace("%backcorrection%", back, Qt::CaseInsensitive);
    result=result.replace("%correlator%", corr, Qt::CaseInsensitive);
    return result;
}

void QFRDRImagingFCSCorrelationJobThread::run() {
    QTime ptime;

    ptime.start();

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
        emit rangeChanged(0,2630);


        reader=NULL;
        bool OK=false;
        if (job.fileFormat>=0 && job.fileFormat<getImageReaderCount()) {
            reader=getImageReader(job.fileFormat);
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


                    ////////////////////////////////////////////////////////////////////////////////////////////
                    // CREATE FILENAMES FOR RESULTS AND MAKE SURE THE DIRECTORY FOR THE FILES EXISTS (mkpath() )
                    ////////////////////////////////////////////////////////////////////////////////////////////
                    QDir d=QFileInfo(job.filename).dir();

                    int counter=-1;
                    do {
                        outputFilenameBase=d.absoluteFilePath(replacePostfixSpecials(job.prefix)+QFileInfo(job.filename).completeBaseName()+replacePostfixSpecials(job.postfix, counter));
                        counter++;
                    } while (QFile::exists(outputFilenameBase+".evalsettings.txt"));
                    //qDebug()<<outputFilenameBase;
                    QString configFilename=outputFilenameBase+".evalsettings.txt";
                    QString averageFilename="";
                    QString videoFilename="";
                    QString statisticsFilename="";
                    QString acfFilename="";
                    QString ccfFilename="";
                    QString localFileDirectory=QFileInfo(d.absoluteFilePath(configFilename)).dir().absolutePath();
                    if (d.mkpath(localFileDirectory)) {



                        ////////////////////////////////////////////////////////////////////////////////////////////
                        // RUN THE CORRELATION
                        ////////////////////////////////////////////////////////////////////////////////////////////
                        if (job.correlator>0) {
                            correlate_loadsingle();
                        } else {
                            correlate_loadall();
                        }





                        ////////////////////////////////////////////////////////////////////////////////////////////
                        // SAVE THE RESULTS
                        ////////////////////////////////////////////////////////////////////////////////////////////
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

                        }
                        emit progressIncrement(10);

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
                                    //TIFFTWriteUint16(tif, img, frame_width, frame_height);
                                    //TIFFWriteDirectory(tif);
                                    TinyTIFFWriter_writeImage(tif, img);
                                    if (c%5==0) {
                                        emit messageChanged(tr("saving video %1/%2...").arg(c+1).arg(video_count));
                                        if (real_video_count<500) emit progressIncrement(5*500/real_video_count);
                                        else if (c%(real_video_count/500)==0) emit progressIncrement(5);
                                    }
                                }
                                free(img);
                                //TIFFClose(tif);
                                TinyTIFFWriter_close(tif);
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create video '%1'!").arg(localFilename));
                            }

                        }
                        emit progressIncrement(50);

                        if ((m_status==1) && !was_canceled && job.statistics) {
                            emit messageChanged(tr("saving statistics ..."));
                            QString localFilename=statisticsFilename=outputFilenameBase+".statistics.dat";
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
                                emit messageChanged(tr("could not create statistics file '%1': %2!").arg(localFilename).arg(f.errorString()));
                            }

                        }
                        emit progressIncrement(10);

                        if ((m_status==1) && !was_canceled && job.acf && acf && acf_tau && acf_N>0) {
                            QString localFilename=acfFilename=outputFilenameBase+".autocorrelation.dat";
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving autocorrelation ..."));

                                for (register uint32_t p=0; p<frame_width*frame_height; p++) {
                                    for (register uint32_t i=0; i<acf_N; i++) {
                                        if (acf_tau[i]<input_length)  {
                                            text<<acf_tau[i]<<", "<<acf[p*acf_N+i]-1.0;
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
                                emit messageChanged(tr("could not create autocorrelation file '%1': %2!").arg(localFilename).arg(f.errorString()));
                            }

                        }
                        emit progressIncrement(10);


                        if ((m_status==1) && !was_canceled && job.ccf && ccf_tau && ccf1 && ccf2 && ccf3 && ccf4 && ccf_N>0) {
                            QString localFilename=ccfFilename=outputFilenameBase+".crosscorrelation.dat";
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);
                                emit messageChanged(tr("saving crosscorrelation ..."));

                                for (register uint32_t p=0; p<frame_width*frame_height; p++) {
                                    for (register uint32_t i=0; i<ccf_N; i++) {
                                        if (ccf_tau[i]<input_length)  {
                                            text<<ccf_tau[i]<<", "<<ccf1[p*acf_N+i]-1.0<<", "<<ccf2[p*acf_N+i]-1.0<<", "<<ccf3[p*acf_N+i]-1.0<<", "<<ccf4[p*acf_N+i]-1.0;
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
                                emit messageChanged(tr("could not create crosscorrelation file '%1': %2!").arg(localFilename).arg(f.errorString()));
                            }

                        }
                        emit progressIncrement(10);

                        if ((m_status==1) && !was_canceled) {
                            emit messageChanged(tr("saving settings ..."));
                            QString& localFilename=configFilename;
                            QFile f(localFilename);
                            if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
                                QTextStream text(&f);
                                text.setLocale(outLocale);

                                text<<"date/time                   : "<<QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss") << "\n";
                                text<<"input file                  : "<<job.filename << "\n";
                                if (!job.descriptionFilename.isEmpty()) text<<"input description file      : "<<job.descriptionFilename << "\n";
                                if (!averageFilename.isEmpty())         text<<"overview image file         : " << averageFilename << "\n";
                                if (!videoFilename.isEmpty())           text<<"video file                  : " << videoFilename << "\n";
                                if (!statisticsFilename.isEmpty())      text<<"statistics file             : " << statisticsFilename << "\n";
                                if (!acfFilename.isEmpty())             text<<"autocorrelation file        : " << acfFilename << "\n";
                                if (!ccfFilename.isEmpty())             text<<"crosscorrelation file       : " << ccfFilename << "\n";
                                text<<"width                       : "<<outLocale.toString(frame_width) << "\n";
                                text<<"height                      : "<<outLocale.toString(frame_height) << "\n";
                                text<<"frame count in file         : "<<outLocale.toString(frame_count) << "\n";
                                text<<"frame count                 : "<<outLocale.toString(frames) << "\n";
                                text<<"first frame                 : "<<outLocale.toString(first_frame) << "\n";
                                text<<"last frame                  : "<<outLocale.toString(first_frame+frames-1) << "\n";
                                text<<"correlator S                : "<<outLocale.toString(job.S) << "\n";
                                text<<"correlator m                : "<<outLocale.toString(job.m) << "\n";
                                text<<"correlator P                : "<<outLocale.toString(job.P) << "\n";
                                text<<"correlator type             : "<<outLocale.toString(job.correlator) << "\n";
                                text<<"smallest tau [s]            : "<<outLocale.toString(job.frameTime) << "\n";
                                text<<"baseline                    : "<<outLocale.toString(baseline) << "\n";
                                if (video && job.video) {
                                    text<<"video sum up                : "<<outLocale.toString(job.video_frames) << "\n";
                                    text<<"video frames                : "<<outLocale.toString(real_video_count) << "\n";
                                }
                                if (job.statistics) {
                                    text<<"statistics over             : "<<outLocale.toString(job.statistics_frames) << "\n";
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
                        m_status=-1; emit statusChanged(m_status);
                        emit messageChanged(tr("could not create output subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                    }

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

    /*if (m_status==2) {
        job.processingOK=true;
        job.filesToAdd=addFiles;
    } else {
        job.processingOK=false;
        job.filesToAdd.clear();
    }*/

    duration=ptime.elapsed();
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
        image_series[i]=image_series[i]-baseline-backgroundImage[i%(frame_width*frame_height)];
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CALCULATE THE ACFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (job.acf && job.correlator==0) {
        emit messageChanged(tr("calculating autocorrelations ..."));
        acf_N=job.S*job.P;
        acf=(double*)calloc(acf_N*frame_width*frame_height,sizeof(double));
        for (uint32_t nn=0; nn<acf_N*frame_width*frame_height; nn++) {
            acf[nn]=1.0;
        }
        if (job.segments>1) {
            acf_std=(double*)calloc(acf_N*frame_width*frame_height,sizeof(double));
        }
        acf_tau=(double*)calloc(acf_N,sizeof(double));
        long* acf_t=(long*)calloc(acf_N*frame_width*frame_height,sizeof(long));
        statisticsAutocorrelateCreateMultiTau(acf_t, job.S, job.m, job.P);
        for (uint32_t p=0; p<frame_width*frame_height; p++) {
            if (job.segments<=1) {
                statisticsAutocorrelateMultiTauSymmetric(&(acf[p*acf_N]), &(image_series[p]), frames, acf_t, acf_N, frame_width*frame_height);
            } else {
                uint32_t segment_frames=frames/job.segments;
                double* cftemp=(double*)calloc(acf_N,sizeof(double));
                double* sum=(double*)calloc(acf_N,sizeof(double));
                double* sum2=(double*)calloc(acf_N,sizeof(double));
                for (register uint32_t ct=0; ct<acf_N; ct++) {
                    sum[ct]=0;
                    sum2[ct]=0;
                }
                for (int32_t seg=0; seg<job.segments; seg++) {
                    for (register uint32_t ct=0; ct<acf_N; ct++) cftemp[ct]=0;
                    statisticsAutocorrelateMultiTauSymmetric(cftemp, &(image_series[seg*segment_frames*frame_width*frame_height+p]), segment_frames, acf_t, acf_N, frame_width*frame_height);
                    for (register uint32_t ct=0; ct<acf_N; ct++) {
                        sum[ct]=sum[ct]+cftemp[ct];
                        sum2[ct]=sum2[ct]+cftemp[ct]*cftemp[ct];
                    }
                }
                free(cftemp);
                double segs=job.segments;
                for (register uint32_t ct=0; ct<acf_N; ct++) {
                    acf[p*acf_N+ct]=sum[ct]/segs;
                    acf_std[p*acf_N+ct]=sqrt((sum2[ct]-sum[ct]*sum[ct]/segs)/(segs-1.0));
                }
                free(sum);
                free(sum2);
            }
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
        for (uint32_t nn=0; nn<ccf_N*frame_width*frame_height; nn++) {
            ccf1[nn]=1.0;
            ccf2[nn]=1.0;
            ccf3[nn]=1.0;
            ccf4[nn]=1.0;
        }
        if (job.segments>1) {
            ccf1_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
            ccf2_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
            ccf3_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
            ccf4_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
        }
        ccf_tau=(double*)calloc(ccf_N,sizeof(double));
        long* ccf_t=(long*)calloc(ccf_N*frame_width*frame_height,sizeof(long));
        statisticsAutocorrelateCreateMultiTau(ccf_t, job.S, job.m, job.P);
        for (uint32_t p=0; p<frame_width*frame_height; p++) {


            if (job.segments<=1) {
                if ((int32_t)p-1>=0) statisticsCrosscorrelateMultiTauSymmetric(&(ccf1[p*ccf_N]), &(image_series[p-1]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
                if ((int32_t)p+1<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(ccf2[p*ccf_N]), &(image_series[p+1]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
                if ((int32_t)p-(int32_t)frame_width>=0) statisticsCrosscorrelateMultiTauSymmetric(&(ccf3[p*ccf_N]), &(image_series[p-frame_width]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
                if ((int32_t)p+(int32_t)frame_width<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(ccf4[p*ccf_N]), &(image_series[p+frame_width]), &(image_series[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
            } else {
                uint32_t segment_frames=frames/job.segments;
                double* cftemp=(double*)calloc(4*ccf_N,sizeof(double));
                double* sum=(double*)calloc(4*ccf_N,sizeof(double));
                double* sum2=(double*)calloc(4*ccf_N,sizeof(double));
                for (register uint32_t ct=0; ct<4*ccf_N; ct++) {
                    sum[ct]=0;
                    sum2[ct]=0;
                }
                for (int32_t seg=0; seg<job.segments; seg++) {
                    for (register uint32_t ct=0; ct<4*ccf_N; ct++) cftemp[ct]=0;
                    if ((int32_t)p-1>=0)                                                     statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[0*ccf_N]), &(image_series[seg*segment_frames*frame_width*frame_height+p-1]), &(image_series[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    if ((int32_t)p+1<(int32_t)(frame_width*frame_height))                    statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[1*ccf_N]), &(image_series[seg*segment_frames*frame_width*frame_height+p+1]), &(image_series[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    if ((int32_t)p-(int32_t)frame_width>=0)                                  statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[2*ccf_N]), &(image_series[seg*segment_frames*frame_width*frame_height+p-frame_width]), &(image_series[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    if ((int32_t)p+(int32_t)frame_width<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[3*ccf_N]), &(image_series[seg*segment_frames*frame_width*frame_height+p+frame_width]), &(image_series[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    for (register uint32_t ct=0; ct<ccf_N; ct++) {
                        if ((int32_t)p-1>=0) {
                            sum[0*ccf_N+ct]  += cftemp[0*ccf_N+ct];
                            sum2[0*ccf_N+ct] += cftemp[0*ccf_N+ct]*cftemp[0*ccf_N+ct];
                        } else {
                            sum[0*ccf_N+ct] += 1.0;
                            sum2[0*ccf_N+ct] += 1.0*1.0;
                        }
                        if ((int32_t)p+1<(int32_t)(frame_width*frame_height)) {
                            sum[1*ccf_N+ct]  += cftemp[1*ccf_N+ct];
                            sum2[1*ccf_N+ct] += cftemp[1*ccf_N+ct]*cftemp[1*ccf_N+ct];
                        } else {
                            sum[1*ccf_N+ct] += 1.0;
                            sum2[1*ccf_N+ct] += 1.0*1.0;
                        }
                        if ((int32_t)p-(int32_t)frame_width>=0) {
                            sum[2*ccf_N+ct]  += cftemp[2*ccf_N+ct];
                            sum2[2*ccf_N+ct] += cftemp[2*ccf_N+ct]*cftemp[2*ccf_N+ct];
                        } else {
                            sum[2*ccf_N+ct] += 1.0;
                            sum2[2*ccf_N+ct] += 1.0*1.0;
                        }
                        if ((int32_t)p+(int32_t)frame_width<(int32_t)(frame_width*frame_height)) {
                            sum[3*ccf_N+ct]  += cftemp[3*ccf_N+ct];
                            sum2[3*ccf_N+ct] += cftemp[3*ccf_N+ct]*cftemp[3*ccf_N+ct];
                        } else {
                            sum[3*ccf_N+ct] += 1.0;
                            sum2[3*ccf_N+ct] += 1.0*1.0;
                        }
                    }
                }
                free(cftemp);
                double segs=job.segments;
                for (register uint32_t ct=0; ct<ccf_N; ct++) {
                    ccf1[p*ccf_N+ct]=sum[0*ccf_N+ct]/segs;
                    ccf1_std[p*ccf_N+ct]=sqrt((sum2[0*ccf_N+ct]-sum[0*ccf_N+ct]*sum[0*ccf_N+ct]/segs)/(segs-1.0));

                    ccf2[p*ccf_N+ct]=sum[1*ccf_N+ct]/segs;
                    ccf2_std[p*ccf_N+ct]=sqrt((sum2[1*ccf_N+ct]-sum[1*ccf_N+ct]*sum[1*ccf_N+ct]/segs)/(segs-1.0));

                    ccf3[p*ccf_N+ct]=sum[2*ccf_N+ct]/segs;
                    ccf3_std[p*ccf_N+ct]=sqrt((sum2[2*ccf_N+ct]-sum[2*ccf_N+ct]*sum[2*ccf_N+ct]/segs)/(segs-1.0));

                    ccf4[p*ccf_N+ct]=sum[3*ccf_N+ct]/segs;
                    ccf4_std[p*ccf_N+ct]=sqrt((sum2[3*ccf_N+ct]-sum[3*ccf_N+ct]*sum[3*ccf_N+ct]/segs)/(segs-1.0));
                }
                free(sum);
                free(sum2);
            }

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
    QList<MultiTauCorrelator<double, double>* > acfjk;
    QList<MultiTauCorrelator<double, double>* > ccf1jk;
    QList<MultiTauCorrelator<double, double>* > ccf2jk;
    QList<MultiTauCorrelator<double, double>* > ccf3jk;
    QList<MultiTauCorrelator<double, double>* > ccf4jk;

    QList<correlatorjb<double, double>* > acfjb;
    QList<correlatorjb<double, double>* > ccf1jb;
    QList<correlatorjb<double, double>* > ccf2jb;
    QList<correlatorjb<double, double>* > ccf3jb;
    QList<correlatorjb<double, double>* > ccf4jb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // AS NOT ENOUGH MEMORY MAY BE AVAILABLE; WE WAIT HERE UNTIL ENOUGH MEMORY IS AVAILABLE AND MAY BE ALLOCATED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    emit messageChanged(tr("allocating memory ..."));
    frames_min=0;
    frames_max=0;
    if (job.correlator==1) {
        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
            acfjk.append(new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime));
            ccf1jk.append(new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime));
            ccf2jk.append(new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime));
            ccf3jk.append(new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime));
            ccf4jk.append(new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime));
        }
    } else {
        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
            acfjb.append(new correlatorjb<double, double>(job.S, job.P));
            ccf1jb.append(new correlatorjb<double, double>(job.S, job.P));
            ccf2jb.append(new correlatorjb<double, double>(job.S, job.P));
            ccf3jb.append(new correlatorjb<double, double>(job.S, job.P));
            ccf4jb.append(new correlatorjb<double, double>(job.S, job.P));
        }
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
    // NOW WE CALCULATE THE IMAGE BASELINE (ACCORDING TO THE USER SETTINGS). IT WILL BE APPLIED LATER!
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled) calcBackgroundCorrection();



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CALCULATE THE ACFs AND CCFs
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled) {
        emit messageChanged(tr("calculating correlations ..."));

        float* frame_data=(float*)malloc(frame_width*frame_height*sizeof(float));

        acf_N=0;
        if (job.acf) {
            if (job.correlator==1) {
                acf_N=acfjk[0]->getSlots();
            } else {
                acf_N=job.S*job.P;
            }

            if (acf_N>0) {
                acf=(double*)calloc(acf_N*frame_width*frame_height,sizeof(double));
                for (uint32_t nn=0; nn<acf_N*frame_width*frame_height; nn++) {
                    acf[nn]=0.0;
                }
                acf_std=(double*)calloc(acf_N*frame_width*frame_height,sizeof(double));
                for (uint32_t nn=0; nn<acf_N*frame_width*frame_height; nn++) {
                    acf_std[nn]=0.0;
                }
                acf_tau=(double*)calloc(acf_N,sizeof(double));
                if (job.correlator==1) {
                    double* tau=acfjk[0]->getCorTau();
                    for (uint32_t tt=0; tt<acf_N; tt++) {
                        acf_tau[tt]=tau[tt];
                    }
                } else {
                    double** corr1=acfjb[0]->get_array_G();
                    for (uint32_t tt=0; tt<acf_N; tt++) {
                        acf_tau[tt]=job.frameTime*corr1[0][tt];
                    }
                    free(corr1[0]);
                    free(corr1[1]);
                }
            }
        }

        ccf_N=0;
        if (job.ccf) {
            if (job.correlator==1) {
                ccf_N=ccf1jk[0]->getSlots();
            } else {
                ccf_N=job.S*job.P;
            }

            if (ccf_N>0) {
                ccf1=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                ccf2=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                ccf3=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                ccf4=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                for (uint32_t nn=0; nn<ccf_N*frame_width*frame_height; nn++) {
                    ccf1[nn]=0.0;
                    ccf2[nn]=0.0;
                    ccf3[nn]=0.0;
                    ccf4[nn]=0.0;
                }
                ccf1_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                ccf2_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                ccf3_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                ccf4_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
                for (uint32_t nn=0; nn<ccf_N*frame_width*frame_height; nn++) {
                    ccf1_std[nn]=0.0;
                    ccf2_std[nn]=0.0;
                    ccf3_std[nn]=0.0;
                    ccf4_std[nn]=0.0;
                }
                ccf_tau=(double*)calloc(ccf_N,sizeof(double));
                if (job.correlator==1) {
                    double* tau=ccf1jk[0]->getCorTau();
                    for (uint32_t tt=0; tt<ccf_N; tt++) {
                        ccf_tau[tt]=tau[tt];
                    }
                } else {
                    double** corr1=ccf1jb[0]->get_array_G();
                    for (uint32_t tt=0; tt<acf_N; tt++) {
                        ccf_tau[tt]=job.frameTime*corr1[0][tt];
                    }
                    free(corr1[0]);
                    free(corr1[1]);
                }
            }
        }

        reader->reset();
        register uint32_t frame=0;
        bool OK=true;
        while (OK && (frame<first_frame)) {
            OK=reader->nextFrame();
            frame++;
        }
        frame=0;
        uint32_t segment_frames=frames/job.segments;
        do {
            if (!reader->readFrameFloat(frame_data)) {
                m_status=-1; emit statusChanged(m_status);
                emit messageChanged(tr("error reading frame: %1").arg(reader->lastError()));
            } else {
                // APPLY BACKGROUND CORRECTION TO FRAME
                for (register uint16 i=0; i<frame_width*frame_height; i++) {
                    frame_data[i]=frame_data[i]-baseline-backgroundImage[i];
                }
                // CALCULATE CORRELATIONS, we store the sum in acf and the sum of squares in acf_std
                if (job.acf && acf_N>0) {
                    if (job.correlator==1) {
                        for (register uint16 i=0; i<frame_width*frame_height; i++) {
                            register double v=frame_data[i];
                            acfjk[i]->correlate_step(v);

                            if ((frame%segment_frames)==(segment_frames-1)) {
                                acfjk[i]->normalize();
                                double* corr=acfjk[i]->getCor();
                                for (register uint32_t tt=0; tt<acf_N; tt++) {
                                    register double v=corr[tt];
                                    acf[i*acf_N+tt]=acf[i*acf_N+tt]+v;
                                    acf_std[i*acf_N+tt]=acf_std[i*acf_N+tt]+v*v;
                                }
                                delete acfjk[i];
                                acfjk[i]=new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime);
                            }
                        }
                    } else {
                        for (register uint16 i=0; i<frame_width*frame_height; i++) {
                            register double v=frame_data[i];
                            acfjb[i]->run(v, v);

                            if ((frame%segment_frames)==(segment_frames-1)) {
                                double** corr=acfjb[i]->get_array_G();
                                for (register uint32_t tt=0; tt<acf_N; tt++) {
                                    register double v=corr[1][tt];
                                    acf[i*acf_N+tt]=acf[i*acf_N+tt]+v;
                                    acf_std[i*acf_N+tt]=acf_std[i*acf_N+tt]+v*v;
                                }
                                delete acfjb[i];
                                acfjb[i]=new correlatorjb<double, double>(job.S, job.P);
                                free(corr[0]);
                                free(corr[1]);
                            }
                        }
                    }
                }
                if (job.ccf && ccf_N>0) {
                    if (job.correlator==1) {
                        for (register uint16 i=0; i<frame_width*frame_height; i++) {
                            if ((int32_t)i-1>=0) ccf1jk[i]->crosscorrelate_step(frame_data[i], frame_data[i]-1);
                            if ((int32_t)i+1<(int32_t)(frame_width*frame_height))  ccf2jk[i]->crosscorrelate_step(frame_data[i], frame_data[i]+1);
                            if ((int32_t)i-(int32_t)frame_width>=0)  ccf3jk[i]->crosscorrelate_step(frame_data[i], frame_data[i]-frame_width);
                            if ((int32_t)i+(int32_t)frame_width<(int32_t)(frame_width*frame_height))  ccf4jk[i]->crosscorrelate_step(frame_data[i], frame_data[i]+frame_width);




                            if ((frame%segment_frames)==(segment_frames-1)) {
                                ccf1jk[i]->crossnormalize();
                                ccf2jk[i]->crossnormalize();
                                ccf3jk[i]->crossnormalize();
                                ccf4jk[i]->crossnormalize();
                                double* corr1=ccf1jk[i]->getCor();
                                double* corr2=ccf2jk[i]->getCor();
                                double* corr3=ccf3jk[i]->getCor();
                                double* corr4=ccf4jk[i]->getCor();
                                for (register uint32_t tt=0; tt<acf_N; tt++) {
                                    register double v=corr1[tt];
                                    ccf1[i*acf_N+tt]=ccf1[i*acf_N+tt]+v;
                                    ccf1_std[i*acf_N+tt]=ccf1_std[i*acf_N+tt]+v*v;
                                    v=corr2[tt];
                                    ccf2[i*acf_N+tt]=ccf2[i*acf_N+tt]+v;
                                    ccf2_std[i*acf_N+tt]=ccf2_std[i*acf_N+tt]+v*v;
                                    v=corr3[tt];
                                    ccf3[i*acf_N+tt]=ccf3[i*acf_N+tt]+v;
                                    ccf3_std[i*acf_N+tt]=ccf3_std[i*acf_N+tt]+v*v;
                                    v=corr4[tt];
                                    ccf4[i*acf_N+tt]=ccf4[i*acf_N+tt]+v;
                                    ccf4_std[i*acf_N+tt]=ccf4_std[i*acf_N+tt]+v*v;
                                }
                                delete ccf1jk[i];
                                ccf1jk[i]=new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime);
                                delete ccf2jk[i];
                                ccf2jk[i]=new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime);
                                delete ccf3jk[i];
                                ccf3jk[i]=new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime);
                                delete ccf4jk[i];
                                ccf4jk[i]=new MultiTauCorrelator<double, double>(job.S, job.m, job.P, job.frameTime);
                            }
                        }
                    } else {
                        for (register uint16 i=0; i<frame_width*frame_height; i++) {
                            if ((int32_t)i-1>=0) ccf1jb[i]->run(frame_data[i], frame_data[i]-1);
                            if ((int32_t)i+1<(int32_t)(frame_width*frame_height))  ccf2jb[i]->run(frame_data[i], frame_data[i]+1);
                            if ((int32_t)i-(int32_t)frame_width>=0)  ccf3jb[i]->run(frame_data[i], frame_data[i]-frame_width);
                            if ((int32_t)i+(int32_t)frame_width<(int32_t)(frame_width*frame_height))  ccf4jb[i]->run(frame_data[i], frame_data[i]+frame_width);




                            if ((frame%segment_frames)==(segment_frames-1)) {
                                double** corr1=ccf1jb[i]->get_array_G();
                                double** corr2=ccf2jb[i]->get_array_G();
                                double** corr3=ccf3jb[i]->get_array_G();
                                double** corr4=ccf4jb[i]->get_array_G();
                                for (register uint32_t tt=0; tt<acf_N; tt++) {
                                    register double v=corr1[1][tt];
                                    ccf1[i*acf_N+tt]=ccf1[i*acf_N+tt]+v;
                                    ccf1_std[i*acf_N+tt]=ccf1_std[i*acf_N+tt]+v*v;
                                    v=corr2[1][tt];
                                    ccf2[i*acf_N+tt]=ccf2[i*acf_N+tt]+v;
                                    ccf2_std[i*acf_N+tt]=ccf2_std[i*acf_N+tt]+v*v;
                                    v=corr3[1][tt];
                                    ccf3[i*acf_N+tt]=ccf3[i*acf_N+tt]+v;
                                    ccf3_std[i*acf_N+tt]=ccf3_std[i*acf_N+tt]+v*v;
                                    v=corr4[1][tt];
                                    ccf4[i*acf_N+tt]=ccf4[i*acf_N+tt]+v;
                                    ccf4_std[i*acf_N+tt]=ccf4_std[i*acf_N+tt]+v*v;
                                }
                                delete ccf1jb[i];
                                ccf1jb[i]=new correlatorjb<double, double>(job.S, job.P);
                                delete ccf2jb[i];
                                ccf2jb[i]=new correlatorjb<double, double>(job.S, job.P);
                                delete ccf3jb[i];
                                ccf3jb[i]=new correlatorjb<double, double>(job.S, job.P);
                                delete ccf4jb[i];
                                ccf4jb[i]=new correlatorjb<double, double>(job.S, job.P);
                                free(corr1[0]); free(corr1[1]);
                                free(corr2[0]); free(corr2[1]);
                                free(corr3[0]); free(corr3[1]);
                                free(corr4[0]); free(corr4[1]);
                            }
                        }
                    }
                }
            }

            if (frame%(frames/1000)==0) {
                emit messageChanged(tr("calculating correlations ..."));
                emit progressIncrement(1);
            }
            frame++;
            if (was_canceled) break;
        } while (reader->nextFrame() && (m_status==1) && (frame<frames));

        // calculate avg + stddev from sum and square-sum, as calculated above
        if (job.acf && acf_N>0) {
            for (register uint64_t tt=0; tt<acf_N*frame_width*frame_height; tt++) {
                double sum=acf[tt];
                double sum2=acf_std[tt];
                acf[tt]=sum/(double)job.segments;
                if (job.segments>1) {
                    acf_std[tt]=sqrt((sum2-sum*sum/(double)job.segments)/(double)(job.segments-1));
                } else {
                    acf_std[tt]=0;
                }
            }
            if (job.segments<=1) {
                free(acf_std);
                acf_std=NULL;
            }
        }
        if (job.ccf && ccf_N>0) {
            for (register uint64_t tt=0; tt<ccf_N*frame_width*frame_height; tt++) {
                double sum=ccf1[tt];
                double sum2=ccf1_std[tt];
                ccf1[tt]=sum/(double)job.segments;
                if (job.segments>1) {
                    ccf1_std[tt]=sqrt((sum2-sum*sum/(double)job.segments)/(double)(job.segments-1));
                } else {
                    ccf1_std[tt]=0;
                }

                sum=ccf2[tt];
                sum2=ccf2_std[tt];
                ccf2[tt]=sum/(double)job.segments;
                if (job.segments>1) {
                    ccf2_std[tt]=sqrt((sum2-sum*sum/(double)job.segments)/(double)(job.segments-1));
                } else {
                    ccf2_std[tt]=0;
                }

                sum=ccf3[tt];
                sum2=ccf3_std[tt];
                ccf3[tt]=sum/(double)job.segments;
                if (job.segments>1) {
                    ccf3_std[tt]=sqrt((sum2-sum*sum/(double)job.segments)/(double)(job.segments-1));
                } else {
                    ccf3_std[tt]=0;
                }

                sum=ccf4[tt];
                sum2=ccf4_std[tt];
                ccf4[tt]=sum/(double)job.segments;
                if (job.segments>1) {
                    ccf4_std[tt]=sqrt((sum2-sum*sum/(double)job.segments)/(double)(job.segments-1));
                } else {
                    ccf4_std[tt]=0;
                }
            }
            if (job.segments<=1) {
                free(ccf1_std);
                ccf1_std=NULL;
                free(ccf2_std);
                ccf2_std=NULL;
                free(ccf3_std);
                ccf3_std=NULL;
                free(ccf4_std);
                ccf4_std=NULL;
            }
        }
        free(frame_data);
    }

/*
        statisticsAutocorrelateCreateMultiTau(acf_t, job.S, job.m, job.P);
        for (uint32_t p=0; p<frame_width*frame_height; p++) {
            if (job.segments<=1) {
                statisticsAutocorrelateMultiTauSymmetric(&(acf[p*acf_N]), &(current_frame[p]), frames, acf_t, acf_N, frame_width*frame_height);
            } else {
                uint32_t segment_frames=frames/job.segments;
                double* cftemp=(double*)calloc(acf_N,sizeof(double));
                double* sum=(double*)calloc(acf_N,sizeof(double));
                double* sum2=(double*)calloc(acf_N,sizeof(double));
                for (register uint32_t ct=0; ct<acf_N; ct++) {
                    sum[ct]=0;
                    sum2[ct]=0;
                }
                for (int32_t seg=0; seg<job.segments; seg++) {
                    for (register uint32_t ct=0; ct<acf_N; ct++) cftemp[ct]=0;
                    statisticsAutocorrelateMultiTauSymmetric(cftemp, &(current_frame[seg*segment_frames*frame_width*frame_height+p]), segment_frames, acf_t, acf_N, frame_width*frame_height);
                    for (register uint32_t ct=0; ct<acf_N; ct++) {
                        sum[ct]=sum[ct]+cftemp[ct];
                        sum2[ct]=sum2[ct]+cftemp[ct]*cftemp[ct];
                    }
                }
                free(cftemp);
                double segs=job.segments;
                for (register uint32_t ct=0; ct<acf_N; ct++) {
                    acf[p*acf_N+ct]=sum[ct]/segs;
                    acf_std[p*acf_N+ct]=sqrt((sum2[ct]-sum[ct]*sum[ct]/segs)/(segs-1.0));
                }
                free(sum);
                free(sum2);
            }
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
        for (uint32_t nn=0; nn<ccf_N*frame_width*frame_height; nn++) {
            ccf1[nn]=1.0;
            ccf2[nn]=1.0;
            ccf3[nn]=1.0;
            ccf4[nn]=1.0;
        }
        if (job.segments>1) {
            ccf1_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
            ccf2_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
            ccf3_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
            ccf4_std=(double*)calloc(ccf_N*frame_width*frame_height,sizeof(double));
        }
        ccf_tau=(double*)calloc(ccf_N,sizeof(double));
        long* ccf_t=(long*)calloc(ccf_N*frame_width*frame_height,sizeof(long));
        statisticsAutocorrelateCreateMultiTau(ccf_t, job.S, job.m, job.P);
        for (uint32_t p=0; p<frame_width*frame_height; p++) {


            if (job.segments<=1) {
                if ((int32_t)p-1>=0) statisticsCrosscorrelateMultiTauSymmetric(&(ccf1[p*ccf_N]), &(current_frame[p-1]), &(current_frame[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
                if ((int32_t)p+1<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(ccf2[p*ccf_N]), &(current_frame[p+1]), &(current_frame[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
                if ((int32_t)p-(int32_t)frame_width>=0) statisticsCrosscorrelateMultiTauSymmetric(&(ccf3[p*ccf_N]), &(current_frame[p-frame_width]), &(current_frame[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
                if ((int32_t)p+(int32_t)frame_width<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(ccf4[p*ccf_N]), &(current_frame[p+frame_width]), &(current_frame[p]), frames, ccf_t, ccf_N, frame_width*frame_height);
            } else {
                uint32_t segment_frames=frames/job.segments;
                double* cftemp=(double*)calloc(4*ccf_N,sizeof(double));
                double* sum=(double*)calloc(4*ccf_N,sizeof(double));
                double* sum2=(double*)calloc(4*ccf_N,sizeof(double));
                for (register uint32_t ct=0; ct<4*ccf_N; ct++) {
                    sum[ct]=0;
                    sum2[ct]=0;
                }
                for (int32_t seg=0; seg<job.segments; seg++) {
                    for (register uint32_t ct=0; ct<4*ccf_N; ct++) cftemp[ct]=0;
                    if ((int32_t)p-1>=0)                                                     statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[0*ccf_N]), &(current_frame[seg*segment_frames*frame_width*frame_height+p-1]), &(current_frame[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    if ((int32_t)p+1<(int32_t)(frame_width*frame_height))                    statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[1*ccf_N]), &(current_frame[seg*segment_frames*frame_width*frame_height+p+1]), &(current_frame[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    if ((int32_t)p-(int32_t)frame_width>=0)                                  statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[2*ccf_N]), &(current_frame[seg*segment_frames*frame_width*frame_height+p-frame_width]), &(current_frame[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    if ((int32_t)p+(int32_t)frame_width<(int32_t)(frame_width*frame_height)) statisticsCrosscorrelateMultiTauSymmetric(&(cftemp[3*ccf_N]), &(current_frame[seg*segment_frames*frame_width*frame_height+p+frame_width]), &(current_frame[seg*segment_frames*frame_width*frame_height+p]), segment_frames, ccf_t, ccf_N, frame_width*frame_height);
                    for (register uint32_t ct=0; ct<ccf_N; ct++) {
                        if ((int32_t)p-1>=0) {
                            sum[0*ccf_N+ct]  += cftemp[0*ccf_N+ct];
                            sum2[0*ccf_N+ct] += cftemp[0*ccf_N+ct]*cftemp[0*ccf_N+ct];
                        } else {
                            sum[0*ccf_N+ct] += 1.0;
                            sum2[0*ccf_N+ct] += 1.0*1.0;
                        }
                        if ((int32_t)p+1<(int32_t)(frame_width*frame_height)) {
                            sum[1*ccf_N+ct]  += cftemp[1*ccf_N+ct];
                            sum2[1*ccf_N+ct] += cftemp[1*ccf_N+ct]*cftemp[1*ccf_N+ct];
                        } else {
                            sum[1*ccf_N+ct] += 1.0;
                            sum2[1*ccf_N+ct] += 1.0*1.0;
                        }
                        if ((int32_t)p-(int32_t)frame_width>=0) {
                            sum[2*ccf_N+ct]  += cftemp[2*ccf_N+ct];
                            sum2[2*ccf_N+ct] += cftemp[2*ccf_N+ct]*cftemp[2*ccf_N+ct];
                        } else {
                            sum[2*ccf_N+ct] += 1.0;
                            sum2[2*ccf_N+ct] += 1.0*1.0;
                        }
                        if ((int32_t)p+(int32_t)frame_width<(int32_t)(frame_width*frame_height)) {
                            sum[3*ccf_N+ct]  += cftemp[3*ccf_N+ct];
                            sum2[3*ccf_N+ct] += cftemp[3*ccf_N+ct]*cftemp[3*ccf_N+ct];
                        } else {
                            sum[3*ccf_N+ct] += 1.0;
                            sum2[3*ccf_N+ct] += 1.0*1.0;
                        }
                    }
                }
                free(cftemp);
                double segs=job.segments;
                for (register uint32_t ct=0; ct<ccf_N; ct++) {
                    ccf1[p*ccf_N+ct]=sum[0*ccf_N+ct]/segs;
                    ccf1_std[p*ccf_N+ct]=sqrt((sum2[0*ccf_N+ct]-sum[0*ccf_N+ct]*sum[0*ccf_N+ct]/segs)/(segs-1.0));

                    ccf2[p*ccf_N+ct]=sum[1*ccf_N+ct]/segs;
                    ccf2_std[p*ccf_N+ct]=sqrt((sum2[1*ccf_N+ct]-sum[1*ccf_N+ct]*sum[1*ccf_N+ct]/segs)/(segs-1.0));

                    ccf3[p*ccf_N+ct]=sum[2*ccf_N+ct]/segs;
                    ccf3_std[p*ccf_N+ct]=sqrt((sum2[2*ccf_N+ct]-sum[2*ccf_N+ct]*sum[2*ccf_N+ct]/segs)/(segs-1.0));

                    ccf4[p*ccf_N+ct]=sum[3*ccf_N+ct]/segs;
                    ccf4_std[p*ccf_N+ct]=sqrt((sum2[3*ccf_N+ct]-sum[3*ccf_N+ct]*sum[3*ccf_N+ct]/segs)/(segs-1.0));
                }
                free(sum);
                free(sum2);
            }

            emit messageChanged(tr("calculating crosscorrelations %1/%2 ...").arg(p+1).arg(frame_width*frame_height));
            if (frame_width*frame_height<500) emit progressIncrement(ceil(500/(frame_width*frame_height)));
            else if (p%(frame_width*frame_height/500)==0) emit progressIncrement(1);
        }
        for (uint32_t i=0; i<acf_N; i++) {
            ccf_tau[i]=(double)ccf_t[i]*job.frameTime;
        }
        free(ccf_t);
    }
*/

    for (register int i=0; i<acfjk.size(); i++) {
        if (acfjk[i]) delete acfjk[i];
    }
    for (register int i=0; i<ccf1jk.size(); i++) {
        if (ccf1jk[i]) delete ccf1jk[i];
    }
    for (register int i=0; i<ccf2jk.size(); i++) {
        if (ccf2jk[i]) delete ccf2jk[i];
    }
    for (register int i=0; i<ccf3jk.size(); i++) {
        if (ccf3jk[i]) delete ccf3jk[i];
    }
    for (register int i=0; i<ccf4jk.size(); i++) {
        if (ccf4jk[i]) delete ccf4jk[i];
    }

    for (register int i=0; i<acfjb.size(); i++) {
        if (acfjb[i]) delete acfjb[i];
    }
    for (register int i=0; i<ccf1jb.size(); i++) {
        if (ccf1jb[i]) delete ccf1jb[i];
    }
    for (register int i=0; i<ccf2jb.size(); i++) {
        if (ccf2jb[i]) delete ccf2jb[i];
    }
    for (register int i=0; i<ccf3jb.size(); i++) {
        if (ccf3jb[i]) delete ccf3jb[i];
    }
    for (register int i=0; i<ccf4jb.size(); i++) {
        if (ccf4jb[i]) delete ccf4jb[i];
    }

}


void QFRDRImagingFCSCorrelationJobThread::calcBackgroundCorrection() {
    if (backgroundImage) free(backgroundImage);
    backgroundImage=(float*)malloc(frame_width*frame_height*sizeof(float));
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
    for (register uint64_t i=0; i<frame_width*frame_height; i++) {
        backgroundImage[i]=0;
    }
}



