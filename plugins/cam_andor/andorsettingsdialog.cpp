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

    ui->spinWidth->setRange(1, m_sensorWidth);
    ui->spinWidth->setValue(m_sensorWidth);
    ui->spinHeight->setRange(1, m_sensorHeight);
    ui->spinHeight->setValue(m_sensorWidth);
    ui->spinLeft->setRange(0, m_sensorWidth-1);
    ui->spinTop->setRange(0, m_sensorHeight-1);
    ui->spinLeft->setValue(0);
    ui->spinTop->setValue(0);
    ui->plotter->set_aspectRatio(1);
    ui->plotter->set_maintainAspectRatio(true);
    ui->plotter->setXRange(0, m_sensorWidth-1);
    ui->plotter->setYRange(0, m_sensorHeight-1);
    ui->plotter->set_xTickDistance(10);
    ui->plotter->set_yTickDistance(10);


}

AndorSettingsDialog::~AndorSettingsDialog()
{
    delete ui;
}

void AndorSettingsDialog::readSettings(const QSettings& settings, const QString& prefix) {
    //ui->cmbAcquisitionMode->setCurrentIndex(settings.value(prefix+"acquisition_mode", 0).toInt());
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
}

void AndorSettingsDialog::writeSettings(QSettings& settings, const QString& prefix) const {
    settings.setValue(prefix+"head_model", m_headModel);
    //settings.setValue(prefix+"acquisition_mode", ui->cmbAcquisitionMode->currentIndex());
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
}


void AndorSettingsDialog::setInfo(const QString& info) {
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

QRect AndorSettingsDialog::calcImageRect() {
    int l=ui->spinLeft->value();
    int t=ui->spinTop->value();
    int w=ui->spinWidth->value();
    int h=ui->spinHeight->value();
    int mode=ui->cmbReadMode->currentIndex();
    if (mode==1) { // FVB mode
        w=m_sensorWidth;
        t=0; l=0;
    } else if (mode==2) {
        t=0; l=0;
    }

    if (l+w>m_sensorWidth) l=m_sensorWidth-w;
    if (t+h>m_sensorHeight) t=m_sensorHeight-h;
    if (l<0) l=0;
    if (t<0) t=0;

    return QRect(l,t,w,h);
}

void AndorSettingsDialog::updateSubregion() {
    int mode=ui->cmbReadMode->currentIndex();

    ui->spinWidth->setEnabled(mode!=1);
    ui->spinHeight->setEnabled(true);
    ui->spinLeft->setEnabled(mode==0);
    ui->spinTop->setEnabled(mode==0);
    ui->spinHorizontalBinning->setEnabled(true);
    ui->spinVerticalBinning->setEnabled(mode!=1);
    if (mode==1) ui->spinVerticalBinning->setValue(1);

    QRect r=calcImageRect();

    if (ui->spinWidth->value()!=r.width()) ui->spinWidth->setValue(r.width());
    if (ui->spinHeight->value()!=r.height()) ui->spinHeight->setValue(r.height());
    if (ui->spinLeft->value()!=r.left()) ui->spinLeft->setValue(r.left());
    if (ui->spinTop->value()!=r.top()) ui->spinTop->setValue(r.top());

    ui->btnCenter->setEnabled(mode==0);
}
/** \brief resize subregion to given size and keep the center */
void AndorSettingsDialog::resizeSubregion(int width, int height) {
    QRect r=calcImageRect();
    QPoint c=r.center();

    ui->spinLeft->setValue(r.left()+r.width()/2-width/2);
    ui->spinTop->setValue(r.top()+r.height()/2-height/2);
    ui->spinWidth->setValue(width);
    ui->spinHeight->setValue(height);
    updateSubregion();
}











void AndorSettingsDialog::on_cmbReadMode_currentIndexChanged(int currentIndex) {

    ui->chkFrameTransfer->setEnabled(currentIndex==0);
    if (currentIndex!=0) ui->chkFrameTransfer->setChecked(true);

    updateSubregion();
}


void AndorSettingsDialog::on_btnCenter_clicked() {
    int w=ui->spinWidth->value();
    int h=ui->spinHeight->value();
    ui->spinLeft->setValue((m_sensorWidth-w)/2);
    ui->spinTop->setValue((m_sensorHeight-h)/2);
    updateSubregion();
}

void AndorSettingsDialog::on_btnFull_clicked() {
    ui->spinLeft->setValue(0);
    ui->spinTop->setValue(0);
    ui->spinWidth->setValue(m_sensorWidth);
    ui->spinHeight->setValue(m_sensorHeight);
    updateSubregion();
}

void AndorSettingsDialog::on_btn32_clicked() {
    resizeSubregion(32,32);
}

void AndorSettingsDialog::on_btn16_clicked() {
    resizeSubregion(16,16);
}

void AndorSettingsDialog::on_btn8_clicked() {
    resizeSubregion(8,8);
}



