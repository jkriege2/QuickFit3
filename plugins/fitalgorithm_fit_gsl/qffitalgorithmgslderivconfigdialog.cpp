/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qffitalgorithmgslderivconfigdialog.h"
#include "ui_qffitalgorithmgslderivconfigdialog.h"

QFFitAlgorithmGSLDerivativeBaseConfigDialog::QFFitAlgorithmGSLDerivativeBaseConfigDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFFitAlgorithmGSLDerivativeBaseConfigDialog)
{
    ui->setupUi(this);
}

QFFitAlgorithmGSLDerivativeBaseConfigDialog::~QFFitAlgorithmGSLDerivativeBaseConfigDialog()
{
    delete ui;
}

void QFFitAlgorithmGSLDerivativeBaseConfigDialog::init(const QFFitAlgorithm *algorithm)
{
    ui->spinStepsize->setValue(algorithm->getParameter("stepsize").toDouble());
    ui->spinMaxIter->setValue(algorithm->getParameter("max_iterations").toInt());
    ui->spinTolerance->setValue(algorithm->getParameter("tol").toDouble());
    ui->spinGTolerance->setValue(algorithm->getParameter("gtol").toDouble());
}

void QFFitAlgorithmGSLDerivativeBaseConfigDialog::storeParams(QFFitAlgorithm *algorithm) const
{
    algorithm->setParameter("stepsize", ui->spinStepsize->value());
    algorithm->setParameter("max_iterations", ui->spinMaxIter->value());
    algorithm->setParameter("tol", ui->spinTolerance->value());
    algorithm->setParameter("gtol", ui->spinGTolerance->value());
}
