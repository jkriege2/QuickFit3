#ifndef QFESPIMB040SAMPLESTAGECONFIGTHREAD_H
#define QFESPIMB040SAMPLESTAGECONFIGTHREAD_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include "qfextensionlinearstage.h"


class QFESPIMB040SampleStageConfig; // forward

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): thread that reads three stages and sends signals for the current states
    \ingroup qf3ext_spimb040


 */
class QFESPIMB040SampleStageConfigThread: public QThread {
        Q_OBJECT
    public:
        QFESPIMB040SampleStageConfigThread(QFESPIMB040SampleStageConfig* parent);
        ~QFESPIMB040SampleStageConfigThread();
        void run();
        void move(double x, double y, double z);
        void moveRel(double x, double y, double z);
        void setJoystick(bool enabled, double maxSpeed);
        /** \brief stop the thread and block until it is stopped! */
        void stopThread();
        bool anyConnected() const;
    public slots:
        void start(Priority priority = InheritPriority );
    signals:
        void stageXMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void stageYMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void stageZMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void joystickStateChanged(bool enabled);
        void stagesConnectedChanged(bool connX, bool connY, bool connZ);
    protected:
        QFESPIMB040SampleStageConfig* m_parent;
        QMutex* InstructionMutex;
        bool stopped;

        enum InstructionType { Move, MoveRel, SetJoystick };
        struct Instruction {
            InstructionType type;
            double pd1;
            double pd2;
            double pd3;
            bool pb1;
        };

        QQueue<Instruction> instructions;

        int readCounter;
        bool connX;
        bool connY;
        bool connZ;
    protected slots:
        void nextInstruction();

};

#endif // QFESPIMB040SAMPLESTAGECONFIGTHREAD_H
