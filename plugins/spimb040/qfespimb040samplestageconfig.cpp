#include "qfespimb040samplestageconfig.h"
#include "qfespimb040mainwindow.h"
#include <QtGui>
#include <iostream>

#define THREAD_TIMEOUT 5


QFESPIMB040SampleStageConfigThread::QFESPIMB040SampleStageConfigThread(QFESPIMB040SampleStageConfig* parent):
    QThread(parent)
{

    m_parent=parent;
    qRegisterMetaType<QFExtensionLinearStage::AxisState>("QFExtensionLinearStage::AxisState");
    stopped=true;
    readCounter=0;
    connX=false;
    connY=false;
    connZ=false;
    InstructionMutex=new QMutex();
}

QFESPIMB040SampleStageConfigThread::~QFESPIMB040SampleStageConfigThread() {
    stopThread();
    delete InstructionMutex;
}

void QFESPIMB040SampleStageConfigThread::run() {
    stopped=false;
    /*readCounter=0;
    connX=false;
    connY=false;
    connZ=false;*/
    /*QTimer::singleShot(THREAD_TIMEOUT, this, SLOT(nextInstruction()));
    exec();*/
    while (!stopped) {
        nextInstruction();
        msleep(50);
    }
}

bool QFESPIMB040SampleStageConfigThread::anyConnected() const {
    return connX||connY||connZ;
}

void QFESPIMB040SampleStageConfigThread::start(Priority priority ) {
    QThread::start(priority);
    qDebug()<<"started";
}

