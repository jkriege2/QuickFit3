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
    seriesAcquisitions=20;
    seriesDelay=250;
    seriesCount[0]=seriesCount[1]=0;
    seriesRunning[0]=seriesRunning[1]=false;
    seriesFilenamePrefix[0]=seriesFilenamePrefix[1]="";

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

}

void QFECamTestCamera::storeSettings(ProgramOptions* settingspo) {
    QSettings& settings=*(settingspo->getQSettings());

    /*settings.setValue("testdevice/width0", width[0]);
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
    settings.setValue("testdevice/hotpixels1", hotpixels[1]);*/
}

unsigned int QFECamTestCamera::getCameraCount() {
    return 2;
}

void QFECamTestCamera::useCameraSettings(unsigned int camera, const QSettings& settings) {

    width[camera]=               settings.value(QString("testdevice/width"), 100).toInt();
    height[camera]=              settings.value(QString("testdevice/height"), 100).toInt();
    testpattern[camera]=         settings.value(QString("testdevice/testpattern"), 0).toInt();
    noise[camera]=               settings.value(QString("testdevice/noise"), 0.1).toDouble();
    particleN[camera]=           settings.value(QString("testdevice/particle_n"), 5).toInt();
    particleBrightnes[camera]=   settings.value(QString("testdevice/particle_brightnes"), 10).toDouble();
    particlePSF[camera]=         settings.value(QString("testdevice/particle_psf"), 2).toDouble();
    particleBackground[camera]=  settings.value(QString("testdevice/particle_background"), 0.2).toDouble();
    hotpixels[camera]=           settings.value(QString("testdevice/hotpixels"), 0).toInt();

    initParticles(camera, particleN[camera]);


    if (width[camera]<=0) width[camera]=100;
    if (height[camera]<=0) height[camera]=100;



}

bool QFECamTestCamera::prepareAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix) {
    useCameraSettings(camera, settings);
    seriesFilenamePrefix[camera]=QString("%3%1_cam%2.tif").arg(getID()).arg(camera).arg(filenamePrefix);
    return true;
}


void QFECamTestCamera::showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent) {
    QDialog* dlg=new QDialog(parent);
    dlg->setWindowTitle(tr("Configuration for Testcamera #%1").arg(camera));

    QVBoxLayout* lay=new QVBoxLayout(dlg);
    dlg->setLayout(lay);

    QWidget* r=new QWidget(dlg);
    QFormLayout* fl=new QFormLayout(r);
    r->setLayout(fl);
    QComboBox* cmbPattern=new QComboBox(r);
    cmbPattern->addItem(tr("many moving rings"));
    cmbPattern->addItem(tr("blinking rings"));
    cmbPattern->addItem(tr("few moving rings"));
    cmbPattern->addItem(tr("particles"));
    cmbPattern->setCurrentIndex(settings.value(QString("testdevice/testpattern"), testpattern[camera]).toInt());
    fl->addRow(tr("pattern:"), cmbPattern);

    //

    QDoubleSpinBox* spindNoise=new QDoubleSpinBox(r);
    fl->addRow(tr("noise:"), spindNoise);
    spindNoise->setMinimum(0);
    spindNoise->setMaximum(10);
    spindNoise->setValue(settings.value(QString("testdevice/noise"), noise[camera]).toInt());

    QSpinBox* spinParticles=new QSpinBox(r);
    fl->addRow(tr("# particles:"), spinParticles);
    spinParticles->setMinimum(1);
    spinParticles->setMaximum(1000);
    spinParticles->setValue(settings.value(QString("testdevice/particle_n"), particleN[camera]).toInt());

    QDoubleSpinBox* spindBrightnes=new QDoubleSpinBox(r);
    fl->addRow(tr("particle brightnes:"), spindBrightnes);
    spindBrightnes->setMinimum(0);
    spindBrightnes->setMaximum(100000);
    spindBrightnes->setValue(settings.value(QString("testdevice/particle_brightnes"), particleBrightnes[camera]).toDouble());

    QDoubleSpinBox* spindPSF=new QDoubleSpinBox(r);
    fl->addRow(tr("particle PSF:"), spindPSF);
    spindPSF->setMinimum(0);
    spindPSF->setMaximum(100000);
    spindPSF->setValue(settings.value(QString("testdevice/particle_psf"), particlePSF[camera]).toDouble());

    QDoubleSpinBox* spindBackground=new QDoubleSpinBox(r);
    fl->addRow(tr("particle background:"), spindBackground);
    spindBackground->setMinimum(0);
    spindBackground->setMaximum(100000);
    spindBackground->setValue(settings.value(QString("testdevice/particle_background"), particleBackground[camera]).toDouble());

    QSpinBox* spinWidth=new QSpinBox(r);
    fl->addRow(tr("width:"), spinWidth);
    spinWidth->setMinimum(10);
    spinWidth->setMaximum(10000);
    spinWidth->setValue(settings.value(QString("testdevice/width"), width[camera]).toInt());

    QSpinBox* spinHeight=new QSpinBox(r);
    fl->addRow(tr("height:"), spinHeight);
    spinHeight->setMinimum(10);
    spinHeight->setMaximum(10000);
    spinHeight->setValue(settings.value(QString("testdevice/height"), height[camera]).toInt());

    QSpinBox* spinHotpixels=new QSpinBox(r);
    fl->addRow(tr("hotpixels:"), spinHotpixels);
    spinHotpixels->setMinimum(0);
    spinHotpixels->setMaximum(1000);
    spinHotpixels->setValue(settings.value(QString("testdevice/hotpixels"), particleBackground[camera]).toInt());


    lay->addWidget(r);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dlg);
    lay->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dlg, SLOT(reject()));

    if ( dlg->exec()==QDialog::Accepted ) {
        settings.setValue(QString("testdevice/testpattern"), cmbPattern->currentIndex());
        settings.setValue(QString("testdevice/noise"), spindNoise->value());
        settings.setValue(QString("testdevice/particle_n"), spinParticles->value());
        settings.setValue(QString("testdevice/particle_brightnes"), spindBrightnes->value());
        settings.setValue(QString("testdevice/particle_psf"), spindPSF->value());
        settings.setValue(QString("testdevice/particle_background"), spindBackground->value());
        settings.setValue(QString("testdevice/width"), spinWidth->value());
        settings.setValue(QString("testdevice/height"), spinHeight->value());
        settings.setValue(QString("testdevice/hotpixels"), spinHotpixels->value());
    }
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


