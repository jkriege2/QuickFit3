/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

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

#include "qffitalgorithmnloptnonderivconfigdialog.h"
#include "ui_qffitalgorithmnloptnonderivconfigdialog.h"

QFFitAlgorithmNLOptNonDerivConfigDialog::QFFitAlgorithmNLOptNonDerivConfigDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFFitAlgorithmNLOptNonDerivConfigDialog)
{
    ui->setupUi(this);
}

QFFitAlgorithmNLOptNonDerivConfigDialog::~QFFitAlgorithmNLOptNonDerivConfigDialog()
{
    delete ui;
}

void QFFitAlgorithmNLOptNonDerivConfigDialog::init(const QFFitAlgorithm *algorithm)
{
    ui->spinStepsize->setValue(algorithm->getParameter("stepsize").toDouble());
    ui->spinFTol->setValue(algorithm->getParameter("ftol").toDouble());
    ui->spinXTol->setValue(algorithm->getParameter("xtol").toDouble());
    ui->spinMaxTime->setValue(algorithm->getParameter("max_time").toDouble());
    ui->spinMaxEval->setValue(algorithm->getParameter("max_evals").toInt());
}

void QFFitAlgorithmNLOptNonDerivConfigDialog::storeParams(QFFitAlgorithm *algorithm) const
{
    algorithm->setParameter("stepsize", ui->spinStepsize->value());
    algorithm->setParameter("ftol", ui->spinFTol->value());
    algorithm->setParameter("xtol", ui->spinXTol->value());
    algorithm->setParameter("max_time", ui->spinMaxTime->value());
    algorithm->setParameter("max_evals", ui->spinMaxEval->value());
}