void QFESPIMB040SampleStageConfigThread::nextInstruction() {
    //qDebug()<<"QFESPIMB040SampleStageConfigThread::nextInstruction()   stopped="<<stopped<<"  readCounter="<<readCounter;
    if (stopped) {
       /* qDebug()<<"   quit";
        quit();*/
    } else {
        if (m_parent->getXStage()) connX=m_parent->getXStage()->isConnected(m_parent->getXStageAxis()); else connX=false;
        if (m_parent->getYStage()) connX=m_parent->getYStage()->isConnected(m_parent->getYStageAxis()); else connY=false;
        if (m_parent->getZStage()) connX=m_parent->getZStage()->isConnected(m_parent->getZStageAxis()); else connZ=false;
        InstructionMutex->lock();
        if (instructions.isEmpty()) {
            InstructionMutex->unlock();
            if (readCounter<=0) {
                /*connX=false;
                connY=false;
                connZ=false;*/
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) connX=stage->isConnected(axis);
                else connX=false;

                stage=m_parent->getYStage();
                axis=m_parent->getYStageAxis();
                if (stage) connY=stage->isConnected(axis);
                else connY=false;

                stage=m_parent->getZStage();
                axis=m_parent->getZStageAxis();
                if (stage) connZ=stage->isConnected(axis);
                else connZ=false;

                emit stagesConnectedChanged(connX,  connY,  connZ);
            } else if (readCounter==1) {
                /////////////////////////////////////////////////////////////////////////////
                // read stage x state
                /////////////////////////////////////////////////////////////////////////////
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) {
                    QFExtensionLinearStage::AxisState state=stage->getAxisState(axis);
                    double position=stage->getPosition(axis);
                    double speed=stage->getSpeed(axis);
                    emit stageXMoved(state, position, speed);
                }
            } else if (readCounter==2) {
                /////////////////////////////////////////////////////////////////////////////
                // read stage y state
                /////////////////////////////////////////////////////////////////////////////
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getYStage();
                axis=m_parent->getYStageAxis();
                if (stage) {
                    QFExtensionLinearStage::AxisState state=stage->getAxisState(axis);
                    double position=stage->getPosition(axis);
                    double speed=stage->getSpeed(axis);
                    emit stageYMoved(state, position, speed);
                }
            } else if (readCounter==3) {
                /////////////////////////////////////////////////////////////////////////////
                // read stage z state
                /////////////////////////////////////////////////////////////////////////////
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getZStage();
                axis=m_parent->getZStageAxis();
                if (stage) {
                    QFExtensionLinearStage::AxisState state=stage->getAxisState(axis);
                    double position=stage->getPosition(axis);
                    double speed=stage->getSpeed(axis);
                    emit stageZMoved(state, position, speed);
                }

            } else if (readCounter==4) {
                /////////////////////////////////////////////////////////////////////////////
                // read joystick
                /////////////////////////////////////////////////////////////////////////////
                bool joystick=false;
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) joystick=joystick|stage->isJoystickActive(axis);
                stage=m_parent->getYStage();
                axis=m_parent->getYStageAxis();
                if (stage) joystick=joystick|stage->isJoystickActive(axis);
                stage=m_parent->getZStage();
                axis=m_parent->getZStageAxis();
                if (stage) joystick=joystick|stage->isJoystickActive(axis);

                emit joystickStateChanged(joystick);
            } else {
                readCounter=-1;
            }
            readCounter++;
        } else {
            Instruction inst=instructions.dequeue();
            InstructionMutex->unlock();
            if (inst.type==QFESPIMB040SampleStageConfigThread::Move) {
                /////////////////////////////////////////////////////////////////////////////
                // move
                /////////////////////////////////////////////////////////////////////////////
                double x=inst.pd1;
                double y=inst.pd2;
                double z=inst.pd3;

                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, x);
                    }
                }
                stage=m_parent->getYStage();
                axis=m_parent->getYStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, y);
                    }
                }
                stage=m_parent->getZStage();
                axis=m_parent->getZStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, z);
                    }
                }
            } else if (inst.type==QFESPIMB040SampleStageConfigThread::MoveRel) {
                /////////////////////////////////////////////////////////////////////////////
                // move relative
                /////////////////////////////////////////////////////////////////////////////
                double x=inst.pd1;
                double y=inst.pd2;
                double z=inst.pd3;

                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, stage->getPosition(axis)+x);
                    }
                }
                stage=m_parent->getYStage();
                axis=m_parent->getYStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, stage->getPosition(axis)+y);
                    }
                }
                stage=m_parent->getZStage();
                axis=m_parent->getZStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, stage->getPosition(axis)+z);
                    }
                }
            } else if (inst.type==QFESPIMB040SampleStageConfigThread::SetJoystick) {
                /////////////////////////////////////////////////////////////////////////////
                // set the joystick state
                /////////////////////////////////////////////////////////////////////////////
                double speed=inst.pd1;
                bool enabled=inst.pb1;
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->setJoystickActive(axis, enabled, speed);
                    }
                }
                stage=m_parent->getYStage();
                axis=m_parent->getYStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->setJoystickActive(axis, enabled, speed);
                    }
                }
                stage=m_parent->getZStage();
                axis=m_parent->getZStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->setJoystickActive(axis, enabled, speed);
                    }
                }
            }
        }
        /*qDebug()<<"   restart timer";
        QTimer::singleShot(THREAD_TIMEOUT, this, SLOT(nextInstruction()));*/
    }

}

void QFESPIMB040SampleStageConfigThread::move(double x, double y, double z) {
    QFESPIMB040SampleStageConfigThread::Instruction inst;
    inst.type=QFESPIMB040SampleStageConfigThread::Move;
    inst.pd1=x;
    inst.pd2=y;
    inst.pd3=z;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}


void QFESPIMB040SampleStageConfigThread::moveRel(double x, double y, double z) {
    QFESPIMB040SampleStageConfigThread::Instruction inst;
    inst.type=QFESPIMB040SampleStageConfigThread::MoveRel;
    inst.pd1=x;
    inst.pd2=y;
    inst.pd3=z;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}

void QFESPIMB040SampleStageConfigThread::setJoystick(bool enabled, double maxSpeed) {
    QFESPIMB040SampleStageConfigThread::Instruction inst;
    inst.type=QFESPIMB040SampleStageConfigThread::SetJoystick;
    inst.pd1=maxSpeed;
    inst.pb1=enabled;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}

