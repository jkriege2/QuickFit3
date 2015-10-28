#include "qfeb040ffmcontrol.h"

#define LOG_PREFIX QString("b040_ffmcontrol >>> ").toUpper()

QFEB040FFMControl::QFEB040FFMControl(QObject* parent):
    QObject(parent)
{
    logService=NULL;
    plugin_open = false;
    mw=NULL;
}

QFEB040FFMControl::~QFEB040FFMControl() {

}


void QFEB040FFMControl::deinit() {
    /* add code for cleanup here */
}
/*
MulticontrolStage *QFEB040FFMControl::get_stage()
{
    if (stage)
        return stage;
    else
        return NULL;
}
*/

void QFEB040FFMControl::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */

    Q_UNUSED(project);
    Q_UNUSED(oldProject);
}

void QFEB040FFMControl::initExtension() {
    /* do initializations here but do not yet connect to the camera! */


    // some example code that may be used to register a menu and a tool button:
    services->log_global_text(tr("initializing extension '%1' ...\n").arg(getName()));
    QAction *actStartPlugin=new QFActionWithNoMenuRole(QIcon(getIconFilename()), tr("Start Extension QFEB040FFMControl"), this);
    QObject::connect(actStartPlugin, SIGNAL(triggered()), this, SLOT(startPlugin()));

    QToolBar* exttb=services->getToolbar("extensions");
    if (exttb) {
        exttb->addAction(actStartPlugin);
    }
    QMenu* extm=services->getMenu("extensions");
    if (extm) {
        extm->addAction(actStartPlugin);
    }
    services->log_global_text(tr("initializing extension '%1' ... DONE\n").arg(getName()));

  //  QObject* obj=services->getExtensionManager()->getQObjectInstance("stage_mc2000");
    //stage=NULL;
    //if (obj) {
      //  stage = (MulticontrolStage*) obj;
        //stage->connectDevice(3);
       // qDebug() << "Call this function: QFEB040FFMControl::initextension"; // debug
        //stage=qobject_cast<MulticontrolStage*>(obj);
    //}
    // FOR TESTING



    //startPlugin();

/*
    if (stage) {
        qDebug()<<"found stage_mc2000 plugin!!!";
    } else {
        qDebug()<<"didn't find' stage_mc2000 plugin!!!    obj="<<obj;
    }*/
}

void QFEB040FFMControl::startPlugin() {
/*                 */
  //  qDebug() << "Call this function: QFEB040FFMControl::startPlugin() 1111";
    if(!mw){
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        mw = new ffmMainWindow(this);
        //mw = new ffmMainWindow(this);
        //if (settings) mw->loadSettings(settings);



        //* HIDE UNTIL
        //mw->show();
        //*/


        QApplication::restoreOverrideCursor();
    //    qDebug() << "Call this function: QFEB040FFMControl::startPlugin() 2222";
    }
    mw->show();
    mw->activateWindow();
    //qDebug() << "Call this function: QFEB040FFMControl::startPlugin() 3333";
}

void QFEB040FFMControl::loadSettings(ProgramOptions* settingspo) {
    /* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings());
    bool ok = false;
    int height = settings.value("/qfeb040ffm/window/height", "800").toInt(&ok);
    int width = settings.value("/qfeb040ffm/window/width", "1000").toInt(&ok);
    int posx = settings.value("/qfeb040ffm/window/posx", "0").toInt(&ok);
    int posy = settings.value("/qfeb040ffm/window/posy", "0").toInt(&ok);
    if (mw) {
        mw->resize(width, height);
        mw->move(posx, posy);
    }

    // the QSettings object for quickfit.ini
    // ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEB040FFMControl::storeSettings(ProgramOptions* settingspo) {
    /* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

    if (mw) {
        int height = mw->height();
        int width = mw->width();
        int posx = mw->pos().x();
        int posy = mw->pos().y();
        settings.setValue("/qfeb040ffm/window/height", QLocale::system().toString(height));
        settings.setValue("/qfeb040ffm/window/width", QLocale::system().toString(width));
        settings.setValue("/qfeb040ffm/window/posx", QLocale::system().toString(posx));
        settings.setValue("/qfeb040ffm/window/posy", QLocale::system().toString(posy));
    }

    // ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFEB040FFMControl::log_text(QString message) {
    if (logService) logService->log_text(LOG_PREFIX+message);
    else if (services) services->log_text(LOG_PREFIX+message);
}

void QFEB040FFMControl::log_warning(QString message) {
    if (logService) logService->log_warning(LOG_PREFIX+message);
    else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFEB040FFMControl::log_error(QString message) {
    if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}


Q_EXPORT_PLUGIN2(b040_ffmcontrol, QFEB040FFMControl)

/*
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    mw = new ffmMainWindow();
    mw->show();
    qDebug() << "startPlugin";
    QApplication::restoreOverrideCursor();
*/
