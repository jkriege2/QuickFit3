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

#include <QTextStream>

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
    if (!mutexFilename) mutexFilename=new QMutex(QMutex::Recursive);
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
    if (backgroundImage) qfFree(backgroundImage);
    if (backgroundImageStd) qfFree(backgroundImageStd);
}

double QFRDRImagingFCSCorrelationJobThread::durationMS() const {
    return duration;
}

double QFRDRImagingFCSCorrelationJobThread::durationS() const {
    return duration;
}

QList<QFRDRImagingFCSCorrelationJobThread::Fileinfo> QFRDRImagingFCSCorrelationJobThread::getAddFiles() const {
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
    if (job.correlator==CORRELATOR_CORRELATORFROMSHAREDLIB) corr="correlatorfromshared";
    QString bleach="none";
    if (job.bleach==BLEACH_REMOVEAVG) bleach="delavg";
    if (job.bleach==BLEACH_EXP) bleach="exp";
    if (job.bleach==BLEACH_EXPREG) bleach="expreg";
    if (job.bleach==BLEACH_EXP_POLY2) bleach="expofpoly2";
    if (job.bleach==BLEACH_EXP_POLY3) bleach="expofpoly3";
    if (job.bleach==BLEACH_EXP_POLY4) bleach="expofpoly4";
    if (job.bleach==BLEACH_EXP_POLY5) bleach="expofpoly5";
    if (job.bleach==BLEACH_DBL_EXP) bleach="dblexp";

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
        emit rangeChanged(0,3780+1000*job.DCCFDeltaX.size());


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
                props=reader->getFileInfo().properties;
                comment=reader->getFileInfo().comment;
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
                    average_frame=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    sqrsum_frame=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    average_uncorrected_frame=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    sqrsum_uncorrected_frame=(float*)qfCalloc(frame_width*frame_height, sizeof(float));

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
                    //dccf_tau=NULL;
                    //dccf=NULL;
                    //dccf_segments=NULL;
                    //dccf_std=NULL;
                    //dccf_N=0;
                    //dccfframe_width=0;
                    //dccfframe_height=0;
                    dccf.clear();
                    for (int id=0; id<job.DCCFDeltaX.size(); id++) {
                        DCCFRecord rec;
                        rec.dccf_tau=NULL;
                        rec.dccf=NULL;
                        rec.dccf_segments=NULL;
                        rec.dccf_std=NULL;
                        rec.dccf_N=0;
                        rec.dccfframe_width=0;
                        rec.dccfframe_height=0;
                        dccf.append(rec);
                    }
                    lastFrames=NULL;
                    firstFrames=NULL;
                    bleachAmplitude=NULL;
                    bleachTime=NULL;
                    bleachPoly2=NULL;
                    bleachPoly3=NULL;
                    fit_frames=NULL;
                    fit_t=NULL;
                    NFitFrames=0;

                    video_count=floor(frames/job.video_frames);
                    real_video_count=video_count;
                    video=(float*)qfCalloc(frame_width*frame_height*video_count, sizeof(float));
                    video_uncorrected=(float*)qfCalloc(frame_width*frame_height*video_count, sizeof(float));
                    bleachAmplitude=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachTime=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachAmplitude2=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachTime2=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachPoly2=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachPoly3=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachPoly4=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachPoly5=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    bleachFitOK=(uint8_t*)qfCalloc(frame_width*frame_height, sizeof(uint8_t));
                    //firstFrames=(float*)qfCalloc(frame_width*frame_height, sizeof(float));
                    //lastFrames=(float*)qfCalloc(frame_width*frame_height, sizeof(float));

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
                    QString averageUncorrectedFilename="";
                    QString stdUncorrectedFilename="";
                    QString backstdFilename="";
                    QString averageFilenameF="";
                    QString backgroundFilename="";
                    QString videoFilename="";
                    QString videoUncorrectedFilename="";
                    QString statisticsFilename="";
                    QString statisticsFilename_dv1="";
                    QString statisticsFilename_dv2="";
                    QString uncorrectedStatisticsFilename="";
                    QString uncorrectedStatisticsFilename_dv1="";
                    QString uncorrectedStatisticsFilename_dv2="";
                    QString backstatisticsFilename="";
                    QString backstatisticsFilename_dv1="";
                    QString backstatisticsFilename_dv2="";
                    QString acfFilename="";
                    QString ccfFilename="";
                    QStringList dccfFilename;
                    QString acfFilenameBin="";
                    QString ccfFilenameBin="";
                    QStringList dccfFilenameBin;
                    QString bleachAmplitudeFilename="";
                    QString bleachTimeFilename="";
                    QString bleachAmplitude2Filename="";
                    QString bleachTime2Filename="";
                    QString bleachFitSuccessFilename="";
                    QString bleachPoly2Filename="";
                    QString bleachPoly3Filename="";
                    QString bleachPoly4Filename="";
                    QString bleachPoly5Filename="";
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
                        reader->close();




                        ////////////////////////////////////////////////////////////////////////////////////////////
                        // SAVE THE RESULTS
                        ////////////////////////////////////////////////////////////////////////////////////////////

                        //************** SAVE OVERVIEW IMAGE
                        if ((m_status==1) && !was_canceled && average_frame) {
                            emit messageChanged(tr("saving overview image ..."));
                            QString localFilename=averageFilename=outputFilenameBase+".overview.tif";
                            QString error="";
                            if (!SaveTIFFUInt16_scaled(localFilename, average_frame, frame_width, frame_height, tr("UInt16 overview image"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                averageFilename="";
                                emit messageChanged(error);
                            }

                            localFilename=averageFilenameF=outputFilenameBase+".overview_float.tif";
                            if (!SaveTIFFFloat(localFilename, average_frame, frame_width, frame_height, tr("float overview image"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                averageFilenameF="";
                                emit messageChanged(error);
                            }

                        }
                        emit progressIncrement(5);

                        //************** SAVE OVERVIEWSTD IMAGE
                        if ((m_status==1) && !was_canceled && sqrsum_frame) {
                            emit messageChanged(tr("saving overview stddev image ..."));
                            QString localFilename=stdFilename=outputFilenameBase+".overviewstd.tif";

                            QString error="";
                            if (!SaveSDTIFF(localFilename, average_frame, sqrsum_frame, frame_width, frame_height, frames, tr("stddev overview image"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                stdFilename="";
                                emit messageChanged(error);
                            }
                        }
                        emit progressIncrement(5);

                        //************** SAVE UNCORRECTED OVERVIEW IMAGE
                        if ((m_status==1) && !was_canceled && average_uncorrected_frame) {
                            emit messageChanged(tr("saving uncorrected overview image ..."));
                            QString localFilename=averageUncorrectedFilename=outputFilenameBase+".overview_uncorrected.tif";

                            QString error="";
                            if (!SaveTIFFFloat(localFilename, average_uncorrected_frame, frame_width, frame_height, tr("uncorrected overview image"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                averageUncorrectedFilename="";
                                emit messageChanged(error);
                            }

                        }
                        emit progressIncrement(5);

                        //************** SAVE UNCORRECTED OVERVIEWSTD IMAGE
                        if ((m_status==1) && !was_canceled && sqrsum_uncorrected_frame && average_uncorrected_frame) {
                            emit messageChanged(tr("saving uncorrected overview stddev image ..."));
                            QString localFilename=stdUncorrectedFilename=outputFilenameBase+".overviewstd_uncorrected.tif";

                            QString error="";
                            if (!SaveSDTIFF(localFilename, average_uncorrected_frame, sqrsum_uncorrected_frame, frame_width, frame_height, frames, tr("uncorrected overview stddev image"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                stdUncorrectedFilename="";
                                emit messageChanged(error);
                            }
                        }
                        emit progressIncrement(5);

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

                                QString error="";
                                if (!SaveTIFFFloat(localFilename, backgroundImage, frame_width, frame_height, tr("background image"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    backgroundFilename="";
                                    emit messageChanged(error);
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

                                QString error="";
                                if (!SaveTIFFFloat(localFilename, backgroundImageStd, frame_width, frame_height, tr("background S.D. image"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    backstdFilename="";
                                    emit messageChanged(error);
                                }
                            }
                        }
                        emit progressIncrement(10);


                        //************** SAVE BLEACHING PARAMETERS IMAGE
                        if ((m_status==1) && !was_canceled ) {
                            if (job.bleach==BLEACH_EXP || job.bleach==BLEACH_EXP_POLY2 || job.bleach==BLEACH_EXP_POLY3  || job.bleach==BLEACH_EXP_POLY4 || job.bleach==BLEACH_EXP_POLY5  || job.bleach==BLEACH_DBL_EXP || job.bleach==BLEACH_EXPREG) {
                                emit messageChanged(tr("saving bleach parameter images ..."));
                                QString localFilename;
                                TIFF* tif;
                                bool error=false;

                                localFilename=bleachAmplitudeFilename=outputFilenameBase+".bleachamplitude.tif";
                                tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, bleachAmplitude, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=bleachTimeFilename=outputFilenameBase+".bleachtime.tif";
                                tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, bleachTime, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                if (job.bleach==BLEACH_DBL_EXP) {
                                    localFilename=bleachAmplitude2Filename=outputFilenameBase+".bleachamplitude2.tif";
                                    tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                    if (tif) {
                                        TIFFTWriteFloat(tif, bleachAmplitude2, frame_width, frame_height);
                                        TIFFClose(tif);
                                    } else error=true;

                                    localFilename=bleachTime2Filename=outputFilenameBase+".bleachtime2.tif";
                                    tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                    if (tif) {
                                        TIFFTWriteFloat(tif, bleachTime2, frame_width, frame_height);
                                        TIFFClose(tif);
                                    } else error=true;

                                }

                                if (job.bleach==BLEACH_EXP_POLY2||job.bleach==BLEACH_EXP_POLY3||job.bleach==BLEACH_EXP_POLY4||job.bleach==BLEACH_EXP_POLY5 ) {
                                    localFilename=bleachPoly2Filename=outputFilenameBase+".bleachpoly2.tif";
                                    tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                    if (tif) {
                                        TIFFTWriteFloat(tif, bleachPoly2, frame_width, frame_height);
                                        TIFFClose(tif);
                                    } else error=true;

                                }
                                if (job.bleach==BLEACH_EXP_POLY3||job.bleach==BLEACH_EXP_POLY4||job.bleach==BLEACH_EXP_POLY5 ) {
                                    localFilename=bleachPoly3Filename=outputFilenameBase+".bleachpoly3.tif";
                                    tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                    if (tif) {
                                        TIFFTWriteFloat(tif, bleachPoly3, frame_width, frame_height);
                                        TIFFClose(tif);
                                    } else error=true;
                                }
                                if (job.bleach==BLEACH_EXP_POLY4||job.bleach==BLEACH_EXP_POLY5 ) {
                                    localFilename=bleachPoly4Filename=outputFilenameBase+".bleachpoly4.tif";
                                    tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                    if (tif) {
                                        TIFFTWriteFloat(tif, bleachPoly4, frame_width, frame_height);
                                        TIFFClose(tif);
                                    } else error=true;
                                }
                                if (job.bleach==BLEACH_EXP_POLY5 ) {
                                    localFilename=bleachPoly5Filename=outputFilenameBase+".bleachpoly5.tif";
                                    tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                    if (tif) {
                                        TIFFTWriteFloat(tif, bleachPoly5, frame_width, frame_height);
                                        TIFFClose(tif);
                                    } else error=true;
                                }
                                localFilename=bleachFitSuccessFilename=outputFilenameBase+".bleachfitok.tif";
                                tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                if (tif) {
                                    TIFFTWriteUint8(tif, bleachFitOK, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=firstFramesFilename=outputFilenameBase+".firstframes.tif";
                                tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                                if (tif) {
                                    TIFFTWriteFloat(tif, firstFrames, frame_width, frame_height);
                                    TIFFClose(tif);
                                } else error=true;

                                localFilename=lastFramesFilename=outputFilenameBase+".lastframes.tif";
                                tif = TIFFOpen(localFilename.toLatin1().data(),"w");
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
                                    tif = TIFFOpen(localFilename.toLatin1().data(),"w");
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
                            TinyTIFFFile* tif=TinyTIFFWriter_open(localFilename.toLatin1().data(), 16, frame_width, frame_height);
                            //TIFF* tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                            if (tif) {
                                float avgMin=video[0];
                                float avgMax=video[0];
                                for (register uint32_t i=0; i<frame_width*frame_height*video_count; i++) {
                                    avgMin=(video[i]<avgMin)?video[i]:avgMin;
                                    avgMax=(video[i]>avgMax)?video[i]:avgMax;
                                }
                                uint16_t* img=(uint16_t*)qfMalloc(frame_width*frame_height*sizeof(uint16_t));
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
                                qfFree(img);
                                TinyTIFFWriter_close(tif);
                                videoAvgMin = avgMin;
                                videoAvgMax = avgMax;
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create video '%1'!").arg(localFilename));
                            }

                        } else emit progressIncrement(100);

                        double videoUncorrectedAvgMin=0;
                        double videoUncorrectedAvgMax=(float)0xFFFF;
                        //************** SAVE VIDEO
                        if ((m_status==1) && !was_canceled && video_uncorrected && real_video_count>0 ) {
                            emit messageChanged(tr("saving uncorrected video ..."));
                            QString localFilename=videoUncorrectedFilename=outputFilenameBase+".videouncorr.tif";
                            TinyTIFFFile* tif=TinyTIFFWriter_open(localFilename.toLatin1().data(), 16, frame_width, frame_height);
                            //TIFF* tif = TIFFOpen(localFilename.toLatin1().data(),"w");
                            if (tif) {
                                float avgMin=video_uncorrected[0];
                                float avgMax=video_uncorrected[0];
                                for (register uint32_t i=0; i<frame_width*frame_height*video_count; i++) {
                                    avgMin=(video_uncorrected[i]<avgMin)?video_uncorrected[i]:avgMin;
                                    avgMax=(video_uncorrected[i]>avgMax)?video_uncorrected[i]:avgMax;
                                }
                                uint16_t* img=(uint16_t*)qfMalloc(frame_width*frame_height*sizeof(uint16_t));
                                for (register uint32_t c=0; c<real_video_count; c++) {
                                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                                        img[i]=(uint16_t)round((float)(video_uncorrected[c*frame_width*frame_height+i]-avgMin)*(float)0xFFFF/fabs(avgMax-avgMin));
                                    }
                                    TinyTIFFWriter_writeImage(tif, img);
                                    if (real_video_count/10>0) {
                                        if (c%(real_video_count/10)==0) {
                                            emit messageChanged(tr("saving uncorrected video %1/%2...").arg(c+1).arg(video_count));
                                            progressIncrement(10);
                                        }
                                    }
                                }
                                if (real_video_count/10<=0) progressIncrement(100);
                                qfFree(img);
                                TinyTIFFWriter_close(tif);
                                videoUncorrectedAvgMin = avgMin;
                                videoUncorrectedAvgMax = avgMax;
                            } else {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create uncorrected video '%1'!").arg(localFilename));
                            }

                        } else emit progressIncrement(100);

                        //************** SAVE STATISTICS
                        if ((m_status==1) && !was_canceled && job.statistics) {
                            QString error="";
                            emit messageChanged(tr("saving statistics ..."));
                            if (!saveStatistics(statisticsFilename=outputFilenameBase+".statistics.dat", outputFilenameBase+".statistics.plt", statistics, tr("Corrected Statistics"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create statistics file %1!").arg(error));
                            }

                            emit messageChanged(tr("saving background statistics ..."));
                            if (!saveStatistics(backstatisticsFilename=outputFilenameBase+".backstatistics.dat", outputFilenameBase+".backstatistics.plt", backstatistics, tr("Background Statistics"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create backgrond statistics file %1!").arg(error));
                            }

                            emit messageChanged(tr("saving uncorrected statistics ..."));
                            if (!saveStatistics(uncorrectedStatisticsFilename=outputFilenameBase+".uncorrstatistics.dat", outputFilenameBase+".uncorrstatistics.plt", statistics_uncorrected, tr("Uncorrected Statistics"), &error)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create uncorrected statistics file %1!").arg(error));
                            }

                            if (job.dualViewMode!=0) {
                                emit messageChanged(tr("saving DV 1 statistics ..."));
                                if (!saveStatistics(statisticsFilename_dv1=outputFilenameBase+".statistics_dv1.dat", outputFilenameBase+".statistics_dv1.plt", dv_statistics[0], tr("Corrected Statistics, DV 1"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    emit messageChanged(tr("could not create DV 1 statistics file %1!").arg(error));
                                }
                                emit messageChanged(tr("saving DV 2 statistics ..."));
                                if (!saveStatistics(statisticsFilename_dv2=outputFilenameBase+".statistics_dv2.dat", outputFilenameBase+".statistics_dv2.plt", dv_statistics[1], tr("Corrected Statistics, DV 2"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    emit messageChanged(tr("could not create DV 2 statistics file %1!").arg(error));
                                }


                                emit messageChanged(tr("saving DV 1 background statistics ..."));
                                if (!saveStatistics(backstatisticsFilename_dv1=outputFilenameBase+".backstatistics_dv1.dat", outputFilenameBase+".backstatistics_dv1.plt", dv_backstatistics[0], tr("Background Statistics, DV 1"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    emit messageChanged(tr("could not create DV 1 background statistics file %1!").arg(error));
                                }
                                emit messageChanged(tr("saving DV 2 background statistics ..."));
                                if (!saveStatistics(backstatisticsFilename_dv2=outputFilenameBase+".backstatistics_dv2.dat", outputFilenameBase+".backstatistics_dv2.plt", dv_backstatistics[1], tr("Background Statistics, DV 2"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    emit messageChanged(tr("could not create DV 2 background statistics file %1!").arg(error));
                                }


                                emit messageChanged(tr("saving DV 1 uncorrected statistics ..."));
                                if (!saveStatistics(uncorrectedStatisticsFilename_dv1=outputFilenameBase+".uncorrstatistics_dv1.dat", outputFilenameBase+".uncorrstatistics_dv1.plt", dv_statistics_uncorrected[0], tr("Background Statistics, DV 1"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    emit messageChanged(tr("could not create DV 1 uncorrected statistics file %1!").arg(error));
                                }
                                emit messageChanged(tr("saving DV 2 uncorrected statistics ..."));
                                if (!saveStatistics(uncorrectedStatisticsFilename_dv2=outputFilenameBase+".uncorrstatistics_dv2.dat", outputFilenameBase+".uncorrstatistics_dv2.plt", dv_statistics_uncorrected[1], tr("Background Statistics, DV 2"), &error)) {
                                    m_status=-1; emit statusChanged(m_status);
                                    emit messageChanged(tr("could not create DV 2 uncorrected statistics file %1!").arg(error));
                                }

                            }

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
                                if (!averageUncorrectedFilename.isEmpty())         text<<"uncorr. overview image file : " << d.relativeFilePath(averageUncorrectedFilename) << "\n";
                                if (!stdUncorrectedFilename.isEmpty())             text<<"uncorr. overview std image  : " << d.relativeFilePath(stdUncorrectedFilename) << "\n";
                                if (!videoFilename.isEmpty())           text<<"video file                  : " << d.relativeFilePath(videoFilename) << "\n";
                                if (!videoUncorrectedFilename.isEmpty())text<<"uncorrected video file      : " << d.relativeFilePath(videoUncorrectedFilename) << "\n";
                                if (!statisticsFilename.isEmpty())      text<<"statistics file             : " << d.relativeFilePath(statisticsFilename) << "\n";
                                if (!backstatisticsFilename.isEmpty())         text<<"background statistics file  : " << d.relativeFilePath(backstatisticsFilename) << "\n";
                                if (!uncorrectedStatisticsFilename.isEmpty())  text<<"uncorrected statistics file : " << d.relativeFilePath(uncorrectedStatisticsFilename) << "\n";

                                if (!statisticsFilename_dv1.isEmpty())      text<<"statistics file DV1         : " << d.relativeFilePath(statisticsFilename_dv1) << "\n";
                                if (!backstatisticsFilename_dv1.isEmpty())         text<<"background statistics file DV1 : " << d.relativeFilePath(backstatisticsFilename_dv1) << "\n";
                                if (!uncorrectedStatisticsFilename_dv1.isEmpty())  text<<"uncorrected statistics file DV1 : " << d.relativeFilePath(uncorrectedStatisticsFilename_dv1) << "\n";
                                if (!statisticsFilename_dv2.isEmpty())      text<<"statistics file DV2         : " << d.relativeFilePath(statisticsFilename_dv2) << "\n";
                                if (!backstatisticsFilename_dv2.isEmpty())         text<<"background statistics file DV2 : " << d.relativeFilePath(backstatisticsFilename_dv2) << "\n";
                                if (!uncorrectedStatisticsFilename_dv2.isEmpty())  text<<"uncorrected statistics file DV2 : " << d.relativeFilePath(uncorrectedStatisticsFilename_dv2) << "\n";

                                if (!acfFilename.isEmpty())             text<<"autocorrelation file        : " << d.relativeFilePath(acfFilename) << "\n";
                                if (!acfFilenameBin.isEmpty())          text<<"bin. autocorrelation file   : " << d.relativeFilePath(acfFilenameBin) << "\n";
                                if (!ccfFilename.isEmpty())             text<<"crosscorrelation file       : " << d.relativeFilePath(ccfFilename) << "\n";
                                if (!ccfFilenameBin.isEmpty())          text<<"bin. crosscorrelation file  : " << d.relativeFilePath(ccfFilenameBin) << "\n";
                                for (int id=0; id<qMax(dccfFilename.size(), dccfFilenameBin.size()); id++) {
                                    if (id<dccfFilename.size())            text<<QString("distance ccf file %1        : ").arg(id) << d.relativeFilePath(dccfFilename[id]) << "\n";
                                    if (id<dccfFilenameBin.size())         text<<QString("bin. distance ccf file %1   : ").arg(id) << d.relativeFilePath(dccfFilenameBin[id]) << "\n";
                                }
                                text<<"width                       : "<<outLocale.toString(frame_width) << "\n";
                                text<<"height                      : "<<outLocale.toString(frame_height) << "\n";
                                if (job.cameraSettingsGiven) {
                                    text<<"pixel width                 : "<<outLocale.toString(job.cameraPixelWidth) << "\n";
                                    text<<"pixel height                : "<<outLocale.toString(job.cameraPixelHeight) << "\n";
                                }
                                text<<"frametime (ms)              : "<<job.frameTime*1.0e3 << "\n";
                                text<<"frametime (s)               : "<<job.frameTime << "\n";
                                text<<"dualview mode               : "<<job.dualViewMode << "\n";
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
                                text<<"correlation segments        : "<<outLocale.toString(double(frames)/double(job.segments)*job.frameTime) << "\n";
                                text<<"segments length (s)         : "<<outLocale.toString(job.segments) << "\n";
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
                                    case CORRELATOR_CORRELATORFROMSHAREDLIB:  text<<"multi-tau from shared library\n"; break;

                                    default: text<<"correlator type name        : unknown\n"; break;
                                }
                                text<<"smallest tau [s]            : "<<outLocale.toString(job.frameTime) << "\n";
                                text<<"baseline                    : "<<outLocale.toString(baseline) << "\n";
                                if (video && job.video) {
                                    text<<"video sum up                : "<<outLocale.toString(job.video_frames) << "\n";
                                    text<<"video frames                : "<<outLocale.toString(real_video_count) << "\n";
                                    text<<"video avgMin                : "<<outLocale.toString(videoAvgMin) << "\n";
                                    text<<"video avgMax                : "<<outLocale.toString(videoAvgMax) << "\n";
                                    text<<"video uncorrected avgMin    : "<<outLocale.toString(videoUncorrectedAvgMin) << "\n";
                                    text<<"video uncorrected avgMax    : "<<outLocale.toString(videoUncorrectedAvgMax) << "\n";
                                }
                                if (job.statistics) {
                                    text<<"statistics over             : "<<outLocale.toString(job.statistics_frames) << "\n";
                                }
                                if (job.distanceCCF) {
                                    for (int id=0; id<job.DCCFDeltaX.size(); id++) {
                                        text<<QString("DCCF %1 role                   : ").arg(id)<<job.DCCFrole[id] << "\n";
                                        text<<QString("DCCF %1 Delta x                : ").arg(id)<<outLocale.toString(job.DCCFDeltaX[id]) << "\n";
                                        text<<QString("DCCF %1 Delta y                : ").arg(id)<<outLocale.toString(job.DCCFDeltaY[id]) << "\n";
                                        text<<QString("DCCF %1 frame width            : ").arg(id)<<outLocale.toString(dccf[id].dccfframe_width) << "\n";
                                        text<<QString("DCCF %1 frame height           : ").arg(id)<<outLocale.toString(dccf[id].dccfframe_height) << "\n";
                                    }

                                }
                                text<<"bleach correction           : ";
                                if (job.bleach==BLEACH_EXP) {
                                    text<<"remove mono-exponential f(t)=A*exp(-t/tau) using LM-Fit\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach time file            : "<<d.relativeFilePath(bleachTimeFilename) << "\n";
                                    text<<"bleach fit success          : "<<d.relativeFilePath(bleachFitSuccessFilename) << "\n";
                                    text<<"bleach timeseries t file    : "<<d.relativeFilePath(bleachtimesFilename) << "\n";
                                    text<<"bleach timeseries I file    : "<<d.relativeFilePath(bleachframesFilename) << "\n";
                                    text<<"bleach correction           : ";
                                } else if (job.bleach==BLEACH_EXP_POLY2 ) {
                                    text<<"remove mono-exponential(polynomial) f(t)=A*exp(-(t+f2*t^2)/tau) using LM-Fit\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach time file            : "<<d.relativeFilePath(bleachTimeFilename) << "\n";
                                    text<<"bleach polynomial factor file : "<<d.relativeFilePath(bleachPoly2Filename) << "\n";
                                    text<<"bleach polynomial factor 3 file : "<<d.relativeFilePath(bleachPoly3Filename) << "\n";
                                    text<<"bleach fit success          : "<<d.relativeFilePath(bleachFitSuccessFilename) << "\n";
                                    text<<"bleach timeseries t file    : "<<d.relativeFilePath(bleachtimesFilename) << "\n";
                                    text<<"bleach timeseries I file    : "<<d.relativeFilePath(bleachframesFilename) << "\n";
                                } else if (job.bleach==BLEACH_EXP_POLY3) {
                                    text<<"remove mono-exponential(polynomial) f(t)=A*exp(-(t+f2*t^2+f3*t^3)/tau) using LM-Fit\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach time file            : "<<d.relativeFilePath(bleachTimeFilename) << "\n";
                                    text<<"bleach polynomial factor file : "<<d.relativeFilePath(bleachPoly2Filename) << "\n";
                                    text<<"bleach polynomial factor 3 file : "<<d.relativeFilePath(bleachPoly3Filename) << "\n";
                                    text<<"bleach fit success          : "<<d.relativeFilePath(bleachFitSuccessFilename) << "\n";
                                    text<<"bleach timeseries t file    : "<<d.relativeFilePath(bleachtimesFilename) << "\n";
                                    text<<"bleach timeseries I file    : "<<d.relativeFilePath(bleachframesFilename) << "\n";
                                } else if (job.bleach==BLEACH_EXP_POLY4) {
                                    text<<"remove mono-exponential(polynomial) f(t)=A*exp(-(t+f*t^2+f3*t^3+f4*t^4)/tau) using LM-Fit\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach time file            : "<<d.relativeFilePath(bleachTimeFilename) << "\n";
                                    text<<"bleach polynomial factor file : "<<d.relativeFilePath(bleachPoly2Filename) << "\n";
                                    text<<"bleach polynomial factor 3 file : "<<d.relativeFilePath(bleachPoly3Filename) << "\n";
                                    text<<"bleach polynomial factor 4 file : "<<d.relativeFilePath(bleachPoly4Filename) << "\n";
                                    text<<"bleach fit success          : "<<d.relativeFilePath(bleachFitSuccessFilename) << "\n";
                                    text<<"bleach timeseries t file    : "<<d.relativeFilePath(bleachtimesFilename) << "\n";
                                    text<<"bleach timeseries I file    : "<<d.relativeFilePath(bleachframesFilename) << "\n";
                                } else if (job.bleach==BLEACH_EXP_POLY5) {
                                    text<<"remove mono-exponential(polynomial) f(t)=A*exp(-(t+f*t^2+f3*t^3+f4*t^4+f5*t^5)/tau) using LM-Fit\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach time file            : "<<d.relativeFilePath(bleachTimeFilename) << "\n";
                                    text<<"bleach polynomial factor file : "<<d.relativeFilePath(bleachPoly2Filename) << "\n";
                                    text<<"bleach polynomial factor 3 file : "<<d.relativeFilePath(bleachPoly3Filename) << "\n";
                                    text<<"bleach polynomial factor 4 file : "<<d.relativeFilePath(bleachPoly4Filename) << "\n";
                                    text<<"bleach polynomial factor 5 file : "<<d.relativeFilePath(bleachPoly5Filename) << "\n";
                                    text<<"bleach fit success          : "<<d.relativeFilePath(bleachFitSuccessFilename) << "\n";
                                    text<<"bleach timeseries t file    : "<<d.relativeFilePath(bleachtimesFilename) << "\n";
                                    text<<"bleach timeseries I file    : "<<d.relativeFilePath(bleachframesFilename) << "\n";
                                } else if (job.bleach==BLEACH_DBL_EXP) {
                                    text<<"remove double-exponential f(t)=A*exp(-t/tau)+A2*exp(-t/tau2) using LM-Fit\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
                                    text<<"bleach amplitude file       : "<<d.relativeFilePath(bleachAmplitudeFilename) << "\n";
                                    text<<"bleach time file            : "<<d.relativeFilePath(bleachTimeFilename) << "\n";
                                    text<<"bleach amplitude 2 file       : "<<d.relativeFilePath(bleachAmplitude2Filename) << "\n";
                                    text<<"bleach time 2 file            : "<<d.relativeFilePath(bleachTime2Filename) << "\n";
                                    text<<"bleach fit success          : "<<d.relativeFilePath(bleachFitSuccessFilename) << "\n";
                                    text<<"bleach timeseries t file    : "<<d.relativeFilePath(bleachtimesFilename) << "\n";
                                    text<<"bleach timeseries I file    : "<<d.relativeFilePath(bleachframesFilename) << "\n";
                                } else if (job.bleach==BLEACH_EXPREG) {
                                    text<<"remove mono-exponential f(t)=A*exp(-t/tau) using linear regression\n";
                                    text<<"bleach average frames       : "<<outLocale.toString(job.bleachAvgFrames) << "\n";
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
                            if (!saveCorrelationCSV(localFilename, acf_tau, ccf, ccferr, 1, acf_N, frame_width, frame_height, input_length, error,125, double(frames)/double(job.segments)*job.frameTime)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create autocorrelation file '%1': %2!").arg(localFilename).arg(error));
                            }
                            if (!saveCorrelationBIN(localFilename1, acf_tau, ccf, ccferr, 1, acf_N, frame_width, frame_height, ccfsegments, error,125, double(frames)/double(job.segments)*job.frameTime)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create binary autocorrelation file '%1': %2!").arg(localFilename1).arg(error));
                            }
                            if (job.addFCCSSeparately && job.dualViewMode!=0) {
                                if (QFile::exists(localFilename1)) {
                                    addFiles.append(getFileInfo(localFilename1, configFilename, "ACF0", 0, getGroupName()));
                                    addFiles.append(getFileInfo(localFilename1, configFilename, "ACF1", 1, getGroupName()));
                                } else {
                                    addFiles.append(getFileInfo(localFilename, configFilename, "ACF0", 0, getGroupName()));
                                    addFiles.append(getFileInfo(localFilename, configFilename, "ACF1", 1, getGroupName()));
                                }
                            } else {
                                if (QFile::exists(localFilename1)) addFiles.append(getFileInfo(localFilename1, configFilename, "ACF", getGroupName()));
                                else addFiles.append(getFileInfo(localFilename, configFilename, "ACF", getGroupName()));
                            }

                        }
                        emit progressIncrement(250);


                        //************** SAVE DCCF
                        if ((m_status==1) && !was_canceled && job.distanceCCF) {
                            for (int id=0; id<job.DCCFDeltaX.size(); id++) {
                                if (dccf[id].dccf && dccf[id].dccf_tau && dccf[id].dccf_N>0) {
                                    QString localFilename=outputFilenameBase+QString(".dccf%1.dat").arg(id,3,10,QLatin1Char('0'));
                                    QString localFilename1=outputFilenameBase+QString(".dccf%1.bin").arg(id,3,10,QLatin1Char('0'));;
                                    dccfFilename.append(localFilename);
                                    dccfFilenameBin.append(localFilename1);

                                    emit messageChanged(tr("saving distance distance crosscorrelation ..."));
                                    double* ccf[1]={dccf[id].dccf};
                                    double* ccferr[1]={dccf[id].dccf_std};
                                    double* ccfsegments[1]={dccf[id].dccf_segments};
                                    QString error;
                                    if (!saveCorrelationCSV(localFilename, dccf[id].dccf_tau, ccf, ccferr, 1, dccf[id].dccf_N, frame_width, frame_height, input_length, error,125, double(frames)/double(job.segments)*job.frameTime)) {
                                        m_status=-1; emit statusChanged(m_status);
                                        emit messageChanged(tr("could not create distance crosscorrelation file '%1': %2!").arg(localFilename).arg(error));
                                    }
                                    if (!saveCorrelationBIN(localFilename1, dccf[id].dccf_tau, ccf, ccferr, 1, dccf[id].dccf_N, frame_width, frame_height, ccfsegments, error,125, double(frames)/double(job.segments)*job.frameTime)) {
                                        m_status=-1; emit statusChanged(m_status);
                                        emit messageChanged(tr("could not create binary distance crosscorrelation file '%1': %2!").arg(localFilename1).arg(error));
                                    }
                                    QString role=job.DCCFrole.value(id, "DCCF");
                                    if (role.toLower()=="fccs") {
                                        if (QFile::exists(localFilename1)) addFiles.append(getFileInfo(localFilename1, configFilename, role, 0, getGroupName()));
                                        else addFiles.append(getFileInfo(localFilename, configFilename, role, 0, getGroupName()));
                                    } else {
                                        if (QFile::exists(localFilename1)) addFiles.append(getFileInfo(localFilename1, configFilename, role, getGroupName()));
                                        else addFiles.append(getFileInfo(localFilename, configFilename, role, getGroupName()));
                                    }
                                }
                            }
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
                            if (!saveCorrelationCSV(localFilename, ccf_tau, ccf, ccferr, 4, ccf_N, frame_width, frame_height, input_length, error, 0, double(frames)/double(job.segments)*job.frameTime)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create crosscorrelation file '%1': %2!").arg(localFilename).arg(error));
                            }
                            if (!saveCorrelationBIN(localFilename1, ccf_tau, ccf, ccferr, 4, ccf_N, frame_width, frame_height, ccfsegments, error, 0, double(frames)/double(job.segments)*job.frameTime)) {
                                m_status=-1; emit statusChanged(m_status);
                                emit messageChanged(tr("could not create binary crosscorrelation file '%1': %2!").arg(localFilename1).arg(error));
                            }
                            if (QFile::exists(localFilename1)) addFiles.append(getFileInfo(localFilename1, configFilename, QString("CCF"), getGroupName()));
                            else addFiles.append(getFileInfo((localFilename), configFilename, "CCF", getGroupName()));

                        }
                        emit progressIncrement(10);



                        //qDebug()<<job.addNandB<< QFile::exists(averageFilenameF)<< QFile::exists(stdFilename)<< QFile::exists(backgroundFilename)<< QFile::exists(backstdFilename);

                        //************** CREATE N&B job
                        if (job.addNandB
                                && QFile::exists(averageFilenameF)
                                && QFile::exists(stdFilename)
                                /*&& QFile::exists(backgroundFilename)
                                && QFile::exists(backstdFilename)*/) {

                            if (job.addFCCSSeparately && job.dualViewMode!=0) {
                                addFiles.append(getFileInfoNandB(averageFilenameF, stdFilename, backgroundFilename, backstdFilename, configFilename, "N&B1", 0, getGroupName()));
                                addFiles.append(getFileInfoNandB(averageFilenameF, stdFilename, backgroundFilename, backstdFilename, configFilename, "N&B2", 1, getGroupName()));
                            } else {
                                addFiles.append(getFileInfoNandB(averageFilenameF, stdFilename, backgroundFilename, backstdFilename, configFilename, "N&B", 0, getGroupName()));
                            }
                        }
                    } else {
                        if (mutexFilename) mutexFilename->unlock();
                        m_status=-1; emit statusChanged(m_status);
                        emit messageChanged(tr("could not create output subdirectory '%1' in '%2'!").arg(localFileDirectory).arg(d.absolutePath()));
                    }

                    if (video) qfFree(video);
                    if (video_uncorrected) qfFree(video_uncorrected);
                    if (fit_frames) qfFree(fit_frames);
                    if (fit_t) qfFree(fit_t);
                    if (bleachAmplitude) qfFree(bleachAmplitude);
                    if (bleachTime) qfFree(bleachTime);
                    if (bleachAmplitude2) qfFree(bleachAmplitude2);
                    if (bleachTime2) qfFree(bleachTime2);
                    if (bleachPoly2) qfFree(bleachPoly2);
                    if (bleachPoly3) qfFree(bleachPoly3);
                    if (bleachPoly4) qfFree(bleachPoly4);
                    if (bleachPoly5) qfFree(bleachPoly5);
                    if (bleachFitOK) qfFree(bleachFitOK);
                    if (average_frame) qfFree(average_frame);
                    if (sqrsum_frame) qfFree(sqrsum_frame);
                    if (acf_tau) qfFree(acf_tau);
                    if (acf) qfFree(acf);
                    if (acf_segments) qfFree(acf_segments);
                    if (acf_std) qfFree(acf_std);
                    if (ccf_tau) qfFree(ccf_tau);
                    if (ccf1) qfFree(ccf1);
                    if (ccf2) qfFree(ccf2);
                    if (ccf3) qfFree(ccf3);
                    if (ccf4) qfFree(ccf4);
                    if (ccf1_std) qfFree(ccf1_std);
                    if (ccf2_std) qfFree(ccf2_std);
                    if (ccf3_std) qfFree(ccf3_std);
                    if (ccf4_std) qfFree(ccf4_std);
                    if (ccf1_segments) qfFree(ccf1_segments);
                    if (ccf2_segments) qfFree(ccf2_segments);
                    if (ccf3_segments) qfFree(ccf3_segments);
                    if (ccf4_segments) qfFree(ccf4_segments);
                    for (int id=0; id<dccf.size(); id++) {
                        if (dccf[id].dccf_tau) qfFree(dccf[id].dccf_tau);
                        if (dccf[id].dccf) qfFree(dccf[id].dccf);
                        if (dccf[id].dccf_std) qfFree(dccf[id].dccf_std);
                        if (dccf[id].dccf_segments) qfFree(dccf[id].dccf_segments);
                    }
                    dccf.clear();
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


bool QFRDRImagingFCSCorrelationJobThread::saveCorrelationCSV(const QString &filename, double *corrTau, double **corrs, double** correrrs, uint32_t corrN, uint32_t N_in, uint32_t width, uint32_t height, double input_length, QString& error, int progress_steps, double segmentLengthSeconds) {
   /* qDebug()<<corrTau<<corrs<<correrrs<<corrN<<N<<width<<height<<input_length;
    for (uint32_t j=0; j<corrN; j++) {
        qDebug()<<"corrs["<<j<<"] = "<<corrs[j];
    }*/

    uint32_t N=N_in;
    if (corrTau && segmentLengthSeconds>0.0) {
        for (int i=N_in-1; i>=0; i--) {
            if (corrTau[i]>segmentLengthSeconds) N--;
        }
        if (N<=5) corrN=N_in;
    }
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
                       text<<", "<<(corrs[j])[p*N_in+i]-1.0;
                    }
                    if (correrrs && correrrs[0]) {
                        for (uint32_t j=0; j<corrN; j++) {
                           text<<", "<<(correrrs[j])[p*N_in+i]-1.0;
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

bool QFRDRImagingFCSCorrelationJobThread::saveCorrelationBIN(const QString &filename, double *corrTau, double **corrs, double** correrrs, uint32_t corrN, uint32_t N_in, uint32_t width, uint32_t height, double **corrSegments, QString& error, int progress_steps, double segmentLengthSeconds) {
    QFile f(filename);

    uint32_t N=N_in;
    if (corrTau && segmentLengthSeconds>0.0) {
        for (int i=N_in-1; i>=0; i--) {
            if (corrTau[i]>segmentLengthSeconds) N--;
        }
        if (N<=5) corrN=N_in;
    }
    int dataToWrite=width*height*(job.segments+1);
    int progressEvery=0;
    if (progress_steps>0) progressEvery=dataToWrite/progress_steps;
    int progress=0;
    int progem=0;
    bool ok=true;
    if (f.open(QIODevice::WriteOnly)) {
        f.write("QF3.1imFCS"); // write header
        binfileWriteUint32(f, width);
        binfileWriteUint32(f, height);
        binfileWriteUint32(f, corrN);
        binfileWriteUint32(f, N);
        uint32_t sets=(correrrs==NULL || job.segments<=1)?1:2;
        binfileWriteUint32(f, sets);

        binfileWriteDoubleArray(f, corrTau, N);

        for (register uint32_t p=0; p<width*height; p++) {
            for (uint32_t j=0; j<corrN; j++) {
                binfileWriteDoubleArrayMinus1(f, &((corrs[j])[p*N_in]), N);

            }
            if (sets>1) {
                for (uint32_t j=0; j<corrN; j++) {
                    binfileWriteDoubleArray(f, &((correrrs[j])[p*N_in]), N);

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
                        binfileWriteDoubleArrayMinus1(f, &((corrSegments[j])[seg*width*height*N_in+p*N_in]), N);

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
    double* ccf=(double*)qfCalloc(ccf_N*frame_width*frame_height,sizeof(double));
    double* ccf_std=NULL;
    double* ccf_tau=(double*)qfCalloc(ccf_N,sizeof(double));
    long* ccf_t=(long*)qfCalloc(ccf_N,sizeof(long));
    if (ccf_segments_io && job.segments>1) *ccf_segments_io=(double*)qfCalloc(ccf_N*frame_width*frame_height*job.segments,sizeof(double));

    statisticsAutocorrelateCreateMultiTau(ccf_t, job.S, job.m, job.P);

    for (uint32_t nn=0; nn<ccf_N*frame_width*frame_height; nn++) {
        ccf[nn]=1.0;
    }
    if (job.segments>1) {
        ccf_std=(double*)qfCalloc(ccf_N*frame_width*frame_height,sizeof(double));
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
                double* cftemp=(double*)qfCalloc(ccf_N,sizeof(double));
                double* sum=(double*)qfCalloc(ccf_N,sizeof(double));
                double* sum2=(double*)qfCalloc(ccf_N,sizeof(double));
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
                qfFree(cftemp);
                double segs=job.segments;
                for (register uint32_t ct=0; ct<ccf_N; ct++) {
                    ccf[p*ccf_N+ct]=sum[0*ccf_N+ct]/segs;
                    ccf_std[p*ccf_N+ct]=sqrt((sum2[0*ccf_N+ct]-sum[0*ccf_N+ct]*sum[0*ccf_N+ct]/segs)/(segs-1.0));
                }
                qfFree(sum);
                qfFree(sum2);
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
    qfFree(ccf_t);


    *ccf_tau_io=ccf_tau;
    *ccf_io=ccf;
    *ccf_std_io=ccf_std;

}

inline double QFRDRImagingFCSCorrelationJobThread_bleachCorrect(const double& intensity, const double& expT, const double& exp0) {
    return intensity/sqrt(expT/exp0)+exp0*(1.0-sqrt(expT/exp0));
}

double QFRDRImagingFCSCorrelationJobThread::bleachCorrectExp(const double& intensity, int i, double t) {
    double expT=bleachAmplitude[i]*exp(-(double)t/bleachTime[i]);
    double exp0=bleachAmplitude[i];
    return QFRDRImagingFCSCorrelationJobThread_bleachCorrect(intensity, expT, exp0);
}

double QFRDRImagingFCSCorrelationJobThread::bleachCorrectExpPoly2(const double& intensity, int i, double t) {
    double expT=bleachAmplitude[i]*exp(-((double)t+bleachPoly2[i]*qfSqr((double)t))/bleachTime[i]);
    double exp0=bleachAmplitude[i];
    return QFRDRImagingFCSCorrelationJobThread_bleachCorrect(intensity, expT, exp0);
}

double QFRDRImagingFCSCorrelationJobThread::bleachCorrectExpPoly3(const double& intensity, int i, double t) {
    double expT=bleachAmplitude[i]*exp(-((double)t+bleachPoly2[i]*qfSqr((double)t)+bleachPoly3[i]*qfCube((double)t))/bleachTime[i]);
    double exp0=bleachAmplitude[i];
    return QFRDRImagingFCSCorrelationJobThread_bleachCorrect(intensity, expT, exp0);
}

double QFRDRImagingFCSCorrelationJobThread::bleachCorrectExpPoly4(const double& intensity, int i, double t) {
    double expT=bleachAmplitude[i]*exp(-((double)t+bleachPoly2[i]*qfSqr((double)t)+bleachPoly3[i]*qfCube((double)t)+bleachPoly4[i]*qfPow4((double)t))/bleachTime[i]);
    double exp0=bleachAmplitude[i];
    return QFRDRImagingFCSCorrelationJobThread_bleachCorrect(intensity, expT, exp0);
}

double QFRDRImagingFCSCorrelationJobThread::bleachCorrectExpPoly5(const double& intensity, int i, double t) {
    double expT=bleachAmplitude[i]*exp(-((double)t+bleachPoly2[i]*qfSqr((double)t)+bleachPoly3[i]*qfCube((double)t)+bleachPoly4[i]*qfPow4((double)t)+bleachPoly5[i]*qfPow5((double)t))/bleachTime[i]);
    double exp0=bleachAmplitude[i];
    return QFRDRImagingFCSCorrelationJobThread_bleachCorrect(intensity, expT, exp0);
}

double QFRDRImagingFCSCorrelationJobThread::bleachCorrectDblExp(const double& intensity, int i, double t) {
    double expT=bleachAmplitude[i]*exp(-(double)t/bleachTime[i])+bleachAmplitude2[i]*exp(-(double)t/bleachTime2[i]);
    double exp0=bleachAmplitude[i]+bleachAmplitude2[i];
    return QFRDRImagingFCSCorrelationJobThread_bleachCorrect(intensity, expT, exp0);
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
        image_series=(float*)qfCalloc((frames+2)*frame_width*frame_height,sizeof(float));
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
    firstFrames=(float*)qfCalloc(frame_width*frame_height,sizeof(float));
    lastFrames=(float*)qfCalloc(frame_width*frame_height,sizeof(float));

    if (!was_canceled) {
        emit messageChanged(tr("reading frames ..."));

        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected(frame_width*frame_height);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected_dv0(frame_width*frame_height/2);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected_dv1(frame_width*frame_height/2);
        statistics_uncorrected.clear();
        dv_statistics_uncorrected[0].clear();
        dv_statistics_uncorrected[1].clear();
        uint32_t video_frame_num=0;
        float fframes_min=0;
        float fframes_max=0;

        register uint32_t frame=0;
        //float sframe_min=0;
        //float sframe_max=0;
        float* frame_data=(float*)qfCalloc(frame_width*frame_height,sizeof(float));
        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
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
                    contribute_to_statistics(stat_state_uncorrected, frame_data, frame_width, frame_height, frame, frames, &average_uncorrected_frame, &sqrsum_uncorrected_frame, &video_uncorrected, video_frame_num, fframes_min, fframes_max, statistics_uncorrected);
                    contribute_to_dv2_statistics(stat_state_uncorrected_dv0, stat_state_uncorrected_dv1, frame_data, frame_width, frame_height, frame, frames, dv_statistics_uncorrected[0], dv_statistics_uncorrected[1]);

                    float frame_min=frame_data[0];
                    float frame_max=frame_data[0];

                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
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
                        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                            firstFrames[i]=firstFrames[i]+(float)frame_data[i]/(float)job.bleachAvgFrames;
                        }
                    } else if (frame>frames-job.bleachAvgFrames) {
                        for (register uint32_t i=0; i<frame_width*frame_height; i++) {
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
        qfFree(frame_data);
    }

    if (m_status==1 && !was_canceled) {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // NOW WE CORRECT THE IMAGE FOR IT'S BASELINE (ACCORDING TO THE USER SETTINGS)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        calcBackgroundCorrection();
        NFitFrames=qMin((int64_t)300, (int64_t)frames);
        fit_t=(double*)qfCalloc(NFitFrames,sizeof(double));
        fit_frames=(float*)qfCalloc(NFitFrames*frame_width*frame_height,sizeof(float));
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
                if (iidx>=(int64_t)frames) iidx=frames-1;
                fit_t[i]=(double)iidx+(double)job.bleachAvgFrames/2.0;
                //qDebug()<<i<<"/"<<NFitFrames<<":    "<<iidx<<"/"<<frames;
                for (uint64_t idx=0; idx<frame_width*frame_height; idx++) {
                    fit_frames[i*frame_width*frame_height+idx]=image_series[iidx*frame_width*frame_height+idx];
                }
                if (job.bleachAvgFrames>1) {
                    float sum=1;
                    for (uint64_t f=1; f<job.bleachAvgFrames; f++) {
                        if (iidx+(int64_t)f<(int64_t)frames) {
                            for (uint64_t idx=0; idx<frame_width*frame_height; idx++) {
                                fit_frames[i*frame_width*frame_height+idx]+=image_series[(iidx+f)*frame_width*frame_height+idx];
                            }
                            sum++;
                        }
                    }
                    for (uint64_t idx=0; idx<frame_width*frame_height; idx++) {
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
            if (job.bleach==BLEACH_EXP || job.bleach==BLEACH_EXPREG) {
                for (register uint64_t t=0; t<frames; t++) {
                    for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                        uint64_t idx=t*frame_width*frame_height+i;
                        if (bleachFitOK[i]!=0) {
                            image_series[idx]=bleachCorrectExp(image_series[idx], i, t);
                        }
                    }
                }
            } else if (job.bleach==BLEACH_EXP_POLY2) {
                    for (register uint64_t t=0; t<frames; t++) {
                        for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                            uint64_t idx=t*frame_width*frame_height+i;
                            if (bleachFitOK[i]!=0) {
                                image_series[idx]=bleachCorrectExpPoly2(image_series[idx], i, t);
                            }
                        }
                    }
            } else if (job.bleach==BLEACH_EXP_POLY3) {
                    for (register uint64_t t=0; t<frames; t++) {
                        for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                            uint64_t idx=t*frame_width*frame_height+i;
                            if (bleachFitOK[i]!=0) {
                                image_series[idx]=bleachCorrectExpPoly3(image_series[idx], i, t);
                            }
                        }
                    }
            } else if (job.bleach==BLEACH_EXP_POLY4) {
                    for (register uint64_t t=0; t<frames; t++) {
                        for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                            uint64_t idx=t*frame_width*frame_height+i;
                            if (bleachFitOK[i]!=0) {
                                image_series[idx]=bleachCorrectExpPoly4(image_series[idx], i, t);
                            }
                        }
                    }
            } else if (job.bleach==BLEACH_EXP_POLY5) {
                    for (register uint64_t t=0; t<frames; t++) {
                        for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                            uint64_t idx=t*frame_width*frame_height+i;
                            if (bleachFitOK[i]!=0) {
                                image_series[idx]=bleachCorrectExpPoly5(image_series[idx], i, t);
                            }
                        }
                    }
            } else if (job.bleach==BLEACH_DBL_EXP) {
                    for (register uint64_t t=0; t<frames; t++) {
                        for (register uint64_t i=0; i<frame_width*frame_height; i++) {
                            uint64_t idx=t*frame_width*frame_height+i;
                            if (bleachFitOK[i]!=0) {
                                image_series[idx]=bleachCorrectDblExp(image_series[idx], i, t);
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
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state(frame_width*frame_height);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_dv1(frame_width*frame_height/2);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_dv2(frame_width*frame_height/2);
        statistics.clear();
        dv_statistics[0].clear();
        dv_statistics[1].clear();
        uint32_t video_frame_num=0;

        //bool statFirst=true;
        for(register uint32_t frame=0; frame<frames; frame++) {
            float* frame_data=&(image_series[frame*frame_width*frame_height]);
            contribute_to_statistics(stat_state, frame_data, frame_width, frame_height, frame, frames, &average_frame, &sqrsum_frame, &video, video_frame_num, frames_min, frames_max, statistics);
            contribute_to_dv2_statistics(stat_state_dv1, stat_state_dv2, frame_data, frame_width, frame_height, frame, frames, dv_statistics[0], dv_statistics[1]);

            if (frames<500) {
                emit messageChanged(tr("calculating statistics/video (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(500/frames);
            } else if ((frames/500==0) || (frame%(frames/500)==0)) {
                emit messageChanged(tr("calculating statistics/video (%1/%2)...").arg(frame).arg(frames)); emit progressIncrement(2);
            }

            real_video_count=video_frame_num;


            if (was_canceled) break;
        }



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

            for (int di=0; di<job.DCCFDeltaX.size(); di++) {


                dccf[di].dccfframe_width=frame_width;//-abs(job.DCCFDeltaX);
                dccf[di].dccfframe_height=frame_height;//-abs(job.DCCFDeltaY);

                correlate_series(image_series, frame_width, frame_height, job.DCCFDeltaX[di], job.DCCFDeltaY[di], frames, &(dccf[di].dccf_tau), &(dccf[di].dccf), &(dccf[di].dccf_std), dccf[di].dccf_N, tr("calculating distance crosscorrelation (%1,%2)").arg(job.DCCFDeltaX[di]).arg(job.DCCFDeltaY[di]), 1000, &(dccf[di].dccf_segments));
            }

        } else {
            emit progressIncrement(1000*job.DCCFDeltaX.size());
        }
    }



    if (firstFrames) qfFree(firstFrames);
    if (lastFrames) qfFree(lastFrames);
    if (image_series) qfFree(image_series);
    image_series=NULL;
}




void QFRDRImagingFCSCorrelationJobThread::contribute_to_correlations(QList<MultiTauCorrelator<double, double>* >& ccfjk, QList<correlatorjb<double, double>* >& ccfjb, float* frame_data, uint32_t frame_width, uint32_t frame_height, uint32_t shiftX, uint32_t shiftY, uint64_t frame, uint64_t segment_frames, double *ccf_tau, double *ccf, double *ccf_std, uint64_t ccf_N) {
    if (job.correlator==CORRELATOR_MTAUALLMON) {
        uint32_t i=0;
        for (register uint32_t y=0; y<frame_height; y++) {
            for (register uint32_t x=0; x<frame_width; x++) {

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
        uint32_t i=0;
        for (register uint32_t y=0; y<frame_height; y++) {
            for (register uint32_t x=0; x<frame_width; x++) {

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
                        qfFree(corr1[0]);
                        qfFree(corr1[1]);
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
        qfFree(*acf_std);
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
        (*acf)=(double*)qfCalloc(acf_N*frame_width*frame_height,sizeof(double));
        for (uint64_t nn=0; nn<acf_N*frame_width*frame_height; nn++) {
            (*acf)[nn]=1.0;
        }
        (*acf_std)=(double*)qfCalloc(acf_N*frame_width*frame_height,sizeof(double));
        for (uint64_t nn=0; nn<acf_N*frame_width*frame_height; nn++) {
            (*acf_std)[nn]=1.0;
        }
        (*acf_t)=(double*)qfCalloc(acf_N,sizeof(double));
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
            qfFree(corr1[0]);
            qfFree(corr1[1]);
        }
    }
}



void QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics(QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state &state, float *frame_data, uint32_t frame_width, uint32_t frame_height, uint32_t frame, uint32_t frames, float **average_frame, float **sqrsum_frame, float **video, uint32_t &video_frame_num, float &frames_min, float &frames_max, QFRDRImagingFCSCorrelationJobThread::StatisticsDataset &statistics, bool isBackground)
{

    float frame_min=frame_data[0];
    float frame_max=frame_data[0];
    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
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
        statistics.time.append((float)frame*job.frameTime);
        statistics.mean.append(state.sum/N);
        statistics.min.append(state.sframe_min);
        statistics.max.append(state.sframe_max);
        if (!isBackground && job.statistics_frames>1)         statistics.std.append(sqrt((state.sum2-state.sum*state.sum/N)/(N-1.0)));
        else if (isBackground && job.backstatistics_frames>1) statistics.std.append(sqrt((state.sum2-state.sum*state.sum/N)/(N-1.0)));
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

void QFRDRImagingFCSCorrelationJobThread::contribute_to_dv2_statistics(QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state &state1, QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state &state2, float *frame_data, uint32_t frame_width, uint32_t frame_height, uint32_t frame, uint32_t frames, QFRDRImagingFCSCorrelationJobThread::StatisticsDataset &statistics1, QFRDRImagingFCSCorrelationJobThread::StatisticsDataset &statistics2, bool isBackground)
{
    uint32_t dummyUI32;
    float dummyF;
    int dv_width=frame_width;
    int dv_height=frame_height;
    int shiftX1=0;
    int shiftY1=0;
    if (job.dualViewMode==1) {
        dv_width=dv_width/2;
        shiftX1=dv_width/2;
    } else if (job.dualViewMode==2) {
        dv_height=dv_height/2;
        shiftY1=dv_height/2;
    } else {
        //qDebug()<<"contribute_to_dv2_statistics job.dualViewMode="<<job.dualViewMode;
        return;
    }
    //qDebug()<<"contribute_to_dv2_statistics "<<dv_width<<"x"<<dv_height<<"   shift=("<<shiftX1<<","<<shiftY1<<")";
    float* frame_data1=(float*)qfMalloc(dv_width*dv_height*sizeof(float));
    // copy frame 1
    for (int y=0; y<dv_height; y++) {
        for (int x=0; x<dv_width; x++) {
            frame_data1[y*dv_width+x]=frame_data[y*frame_width+x];
        }
    }
    // calc frame 1 statistics
    contribute_to_statistics(state1, frame_data1, dv_width, dv_height, frame, frames, NULL, NULL, NULL, dummyUI32, dummyF, dummyF, statistics1, isBackground);

    // copy frame 2
    for (int y=0; y<dv_height; y++) {
        for (int x=0; x<dv_width; x++) {
            frame_data1[y*dv_width+x]=frame_data[(y+shiftY1)*frame_width+(x+shiftX1)];
        }
    }
    // calc frame 2 statistics
    contribute_to_statistics(state2, frame_data1, dv_width, dv_height, frame, frames, NULL, NULL, NULL, dummyUI32, dummyF, dummyF, statistics2, isBackground);

    qfFree(frame_data1);
}




bool QFRDRImagingFCSCorrelationJobThread::saveStatistics(const QString &filename, const QString& filename_plot, const QFRDRImagingFCSCorrelationJobThread::StatisticsDataset &statistics, const QString &title, QString *error)
{
    bool ok=true;
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream text(&f);
        text.setLocale(outLocale);
        int count=statistics.time.size();
        for (int i=0; i<count; i++) {
            text<<statistics.time[i];
            if (i<statistics.mean.size()) text<<", "<<statistics.mean[i];
            else text<<", 0";
            if (i<statistics.std.size()) text<<", "<<statistics.std[i];
            else text<<", 0";
            if (i<statistics.min.size()) text<<", "<<statistics.min[i];
            else text<<", 0";
            if (i<statistics.max.size()) text<<", "<<statistics.max[i];
            else text<<", 0";
            text<<"\n";
        }
        f.close();
    } else {
        if (error) *error=QString("'%1': \n   %2").arg(filename).arg(f.errorString());
        ok=false;
    }
    QFile f1(filename_plot);
    if (f1.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream text(&f1);
        text.setLocale(outLocale);
        text<<QString("set xlabel 'time [seconds]'\n");
        text<<QString("set ylabel 'pixel grey value'\n");
        text<<QString("set title \"%2 '%1'\" noenhanced\n").arg(job.filename).arg(title);
        text<<QString("set style fill transparent solid 0.5 noborder\n");
        text<<QString("set multiplot layout 1,2\n");
        text<<QString("plot '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(filename).fileName());
        text<<QString("plot '%1' using 1:4:5 title 'min/max' with filledcu, '%1' using 1:(($2)-(($3)/2.0)):(($2)+(($3)/2.0)) title '+/- stddev' with filledcu, '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(filename).fileName());
        text<<QString("unset multiplot\n");
        text<<QString("pause -1\n");
        text<<QString("set multiplot layout 2,2\n");
        text<<QString("plot '%1' using 1:2 title 'mean' with lines lt 1\n").arg(QFileInfo(filename).fileName());
        text<<QString("plot '%1' using 1:3 title 'stddev' with lines lt 1\n").arg(QFileInfo(filename).fileName());
        text<<QString("plot '%1' using 1:4 title 'min' with lines lt 1\n").arg(QFileInfo(filename).fileName());
        text<<QString("plot '%1' using 1:5 title 'max' with lines lt 1\n").arg(QFileInfo(filename).fileName());
        text<<QString("unset multiplot\n");
        text<<QString("pause -1\n");
        text<<QString("f(t)=a0+a1*exp(-t/t1)\n");
        text<<QString("t1=50\n");
        text<<QString("a0=0\n");
        text<<QString("a1=500\n");
        text<<QString("fit f(x) '%1' using 1:2 via a0, a1, t1\n").arg(QFileInfo(filename).fileName());
        text<<QString("set label sprintf('1exp: a0=%f a1=%f t1=%f', a0, a1, t1) at graph 0.5,0.9\n");
        text<<QString("plot '%1' using 1:2 title 'mean' with points, f(x) title 'exponential fit\n").arg(QFileInfo(filename).fileName());
        text<<QString("pause -1\n");
        text<<QString("\n");
        text<<QString("\n");
        text<<QString("\n");
        f1.close();
    } else {
        ok=false;
        if (error) *error=tr(" (plot) '%1': \n   %2").arg(filename_plot).arg(f1.errorString());
    }
    return ok;
}

bool QFRDRImagingFCSCorrelationJobThread::SaveSDTIFF(const QString &filename, float *average_frame, float *sqrsum_frame, int frame_width, int frame_height, int frames, const QString &title, QString *error)
{
    TIFF* tif = TIFFOpen(filename.toLatin1().data(),"w");
    if (tif) {
        float* sd=(float*)qfCalloc(frame_width*frame_height, sizeof(float));

        if (frames>1) {
            for (int i=0; i<frame_width*frame_height; i++) {
                sd[i]=sqrt(float(frames)/float(frames-1)*(sqrsum_frame[i]-average_frame[i]*average_frame[i]));
            }
        } else {
            for (int i=0; i<frame_width*frame_height; i++) {
                sd[i]=0;
            }

        }

        TIFFTWriteFloat(tif, sd, frame_width, frame_height);
        TIFFClose(tif);
        qfFree(sd);
        return true;
    } else {
        if (error) *error=tr("could not create %2 '%1'!").arg(filename).arg(title);
        return false;
    }
}

bool QFRDRImagingFCSCorrelationJobThread::SaveTIFFFloat(const QString &filename, float *average_frame, int frame_width, int frame_height, const QString &title, QString *error)
{
    TIFF* tif = TIFFOpen(filename.toLatin1().data(),"w");
    if (tif) {

        TIFFTWriteFloat(tif, average_frame, frame_width, frame_height);
        TIFFClose(tif);
        return true;
    } else {
        if (error) *error=tr("could not create %2 image '%1'!").arg(filename).arg(title);
        return false;
    }
}

bool QFRDRImagingFCSCorrelationJobThread::SaveTIFFUInt16_scaled(const QString &filename, float *average_frame, int frame_width, int frame_height, const QString &title, QString *error)
{
    TIFF* tif = TIFFOpen(filename.toLatin1().data(),"w");
    if (tif) {
        float avgMin=average_frame[0];
        float avgMax=average_frame[0];
        for (int64_t i=0; i<frame_width*frame_height; i++) {
            avgMin=(average_frame[i]<avgMin)?average_frame[i]:avgMin;
            avgMax=(average_frame[i]>avgMax)?average_frame[i]:avgMax;
        }
        uint16_t* img=(uint16_t*)qfMalloc(frame_width*frame_height*sizeof(uint16_t));
        for (int64_t i=0; i<frame_width*frame_height; i++) {
            img[i]=(uint16_t)round((double)(average_frame[i]-avgMin)*(double)0xFFFF/fabs(avgMax-avgMin));
        }
        TIFFTWriteUint16(tif, img, frame_width, frame_height);
        qfFree(img);
        TIFFClose(tif);
        return true;
    } else {
        if (error) *error=tr("could not create %2 '%1'!").arg(filename).arg(title);
        return false;
    }
}


void QFRDRImagingFCSCorrelationJobThread::correlate_loadsingle() {
    QList<MultiTauCorrelator<double, double>* > acfjk;
    QList<MultiTauCorrelator<double, double>* > ccf1jk;
    QList<MultiTauCorrelator<double, double>* > ccf2jk;
    QList<MultiTauCorrelator<double, double>* > ccf3jk;
    QList<MultiTauCorrelator<double, double>* > ccf4jk;
    QList<QList<MultiTauCorrelator<double, double>* > > dccfjk;

    QList<correlatorjb<double, double>* > acfjb;
    QList<correlatorjb<double, double>* > ccf1jb;
    QList<correlatorjb<double, double>* > ccf2jb;
    QList<correlatorjb<double, double>* > ccf3jb;
    QList<correlatorjb<double, double>* > ccf4jb;
    QList<QList<correlatorjb<double, double>* > > dccfjb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // AS NOT ENOUGH MEMORY MAY BE AVAILABLE; WE WAIT HERE UNTIL ENOUGH MEMORY IS AVAILABLE AND MAY BE ALLOCATED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    emit messageChanged(tr("allocating memory ..."));
    frames_min=0;
    frames_max=0;
    if (job.distanceCCF) {
        for (int di=0; di<job.DCCFDeltaX.size(); di++) {
            dccfjb.append(QList<correlatorjb<double, double>* >());
            dccfjk.append(QList<MultiTauCorrelator<double, double>* >());
            dccf[di].dccfframe_width=frame_width;//-abs(job.DCCFDeltaX);
            dccf[di].dccfframe_height=frame_height;//-abs(job.DCCFDeltaY);
        }
    }
    firstFrames=(float*)qfCalloc(frame_width*frame_height,sizeof(float));
    lastFrames=(float*)qfCalloc(frame_width*frame_height,sizeof(float));
    NFitFrames=qMin((int64_t)300, (int64_t)frames);
    fit_t=(double*)qfCalloc(NFitFrames,sizeof(double));
    fit_frames=(float*)qfCalloc(NFitFrames*frame_width*frame_height,sizeof(float));


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOW WE READ ALL FRAMES IN THE TIFF FILE TO EXTRACT SOME BASIC STATISTICS INFORMATION ABOUT THE FRAMES (used for background correction)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!was_canceled) {
        emit messageChanged(tr("reading frames ..."));
        register uint32_t frame=0;


        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected(frame_width*frame_height);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected_dv1(frame_width*frame_height/2);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_uncorrected_dv2(frame_width*frame_height/2);
        statistics_uncorrected.clear();
        dv_statistics_uncorrected[0].clear();
        dv_statistics_uncorrected[1].clear();
        uint32_t video_frame_num=0;
        float fframes_min=0;
        float fframes_max=0;

        float* frame_data=(float*)qfCalloc(frame_width*frame_height,sizeof(float));
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
                contribute_to_statistics(stat_state_uncorrected, frame_data, frame_width, frame_height, frame, frames, &average_uncorrected_frame, &sqrsum_uncorrected_frame, &video_uncorrected, video_frame_num, fframes_min, fframes_max, statistics_uncorrected);
                contribute_to_dv2_statistics(stat_state_uncorrected_dv1, stat_state_uncorrected_dv2, frame_data, frame_width, frame_height, frame, frames, dv_statistics_uncorrected[0], dv_statistics_uncorrected[1]);

                float frame_min=frame_data[0];
                float frame_max=frame_data[0];
                for (register uint32_t i=0; i<frame_width*frame_height; i++) {
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
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        firstFrames[i]=firstFrames[i]+(float)frame_data[i]/(float)job.bleachAvgFrames;
                    }
                } else if (frame>frames-job.bleachAvgFrames) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
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

        qfFree(frame_data);
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
        if (job.bleach==BLEACH_EXP||job.bleach==BLEACH_EXPREG||job.bleach==BLEACH_EXP_POLY2||job.bleach==BLEACH_EXP_POLY3||job.bleach==BLEACH_EXP_POLY4||job.bleach==BLEACH_EXP_POLY5||job.bleach==BLEACH_DBL_EXP) {
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

            float* frame_data=(float*)qfMalloc(frame_width*frame_height*sizeof(float));
            for (int i=0; i<NFitFrames; i++) {
                int iidx=trunc((double)i/(double)(NFitFrames-1)*(double)frames);
                if (iidx<0) iidx=0;
                if (iidx>=(int64_t)frames) iidx=frames-1;
                fit_t[i]=(double)iidx+(double)job.bleachAvgFrames/2.0;
                while ((int64_t)frame<iidx) {
                    reader->nextFrame();
                    frame++;
                }
                if (!reader->readFrameFloat(frame_data)) {
                    m_status=-1;
                }
                //qDebug()<<i<<"/"<<NFitFrames<<":    "<<iidx<<"/"<<frames;
                for (uint64_t idx=0; idx<frame_width*frame_height; idx++) {
                    fit_frames[i*frame_width*frame_height+idx]=frame_data[idx];
                }
                if (job.bleachAvgFrames>1) {
                    for (uint32_t f=1; f<job.bleachAvgFrames; f++) {
                        reader->nextFrame();
                        frame++;
                        if (!reader->readFrameFloat(frame_data)) { m_status=-1; }
                        for (uint64_t idx=0; idx<frame_width*frame_height; idx++) {
                            fit_frames[i*frame_width*frame_height+idx]+=frame_data[idx];
                        }
                    }
                    for (uint64_t idx=0; idx<frame_width*frame_height; idx++) {
                        fit_frames[i*frame_width*frame_height+idx]/=(float)job.bleachAvgFrames;
                    }
                }
                if (was_canceled || (m_status!=1)) break;
                if (i%(NFitFrames/100)==0) {
                    emit messageChanged(tr("calculating bleach correction ... frame %1/%2 ...").arg(i+1).arg(NFitFrames));
                }
            }
            qfFree(frame_data);

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

        float* frame_data=(float*)qfMalloc(frame_width*frame_height*sizeof(float));

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

        if (job.distanceCCF) {
            for (int id=0; id<job.DCCFDeltaX.size(); id++) {
                dccf[id].dccf_N=0;
                prepare_ccfs(dccfjk[id], dccfjb[id], &(dccf[id].dccf), &(dccf[id].dccf_std), &(dccf[id].dccf_tau), dccf[id].dccf_N, dccf[id].dccfframe_width, dccf[id].dccfframe_height, job.segments);
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

        uint32_t video_frame_num=0;

        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state(frame_width*frame_height);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_dv1(frame_width*frame_height);
        QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_dv2(frame_width*frame_height);
        statistics.clear();
        dv_statistics[0].clear();
        dv_statistics[1].clear();


        //bool videoFirst=true;

        uint32_t segment_frames=frames/job.segments;
        do {
            if (!reader->readFrameFloat(frame_data)) {
                m_status=-1; emit statusChanged(m_status);
                emit messageChanged(tr("error reading frame: %1").arg(reader->lastError()));
            } else {
                // APPLY BACKGROUND CORRECTION TO FRAME
                double avg=0;
                for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                    frame_data[i]=frame_data[i]-baseline-backgroundImage[i];
                    avg=avg+frame_data[i]/ ((double)(frame_width*frame_height));
                }

                if (job.bleach==BLEACH_EXP || job.bleach==BLEACH_EXPREG) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        if (bleachFitOK[i]!=0) {
                            frame_data[i]=bleachCorrectExp(frame_data[i], i, frame);
                        }
                    }
                } else if (job.bleach==BLEACH_DBL_EXP) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        if (bleachFitOK[i]!=0) {
                            frame_data[i]=bleachCorrectDblExp(frame_data[i], i, frame);
                        }
                    }
                } else if (job.bleach==BLEACH_EXP_POLY2) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        if (bleachFitOK[i]!=0) {
                            frame_data[i]=bleachCorrectExpPoly2(frame_data[i], i, frame);
                        }
                    }
                } else if (job.bleach==BLEACH_EXP_POLY3) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        if (bleachFitOK[i]!=0) {
                            frame_data[i]=bleachCorrectExpPoly3(frame_data[i], i, frame);
                        }
                    }
                } else if (job.bleach==BLEACH_EXP_POLY4) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        if (bleachFitOK[i]!=0) {
                            frame_data[i]=bleachCorrectExpPoly4(frame_data[i], i, frame);
                        }
                    }
                } else if (job.bleach==BLEACH_EXP_POLY5) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        if (bleachFitOK[i]!=0) {
                            frame_data[i]=bleachCorrectExpPoly5(frame_data[i], i, frame);
                        }
                    }
                } else if (job.bleach==BLEACH_REMOVEAVG) {
                    for (register uint32_t i=0; i<frame_width*frame_height; i++) {
                        frame_data[i]=frame_data[i]*firstFrames[i]/avg;
                    }
                }

                // CALCULATE STATISTICS AND VIDEO
                contribute_to_statistics(stat_state, frame_data, frame_width, frame_height, frame, frames, &average_frame, &sqrsum_frame, &video, video_frame_num, frames_min, frames_max, statistics);
                contribute_to_dv2_statistics(stat_state_dv1, stat_state_dv2, frame_data, frame_width, frame_height, frame, frames, dv_statistics[0], dv_statistics[1]);




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
                if (job.distanceCCF) {
                    for (int id=0; id<job.DCCFDeltaX.size(); id++) {
                        if (dccf[id].dccf_N>0) {
                            contribute_to_correlations(dccfjk[id], dccfjb[id], frame_data, frame_width, frame_height, job.DCCFDeltaX[id], job.DCCFDeltaY[id], frame, segment_frames, dccf[id].dccf_tau, dccf[id].dccf, dccf[id].dccf_std, dccf[id].dccf_N);
                        }
                    }
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
        if (job.distanceCCF && (m_status==1) && (!was_canceled)) {
            for (int id=0; id<job.DCCFDeltaX.size(); id++) {
                if (dccf[id].dccf_N>0) {
                    emit messageChanged(tr("averaging distance CCF segments ..."));
                    average_ccfs(&(dccf[id].dccf), &(dccf[id].dccf_std), dccf[id].dccf_N, dccf[id].dccfframe_width, dccf[id].dccfframe_height, job.segments);
                }
            }
        }
        qfFree(frame_data);
    }

    qDeleteAll(acfjk);
    qDeleteAll(ccf1jk);
    qDeleteAll(ccf2jk);
    qDeleteAll(ccf3jk);
    qDeleteAll(ccf4jk);
    for (int i=0; i<dccfjk.size(); i++) {
        qDeleteAll(dccfjk[i]);
    }

    qDeleteAll(acfjb);
    qDeleteAll(ccf1jb);
    qDeleteAll(ccf2jb);
    qDeleteAll(ccf3jb);
    qDeleteAll(ccf4jb);
    for (int i=0; i<dccfjk.size(); i++) {
        qDeleteAll(dccfjb[i]);
    }


}



static double QFRDRImagingFCSCorrelationJobThread_fExpLin( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    return A-t/tau;

}


static double QFRDRImagingFCSCorrelationJobThread_fExpPoly2Lin( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    const double f=par[2];
    return A-(t+f*t*t)/tau;

}

static double QFRDRImagingFCSCorrelationJobThread_fDblExp( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    const double A2=par[2];
    const double tau2=par[3];
    return A*exp(-t/tau)+A2*exp(-t/tau2);

}

static double QFRDRImagingFCSCorrelationJobThread_fExp( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    return A*exp(-t/tau);

}

static double QFRDRImagingFCSCorrelationJobThread_fExpPoly3Lin( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    const double f=par[2];
    const double f3=par[3];
    return A-(t+f*t*t+f3*t*t*t)/tau;

}
static double QFRDRImagingFCSCorrelationJobThread_fExpPoly4Lin( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    const double f=par[2];
    const double f3=par[3];
    const double f4=par[4];
    return A-(t+f*t*t+f3*t*t*t+f4*t*t*t*t)/tau;

}
static double QFRDRImagingFCSCorrelationJobThread_fExpPoly5Lin( double t, const double *par )
{
    const double A=par[0];
    const double tau=par[1];
    const double f=par[2];
    const double f3=par[3];
    const double f4=par[4];
    const double f5=par[5];
    return A-(t+f*t*t+f3*t*t*t+f4*t*t*t*t+f5*t*t*t*t*t)/tau;

}


void QFRDRImagingFCSCorrelationJobThread::calcBleachCorrection(float* fit_frames, double* fit_tin, int NFitFrames) {
    if (job.bleach==BLEACH_EXP) {
        if (fit_frames && fit_t && NFitFrames>2) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                lm_control_struct control=lm_control_double;
                control.maxcall=500;
                lm_status_struct status;

                int NFitFramesInt=0;
                double* fit_I=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_t=(double*)qfMalloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    const double v=fit_frames[jj*frame_width*frame_height+i];
                    if (v>0.0 && QFFloatIsOK(v)) {
                        fit_t[NFitFramesInt]=fit_tin[jj];
                        fit_I[NFitFramesInt]=log(v);
                        NFitFramesInt++;
                    }
                }
                bleachFitOK[i]=0;
                if (NFitFramesInt>2) {
                    double pA=0, pB=0;
                    if (!statisticsLinearRegression(fit_t, fit_I, NFitFramesInt, pA, pB)) {
                        pA=fit_I[0];
                        pB=-1.0/(fit_t[NFitFramesInt-2]/(fit_I[0]-fit_I[NFitFramesInt-2]));
                    }


                    //double par[2]={fit_I[0], fit_t[NFitFrames-2]/(fit_I[0]-fit_I[NFitFrames-2])};
                    double par[2]={pA, -1.0/pB};
                    //qDebug()<<i<<": initA="<<par[0]<<" initTau="<<par[1];
                    lmcurve_fit(2, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpLin, &control, &status);
                    //qDebug()<<i<<": A="<<par[0]<<" tau="<<par[1]<<"     norm="<<status.fnorm<<" feval="<<status.nfev<<" message="<<lm_shortmsg[status.info];

                    bleachAmplitude[i]=exp(par[0]);
                    bleachTime[i]=par[1];
                    bleachFitOK[i]=1;
                    double t1=fit_t[NFitFramesInt/2];
                    double t2=fit_t[2*NFitFramesInt/3];
                    if ( (!QFFloatIsOK(bleachAmplitude[i]))
                         || (!QFFloatIsOK(bleachTime[i]))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                         || (bleachTime[i]==0)
                         || (std::isinf(bleachTime[i]))
                         || (std::isinf(bleachAmplitude[i]))
                         || (bleachAmplitude[i]==0) ) {
                        bleachFitOK[i]=0;
                    }
                }
                qfFree(fit_I);
                qfFree(fit_t);
                if (i>5 && i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }
        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachTime[i]=1;
                bleachFitOK[i]=0;
            }
        }
    } else if (job.bleach==BLEACH_EXP_POLY2) {
        if (fit_frames && fit_t && NFitFrames>3) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                lm_control_struct control=lm_control_double;
                control.maxcall=500;
                lm_status_struct status;

                int NFitFramesInt=0;
                double* fit_I=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_t=(double*)qfMalloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    const double v=fit_frames[jj*frame_width*frame_height+i];
                    if (v>0 && QFFloatIsOK(v)) {
                        fit_I[NFitFramesInt]=log(v);
                        fit_t[NFitFramesInt]=fit_tin[jj];
                        NFitFramesInt++;
                    }
                }
                bleachFitOK[i]=0;
                if (NFitFramesInt>3) {

                    double pA=0, pB=0;
                    if (!statisticsIterativelyReweightedLeastSquaresRegression(fit_t, fit_I, NFitFramesInt, pA, pB)) {
                        pA=fit_I[0];
                        pB=-1.0/(fit_t[NFitFramesInt-2]/(fit_I[0]-fit_I[NFitFramesInt-2]));
                    }


                    //double par[2]={fit_I[0], fit_t[NFitFrames-2]/(fit_I[0]-fit_I[NFitFrames-2])};
                    double par[4]={pA, -1.0/pB,0};
                    //qDebug()<<i<<": initA="<<par[0]<<" initTau="<<par[1];
                    int npar=3;
                    lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly2Lin, &control, &status);
                    //qDebug()<<i<<": A="<<par[0]<<" tau="<<par[1]<<"     norm="<<status.fnorm<<" feval="<<status.nfev<<" message="<<lm_shortmsg[status.info];

                    bleachAmplitude[i]=exp(par[0]);
                    bleachTime[i]=par[1];
                    bleachPoly2[i]=par[2];
                    bleachPoly3[i]=0;
                    bleachPoly4[i]=0;
                    bleachPoly5[i]=0;
                    bleachFitOK[i]=1;
                    double t1=fit_t[NFitFramesInt/2];
                    double t2=fit_t[2*NFitFramesInt/3];
                    if ( (!QFFloatIsOK(bleachAmplitude[i]))
                         || (!QFFloatIsOK(bleachTime[i]))
                         || (!QFFloatIsOK(bleachPoly2[i]))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                         || (bleachTime[i]==0)
                         || (std::isinf(bleachTime[i]))
                         || (std::isinf(bleachAmplitude[i]))
                         || (bleachAmplitude[i]==0) ) {
                        bleachFitOK[i]=0;
                    }
                }
                qfFree(fit_I);
                qfFree(fit_t);
                //qfFree(fit_Inl);
                if (i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }
        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachTime[i]=1;
                bleachPoly2[i]=0;
                bleachPoly3[i]=0;
                bleachPoly4[i]=0;
                bleachPoly5[i]=0;
                bleachFitOK[i]=0;
            }
        }
    } else if (job.bleach==BLEACH_EXP_POLY3) {
        if (fit_frames && fit_t && NFitFrames>4) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                lm_control_struct control=lm_control_double;
                control.maxcall=500;
                lm_status_struct status;

                int NFitFramesInt=0;
                double* fit_I=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_t=(double*)qfMalloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    const double v=fit_frames[jj*frame_width*frame_height+i];
                    if (v>0 && QFFloatIsOK(v)) {
                        fit_I[NFitFramesInt]=log(v);
                        fit_t[NFitFramesInt]=fit_tin[jj];
                        NFitFramesInt++;
                    }
                }
                bleachFitOK[i]=0;
                if (NFitFramesInt>4) {

                    double pA=0, pB=0;



                    double par[4]={pA, -1.0/pB,0,0};
                    int npar=3;

                    QVector<double> pFit(4,0.0);
                    if (statisticsPolyFit(fit_t, fit_I, NFitFramesInt, 3, pFit.data())) {
                        par[0]=pFit[0];
                        par[1]=-1.0/pFit[1];
                        par[2]=-pFit[2]/pFit[1];
                        par[3]=-pFit[3]/pFit[1];
                        npar=4;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly3Lin, &control, &status);

                    } else {
                        if (statisticsIterativelyReweightedLeastSquaresRegression(fit_t, fit_I, NFitFramesInt, pA, pB)) {
                            par[0]=pA;
                            par[1]=-1.0/pB;
                        }
                        npar=3;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly2Lin, &control, &status);
                        npar++;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly3Lin, &control, &status);

                    }


                    //double par[2]={fit_I[0], fit_t[NFitFrames-2]/(fit_I[0]-fit_I[NFitFrames-2])};

                    //qDebug()<<i<<": initA="<<par[0]<<" initTau="<<par[1];
                    //qDebug()<<i<<": A="<<par[0]<<" tau="<<par[1]<<"     norm="<<status.fnorm<<" feval="<<status.nfev<<" message="<<lm_shortmsg[status.info];

                    bleachAmplitude[i]=exp(par[0]);
                    bleachTime[i]=par[1];
                    bleachPoly2[i]=par[2];
                    bleachPoly3[i]=par[3];
                    bleachFitOK[i]=1;
                    double t1=fit_t[NFitFramesInt/2];
                    double t2=fit_t[2*NFitFramesInt/3];
                    if ( (!QFFloatIsOK(bleachAmplitude[i]))
                         || (!QFFloatIsOK(bleachTime[i]))
                         || (!QFFloatIsOK(bleachPoly2[i]))
                         || (!QFFloatIsOK(bleachPoly3[i]))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                         || (bleachTime[i]==0)
                         || (std::isinf(bleachTime[i]))
                         || (std::isinf(bleachAmplitude[i]))
                         || (bleachAmplitude[i]==0) ) {
                        bleachFitOK[i]=0;
                    }
                }
                qfFree(fit_I);
                qfFree(fit_t);
                //qfFree(fit_Inl);
                if (i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }

        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachTime[i]=1;
                bleachPoly2[i]=0;
                bleachPoly3[i]=0;
                bleachPoly4[i]=0;
                bleachPoly5[i]=0;
                bleachFitOK[i]=0;
            }
        }


    } else if (job.bleach==BLEACH_EXP_POLY4) {
        if (fit_frames && fit_t && NFitFrames>5) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                lm_control_struct control=lm_control_double;
                control.maxcall=500;
                lm_status_struct status;

                int NFitFramesInt=0;
                double* fit_I=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_t=(double*)qfMalloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    const double v=fit_frames[jj*frame_width*frame_height+i];
                    if (v>0 && QFFloatIsOK(v)) {
                        fit_I[NFitFramesInt]=log(v);
                        fit_t[NFitFramesInt]=fit_tin[jj];
                        NFitFramesInt++;
                    }
                }
                bleachFitOK[i]=0;
                if (NFitFramesInt>5) {

                    double pA=0, pB=0;

                    double par[5]={pA, -1.0/pB,0,0,0};
                    int npar=4;

                    QVector<double> pFit(5,0.0);
                    if (statisticsPolyFit(fit_t, fit_I, NFitFramesInt, 4, pFit.data())) {
                        par[0]=pFit[0];
                        par[1]=-1.0/pFit[1];
                        par[2]=-pFit[2]/pFit[1];
                        par[3]=-pFit[3]/pFit[1];
                        par[4]=-pFit[4]/pFit[1];
                        npar=5;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly4Lin, &control, &status);

                    } else {
                        if (statisticsIterativelyReweightedLeastSquaresRegression(fit_t, fit_I, NFitFramesInt, pA, pB)) {
                            par[0]=pA;
                            par[1]=-1.0/pB;
                        }
                        npar=3;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly2Lin, &control, &status);
                        npar++;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly3Lin, &control, &status);
                        npar++;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly4Lin, &control, &status);

                    }
                    //qDebug()<<i<<": A="<<par[0]<<" tau="<<par[1]<<"     norm="<<status.fnorm<<" feval="<<status.nfev<<" message="<<lm_shortmsg[status.info];

                    bleachAmplitude[i]=exp(par[0]);
                    bleachTime[i]=par[1];
                    bleachPoly2[i]=par[2];
                    bleachPoly3[i]=par[3];
                    bleachPoly4[i]=par[4];
                    bleachFitOK[i]=1;
                    double t1=fit_t[NFitFramesInt/2];
                    double t2=fit_t[2*NFitFramesInt/3];
                    if ( (!QFFloatIsOK(bleachAmplitude[i]))
                         || (!QFFloatIsOK(bleachTime[i]))
                         || (!QFFloatIsOK(bleachPoly2[i]))
                         || (!QFFloatIsOK(bleachPoly3[i]))
                         || (!QFFloatIsOK(bleachPoly4[i]))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                         || (bleachTime[i]==0)
                         || (std::isinf(bleachTime[i]))
                         || (std::isinf(bleachAmplitude[i]))
                         || (bleachAmplitude[i]==0) ) {
                        bleachFitOK[i]=0;
                    }
                }
                qfFree(fit_I);
                qfFree(fit_t);
                //qfFree(fit_Inl);
                if (i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }

        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachTime[i]=1;
                bleachPoly2[i]=0;
                bleachPoly3[i]=0;
                bleachPoly4[i]=0;
                bleachPoly5[i]=0;
                bleachFitOK[i]=0;
            }
        }

    } else if (job.bleach==BLEACH_EXP_POLY5) {
        if (fit_frames && fit_t && NFitFrames>6) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                lm_control_struct control=lm_control_double;
                control.maxcall=500;
                lm_status_struct status;

                int NFitFramesInt=0;
                double* fit_I=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_t=(double*)qfMalloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    const double v=fit_frames[jj*frame_width*frame_height+i];
                    if (v>0 && QFFloatIsOK(v)) {
                        fit_I[NFitFramesInt]=log(v);
                        fit_t[NFitFramesInt]=fit_tin[jj];
                        NFitFramesInt++;
                    }
                }
                bleachFitOK[i]=0;
                if (NFitFramesInt>6) {

                    double pA=0, pB=0;
                    double par[6]={pA, -1.0/pB,0,0,0,0};
                    int npar=5;

                    QVector<double> pFit(6,0.0);
                    if (statisticsPolyFit(fit_t, fit_I, NFitFramesInt, 5, pFit.data())) {
                        par[0]=pFit[0];
                        par[1]=-1.0/pFit[1];
                        par[2]=-pFit[2]/pFit[1];
                        par[3]=-pFit[3]/pFit[1];
                        par[4]=-pFit[4]/pFit[1];
                        par[5]=-pFit[5]/pFit[1];
                        npar=6;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly5Lin, &control, &status);

                    } else {
                        if (statisticsIterativelyReweightedLeastSquaresRegression(fit_t, fit_I, NFitFramesInt, pA, pB)) {
                            par[0]=pA;
                            par[1]=-1.0/pB;
                        }
                        npar=3;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly2Lin, &control, &status);
                        npar++;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly3Lin, &control, &status);
                        npar++;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly4Lin, &control, &status);
                        npar++;
                        lmcurve_fit(npar, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExpPoly5Lin, &control, &status);

                    }
                    //qDebug()<<i<<": A="<<par[0]<<" tau="<<par[1]<<"     norm="<<status.fnorm<<" feval="<<status.nfev<<" message="<<lm_shortmsg[status.info];

                    bleachAmplitude[i]=exp(par[0]);
                    bleachTime[i]=par[1];
                    bleachPoly2[i]=par[2];
                    bleachPoly3[i]=par[3];
                    bleachPoly4[i]=par[4];
                    bleachPoly5[i]=par[5];
                    bleachFitOK[i]=1;
                    double t1=fit_t[NFitFramesInt/2];
                    double t2=fit_t[2*NFitFramesInt/3];
                    if ( (!QFFloatIsOK(bleachAmplitude[i]))
                         || (!QFFloatIsOK(bleachTime[i]))
                         || (!QFFloatIsOK(bleachPoly2[i]))
                         || (!QFFloatIsOK(bleachPoly3[i]))
                         || (!QFFloatIsOK(bleachPoly4[i]))
                         || (!QFFloatIsOK(bleachPoly5[i]))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                         || (bleachTime[i]==0)
                         || (std::isinf(bleachTime[i]))
                         || (std::isinf(bleachAmplitude[i]))
                         || (bleachAmplitude[i]==0) ) {
                        bleachFitOK[i]=0;
                    }
                }
                qfFree(fit_I);
                qfFree(fit_t);
                //qfFree(fit_Inl);
                if (i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }

        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachTime[i]=1;
                bleachPoly2[i]=0;
                bleachPoly3[i]=0;
                bleachPoly4[i]=0;
                bleachPoly5[i]=0;
                bleachFitOK[i]=0;
            }
        }

    } else if (job.bleach==BLEACH_DBL_EXP) {
        if (fit_frames && fit_t && NFitFrames>4) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                lm_control_struct control=lm_control_double;
                control.maxcall=500;
                lm_status_struct status;




                int NFitFramesInt=0;
                double* fit_I=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_t=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_Il=(double*)qfMalloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    const double v=fit_frames[jj*frame_width*frame_height+i];
                    if (v>0 && QFFloatIsOK(v)) {
                        fit_I[NFitFramesInt]=v;
                        fit_t[NFitFramesInt]=fit_tin[jj];
                        fit_Il[NFitFramesInt]=log(v);
                        NFitFramesInt++;
                    }
                }
                bleachFitOK[i]=0;
                if (NFitFramesInt>4) {

                    double pA=0, pB=0;
                    if (!statisticsIterativelyReweightedLeastSquaresRegression(fit_t, fit_Il, NFitFramesInt, pA, pB)) {
                        pA=fit_Il[0];
                        pB=-1.0/(fit_t[NFitFramesInt-2]/(fit_Il[0]-fit_Il[NFitFramesInt-2]));
                    }


                    //double par[2]={fit_I[0], fit_t[NFitFrames-2]/(fit_I[0]-fit_I[NFitFrames-2])};
                    double par[4]={exp(par[0]), -1.0/pB,0,0};
                    //qDebug()<<i<<": initA="<<par[0]<<" initTau="<<par[1];
                    lmcurve_fit(2, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fExp, &control, &status);
                    par[2]=par[0]/2.0;
                    par[0]=par[0]/2.0;
                    par[3]=par[1];
                    lmcurve_fit(4, par, NFitFramesInt, fit_t, fit_I, QFRDRImagingFCSCorrelationJobThread_fDblExp, &control, &status);
                    //qDebug()<<i<<": A="<<par[0]<<" tau="<<par[1]<<"     norm="<<status.fnorm<<" feval="<<status.nfev<<" message="<<lm_shortmsg[status.info];

                    bleachAmplitude[i]=par[0];
                    bleachTime[i]=par[1];
                    bleachAmplitude2[i]=par[2];
                    bleachTime2[i]=par[3];
                    bleachPoly2[i]=0;
                    bleachPoly3[i]=0;
                    bleachPoly4[i]=0;
                    bleachPoly5[i]=0;
                    bleachFitOK[i]=1;
                    double t1=fit_t[NFitFramesInt/2];
                    double t2=fit_t[2*NFitFramesInt/3];
                    if ( (!QFFloatIsOK(bleachAmplitude[i]))
                         || (!QFFloatIsOK(bleachTime[i]))
                         || (!QFFloatIsOK(bleachTime2[i]))
                         || (!QFFloatIsOK(bleachAmplitude2[i]))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude2[i]*exp(-t1/(bleachTime2[i]))))
                         || (!QFFloatIsOK(bleachAmplitude2[i]*exp(-t2/(bleachTime2[i]))))
                         || (bleachTime[i]==0)
                         || (bleachTime2[i]==0)
                         || (std::isinf(bleachTime[i]))
                         || (std::isinf(bleachAmplitude[i]))
                         || (std::isinf(bleachTime2[i]))
                         || (std::isinf(bleachAmplitude2[i]))
                         || (bleachAmplitude[i]==0)
                         || (bleachAmplitude2[i]==0) ) {
                        bleachFitOK[i]=0;
                    }
                }
                qfFree(fit_I);
                qfFree(fit_t);
                qfFree(fit_Il);
                //qfFree(fit_Inl);
                if (i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }
        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachTime[i]=1;
                bleachPoly2[i]=0;
                bleachPoly3[i]=0;
                bleachPoly4[i]=0;
                bleachPoly5[i]=0;
                bleachFitOK[i]=0;
            }
        }
    } else if (job.bleach==BLEACH_EXPREG) {
        if (fit_frames && fit_t && NFitFrames>2) {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {

                int NFitFramesInt=0;
                double* fit_I=(double*)qfMalloc(NFitFrames*sizeof(double));
                double* fit_t=(double*)qfMalloc(NFitFrames*sizeof(double));
                for (int jj=0; jj<NFitFrames; jj++) {
                    const double v=fit_frames[jj*frame_width*frame_height+i];
                    if (v>0 && QFFloatIsOK(v)) {
                        fit_I[NFitFramesInt]=log(v);
                        fit_t[NFitFramesInt]=fit_tin[jj];
                        NFitFramesInt++;
                    }
                }
                bleachFitOK[i]=0;
                if (NFitFramesInt>2) {
                    double pA=0, pB=0;
                    statisticsIterativelyReweightedLeastSquaresRegression(fit_t, fit_I, NFitFramesInt, pA, pB);

                    bleachAmplitude[i]=exp(pA);
                    bleachTime[i]=-1.0/pB;
                    bleachFitOK[i]=1;
                    double t1=fit_t[NFitFramesInt/2];
                    double t2=fit_t[2*NFitFramesInt/3];
                    if ( (!QFFloatIsOK(bleachAmplitude[i]))
                         || (!QFFloatIsOK(bleachTime[i]))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t1/(bleachTime[i]))))
                         || (!QFFloatIsOK(bleachAmplitude[i]*exp(-t2/(bleachTime[i]))))
                         || (bleachTime[i]==0)
                         || (std::isinf(bleachTime[i]))
                         || (std::isinf(bleachAmplitude[i]))
                         || (bleachAmplitude[i]==0) ) {
                        bleachFitOK[i]=0;
                    }
                }
                qfFree(fit_I);
                qfFree(fit_t);
                if (i%(frame_width*frame_height/20)==0) {
                    emit messageChanged(tr("calculating bleach correction parameters (%1/%2) ...").arg(i+1).arg(frame_width*frame_height));
                }
            }
        } else {
            for (uint32_t i=0; i<frame_width*frame_height; i++) {
                bleachAmplitude[i]=0;
                bleachTime[i]=1;
                bleachPoly2[i]=0;
                bleachPoly3[i]=0;
                bleachPoly4[i]=0;
                bleachPoly5[i]=0;
                bleachFitOK[i]=0;
            }
        }
    } else {
        for (uint32_t i=0; i<frame_width*frame_height; i++) {
            bleachAmplitude[i]=0;
            bleachTime[i]=1;
            bleachPoly2[i]=0;
            bleachPoly3[i]=0;
            bleachPoly4[i]=0;
            bleachPoly5[i]=0;
            bleachFitOK[i]=1;
        }
    }
}

