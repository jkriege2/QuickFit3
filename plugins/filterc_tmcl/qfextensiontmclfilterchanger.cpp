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

    QMenu* fcMenu=NULL;
    if (count>0) {
        QMenu* extm=services->getMenu("extensions");
        //std::cout<<"extensions menu: "<<extm<<std::endl;
        if (extm) {
            fcMenu=extm->addMenu(QIcon(getIconFilename()), tr("TMCL filter wheels"));
        }

    }
    //qDebug()<<"FILTERWHEEL_COUNT = "<<count;
    for (int i=0; i<count; i++) {
        FILTERWHEEL s;
        s.port=ports.addCOMPort(inifile, "filterwheel"+QString::number(i+1)+"/");
        s.id=inifile.value("filterwheel"+QString::number(i+1)+"/id", 1).toInt();
        s.motor=inifile.value("filterwheel"+QString::number(i+1)+"/axis", 0).toUInt();
        s.filters=inifile.value("filterwheel"+QString::number(i+1)+"/filter_count", 6).toInt();
        s.steps_per_revolution=inifile.value("filterwheel"+QString::number(i+1)+"/steps_per_revolution", 200*16).toULongLong();
        s.fastRFSSpeed=inifile.value("filterwheel"+QString::number(i+1)+"/fast_ref_speed", 1).toULongLong();
        s.maximumAcceleration=inifile.value("filterwheel"+QString::number(i+1)+"/max_acceleration", 3).toULongLong();
        s.maximumCurrent=inifile.value("filterwheel"+QString::number(i+1)+"/max_current", 15).toULongLong();
        s.maximumSpeed=inifile.value("filterwheel"+QString::number(i+1)+"/max_speed", 15).toULongLong();
        s.slowRFSSpeed=inifile.value("filterwheel"+QString::number(i+1)+"/slow_ref_speed", 1).toULongLong();
        s.standbyCurrent=inifile.value("filterwheel"+QString::number(i+1)+"/standby_current", 6).toULongLong();

        QString motor=inifile.value("filterwheel"+QString::number(i+1)+"/motor", "none").toString().toLower();
        if (motor=="pd-108-28" || motor=="pd1-108-28" || motor=="pd3-108-28" || motor=="pdx-108-28") {
            s.steps_per_revolution=200*16;
        }
        s.infoMessage="";
        s.lastAction=QTime::currentTime();
        s.tmcl=new QF3TMCLProtocolHandler(ports.getCOMPort(s.port), getName());
        s.actRealign=new QAction(tr("realign filter changer #%1").arg(i+1), this);
        connect(s.actRealign, SIGNAL(triggered()), this, SLOT(realignFW()));
        s.actRealign->setEnabled(false);
        if (fcMenu) fcMenu->addAction(s.actRealign);
        wheels.append(s);
        //qDebug()<<"FILTERWHEEL("<<i<<") ... id="<<s.id;
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
            delete wheels[i].actRealign;
        }
    }
}

unsigned int QFExtensionTMCLFilterChanger::getFilterChangerCount() {
    return wheels.size();
}

#define TMCL_TESTOK(command, errormessage) \
    ok=ok&&(command);\
    if (!ok) {\
        log_error(QString("      ")+(errormessage)+QString("\n"));\
        ok=false;\
    }

#define TMCL_TESTOK_STATUS(command, errormessage, status_okval) \
    ok=ok&&(command);\
    if (!ok || status!=(status_okval)) {\
    log_error(QString("       ")+(errormessage)+QString("\n"));\
        ok=false;\
    }


void QFExtensionTMCLFilterChanger::filterChangerConnect(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    QF3TMCLProtocolHandler* tmcl=wheels[filterChanger].tmcl;
    if (!com) return;
    com->set_binary(true);
    com->open();
    wheels[filterChanger].actRealign->setEnabled(false);
    if (com->isConnectionOpen()) {
        wheels[filterChanger].actRealign->setEnabled(true);

        QTime t;
        t.start();
        // wait CONNECTION_DELAY_MS ms for connection!
        while (t.elapsed()<CONNECTION_DELAY_MS)  {
            QApplication::processEvents();
        }


        if (!TMCLRealignFW(filterChanger)) {
            com->close();
            log_error(tr("  - connection closed\n"));
            wheels[filterChanger].actRealign->setEnabled(false);
        }

    } else {
        log_error(tr(" Could not connect to TMCL filterwheel [port=%1  baud=%2]!!!\n").arg(com->get_port().c_str()).arg(com->get_baudrate()));
        log_error(tr(" reason: %1\n").arg(wheels[filterChanger].tmcl->getLastError()));
    }
}

void QFExtensionTMCLFilterChanger::filterChangerDisonnect(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return;
    com->close();
    wheels[filterChanger].actRealign->setEnabled(true);

}