void QFESPIMB040SampleStageConfigThread::stopThread() {
    stopped=true;
    if (isRunning()) wait();
    qDebug()<<"thread stopped";
}

















QFESPIMB040SampleStageConfig::QFESPIMB040SampleStageConfig(QWidget* parent, bool useThread):
    QGroupBox(parent)
{
    setTitle(tr(" Sample Translation Stages: "));

    timUpdate=new QTimer(this);
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(stageStateUpdateInterval);

    m_log=NULL;
    m_pluginServices=NULL;
    locked=false;

    this->useThread=useThread;
    stageThread=new QFESPIMB040SampleStageConfigThread(this);
    connect(stageThread, SIGNAL(joystickStateChanged(bool)), this, SLOT(joystickStateChanged(bool)));
    connect(stageThread, SIGNAL(stagesConnectedChanged(bool,bool,bool)), this, SLOT(stagesConnectedChanged(bool,bool,bool)));
    connect(stageThread, SIGNAL(stageXMoved(QFExtensionLinearStage::AxisState,double,double)), this, SLOT(stageXMoved(QFExtensionLinearStage::AxisState,double,double)));
    connect(stageThread, SIGNAL(stageYMoved(QFExtensionLinearStage::AxisState,double,double)), this, SLOT(stageYMoved(QFExtensionLinearStage::AxisState,double,double)));
    connect(stageThread, SIGNAL(stageZMoved(QFExtensionLinearStage::AxisState,double,double)), this, SLOT(stageZMoved(QFExtensionLinearStage::AxisState,double,double)));
    connect(stageThread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(stageThread, SIGNAL(finished()), this, SLOT(threadFinished()));

    stageStateUpdateInterval=250;
    iconDisconnected=QPixmap(":/spimb040/stage_disconnected.png");
    iconReady=QPixmap(":/spimb040/stage_ready.png");
    iconError=QPixmap(":/spimb040/stage_error.png");
    iconMoving=QPixmap(":/spimb040/stage_moving.png");
    iconMovingOpposite=QPixmap(":/spimb040/stage_moving2.png");
    iconJoystick=QPixmap(":/spimb040/stage_joystick.png");
    iconNoJoystick=QPixmap(":/spimb040/stage_nojoystick.png");
    //timerDisplayUpdate.setInterval(stageStateUpdateInterval);
    //connect(&timerDisplayUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
    //timerDisplayUpdate.start();

    //findStages(m_pluginServices->getExtensionManager());
    createWidgets();
    createActions();
    updateStates();

    if (useThread) {
        stageThread->start();
    } else {
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
        timUpdate->start(stageStateUpdateInterval);
    }
    //QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}


QFESPIMB040SampleStageConfig::~QFESPIMB040SampleStageConfig()
{
}

void QFESPIMB040SampleStageConfig::lockStages() {
    if (useThread) {
        locked=true;
        stageThread->stopThread();
        disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
        timUpdate->stop();
    } else {
        locked=true;
        disconnect(timUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
        timUpdate->stop();
    }
}

void QFESPIMB040SampleStageConfig::unlockStages() {
    if (useThread) {
        locked=false;
        stageThread->start();
    } else {
        locked=false;
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(stageStateUpdateInterval);
        timUpdate->start(stageStateUpdateInterval);
    }
}


void QFESPIMB040SampleStageConfig::setLog(QFPluginLogService* log) {
    m_log=log;
}

void QFESPIMB040SampleStageConfig::init(QFPluginLogService* log, QFPluginServices* pluginServices) {
    m_log=log;
    m_pluginServices=pluginServices;

    if (m_pluginServices) {
        /*findStages(m_pluginServices->getExtensionManager());
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
        }*/
        cmbStageX->init(m_pluginServices->getExtensionManager());
        cmbStageY->init(m_pluginServices->getExtensionManager());
        cmbStageZ->init(m_pluginServices->getExtensionManager());
    } else {
        //stages.clear();
        cmbStageX->clear();
        cmbStageY->clear();
        cmbStageZ->clear();
    }
    updateStates();
}

void QFESPIMB040SampleStageConfig::loadSettings(QSettings& settings, QString prefix) {
    /*cmbStageX->setCurrentIndex(settings.value(prefix+"stage_x", 0).toInt());
    cmbStageY->setCurrentIndex(settings.value(prefix+"stage_y", 1).toInt());
    cmbStageZ->setCurrentIndex(settings.value(prefix+"stage_z", 2).toInt());*/
    cmbStageX->loadSettings(settings, prefix+"stage_x/");
    cmbStageY->loadSettings(settings, prefix+"stage_y/");
    cmbStageZ->loadSettings(settings, prefix+"stage_z/");

    spinJoystickMaxSpeed->setValue(settings.value(prefix+"joystick_max_speed", 500).toDouble());
    stageStateUpdateInterval=settings.value(prefix+"update_interval", stageStateUpdateInterval).toDouble();
    chkJoystick->setChecked(settings.value(prefix+"joystick_enabled", true).toBool());
    //timerDisplayUpdate.setInterval(stageStateUpdateInterval);
}

void QFESPIMB040SampleStageConfig::storeSettings(QSettings& settings, QString prefix) {
    /*settings.setValue(prefix+"stage_x", cmbStageX->currentIndex());
    settings.setValue(prefix+"stage_y", cmbStageY->currentIndex());
    settings.setValue(prefix+"stage_z", cmbStageZ->currentIndex());*/
    cmbStageX->storeSettings(settings, prefix+"stage_x/");
    cmbStageY->storeSettings(settings, prefix+"stage_y/");
    cmbStageZ->storeSettings(settings, prefix+"stage_z/");
    settings.setValue(prefix+"joystick_max_speed", spinJoystickMaxSpeed->value());
    settings.setValue(prefix+"joystick_enabled", chkJoystick->isChecked());
    settings.setValue(prefix+"update_interval", stageStateUpdateInterval);
}

/*
void QFESPIMB040SampleStageConfig::findStages(QFExtensionManager* extManager) {
    // load available acquisition devices from extManager
    QStringList ids=extManager->getIDList();
    stages.clear();
    for (int i=0; i<ids.size(); i++) {
        QObject* extobj=extManager->getQObjectInstance(ids[i]);
        //QFExtension* extension=extManager->getInstance(ids[i]);
        QFExtensionLinearStage* stage = qobject_cast<QFExtensionLinearStage*>(extobj);
        if (stage) stages.append(extobj);
    }
}*/

void QFESPIMB040SampleStageConfig::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QFormLayout* stagelayout=new QFormLayout(this);
    setLayout(stagelayout);



    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    cmbStageX=new QFStageComboBox(this);
    QHBoxLayout* hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(1);
    hbl->addWidget(cmbStageX);
    btnConnectX=new QToolButton(this);
    hbl->addWidget(btnConnectX);
    btnConfigureX=new QToolButton(this);
    hbl->addWidget(btnConfigureX);
    hbl->addStretch();
    stagelayout->addRow(tr("<b>x axis:</b>"), hbl);
    cmbStageX->setEnabled(false);

    cmbStageY=new QFStageComboBox(this);
    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(1);
    hbl->addWidget(cmbStageY);
    btnConnectY=new QToolButton(this);
    hbl->addWidget(btnConnectY);
    btnConfigureY=new QToolButton(this);
    hbl->addWidget(btnConfigureY);
    hbl->addStretch();
    stagelayout->addRow(tr("<b>y axis:</b>"), hbl);
    cmbStageY->setEnabled(false);

    cmbStageZ=new QFStageComboBox(this);
    hbl=new QHBoxLayout(this);
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(1);
    hbl->addWidget(cmbStageZ);
    btnConnectZ=new QToolButton(this);
    hbl->addWidget(btnConnectZ);
    btnConfigureZ=new QToolButton(this);
    hbl->addWidget(btnConfigureZ);
    hbl->addStretch();
    stagelayout->addRow(tr("<b>z axis:</b>"), hbl);
    cmbStageZ->setEnabled(false);




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
    gl->addWidget(new QLabel("<b>move ...</b>", this), 0, 3,1,2);
    spinMoveX=new QDoubleSpinBox(this);
    spinMoveX->setRange(-1e6,1e6);
    spinMoveX->setSingleStep(1);
    spinMoveX->setDecimals(2);
    spinMoveX->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    gl->addWidget(spinMoveX, 1,0);
    spinMoveY=new QDoubleSpinBox(this);
    spinMoveY->setRange(-1e6,1e6);
    spinMoveY->setSingleStep(1);
    spinMoveY->setDecimals(2);
    spinMoveY->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    gl->addWidget(spinMoveY, 1,1);
    spinMoveZ=new QDoubleSpinBox(this);
    spinMoveZ->setRange(-1e6,1e6);
    spinMoveZ->setSingleStep(1);
    spinMoveZ->setDecimals(2);
    spinMoveZ->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    gl->addWidget(spinMoveZ, 1,2);
    btnMoveAbsolute=new QPushButton(QIcon(":/spimb040/move_abs.png"), "", this);
    btnMoveRelative=new QPushButton(QIcon(":/spimb040/move_rel.png"), "", this);
    gl->addWidget(btnMoveRelative, 1, 3);
    gl->addWidget(btnMoveAbsolute, 1, 4);
    gl->addWidget(new QWidget(this), 1, 5);
    gl->setColumnStretch(5,10);
    gl->setContentsMargins(0,0,0,0);
    gl->setHorizontalSpacing(1);
    gl->setVerticalSpacing(1);
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
    l=new QLabel(tr("<b>position [&mu;m]:</b>"));
    l->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    l->setStyleSheet("background-color: darkgrey;");
    l->setAutoFillBackground(true);
    gl->addWidget(l, 2, 0);
    l=new QLabel(tr("<b>speed [&mu;m/s]:</b>"));
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

    labThread=new QLabel(this);
    QFont f=labThread->font();
    f.setPointSizeF(f.pointSizeF()*0.75);
    labThread->setFont(f);

    gl->addWidget(labXState, 1,1);
    gl->addWidget(labYState, 1,2);
    gl->addWidget(labZState, 1,3);
    gl->addWidget(labXPosition, 2,1);
    gl->addWidget(labYPosition, 2,2);
    gl->addWidget(labZPosition, 2,3);
    gl->addWidget(labXSpeed, 3,1);
    gl->addWidget(labYSpeed, 3,2);
    gl->addWidget(labZSpeed, 3,3);
    gl->addWidget(new QWidget(this), 4,4);
    gl->addWidget(labThread, 5,0);
    gl->setColumnStretch(4,10);
    gl->setRowStretch(4,10);
    gl->setContentsMargins(0,0,0,0);
    gl->setHorizontalSpacing(2);
    gl->setVerticalSpacing(2);
    stagelayout->addRow(tr("<b>status:</b>"), gl);

}

void QFESPIMB040SampleStageConfig::createActions() {
    std::cout<<">>>> createActions()\n";
    actConnectX=new QAction(QIcon(":/spimb040/stageconnect.png"), tr("Connect x-axis ..."), this);
    actConnectX->setCheckable(true);
    connect(actConnectX, SIGNAL(toggled(bool)), this, SLOT(disConnectX()));
    btnConnectX->setDefaultAction(actConnectX);

    actConnectY=new QAction(QIcon(":/spimb040/stageconnect.png"), tr("Connect y-axis ..."), this);
    actConnectY->setCheckable(true);
    connect(actConnectY, SIGNAL(toggled(bool)), this, SLOT(disConnectY()));
    btnConnectY->setDefaultAction(actConnectY);

    actConnectZ=new QAction(QIcon(":/spimb040/stageconnect.png"), tr("Connect z-axis ..."), this);
    actConnectZ->setCheckable(true);
    connect(actConnectZ, SIGNAL(toggled(bool)), this, SLOT(disConnectZ()));
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

    if (useThread) {
        /*anyconn=stageThread->anyConnected();
        btnMoveAbsolute->setEnabled(anyconn);
        btnMoveRelative->setEnabled(anyconn);

        chkJoystick->setEnabled(anyconn);
        spinJoystickMaxSpeed->setEnabled(anyconn);*/
    } else {
        labThread->setText("");
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
            if (anyjoy || chkJoystick->isChecked()) labJoystick->setPixmap(iconJoystick);
            else labJoystick->setPixmap(iconNoJoystick);
        } else {
            labJoystick->setPixmap(QPixmap());
        }
    }


    //if (!anyconn) locked=false;
}

void QFESPIMB040SampleStageConfig::disConnectX() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (useThread) stageThread->stopThread();
    bool conn=actConnectX->isChecked();
    QFExtensionLinearStage* stage=getXStage();
    int axis=getXStageAxis();
    bool oldJoystick=chkJoystick->isChecked();
    chkJoystick->setChecked(false);
    if (stage) {
        if (conn) {
            stage->setLogging(m_log);
            stage->connectDevice(axis);
            if (stage->isConnected(axis) && (stage->getAxisState(axis)==QFExtensionLinearStage::Ready)) {
                m_log->log_text("connected to x-axis stage driver ...\n");
            } else {
                actConnectX->setChecked(false);
                stage->disconnectDevice(axis);
                m_log->log_error("error connecting to x-axis stage driver ...\n");
            }
        } else {
            stage->disconnectDevice(axis);
            m_log->log_text("disconnected from x-axis stage driver ...\n");
        }
    } else {
        actConnectX->setChecked(false);
    }
    chkJoystick->setChecked(oldJoystick);
    updateStates();
    QApplication::restoreOverrideCursor();
    if (useThread) stageThread->start();
}


