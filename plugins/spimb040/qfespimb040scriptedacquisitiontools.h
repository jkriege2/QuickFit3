/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
        explicit QFESPIMB040ScriptedAcquisitionTools(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QObject* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription);


    public slots:
        void logText(const QString& text);
        void logError(const QString& text);
        void logWarning(const QString& text);
        void setStatus(const QString& text);
        void sleepMS(int duration);
        void createTextFile(const QString& filename);
        void appendTextFile(const QString& filename, const QString& data);
        void showMessageBox(const QString& text);
    private:
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetupBase* opticsSetup;
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
        explicit QFESPIMB040ScriptedAcquisitionInstrumentControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QObject* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription);

    public slots:
        void setLaserIntensity(int laser, int line, double intensity);
        void setBrightfieldIntensity(int lightsource, int line, double intensity);
        void setShutter(const QString& shutter, bool state);
        void moveStageRel(const QString& stage, double delta);
        void moveStageAbs(const QString& stage, double new_position);
        QVariant getStagePos(const QString& stage);
        void setFilterWheel(const QString& wheel, int filter);

        void setAcqDescCurrentSample(const QString& name);
        void setAcqDescPrefix(const QString& name);
        void setAcqDescPlate(int p);
        void setAcqDescWell(int p);
        void setAcqDescCell(int p);
        void setAcqDescComment(const QString& name);

        int getAcqDescCell() const;
        int getAcqDescWell() const;
        int getAcqDescPlate() const;
        QString getAcqDescComment() const;
        QString getAcqDescCurrentSample() const;
        QString getAcqDescPrefix() const;

        void MDConnect(const QString& device_name, int id);
        void MDDisconnect(const QString& device_name, int id);
        QVariant MDGet(const QString& device_name, int id, int parameter);
        void MDSet(const QString& device_name, int id, int parameter, const QVariant& value);
        QVariant MDGetN(const QString& device_name, int id, const QString &parameter);
        void MDSetN(const QString& device_name, int id, const QString & parameter, const QVariant& value);
        int MDGetParamCount(const QString& device_name, int id);
        QString MDGetParamName(const QString& device_name, int id, int parameter);

    private:
        QFExtensionMeasurementAndControlDevice* getMDevice(const QString& device_name);
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetupBase* opticsSetup;
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
        explicit QFESPIMB040ScriptedAcquisitionAcquisitionControl(QFESPIMB040ScriptedAcquisition* widAcquisition, QFESPIMB040MainWindow2* mainWindow, QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QObject* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription);

    public slots:
        void doAcquisition();
        void doImageStack();
        void acquirePreviewFrames(int camera, int N, const QString& filename);
        void acquirePreviewFramesWithParams(int camera, int N, const QString& filename);

        void setAcquisitionConfigIndex(int camera, int id);
        void setAcquisitionConfig(int camera, const QString& id);
        void setAcquisitionOverviewConfigIndex(int camera, int overview, int id);
        void setAcquisitionOverviewConfig(int camera, int overview, const QString& id);


        void setPreviewConfigIndex(int camera, int id);
        void setPreviewConfig(int camera, const QString& id);
        QStringList getPreviewConfigs(int camera);
        int getPreviewConfigsCount(int camera);
        QString getPreviewConfig(int camera, int id);
        void clearPreviewParams();
        void setPreviewParam(const QString& name, const QVariant& value);
        void clearAcquisitionCamera1Settings();
        void setAcquisitionCamera1Setting(const QString which, QVariant value);
        QVariant getAcquisitionCamera1Setting(const QString which);

        void setAcquisitionCamera1SettingN(const QString which, QVariant value);
        QVariant getAcquisitionCamera1SettingN(const QString which);

        void clearAcquisitionCamera2Settings();
        void setAcquisitionCamera2Setting(const QString which, QVariant value);
        QVariant getAcquisitionCamera2Setting(const QString which);

        void setAcquisitionCamera2SettingN(const QString which, QVariant value);
        QVariant getAcquisitionCamera2SettingN(const QString which);

    private:
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetupBase* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;
        QFESPIMB040MainWindow2* mainWindow;
        QFESPIMB040ScriptedAcquisition* widAcquisition;

        QMap<QString, QVariant> preview_params;
};

#endif // QFESPIMB040SCRTIPTEDACQUISITIONTOOLS_H
