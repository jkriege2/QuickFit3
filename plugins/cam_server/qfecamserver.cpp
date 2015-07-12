/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfecamserver.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfplugin.h"
#include <iostream>


#define LOG_PREFIX QString("cam_server >>> ").toUpper()
#define CAMSERVER_ACTION_DURATION 1
#define GLOBAL_CONFIGFILE "cam_server.ini"
#define CONNECTION_DELAY_MS 200


QFECamServer::QFECamServer(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFECamServer::~QFECamServer() {

}


void QFECamServer::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */

    Q_UNUSED(project);
    Q_UNUSED(oldProject);

}

void QFECamServer::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    QFPluginServices::getInstance()->log_global_text(tr("%1loading INI-file %2\n").arg(QString(LOG_PREFIX)).arg(ini));
    QSettings inifile(ini, QSettings::IniFormat);
    int count=inifile.value("camera_count", 0).toUInt();
    for (int i=0; i<count; i++) {
        DEVICE_CONFIG s;
        s.lastAction=QTime::currentTime();
        s.host=inifile.value("camera"+QString::number(i+1)+"/host", "127.0.0.1").toString();
        s.port=inifile.value("camera"+QString::number(i+1)+"/port", 512341).toInt();
        s.camera_name=inifile.value("camera"+QString::number(i+1)+"/camera_name", QString("cam_server %1:%2").arg(s.host).arg(s.port)).toString();
        s.sensor_name=inifile.value("camera"+QString::number(i+1)+"/sensor_name", QString("???")).toString();
        s.answer_bits=inifile.value("camera"+QString::number(i+1)+"/answer_bits", 16).toInt();
        s.timeout_connection=inifile.value("camera"+QString::number(i+1)+"/timeout_connection", 10000).toInt();
        s.timeout_instruction=inifile.value("camera"+QString::number(i+1)+"/timeout_instruction", 5000).toInt();
        s.pixel_width=inifile.value("camera"+QString::number(i+1)+"/pixel_width", 24).toDouble();
        s.pixel_height=inifile.value("camera"+QString::number(i+1)+"/pixel_height", 24).toDouble();
        s.hasInstSingleParameterGet=inifile.value("camera"+QString::number(i+1)+"/has_single_parameter_get", true).toBool();
        s.hasInstProgress=inifile.value("camera"+QString::number(i+1)+"/has_progress_instruction", true).toBool();
        s.hasInstCancel=inifile.value("camera"+QString::number(i+1)+"/has_cancel_instruction", false).toBool();
        s.exposure=1;

        s.server=new QTcpSocket();
        s.mutex=new QMutex(QMutex::Recursive);
        sources.append(s);
    }
}


void QFECamServer::deinit() {
    /* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE), QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("camera_count", getCameraCount());
        for (int i=0; i<sources.size(); i++) {
            inifile.setValue("camera"+QString::number(i+1)+"/host", sources[i].host);
            inifile.setValue("camera"+QString::number(i+1)+"/port", sources[i].port);
            inifile.setValue("camera"+QString::number(i+1)+"/camera_name", sources[i].camera_name);
            inifile.setValue("camera"+QString::number(i+1)+"/sensor_name", sources[i].sensor_name);
            inifile.setValue("camera"+QString::number(i+1)+"/answer_bits", sources[i].answer_bits);
            inifile.setValue("camera"+QString::number(i+1)+"/timeout_connection", sources[i].timeout_connection);
            inifile.setValue("camera"+QString::number(i+1)+"/timeout_instruction", sources[i].timeout_instruction);
            inifile.setValue("camera"+QString::number(i+1)+"/pixel_width", sources[i].pixel_width);
            inifile.setValue("camera"+QString::number(i+1)+"/pixel_height", sources[i].pixel_height);
            inifile.setValue("camera"+QString::number(i+1)+"/has_single_parameter_get", sources[i].hasInstSingleParameterGet);
            inifile.setValue("camera"+QString::number(i+1)+"/has_progress_instruction", sources[i].hasInstProgress);
            inifile.setValue("camera"+QString::number(i+1)+"/has_cancel_instruction", sources[i].hasInstCancel);

        }
    }
}

void QFECamServer::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFECamServer::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int QFECamServer::getMeasurementDeviceCount()
{
    return getCameraCount();
}

void QFECamServer::showMeasurementDeviceSettingsDialog(unsigned int /*measurementDevice*/, QWidget *parent)
{
    QMessageBox::information(parent, tr("Andor camera driver"), tr("there is no configuration dialog for the camera as measurement device!"));
}

bool QFECamServer::isMeasurementDeviceConnected(unsigned int measurementDevice)
{
    return isCameraConnected(measurementDevice);

}

void QFECamServer::connectMeasurementDevice(unsigned int measurementDevice)
{
    connectCameraDevice(measurementDevice);
}

void QFECamServer::disconnectMeasurementDevice(unsigned int /*measurementDevice*/)
{
    //disconnectCameraDevice(measurementDevice);
}

QString QFECamServer::getMeasurementDeviceName(unsigned int measurementDevice)
{
    return getCameraName(measurementDevice);
}

void QFECamServer::setMeasurementDeviceLogging(QFPluginLogService *logService)
{
    setCameraLogging(logService);
}

unsigned int QFECamServer::getMeasurementDeviceValueCount(unsigned int measurementDevice)
{
    if (/*measurementDevice>=0 && */measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        return sources[measurementDevice].params.size();
    }
    return 0;

}

