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

#include "qfpevalfcsfit.h"
#include "optionswidget.h"
#include <QtGui>
#include "qffcsfitevaluation.h"

QFPEvalFCSFit::QFPEvalFCSFit(QObject* parent):
    QObject(parent)
{
    //ctor
    QFPluginServices::getInstance()->registerSettingsPane(this);
}

QFPEvalFCSFit::~QFPEvalFCSFit()
{
    //dtor
}

QFEvaluationItem* QFPEvalFCSFit::createRecord(QFProject* parent) {
    return new QFFCSFitEvaluation(parent);
}


void QFPEvalFCSFit::registerToMenu(QMenu* menu) {
    QAction* actFCS=new QAction(QIcon(":/fcs_fit.png"), tr("FCS Curve Fitting"), parentWidget);
    actFCS->setStatusTip(tr("Insert a new FCS least-squares fit evaluation"));
    connect(actFCS, SIGNAL(triggered()), this, SLOT(insertFCSFit()));
    menu->addAction(actFCS);
}



QString QFPEvalFCSFit::pluginOptionsName() const
{
    return getName();
}

QIcon QFPEvalFCSFit::pluginOptionsIcon() const
{
    return QIcon(getIconFilename());
}

QFPluginOptionsWidget *QFPEvalFCSFit::createOptionsWidget(QWidget *parent)
{
    return new OptionsWidget(this, parent);
}

void QFPEvalFCSFit::insertFCSFit() {
    if (project) {
        project->addEvaluation(getID(), "FCS Fit");
    }
}

Q_EXPORT_PLUGIN2(qfevalfcsfit, QFPEvalFCSFit)
