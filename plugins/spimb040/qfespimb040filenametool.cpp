#include "qfespimb040filenametool.h"
#include <QDebug>
#include "qftools.h"

QFESPIMB040FilenameTool::QFESPIMB040FilenameTool()
{
}

QString QFESPIMB040FilenameTool::transformFilename(const QString &filename) const
{
    QString data=filename;
    foreach(QString item, replaceValues.keys()) {
        data=data.replace("%"+item+"%", replaceValues[item], Qt::CaseInsensitive);
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

void QFESPIMB040FilenameTool::setGlobalReplaces(QFESPIMB040OpticsSetup *setup, QFESPIMB040ExperimentDescription* exp, QFESPIMB040AcquisitionDescription* acq) {
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
    }
    setReplaceValue("today", QDate::currentDate().toString("yyyy_MM_dd"));
    setReplaceValue("date", QDate::currentDate().toString("yyyy_MM_dd"));
}
