#include "cam_testcamera.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>

QFECamTestCamera::QFECamTestCamera(QObject* parent):
    QObject(parent)
{
    logService=NULL;
    conn[0]=conn[1]=false;
    counter[0]=counter[1]=0;
    width[0]=width[1]=100;
    height[0]=height[1]=100;
    testpattern[0]=testpattern[1]=0;
    noise[0]=noise[1]=0.1;
    particleN[0]=particleN[1]=0;
    particleX[0]=particleX[1]=NULL;
    particleY[0]=particleY[1]=NULL;
    initParticles(0, 10);
    initParticles(1, 10);
    particleBrightnes[0]=particleBrightnes[1]=10;
    particlePSF[0]=particlePSF[1]=2;
    particleBackground[0]=particleBackground[1]=0.2;
    srand(time(0));
    hotpixels[0]=hotpixels[1]=0;

}

QFECamTestCamera::~QFECamTestCamera()
{
    if (particleX[0]) free(particleX[0]);
    if (particleY[0]) free(particleY[0]);
    if (particleX[1]) free(particleX[1]);
    if (particleY[1]) free(particleY[1]);
    particleX[0]=particleY[0]=particleX[1]=particleY[1]=NULL;
}


void QFECamTestCamera::deinit() {
}

void QFECamTestCamera::projectChanged(QFProject* oldProject, QFProject* project) {
}

void QFECamTestCamera::initExtension() {
}

void QFECamTestCamera::loadSettings(ProgramOptions* settingspo) {
    QSettings& settings=*(settingspo->getQSettings());

    width[0]=settings.value("testdevice/width0", width[0]).toInt();
    height[0]=settings.value("testdevice/height0", height[0]).toInt();
    testpattern[0]=settings.value("testdevice/testpattern0", testpattern[0]).toInt();
    noise[0]=settings.value("testdevice/noise0", noise[0]).toDouble();
    particleN[0]=settings.value("testdevice/particle_n0", particleN[0]).toInt();
    particleBrightnes[0]=settings.value("testdevice/particle_brightnes0", particleBrightnes[0]).toDouble();
    particlePSF[0]=settings.value("testdevice/particle_psf0", particlePSF[0]).toDouble();
    particleBackground[0]=settings.value("testdevice/particle_background0", particleBackground[0]).toDouble();
    hotpixels[0]=settings.value("testdevice/hotpixels0", particleBackground[0]).toInt();

    initParticles(0, 10);


    if (width[0]<=0) width[0]=100;
    if (height[0]<=0) height[0]=100;

    emit displayParticleN0(particleN[0]);
    emit displayParticleBrightnes0(particleBrightnes[0]);
    emit displayNoise0(noise[0]);
    emit displayWidth0(width[0]);
    emit displayHeight0(height[0]);
    emit displayParticlePSF0(particlePSF[0]);
    emit displayParticleBackground0(particleBackground[0]);
    emit displayTestPattern0(testpattern[0]);
    emit displayHotpixels0(hotpixels[0]);


    width[1]=settings.value("testdevice/width1", width[1]).toInt();
    height[1]=settings.value("testdevice/height1", height[1]).toInt();
    testpattern[1]=settings.value("testdevice/testpattern1", testpattern[1]).toInt();
    noise[1]=settings.value("testdevice/noise1", noise[1]).toDouble();
    particleN[1]=settings.value("testdevice/particle_n1", particleN[1]).toInt();
    particleBrightnes[1]=settings.value("testdevice/particle_brightnes1", particleBrightnes[1]).toDouble();
    particlePSF[1]=settings.value("testdevice/particle_psf1", particlePSF[1]).toDouble();
    particleBackground[1]=settings.value("testdevice/particle_background1", particleBackground[1]).toDouble();
    hotpixels[1]=settings.value("testdevice/hotpixels1", particleBackground[1]).toInt();

    initParticles(0, 10);


    if (width[1]<=0) width[1]=100;
    if (height[1]<=0) height[1]=100;

    emit displayParticleN1(particleN[1]);
    emit displayParticleBrightnes1(particleBrightnes[1]);
    emit displayNoise1(noise[1]);
    emit displayWidth1(width[1]);
    emit displayHeight1(height[1]);
    emit displayParticlePSF1(particlePSF[1]);
    emit displayParticleBackground1(particleBackground[1]);
    emit displayTestPattern1(testpattern[1]);
    emit displayHotpixels1(hotpixels[1]);
}

