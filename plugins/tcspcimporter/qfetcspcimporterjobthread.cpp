#include "qfetcspcimporterjobthread.h"
#include "qfetcspcimporterdialog.h"
#include "qftcspcreader.h"
#include "statistics_tools.h"
#include <stdint.h>
#include "qfimportermanager.h"
#include "binarydatatools.h"

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

QList<QPair<QString, QString> > QFETCSPCImporterJobThread::getAddFiles() const {
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
    /*if (job.correlator==CORRELATOR_DIRECT) corr="direct";
    if (job.correlator==CORRELATOR_DIRECTAVG) corr="directavg";*/
    if (job.correlator==CORRELATOR_MTAUALLMON) corr="mtauallmon";
    if (job.correlator==CORRELATOR_MTAUONEMON) corr="mtauonemon";

    result=result.replace("%correlator%", corr, Qt::CaseInsensitive);
    result=result.replace("%correlatorid%", QString::number(job.correlator), Qt::CaseInsensitive);
    return result;
}

void QFETCSPCImporterJobThread::run() {
    QTime ptime;

    ptime.start();

    starttime=0;
    range_duration=0;
    file_duration=0;
    channels=0;

    emit messageChanged(tr("loading data ..."));
    if ((job.fileFormat<0)||(job.fileFormat>=getImporterCount(pluginServices))) {
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
        emit rangeChanged(0,1020);


        reader=NULL;
        bool OK=false;
        if (job.fileFormat>=0 && job.fileFormat<getImporterCount(pluginServices)) {
            reader=getImporter(job.fileFormat, pluginServices);
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
                QString crFilenameBin=outputFilenameBase+".photoncounts.dat";

                QString localFileDirectory=QFileInfo(d.absoluteFilePath(configFilename)).dir().absolutePath();

                if (d.mkpath(localFileDirectory)) {
                    ////////////////////////////////////////////////////////////////////////////////////////////
                    // TOUCH OUTPUT FILE (.evalsettings.txt)
                    ////////////////////////////////////////////////////////////////////////////////////////////

                    touchFile(configFilename);

                    if (mutexFilename) mutexFilename->unlock();



                    ////////////////////////////////////////////////////////////////////////////////////////////
                    // DETERMINE EVALUATION SETTINGS
                    ////////////////////////////////////////////////////////////////////////////////////////////
                    channels=reader->inputChannels();
                    file_duration=reader->measurementDuration();
                    starttime=0;
                    if (job.range_min>0) starttime=qMin(job.range_min, file_duration);
                    range_duration=file_duration-starttime;
                    if (job.range_max>0) range_duration=qMin(job.range_max, file_duration)-starttime;
                    if (range_duration<0) range_duration=0;
                    countrate_items=0;

                                        ////////////////////////////////////////////////////////////////////////////////////////////
                    // OPEN COUNTRATE FILE
                    ////////////////////////////////////////////////////////////////////////////////////////////
                    QFile crFile(crFilenameBin);
                    if (job.doCountrate) {
                        countrate_items=uint64_t(ceil(range_duration/job.countrate_binning));
                        if (crFile.open(QIODevice::WriteOnly)) {
                            addFiles.append(qMakePair(crFilenameBin, QString("photoncounts_binary")));
                            crFile.write("QF3.0CNTRT");
                            binfileWriteUint16(crFile, channels);
                            binfileWriteUint64(crFile, countrate_items);
                            binfileWriteDouble(crFile, job.countrate_binning);
                        } else {
                            m_status=-1; emit statusChanged(m_status);
                            emit messageChanged(tr("could not create countrate file '%1': %2!").arg(crFilenameBin).arg(crFile.errorString()));
                            crFile.close();
                        }
                    }



                    ////////////////////////////////////////////////////////////////////////////////////////////
                    // RUN THE EVALUATION
                    ////////////////////////////////////////////////////////////////////////////////////////////
                    if ((m_status==1) && !was_canceled) {
                        runEval(reader, &crFile);

                        if (crFile.isOpen()) crFile.close();
                    }



                    ////////////////////////////////////////////////////////////////////////////////////////////
                    // SAVE THE RESULTS
                    ////////////////////////////////////////////////////////////////////////////////////////////

                    //************** SAVE ACF
                    /*if ((m_status==1) && !was_canceled && job.acf && acf && acf_tau && acf_N>0) {
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
                    emit progressIncrement(10);*/

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
                            text<<"input filetype              : "<<d.relativeFilePath(reader->formatName()) << "\n";
                            //if (!acfFilename.isEmpty())             text<<"autocorrelation file        : " << d.relativeFilePath(acfFilename) << "\n";
                            //if (!acfFilenameBin.isEmpty())          text<<"bin. autocorrelation file   : " << d.relativeFilePath(acfFilenameBin) << "\n";
                            text<<"range start                 : "<<outLocale.toString(starttime) << "\n";
                            text<<"range duration              : "<<outLocale.toString(range_duration) << "\n";
                            if (job.doFCS) {
                                text<<"FCS: segments               : "<<outLocale.toString(job.fcs_segments) << "\n";
                                text<<"FCS: correlator S           : "<<outLocale.toString(job.S) << "\n";
                                text<<"FCS: correlator m           : "<<outLocale.toString(job.m) << "\n";
                                text<<"FCS: correlator P           : "<<outLocale.toString(job.P) << "\n";
                                text<<"FCS: correlator type        : "<<outLocale.toString(job.correlator) << "\n";
                                text<<"FCS: correlator type name   : ";
                                switch(job.correlator) {
                                    case CORRELATOR_MTAUALLMON:  text<<"multi-tau with monitors for all channels\n"; break;
                                    case CORRELATOR_MTAUONEMON:  text<<"multi-tau with a single monitor\n"; break;

                                    default: text<<"FCS: correlator type name        : unknown\n"; break;
                                }
                                text<<"FCS: smallest tau [s]            : "<<outLocale.toString(job.fcs_taumin) << "\n";
                                text<<"FCS: count rate binning [s]      : "<<outLocale.toString(job.fcs_crbinning) << "\n";
                            }
                            if (job.doCountrate) {
                                text<<"PhotonsCounts: binning [s]       : "<<outLocale.toString(job.countrate_binning) << "\n";
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

void QFETCSPCImporterJobThread::runEval(QFTCSPCReader *reader,  QFile* countrateFile) {
    double nextcrInterval=job.countrate_binning;
    uint16_t* countrate=(uint16_t*)calloc(channels, sizeof(uint16_t));
    uint64_t crCounter=0;
    double pos=0;
    do {
        QFTCSPCRecord record=reader->getCurrentRecord();
        double t=record.absoluteTime()-starttime;
        int c=record.input_channel;
        //qDebug()<<c<<t;
        if (t>=0 && t<=range_duration) {

            // PROCESS COUNTRATE
            if (job.doCountrate) {
                if (t<nextcrInterval) {
                    countrate[c]++;
                } else if (crCounter<countrate_items) {
                    int64_t emptyrecords=(int64_t)floor((t-nextcrInterval)/job.countrate_binning)+1;
                    //qDebug()<<emptyrecords;
                    for (register uint64_t i=0; i<channels*emptyrecords; i++) {
                        binfileWriteUint16(*countrateFile, countrate[i%channels]);
                        countrate[i%channels]=0;
                    }

                    countrate[c]++;
                    crCounter+=emptyrecords;
                    nextcrInterval=nextcrInterval+emptyrecords*job.countrate_binning;
                    if (crCounter>=countrate_items) nextcrInterval=2*range_duration;
                }
            }
        }
        if (reader->percentCompleted()-pos>0.1) {
            emit progressIncrement(1);
            pos=reader->percentCompleted();
        }
    } while (reader->nextRecord() && (m_status==1) && (!was_canceled));
    free(countrate);
}



