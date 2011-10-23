#include "datacutslider.h"

DataCutSliders::DataCutSliders(QWidget* parent):
    QWidget(parent)
{
    //setMaximumHeight(60);
    //setMinimumHeight(60);

    runsName=tr("runs");
    filesName=tr("files");
    copyToRunsEnabled=true;
    copyToFilesEnabled=true;

    sliderSignals=true;
    allowCopyToAll=true;
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


    QGridLayout* layout=new QGridLayout(this);
    layout->setContentsMargins(0,0,0,0);

    /*QLabel* lab1=new QLabel(tr("<b>lower cut-off</b>"), this);
    QLabel* lab2=new QLabel(tr("<b>upper cut-off</b>"), this);
    lab2->setAlignment(Qt::AlignRight);
    layout->addWidget(lab1, 0, 0, Qt::AlignLeft|Qt::AlignBottom);
    layout->addWidget(lab2, 0, 3, Qt::AlignRight|Qt::AlignBottom);
    layout->addWidget(editLow,  1, 0, Qt::AlignLeft|Qt::AlignTop);
    layout->addWidget(editHigh, 1, 3, Qt::AlignRight|Qt::AlignTop);*/

    QLabel* lab1=new QLabel(tr("<b>lower & upper cut-off</b>"), this);
    lab1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    layout->addWidget(editLow,  0, 0, Qt::AlignLeft|Qt::AlignTop);
    layout->addWidget(lab1, 0, 1, Qt::AlignRight|Qt::AlignBottom);
    layout->addWidget(editHigh, 0, 2, Qt::AlignRight|Qt::AlignTop);
    layout->setColumnStretch(0,1);
    layout->setColumnStretch(2,1);
    contextMenu=NULL;
    setLayout(layout);

    connect(editLow, SIGNAL(valueChanged(int)), this, SLOT(sliderLowValueChanged(int)));
    connect(editHigh, SIGNAL(valueChanged(int)), this, SLOT(sliderHighValueChanged(int)));

    connect(editLow, SIGNAL(editingFinished()), this, SLOT(sliderLowEditingFinished()));
    connect(editHigh, SIGNAL(editingFinished()), this, SLOT(sliderHighEditingFinished()));

    update();
}

void DataCutSliders::contextMenuEvent(QContextMenuEvent *event) {
    QMenu *menu = new QMenu(this);
    if (allowCopyToAll) {
        if (copyToFilesEnabled) {
            menu->addSeparator();
            QAction* actCopyMin=menu->addAction(tr("copy m&in to all %1").arg(filesName));
            connect(actCopyMin, SIGNAL(triggered()), this, SLOT(copyUserMinClicked()));
            QAction* actCopyMax=menu->addAction(tr("copy m&ax to all %1").arg(filesName));
            connect(actCopyMax, SIGNAL(triggered()), this, SLOT(copyUserMaxClicked()));
            QAction* actCopyBoth=menu->addAction(tr("copy min&+max to all %1").arg(filesName));
            connect(actCopyBoth, SIGNAL(triggered()), this, SLOT(copyUserMinMaxClicked()));
        }
        if (copyToRunsEnabled) {
            menu->addSeparator();
            QAction* actCopyMin=menu->addAction(tr("copy mi&n to all %1").arg(runsName));
            connect(actCopyMin, SIGNAL(triggered()), this, SLOT(copyUserMinRunsClicked()));
            QAction* actCopyMax=menu->addAction(tr("copy ma&x to all %1").arg(runsName));
            connect(actCopyMax, SIGNAL(triggered()), this, SLOT(copyUserMaxRunsClicked()));
            QAction* actCopyBoth=menu->addAction(tr("copy min+max to all &%1").arg(runsName));
            connect(actCopyBoth, SIGNAL(triggered()), this, SLOT(copyUserMinMaxRunsClicked()));
        }
    }
    menu->exec(event->globalPos());
    delete menu;
}

void DataCutSliders::sliderLowValueChanged(int value){
    userMin=value;
    if (userMin>=userMax) userMin=userMax-1;
    if (userMin<min) userMin=min;
    if (userMax>max) userMax=max;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }

}

void DataCutSliders::sliderHighValueChanged(int value){
    userMax=value;
    if (userMax<=userMin) userMax=userMin+1;
    if (userMax>max) userMax=max;
    if (userMax<min) userMax=min;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }
}


void DataCutSliders::sliderLowEditingFinished(){
    userMin=editLow->value();
    if (userMin>userMax) {
        userMax=userMin+1;
    }
    update();
}

void DataCutSliders::sliderHighEditingFinished() {
    userMax=editHigh->value();
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
    if (userMin>=userMax) {
        userMax=userMin+1;
    }
    bool h=sliderSignals;
    sliderSignals=false;
    bool upd=updatesEnabled();
    setUpdatesEnabled(false);
    editLow->setRange(min, qMin(userMax, max));
    editHigh->setRange(qMax(userMin, min), max);
    editLow->setValue(userMin);
    editLow->setSuffix(tr("/%1").arg(max));
    editHigh->setValue(userMax);
    editHigh->setSuffix(tr("/%1").arg(max));
    setUpdatesEnabled(upd);
    sliderSignals=h;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }
}













