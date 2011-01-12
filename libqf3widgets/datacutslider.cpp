#include "datacutslider.h"

DataCutSliders::DataCutSliders(QWidget* parent):
    QWidget(parent)
{
    setMaximumHeight(60);
    setMinimumHeight(60);

    sliderSignals=true;
    allowCopyToAll=false;
    min=0;
    max=100;
    userMin=0;
    userMax=100;

    editLow=new QSpinBox(this);
    editHigh=new QSpinBox(this);
    editLow->setRange(min, max);
    editHigh->setRange(min, max);
    editLow->setContextMenuPolicy(Qt::NoContextMenu);
    editHigh->setContextMenuPolicy(Qt::NoContextMenu);

    QLabel* lab1=new QLabel(tr("<b>lower cut-off</b>"), this);
    QLabel* lab2=new QLabel(tr("<b>upper cut-off</b>"), this);
    lab2->setAlignment(Qt::AlignRight);

    QGridLayout* layout=new QGridLayout(this);
    layout->addWidget(lab1, 0, 0, Qt::AlignLeft|Qt::AlignBottom);
    layout->addWidget(lab2, 0, 3, Qt::AlignRight|Qt::AlignBottom);
    layout->addWidget(editLow,  1, 0, Qt::AlignLeft|Qt::AlignTop);
    layout->addWidget(editHigh, 1, 3, Qt::AlignRight|Qt::AlignTop);

    contextMenu=NULL;
    setLayout(layout);

    connect(editLow, SIGNAL(valueChanged(int)), this, SLOT(sliderLowValueChanged(int)));
    connect(editHigh, SIGNAL(valueChanged(int)), this, SLOT(sliderHighValueChanged(int)));

    update();
}

void DataCutSliders::contextMenuEvent(QContextMenuEvent *event) {
    if (!allowCopyToAll) return;
    if (contextMenu==NULL) {
        contextMenu=new QMenu(this);

        actCopyMin=contextMenu->addAction(tr("copy m&in to all files"));
        connect(actCopyMin, SIGNAL(triggered()), this, SLOT(copyUserMinClicked()));
        actCopyMax=contextMenu->addAction(tr("copy m&ax to all files"));
        connect(actCopyMax, SIGNAL(triggered()), this, SLOT(copyUserMaxClicked()));
        actCopyBoth=contextMenu->addAction(tr("copy min&+max to all files"));
        connect(actCopyBoth, SIGNAL(triggered()), this, SLOT(copyUserMinClicked()));
        connect(actCopyBoth, SIGNAL(triggered()), this, SLOT(copyUserMaxClicked()));
        contextMenu->addSeparator();
        actFitCurrent=contextMenu->addAction(tr("fit &current file"));
        connect(actFitCurrent, SIGNAL(triggered()), this, SLOT(fitCurrentClicked()));
        actFitAll=contextMenu->addAction(tr("fit &all files"));
        connect(actFitAll, SIGNAL(triggered()), this, SLOT(fitAllClicked()));
    }
    contextMenu->exec(event->globalPos());
}

void DataCutSliders::sliderLowValueChanged(int value){
    userMin=value;
    if (userMin>userMax) {
        editHigh->setValue(userMin+1);
    }
    update();
}

void DataCutSliders::sliderHighValueChanged(int value){
    userMax=value;
    if (userMin>userMax) {
        editLow->setValue(userMax-1);
    }
    update();
}

void DataCutSliders::setValues(int userMin, int userMax, int min, int max) {
    this->userMin=userMin;
    this->userMax=userMax;
    this->min=min;
    this->max=max;
    update();
}

void DataCutSliders::update() {
    if (userMin>userMax) {
        userMax=userMin+1;
    }
    bool h=sliderSignals;
    sliderSignals=false;
    editLow->setRange(min, max);
    editHigh->setRange(min, max);
    editLow->setValue(userMin);
    editLow->setSuffix(tr("/%1").arg(max));
    editHigh->setValue(userMax);
    editHigh->setSuffix(tr("/%1").arg(max));

    sliderSignals=h;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }
}













