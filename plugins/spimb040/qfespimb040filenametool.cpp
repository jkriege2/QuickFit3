#include "qfespimb040filenametool.h"
#include <QDebug>

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
            edit->addInsertContextMenuEntry(QObject::tr("insert %1 ...").arg("%"+item+"%"), "%"+item+"%");
        }
    }
}

void QFESPIMB040FilenameTool::setGlobalReplaces(QFESPIMB040OpticsSetup *setup) {

}
