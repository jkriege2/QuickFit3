#include "qfespimb040samplestageconfig.h"
#include "qfespimb040mainwindow.h"
#include <QtGui>
#include <iostream>

QFESPIMB040SampleStageConfig::QFESPIMB040SampleStageConfig(QFESPIMB040MainWindow* parent, QFExtensionServices* pluginServices):
        QGroupBox(parent)
{
    setTitle(tr(" Sample Translation Stages: "));
    m_parent=parent;
    m_pluginServices=pluginServices;
    locked=false;
    stageStateUpdateInterval=250;
    iconDisconnected=QPixmap(":/spimb040/stage_disconnected.png");
    iconReady=QPixmap(":/spimb040/stage_ready.png");
    iconError=QPixmap(":/spimb040/stage_error.png");
    iconMoving=QPixmap(":/spimb040/stage_moving.png");
    iconMovingOpposite=QPixmap(":/spimb040/stage_moving2.png");
    iconJoystick=QPixmap(":/spimb040/stage_joystick.png");
    iconNoJoystick=QPixmap(":/spimb040/stage_nojoystick.png");
    timerDisplayUpdate.setInterval(stageStateUpdateInterval);
    connect(&timerDisplayUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
    timerDisplayUpdate.start();

    findStages(m_pluginServices->getExtensionManager());
    createWidgets();
    createActions();
    updateStates();
}

QFESPIMB040SampleStageConfig::~QFESPIMB040SampleStageConfig()
{
    //dtor
}

void QFESPIMB040SampleStageConfig::loadSettings(ProgramOptions* settings, QString prefix) {
    cmbStageX->setCurrentIndex((settings->getQSettings())->value(prefix+"stage_x", 0).toInt());
    cmbStageY->setCurrentIndex((settings->getQSettings())->value(prefix+"stage_y", 1).toInt());
    cmbStageZ->setCurrentIndex((settings->getQSettings())->value(prefix+"stage_z", 2).toInt());
    spinJoystickMaxSpeed->setValue((settings->getQSettings())->value(prefix+"joystick_max_speed", 500).toDouble());
    stageStateUpdateInterval=(settings->getQSettings())->value(prefix+"update_interval", stageStateUpdateInterval).toDouble();
    timerDisplayUpdate.setInterval(stageStateUpdateInterval);
}

void QFESPIMB040SampleStageConfig::storeSettings(ProgramOptions* settings, QString prefix) {
    (settings->getQSettings())->setValue(prefix+"stage_x", cmbStageX->currentIndex());
    (settings->getQSettings())->setValue(prefix+"stage_y", cmbStageY->currentIndex());
    (settings->getQSettings())->setValue(prefix+"stage_z", cmbStageZ->currentIndex());
    (settings->getQSettings())->setValue(prefix+"joystick_max_speed", spinJoystickMaxSpeed->value());
    (settings->getQSettings())->setValue(prefix+"update_interval", stageStateUpdateInterval);
}

void QFESPIMB040SampleStageConfig::findStages(QFExtensionManager* extManager) {
    // load available acquisition devices from extManager
    QStringList ids=extManager->getIDList();
    stages.clear();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=extManager->getQObjectInstance(ids[i]);
        QFExtension* extension=extManager->getInstance(ids[i]);
        QFExtensionLinearStage* stage = qobject_cast<QFExtensionLinearStage*>(extobj);
        if (stage) stages.append(extobj);
    }
}

