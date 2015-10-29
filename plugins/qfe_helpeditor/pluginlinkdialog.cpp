/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "pluginlinkdialog.h"
#include "ui_pluginlinkdialog.h"
#include "qfpluginservices.h"
#include "qfextensionmanager.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "qfrawdatarecordfactory.h"
#include "qfevaluationitemfactory.h"

PluginLinkDialog::PluginLinkDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::PluginLinkDialog)
{
    ui->setupUi(this);
    QStringList ids;
    ids=QFPluginServices::getInstance()->getRawDataRecordFactory()->getIDList();
    for (int i=0; i<ids.size(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getRawDataRecordFactory()->getIconFilename(ids[i])), QFPluginServices::getInstance()->getRawDataRecordFactory()->getName(ids[i]), ids[i]);
    }
    ids=QFPluginServices::getInstance()->getEvaluationItemFactory()->getIDList();
    for (int i=0; i<ids.size(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getEvaluationItemFactory()->getIconFilename(ids[i])), QFPluginServices::getInstance()->getEvaluationItemFactory()->getName(ids[i]), ids[i]);
    }
    ids=QFPluginServices::getInstance()->getExtensionManager()->getIDList();
    for (int i=0; i<ids.size(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getExtensionManager()->getIconFilename(ids[i])), QFPluginServices::getInstance()->getExtensionManager()->getName(ids[i]), ids[i]);
    }

    for (int i=0; i<QFPluginServices::getInstance()->getFitFunctionManager()->pluginCount(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getFitFunctionManager()->getIconFilename(i)), QFPluginServices::getInstance()->getFitFunctionManager()->getName(i), QFPluginServices::getInstance()->getFitFunctionManager()->getID(i));
    }
    for (int i=0; i<QFPluginServices::getInstance()->getFitAlgorithmManager()->pluginCount(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getFitAlgorithmManager()->getIconFilename(i)), QFPluginServices::getInstance()->getFitAlgorithmManager()->getName(i), QFPluginServices::getInstance()->getFitAlgorithmManager()->getID(i));
    }

}

PluginLinkDialog::~PluginLinkDialog()
{
    delete ui;
}

QString PluginLinkDialog::insertText() const
{
    QString id=ui->cmbPlugin->itemData(ui->cmbPlugin->currentIndex()).toString();
    switch (ui->cmbType->currentIndex()) {
        case 0:
            return QString("<a href=\"$$plugin_info:help:")+id+QString("$$\">")+id+QString(" Help</a>");
        case 1:
            return QString("<a href=\"$$plugin_info:tutorial:")+id+QString("$$\">")+id+QString(" Tutorial</a>");
        case 2:
            return QString("$$plugin_info:helpdir:")+id+QString("$$");
        case 3:
            return QString("$$plugin_info:assetsdir:")+id+QString("$$");
        case 4:
            return QString("$$plugin_info:examplesdir:")+id+QString("$$");
        case 5:
            return QString("$$plugin_info:configdir:")+id+QString("$$");
        case 6:
            return QString("$$plugin_info:faq:")+id+QString("$$");
        case 7:
            return QString("$$list:plugin_tutorials:")+id+QString("$$");
        default:
            return id;
    }
    return id;
}
