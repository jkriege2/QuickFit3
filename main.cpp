#include "mainwindow.h"
#include "version.h"
#include "programoptions.h"

#include <QtGui>
#include <QSplashScreen>

int main(int argc, char * argv[])
{
    Q_INIT_RESOURCE(quickfit3);
    QApplication app(argc, argv);
    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap,Qt::WindowStaysOnTopHint);
    splash.show();
    app.processEvents();
    app.processEvents();
    app.processEvents();

    app.setOrganizationName("");
    app.setApplicationName(QString("QuickFit %1").arg(AutoVersion::FULLVERSION_STRING));
    app.setOrganizationDomain("http://www.jkrieger.de/");
    app.setApplicationVersion(QString("%1 (%2 BUILD %3)").arg(AutoVersion::FULLVERSION_STRING).arg(AutoVersion::STATUS).arg(AutoVersion::BUILDS_COUNT));
    app.addLibraryPath(QCoreApplication::applicationDirPath()+"/qtplugins");
    splash.showMessage(QString("version %1 (%2 BUILD %3)").arg(AutoVersion::FULLVERSION_STRING).arg(AutoVersion::STATUS).arg(AutoVersion::BUILDS_COUNT));
    app.processEvents();

    /*for (int i=0; i<10000; i++) {
        std::cout<<"Test "<<i<<std::endl;
    }*/
    /** \brief this object manages program settings */
    ProgramOptions* settings=new ProgramOptions("", &app, &app);

    MainWindow win;
    win.setSettings(settings);
    win.show();
    //splash.finish(&win);
    QTimer::singleShot(2500, &splash, SLOT(close()));
    return app.exec();
}