void QFESPIMB040SampleStageConfig::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QFormLayout* stagelayout=new QFormLayout(this);
    setLayout(stagelayout);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbStageX=new QComboBox(this);
    QHBoxLayout* hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbStageX);
    btnConnectX=new QToolButton(this);
    hbl->addWidget(btnConnectX);
    btnConfigureX=new QToolButton(this);
    hbl->addWidget(btnConfigureX);
    hbl->addStretch();
    stagelayout->addRow(tr("<b>x axis:</b>"), hbl);
    cmbStageX->setEnabled(false);

    cmbStageY=new QComboBox(this);
    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbStageY);
    btnConnectY=new QToolButton(this);
    hbl->addWidget(btnConnectY);
    btnConfigureY=new QToolButton(this);
    hbl->addWidget(btnConfigureY);
    hbl->addStretch();
    stagelayout->addRow(tr("<b>y axis:</b>"), hbl);
    cmbStageY->setEnabled(false);

    cmbStageZ=new QComboBox(this);
    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->addWidget(cmbStageZ);
    btnConnectZ=new QToolButton(this);
    hbl->addWidget(btnConnectZ);
    btnConfigureZ=new QToolButton(this);
    hbl->addWidget(btnConfigureZ);
    hbl->addStretch();
    stagelayout->addRow(tr("<b>z axis:</b>"), hbl);
    cmbStageZ->setEnabled(false);

    // fill axis selection comboboxes with available choices
    for (int i=0; i<stages.size(); i++) {
        QFExtension* extension=qobject_cast<QFExtension*>(stages[i]);
        QFExtensionLinearStage* stage = qobject_cast<QFExtensionLinearStage*>(stages[i]);
        for (unsigned int j=0; j<stage->getAxisCount(); j++) {
            QPoint sl(i, j);
            QString name=extension->getName();
            if (stage->getAxisCount()>0) name=name+QString(" #%1").arg(j);
            cmbStageX->addItem(QIcon(extension->getIconFilename()), name, sl);
            cmbStageY->addItem(QIcon(extension->getIconFilename()), name, sl);
            cmbStageZ->addItem(QIcon(extension->getIconFilename()), name, sl);
        }
    }


    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    chkJoystick=new QCheckBox(tr("enabled"), this);
    connect(chkJoystick, SIGNAL(clicked(bool)), this, SLOT(updateJoystick()));
    hbl->addWidget(chkJoystick);
    hbl->addSpacing(24);
    hbl->addWidget(new QLabel("max. speed [micron/sec]: ", this));
    spinJoystickMaxSpeed=new QDoubleSpinBox(this);
    spinJoystickMaxSpeed->setMinimum(1);
    spinJoystickMaxSpeed->setMaximum(10000);
    spinJoystickMaxSpeed->setSingleStep(10);
    spinJoystickMaxSpeed->setDecimals(0);
    connect(spinJoystickMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(updateJoystick()));
    hbl->addWidget(spinJoystickMaxSpeed);
    hbl->addStretch();
    stagelayout->addRow(tr("<b>joystick:</b>"), hbl);

    QGridLayout* gl=new QGridLayout(this);
    gl->addWidget(new QLabel("<b>x [&mu;m]:</b>", this), 0, 0);
    gl->addWidget(new QLabel("<b>y [&mu;m]:</b>", this), 0, 1);
    gl->addWidget(new QLabel("<b>z [&mu;m]:</b>", this), 0, 2);
    gl->addWidget(new QLabel("<b>move ...</b>", this), 0, 3);
    spinMoveX=new QDoubleSpinBox(this);
    spinMoveX->setRange(-1e6,1e6);
    spinMoveX->setSingleStep(1);
    gl->addWidget(spinMoveX, 1,0);
    spinMoveY=new QDoubleSpinBox(this);
    spinMoveY->setRange(-1e6,1e6);
    spinMoveY->setSingleStep(1);
    gl->addWidget(spinMoveY, 1,1);
    spinMoveZ=new QDoubleSpinBox(this);
    spinMoveZ->setRange(-1e6,1e6);
    spinMoveZ->setSingleStep(1);
    gl->addWidget(spinMoveZ, 1,2);
    btnMoveAbsolute=new QPushButton(QIcon(":/spimb040/stagemove.png"), tr("... &absolute"), this);
    gl->addWidget(btnMoveAbsolute, 1, 3);
    btnMoveRelative=new QPushButton(QIcon(":/spimb040/stagemove.png"), tr("... &relative"), this);
    gl->addWidget(btnMoveRelative, 1, 4);
    gl->addWidget(new QWidget(this), 1, 5);
    gl->setColumnStretch(5,10);
    connect(btnMoveAbsolute, SIGNAL(clicked()), this, SLOT(moveAbsolute()));
    connect(btnMoveRelative, SIGNAL(clicked()), this, SLOT(moveRelative()));
    stagelayout->addRow(tr("<b>movement:</b>"), gl);


    gl=new QGridLayout(this);
    QLabel* l;
    l=new QLabel(tr("<b><i>x</i>-axis</b>"));
    l->setStyleSheet("background-color: darkgrey;");
    l->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    l->setAutoFillBackground(true);
    gl->addWidget(l, 0, 1);
    l=new QLabel(tr("<b><i>y</i>-axis</b>"));
    l->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    l->setStyleSheet("background-color: darkgrey;");
    l->setAutoFillBackground(true);
    gl->addWidget(l, 0, 2);
    l=new QLabel(tr("<b><i>z</i>-axis</b>"));
    l->setAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    l->setStyleSheet("background-color: darkgrey;");
    l->setAutoFillBackground(true);
    gl->addWidget(l, 0, 3);
    l=new QLabel(tr("<b>status:</b>"));
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setStyleSheet("background-color: darkgrey;");
    l->setAutoFillBackground(true);
    gl->addWidget(l, 1, 0);
    l=new QLabel(tr("<b>speed [&mu;m/s]:</b>"));
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setStyleSheet("background-color: darkgrey;");
    l->setAutoFillBackground(true);
    gl->addWidget(l, 2, 0);
    l=new QLabel(tr("<b>position [&mu;m]:</b>"));
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setStyleSheet("background-color: darkgrey;");
    l->setAutoFillBackground(true);
    gl->addWidget(l, 3, 0);
    labXPosition=new QLabel(this);
    labXPosition->setStyleSheet("background-color: 	lightgrey;");;
    labXPosition->setAutoFillBackground(true);
    labXPosition->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labXSpeed=new QLabel(this);
    labXSpeed->setStyleSheet("background-color: 	lightgrey;");;
    labXSpeed->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labXSpeed->setAutoFillBackground(true);
    labXState=new QLabel(this);
    labXState->setStyleSheet("background-color: 	lightgrey;");;
    labXState->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labXState->setAutoFillBackground(true);
    labYPosition=new QLabel(this);
    labYPosition->setStyleSheet("background-color: 	lightgrey;");;
    labYPosition->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labYPosition->setAutoFillBackground(true);
    labYSpeed=new QLabel(this);
    labYSpeed->setStyleSheet("background-color: 	lightgrey;");;
    labYSpeed->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labYSpeed->setAutoFillBackground(true);
    labYState=new QLabel(this);
    labYState->setStyleSheet("background-color: 	lightgrey;");;
    labYState->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labYState->setAutoFillBackground(true);
    labZPosition=new QLabel(this);
    labZPosition->setStyleSheet("background-color: 	lightgrey;");;
    labZPosition->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labZPosition->setAutoFillBackground(true);
    labZSpeed=new QLabel(this);
    labZSpeed->setStyleSheet("background-color: 	lightgrey;");;
    labZSpeed->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labZSpeed->setAutoFillBackground(true);
    labZState=new QLabel(this);
    labZState->setStyleSheet("background-color: 	lightgrey;");;
    labZState->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labZState->setAutoFillBackground(true);

    labJoystick=new QLabel(this);
    labJoystick->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    gl->addWidget(labJoystick, 0,0);

    gl->addWidget(labXState, 1,1);
    gl->addWidget(labYState, 1,2);
    gl->addWidget(labZState, 1,3);
    gl->addWidget(labXSpeed, 2,1);
    gl->addWidget(labYSpeed, 2,2);
    gl->addWidget(labZSpeed, 2,3);
    gl->addWidget(labXPosition, 3,1);
    gl->addWidget(labYPosition, 3,2);
    gl->addWidget(labZPosition, 3,3);
    gl->addWidget(new QWidget(this), 4,4);
    gl->setColumnStretch(4,10);
    gl->setRowStretch(4,10);
    stagelayout->addRow(tr("<b>status:</b>"), gl);

}

