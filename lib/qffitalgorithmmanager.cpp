/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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
#include "qffitalgorithmmanager.h"
#include <QtPlugin>
#include <QPluginLoader>
#include <QDir>
#include "qfpluginfitalgorithm.h"
#include "qffitalgorithm.h"
#include "qftools.h"

QFFitAlgorithmManager::QFFitAlgorithmManager(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
    mutex=new QMutex(QMutex::Recursive);
}

QFFitAlgorithmManager::~QFFitAlgorithmManager()
{
    delete mutex;
}

bool QFFitAlgorithmManager::contains(const QString &ID)
{
    for (int i=0; i<fitPlugins.size(); i++)
        if (fitPlugins[i]->getID()==ID) return true;
    return false;
}

void QFFitAlgorithmManager::searchPlugins(QString directory, QFPluginHelpData &helpdata) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, qfDirListFilesRecursive(pluginsDir)) {//pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (QApplication::arguments().contains("--verboseplugin")) {
            QFPluginServices::getInstance()->log_global_text("fit algorithm plugin manager:\n  trying "+fileName+"\n");
            if (!plugin) QFPluginServices::getInstance()->log_global_text("    error: "+loader.errorString()+"\n");
        }
        if (plugin) {
            if (QApplication::arguments().contains("--verboseplugin")) QFPluginServices::getInstance()->log_global_text("    instance OK\n");
            QFPluginFitAlgorithm* iRecord = qobject_cast<QFPluginFitAlgorithm*>(plugin);
            if (iRecord) {
                int pmajor, pminor;
                iRecord->getQFLibVersion(pmajor, pminor);
                if (QApplication::arguments().contains("--verboseplugin")) {
                    QFPluginLogTools::log_global_text("    QFPluginFitAlgorithm OK\n");
                    QFPluginLogTools::log_global_text(tr("    plugin built agains QFLib v%1.%2, this QFLib %3.%4\n").arg(pmajor).arg(pminor).arg(QF3LIB_APIVERSION_MAJOR).arg(QF3LIB_APIVERSION_MINOR));
                }
                fitPlugins.append(iRecord);
                filenames.append(pluginsDir.absoluteFilePath(fileName));
                emit showMessage(tr("loaded fit algorithm plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded fit algorithm plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(pluginsDir.absoluteFilePath(fileName)).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
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
                info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_fitalg.html");
                info.plugintypename=tr("Fit Algorithm Plugins");
                info.pluginDLLbasename=QFileInfo(fileName).baseName();
                info.pluginDLLSuffix=QFileInfo(fileName).suffix();
                helpdata.pluginHelpList.append(info);

                parseTooltips(info.directory, helpdata.tooltips);
                parseAutolinks(info.directory, helpdata.autolinks);
                parseGlobalreplaces(info.directory);

            }
        }
    }
}

QString QFFitAlgorithmManager::getName(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getName();
}

QString QFFitAlgorithmManager::getPluginFilename(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return filenames[i];
}

QString QFFitAlgorithmManager::getDescription(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getDescription();
}

QString QFFitAlgorithmManager::getAuthor(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getAuthor();
}

QString QFFitAlgorithmManager::getCopyright(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getCopyright();
}

QString QFFitAlgorithmManager::getWeblink(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getWeblink();
}

QStringList QFFitAlgorithmManager::getIDList(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return QStringList();
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
        if (fitPlugins[i]->getID()==id) return i;
         QStringList ids=fitPlugins[i]->getIDs();
         if (ids.contains(id)) return i;
   }
   return -1;
}

QFFitAlgorithm* QFFitAlgorithmManager::createAlgorithm(QString id, QObject* parent) const {
    QMutexLocker  locker(mutex);
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
    if ((id>=0) && (id<fitPlugins.size())) {
        fitPlugins[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFFitAlgorithmManager::getMinorVersion(int id) {
    int ma, mi;
    if ((id>=0) && (id<fitPlugins.size())) {
        fitPlugins[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}


QString QFFitAlgorithmManager::getID(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    return fitPlugins[i]->getID();
}


QString QFFitAlgorithmManager::getIconFilename(int i) const {
    if ((i<0) || (i>=fitPlugins.size())) return "";
    QString ic= fitPlugins[i]->getIconFilename();
    if (QFile::exists(ic)) return ic;
    else return QString(":/lib/fitalg_icon.png");
}


QString QFFitAlgorithmManager::getPluginHelp(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(getID(ID));
    }
    return "";
}

QString QFFitAlgorithmManager::getPluginTutorialMain(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFFitAlgorithmManager::getPluginFAQ(int ID)
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

QString QFFitAlgorithmManager::getPluginSettings(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/settings.html").arg(basename);
    }
    return "";
}


QString QFFitAlgorithmManager::getPluginCopyrightFile(int ID) {
    if ((ID>=0) && (ID<fitPlugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}

void QFFitAlgorithmManager::getPluginTutorials(int ID, QStringList &names, QStringList &files)
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




QString QFFitAlgorithmManager::getPluginHelp(int ID, QString faID) {
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


void QFFitAlgorithmManager::deinit() {
    for (int i=0; i<pluginCount(); i++) {
        if (fitPlugins.value(i, NULL)) fitPlugins.value(i, NULL)->deinit();
    }
}

void QFFitAlgorithmManager::init()
{
    for (int i=0; i<pluginCount(); i++) {
        if (fitPlugins.value(i, NULL)) fitPlugins.value(i, NULL)->init();
    }
}

