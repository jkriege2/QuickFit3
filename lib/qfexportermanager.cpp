/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-12-18 17:44:11 +0100 (Do, 18 Dez 2014) $  (revision $Rev: 3687 $)

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
#include "qfexportermanager.h"
#include "qftools.h"

QFExporterManager::QFExporterManager(ProgramOptions* options, QObject *parent) :
    QObject(parent)
{
    m_options=options;
}



void QFExporterManager::finalizePluginSearch()
{

}

bool QFExporterManager::registerPlugin(const QString& filename_in, QObject *plugin, QFPluginHelpData &helpdata)
{
    QString fileName=QFileInfo(filename_in).fileName();

    QFPluginExporters* iRecord = qobject_cast<QFPluginExporters*>(plugin);
    if (iRecord) {
        if (QApplication::arguments().contains("--verboseplugin")) QFPluginServices::getInstance()->log_global_text("    QFPluginExporters OK\n");
        plugins.append(iRecord);
        filenames.append(QFileInfo(filename_in).absoluteFilePath());
        emit showMessage(tr("loaded exporter plugin '%2' (%1) ...").arg(fileName).arg(iRecord->getName()));
        emit showLongMessage(tr("loaded exportern plugin '%2':\n   author: %3\n   copyright: %4\n   file: %1").arg(QFileInfo(filename_in).absoluteFilePath()).arg(iRecord->getName()).arg(iRecord->getAuthor()).arg(iRecord->getCopyright()));
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
        info.plugintypehelp=m_options->getAssetsDirectory()+QString("/help/qf3_exporter.html");
        info.plugintypename=tr("Emporter Plugins");
        info.pluginDLLbasename=libbasename;
        info.pluginDLLSuffix=QFileInfo(fileName).suffix();
        helpdata.pluginHelpList.append(info);

        parseTooltips(info.directory, helpdata.tooltips);
        parseAutolinks(info.directory, helpdata.autolinks);
        parseGlobalreplaces(info.directory);
        return true;
    }
    return false;
}

QFExporter *QFExporterManager::createExporter(const QString &id) const {
    for (int i=0; i<plugins.size(); i++) {
        QStringList ids=plugins[i]->getIDs();
        for (int j=0; j<ids.size(); j++) {
            if (id==ids[j]) return plugins[i]->createExporter(id);
        }
    }
    return NULL;
}


bool QFExporterManager::contains(const QString &ID)
{
    for (int i=0; i<plugins.size(); i++)
        if (plugins[i]->getID()==ID) return true;
    return false;
}

QString QFExporterManager::getName(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getName();
}

QString QFExporterManager::getPluginFilename(int i) const {
    if ((i<0) || (i>=filenames.size())) return "";
    return filenames[i];
}

QString QFExporterManager::getDescription(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getDescription();
}

QString QFExporterManager::getAuthor(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getAuthor();
}

QString QFExporterManager::getCopyright(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getCopyright();
}

QString QFExporterManager::getWeblink(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getWeblink();
}

QString QFExporterManager::getID(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    return plugins[i]->getID();
}

QStringList QFExporterManager::getIDList(int i) const {
    if ((i<0) || (i>=plugins.size())) return QStringList();
    return plugins[i]->getIDs();
}

int QFExporterManager::getPluginForID(QString id) const {
    for (int i=0; i<plugins.size(); i++) {
        if (plugins[i]->getID()==id) return i;
         QStringList ids=plugins[i]->getIDs();
         if (ids.contains(id)) return i;
   }
   return -1;
}

int QFExporterManager::getMajorVersion(int id) {
    if ((id<0) || (id>=plugins.size())) return 0;
    int ma, mi;
    if (id<plugins.size()) {
        plugins[id]->getVersion(ma, mi);
        return ma;
    }
    return 0;
}

int QFExporterManager::getMinorVersion(int id) {
    if ((id<0) || (id>=plugins.size())) return 0;
    int ma, mi;
    if (id<plugins.size()) {
        plugins[id]->getVersion(ma, mi);
        return mi;
    }
    return 0;
}

QString QFExporterManager::getIconFilename(int i) const {
    if ((i<0) || (i>=plugins.size())) return "";
    QString ic= plugins[i]->getIconFilename();
    if (QFile::exists(ic)) return ic;
    else return QString(":/lib/exporter_icon.png");
}


QString QFExporterManager::getPluginHelp(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/%2.html").arg(basename).arg(getID(ID));
    }
    return "";
}

QString QFExporterManager::getPluginTutorialMain(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/tutorial.html").arg(basename);
    }
    return "";
}

QString QFExporterManager::getPluginFAQ(int ID)
{
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/faq.html").arg(basename);
    }
    return "";

}

void QFExporterManager::getPluginTutorials(int ID, QStringList &names, QStringList &files)
{
    if ((ID>=0) && (ID<plugins.size())) {
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

QString QFExporterManager::getPluginSettings(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/settings.html").arg(basename);
    }
    return "";
}
QString QFExporterManager::getPluginCopyrightFile(int ID) {
    if ((ID>=0) && (ID<plugins.size())) {
        QString basename=QFileInfo(getPluginFilename(ID)).baseName();
    #ifndef Q_OS_WIN32
        if (basename.startsWith("lib")) basename=basename.right(basename.size()-3);
    #endif
        return m_options->getAssetsDirectory()+QString("/plugins/help/%1/copyright.html").arg(basename);
    }
    return "";
}




QString QFExporterManager::getPluginHelp(int ID, QString faID) {
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




void QFExporterManager::deinit() {
    for (int i=0; i<pluginCount(); i++) {
        if (plugins.value(i, NULL)) plugins.value(i, NULL)->deinit();
    }
}

void QFExporterManager::init()
{
    for (int i=0; i<pluginCount(); i++) {
        if (plugins.value(i, NULL)) plugins.value(i, NULL)->init();
        emit incProgress();
        if (i%3==0) QApplication::processEvents();
    }
}
