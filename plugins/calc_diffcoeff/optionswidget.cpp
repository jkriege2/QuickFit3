/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "optionswidget.h"
#include "ui_optionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"
#include "qfstyledbutton.h"
#include "qfpluginservices.h"

OptionsWidget::OptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::OptionsWidget)
{
    ui->setupUi(this);
    QFStyledButton* btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtUserDatabase, ui->edtUserDatabase);
    btn->setFilter("user database files (*.ini)");
    ui->edtUserDatabase->addButton(btn);
}

OptionsWidget::~OptionsWidget()
{
    delete ui;
}

void OptionsWidget::readSettings(ProgramOptions *options)
{
    ui->edtUserDatabase->setText(options->getConfigValue("calc_diffcoeff/user_database", QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/calc_diffcoeff/mysamples.ini").toString());
}

void OptionsWidget::writeSettings(ProgramOptions *options)
{

    options->setConfigValue("calc_diffcoeff/user_database", ui->edtUserDatabase->text());
}
