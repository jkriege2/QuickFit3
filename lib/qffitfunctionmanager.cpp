#include "qffitfunctionmanager.h"
#include <QDir>
#include <QtPlugin>
#include <QPluginLoader>
#include "qffitfunctionparsed.h"

QFFitFunctionManager::QFFitFunctionManager(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
    mutex=new QMutex();
    reloadUserFitFunctions();
}

QFFitFunctionManager::~QFFitFunctionManager()
{
    delete mutex;
}

void QFFitFunctionManager::reloadUserFitFunctions()
{
    QMutexLocker locker(mutex);

    QMap<QString, QString> userFF;

    for (int d=0; d<2; d++ ) {
        QString userffDir;
        if (d==0) userffDir=QFPluginServices::getInstance()->getAssetsDirectory()+"/userfitfunctions/";
        if (d==1) userffDir=ProgramOptions::getInstance()->getConfigValue("quickfit/user_fitfunctions", QFPluginServices::getInstance()->getConfigFileDirectory()+"/userfitfunctions/").toString();
        QDir dir(userffDir);
        emit showLongMessage(tr("searching in directory '%1' for user fit functions:").arg(userffDir));
        if (dir.exists()) {
            QStringList ff=dir.entryList(QStringList("*.qff"), QDir::Files);
            for (int i=0; i<ff.size(); i++) {
                QString fn=dir.absoluteFilePath(ff[i]);
                QSettings set(fn, QSettings::IniFormat);
                //qDebug()<<set.allKeys().join("\n")<<"\n\n";
                QString id=set.value("function/id", "").toString();
                if (!id.isEmpty()) {
                    QString isOK=tr("OK");
                    QFFitFunctionParsed* f=new QFFitFunctionParsed(fn);
                    if (!f->isValid()) {
                        isOK=tr("INVALID\n         ")+f->getErrors();
                    }
                    emit showLongMessage(tr("    * adding function %1 from '%2' ... %3").arg(id).arg(fn).arg(isOK));
                    if (f->isValid()) {
                        userFF[id]=fn;
                    }
                } else {
                    emit showLongMessage(tr("    * no ID found in '%1'").arg(fn));
                }
            }
        }
    }

    userFitFunctions=userFF;
    emit fitFunctionsChanged();
}

void QFFitFunctionManager::searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList, QMap<QString, QFToolTipsData>& tooltips) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (QApplication::arguments().contains("--verboseplugin")) {
            QFPluginServices::getInstance()->log_global_text("fit function plugin manager:\n  trying "+fileName+"\n");
            if (!plugin) QFPluginServices::getInstance()->log_global_text("    error: "+loader.errorString()+"\n");
        }
        if (plugin) {
            if (QApplication::arguments().contains("--verboseplugin")) QFPluginServices::getInstance()->log_global_text("    instance OK\n");
            QFPluginFitFunction* iRecord = qobject_cast<QFPluginFitFunction*>(plugin);
            if (iRecord) {
                int pmajor, pminor;
                iRecord->getQFLibVersion(pmajor, pminor);
                if (QApplication::arguments().contains("--verboseplugin")) {
                    QFPluginLogTools::log_global_text("    QFPluginFitFunction OK\n");
                    QFPluginLogTools::log_global_text(tr("    plugin built agains QFLib v%1.%2, this QFLib %3.%4\n").arg(pmajor).arg(pminor).arg(QF3LIB_APIVERSION_MAJOR).arg(QF3LIB_APIVERSION_MINOR));
                }
                fitPlugins.append(iRecord);
                fitPluginObjects.append(plugin);
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
                    info.settings=info.directory+QString("settings.html");
                    if (!QFile::exists(info.mainhelp)) info.mainhelp="";
                    if (!QFile::exists(info.tutorial)) info.tutorial="";
                    if (!QFile::exists(info.settings)) info.settings="";
                    info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_fitfunc.html");
                    info.plugintypename=tr("Fit Function Plugins");
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

    reloadUserFitFunctions();

}


QMap<QString, QFFitFunction*> QFFitFunctionManager::getModels(QString id_start, QObject* parent) {
    QMutexLocker locker(mutex);
    QMap<QString, QFFitFunction*> res;

    for (int i=0; i<fitPlugins.size(); i++) {
        QStringList ids=fitPlugins[i]->getIDs();
        for (int j=0; j<ids.size(); j++) {
            if (id_start.isEmpty() || ids[j].startsWith(id_start)) {
                res[ids[j]]=fitPlugins[i]->get(ids[j], parent);
            }
        }
    }

    QMap<QString, QString>::const_iterator it;
    for (it=userFitFunctions.begin(); it!=userFitFunctions.end(); ++it) {
        if (id_start.isEmpty() || it.key().startsWith(id_start)) {
            QFFitFunctionParsed* f=new QFFitFunctionParsed(it.value());
            if ( f && f->isValid() && !res.contains(it.key())) res[it.key()]=f;
            else if (f) delete f;
        }
    }

    return res;
}

QFFitFunction *QFFitFunctionManager::createFunction(QString ID, QObject *parent) {
    QMutexLocker locker(mutex);
    for (int i=0; i<fitPlugins.size(); i++) {
        QStringList ids=fitPlugins[i]->getIDs();
        if (ids.contains(ID)) {
            return fitPlugins[i]->get(ID, parent);
        }
    }

    if (userFitFunctions.contains(ID)) {
        QFFitFunctionParsed* f=new QFFitFunctionParsed(userFitFunctions[ID]);
        if (f->isValid()) return f;
        delete f;
        return NULL;
    }
    return NULL;
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

bool QFFitFunctionManager::contains(const QString &ID)
{
    for (int i=0; i<fitPlugins.size(); i++)
        if (fitPlugins[i]->getID()==ID) return true;
    return userFitFunctions.contains(ID);
}

QObject *QFFitFunctionManager::getPluginObject(int i) const
{
    return fitPluginObjects.value(i, NULL);
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

QString QFFitFunctionManager::getPluginSettings(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/settings.html").arg(basename);
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


