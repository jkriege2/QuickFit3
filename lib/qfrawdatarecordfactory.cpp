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

#include "qfrawdatarecordfactory.h"
#include "qfrawdatarecord.h"
#include "qfextensionmanager.h"
#include "qfevaluationitemfactory.h"
#include "qfhtmlhelptools.h"
#include "qftools.h"
QFRawDataRecordFactory::QFRawDataRecordFactory(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
}

QFRawDataRecordFactory::~QFRawDataRecordFactory()
{
    //dtor
}

void QFRawDataRecordFactory::searchPlugins(QString directory, QFPluginHelpData &helpdata) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, qfDirListFilesRecursive(pluginsDir)) {//pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (QApplication::arguments().contains("--verboseplugin")) {
            QFPluginServices::getInstance()->log_global_text("raw data record plugin manager:\n  trying "+fileName+"\n");
            if (!plugin) QFPluginServices::getInstance()->log_global_text("    error: "+loader.errorString()+"\n");
        }
        if (plugin) {
            if (QApplication::arguments().contains("--verboseplugin")) QFPluginServices::getInstance()->log_global_text("    instance OK\n");
            QFPluginRawDataRecord* iRecord = qobject_cast<QFPluginRawDataRecord*>(plugin);
            if (iRecord) {
                int pmajor, pminor;
                iRecord->getQFLibVersion(pmajor, pminor);
                if (QApplication::arguments().contains("--verboseplugin")) {
                    QFPluginLogTools::log_global_text("    QFPluginRawDataRecord OK\n");
                    QFPluginLogTools::log_global_text(tr("    plugin built agains QFLib v%1.%2, this QFLib %3.%4\n").arg(pmajor).arg(pminor).arg(QF3LIB_APIVERSION_MAJOR).arg(QF3LIB_APIVERSION_MINOR));
                }
                items[iRecord->getID()]=iRecord;
                filenames[iRecord->getID()]=pluginsDir.absoluteFilePath(fileName);
                emit showMessage(tr("loaded raw data plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
                emit showLongMessage(tr("loaded raw data plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
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
                info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_rdrscreen.html");
                info.plugintypename=tr("Raw Data Plugins");
                info.pluginDLLbasename=QFileInfo(fileName).baseName();
                info.pluginDLLSuffix=QFileInfo(fileName).suffix();
                helpdata.pluginHelpList.append(info);

                parseTooltips(info.directory, helpdata.tooltips);
                parseAutolinks(info.directory, helpdata.autolinks);
                parseGlobalreplaces(info.directory);

                QFPluginServices::getInstance()->getExtensionManager()->addExtensionPlugins(pluginsDir.absoluteFilePath(fileName), iRecord->getAdditionalExtensionPlugins());
                QFPluginServices::getInstance()->getEvaluationItemFactory()->addEvalPlugins(pluginsDir.absoluteFilePath(fileName), iRecord->getAdditionalEvaluationPlugins());
                addRDRPlugins(pluginsDir.absoluteFilePath(fileName), iRecord->getAdditionalRDRPlugins());
            }
        }
    }
}

void QFRawDataRecordFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent) {
    QMapIterator<QString, QFPluginRawDataRecord*> i(items);
    while (i.hasNext()) {
        i.next();
        if (i.value())  {
            i.value()->setProject(project);
            i.value()->setSettings(settings);
            i.value()->setServices(services);
            i.value()->setParentWidget(parent);
        }
    }
}

void QFRawDataRecordFactory::deinit() {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->deinit();
    }
}

void QFRawDataRecordFactory::init()
{
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->init();
    }
}

int QFRawDataRecordFactory::getMajorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFRawDataRecordFactory::getMinorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}


QStringList QFRawDataRecordFactory::getIDList() {
    return items.keys();
}

QString QFRawDataRecordFactory::getDescription(QString ID) {
    if (items.contains(ID)) return items[ID]->getDescription();
    return QString("");
};

QString QFRawDataRecordFactory::getName(QString ID) {
    if (items.contains(ID)) return items[ID]->getName();
    return QString("");
};

QString QFRawDataRecordFactory::getAuthor(QString ID) {
    if (items.contains(ID)) return items[ID]->getAuthor();
    return QString("");
};

QString QFRawDataRecordFactory::getCopyright(QString ID) {
    if (items.contains(ID)) return items[ID]->getCopyright();
    return QString("");
};

QString QFRawDataRecordFactory::getWeblink(QString ID) {
    if (items.contains(ID)) return items[ID]->getWeblink();
    return QString("");
};

QString QFRawDataRecordFactory::getIconFilename(QString ID) {
    if (items.contains(ID)) return items[ID]->getIconFilename();
    return QString("");
};

QString QFRawDataRecordFactory::getPluginFilename(QString ID) {
    if (items.contains(ID)) return filenames[ID];
    return QString("");
};


QFRawDataRecord* QFRawDataRecordFactory::createRecord(QString ID, QFProject* parent)  {
    if (items.contains(ID)) return items[ID]->createRecord(parent);
    return NULL;
};

void QFRawDataRecordFactory::registerMenu(QString ID, QMenu* menu)  {
    if (items.contains(ID)) {
        return items[ID]->registerToMenu(menu);
    }
}

void QFRawDataRecordFactory::addRDRPlugin(const QString &filename, QFPluginRawDataRecord *record)
{
    if (record) {
        QString id=record->getID();
        items[id]=record;
        filenames[id]=filename;
        emit showMessage(tr("loaded raw data plugin '%2' (%1) ...").arg(filename).arg(record->getName()));
        emit showLongMessage(tr("loaded raw data plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[record->getID()]).arg(record->getName()).arg(record->getAuthor()).arg(record->getCopyright()));
    }
}

void QFRawDataRecordFactory::addRDRPlugins(const QString &filename, QList<QFPluginRawDataRecord *> records)
{
    for (int i=0; i<records.size(); i++) {
        addRDRPlugin(filename, records[i]);
    }

};


QString QFRawDataRecordFactory::getPluginHelp(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(ID);
    }
    return "";
}

QString QFRawDataRecordFactory::getPluginTutorialMain(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFRawDataRecordFactory::getPluginFAQ(QString ID)
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

void QFRawDataRecordFactory::getPluginTutorials(const QString &ID, QStringList &names, QStringList &files)
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

QString QFRawDataRecordFactory::getPluginSettings(QString ID)
{
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/settings.html").arg(basename);
    }
    return "";
}

QString QFRawDataRecordFactory::getPluginCopyrightFile(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}


bool  QFRawDataRecordFactory::contains(QString ID) {
    return items.contains(ID);
}

QFPluginRawDataRecord *QFRawDataRecordFactory::getPlugin(const QString &ID)
{
    if (items.contains(ID)) return items[ID];
    return NULL;
}