bool QFECamTestCamera::startAcquisition(unsigned int camera) {
    seriesRunning[camera]=false;
    tif[camera]=TIFFOpen(seriesFilenamePrefix[camera].toAscii().data(), "w");
    if (tif[camera]==NULL) return false;

    seriesRunning[camera]=true;
    seriesCount[camera]=0;

    if (camera==0) QTimer::singleShot(0, this, SLOT(seriesStep1()));
    if (camera==1) QTimer::singleShot(1, this, SLOT(seriesStep2()));

    return true;
}

void QFECamTestCamera::cancelAcquisition(unsigned int camera) {
    seriesRunning[camera]=false;
}

bool QFECamTestCamera::isAcquisitionRunning(unsigned int camera, double* percentageDone) {
    return seriesRunning[camera];
}

void QFECamTestCamera::getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters) {
    if (files) files->append(seriesFilenamePrefix[camera]);
    if (parameters) {
        (*parameters)["images"]=seriesCount[camera];
        (*parameters)["fileformat"]="TIFF, 8-bit";
    }
}

bool QFECamTestCamera::getAcquisitionPreview(unsigned int camera, uint32_t* data) {
    return false;
}

int QFECamTestCamera::getAcquisitionProgress(unsigned int camera) {
    if (seriesRunning[camera]) {
        return seriesCount[camera]*100/seriesAcquisitions;
    } else {
        return 0;
    }
}

void QFECamTestCamera::seriesStep1() {
    int camera=0;

    uint16 frame_samplesperpixel=1;
    uint16 frame_bitspersample=8;
    uint16 frame_sampleformat = SAMPLEFORMAT_UINT;
    uint32 frame_width=getImageWidth(camera);
    uint32 frame_height=getImageHeight(camera);
    uint32 rowsperstrip = (uint32)-1;

    uint32* frame32 = (uint32*)calloc(frame_width*frame_height, sizeof(uint32));
    uint32 frame_min=0xFFFFFFFF;
    uint32 frame_max=0;
    acquire(camera, frame32);
    for (register int i=0; i<frame_width*frame_height; i++) {
        register uint32 v=frame32[i];
        if (v<frame_min) frame_min=v;
        if (v>frame_max) frame_max=v;
    }
    uint8* frame = (uint8*)malloc(frame_width*frame_height*sizeof(uint8));
    for (register int i=0; i<frame_width*frame_height; i++) {
        register uint64_t v=(frame32[i]-frame_min)*255/(frame_max-frame_min);
        if (v>255) v=255;
        frame[i]=v;
    }


    TIFFSetField(tif[camera], TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif[camera], TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif[camera], TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif[camera], TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif[camera], TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif[camera], TIFFTAG_BITSPERSAMPLE, frame_bitspersample);
    TIFFSetField(tif[camera], TIFFTAG_SAMPLESPERPIXEL, frame_samplesperpixel);
    rowsperstrip = TIFFDefaultStripSize(tif[camera], rowsperstrip);
    TIFFSetField(tif[camera], TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif[camera], TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif[camera],TIFFTAG_SAMPLEFORMAT,frame_sampleformat);
    TIFFSetField(tif[camera],TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint8* const buf = (uint8*)_TIFFmalloc(TIFFStripSize(tif[camera])); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32 nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif[camera],row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (uint8)frame[cc+(row + rr)*frame_width];
            }
        }
        TIFFWriteEncodedStrip(tif[camera],strip,buf,bi*sizeof(uint8));
    }
    _TIFFfree(buf);
    // write current directory, i.e. start new image
    TIFFWriteDirectory(tif[camera]);


    free(frame);
    free(frame32);

    seriesCount[camera]++;
    seriesRunning[camera] = seriesRunning[camera] && (seriesCount[camera]<seriesAcquisitions);

    if (seriesRunning[camera]) {
        QTimer::singleShot(seriesDelay, this,SLOT(seriesStep1()));
    } else {
        TIFFClose(tif[camera]);
    }
}

