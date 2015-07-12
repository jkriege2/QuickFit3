/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "stage_pi.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfplugin.h"
#include <iostream>
#include "pimercury863calibrationdialog.h"
#include <QDebug>

#define LOG_PREFIX "[PI863]: "

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
    currentID=-1;
    lengthFactor=6.9e-3;
    velocityFactor=6.9e-3;
    accelerationFactor=6.9e-3;
    mutexSerial=new QMutex(QMutex::Recursive);
}

QFExtensionLinearStagePI::~QFExtensionLinearStagePI() {
    delete mutexSerial;
}


void QFExtensionLinearStagePI::deinit() {
	/* add code for cleanup here */
    QSettings inifile(services->getGlobalConfigFileDirectory()+"/stage_pi863.ini", QSettings::IniFormat);
    if (inifile.isWritable()) {
        inifile.setValue("driver/port", COMPort);
        inifile.setValue("driver/port_speed", COMPortSpeed);
        inifile.setValue("driver/pterm", PTerm);
        inifile.setValue("driver/iterm", iTerm);
        inifile.setValue("driver/dterm", DTerm);
        inifile.setValue("driver/ilimit", iLimit);
        inifile.setValue("driver/acceleration", acceleration);
        inifile.setValue("driver/initvelocity", initVelocity);
        inifile.setValue("driver/maxvelocity", maxVelocity);


        for (int i=0; i<axes.size(); i++) {
            QString axisname=axes[i].name;
            inifile.setValue(axisname+"/id", QString(QChar(axes[i].ID)));
        }
    }
}

void QFExtensionLinearStagePI::projectChanged(QFProject* /*oldProject*/, QFProject* /*project*/) {
	/* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
	   But: possibly you could read config information from the project here
	 */
}

void QFExtensionLinearStagePI::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    QString ini=services->getGlobalConfigFileDirectory()+QString("/stage_pi863.ini");
    if (!QFile::exists(ini)) ini=services->getConfigFileDirectory()+QString("/stage_pi863.ini");
    if (!QFile::exists(ini)) ini=services->getAssetsDirectory()+QString("/plugins/")+getID()+QString("/stage_pi863.ini");
    QFPluginServices::getInstance()->log_global_text(tr("%1loading INI-file %2\n").arg(QString(LOG_PREFIX)).arg(ini));
    QSettings inifile(ini, QSettings::IniFormat);
    COMPort=inifile.value("driver/port", COMPort).toString();
    COMPortSpeed=inifile.value("driver/port_speed", COMPortSpeed).toUInt();
    PTerm=inifile.value("driver/pterm", PTerm).toUInt();
    iTerm=inifile.value("driver/iterm", iTerm).toUInt();
    DTerm=inifile.value("driver/dterm", DTerm).toUInt();
    iLimit=inifile.value("driver/ilimit", iLimit).toUInt();
    acceleration=inifile.value("driver/acceleration", acceleration).toDouble();
    initVelocity=inifile.value("driver/initvelocity", initVelocity).toDouble();
    maxVelocity=inifile.value("driver/maxvelocity", maxVelocity).toDouble();

    axes.clear();
    for (int i=0; i<=15; i++) {
        QString axisname=QString("axis%1").arg(i);
        if (inifile.contains(axisname+"/id")) {
            AxisDescription d;
            QString s=inifile.value(axisname+"/id", '0').toString();
            d.ID='0';
            if (s.size()>0) d.ID=s[0];
            d.velocity=initVelocity;
            d.joystickEnabled=false;
            d.state=QFExtensionLinearStage::Disconnected;
            d.name=axisname;
            d.label=inifile.value(axisname+"/label", tr("PI Mercury 863, axis %1").arg(i)).toString();
            axes.append(d);
            actCalibrate.append(new QAction(tr("calibrate axis '%1'").arg(d.label), this));
            connect(actCalibrate.last(), SIGNAL(triggered()), this, SLOT(calibrateAxis()));
        }
    }

    menuStage=new QMenu(tr("PI stage"));
    menuStage->setIcon(QIcon(":/stage_pi_logo.png"));
    actCalibrateJoysticks=new QAction(QIcon(":/stage_pi/pi_joystick.png"), tr("calibrate PI stage joysticks"), this);
    connect(actCalibrateJoysticks, SIGNAL(triggered()), this, SLOT(calibrateJoysticks()));


    menuStage->addAction(actCalibrateJoysticks);
    menuStage->addSeparator();
    for (int i=0; i<actCalibrate.size(); i++) {
        menuStage->addAction(actCalibrate[i]);
    }

    if (services) {
        QMenu* m=services->getMenu("extensions");
        if (m) {
            m->addAction(menuStage->menuAction());
        }
    }
}

