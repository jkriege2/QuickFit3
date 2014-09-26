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

#include "qfmatchrdrfunctor.h"

QFMatchRDRFunctor::QFMatchRDRFunctor()
{
    //qDebug()<<"QFSelectRDRDialogMatchFunctor:constructor ";
}

QFMatchRDRFunctor::~QFMatchRDRFunctor()
{
}

QList<QPointer<QFRawDataRecord > > QFMatchRDRFunctor::getFilteredList(QFProject *project)
{
    QList<QPointer<QFRawDataRecord > >  rdrList;
    if (project) {
        for (int i=0; i<project->getRawDataCount(); i++) {
            QFRawDataRecord* r=project->getRawDataByNum(i);
            if (r && matches(r)) {
                rdrList<<r;
            }
        }
    }
    return rdrList;
}

QFMatchRDRFunctorSelectAll::QFMatchRDRFunctorSelectAll():
    QFMatchRDRFunctor()
{
    //qDebug()<<"QFSelectRDRDialogMatchFunctorSelectAll:constructor ";
}

bool QFMatchRDRFunctorSelectAll::matches(const QFRawDataRecord *record) const
{
    //qDebug()<<"QFSelectRDRDialogMatchFunctorSelectAll";
    return true;
}




QFMatchRDRFunctorSelectType::QFMatchRDRFunctorSelectType(const QString &type)
{
    this->type=type;
}

bool QFMatchRDRFunctorSelectType::matches(const QFRawDataRecord *record) const
{
    return record->getType().toLower()==type;
}

QString QFMatchRDRFunctorSelectType::getType() const
{
    return type;
}

void QFMatchRDRFunctorSelectType::setType(const QString &type)
{
    this->type=type;
}


QFMatchRDRFunctorSelectApplicable::QFMatchRDRFunctorSelectApplicable(QFEvaluationItem *item)
{
    this->item=item;
}

bool QFMatchRDRFunctorSelectApplicable::matches(const QFRawDataRecord *record) const
{
    return item && item->isApplicable(record);
}

QFEvaluationItem *QFMatchRDRFunctorSelectApplicable::getItem() const
{
    return item;
}

void QFMatchRDRFunctorSelectApplicable::setItem(QFEvaluationItem *item)
{
    this->item=item;
}
