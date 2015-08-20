/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)

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

#include "qf3correlationdataformattool.h"

QF3CorrelationDataFormatTool::QF3CorrelationDataFormatTool()
{
    clear();
}

bool QF3CorrelationDataFormatTool::loadFile(const QString &filename, bool propertiesOnly)
{
    clearErrors();
    clear();
    this->filename=filename;
    ok=true;

    QFile f(filename);
    int lcount=0;
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        int section=-1;
        QRegExp rxHeader("\\s*\\[(\\w+)\\]\\s*");
        QRegExp rxRole("role(\\d+)");
        QRegExp rxPC("preferred_channel(\\d+)");
        QRegExp rxCHName("(channel_name|channelname)(\\d+)");
        rxCHName.setCaseSensitivity(Qt::CaseInsensitive);
        QTextCodec* codec=QTextCodec::codecForName("ISO-8859-1");
        while (ok && !f.atEnd()) {
            QString line;
            if (codec) {
                line=codec->toUnicode(f.readLine()).trimmed();
            } else {
                QByteArray l=f.readLine();
                line=QString::fromLatin1(l.data(), l.size()).trimmed();
            }
            lcount++;

            // first we search for a comment character, which has not been escaped with \#
            // if we are in section==0 (properties), the # is also ignored, if it is found within " ... " (which may also be escaped by \")
            int comment=-1;
            bool isQuot=false;
            for (int i=0; i<line.size(); i++) {
                if (section==0) {
                    if (line[i]=='"' && (i<=0 || !isQuot || line[i-1]!='\\')) {
                        isQuot=!isQuot;
                    }
                } else {
                    isQuot=false;
                }
                if (line[i]=='#' && !isQuot && (i<=0 || line[i-1]!='\\')) {
                    comment=i;
                    break;
                }
            }
            // only use the part in front of the comment character
            if (comment>=0) {
                line=line.left(comment).trimmed();
            }
            //qDebug()<<"line "<<lcount<<": "<<line;
            if (line.size()>0) { // we only have to process non-empty lines
                if (rxHeader.exactMatch(line)) { // first we check, whether the line starts a new section ... and recognize the section
                    QString sec=rxHeader.cap(1).trimmed().toLower();                    
                    if (sec=="properties") {
                        section=0;
                    } else if (sec=="correlationdata" || sec=="correlation") {
                        section=1;
                    } else if (sec=="ratedata" || sec=="countratedata" || sec=="rates" || sec=="countrates") {
                        section=2;
                    }
                    //qDebug()<<"   section "<<sec<<section;
                } else {
                    if (section==0) { // properties section
                        QString name, value;
                        bool isName=true;
                        for (int i=0; i<line.size(); i++) {
                            if (isName) {
                                if (line[i]!='=') {
                                    name+=line[i];
                                } else {
                                    isName=false;
                                }
                            } else {
                                value+=line[i];
                            }
                        }
                        name=name.trimmed();
                        value=value.trimmed();
                        if (value.startsWith('"') && value.endsWith('"')) {
                            value=value.mid(1,value.size()-2);
                            QString val;
                            bool skip=false;
                            for (int i=0; i<value.size(); i++) {
                                if (!skip) {
                                    if (value[i]=='\\' && i+1<value.size()) {
                                        if (value[i+1]=='"') { val+='"'; skip=true; }
                                        else if (value[i+1]=='\\') { val+='\\'; skip=true; }
                                        else if (value[i+1]=='n') { val+='\n'; skip=true; }
                                        else if (value[i+1]=='r') { val+='\r'; skip=true; }
                                        else if (value[i+1]=='t') { val+='\t'; skip=true; }
                                        else if (value[i+1]=='\'') { val+='\''; skip=true; }
                                        else {
                                            val+=value[i];
                                        }
                                    } else {
                                        val+=value[i];
                                    }
                                } else {
                                    skip=false;
                                }
                            }
                            value=val;
                        }
                        value=value.trimmed();
                        const QString nameL=name.toLower();
                        //qDebug()<<"   prop "<<name<<value;
                        if (nameL=="codec") {
                            if (value.trimmed().toLower()=="latin-1") {
                                codec=0;
                            } else {
                                QTextCodec* cc=QTextCodec::codecForName(value.toLatin1());
                                if (cc) {
                                    codec=cc;
                                }
                            }
                        } else if (nameL=="group") {
                            group=value;
                        } else if (nameL=="folder") {
                            folder=value;
                        } else if (nameL=="role") {
                            role=value;
                        } else if (nameL=="channel_name" || nameL=="channelname") {
                            channel_name=value;
                        } else if (nameL=="input_file" || nameL=="inputfile") {
                            input_file=value;
                        } else if (nameL=="runs") {
                            runs=value.toInt();
                        } else if (nameL=="rateruns" || nameL=="rate_runs") {
                            rateRuns=value.toInt();
                        } else if (nameL=="channels") {
                            channels=value.toInt();
                        } else if (nameL=="correlations"||nameL=="correlationtypes"||nameL=="correlation_types") {
                            correlationTypes=value.toInt();
                        } else if (rxRole.exactMatch(nameL)) {
                            int idx=rxRole.cap(1).toInt();
                            roles[idx]=value;
                        } else if (rxCHName.exactMatch(nameL)) {
                            int idx=rxCHName.cap(1).toInt();
                            channel_names[idx]=value;
                        } else if (rxPC.exactMatch(nameL)) {
                            int idx=rxPC.cap(1).toInt();
                            preferred_channels[idx]=value.toInt();
                        } else {
                            QRegExp rxInt("[+|-]?\\d+");
                            QRegExp rxDouble("[+|-]?\\d+[\\.]?\\d*[eE]?\\d*");
                            if (rxInt.exactMatch(value)) {
                                properties[name]=value.toInt();
                            } else if (rxDouble.exactMatch(value)) {
                                properties[name]=value.toDouble();
                            } else {
                                properties[name]=value;
                            }

                        }
                    } else if (!propertiesOnly && section==1) { // correlation data section
                        if (correlations.size()<=0) reserveCorrelations(); // if data arrays have to yet been reserved, do so now
                        QTextStream ls(&line);
                        QVector<double> ld=csvReadline(ls, ',', '#', NAN);
                        //qDebug()<<"  corr: "<<ld;
                        if (ld.size()>=correlationTypes*runs+1) {
                            taus.append(ld[0]);
                            int ii=1;
                            for (int c=0; c<correlationTypes; c++) {
                                for (int r=0; r<runs; r++) {
                                    addCorrelationEntry(ld[ii], r, c);
                                    ii++;
                                }
                            }
                        } else if (ld.size()>0){
                            setError(QObject::tr("error in file '%1' while reading correlations: not enough entries in line %2 (%3 columns found, %4 columns expected, correlationtypes=%5, runs=%6)").arg(filename).arg(lcount).arg(ld.size()).arg(correlationTypes*runs+1).arg(correlationTypes).arg(runs));
                            ok=false;
                        }
                    } else if (!propertiesOnly && section==2) { // count-rates section
                        if (countrates.size()<=0) reserveRate(); // if data arrays have to yet been reserved, do so now
                        QTextStream ls(&line);
                        QVector<double> ld=csvReadline(ls, ',', '#', NAN);
                        //qDebug()<<"  rate: "<<ld;
                        if (ld.size()>=channels*rateRuns+1) {
                            times.append(ld[0]);
                            int ii=1;
                            for (int c=0; c<channels; c++) {
                                for (int r=0; r<rateRuns; r++) {
                                    addCountrateEntry(ld[ii], r, c);
                                    ii++;
                                }
                            }
                        } else if (ld.size()>0){
                            setError(QObject::tr("error in file '%1' while reading count rates: not enough entries in line %2 (%3 columns found, %4 columns expected, channels=%5, runs=%6)").arg(filename).arg(lcount).arg(ld.size()).arg(channels*rateRuns+1).arg(channels).arg(rateRuns));
                            ok=false;
                        }
                    }
                }
            }
        }
        if (!propertiesOnly) {

        }
        f.close();
    } else {
        setError(QObject::tr("Could not open file '%1', reason: %2").arg(filename).arg(f.errorString()));
        ok=false;
    }
    return ok;
}