QVariant QFECamServer::getMeasurementDeviceValue(unsigned int measurementDevice, unsigned int value)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 &&*/ value<(long long)sources[measurementDevice].params.size()) {
            QTcpSocket* server=sources[measurementDevice].server;
            if (!server) return QVariant::Invalid;

            QMutex* mutex=sources[measurementDevice].mutex;
            QMutexLocker locker(mutex);

            if (sources[measurementDevice].hasInstSingleParameterGet) {
                bool ok=false;
                QList<QByteArray> bal=queryData(sources[measurementDevice], QByteArray("PARAMETER_GET\n")+sources[measurementDevice].params[value].id.toLatin1()+QByteArray("\n\n"), 1, "\n\n", &ok, true);
                if (ok && bal.size()>0) {
                    bal=bal[0].split('\n');
                    for (int i=0; i<bal.size(); i++) {
                        //qDebug()<<bal[i];
                        QList<QByteArray> a=bal[i].split(';');
                        if (a.size()>2) {
                            if (a[1]==sources[measurementDevice].params[value].id) {
                                //qDebug()<<QString(bal[i]);
                                //qDebug()<<value<<": "<<QString(a[1])<<" = "<<QString(a[2]);
                                QVariant v=a[2];
                                if (v.convert(sources[measurementDevice].params[value].type)) {
                                    //qDebug()<<"   -> "<<v;
                                    return v;
                                }
                                return a[2];
                            }
                        }
                    }
                    log_error(tr("error reading parameter '%2' (timeout: %1s): PARAMETER NOT AVAILABLE IN DEVICE\n\n").arg(double(sources[measurementDevice].timeout_connection)/1000.0).arg(sources[measurementDevice].params[value].id));
                } else  {
                    log_error(tr("error reading parameter '%4' (timeout: %3s): ERROR DURING PARAMETER_GET\n%1     error description: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[measurementDevice].timeout_connection)/1000.0).arg(sources[measurementDevice].params[value].id));
                }
            } else {
                bool ok=false;
                QList<QByteArray> bal=queryData(sources[measurementDevice], "PARAMETERS_GET\n\n", 1, "\n\n", &ok, true);
                if (ok && bal.size()>0) {
                    //qDebug()<<bal;
                    bal=bal[0].split('\n');
                    for (int i=0; i<bal.size(); i++) {
                        QList<QByteArray> a=bal[i].split(';');
                        if (a.size()>2) {
                            if (a[1]==sources[measurementDevice].params[value].id) {
                                //qDebug()<<QString(bal[i]);
                                //qDebug()<<value<<": "<<QString(a[1])<<" = "<<QString(a[2]);
                                QVariant v=a[2];
                                if (v.convert(sources[measurementDevice].params[value].type)) {
                                    //qDebug()<<"   -> "<<v;
                                    return v;
                                }
                                return a[2];
                            }
                        }
                    }
                    log_error(tr("error reading parameter from all parameters '%2' (timeout: %1s): PARAMETER NOT AVAILABLE IN DEVICE\n\n").arg(double(sources[measurementDevice].timeout_connection)/1000.0).arg(sources[measurementDevice].params[value].id));
                } else  {
                    log_error(tr("error reading parameter from all parameters '%4' (timeout: %3s):\n%1     error description: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[measurementDevice].timeout_connection)/1000.0).arg(sources[measurementDevice].params[value].id));
                }
            }
        } else {
            log_error(tr("error reading parameter %2 (timeout: %1s): PARAMETER NOT AVAILABLE IN DEVICE\n\n").arg(double(sources[measurementDevice].timeout_connection)/1000.0).arg(value));
        }
    }
    return QVariant();
}

QString QFECamServer::getMeasurementDeviceValueName(unsigned int measurementDevice, unsigned int value)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 &&*/ value<(long long)sources[measurementDevice].params.size()) {
            return sources[measurementDevice].params[value].description;
        }
    }
    return QString();
}

QString QFECamServer::getMeasurementDeviceValueShortName(unsigned int measurementDevice, unsigned int value)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 && */value<(long long)sources[measurementDevice].params.size()) {
            return sources[measurementDevice].params[value].id;
        }
    }
    return QString();
}

bool QFECamServer::isMeasurementDeviceValueEditable(unsigned int measurementDevice, unsigned int value)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 && */value<(long long)sources[measurementDevice].params.size()) {
            return sources[measurementDevice].params[value].editable;
        }

    }
    return false;
}

void QFECamServer::setMeasurementDeviceValue(unsigned int measurementDevice, unsigned int value, const QVariant &data)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 &&*/ value<(long long)sources[measurementDevice].params.size() && sources[measurementDevice].params[value].editable) {
            QTcpSocket* server=sources[measurementDevice].server;
            if (!server) return ;

            QMutex* mutex=sources[measurementDevice].mutex;
            QMutexLocker locker(mutex);

            bool ok=sendCommand(sources[measurementDevice], QByteArray("PARAMETERS_SET\n")+sources[measurementDevice].params[value].id.toLatin1()+QByteArray(";")+getQVariantData(data).toLatin1()+QByteArray("\n\n"));
            if (!ok) {
                log_error(tr("error setting parameter '%4' (timeout: %3s):\n%1     new value:         '%5'!\n%1     error description: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[measurementDevice].timeout_connection)/1000.0).arg(sources[measurementDevice].params[value].id).arg(getQVariantData(data)));
            }

        } else {
            log_error(tr("error setting parameter '%4' (timeout: %3s):\n%1     new value:         '%5'!\n%1     error description: %2!\n\n").arg(LOG_PREFIX).arg(tr("parameter not writeable or not found")).arg(double(sources[measurementDevice].timeout_connection)/1000.0).arg(sources[measurementDevice].params[value].id).arg(getQVariantData(data)));
        }
    }
}

QVariant::Type QFECamServer::getMeasurementDeviceEditableValueType(unsigned int measurementDevice, unsigned int value)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 &&*/ value<(long long)sources[measurementDevice].params.size()) {
            return sources[measurementDevice].params[value].type;
        }

    }
    return QVariant::Invalid;
}

QFExtensionMeasurementAndControlDevice::WidgetTypes QFECamServer::getMeasurementDeviceValueWidget(unsigned int measurementDevice, unsigned int value, QStringList */*comboboxEntries*/)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 &&*/ value<(long long)sources[measurementDevice].params.size()) {
            return sources[measurementDevice].params[value].widget;
        }
    }
    return QFExtensionMeasurementAndControlDevice::mdDefault;
}

