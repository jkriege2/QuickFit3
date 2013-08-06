#include "qfstageconfigwidgetthread.h"

#include "qfstageconfigwidget.h"

#include <QtGui>
#include <iostream>

#define THREAD_TIMEOUT 77


QFStageConfigWidgetThread::QFStageConfigWidgetThread(QFStageConfigWidget* parent):
    QThread(parent)
{

    m_parent=parent;
    qRegisterMetaType<QFExtensionLinearStage::AxisState>("QFExtensionLinearStage::AxisState");
    qRegisterMetaType<QFExtensionLinearStage::StageInfo>("QFExtensionLinearStage::StageInfo");
    stopped=true;
    readCounter=0;
    connX=false;
    InstructionMutex=new QMutex();
}

QFStageConfigWidgetThread::~QFStageConfigWidgetThread() {
    stopThread();
    delete InstructionMutex;
}

void QFStageConfigWidgetThread::run() {
    stopped=false;
    while (!stopped) {
        nextInstruction();
        msleep(THREAD_TIMEOUT);
    }
}

bool QFStageConfigWidgetThread::anyConnected() const {
    return connX;
}

void QFStageConfigWidgetThread::start(Priority priority ) {
    QThread::start(priority);
    //qDebug()<<"started";
}

void QFStageConfigWidgetThread::nextInstruction() {
    //qDebug()<<"QFStageConfigWidgetThread::nextInstruction()   stopped="<<stopped<<"  readCounter="<<readCounter;
    if (stopped) {
       /* qDebug()<<"   quit";
        quit();*/
    } else {
        if (m_parent->getXStage()) connX=m_parent->getXStage()->isConnected(m_parent->getXStageAxis()); else connX=false;
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


                emit stagesConnectedChanged(connX);
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
                // read joystick
                /////////////////////////////////////////////////////////////////////////////
                bool joystick=false;
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) joystick=joystick|stage->isJoystickActive(axis);

                emit joystickStateChanged(joystick);

            } else if (readCounter==3) {
                /////////////////////////////////////////////////////////////////////////////
                // set joystick
                /////////////////////////////////////////////////////////////////////////////
                setJoystick(m_parent->isJoystickChecked(), m_parent->joystickMaxSpeed());

            } else {
                readCounter=-1;
            }
            readCounter++;
        } else {
            Instruction inst=instructions.dequeue();
            InstructionMutex->unlock();
            if (inst.type==QFStageConfigWidgetThread::Move) {
                /////////////////////////////////////////////////////////////////////////////
                // move
                /////////////////////////////////////////////////////////////////////////////
                double x=inst.pd1;

                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, x);
                    }
                }

            } else if (inst.type==QFStageConfigWidgetThread::MoveRel) {
                /////////////////////////////////////////////////////////////////////////////
                // move relative
                /////////////////////////////////////////////////////////////////////////////
                double x=inst.pd1;
                QFExtensionLinearStage* stage;
                int axis;

                stage=m_parent->getXStage();
                axis=m_parent->getXStageAxis();
                if (stage) {
                    if (stage->isConnected(axis)) {
                        stage->move(axis, stage->getPosition(axis)+x);
                    }
                }

            } else if (inst.type==QFStageConfigWidgetThread::SetJoystick) {
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

            }
        }
        /*qDebug()<<"   restart timer";
        QTimer::singleShot(THREAD_TIMEOUT, this, SLOT(nextInstruction()));*/
    }

}

void QFStageConfigWidgetThread::move(double x) {
    QFStageConfigWidgetThread::Instruction inst;
    inst.type=QFStageConfigWidgetThread::Move;
    inst.pd1=x;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}


void QFStageConfigWidgetThread::moveRel(double x) {
    QFStageConfigWidgetThread::Instruction inst;
    inst.type=QFStageConfigWidgetThread::MoveRel;
    inst.pd1=x;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}

void QFStageConfigWidgetThread::setJoystick(bool enabled, double maxSpeed) {
    QFStageConfigWidgetThread::Instruction inst;
    inst.type=QFStageConfigWidgetThread::SetJoystick;
    inst.pd1=maxSpeed;
    inst.pb1=enabled;
    InstructionMutex->lock();
    instructions.enqueue(inst);
    InstructionMutex->unlock();
}

void QFStageConfigWidgetThread::stopThread() {
    stopped=true;
    if (isRunning()) wait();
    //qDebug()<<"thread stopped";
}

