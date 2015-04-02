/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfevaluationitemfactory.h"
#include "qfevaluationitem.h"
#include "qflib_version.h"
#include "qfextensionmanager.h"
#include "qfrawdatarecordfactory.h"
#include "qfevaluationitemfactory.h"
#include "qfhtmlhelptools.h"
#include "qftools.h"
QFEvaluationItemFactory::QFEvaluationItemFactory(ProgramOptions* options, QObject* parent):
    QObject(parent)
{
    m_options=options;
}

QFEvaluationItemFactory::~QFEvaluationItemFactory()
{
    //dtor
}

bool QFEvaluationItemFactory::registerPlugin(const QString& filename_in, QObject *plugin, QFPluginHelpData &helpdata)
{
    QString fileName=QFileInfo(filename_in).fileName();

    QFPluginEvaluationItem* iRecord = qobject_cast<QFPluginEvaluationItem*>(plugin);
    if (iRecord) {
        int pmajor, pminor;
        iRecord->getQFLibVersion(pmajor, pminor);
        if (QApplication::arguments().contains("--verboseplugin")) {
            QFPluginLogTools::log_global_text("    QFPluginEvaluationItem OK\n");
            QFPluginLogTools::log_global_text(tr("    plugin built agains QFLib v%1.%2, this QFLib %3.%4\n").arg(pmajor).arg(pminor).arg(QF3LIB_APIVERSION_MAJOR).arg(QF3LIB_APIVERSION_MINOR));
        }
        items[iRecord->getID()]=iRecord;
        filenames[iRecord->getID()]=QFileInfo(filename_in).absoluteFilePath();
        emit showMessage(tr("loaded evaluation plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
        emit showLongMessage(tr("loaded evaluation plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[iRecord->getID()]).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
        // , QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList

        QFHelpDirectoryInfo info;
        info.plugin=iRecord;
        QString libbasename=QFileInfo(fileName).baseName();
        if (fileName.contains(".so")) {
            if (libbasename.startsWith("lib")) libbasename=libbasename.right(libbasename.size()-3);
        }
        info.directory=m_options->getAssetsDirectory()+QString("/plugins/help/")+libbasename+QString("/");
        info.mainhelp=info.directory+iRecord->getID()+QString(".html");
        info.tutorial=info.directory+QString("tutorial.html");
        info.settings=info.directory+QString("settings.html");
        info.faq=info.directory+QString("faq.html");
        info.faq_parser=info.directory+QString("faq_parser.html");
        if (!QFile::exists(info.mainhelp)) info.mainhelp="";
        if (!QFile::exists(info.tutorial)) info.tutorial="";
        if (!QFile::exists(info.settings)) info.settings="";
        if (!QFile::exists(info.faq)) info.faq="";
        if (!QFile::exists(info.faq_parser)) info.faq_parser="";
        if (!info.faq.isEmpty()) parseFAQ(info.faq, iRecord->getID(), helpdata.faqs);
        if (!info.faq_parser.isEmpty()) parseFAQ(info.faq_parser, QString("parser/")+iRecord->getID(), helpdata.faqs);
        info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_evalscreen.html");
        info.plugintypename=tr("Evaluation Plugins");
        info.pluginDLLbasename=libbasename;
        info.pluginDLLSuffix=QFileInfo(fileName).suffix();
        helpdata.pluginHelpList.append(info);

        parseTooltips(info.directory, helpdata.tooltips);
        parseAutolinks(info.directory, helpdata.autolinks);
        parseGlobalreplaces(info.directory);



        QFPluginServices::getInstance()->getExtensionManager()->addExtensionPlugins(QFileInfo(filename_in).absoluteFilePath(), iRecord->getAdditionalExtensionPlugins());
        addEvalPlugins(QFileInfo(filename_in).absoluteFilePath(), iRecord->getAdditionalEvaluationPlugins());
        QFPluginServices::getInstance()->getRawDataRecordFactory()->addRDRPlugins(QFileInfo(filename_in).absoluteFilePath(), iRecord->getAdditionalRDRPlugins());
        return true;
    }
    return false;
}


void QFEvaluationItemFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent) {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->setProject(project);
        items[getIDList().at(i)]->setSettings(settings);
        items[getIDList().at(i)]->setServices(services);
        items[getIDList().at(i)]->setParentWidget(parent);
    }
}

void QFEvaluationItemFactory::deinit() {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->deinit();
    }
}

void QFEvaluationItemFactory::init()
{
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->init();
        emit incProgress();
        if (i%3==0) QApplication::processEvents();
    }
}

