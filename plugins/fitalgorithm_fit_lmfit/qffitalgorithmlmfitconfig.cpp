/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
    All rights reserved.

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice, this
       list of conditions and the following disclaimer in the documentation and/or other
       materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may
       be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
