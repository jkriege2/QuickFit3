#include "qfetcspcimporterjobthread.h"
#include "qfetcspcimporterdialog.h"
#include "qftcspcreader.h"
#include "statistics_tools.h"
#include <stdint.h>
#include "qfimportermanager.h"

QMutex* QFETCSPCImporterJobThread::mutexFilename=NULL;

QFETCSPCImporterJobThread::QFETCSPCImporterJobThread(QFPluginServices *services, QObject *parent) :
    QThread(parent)
{
    if (!mutexFilename) mutexFilename=new QMutex();
    outLocale=QLocale::c();
    outLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    m_status=0;
    was_canceled=false;
    duration=0;
    this->pluginServices=services;
}

QFETCSPCImporterJobThread::~QFETCSPCImporterJobThread() {
}

double QFETCSPCImporterJobThread::durationMS() const {
    return duration;
}

double QFETCSPCImporterJobThread::durationS() const {
    return duration;
}

QStringList QFETCSPCImporterJobThread::getAddFiles() const {
    return addFiles;
}

TCSPCImporterJob QFETCSPCImporterJobThread::getJob() const {
    return job;
}

QStringList QFETCSPCImporterJobThread::getImporterFilterList(QFPluginServices* pluginServices)  {
    QStringList l;
    int i=0;
    QFTCSPCReader* r=NULL;
    while ((r=getImporter(i, pluginServices))!=NULL) {
        l.append(r->filter());
        delete r;
        i++;
    }
    return l;
}

QStringList QFETCSPCImporterJobThread::getImporterFormatNameList(QFPluginServices *pluginServices)  {
     QStringList l;
     int i=0;
     QFTCSPCReader* r=NULL;
     while ((r=getImporter(i, pluginServices))!=NULL) {
         l.append(r->formatName());
         delete r;
         i++;
     }
     return l;
}

QFTCSPCReader *QFETCSPCImporterJobThread::getImporter(int idx, QFPluginServices *pluginservices)  {
    QFTCSPCReader* r=NULL;

    QStringList imp=pluginservices->getImporterManager()->getImporters<QFTCSPCReader*>();

    if (idx>=0 && idx<imp.size()) {
        r=dynamic_cast<QFTCSPCReader*>(pluginservices->getImporterManager()->createImporter(imp[idx]));
    }

    return r;

}

int QFETCSPCImporterJobThread::getImporterCount(QFPluginServices *pluginservices) {
    QStringList imp=pluginservices->getImporterManager()->getImporters<QFTCSPCReader*>();
    return imp.size();

}

int QFETCSPCImporterJobThread::status() const {
    return m_status;
}

void QFETCSPCImporterJobThread::cancel() {
    was_canceled=true;
    if (!isRunning()) {
        m_status=-1;
        emit messageChanged(tr("canceled by user"));
        emit statusChanged(m_status);
    }
}

void QFETCSPCImporterJobThread::init(TCSPCImporterJob job) {
    this->job=job;
    m_status=0;
    was_canceled=false;
    addFiles.clear();
}

QString QFETCSPCImporterJobThread::replacePostfixSpecials(const QString& input, int counter) const {
    QString cnt="";
    if (counter>0) cnt=QString::number(counter);
    QString result=input;
    result=result.replace("%counter%", cnt, Qt::CaseInsensitive);
    result=result.replace("%s%", QString::number(job.S), Qt::CaseInsensitive);
    result=result.replace("%p%", QString::number(job.P), Qt::CaseInsensitive);
    result=result.replace("%m%", QString::number(job.m), Qt::CaseInsensitive);
    result=result.replace("%start%", QString::number(job.range_min), Qt::CaseInsensitive);
    result=result.replace("%end%", QString::number(job.range_max), Qt::CaseInsensitive);
    result=result.replace("%fcs_segments%", QString::number(job.fcs_segments), Qt::CaseInsensitive);
    result=result.replace("%fcs_taumin%", QString::number(job.fcs_taumin), Qt::CaseInsensitive);
    result=result.replace("%cr_binning%", QString::number(job.countrate_binning), Qt::CaseInsensitive);

    QString corr="unknown";
    if (job.correlator==CORRELATOR_DIRECT) corr="direct";
    if (job.correlator==CORRELATOR_DIRECTAVG) corr="directavg";
    if (job.correlator==CORRELATOR_MTAUALLMON) corr="mtauallmon";
    if (job.correlator==CORRELATOR_MTAUONEMON) corr="mtauonemon";

    result=result.replace("%correlator%", corr, Qt::CaseInsensitive);
    result=result.replace("%correlatorid%", QString::number(job.correlator), Qt::CaseInsensitive);
    return result;
}

