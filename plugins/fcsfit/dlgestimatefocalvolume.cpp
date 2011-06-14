#include "dlgestimatefocalvolume.h"
#include "tools.h"
#include "qftools.h"
#include <cmath>
#define sqr(x) ((x)*(x))

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
    spinC->setValue(settings->getQSettings()->value("fcsfitevaleditor/dlgEstVol/c", 10).toDouble());
    spinCError->setValue(settings->getQSettings()->value("fcsfitevaleditor/dlgEstVol/c_error", 0.1).toDouble());
    spinD->setValue(settings->getQSettings()->value("fcsfitevaleditor/dlgEstVol/d", 500).toDouble());
    spinDError->setValue(settings->getQSettings()->value("fcsfitevaleditor/dlgEstVol/d_error", 1).toDouble());

    labNParticles->setText(QString("%1 &plusmn; %2").arg(floattohtmlstr(roundWithError(particles, particles_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(particles_error,2), 6, true, -1).c_str()));
    labGamma->setText(QString("%1 &plusmn; %2").arg(floattohtmlstr(roundWithError(particles, particles_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(particles_error,2), 6, true, -1).c_str()));
    labTauD->setText(QString("(%1 &plusmn; %2) &mu;s").arg(floattohtmlstr(roundWithError(particles, particles_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(particles_error,2), 6, true, -1).c_str()));

    calc_from_C();
    calc_from_D();
}

void dlgEstimateFocalVolume::on_dlgEstimateFocalVolume_accepted() {
    settings->getQSettings()->setValue("fcsfitevaleditor/dlgEstVol/c", spinC->value());
    settings->getQSettings()->setValue("fcsfitevaleditor/dlgEstVol/c_error", spinCError->value());
    settings->getQSettings()->setValue("fcsfitevaleditor/dlgEstVol/d", spinD->value());
    settings->getQSettings()->setValue("fcsfitevaleditor/dlgEstVol/d_error", spinDError->value());
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
        wxy=cbrt(particles/(pow(M_PI, 3.0/2.0)*spinC->value()*gamma));
        wxy_error=0;
        labFocusparamConcentration->setText(QString("(%1 &plusmn; %2) nm").arg(floattohtmlstr(roundWithError(wxy, wxy_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(wxy_error,2), 6, true, -1).c_str()));
    }
}

void dlgEstimateFocalVolume::calc_from_D() {
    if (tabWidget->currentIndex()==0) {
        calc_from_C();
    } else {
        wxy=sqrt(4.0*tauD*spinD->value());
        wxy_error=sqrt(sqr(2.0*spinDError->value()*tauD/sqrt(4.0*spinD->value()*tauD)) + sqr(2.0*tauD_error*spinD->value()/sqrt(4.0*spinD->value()*tauD)));
        labFocusparamDiffusion->setText(QString("(%1 &plusmn; %2) nm").arg(floattohtmlstr(roundWithError(wxy, wxy_error,2), 6, true, -1).c_str()).arg(floattohtmlstr(roundError(wxy_error,2), 6, true, -1).c_str()));
    }
}

