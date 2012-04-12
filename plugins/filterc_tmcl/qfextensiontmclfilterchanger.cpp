#include "qfextensiontmclfilterchanger.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

#define LOG_PREFIX "[FilterWheelTMCL]: "
#define GLOBAL_CONFIGFILE "filterwheel_tmcl.ini"
#define CONNECTION_DELAY_MS 1000
#define RFS_TIMEOUT (20*1000)
#define MVP_TIMEOUT (5*1000)

QFExtensionTMCLFilterChanger::QFExtensionTMCLFilterChanger(QObject* parent):
    QObject(parent)
{
	logService=NULL;
}

QFExtensionTMCLFilterChanger::~QFExtensionTMCLFilterChanger() {

}


void QFExtensionTMCLFilterChanger::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionTMCLFilterChanger::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/%1").arg(GLOBAL_CONFIGFILE);
    QSettings inifile(ini, QSettings::IniFormat);
    int count=inifile.value("filterwheel_count", 0).toUInt();
    qDebug()<<"FILTERWHEEL_COUNT = "<<count;
    for (int i=0; i<count; i++) {
        FILTERWHEEL s;
        s.port=ports.addCOMPort(inifile, "filterwheel"+QString::number(i+1)+"/");
        s.id=inifile.value("filterwheel"+QString::number(i+1)+"/id", 1).toInt();
        s.filters=inifile.value("filterwheel"+QString::number(i+1)+"/filter_count", 200*16).toInt();
        s.steps_per_revolution=inifile.value("filterwheel"+QString::number(i+1)+"/steps_per_revolution", 2).toULongLong();
        QString motor=inifile.value("filterwheel"+QString::number(i+1)+"/motor", "none").toString().toLower();
        if (motor=="pd-108-28" || motor=="pd1-108-28" || motor=="pd3-108-28" || motor=="pdx-108-28") {
            s.steps_per_revolution=200*16;
        }
        s.infoMessage="";
        s.lastAction=QTime::currentTime();
        s.tmcl=new QF3TMCLProtocolHandler(ports.getCOMPort(s.port), getName());
        wheels.append(s);
        qDebug()<<"FILTERWHEEL("<<i<<") ... id="<<s.id;
    }
}


void QFExtensionTMCLFilterChanger::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini
	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionTMCLFilterChanger::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionTMCLFilterChanger::log_text(QString message) {
	if (logService) logService->log_text(LOG_PREFIX+message);
	else if (services) services->log_text(LOG_PREFIX+message);
    QApplication::processEvents();
}

void QFExtensionTMCLFilterChanger::log_warning(QString message) {
	if (logService) logService->log_warning(LOG_PREFIX+message);
	else if (services) services->log_warning(LOG_PREFIX+message);
    QApplication::processEvents();
}

void QFExtensionTMCLFilterChanger::log_error(QString message) {
	if (logService) logService->log_error(LOG_PREFIX+message);
    else if (services) services->log_error(LOG_PREFIX+message);
    QApplication::processEvents();
}






