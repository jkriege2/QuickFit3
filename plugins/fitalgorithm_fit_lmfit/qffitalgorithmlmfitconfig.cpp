/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qffitalgorithmlmfitconfig.h"
#include "ui_qffitalgorithmlmfitconfig.h"

QFFitAlgorithmLMFitConfigDialog::QFFitAlgorithmLMFitConfigDialog(QWidget *parent, bool IRLS) :
    QDialog(parent),
    ui(new Ui::QFFitAlgorithmLMFitConfigDialog)
{
    ui->setupUi(this);
    ui->grpIRLS->setVisible(IRLS);
}

QFFitAlgorithmLMFitConfigDialog::~QFFitAlgorithmLMFitConfigDialog()
{
    delete ui;
}

void QFFitAlgorithmLMFitConfigDialog::init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations) {
    ui->spinEpsilon->setValue(epsilon);
    ui->spinFTol->setValue(ftol);
    ui->spinGTol->setValue(gtol);
    ui->spinIterations->setValue(max_iterations);
    ui->spinStepbound->setValue(stepbound);
    ui->spinXTol->setValue(xtol);
    ui->grpIRLS->setVisible(false);
}

void QFFitAlgorithmLMFitConfigDialog::init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations, double irls_parameter, int irls_iterations) {
    init(ftol, xtol, gtol, epsilon, stepbound, max_iterations);
    ui->spinIRLSIterations->setValue(irls_iterations);
    ui->spinIRLSParameter->setValue(irls_parameter);
    ui->grpIRLS->setVisible(true);
}

double QFFitAlgorithmLMFitConfigDialog::irls_parameter() const {
    return ui->spinIRLSParameter->value();
}

int QFFitAlgorithmLMFitConfigDialog::irls_iterations() const {
    return ui->spinIRLSIterations->value();
}

double QFFitAlgorithmLMFitConfigDialog::ftol() const {
    return ui->spinFTol->value();
}

double QFFitAlgorithmLMFitConfigDialog::xtol() const {
    return ui->spinXTol->value();
}

double QFFitAlgorithmLMFitConfigDialog::gtol() const {
    return ui->spinGTol->value();
}

double QFFitAlgorithmLMFitConfigDialog::epsilon() const {
    return ui->spinEpsilon->value();
}

double QFFitAlgorithmLMFitConfigDialog::stepbound() const {
    return ui->spinStepbound->value();
}

int QFFitAlgorithmLMFitConfigDialog::max_iterations() const {
    return ui->spinIterations->value();
}
