#include "qfrdrrunselectiontools.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>

QFRDRRunSelectionTools::QFRDRRunSelectionTools()
{
}

QFRDRRunSelectionTools::~QFRDRRunSelectionTools()
{

}

void QFRDRRunSelectionTools::leaveoutLoad(const QString &filename)
{
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        leaveoutClear();
        leaveoutLoadFromString(f.readAll());
        f.close();
    }
}

void QFRDRRunSelectionTools::leaveoutLoadFromString(const QString &leaveoutstring)
{
    leaveoutLoadFromIndexString(leaveoutstring);

}

void QFRDRRunSelectionTools::leaveoutSave(const QString &filename) const
{
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream str(&f);
        str<<leaveoutToString();
        f.close();
    }

}

QString QFRDRRunSelectionTools::leaveoutToString() const
{
    return leaveoutToIndexString();

}

QString QFRDRRunSelectionTools::leaveoutToIndexString(QChar separator) const
{
    QString res="";
    bool first=true;
    for (int i=0; i<leaveoutGetRunCount(); i++) {
        if (leaveoutRun(i)) {
            if (!first) res+=separator;
            res+=QString::number(i);
            first=false;
        }
    }
    return res;

}

void QFRDRRunSelectionTools::leaveoutLoadFromIndexString(const QString &data, QChar separator)
{
    QStringList sl=data.split(separator);

    for (int i=0; i<sl.size(); i++) {
        bool ok=false;
        int idx=sl[i].toUInt(&ok);
        if (ok) leaveoutAddRun(idx);
    }
}



void QFRDRRunSelectionTools::leaveoutSetAll()
{
    for (int i=0; i<leaveoutGetRunCount(); i++) {
        leaveoutAddRun(i);
    }

}

void QFRDRRunSelectionTools::leaveoutInvert()
{
    for (int i=0; i<leaveoutGetRunCount(); i++) {
        if (leaveoutRun(i)) {
            leaveoutRemoveRun(i);
        } else {
            leaveoutAddRun(i);
        }
    }

}

QList<int> QFRDRRunSelectionTools::leaveoutToIndexList()
{
    QList<int> lst;
    for (int i=0; i<leaveoutGetRunCount(); i++) {
        if (leaveoutRun(i)) {
            lst<<i;
        }
    }

    return lst;
}

void QFRDRRunSelectionTools::leaveoutChangedEvent()
{

}
