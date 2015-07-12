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

#include "qfbasicimageimportersoptionswidget.h"
#include "ui_qfbasicimageimportersoptionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"

QFBasicImageImportersOptionsWidget::QFBasicImageImportersOptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::QFBasicImageImportersOptionsWidget)
{
    ui->setupUi(this);
    ui->edtLibTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::OpenFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
    ui->edtLibTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::SelectFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
    ui->edtTinyTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::OpenFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
    ui->edtTinyTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::SelectFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
}

QFBasicImageImportersOptionsWidget::~QFBasicImageImportersOptionsWidget()
{
    delete ui;
}

void QFBasicImageImportersOptionsWidget::readSettings(ProgramOptions *options)
{
    ui->chkLibTIFFLogging->setChecked(options->getConfigValue("importers_basicimages/libtiff/log", false).toBool());
    ui->edtLibTIFFLogfile->setText(options->getConfigValue("importers_basicimages/libtiff/logfile", options->getConfigFileDirectory()+"/importers_basicimages_libtiff.log").toString());
    ui->chkTinyTIFFLogging->setChecked(options->getConfigValue("importers_basicimages/tinytiff/log", false).toBool());
    ui->edtTinyTIFFLogfile->setText(options->getConfigValue("importers_basicimages/tinytiff/logfile", options->getConfigFileDirectory()+"/importers_basicimages_tinytiff.log").toString());
}

void QFBasicImageImportersOptionsWidget::writeSettings(ProgramOptions *options)
{
    options->setConfigValue("importers_basicimages/libtiff/log", ui->chkLibTIFFLogging->isChecked());
    options->setConfigValue("importers_basicimages/libtiff/logfile", ui->edtLibTIFFLogfile->text());
    options->setConfigValue("importers_basicimages/tinytiff/log", ui->chkTinyTIFFLogging->isChecked());
    options->setConfigValue("importers_basicimages/tinytiff/logfile", ui->edtTinyTIFFLogfile->text());
}

void QFBasicImageImportersOptionsWidget::on_btnClearLibTIFFLog_clicked()
{
    if (QFile::exists(ui->edtLibTIFFLogfile->text())) QFile::remove(ui->edtLibTIFFLogfile->text());
}

void QFBasicImageImportersOptionsWidget::on_btnClearTinyTIFFLog_clicked()
{
    if (QFile::exists(ui->edtTinyTIFFLogfile->text())) QFile::remove(ui->edtTinyTIFFLogfile->text());
}