void QFECamTestCamera::storeSettings(ProgramOptions* settingspo) {
    QSettings& settings=*(settingspo->getQSettings());

    settings.setValue("testdevice/width0", width[0]);
    settings.setValue("testdevice/height0", height[0]);
    settings.setValue("testdevice/testpattern0", testpattern[0]);
    settings.setValue("testdevice/noise0", noise[0]);
    settings.setValue("testdevice/particle_n0", particleN[0]);
    settings.setValue("testdevice/particle_brightnes0", particleBrightnes[0]);
    settings.setValue("testdevice/particle_psf0", particlePSF[0]);
    settings.setValue("testdevice/particle_background0", particleBackground[0]);
    settings.setValue("testdevice/hotpixels0", hotpixels[0]);

    settings.setValue("testdevice/width1", width[1]);
    settings.setValue("testdevice/height1", height[1]);
    settings.setValue("testdevice/testpattern1", testpattern[1]);
    settings.setValue("testdevice/noise1", noise[1]);
    settings.setValue("testdevice/particle_n1", particleN[1]);
    settings.setValue("testdevice/particle_brightnes1", particleBrightnes[1]);
    settings.setValue("testdevice/particle_psf1", particlePSF[1]);
    settings.setValue("testdevice/particle_background1", particleBackground[1]);
    settings.setValue("testdevice/hotpixels1", hotpixels[1]);
}

unsigned int QFECamTestCamera::getCameraCount() {
    return 2;
}

void QFECamTestCamera::useCameraSettings(unsigned int camera, const QSettings& settings) {
}

