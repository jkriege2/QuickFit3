#include "multicontrol_stage.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>


#define LOG_PREFIX "[MC2000]: "

MulticontrolStage::MulticontrolStage(QObject* parent):
    QObject(parent)
{
    QFPluginServices *services = QFPluginServices::getInstance();
    QString ini = services->getGlobalConfigFileDirectory()+QString("/multicontrolstageforffm.ini");
    if (!QFile::exists(ini)) ini = services->getConfigFileDirectory()+QString("/multicontrolstageforffm.ini");
    if (!QFile::exists(ini)) ini = services->getAssetsDirectory()+QString("/plugins/multicontrol_stage/multicontrolstageffm.ini");
    stageSettings = new QSettings(ini, QSettings::IniFormat, this);

    logService=NULL;

    //mutexSerial=new QMutex(QMutex::Recursive);
    stagecontroller = NULL;
    settingsDialog = NULL;



}

MulticontrolStage::~MulticontrolStage() {
    deinit();
    delete stagecontroller;
}


void MulticontrolStage::deinit() {
    /* add code for cleanup here */
    //QSettings stageSettings(services->getGlobalConfigFileDirectory()+"/stage_pi863.ini", QSettings::IniFormat);
    if (stageSettings->isWritable()) {
        stageSettings->setValue("driver/port", COMPort);
        stageSettings->setValue("driver/maxacceleration", QString::number(maxAcceleration, 'f'));
        stageSettings->setValue("driver/maxvelocity", QString::number(maxVelocity, 'f'));
        stageSettings->setValue("driver/maxjoyspeed", QString::number(maxJoySpeed, 'f'));
/*
        for (int i=0; i<axes.size(); i++) {
            QString axisname=axes[i].name;
            stageSettings.setValue(axisname+"/id", QString(QChar(axes[i].ID)));
        }
        */
    }
}

void MulticontrolStage::projectChanged(QFProject* oldProject, QFProject* project) {
    /* usually cameras do not have to react to a change of the project in QuickFit .. so you don't need to do anything here
       But: possibly you could read config information from the project here
     */
}

void MulticontrolStage::initExtension() {
    /* do initializations here but do not yet connect to the camera! */
    bool ok = false;
    COMPort = stageSettings->value("driver/port", "COM2").toString();
    maxAcceleration = stageSettings->value("driver/maxacceleration", "10.0").toFloat(&ok);
    maxVelocity = stageSettings->value("driver/maxvelocity", "10.0").toFloat(&ok);
    maxJoySpeed = stageSettings->value("driver/maxjoyspeed", "10.0").toFloat(&ok);
    if(ok)
        stagecontroller = new MCStageController(COMPort.toStdString(), maxAcceleration, maxVelocity, maxJoySpeed, this);
    else {
        log_warning("Settings for Stage could not be loaded due to an invalid entry of the init file.");
        log_warning("StageController was built with default values.");
        stagecontroller = new MCStageController(std::string("COM2"), 10.0, 10.0, 10.0, this);
    }


    ///TEST

    //SettingsDialog(0, settingsDialog);
    ///
}

void MulticontrolStage::loadSettings(ProgramOptions* settingspo) {
    /* here you could read config information from the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

    // ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

void MulticontrolStage::storeSettings(ProgramOptions* settingspo) {
    /* here you could write config information to the quickfit.ini file using settings object */
    if (!settingspo) return;
    if (settingspo->getQSettings()==NULL) return;
    //QSettings& settings=*(settingspo->getQSettings()); // the QSettings object for quickfit.ini

    // ALTERNATIVE: read/write Information to/from plugins/<ID>/<ID>.ini file
    // QSettings settings(services->getConfigFileDirectory()+"/plugins/"+getID()+"/"+getID()+".ini", QSettings::IniFormat);

}

unsigned int MulticontrolStage::getAxisCount()
{
    return stagecontroller->getDimensions();
}

uint8_t MulticontrolStage::getDimensions() {
    return stagecontroller->getDimensions();
}