void QFExtensionTMCLFilterChanger::setFilterChangerLogging(QFPluginLogService *logService) {
    this->logService=logService;
}


bool QFExtensionTMCLFilterChanger::isFilterChangerConnected(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    if (!com) return false;
    bool res= com->isConnectionOpen();
    wheels[filterChanger].actRealign->setEnabled(res);
    return res;

}

unsigned int QFExtensionTMCLFilterChanger::getFilterChangerFilterCount(unsigned int filterChanger) {
     if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return 0;
     return wheels[filterChanger].filters;
}

void QFExtensionTMCLFilterChanger::setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return ;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    QF3TMCLProtocolHandler* tmcl=wheels[filterChanger].tmcl;
    if (!com || !tmcl) return;
    if (isFilterChangerConnected(filterChanger)) {
        int cfilter=wheels[filterChanger].currentFilter;
        int32_t pos=wheels[filterChanger].steps_per_revolution*filter/wheels[filterChanger].filters;
        int32_t cpos=wheels[filterChanger].steps_per_revolution*cfilter/wheels[filterChanger].filters;
        int32_t halfpos=wheels[filterChanger].steps_per_revolution/2;
        if (cpos!=pos) {
            int32_t d=pos-cpos;
            if (abs(d)<=halfpos) {
                bool ok=true;
                uint8_t status;
                TMCL_TESTOK_STATUS(tmcl->queryStatus(wheels[filterChanger].id, TMCL_MVP, MVP_REL, wheels[filterChanger].motor, d, &status), tr("error moving motor (status=%1)\n").arg(status), 100);
                if (ok) wheels[filterChanger].currentFilter=filter;
            } else {
                if (d>halfpos) {
                    d=d-2*halfpos;
                }
                if (d<-halfpos) {
                    d=d+2*halfpos;
                }
                bool ok=true;
                uint8_t status;
                TMCL_TESTOK_STATUS(tmcl->queryStatus(wheels[filterChanger].id, TMCL_MVP, MVP_REL, wheels[filterChanger].motor, d, &status), tr("error moving motor (status=%1)\n").arg(status), 100);
                if (ok) wheels[filterChanger].currentFilter=filter;
            }

        }
        /*bool ok=true;
        uint8_t status;
        TMCL_TESTOK_STATUS(tmcl->queryStatus(wheels[filterChanger].id, TMCL_MVP, MVP_ABS, wheels[filterChanger].motor, pos, &status), tr("error moving motor (status=%1)\n").arg(status), 100);
        if (ok) wheels[filterChanger].currentFilter=filter;*/
    }

}

unsigned int QFExtensionTMCLFilterChanger::getFilterChangerCurrentFilter(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return 0;
    return wheels[filterChanger].currentFilter;
}