void QFESPIMB040SampleStageConfig::disConnectY() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (useThread) stageThread->stopThread();
    bool conn=actConnectY->isChecked();
    QFExtensionLinearStage* stage=getYStage();
    int axis=getYStageAxis();
    bool oldJoystick=chkJoystick->isChecked();
    chkJoystick->setChecked(false);
    if (stage) {
        if (conn) {
            stage->setLogging(m_log);
            stage->connectDevice(axis);
            if (stage->isConnected(axis) && (stage->getAxisState(axis)==QFExtensionLinearStage::Ready)) {
                m_log->log_text("connected to y-axis stage driver ...\n");
            } else {
                actConnectY->setChecked(false);
                stage->disconnectDevice(axis);
                m_log->log_error("error connecting to y-axis stage driver ...\n");
            }
        } else {
            stage->disconnectDevice(axis);
            m_log->log_text("disconnected from y-axis stage driver ...\n");
        }
    } else {
        actConnectY->setChecked(false);
    }
    updateStates();
    QApplication::restoreOverrideCursor();
    if (useThread) stageThread->start();
}


void QFESPIMB040SampleStageConfig::disConnectZ() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (useThread) stageThread->stopThread();
    bool conn=actConnectZ->isChecked();
    QFExtensionLinearStage* stage=getZStage();
    int axis=getZStageAxis();
    bool oldJoystick=chkJoystick->isChecked();
    chkJoystick->setChecked(false);
    if (stage) {
        if (conn) {
            stage->setLogging(m_log);
            stage->connectDevice(axis);
            if (stage->isConnected(axis) && (stage->getAxisState(axis)==QFExtensionLinearStage::Ready)) {
                m_log->log_text("connected to z-axis stage driver ...\n");
            } else {
                actConnectZ->setChecked(false);
                stage->disconnectDevice(axis);
                m_log->log_error("error connecting to z-axis stage driver ...\n");
            }
        } else {
            stage->disconnectDevice(axis);
            m_log->log_text("disconnected from z-axis stage driver ...\n");
        }
    } else {
        actConnectZ->setChecked(false);
    }
    chkJoystick->setChecked(oldJoystick);
    updateStates();
    QApplication::restoreOverrideCursor();
    if (useThread) stageThread->start();
}



