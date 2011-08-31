#include "andorsettingsdialog.h"
#include "ui_andorsettingsdialog.h"
#include "jkqtfastplotter.h"
#include "cam_andor.h"
#include "andortools.h"



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


#define CHECK(s) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        ok=false; \
        qDebug()<<QString("error during '%4'\n  error code was: %1 [%2]").arg(error).arg(andorErrorToString(error)).arg(#s); \
    } \
}

/* qDebug()<<QString("error during '%4'\n  error code was: %1 [%2]").arg(error).arg(andorErrorToString(error)).arg(#s); \
*/
AndorSettingsDialog::AndorSettingsDialog(int camera, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AndorSettingsDialog)
{
    m_image=NULL;
    m_headModel="";
    m_sensorWidth=0;
    m_sensorHeight=0;
    m_camera=camera;
    m_updatingSubregion=true;
    m_updatingPreview=true;
    m_calcTiming=true;
    m_updatingSensorSetup=true;
    ui->setupUi(this);
    m_updatingSubregion=false;
    m_updatingPreview=false;
    m_calcTiming=false;
    m_updatingSensorSetup=false;
}

AndorSettingsDialog::~AndorSettingsDialog()
{
    delete ui;
}

void AndorSettingsDialog::setupWidgets() {
    if (m_updatingSubregion) return;
    bool old_m_updatingSensorSetup=m_updatingSubregion;
    m_updatingSubregion=true;
    int camera=m_camera;

    setInfo("");
    selectCamera(camera);
    GetDetector(&m_sensorWidth, &m_sensorHeight);
  #ifdef MAX_PATH
    char text[MAX_PATH];
  #else
    char text[1024];
  #endif
    GetHeadModel(text);
    m_headModel=text;

    int i1, i2;
    float f1;
    GetEMGainRange(&i1, &i2);
    ui->spinEMGain->setRange(i1, i2);

    GetMaximumExposure(&f1);
    ui->spinExposure->setValue(f1*1000.0);

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
    /*ui->plotter->set_aspectRatio(1);
    ui->plotter->set_maintainAspectRatio(true);
    ui->plotter->setXRange(0, m_sensorWidth-1);
    ui->plotter->setYRange(0, m_sensorHeight-1);
    ui->plotter->set_xTickDistance(10);
    ui->plotter->set_yTickDistance(10);*/
    ui->labPlot->setText("");




    int NumChannels, NumAmp, NumPreampGains;
    int BitDepth;
    int NumVSSpeeds;
    float VSSpeed;
    float PreampGain;
    int fastestVSS=0;
    float fastestVSSF=0;
    //QString AmpDescription="---";


    ui->cmbADChannel->clear();
    GetNumberADChannels(&NumChannels);
    for (int channel =0; channel<NumChannels; channel++) {
        GetBitDepth(channel, &BitDepth);
        ui->cmbADChannel->addItem(QString("%1 bits").arg(BitDepth));
    }

    ui->cmbAmplifier->clear();
    GetNumberAmp(&NumAmp);
    for (int amp=0; amp<NumAmp; amp++) {
        GetAmpDesc(amp, text, 512);
        ui->cmbAmplifier->addItem(text);
    }
    //ui->cmbAmplifier->addItem("EMCCD register");
    //ui->cmbAmplifier->addItem("CCD register");

    ui->cmbPreampGain->clear();
    GetNumberPreAmpGains(&NumPreampGains);
    for (int gain=0; gain<NumPreampGains; gain++) {
        GetPreAmpGain(gain, &PreampGain);
        ui->cmbPreampGain->addItem(QString("%1x").arg(PreampGain), gain);
    }


    ui->cmbVerticalShiftSpeed->clear();
    GetNumberVSSpeeds(&NumVSSpeeds);
    GetFastestRecommendedVSSpeed(&fastestVSS, &fastestVSSF);
    for (int vsspeed=0; vsspeed<NumVSSpeeds; vsspeed++) {
        GetVSSpeed(vsspeed, &VSSpeed);
        if (vsspeed<fastestVSS) ui->cmbVerticalShiftSpeed->addItem(QString("[%1 µs/pixel]").arg(VSSpeed), VSSpeed);
        else ui->cmbVerticalShiftSpeed->addItem(QString("%1 µs/pixel").arg(VSSpeed), VSSpeed);
    }


    m_updatingSubregion=old_m_updatingSensorSetup;
    updateSensorSetup();
}

