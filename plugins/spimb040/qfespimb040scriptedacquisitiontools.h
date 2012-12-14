#ifndef QFESPIMB040SCRTIPTEDACQUISITIONTOOLS_H
#define QFESPIMB040SCRTIPTEDACQUISITIONTOOLS_H

#include <QObject>

#include <QWidget>
#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QtScript>
#include "../interfaces/qfextensionlinearstage.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040filenametool.h"
#include "qtriple.h"
#include "qfespimb040acquisitiontools.h"


class QFESPIMB040MainWindow2; // forward
class QFPluginServices; // forward
class QFESPIMB040ScriptedAcquisition; // forward

class QFESPIMB040ScriptedAcquisitionToolsBase {
    public:
        QFESPIMB040ScriptedAcquisitionToolsBase() {
            engine=NULL;
        }
        void setEngine(QScriptEngine* engine) { this->engine=engine; }
        void throwEngineException(const QString& message) {
            if (engine) engine->currentContext()->throwError(message);
        }

    protected:
        QScriptEngine* engine;

};



class QFESPIMB040ScriptedAcquisitionTools : public QObject, public QFESPIMB040ScriptedAcquisitionToolsBase {
        Q_OBJECT
    public:
        explicit QFESPIMB040ScriptedAcquisitionTools(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QObject* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription);


    public slots:
        void logText(const QString& text);
        void logError(const QString& text);
        void logWarning(const QString& text);
        void setStatus(const QString& text);
        void sleepMS(int duration);

    private:
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetup* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;
        QFESPIMB040MainWindow2* mainWindow;
        QFESPIMB040ScriptedAcquisition* widAcquisition;
};



class QFESPIMB040ScriptedAcquisitionInstrumentControl : public QObject, public QFESPIMB040ScriptedAcquisitionToolsBase {
        Q_OBJECT
    public:
        explicit QFESPIMB040ScriptedAcquisitionInstrumentControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QObject* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription);

    public slots:
        void setLaserIntensity(int laser, int line, double intensity);
        void setTransmissionIntensity(int line, double intensity);
        void setShutter(const QString& shutter, bool state);
        void moveStageRel(const QString& stage, double delta);
        void moveStageAbs(const QString& stage, double new_position);
        void setFilterWheel(const QString& wheel, int filter);

    private:
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetup* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;
        QFESPIMB040MainWindow2* mainWindow;
        QFESPIMB040ScriptedAcquisition* widAcquisition;
};




class QFESPIMB040ScriptedAcquisitionAcquisitionControl : public QObject, public QFESPIMB040ScriptedAcquisitionToolsBase {
        Q_OBJECT
    public:
        explicit QFESPIMB040ScriptedAcquisitionAcquisitionControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QObject* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription);

    public slots:
        void doAcquisition();
        void doImageStack();
        void setAcquisitionCamera1Setting(const QString which, QVariant value);
        void clearAcquisitionCamera1Settings();
        QVariant getAcquisitionCamera1Setting(const QString which);

        void setAcquisitionCamera2Setting(const QString which, QVariant value);
        void clearAcquisitionCamera2Settings();
        QVariant getAcquisitionCamera2Setting(const QString which);

    private:
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetup* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;
        QFESPIMB040MainWindow2* mainWindow;
        QFESPIMB040ScriptedAcquisition* widAcquisition;
};

#endif // QFESPIMB040SCRTIPTEDACQUISITIONTOOLS_H
