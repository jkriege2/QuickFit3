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

#include "dlgfccsfitallconfiguration.h"
#include "ui_dlgfccsfitallconfiguration.h"

DlgFCCSFitAllConfiguration::DlgFCCSFitAllConfiguration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgFCCSFitAllConfiguration)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

DlgFCCSFitAllConfiguration::~DlgFCCSFitAllConfiguration()
{
    delete ui;
}

bool DlgFCCSFitAllConfiguration::getFitGuessed() const
{
    return ui->chkFitGuessed->isChecked();
}


bool DlgFCCSFitAllConfiguration::getFitAllRuns() const
{
    return ui->cmbRuns->currentIndex()==0;
}


bool DlgFCCSFitAllConfiguration::getFitFitted() const
{
    return ui->chkFitFitted->isChecked();
}
