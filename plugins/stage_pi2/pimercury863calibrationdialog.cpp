/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pimercury863calibrationdialog.h"
#include "stage_pi2.h"
#include <QtGui>

PIMercury863CalibrationDialog::PIMercury863CalibrationDialog(QWidget* parent, QFExtensionLinearStagePI2 *stage, int axis):
    QDialog(parent)
{
    this->stage=stage;
    this->com=stage->axes[axis].serial->getCOM();
    this->serial=stage->axes[axis].serial;
    this->axis=axis;
    maxX=-10000;
    minX=-10000;
    zeroX=127;

    for (int i=0; i<256; i++) {
        calX[i]=i;//(int)round(((double)i/255.0-0.5)*255.0);
    }

    QVBoxLayout* vbl=new QVBoxLayout();
    vbl->addWidget(new QLabel(tr("calibration of axis %1<br><ol><li>press RESET CAL.</li><li>move joystick to center position and press CENTERED.</li><li>move joystick to maximum positions and press CALIBRATE.</li><li>accept calibration with OK or reject with CANCEL.</li></ol>").arg(axis), this));

    QGridLayout* grid=new QGridLayout();

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

    vbl->addLayout(grid);

    chkInvert=new QCheckBox(tr("invert axis"), this);
    chkInvert->setChecked(false);
    vbl->addWidget(chkInvert);

    QHBoxLayout* hbl=new QHBoxLayout();

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
    setWindowIcon(QIcon(":/stage_pi/pi_joystick.png"));
    setWindowTitle(tr("PI Mercury C863 joystick calibration, stage %1").arg(axis));

    QTimer::singleShot(100, this, SLOT(getJoystickValues()));
}

PIMercury863CalibrationDialog::~PIMercury863CalibrationDialog()
{
    //dtor
}

void PIMercury863CalibrationDialog::resetCal() {
    QMutexLocker locker(serial->getMutex());

    // write calibration to controllers
    std::cout<<"\n\ncalX[...]=\n";
    for (int i=0; i<256; i++) {
        serial->selectAxis(stage->axes[axis].ID);
        serial->sendCommand("SI"+inttostr(i));
        int c=(int)round(1024.0*((double)i-127.0)/128.0);
        serial->sendCommand("SJ"+inttostr(c));
        //std::cout<<"   "<<i<<":  "<<c<<std::endl;
        calX[i]=i;
    }
    minX=-10000;
    maxX=-10000;
    zeroX=127;

}

void PIMercury863CalibrationDialog::acceptCal() {
    QMutexLocker locker(serial->getMutex());
    accepted=true;
    // write calibration to controllers
    stage->log_text(tr("sending calibration table for axis %1:\n").arg(axis));
    stage->log_text(tr("  cal[i]=\tvalue\n").arg(axis));
    for (int i=0; i<256; i++) {
        serial->selectAxis(stage->axes[axis].ID);
        serial->sendCommand("SI"+inttostr(i));
        serial->sendCommand("SJ"+inttostr(calX[i]));
        stage->log_text(tr("  %1\t%2\n").arg(i).arg(calX[i]));
        //qDebug()<<stage->queryCommand("SI?").c_str()<<stage->queryCommand("SJ?").c_str();
    }
    accept();
}

void PIMercury863CalibrationDialog::calibrate() {
    /* calculate calibration curves as two half parabulas:
         one from (minX, -1000) to (zerX, 0)
         one from (zerX, 0) to (maxX, 1000)
         the vertex of the parabulas is in both cases (zeroX, 0)

         thus the quadratic function may be written as:
            f(x)=a*(x-zeroX)^2

            where a=1000/(maxX-zeroX)^2 above zeroX
            and   a=-1000/(minX-zeroX)^2 below zeroX
            above maxX calX equals 1000
            below minX calX equals -1000
    */
    double zerowidth=20;
    double ap=1, an=-1;
    ap=1000.0/pow(maxX-(zeroX+zerowidth), 2.0);
    an=-1000.0/pow(minX-(zeroX-zerowidth), 2.0);
    for (int i=0; i<256; i++) {
        if (i<minX) calX[i]=-1000;
        else if (i<zeroX-zerowidth) calX[i]=(int)round(an*pow(i-(zeroX-zerowidth), 2.0));
        else if ((i>zeroX+zerowidth) && (i<=maxX)) calX[i]=(int)round(ap*pow(i-(zeroX+zerowidth), 2.0));
        else if ((i>=zeroX-zerowidth) && (i<=zeroX+zerowidth)) calX[i]=0;
        else if (i>=maxX) calX[i]=1000;
        else if (i<=minX) calX[i]=-1000;
        else calX[i]=1000;

        if (calX[i]>1000) calX[i]=1000;
        if (calX[i]<-1000) calX[i]=-1000;
        //qDebug()<<"x "<<i<<minX<<i<<maxX<<calX[i];
    }
    if (chkInvert->isChecked()) {
        for (int i=0; i<256; i++) calX[i]=calX[i]*-1;
    }


    zeroX=0;
    QTimer::singleShot(10, this, SLOT(getJoystickValues()));
}

void PIMercury863CalibrationDialog::center() {
    QMutexLocker locker(serial->getMutex());
    if (com->isConnectionOpen()) {
        //stage->resetError();
        std::string r;
        serial->selectAxis(stage->axes[axis].ID);
        r=serial->queryCommand("TA5")+"\n";
        //if (!stage->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &zeroX);
    }
    minX=-1;
    maxX=-1;

    //QTimer::singleShot(1, this, SLOT(getJoystickValues()));
}

void PIMercury863CalibrationDialog::getJoystickValues() {
    QMutexLocker locker(serial->getMutex());
    if (com->isConnectionOpen()) {
        int jx=0, jy=0, jz=0;
        std::string r;
        serial->selectAxis(stage->axes[axis].ID);
        r=serial->queryCommand("TA5")+"\n";
        //if (!serial->hasErrorOccured())
            sscanf(r.c_str(), "A5:%d", &jx);

        int jjx=jx;
        if (jx<0) jx=0; if (jx>255) jx=255;

        //std::cout<<"("<<jx<<", "<<jy<<", "<<jz<<")\n";
        labX->setText(tr("%1  (%2)").arg(calX[jx]-zeroX).arg(jjx));
        prgX->setValue(jx);
        if (minX<=-10000) minX=maxX=calX[jx];
        else {
            if (calX[jx]<minX) minX=calX[jx];
            if (calX[jx]>maxX) maxX=calX[jx];
        }
        labMinX->setText(tr("zero=%2 min=%1").arg(minX-zeroX).arg(zeroX));
        labMaxX->setText(tr("max=%1").arg(maxX-zeroX));
    } else {
        labX->setText(tr("Not Connected"));
    }

    if (!accepted) QTimer::singleShot(1, this, SLOT(getJoystickValues()));
}