bool QFExtensionTMCLFilterChanger::isLastFilterChangerActionFinished(unsigned int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return true;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    QF3TMCLProtocolHandler* tmcl=wheels[filterChanger].tmcl;
    if (!com || !tmcl) return true;
    bool reached=false;
    if (isFilterChangerConnected(filterChanger)) {
        uint8_t rAddress;
        int32_t value=0;
        uint8_t status;
        // read parameter 8 (target pos. reached) until it is 1
        bool ok=tmcl->queryCommand(wheels[filterChanger].id, TMCL_GAP, 8, wheels[filterChanger].motor, 1, &rAddress, &status, &value);
        if (ok && status==100) {
            reached=(value==0);
        }
    }
    return reached;
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

int QFExtensionTMCLFilterChanger::findActRealign(QAction *realign) {
    for (int i=0; i<wheels.size(); i++) {
        if (wheels[i].actRealign==realign) return i;
    }
    return -1;
}


void QFExtensionTMCLFilterChanger::realignFW() {
    int i=findActRealign(qobject_cast<QAction*>(sender()));
    if (i>=0 && i<wheels.size())  {
        if (isFilterChangerConnected(i)) {
            if (!TMCLRealignFW(i)) {
                QMessageBox::critical(NULL, getName(), tr("Can not realign filter changer.\n reason: error during realignment.").arg(i+1));
            }
        } else {
            QMessageBox::critical(NULL, getName(), tr("Can not realign filter changer.\n reason: Filter changer #%1 is not connected.").arg(i+1));
        }
    }
}




bool QFExtensionTMCLFilterChanger::TMCLRealignFW(int filterChanger) {
    if (filterChanger<0 || filterChanger>=getFilterChangerCount()) return false;
    JKSerialConnection* com=ports.getCOMPort(wheels[filterChanger].port);
    QF3TMCLProtocolHandler* tmcl=wheels[filterChanger].tmcl;
    if (!com) return false;


    QTime t;
    t.start();

    uint8_t status=0;
    bool ok=true;

    // reset position
    wheels[filterChanger].currentFilter=0;

    log_text(tr("  - port: %1   speed: %4   id: %2   axis: %3\n").arg(com->get_port().c_str()).arg(wheels[filterChanger].id).arg(wheels[filterChanger].motor).arg(com->get_baudrate()));
    log_text(tr("  - firmware version: %1\n").arg(tmcl->getFirmwareVersion(wheels[filterChanger].id, wheels[filterChanger].motor)));
    int32_t value=0;
    log_text(tr("  - resetting position\n"));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 1, 0, wheels[filterChanger].motor), tr("error resetting position\n"));
    log_text(tr("  - set fast referencing speed to %1\n").arg(wheels[filterChanger].fastRFSSpeed));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 195, wheels[filterChanger].fastRFSSpeed, wheels[filterChanger].motor), tr("error setting fast referencing speed"));
    log_text(tr("  - set slow referencing speed to %1\n").arg(wheels[filterChanger].slowRFSSpeed));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 194, wheels[filterChanger].slowRFSSpeed, wheels[filterChanger].motor), tr("error setting slow referencing speed"));
    log_text(tr("  - set max. acceleration to %1\n").arg(wheels[filterChanger].maximumAcceleration));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 5, wheels[filterChanger].maximumAcceleration, wheels[filterChanger].motor), tr("error setting max. acceleration"));
    log_text(tr("  - set max. speed to %1\n").arg(wheels[filterChanger].maximumSpeed));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 4, wheels[filterChanger].maximumSpeed, wheels[filterChanger].motor), tr("error setting max. speed"));
    log_text(tr("  - set standby current to %1\n").arg(wheels[filterChanger].standbyCurrent));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 7, wheels[filterChanger].standbyCurrent, wheels[filterChanger].motor), tr("error setting standby current"));
    log_text(tr("  - set maximum current to %1\n").arg(wheels[filterChanger].maximumCurrent));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 6, wheels[filterChanger].maximumCurrent, wheels[filterChanger].motor), tr("error setting maximum current"));
    log_text(tr("  - set acceleration with ramps\n"));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 15, 0, wheels[filterChanger].motor), tr("error setting acceleration with ramps"));

    // move about a third rotation and wait until movement finished
    com->clearBuffer();
    log_text(tr("  - moving motor\n"));
    TMCL_TESTOK_STATUS(tmcl->queryStatus(wheels[filterChanger].id, TMCL_MVP, MVP_REL, wheels[filterChanger].motor, wheels[filterChanger].steps_per_revolution/3, &status), tr("error moving motor (status=%1)\n").arg(status), 100);
    bool reached=false;
    t.start();
    while (ok && !reached) {
        uint8_t rAddress;
        int32_t value=0;
        // read parameter 8 (target pos. reached) until it is 1
        tmcl->queryCommand(wheels[filterChanger].id, TMCL_GAP, 8, wheels[filterChanger].motor, 1, &rAddress, &status, &value);
        if (ok && status==100) {
            reached=((value==0)&&(t.elapsed()>100));
        }

        if (t.elapsed()>MVP_TIMEOUT) {
            log_error(tr("      timeout during move\n"));
            ok=false;
        }
    }
    ok=reached;

    log_text(tr("  - performing reference search\n"));

    // set the correct reference search algorithm (#1 in parameter 193)
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 193, 1, wheels[filterChanger].motor), tr("error setting reference search algorithm (status=%1)\n").arg(status));

    // do a reference queryStatus
    if (ok) {
        TMCL_TESTOK_STATUS(tmcl->queryStatus(wheels[filterChanger].id, TMCL_RFS, 0, wheels[filterChanger].motor, 0, &status), tr("error starting reference search (status=%1)\n").arg(status), 100);
        reached=false;
        t.start();
        while (ok && !reached) {
            uint8_t rAddress;
            int32_t value=0;
            // check whether RFS finished
            tmcl->queryCommand(wheels[filterChanger].id, TMCL_RFS, 2, wheels[filterChanger].motor, 0, &rAddress, &status, &value);
            if (ok && status==100) {
                reached=(value==0)&&(t.elapsed()>100);
            }
            if (t.elapsed()>RFS_TIMEOUT) {
                log_error(tr("      timeout during reference search\n"));
                ok=false;
            }
        }
    }

    // set the encoder position to 0
    log_text(tr("  - reseting position\n"));
    TMCL_TESTOK(tmcl->setAxisParameter(wheels[filterChanger].id, 1, 0, wheels[filterChanger].motor), tr("error resetting position (status=%1)\n").arg(status));
    return ok;
}





Q_EXPORT_PLUGIN2(filterc_tmcl, QFExtensionTMCLFilterChanger)
