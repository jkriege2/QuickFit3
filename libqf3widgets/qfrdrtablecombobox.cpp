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


#include "qfrdrtablecombobox.h"


QFRDRTableComboBoxMatchFunctor::QFRDRTableComboBoxMatchFunctor(bool requireColumnGraphics, bool requireNotReadonly):
    QFMatchRDRFunctor()
{
    this->requireColumnGraphics=requireColumnGraphics;
    this->requireNotReadonly=requireNotReadonly;
}

void QFRDRTableComboBoxMatchFunctor::setRequireColumnGraphics(bool require)
{
    this->requireColumnGraphics=require;
}

void QFRDRTableComboBoxMatchFunctor::setRequireNotReadonly(bool require)
{
    this->requireNotReadonly=require;
}


bool QFRDRTableComboBoxMatchFunctor::matches(const QFRawDataRecord *record) const
{
    if (!record) return false;
    const QFRDRTableInterface* tab=dynamic_cast<const QFRDRTableInterface*>(record);
    bool ok=false;
    if (tab) {
        ok=true;
        //qDebug()<<"        "<<this->requireColumnGraphics<<record->inherits("QFRDRColumnGraphsInterface");
        //qDebug()<<"        "<<this->requireNotReadonly<<tab->tableIsReadonly();
        if (this->requireColumnGraphics && !record->inherits("QFRDRColumnGraphsInterface")) ok=false;
        if (this->requireNotReadonly && tab->tableIsReadonly()) ok=false;

    }
    //qDebug()<<"match "<<record->getName()<<" : "<<ok;
    return ok;
}










QFRDRTableComboBox::QFRDRTableComboBox(QWidget *parent) :
    QFRDRComboBox(parent)
{
    tabFunctor=new QFRDRTableComboBoxMatchFunctor();
}

QFRDRTableComboBox::~QFRDRTableComboBox()
{
    delete tabFunctor;
}

void QFRDRTableComboBox::init(QFProject *project)
{
    QFRDRComboBox::init(project, tabFunctor, false);
}

QFRDRColumnGraphsInterface *QFRDRTableComboBox::currentColumnGraphics() const
{
    return dynamic_cast<QFRDRColumnGraphsInterface*>(currentRDR());
}

QFRDRTableInterface *QFRDRTableComboBox::currentTable() const
{
    return dynamic_cast<QFRDRTableInterface*>(currentRDR());
}

void QFRDRTableComboBox::setRequireColumnGraphics(bool require)
{
    tabFunctor->setRequireColumnGraphics(require);
}

void QFRDRTableComboBox::setRequireNotReadonly(bool require)
{
    tabFunctor->setRequireNotReadonly(require);
}

void QFRDRTableComboBox::myCurrentIndexChanged(int i)
{
    QFRDRComboBox::myCurrentIndexChanged(i);
    //Debug()<<"emitting currentColumnGraphicsChanged("<<currentColumnGraphics()<<")";
    emit currentColumnGraphicsChanged(currentColumnGraphics());
    //qDebug()<<"emitting currentTableChanged("<<currentTable()<<")";
    emit currentTableChanged(currentTable());

}

QFRDRColumnGraphsComboBox::QFRDRColumnGraphsComboBox(QWidget *parent):
    QComboBox(parent)
{
    cols=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
}

QFRDRColumnGraphsComboBox::~QFRDRColumnGraphsComboBox()
{
}

QFRDRColumnGraphsInterface *QFRDRColumnGraphsComboBox::currentColumnGraphics() const
{
    return cols;
}

void QFRDRColumnGraphsComboBox::setColumnGraphs(QFRDRColumnGraphsInterface *cols)
{
    this->cols=cols;
    refill();
}

void QFRDRColumnGraphsComboBox::refill()
{
    QString current=currentText();

    setUpdatesEnabled(false);
    disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));

    clear();
    if (cols) {
        for (int i=0; i<cols->colgraphGetPlotCount(); i++) {
            addItem(cols->colgraphGetPlotTitle(i));
        }
    }

    setUpdatesEnabled(true);

    int idx=findText(current);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(0);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
    if (count()<=0) emit refilledEmpty();
    emit refilled(count()>0);
}

void QFRDRColumnGraphsComboBox::myCurrentIndexChanged(int i)
{
    //qDebug()<<"myCurrentIndexChanged(index="<<i<<")";
}