void QFExtensionLinearStagePI::loadSettings(ProgramOptions* settingspo) {
	/* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void QFExtensionLinearStagePI::storeSettings(ProgramOptions* settingspo) {
	/* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
	if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

	// ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
	// QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int QFExtensionLinearStagePI::getAxisCount() {
	return axes.size();
}

void QFExtensionLinearStagePI::showSettingsDialog(unsigned int /*axis*/, QWidget* parent) {

    bool globalIniWritable=QSettings(services->getGlobalConfigFileDirectory()+"/stage_pi863.ini", QSettings::IniFormat).isWritable();


    /* open a dialog that configures the camera.

       usually you should display a modal QDialog descendent which writes back config when the user clicks OK

       alternatively you may also display a window which stays open and allows the suer to set settings also
       during the measurement.

       if the global ini file is not writable, there is no point in displaying a config dialog, as the data may not be stored
    */


    if (globalIniWritable) {
        /////////////////////////////////////////////////////////////////////////////////
        // if you want the settings dialog to be modal, you may uncomment the next lines
        // and add implementations
        /////////////////////////////////////////////////////////////////////////////////

        QDialog* dlg=new QDialog(parent);

        QVBoxLayout* lay=new QVBoxLayout();
        dlg->setLayout(lay);

        QFormLayout* formlayout=new QFormLayout();


        formlayout->addRow("", new QLabel(tr("<b>All settings marked with * will be<br>used when connecting the next time!</b>"), dlg));

        QComboBox* cmbPort=new QComboBox(dlg);
        std::vector<std::string> ports=QFSerialConnection::listPorts();
        for (unsigned int i=0; i<ports.size(); i++) {
            cmbPort->addItem(ports[i].c_str());
        }
        cmbPort->setEditable(false);
        cmbPort->setCurrentIndex(cmbPort->findText(COMPort));
        formlayout->addRow(tr("serial &port*:"), cmbPort);

        QComboBox* cmbSpeed=new QComboBox(dlg);
        cmbSpeed->setEditable(false);
        cmbSpeed->addItem("9600");
        cmbSpeed->addItem("19200");
        cmbSpeed->addItem("38400");
        cmbSpeed->setCurrentIndex(cmbSpeed->findText(QString::number(COMPortSpeed)));
        formlayout->addRow(tr("serial port &baudrate*:"), cmbSpeed);


        lay->addLayout(formlayout);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
        lay->addWidget(buttonBox);

        connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

        if ( dlg->exec()==QDialog::Accepted ) {
             //  read back values entered into the widgets and store in settings
             COMPort=cmbPort->currentText();
             COMPortSpeed=cmbSpeed->itemText(cmbSpeed->currentIndex()).toInt();
        }
        delete dlg;

    } else {
        QMessageBox::information(parent, getName(), tr("Your user account is not allowd to write to the global config directory!"));
    }
	//QMessageBox::information(parent, getName(), tr("There is currently no configuration dialog!"));
}

void QFExtensionLinearStagePI::selectAxis(int i) {
    QMutexLocker lock(mutexSerial);
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
    QMutexLocker lock(mutexSerial);
    if (com.hasErrorOccured()) {
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Error;
            log_error(tr(LOG_PREFIX " serial port error: %1\n").arg(com.getLastError().c_str()));
        }
    }
}

