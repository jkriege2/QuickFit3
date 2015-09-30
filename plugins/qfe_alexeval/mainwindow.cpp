/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "mainwindow.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QApplication>
#include "DialogReconstructMarkerRecords.h"
#include<QDebug>
#include<QFileDialog>

static QPointer<QtLogFile> logFile=NULL;


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    ad=new AlexData;
    fd=new FCSdata;

    setMinimumSize(700,700);
    loadSettings();
    createWidgets();
    loadWidgetsSettings();
    connect(pushButtonBatch,SIGNAL(clicked()),this,SLOT(on_pushButtonBatch_clicked()));

//    ad->init(); // overwrites the last settings
    //    //qDebug()<<ad->printParamsBurstSearch();
}



void MainWindow::on_mainTabs_currentChanged(int index)
{
    //qDebug()<<"on_mainTabs_currentChanged "<<index;
    switch(index) {
    case 0:
//        widPhotons;
//        widBurstSearch;
        fd->clearMarker(); // FIXME if a new file is loaded, the marker ACF must be cleared
        break;
    case 1:
        widBurstView->refresh();
        break;
    case 2:
        widBurstStatisticsView->evaluate();
        break;
    case 3:
//        widAlex->evaluate();
        break;
    case 4:
        widFCS->plotAlex();
        break;
    }

}

void MainWindow::on_pushButtonBatch_clicked()
{
    AlexEvalLog::text("batch mode");
    logWidget->clearLogStore();
    QStringList files = QFileDialog::getOpenFileNames(this,"choose files for batch processing",ad->currentDir,"TTTR (*.t3r);;All (*.*)"); //"TTTR (*.t3r);;All (*.*)"
    if(files.isEmpty()) {AlexEvalLog::text("no files selected");return;}
//    if(ad->iniFilename.isEmpty()) {
//        AlexEvalLog::text("loadIni: ini filename=" + ad->iniFilename);
//        if(ad->init()) {
//            AlexEvalLog::warning("Error reading ini file "+ad->iniFilename);
//            ad->iniFilename="";
//            ad->init();
//        }
//    } else {
//        // FIXME do something
//    }
    logWidget->open_logfile(ad->currentDir+"/"+FILENAME_LOGFILE,true);

    for(QList<QString>::const_iterator file = files.begin();file!=files.end();++file) {
        widPhotons->automatedRun(*file);
        widBurstSearch->automatedRun();
        widAlex->automatedRun();
    }
    // save log file

    logWidget->close_logfile();
}

void MainWindow::intLogError(const QString &message)
{
    logWidget->log_error_linebreak(message);
}

void MainWindow::intLogWarning(const QString &message)
{
    logWidget->log_warning_linebreak(message);
}

void MainWindow::intLogText(const QString &message)
{
    logWidget->log_text_linebreak(message);
}



