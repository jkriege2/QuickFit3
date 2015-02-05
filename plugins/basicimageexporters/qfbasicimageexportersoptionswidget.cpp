/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfbasicimageexportersoptionswidget.h"
#include "ui_qfbasicimageexportersoptionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"

QFBasicImageExportersOptionsWidget::QFBasicImageExportersOptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::QFBasicImageExportersOptionsWidget)
{
    ui->setupUi(this);
    ui->edtLibTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::OpenFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
    ui->edtLibTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::SelectFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
}

QFBasicImageExportersOptionsWidget::~QFBasicImageExportersOptionsWidget()
{
    delete ui;
}

void QFBasicImageExportersOptionsWidget::readSettings(ProgramOptions *options)
{
    ui->chkLibTIFFLogging->setChecked(options->getConfigValue("exporters_basicimages/libtiff/log", false).toBool());
    ui->edtLibTIFFLogfile->setText(options->getConfigValue("exporters_basicimages/libtiff/logfile", options->getConfigFileDirectory()+"/exporters_basicimages_libtiff.log").toString());
}

void QFBasicImageExportersOptionsWidget::writeSettings(ProgramOptions *options)
{
    options->setConfigValue("exporters_basicimages/libtiff/log", ui->chkLibTIFFLogging->isChecked());
    options->setConfigValue("exporters_basicimages/libtiff/logfile", ui->edtLibTIFFLogfile->text());
}

void QFBasicImageExportersOptionsWidget::on_btnClearLibTIFFLog_clicked()
{
    if (QFile::exists(ui->edtLibTIFFLogfile->text())) QFile::remove(ui->edtLibTIFFLogfile->text());
}

