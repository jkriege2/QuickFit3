#include "qfextensionmanager.h"
#include <QPluginLoader>
#include <QtPlugin>
#include <QDir>
#include "qfextension.h"
#include "qfpluginservices.h"


QFExtensionManager::QFExtensionManager(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
}

QFExtensionManager::~QFExtensionManager()
{
    //dtor
}



void QFExtensionManager::searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (QApplication::arguments().contains("--verboseplugin")) {
            QFPluginServices::getInstance()->log_global_text("extension plugin manager:\n  trying "+fileName+"\n");
            if (!plugin) QFPluginServices::getInstance()->log_global_text("    error: "+loader.errorString()+"\n");
        }
        if (plugin) {
            if (QApplication::arguments().contains("--verboseplugin")) QFPluginServices::getInstance()->log_global_text("    instance OK\n");
            QFExtension* iRecord = qobject_cast<QFExtension*>(plugin);
            if (iRecord) {
                int pmajor, pminor;
                iRecord->getQFLibVersion(pmajor, pminor);
                if (QApplication::arguments().contains("--verboseplugin")) {
                    QFPluginLogTools::log_global_text("    QFExtension OK\n");
                    QFPluginLogTools::log_global_text(tr("    plugin built agains QFLib v%1.%2, this QFLib %3.%4\n").arg(pmajor).arg(pminor).arg(QF3LIB_APIVERSION_MAJOR).arg(QF3LIB_APIVERSION_MINOR));
                }
                items[iRecord->getID()]=iRecord;
                itemobjects[iRecord->getID()]=plugin;
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded extension plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded extension plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
                // , QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList
                if (pluginHelpList) {
                    QFPluginServices::HelpDirectoryInfo info;
                    info.plugin=iRecord;
                    info.directory=m_options->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(fileName).baseName()+QString("/");
                    info.mainhelp=info.directory+iRecord->getID()+QString(".html");
                    info.tutorial=info.directory+QString("tutorial.html");
                    info.settings=info.directory+QString("settings.html");
                    if (!QFile::exists(info.mainhelp)) info.mainhelp="";
                    if (!QFile::exists(info.tutorial)) info.tutorial="";
                    if (!QFile::exists(info.settings)) info.settings="";
                    info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_extension.html");
                    info.plugintypename=tr("Extension Plugins");
                    info.pluginDLLbasename=QFileInfo(fileName).baseName();
                    info.pluginDLLSuffix=QFileInfo(fileName).suffix();
                    pluginHelpList->append(info);
                }
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


void QFExtensionManager::init(QFPluginServices* services, QWidget* parentWidget) {
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

int QFExtensionManager::getMajorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFExtensionManager::getMinorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}

QString QFExtensionManager::getPluginHelp(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(ID);
    }
    return "";
}

QString QFExtensionManager::getPluginTutorial(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFExtensionManager::getPluginSettings(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/settings.html").arg(basename);
    }
    return "";
}

QString QFExtensionManager::getPluginCopyrightFile(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}

bool QFExtensionManager::contains(QString ID) {
    return items.contains(ID);
}
