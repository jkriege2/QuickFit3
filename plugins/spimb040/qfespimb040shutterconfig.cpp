#include "qfespimb040shutterconfig.h"
#include "qfespimb040mainwindow.h"
#include <QtGui>
#include <iostream>










QFESPIMB040ShutterConfig::QFESPIMB040ShutterConfig(QWidget* parent):
    QWidget(parent)
{

    timUpdate=new QTimer(this);
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(shutterStateUpdateInterval);

    m_log=NULL;
    m_pluginServices=NULL;
    locked=false;


    shutterStateUpdateInterval=250;
    iconOpened=QPixmap(":/spimb040/shutter_opened.png");
    iconClosed=QPixmap(":/spimb040/shutter_closed.png");
    createWidgets();
    createActions();
    updateStates();


    connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayShutterStates()));
    timUpdate->start(shutterStateUpdateInterval);
    //QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}


QFESPIMB040ShutterConfig::~QFESPIMB040ShutterConfig()
{
    locked=true;
    disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayShutterStates()));
    timUpdate->stop();
}

void QFESPIMB040ShutterConfig::unlockShutters() {
    locked=false;
    connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayShutterStates()));
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(shutterStateUpdateInterval);
    timUpdate->start(shutterStateUpdateInterval);
}


void QFESPIMB040ShutterConfig::setLog(QFPluginLogService* log) {
    m_log=log;
}

void QFESPIMB040ShutterConfig::init(QFPluginLogService* log, QFPluginServices* pluginServices) {
    m_log=log;
    m_pluginServices=pluginServices;

    if (m_pluginServices) {
        cmbShutter->init(m_pluginServices->getExtensionManager());
    } else {
        //stages.clear();
        cmbShutter->clear();
    }
    updateStates();
}

void QFESPIMB040ShutterConfig::loadSettings(QSettings& settings, QString prefix) {
    cmbShutter->loadSettings(settings, prefix+"shutter/");

    shutterStateUpdateInterval=settings.value(prefix+"update_interval", shutterStateUpdateInterval).toDouble();
}

void QFESPIMB040ShutterConfig::saveSettings(QSettings& settings, QString prefix) {
    cmbShutter->storeSettings(settings, prefix+"shutter/");
    settings.setValue(prefix+"update_interval", shutterStateUpdateInterval);
}

void QFESPIMB040ShutterConfig::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QGridLayout* widgetLayout=new QGridLayout(this);
    setLayout(widgetLayout);
    widgetLayout->setContentsMargins(0,0,0,0);
    widgetLayout->setHorizontalSpacing(2);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    btnState=new QToolButton(this);
    btnState->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    widgetLayout->addWidget(btnState,0,0);
    cmbShutter=new QFShutterComboBox(this);
    widgetLayout->addWidget(cmbShutter,0,1);
    btnConnect=new QToolButton(this);
    widgetLayout->addWidget(btnConnect,0,2);
    btnConfigure=new QToolButton(this);
    widgetLayout->addWidget(btnConfigure,0,3);
    QWidget* w=new QWidget(this);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    widgetLayout->addWidget(w,0,4);
    widgetLayout->setColumnStretch(4,1);
    cmbShutter->setEnabled(false);


}

