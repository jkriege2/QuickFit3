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

#include "qffcsmsdevaluationaveragechannelsdialog.h"
#include "ui_qffcsmsdevaluationaveragechannelsdialog.h"
#include "programoptions.h"

QFFCSMSDEvaluationAverageChannelsDialog::QFFCSMSDEvaluationAverageChannelsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFCSMSDEvaluationAverageChannelsDialog)
{
    ui->setupUi(this);
    ui->cmbApplyTo->setCurrentIndex(ProgramOptions::getConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/applyto", 0).toInt());
    ui->spinPoints->setValue(ProgramOptions::getConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/points", 5).toInt());
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFFCSMSDEvaluationAverageChannelsDialog::~QFFCSMSDEvaluationAverageChannelsDialog()
{
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/applyto", getApplyTo());
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationAverageChannelsDialog/points", getPoints());
    delete ui;
}

void QFFCSMSDEvaluationAverageChannelsDialog::init(int pointsToAvg, int rangeMin, int rangeMax)
{
    ui->spinPoints->setRange(rangeMin, rangeMax);
    ui->spinPoints->setValue(pointsToAvg);
}

int QFFCSMSDEvaluationAverageChannelsDialog::getPoints() const
{
    return ui->spinPoints->value();
}

int QFFCSMSDEvaluationAverageChannelsDialog::getApplyTo() const
{
    return qMax(0, ui->cmbApplyTo->currentIndex());
}