void AndorSettingsDialog::readSettings(QSettings& settings) {
    if (m_updatingSubregion) return;
    bool old_m_updatingSensorSetup=m_updatingSubregion;
    m_updatingSubregion=true;
    QString prefix="cam_andor/";
    ui->cmbTriggerMode->setCurrentIndex(settings.value(prefix+"trigger_mode", 0).toInt());
    ui->chkTriggerInvert->setChecked(settings.value(prefix+"trigger_invert", false).toBool());


    ui->cmbReadMode->setCurrentIndex(settings.value(prefix+"read_mode", 0).toInt());
    ui->cmbFileFormat->setCurrentIndex(settings.value(prefix+"fileformat", 0).toInt());
    ui->spinExposure->setValue(settings.value(prefix+"exposure_time", 100000.0).toDouble()/1000.0);
    ui->spinKineticCycleTime->setValue(settings.value(prefix+"kinetic_cycle_time", 100000.0).toDouble()/1000.0);
    ui->spinKineticCycles->setValue(settings.value(prefix+"kinetic_cycles", 1).toInt());
    ui->spinAccumulates->setValue(settings.value(prefix+"accumulate_cycles", 1).toInt());
    ui->spinAccCycleTime->setValue(settings.value(prefix+"accumulate_cycles_time", 100000.0).toDouble()/1000.0);
    ui->chkFrameTransfer->setChecked(settings.value(prefix+"frame_transfer", true).toBool());
    ui->chkBaselineClamp->setChecked(settings.value(prefix+"baseline_clamp", true).toBool());
    ui->spinBaselineOffset->setValue(settings.value(prefix+"baseline_offset", 0).toInt());
    ui->spinEMGain->setValue(settings.value(prefix+"emgain", 1).toInt());
    ui->chkEMGain->setChecked(settings.value(prefix+"emgain_enabled", true).toBool());
    ui->cmbPreampGain->setCurrentIndex(settings.value(prefix+"preamp_gain", 0).toInt());
    ui->cmbVerticalShiftSpeed->setCurrentIndex(settings.value(prefix+"vertical_shift_speed", 0).toInt());
    ui->cmbVerticalShiftAmplitude->setCurrentIndex(settings.value(prefix+"vertical_shift_amplitude", 0).toInt());
    ui->cmbHorizontalShiftSpeed->setCurrentIndex(settings.value(prefix+"horizontal_shift_speed", 0).toInt());

    int hstart=settings.value(prefix+"subimage_hstart", 1).toInt()-1;
    int vstart=settings.value(prefix+"subimage_vstart", 1).toInt()-1;
    int hend=settings.value(prefix+"subimage_hend", m_sensorWidth).toInt();
    int vend=settings.value(prefix+"subimage_vend", m_sensorHeight).toInt();

    ui->spinLeft->setValue(hstart);
    ui->spinTop->setValue(m_sensorHeight-vstart);
    ui->spinWidth->setValue(abs(hend-hstart));
    ui->spinHeight->setValue(abs(vend-vstart));
    ui->spinHorizontalBinning->setValue(settings.value(prefix+"horizontal_binning", 1).toInt());
    ui->spinVerticalBinning->setValue(settings.value(prefix+"vertical_binning", 1).toInt());
    ui->cmbAmplifier->setCurrentIndex(settings.value(prefix+"amplifier", 0).toInt());
    ui->cmbADChannel->setCurrentIndex(settings.value(prefix+"ad_channel", 0).toInt());

    m_updatingSubregion=old_m_updatingSensorSetup;
    updateSensorSetup();
    updateSubregion();
}