void QFECamTestCamera::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
    QDialog* dlg=new QDialog(parent);

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QSpinBox* spin;
    QWidget* r=new QWidget(dlg);
    QFormLayout* fl=new QFormLayout(r);
    r->setLayout(fl);
    QComboBox* cmb=new QComboBox(r);
    cmb->addItem(tr("many moving rings"));
    cmb->addItem(tr("blinking rings"));
    cmb->addItem(tr("few moving rings"));
    cmb->addItem(tr("particles"));
    cmb->setCurrentIndex(testpattern[camera]);
    fl->addRow(tr("pattern:"), cmb);
    if (camera==1) {
        connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setTestPattern1(int)));
        connect(this, SIGNAL(displayTestPattern1(int)), cmb, SLOT(setCurrentIndex(int)));
    } else {
        connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setTestPattern0(int)));
        connect(this, SIGNAL(displayTestPattern0(int)), cmb, SLOT(setCurrentIndex(int)));
    }



    QDoubleSpinBox* spind=new QDoubleSpinBox(r);
    fl->addRow(tr("noise:"), spind);
    spind->setMinimum(0);
    spind->setMaximum(10);
    spind->setValue(noise[camera]);
    if (camera==1) {
        connect(spind, SIGNAL(valueChanged(double)), this, SLOT(setNoise1(double)));
        connect(this, SIGNAL(displayNoise1(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableDefault1(bool)), spind, SLOT(setEnabled(bool)));
    } else {
        connect(spind, SIGNAL(valueChanged(double)), this, SLOT(setNoise0(double)));
        connect(this, SIGNAL(displayNoise0(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableDefault0(bool)), spind, SLOT(setEnabled(bool)));
    }

    spin=new QSpinBox(r);
    fl->addRow(tr("# particles:"), spin);
    spin->setMinimum(1);
    spin->setMaximum(1000);
    spin->setValue(particleN[camera]);
    if (camera==1) {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setParticleN1(int)));
        connect(this, SIGNAL(displayParticleN1(int)), spin, SLOT(setValue(int)));
        connect(this, SIGNAL(enableParticles1(bool)), spin, SLOT(setEnabled(bool)));
    } else {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setParticleN0(int)));
        connect(this, SIGNAL(displayParticleN0(int)), spin, SLOT(setValue(int)));
        connect(this, SIGNAL(enableParticles0(bool)), spin, SLOT(setEnabled(bool)));
    }

    spind=new QDoubleSpinBox(r);
    fl->addRow(tr("particle brightnes:"), spind);
    spind->setMinimum(0);
    spind->setMaximum(100000);
    spind->setValue(particleBrightnes[camera]);
    if (camera==1) {
        connect(spind, SIGNAL(valueChanged(double)), this, SLOT(setParticleBrightnes1(double)));
        connect(this, SIGNAL(displayParticleBrightnes1(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableParticles1(bool)), spind, SLOT(setEnabled(bool)));
    } else {
        connect(this, SIGNAL(displayParticleBrightnes0(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableParticles0(bool)), spind, SLOT(setEnabled(bool)));
        connect(this, SIGNAL(enableParticles0(bool)), spind, SLOT(setEnabled(bool)));
    }

    spind=new QDoubleSpinBox(r);
    fl->addRow(tr("particle PSF:"), spind);
    spind->setMinimum(0);
    spind->setMaximum(100000);
    spind->setValue(particlePSF[camera]);
    if (camera==1) {
        connect(spind, SIGNAL(valueChanged(double)), this, SLOT(setParticlePSF1(double)));
        connect(this, SIGNAL(displayParticlePSF1(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableParticles1(bool)), spind, SLOT(setEnabled(bool)));
    } else {
        connect(spind, SIGNAL(valueChanged(double)), this, SLOT(setParticlePSF0(double)));
        connect(this, SIGNAL(displayParticlePSF0(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableParticles0(bool)), spind, SLOT(setEnabled(bool)));
    }

    spind=new QDoubleSpinBox(r);
    fl->addRow(tr("particle background:"), spind);
    spind->setMinimum(0);
    spind->setMaximum(100000);
    spind->setValue(particleBackground[camera]);
    if (camera==1) {
        connect(spind, SIGNAL(valueChanged(double)), this, SLOT(setParticleBackground1(double)));
        connect(this, SIGNAL(displayParticleBackground1(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableParticles1(bool)), spind, SLOT(setEnabled(bool)));
    } else {
        connect(spind, SIGNAL(valueChanged(double)), this, SLOT(setParticleBackground0(double)));
        connect(this, SIGNAL(displayParticleBackground0(double)), spind, SLOT(setValue(double)));
        connect(this, SIGNAL(enableParticles0(bool)), spind, SLOT(setEnabled(bool)));
    }



    spin=new QSpinBox(r);
    fl->addRow(tr("width:"), spin);
    spin->setMinimum(10);
    spin->setMaximum(10000);
    spin->setValue(width[camera]);
    if (camera==1) {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setWidth1(int)));
        connect(this, SIGNAL(displayWidth1(int)), spin, SLOT(setValue(int)));
    } else {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setWidth0(int)));
        connect(this, SIGNAL(displayWidth0(int)), spin, SLOT(setValue(int)));
    }


    spin=new QSpinBox(r);
    fl->addRow(tr("height:"), spin);
    spin->setMinimum(10);
    spin->setMaximum(10000);
    spin->setValue(height[camera]);
    if (camera==1) {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setHeight1(int)));
        connect(this, SIGNAL(displayHeight1(int)), spin, SLOT(setValue(int)));
    } else {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setHeight0(int)));
        connect(this, SIGNAL(displayHeight0(int)), spin, SLOT(setValue(int)));
    }

    spin=new QSpinBox(r);
    fl->addRow(tr("hotpixels:"), spin);
    spin->setMinimum(0);
    spin->setMaximum(1000);
    spin->setValue(hotpixels[camera]);
    if (camera==1) {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setHotpixels1(int)));
        connect(this, SIGNAL(displayHotpixels1(int)), spin, SLOT(setValue(int)));
    } else {
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(setHotpixels0(int)));
        connect(this, SIGNAL(displayHotpixels0(int)), spin, SLOT(setValue(int)));
    }


    cmb->setCurrentIndex(testpattern[camera]);


    lay->addWidget(r);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    dlg->exec();
    delete dlg;
}

int QFECamTestCamera::getImageWidth(unsigned int camera) {
    return width[camera];
}

int QFECamTestCamera::getImageHeight(unsigned int camera) {
    return height[camera];
}

bool QFECamTestCamera::isConnected(unsigned int camera) {
    return conn[camera];
}

double Exponential(double m)
/* =========================================================
 * Returns an exponentially distributed positive real number.
 * NOTE: use m > 0.0
 * =========================================================
 */
{
  return (-m * log(1.0 - (double)rand()/(double)RAND_MAX));
}