void QF3CorrelationDataFormatTool::saveFile(const QString &filename) const
{
    QTextCodec* codec=QTextCodec::codecForName("ISO-8859-1");
    QFile f(filename);
    if (f.open(QFile::WriteOnly|QFile::Text)) {
        QTextStream str(&f);
        str.setCodec(codec);
        str<<"[Properties]\n";
        if (codec) {
            str<<"codec = "<< codec->name() <<"\n";
        } else {
            str<<"codec = latin-1\n";
        }
        str<<"fileformat_name = QF3ASCIICorrelationData\n";
        str<<"fileformat_version = \"1.0\"\n";
        str<<"group = \""<< qfCEscaped(group) <<"\"\n";
        str<<"folder = \""<< qfCEscaped(folder) <<"\"\n";
        str<<"input_file = \""<< qfCEscaped(QFileInfo(filename).absoluteDir().relativeFilePath(input_file)) <<"\"\n";
        str<<"runs = "<< runs <<"\n";
        str<<"rateRuns = "<< rateRuns <<"\n";
        str<<"channels = "<< channels <<"\n";
        str<<"correlations = "<< correlationTypes <<"\n";
        str<<"role = \""<< qfCEscaped(role) <<"\"\n";
        QMapIterator<int, QString> itr(roles);
        while (itr.hasNext()) {
            itr.next();
            str<<"role"<<itr.key()<<" = \""<< qfCEscaped(itr.value()) <<"\"\n";
        }
        str<<"channel_name = \""<< qfCEscaped(channel_name) <<"\"\n";
        QMapIterator<int, QString> itc(channel_names);
        while (itc.hasNext()) {
            itc.next();
            str<<"channel_name"<<itc.key()<<" = \""<< qfCEscaped(itc.value()) <<"\"\n";
        }

        QMapIterator<int, int> itp(preferred_channels);
        while (itp.hasNext()) {
            itp.next();
            str<<"preferred_channel"<<itp.key()<<" = "<< itp.value() <<"\n";
        }

        QMapIterator<QString, QVariant> it(properties);
        while (it.hasNext()) {
            it.next();
            str<<it.key()<<" = "<< getQVariantDataCEscaped(it.value(), '\"') <<"\n";
        }
        //str<<"group = "<< group <<"\n";
        //str<<"="<<  <<"\n";


        QChar decimalSep='.';
        QString colSep=", ";
        char format='g';
        int prec=10;

        if (correlations.size()>0 && taus.size()>0 && correlationTypes>0 && runs>0) {
            str<<"\n\n[CorrelationData]\n";
            for (int i=0; i<taus.size(); i++) {
                str<<doubleToQString(taus[i], prec, format, decimalSep);
                for (int c=0; c<correlationTypes; c++) {
                    for (int r=0; r<runs; r++) {
                        str<<colSep<<doubleToQString(getCorrelationEntry(i, r, c), prec, format, decimalSep);
                    }
                }
                str<<"\n";
            }
        }


        if (countrates.size()>0 && times.size()>0 && channels>0 && rateRuns>0) {
            str<<"\n\n[RateData]\n";
            for (int i=0; i<times.size(); i++) {
                str<<doubleToQString(times[i], prec, format, decimalSep);
                for (int c=0; c<channels; c++) {
                    for (int r=0; r<rateRuns; r++) {
                        str<<colSep<<doubleToQString(getCountrateEntry(i,r,c), prec, format, decimalSep);
                    }
                }
                str<<"\n";
            }
        }
        str<<"\n";
        f.close();
    }
}

