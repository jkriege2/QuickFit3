#ifndef QFMeasurementDeviceConfigWidgetThread_H
#define QFMeasurementDeviceConfigWidgetThread_H

#include <QWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QComboBox>
#include <QSettings>
#include <QMenu>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <cmath>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QTime>
#include <QTabWidget>
#include <QGroupBox>
#include <QTimer>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include "programoptions.h"
#include "jkqttools.h"
#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "qfextensionmeasurementdevice.h"
#include "qfextension.h"
#include "tools.h"
#include "qfmeasurementdevicecombobox.h"
#include "filters.h"
#include "libwid_imexport.h"



class QFMeasurementDeviceConfigWidget; //forward

/*! \brief thread used by QFMeasurementDeviceConfigWidget to control measurement devices
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFMeasurementDeviceConfigWidgetThread : public QThread {
        Q_OBJECT
    public:

        explicit QFMeasurementDeviceConfigWidgetThread(QFMeasurementDeviceConfigWidget *parent = 0);
        ~QFMeasurementDeviceConfigWidgetThread();

    public slots:
        void stopThread();
        void setValue(int line, const QVariant& value);
    signals:
        void valuesChanged(QTime time, QList<QVariant> values, QStringList lineNames, QList<bool> widgetsEnabled);
    protected:
        virtual void run();

        void nextInstruction();
        void checkInstructions();

        QFMeasurementDeviceConfigWidget* m_parent;
        QMutex* InstructionMutex;
        volatile bool stopped;
        int readCounter;

        enum InstructionType { SetValue };
        struct Instruction {
            InstructionType type;
            QVariant value;
            int line;
        };

        QQueue<Instruction> instructions;
};

#endif // QFMeasurementDeviceConfigWidgetThread_H