void QFESPIMB040ShutterConfig::createActions() {
    actConnect=new QAction(QIcon(":/spimb040/connect_shutter.png"), tr("Connect to shutter driver/hardware ..."), this);
    actConnect->setCheckable(true);
    connect(actConnect, SIGNAL(toggled(bool)), this, SLOT(disConnect()));
    btnConnect->setDefaultAction(actConnect);


    actConfigure=new QAction(QIcon(":/spimb040/configure_shutter.png"), tr("Configure shutter ..."), this);
    connect(actConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    btnConfigure->setDefaultAction(actConfigure);


    actState=new QAction(iconClosed, tr("closed"), this);
    connect(actState, SIGNAL(toggled(bool)), this, SLOT(setShutter(bool)));
    btnState->setDefaultAction(actState);

}

void QFESPIMB040ShutterConfig::updateStates() {
    QFExtensionShutter* shutter;
    int shutterID;
    bool conn;


    shutter=getShutter();
    shutterID=getShutterID();
    conn=false;
    if (shutter) {
        conn=shutter->isShutterConnected(shutterID);
        if (conn) {
            actConnect->setChecked(true);
            actConnect->setIcon(QIcon(":/spimb040/disconnect_shutter.png"));
            actConnect->setText(tr("Disconnect from shutter driver/hardware ..."));
        } else {
            actConnect->setChecked(false);
            actConnect->setIcon(QIcon(":/spimb040/connect_shutter.png"));
            actConnect->setText(tr("Connect from shutter driver/hardware ..."));
        }
    }
    actConfigure->setEnabled(shutter!=NULL && shutterID>=0);
    actConnect->setEnabled(shutter!=NULL && shutterID>=0);
    cmbShutter->setEnabled(!conn);
    actState->setEnabled(conn && shutter!=NULL);

}

void QFESPIMB040ShutterConfig::disConnect() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool conn=actConnect->isChecked();
    QFExtensionShutter* shutter=getShutter();
    int shutterID=getShutterID();

    if (shutter) {
        if (conn) {
            shutter->setShutterLogging(m_log);
            shutter->shutterConnect(shutterID);
            if (shutter->isShutterConnected(shutterID)) {
                m_log->log_text("connected to shutter driver ...\n");
            } else {
                actConnect->setChecked(false);
                shutter->shutterDisonnect(shutterID);
                m_log->log_error("error connecting to shutter driver ...\n");
            }
        } else {
            shutter->shutterDisonnect(shutterID);
            m_log->log_text("disconnected from shutter driver ...\n");
        }
    } else {
        actConnect->setChecked(false);
    }
    updateStates();
    QApplication::restoreOverrideCursor();
}





void QFESPIMB040ShutterConfig::configure() {
    QFExtensionShutter* shutter=getShutter();
    int shutterID=getShutterID();
    if (shutter) shutter->showShutterSettingsDialog(shutterID, this);
    updateStates();
}




QFExtensionShutter* QFESPIMB040ShutterConfig::getShutter() {
    return cmbShutter->currentExtensionShutter();
}

QFExtension* QFESPIMB040ShutterConfig::getShutterExtension() {
    return cmbShutter->currentExtension();
}

int QFESPIMB040ShutterConfig::getShutterID() {

    return cmbShutter->currentShutterID();
}

bool QFESPIMB040ShutterConfig::getShutterState() {
    QFExtensionShutter* shutter;
    int shutterID;
    shutter=getShutter();
    shutterID=getShutterID();
    if (shutter) {
        return shutter->isShutterOpen(shutterID);
    }
    return false;
}


void QFESPIMB040ShutterConfig::displayShutterStates(/*bool automatic*/) {
    QFExtensionShutter* shutter;
    int shutterID;
    shutter=getShutter();
    shutterID=getShutterID();
    if (shutter) {
        bool opened=shutter->isShutterOpen(shutterID);
        if (opened) {
            actState->setIcon(iconOpened);
            actState->setText(tr("opened"));
            actState->setChecked(true);
        } else {
            actState->setIcon(iconClosed);
            actState->setText(tr("closed"));
            actState->setChecked(false);
        }
    }
    updateStates();

    QTimer::singleShot(shutterStateUpdateInterval, this, SLOT(displayShutterStates()));
}

void QFESPIMB040ShutterConfig::setShutter(bool opened) {
    actState->setChecked(opened);
}

void QFESPIMB040ShutterConfig::shutterActionClicked(bool opened) {
    QFExtensionShutter* shutter;
    int shutterID;
    shutter=getShutter();
    shutterID=getShutterID();
    if (shutter) {
        shutter->setShutterState(shutterID, opened);
        QTime started=QTime::currentTime();
        while (!shutter->isLastShutterActionFinished(shutterID) && (started.elapsed()<5000)) {
            QApplication::processEvents();
        }
    }
}

void QFESPIMB040ShutterConfig::connectShutter() {
    actConnect->setChecked(true);
}

void QFESPIMB040ShutterConfig::disconnectShutter() {
    actConnect->setChecked(false);
}

void QFESPIMB040ShutterConfig::setReadOnly(bool readonly) {
    cmbShutter->setReadOnly(readonly);
}



bool QFESPIMB040ShutterConfig::isShutterConnected() const {
    return actConnect->isChecked();
}
