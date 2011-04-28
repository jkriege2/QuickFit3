#include "stage_pi.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

QFExtensionLinearStagePI::QFExtensionLinearStagePI(QObject* parent):
    QObject(parent)
{
	logService=NULL;
    PTerm=150;
    iTerm=45;
    DTerm=300;
    iLimit=2000;
    acceleration=1000000;
    maxVelocity=2000;
    initVelocity=1000;
    COMPort="COM1";
    COMPortSpeed=38400;
    maxJoystickVelocity=100; // micron/sed
    currentID=-1;
    lengthFactor=6.9e-3;
    velocityFactor=6.9e-3;
    accelerationFactor=6.9e-3;
}

QFExtensionLinearStagePI::~QFExtensionLinearStagePI() {

}


void QFExtensionLinearStagePI::deinit() {
	/* add code for cleanup here */
    QSettings inifile(services->getConfigFileDirectory()+"/stage_pi863.ini", QSettings::IniFormat);
    inifile.setValue("driver/port", COMPort);
    inifile.setValue("driver/port_speed", COMPortSpeed);
    inifile.setValue("driver/pterm", PTerm);
    inifile.setValue("driver/iterm", iTerm);
    inifile.setValue("driver/dterm", DTerm);
    inifile.setValue("driver/ilimit", iLimit);
    inifile.setValue("driver/acceleration", acceleration);
    inifile.setValue("driver/initvelocity", initVelocity);
    inifile.setValue("driver/maxvelocity", maxVelocity);
    inifile.setValue("driver/maxjoystickvelocity", maxJoystickVelocity);


    for (int i=0; i<=axes.size(); i++) {
        QString axisname=axes[i].name;
        inifile.setValue(axisname+"id", QString(QChar(axes[i].ID)));
    }
}

void QFExtensionLinearStagePI::projectChanged(QFProject* oldProject, QFProject* project) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionLinearStagePI::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QSettings inifile(services->getConfigFileDirectory()+"/stage_pi863.ini", QSettings::IniFormat);
    COMPort=inifile.value("driver/port", COMPort).toString();
    COMPortSpeed=inifile.value("driver/port_speed", COMPortSpeed).toUInt();
    PTerm=inifile.value("driver/pterm", PTerm).toUInt();
    iTerm=inifile.value("driver/iterm", iTerm).toUInt();
    DTerm=inifile.value("driver/dterm", DTerm).toUInt();
    iLimit=inifile.value("driver/ilimit", iLimit).toUInt();
    acceleration=inifile.value("driver/acceleration", acceleration).toDouble();
    initVelocity=inifile.value("driver/initvelocity", initVelocity).toDouble();
    maxVelocity=inifile.value("driver/maxvelocity", maxVelocity).toDouble();
    maxJoystickVelocity=inifile.value("driver/maxjoystickvelocity", maxJoystickVelocity).toDouble();


    axes.clear();
    for (int i=0; i<=15; i++) {
        QString axisname=QString("axis%1/").arg(i);
        if (inifile.contains(axisname+"id")) {
            AxisDescription d;
            d.ID=inifile.value(axisname+"id", '0').toChar();
            d.velocity=initVelocity;
            d.joystickEnabled=false;
            d.state=QFExtensionLinearStage::Disconnected;
            d.name=axisname;
            axes.append(d);
        }
    }
}

void QFExtensionLinearStagePI::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionLinearStagePI::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/extensions/<ID>/<ID>.ini file
	// QSettings settings(QApplication::applicationDirPath()+"/plugins/extensions/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int QFExtensionLinearStagePI::getAxisCount() {
	return axes.size();
}

void QFExtensionLinearStagePI::showSettingsDialog(QWidget* parent) {
	/* open a dialog that configures the camera.

	   usually you should display a modal QDialog descendent which writes back config when the user clicks OK

	   alternatively you may also display a window which stays open and allows the suer to set settings also
	   during the measurement.
	*/


	/////////////////////////////////////////////////////////////////////////////////
	// if you want the settings dialog to be modal, you may uncomment the next lines
	// and add implementations
	/////////////////////////////////////////////////////////////////////////////////
    /*
	QDialog* dlg=new QDialog(parent);

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QFormLayout* formlayout=new QFormLayout(dlg);


    //  create your widgets here, do not to initialize them with the current settings
    // QWidget* widget=new QWidget(dlg);
    // lay->addRow(tr("Name"), widget);


    lay->addLayout(formlayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
         //  read back values entered into the widgets and store in settings
    }
    delete dlg;
	*/
}