void MulticontrolStage::reconfigureStage()
{
    // read Values from Qsettings Object again :
    bool ok = false;
    maxAcceleration = stageSettings->value("driver/maxacceleration", "10.0").toFloat(&ok);
    maxVelocity = stageSettings->value("driver/maxvelocity", "10.0").toFloat(&ok);
    maxJoySpeed = stageSettings->value("driver/maxjoyspeed", "10.0").toFloat(&ok);
    if(COMPort.compare(stageSettings->value("driver/port", "COM2").toString())){ // if another COMPORT was selected
        if(isConnected(3)){
            log_warning("Connection port for the multicontrol stage changed.");
            log_warning(tr("Disconnect the stage and plug it into your chosen port \'%1\', then reconnect.").arg(COMPort));
        }
        COMPort = stageSettings->value("driver/port", "COM2").toString();
    }
    if (ok)
        stagecontroller->reconfigureStage(maxAcceleration, maxVelocity, maxJoySpeed);
    else {
        log_warning("Setting configuration could not be read. Stage parameters will be set to default");
        stagecontroller->reconfigureStage(10.0, 10.0, 10.0);
    }
}

void MulticontrolStage::showSettingsDialog(unsigned int axis, QWidget* parent) {


    settingsDialog = new MulticontrolSettingsDialog(stageSettings, parent);
    QObject::connect(settingsDialog, SIGNAL(valuesChanged()), this, SLOT(reconfigureStage()));




   // bool globalIniWritable = QSettings(services->getGlobalConfigFileDirectory()+"/multicontrolstageforffm.ini", QSettings::IniFormat).isWritable();





    /* open a dialog that configures the

       usually you should display a modal QDialog descendent which writes back config when the user clicks OK

       alternatively you may also display a window which stays open and allows the suer to set settings also
       during the measurement.

       if the global ini file is not writable, there is no point in displaying a config dialog, as the data may not be stored
    */



    //QMessageBox::information(parent, getName(), tr("There is currently no configuration dialog!"));
}

/*
void MulticontrolStage::selectAxis(int i) {
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
*/
/*
void MulticontrolStage::sendCommand(std::string command) {
    com.write(command+"\x0D");
}
*/
/*
std::string MulticontrolStage::queryCommand(std::string command) {
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
*/


/*
void MulticontrolStage::checkComError() {

    QMutexLocker lock(mutexSerial);
    if (com.hasErrorOccured()) {
        for (int i=0; i<axes.size(); i++) {
            axes[i].state=QFExtensionLinearStage::Error;
            log_error(tr(LOG_PREFIX " serial port error: %1\n").arg(com.getLastError().c_str()));
        }
    }

}
*/
/*
MCStageController* MulticontrolStage::get_stagecontroller()
{
    if(stagecontroller)
        return stagecontroller;
    else
        return NULL;
}
*/


/*
bool MulticontrolStage::checkComConnected() {
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

*/
bool MulticontrolStage::isConnected(unsigned int axis) {
    //QMutexLocker lock(mutexSerial);
    if (!stagecontroller){
        log_error("Error No Stage Controller created in MulticontrolStage::isConnected(unsigned int axis)!");
        return false;
    }
    if (!stagecontroller->connected())
        return false;
    // check the dimension parameter of the stagecontroller;
    if (stagecontroller->getDimensions() == 1){
        if ((axis == 1) || (axis == 2) || (axis == 3))
            return true;
        else
            return false;
    } else if (stagecontroller->getDimensions() == 2){
        if((axis == 2) || (axis == 3))
            return true;
        else
            return false;
    } else if (stagecontroller->getDimensions() == 3){
        if(axis == 3)
            return true;
        else
            return false;
    } else {
        log_error("Wrong dimensions parameter specified in MulticontrolStage::isConnected(unsigned int axis!");
        return false;
    }
}

void MulticontrolStage::connectDevice(unsigned int axis) {
    stagecontroller->connectToStage();
    stagecontroller->initStage();
}

void MulticontrolStage::disconnectDevice(unsigned int axis) {
    //QMutexLocker lock(mutexSerial);
    stagecontroller->disconnect();
}

