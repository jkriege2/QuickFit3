/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#include "qfpicoquanttexttcspcimporter.h"
#include <QObject>
#include <QDebug>
#include "qftools.h"
#include "qfpluginservices.h"



QFPicoquantTextTCSPCImporter::QFPicoquantTextTCSPCImporter() {
    currentTTTRRecordNum=0;
    //temporalResolutionSeconds=6e-9;
    //lastTTTRMacroTime=0;
    mtResolutionPS=154.727;
    mtChannels=65535;
    duration=0;
    inChannels=4;
    timeFactorXToS=1;
    idxTime=2;
    idxChannel=4;
    idxRoute=3;
}

QFPicoquantTextTCSPCImporter::~QFPicoquantTextTCSPCImporter() {
    tttrfile.close();
}

QString QFPicoquantTextTCSPCImporter::filter() const {
    return QObject::QObject::tr("Text TCSPC file [PicoQuant Format] (*.txt)");
}

QString QFPicoquantTextTCSPCImporter::formatName() const {
    return QObject::QObject::tr("Text TCSPC file [PicoQuant Format]");
}

bool QFPicoquantTextTCSPCImporter::open(const QString &filename, const QString &/*parameters*/) {
    close();
    fileinfo.init(filename);
    tttrfile.setFileName(filename);
    tttrfile.open(QFile::ReadOnly|QFile::Text);
    mtResolutionPS=154;
    mtChannels=65535;
    fileResetLine=0;
    recordsInTTTRFile=0;
    timeFactorXToS=1;
    idxTime=2;
    idxChannel=4;
    idxRoute=3;
    inChannels=4;
    qint64 datastartfilepos=0;


    if (tttrfile.isOpen()) {
        QString error="";
        bool inHeader=true;
        bool dataHeaderFound=false;
        QRegExp rxc("\\s*([a-zA-Z][^\\:\\=]*)(\\=\\s+|\\:\\s+|\\s\\s)([^\\:\\=\\s].*)");
        QRegExp rxd("([\\.\\deE\\+\\-]+\\s+|cha\\s+|mar\\s+|ofl\\s+)+[\\.\\deE\\+\\-]+");
        rxd.setCaseSensitivity(Qt::CaseInsensitive);
        uint64_t l=0;
        int spacesepcnt=0;
        int colonsepcount=0;
        int equalssepcount=0;
        while (!tttrfile.atEnd() && inHeader) {
            const QByteArray line=tttrfile.readLine().trimmed();
            QString lines=line;
            QString linell=lines.toLower().simplified().trimmed();
            if (inHeader) {
                bool rxcMatch=rxc.exactMatch(lines);
                bool potentialHeaderLine=(
                            linell.contains("record#")
                             || (
                                   (linell.contains("timetag") || linell.contains("time/") || linell.contains("truetime"))
                                   && (linell.contains("route") || linell.contains("chan ")|| (linell.contains("channel")||linell.contains("dtime")))
                                )
                         );
                if (!potentialHeaderLine && rxcMatch) {
                    if (colonsepcount>spacesepcnt && colonsepcount>equalssepcount && colonsepcount>1) {
                        if (rxc.cap(2)!=":") potentialHeaderLine=true;
                    }
                    if (equalssepcount>spacesepcnt && equalssepcount>colonsepcount && equalssepcount>1) {
                        if (rxc.cap(2)!="=") potentialHeaderLine=true;
                    }
                }

                //qDebug()<<lines<<"\n"<<linell<<potentialHeaderLine << rxcMatch << rxc.cap(1)<< rxc.cap(2)<< rxc.cap(3);

                if (!potentialHeaderLine && rxcMatch && rxc.cap(1).trimmed().size()>0 && rxc.cap(3).trimmed().size()>0) {
                    QString name=rxc.cap(1).trimmed();
                    QString value=rxc.cap(3).trimmed();
                    if (value.startsWith('=')) value.right(value.size()-1);
                    if (value.startsWith(':')) value.right(value.size()-1);
                    if (name.endsWith('=')) name.left(name.size()-1);
                    if (name.endsWith(':')) name.left(name.size()-1);
                    if (name.endsWith(':')) name.left(name.size()-1);
                    if (rxc.cap(2)=="=") equalssepcount++;
                    if (rxc.cap(2)==":") colonsepcount++;
                    if (rxc.cap(2).simplified().trimmed().size()==0) spacesepcnt++;
                    value=value.trimmed();
                    name=name.trimmed();
                    fileinfo.properties[name]=qfStringToVariantAutoRecognizeType(value);
                    //QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: found name/value: %1 = %2\n").arg(name).arg(value));
                    qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: found name/value: '%1' => '%2'\n").arg(name).arg(value);
                    name=name.toLower().simplified().trimmed();
                    if (((name=="no of channels") || (name=="no. of channels") || (name=="number of channels")) && (QStringToDouble(value)>0)) {
                        mtChannels=QStringToDouble(value);
                        QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: read mtChannels = %1\n").arg(mtChannels));
                        qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: read mtChannels = %1\n").arg(mtChannels);
                    }
                    if (((name=="resolution")) && (QStringToDouble(value)>0)) {
                        mtResolutionPS=QStringToDouble(value)*1e3;
                        QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: mtResolutionPS = %1 ps\n").arg(mtResolutionPS));
                        qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: mtResolutionPS = %1 ps\n").arg(mtResolutionPS);
                    }
                    if (((name=="resolution/ns")) && (QStringToDouble(value)>0)) {
                        mtResolutionPS=QStringToDouble(value)*1e3;
                        QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: mtResolutionPS = %1 ps\n").arg(mtResolutionPS));
                        qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: mtResolutionPS = %1 ps\n").arg(mtResolutionPS);
                    }
                    if (((name=="routing channels") || (name=="input channels") || (name=="routingchannels") || (name=="inputchannels") || (name=="hwrouter_channels") || (name=="HWRouter_Channels")) && (QStringToDouble(value)>0)) {
                        inChannels=QStringToDouble(value);
                        QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: read inChannels = %1\n").arg(inChannels));
                        qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: read inChannels = %1\n").arg(inChannels);
                    }
                    if (((name=="acquisitiontime") || (name=="acquisition time") || (name=="acquisition duration")|| (name=="acquisitionduration") || (name=="measdesc_acquisitiontime") || (name=="MeasDesc_AcquisitionTime") || name.contains("acquisitiontime")) && (value.toInt()>0 || QStringToDouble(value)>0)) {

                        duration=QStringToDouble(value)*1e-3;
                        QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: read duration = %1\n").arg(duration));
                        qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: read duration = %1\n").arg(duration);
                    }
                } else if (lines.size()>0) {
                    if (rxd.indexIn(lines)>=0 && rxd.matchedLength()>lines.size()/3) {
                        fileResetLine=l;
                        datastartfilepos=tttrfile.pos();
                        inHeader=false;
                        QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: found data section start in line %1\n").arg(fileResetLine));
                        qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: found data section start in line %1\n").arg(fileResetLine);
                        break;
                    } else {
                        if (!dataHeaderFound && potentialHeaderLine) {
                            lines=lines.toLower();
                            lines=lines.remove("chn", Qt::CaseInsensitive);
                            lines=lines.simplified();
                            if (lines.contains("time/s") || lines.contains("time/sec")) {
                                timeFactorXToS=1;
                                dataHeaderFound=true;
                            } else if (lines.contains("time/ns")) {
                                timeFactorXToS=1e-9;
                                dataHeaderFound=true;
                            } else if (lines.contains("time/ms")) {
                                timeFactorXToS=1e-3;
                                dataHeaderFound=true;
                            } else if (lines.contains("time/us")) {
                                timeFactorXToS=1e-6;
                                dataHeaderFound=true;
                            }
                            QStringList hl=lines.split(' ');
                            idxRoute=hl.indexOf("route");
                            idxChannel=hl.indexOf("channel");
                            for (int i=0; i<hl.size(); i++) {
                                if (hl[i]=="route" || hl[i]=="chan") {
                                    idxRoute=i;
                                    dataHeaderFound=true;
                                } else if (hl[i]=="channel"||hl[i]=="dtime") {
                                    idxChannel=i;
                                    dataHeaderFound=true;
                                } else if (hl[i].contains("time") && hl[i]!="timetag") {
                                    idxTime=i;
                                    dataHeaderFound=true;
                                } else if (hl[i].contains("truetime")) {
                                    idxTime=i;
                                    dataHeaderFound=true;
                                }
                            }
                            QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: found dataHeader idxRoute = %1   idxChannel = %2   idxTime = %3   timeFactorXToS = %4\n").arg(idxRoute).arg(idxChannel).arg(idxTime).arg(timeFactorXToS));
                            qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: found dataHeader idxRoute = %1   idxChannel = %2   idxTime = %3   timeFactorXToS = %4\n").arg(idxRoute).arg(idxChannel).arg(idxTime).arg(timeFactorXToS)<<lines<<hl<<"\n\n";


                        }

                    }
                }
            } else if (!lines.isEmpty() && !lines.contains("ofl")  && !lines.contains("mar")) {
                //recordsInTTTRFile++;
                inHeader=false;
                break;
            }
            l++;
//            if (l%1000==0) {
//                QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: pre-reading line %1\n").arg(l));
//                qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: pre-reading line %1\n").arg(l);
//            }
        }

        tttrfile.seek(tttrfile.size()-2000);
        tttrfile.readLine();
        double avglen=0;
        double cnt=0;
        while (!tttrfile.atEnd()) {
            QString l=tttrfile.readLine();
            avglen=avglen+l.size();
            cnt++;
        }
        avglen=avglen/cnt;
        recordsInTTTRFile=ceil(double(tttrfile.size()-datastartfilepos)/avglen);

        if (recordsInTTTRFile<10) {
            setLastError(QObject::QObject::tr("no records found in TTTR file '%1'").arg(filename));
            close();
            return false;
        }

        //QFPluginServices::getInstance()->log_text(QObject::tr("QFPicoquantTextTCSPCImporter: recordsInTTTRFile = %1   fileResetLine = %2\n").arg(recordsInTTTRFile).arg(fileResetLine));
        //qDebug()<<QObject::tr("QFPicoquantTextTCSPCImporter: recordsInTTTRFile = %1   fileResetLine = %2\n").arg(recordsInTTTRFile).arg(fileResetLine);

        reset();
        currentTTTRRecordNum=0;

        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=mtResolutionPS*1e-3;
        nextRecord();

        // read some photons to estimate a countrate
        cr.clear();
        double t=0;
        long cnt1=0;
        do {
            QFTCSPCRecord r=getCurrentRecord();
            t=r.absoluteTime();
            if (r.isPhoton) {
                //qDebug()<<r.input_channel<<t;
                cr[r.input_channel]=cr.value(r.input_channel, 0)+1;
                cnt1++;
            }
        } while (nextRecord() && (t<1) && (cnt1<5000));
        for (int i=0; i<inputChannels(); i++) {
            cr[i]=cr.value(i, 0)/t/1000.0;
            //qDebug()<<"cr["<<i<<"]="<<cr[i];
        }
        return true;

    }
    setLastError(QObject::QObject::tr("could not open TTTR file '%1'").arg(filename));
    return false;
}

