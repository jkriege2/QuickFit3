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

#include "qfrawdataresultscombobox.h"

QFRawDataResultsComboBox::QFRawDataResultsComboBox(QWidget *parent) :
    QComboBox(parent)
{
    rdr=NULL;
    egroup="";
    resultGroupFilters<<"";
    evaluationIDFilter="";
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
}

void QFRawDataResultsComboBox::setRDR(QFRawDataRecord *rdr)
{
    if (this->rdr) disconnect(this->rdr, 0, this, 0);
    QVariant oldData=itemData(currentIndex());
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);

    this->rdr=rdr;
    if (rdr) {

        QList<QTriple<QString, QString, QString> > params;
        if (resultGroupFilters.size()<=0) params=rdr->resultsCalcNamesEvalIDsAndLabels(evaluationIDFilter, "", egroup);
        for (int i=0; i<resultGroupFilters.size(); i++) {
            QList<QTriple<QString, QString, QString> > params1=rdr->resultsCalcNamesEvalIDsAndLabels(evaluationIDFilter, resultGroupFilters[i], egroup);
            params.append(params1);
        }

        clear();
        evalIDs.clear();
        for (int i=0; i<params.size(); i++) {
            addItem(params[i].first, params[i].second);
            evalIDs[i]=params[i].third;
        }

        int idxFound=findData(oldData);
        if (oldData.isValid() && (idxFound>=0)) setCurrentIndex(idxFound);
        else setCurrentIndex(0);
    } else {
        evalIDs.clear();
        clear();
    }
    if (widVisible) setUpdatesEnabled(true);
    if (rdr) connect(rdr, SIGNAL(rawDataChanged()), this, SLOT(refill()));
}

QString QFRawDataResultsComboBox::currentResult() const
{
    return itemData(currentIndex()).toString();
}

QString QFRawDataResultsComboBox::currentEvaluationID() const
{
    return evalIDs.value(currentIndex(), "");
}


QString QFRawDataResultsComboBox::evaluationGroup() const
{
    return egroup;
}

void QFRawDataResultsComboBox::setResultGroupFilters(QStringList &resultGroupFilters)
{
    this->resultGroupFilters=resultGroupFilters;
    refill();
}

QFRawDataRecord *QFRawDataResultsComboBox::getRDR() const
{
    return rdr;
}


void QFRawDataResultsComboBox::setEvaluationGroup(const QString &group)
{
    this->egroup=group;
    refill();
}

void QFRawDataResultsComboBox::setCurrentResult(const QString &result)
{
    int idx=findData(result);
    if (idx>=0) setCurrentIndex(idx);
}

void QFRawDataResultsComboBox::setEvaluationIDFilter(const QString &evalID)
{
    evaluationIDFilter=evalID;
    refill();
}

void QFRawDataResultsComboBox::myCurrentIndexChanged(int /*i*/)
{
    emit currentResultChanged(currentResult(), currentEvaluationID());
}

void QFRawDataResultsComboBox::refill()
{
    if (rdr) setRDR(rdr);
    else clear();
}
