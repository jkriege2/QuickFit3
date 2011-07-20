#include "andorsettingsdialog.h"
#include "ui_andorsettingsdialog.h"





#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif


#ifdef __WINDOWS__
#  include "ATMCD32D.H"
#else
#  include "atmcdLXd.h"
#endif




AndorSettingsDialog::AndorSettingsDialog(int camera, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AndorSettingsDialog)
{
    m_headModel="";
    m_sensorWidth=0;
    m_sensorHeight=0;
    m_camera=camera;
    ui->setupUi(this);
    setInfo("");
    selectCamera(camera);
    GetDetector(&m_sensorWidth, &m_sensorHeight);
    char text[MAX_PATH];
    GetHeadModel(text);
    m_headModel=text;

    int i1, i2;
    GetEMGainRange(&i1, &i2);
    ui->spinEMGain->setRange(i1, i2);

    ui->labSensorSize->setTextFormat(Qt::RichText);
    ui->labCameraHead->setText(m_headModel);
    ui->labSensorSize->setText(QString("%1 &times; %2").arg(m_sensorWidth).arg(m_sensorHeight));


}

AndorSettingsDialog::~AndorSettingsDialog()
{
    delete ui;
}

void AndorSettingsDialog::readSettings(const QSettings& settings, const QString& prefix) {
    ui->cmbAcquisitionMode->setCurrentIndex(settings.value(prefix+"acquisition_mode", 0).toInt());
    ui->cmbReadMode->setCurrentIndex(settings.value(prefix+"read_mode", 0).toInt());
    ui->cmbFileFormat->setCurrentIndex(settings.value(prefix+"fileformat", 0).toInt());
    ui->spinExposure->setValue(settings.value(prefix+"exposure_time", 100).toDouble());
    ui->spinKineticCycleTime->setValue(settings.value(prefix+"kinetic_cycle_time", 100).toDouble());
    ui->spinKineticCycles->setValue(settings.value(prefix+"kinetic_cycles", 1).toInt());
    ui->spinAccumulates->setValue(settings.value(prefix+"accumulate_cycles", 1).toInt());
    ui->spinAccCycleTime->setValue(settings.value(prefix+"accumulate_cycles_time", 100).toDouble());
    ui->chkFrameTransfer->setChecked(settings.value(prefix+"frame_transfer", true).toBool());
    ui->chkBaselineClamp->setChecked(settings.value(prefix+"baseline_clamp", true).toBool());
    ui->spinBaselineOffset->setValue(settings.value(prefix+"baseline_offset", 0).toInt());
    ui->chkEMGain->setChecked(settings.value(prefix+"emgain_enabled", false).toBool());
    ui->spinEMGain->setValue(settings.value(prefix+"emgain", 1).toInt());
    ui->cmbPreampGain->setCurrentIndex(settings.value(prefix+"preamp_gain", 0).toInt());
    ui->cmbVerticalShiftSpeed->setCurrentIndex(settings.value(prefix+"vertical_shift_speed", 0).toInt());
    ui->cmbVerticalShiftAmplitude->setCurrentIndex(settings.value(prefix+"vertical_shift_amplitude", 0).toInt());
    ui->cmbHorizontalShiftSpeed->setCurrentIndex(settings.value(prefix+"horizontal_shift_speed", 0).toInt());
    ui->spinLeft->setValue(settings.value(prefix+"subimage_left", 0).toInt());
    ui->spinTop->setValue(settings.value(prefix+"subimage_top", 0).toInt());
    ui->spinWidth->setValue(settings.value(prefix+"subimage_width", m_sensorWidth).toInt());
    ui->spinHeight->setValue(settings.value(prefix+"subimage_height", m_sensorHeight).toInt());
    ui->spinHorizontalBinning->setValue(settings.value(prefix+"horizontal_binning", 1).toInt());
    ui->spinVerticalBinning->setValue(settings.value(prefix+"vertical_binning", 1).toInt());
    ui->chkCroppedMode->setChecked(settings.value(prefix+"cropped_mode", false).toBool());
}

void AndorSettingsDialog::writeSettings(QSettings& settings, const QString& prefix) const {
    settings.setValue(prefix+"head_model", m_headModel);
    settings.setValue(prefix+"acquisition_mode", ui->cmbAcquisitionMode->currentIndex());
    settings.setValue(prefix+"read_mode", ui->cmbReadMode->currentIndex());
    settings.setValue(prefix+"fileformat", ui->cmbFileFormat->currentIndex());
    settings.setValue(prefix+"exposure_time", ui->spinExposure->value());
    settings.setValue(prefix+"kinetic_cycle_time", ui->spinKineticCycleTime->value());
    settings.setValue(prefix+"kinetic_cycles", ui->spinKineticCycles->value());
    settings.setValue(prefix+"accumulate_cycles", ui->spinAccumulates->value());
    settings.setValue(prefix+"accumulate_cycles_time", ui->spinAccCycleTime->value());
    settings.setValue(prefix+"frame_transfer", ui->chkFrameTransfer->isChecked());
    settings.setValue(prefix+"baseline_clamp", ui->chkBaselineClamp->isChecked());
    settings.setValue(prefix+"baseline_offset", ui->spinBaselineOffset->value());
    settings.setValue(prefix+"emgain_enabled", ui->chkEMGain->isChecked());
    settings.setValue(prefix+"emgain", ui->spinEMGain->value());
    settings.setValue(prefix+"preamp_gain", ui->cmbPreampGain->currentIndex());
    settings.setValue(prefix+"vertical_shift_speed", ui->cmbVerticalShiftSpeed->currentIndex());
    settings.setValue(prefix+"vertical_shift_amplitude", ui->cmbVerticalShiftAmplitude->currentIndex());
    settings.setValue(prefix+"horizontal_shift_speed", ui->cmbHorizontalShiftSpeed->currentIndex());
    settings.setValue(prefix+"subimage_left", ui->spinLeft->value());
    settings.setValue(prefix+"subimage_top", ui->spinTop->value());
    settings.setValue(prefix+"subimage_width", ui->spinWidth->value());
    settings.setValue(prefix+"subimage_height", ui->spinHeight->value());
    settings.setValue(prefix+"horizontal_binning", ui->spinHorizontalBinning->value());
    settings.setValue(prefix+"vertical_binning", ui->spinVerticalBinning->value());
    settings.setValue(prefix+"cropped_mode", ui->chkCroppedMode->isChecked());
}


void AndorSettingsDialog::setInfo(const QString& info) {
    ui->grpCameraInfo->setVisible(!info.isEmpty());
    ui->labCameraInfo->setText(info);
}


void AndorSettingsDialog::selectCamera(int iSelectedCamera) {
    //std::cout<<"select ANDOR camera "<<iSelectedCamera<<std::endl;
    #ifdef __WINDOWS__
    long int SelectedCamera=0;
    #else
    int SelectedCamera=0;
    #endif
    GetCurrentCamera(&SelectedCamera);
    if (iSelectedCamera==SelectedCamera) return;
    at_32 lCameraHandle;
    GetCameraHandle(iSelectedCamera, &lCameraHandle);
    SetCurrentCamera(lCameraHandle);
}