void AndorSettingsDialog::writeSettings(QSettings& settings) const {
    QString prefix="cam_andor/";
    settings.setValue(prefix+"head_model", m_headModel);
    settings.setValue(prefix+"acquisition_mode", 3); // linetic
    settings.setValue(prefix+"trigger_mode", 0); // internal
    settings.setValue(prefix+"read_mode", ui->cmbReadMode->currentIndex());
    settings.setValue(prefix+"fileformat", ui->cmbFileFormat->currentIndex());
    settings.setValue(prefix+"exposure_time", ui->spinExposure->value()*1000.0);
    settings.setValue(prefix+"kinetic_cycle_time", ui->spinKineticCycleTime->value()*1000.0);
    settings.setValue(prefix+"kinetic_cycles", ui->spinKineticCycles->value());
    settings.setValue(prefix+"accumulate_cycles", ui->spinAccumulates->value());
    settings.setValue(prefix+"accumulate_cycles_time", ui->spinAccCycleTime->value()*1000.0);
    settings.setValue(prefix+"frame_transfer", ui->chkFrameTransfer->isChecked());
    settings.setValue(prefix+"baseline_clamp", ui->chkBaselineClamp->isChecked());
    settings.setValue(prefix+"baseline_offset", ui->spinBaselineOffset->value());
    settings.setValue(prefix+"advanced_emgain", false); // no advanced EM gain
    settings.setValue(prefix+"emgain", ui->spinEMGain->value());
    settings.setValue(prefix+"emgain_enabled", ui->chkEMGain->isChecked());
    settings.setValue(prefix+"preamp_gain", ui->cmbPreampGain->currentIndex());
    settings.setValue(prefix+"vertical_shift_speed", ui->cmbVerticalShiftSpeed->currentIndex());
    settings.setValue(prefix+"vertical_shift_amplitude", ui->cmbVerticalShiftAmplitude->currentIndex());
    settings.setValue(prefix+"horizontal_shift_speed", ui->cmbHorizontalShiftSpeed->currentIndex());
    settings.setValue(prefix+"subimage_hstart", ui->spinLeft->value()+1);
    settings.setValue(prefix+"subimage_hend", ui->spinLeft->value()+ui->spinWidth->value());
    settings.setValue(prefix+"subimage_vend", m_sensorHeight-ui->spinTop->value());
    settings.setValue(prefix+"subimage_vstart", m_sensorHeight-(ui->spinTop->value()+ui->spinHeight->value())+1);
    settings.setValue(prefix+"horizontal_binning", ui->spinHorizontalBinning->value());
    settings.setValue(prefix+"vertical_binning", ui->spinVerticalBinning->value());
    settings.setValue(prefix+"amplifier", ui->cmbAmplifier->currentIndex());
    settings.setValue(prefix+"ad_channel", ui->cmbADChannel->currentIndex());
    settings.setValue(prefix+"trigger_mode", ui->cmbTriggerMode->currentIndex());
    settings.setValue(prefix+"trigger_invert", ui->chkTriggerInvert->isChecked());

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
    qDebug()<<"updateSubregion()";
    if (m_updatingSubregion) return;
    bool old_m_updatingSensorSetup=m_updatingSubregion;
    m_updatingSubregion=true;


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

    int maxBinning;
    GetMaximumBinning(getReadMode(), 0, &maxBinning);
    ui->spinHorizontalBinning->setRange(1,maxBinning);
    GetMaximumBinning(getReadMode(), 1, &maxBinning);
    ui->spinVerticalBinning->setRange(1,maxBinning);
    updatePreview();
    calcTiming();
    m_updatingSubregion=old_m_updatingSensorSetup;
    qDebug()<<"updateSubregion() ... DONE";
}

int  AndorSettingsDialog::getReadMode() {
    if (ui->cmbReadMode->currentIndex()==1) return 0;
    else return 4;
}