long Poisson(double m)
/* ==================================================
 * Returns a Poisson distributed non-negative integer.
 * NOTE: use m > 0
 * ==================================================
 */
{
  double t = 0.0;
  long   x = 0;

  while (t < m) {
    t += Exponential(1.0);
    x++;
  }
  return (x - 1);
}

bool QFECamTestCamera::acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp) {
    //std::cout<<"capturing single frame to "<<data<<"   timestamp="<<timestamp<<std::endl;
    if (timestamp!=NULL) {
        //std::cout<<"difftime(startTime, time(0))="<<difftime(startTime, time(0))<<std::endl;
        *timestamp=(uint64_t)round(timer[camera].get_time());
    }

    if (testpattern[camera]==0) {
        for (int y=0; y<getImageHeight(camera); y++) {
            for (int x=0; x<getImageWidth(camera); x++) {
                double r=(double)rand()/(double)RAND_MAX;
                double xx=(double)((double)x-(double)getImageWidth(camera)/2.0)/(double)getImageWidth(camera);
                double yy=(double)((double)y-(double)getImageHeight(camera)/2.0)/(double)getImageHeight(camera);
                data[y*getImageWidth(camera)+x]=noise[camera]*r*200.0+abs(200.0*sin(sqrt(xx*xx+yy*yy)*20.0*M_PI+(double)counter[camera]/10.0*M_PI));
            }
        }
    } else if (testpattern[camera]==1) {
        for (int y=0; y<getImageHeight(camera); y++) {
            for (int x=0; x<getImageWidth(camera); x++) {
                double r=(double)rand()/(double)RAND_MAX;
                double xx=(double)((double)x-(double)getImageWidth(camera)/2.0)/(double)getImageWidth(camera);
                double yy=(double)((double)y-(double)getImageHeight(camera)/2.0)/(double)getImageHeight(camera);
                data[y*getImageWidth(camera)+x]=noise[camera]*r*200.0+abs(200.0*sin(sqrt(xx*xx+yy*yy)*2.0*M_PI)*sin((double)counter[camera]/10.0*M_PI));
            }
        }
    } else if (testpattern[camera]==2) {
        for (int y=0; y<getImageHeight(camera); y++) {
            for (int x=0; x<getImageWidth(camera); x++) {
                double r=(double)rand()/(double)RAND_MAX;
                double xx=(double)((double)x-(double)getImageWidth(camera)/2.0)/(double)getImageWidth(camera);
                double yy=(double)((double)y-(double)getImageHeight(camera)/2.0)/(double)getImageHeight(camera);
                data[y*getImageWidth(camera)+x]=noise[camera]*r*200.0+abs(200.0*sin(sqrt(xx*xx+yy*yy)*2.0*M_PI+(double)counter[camera]/10.0*M_PI));
            }
        }
    } else {
        stepParticles(camera);
        for (int y=0; y<getImageHeight(camera); y++) {
            for (int x=0; x<getImageWidth(camera); x++) {
                double v=particleBackground[camera];
                for (int i=0; i<particleN[camera]; i++) {
                    double r=((double)x-(double)particleX[camera][i])*((double)x-(double)particleX[camera][i]) + ((double)y-(double)particleY[camera][i])*((double)y-(double)particleY[camera][i]);
                    v=v+exp(-0.5*r/(particlePSF[camera]*particlePSF[camera]))*particleBrightnes[camera];
                }

                data[y*getImageWidth(camera)+x]=Poisson(v);
            }
        }
    }
    if (hotpixels[camera]>0) {
        // generate some hot pixels by use of a linear congruency random number generator
        // with given initial and seed values
        int a=106;
        int m=6075;
        int c=1283;
        int x=1234*(camera+1);
        for (int i=0; i<hotpixels[camera]; i++) {
            x=(a*x+c) % m;
            int xx=(double)x/(double)m*width[camera];
            x=(a*x+c) % m;
            int yy=(double)x/(double)m*height[camera];
            x=(a*x+c) % m;
            data[yy*getImageWidth(camera)+xx]=1000+(double)x/(double)m*100;
        }
    }
    counter[camera]++;

    return true;
}

