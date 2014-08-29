/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfhtmlhelptools.h"
#include "qffitfunctionmanager.h"
#include <QDir>
#include <QtPlugin>
#include <QPluginLoader>
#include "qffitfunctionparsed.h"
#include "qflibraryfitfunction.h"
#include "qftools.h"

QFFitFunctionManager::QFFitFunctionManager(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
    mutex=new QMutex();
    reloadUserFitFunctions();
    reloadLibraryFitFunctions();
}

QFFitFunctionManager::~QFFitFunctionManager()
{
    freeLibraryFitFunctions(false);
    delete mutex;
}

void QFFitFunctionManager::reloadUserFitFunctions()
{
    QMutexLocker locker(mutex);

    QMap<QString, QString> userFF;

    for (int d=0; d<3; d++ ) {
        QString userffDir;
        if (d==0) userffDir=QFPluginServices::getInstance()->getAssetsDirectory()+"/userfitfunctions/";
        if (d==1) userffDir=QFPluginServices::getInstance()->getConfigFileDirectory()+"/userfitfunctions/";
        if (d==2) userffDir=ProgramOptions::getInstance()->getConfigValue("quickfit/user_fitfunctions", ProgramOptions::getInstance()->getHomeQFDirectory()+"/userfitfunctions/").toString();
        QDir dir(userffDir);
        emit showLongMessage(tr("searching in directory '%1' for user fit functions:").arg(userffDir));
        if (dir.exists()) {
            QStringList ff=qfDirListFilesRecursive(dir, QStringList("*.qff"));
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
                    } else {
                        if(f->usesBytecode()) {
                            isOK=isOK+tr(" [BYTECODE, FAST!]");
                        } else {
                            isOK=isOK+tr(" [TREE-EVALUATION, SLOW!]");
                        }
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

void QFFitFunctionManager::reloadLibraryFitFunctions()
{
    //QMutexLocker locker(mutex);

    QMap<QString, QLibrary*> userFF;
    QDir bd(QApplication::applicationDirPath());
    freeLibraryFitFunctions();

    for (int d=0; d<3; d++ ) {
        QString userffDir;
        if (d==0) userffDir=bd.absolutePath()+"/sdk/sdk_fitfunctions";
        if (d==1) userffDir=bd.absolutePath()+"/sdk_fitfunctions";
        if (d==2) userffDir=QFPluginServices::getInstance()->getAssetsDirectory()+"/sdk_fitfunctions/";
        QDir dir(userffDir);
        emit showLongMessage(tr("searching in directory '%1' for fit functions in shared libraries:").arg(userffDir));
        if (dir.exists()) {
            QStringList ff=qfDirListFilesRecursive(dir);
            //qDebug()<<ff.join("\n");
            for (int i=0; i<ff.size(); i++) {
                QString isOK=tr("OK");
                QString feat="";
                QString fn=dir.absoluteFilePath(ff[i]);
                //qDebug()<<"checking "<<fn;
                if (QLibrary::isLibrary(fn)) {
                    //qDebug()<<"  isLIB "<<fn;
                    QLibrary* lib=new QLibrary(fn);
                    if (lib) {
                        //qDebug()<<"  LIB_OK "<<fn;
                        QFLibraryFitFunction* libff=new QFLibraryFitFunction(lib);
                        if (libff && libff->isValid()) {
                            //qDebug()<<"  isLIBFF "<<fn;
                            userFF[libff->id()]=lib;
                            feat=libff->features();
                            emit showLongMessage(tr("    * adding function %1 from '%2' ... %3 [additional features: %4]").arg(libff->id()).arg(fn).arg(isOK).arg(feat));
                            if (libff) delete libff;
                        } else {
                            isOK=tr("ERROR: NOT A VALID FIT FUNCTION LIBRARY, ERROR: %1").arg(libff->lastError());
                            delete lib;
                            if (libff) delete libff;
                        }
                    } else {
                        isOK=tr("ERROR: COULD NOT LOAD LIBRARY");
                    }
                    //qDebug()<<"  "<<isOK<<fn;
                    if (isOK!=tr("OK")) {
                        emit showLongMessage(tr("    * no fit function added from '%1': %2").arg(fn).arg(isOK));
                    }
                }
            }
        }
    }

    libraryFitFunctions=userFF;
}

void QFFitFunctionManager::freeLibraryFitFunctions(bool emitSignals)
{
    //QMutexLocker locker(mutex);

    QMapIterator<QString, QLibrary*> it(libraryFitFunctions);
    while (it.hasNext()) {
        it.next();
        if (it.value()) {
            it.value()->unload();
            delete it.value();
        }
    }

    libraryFitFunctions.clear();
    if (emitSignals) emit fitFunctionsChanged();
}

void QFFitFunctionManager::searchPlugins(QString directory, QFPluginHelpData &helpdata) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, qfDirListFilesRecursive(pluginsDir)) {//pluginsDir.entryList(QDir::Files)) {
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
                QFHelpDirectoryInfo info;
                info.plugin=iRecord;
                info.directory=m_options->getAssetsDirectory()+QString("/plugins/help/")+QFileInfo(fileName).baseName()+QString("/");
                info.mainhelp=info.directory+iRecord->getID()+QString(".html");
                info.tutorial=info.directory+QString("tutorial.html");
                info.settings=info.directory+QString("settings.html");
                info.faq=info.directory+QString("faq.html");
                if (!QFile::exists(info.mainhelp)) info.mainhelp="";
                if (!QFile::exists(info.tutorial)) info.tutorial="";
                if (!QFile::exists(info.settings)) info.settings="";
                if (!QFile::exists(info.faq)) info.faq="";
                if (!info.faq.isEmpty()) parseFAQ(info.faq, iRecord->getID(), helpdata.faqs);
                info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_fitfunc.html");
                info.plugintypename=tr("Fit Function Plugins");
                info.pluginDLLbasename=QFileInfo(fileName).baseName();
                info.pluginDLLSuffix=QFileInfo(fileName).suffix();
                helpdata.pluginHelpList.append(info);

                parseTooltips(info.directory, helpdata.tooltips);
                parseAutolinks(info.directory, helpdata.autolinks);
            }
        }
    }

    reloadUserFitFunctions();
    reloadLibraryFitFunctions();

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

    {
        QMap<QString, QString>::const_iterator it;
        for (it=userFitFunctions.begin(); it!=userFitFunctions.end(); ++it) {
            if (id_start.isEmpty() || it.key().startsWith(id_start)) {
                QFFitFunctionParsed* f=new QFFitFunctionParsed(it.value());
                if ( f && f->isValid() && !res.contains(it.key())) res[it.key()]=f;
                else if (f) delete f;
            }
        }
    }


    {
        QMap<QString, QLibrary*>::const_iterator it;
        for (it=libraryFitFunctions.begin(); it!=libraryFitFunctions.end(); ++it) {
            if (id_start.isEmpty() || it.key().startsWith(id_start)) {
                QFLibraryFitFunction* f=new QFLibraryFitFunction(it.value());
                if ( f && f->isValid() && !res.contains(it.key())) res[it.key()]=f;
                else if (f) delete f;
            }
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

    if (libraryFitFunctions.contains(ID)) {
        QFLibraryFitFunction* f=new QFLibraryFitFunction(libraryFitFunctions[ID]);
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
    return userFitFunctions.contains(ID) || libraryFitFunctions.contains(ID);
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
    if (userFitFunctions.contains(id)) return -2;
    if (libraryFitFunctions.contains(id)) return -3;
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

QStringList QFFitFunctionManager::getUserFitFunctionIDs() const
{
    return userFitFunctions.keys();
}

QString QFFitFunctionManager::getUserFitFunctionFile(const QString &id) const
{
    return userFitFunctions.value(id, "");
}

QStringList QFFitFunctionManager::getLibraryFitFunctionIDs() const
{
    return libraryFitFunctions.keys();
}

QLibrary *QFFitFunctionManager::getLibraryFitFunctionLibrary(const QString &id) const
{
    return libraryFitFunctions.value(id, NULL);
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

QString QFFitFunctionManager::getPluginTutorialMain(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFFitFunctionManager::getPluginFAQ(int ID)
{
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/faq.html").arg(basename);
    }
    return "";

}

void QFFitFunctionManager::getPluginTutorials(int ID, QStringList &names, QStringList &files)
{
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        QString tutini=m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorials.ini").arg(basename);
        if (QFile::exists(tutini)) {
            QSettings set(tutini, QSettings::IniFormat);
            QStringList groups=set.childGroups();
            for (int i=0; i<groups.size(); i++) {
                set.beginGroup(groups[i]);
                QString name=set.value("name", "").toString();
                QString file=set.value("file", "").toString();
                if (!file.isEmpty()) {
                    names.append(name);
                    files.append(m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2").arg(basename).arg(file));
                }
                set.endGroup();
            }
        } else {
            QDir d(m_options->getAssetsDirectory()+QString("/plugins/help/%1/").arg(basename));
            QStringList filters;
            filters<<"tutorial*.html";
            filters<<"tutorial*.htm";
            QStringList tuts=d.entryList(filters, QDir::Files);
            QString mainTut="";
            QString mainTutName="";
            for (int i=0; i<tuts.size(); i++) {
                QString fn=d.absoluteFilePath(tuts[i]);
                if (fn.toLower()=="tutorial.html") {
                    mainTut=fn;
                    QFile f(fn);
                    if (f.open(QIODevice::ReadOnly)) {
                        mainTutName=HTMLGetTitle(f.readAll());
                        f.close();
                    }
                } else {
                    files.append(fn);
                    QString name;
                    QFile f(fn);
                    if (f.open(QIODevice::ReadOnly)) {
                        name=HTMLGetTitle(f.readAll());
                        f.close();
                    }
                    names.append(name);
                }
            }
            if (files.size()<=0) {
                files.append(mainTut);
                names.append(mainTutName);
            }
        }
        return ;
    }
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
    if (ID==-3 && libraryFitFunctions.contains(faID)) {
        QFFitFunction* ff=createFunction(faID, NULL);
        QFLibraryFitFunction* lff=dynamic_cast<QFLibraryFitFunction*>(ff);
        QString h;
        if (lff) {
            h=lff->helpFile();
        }
        if (ff) delete ff;
        return h;
    }
    return "";
}


