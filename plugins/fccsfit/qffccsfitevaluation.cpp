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

#include "qffccsfitevaluation.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qffccsfitevaluation_item.h"
#include "qffcstools.h"
#include "optionswidget.h"
#include "qfrdrimagetoruninterface.h"
#include <stdint.h>
#include "qfmathtools.h"
#include "qfrawdatarecord.h"

QFFCCSFitEvaluation::QFFCCSFitEvaluation(QObject* parent):
    QObject(parent)
{
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFFCCSFitEvaluation::~QFFCCSFitEvaluation()
{
    //dtor
}

QFEvaluationItem* QFFCCSFitEvaluation::createRecord(QFProject* parent) {
    return new QFFCCSFitEvaluationItem(parent);
}


void QFFCCSFitEvaluation::registerToMenu(QMenu* menu) {
    QAction* actAddEval=new QAction(QIcon(getIconFilename()), tr("FCCS Curve Fitting"), parentWidget);
    actAddEval->setStatusTip(getDescription());
    connect(actAddEval, SIGNAL(triggered()), this, SLOT(insertEvaluation()));
    menu->addAction(actAddEval);
}

void QFFCCSFitEvaluation::init()
{
}

QString QFFCCSFitEvaluation::pluginOptionsName() const
{
    return getName();
}

QIcon QFFCCSFitEvaluation::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFFCCSFitEvaluation::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}




void QFFCCSFitEvaluation::insertEvaluation() {
    if (project) {
        project->addEvaluation(getID(), getName());
    }
}



Q_EXPORT_PLUGIN2(fccs_fit, QFFCCSFitEvaluation)