void QFESPIMB040SampleStageConfig::configureX() {
    if (useThread) stageThread->stopThread();
    QFExtensionLinearStage* stage=getXStage();
    int axis=getXStageAxis();
    if (stage) stage->showSettingsDialog(axis, this);
    updateStates();
    if (useThread) stageThread->start();
}

void QFESPIMB040SampleStageConfig::configureY() {
    if (useThread) stageThread->stopThread();
    QFExtensionLinearStage* stage=getYStage();
    int axis=getYStageAxis();
    if (stage) stage->showSettingsDialog(axis, this);
    updateStates();
    if (useThread) stageThread->start();
}

void QFESPIMB040SampleStageConfig::configureZ() {
    if (useThread) stageThread->stopThread();
    QFExtensionLinearStage* stage=getZStage();
    int axis=getZStageAxis();
    if (stage) stage->showSettingsDialog(axis, this);
    updateStates();
    if (useThread) stageThread->start();
}



QFExtensionLinearStage* QFESPIMB040SampleStageConfig::getXStage() {
    return cmbStageX->currentExtensionLinearStage();
}

QFExtension* QFESPIMB040SampleStageConfig::getXStageExtension() {
    return cmbStageX->currentExtension();
}

int QFESPIMB040SampleStageConfig::getXStageAxis() {

    return cmbStageX->currentAxisID();
}


