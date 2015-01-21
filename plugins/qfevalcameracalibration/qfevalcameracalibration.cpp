/*
    Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-02 17:43:02 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3433 $)

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


#include "qfevalcameracalibration.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfevalcameracalibration_item.h"

QFEvalCameraCalibration::QFEvalCameraCalibration(QObject* parent):
    QObject(parent)
{
    //ctor
}

QFEvalCameraCalibration::~QFEvalCameraCalibration()
{
    //dtor
}

QFEvaluationItem* QFEvalCameraCalibration::createRecord(QFProject* parent) {
    return new QFEvalCameraCalibrationItem(parent);
}


void QFEvalCameraCalibration::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(getIconFilename()), getName(), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}




void QFEvalCameraCalibration::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}

Q_EXPORT_PLUGIN2(eval_cameracalibration, QFEvalCameraCalibration)
