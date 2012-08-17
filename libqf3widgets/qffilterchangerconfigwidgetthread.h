#ifndef QFFILTERCHANGERCONFIGWIDGETTHREAD_H
#define QFFILTERCHANGERCONFIGWIDGETTHREAD_H

#include <QThread>
#include <QQueue>
#include "libwid_imexport.h"
#include <QMutex>
#include "qfextensionfilterchanger.h"

class QFFilterChangerConfigWidget;

class QFWIDLIB_EXPORT QFFilterChangerConfigWidgetThread : public QThread
{
    Q_OBJECT
public:
    explicit QFFilterChangerConfigWidgetThread(QFFilterChangerConfigWidget *parent = 0);
    QFFilterChangerConfigWidgetThread();

public slots:
    void stopThread();
    /** \brief sets the shutter state of the given shutter */
    void setFilter(int filter);
signals:
    void filterChanged(int filter);
protected:
    virtual void run();

    void nextInstruction();
    void checkInstructions();

    QFFilterChangerConfigWidget* m_parent;
    QMutex* InstructionMutex;
    volatile bool stopped;
    int readCounter;

    enum InstructionType { SetFilter };
    struct Instruction {
        InstructionType type;
        int filter;
    };

    int lastState;

    QQueue<Instruction> instructions;

};

#endif // QFFILTERCHANGERCONFIGWIDGETTHREAD_H
