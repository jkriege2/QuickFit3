#include "mainwindow.h"
#include "../version.h"
#include "../lib/programoptions.h"
#include "../lib/qftools.h"
#include "jkqtpbaseplotter.h"


#include <QtGui>
#include <QSplashScreen>
#include <QPainter>

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
    }
    Q_INIT_RESOURCE(quickfit3);
    QApplication app(argc, argv);

    app.setOrganizationName("German Cancer Research Center: B040 (Biophysics of Macromolecules)");
    app.setApplicationName(QString("QuickFit %1 (SVN %2 DATE %3)").arg(VERSION_FULL).arg(QString(SVNVERSION).trimmed()).arg(QString(COMPILEDATE).trimmed()));
    app.setOrganizationDomain("http://www.dkfz.de/Macromol");
    app.setApplicationVersion(QString("%1 (%2 SVN %3 DATE %4)").arg(VERSION_FULL).arg(VERSION_STATUS).arg(QString(SVNVERSION).trimmed()).arg(QString(COMPILEDATE).trimmed()));
    app.setWindowIcon(QIcon(":/icon_large.png"));

    QPixmap pixmap(":/splash.png");
    QPainter* painter=new QPainter(&pixmap);
    painter->setFont(QFont("Arial", 9));
    painter->drawText(QPoint(5,290), QString("version %1 (%2 SVN %3 DATE %4), %5-bit").arg(VERSION_FULL).arg(VERSION_STATUS).arg(QString(SVNVERSION).trimmed()).arg(QString(COMPILEDATE).trimmed()).arg(getApplicationBitDepth()));
    delete painter;
    painter=NULL;
    QSplashScreen splash(pixmap);//,Qt::WindowStaysOnTopHint);
    splash.showMessage("initializing ...");
    if (!nosplash) splash.show();
    app.processEvents();
    app.processEvents();
    app.processEvents();

    #ifdef __WINDOWS__
      app.addLibraryPath(QCoreApplication::applicationDirPath()+"/qtplugins");
    #endif
    app.processEvents();

    ProgramOptions* settings=new ProgramOptions("", &app, &app);

    JKQtBasePlotter::setDefaultJKQtBasePrinterUserSettings(settings->getIniFilename(), "JKQtBasePlotterUserSettings/");

    if (settings->debugLogVisible()) {
        settings->setDebugLogVisible(false);
        QMessageBox::warning(NULL, QObject::tr("debug messages"), QObject::tr("You activated the debug message output in the last run of QuickFit. As this function is known to be instable and cause crashes in multi-threaded applications, it is now deactivated. If you absolutely need this function and know what you are doing, please reactivate it in the settinsg dialog!"));
    }

    QFontDatabase fdb;
    splash.showMessage("loading XITS fonts ...");
    app.processEvents();
    app.processEvents();
    app.processEvents();
    //if (!fdb.families().contains("XITS")) {
        if (QFile::exists(":/JKQTmathText/fonts/xits-bold.otf")) { QFontDatabase::addApplicationFont(":/JKQTmathText/fonts/xits-bold.otf"); }
        if (QFile::exists(":/JKQTmathText/fonts/xits-bolditalic.otf")) { QFontDatabase::addApplicationFont(":/JKQTmathText/fonts/xits-bolditalic.otf"); }
        if (QFile::exists(":/JKQTmathText/fonts/xits-italic.otf")) { QFontDatabase::addApplicationFont(":/JKQTmathText/fonts/xits-italic.otf"); }
        if (QFile::exists(":/JKQTmathText/fonts/xits-math.otf")) { QFontDatabase::addApplicationFont(":/JKQTmathText/fonts/xits-math.otf"); }
        if (QFile::exists(":/JKQTmathText/fonts/xits-mathbold.otf")) { QFontDatabase::addApplicationFont(":/JKQTmathText/fonts/xits-mathbold.otf"); }
        if (QFile::exists(":/JKQTmathText/fonts/xits-regular.otf")) { QFontDatabase::addApplicationFont(":/JKQTmathText/fonts/xits-regular.otf"); }
    //}
    splash.showMessage("initializing ...");
    app.processEvents();
    app.processEvents();
    app.processEvents();

    qRegisterMetaType<QTextCursor>("QTextCursor");
    MainWindow win(settings, &splash);
    win.show();
    QTimer::singleShot(2500, &splash, SLOT(close()));
    return app.exec();
}