int AndorSettingsDialog::getPreamp() {
    return ui->cmbPreampGain->itemData(ui->cmbPreampGain->currentIndex()).toInt();
}

void AndorSettingsDialog::resizeSubregion(int width, int height) {
    if (m_updatingSubregion) return;
    qDebug()<<"resizeSubregion()";
    QRect r=calcImageRect();
    QPoint c=r.center();

    ui->spinLeft->setValue(r.left()+r.width()/2-width/2);
    ui->spinTop->setValue(r.top()+r.height()/2-height/2);
    ui->spinWidth->setValue(width);
    ui->spinHeight->setValue(height);
    updateSubregion();
    qDebug()<<"resizeSubregion() ... DONE";
}

void AndorSettingsDialog::updateSensorSetup(int leaveout) {
    if (m_updatingSensorSetup) return;
    qDebug()<<"updateSensorSetup()";

    bool old_m_updatingSensorSetup=m_updatingSensorSetup;
    m_updatingSensorSetup=true;
    int NumPreampGains;
    int NumHSpeeds, IsPreAmpAvailable;
    float HSSpeed, PreampGain;

    float oldHSSpeed=ui->cmbHorizontalShiftSpeed->itemData(ui->cmbHorizontalShiftSpeed->currentIndex()).toFloat();
    float oldPreampGain=ui->cmbPreampGain->itemData(ui->cmbPreampGain->currentIndex()).toFloat();

    GetNumberPreAmpGains(&NumPreampGains);



    int channel=ui->cmbADChannel->currentIndex();
    int amp=ui->cmbAmplifier->currentIndex();
    if (leaveout!=1) {
        ui->cmbHorizontalShiftSpeed->clear();
        GetNumberHSSpeeds(channel, amp, &NumHSpeeds);
        for (int hs=0; hs<NumHSpeeds; hs++) {
            GetHSSpeed(channel, amp, hs, &HSSpeed);
            ui->cmbHorizontalShiftSpeed->addItem(QString("%1 MHz").arg(HSSpeed), QVariant(HSSpeed));
        }
        ui->cmbHorizontalShiftSpeed->setCurrentIndex(qMax(0, ui->cmbHorizontalShiftSpeed->findData(oldHSSpeed)));
    }

    if (leaveout!=0) {
        int hspeed=ui->cmbHorizontalShiftSpeed->currentIndex();
        ui->cmbPreampGain->clear();
        for (int gain=0; gain<NumPreampGains; gain++) {
            GetPreAmpGain(gain, &PreampGain);
            IsPreAmpGainAvailable(channel, amp, hspeed, gain, &IsPreAmpAvailable);
            if (IsPreAmpAvailable!=0) {
                ui->cmbPreampGain->addItem(QString("%1x").arg(PreampGain), QVariant(gain));
            }
        }
        ui->cmbPreampGain->setCurrentIndex(qMax(0, ui->cmbPreampGain->findData(oldPreampGain)));
    }

    m_updatingSensorSetup=old_m_updatingSensorSetup;
    qDebug()<<"updateSensorSetup() ... DONE";
}

void AndorSettingsDialog::setImage(uint32_t* image)  {
    m_image=image;
    QImage pix(m_sensorWidth, m_sensorHeight, QImage::Format_RGB32);
    if (m_image) {
        JKQTFPimagePlot_array2image<uint32_t>(image, m_sensorWidth, m_sensorHeight, pix, JKQTFP_GRAY, 0, 0);
    }
    m_pix=pix;
    updatePreview();
}