void QFExtensionLinearStagePI::calibrateJoysticks() {
    QMutexLocker lock(mutexSerial);

    for (unsigned int axis=0; axis<getAxisCount(); axis++) {
        QMessageBox::StandardButton answer=QMessageBox::question(NULL, tr("PI Mercury C863 joystick calibration"), tr("Do you want to calibrate a joystick on axis %1?").arg(axis), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
        if (answer==QMessageBox::Yes) {
            bool wasConnected=isConnected(axis);
            if (!wasConnected) connectDevice(axis);
            setJoystickActive(axis, false);
            PIMercury863CalibrationDialog* dlg=new PIMercury863CalibrationDialog(NULL, this, axis);
            dlg->exec();
            delete dlg;
            if (!wasConnected) disconnectDevice(axis);
        } else if (answer==QMessageBox::Cancel)  {
            break;
        }
    }
}

void QFExtensionLinearStagePI::calibrateAxis()
{
    QMutexLocker lock(mutexSerial);
    QAction* act=qobject_cast<QAction*>(sender());
    int idx=actCalibrate.indexOf(act);
    if (act && idx>=0 && com.isConnectionOpen()) {
        selectAxis(idx);
        sendCommand("FE2");
    }
}

bool QFExtensionLinearStagePI::checkComConnected() {
    QMutexLocker lock(mutexSerial);
    bool c=com.isConnectionOpen();
    if (!c) {
        log_error(tr(LOG_PREFIX " Could not connect to Mercury C-863 Motor Controller!!!\n"));
        log_error(tr(LOG_PREFIX " reason: %1\n").arg(com.getLastError().c_str()));
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Error;
        }
    }
    return c;
}

bool QFExtensionLinearStagePI::isConnected(unsigned int /*axis*/) {
    QMutexLocker lock(mutexSerial);
    return com.isConnectionOpen();
}

void QFExtensionLinearStagePI::connectDevice(unsigned int /*axis*/) {
    QMutexLocker lock(mutexSerial);
    com.set_port(COMPort.toStdString());
    com.set_baudrate(COMPortSpeed);
    com.set_stopbits(QFSConeStopbit);
    com.set_parity(QFSCnoParity);
    com.set_databits(QFSC8databits);
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
        log_error(tr(LOG_PREFIX " Could not connect to Mercury C-863 Motor Controller!!!\n"));
        log_error(tr(LOG_PREFIX " reason: %1\n").arg(com.getLastError().c_str()));
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Disconnected;
        }
    }
}

void QFExtensionLinearStagePI::disconnectDevice(unsigned int /*axis*/) {
    QMutexLocker lock(mutexSerial);
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
    QMutexLocker lock(mutexSerial);
    if ((enabled && !axes[axis].joystickEnabled) || (enabled && maxVelocity!=axes[axis].joyVelocity)) {
        selectAxis(axis);
        sendCommand("JN"+inttostr((long)round(maxVelocity/velocityFactor)));
        axes[axis].joystickEnabled=true;
        axes[axis].joyVelocity=maxVelocity;
    } else if (!enabled && axes[axis].joystickEnabled) {
        selectAxis(axis);
        sendCommand("JF");
        axes[axis].joystickEnabled=false;
    }
}

bool QFExtensionLinearStagePI::isJoystickActive(unsigned int axis) {
	return axes[axis].joystickEnabled;
}

