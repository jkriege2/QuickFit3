#include "mainwindow.h"
#include "version.h"
#include "../lib/programoptions.h"

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
    Q_INIT_RESOURCE(quickfit3);
    QApplication app(argc, argv);

    app.setOrganizationName("");
    app.setApplicationName(QString("QuickFit %1").arg(AutoVersion::FULLVERSION_STRING));
    app.setOrganizationDomain("http://www.jkrieger.de/");
    app.setApplicationVersion(QString("%1 (%2 BUILD %3)").arg(AutoVersion::FULLVERSION_STRING).arg(AutoVersion::STATUS).arg(AutoVersion::BUILDS_COUNT));

    QPixmap pixmap(":/splash.png");
    QPainter* painter=new QPainter(&pixmap);
    painter->setFont(QFont("Arial", 9));
    painter->drawText(QPoint(5,290), QString("version %1 (%2 BUILD %3)").arg(AutoVersion::FULLVERSION_STRING).arg(AutoVersion::STATUS).arg(AutoVersion::BUILDS_COUNT));
    delete painter;
    painter=NULL;
    QSplashScreen splash(pixmap,Qt::WindowStaysOnTopHint);
    splash.showMessage("initializing ...");
    splash.show();
    app.processEvents();
    app.processEvents();
    app.processEvents();

    #ifdef __WINDOWS__
      app.addLibraryPath(QCoreApplication::applicationDirPath()+"/qtplugins");
    #endif
    //splash.showMessage(QString("version %1 (%2 BUILD %3)").arg(AutoVersion::FULLVERSION_STRING).arg(AutoVersion::STATUS).arg(AutoVersion::BUILDS_COUNT));
    app.processEvents();

    /*for (int i=0; i<10000; i++) {
        std::cout<<"Test "<<i<<std::endl;
    }*/
    /** \brief this object manages program settings */
    ProgramOptions* settings=new ProgramOptions("", &app, &app);

    MainWindow win(settings, &splash);
    win.show();
    QTimer::singleShot(2500, &splash, SLOT(close()));
    return app.exec();
}