bool QFECamTestCamera::connectDevice(unsigned int camera) {
    startTime[camera]=time(0);
    timer[camera].start();
    conn[camera]=true;
    counter[camera]=0;
    return conn[camera];
}

void QFECamTestCamera::disconnectDevice(unsigned int camera) {
    conn[camera] = false;
}

double QFECamTestCamera::getExposureTime(unsigned int camera) {
    if (camera==0) return 1;
    return 1e-3;
}




void QFECamTestCamera::initParticles(int camera, int n) {
    this->particleN[camera]=n;
    if (particleX[camera]) free(particleX[camera]);
    if (particleY[camera]) free(particleY[camera]);
    particleX[camera]=(int*)malloc(particleN[camera]*sizeof(int));
    particleY[camera]=(int*)malloc(particleN[camera]*sizeof(int));

    for (int i=0; i<n; i++) {
        particleX[camera][i]=(int)round((double)rand()/(double)RAND_MAX*(double)width[camera]*1.25)-width[camera]/8;
        particleY[camera][i]=(int)round((double)rand()/(double)RAND_MAX*(double)height[camera]*1.25)-height[camera]/8;
    }
}

void QFECamTestCamera::stepParticles(int camera) {
    for (int i=0; i<particleN[camera]; i++) {
        double r=(double)rand()/(double)RAND_MAX;
        if (r<1.0/4.0) particleX[camera][i]+=2;
        else if (r<2.0/4.0) particleX[camera][i]-=2;
        else if (r<3.0/4.0) particleY[camera][i]+=2;
        else particleY[camera][i]-=2;

        if (particleX[camera][i]<-width[camera]/8.0) particleX[camera][i]=9.0*width[camera]/8.0;
        if (particleX[camera][i]>9.0*width[camera]/8.0) particleX[camera][i]=-width[camera]/8.0;
        if (particleY[camera][i]<-height[camera]/8.0) particleY[camera][i]=9.0*height[camera]/8.0;
        if (particleY[camera][i]>9.0*height[camera]/8.0) particleY[camera][i]=-height[camera]/8.0;
    }
}






void QFECamTestCamera::setWidth0(int width) {
    this->width[0]=width;
}
void QFECamTestCamera::setHeight0(int height) {
    this->height[0]=height;
}
void QFECamTestCamera::setTestPattern0(int pattern) {
    this->testpattern[0]=pattern;
}
void QFECamTestCamera::setNoise0(double n) {
    this->noise[0]=n;
}
void QFECamTestCamera::setParticleN0(int n) {
    initParticles(0, n);
}
void QFECamTestCamera::setParticleBrightnes0(double b) {
    this->particleBrightnes[0]=b;
}
void QFECamTestCamera::setParticleBackground0(double b) {
    this->particleBackground[0]=b;
}
void QFECamTestCamera::setParticlePSF0(double psf) {
    this->particlePSF[0]=psf;
}
void QFECamTestCamera::setHotpixels0(int num) {
    this->hotpixels[0]=num;
}


void QFECamTestCamera::setWidth1(int width) {
    this->width[1]=width;
}
void QFECamTestCamera::setHeight1(int height) {
    this->height[1]=height;
}
void QFECamTestCamera::setTestPattern1(int pattern) {
    this->testpattern[1]=pattern;
}
void QFECamTestCamera::setNoise1(double n) {
    this->noise[1]=n;
}
void QFECamTestCamera::setParticleN1(int n) {
    initParticles(1, n);
}
void QFECamTestCamera::setParticleBrightnes1(double b) {
    this->particleBrightnes[1]=b;
}
void QFECamTestCamera::setParticleBackground1(double b) {
    this->particleBackground[1]=b;
}
void QFECamTestCamera::setParticlePSF1(double psf) {
    this->particlePSF[1]=psf;
}
void QFECamTestCamera::setHotpixels1(int num) {
    this->hotpixels[1]=num;
}




void QFECamTestCamera::log_text(QString message) {
	if (logService) logService->log_text(message);
	else if (services) services->log_text(message);
}

void QFECamTestCamera::log_warning(QString message) {
	if (logService) logService->log_warning(message);
	else if (services) services->log_warning(message);
}

void QFECamTestCamera::log_error(QString message) {
	if (logService) logService->log_error(message);
	else if (services) services->log_error(message);
}


Q_EXPORT_PLUGIN2(cam_testcamera, QFECamTestCamera)
