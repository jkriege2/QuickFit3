#include "qfextensionmanager.h"
#include <QPluginLoader>
#include <QtPlugin>
#include <QDir>
#include "qfextension.h"
#include "qfpluginservices.h"


QFExtensionManager::QFExtensionManager(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFExtensionManager::~QFExtensionManager()
{
    //dtor
}



void QFExtensionManager::searchPlugins(QString directory) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFExtension* iRecord = qobject_cast<QFExtension*>(plugin);
            if (iRecord) {
                items[iRecord->getID()]=iRecord;
                itemobjects[iRecord->getID()]=plugin;
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded extension plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded extension plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
            }
        }
    }
}

void QFExtensionManager::distribute(QFProject* project) {
    QMapIterator<QString, QFExtension*> i(items);
    while (i.hasNext()) {
        i.next();
        i.value()->setProject(project);
    }
}


void QFExtensionManager::init(QFExtensionServices* services, QWidget* parentWidget) {
    QMapIterator<QString, QFExtension*> i(items);
    while (i.hasNext()) {
        i.next();
        i.value()->init(services, parentWidget);
    }
}

void QFExtensionManager::deinit() {
    QMapIterator<QString, QFExtension*> i(items);
    while (i.hasNext()) {
        i.next();
        i.value()->deinit();
    }
}

void QFExtensionManager::readPluginSettings(ProgramOptions* settings) {
    QMapIterator<QString, QFExtension*> i(items);
    while (i.hasNext()) {
        i.next();
        i.value()->readSettings(settings);
    }
}


void QFExtensionManager::writePluginSettings(ProgramOptions* settings) {
    QMapIterator<QString, QFExtension*> i(items);
    while (i.hasNext()) {
        i.next();
        i.value()->writeSettings(settings);
    }
}


QString QFExtensionManager::getDescription(QString ID) {
    if (items.contains(ID)) return items[ID]->getDescription();
    return QString("");
};


QString QFExtensionManager::getName(QString ID) {
    if (items.contains(ID)) return items[ID]->getName();
    return QString("");
};



QString QFExtensionManager::getAuthor(QString ID) {
    if (items.contains(ID)) return items[ID]->getAuthor();
    return QString("");
};


QString QFExtensionManager::getCopyright(QString ID) {
    if (items.contains(ID)) return items[ID]->getCopyright();
    return QString("");
};

QString QFExtensionManager::getWeblink(QString ID) {
    if (items.contains(ID)) return items[ID]->getWeblink();
    return QString("");
};

QString QFExtensionManager::getIconFilename(QString ID) {
    if (items.contains(ID)) return items[ID]->getIconFilename();
    return QString("");
};

QString QFExtensionManager::getPluginFilename(QString ID) {
    if (items.contains(ID)) return filenames[ID];
    return QString("");
};

QFExtension* QFExtensionManager::getInstance(QString ID) {
    if (items.contains(ID)) return items[ID];
    return NULL;
}

QObject* QFExtensionManager::getQObjectInstance(QString ID) {
    if (itemobjects.contains(ID)) return itemobjects[ID];
    return NULL;
}
