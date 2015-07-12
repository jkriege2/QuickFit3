/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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