void QFExtensionLinearStagePI::selectAxis(int i) {
    if ((i>=0)&&(i<axes.size())) {
        QChar IDx=axes[i].ID;
        if (currentID!=IDx) {
            //std::cout<<"selecting motor "<<IDx<<"\n";
            std::string s(2, IDx.toLatin1());
            s[0]=1;
            com.write(s);
            //com.write(s);
            checkComError();
            queryCommand("TB");
            currentID=IDx;
        }
    }
}

void QFExtensionLinearStagePI::sendCommand(std::string command) {
    com.write(command+"\x0D");
}

std::string QFExtensionLinearStagePI::queryCommand(std::string command) {
    std::string res="";
    //std::cout<<"\n\ncommand (stage "<<currentID<<"): '"<<command<<"'";
    com.clearBuffer();
    if (com.write(command+"\x0D")) {
        res=com.readUntil(3);
        //std::cout<<" ... reading ... ";
    }
    //std::cout<<"   direct_result: '"<<toprintablestr(res)<<"' ";
    checkComError();
    if (res.size()>=3) res=res.erase(res.size()-3, 3);
    //std::cout<<"   returned_result: '"<<toprintablestr(res)<<"'\n\n";
    return res;
}

void QFExtensionLinearStagePI::checkComError() {
    if (com.hasErrorOccured()) {
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Error;
            log_error(tr(">> PI863:  serial port error: %1").arg(com.getLastError().c_str()));
        }
    }
}

bool QFExtensionLinearStagePI::checkComConnected() {
    bool c=com.isConnectionOpen();
    if (!c) {
        log_error(tr(">> PI863:  Could not connect to Mercury C-863 Motor Controller!!!"));
        log_error(tr(">> PI863:  reason: %1").arg(com.getLastError().c_str()));
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Error;
        }
    }
    return c;
}

bool QFExtensionLinearStagePI::isConnected(unsigned int axis) {
    return com.isConnectionOpen();
}

void QFExtensionLinearStagePI::connectDevice(unsigned int axis) {
    com.set_port(COMPort.toStdString());
    com.set_baudrate(COMPortSpeed);
    com.set_stopbits(JKSConeStopbit);
    com.set_parity(JKSCnoParity);
    com.set_databits(JKSC8databits);
    com.open();
    if (com.isConnectionOpen()) {
        for (int i=0; i<axes.size(); i++) {
            selectAxis(i);
            selectAxis(i);
            queryCommand("VE");
            sendCommand("DP"+inttostr(PTerm));
            queryCommand("GP");
            sendCommand("DI"+inttostr(iTerm));
            sendCommand("DD"+inttostr(DTerm));
            sendCommand("DL"+inttostr(iLimit));
            sendCommand("SA"+inttostr(acceleration));
            sendCommand("MN");
            sendCommand("DH");
            axes[i].velocity=initVelocity;
            axes[i].joystickEnabled=false;
        }
        currentID=-1;
        if (com.hasErrorOccured()) {
            for (int i=0; i<axes.size(); i++) {
                axes[i].state=QFExtensionLinearStage::Error;
            }
        } else {
            for (int i=0; i<axes.size(); i++) {
                axes[i].state=QFExtensionLinearStage::Ready;
            }
        }
    } else {
        log_error(tr(">> PI863:  Could not connect to Mercury C-863 Motor Controller!!!"));
        log_error(tr(">> PI863:  reason: %1").arg(com.getLastError().c_str()));
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Disconnected;
        }
    }
}

void QFExtensionLinearStagePI::disconnectDevice(unsigned int axis) {
    com.close();
    for (int i=0; i<axes.size(); i++) {
        axes[i].state=QFExtensionLinearStage::Disconnected;
    }
    currentID=-1;
}

void QFExtensionLinearStagePI::setLogging(QFPluginLogService* logService) {
	this->logService=logService;
}

void QFExtensionLinearStagePI::setJoystickActive(unsigned int axis, bool enabled, double maxVelocity) {
    if (enabled) {
        selectAxis(axis);
        sendCommand("JN"+inttostr((long)round(maxJoystickVelocity/velocityFactor)));
        axes[axis].joystickEnabled=true;
    } else {
        selectAxis(axis);
        sendCommand("JF");
        axes[axis].joystickEnabled=false;
    }
}

bool QFExtensionLinearStagePI::isJoystickActive(unsigned int axis) {
	return axes[axis].joystickEnabled;
}