void QFESPIMB040SampleStageConfig::createActions() {
    std::cout<<">>>> createActions()\n";
    actConnectX=new QAction(QIcon(":/spimb040/stageconnect.png"), tr("Connect x-axis ..."), this);
    actConnectX->setCheckable(true);
    connect(actConnectX, SIGNAL(triggered()), this, SLOT(disConnectX()));
    btnConnectX->setDefaultAction(actConnectX);

    actConnectY=new QAction(QIcon(":/spimb040/stageconnect.png"), tr("Connect y-axis ..."), this);
    actConnectY->setCheckable(true);
    connect(actConnectY, SIGNAL(triggered()), this, SLOT(disConnectY()));
    btnConnectY->setDefaultAction(actConnectY);

    actConnectZ=new QAction(QIcon(":/spimb040/stageconnect.png"), tr("Connect z-axis ..."), this);
    actConnectZ->setCheckable(true);
    connect(actConnectZ, SIGNAL(triggered()), this, SLOT(disConnectZ()));
    btnConnectZ->setDefaultAction(actConnectZ);



    actConfigureX=new QAction(QIcon(":/spimb040/stagesettings.png"), tr("Configure x-axis ..."), this);
    connect(actConfigureX, SIGNAL(triggered()), this, SLOT(configureX()));
    btnConfigureX->setDefaultAction(actConfigureX);

    actConfigureY=new QAction(QIcon(":/spimb040/stagesettings.png"), tr("Configure y-axis ..."), this);
    connect(actConfigureY, SIGNAL(triggered()), this, SLOT(configureY()));
    btnConfigureY->setDefaultAction(actConfigureY);

    actConfigureZ=new QAction(QIcon(":/spimb040/stagesettings.png"), tr("Configure z-axis ..."), this);
    connect(actConfigureZ, SIGNAL(triggered()), this, SLOT(configureZ()));
    btnConfigureZ->setDefaultAction(actConfigureZ);


}

