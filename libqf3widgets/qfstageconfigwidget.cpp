/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qfstageconfigwidget.h"

#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <iostream>

#include "qfstageconfigwidgetthread.h"



QFStageConfigWidget::QFStageConfigWidget(QWidget* parent, bool useThread):
    QFrame(parent)
{

    timUpdate=new QTimer(this);
    timUpdate->setSingleShot(true);
    timUpdate->setInterval(stageStateUpdateInterval);
    setFrameStyle(QFrame::Panel|QFrame::Raised);
    setLineWidth(1);

    m_log=NULL;
    m_pluginServices=NULL;
    locked=false;

    m_stepX=1;

    this->useThread=useThread;
    stageThread=new QFStageConfigWidgetThread(this);
    connect(stageThread, SIGNAL(joystickStateChanged(bool)), this, SLOT(joystickStateChanged(bool)));
    connect(stageThread, SIGNAL(stagesConnectedChanged(bool)), this, SLOT(stagesConnectedChanged(bool)));
    connect(stageThread, SIGNAL(stageXMoved(QFExtensionLinearStage::AxisState,double,double)), this, SLOT(stageXMoved(QFExtensionLinearStage::AxisState,double,double)));
    connect(stageThread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(stageThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(stageThread, SIGNAL(terminated()), this, SLOT(threadFinished()));

    stageStateUpdateInterval=179;
    iconDisconnected=QPixmap(":/libqf3widgets/stage_disconnected.png");
    iconReady=QPixmap(":/libqf3widgets/stage_ready.png");
    iconError=QPixmap(":/libqf3widgets/stage_error.png");
    iconMoving=QPixmap(":/libqf3widgets/stage_moving.png");
    iconMovingOpposite=QPixmap(":/libqf3widgets/stage_moving2.png");
    iconJoystick=QPixmap(":/libqf3widgets/stage_joystick.png");
    iconNoJoystick=QPixmap(":/libqf3widgets/stage_nojoystick.png");
    //timerDisplayUpdate.setInterval(stageStateUpdateInterval);
    //connect(&timerDisplayUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
    //timerDisplayUpdate.start();

    //findStages(m_pluginServices->getExtensionManager());
    createWidgets();
    createActions();
    updateStates();

    if (useThread) {
        stageThread->start(QThread::LowPriority);
    } else {
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
        timUpdate->start(stageStateUpdateInterval);
    }
    //QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}


QFStageConfigWidget::~QFStageConfigWidget()
{
}

void QFStageConfigWidget::lockStages() {
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

void QFStageConfigWidget::unlockStages() {
    if (useThread) {
        locked=false;
        stageThread->start(QThread::LowPriority);
    } else {
        locked=false;
        connect(timUpdate, SIGNAL(timeout()), this, SLOT(displayAxisStates()));
        timUpdate->setSingleShot(true);
        timUpdate->setInterval(stageStateUpdateInterval);
        timUpdate->start(stageStateUpdateInterval);
    }
}


void QFStageConfigWidget::setLog(QFPluginLogService* log) {
    m_log=log;
}

void QFStageConfigWidget::init(QFPluginLogService* log, QFPluginServices* pluginServices) {
    m_log=log;
    m_pluginServices=pluginServices;

    if (m_pluginServices) {
        cmbStageX->init(m_pluginServices->getExtensionManager());
    } else {
        //stages.clear();
        cmbStageX->clear();
    }
    updateStates();
}

void QFStageConfigWidget::loadSettings(QSettings& settings, QString prefix, bool dontLoadDevices) {
    if (!dontLoadDevices) {
        cmbStageX->loadSettings(settings, prefix+"stage_x/");
    }

    spinJoystickMaxSpeed->setValue(settings.value(prefix+"joystick_max_speed", 500).toDouble());
    stageStateUpdateInterval=settings.value(prefix+"update_interval", stageStateUpdateInterval).toDouble();
    chkJoystick->setChecked(settings.value(prefix+"joystick_enabled", true).toBool());
    m_stepX=settings.value(prefix+"step_x", m_stepX).toDouble();
    //timerDisplayUpdate.setInterval(stageStateUpdateInterval);
}

void QFStageConfigWidget::loadSettings(QFManyFilesSettings &settings, QString prefix, bool dontLoadDevices)
{
    if (!dontLoadDevices) {
        cmbStageX->loadSettings(settings, prefix+"stage_x/");
    }

    spinJoystickMaxSpeed->setValue(settings.value(prefix+"joystick_max_speed", 500).toDouble());
    stageStateUpdateInterval=settings.value(prefix+"update_interval", stageStateUpdateInterval).toDouble();
    chkJoystick->setChecked(settings.value(prefix+"joystick_enabled", true).toBool());
    m_stepX=settings.value(prefix+"step_x", m_stepX).toDouble();
}

void QFStageConfigWidget::saveSettings(QSettings& settings, QString prefix) {
    /*settings.setValue(prefix+"stage_x", cmbStageX->currentIndex());
    settings.setValue(prefix+"stage_y", cmbStageY->currentIndex());
    settings.setValue(prefix+"stage_z", cmbStageZ->currentIndex());*/
    cmbStageX->storeSettings(settings, prefix+"stage_x/");
    settings.setValue(prefix+"joystick_max_speed", spinJoystickMaxSpeed->value());
    settings.setValue(prefix+"joystick_enabled", chkJoystick->isChecked());
    settings.setValue(prefix+"update_interval", stageStateUpdateInterval);
    settings.setValue(prefix+"step_x", m_stepX);
}

void QFStageConfigWidget::saveSettings(QFManyFilesSettings &settings, QString prefix)
{
    /*settings.setValue(prefix+"stage_x", cmbStageX->currentIndex());
    settings.setValue(prefix+"stage_y", cmbStageY->currentIndex());
    settings.setValue(prefix+"stage_z", cmbStageZ->currentIndex());*/
    cmbStageX->storeSettings(settings, prefix+"stage_x/");
    settings.setValue(prefix+"joystick_max_speed", spinJoystickMaxSpeed->value());
    settings.setValue(prefix+"joystick_enabled", chkJoystick->isChecked());
    settings.setValue(prefix+"update_interval", stageStateUpdateInterval);
    settings.setValue(prefix+"step_x", m_stepX);
}


void QFStageConfigWidget::createWidgets() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create main layout
    ////////////////////////////////////////////////////////////////////////////////////////////////
    QFormLayout* stagelayout=new QFormLayout();
    stagelayout->setHorizontalSpacing(2);
    setLayout(stagelayout);


    QLabel* l;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // create input widgets for camera devices
    ////////////////////////////////////////////////////////////////////////////////////////////////
    btnConfigSteps=new QToolButton(this);

    cmbStageX=new QFStageComboBox(this);
    QHBoxLayout* hbl=new QHBoxLayout();
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(1);
    hbl->addWidget(cmbStageX);
    btnConnectX=new QToolButton(this);
    hbl->addWidget(btnConnectX);
    btnConfigureX=new QToolButton(this);
    hbl->addWidget(btnConfigureX);
    hbl->addStretch();
    hbl->addWidget(btnConfigSteps);
    stagelayout->addRow(tr("<b>axis:</b>"), hbl);
    cmbStageX->setEnabled(false);




    hbl=new QHBoxLayout();
    hbl->setContentsMargins(0,0,0,0);
    chkJoystick=new QCheckBox(tr("joystick:"), this);
    QFont fb=chkJoystick->font();
    fb.setBold(true);
    chkJoystick->setFont(fb);
    connect(chkJoystick, SIGNAL(toggled(bool)), this, SLOT(updateJoystick()));
    hbl->addWidget(l=new QLabel("max. speed [&mu;m/s]: ", this));
    l->setTextFormat(Qt::RichText);
    spinJoystickMaxSpeed=new QDoubleSpinBox(this);
    spinJoystickMaxSpeed->setMinimum(1);
    spinJoystickMaxSpeed->setMaximum(10000);
    spinJoystickMaxSpeed->setSingleStep(10);
    spinJoystickMaxSpeed->setDecimals(0);
    connect(spinJoystickMaxSpeed, SIGNAL(valueChanged(double)), this, SLOT(updateJoystick()));
    hbl->addWidget(spinJoystickMaxSpeed);
    btnX2=new QToolButton(this);
    btnX2->setIcon(QIcon(":/spimb040/x2.png"));
    btnX2->setToolTip(tr("double the joystick maximum speed"));
    //btnX2->setMaximumSize(cmbStageX->height()*3, cmbStageX->height()*1.2);
    hbl->addWidget(btnX2);
    connect(btnX2, SIGNAL(clicked()), this, SLOT(speedX2()));
    btnX10=new QToolButton(this);
    btnX10->setIcon(QIcon(":/spimb040/x10.png"));
    btnX10->setToolTip(tr("increase the joystick maximum speed ten-fold"));
    //btnX10->setMaximumSize(cmbStageX->height()*3, cmbStageX->height()*1.2);
    hbl->addWidget(btnX10);
    connect(btnX10, SIGNAL(clicked()), this, SLOT(speedX10()));
    btnD2=new QToolButton(this);
    btnD2->setIcon(QIcon(":/spimb040/d2.png"));
    btnD2->setToolTip(tr("half the joystick maximum speed"));
    //btnD2->setMaximumSize(cmbStageX->height()*3, cmbStageX->height()*1.2);
    hbl->addWidget(btnD2);
    connect(btnD2, SIGNAL(clicked()), this, SLOT(speedD2()));
    btnD10=new QToolButton(this);
    //btnD10->setMaximumSize(cmbStageX->height()*3, cmbStageX->height()*1.2);
    btnD10->setIcon(QIcon(":/spimb040/d10.png"));
    btnD10->setToolTip(tr("decrease the joystick maximum speed ten-fold"));
    hbl->addWidget(btnD10);
    connect(btnD10, SIGNAL(clicked()), this, SLOT(speedD10()));
    hbl->addStretch();
    stagelayout->addRow(chkJoystick, hbl);

    QGridLayout* gl=new QGridLayout();
    spinMoveX=new QDoubleSpinBox(this);
    spinMoveX->setRange(-1e6,1e6);
    spinMoveX->setSingleStep(1);
    spinMoveX->setDecimals(2);
    spinMoveX->setSuffix(QLatin1String(" \xB5m"));
    spinMoveX->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    btnMoveAbsolute=new QPushButton(QIcon(":/spimb040/move_abs.png"), "", this);
    btnMoveRelative=new QPushButton(QIcon(":/spimb040/move_rel.png"), "", this);
    gl->addWidget(spinMoveX, 0, 0);
    gl->addWidget(btnMoveRelative, 0, 1);
    gl->addWidget(btnMoveAbsolute, 0, 2);
    gl->setContentsMargins(0,0,0,0);
    gl->setHorizontalSpacing(1);
    gl->setVerticalSpacing(1);
    connect(btnMoveAbsolute, SIGNAL(clicked()), this, SLOT(moveAbsolute()));
    connect(btnMoveRelative, SIGNAL(clicked()), this, SLOT(moveRelative()));

    gl->setColumnStretch(3,1);
    gl->addWidget(new QLabel("<b>status:</b>"), 0, 4);
    labXState=new QLabel(this);
    labXState->setStyleSheet("background-color: 	lightgrey;");;
    labXState->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labXState->setAutoFillBackground(true);
    gl->addWidget(labXState, 0, 5);
    gl->addWidget(new QLabel("pos = "), 0, 6);
    labXPosition=new QLabel(this);
    labXPosition->setStyleSheet("background-color: 	lightgrey;");;
    labXPosition->setAutoFillBackground(true);
    labXPosition->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    gl->addWidget(labXPosition, 0, 7);
    gl->addWidget(new QLabel("vel = "), 0, 8);
    labXSpeed=new QLabel(this);
    labXSpeed->setStyleSheet("background-color: 	lightgrey;");;
    labXSpeed->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labXSpeed->setAutoFillBackground(true);
    gl->addWidget(labXSpeed, 0, 9);
    labJoystick=new QLabel(this);
    labJoystick->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    gl->addWidget(labJoystick, 0,10);

    stagelayout->addRow(tr("<b>move:</b>"), gl);

}

void QFStageConfigWidget::createActions() {
    std::cout<<">>>> createActions()\n";
    actConnectX=new QAction(QIcon(":/spimb040/stageconnect.png"), tr("Connect x-axis ..."), this);
    actConnectX->setCheckable(true);
    connect(actConnectX, SIGNAL(triggered()), this, SLOT(disConnectX()));
    btnConnectX->setDefaultAction(actConnectX);

    actConfigureX=new QAction(QIcon(":/spimb040/stagesettings.png"), tr("Configure x-axis ..."), this);
    connect(actConfigureX, SIGNAL(triggered()), this, SLOT(configureX()));
    btnConfigureX->setDefaultAction(actConfigureX);

    actConfigSteps=new QAction(QIcon(":/spimb040/configsteps.png"), tr("Configure step sizes ..."), this);
    connect(actConfigSteps, SIGNAL(triggered()), this, SLOT(configSteps()));
    btnConfigSteps->setDefaultAction(actConfigSteps);

}

void QFStageConfigWidget::updateStates() {
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
        //labThread->setText("");
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

        btnMoveAbsolute->setEnabled(anyconn);
        btnMoveRelative->setEnabled(anyconn);

        chkJoystick->setEnabled(anyconn);
        spinJoystickMaxSpeed->setEnabled(anyconn);
        btnX2->setEnabled(anyconn);
        btnX10->setEnabled(anyconn);
        btnD2->setEnabled(anyconn);
        btnD10->setEnabled(anyconn);
        if (anyconn) {
            if (anyjoy || chkJoystick->isChecked()) labJoystick->setPixmap(iconJoystick);
            else labJoystick->setPixmap(iconNoJoystick);
        } else {
            labJoystick->setPixmap(QPixmap());
        }
    }


    //if (!anyconn) locked=false;
}

void QFStageConfigWidget::disConnectX() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (useThread) stageThread->stopThread();
    bool conn=actConnectX->isChecked();
    QFExtensionLinearStage* stage=getXStage();
    int axis=getXStageAxis();
    bool oldJoystick=chkJoystick->isChecked();
    chkJoystick->setChecked(false);
    updateJoystick();

    if (stage) {
        if (conn) {
            stage->setLogging(m_log);
            stage->connectDevice(axis);
            if (stage->isConnected(axis) && (stage->getAxisState(axis)==QFExtensionLinearStage::Ready)) {
                m_log->log_text("connected to x-axis stage driver ...\n");
                updateStageSettings(stage->getStageInfo(axis));
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
    updateJoystick();
    updateStates();
    QApplication::restoreOverrideCursor();
    if (useThread) stageThread->start(QThread::LowPriority);
}



void QFStageConfigWidget::configureX() {
    if (useThread) stageThread->stopThread();
    QFExtensionLinearStage* stage=getXStage();
    int axis=getXStageAxis();
    if (stage) stage->showSettingsDialog(axis, this);
    updateStates();
    if (useThread) stageThread->start(QThread::LowPriority);
}


QFExtensionLinearStage* QFStageConfigWidget::getXStage() {
    return cmbStageX->currentExtensionLinearStage();
}

QFExtension* QFStageConfigWidget::getXStageExtension() {
    return cmbStageX->currentExtension();
}

int QFStageConfigWidget::getXStageAxis() {

    return cmbStageX->currentAxisID();
}


QObject *QFStageConfigWidget::getXStageExtensionObject() {
    return cmbStageX->currentExtensionObject();
}




void QFStageConfigWidget::updateJoystick() {

    double speed=spinJoystickMaxSpeed->value();
    bool enabled=chkJoystick->isChecked();

    if (useThread) {
        stageThread->setJoystick(enabled, speed);
    } else {

        spinJoystickMaxSpeed->setEnabled(enabled);
        btnX2->setEnabled(enabled);
        btnX10->setEnabled(enabled);
        btnD2->setEnabled(enabled);
        btnD10->setEnabled(enabled);
        QFExtensionLinearStage* stage;
        int axis;

        stage=getXStage();
        axis=getXStageAxis();
        if (stage) {
            if (stage->isConnected(axis)) {
                stage->setJoystickActive(axis, enabled, speed);
            }
        }
        updateStates();
    }
}

void QFStageConfigWidget::displayAxisStates(/*bool automatic*/) {
    if (useThread) return;


    bool updt=updatesEnabled(); bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
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


        updateStates();
        if (widVisible) setUpdatesEnabled(updt);

    /*if ((!locked) && anyconn && automatic) {
        QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
    }*/
    QTimer::singleShot(stageStateUpdateInterval, this, SLOT(displayAxisStates()));
}

void QFStageConfigWidget::moveAbsolute() {
    double x=spinMoveX->value();

    if (useThread) {
        stageThread->move(x);
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

    }
}

void QFStageConfigWidget::moveRelative() {
    double x=spinMoveX->value();
    moveRelative(x);
}

void QFStageConfigWidget::configSteps() {
    bool ok=false;
    double res=QInputDialog::getDouble(this, tr("select stage step size"), tr("new step size:"),m_stepX, 0, 1e28,3,&ok);
    if (ok) m_stepX=res;
}


void QFStageConfigWidget::moveRelative(double x) {
    if (useThread) {
        stageThread->moveRel(x);
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

    }


}

void QFStageConfigWidget::connectStages() {
    bool updt=updatesEnabled(); bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    bool bx=actConnectX->signalsBlocked();
    actConnectX->blockSignals(true);

    if (cmbStageX->currentExtensionLinearStage() && !actConnectX->isChecked()) {
        actConnectX->setChecked(true);
        disConnectX();
    }
    actConnectX->blockSignals(bx);
    if (widVisible) setUpdatesEnabled(updt);
}

void QFStageConfigWidget::disconnectStages() {
    bool updt=updatesEnabled(); bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    if (cmbStageX->currentExtensionLinearStage() && actConnectX->isChecked()) {
        actConnectX->setChecked(true);
        actConnectX->trigger();
    }
    if (widVisible) setUpdatesEnabled(updt);
}

void QFStageConfigWidget::setReadOnly(bool readonly) {
    bool updt=updatesEnabled(); bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    cmbStageX->setReadOnly(readonly);
    if (widVisible) setUpdatesEnabled(updt);
}

void QFStageConfigWidget::stageXMoved(QFExtensionLinearStage::AxisState state, double position, double speed) {
    updateStageStateWidgets(labXPosition, labXSpeed, labXState, getXStage(), state, position, speed);
}


void QFStageConfigWidget::updateStageStateWidgets(QLabel* labPos, QLabel* labSpeed, QLabel* labState, bool present, QFExtensionLinearStage::AxisState state, double position, double speed) {
    if (present) {
        //bool updt=updatesEnabled(); setUpdatesEnabled(false);
        if (labState->text()!="") labState->setText("");
        switch(state) {
            case QFExtensionLinearStage::Ready : {
                    QString txt=tr("stage ready");
                    if (labState->toolTip()!=txt) {
                        labState->setPixmap(iconReady);
                        labState->setToolTip(txt);
                    }
                } break;
            case QFExtensionLinearStage::Disconnected : {
                    QString txt=tr("stage disconnected");
                    if (labState->toolTip()!=txt) {
                        labState->setPixmap(iconDisconnected);
                        labState->setToolTip(txt);
                    }
                } break;
            case QFExtensionLinearStage::Moving : {
                    if (speed>0) {
                        QString txt=tr("stage moving forward");
                        if (labState->toolTip()!=txt) {
                            labState->setPixmap(iconMoving);
                            labState->setToolTip(txt);
                        }
                    } else {
                        QString txt=tr("stage moving backward");
                        if (labState->toolTip()!=txt) {
                            labState->setPixmap(iconMovingOpposite);
                            labState->setToolTip(txt);
                        }
                    }
                } break;
            case QFExtensionLinearStage::Error : {
                    QString txt=tr("an error occured on this stage");
                    if (labState->toolTip()!=txt) {
                        labState->setPixmap(iconError);
                        labState->setToolTip(txt);
                    }
                } break;
            default:
                if (labState->text()!=tr("?")) labState->setText(tr("?"));
                break;
        }
        QString txt=QString::number(speed, 'f', 2);
        if (labSpeed->text()!=txt) labSpeed->setText(txt);
        txt=QString::number(position, 'f', 2);
        if (labPos->text()!=txt) labPos->setText(txt);
        //setUpdatesEnabled(updt);
    } else {
        //bool updt=updatesEnabled(); setUpdatesEnabled(false);
        if (labState->text()!="---") labState->setText("---");
        if (!labPos->text().isEmpty()) labPos->setText("");
        if (!labSpeed->text().isEmpty()) labSpeed->setText("");
        //setUpdatesEnabled(updt);
    }
}

void QFStageConfigWidget::updateStageSettings(QFExtensionLinearStage::StageInfo info)
{
    spinJoystickMaxSpeed->setRange(info.minSpeed, info.maxSpeed);
    spinJoystickMaxSpeed->setSuffix(info.speedUnit);

    spinMoveX->setRange(info.minPos, info.maxPos);
    spinMoveX->setSuffix(info.positionUnit);
}

void QFStageConfigWidget::joystickStateChanged(bool enabled) {
    //bool updt=updatesEnabled(); setUpdatesEnabled(false);
    bool anyconn=stageThread->anyConnected();
    //chkJoystick->setEnabled(anyconn);
    bool en=anyconn && enabled;
    if (spinJoystickMaxSpeed->isEnabled()!=en) spinJoystickMaxSpeed->setEnabled(en);
    if (btnX2->isEnabled()!=en) btnX2->setEnabled(en);
    if (btnX10->isEnabled()!=en) btnX10->setEnabled(en);
    if (btnD2->isEnabled()!=en) btnD2->setEnabled(en);
    if (btnD10->isEnabled()!=en) btnD10->setEnabled(en);
    if (anyconn) {
        if (enabled!=chkJoystick->isChecked()) chkJoystick->setChecked(enabled);
        if (enabled) {
            QString txt=tr("joystick enabled");
            if (labJoystick->toolTip()!=txt) {
                labJoystick->setPixmap(iconJoystick);
                labJoystick->setToolTip(txt);
            }

        } else {
            QString txt=tr("joystick disabled");
            if (labJoystick->toolTip()!=txt) {
                labJoystick->setPixmap(iconNoJoystick);
                labJoystick->setToolTip(txt);
            }
        }
    } else {
        QString txt="";
        if (labJoystick->toolTip()!=txt) {
            labJoystick->setPixmap(QPixmap());
            labJoystick->setToolTip(txt);
        }
    }
    //setUpdatesEnabled(updt);
}

void QFStageConfigWidget::stagesConnectedChanged(bool connX) {
    bool anyconn=false;
    bool conn=connX;
    //bool updt=updatesEnabled(); setUpdatesEnabled(false);
    anyconn=anyconn||conn;
    if (conn) {
        if (!actConnectX->isChecked()) actConnectX->setChecked(true);
        QString txt=tr("Disconnect x-axis ...");
        if (actConnectX->text()!=txt) {
            actConnectX->setIcon(QIcon(":/spimb040/stagedisconnect.png"));
            actConnectX->setText(txt);
        }
    } else {
        if (actConnectX->isChecked()) actConnectX->setChecked(false);
        QString txt=tr("Connect x-axis ...");
        if (actConnectX->text()!=txt) {
            actConnectX->setIcon(QIcon(":/spimb040/stageconnect.png"));
            actConnectX->setText(txt);
        }
    }
    if (!actConfigureX->isEnabled()) actConfigureX->setEnabled(true);
    if (cmbStageX->isEnabled()!=!conn) cmbStageX->setEnabled(!conn);
    if (spinMoveX->isEnabled()!=conn) spinMoveX->setEnabled(conn);


    if (btnMoveAbsolute->isEnabled()!=anyconn) btnMoveAbsolute->setEnabled(anyconn);
    if (btnMoveRelative->isEnabled()!=anyconn) btnMoveRelative->setEnabled(anyconn);

    if (chkJoystick->isEnabled()!=anyconn) chkJoystick->setEnabled(anyconn);
    //setUpdatesEnabled(updt);

}

void QFStageConfigWidget::threadStarted() {
    //labThread->setText(tr("thread started ..."));
    QApplication::processEvents();
}

void QFStageConfigWidget::threadFinished() {
    //labThread->setText(tr("thread finished ..."));
    QApplication::processEvents();
}

bool QFStageConfigWidget::isXStageConnected() const {
    return actConnectX->isChecked();
}

bool QFStageConfigWidget::isJoystickChecked() const {
    return chkJoystick->isChecked();
}

double QFStageConfigWidget::joystickMaxSpeed() {
    return spinJoystickMaxSpeed->value();
}
void QFStageConfigWidget::speedX2() {
    spinJoystickMaxSpeed->setValue(spinJoystickMaxSpeed->value()*2);
}

void QFStageConfigWidget::speedX10() {
    spinJoystickMaxSpeed->setValue(spinJoystickMaxSpeed->value()*10);
}

void QFStageConfigWidget::speedD2() {
    spinJoystickMaxSpeed->setValue(spinJoystickMaxSpeed->value()/2);
}

void QFStageConfigWidget::speedD10() {
    spinJoystickMaxSpeed->setValue(spinJoystickMaxSpeed->value()/10);
}

void QFStageConfigWidget::toggleJoystick() {
    chkJoystick->setChecked(!chkJoystick->isChecked());
}

void QFStageConfigWidget::joystickOn() {
    chkJoystick->setChecked(true);
}

void QFStageConfigWidget::joystickOff() {
    chkJoystick->setChecked(false);
}

void QFStageConfigWidget::stepX() {
    moveRelative(m_stepX);
}


void QFStageConfigWidget::stepMinusX() {
    moveRelative(-m_stepX);
}

