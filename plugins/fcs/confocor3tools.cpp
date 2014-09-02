/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "confocor3tools.h"
#include <QDebug>
#include <QFileInfo>
#include <QDateTime>

#define MAX_OLD_ITEMS 25
QList<Confocor3Tools::ConfocorDataset> Confocor3Tools::oldData=QList<Confocor3Tools::ConfocorDataset>();

Confocor3Tools::Confocor3Tools(const QString &filename)
{
    data.clear();
    bool found=false;
    for (int i=0; i<oldData.size(); i++) {
        if (QFileInfo(filename)==QFileInfo(oldData[i].filename)) {
            data=oldData[i];
            found=true;
            break;
        }
    }

    if (!found) {
        loadFile(data, filename);
    }
}

Confocor3Tools::Confocor3Tools()
{
    data.clear();
}

bool Confocor3Tools::loadFile(const QString &filename)
{

    data.clear();
    bool found=false;
    for (int i=0; i<oldData.size(); i++) {
        if (QFileInfo(filename)==QFileInfo(oldData[i].filename)) {
            data=oldData[i];
            found=true;
            break;
        }
    }

    if (!found) {
        return loadFile(data, filename);
    }

    return true;
}

bool Confocor3Tools::loadFile(Confocor3Tools::ConfocorDataset &data, const QString &filename)
{

    clearErrors();
    QFile f(filename);
    if (f.open(QFile::ReadOnly|QFile::Text)) {
        data.clear();
        data.filename=filename;

        QString line=f.readLine().trimmed().simplified();
        if (line.toLower().startsWith("carl zeiss confocor3")) {
            readBlock(0, data, f, true);
        } else {
            setError(QObject::tr("Did not find ConfoCor3 header in file '%1'. Header was: '%2'").arg(filename).arg(line));
        }


        f.close();

        // try to group FCS records
        for (int i=0; i<data.fcsdatasets.size(); i++) {
            FCSDataSet& ds=data.fcsdatasets[i];
            if (ds.group.isEmpty()) {
                //QString acq=ds.acqtime;
                //if (acq.isEmpty()) {
                //    acq=QFileInfo(data.filename).created().toString("hh:mm:ss M/d/yyyy");
                //}
                //ds.group=QString("P%1_K%2_D%3").arg(ds.position).arg(ds.kinetic).arg(acq.replace(QRegExp("\\s"), "-"));
                ds.group=QString("P%1_K%2").arg(ds.position).arg(ds.kinetic);
                ds.role="ACF0";
                if (ds.type==fdtACF) {
                    ds.role=QString("ACF");
                    if (ds.channelNo>=0) ds.role=QString("ACF%1").arg(ds.channelNo);
                } else if (ds.type==fdtCCF) {
                    ds.role=QString("FCCS");
                    if (ds.channelNo>=0 && ds.channelNo2>=0) ds.role=QString("FCCS%1%2").arg(ds.channelNo).arg(ds.channelNo2);
                    if (ds.role=="FCCS01") ds.role=QString("FCCS");
                }
            }
        }

        // in FCCS records, no counts are save, but they can be extracted from the according ACFs.
        // Therefore we assign the acfs to the FCCS records:
        for (int i=0; i<data.fcsdatasets.size(); i++) {
            FCSDataSet& ds=data.fcsdatasets[i];

            for (int j=0; j<data.fcsdatasets.size(); j++) {
                if (i!=j) {
                    const FCSDataSet& ds2=data.fcsdatasets[j];
                    if (ds.group==ds2.group && ds.repetition==ds2.repetition && ds.acqtime==ds2.acqtime) {
                        if (ds.type==fdtCCF) {
                            if (ds2.type==fdtACF) {
                                if (ds2.channelNo==ds.channelNo) {
                                    ds.recCnt1=j;
                                } else if (ds2.channelNo==ds.channelNo2) {
                                    ds.recCnt2=j;
                                }
                            }
                        } else if (ds.type==fdtACF) {
                            if (ds2.type==fdtACF) {
                                if (ds2.channelNo<ds.channelNo) {
                                    ds.recCnt1=j;
                                } else if (ds2.channelNo>ds.channelNo) {
                                    ds.recCnt2=j;
                                }
                            }
                        }
                    }
                }
            }
            if (ds.channelNo>ds.channelNo2) {
                ds.reverseFCCS=true;
                qSwap(ds.recCnt1, ds.recCnt2);
            }
        }


        // finally add data object to OLD DATA STORE

        for (int i=oldData.size()-1; i>=0; i--) {
            if (QFileInfo(filename)==QFileInfo(oldData[i].filename)) {
                oldData.removeAt(i);
            }
        }
        while (oldData.size()>=MAX_OLD_ITEMS) {
            oldData.removeFirst();
        }
        oldData.append(data);
        return wasError();
    } else {
        setError(QObject::tr("could not open ConfoCor3 file '%1'. Reason: %2").arg(filename).arg(f.errorString()));
    }
    return false;
}

