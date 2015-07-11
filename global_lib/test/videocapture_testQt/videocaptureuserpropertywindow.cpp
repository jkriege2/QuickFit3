#include "videocaptureuserpropertywindow.h"

VideoCaptureUserPropertyWindow::VideoCaptureUserPropertyWindow(QWidget *parent) :
    QDialog(parent)
{
    layout=new QFormLayout(this);
    setLayout(layout);
}

VideoCaptureUserPropertyWindow::~VideoCaptureUserPropertyWindow()
{
}

void VideoCaptureUserPropertyWindow::init(VideoCapture *vid) {
    this->vid=vid;

    for (int d=0; d<vid->getConfigDialogCount(); d++) {
        QPushButton* btn=new QPushButton(vid->getConfigDialogName(d).c_str(), this);
        layout->addRow("", btn);
        buttons[btn]=d;
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    }

    VideoCapture::UserProperties p;
    if (vid->supportsUserProperty(p=VideoCapture::upBrightness)) {
        QSpinBox* spin=new QSpinBox(this);
        int min, max;
        vid->getUserPropertyRange(p, min, max);
        spin->setRange(min, max);
        spin->setValue(vid->getUserProperty(p));
        layout->addRow(tr("brightness:"), spin);
        widgets[spin]=p;
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(spinChanged(int)));
    }

}

void VideoCaptureUserPropertyWindow::spinChanged(int value) {
    VideoCapture::UserProperties p=widgets[sender()];
    vid->setUserProperty(p, value);
}

void VideoCaptureUserPropertyWindow::buttonClicked() {
    int d=buttons[sender()];
    vid->showConfigDialog(d);
}

/*
            upBrightness,
            upContrast,
            upSaturation,
            upHue,
            upAutoWhitebalance,
            upRedBalance,
            upBlueBalance,
            upGamma,
            upExposure,
            upAutogain,
            upGain,
            upPowerlineFilter,
            upSharpness,
            upBacklightCompensation
  */

