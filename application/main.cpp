/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "mainwindow.h"
#include "qfversion.h"
#include "../lib/programoptions.h"
#include "../lib/qftools.h"
#include "jkqtpbaseplotter.h"
#include "jkqtpimagetools.h"

#include <QtGlobal>
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "qfsplashscreen.h"
#include <QPainter>

#ifdef _OPENMP
# include <omp.h>
#endif

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

int main(int argc, char * argv[])
{
    bool nosplash=false;
    for (int i=0; i<argc; i++) {
        if (QString(argv[i])=="--nosplash") nosplash=true;
        if (QString(argv[i])=="--getbits") {
            std::cout<<sizeof(void*)*8<<std::endl;
            return 0;
        }
    }

#ifdef __WINDOWS__
    QCoreApplication::addLibraryPath("./qtplugins");
    QCoreApplication::addLibraryPath(QFileInfo(argv[0]).absolutePath()+"/qtplugins");
#endif
#ifdef Q_OS_MAC
    qDebug()<<"lib paths:"<<QCoreApplication::libraryPaths();
    QStringList macoslibpaths;
    macoslibpaths<<"../QtPlugIns";
    macoslibpaths<<(QFileInfo(argv[0]).absolutePath()+"/../QtPlugIns");
    QCoreApplication::setLibraryPaths(macoslibpaths);
    qDebug()<<"lib paths:"<<QCoreApplication::libraryPaths();
#endif

    Q_INIT_RESOURCE(quickfit3);
    int res=0;
    {
        QApplication app(argc, argv);

        {
            if (!QFile::exists(app.applicationDirPath()+"/plugins/ATMCD32D.dll") && QFile::exists(app.applicationDirPath()+"/ATMCD32D.dll")) {
                QFile::copy(app.applicationDirPath()+"/ATMCD32D.dll", app.applicationDirPath()+"/plugins/ATMCD32D.dll");
            }
            if (!QFile::exists(app.applicationDirPath()+"/plugins/atmcd64d.dll") && QFile::exists(app.applicationDirPath()+"/atmcd64d.dll")) {
                QFile::copy(app.applicationDirPath()+"/atmcd64d.dll", app.applicationDirPath()+"/plugins/atmcd64d.dll");
            }

            app.setOrganizationName("German Cancer Research Center: B040 (Biophysics of Macromolecules)");
            app.setApplicationName(QString("QuickFit %1 (SVN %2 DATE %3)").arg(qfInfoVersionFull()).arg(QString(qfInfoSVNVersion()).trimmed()).arg(QString(qfInfoCompileDate()).trimmed()));
            app.setOrganizationDomain("http://www.dkfz.de/Macromol");
            app.setApplicationVersion(QString("%1 (%2 SVN %3 DATE %4)").arg(qfInfoVersionFull()).arg(qfInfoVersionStatus()).arg(QString(qfInfoSVNVersion()).trimmed()).arg(QString(qfInfoCompileDate()).trimmed()));
            app.setWindowIcon(QIcon(":/icon_large.png"));

            QFSplashScreen* splash=new QFSplashScreen();

            QPixmap pixmap(":/splash.png");
            QPainter* painter=new QPainter(&pixmap);
            painter->setFont(QFont("Arial", 9));
            painter->drawText(QPoint(5,290), QString("version %1 (%2 SVN %3 DATE %4), %5-bit").arg(qfInfoVersionFull()).arg(qfInfoVersionStatus()).arg(QString(qfInfoSVNVersion()).trimmed()).arg(QString(qfInfoCompileDate()).trimmed()).arg(getApplicationBitDepth()));
            painter->end();
            delete painter;
            painter=NULL;
            splash->setPixmap(pixmap);//,Qt::WindowStaysOnTopHint);


            splash->showMessage("initializing ...");
            if (!nosplash) splash->show();
            app.processEvents();
            app.processEvents();
            app.processEvents();


            app.processEvents();

            ProgramOptions* settings=new ProgramOptions("", &app, &app);

            JKQtBasePlotter::setDefaultJKQtBasePrinterUserSettings(settings->getIniFilename(), "JKQtBasePlotterUserSettings/");

            if (settings->debugLogVisible()) {
                settings->setDebugLogVisible(false);
                QMessageBox::warning(NULL, QObject::tr("debug messages"), QObject::tr("You activated the debug message output in the last run of QuickFit. As this function is known to be instable and cause crashes in multi-threaded applications, it is now deactivated. If you absolutely need this function and know what you are doing, please reactivate it in the settinsg dialog!"));
            }

            splash->showMessage("loading XITS fonts ...");
            app.processEvents();
            app.processEvents();
            app.processEvents();
            //if (!fdb.families().contains("XITS")) {
            QList<int> appFonts;
            if (QFile::exists(":/JKQTmathText/fonts/xits-bold.otf")) { appFonts<<QFontDatabase::addApplicationFont(QLatin1String(":/JKQTmathText/fonts/xits-bold.otf")); }
            if (QFile::exists(":/JKQTmathText/fonts/xits-bolditalic.otf")) { appFonts<<QFontDatabase::addApplicationFont(QLatin1String(":/JKQTmathText/fonts/xits-bolditalic.otf")); }
            if (QFile::exists(":/JKQTmathText/fonts/xits-italic.otf")) { appFonts<<QFontDatabase::addApplicationFont(QLatin1String(":/JKQTmathText/fonts/xits-italic.otf")); }
            if (QFile::exists(":/JKQTmathText/fonts/xits-math.otf")) { appFonts<<QFontDatabase::addApplicationFont(QLatin1String(":/JKQTmathText/fonts/xits-math.otf")); }
            if (QFile::exists(":/JKQTmathText/fonts/xits-mathbold.otf")) { appFonts<<QFontDatabase::addApplicationFont(QLatin1String(":/JKQTmathText/fonts/xits-mathbold.otf")); }
            if (QFile::exists(":/JKQTmathText/fonts/xits-regular.otf")) { appFonts<<QFontDatabase::addApplicationFont(QLatin1String(":/JKQTmathText/fonts/xits-regular.otf")); }
            //}
            splash->showMessage("initializing ...");
            app.processEvents();
            app.processEvents();
            app.processEvents();

            qRegisterMetaType<QTextCursor>("QTextCursor");
            MainWindow win(settings, splash);
            splash->setParent(&win);
            win.show();
            QTimer::singleShot(2500, splash, SLOT(close()));

            // run application
            res=app.exec();

            //QFontDatabase::removeAllApplicationFonts();
        }

        /*for (int i=0; i<appFonts.size(); i++) {
            //qDebug()<<appFonts[i];
            QFontDatabase::removeApplicationFont(appFonts[i]);
        }*/
        //appFonts.clear();
        //QFontDatabase::removeAllApplicationFonts();
    }

    // clean up some internal stores ...
    //qDebug()<<"free LUTs";
    JKQTPimagePlot_freeLUTs(global_jkqtpimagetools_lutstore);
    // clean up some internal stores ...
    //qDebug()<<"cleanup resource";
    Q_CLEANUP_RESOURCE(quickfit3);

    // return app result
    return res;
}
