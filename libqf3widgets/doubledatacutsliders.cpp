/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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


#include "doubledatacutsliders.h"
#include <QGridLayout>
#include <QtGui>

DoubleDataCutSliders::DoubleDataCutSliders(QWidget* parent):
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

    editLow=new QFDoubleEdit(this);
    editHigh=new QFDoubleEdit(this);
    editLow->setRange(min, max);
    editHigh->setRange(min, max);
    editLow->setContextMenuPolicy(Qt::NoContextMenu);
    editHigh->setContextMenuPolicy(Qt::NoContextMenu);


    QGridLayout* layout=new QGridLayout();
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
    labCentral=lab1;
    layout->addWidget(editLow,  0, 0, Qt::AlignLeft|Qt::AlignTop);
    layout->addWidget(lab1, 0, 1, Qt::AlignRight|Qt::AlignBottom);
    layout->addWidget(editHigh, 0, 2, Qt::AlignRight|Qt::AlignTop);
    layout->setColumnStretch(0,1);
    layout->setColumnStretch(2,1);
    contextMenu=NULL;
    setLayout(layout);

    connect(editLow, SIGNAL(valueChanged(double)), this, SLOT(sliderLowValueChanged(double)));
    connect(editHigh, SIGNAL(valueChanged(double)), this, SLOT(sliderHighValueChanged(double)));

    connect(editLow, SIGNAL(editingFinished()), this, SLOT(sliderLowEditingFinished()));
    connect(editHigh, SIGNAL(editingFinished()), this, SLOT(sliderHighEditingFinished()));

    update();
}

void DoubleDataCutSliders::setLogScale(bool enabled, double increments)
{
    editLow->setLogScale(enabled, increments);
    editHigh->setLogScale(enabled, increments);
    if (!enabled) {
        editLow->setIncrement(increments);
        editHigh->setIncrement(increments);
    }
}

void DoubleDataCutSliders::contextMenuEvent(QContextMenuEvent *event) {
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

void DoubleDataCutSliders::sliderLowValueChanged(double value){
    userMin=value;
    if (userMin>=userMax) userMin=userMax-1;
    if (userMin<min) userMin=min;
    if (userMax>max) userMax=max;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }

}

void DoubleDataCutSliders::sliderHighValueChanged(double value){
    userMax=value;
    if (userMax<=userMin) userMax=userMin+1;
    if (userMax>max) userMax=max;
    if (userMax<min) userMax=min;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }
}


void DoubleDataCutSliders::sliderLowEditingFinished(){
    userMin=editLow->value();
    if (userMin>userMax) {
        userMax=userMin+1;
    }
    update();
}

void DoubleDataCutSliders::sliderHighEditingFinished() {
    userMax=editHigh->value();
    update();
}

void DoubleDataCutSliders::setValues(double userMin, double userMax, double min, double max) {
    this->userMin=userMin;
    this->userMax=userMax;
    this->min=min;
    this->max=max;
    update();
}

void DoubleDataCutSliders::setCentralText(const QString &label)
{
    labCentral->setText(label);
}

void DoubleDataCutSliders::update() {
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
    //editLow->setSuffix(tr("/%1").arg(max));
    editHigh->setValue(userMax);
    //editHigh->setSuffix(tr("/%1").arg(max));
    setUpdatesEnabled(upd);
    sliderSignals=h;
    if (sliderSignals) {
        emit slidersChanged(userMin, userMax, min, max);
    }
}