void Confocor3Tools::readBlock(int level, Confocor3Tools::ConfocorDataset &data, QIODevice &f, bool readNewLine, const QString lastLine, Confocor3Tools::FCSDataSet* fcsds)
{
    QString ll=lastLine.trimmed().simplified();
    if (readNewLine) ll=f.readLine().trimmed().simplified();
    while (!ll.toUpper().startsWith("BEGIN")) {
        ll=f.readLine().trimmed().simplified();
    }
    QStringList items=ll.split(' ');
    //qDebug()<<QString(level*2, ' ')<<"BEGIN"<<items.value(1, "---")<<items.value(2, "---");
    bool done=false;
    bool isFCSDataSet=(items.value(1,"").toLower().trimmed()=="fcsdataset");
    Confocor3Tools::FCSDataSet fcs_local;
    Confocor3Tools::FCSDataSet* fcs=fcsds;
    if (isFCSDataSet) {
        fcs=&fcs_local;
        fcs->id=items.value(2,"").toLower().trimmed();
    }
    bool readNextLine=true;
    QRegExp rxProp("\\s*([^\\s]+)\\s*\\=\\s*(.*)");
    while (!done && !f.atEnd()) {
        if (readNextLine) ll=f.readLine().trimmed().simplified();
        readNextLine=true;


        if (ll.startsWith("BEGIN")) {
            readBlock(level+1, data, f, false, ll, fcs);
        } else if (ll.startsWith("END")) {
            done=true;
        } else {

            //rxProp.setMinimal(true);
            if (rxProp.exactMatch(ll)) {
                QString n=rxProp.cap(1).trimmed().simplified();
                QString v=rxProp.cap(2).trimmed().simplified();
                //qDebug()<<QString(level*2, ' ')<<n<<" = "<<v;
                QString norig=n;
                n=n.toLower();
                int vi=v.toInt();
                //int vd=QStringToDouble(v);

                if (level==0) {
                    if (n=="name") data.name=v;
                    else if (n=="comment") data.comment=v;
                    else if (n=="sortorder") data.sortorder=v;
                } else {
                    if (n=="position") {
                        if (vi>=0 && fcs && isFCSDataSet) fcs->position=vi;
                    } else if (n=="kinetics") {
                        if (vi>=0 && fcs && isFCSDataSet) fcs->kinetic=vi;
                    } else if (n=="repetition") {
                        if (vi>=0 && fcs && isFCSDataSet) fcs->repetition=vi;
                    } else if (n=="channel") {
                        if (fcs && isFCSDataSet) fcs->channel=v;
                    } else if (n=="rawdata") {
                        if (fcs && isFCSDataSet) fcs->rawdata=v;
                    } else if (n=="acquisitiontime") {
                        if (fcs && isFCSDataSet) fcs->acqtime=v;
                    } else if (n=="countratearray") {
                        if (fcs && isFCSDataSet) {
                            QStringList lst=v.split(' ');
                            int lines=lst.value(0,"0").toInt();
                            int cols=lst.value(1,"0").toInt();
                            if (lines>0 && cols>0) {
                                ll=readArray(f, lines, cols, readNextLine, fcs->rate);
                                if (fcs->rate.size()>1) {
                                    fcs->time=fcs->rate[0];
                                    fcs->rate.removeFirst();
                                }
                            }
                        }
                    } else if (n=="correlationarray") {
                        if (fcs && isFCSDataSet) {
                            QStringList lst=v.split(' ');
                            int lines=lst.value(0,"0").toInt();
                            int cols=lst.value(1,"0").toInt();
                            if (lines>0 && cols>0) {
                                ll=readArray(f, lines, cols, readNextLine, fcs->corr);
                                if (fcs->corr.size()>1) {
                                    fcs->tau=fcs->corr[0];
                                    fcs->corr.removeFirst();
                                }
                            }
                        }
                    } else if (n=="photoncounthistogramarray") {
                        if (fcs && isFCSDataSet) {
                            QStringList lst=v.split(' ');
                            int lines=lst.value(0,"0").toInt();
                            int cols=lst.value(1,"0").toInt();
                            if (lines>0 && cols>0) {
                                ll=readArray(f, lines, cols, readNextLine, fcs->pch);
                                if (fcs->pch.size()>1) {
                                    fcs->pch_photons=fcs->pch[0];
                                    fcs->pch.removeFirst();
                                }
                            }
                        }
                    } else {
                        //qDebug()<<n<<v;
                        if (fcs&&isFCSDataSet) {
                            fcs->props[norig]=v;
                        }
                    }
                }
            }
        }
    }
    if (isFCSDataSet && fcs_local.kinetic>=0 && fcs_local.position>=0 && fcs_local.repetition>=0) {
        QString ch=fcs_local.channel.toLower().trimmed();
        QRegExp rxDet("detector\\s*(meta)*\\s*(\\d+)");
        if (ch.contains("auto-correlation")) {
            fcs_local.type=fdtACF;
            if (rxDet.indexIn(ch)>=0) {
                bool ok=false;
                int ii=rxDet.cap(2).toInt(&ok);
                if (ok) fcs_local.channelNo=ii-1;
            }
        } else if (ch.contains("cross-correlation")) {
            fcs_local.type=fdtCCF;
            int idx=0;
            if ((idx=rxDet.indexIn(ch, idx))>=0) {
                bool ok=false;
                int ii=rxDet.cap(2).toInt(&ok);
                if (ok) fcs_local.channelNo=ii-1;
                idx=idx+rxDet.matchedLength();
            }
            if ((idx=rxDet.indexIn(ch, idx))>=0) {
                bool ok=false;
                int ii=rxDet.cap(2).toInt(&ok);
                if (ok) fcs_local.channelNo2=ii-1;
            }
        }
        data.fcsdatasets.append(fcs_local);        
        //qDebug()<<"!!! ADDED FCS DATASET   P="<<fcs_local.position<<" K="<<fcs_local.kinetic<<" R="<<fcs_local.repetition<<"   Channel="<<fcs_local.channel;
    }
}

