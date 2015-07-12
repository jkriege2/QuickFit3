/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfrdrcombobox.h"

QFRDRComboBox::QFRDRComboBox(QWidget *parent) :
    QComboBox(parent)
{
    project=NULL;
    matchFunctor=NULL;
    functorPrivate=false;
    view()->setTextElideMode(Qt::ElideMiddle);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
}

QFRDRComboBox::~QFRDRComboBox()
{
    if (functorPrivate && matchFunctor) delete matchFunctor;
}

void QFRDRComboBox::init(QFProject *project, QFMatchRDRFunctor *matchFunctor, bool functorPrivate)
{

    this->project=project;
    if (this->functorPrivate &&this->matchFunctor) delete this->matchFunctor;
    this->matchFunctor=matchFunctor;
    this->functorPrivate=functorPrivate;

    refill();

}

QFRawDataRecord *QFRDRComboBox::currentRDR() const
{
    int ID=itemData(currentIndex()).toInt();
    if (project) {
        return project->getRawDataByID(ID);
    }
    return NULL;
}

int QFRDRComboBox::currentRDRID() const
{
    int ID=itemData(currentIndex()).toInt();
    if (project && currentIndex()>=0) return ID;
    return -1;
}


void QFRDRComboBox::setCurrentRDR(const QFRawDataRecord *record)
{
    if (!record) {
        setCurrentIndex(-1);
    } else {
        int idx=findData(record->getID());
        setCurrentIndex(idx);
    }
}

void QFRDRComboBox::setCurrentRDRID(int record)
{
    int idxFound=findData(record);
    if (idxFound>=0) setCurrentIndex(idxFound);

}

void QFRDRComboBox::refill()
{
    QFRawDataRecord *rdr=currentRDR();

    bool idxOK=true;
    int rdrID=itemData(currentIndex()).toInt(&idxOK);
    if (currentIndex()<0) idxOK=false;
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);

    clear();
    if (project) disconnect(project, SIGNAL(structureChanged()), this, SLOT(refill()));
    disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));

    QList<QPointer<QFRawDataRecord> > lst;
    if (matchFunctor && project) lst=matchFunctor->getFilteredList(project);
    else if (project) {
        QList<QFRawDataRecord*> rl=project->getRawDataList();
        for (int i=0; i<rl.size(); i++) {
            lst<<rl[i];
        }
    }

    for (int i=0; i<lst.size(); i++) {
        if (lst[i]) addItem(lst[i]->getSmallIcon(), lst[i]->getName(), lst[i]->getID());
    }

    int idxFound=findData(rdrID);
    if (idxOK && (idxFound>=0)) setCurrentIndex(idxFound);
    else setCurrentIndex(0);
    if (widVisible) setUpdatesEnabled(true);
    if (this->project) connect(this->project, SIGNAL(structureChanged()), this, SLOT(refill()));
    setCurrentRDR(rdr);
    if (currentIndex()<0 && count()>0) setCurrentIndex(0);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
    if (currentRDR()!=rdr) emit currentRDRChanged(currentRDR());
    if (count()<=0) emit refilledEmpty();
    emit refilled(count()>0);
}

void QFRDRComboBox::myCurrentIndexChanged(int /*i*/)
{
    emit currentRDRChanged(currentRDR());
}