void QFECamServer::getMeasurementDeviceEditableValueRange(unsigned int measurementDevice, unsigned int value, double &minimum, double &maximum)
{
    if (/*measurementDevice>=0 &&*/ measurementDevice<getMeasurementDeviceCount() && isMeasurementDeviceConnected(measurementDevice)) {
        if (/*value>=0 &&*/ value<(long long)sources[measurementDevice].params.size()) {
            minimum=sources[measurementDevice].params[value].range_min;
            maximum=sources[measurementDevice].params[value].range_max;
        }
    }
}

unsigned int QFECamServer::getCameraCount() const {
    return sources.size();
	/* how man cameras may be accessed by your plugin (e.g. if you use one driver to access several cameras */
}

void QFECamServer::useCameraSettings(unsigned int camera, const QSettings& settings) {
    useCameraSettingsInt(camera, settings, true);
}

void QFECamServer::useCameraSettingsInt(unsigned int camera, const QSettings& settings, bool live_view, bool waitToComplete) {
    /* set the camera settings to the values specified in settings parameter, called before acquire() */
    //qDebug()<<"useCameraSetings: "<<camera;
    //for (int i=0; i<settings.allKeys().size(); i++) {
    //    //qDebug()<<"   "<<settings.allKeys().value(i, "")<<" = "<<settings.value(settings.allKeys().value(i, ""));
    //}
    if (logService) {
        logService->log_text(QString("\n         -- useCameraSetings: %1\n").arg(camera));
    }
    if (/*camera<0 ||*/ camera>=getCameraCount()) return;
    if (isMeasurementDeviceConnected(camera) ) {
        int cnt=0;
        for (unsigned int i=0; i<getMeasurementDeviceValueCount(camera); i++) {
            if (sources[camera].params[i].editable && settings.contains(sources[camera].params[i].id)) {
                //qDebug()<<"set value "<<sources[camera].params[i].id<<"("<<i<<")"<<" = "<<settings.value(sources[camera].params[i].id);
                if (logService) {
                    logService->log_text(QString("         -- setValue: %1 (%2) = %3\n").arg(sources[camera].params[i].id).arg(i).arg(settings.value(sources[camera].params[i].id).toString()));
                }
                setMeasurementDeviceValue(camera, i, settings.value(sources[camera].params[i].id));
                cnt++;
            }
        }
        if (cnt>0 && waitToComplete)  {
            if (logService) {
                logService->log_text(QString("\n         -- useCameraSetings: %1: 2s DELAY\n").arg(camera));
            }
            QElapsedTimer timer;
            timer.start();
            while (timer.elapsed()<2000) {
                QApplication::processEvents();
            }
        }
        if (live_view) {
            log_text(tr("starting live-view mode!\n"));
            bool ok=sendCommand(sources[camera], "LIVE_START\n\n");
            ok=waitForString(sources[camera], "ACK_LIVE_START\n\n");

        }
        if (logService) {
            logService->log_text(QString("\n         -- useCameraSetings: %1: DONE!!!\n").arg(camera));
        }
    }
}



void QFECamServer::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/

    if (/*camera<0 ||*/ camera>=getCameraCount() || !isMeasurementDeviceConnected(camera) ) return;

    useCameraSettingsInt(camera, settings, false, false);

	/////////////////////////////////////////////////////////////////////////////////
	// if you want the settings dialog to be modal, you may uncomment the next lines
	// and add implementations
	/////////////////////////////////////////////////////////////////////////////////
	QDialog* dlg=new QDialog(parent);
    dlg->setWindowTitle(tr("camera settings for %1 (sensor: %2)").arg(getCameraName(camera)).arg(getCameraSensorName(camera)));

    QVBoxLayout* lay=new QVBoxLayout();
    dlg->setLayout(lay);


    QFMeasurementDeviceConfigWidget* wid=new  QFMeasurementDeviceConfigWidget(dlg, false);
    wid->init(logService, QFPluginServices::getInstance());
    wid->getMeasurementDeviceComboBox()->setCurrentItem(this, camera);
    wid->connectMeasurementDevice();
    wid->disableDeviceCombobox();
    lay->addWidget(wid);


    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed()<2000) {
        QApplication::processEvents();
    }
    if ( dlg->exec()==QDialog::Accepted ) {
        //  read back values entered into the widgets and store in settings
        // settings.setValue(QString("device/name%1").arg(camera), widget->value() );
        for (unsigned int i=0; i<getMeasurementDeviceValueCount(camera); i++) {
            settings.setValue(getMeasurementDeviceValueShortName(camera, i), getMeasurementDeviceValue(camera, i));
        }
    }
    delete dlg;
}



