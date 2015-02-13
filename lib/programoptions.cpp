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

#include "programoptions.h"
#include <iostream>
#include <QtCore>
#include <QDir>
#include <QNetworkProxy>
#include "qftools.h"
#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif

ProgramOptions* ProgramOptions::inst=NULL;

ProgramOptions::ProgramOptions( QString ini, QObject * parent, QApplication* app  ):
    QObject(parent)
{

    userSaveAfterFirstEdit=true;
    childWindowsStayOnTop=true;
    helpWindowsStayOnTop=true;
    projectWindowStayOnTop=false;
    m_debugLogVisible=false;

    proxyHost=QNetworkProxy::applicationProxy().hostName();
    proxyType=QNetworkProxy::applicationProxy().type();
    proxyPort=QNetworkProxy::applicationProxy().port();

    QFileInfo fi(app->applicationFilePath());
    appDir=app->applicationDirPath();
    #if defined(Q_OS_MAC)
        // get out of /MyApp.app/Contents/MacOS in MacOSX App-Bundle
        QDir aappDir(appDir);
        if (aappDir.dirName() == "MacOS") {
            aappDir.cdUp();
            aappDir.cdUp();
            aappDir.cdUp();
        }
        appDir=	aappDir.absolutePath();
    #endif
    globalConfigDir=appDir+"/globalconfig/";
    configDir=QDir::homePath()+"/."+fi.completeBaseName()+"/";
    assetsDir=appDir+"/assets/";
    examplesDir=appDir+"/examples/";
    pluginsDir=appDir+"/plugins/";

    #ifdef __WINDOWS__
    #endif

    #if defined(__LINUX__) || defined(Q_OS_MAC)
    globalConfigDir=QString("%1/quickfit3/").arg(GLOBALCONFIGDIR);
    #endif




    QDir d(appDir);
    d.mkpath(configDir);
    d.mkpath(globalConfigDir);

    this->app=app;
    iniFilename=ini;
    if (iniFilename.isEmpty()) {
        iniFilename= configDir+"/"+fi.completeBaseName()+".ini";
    }
    currentRawDataDir=fi.absolutePath();
    //std::cout<<"config file is: "<<iniFilename.toStdString()<<std::endl;
    settings=NULL;

    // default values
    style= app->style()->metaObject()->className();
    stylesheet="default";
    languageID="en";
    maxThreads=qMax(4,QThread::idealThreadCount());
    autosave=5;

    readSettings();
    if (inst==NULL) inst=this;
}

ProgramOptions::~ProgramOptions()
{
    writeSettings();
    if (inst==this) inst=NULL;
}

void ProgramOptions::setCurrentRawDataDir(QString d)
{
    currentRawDataDir=d;
    writeSettings();
}


void ProgramOptions::writeSettings() {
    settings->setValue("quickfit/language", languageID);
    settings->setValue("quickfit/stylesheet", stylesheet);
    settings->setValue("quickfit/style", style);
    settings->setValue("quickfit/max_threads", maxThreads);
    settings->setValue("quickfit/autosave", autosave);
    settings->setValue("quickfit/current_raw_data_dir", currentRawDataDir);
    settings->setValue("quickfit/userSaveAfterFirstEdit", userSaveAfterFirstEdit);
    settings->setValue("quickfit/childWindowsStayOnTop", childWindowsStayOnTop);
    settings->setValue("quickfit/helpWindowsStayOnTop", helpWindowsStayOnTop);
    settings->setValue("quickfit/projectWindowStayOnTop", projectWindowStayOnTop);
    settings->setValue("quickfit/debugLogVisible", m_debugLogVisible);
    settings->setValue("quickfit/proxy/host", proxyHost);
    settings->setValue("quickfit/proxy/port", proxyPort);
    settings->setValue("quickfit/proxy/type", proxyType);
}


void ProgramOptions::readSettings() {
    if (!settings) {
        //qDebug()<<"iniFilename="<<iniFilename;
        if (iniFilename=="native") { // on windows: registry, on Linux/MacOS: default
            settings= new QSettings(this);
        } else if (iniFilename=="native_inifile") { // ensures a INI file at the default location, even on windows
            settings= new QSettings(QSettings::IniFormat, QSettings::UserScope, app->organizationName(), app->applicationName(), this);
        } else {
            settings= new QSettings(iniFilename, QSettings::IniFormat, this);
        }
    }
    maxThreads=settings->value("quickfit/max_threads", maxThreads).toInt();
    autosave=settings->value("quickfit/autosave", autosave).toInt();
    currentRawDataDir=settings->value("quickfit/current_raw_data_dir", currentRawDataDir).toString();
    userSaveAfterFirstEdit=settings->value("quickfit/userSaveAfterFirstEdit", userSaveAfterFirstEdit).toBool();
    childWindowsStayOnTop=settings->value("quickfit/childWindowsStayOnTop", childWindowsStayOnTop).toBool();
    helpWindowsStayOnTop=settings->value("quickfit/helpWindowsStayOnTop", helpWindowsStayOnTop).toBool();
    projectWindowStayOnTop=settings->value("quickfit/projectWindowStayOnTop", projectWindowStayOnTop).toBool();
    m_debugLogVisible=settings->value("quickfit/debugLogVisible", m_debugLogVisible).toBool();

    proxyHost=(settings->value("quickfit/proxy/host", proxyHost).toString());
    proxyPort=(settings->value("quickfit/proxy/port", proxyPort).toUInt());
    proxyType=((QNetworkProxy::ProxyType)settings->value("quickfit/proxy/type", proxyType).toInt());


    languageID=settings->value("quickfit/language", languageID).toString();
    if (languageID != "en") { // english is default
        QDir d(assetsDir+"/translations");
        QStringList filters;
        filters << "*.qm";
        QStringList sl=qfDirListFilesRecursive(d, filters);//d.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            QString s=sl[i];
            if (s.startsWith(languageID+".")) {
                QString fn=d.absoluteFilePath(s);
                //std::cout<<"loading translation '"<<fn.toStdString()<<"' ... \n";
                QTranslator* translator=new QTranslator(this);
                if (app && translator->load(fn)) {
                    app->installTranslator(translator);
                    //std::cout<<"OK\n";
                } else {
                    //std::cout<<"ERROR\n";
                }
            }
        }
        emit languageChanged(languageID);

    }
    style=settings->value("quickfit/style", style).toString();
    if (app) {
        app->setStyle(style);
    }
    emit styleChanged(style);
    stylesheet=settings->value("quickfit/stylesheet", stylesheet).toString();
    if (app) {
        QString fn=QString(appDir+"/stylesheets/%1.qss").arg(stylesheet);
        //std::cout<<"loading stylesheet '"<<fn.toStdString()<<"' ... ";
        QFile f(fn);
        if (f.open(QFile::ReadOnly)) {
            QTextStream s(&f);
            QString qss=s.readAll();
            //std::cout<<qss.toStdString()<<std::endl;
            app->setStyleSheet(qss);
        } else {
            app->setStyleSheet("");
        }
        //std::cout<<"OK\n";
    }
    emit stylesheetChanged(stylesheet);

}

