/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qfrdrimagecopydataselectdialog.h"
#include "ui_qfrdrimagecopydataselectdialog.h"
#include "programoptions.h"

QFRDRImageCopyDataSelectDialog::QFRDRImageCopyDataSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImageCopyDataSelectDialog)
{
    ui->setupUi(this);
    ui->chkParameterImage->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/param", true).toBool());
    ui->chkGOF->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/gof", true).toBool());
    ui->chkMask->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/mask", true).toBool());
    ui->chkOverview->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/overview", true).toBool());
    ui->chkPixelCoordinate->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/pixelcoordinate", true).toBool());
    ui->chkPixelNumber->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/pixelnum", true).toBool());
}

QFRDRImageCopyDataSelectDialog::~QFRDRImageCopyDataSelectDialog()
{
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/param", ui->chkParameterImage->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/gof", ui->chkGOF->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/mask", ui->chkMask->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/overview", ui->chkOverview->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/pixelcoordinate", ui->chkPixelCoordinate->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/pixelnum", ui->chkPixelNumber->isChecked());

    delete ui;
}

bool QFRDRImageCopyDataSelectDialog::copyParam() const
{
    return ui->chkParameterImage->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyOvf() const
{
    return ui->chkOverview->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyParam2() const
{
    return ui->chkGOF->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyMask() const
{
    return ui->chkMask->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyID() const
{
    return ui->chkPixelNumber->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyCoordinates() const
{
    return ui->chkPixelCoordinate->isChecked();
}
