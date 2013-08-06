#ifndef QFStageConfigWidgetTHREAD_H
#define QFStageConfigWidgetTHREAD_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include "qfextensionlinearstage.h"
#include "libwid_imexport.h"

class QFStageConfigWidget; // forward

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): thread that reads three stages and sends signals for the current states
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFStageConfigWidgetThread: public QThread {
        Q_OBJECT
    public:
        QFStageConfigWidgetThread(QFStageConfigWidget* parent);
        ~QFStageConfigWidgetThread();
        void run();
        void move(double x);
        void moveRel(double x);
        void setJoystick(bool enabled, double maxSpeed);
        /** \brief stop the thread and block until it is stopped! */
        void stopThread();
        bool anyConnected() const;
    public slots:
        void start(Priority priority = InheritPriority );
    signals:
        void stageXMoved(QFExtensionLinearStage::AxisState state, double position, double velocity);
        void joystickStateChanged(bool enabled);
        void stagesConnectedChanged(bool connX);
    protected:
        QFStageConfigWidget* m_parent;
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
    protected slots:
        void nextInstruction();

};

#endif // QFStageConfigWidgetTHREAD_H