void QFExtensionTMCLFilterChanger::deinit() {
    /* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+QString("/%1").arg(GLOBAL_CONFIGFILE), QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("filterwheel_count", getFilterChangerCount());
        for (unsigned int i=0; i<wheels.size(); i++) {
            int p=wheels[i].port;
            if (ports.getCOMPort(p)) {
                ports.storeCOMPort(p, inifile, "filterwheel"+QString::number(i+1)+"/");
                inifile.setValue("filterwheel"+QString::number(i+1)+"/id", wheels[i].id);
            }
        }
    }
}

unsigned int QFExtensionTMCLFilterChanger::getFilterChangerCount() {
    return wheels.size();
}

void QFExtensionTMCLFilterChanger::filterChangerConnect(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    QF3TMCLProtocolHandler* tmcl=wheels[filterChanger].tmcl;
    if (!com) return;
    com->set_binary(true);
    com->open();
    if (com->isConnectionOpen()) {
        QTime t;
        t.start();
        // wait CONNECTION_DELAY_MS ms for connection!
        while (t.elapsed()<CONNECTION_DELAY_MS)  {
            QApplication::processEvents();
        }

        uint8_t status=0;
        bool ok=true;

        // reset position
        log_text(tr("%1  - resetting position\n").arg(LOG_PREFIX));
        if (ok) {
            ok=ok&&tmcl->queryStatus(wheels[filterChanger].id, TMCL_SAP, 1, 0, 0, &status);
            if (!ok || status!=100) {
                log_error(tr("%1 error resetting position (status=%2)\n").arg(LOG_PREFIX).arg(status));
                ok=false;
            }
        }

        // move about a third rotation
        com->clearBuffer();
        log_text(tr("%1  - moving motor\n").arg(LOG_PREFIX));
        ok=ok&&tmcl->queryStatus(wheels[filterChanger].id, TMCL_MVP, MVP_REL, 0, wheels[filterChanger].steps_per_revolution/2, &status);
        if (!ok || status!=100) {
            log_error(tr("%1 error moving motor (status=%2)\n").arg(LOG_PREFIX).arg(status));
            ok=false;
        }
        bool reached=false;
        t.start();
        while (ok && !reached) {
            uint8_t rAddress;
            int32_t value=0;
            // read parameter 8 (target pos. reached) until it is 1
            tmcl->queryCommand(wheels[filterChanger].id, TMCL_GAP, 8, 0, 1, &rAddress, &status, &value);
            if (ok && status==100) {
                reached=((value==0)&&(t.elapsed()>100));
            }
            qDebug()<<"status = "<<status<<"    value = "<<value<<"    reached = "<<reached;

            if (t.elapsed()>MVP_TIMEOUT) {
                log_error(tr("%1 timeout during move\n").arg(LOG_PREFIX));
                ok=false;
            }
        }
        ok=reached;

        // set the correct reference search algorithm (#1 in parameter 193)
        log_text(tr("%1  - performing reference search\n").arg(LOG_PREFIX));
        if (ok) {
            ok=ok&&tmcl->queryStatus(wheels[filterChanger].id, TMCL_SAP, 193, 0, 1, &status);
            if (!ok || status!=100) {
                log_error(tr("%1 error setting reference search algorithm (status=%2)\n").arg(LOG_PREFIX).arg(status));
                ok=false;
            }
        }
        // do a reference queryStatus
        if (ok) {
            ok=ok&&tmcl->queryStatus(wheels[filterChanger].id, TMCL_RFS, 0, 0, 0, &status);
            if (!ok || status!=100) {
                log_error(tr("%1 error doing reference search (status=%2)\n").arg(LOG_PREFIX).arg(status));
                ok=false;
            }
            reached=false;
            t.start();
            while (ok && !reached) {
                uint8_t rAddress;
                int32_t value=0;
                // check whether RFS finished
                if (ok) ok=ok&&tmcl->queryCommand(wheels[filterChanger].id, TMCL_RFS, 2, 0, 0, &rAddress, &status, &value);
                if (!ok || status!=100) {
                    log_error(tr("%1 error reading axis parameter 8 (status=%2)\n").arg(LOG_PREFIX).arg(status));
                    ok=false;
                }
                reached=(value==0)&&(t.elapsed()>100);
                if (t.elapsed()>RFS_TIMEOUT) {
                    log_error(tr("%1 timeout during reference search\n").arg(LOG_PREFIX));
                    ok=false;
                }
            }
        }

        // set the encoder position to 0
        log_text(tr("%1  - reseting position\n").arg(LOG_PREFIX));
        if (ok) {
            ok=ok&&tmcl->queryStatus(wheels[filterChanger].id, TMCL_SAP, 209, 0, 0, &status);
            if (!ok || status!=100) {
                log_error(tr("%1 error setting encoder position (status=%2)\n").arg(LOG_PREFIX).arg(status));
                ok=false;
            }
        }
        if (!ok) {
            com->close();
            log_error(tr("%1 connection closed\n").arg(LOG_PREFIX));
        }

    } else {
        log_error(tr("%1 Could not connect to TMCL filterwheel [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr("%1 reason: %2\n").arg(LOG_PREFIX).arg(wheels[filterChanger].tmcl->getLastError()));
    }
}

void QFExtensionTMCLFilterChanger::filterChangerDisonnect(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return;
    com->close();
}

void QFExtensionTMCLFilterChanger::setFilterChangerLogging(QFPluginLogService *logService) {
    this->logService=logService;
}


bool QFExtensionTMCLFilterChanger::isFilterChangerConnected(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return false;
    return com->isConnectionOpen();
}

unsigned int QFExtensionTMCLFilterChanger::getFilterChangerFilterCount(unsigned int filterChanger) {
     if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return 0;
     return wheels[filterChanger].filters;
}

void QFExtensionTMCLFilterChanger::setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) {
}

unsigned int QFExtensionTMCLFilterChanger::getFilterChangerCurrentFilter(unsigned int filterChanger) {
    return 0;
}

bool QFExtensionTMCLFilterChanger::isLastFilterChangerActionFinished(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return true;
    return wheels[filterChanger].lastAction.elapsed()>100;
}

QString QFExtensionTMCLFilterChanger::getFilterChangerDescription(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return "";
    return tr("TMCL controller filter wheel");
}

QString QFExtensionTMCLFilterChanger::getFilterChangerShortName(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return "";
    return tr("TMCL filter wheel");
}

void QFExtensionTMCLFilterChanger::showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget *parent) {
}



Q_EXPORT_PLUGIN2(filterc_tmcl, QFExtensionTMCLFilterChanger)