bool QFECamServer::connectCameraDevice(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return false;
    QTcpSocket* server=sources[camera].server;
    if (!server) return false;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    if (!server->isOpen() || (server->state()!=QAbstractSocket::ConnectedState) ) {
        log_text(tr("connecting to camera server '%1', port: %2..\n").arg(sources[camera].host).arg(sources[camera].port));
        server->connectToHost(sources[camera].host, sources[camera].port);
        if (server->waitForConnected(sources[camera].timeout_connection)) {
            log_text(tr("TCP/IP connection established!\n"));
            sources[camera].connected_timer.start();
            sources[camera].acquiring=false;
            bool ok=true;
            QString lastOp=tr("CONNECT TO SENSOR");
            log_text(tr("connecting to camera!\n"));
            sendCommand(sources[camera], "CONNECT\n\n");
            lastOp=tr("CONNECT TO SENSOR: WAIT ACK");
            ok=waitForString(sources[camera], "ACK_CONNECT\n\n");
            if (ok) {
                lastOp=tr("START LIVE-VIEW");
                log_text(tr("starting live-view mode!\n"));
                sendCommand(sources[camera], "LIVE_START\n\n");
                lastOp=tr("START LIVE-VIEW: WAIT ACK");
                ok=waitForString(sources[camera], "ACK_LIVE_START\n\n");
            }
            QString pnames;
            if (ok) {
                sources[camera].params.clear();
                lastOp=tr("GET PARAMETERS");
                log_text(tr("reading available parameters!\n"));
                queryData(sources[camera], "PARAMETERS_GET\n\n");
                lastOp=tr("GET PARAMETERS: WAIT FOR ANSWER");
                QByteArray res=readString(sources[camera], "\n\n", &ok);
                if (ok) {
                    QList<QByteArray> params=res.split('\n');

                    for (int i=0; i<params.size(); i++) {
                        //qDebug()<<params[i];
                        QList<QByteArray> p= params[i].split(';');
                        if (p.size()>=7) {
                            QFECamServer::CAM_PARAM pa;
                            pa.id=p[1];
                            pa.description=pa.id;
                            if (!p[3].isEmpty()) pa.description=pa.description+QString(" (")+p[3]+QString(")");
                            pa.editable=(p[6].toUpper()=="RW");
                            pa.range_min=-DBL_MAX;
                            pa.range_max=DBL_MAX;
                            pa.type=QVariant::Invalid;
                            pa.widget=QFExtensionMeasurementAndControlDevice::mdDefault;
                            bool pok=true;

                            if (p[0].toUpper()=="PARAM_INT") {
                                pa.type=QVariant::Int;
                                pa.widget=QFExtensionMeasurementAndControlDevice::mdIntEdit;
                                bool iok=false;
                                if (!p[4].isEmpty()) pa.range_min=p[4].toDouble(&iok);
                                if (!iok) pa.range_min=-INT_MAX;
                                iok=false;
                                if (!p[5].isEmpty()) pa.range_max=p[5].toDouble(&iok);
                                if (!iok) pa.range_min=INT_MAX;
                            } else if (p[0].toUpper()=="PARAM_FLOAT") {
                                pa.type=QVariant::Double;
                                pa.widget=QFExtensionMeasurementAndControlDevice::mdDoubleEdit;
                                bool iok=false;
                                if (!p[4].isEmpty()) pa.range_min=p[4].toDouble(&iok);
                                if (!iok) pa.range_min=-DBL_MAX;
                                iok=false;
                                if (!p[5].isEmpty()) pa.range_max=p[5].toDouble(&iok);
                                if (!iok) pa.range_min=DBL_MAX;
                            } else if (p[0].toUpper()=="PARAM_BOOL") {
                                pa.type=QVariant::Bool;
                                pa.widget=QFExtensionMeasurementAndControlDevice::mdCheckBox;
                                bool iok=false;
                                pa.range_min=0;
                                pa.range_max=1;
                                if (pa.id.toUpper()=="HAS_INSTRUCTION_PROGRESS") {
                                    sources[camera].hasInstProgress=QStringToBool(p[2]);
                                    pok=false;
                                } else if (pa.id.toUpper()=="HAS_INSTRUCTION_CANCEL_ACQUISITION") {
                                    sources[camera].hasInstCancel=QStringToBool(p[2]);
                                    pok=false;
                                }

                            } else if (p[0].toUpper()=="PARAM_STRING") {
                                pa.type=QVariant::String;
                                pa.widget=QFExtensionMeasurementAndControlDevice::mdLineEdit;
                            } else {
                                log_error(tr("      -> unkown parameter type '%1' for parameter '%2' (description: %3").arg(QString(p[0].toUpper())).arg(QString(p[1])).arg(QString(p[3])));
                                pok=false;
                            }
                            if (pok) {
                                sources[camera].params.append(pa);
                                if (!pnames.isEmpty()) pnames+=", ";
                                pnames+=sources[camera].params.last().id;
                                if (!pa.editable) pnames+="[RO]";
                            }
                        }
                    }
                }
            }
            if (ok) {
                log_text(tr("   peer name:     %1\n").arg(server->peerName()));
                log_text(tr("   peer adress:   %1\n").arg(server->peerAddress().toString()));
                log_text(tr("   peer port:     %1\n").arg(server->peerPort()));
                log_text(tr("   local adress:  %1\n").arg(server->localAddress().toString()));
                log_text(tr("   local port:    %1\n").arg(server->localPort()));
                log_text(tr("   camera name:   %1\n").arg(getCameraName(camera)));
                log_text(tr("   sensor name:   %1\n").arg(getCameraSensorName(camera)));
                log_text(tr("   frame size:    %1 x %2\n").arg(getCameraImageWidth(camera)).arg(getCameraImageHeight(camera)));
                log_text(tr("   exposure time: %1 ms\n").arg(getCameraExposureTime(camera)*1000.0));
                log_text(tr("   pixel size:    %1 x %2 micrometer^2\n").arg(getCameraPixelWidth(camera)).arg(getCameraPixelHeight(camera)));
                log_text(tr("   Parameters:    %1: %2\n\n").arg(sources[camera].params.size()).arg(pnames));
            }
            if (!ok) {
                log_error(tr("error connecting (timeout: %3s) during operation %4:\n%1     error description: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_connection)/1000.0).arg(lastOp));
                server->close();
            }
            return ok;
        } else {
            log_error(tr("error connecting (timeout: %3s):\n%1     error description: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_connection)/1000.0));
            server->close();
            return false;
        }
    } else {
        return true;
    }
    return false;
}

void QFECamServer::disconnectCameraDevice(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return ;
    QTcpSocket* server=sources[camera].server;
    if (!server) return ;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    if (server->isOpen() && (server->state()==QAbstractSocket::ConnectedState) ) {
        QString lastOp=tr("STOP LIVE-VIEW");
        log_text(tr("stopping live-view mode!\n"));
        sendCommand(sources[camera], "LIVE_STOP\n\n");
        waitForString(sources[camera], "ACK_LIVE_STOP\n\n");
        lastOp=tr("DISCONNECT FROM SENSOR");
        log_text(tr("disconnecting from camera!\n"));
        sendCommand(sources[camera], "DISCONNECT\n\n");
        waitForString(sources[camera], "ACK_DISCONNECT\n\n");
        log_text(tr("disconnecting from camera server '%2', port: %3..\n").arg(sources[camera].host).arg(sources[camera].port));
        server->disconnectFromHost();
        server->close();
        log_text(tr("connection closed!\n"));
    }
    log_text(tr("disconnecting from camera server '%2', port: %3..\n").arg(sources[camera].host).arg(sources[camera].port));
    server->disconnectFromHost();
    server->close();
    log_text(tr("connection closed!\n"));
}

bool QFECamServer::isCameraConnected(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return false;
    QTcpSocket* server=sources[camera].server;
    if (!server) return false;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    return server->isOpen() && (server->state()==QAbstractSocket::ConnectedState);
}

bool QFECamServer::acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* timestamp, QMap<QString, QVariant> *parameters) {
    if (timestamp!=NULL) {
        *timestamp=sources[camera].connected_timer.elapsed();
    }

    if (/*camera<0 ||*/ camera>=getCameraCount()) return 0;
    QTcpSocket* server=sources[camera].server;
    if (!server) return 0;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    bool ok=false;

    int oldW=sources[camera].width;
    int oldH=sources[camera].height;
    int newW=oldW;
    int newH=oldH;
    double expos=sources[camera].exposure;

    if (server->isOpen() && (server->state()==QAbstractSocket::ConnectedState)) {
        //flushBuffers(sources[camera], true);
        ok=sendCommand(sources[camera], "IMAGE_NEXT_GET\n\n");
        if (ok) {
            QByteArray answer=readString(sources[camera], "\n", &ok, true);
            //qDebug()<<"AW: "<<answer<<answer.startsWith("IMAGE");
            if (answer.startsWith("IMAGE")) {
                QByteArray answer1=readString(sources[camera], "\n", &ok, true);
                //qDebug()<<"AW1: "<<answer1<<ok;
                QByteArray answer2=readString(sources[camera], "\n", &ok, true);
                //qDebug()<<"AW2: "<<answer2<<ok;
                //QByteArray answer3=readString(sources[camera], "\n", &ok, true);
                //qDebug()<<"AW3: "<<answer3<<ok;
                if (ok) {
                    newW=answer1.toInt(&ok);
                    if (ok) newH=answer2.toInt(&ok);
                    //if (ok) expos=answer3.toDouble(&ok);
                    //qDebug()<<"CONV:  "<<ok<<newW<<newH<<expos;
                    if (ok) {
                        sources[camera].width=newW;
                        sources[camera].height=newH;
                        int answ_bits=sources[camera].answer_bits;
                        if (answer.startsWith("IMAGE8")) answ_bits=8;
                        if (answer.startsWith("IMAGE16")) answ_bits=16;
                        if (answer.startsWith("IMAGE32")) answ_bits=32;
                        if (answer.startsWith("IMAGE64")) answ_bits=64;
                        //qDebug()<<"READ_IMAGE_ANSWER: "<<answer<<newW<<newH<<answ_bits<<oldW<<oldH;
                        memset(data, 0, oldW*oldH*sizeof(uint32_t));
                        if (answ_bits==8) {
                            QByteArray temp=readData(sources[camera], newW*newH*sizeof(uint8_t), &ok);
                            //qDebug()<<"READ_8BIT:  "<<ok<<temp.size();
                            if (ok) {
                                const uint8_t* d=(const uint8_t*)temp.constData();
                                for (int y=0; y<oldH; y++) {
                                    for (int x=0; x<oldW; x++) {
                                        if (x<newW && y<newH) data[y*oldW+x]=d[y*newW+x];
                                        else data[y*oldW+x]=0;
                                    }
                                }
                            }
                        } else if (answ_bits==16) {
                            QByteArray temp=readData(sources[camera], newW*newH*sizeof(uint16_t), &ok);
                            //qDebug()<<"READ_16BIT:  "<<ok<<temp.size();
                            if (ok) {
                                const uint16_t* d=(const uint16_t*)temp.constData();
                                for (int y=0; y<oldH; y++) {
                                    for (int x=0; x<oldW; x++) {
                                        if (x<newW && y<newH) data[y*oldW+x]=d[y*newW+x];
                                        else data[y*oldW+x]=0;
                                    }
                                }
                            }
                        } else if (answ_bits==32) {
                            QByteArray temp=readData(sources[camera], newW*newH*sizeof(uint32_t), &ok);
                            //qDebug()<<"READ_32BIT:  "<<ok<<temp.size();
                            if (ok) {
                                const uint32_t* d=(const uint32_t*)temp.constData();
                                for (int y=0; y<oldH; y++) {
                                    for (int x=0; x<oldW; x++) {
                                        if (x<newW && y<newH) data[y*oldW+x]=d[y*newW+x];
                                        else data[y*oldW+x]=0;
                                    }
                                }
                            }
                        } else if (answ_bits==64) {
                            QByteArray temp=readData(sources[camera], newW*newH*sizeof(uint64_t), &ok);
                            //qDebug()<<"READ_64BIT:  "<<ok<<temp.size();
                            if (ok) {
                                const uint64_t* d=(const uint64_t*)temp.constData();
                                for (int y=0; y<oldH; y++) {
                                    for (int x=0; x<oldW; x++) {
                                        if (x<newW && y<newH) data[y*oldW+x]=d[y*newW+x];
                                        else data[y*oldW+x]=0;
                                    }
                                }
                            }
                        }
                        QByteArray params_in=readString(sources[camera], "\n\n", &ok, true);
                        QList<QByteArray> params=params_in.split('\n');
                        for (int i=0; i<params.size(); i++) {
                            QString p=params[i];
                            QStringList f=p.split(";");
                            if (f.size()>=3) {
                                bool ok=false;
                                QString n=f[1];
                                QString d=f[2];
                                QString desc=f.value(3);
                                QString t=f[0].toUpper();
                                QVariant vv;
                                if (t=="PARAM_FLOAT") {
                                    vv=QStringToDouble(d);
                                    if (n.toUpper()=="EXPOSURE") {
                                        sources[camera].exposure=vv.toDouble();
                                        if (parameters) (*parameters)["exposure_time"]=vv;
                                    } else if (n.toUpper()=="DURATION") {
                                        if (parameters) (*parameters)["duration_milliseconds"]=vv.toDouble()*1000.0;
                                    }
                                } else if (t=="PARAM_INT") {
                                    vv=d.toInt(&ok);
                                } else if (t=="PARAM_BOOL") {
                                    vv=QStringToBool(d);
                                } else if (t=="PARAM_STRING") {
                                    vv=d;
                                }
                                if (parameters) (*parameters)[n]=vv;
                            }
                        }
                        if (parameters) {
                            (*parameters)["image_width"]=newW;
                            (*parameters)["image_height"]=newH;
                            (*parameters)["pixel_width"]=getCameraPixelWidth(camera);
                            (*parameters)["pixel_height"]=getCameraPixelWidth(camera);
                        }
                        //server->flush();
                        return true;
                    } else {
                        log_error(tr("error reading frame (timeout: %3s):\n%1     error description: COULD NOT REEAD COMPLETE RESULT RECORD FROM HOST!\n%1     last error string: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_instruction)/1000.0));
                    }
                } else {
                    log_error(tr("error reading frame (timeout: %3s):\n%1     error description: COULD NOT REEAD COMPLETE RESULT RECORD FROM HOST!\n%1     last error string: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_instruction)/1000.0));
                }
            } else {
                log_error(tr("error reading frame (timeout: %3s):\n%1     error description: DID NOT GET \"IMAGE\" RECORD FROM HOST!\n%1     last error string: %2!n%1     answer was: \"%4\"!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_instruction)/1000.0).arg(QString(answer)));
            }
        } else {
            log_error(tr("error reading frame (timeout: %3s):\n%1     error description: COULD NOT SEND COMMAND!\n%1     last error string: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_instruction)/1000.0));
        }
    } else {
        log_error(tr("error reading frame (timeout: %3s):\n%1     error description: NOT CONNECTED!\n\n").arg(LOG_PREFIX));
    }

    return false;
}

int QFECamServer::getCameraImageWidth(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return 0;
    QTcpSocket* server=sources[camera].server;
    if (!server) return 0;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    bool ok=false;
    int val=sources[camera].width;
    QList<QByteArray> bal=queryData(sources[camera], "SIZE_X_GET\n\n", 1, "\n\n", &ok, true);
    if (ok && bal.size()>0) {
        ok=false;
        val=bal.first().toInt(&ok);
        if (val>=0) sources[camera].width=val;
    }


    return sources[camera].width;
}

int QFECamServer::getCameraImageHeight(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return 0;
    QTcpSocket* server=sources[camera].server;
    if (!server) return 0;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    bool ok=false;
    int val=sources[camera].height;
    QList<QByteArray> bal=queryData(sources[camera], "SIZE_Y_GET\n\n", 1, "\n\n", &ok, true);
    if (ok && bal.size()>0) {
        ok=false;
        val=bal.first().toInt(&ok);
        if (val>=0) sources[camera].height=val;
    }


    return sources[camera].height;
}

double QFECamServer::getCameraPixelWidth(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return 0;

    return sources[camera].pixel_width;
}

double QFECamServer::getCameraPixelHeight(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return 0;

    return sources[camera].pixel_height;
}


 QString QFECamServer::getCameraName(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return "";

    return sources[camera].camera_name;
}

 QString QFECamServer::getCameraSensorName(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return "";

    return sources[camera].sensor_name;
}

double QFECamServer::getCameraExposureTime(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return 0;
    /*QTcpSocket* server=sources[camera].server;
    if (!server) return 0;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    bool ok=false;
    double val=sources[camera].exposure;
    QList<QByteArray> bal=queryData(sources[camera], "GET_EXPOSURE\n\n", 1, "\n\n", &ok, true);
    if (ok && bal.size()>0) {
        ok=false;
        val=bal.first().toDouble(&ok);
        if (val>=0) sources[camera].exposure=val;
    }*/


    return sources[camera].exposure;
}


bool QFECamServer::prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return false;
    useCameraSettingsInt(camera, settings, false);
    sources[camera].last_filenameprefix=filenamePrefix.toLocal8Bit();
    return true;
}

bool QFECamServer::startCameraAcquisition(unsigned int camera) {
    //RECORD\n<filename>\n
    if (/*camera<0 ||*/ camera>=getCameraCount()) return false;
    QTcpSocket* server=sources[camera].server;
    if (!server) return false;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    QElapsedTimer timer;
    timer.start();
    log_text(tr("stopping live-view mode!\n"));
    bool ok=sendCommand(sources[camera], "LIVE_STOP\n\n");
    ok=waitForString(sources[camera], "ACK_LIVE_STOP\n\n");
    if (ok) {
        while (timer.elapsed()<5000) {
            QApplication::processEvents();
        }

        //if (sendCommand(sources[camera], QByteArray("RECORD\n"+sources[camera].last_filenameprefix+"\n\n"))) {
        QList<QByteArray> answerl=queryData(sources[camera], QByteArray("RECORD\n"+sources[camera].last_filenameprefix+"\n\n"), 1, "\n\n", &ok);
        QByteArray answer="";
        if (answerl.size()>0) answer=answerl.first();

        if (ok) {
            sources[camera].lastfiles=answer;
            //qDebug()<<answer;
            //if (waitForString(sources[camera], "ACK_RECORD\n\n" )) {
            sources[camera].acquiring=true;
            return true;
            //} else {
            //    log_error(tr("error starting acquisiton (timeout: %3s):\n%1     error description: DID NOT GET ACK_RECCORD FROM HOST!\n%1     last error string: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_instruction)/1000.0));
            //}
        } else {
            log_error(tr("error starting acquisiton (timeout: %3s):\n%1     error description: COULD NOT SEND RECORD COMMAND TO HOST!\n%1     last error string: %2!\n%1     answer: %4!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_instruction)/1000.0).arg(QString(answer)));
        }
    } else {
        log_error(tr("error starting acquisiton (timeout: %3s):\n%1     error description: ERROR WHILE SWITCHING OFF THE LIVE-VIEW!\n%1     last error string: %2!\n\n").arg(LOG_PREFIX).arg(server->errorString()).arg(double(sources[camera].timeout_instruction)/1000.0));
    }

    return false;
}

void QFECamServer::cancelCameraAcquisition(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return ;
    QTcpSocket* server=sources[camera].server;
    if (!server) return ;
    if (!sources[camera].hasInstCancel) return;

    if (!isCameraAcquisitionRunning(camera)) return;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    bool ok=sendCommand(sources[camera], "CANCEL_ACQUISITION\n\n");
    ok=waitForString(sources[camera], "ACK_CANCEL_ACQUISITION\n\n");
}

bool QFECamServer::isCameraAcquisitionRunning(unsigned int camera) {
    if (/*camera<0 ||*/ camera>=getCameraCount()) return false;
    QTcpSocket* server=sources[camera].server;
    if (!server) return false;


    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);


    if (sources[camera].acquiring) {
        QList<QByteArray> answerl=queryData(sources[camera], "PARAMETER_GET\nACQ_RUNNING\n\n", 1, "\n\n");
        QByteArray answer="";
        if (answerl.size()>0) {
            answer=answerl.first();
            QVariant p=extractParameter(answer, "ACQ_RUNNING");
            if (p.isValid() && p.canConvert(QVariant::Bool)) {
                sources[camera].acquiring= p.toBool();
            }
        }

    }



    return sources[camera].acquiring;
}

void QFECamServer::getCameraAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::CameraAcquititonFileDescription> *files, QMap<QString, QVariant> *parameters)
{
    if (/*camera<0 ||*/ camera>=getCameraCount()) return ;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    if (!sources[camera].lastfiles.isEmpty()) {
        QList<QByteArray> params=sources[camera].lastfiles.split('\n');
        for (int i=0; i<params.size(); i++) {
            QString p=params[i];
            QStringList f=p.split(";");
            if (f.size()>=3) {
                bool ok=false;
                QString n=f[1];
                QString d=f[2];
                QString desc=f.value(3);
                QString t=f[0].toUpper();
                QVariant vv;
                //qDebug()<<f<<n<<d<<desc;
                if (t=="PARAM_FLOAT") {
                    vv=QStringToDouble(d);
                    if (n.toUpper()=="EXPOSURE") {
                        sources[camera].exposure=vv.toDouble();
                        if (parameters) (*parameters)["exposure_time"]=vv;
                    } else if (n.toUpper()=="DURATION") {
                        if (parameters) (*parameters)["duration_milliseconds"]=vv.toDouble()*1000.0;
                    }
                } else if (t=="PARAM_INT") {
                    vv=d.toInt(&ok);
                    if (n.toUpper()=="EXPOSURE") {
                        sources[camera].exposure=vv.toDouble();
                        if (parameters) (*parameters)["exposure_time"]=vv;
                    } else if (n.toUpper()=="DURATION") {
                        if (parameters) (*parameters)["duration_milliseconds"]=vv.toDouble()*1000.0;
                    }
                } else if (t=="PARAM_BOOL") {
                    vv=QStringToBool(d);
                } else if (t=="PARAM_STRING") {
                    vv=d;
                } else if (t=="FILE") {
                    if (files)  {
                        QFExtensionCamera::CameraAcquititonFileDescription fi;
                        fi.type=n;
                        fi.name=d;
                        fi.description=desc;
                        files->append(fi);
                    }
                    vv=QVariant();
                }
                if (vv.isValid() && parameters) (*parameters)[n]=vv;
            }
        }
        if (parameters) {
            (*parameters)["pixel_width"]=getCameraPixelWidth(camera);
            (*parameters)["pixel_height"]=getCameraPixelWidth(camera);
        }
    }

}


bool QFECamServer::getCameraAcquisitionPreview(unsigned int /*camera*/, uint32_t* /*data*/) {
    return false;
}

int QFECamServer::getCameraAcquisitionProgress(unsigned int camera) {

    if (/*camera<0 ||*/ camera>=getCameraCount()) return 0;
    if (!sources[camera].hasInstProgress ) return 0;
    QTcpSocket* server=sources[camera].server;
    if (!server) return 0;

    QMutex* mutex=sources[camera].mutex;
    QMutexLocker locker(mutex);

    if (sources[camera].acquiring) {
        QList<QByteArray> answerl=queryData(sources[camera], "PARAMETER_GET\nPROGRESS\n\n", 1, "\n\n");
        QByteArray answer="";
        if (answerl.size()>0) {
            answer=answerl.first();
            QVariant p=extractParameter(answer, "PROGRESS");
            if (p.isValid() && p.canConvert(QVariant::Int)) {
                return p.toInt();
            }
        }
    }

    return 0; // return a number between 0 and 100 which indicates the progress of a currently running acquisition
}

bool QFECamServer::isCameraSettingChangable(QFExtensionCamera::CameraSetting which)  {
    return (which==QFExtensionCamera::CamSetNumberFrames);
}

void QFECamServer::changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value)  {  
    if (which==QFExtensionCamera::CamSetNumberFrames) {
        settings.setValue("FRAMES", value);
    }
}

QVariant QFECamServer::getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which)   {
    if (which==QFExtensionCamera::CamSetNumberFrames) {
        return settings.value("FRAMES");
    }
    return QVariant();
}

QVariant QFECamServer::getCameraCurrentSetting(int camera, QFExtensionCamera::CameraSetting which)
{
    if (which==QFExtensionCamera::CamSetNumberFrames) {
        for (int i=0; i<sources[camera].params.size(); i++) {
            if (sources[camera].params[i].id=="FRAMES") return getMeasurementDeviceValue(camera, i);
        }
    }
    return QVariant();
}

bool QFECamServer::isCameraSettingChangable(const QString &which)
{
    int camera=0;
    if (camera>=0 && camera<sources.size() ) {
        for (int i=0; i<sources[camera].params.size(); i++) {
            if (sources[camera].params[i].id==which) return sources[camera].params[i].editable;
        }
        logService->log_error(tr("\ndid not find parameter %1\n").arg(which));
    }
    return false;
}

void QFECamServer::changeCameraSetting(QSettings &settings, const QString &which, QVariant value)
{
    settings.setValue(which, value);
}

QVariant QFECamServer::getCameraSetting(QSettings &settings, const QString &which)
{
    return settings.value(which);
}

QVariant QFECamServer::getCameraCurrentSetting(int camera, const QString &which)
{
    if (camera>=0 && camera<sources.size() ) {
        for (int i=0; i<sources[camera].params.size(); i++) {
            if (sources[camera].params[i].id==which) return getMeasurementDeviceValue(camera, i);
        }
        logService->log_error(tr("\ndid not find parameter %1\n").arg(which));
    }
    return QVariant();
}

void QFECamServer::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
}

void QFECamServer::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
}

void QFECamServer::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
}

bool QFECamServer::flushBuffers(const  QFECamServer::DEVICE_CONFIG& device, bool read_all)
{
    if (device.server && device.server->isOpen() && (device.server->state()==QAbstractSocket::ConnectedState)) {
        if (read_all) device.server->readAll();
        device.server->flush();
    }
    return false;

}

bool QFECamServer::sendCommand(const  QFECamServer::DEVICE_CONFIG& device, const QByteArray &command)
{
    if (device.server && device.server->isOpen()&& (device.server->state()==QAbstractSocket::ConnectedState)) {
        device.server->write(command);
        return true; //device.server->waitForBytesWritten(device.timeout_instruction);
    }
    return false;
}

QByteArray QFECamServer::readString(const  QFECamServer::DEVICE_CONFIG& device, const QByteArray &end_string, bool *ok_out, bool cleanResult)
{
    QByteArray res;
    bool ok=false;
    if (device.server && device.server->isOpen()&& (device.server->state()==QAbstractSocket::ConnectedState)) {
        //res=device.server->readLine();
        //ok=res.size()>0;
        int i=0;
        ok=true;
        bool done=false;
        QElapsedTimer timer;
        timer.start();
        while (!done && ok) {
            ok=(device.server->bytesAvailable()>0) || device.server->waitForReadyRead(device.timeout_instruction);
            char dat;
            if (device.server->read(&dat, 1)==1){
                res.append(dat);
                done=res.endsWith(end_string);
                if (done && cleanResult) {
                    res=res.left(res.size()-end_string.size());
                }
                i++;
            } else {
                ok=false;
            }
            if (timer.elapsed()>device.timeout_instruction) {
                ok=false;
            }
        }
    }
    if (ok_out) *ok_out=ok;
    return res;

}

QByteArray QFECamServer::readData(const  QFECamServer::DEVICE_CONFIG& device, int bytes_to_read, bool *ok_out)
{
    QByteArray res;
    bool ok=false;
    if (device.server && device.server->isOpen()&& (device.server->state()==QAbstractSocket::ConnectedState)) {
        //res=device.server->read(bytes_to_read);
        //ok=res.size()==bytes_to_read;
        int i=0;
        ok=true;
        QElapsedTimer timer;
        timer.start();
        while (i<bytes_to_read && ok) {
            ok=(device.server->bytesAvailable()>0) || device.server->waitForReadyRead(device.timeout_instruction);
            char dat;
            if (device.server->read(&dat, 1)==1){
                res.append(dat);
                i++;
            } else {
                ok=false;
            }
            if (timer.elapsed()>device.timeout_instruction) {
                ok=false;
            }
        }
    }
    if (ok_out) *ok_out=ok;
    return res;

}

bool QFECamServer::waitForString(const QFECamServer::DEVICE_CONFIG &device, const QByteArray &end_string)
{
    bool ok=false;
    if (device.server && device.server->isOpen()&& (device.server->state()==QAbstractSocket::ConnectedState)) {
        //res=device.server->read(bytes_to_read);
        //ok=res.size()==bytes_to_read;
        int i=0;
        ok=true;
        bool done=false;
        QByteArray res;
        QElapsedTimer timer;
        timer.start();
        while (!done && ok) {
            ok=(device.server->bytesAvailable()>0) || device.server->waitForReadyRead(device.timeout_instruction);
            char dat;
            if (device.server->read(&dat, 1)==1){
                res.append(dat);
                done=res.endsWith(end_string);
                i++;
            } else {
                ok=false;
            }
            if (timer.elapsed()>device.timeout_instruction) {
                ok=false;
            }
        }
    }
    return ok;
}

QList<QByteArray> QFECamServer::queryData(const  QFECamServer::DEVICE_CONFIG& device, const QByteArray &command, int line_to_read, const QByteArray &end_string, bool *ok_out, bool cleanResult)
{
    QList<QByteArray> res;
    bool ok=false;
    if (device.server && device.server->isOpen()&& (device.server->state()==QAbstractSocket::ConnectedState)) {
        ok=true;
        if (sendCommand(device, command)) {
            int i=0;
            QElapsedTimer timer;
            timer.start();
            while (i<line_to_read && ok) {
                QByteArray r=readString(device, end_string, &ok, cleanResult);
                if (ok) {
                    res.append(r);
                    i++;
                }
                if (timer.elapsed()>device.timeout_instruction) {
                    ok=false;
                }
            }
        } else {
            ok=false;
        }
    }
    if (ok_out) *ok_out=ok;
    return res;
}

QVariant QFECamServer::extractParameter(const QByteArray &input, const QByteArray &parameter_name)
{
    QList<QByteArray> params=input.split('\n');
    for (int i=0; i<params.size(); i++) {
        QString p=params[i];
        QStringList f=p.split(";");
        if (f.size()>=3) {
            QString n=f[1];
            if (n==parameter_name) {
                bool ok=false;
                QString d=f[2];
                QString t=f[0].toUpper();
                //qDebug()<<f<<n<<d<<desc;
                if (t=="PARAM_FLOAT") {
                    return QStringToDouble(d);
                } else if (t=="PARAM_INT") {
                    int ir=d.toInt(&ok);
                    if (ok) return ir; else return QVariant();
                } else if (t=="PARAM_BOOL") {
                    return QStringToBool(d);
                } else if (t=="PARAM_STRING") {
                    return d;
                }
            }
        }
    }
    return QVariant();
}

QVariant QFECamServer::extractParameter(const QList<QByteArray> &input, const QByteArray &parameter_name)
{
    if (input.size()>0) return extractParameter(input.first(), parameter_name);
    return QVariant();
}


Q_EXPORT_PLUGIN2(cam_server, QFECamServer)
