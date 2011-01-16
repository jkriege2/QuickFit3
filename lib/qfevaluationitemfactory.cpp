#include "qfevaluationitemfactory.h"
#include "qfevaluationitem.h"

QFEvaluationItemFactory::QFEvaluationItemFactory(QObject* parent):
    QObject(parent)
{

}

QFEvaluationItemFactory::~QFEvaluationItemFactory()
{
    //dtor
}


void QFEvaluationItemFactory::searchPlugins(QString directory) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFPluginEvaluationItem* iRecord = qobject_cast<QFPluginEvaluationItem*>(plugin);
            if (iRecord) {
                items[iRecord->getID()]=iRecord;
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded evaluation plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded evaluation plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
            }
        }
    }
}

void QFEvaluationItemFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent) {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->setProject(project);
        items[getIDList().at(i)]->setSettings(settings);
        items[getIDList().at(i)]->setServices(services);
        items[getIDList().at(i)]->setParentWidget(parent);
    }
}




