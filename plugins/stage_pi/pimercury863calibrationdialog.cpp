#include "pimercury863calibrationdialog.h"
#include <QtGui>

PIMercury863CalibrationDialog::PIMercury863CalibrationDialog(QWidget* parent, PIMercury863Stage* stage):
    QDialog(parent)
{
    this->stage=stage;
    maxX=maxY=maxZ=-1;
    minX=minY=minZ=-1;
    zeroX=zeroY=zeroZ=127;

    for (int i=0; i<256; i++) {
        calX[i]=calY[i]=calZ[i]=i;//(int)round(((double)i/255.0-0.5)*255.0);
    }

    QVBoxLayout* vbl=new QVBoxLayout(this);
    vbl->addWidget(new QLabel(tr("calibration<br><ol><li>press RESET CAL.</li><li>move joystick to center position and press CENTERED.</li><li>move joystick to maximum positions and press CALIBRATE.</li><li>accept calibration with OK or reject with CANCEL.</li></ol>"), this));

    QGridLayout* grid=new QGridLayout(this);

    prgX=new QProgressBar(this);
    prgX->setRange(0,255);
    prgX->setFormat("%v");
    prgX->setValue(127);
    grid->addWidget(new QLabel("x: ", this), 0, 0);
    grid->addWidget(prgX, 0, 1);
    labX=new QLabel(this);
    grid->addWidget(labX, 0, 2);
    labMinX=new QLabel(this);
    grid->addWidget(labMinX, 0, 3);
    labMaxX=new QLabel(this);
    grid->addWidget(labMaxX, 0, 4);

    prgY=new QProgressBar(this);
    prgY->setRange(0,255);
    prgY->setFormat("%v");
    prgY->setValue(127);
    grid->addWidget(new QLabel("y: ", this), 1, 0);
    grid->addWidget(prgY, 1, 1);
    labY=new QLabel(this);
    grid->addWidget(labY, 1, 2);
    labMinY=new QLabel(this);
    grid->addWidget(labMinY, 1, 3);
    labMaxY=new QLabel(this);
    grid->addWidget(labMaxY, 1, 4);

    prgZ=new QProgressBar(this);
    prgZ->setRange(0,255);
    prgZ->setFormat("%v");
    prgZ->setValue(127);
    grid->addWidget(new QLabel("z: ", this), 2, 0);
    grid->addWidget(prgZ, 2, 1);
    labZ=new QLabel(this);
    grid->addWidget(labZ, 2, 2);
    labMinZ=new QLabel(this);
    grid->addWidget(labMinZ, 2, 3);
    labMaxZ=new QLabel(this);
    grid->addWidget(labMaxZ, 2, 4);

    vbl->addLayout(grid);

    QHBoxLayout* hbl=new QHBoxLayout(this);

    btnResetCal=new QPushButton(tr("&Reset Calibration"), this);;
    connect(btnResetCal, SIGNAL(clicked()), this, SLOT(resetCal()));
    hbl->addWidget(btnResetCal);

    btnCentered=new QPushButton(tr("Ce&ntered"), this);;
    connect(btnCentered, SIGNAL(clicked()), this, SLOT(center()));
    hbl->addWidget(btnCentered);

    btnCalibrated=new QPushButton(tr("C&alibrate"), this);;
    connect(btnCalibrated, SIGNAL(clicked()), this, SLOT(calibrate()));
    hbl->addWidget(btnCalibrated);

    hbl->addSpacing(100);

    btnOK=new QPushButton(tr("&OK"), this);
    connect(btnOK, SIGNAL(clicked()), this, SLOT(acceptCal()));
    hbl->addWidget(btnOK);

    btnCancel=new QPushButton(tr("&Cancel"), this);
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    hbl->addWidget(btnCancel);


    vbl->addLayout(hbl);
    setLayout(vbl);

    accepted=false;

    QTimer::singleShot(100, this, SLOT(getJoystickValues()));
}

PIMercury863CalibrationDialog::~PIMercury863CalibrationDialog()
{
    //dtor
}

