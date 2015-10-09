/*
Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfpevalcurvefit.h"
#include "optionswidget.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfcurvefitevaluation.h"

QFPEvalCurveFit::QFPEvalCurveFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFPEvalCurveFit::~QFPEvalCurveFit()
{
    //dtor
}

QFEvaluationItem* QFPEvalCurveFit::createRecord(QFProject* parent) {
    return new QFCurveFitEvaluation(parent);
}


void QFPEvalCurveFit::registerToMenu(QMenu* menu) {
    QAction* actCURVE=new QAction(QIcon(":/curve_fit.png"), tr("Curve Fitting"), parentWidget);
    actCURVE->setStatusTip(tr("Insert a least-squares fit evaluation"));
    connect(actCURVE, SIGNAL(triggered()), this, SLOT(insertCurveFit()));
    menu->addAction(actCURVE);
}



QString QFPEvalCurveFit::pluginOptionsName() const
{
    return getName();
}

QIcon QFPEvalCurveFit::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFPEvalCurveFit::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}

void QFPEvalCurveFit::insertCurveFit() {
    if (project) {
        project->addEvaluation(getID(), "Curve Fit");
    }
}

Q_EXPORT_PLUGIN2(curve_fit, QFPEvalCurveFit)