void MainWindow::createWidgets()
{
    //qDebug("creating widgets...");
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->addWidget(new QLabel("<center><b>WARNING: This is a beta-version!!! There is no support for this plugin and it has not yet been tested fully!!!</b></center>"));
    setLayout(lay);
    splitLog=new QSplitter(this);
    splitLog->setOrientation(Qt::Vertical);
    lay->addWidget(splitLog);
    mainTabs=new QTabWidget(this);
    splitLog->addWidget(mainTabs);
    logWidget=new QtLogFile(this);
    logFile=logWidget;
    splitLog->addWidget(logWidget);

    //qDebug("creating PhotonsWidget...");
    widPhotons=new PhotonsWidget(this,ad);
    //qDebug("creating BurstSearchWidget...");
    widBurstSearch=new BurstSearchWidget(this,ad);
    QVBoxLayout* layPhotonsAndBurstSearch=new QVBoxLayout(this);

    layPhotonsAndBurstSearch->addWidget(widPhotons,1);
    layPhotonsAndBurstSearch->addWidget(widBurstSearch,0);
    pushButtonBatch= new QPushButton("&Batch Evaluation",this);
    layPhotonsAndBurstSearch->addWidget(pushButtonBatch);

    QWidget* widPhotonsAndBurstSearch=new QWidget(this);
    widPhotonsAndBurstSearch->setLayout(layPhotonsAndBurstSearch);
    mainTabs->addTab(widPhotonsAndBurstSearch, tr("Import and Burst Search"));


    //qDebug("creating BurstView...");
    widBurstView=new BurstView(this,ad);
    mainTabs->addTab(widBurstView, tr("Burst View"));

    //qDebug("creating BurstStatisticsView...");
    widBurstStatisticsView=new BurstStatisticsView(this,ad);
    mainTabs->addTab(widBurstStatisticsView, tr("Burst Statistics"));

    //qDebug("creating ALEXHistogramsWidget...");
    widAlex=new ALEXHistogramsWidget(this,ad);
    mainTabs->addTab(widAlex, tr("ALEX Histograms"));

    //qDebug("creating FCSwidget...");
    widFCS=new FCSwidget(this,ad,fd);
    mainTabs->addTab(widFCS, tr("FCS"));

    connect(mainTabs,SIGNAL(currentChanged(int)),this,SLOT(on_mainTabs_currentChanged(int)));
    connect(widPhotons,SIGNAL(backgroundCalculated()),widAlex,SLOT(setBackground()));

}



void MainWindow::loadWidgetsSettings()
{
    QSettings settings(QApplication::applicationDirPath()+"/" FILENAME_SETTINGS,QSettings::IniFormat);

    widPhotons->loadSettings(settings, "photons_widget/");
    widBurstSearch->loadSettings(settings,"burstsearch_widget/");
    widAlex->loadSettings(settings,"alexhistograms_widget/");
    widBurstView->loadSettings(settings,"BurstView_widget/");
    widBurstStatisticsView->loadSettings(settings,"BurstStatisticsView_widget/");
    widFCS->loadSettings(settings,"FCS_widget/");
    logWidget->saveSettings(settings,"logFile_widget/");
}

void MainWindow::loadSettings()
{
    QSettings settings(QApplication::applicationDirPath()+"/" FILENAME_SETTINGS,QSettings::IniFormat);
    settings.beginGroup("mainwindow");
    ad->currentDir=settings.value("working_directory", ad->currentDir).toString();
    ad->iniFilename=settings.value("alex_ini", ad->iniFilename).toString();
    ad->TTTRfilename=settings.value("TTTRfilename", ad->TTTRfilename).toString();
    this->restoreGeometry(settings.value("geometry",this->geometry()).toByteArray());
//    splitLog->restoreState(settings.value("splitterGeometry").toByteArray());
    settings.endGroup();
}

void MainWindow::storeSettings()
{
    QSettings settings(QApplication::applicationDirPath()+"/" FILENAME_SETTINGS,QSettings::IniFormat);
    settings.beginGroup("mainwindow");
    settings.setValue("working_directory", ad->currentDir);
    settings.setValue("TTTRfilename", ad->TTTRfilename);
    settings.setValue("TTTRfilename", ad->TTTRfilename);
    settings.setValue("geometry",this->saveGeometry());
    settings.setValue("splitterGeometry",splitLog->saveState());
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QApplication::applicationDirPath()+"/" FILENAME_SETTINGS,QSettings::IniFormat);
    this->storeSettings();
    widPhotons->storeSettings(settings,"photons_widget/");
    widBurstSearch->storeSettings(settings,"burstsearch_widget/");
    widAlex->storeSettings(settings,"alexhistograms_widget/");
    widBurstView->storeSettings(settings,"BurstView_widget/");
    widBurstStatisticsView->storeSettings(settings,"BurstStatisticsView_widget/");
    widFCS->storeSettings(settings,"FCS_widget/");
    logWidget->saveSettings(settings,"logFile_widget/");
}