void PIMercury863CalibrationDialog::resetCal() {

    // write calibration to controllers
    std::cout<<"\n\ncalX[...]=\n";
    for (int i=0; i<256; i++) {
        stage->sendCommandX("SI"+inttostr(i));
        int c=(int)round(1024.0*((double)i-127.0)/128.0);
        stage->sendCommandX("SJ"+inttostr(c));
        std::cout<<"   "<<i<<":  "<<c<<std::endl;
    }
    std::cout<<"\n\ncalY[...]=\n";
    for (int i=0; i<256; i++) {
        stage->sendCommandY("SI"+inttostr(i));
        int c=(int)round(1024.0*((double)i-127.0)/128.0);
        stage->sendCommandY("SJ"+inttostr(c));
        std::cout<<"   "<<i<<":  "<<c<<std::endl;
    }
    std::cout<<"\n\ncalZ[...]=\n";
    for (int i=0; i<256; i++) {
        stage->sendCommandZ("SI"+inttostr(i));
        int c=(int)round(1024.0*((double)i-127.0)/128.0);
        stage->sendCommandZ("SJ"+inttostr(c));
        std::cout<<"   "<<i<<":  "<<c<<std::endl;
    }
}

void PIMercury863CalibrationDialog::acceptCal() {
    accepted=true;
    // write calibration to controllers
    std::cout<<"\n\ncalX[...]=\n";
    for (int i=0; i<256; i++) {
        stage->sendCommandX("SI"+inttostr(i));
        stage->sendCommandX("SJ"+inttostr(calX[i]));
        std::cout<<"   "<<i<<":  "<<calX[i]<<std::endl;
    }
    std::cout<<"\n\ncalY[...]=\n";
    for (int i=0; i<256; i++) {
        stage->sendCommandY("SI"+inttostr(i));
        stage->sendCommandY("SJ"+inttostr(calY[i]));
        std::cout<<"   "<<i<<":  "<<calY[i]<<std::endl;
    }
    std::cout<<"\n\ncalZ[...]=\n";
    for (int i=0; i<256; i++) {
        stage->sendCommandZ("SI"+inttostr(i));
        stage->sendCommandZ("SJ"+inttostr(calZ[i]));
        std::cout<<"   "<<i<<":  "<<calZ[i]<<std::endl;
    }
    accept();
}

void PIMercury863CalibrationDialog::calibrate() {
    /* calculate calibration curves as two half parabulas:
         one from (minX, -1024) to (zerX, 0)
         one from (zerX, 0) to (maxX, 1024)
         the vertex of the parabulas is in both cases (zeroX, 0)

         thus the quadratic function may be written as:
            f(x)=a*(x-zeroX)^2

            where a=1024/(maxX-zeroX)^2 above zeroX
            and   a=-1024/(minX-zeroX)^2 below zeroX
            above maxX calX equals 1024
            below minX calX equals -1024
    */
    double zerowidth=9;
    double ap=1, an=-1;
    ap=1024.0/pow(maxX-(zeroX+zerowidth), 2.0);
    an=-1024.0/pow(minX-(zeroX-zerowidth), 2.0);
    for (int i=0; i<256; i++) {
        if (i<minX) calX[i]=-1024;
        else if (i<zeroX-zerowidth) calX[i]=(int)round(an*pow(i-(zeroX-zerowidth), 2.0));
        else if ((i>zeroX+zerowidth) && (i<maxX)) calX[i]=(int)round(ap*pow(i-(zeroX+zerowidth), 2.0));
        else if ((i>=zeroX-zerowidth) && (i<=zeroX+zerowidth)) calX[i]=0;
        else calX[i]=1024;
    }

    ap=1024.0/pow(maxY-(zeroY+zerowidth), 2.0);
    an=-1024.0/pow(minY-(zeroY-zerowidth), 2.0);
    for (int i=0; i<256; i++) {
        if (i<minY) calY[i]=-1024;
        else if (i<zeroY-zerowidth) calY[i]=(int)round(an*pow(i-(zeroY-zerowidth), 2.0));
        else if ((i>zeroY+zerowidth) && (i<maxY)) calY[i]=(int)round(ap*pow(i-(zeroY+zerowidth), 2.0));
        else if ((i>=zeroY-zerowidth) && (i<=zeroY+zerowidth)) calY[i]=0;
        else calY[i]=1024;
    }

    ap=1024.0/pow(maxZ-(zeroZ+zerowidth), 2.0);
    an=-1024.0/pow(minZ-(zeroZ-zerowidth), 2.0);
    for (int i=0; i<256; i++) {
        if (i<minZ) calZ[i]=-1024;
        else if (i<zeroZ-zerowidth) calZ[i]=(int)round(an*pow(i-(zeroZ-zerowidth), 2.0));
        else if ((i>zeroZ+zerowidth) && (i<maxZ)) calZ[i]=(int)round(ap*pow(i-(zeroZ+zerowidth), 2.0));
        else if ((i>=zeroZ-zerowidth) && (i<=zeroZ+zerowidth)) calZ[i]=0;
        else calZ[i]=1024;
    }

    zeroX=zeroY=zeroZ=0;
    QTimer::singleShot(10, this, SLOT(getJoystickValues()));
}

