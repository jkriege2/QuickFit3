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
#include "qfextensionmanager.h"
#include <QPluginLoader>
#include <QtPlugin>
#include <QDir>
#include "qfextension.h"
#include "qfpluginservices.h"
#include "qfevaluationitemfactory.h"
#include "qfrawdatarecordfactory.h"
#include "qftools.h"
QFExtensionManager::QFExtensionManager(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
}

QFExtensionManager::~QFExtensionManager()
{
    //dtor
}



void QFExtensionManager::searchPlugins(QString directory, QFPluginHelpData &helpdata) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, qfDirListFilesRecursive(pluginsDir)) {//pluginsDir.entryList(QDir::Files)) {
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
                info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_extension.html");
                info.plugintypename=tr("Extension Plugins");
                info.pluginDLLbasename=QFileInfo(fileName).baseName();
                info.pluginDLLSuffix=QFileInfo(fileName).suffix();
                helpdata.pluginHelpList.append(info);

                parseTooltips(info.directory, helpdata.tooltips);
                parseAutolinks(info.directory, helpdata.autolinks);
                parseGlobalreplaces(info.directory);


                addExtensionPlugins(pluginsDir.absoluteFilePath(fileName), iRecord->getAdditionalExtensionPlugins());
                QFPluginServices::getInstance()->getEvaluationItemFactory()->addEvalPlugins(pluginsDir.absoluteFilePath(fileName), iRecord->getAdditionalEvaluationPlugins());
                QFPluginServices::getInstance()->getRawDataRecordFactory()->addRDRPlugins(pluginsDir.absoluteFilePath(fileName), iRecord->getAdditionalRDRPlugins());

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

QString QFExtensionManager::getPluginTutorialMain(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFExtensionManager::getPluginFAQ(QString ID)
{
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/faq.html").arg(basename);
    }
    return "";

}

void QFExtensionManager::getPluginTutorials(const QString &ID, QStringList &names, QStringList &files)
{
    if (items.contains(ID)) {
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

void QFExtensionManager::addExtensionPlugin(const QString &filename, QFExtension *record)
{
    if (record) {
        QString id=record->getID();
        items[id]=record;
        filenames[id]=filename;
        emit showMessage(tr("loaded extension plugin '%2' (%1) ...").arg(filename).arg(record->getName()));
        emit showLongMessage(tr("loaded extension plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[record->getID()]).arg(record->getName()).arg(record->getAuthor()).arg(record->getCopyright()));
    }
}

void QFExtensionManager::addExtensionPlugins(const QString &filename, QList<QFExtension *> records)
{
    for (int i=0; i<records.size(); i++) {
        addExtensionPlugin(filename, records[i]);
    }
}

bool QFExtensionManager::contains(QString ID) {
    return items.contains(ID);
}
