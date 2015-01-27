#include "multicontrol_settingsdialog.h"
#define LOG_PREFIX "[MulticontrolSettingsDialog]: "

MulticontrolSettingsDialog::MulticontrolSettingsDialog(QSettings *newSettings, QWidget *parent) :
    QDialog(parent)
{
    settings = newSettings;
    setWindowTitle("Settings of the multicontrol stage");
    setMinimumSize(300, 150);

    velGroup = new QGroupBox("Speed properties", this);
    comGroup = new QGroupBox("Connection", this);

    settingsLayout = new QGridLayout(this);
    velGroupLayout = new QGridLayout(velGroup);
    comGroupLayout = new QGridLayout(comGroup);
    QLocale local(QLocale::English);
    maxAccelBox = new QDoubleSpinBox(velGroup);
    maxVelBox = new QDoubleSpinBox(velGroup);
    maxJoyspeedBox = new QDoubleSpinBox(velGroup);
    maxAccelBox->setMaximum(15.0);
    maxVelBox->setMaximum(15.0);
    maxJoyspeedBox->setMaximum(15.0);
    maxAccelBox->setMinimum(5.0);
    maxVelBox->setMinimum(5.0);
    maxJoyspeedBox->setMinimum(5.0);
    bool ok = false;
    float maxAccel = settings->value("driver/maxacceleration", "10.0").toFloat(&ok);
    float maxVel = settings->value("driver/maxvelocity", "10.0").toFloat(&ok);
    float maxJoyspeed = settings->value("driver/maxjoyspeed", "10.0").toFloat(&ok);
    if(!ok){
        maxAccel = 10.0;
        maxVel = 10.0;
        maxJoyspeed = 10.0;
        log_warning("");
    }


    maxAccelBox->setValue(maxAccel);
    maxVelBox->setValue(maxVel);
    maxJoyspeedBox->setValue(maxJoyspeed);
    maxAccelBox->setLocale(local);
    maxVelBox->setLocale(local);
    maxJoyspeedBox->setLocale(local);

    velGroupLayout->addWidget(new QLabel("Max. Acceleration:", velGroup), 0, 0);
    velGroupLayout->addWidget(maxAccelBox, 0, 1);
    velGroupLayout->addWidget(new QLabel("Max. Velocity:", velGroup), 1, 0);
    velGroupLayout->addWidget(maxVelBox, 1, 1);
    velGroupLayout->addWidget(new QLabel("Max. Speed of Joystick:", velGroup), 2, 0);
    velGroupLayout->addWidget(maxJoyspeedBox, 2, 1);


    comCombo = new QComboBox(comGroup);
    comCombo->setInsertPolicy(QComboBox::NoInsert);
    for (int i=1; i<=8; i++){
        comCombo->addItem(tr("COM%1").arg(i));
    }

    QString comPort = settings->value("driver/port", "COM2").toString();
    int portIndex;
    if((comPort.length() == 4) && comPort.at(3).isDigit())
        portIndex = comPort.right(1).toInt();
    else if ((comPort.length() == 5) && comPort.at(3).isDigit() && comPort.at(4).isDigit())
        portIndex = comPort.right(2).toInt();
    else {
        log_warning(tr("Entry for \"driver/port\" in constructor MultiControlSettingsDialog could not be read: \'%1\'").arg(comPort));
        log_warning("The combobox init will be set to default value COM2)");
        portIndex = 2;
    }
    if ((portIndex < 1) || (portIndex > 8)){
        log_warning(tr("Entry for \"driver/port\" in constructor MultiControlSettingsDialog out of range: \'%1\'").arg(comPort));
        log_warning("The combox init will be set to default value COM2)");
        portIndex = 2;
    }
    comCombo->setCurrentIndex(portIndex-1);
    comGroupLayout->addWidget(comCombo, 0, 0);
    settingsLayout->addWidget(velGroup, 0, 0, 3, 3);
    settingsLayout->addWidget(comGroup, 3, 0, 1, 3);
    confirmer = new QPushButton("Confirm", this);
    canceler = new QPushButton("Cancel", this);
    settingsLayout->addWidget(confirmer, 4, 1);
    settingsLayout->addWidget(canceler, 4, 2);
    QObject::connect(canceler, SIGNAL(clicked()), this, SLOT(cancel()));
    QObject::connect(confirmer, SIGNAL(clicked()), this, SLOT(confirm()));
    show();

}

MulticontrolSettingsDialog::~MulticontrolSettingsDialog()
{
}

void MulticontrolSettingsDialog::cancel()
{
    delete this;
}

void MulticontrolSettingsDialog::confirm()
{

    if(settings->isWritable()){
        settings->setValue("driver/maxacceleration", QLocale::system().toString(maxAccelBox->value(), 'f'));
        settings->setValue("driver/maxvelocity", QLocale::system().toString(maxVelBox->value(), 'f'));
        settings->setValue("driver/maxjoyspeed", QLocale::system().toString(maxJoyspeedBox->value(), 'f'));
        QString newComPort(tr("COM%1").arg(comCombo->currentIndex()+1));
        //qDebug() << newComPort;
        settings->setValue("driver/port", newComPort);
    } else {
        log_error("Settings could not be written.");
    }
    valuesChanged();
    delete this;
}

void MulticontrolSettingsDialog::log_text(QString message) {
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void MulticontrolSettingsDialog::log_warning(QString message) {
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void MulticontrolSettingsDialog::log_error(QString message) {
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}