void PIMercury863CalibrationDialog::center() {
    if (stage->isConnected()) {
        stage->resetError();
        std::string r;
        r=stage->queryCommandX("TA5")+"\n";
        if (!stage->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &zeroX);
        r=stage->queryCommandY("TA5")+"\n";
        if (!stage->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &zeroY);
        r=stage->queryCommandZ("TA5")+"\n";
        if (!stage->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &zeroZ);
    }
    QTimer::singleShot(1, this, SLOT(getJoystickValues()));
}

void PIMercury863CalibrationDialog::getJoystickValues() {
    if (stage->isConnected()) {
        stage->resetError();
        int jx=0, jy=0, jz=0;
        std::string r;
        r=stage->queryCommandX("TA5")+"\n";
        if (!stage->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &jx);
        r=stage->queryCommandY("TA5")+"\n";
        if (!stage->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &jy);
        r=stage->queryCommandZ("TA5")+"\n";
        if (!stage->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &jz);
        if (jx<0) jx=0; if (jx>255) jx=255;
        if (jy<0) jy=0; if (jy>255) jy=255;
        if (jz<0) jz=0; if (jz>255) jz=255;

        std::cout<<"("<<jx<<", "<<jy<<", "<<jz<<")\n";
        labX->setText(tr("%1").arg(calX[jx]-zeroX));
        prgX->setValue(jx);
        labY->setText(tr("%1").arg(calY[jy]-zeroY));
        prgY->setValue(jy);
        labZ->setText(tr("%1").arg(calZ[jz]-zeroZ));
        prgZ->setValue(jz);
        if (minX<0) minX=maxX=calX[jx];
        else {
            if (calX[jx]<minX) minX=calX[jx];
            if (calX[jx]>maxX) maxX=calX[jx];
        }
        labMinX->setText(tr("min=%1").arg(minX-zeroX));
        labMaxX->setText(tr("max=%1").arg(maxX-zeroX));
        if (minY<0) minY=maxY=calY[jy];
        else {
            if (calY[jy]<minY) minY=calY[jy];
            if (calY[jy]>maxY) maxY=calY[jy];
        }
        labMinY->setText(tr("min=%1").arg(minY-zeroY));
        labMaxY->setText(tr("max=%1").arg(maxY-zeroY));
        if (minZ<0) minZ=maxZ=calZ[jz];
        else {
            if (calZ[jz]<minZ) minZ=calZ[jz];
            if (calZ[jz]>maxZ) maxZ=calZ[jz];
        }
        labMinZ->setText(tr("min=%1").arg(minZ-zeroZ));
        labMaxZ->setText(tr("max=%1").arg(maxZ-zeroZ));
    } else {
        labX->setText(tr("Not Connected"));
        labY->setText(tr("Not Connected"));
        labZ->setText(tr("Not Connected"));
    }

    if (!accepted) QTimer::singleShot(1, this, SLOT(getJoystickValues()));
}