void QFRDRImagingFCSCorrelationJobThread::calcBackgroundCorrection() {
    if (backgroundImage) qfFree(backgroundImage);
    if (backgroundImageStd) qfFree(backgroundImageStd);
    backgroundImage=(float*)qfMalloc(frame_width*frame_height*sizeof(float));
    backgroundImageStd=(float*)qfMalloc(frame_width*frame_height*sizeof(float));
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


    QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state(frame_width*frame_height);
    QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_dv0(frame_width*frame_height/2);
    QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state stat_state_dv1(frame_width*frame_height/2);
    backstatistics.clear();
    dv_backstatistics[0].clear();
    dv_backstatistics[1].clear();
    uint32_t vidfnum=0;
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
                    uint32_t bframe_width=reader->frameWidth();
                    uint32_t bframe_height=reader->frameHeight();
                    uint32_t backFrames=reader->countFrames();

                    if ((bframe_width==frame_width)&&(bframe_height==frame_height))  {

                        float* frame_data=(float*)qfMalloc(frame_width*frame_height*sizeof(float));
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

                                contribute_to_statistics(stat_state, frame_data, frame_width, frame_height, frames, backFrames, NULL, NULL, NULL, vidfnum, bframes_min, bframes_max, backstatistics, true);
                                contribute_to_dv2_statistics(stat_state_dv0, stat_state_dv1, frame_data, frame_width, frame_height, frames, backFrames, dv_backstatistics[0], dv_backstatistics[1], true);

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

                        qfFree(frame_data);
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

QFRDRImagingFCSCorrelationJobThread::Fileinfo::Fileinfo(const QString &filename, const QString &role, int internalDualViewMode, int dualViewID, bool isNandB, const QString& group)
{
    this->filename=filename;
    this->filenameVar="";
    this->filenameBack="";
    this->filenameBackVar="";
    this->role=role;
    this->group=group;
    this->internalDualViewMode=internalDualViewMode;
    this->dualViewID=dualViewID;
    this->filetype=QFRDRImagingFCSCorrelationJobThread::ftCorrelation;
}

QFRDRImagingFCSCorrelationJobThread::Fileinfo QFRDRImagingFCSCorrelationJobThread::getFileInfo(const QString &filename, const QString& filenameEvalSettings, const QString &role, const QString &group)
{
    QFRDRImagingFCSCorrelationJobThread::Fileinfo i;
    i.filename=filename;
    i.filenameEvalSettings=filenameEvalSettings;
    i.role=role;
    i.group=group;
    i.dualViewID=0;
    i.internalDualViewMode=0;
    i.props=props;
    i.comment=comment;
    return i;
}

QFRDRImagingFCSCorrelationJobThread::Fileinfo QFRDRImagingFCSCorrelationJobThread::getFileInfo(const QString &filename, const QString &filenameEvalSettings, const QString &role, int dualViewID, const QString& group)
{
    QFRDRImagingFCSCorrelationJobThread::Fileinfo i;
    i.filename=filename;
    i.filenameEvalSettings=filenameEvalSettings;
    i.role=role;
    i.group=group;
    i.dualViewID=dualViewID;
    i.internalDualViewMode=job.dualViewMode;
    i.props=props;
    i.comment=comment;
    return i;
}

QFRDRImagingFCSCorrelationJobThread::Fileinfo QFRDRImagingFCSCorrelationJobThread::getFileInfoNandB(const QString &filename, const QString &filenameVar, const QString &filenameBack, const QString &filenameBackVar, const QString& filenameEvalSettings, const QString &role, int dualViewID, const QString& group)
{
    QFRDRImagingFCSCorrelationJobThread::Fileinfo i;
    i.filename=filename;
    i.filenameVar=filenameVar;
    i.filenameBack=filenameBack;
    i.filenameBackVar=filenameBackVar;
    i.filenameEvalSettings=filenameEvalSettings;
    i.role=role;
    i.group=group;
    i.dualViewID=dualViewID;
    i.internalDualViewMode=job.dualViewMode;
    i.filetype=QFRDRImagingFCSCorrelationJobThread::ftNandB;
    i.props=props;
    i.comment=comment;
    //qDebug()<<"getFileInfoNandB:"<<filename<<filenameVar<<filenameBack<<filenameBackVar;
    return i;
}

QString QFRDRImagingFCSCorrelationJobThread::getGroupName() const
{
    return outputFilenameBase;
}

void QFRDRImagingFCSCorrelationJobThread::StatisticsDataset::clear()
{
    time.clear();
    min.clear();
    max.clear();
    mean.clear();
    std.clear();
}

QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state::contribute_to_statistics_state(int size)
{
    sum=0;
    sum2=0;
    sframe_min=0;
    sframe_max=0;
    cnt=0;
    statFirst=true;
    video_frame=(float*)qfCalloc(size, sizeof(float));
    for (int64_t i=0; i<size; i++)  {
        video_frame[i]=0;
    }
}

QFRDRImagingFCSCorrelationJobThread::contribute_to_statistics_state::~contribute_to_statistics_state()
{
    qfFree(video_frame);
}