QString Confocor3Tools::readArray(QIODevice &f, int lines, int cols, bool &readNextLine, QList<QVector<double> > &dataout)
{
    int l=0;
    bool done=false;
    dataout.clear();
    for (int c=0; c<cols; c++) dataout.append(QVector<double>(lines, 0.0));
    int lc=0;
    QRegExp rxProp("\\s*([^\\s]+)\\s*\\=\\s*(.*)");
    QString ll;
    while (l<lines && !done && !f.atEnd()) {
        ll=f.readLine().trimmed().simplified();
        /*if (rxProp.exactMatch(ll)) {
            readNextLine=false;
            done=true;
        } else {
            QStringList it=ll.split(' ');
            if (it.size()>=cols) {
                for (int c=0; c<cols; c++) dataout[c].operator [](lc)=QStringToDouble(it[c]);
                lc++;
            }
        }*/
        if (ll.size()>0 && (ll[0].isDigit() || ll[0]=='.' || ll[0]=='-' || ll[0]=='+')) {
            QStringList it=ll.split(' ');
            if (it.size()>=cols) {
                for (int c=0; c<cols; c++) dataout[c].operator [](lc)=QStringToDouble(it[c]);
                lc++;
            }
        } else {
            readNextLine=false;
            done=true;
        }
        l++;
    }
    if (lc<lines) {
        for (int c=0; c<cols; c++) dataout[c].resize(lc);
    }
    return ll;
}

void Confocor3Tools::clearStore()
{
    oldData.clear();
}


QStringList Confocor3Tools::ConfocorDataset::getGroups() const
{
    QStringList g;
    for (int i=0; i<fcsdatasets.size(); i++) {
        const QString gg=fcsdatasets[i].group;
        if (!g.contains(gg)) g.append(gg);
    }
    return g;
}

QList<int> Confocor3Tools::ConfocorDataset::getKinetics() const
{
    QStringList g;
    QList<int> res;
    for (int i=0; i<fcsdatasets.size(); i++) {
        const QString gg=fcsdatasets[i].group;
        if (!g.contains(gg)) {
            g.append(gg);
            res.append(fcsdatasets[i].kinetic);
        }
    }
    return res;
}

QList<int> Confocor3Tools::ConfocorDataset::getPositions() const
{
    QStringList g;
    QList<int> res;
    for (int i=0; i<fcsdatasets.size(); i++) {
        const QString gg=fcsdatasets[i].group;
        if (!g.contains(gg)) {
            g.append(gg);
            res.append(fcsdatasets[i].position);
        }
    }
    return res;
}

QStringList Confocor3Tools::ConfocorDataset::getRoles() const
{
    QStringList g;
    for (int i=0; i<fcsdatasets.size(); i++) {
        const QString gg=fcsdatasets[i].role;
        if (!g.contains(gg)) g.append(gg);
    }
    return g;
}

QList<QVector<int> > Confocor3Tools::ConfocorDataset::getRepetitions() const
{
    QList<QVector<int> > reps;
    QStringList grps=getGroups();
    QSet<int> used;
    for (int i=0; i<grps.size(); i++) {
        QVector<int> rep;
        QString g=grps[i];
        for (int d=0; d<fcsdatasets.size(); d++) {
            if (!used.contains(d) && fcsdatasets[d].group==g) {
                rep.append(d);
                used.insert(d);
            }
        }
        reps.append(rep);
    }

    return reps;
}

void Confocor3Tools::ConfocorDataset::clear()
{
    name.clear();
    comment.clear();
    sortorder.clear();
    filename.clear();
    fcsdatasets.clear();
}
