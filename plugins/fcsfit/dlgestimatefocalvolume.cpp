#include "dlgestimatefocalvolume.h"
#include "tools.h"
#include "qftools.h"
#include <cmath>


inline double sqr(double x) { return x*x; }

dlgEstimateFocalVolume::dlgEstimateFocalVolume(ProgramOptions* settings, QWidget* parent):
    QDialog(parent)
{
    this->settings=settings;
    setupUi(this);
    particles=1;
    particles_error=0;
    has_nparticles=false;
    tauD=20;
    tauD_error=0;
    has_tauD=false;
    wxy=250;
    wxy_error=0;
}

dlgEstimateFocalVolume::~dlgEstimateFocalVolume()
{
    //dtor
}

void dlgEstimateFocalVolume::init(double particles, double particles_error, bool has_nparticles, double tauD, double tauD_error, bool has_tauD, double gamma, double gamma_error, bool has_gamma) {
    this->particles = particles;
    this->particles_error = particles_error;
    this->has_nparticles = has_nparticles;
    this->tauD = tauD;
    this->tauD_error = tauD_error;
    this->has_tauD = has_tauD;
    this->gamma=gamma;
    this->gamma_error=gamma_error;
    this->has_gamma=has_gamma;

    tabWidget->setTabEnabled(0, has_nparticles&&has_gamma);
    tabWidget->setTabEnabled(1, has_tauD);
    settings->getQSettings()->sync();
    spinC->setValue(settings->getQSettings()->value("fcsfitevaleditor/estimate_focal_volume.c", 10).toDouble());
    spinCError->setValue(settings->getQSettings()->value("fcsfitevaleditor/estimate_focal_volume.c_error", 0.1).toDouble());
    spinD->setValue(settings->getQSettings()->value("fcsfitevaleditor/estimate_focal_volume.d", 500).toDouble());
    spinDError->setValue(settings->getQSettings()->value("fcsfitevaleditor/estimate_focal_volume.d_error", 1).toDouble());

    labNParticles->setText(QString("%1 &plusmn; %2").arg(floattohtmlstr(roundWithError(particles, particles_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(particles_error,2), 6, true, -1).c_str()));
    labGamma->setText(QString("%1 &plusmn; %2").arg(floattohtmlstr(roundWithError(gamma, gamma_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(gamma_error,2), 6, true, -1).c_str()));
    labTauD->setText(QString("(%1 &plusmn; %2) &mu;s").arg(floattohtmlstr(roundWithError(tauD, tauD_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(tauD_error,2), 6, true, -1).c_str()));

    tabWidget->setCurrentIndex(0);
    calc_from_C();
    tabWidget->setCurrentIndex(1);
    calc_from_D();
    tabWidget->setCurrentIndex(settings->getQSettings()->value("fcsfitevaleditor/estimate_focal_volume.last_tab", 1).toInt());

}

void dlgEstimateFocalVolume::on_buttonBox_accepted() {
    settings->getQSettings()->setValue("fcsfitevaleditor/estimate_focal_volume.c", spinC->value());
    settings->getQSettings()->setValue("fcsfitevaleditor/estimate_focal_volume.c_error", spinCError->value());
    settings->getQSettings()->setValue("fcsfitevaleditor/estimate_focal_volume.d", spinD->value());
    settings->getQSettings()->setValue("fcsfitevaleditor/estimate_focal_volume.d_error", spinDError->value());
    settings->getQSettings()->setValue("fcsfitevaleditor/estimate_focal_volume.last_tab", tabWidget->currentIndex());
    settings->getQSettings()->sync();
    calc_from_C();
    calc_from_D();
}

void dlgEstimateFocalVolume::on_spinC_valueChanged(double d) {
    calc_from_C();
}

void dlgEstimateFocalVolume::on_spinCError_valueChanged(double d) {
    calc_from_C();
}

void dlgEstimateFocalVolume::on_spinD_valueChanged(double d) {
    calc_from_D();
}

void dlgEstimateFocalVolume::on_spinDError_valueChanged(double d) {
    calc_from_D();
}

void dlgEstimateFocalVolume::calc_from_C() {
    if (tabWidget->currentIndex()==1) {
        calc_from_D();
    } else {
        double C=spinC->value();
        double Ce=spinCError->value();
        double p32=sqrt(M_PI*M_PI*M_PI);
        double h = 1.0e10/6.022/p32/C/gamma;
        double h23=cbrt(sqr(particles*h)); // ^2/3
        wxy=cbrt(particles*1.0e10/6.022/p32/C/gamma);
        wxy_error=sqrt(sqr(particles_error*h/3.0/h23)+sqr(gamma_error*particles*h/gamma/h23/3.0)+sqr(Ce*particles*h/3.0/C/h23));
        labFocusparamConcentration->setText(QString("(%1 &plusmn; %2) nm").arg(floattohtmlstr(roundWithError(wxy, wxy_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(wxy_error,2), 6, true, -1).c_str()));
    }
}

void dlgEstimateFocalVolume::calc_from_D() {
    if (tabWidget->currentIndex()==0) {
        calc_from_C();
    } else {
        double D=spinD->value();
        double De=spinDError->value();
        wxy=sqrt(4.0*tauD*D*1.0e-6)*1000.0;
        //wxy_error=sqrt(sqr(spinDError->value()*tauD/sqrt(spinD->value()*tauD)) + sqr(tauD_error*spinD->value()/sqrt(spinD->value()*tauD)));
        wxy_error=sqrt(tauD*De*De*1.0e-6/D + D*tauD_error*tauD_error*1.0e-6/tauD)*1000.0;
        labFocusparamDiffusion->setText(QString("(%1 &plusmn; %2) nm").arg(floattohtmlstr(roundWithError(wxy, wxy_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(wxy_error,2), 6, true, -1).c_str()));
    }
}

