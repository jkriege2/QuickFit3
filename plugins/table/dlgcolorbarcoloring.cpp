/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "dlgcolorbarcoloring.h"
#include "ui_dlgcolorbarcoloring.h"
#include "jkqtpimageelements.h"
#include "jkqtpimagetools.h"
#include "programoptions.h"

DlgColorbarColoring::DlgColorbarColoring(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgColorbarColoring)
{
    ui->setupUi(this);
    ui->radBar->setChecked(ProgramOptions::getConfigValue("table/DlgColorbarColoring/rad_bar", ui->radBar->isChecked()).toBool());
    ui->radBar2->setChecked(ProgramOptions::getConfigValue("table/DlgColorbarColoring/rad_bar2", ui->radBar2->isChecked()).toBool());
    ui->radBar3->setChecked(ProgramOptions::getConfigValue("table/DlgColorbarColoring/rad_bar3", ui->radBar3->isChecked()).toBool());
    ui->cmbColorbar->setCurrentIndex(ProgramOptions::getConfigValue("table/DlgColorbarColoring/colorbar", ui->cmbColorbar->currentIndex()).toInt());
    ui->cmbCol21->setCurrentIndex(ProgramOptions::getConfigValue("table/DlgColorbarColoring/color21", ui->cmbCol21->currentIndex()).toInt());
    ui->cmbCol22->setCurrentIndex(ProgramOptions::getConfigValue("table/DlgColorbarColoring/color22", ui->cmbCol22->currentIndex()).toInt());
    ui->cmbCol31->setCurrentIndex(ProgramOptions::getConfigValue("table/DlgColorbarColoring/color31", ui->cmbCol31->currentIndex()).toInt());
    ui->cmbCol32->setCurrentIndex(ProgramOptions::getConfigValue("table/DlgColorbarColoring/color32", ui->cmbCol32->currentIndex()).toInt());
    ui->cmbCol33->setCurrentIndex(ProgramOptions::getConfigValue("table/DlgColorbarColoring/color33", ui->cmbCol33->currentIndex()).toInt());
}

DlgColorbarColoring::~DlgColorbarColoring()
{
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/rad_bar", ui->radBar->isChecked());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/rad_bar2", ui->radBar2->isChecked());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/rad_bar3", ui->radBar3->isChecked());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/colorbar", ui->cmbColorbar->currentIndex());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/color21", ui->cmbCol21->currentIndex());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/color22", ui->cmbCol22->currentIndex());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/color31", ui->cmbCol31->currentIndex());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/color32", ui->cmbCol32->currentIndex());
    ProgramOptions::setConfigValue("table/DlgColorbarColoring/color33", ui->cmbCol33->currentIndex());
    delete ui;
}

void DlgColorbarColoring::init(const QStringList &graphs)
{
    for (int i=0; i<graphs.size(); i++) {
        QListWidgetItem* item=new QListWidgetItem(graphs[i]);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        item->setCheckState(Qt::Checked);
        ui->listWidget->addItem(item);
    }
}

QList<bool> DlgColorbarColoring::getSelected() const {
    QList<bool> res;
    for (int i=0; i<ui->listWidget->count(); i++) {
        QListWidgetItem* it=ui->listWidget->item(i);
        if (it) {
            res<<(it->checkState()==Qt::Checked);
        }
    }
    return res;
}

QList<QColor> DlgColorbarColoring::getColors() const
{
    QList<QColor> res;
    QList<bool> chk=getSelected();
    int count=0;
    for (int i=0; i<chk.size(); i++) {
        if (chk.value(i, true)) {
            count++;
        }
    }
    int cnt=0;
    QImage col(count,1,QImage::Format_ARGB32);
    if (ui->radBar->isChecked()) {
        col=JKQTPMathImage::getPaletteImage(ui->cmbColorbar->colorPalette(), count);
    } else if (ui->radBar2->isChecked()) {
        QList<QPair<double, QRgb> > vals;
        vals<<qMakePair(0.0, ui->cmbCol21->currentColor().rgb());
        vals<<qMakePair(1.0, ui->cmbCol22->currentColor().rgb());
        int* lut=(int*)qfMalloc((2+count)*sizeof(int));
        JKQTPimagePlot_buildDefinedPaletteLinInterpolate(lut, vals, count);
        for (int i=0; i<count; i++) {
            col.setPixel(i,0,lut[i]);
        }
        qfFree(lut);
    } else if (ui->radBar3->isChecked()) {
        QList<QPair<double, QRgb> > vals;
        vals<<qMakePair(0.0, ui->cmbCol31->currentColor().rgb());
        vals<<qMakePair(1.0, ui->cmbCol32->currentColor().rgb());
        vals<<qMakePair(2.0, ui->cmbCol33->currentColor().rgb());
        int* lut=(int*)qfMalloc((2+count)*sizeof(int));
        JKQTPimagePlot_buildDefinedPaletteLinInterpolate(lut, vals, count);
        for (int i=0; i<count; i++) {
            col.setPixel(i,0,lut[i]);
        }
        qfFree(lut);
    }
    for (int i=0; i<chk.size(); i++) {
        if (chk.value(i, true)) {
            res<<col.pixel(cnt,0);
            cnt++;
        } else {
            res<<QColor("black");
        }
    }
    return res;
}
