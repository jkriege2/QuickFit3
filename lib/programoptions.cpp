#include "programoptions.h"
#include <iostream>

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

ProgramOptions::ProgramOptions( QString ini, QObject * parent, QApplication* app  ):
    QObject(parent)
{
    QFileInfo fi(QApplication::applicationFilePath());

    globalConfigDir=QApplication::applicationDirPath()+"/globalconfig/";
    configDir=QDir::homePath()+"/."+fi.completeBaseName()+"/";
    assetsDir=QApplication::applicationDirPath()+"/assets/";
    pluginsDir=QApplication::applicationDirPath()+"/plugins/";

    #ifdef __WINDOWS__
    //configDir=QApplication::applicationDirPath();
    #endif

    #ifdef __LINUX__
    //configDir=QApplication::applicationDirPath();
    globalConfigDir="/usr/share/quickfit3/";
    #endif


    QDir d(QApplication::applicationDirPath());
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
    style= QApplication::style()->metaObject()->className();
    stylesheet="default";
    languageID="en";
    maxThreads=1;
    autosave=5;

    readSettings();
}

ProgramOptions::~ProgramOptions()
{
    writeSettings();
}


void ProgramOptions::writeSettings() {
    settings->setValue("quickfit/language", languageID);
    settings->setValue("quickfit/stylesheet", stylesheet);
    settings->setValue("quickfit/style", style);
    settings->setValue("quickfit/max_threads", maxThreads);
    settings->setValue("quickfit/autosave", autosave);
    settings->setValue("quickfit/current_raw_data_dir", currentRawDataDir);
}


void ProgramOptions::readSettings() {
    if (!settings) {
        qDebug()<<"iniFilename="<<iniFilename;
        if (iniFilename=="native") { // on windows: registry, on Linux/MacOS: default
            settings= new QSettings(this);
        } else if (iniFilename=="native_inifile") { // ensures a INI file at the default location, even on windows
            settings= new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName(), this);
        } else {
            settings= new QSettings(iniFilename, QSettings::IniFormat, this);
        }
    }
    maxThreads=settings->value("quickfit/max_threads", maxThreads).toInt();
    autosave=settings->value("quickfit/autosave", autosave).toInt();
    currentRawDataDir=settings->value("quickfit/current_raw_data_dir", currentRawDataDir).toString();

    languageID=settings->value("quickfit/language", languageID).toString();
    if (languageID != "en") { // english is default
        QDir d(assetsDir+"/translations");
        QStringList filters;
        filters << "*.qm";
        QStringList sl=d.entryList(filters, QDir::Files);
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
        QString fn=QString(QCoreApplication::applicationDirPath()+"/stylesheets/%1.qss").arg(stylesheet);
        //std::cout<<"loading stylesheet '"<<fn.toStdString()<<"' ... ";
        QFile f(fn);
        f.open(QFile::ReadOnly);
        QTextStream s(&f);
        QString qss=s.readAll();
        //std::cout<<qss.toStdString()<<std::endl;
        app->setStyleSheet(qss);
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

void ProgramOptions::setLanguageID(QString id) {
    languageID=id;
}

void ProgramOptions::setStylesheet(QString st) {
    stylesheet=st;
}

void ProgramOptions::setStyle(QString st) {
    style=st;
}