QFPluginEvaluationItem *QFEvaluationItemFactory::getPlugin(const QString& ID)
{
    if (items.contains(ID)) return items[ID];
    return NULL;

}

int QFEvaluationItemFactory::getMajorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFEvaluationItemFactory::getMinorVersion(QString id) {
    int ma, mi;
    if (items.contains(id)) {
        items[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}

void QFEvaluationItemFactory::addEvalPlugin(const QString &filename, QFPluginEvaluationItem *record)
{
    if (record) {
        QString id=record->getID();
        items[id]=record;
        filenames[id]=filename;
        emit showMessage(tr("loaded evaluation plugin '%2' (%1) ...").arg(filename).arg(record->getName()));
        emit showLongMessage(tr("loaded evaluation plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(filenames[record->getID()]).arg(record->getName()).arg(record->getAuthor()).arg(record->getCopyright()));
    }
}

void QFEvaluationItemFactory::addEvalPlugins(const QString &filename, QList<QFPluginEvaluationItem *> records)
{
    for (int i=0; i<records.size(); i++) {
        addEvalPlugin(filename, records[i]);
    }
}

void QFEvaluationItemFactory::finalizePluginSearch()
{

}




QStringList QFEvaluationItemFactory::getIDList() {
    return items.keys();
}

QString QFEvaluationItemFactory::getDescription(QString ID) {
    if (items.contains(ID)) return items[ID]->getDescription();
    return QString("");
};

QString QFEvaluationItemFactory::getName(QString ID) {
    if (items.contains(ID)) return items[ID]->getName();
    return QString("");
};

QFEvaluationItem* QFEvaluationItemFactory::createRecord(QString ID, QFPluginServices* /*services*/, QFProject* parent)  {
    if (items.contains(ID)) return items[ID]->createRecord(parent);
    return NULL;
};


QString QFEvaluationItemFactory::getAuthor(QString ID) {
    if (items.contains(ID)) return items[ID]->getAuthor();
    return QString("");
};

QString QFEvaluationItemFactory::getCopyright(QString ID) {
    if (items.contains(ID)) return items[ID]->getCopyright();
    return QString("");
};

QString QFEvaluationItemFactory::getWeblink(QString ID) {
    if (items.contains(ID)) return items[ID]->getWeblink();
    return QString("");
};

QString QFEvaluationItemFactory::getIconFilename(QString ID) {
    if (items.contains(ID)) return items[ID]->getIconFilename();
    return QString("");
};

QString QFEvaluationItemFactory::getPluginFilename(QString ID) {
    if (items.contains(ID)) return filenames[ID];
    return QString("");
};

void QFEvaluationItemFactory::registerMenu(QString ID, QMenu* menu)  {
    if (items.contains(ID)) {
        return items[ID]->registerToMenu(menu);
    }
};

QString QFEvaluationItemFactory::getPluginHelp(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(ID);
    }
    return "";
}

QString QFEvaluationItemFactory::getPluginTutorialMain(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFEvaluationItemFactory::getPluginFAQ(QString ID)
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

void QFEvaluationItemFactory::getPluginTutorials(const QString &ID, QStringList &names, QStringList &files)
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

QString QFEvaluationItemFactory::getPluginSettings(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/settings.html").arg(basename);
    }
    return "";
}
QString QFEvaluationItemFactory::getPluginCopyrightFile(QString ID) {
    if (items.contains(ID)) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}

bool QFEvaluationItemFactory::contains(QString ID) {
    return items.contains(ID);
}

