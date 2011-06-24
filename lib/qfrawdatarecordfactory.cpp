#include "qfrawdatarecordfactory.h"
#include "qfrawdatarecord.h"

QFRawDataRecordFactory::QFRawDataRecordFactory(QObject* parent):
    QObject(parent)
{

}

QFRawDataRecordFactory::~QFRawDataRecordFactory()
{
    //dtor
}

void QFRawDataRecordFactory::searchPlugins(QString directory) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFPluginRawDataRecord* iRecord = qobject_cast<QFPluginRawDataRecord*>(plugin);
            if (iRecord) {
                items[iRecord->getID()]=iRecord;
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded raw data plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded raw data plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
            }
        }
    }
}

void QFRawDataRecordFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent) {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->setProject(project);
        items[getIDList().at(i)]->setSettings(settings);
        items[getIDList().at(i)]->setServices(services);
        items[getIDList().at(i)]->setParentWidget(parent);
    }
}


int QFRawDataRecordFactory::getMajorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFRawDataRecordFactory::getMinorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}
