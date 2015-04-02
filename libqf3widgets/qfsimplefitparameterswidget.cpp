/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfsimplefitparameterswidget.h"
#include <QDebug>

QFSimpleFitParametersWidget::QFSimpleFitParametersWidget(QFSimpleFitParameterInterface *paraminterface, QWidget *parent) :
    QWidget(parent)
{
    this->paraminterface=paraminterface;
    updating=false;

    mainLayout=new QGridLayout();
    //mainLayout->setContentsMargins(0,0,0,0);
    spacer=new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::Expanding);
    mainLayout->addItem(spacer, 0, 0);
    setLayout(mainLayout);
}

void QFSimpleFitParametersWidget::clearWidgets() {
    bool upd=updatesEnabled();
    setUpdatesEnabled(false);
    bool oldupd=updating;
    updating=true;
    for (int i=mainLayout->count()-1; i>=0; i--) {
        mainLayout->removeItem(mainLayout->itemAt(i));
    }
    mainLayout->addItem(spacer, 0, 0);
    for (int i=0; i<widgets.size(); i++) {
        widgets[i].label->deleteLater();
        widgets[i].edit->deleteLater();
    }
    widgets.clear();
    updating=oldupd;
    setUpdatesEnabled(upd);
}

void QFSimpleFitParametersWidget::setParameterInterface(QFSimpleFitParameterInterface *paraminterface) {
    this->paraminterface=paraminterface;
}

QString QFSimpleFitParametersWidget::getIDForWidget(QObject *edit) {
    for (int i=0; i<widgets.size(); i++) {
        if (widgets[i].edit==edit) return widgets[i].id;
    }
    return "";
}

void QFSimpleFitParametersWidget::valueChanged(double value) {
    if (updating) return;
    QString id=getIDForWidget(sender());
    if (paraminterface && !id.isEmpty()) {
        paraminterface->setFitValue(id, value);
        emit parameterChanged(id, value);
    }
}

void QFSimpleFitParametersWidget::updateWidgetValues() {
    updating=true;
    if (paraminterface) {
        for (int i=0; i<widgets.size(); i++) {
            if (!widgets[i].id.isEmpty()) {
                widgets[i].edit->setValue(paraminterface->getFitValue(widgets[i].id));
            }
        }
    }
    updating=false;
}

void QFSimpleFitParametersWidget::addParameter(const QString &id, const QString &label) {
    QFSimpleFitParametersWidget::subWidget w;
    w.id=id;
    w.label=new QLabel(label, this);
    w.label->setTextFormat(Qt::RichText);
    w.edit=new QFDoubleEdit(this);
    w.edit->setCheckBounds(false, false);
    w.edit->setDecimals(6);
    connect(w.edit, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
    w.label->setBuddy(w.edit);
    widgets.append(w);
    mainLayout->removeItem(spacer);
    int row=mainLayout->rowCount();
    mainLayout->addWidget(w.label, row, 0);
    mainLayout->addWidget(w.edit, row, 1);
    mainLayout->addItem(spacer, row+1, 0);
    updateWidgetValues();
}

void QFSimpleFitParametersWidget::setParameters(const QStringList &ids, const QStringList &labels) {
    int cnt=qMin(ids.count(), labels.count());
    int oldCnt=widgets.size();
    if (cnt>widgets.size()) {
        //qDebug()<<"cnt>widgets.size(): "<<cnt<<">"<<widgets.size();
        for (int i=0; i<cnt; i++) {
            if (i<oldCnt) {
                widgets[i].id=ids[i];
                widgets[i].label->setText(labels[i]);
            } else {
                addParameter(ids[i], labels[i]);
            }
        }
        //qDebug()<<widgets.size();
    } else if (cnt<widgets.size()) {
        //qDebug()<<"cnt<widgets.size(): "<<cnt<<"<"<<widgets.size();
        mainLayout->removeItem(spacer);
        for (int i=widgets.size()-1; i>=cnt; i--) {
            mainLayout->removeWidget(widgets[i].label);
            mainLayout->removeWidget(widgets[i].edit);
            if (widgets[i].label) widgets[i].label->deleteLater();
            if (widgets[i].edit) widgets[i].edit->deleteLater();
            widgets.removeAt(i);
        }
        mainLayout->addItem(spacer, mainLayout->rowCount(), 0);
        //qDebug()<<widgets.size();
    }
    for (int i=0; i<cnt; i++) {
        //qDebug()<< widgets[i].id<<widgets[i].label;
        widgets[i].id=ids[i];
        widgets[i].label->setText(labels[i]);
    }
    updateWidgetValues();
}
