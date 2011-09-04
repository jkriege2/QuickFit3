#include "qffitfunctionmanager.h"
#include <QDir>
#include <QtPlugin>
#include <QPluginLoader>

QFFitFunctionManager::QFFitFunctionManager(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
}

QFFitFunctionManager::~QFFitFunctionManager()
{
    //dtor
}

void QFFitFunctionManager::searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFPluginFitFunction* iRecord = qobject_cast<QFPluginFitFunction*>(plugin);
            if (iRecord) {
                fitPlugins.append(iRecord);
                filenames.append(pluginsDir.absoluteFilePath(fileName));
                emit showMessage(tr("loaded fit function plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded fit function plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(pluginsDir.absoluteFilePath(fileName)).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
                // , QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList
                if (pluginHelpList) {
                    QFPluginServices::HelpDirectoryInfo info;
                    info.plugin=iRecord;
                    info.directory=m_options->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(fileName).baseName()+QString("/");
                    info.mainhelp=info.directory+iRecord->getID()+QString(".html");
                    info.tutorial=info.directory+QString("tutorial.html");
                    if (!QFile::exists(info.mainhelp)) info.mainhelp="";
                    if (!QFile::exists(info.tutorial)) info.tutorial="";
                    info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_fitfunc.html");
                    info.plugintypename=tr("Fit Function Plugins");
                    pluginHelpList->append(info);
                }
            }
        }
    }
}


QMap<QString, QFFitFunction*> QFFitFunctionManager::getModels(QString id_start, QObject* parent) {
    QMap<QString, QFFitFunction*> res;

    for (int i=0; i<fitPlugins.size(); i++) {
        QStringList ids=fitPlugins[i]->getIDs();
        for (int j=0; j<ids.size(); j++) {
            if (id_start.isEmpty() || ids[j].startsWith(id_start)) {
                res[ids[j]]=fitPlugins[i]->get(ids[j], parent);
            }
        }
    }

    return res;
}

QString QFFitFunctionManager::getName(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getName();
}

QString QFFitFunctionManager::getPluginFilename(int i) const {
    if ((i<0) || (i>=filenames.size())) return "";
    return filenames[i];
}

QString QFFitFunctionManager::getDescription(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getDescription();
}

QString QFFitFunctionManager::getAuthor(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getAuthor();
}

QString QFFitFunctionManager::getCopyright(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getCopyright();
}

QString QFFitFunctionManager::getWeblink(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getWeblink();
}

QString QFFitFunctionManager::getID(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getID();
}

QStringList QFFitFunctionManager::getIDList(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return QStringList();
    return fitPlugins[i]->getIDs();
}

int QFFitFunctionManager::getPluginForID(QString id) const {
    for (int i=0; i<fitPlugins.size(); i++) {
         QStringList ids=fitPlugins[i]->getIDs();
         if (ids.contains(id)) return i;
   }
   return -1;
}

int QFFitFunctionManager::getMajorVersion(int id) {
    if ((id<0) || (id>=fitPlugins.size())) return 0;
    int ma, mi;
    if (id<fitPlugins.size()) {
        fitPlugins[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFFitFunctionManager::getMinorVersion(int id) {
    if ((id<0) || (id>=fitPlugins.size())) return 0;
    int ma, mi;
    if (id<fitPlugins.size()) {
        fitPlugins[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}

QString QFFitFunctionManager::getIconFilename(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    QString ic= fitPlugins[i]->getIconFilename();
    if (QFile::exists(ic)) return ic;
    else return QString(":/lib/fitfunc_icon.png");
}


QString QFFitFunctionManager::getPluginHelp(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(getID(ID));
    }
    return "";
}

QString QFFitFunctionManager::getPluginTutorial(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFFitFunctionManager::getPluginCopyrightFile(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}




QString QFFitFunctionManager::getPluginHelp(int ID, QString faID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
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