int QF3CorrelationDataFormatTool::getCorrelationTypePreferredChannel(int idx, int defaultVal)
{
    int def=defaultVal;
    QString role=getRole(idx);
    QRegExp rxACF("ACF(\\d+)");
    rxACF.setCaseSensitivity(Qt::CaseInsensitive);
    if (rxACF.exactMatch(role)) {
        int thisACF=rxACF.cap(1).toInt();
        int minA=INT_MAX;
        for (int i=0; i<roles.size(); i++) {
            if (rxACF.exactMatch(roles[i])) {
                minA=qMin(minA, rxACF.cap(1).toInt());
            }
        }
        if (minA==INT_MAX || minA<0) minA=0;
        def=thisACF-minA;
    };
    int c=preferred_channels.value(idx, def);
    return qBound(0, c, channels);
}

void QF3CorrelationDataFormatTool::reserveCorrelations()
{
    correlations.clear();
    QList<QVector<double> > runlst;
    QVector<double> d;
    for (int r=0; r<runs; r++) {
        runlst.append(d);
    }
    for (int c=0; c<correlationTypes; c++) {
        correlations.append(runlst);
    }
}

void QF3CorrelationDataFormatTool::reserveRate()
{
    countrates.clear();
    QList<QVector<double> > runlst;
    QVector<double> d;
    for (int r=0; r<rateRuns; r++) {
        runlst.append(d);
    }
    for (int c=0; c<channels; c++) {
        countrates.append(runlst);
    }
}

void QF3CorrelationDataFormatTool::clear()
{
    clearErrors();
    preferred_channels.clear();
    filename="";
    ok=false;
    correlationTypes=1;
    channels=1;
    runs=0;
    rateRuns=0;
    roles.clear();
    group="";
    folder="";
    input_file="";
    correlations.clear();
    taus.clear();
    times.clear();
    countrates.clear();
    role="";
    channel_name="";
    channel_names.clear();
    properties.clear();
}