void QFESPIMB040SampleStageConfig::updateStates() {
    QFExtensionLinearStage* stage;
    int axis;
    bool conn;

    bool anyconn=false;
    bool anyjoy=false;

    stage=getXStage();
    axis=getXStageAxis();
    conn=false;
    if (stage) {
        conn=stage->isConnected(axis);
        anyconn=anyconn||conn;
        anyjoy=anyjoy||stage->isJoystickActive(axis);
        if (conn) {
            actConnectX->setChecked(true);
            actConnectX->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            actConnectX->setText(tr("Disconnect x-axis ..."));
        } else {
            actConnectX->setChecked(false);
            actConnectX->setIcon(QIcon(":/spimb040/stageconnect.png"));
            actConnectX->setText(tr("Connect x-axis ..."));
        }
    }
    actConfigureX->setEnabled(true);
    cmbStageX->setEnabled(!conn);
    spinMoveX->setEnabled(conn);


    stage=getYStage();
    axis=getYStageAxis();
    conn=false;
    if (stage) {
        conn=stage->isConnected(axis);
        anyconn=anyconn||conn;
        anyjoy=anyjoy||stage->isJoystickActive(axis);
        if (conn) {
            actConnectY->setChecked(true);
            actConnectY->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            actConnectY->setText(tr("Disconnect y-axis ..."));
        } else {
            actConnectY->setChecked(false);
            actConnectY->setIcon(QIcon(":/spimb040/stageconnect.png"));
            actConnectY->setText(tr("Connect y-axis ..."));
        }
    }
    actConfigureY->setEnabled(true);
    cmbStageY->setEnabled(!conn);
    spinMoveY->setEnabled(conn);


    stage=getZStage();
    axis=getZStageAxis();
    conn=false;
    if (stage) {
        conn=stage->isConnected(axis);
        anyconn=anyconn||conn;
        anyjoy=anyjoy||stage->isJoystickActive(axis);
        if (conn) {
            actConnectZ->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            actConnectZ->setChecked(true);
            actConnectZ->setText(tr("Disconnect z-axis ..."));
        } else {
            actConnectZ->setChecked(false);
            actConnectZ->setIcon(QIcon(":/spimb040/stageconnect.png"));
            actConnectZ->setText(tr("Connect z-axis ..."));
        }
    }
    actConfigureZ->setEnabled(true);
    cmbStageZ->setEnabled(!conn);
    spinMoveZ->setEnabled(conn);

    btnMoveAbsolute->setEnabled(anyconn);
    btnMoveRelative->setEnabled(anyconn);

    chkJoystick->setEnabled(anyconn);
    spinJoystickMaxSpeed->setEnabled(anyconn);
    if (anyconn) {
        if (anyjoy) labJoystick->setPixmap(iconJoystick);
        else labJoystick->setPixmap(iconNoJoystick);
    } else {
        labJoystick->setPixmap(QPixmap());
    }

    if (!anyconn) locked=false;
}

