#include "programoptions.h"
#include <iostream>

ProgramOptions::ProgramOptions( QString ini, QObject * parent, QApplication* app  ):
    QObject(parent)
{
    this->app=app;
    iniFilename=ini;
    QFileInfo fi(QCoreApplication::applicationFilePath());
    if (iniFilename.isEmpty()) {
        iniFilename= fi.absolutePath()+"/"+fi.completeBaseName()+".ini";
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
        QString fn=QString(QCoreApplication::applicationDirPath()+"/translations/%1.qm").arg(languageID);
        //std::cout<<"loading translation '"<<fn.toStdString()<<"' ... ";
        QTranslator* translator=new QTranslator(this);
        if (app && translator->load(fn)) {
            app->installTranslator(translator);
            //std::cout<<"OK\n";
            emit languageChanged(languageID);
        } else {
            //std::cout<<"ERROR\n";
        }
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



