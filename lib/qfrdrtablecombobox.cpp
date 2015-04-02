/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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