void QFESPIMB040SampleStageConfig::disConnectX() {
    bool conn=actConnectX->isChecked();
    QFExtensionLinearStage* stage=getXStage();
    int axis=getXStageAxis();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    chkJoystick->setChecked(false);
    if (stage) {
        if (conn) {
            stage->setLogging(m_parent);
            stage->connectDevice(axis);
            if (stage->isConnected(axis) && (stage->getAxisState(axis)==QFExtensionLinearStage::Ready)) {
                m_parent->log_text("connected to x-axis stage driver ...");
            } else {
                actConnectX->setChecked(false);
                stage->disconnectDevice(axis);
                m_parent->log_error("error connecting to x-axis stage driver ...");
            }
        } else {
            stage->disconnectDevice(axis);
            m_parent->log_text("disconnected from x-axis stage driver ...");
        }
    } else {
        actConnectX->setChecked(false);
    }
    updateStates();
    QApplication::restoreOverrideCursor();
}


void QFESPIMB040SampleStageConfig::disConnectY() {
    bool conn=actConnectY->isChecked();
    QFExtensionLinearStage* stage=getYStage();
    int axis=getYStageAxis();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    chkJoystick->setChecked(false);
    if (stage) {
        if (conn) {
            stage->setLogging(m_parent);
            stage->connectDevice(axis);
            if (stage->isConnected(axis) && (stage->getAxisState(axis)==QFExtensionLinearStage::Ready)) {
                m_parent->log_text("connected to y-axis stage driver ...");
            } else {
                actConnectY->setChecked(false);
                stage->disconnectDevice(axis);
                m_parent->log_error("error connecting to y-axis stage driver ...");
            }
        } else {
            stage->disconnectDevice(axis);
            m_parent->log_text("disconnected from y-axis stage driver ...");
        }
    } else {
        actConnectY->setChecked(false);
    }
    updateStates();
    QApplication::restoreOverrideCursor();
}


void QFESPIMB040SampleStageConfig::disConnectZ() {
    bool conn=actConnectZ->isChecked();
    QFExtensionLinearStage* stage=getZStage();
    int axis=getZStageAxis();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (stage) {
        if (conn) {
            stage->setLogging(m_parent);
            stage->connectDevice(axis);
            if (stage->isConnected(axis) && (stage->getAxisState(axis)==QFExtensionLinearStage::Ready)) {
                m_parent->log_text("connected to z-axis stage driver ...");
            } else {
                actConnectZ->setChecked(false);
                stage->disconnectDevice(axis);
                m_parent->log_error("error connecting to z-axis stage driver ...");
            }
        } else {
            stage->disconnectDevice(axis);
            m_parent->log_text("disconnected from z-axis stage driver ...");
        }
    } else {
        actConnectZ->setChecked(false);
    }
    updateStates();
    QApplication::restoreOverrideCursor();
}



void QFESPIMB040SampleStageConfig::configureX() {
    QFExtensionLinearStage* stage=getXStage();
    int axis=getXStageAxis();
    if (stage) stage->showSettingsDialog(axis, this);
    updateStates();
}

void QFESPIMB040SampleStageConfig::configureY() {
    QFExtensionLinearStage* stage=getYStage();
    int axis=getYStageAxis();
    if (stage) stage->showSettingsDialog(axis, this);
    updateStates();
}

void QFESPIMB040SampleStageConfig::configureZ() {
    QFExtensionLinearStage* stage=getZStage();
    int axis=getZStageAxis();
    if (stage) stage->showSettingsDialog(axis, this);
    updateStates();
}




QFExtensionLinearStage* QFESPIMB040SampleStageConfig::getXStage() {
    if (cmbStageX->currentIndex()<0) return NULL;
    QPoint p=cmbStageX->itemData(cmbStageX->currentIndex()).toPoint();
    return qobject_cast<QFExtensionLinearStage*>(stages[p.x()]);
}