bool QFPicoquantTextTCSPCImporter::isOpenParametersUsed( QString */*optionsDescription*/) const
{
    //if (optionsDescription) *optionsDescription=QObject::QObject::tr("temporal resolution [ns]");
    //return true;
    return false;
}

void QFPicoquantTextTCSPCImporter::close() {
    tttrfile.close();
    //fileResetPos=0;
    currentTTTRRecordNum=0;
    //lastTTTRMacroTime=0;
}

void QFPicoquantTextTCSPCImporter::reset() {
    tttrfile.reset();
    for (uint64_t i=0; i<fileResetLine; i++) {
        tttrfile.readLine();
    }
    currentTTTRRecordNum=0;
    //lastTTTRMacroTime=initialTTTRMacroTime;
    nextRecord();

}

bool QFPicoquantTextTCSPCImporter::nextRecord() {
    //uint64_t TTTRrecord;
    bool ok=false;
    while (!tttrfile.atEnd() && !ok) {
        const QByteArray line=tttrfile.readLine().trimmed();
        QString lines=line.simplified().toLower().trimmed();
        if (lines.size()>0) {
            if (!lines.contains("ofl")  && !lines.contains("mar")) {
                lines=lines.remove("chn");
                const QStringList hl=lines.split(' ');
                if ((idxTime<hl.size()) && (idxChannel<hl.size()||idxChannel<0) && (idxRoute<hl.size()||idxRoute<0)) {
                    current.isPhoton=true;
                    current.marker_type=0;
                    current.macrotime=QStringToDouble(hl[idxTime])*timeFactorXToS;
                    //qDebug()<<hl<<hl[idxTime]<<timeFactorXToS<<current.macrotime;

                    if (idxRoute>=0) current.input_channel=hl[idxRoute].toInt();
                    else current.input_channel=0;

                    if (idxChannel>=0) current.microtime_channel=hl[idxChannel].toInt();
                    else current.microtime_channel=0;

                    ok=true;
                    currentTTTRRecordNum++;
                    break;
                }
            }
        }
    }

    return ((currentTTTRRecordNum<recordsInTTTRFile) && !tttrfile.atEnd());
}

double QFPicoquantTextTCSPCImporter::measurementDuration() const {
    return duration;
}

uint16_t QFPicoquantTextTCSPCImporter::inputChannels() const {

    return inChannels;
}

uint32_t QFPicoquantTextTCSPCImporter::microtimeChannels() const
{
    return mtChannels;
}

double QFPicoquantTextTCSPCImporter::microtimeChannelsResolutionPicoSeconds() const
{
    return mtResolutionPS;
}

double QFPicoquantTextTCSPCImporter::avgCountRate(uint16_t channel) const {
    return cr.value(channel, 0);
}

QFTCSPCRecord QFPicoquantTextTCSPCImporter::getCurrentRecord() const {
    return current;
}

double QFPicoquantTextTCSPCImporter::percentCompleted() const {
    return double(currentTTTRRecordNum)/double(recordsInTTTRFile)*100.0;
}

