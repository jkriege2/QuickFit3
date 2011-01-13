#include "qffitalgorithmmanager.h"
#include <QtPlugin>
#include <QPluginLoader>
#include <QDir>


QFFitAlgorithmManager::QFFitAlgorithmManager(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFFitAlgorithmManager::~QFFitAlgorithmManager()
{
    //dtor
}

void QFFitAlgorithmManager::searchPlugins(QString directory) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFPluginFitAlgorithm* iRecord = qobject_cast<QFPluginFitAlgorithm*>(plugin);
            if (iRecord) {
                fitPlugins[iRecord->getID()]=iRecord;
                filenames[iRecord->getID()]=fileName;
            }
        }
    }
}

/** \brief create a new fit algorithm object instance */
QFFitAlgorithm* QFFitAlgorithmManager::createAlgorithm(QString id, QObject* parent) {
    if (fitPlugins.contains(id)) return fitPlugins[id]->create(parent);
    else return NULL;
}
