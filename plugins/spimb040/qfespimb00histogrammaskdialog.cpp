/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#include "qfespimb00histogrammaskdialog.h"
#include "ui_qfespimb00histogrammaskdialog.h"

QFESPIMB00HistogramMaskDialog::QFESPIMB00HistogramMaskDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFESPIMB00HistogramMaskDialog)
{
    ui->setupUi(this);
}

QFESPIMB00HistogramMaskDialog::~QFESPIMB00HistogramMaskDialog()
{
    delete ui;
}

int QFESPIMB00HistogramMaskDialog::mode() const {
    return ui->cmbMode->currentIndex();
}
void QFESPIMB00HistogramMaskDialog::setMode(int m) {
    ui->cmbMode->setCurrentIndex(m);
}

int QFESPIMB00HistogramMaskDialog::pixels() const {
    return ui->spinPixels->value();
}
void QFESPIMB00HistogramMaskDialog::setPixels(int pixels, int maxPixels) {
    ui->spinPixels->setMaximum(maxPixels);
    ui->spinPixels->setMinimum(0);
    ui->spinPixels->setValue(pixels);
}