QFExtensionLinearStage* QFESPIMB040SampleStageConfig::getYStage() {
    return cmbStageY->currentExtensionLinearStage();
}

QFExtension* QFESPIMB040SampleStageConfig::getYStageExtension() {
    return cmbStageY->currentExtension();
}

int QFESPIMB040SampleStageConfig::getYStageAxis() {

    return cmbStageY->currentAxisID();
}


QFExtensionLinearStage* QFESPIMB040SampleStageConfig::getZStage() {
    return cmbStageZ->currentExtensionLinearStage();
}

QFExtension* QFESPIMB040SampleStageConfig::getZStageExtension() {
    return cmbStageZ->currentExtension();
}

int QFESPIMB040SampleStageConfig::getZStageAxis() {

    return cmbStageZ->currentAxisID();
}



void QFESPIMB040SampleStageConfig::updateJoystick() {

    double speed=spinJoystickMaxSpeed->value();
    bool enabled=chkJoystick->isChecked();

    if (useThread) {
        stageThread->setJoystick(enabled, speed);
    } else {

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
        updateStates();
    }
}

void QFESPIMB040SampleStageConfig::displayAxisStates(/*bool automatic*/) {
    if (useThread) return;


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


        updateStates();

    /*if ((!locked) && anyconn && automatic) {
        QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
    }*/
    QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}