QFExtension* QFESPIMB040SampleStageConfig::getXStageExtension() {
    if (cmbStageX->currentIndex()<0) return NULL;
    QPoint p=cmbStageX->itemData(cmbStageX->currentIndex()).toPoint();
    return qobject_cast<QFExtension*>(stages[p.x()]);
}

int QFESPIMB040SampleStageConfig::getXStageAxis() {
    if (cmbStageX->currentIndex()<0) return -1;
    QPoint p=cmbStageX->itemData(cmbStageX->currentIndex()).toPoint();
    return p.y();
}



QFExtensionLinearStage* QFESPIMB040SampleStageConfig::getYStage() {
    if (cmbStageY->currentIndex()<0) return NULL;
    QPoint p=cmbStageY->itemData(cmbStageY->currentIndex()).toPoint();
    return qobject_cast<QFExtensionLinearStage*>(stages[p.x()]);
}

QFExtension* QFESPIMB040SampleStageConfig::getYStageExtension() {
    if (cmbStageY->currentIndex()<0) return NULL;
    QPoint p=cmbStageY->itemData(cmbStageY->currentIndex()).toPoint();
    return qobject_cast<QFExtension*>(stages[p.x()]);
}


int QFESPIMB040SampleStageConfig::getYStageAxis() {
    if (cmbStageY->currentIndex()<0) return -1;
    QPoint p=cmbStageY->itemData(cmbStageY->currentIndex()).toPoint();
    return p.y();
}



QFExtensionLinearStage* QFESPIMB040SampleStageConfig::getZStage() {
    if (cmbStageZ->currentIndex()<0) return NULL;
    QPoint p=cmbStageZ->itemData(cmbStageZ->currentIndex()).toPoint();
    return qobject_cast<QFExtensionLinearStage*>(stages[p.x()]);
}

QFExtension* QFESPIMB040SampleStageConfig::getZStageExtension() {
    if (cmbStageZ->currentIndex()<0) return NULL;
    QPoint p=cmbStageZ->itemData(cmbStageZ->currentIndex()).toPoint();
    return qobject_cast<QFExtension*>(stages[p.x()]);
}

int QFESPIMB040SampleStageConfig::getZStageAxis() {
    if (cmbStageZ->currentIndex()<0) return -1;
    QPoint p=cmbStageZ->itemData(cmbStageZ->currentIndex()).toPoint();
    return p.y();
}

void QFESPIMB040SampleStageConfig::updateJoystick() {
    double speed=spinJoystickMaxSpeed->value();
    bool enabled=chkJoystick->isChecked();
    spinJoystickMaxSpeed->setEnabled(enabled);
    QFExtensionLinearStage* stage;
    int axis;

    stage=getXStage();
    axis=getXStageAxis();
    if (stage) {
        if (stage->isConnected(axis)) {
            stage->setJoystickActive(axis, enabled, speed);
        }
    }
    stage=getYStage();
    axis=getYStageAxis();
    if (stage) {
        if (stage->isConnected(axis)) {
            stage->setJoystickActive(axis, enabled, speed);
        }
    }
    stage=getZStage();
    axis=getZStageAxis();
    if (stage) {
        if (stage->isConnected(axis)) {
            stage->setJoystickActive(axis, enabled, speed);
        }
    }

}