QFExtensionLinearStage::AxisState QFExtensionLinearStagePI::getAxisState(unsigned int axis)  {
    QMutexLocker lock(mutexSerial);

    std::string failedaxes="";
    if (com.isConnectionOpen()) {
        selectAxis(axis);
        std::string r=queryCommand("TS")+"\n";
        int B1=0, B2=0, B3=0, B4=0, B5=0, B6=0;
        if (!com.hasErrorOccured()) {
            if (sscanf(r.c_str(), "S:%x %x %x %x %x %x", &B1, &B2, &B3, &B4, &B5, &B6)) {
                //std::cout<<"\n\n"<<std::string(1, (char)('x'+i-1))<<": "<<r<<"\n\n";
                if (B6==1) {
                    log_error(tr(LOG_PREFIX " error on axis %1: RS-232 timeout\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==2) {
                    log_error(tr(LOG_PREFIX " error on axis %1: RS-232 overflow\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==3) {
                    log_error(tr(LOG_PREFIX " error on axis %1: Macro storage full\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==4) {
                    log_error(tr(LOG_PREFIX " error on axis %1: Macro out of range\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==5) {
                    log_error(tr(LOG_PREFIX " error on axis %1: Macro wrong com\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==6) {
                    log_error(tr(LOG_PREFIX " error on axis %1: wrong command\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==7) {
                    log_error(tr(LOG_PREFIX " error on axis %1: hard stop\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==8) {
                    log_error(tr(LOG_PREFIX " error on axis %1: not defined\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==9) {
                    log_error(tr(LOG_PREFIX " error on axis %1: Position following error\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==10) {
                    log_error(tr(LOG_PREFIX " error on axis %1: Move attempt while servo off\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                } else if (B6==11) {
                    log_error(tr(LOG_PREFIX " error on axis %1: Move attempt while joystick on\n").arg(axis));
                    axes[axis].state=QFExtensionLinearStage::Error;
                }

                if (axes[axis].state!=QFExtensionLinearStage::Error) {
                    // read current velocity
                    axes[axis].state=QFExtensionLinearStage::Ready;
                    double v=getSpeed(axis);
                    if (axes[axis].state!=QFExtensionLinearStage::Error) {
                        if (fabs(v)>0) axes[axis].state=QFExtensionLinearStage::Moving;
                    }
                }
            } else {
                log_error(tr(LOG_PREFIX " invalid result string from TS command [expected S:<6 blocks of 2 hex numbers>] in getAxisState() [axis %1]. String was '%2'.\n").arg(axis).arg(toprintablestr(r).c_str()));
                axes[axis].state=QFExtensionLinearStage::Error;
            }


        } else {
            log_error(tr(LOG_PREFIX " communication error\n").arg(axis));
            axes[axis].state=QFExtensionLinearStage::Error;
        }
    } else {
        axes[axis].state=QFExtensionLinearStage::Disconnected;
    }

	return axes[axis].state;
}

void QFExtensionLinearStagePI::stop(unsigned int axis) {
    QMutexLocker lock(mutexSerial);
    if (com.isConnectionOpen()) {
        selectAxis(axis);
        sendCommand("AB");
    }
}

double QFExtensionLinearStagePI::getSpeed(unsigned int axis) {
    QMutexLocker lock(mutexSerial);
    if (com.isConnectionOpen()) {
        selectAxis(axis);
        std::string r=queryCommand("TV")+"\n";
        double v=0;
        if (!com.hasErrorOccured()) {
            if (sscanf(r.c_str(), "V:%lf", &v)) {
                return v*velocityFactor;
            } else {
                log_error(tr(LOG_PREFIX " invalid result string from TV command [expected V:<number>] from axis %1. String was '%2'.\n").arg(axis).arg(toprintablestr(r).c_str()));
            }
        }
    }
    return 0;
}

double QFExtensionLinearStagePI::getPosition(unsigned int axis) {
    QMutexLocker lock(mutexSerial);
    if (com.isConnectionOpen()) {
        selectAxis(axis);
        std::string r=queryCommand("TP")+"\n";
        double v=0;
        if (!com.hasErrorOccured()) {
            if (sscanf(r.c_str(), "P:%lf", &v)) {
                return v*lengthFactor;
            } else {
                log_error(tr(LOG_PREFIX " invalid result string from TP command [expected P:<number>] from axis %1. String was '%2'.\n").arg(axis).arg(toprintablestr(r).c_str()));
            }
        }
    }
    return 0;
}

void QFExtensionLinearStagePI::move(unsigned int axis, double newPosition) {
    QMutexLocker lock(mutexSerial);
    if (com.isConnectionOpen() && (axes[axis].state==QFExtensionLinearStage::Ready) && (!axes[axis].joystickEnabled)) {
        long xx=(long)round(newPosition/lengthFactor);
        if (!com.hasErrorOccured()) {
            selectAxis(axis);
            sendCommand("SV"+inttostr((long)round(axes[axis].velocity/velocityFactor))+",MA"+inttostr(xx));
            //qDebug()<<"  move("<<axis<<", "<<inttostr(xx).c_str()<<")   SV="<<inttostr((long)round(axes[axis].velocity/velocityFactor)).c_str();
        }
        axes[axis].state=QFExtensionLinearStage::Moving;
    }
}

QString QFExtensionLinearStagePI::getStageName(unsigned int axis) const
{
    QString n=tr("PI Mercury 863, axis %1").arg(axis);
    if (/*axis>=0 &&*/ (int64_t)axis<axes.size()) n=axes[axis].label;
    return n;
}

QFExtensionLinearStage::StageInfo QFExtensionLinearStagePI::getStageInfo(unsigned int /*axis*/) const
{
    QFExtensionLinearStage::StageInfo info;
    info.maxSpeed=maxVelocity;
    return info;
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
