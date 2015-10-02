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

#include "optionswidget.h"
#include "ui_optionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"
#include "qfstyledbutton.h"

OptionsWidget::OptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::OptionsWidget)
{
    ui->setupUi(this);
    QDir().mkpath(QFPluginServices::getInstance()->getGlobalConfigFileDirectory());
    QFStyledButton* btn;
   ui->edtChannels->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtChannels, ui->edtChannels));
    btn->setFilter(tr("INI-File (*.ini)"));
    ui->edtChannels->addButton(btn=new QFStyledButton(QFStyledButton::OpenFile, ui->edtChannels, ui->edtChannels));
    ui->edtPorts->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtPorts, ui->edtPorts));
    btn->setFilter(tr("INI-File (*.ini)"));
    ui->edtPorts->addButton(btn=new QFStyledButton(QFStyledButton::OpenFile, ui->edtPorts, ui->edtPorts));
    ui->edtTiming->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtTiming, ui->edtTiming));
    btn->setFilter(tr("INI-File (*.ini)"));
    ui->edtTiming->addButton(btn=new QFStyledButton(QFStyledButton::OpenFile, ui->edtTiming, ui->edtTiming));

}

OptionsWidget::~OptionsWidget()
{
    delete ui;
}

void OptionsWidget::readSettings(ProgramOptions *options)
{
    ui->edtChannels->setText(options->getConfigValue("qfe_alexcontrol/channelsFilename", QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/alexc_channels.ini").toString());
    ui->edtPorts->setText(options->getConfigValue("qfe_alexcontrol/portFilename", QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/alexc_ports.ini").toString());
    ui->edtTiming->setText(options->getConfigValue("qfe_alexcontrol/timingFilename", QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/alexc_timing.ini").toString());
}

void OptionsWidget::writeSettings(ProgramOptions *options)
{

    options->setConfigValue("qfe_alexcontrol/channelsFilename", ui->edtChannels->text());
    options->setConfigValue("qfe_alexcontrol/portFilename", ui->edtPorts->text());
    options->setConfigValue("qfe_alexcontrol/timingFilename", ui->edtTiming->text());
    emit writingSettings();
}