void QFESPIMB040SampleStageConfig::displayAxisStates(/*bool automatic*/) {
    bool anyconn=false;

        QFExtensionLinearStage* stage;
        int axis;
        QLabel* labPos;
        QLabel* labSpeed;
        QLabel* labState;

        stage=getXStage();
        axis=getXStageAxis();
        labPos=labXPosition;
        labState=labXState;
        labSpeed=labXSpeed;
        if (stage) {
            anyconn=anyconn||stage->isConnected(axis);
            QFExtensionLinearStage::AxisState state=stage->getAxisState(axis);
            double position=stage->getPosition(axis);
            double speed=stage->getSpeed(axis);
            labState->setText("");
            switch(state) {
                case QFExtensionLinearStage::Ready : labState->setPixmap(iconReady); break;
                case QFExtensionLinearStage::Disconnected : labState->setPixmap(iconDisconnected); break;
                case QFExtensionLinearStage::Moving : {
		  if (speed>0) labState->setPixmap(iconMoving); 
		  else labState->setPixmap(iconMovingOpposite); 
		} break;
                case QFExtensionLinearStage::Error : labState->setPixmap(iconError); break;
                default: labState->setText(tr("?")); break;
            }
            labSpeed->setText(QString::number(speed, 'f', 2));
            labPos->setText(QString::number(position, 'f', 2));
        }

        stage=getYStage();
        axis=getYStageAxis();
        labPos=labYPosition;
        labState=labYState;
        labSpeed=labYSpeed;
        if (stage) {
            anyconn=anyconn||stage->isConnected(axis);
            QFExtensionLinearStage::AxisState state=stage->getAxisState(axis);
            double position=stage->getPosition(axis);
            double speed=stage->getSpeed(axis);
            labState->setText("");
            switch(state) {
                case QFExtensionLinearStage::Ready : labState->setPixmap(iconReady); break;
                case QFExtensionLinearStage::Disconnected : labState->setPixmap(iconDisconnected); break;
                case QFExtensionLinearStage::Moving : {
		  if (speed>0) labState->setPixmap(iconMoving); 
		  else labState->setPixmap(iconMovingOpposite); 
		} break;
                case QFExtensionLinearStage::Error : labState->setPixmap(iconError); break;
                default: labState->setText(tr("?")); break;
            }
            labSpeed->setText(QString::number(speed, 'f', 2));
            labPos->setText(QString::number(position, 'f', 2));
        }

        stage=getZStage();
        axis=getZStageAxis();
        labPos=labZPosition;
        labState=labZState;
        labSpeed=labZSpeed;
        if (stage) {
            anyconn=anyconn||stage->isConnected(axis);
            QFExtensionLinearStage::AxisState state=stage->getAxisState(axis);
            double position=stage->getPosition(axis);
            double speed=stage->getSpeed(axis);
            switch(state) {
                case QFExtensionLinearStage::Ready : labState->setPixmap(iconReady); break;
                case QFExtensionLinearStage::Disconnected : labState->setPixmap(iconDisconnected); break;
                case QFExtensionLinearStage::Moving : {
		  if (speed>0) labState->setPixmap(iconMoving); 
		  else labState->setPixmap(iconMovingOpposite); 
		} break;
                case QFExtensionLinearStage::Error : labState->setPixmap(iconError); break;
                default: labState->setText(tr("?")); break;
            }
            labSpeed->setText(QString::number(speed, 'f', 2));
            labPos->setText(QString::number(position, 'f', 2));
        }

    /*if ((!locked) && anyconn && automatic) {
        QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
    }*/
}

void QFESPIMB040SampleStageConfig::moveAbsolute() {
    double x=spinMoveX->value();
    double y=spinMoveY->value();
    double z=spinMoveZ->value();

    QFExtensionLinearStage* stage;
    int axis;

    stage=getXStage();
    axis=getXStageAxis();
    if (stage) {
        if (stage->isConnected(axis)) {
            stage->move(axis, x);
        }
    }
    stage=getYStage();
    axis=getYStageAxis();
    if (stage) {
        if (stage->isConnected(axis)) {
            stage->move(axis, y);
        }
    }
    stage=getZStage();
    axis=getZStageAxis();
    if (stage) {
        if (stage->isConnected(axis)) {
            stage->move(axis, z);
        }
    }

}

void QFESPIMB040SampleStageConfig::moveRelative() {
    double x=spinMoveX->value();
    double y=spinMoveY->value();
    double z=spinMoveZ->value();

    QFExtensionLinearStage* stage;
    int axis;

    stage=getXStage();
    axis=getXStageAxis();
    if (stage) {
        if (stage->isConnected(axis) && (x!=0)) {
            stage->move(axis, stage->getPosition(axis)+x);
        }
    }
    stage=getYStage();
    axis=getYStageAxis();
    if (stage) {
        if (stage->isConnected(axis) && (y!=0)) {
            stage->move(axis, stage->getPosition(axis)+y);
        }
    }
    stage=getZStage();
    axis=getZStageAxis();
    if (stage) {
        if (stage->isConnected(axis) && (z!=0)) {
            stage->move(axis, stage->getPosition(axis)+z);
        }
    }
}
