#include "confocor3tools.h"
#include <QDebug>

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
    return loadFile(data, filename);
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

void Confocor3Tools::readBlock(int level, Confocor3Tools::ConfocorDataset &data, QFile &f, bool readNewLine, const QString lastLine, Confocor3Tools::FCSDataSet* fcsds)
{
    QString ll=lastLine.trimmed().simplified();
    if (readNewLine) ll=f.readLine().trimmed().simplified();
    while (!ll.toUpper().startsWith("BEGIN")) {
        ll=f.readLine().trimmed().simplified();
    }
    QStringList items=ll.split(" ");
    qDebug()<<QString(level*2, ' ')<<"BEGIN"<<items.value(1, "---")<<items.value(2, "---");
    bool done=false;
    bool isFCSDataSet=(items.value(1,"").toLower().trimmed()=="fcsdataset");
    Confocor3Tools::FCSDataSet fcs_local;
    Confocor3Tools::FCSDataSet* fcs=fcsds;
    if (isFCSDataSet) fcs=&fcs_local;
    while (!done && !f.atEnd()) {
        ll=f.readLine().trimmed().simplified();
        if (ll.startsWith("BEGIN")) {
            readBlock(level+1, data, f, false, ll, fcs);
        } else if (ll.startsWith("END")) {
            done=true;
        } else {
            QRegExp rxProp("\\s*([^\\s]+)\\s*\\=\\s*(.*)");
            //rxProp.setMinimal(true);
            if (rxProp.exactMatch(ll)) {
                QString n=rxProp.cap(1).trimmed().simplified();
                QString v=rxProp.cap(2).trimmed().simplified();
                //qDebug()<<QString(level*2, ' ')<<n<<" = "<<v;
                QString norig=n;
                n=n.toLower();
                int vi=v.toInt();
                int vd=v.toDouble();
                if (level==0) {
                    if (n=="name") data.name=v;
                    else if (n=="comment") data.comment=v;
                    else if (n=="sortorder") data.sortorder=v;
                } else {
                    if (n=="position") {
                        if (vi>=0 && !data.positions.contains(vi)) data.positions.append(vi);
                        if (vi>=0 && fcs && isFCSDataSet) fcs->position=vi;
                    } else if (n=="kinetics") {
                        if (vi>=0 && !data.kinetics.contains(vi)) data.kinetics.append(vi);
                        if (vi>=0 && fcs && isFCSDataSet) fcs->kinetic=vi;
                    } else if (n=="repetition") {
                        if (vi>=0 && !data.repetitions.contains(vi)) data.repetitions.append(vi);
                        if (vi>=0 && fcs && isFCSDataSet) fcs->repetition=vi;
                    } else if (n=="channel") {
                        if (fcs && isFCSDataSet) fcs->channel=v;
                    } else if (n=="rawdata") {
                        if (fcs && isFCSDataSet) fcs->rawdata=v;
                    } else if (n=="acquisitiontime") {
                        if (fcs && isFCSDataSet) fcs->acqtime=v;
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
        data.fcsdatasets.append(fcs_local);
        qDebug()<<"!!! ADDED FCS DATASET   P="<<fcs_local.position<<" K="<<fcs_local.kinetic<<" R="<<fcs_local.repetition<<"   Channel="<<fcs_local.channel;
    }
}

void Confocor3Tools::clearStore()
{
    oldData.clear();
}


QString Confocor3Tools::ConfocorDataset::getID(int position, int kinetic, int repetition, int channel, Confocor3Tools::ConfocorDataset::DataType type)
{
    return QString("%1_%2_%3_%4_%5").arg(position).arg(kinetic).arg(repetition).arg(channel).arg((int)type);
}

void Confocor3Tools::ConfocorDataset::clear()
{
    name.clear();
    comment.clear();
    sortorder.clear();
    filename.clear();
    fcsdatasets.clear();
    positions.clear();
    kinetics.clear();
    repetitions.clear();
    channels.clear();
    hasPCH=false;
    hasFCS=false;
    hasCountrate=false;
    channelNames.clear();
    datacolumns.clear();
    datacolumnid.clear();
}