void AndorSettingsDialog::updatePreview() {
    bool old_m_updatingPreview=m_updatingPreview;
    if (m_updatingPreview) return;
    qDebug()<<"updatePreview()";
    m_updatingPreview=true;
    ui->labPlot->setText("");
    int scaleX=qMax(1, (int)floor(ui->labPlot->width()/m_sensorWidth));
    int scaleY=qMax(1, (int)floor(ui->labPlot->height()/m_sensorHeight));
    int scale=qMin(scaleX, scaleY);
    QPixmap pixmap((m_sensorWidth+16)*scale, (m_sensorHeight+16)*scale);
    pixmap.fill(QColor("lightgray"));
    QPainter painter;
    painter.begin(&pixmap);
    painter.scale(scale, scale);
    //painter.translate(8+m_pix.width()/2, 8+m_pix.height()/2);
    //painter.rotate(180);
    painter.drawImage(8,8,m_pix);
    QRect sub=calcImageRect();
    painter.setPen(QColor("red"));
    painter.drawRect(sub.translated(8,8));
    painter.end();
    ui->labPlot->setPixmap(pixmap);
    m_updatingPreview=old_m_updatingPreview;
    qDebug()<<"updatePreview() ... DONE";
}

void AndorSettingsDialog::calcTiming() {
    if (m_calcTiming) return;
    qDebug()<<"calcTiming()";
    bool old_m_calcTiming=m_calcTiming;
    m_calcTiming=true;
    selectCamera(m_camera);
    bool ok=true;
    CHECK(SetAcquisitionMode(3)); // kinetic
    if (ui->cmbReadMode->currentIndex()==2) { // cropped
        CHECK(SetReadMode(4)); // image
    } else if (ui->cmbReadMode->currentIndex()==1) { // FVB
        CHECK(SetReadMode(0)); // FVB
    } else { // imaging mode
        CHECK(SetReadMode(4)); // image
    }
    if (ui->chkFrameTransfer->isChecked()) {
        CHECK(SetFrameTransferMode(1));
    } else {
        CHECK(SetFrameTransferMode(0));
    }
    CHECK(SetADChannel(ui->cmbADChannel->currentIndex()));
    CHECK(SetOutputAmplifier(ui->cmbAmplifier->currentIndex()));
    CHECK(SetHSSpeed(ui->cmbAmplifier->currentIndex(), ui->cmbHorizontalShiftSpeed->currentIndex()));
    CHECK(SetPreAmpGain(getPreamp()));
    CHECK(SetVSAmplitude(ui->cmbVerticalShiftAmplitude->currentIndex()));
    CHECK(SetVSSpeed(ui->cmbVerticalShiftSpeed->currentIndex()));
    CHECK(SetExposureTime(ui->spinExposure->value()/1000.0));
    CHECK(SetAccumulationCycleTime(ui->spinAccCycleTime->value()/1000.0));
    CHECK(SetNumberAccumulations(ui->spinAccumulates->value()));
    CHECK(SetNumberKinetics(ui->spinKineticCycles->value()));
    CHECK(SetKineticCycleTime(ui->spinKineticCycleTime->value()/1000.0));

    if (ui->cmbReadMode->currentIndex()==2) { // cropped
        CHECK(SetIsolatedCropMode(1, ui->spinHeight->value(), ui->spinWidth->value(), ui->spinVerticalBinning->value(), ui->spinHorizontalBinning->value()));
    } else if (ui->cmbReadMode->currentIndex()==1) { // FVB
        CHECK(SetIsolatedCropMode(1, ui->spinHeight->value(), ui->spinWidth->value(), ui->spinVerticalBinning->value(), ui->spinHorizontalBinning->value()));
    } else { // imaging mode
        CHECK(SetIsolatedCropMode(0, ui->spinHeight->value(), ui->spinWidth->value(), ui->spinVerticalBinning->value(), ui->spinHorizontalBinning->value()));
    }

    qDebug()<<"SetImage: "<<ui->spinHorizontalBinning->value()<< ui->spinVerticalBinning->value()<< ui->spinLeft->value()+1<< ui->spinLeft->value()+ui->spinWidth->value()<< ui->spinTop->value()+1<< ui->spinTop->value()+ui->spinHeight->value();
    CHECK(SetImage(ui->spinHorizontalBinning->value(), ui->spinVerticalBinning->value(),
             ui->spinLeft->value()+1, ui->spinLeft->value()+ui->spinWidth->value(),
             ui->spinTop->value()+1, ui->spinTop->value()+ui->spinHeight->value()));
    float exposure=0, accumulate=0, kinetic=0;
    if (ok) {
        CHECK(GetAcquisitionTimings(&exposure, &accumulate, &kinetic));

        if (exposure!=ui->spinExposure->value()) ui->spinExposure->setValue(exposure*1000.0);
        if (accumulate!=ui->spinAccCycleTime->value()) ui->spinAccCycleTime->setValue(accumulate*1000.0);
        if (kinetic!=ui->spinKineticCycleTime->value()) ui->spinKineticCycleTime->setValue(kinetic*1000.0);

        ui->labAccumulateCycleRate->setText(QString("%1 Hz").arg(1.0/accumulate));
        ui->labKineticCycleRate->setText(QString("%1 Hz").arg(1.0/kinetic));
        ui->labError->setText("");
    } else {
        ui->labError->setText("error calculating timings ...");
    }

    m_calcTiming=old_m_calcTiming;
    qDebug()<<"calcTiming() ... done";
}














