#ifndef QFLIGHTSOURCECONFIGWIDGETTHREAD_H
#define QFLIGHTSOURCECONFIGWIDGETTHREAD_H

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
#include "qfextensionlightsource.h"
#include "qfextension.h"
#include "tools.h"
#include "qflightsourcecombobox.h"
#include "filters.h"
#include "libwid_imexport.h"



class QFLightSourceConfigWidget; //forward

/*! \brief thread used by QFLightSourceConfigWidget to control light sources
    \ingroup qf3ext_spimb040


 */
class QFWIDLIB_EXPORT QFLightSourceConfigWidgetThread : public QThread {
        Q_OBJECT
    public:

        explicit QFLightSourceConfigWidgetThread(QFLightSourceConfigWidget *parent = 0);
        ~QFLightSourceConfigWidgetThread();

    public slots:
        void stopThread();
        /** \brief switch the given line on or off. While the widget is NOT locked, this function is blocking until the action completes  */
        void setLineEnabled(int line, bool enabled);
        /** \brief set the given line power. While the widget is NOT locked, this function is blocking until the action completes  */
        void setLinePower(int line, double power);
    signals:
        void linesChanged(QTime time, QList<bool> enabled, QList<double> setValues, QList<double> measuredValues, QStringList powerUnits, QStringList lineNames, QList<bool> widgetsEnabled);
    protected:
        virtual void run();

        void nextInstruction();
        void checkInstructions();

        QFLightSourceConfigWidget* m_parent;
        QMutex* InstructionMutex;
        volatile bool stopped;
        int readCounter;

        enum InstructionType { SetEnabled, SetPower };
        struct Instruction {
            InstructionType type;
            double value;
            bool enabled;
            int line;
        };

        QQueue<Instruction> instructions;
};

#endif // QFLIGHTSOURCECONFIGWIDGETTHREAD_H
