/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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
    max=1000;
    userMin=0;
    userMax=1000;


    QHBoxLayout* layout=new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    editLow=new QSpinBox(this);
    editHigh=new QSpinBox(this);
    editLow->setRange(min, max);
    editHigh->setRange(min, max);
    editLow->setContextMenuPolicy(Qt::NoContextMenu);
    editHigh->setContextMenuPolicy(Qt::NoContextMenu);



    QLabel* lab1=new QLabel(tr("<b>lower & upper cut-off</b>"), this);
    lab1->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labCentral=lab1;
    /*layout->addWidget(editLow,  0, 0, Qt::AlignLeft|Qt::AlignTop);
    layout->addWidget(lab1, 0, 1, Qt::AlignRight|Qt::AlignBottom);
    layout->addWidget(editHigh, 0, 2, Qt::AlignRight|Qt::AlignTop);
    layout->setColumnStretch(0,1);
    layout->setColumnStretch(2,1);*/
    layout->addWidget(editLow,  0, Qt::AlignLeft|Qt::AlignTop);
    layout->addWidget(lab1,  1, Qt::AlignRight|Qt::AlignBottom);
    layout->addWidget(editHigh,  0, Qt::AlignRight|Qt::AlignTop);
    contextMenu=NULL;

    connectWidgets(true);
    updateSliderWidgets();
}

DataCutSliders::~DataCutSliders()
{

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
    updateSliderWidgets();
}

void DataCutSliders::sliderHighEditingFinished() {
    userMax=editHigh->value();
    updateSliderWidgets();
}

void DataCutSliders::setValues(int userMin, int userMax, int min, int max) {
    this->min=min;
    this->max=max;
    this->userMin=userMin;
    this->userMax=userMax;
    updateSliderWidgets();
}

void DataCutSliders::setCentralLabel(const QString &label)
{
    labCentral->setText(label);
}

void DataCutSliders::connectWidgets(bool conn)
{
    if (conn) {
        connect(editLow, SIGNAL(valueChanged(int)), this, SLOT(sliderLowValueChanged(int)));
        connect(editHigh, SIGNAL(valueChanged(int)), this, SLOT(sliderHighValueChanged(int)));

        connect(editLow, SIGNAL(editingFinished()), this, SLOT(sliderLowEditingFinished()));
        connect(editHigh, SIGNAL(editingFinished()), this, SLOT(sliderHighEditingFinished()));
    } else {
        disconnect(editLow, SIGNAL(valueChanged(int)), this, SLOT(sliderLowValueChanged(int)));
        disconnect(editHigh, SIGNAL(valueChanged(int)), this, SLOT(sliderHighValueChanged(int)));

        disconnect(editLow, SIGNAL(editingFinished()), this, SLOT(sliderLowEditingFinished()));
        disconnect(editHigh, SIGNAL(editingFinished()), this, SLOT(sliderHighEditingFinished()));
    }
}

void DataCutSliders::updateSliderWidgets() {
    if (userMin>=userMax) {
        userMax=userMin+1;
    }
    bool h=sliderSignals;
    sliderSignals=false;
    //bool upd=updatesEnabled();
    //setUpdatesEnabled(false);
    int mi=min;
    int ma=max;
    int umi=userMin;
    int uma=userMax;
    disconnectWidgets();
    //qDebug()<<mi<<umi<<uma<<ma;
    editLow->setRange(mi, qMin(uma, ma));
    editHigh->setRange(qMax(umi, mi), ma);
    editLow->setValue(umi);
    editLow->setSuffix(tr("/%1").arg(ma));
    editHigh->setValue(uma);
    editHigh->setSuffix(tr("/%1").arg(ma));
    connectWidgets();
    //setUpdatesEnabled(upd);
    //if (upd) {
        if (layout()) layout()->invalidate();
        QWidget::update();
    //}
    sliderSignals=h;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }
}













