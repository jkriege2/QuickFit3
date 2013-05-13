#include "qfimportermanager.h"

QFImporterManager::QFImporterManager(ProgramOptions* options, QObject *parent) :
    QObject(parent)
{
    m_options=options;
}


void QFImporterManager::searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList, QMap<QString, QFToolTipsData>& tooltips) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (QApplication::arguments().contains("--verboseplugin")) {
            QFPluginServices::getInstance()->log_global_text("importer plugin manager:\n  trying "+fileName+"\n");
            if (!plugin) QFPluginServices::getInstance()->log_global_text("    error: "+loader.errorString()+"\n");
        }
        if (plugin) {
            if (QApplication::arguments().contains("--verboseplugin")) QFPluginServices::getInstance()->log_global_text("    instance OK\n");
            QFPluginImporters* iRecord = qobject_cast<QFPluginImporters*>(plugin);
            if (iRecord) {
                if (QApplication::arguments().contains("--verboseplugin")) QFPluginServices::getInstance()->log_global_text("    QFPluginImporters OK\n");
                plugins.append(iRecord);
                filenames.append(pluginsDir.absoluteFilePath(fileName));
                emit showMessage(tr("loaded importer plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded importern plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(pluginsDir.absoluteFilePath(fileName)).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
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
                    info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_fitfunc.html");
                    info.plugintypename=tr("Importer Plugins");
                    info.pluginDLLbasename=QFileInfo(fileName).baseName();
                    info.pluginDLLSuffix=QFileInfo(fileName).suffix();
                    pluginHelpList->append(info);

                    QSettings setTooltips(info.directory+"tooltips.ini", QSettings::IniFormat);

                    QStringList keys=setTooltips.childKeys();
                    for (int i=0; i<keys.size(); i++) {
                        tooltips[keys[i]].tooltip=setTooltips.value(keys[i], tr("<i>no tooltip available</i>")).toString();
                        tooltips[keys[i]].tooltipfile=info.directory+"tooltips.ini";
                    }
                }
            }
        }
    }
}

QFImporter *QFImporterManager::createImporter(const QString &id) const {
    for (int i=0; i<plugins.size(); i++) {
        QStringList ids=plugins[i]->getIDs();
        for (int j=0; j<ids.size(); j++) {
            if (id==ids[j]) return plugins[i]->createImporter(id);
        }
    }
    return NULL;
}


bool QFImporterManager::contains(const QString &ID)
{
    for (int i=0; i<plugins.size(); i++)
        if (plugins[i]->getID()==ID) return true;
    return false;
}

QString QFImporterManager::getName(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getName();
}

QString QFImporterManager::getPluginFilename(int i) const {
    if ((i<0) || (i>=filenames.size())) return "";
    return filenames[i];
}

QString QFImporterManager::getDescription(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getDescription();
}

QString QFImporterManager::getAuthor(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getAuthor();
}

QString QFImporterManager::getCopyright(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getCopyright();
}

QString QFImporterManager::getWeblink(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getWeblink();
}

QString QFImporterManager::getID(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getID();
}

QStringList QFImporterManager::getIDList(int i) const {
    if ((i<0) || (i>=plugins.size())) return QStringList();
    return plugins[i]->getIDs();
}

int QFImporterManager::getPluginForID(QString id) const {
    for (int i=0; i<plugins.size(); i++) {
         QStringList ids=plugins[i]->getIDs();
         if (ids.contains(id)) return i;
   }
   return -1;
}

int QFImporterManager::getMajorVersion(int id) {
    if ((id<0) || (id>=plugins.size())) return 0;
    int ma, mi;
    if (id<plugins.size()) {
        plugins[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFImporterManager::getMinorVersion(int id) {
    if ((id<0) || (id>=plugins.size())) return 0;
    int ma, mi;
    if (id<plugins.size()) {
        plugins[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}

QString QFImporterManager::getIconFilename(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    QString ic= plugins[i]->getIconFilename();
    if (QFile::exists(ic)) return ic;
    else return QString(":/lib/importer_icon.png");
}


QString QFImporterManager::getPluginHelp(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(getID(ID));
    }
    return "";
}

QString QFImporterManager::getPluginTutorial(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFImporterManager::getPluginSettings(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/settings.html").arg(basename);
    }
    return "";
}
QString QFImporterManager::getPluginCopyrightFile(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}




QString QFImporterManager::getPluginHelp(int ID, QString faID) {
    if ((ID>=0) && (ID<plugins.size())) {
        //QStringList ids=getIDList(ID);
        //if ((faID<0) || (faID>=ids.size())) return "";
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(faID);
    }
    return "";
}

