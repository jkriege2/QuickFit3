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
                filenames[iRecord->getID()]=fileName;
            }
        }
    }
}

void QFRawDataRecordFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginReportWrapper* reporter, QWidget* parent) {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->setProject(project);
        items[getIDList().at(i)]->setSettings(settings);
        items[getIDList().at(i)]->setReporter(reporter);
        items[getIDList().at(i)]->setParentWidget(parent);
    }
}