void QFESPIMB040SampleStageConfig::moveAbsolute() {
    double x=spinMoveX->value();
    double y=spinMoveY->value();
    double z=spinMoveZ->value();

    if (useThread) {
        stageThread->move(x,y,z);
    } else {
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
}

void QFESPIMB040SampleStageConfig::moveRelative() {
    double x=spinMoveX->value();
    double y=spinMoveY->value();
    double z=spinMoveZ->value();

    if (useThread) {
        stageThread->moveRel(x,y,z);
    } else {
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


}

void QFESPIMB040SampleStageConfig::connectStages() {
    actConnectX->setChecked(true);
    actConnectY->setChecked(true);
    actConnectZ->setChecked(true);
}

void QFESPIMB040SampleStageConfig::disconnectStages() {
    actConnectX->setChecked(false);
    actConnectY->setChecked(false);
    actConnectZ->setChecked(false);
}

void QFESPIMB040SampleStageConfig::setReadOnly(bool readonly) {
    cmbStageX->setReadOnly(readonly);
    cmbStageY->setReadOnly(readonly);
    cmbStageZ->setReadOnly(readonly);
}

void QFESPIMB040SampleStageConfig::stageXMoved(QFExtensionLinearStage::AxisState state, double position, double speed) {
    updateStageStateWidgets(labXPosition, labXSpeed, labXState, getXStage(), state, position, speed);
}

void QFESPIMB040SampleStageConfig::stageYMoved(QFExtensionLinearStage::AxisState state, double position, double speed) {
    updateStageStateWidgets(labYPosition, labYSpeed, labYState, getYStage(), state, position, speed);
}

void QFESPIMB040SampleStageConfig::stageZMoved(QFExtensionLinearStage::AxisState state, double position, double speed) {
    updateStageStateWidgets(labZPosition, labZSpeed, labZState, getZStage(), state, position, speed);
}

void QFESPIMB040SampleStageConfig::updateStageStateWidgets(QLabel* labPos, QLabel* labSpeed, QLabel* labState, bool present, QFExtensionLinearStage::AxisState state, double position, double speed) {
    if (present) {
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
    } else {
        labState->setText("---");
        labPos->setText("");
        labSpeed->setText("");
    }
}

void QFESPIMB040SampleStageConfig::joystickStateChanged(bool enabled) {
    bool anyconn=stageThread->anyConnected();
    //chkJoystick->setEnabled(anyconn);
    spinJoystickMaxSpeed->setEnabled(anyconn && enabled);
    if (anyconn) {
        if (enabled!=chkJoystick->isChecked()) chkJoystick->setChecked(enabled);
        if (enabled) labJoystick->setPixmap(iconJoystick);
        else labJoystick->setPixmap(iconNoJoystick);
    } else {
        labJoystick->setPixmap(QPixmap());
    }
}

void QFESPIMB040SampleStageConfig::stagesConnectedChanged(bool connX, bool connY, bool connZ) {
    bool anyconn=false;
    bool conn=connX;
    anyconn=anyconn||conn;
    if (conn) {
        actConnectX->setChecked(true);
        actConnectX->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
        actConnectX->setText(tr("Disconnect x-axis ..."));
    } else {
        actConnectX->setChecked(false);
        actConnectX->setIcon(QIcon(":/spimb040/stageconnect.png"));
        actConnectX->setText(tr("Connect x-axis ..."));
    }
    actConfigureX->setEnabled(true);
    cmbStageX->setEnabled(!conn);
    spinMoveX->setEnabled(conn);


    conn=connY;
    anyconn=anyconn||conn;
    if (conn) {
        actConnectY->setChecked(true);
        actConnectY->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
        actConnectY->setText(tr("Disconnect y-axis ..."));
    } else {
        actConnectY->setChecked(false);
        actConnectY->setIcon(QIcon(":/spimb040/stageconnect.png"));
        actConnectY->setText(tr("Connect y-axis ..."));
    }
    actConfigureY->setEnabled(true);
    cmbStageY->setEnabled(!conn);
    spinMoveY->setEnabled(conn);


    conn=connZ;
    anyconn=anyconn||conn;
    if (conn) {
        actConnectZ->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
        actConnectZ->setChecked(true);
        actConnectZ->setText(tr("Disconnect z-axis ..."));
    } else {
        actConnectZ->setChecked(false);
        actConnectZ->setIcon(QIcon(":/spimb040/stageconnect.png"));
        actConnectZ->setText(tr("Connect z-axis ..."));
    }

    actConfigureZ->setEnabled(true);
    cmbStageZ->setEnabled(!conn);
    spinMoveZ->setEnabled(conn);

    btnMoveAbsolute->setEnabled(anyconn);
    btnMoveRelative->setEnabled(anyconn);

    chkJoystick->setEnabled(anyconn);

}

void QFESPIMB040SampleStageConfig::threadStarted() {
    labThread->setText(tr("thread started ..."));
    QApplication::processEvents();
}

void QFESPIMB040SampleStageConfig::threadFinished() {
    labThread->setText(tr("thread finished ..."));
    QApplication::processEvents();
}

bool QFESPIMB040SampleStageConfig::isXStageConnected() const {
    return actConnectX->isChecked();
}

bool QFESPIMB040SampleStageConfig::isYStageConnected() const {
    return actConnectY->isChecked();
}

bool QFESPIMB040SampleStageConfig::isZStageConnected() const {
    return actConnectZ->isChecked();
}