void MulticontrolStage::setLogging(QFPluginLogService* logService) {
    this->logService=logService;
}

void MulticontrolStage::setJoystickActive(unsigned int axis, bool enabled, double maxVelocity) {
    //    QMutexLocker lock(mutexSerial);
    // unsigned int axis is not used,
    // but remains in the signature to stick consistent with the interface classe
    if (enabled)
        stagecontroller->config_joystick(true, maxVelocity);
    else
        stagecontroller->config_joystick(false, maxVelocity);
}

bool MulticontrolStage::isJoystickActive(unsigned int axis){
    return stagecontroller->joystick_enabled();
}

QFExtensionLinearStage::AxisState MulticontrolStage::getAxisState(unsigned int axis)
{
    QTime t;
    t.start();
    while(t.elapsed() < 10){
    }
    // check, if axis is connected:
    if (!isConnected(axis))
        return Disconnected;
    t.restart();
    while(t.elapsed() < 10){
    }
    if(stagecontroller->hardware_error())
        return Error;
    t.restart();
    while(t.elapsed() < 10){
    }
    if(stagecontroller->moving())
        return Moving;
    else
        return Ready;
}

void MulticontrolStage::stop(unsigned int axis)
{
}
/*
QFExtensionLinearStage::AxisState MulticontrolStage::getAxisState(unsigned int axis)  {
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
*/
/*
void MulticontrolStage::stop(unsigned int axis) {


}
*/
/*
double MulticontrolStage::getSpeed(uint8_t axis) {

    return 0;
}
*/
void MulticontrolStage::move(unsigned int axis, double newPosition)
{
    //stagecontroller->config_joystick(false);
    QTime t;
    t.start();
    while (t.elapsed() < 10){
    }
    while (stagecontroller->moving()){
        log_warning("The Stage was still moving, while receiving a new instruction");
        t.restart();
        while (t.elapsed() < 250){
        }
    }
    stagecontroller->config_joystick(false);
    t.restart();
    while (t.elapsed() < 20){
    }
    //(getAxisState(axis) == Ready)
    //int axisStateInt = getAxisState(axis);
    /*
    while (axisStateInt != Ready){
        log_warning(tr("The Stage was still moving, while receiving a new instruction: axisState = ").arg(axisStateInt));
        t.restart();
        while (t.elapsed() < 500){
        }
        axisStateInt = getAxisState(axis);
    }*/
    //log_text("DEBUG reached reading.");
    // read old positions
    float oldxpos = stagecontroller->get_currentPosition(1);
    t.restart();
    while (t.elapsed() < 10){
    }
    float oldypos = stagecontroller->get_currentPosition(2);
    t.restart();
    while (t.elapsed() < 10){
    }
    float oldzpos = stagecontroller->get_currentPosition(3);
    t.restart();
    while (t.elapsed() < 10){
    }
    //log_text("DEBUG reached moving.");
    //// move to new position
    if (axis == 1){
        stagecontroller->move_lim((float) newPosition, oldypos, oldzpos);
    }
    else if (axis == 2){
        stagecontroller->move_lim(oldxpos, (float) newPosition, oldzpos);
    }
    else if (axis == 3){
        stagecontroller->move_lim(oldxpos, oldypos, (float) newPosition);
    }
    t.restart();
    while (t.elapsed() < 1000){
    }
    while (stagecontroller->moving()){ // append waittime if necessary
        //log_warning("Stage is still moving...");
        t.restart();
        while (t.elapsed() < 200){
        }
    }
    t.restart();
    while (t.elapsed() < 10){
    }
    stagecontroller->config_joystick(true);
}

double MulticontrolStage::getPosition(unsigned int axis) {
    return stagecontroller->get_currentPosition((uint8_t) axis);
}

double MulticontrolStage::getSpeed(unsigned int axis)
{
    return 0.0;
}

void MulticontrolStage::log_text(QString message) {
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void MulticontrolStage::log_warning(QString message) {
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void MulticontrolStage::log_error(QString message) {
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}

Q_EXPORT_PLUGIN2(stage_mc2000, MulticontrolStage)