QFExtensionLinearStage::AxisState QFExtensionLinearStagePI::getAxisState(unsigned int axis)  {

    std::string failedaxes="";
    if (com.isConnectionOpen()) {
        selectAxis(axis);
        std::string r=queryCommand("TS")+"\n";
        int B1=0, B2=0, B3=0, B4=0, B5=0, B6=0;
        if (!com.hasErrorOccured()) {
            if (sscanf(r.c_str(), "S:%x %x %x %x %x %x", &B1, &B2, &B3, &B4, &B5, &B6)) {
                //std::cout<<"\n\n"<<std::string(1, (char)('x'+i-1))<<": "<<r<<"\n\n";
                if (B6==1) {
                    log_error(tr(">> PI863:  error on axis %1: RS-232 timeout").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==2) {
                    log_error(tr(">> PI863:  error on axis %1: RS-232 overflow").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==3) {
                    log_error(tr(">> PI863:  error on axis %1: Macro storage full").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==4) {
                    log_error(tr(">> PI863:  error on axis %1: Macro out of range").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==5) {
                    log_error(tr(">> PI863:  error on axis %1: Macro wrong com").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==6) {
                    log_error(tr(">> PI863:  error on axis %1: wrong command").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==7) {
                    log_error(tr(">> PI863:  error on axis %1: hard stop").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==8) {
                    log_error(tr(">> PI863:  error on axis %1: not defined").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==9) {
                    log_error(tr(">> PI863:  error on axis %1: Position following error").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==10) {
                    log_error(tr(">> PI863:  error on axis %1: Move attempt while servo off").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==11) {
                    log_error(tr(">> PI863:  error on axis %1: Move attempt while joystick on").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                }

                if (axes[axis].state!=QFExtensionLinearStage::Error) {
                    // read current velocity
                    std::string rr=queryCommand("TV")+"\n";
                    double v=0;
                    if (!com.hasErrorOccured()) {
                        if (!sscanf(rr.c_str(), "V:%lf", &v)) {
                            log_error(tr(">> PI863:  invalid result string from TV command [expected V:<number>] in getAxisState() [axis %1]. String was '%2'.").arg(axis).arg(toprintablestr(rr).c_str()));
                            axes[axis].state=QFExtensionLinearStage::Error;
                        } else {
                            axes[axis].state=QFExtensionLinearStage::Ready;
                            if (v>0) axes[axis].state=QFExtensionLinearStage::Moving;
                        }
                    } else {
                        log_error(tr(">> PI863:  communication error"));
                        axes[axis].state=QFExtensionLinearStage::Error;
                    }
                }
            } else {
                log_error(tr(">> PI863:  invalid result string from TS command [expected S:<6 blocks of 2 hex numbers>] in getAxisState() [axis %1]. String was '%2'.").arg(axis).arg(toprintablestr(r).c_str()));
                axes[axis].state=QFExtensionLinearStage::Error;
            }


        } else {
            log_error(tr(">> PI863:  communication error").arg(axis));
            axes[axis].state=QFExtensionLinearStage::Error;
        }
    } else {
        axes[axis].state=QFExtensionLinearStage::Disconnected;
    }

	return axes[axis].state;
}

void QFExtensionLinearStagePI::stop(unsigned int axis) {
    if (com.isConnectionOpen()) {
        selectAxis(axis);
        sendCommand("AB");
    }
}

double QFExtensionLinearStagePI::getPosition(unsigned int axis) {
    if (com.isConnectionOpen()) {
        selectAxis(axis);
        std::string r=queryCommand("TP")+"\n";
        double v=0;
        if (!com.hasErrorOccured()) {
            if (sscanf(r.c_str(), "P:%lf", &v)) {
                return v;
            } else {
                log_error(tr(">> PI863:  invalid result string from TP command [expected P:<number>] from axis %1. String was '%2'.").arg(axis).arg(toprintablestr(r).c_str()));
            }
        }
    }
    return 0;
}

void QFExtensionLinearStagePI::move(unsigned int axis, double newPosition) {
    if (com.isConnectionOpen() && (axes[axis].state==QFExtensionLinearStage::Ready)) {
        long xx=(long)round(newPosition/lengthFactor);
        if (!com.hasErrorOccured()) {
            selectAxis(axis);
            sendCommand("SV"+inttostr((long)round(axes[axis].velocity/velocityFactor))+",MA"+inttostr(xx));
        }
        axes[axis].state=QFExtensionLinearStage::Moving;
    }
}


void QFExtensionLinearStagePI::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFExtensionLinearStagePI::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFExtensionLinearStagePI::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}


Q_EXPORT_PLUGIN2(stage_pi863, QFExtensionLinearStagePI)
