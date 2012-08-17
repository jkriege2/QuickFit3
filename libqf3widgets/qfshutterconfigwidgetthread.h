#ifndef QFSHUTTERCONFIGWIDGETTHREAD_H
#define QFSHUTTERCONFIGWIDGETTHREAD_H

#include <QThread>
#include <QQueue>
#include "libwid_imexport.h"
#include <QMutex>
#include "qfextensionshutter.h"

class QFShutterConfigWidget;

class QFWIDLIB_EXPORT QFShutterConfigWidgetThread : public QThread
{
    Q_OBJECT
public:
    explicit QFShutterConfigWidgetThread(QFShutterConfigWidget *parent = 0);
    QFShutterConfigWidgetThread();

public slots:
    void stopThread();
    /** \brief sets the shutter state of the given shutter */
    void setState(bool state);
signals:
    void stateChanged(bool state);
protected:
    virtual void run();

    void nextInstruction();
    void checkInstructions();

    QFShutterConfigWidget* m_parent;
    QMutex* InstructionMutex;
    volatile bool stopped;
    int readCounter;

    enum InstructionType { SetState };
    struct Instruction {
        InstructionType type;
        bool state;
    };

    QQueue<Instruction> instructions;

};

#endif // QFSHUTTERCONFIGWIDGETTHREAD_H