QString ProgramOptions::getConfigFileDirectory() const {
    return configDir;
}

QString ProgramOptions::getGlobalConfigFileDirectory() const {
    return globalConfigDir;
}

QString ProgramOptions::getPluginDirectory() const {
    return pluginsDir;
}

QString ProgramOptions::getAssetsDirectory() const {
    return assetsDir;
}

QString ProgramOptions::getExamplesDirectory() const {
    return examplesDir;
}
QString ProgramOptions::getMainHelpDirectory() const
{
    return assetsDir+"/help/";
}

QString ProgramOptions::getApplicationDirectory() const {
    return appDir;
}

QString ProgramOptions::getSourceDirectory() const
{
    return appDir+"/source/";

}

QString ProgramOptions::getHomeQFDirectory() const
{
    QString cfg=getConfigValue("quickfit/homedir", "").toString();
    if (!cfg.isEmpty() && QDir(cfg).exists()) return cfg;

    QDir d=QDir::home();
    if (!QDir(d.absolutePath()+"/quickfit3").exists()) d.mkdir("quickfit3");
    return d.absolutePath()+"/quickfit3/";
}

void ProgramOptions::setHomeQFDirectory(const QString &dir)
{
    setConfigValue("quickfit/homedir", dir);
}


void ProgramOptions::setProxyHost(const QString &host)
{
    //proxy.setHostName(host);
    proxyHost=host;
}

void ProgramOptions::setProxyPort(quint16 port)
{
    //proxy.setPort(port);
    proxyPort=port;
}

quint16 ProgramOptions::getProxyPort() const
{
    //return proxy.port();
    return proxyPort;
}

QString ProgramOptions::getProxyHost() const
{
    //return proxy.hostName();
    return proxyHost;
}

int ProgramOptions::getProxyType() const
{
    //return proxy.type();
    return proxyType;
}

void ProgramOptions::setProxyType(int type)
{
    if (type<0) proxyType=0; //proxy.setType(QNetworkProxy::DefaultProxy);
    else proxyType=type; //proxy.setType(type);
}


bool ProgramOptions::getUserSaveAfterFirstEdit() const
{
    return userSaveAfterFirstEdit;
}

void ProgramOptions::setUserSaveAfterFirstEdit(bool set)
{
    userSaveAfterFirstEdit=set;
    writeSettings();
}

bool ProgramOptions::getChildWindowsStayOnTop() const
{
    return childWindowsStayOnTop;
}

void ProgramOptions::setChildWindowsStayOnTop(bool set)
{
    childWindowsStayOnTop=set;
    writeSettings();
}

bool ProgramOptions::getHelpWindowsStayOnTop() const
{
    return helpWindowsStayOnTop;
}

void ProgramOptions::setHelpWindowsStayOnTop(bool set)
{
    helpWindowsStayOnTop=set;
    writeSettings();
}

bool ProgramOptions::getProjectWindowsStayOnTop() const
{
    return projectWindowStayOnTop;
}

void ProgramOptions::setProjectWindowsStayOnTop(bool set)
{
    projectWindowStayOnTop=set;
    writeSettings();
}

bool ProgramOptions::debugLogVisible() const
{
    return m_debugLogVisible;
}

void ProgramOptions::setDebugLogVisible(bool visible)
{
    m_debugLogVisible=visible;
    writeSettings();
}

void ProgramOptions::setLanguageID(QString id) {
    languageID=id;
    writeSettings();
}

void ProgramOptions::setStylesheet(QString st) {
    stylesheet=st;
    writeSettings();
}

void ProgramOptions::setStyle(QString st) {
    style=st;
    writeSettings();
}

void ProgramOptions::setMaxThreads(int threads)
{
    maxThreads=threads;
    writeSettings();
}

void ProgramOptions::setAutosave(int interval)
{
    autosave=interval;
    writeSettings();
}


