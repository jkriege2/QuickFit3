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

#include "qfrdrimagingfcspostprocessbinningdialog.h"
#include "ui_qfrdrimagingfcspostprocessbinningdialog.h"

QFRDRImagingFCSPostProcessBinningDialog::QFRDRImagingFCSPostProcessBinningDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFRDRImagingFCSPostProcessBinningDialog)
{
    ui->setupUi(this);
}

QFRDRImagingFCSPostProcessBinningDialog::~QFRDRImagingFCSPostProcessBinningDialog()
{
    delete ui;
}

void QFRDRImagingFCSPostProcessBinningDialog::init(int bin, bool interleaved)
{
    ui->chkOther->setChecked(false);
    ui->chkInterleaved->setChecked(interleaved);
    ui->spinBin->setValue(bin);

}

int QFRDRImagingFCSPostProcessBinningDialog::getBin() const
{
    return ui->spinBin->value();
}

bool QFRDRImagingFCSPostProcessBinningDialog::getInterleaved() const
{
    return ui->chkInterleaved->isChecked();
}

bool QFRDRImagingFCSPostProcessBinningDialog::getOtherRDR() const
{
    return ui->chkOther->isChecked();
}


void QFRDRImagingFCSPostProcessBinningDialog::on_chkInterleaved_toggled(bool checked)
{
    if (checked) {
        ui->labExplanation->setText(tr("<i>The autocorrelation curves from N*N pixel patch are averaged, but the patches overlap. So the image size is retained.</i>"));
    } else {
        ui->labExplanation->setText(tr("<i>The autocorrelation curves from each N*N pixel patch are averaged. The final image size of the record is reduced to width/N * height/N</i>"));
    }
}