void QFETCSPCImporterJobThread::run() {
    /*QTime ptime;

    ptime.start();

    emit messageChanged(tr("loading data ..."));
    if ((job.fileFormat<0)||(job.fileFormat>=getImageReaderCount())) {
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
        emit rangeChanged(0,4280);


        reader=NULL;
        bool OK=false;
        if (job.fileFormat>=0 && job.fileFormat<getImageReaderCount()) {
            reader=getImageReader(job.fileFormat);
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

                    video_count=floor(frames/job.video_frames);
                    real_video_count=video_count;
                    video=(float*)calloc(frame_width*frame_height*video_count, sizeof(float));
                    bleachOffset=(float*)calloc(frame_width*frame_height, sizeof(float));
                    bleachAmplitude=(float*)calloc(frame_width*frame_height, sizeof(float));
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
                    QString backgroundFilename="";
                    QString videoFilename="";
                    QString statisticsFilename="";
                    QString acfFilename="";
                    QString ccfFilename="";
                    QString dccfFilename="";
                    QString acfFilenameBin="";
                    QString ccfFilenameBin="";
                    QString dccfFilenameBin="";
                    QString bleachOffsetFilename="";
                    QString bleachAmplitudeFilename="";
                    QString firstFramesFilename="";
                    QString lastFramesFilename="";
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
                                if (error) {
                                    m_status=-1; emit statusChanged(m_status);
                                    backgroundFilename="";
                                    emit messageChanged(tr("could not create background image '%1'!").arg(localFilename));
                                }
                            }
                        }
                        emit progressIncrement(10);

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
                                emit messageChanged(tr("could not create statistics plot file '%1': %2!").arg(localFilename).arg(f.errorString()));
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
                            qDebug()<<"acf = "<<acf<<"  acf_std = "<<acf_std;
                            if (!saveCorrelationCSV(localFilename, acf_tau, ccf, ccferr, 1, acf_N, frame_width, frame_height, input_length, error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create autocorrelation file '%1': %2!").arg(localFilename).arg(error));
                            }
                            if (!saveCorrelationBIN(localFilename1, acf_tau, ccf, ccferr, 1, acf_N, frame_width, frame_height, ccfsegments, error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create binary autocorrelation file '%1': %2!").arg(localFilename1).arg(error));
                            }
                            if (QFile::exists(localFilename1)) addFiles.append(localFilename1);
                            else addFiles.append(localFilename);

                        }
                        emit progressIncrement(10);


                        //************** SAVE DCCF
                        if ((m_status==1) && !was_canceled && job.distanceCCF && dccf && dccf_tau && dccf_N>0) {
                            QString localFilename=dccfFilename=outputFilenameBase+".dccf.dat";
                            QString localFilename1=dccfFilenameBin=outputFilenameBase+".dccf.bin";

                            emit messageChanged(tr("saving distance distance crosscorrelation ..."));
                            double* ccf[1]={dccf};
                            double* ccferr[1]={dccf_std};
                            double* ccfsegments[1]={dccf_segments};
                            QString error;
                            if (!saveCorrelationCSV(localFilename, dccf_tau, ccf, ccferr, 1, dccf_N, frame_width, frame_height, input_length, error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create distance crosscorrelation file '%1': %2!").arg(localFilename).arg(error));
                            }
                            if (!saveCorrelationBIN(localFilename1, dccf_tau, ccf, ccferr, 1, dccf_N, frame_width, frame_height, ccfsegments, error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create binary distance crosscorrelation file '%1': %2!").arg(localFilename1).arg(error));
                            }
                            if (QFile::exists(localFilename1)) addFiles.append(localFilename1);
                            else addFiles.append(localFilename);


                        }
                        emit progressIncrement(10);


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
                                if (!backgroundFilename.isEmpty())      text<<"background image file       : " << d.relativeFilePath(backgroundFilename) << "\n";
                                if (!videoFilename.isEmpty())           text<<"video file                  : " << d.relativeFilePath(videoFilename) << "\n";
                                if (!statisticsFilename.isEmpty())      text<<"statistics file             : " << d.relativeFilePath(statisticsFilename) << "\n";
                                if (!acfFilename.isEmpty())             text<<"autocorrelation file        : " << d.relativeFilePath(acfFilename) << "\n";
                                if (!acfFilenameBin.isEmpty())          text<<"bin. autocorrelation file   : " << d.relativeFilePath(acfFilenameBin) << "\n";
                                if (!ccfFilename.isEmpty())             text<<"crosscorrelation file       : " << d.relativeFilePath(ccfFilename) << "\n";
                                if (!ccfFilenameBin.isEmpty())          text<<"bin. crosscorrelation file  : " << d.relativeFilePath(ccfFilenameBin) << "\n";
                                if (!dccfFilename.isEmpty())            text<<"distance ccf file           : " << d.relativeFilePath(dccfFilename) << "\n";
                                if (!dccfFilenameBin.isEmpty())         text<<"bin. distance ccf file      : " << d.relativeFilePath(dccfFilenameBin) << "\n";
                                text<<"width                       : "<<outLocale.toString(frame_width) << "\n";
                                text<<"height                      : "<<outLocale.toString(frame_height) << "\n";
                                text<<"binning                     : "<<outLocale.toString(reader->getBinning()) << "\n";
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
                                    case CORRELATOR_MTAUALLMON:  text<<"multi-tau with monitors for all channels\n"; break;
                                    case CORRELATOR_MTAUONEMON:  text<<"multi-tau with a single monitor\n"; break;

                                    default: text<<"correlator type name        : unknown\n"; break;
                                }
                                text<<"smallest tau [s]            : "<<outLocale.toString(job.frameTime) << "\n";
                                text<<"baseline                    : "<<outLocale.toString(baseline) << "\n";
                                if (video && job.video) {
                                    text<<"video sum up                : "<<outLocale.toString(job.video_frames) << "\n";
                                    text<<"video frames                : "<<outLocale.toString(real_video_count) << "\n";
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
                                if (job.bleach==2) {
                                    text<<"remove exponential\n";
                                    text<<"bleach decay constant [pix] : "<<outLocale.toString(job.bleachDecay) << "\n";
                                    text<<"bleach decay constant [s]   : "<<outLocale.toString((double)job.bleachDecay*job.frameTime) << "\n";
                                    text<<"bleach offset B             : "<<outLocale.toString((double)job.bleachB) << "\n";
                                    text<<"bleach amplitude A          : "<<outLocale.toString((double)job.bleachA) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach offset file          : "<<d.relativeFilePath(bleachOffsetFilename) << "\n";
                                    text<<"bleach: first frames        : "<<d.relativeFilePath(firstFramesFilename) << "\n";
                                    text<<"bleach: last frames         : "<<d.relativeFilePath(lastFramesFilename) << "\n";

                                } else if (job.bleach==1) {
                                        text<<"remove frame average\n";

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
                    if (bleachOffset) free(bleachOffset);
                    if (bleachAmplitude) free(bleachAmplitude);
                    if (average_frame) free(average_frame);
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


    duration=ptime.elapsed();*/
}



