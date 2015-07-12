/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "qfespimb040filenametool.h"
#include <QDebug>
#include "qftools.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040experimentdescription.h"
#include "qfespimb040acquisitiondescription.h"

QFESPIMB040FilenameTool::QFESPIMB040FilenameTool()
{
}

QString QFESPIMB040FilenameTool::transformFilename(const QString &filename) const
{
    QString data=filename;
    for (int i=0; i<5; i++) {
        foreach(QString item, replaceValues.keys()) {
            data=data.replace("%"+item+"%", replaceValues[item], Qt::CaseInsensitive);
        }
    }
    if (data.toLower().contains("%counter%")) {
        bool ok=false;
        int counter=0;
        while (!ok) {
            QString test=data;
            test=test.replace("%counter%", QString("%1").arg((qlonglong)counter, (int)3, (int)10, QChar('0')), Qt::CaseInsensitive);
            QDir d=QFileInfo(test+".dat").absoluteDir();
            if (!d.exists()) ok=true;
            else {
                QStringList files=d.entryList(QStringList("*.*"), QDir::Files);
                bool found=false;
                foreach(QString file, files) {
                    if (file.startsWith(QFileInfo(test).fileName(), Qt::CaseInsensitive)) {
                        found=true;
                        break;
                    }
                }
                //qDebug()<<QFileInfo(test).fileName()<<found<<"\n"<<files;
                if (!found) ok=true;
                else counter++;
            }
        }
        data=data.replace("%counter%", QString("%1").arg((qlonglong)counter, (int)3, (int)10, QChar('0')), Qt::CaseInsensitive);
    }
    return data;
}

void QFESPIMB040FilenameTool::setReplaceValues(const QMap<QString, QString> &replaceValues) {
    this->replaceValues=replaceValues;
    fillLineEdits();
}

void QFESPIMB040FilenameTool::setReplaceValue(const QString &key, const QString &value) {
    replaceValues[key]=value;
}

void QFESPIMB040FilenameTool::bindLineEdit(QEnhancedLineEdit *edit) {
    if (!edits.contains(edit)) edits.append(edit);
    fillLineEdits();
}

void QFESPIMB040FilenameTool::fillLineEdits() {
    foreach(QEnhancedLineEdit* edit, edits) {
        edit->clearContextMenu();
        edit->addInsertContextMenuEntry(QObject::tr("insert %counter% ..."), "%counter%");
        foreach(QString item, replaceValues.keys()) {
            edit->addInsertContextMenuEntry(QObject::tr("insert %1 [example: %2]...").arg("%"+item+"%").arg(replaceValues[item]), "%"+item+"%");
        }
    }
}

void QFESPIMB040FilenameTool::setGlobalReplaces(QFESPIMB040OpticsSetupBase *setup, QFESPIMB040ExperimentDescription* exp, QFESPIMB040AcquisitionDescription* acq) {
    if (setup) setReplaceValue("laser", setup->getLaserConfig());
    if (exp) {
        setReplaceValue("exp_title",  cleanStringForFilename(exp->getTitle()));
        setReplaceValue("exp_id",  cleanStringForFilename(exp->getID()));
        setReplaceValue("experimenter", cleanStringForFilename(exp->getExperimenter()));
    }
    if (acq) {
        setReplaceValue("prefix", acq->getPrefix());
        setReplaceValue("cell", QString::number(acq->getCell()));
        setReplaceValue("cell00", QString("%1").arg(acq->getCell(), 2, 10, QChar('0')));
        setReplaceValue("cell000", QString("%1").arg(acq->getCell(), 3, 10, QChar('0')));
        setReplaceValue("cell0000", QString("%1").arg(acq->getCell(), 4, 10, QChar('0')));
        setReplaceValue("sample", QString::number(acq->getCell()));
        setReplaceValue("sample00", QString("%1").arg(acq->getCell(), 2, 10, QChar('0')));
        setReplaceValue("sample000", QString("%1").arg(acq->getCell(), 3, 10, QChar('0')));
        setReplaceValue("sample0000", QString("%1").arg(acq->getCell(), 4, 10, QChar('0')));
        setReplaceValue("well", QString::number(acq->getWell()));
        setReplaceValue("well00", QString("%1").arg(acq->getWell(), 2, 10, QChar('0')));
        setReplaceValue("well000", QString("%1").arg(acq->getWell(), 3, 10, QChar('0')));
        setReplaceValue("well0000", QString("%1").arg(acq->getWell(), 4, 10, QChar('0')));
        setReplaceValue("plate", QString::number(acq->getPlate()));
        setReplaceValue("plate00", QString("%1").arg(acq->getPlate(), 2, 10, QChar('0')));
        setReplaceValue("plate000", QString("%1").arg(acq->getPlate(), 3, 10, QChar('0')));
        setReplaceValue("plate0000", QString("%1").arg(acq->getPlate(), 4, 10, QChar('0')));
        setReplaceValue("comment", cleanStringForFilename(acq->getComment()));
        setReplaceValue("samplename", cleanStringForFilename(acq->getSample()));
    }
    setReplaceValue("today", QDate::currentDate().toString("yyyy_MM_dd"));
    setReplaceValue("date", QDate::currentDate().toString("yyyy_MM_dd"));
}