void QFECamTestCamera::seriesStep2() {
    int camera=1;

    uint16 frame_samplesperpixel=1;
    uint16 frame_bitspersample=8;
    uint16 frame_sampleformat = SAMPLEFORMAT_UINT;
    uint32 frame_width=getImageWidth(camera);
    uint32 frame_height=getImageHeight(camera);
    uint32 rowsperstrip = (uint32)-1;

    uint32* frame32 = (uint32*)calloc(frame_width*frame_height, sizeof(uint32));
    uint32 frame_min=0xFFFFFFFF;
    uint32 frame_max=0;
    acquire(camera, frame32);
    for (register int i=0; i<frame_width*frame_height; i++) {
        register uint32 v=frame32[i];
        if (v<frame_min) frame_min=v;
        if (v>frame_max) frame_max=v;
    }
    //std::cout<<"frame_min="<<frame_min<<"   frame_max="<<frame_max<<std::endl;
    uint8* frame = (uint8*)malloc(frame_width*frame_height*sizeof(uint8));
    for (register int i=0; i<frame_width*frame_height; i++) {
        register uint64_t v=(frame32[i]-frame_min)*255/(frame_max-frame_min);
        if (v>255) v=255;
        frame[i]=v;
    }


    TIFFSetField(tif[camera], TIFFTAG_IMAGEWIDTH, frame_width);
    TIFFSetField(tif[camera], TIFFTAG_IMAGELENGTH, frame_height);
    TIFFSetField(tif[camera], TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif[camera], TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif[camera], TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif[camera], TIFFTAG_BITSPERSAMPLE, frame_bitspersample);
    TIFFSetField(tif[camera], TIFFTAG_SAMPLESPERPIXEL, frame_samplesperpixel);
    rowsperstrip = TIFFDefaultStripSize(tif[camera], rowsperstrip);
    TIFFSetField(tif[camera], TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif[camera], TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(tif[camera],TIFFTAG_SAMPLEFORMAT,frame_sampleformat);
    TIFFSetField(tif[camera],TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);

    // write frame data
    // data is broken up into strips where each strip contains rowsperstrip complete rows of data
    // each stript then has a size of rowsperstrip*frame_width pixels. the last strip is possibly
    // smaller, so it is NOT padded with dummy data.
    uint8* const buf = (uint8*)_TIFFmalloc(TIFFStripSize(tif[camera])); // data buffer for a strip of the image
    for (unsigned int row = 0; (row<frame_height); row+=rowsperstrip) {
        // compute rows in this strip:
        uint32 nrow = rowsperstrip;
        if ((row + rowsperstrip)>frame_height) {
            nrow=frame_height-row; // this is the last strip ... and it is a bit smaller! ... it only contains the last rows of the image
        }
        tstrip_t strip = TIFFComputeStrip(tif[camera],row,0);
        tsize_t bi = 0;
        // go through the fraem row-wise
        for (unsigned int rr = 0; rr<nrow; ++rr) {
            for (unsigned int cc = 0; cc<frame_width; ++cc) { // go through all pixels in the current row
                buf[bi++] = (uint8)frame[cc+(row + rr)*frame_width];
            }
        }
        TIFFWriteEncodedStrip(tif[camera],strip,buf,bi*sizeof(uint8));
    }
    _TIFFfree(buf);
    // write current directory, i.e. start new image
    TIFFWriteDirectory(tif[camera]);


    free(frame);
    free(frame32);

    seriesCount[camera]++;
    seriesRunning[camera] = seriesRunning[camera] && (seriesCount[camera]<seriesAcquisitions);

    if (seriesRunning[camera]) {
        QTimer::singleShot(seriesDelay, this,SLOT(seriesStep2()));
    } else {
        TIFFClose(tif[camera]);
    }
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
