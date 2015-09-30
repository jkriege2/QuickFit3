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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include "photonswidget.h"
#include "burstsearchwidget.h"
#include "alexhistogramswidget.h"
#include "BurstStatisticsView.h"
#include "BurstView.h"
#include "alexdata.h"
#include <QSettings>
//#include "logfile.h"
#include "qtlogfile.h"
#include <QSplitter>
#include "alexevallog.h"
#include "FCSwidget.h"
#include <QPushButton>

#define FILENAME_SETTINGS "alexeval.ini"
#define FILENAME_LOGFILE "alexeval.log"


void myMessageOutput(QtMsgType type, const char *msg);

class MainWindow : public QWidget, public AlexEvalLog {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

friend class PhotonsWidget;
friend class BurstSearchWidget;
friend class BurstStatisticsView;
friend class BurstView;
friend class ALEXHistogramsWidget;
friend class FCSwidget;
friend class QtLogFile;



signals:

public slots:
    void on_mainTabs_currentChanged(int index);
    void on_pushButtonBatch_clicked();

protected:
    /// handling for the log widget
    void intLogError(const QString& message);
    void intLogWarning(const QString& message);
    void intLogText(const QString& message);


    /// creates the friend widgets in tabs
    void createWidgets();
    PhotonsWidget* widPhotons;
    BurstSearchWidget* widBurstSearch;
    BurstStatisticsView* widBurstStatisticsView;
    BurstView* widBurstView;
    ALEXHistogramsWidget* widAlex;
    FCSwidget* widFCS;
    QTabWidget* mainTabs;
    QtLogFile* logWidget;
    QSplitter* splitLog;
    QPushButton* pushButtonBatch;

    /// the data class instance which is shared by all widgets
    AlexData* ad;
    FCSdata* fd;

    /// calls the loadSettings methods of all widgets
    void loadWidgetsSettings();
    /// load the last user settings for the main window from ini file
    void loadSettings();
    /// saves the user settings from ini file
    void storeSettings();

    /// called when the application is closed. It will call the storeSettings() functions of all widgets
    virtual void closeEvent(QCloseEvent* event);

};


#endif // MAINWINDOW_H