void AndorSettingsDialog::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
    updatePreview();
}

void AndorSettingsDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    updatePreview();
}

void AndorSettingsDialog::on_cmbReadMode_currentIndexChanged(int currentIndex) {
    if (m_updatingSubregion) return;
    ui->chkFrameTransfer->setEnabled(currentIndex==0);
    ui->spinAccCycleTime->setEnabled(false);
    if (currentIndex!=0) ui->chkFrameTransfer->setChecked(true);

    updateSubregion();
    updateSensorSetup();
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

void AndorSettingsDialog::on_cmbADChannel_currentIndexChanged(int currentIndex) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSensorSetup();
    calcTiming();
}

void AndorSettingsDialog::on_cmbAmplifier_currentIndexChanged(int currentIndex) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSensorSetup();
    calcTiming();
}

void AndorSettingsDialog::on_cmbHorizontalShiftSpeed_currentIndexChanged(int currentIndex) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSensorSetup(1);
    calcTiming();
}

void AndorSettingsDialog::on_cmbPreampGain_currentIndexChanged(int currentIndex) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSensorSetup(0);
    calcTiming();
}

void AndorSettingsDialog::on_cmbVerticalShiftSpeed_currentIndexChanged(int currentIndex) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSensorSetup();
    calcTiming();

}


void AndorSettingsDialog::on_spinWidth_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSubregion();
}


void AndorSettingsDialog::on_spinHeight_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSubregion();
}


void AndorSettingsDialog::on_spinLeft_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSubregion();
}


void AndorSettingsDialog::on_spinTop_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSubregion();
}


void AndorSettingsDialog::on_spinHorizontalBinning_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSubregion();
}


void AndorSettingsDialog::on_spinVerticalBinning_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    updateSubregion();
}

void AndorSettingsDialog::on_chkFrameTransfer_toggled(bool value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    calcTiming();
}

void AndorSettingsDialog::on_spinKineticCycles_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    calcTiming();
}

void AndorSettingsDialog::on_spinAccumulates_valueChanged(int value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    calcTiming();
}


void AndorSettingsDialog::on_spinExposure_valueChanged(double value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    calcTiming();
}


void AndorSettingsDialog::on_spinKineticCycleTime_valueChanged(double value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    calcTiming();
}


void AndorSettingsDialog::on_spinAccCycleTime_valueChanged(double value) {
    if (m_updatingSubregion || m_updatingSensorSetup || m_calcTiming) return;
    calcTiming();
}

void AndorSettingsDialog::on_chkBaselineClamp_toggled(bool value) {
    ui->spinBaselineOffset->setEnabled(!value);
}

void AndorSettingsDialog::on_chkEMGain_toggled(bool value) {
    ui->spinEMGain->setEnabled(!value);
}
