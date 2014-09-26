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


#include "qffcsmsdevaluation.h"
#include <QtGui>
#include "qffcsmsdevaluation_item.h"

QFFCSMSDEvaluation::QFFCSMSDEvaluation(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFFCSMSDEvaluation::~QFFCSMSDEvaluation()
{
    //dtor
}

QFEvaluationItem* QFFCSMSDEvaluation::createRecord(QFProject* parent) {
    return new QFFCSMSDEvaluationItem(parent);
}


void QFFCSMSDEvaluation::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(":/fcsmsdevaluation/fcsmsdevaluation_small.png"), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFFCSMSDEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(QFFCSMaxEntEvaluation, QFFCSMSDEvaluation)
