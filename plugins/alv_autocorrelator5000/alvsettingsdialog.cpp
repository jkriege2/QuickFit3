#include "alvsettingsdialog.h"
#define LOG_PREFIX "[ALV5000/alvsettingsdialog]: "
ALVSettingsDialog::ALVSettingsDialog(QSettings& settings, AlvSmA5EDataIn *new_sm_data_in, QWidget *parent) :
    QFDialog(parent)
{
    sm_data_in = new_sm_data_in;

    //settings = newsettings;
    //alvsettings_dialog = new QWidget();
    resize(400, 600);
    //alvsettings_dialog->move(501,431);
    setWindowTitle("Settings of the ALV Software");
    layout = new QGridLayout(this);
    //layout->addWidget(new QLabel("<b>Settings:</b>", alvsettings_dialog), 0,0, Qt::AlignRight);

    // block duration + runnumber
    layout->addWidget(new QLabel("Duration:", this), 1, 0, Qt::AlignRight);
    duration = new QSpinBox(this);
    duration->setMinimum(1);
    duration->setValue((int32_t) settings.value("ALV_AUTOCORRELATOR5000_DURATION").toInt());
    //duration->setValue(30);


    layout->addWidget(duration, 1, 1);
    layout->addWidget(new QLabel("Number of runs:", this), 1,2, Qt::AlignRight);
    runnumber = new QSpinBox(this);
    runnumber->setMinimum(1);
    runnumber->setValue((int32_t)settings.value("ALV_AUTOCORRELATOR5000_RUNNUMBER").toInt());
    layout->addWidget(runnumber, 1, 3);
    //end duration + runnumber

    //// block scaling modes
    scalingmodes = new QGroupBox("Scaling modes", this);
    scalinggrid = new QGridLayout(scalingmodes);
    man = new QRadioButton("Manual", scalingmodes);
    scalinggrid->addWidget(man, 0, 0);
    nor = new QRadioButton("Normal", scalingmodes);
    scalinggrid->addWidget(nor, 0, 1);
    con = new QRadioButton("Conservative", scalingmodes);
    scalinggrid->addWidget(con, 0, 2);
    sec = new QRadioButton("Secure", scalingmodes);
    scalinggrid->addWidget(sec, 0, 3);
    // load default mode from settings
    uint8_t qvAutoScalingMode = (uint8_t) (settings.value("ALV_AUTOCORRELATOR5000_AUTOSCALINGMODE").toInt());
    if (qvAutoScalingMode == 0)
       man->setChecked(true);
    else if (qvAutoScalingMode == 1)
        nor->setChecked(true);
    else if (qvAutoScalingMode == 2)
        con->setChecked(true);
    else if (qvAutoScalingMode == 3)
        sec->setChecked(true);
    layout->addWidget(scalingmodes, 2, 0, 1,4);
    //// end scalingmodes

    // block scaling levels
    scaling0on = new QCheckBox(QString("Use Ch0 Scaling"), this);
    layout->addWidget(scaling0on, 3, 0);
    layout->addWidget(new QLabel("Scaling Level Ch0:", this), 4, 0, Qt::AlignRight);
    scalelevel0 = new QSpinBox(this);
    scalelevel0->setMinimum(0);
    scalelevel0->setMaximum(10);
    scalelevel0->setValue( (int32_t) (settings.value("ALV_AUTOCORRELATOR5000_SCALINGLEVEL0").toInt()) );
    layout->addWidget(scalelevel0, 4, 1);

    scaling1on = new QCheckBox(QString("Use Ch1 Scaling"), this);
    layout->addWidget(scaling1on, 3, 2);
    layout->addWidget(new QLabel("Scaling Level Ch1:", this),4,2, Qt::AlignRight);
    scalelevel1 = new QSpinBox(this);
    scalelevel1->setMinimum(0);
    scalelevel1->setMaximum(10);
    scalelevel1->setValue( (int32_t) (settings.value("ALV_AUTOCORRELATOR5000_SCALINGLEVEL0").toInt()) );
    layout->addWidget(scalelevel1, 4, 3);

    QObject::connect(scaling0on, SIGNAL(clicked(bool)), scalelevel0, SLOT(setEnabled(bool)));
    QObject::connect(scaling1on, SIGNAL(clicked(bool)), scalelevel1, SLOT(setEnabled(bool)));
    scaling0on->setChecked(false);
    scalelevel0->setEnabled(false);
    scaling1on->setChecked(false);
    scalelevel1->setEnabled(false);
    // end scaling levels

    // block A5Emodes
    sdmode = new QGroupBox(this);
    sdgrid = new QGridLayout(sdmode);
    single = new QRadioButton("Single Mode", sdmode);
    dual = new QRadioButton("Dual Mode", sdmode);
    sdgrid->addWidget(single, 0, 0, 0, 2);
    sdgrid->addWidget(dual, 0, 2, 0, 2);
    bool qvDualMode = settings.value("ALV_AUTOCORRELATOR5000_DUALMODE").toBool();
    if (qvDualMode)
        dual->setChecked(true);
    else
        single->setChecked(true);
    layout->addWidget(sdmode, 5, 0 , 1, 4);

    acmode = new QGroupBox(this);
    acgrid = new QGridLayout(acmode);
    autoc = new QRadioButton("Autocorrelation", acmode);
    cross = new QRadioButton("Crosscorrelation", acmode);
    acgrid->addWidget(autoc, 0, 0, 0, 2);
    acgrid->addWidget(cross, 0, 2, 0, 2);
    bool qvCrossMode = settings.value("ALV_AUTOCORRELATOR5000_CROSSMODE").toBool();
    if (qvCrossMode)
        cross->setChecked(true);
    else
        autoc->setChecked(true);
    layout->addWidget(acmode, 6, 0 , 1, 4);

    inmode = new QGroupBox(this);
    ingrid = new QGridLayout(inmode);
    in0 = new QRadioButton("Input (?) 0 Mode", inmode);
    in1 = new QRadioButton("Input (?) 1 Mode", inmode);
    ingrid->addWidget(in0, 0, 0, 0, 2);
    ingrid->addWidget(in1, 0, 2, 0, 2);
    bool qvInput1Mode = settings.value("ALV_AUTOCORRELATOR5000_INPUT1MODE").toBool();
    if (qvInput1Mode)
        in1->setChecked(true);
    else
        in0->setChecked(true);
    layout->addWidget(inmode, 7, 0 , 1, 4);

    // end modes

    fastmode = new QCheckBox("Fast Mode, whatever this means...", this);
    layout->addWidget(fastmode, 8, 0, 1, 3);

    // block floats
    floatvalid = new QDoubleValidator(this);
    floatvalid->setNotation(QDoubleValidator::StandardNotation);
    floatvalid->setLocale(QLocale(QLocale::English));
    floatvalid->setRange(0.0, std::numeric_limits<float>::max(), 3);

    layout->addWidget(new QLabel("Scattering Angle:"), 9, 0, Qt::AlignRight);
    scattangle = new QLineEdit(this);
    scattangle->setValidator(floatvalid);
    scattangle->setText(QLocale::system().toString(settings.value("ALV_AUTOCORRELATOR5000_SCATTANGLE").toFloat()));
    layout->addWidget(scattangle);
    layout->addWidget(new QLabel("Wavelength:"), 9, 2, Qt::AlignRight);
    wavel = new QLineEdit(this);
    wavel->setValidator(floatvalid);
    wavel->setText(QLocale::system().toString(settings.value("ALV_AUTOCORRELATOR5000_WAVELENGTH").toFloat()));
    layout->addWidget(wavel);

    layout->addWidget(new QLabel("RI of solvent:"), 10, 0, Qt::AlignRight);
    ri = new QLineEdit(this);
    ri->setValidator(floatvalid);
    ri->setText(QLocale::system().toString(settings.value("ALV_AUTOCORRELATOR5000_RI").toFloat()));
    layout->addWidget(ri);
    layout->addWidget(new QLabel("Viscosity:"), 10, 2, Qt::AlignRight);
    visc = new QLineEdit(this);
    visc->setValidator(floatvalid);
    visc->setText(QLocale::system().toString(settings.value("ALV_AUTOCORRELATOR5000_VISCOSITY").toFloat()));
    layout->addWidget(visc);

    layout->addWidget(new QLabel("Temperature:"), 11,0, Qt::AlignRight);
    temp = new QLineEdit(this);
    temp->setValidator(floatvalid);
    temp->setText(QLocale::system().toString(settings.value("ALV_AUTOCORRELATOR5000_TEMPERATURE").toFloat()));
    layout->addWidget(temp);
    //end block floats

    confirmer = new QPushButton("Ok");
    canceler = new QPushButton("Cancel");
    QObject::connect(confirmer, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(canceler, SIGNAL(clicked()), this, SLOT(reject()));
    layout->addWidget(confirmer, 13, 3);
    layout->addWidget(canceler, 13, 4);

    this->show();
}
/*
QPushButton *ALVSettingsDialog::getConfirmer()
{
    return confirmer;
}

QPushButton *ALVSettingsDialog::getCanceler()
{
    return canceler;
}

*/
int32_t ALVSettingsDialog::getDuration()
{
    return duration->value();
}

int32_t ALVSettingsDialog::getNumberOfRuns()
{
    return runnumber->value();
}

uint8_t ALVSettingsDialog::getScalingMode()
{
    if (man->isChecked())
        return 0;
    else if (nor->isChecked())
        return 1;
    else if (man->isChecked())
        return 2;
    else if (sec->isChecked())
        return 3;
    else{
        log_warning("Autoscaling mode has to be specified. \'Manual\' will be set per default.");
        return 0;
    }
}

bool ALVSettingsDialog::getScalingOn0()
{
    if(scaling0on->isChecked())
        return true;
    else
        return false;
}

uint8_t ALVSettingsDialog::getScaleLevel0()
{
    return scalelevel0->value();
}

bool ALVSettingsDialog::getScalingOn1()
{
    if(scaling1on->isChecked())
        return true;
    else
        return false;

}

uint8_t ALVSettingsDialog::getScaleLevel1()
{
    return scalelevel1->value();
}

bool ALVSettingsDialog::getDualMode()
{
    if (dual->isChecked())
        return true;
    else if (single->isChecked())
        return false;
    else {
        log_warning("Single or dual mode has to be chosen! Default mode \'Single\' set.");
        return false;
    }
}

bool ALVSettingsDialog::getCrossMode()
{
    if (cross->isChecked())
        return true;
    else if (autoc->isChecked())
        return false;
    else {
        log_warning("Auto or Cross correlation mode has to be chosen! Default mode \'Autocorrelation\' activated.");
        return false;
    }
}

bool ALVSettingsDialog::getIn1Mode()
{
    if (in1->isChecked())
        return true;
    else if (in0->isChecked())
        return false;
    else {
        log_warning("In0 or In1 mode has to be chosen! 'Default Mode In0 will be set.");
        return false;
    }
}

bool ALVSettingsDialog::getFastMode()
{
    if (fastmode->isChecked())
        return true;
    else
        return false;
}

float ALVSettingsDialog::getScatteringAngle()
{
    bool convok;
    float newScattAngle = scattangle->text().toFloat(&convok);
    if(!convok){
        log_warning(tr("Content of the \'Scattering angle box\' (\'%1\') could not be converted to float. Scattering angle will be set back to 90.0").arg(wavel->text()));
        return 90.0;
    } else
        return newScattAngle;
}

float ALVSettingsDialog::getWaveLength()
{
    bool convok;
    float newWaveLength = wavel->text().toFloat(&convok);
    if(!convok){
        log_text(tr("Content of the \'wavelength box\' (\'%1\') could not be converted to float. Wavelength will be set back to 500.0").arg(wavel->text()));
        return 500.0;
    } else
        return newWaveLength;
}

float ALVSettingsDialog::getRI()
{
    bool convok;
    float newRI = ri->text().toFloat(&convok);
    if(!convok){
        log_text(tr("Content of the \'RI of solvent box\' (\'%1\') could not be converted to float. RI will be set back to 1.332").arg(wavel->text()));
        return 1.332;
    } else
        return newRI;
}

float ALVSettingsDialog::getViscosity()
{
    bool convok;
    float newViscosity = visc->text().toFloat(&convok);
    if(!convok){
        log_text(tr("Content of the \'Viscosity box\' (\'%1\') could not be converted to float. RI will be set back to 0.9").arg(wavel->text()));
        return 0.9;
    } else
        return newViscosity;
}

float ALVSettingsDialog::getTemperature()
{
    bool convok;
    float newTemperature = temp->text().toFloat(&convok);
    if(!convok){
        log_text(tr("Content of the \'Temperature box\' (\'%1\') could not be converted to float. Temperature will be set back to 298.16").arg(wavel->text()));
        return 298.16;
    } else
        return newTemperature;
}



void ALVSettingsDialog::storeSettings(QSettings &settings)
{
    sm_data_in->setDuration(getDuration());
    //QSettings *settings = QFPluginServices::getInstance()->getSettings();

    settings.setValue("ALV_AUTOCORRELATOR5000_DURATION", getDuration());
    // set Number of Runs always 1 in the sm inteface as the runs are triggered seperately by this software
    sm_data_in->setNumberOfRuns(1);
    settings.setValue("ALV_AUTOCORRELATOR5000_RUNNUMBER", getNumberOfRuns());
    uint8_t ScaleModeID = getScalingMode();

    if (ScaleModeID == 0)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Manual);
    else if (ScaleModeID == 1)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Normal);
    else if (ScaleModeID == 2)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Manual);
    else if (ScaleModeID == 3)
        sm_data_in->setAutoScaleMode(AlvSmA5EDataIn::Secure);
    settings.setValue("ALV_AUTOCORRELATOR5000_AUTOSCALINGMODE", ScaleModeID);

    if(getScalingOn0()){
        sm_data_in->setScaleLevelCh0(getScaleLevel0());
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGON0", true);
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGLEVEL0", getScaleLevel0());
    } else {
        sm_data_in->setScaleLevelCh0(11);
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGON0", false);
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGLEVEL0", 11);
    }

    if(getScalingOn1()){
        sm_data_in->setScaleLevelCh1(getScaleLevel1());
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGON1", true);
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGLEVEL1", getScaleLevel1());
    } else {
        sm_data_in->setScaleLevelCh1(11);
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGON1", true);
        settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGLEVEL1", getScaleLevel1());
    }
    settings.setValue("ALV_AUTOCORRELATOR5000_SCALINGLEVEL1", getScaleLevel1());

    // A5E modes...
    bool dual = getDualMode();
    settings.setValue("ALV_AUTOCORRELATOR5000_DUALMODE", dual);
    sm_data_in->setDualMode(dual);
    bool cross = getCrossMode();
    settings.setValue("ALV_AUTOCORRELATOR5000_CROSSMODE", cross);
    sm_data_in->setCrossMode(cross);
    bool in1 = getIn1Mode();
    settings.setValue("ALV_AUTOCORRELATOR5000_INPUT1MODE", in1);
    sm_data_in->setIn1Mode(in1);
    bool fastmode = getFastMode();
    sm_data_in->setFastMode(fastmode);
    settings.setValue("ALV_AUTOCORRELATOR5000_FASTMODE", fastmode);
    float newScattAngle = getScatteringAngle();
    sm_data_in->setScatteringAngle(newScattAngle);
    settings.setValue("ALV_AUTOCORRELATOR5000_SCATTANGLE", newScattAngle);
    float newWavelength = getWaveLength();
    sm_data_in->setWavelength(newWavelength);
    settings.setValue("ALV_AUTOCORRELATOR5000_WAVELENGTH", newWavelength);
    float newRI = getRI();
    sm_data_in->setRIOfSolvent(newRI);
    settings.setValue("ALV_AUTOCORRELATOR5000_RI", newRI);
    float newViscosity = getViscosity();
    sm_data_in->setViscosity(newViscosity);
    settings.setValue("ALV_AUTOCORRELATOR5000_VISCOSITY", newViscosity);
    float newTemperature = getTemperature();
    sm_data_in->setTemperature(newTemperature);
    settings.setValue("ALV_AUTOCORRELATOR5000_TEMPERATURE", newTemperature);
    sm_data_in->setRunstate(AlvSmA5EDataIn::ParameterTransferOnly);
    sm_data_in->TriggerALVAction();
}


void ALVSettingsDialog::log_text(QString message) {
 //     QFPluginLogService::log_text(LOG_PREFIX + message + "\n");
    //else if (services) services->log_text(LOG_PREFIX + message + "\n");
    QFPluginLogTools::log_text(LOG_PREFIX + message + "\n");
}

void ALVSettingsDialog::log_warning(QString message) {
 //     QFPluginLogService::log_warning(LOG_PREFIX + message + "\n");
    //else if (services) services->log_warning(LOG_PREFIX + message + "\n");
    QFPluginLogTools::log_warning(LOG_PREFIX + message + "\n");
}

void ALVSettingsDialog::log_error(QString message) {

    //QFPluginLogService::log_error(LOG_PREFIX + message + "\n");
    //else if (services) services->log_error(LOG_PREFIX + message + "\n");
    QFPluginLogTools::log_error(LOG_PREFIX + message + "\n");
}


