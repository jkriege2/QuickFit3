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
                fitPlugins.append(iRecord);
                filenames.append(pluginsDir.absoluteFilePath(fileName));
                emit showMessage(tr("loaded fit algorithm plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded fit algorithm plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(pluginsDir.absoluteFilePath(fileName)).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
            }
        }
    }
}

QString QFFitAlgorithmManager::getName(int i) const {
    return fitPlugins[i]->getName();
}

QString QFFitAlgorithmManager::getFilename(int i) const {
    return filenames[i];
}

QString QFFitAlgorithmManager::getDescription(int i) const {
    return fitPlugins[i]->getDescription();
}

QString QFFitAlgorithmManager::getAuthor(int i) const {
    return fitPlugins[i]->getAuthor();
}

QString QFFitAlgorithmManager::getCopyright(int i) const {
    return fitPlugins[i]->getCopyright();
}

QString QFFitAlgorithmManager::getWeblink(int i) const {
    return fitPlugins[i]->getWeblink();
}

QStringList QFFitAlgorithmManager::getIDList(int i) const {
    return fitPlugins[i]->getIDs();
}


QStringList QFFitAlgorithmManager::getIDList() const {
    QStringList res;
    for (int i=0; i<fitPlugins.size(); i++) {
        res.append(fitPlugins[i]->getIDs());
    }
    res.removeDuplicates();
    return res;
}

int QFFitAlgorithmManager::getPluginForID(QString id) const {
    for (int i=0; i<fitPlugins.size(); i++) {
         QStringList ids=fitPlugins[i]->getIDs();
         if (ids.contains(id)) return i;
   }
   return -1;
}

QFFitAlgorithm* QFFitAlgorithmManager::createAlgorithm(QString id, QObject* parent) const {
    for (int i=0; i<fitPlugins.size(); i++) {
        QStringList ids=fitPlugins[i]->getIDs();
        if (ids.contains(id)) {
            return fitPlugins[i]->get(id, parent);
        }
    }
    return NULL;
}

int QFFitAlgorithmManager::getMajorVersion(int id) {
    int ma, mi;
    if (id<fitPlugins.size()) {
        fitPlugins[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFFitAlgorithmManager::getMinorVersion(int id) {
    int ma, mi;
    if (id<fitPlugins.size()) {
        fitPlugins[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}


QString QFFitAlgorithmManager::getID(int i) const {
    return fitPlugins[i]->getID();
}


QString QFFitAlgorithmManager::getIconFilename(int i) const {
    return fitPlugins[i]->getIconFilename();
}
