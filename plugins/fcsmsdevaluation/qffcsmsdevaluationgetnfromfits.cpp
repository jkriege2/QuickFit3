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

#include "qffcsmsdevaluationgetnfromfits.h"
#include "ui_qffcsmsdevaluationgetnfromfits.h"
#include "qfproject.h"
#include "qfpluginservices.h"
#include "qfevaluationitem.h"

QFFCSMSDEvaluationGetNFromFits::QFFCSMSDEvaluationGetNFromFits(QFRawDataRecord *current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFCSMSDEvaluationGetNFromFits)
{
    ui->setupUi(this);
    ui->cmbApplyTo->setCurrentIndex(ProgramOptions::getConfigValue("QFFCSMSDEvaluationGetNFromFits/applyto", 0).toInt());
    ui->chkGetN->setChecked(ProgramOptions::getConfigValue("QFFCSMSDEvaluationGetNFromFits/getN", true).toBool());
    ui->chkGetTriplet->setChecked(ProgramOptions::getConfigValue("QFFCSMSDEvaluationGetNFromFits/getTriplet", true).toBool());
    ui->chkGetFocus->setChecked(ProgramOptions::getConfigValue("QFFCSMSDEvaluationGetNFromFits/chkGetFocus", true).toBool());

    ui->cmbEval->setRDR(current);
}

QFFCSMSDEvaluationGetNFromFits::~QFFCSMSDEvaluationGetNFromFits()
{
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/applyto", getApplyTo());
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/getN", getN());
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/getTriplet", getTriplet());
    ProgramOptions::setConfigValue("QFFCSMSDEvaluationGetNFromFits/chkGetFocus", getFocusParams());
    delete ui;
}

int QFFCSMSDEvaluationGetNFromFits::getApplyTo() const
{
    return qMax(0, ui->cmbApplyTo->currentIndex());
}

QString QFFCSMSDEvaluationGetNFromFits::getEvalGroup() const
{
    return ui->cmbEval->currentEvaluationGroup();
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterN() const
{
    return "fitparam_n_particle";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterTripletTheta() const
{
    return "fitparam_nonfl_theta1";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterTripletTau() const
{
    return "fitparam_nonfl_tau1";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterDarkTheta() const
{
    return "fitparam_nonfl_theta2";

}

QString QFFCSMSDEvaluationGetNFromFits::getParameterDarkTau() const
{
    return "fitparam_nonfl_tau2";

}

QString QFFCSMSDEvaluationGetNFromFits::getParameterWxy() const
{
    return "fitparam_focus_width";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterWz() const
{
    return "fitparam_focus_height";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterGamma() const
{
    return "fitparam_focus_struct_fac";
}

QString QFFCSMSDEvaluationGetNFromFits::getParameterPixelSize() const
{
    return "fitparam_pixel_width";
}

bool QFFCSMSDEvaluationGetNFromFits::getN() const
{
    return ui->chkGetN->isChecked();
}

bool QFFCSMSDEvaluationGetNFromFits::getTriplet() const
{
    return ui->chkGetTriplet->isChecked();
}

bool QFFCSMSDEvaluationGetNFromFits::getFocusParams() const
{
    return ui->chkGetFocus->isChecked();
}

