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
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded fit algorithm plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded fit algorithm plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(pluginsDir.absoluteFilePath(fileName)).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
            }
        }
    }
}

/** \brief create a new fit algorithm object instance */
QFFitAlgorithm* QFFitAlgorithmManager::createAlgorithm(QString id, QObject* parent) {
    if (fitPlugins.contains(id)) return fitPlugins[id]->create(parent);
    else return NULL;
}
