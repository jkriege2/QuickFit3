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

#include "dlgselectprojectsubset.h"
#include "ui_dlgselectprojectsubset.h"
#include "programoptions.h"
#include "qftools.h"
#include "qfprojecttreemodel.h"

DlgSelectProjectSubset::DlgSelectProjectSubset(QFProject *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSelectProjectSubset)
{
    this->project=project;
    ui->setupUi(this);

    loadWidgetGeometry(*(ProgramOptions::staticGetQSettings()), this, pos(), size(), "DlgSelectProjectSubset/window");
    loadSplitter(*(ProgramOptions::staticGetQSettings()), ui->splitter, "DlgSelectProjectSubset/splitter1");
    ui->treeView->setModel(project->getTreeModel());
    ui->labFile->setText(project->getFile());
    ui->labCreator->setText(project->getCreator());
    ui->labName->setText(project->getName());
    ui->labDescription->setText(project->getDescription());
    ui->treeView->expandAll();
}

DlgSelectProjectSubset::~DlgSelectProjectSubset()
{
    saveSplitter(*(ProgramOptions::staticGetQSettings()), ui->splitter, "DlgSelectProjectSubset/splitter1");
    saveWidgetGeometry(*(ProgramOptions::staticGetQSettings()), this, "DlgSelectProjectSubset/window");
    delete ui;
}

void DlgSelectProjectSubset::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgselectprojectsubset.html");

}
