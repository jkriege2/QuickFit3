/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "spimb040optionswidget.h"
#include "ui_spimb040optionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"

SPIMB040OptionsWidget::SPIMB040OptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::SPIMB040OptionsWidget)
{
    ui->setupUi(this);
    reloadStylesheets(true);
    ui->edtOptSetupDir->addButton(new QFStyledButton(QFStyledButton::SelectDirectory, ui->edtOptSetupDir, ui->edtOptSetupDir));
    ui->edtOptSetupConfigDir->addButton(new QFStyledButton(QFStyledButton::SelectDirectory, ui->edtOptSetupConfigDir, ui->edtOptSetupConfigDir));
    ui->edtOptSetupConfigDirReadonly->addButton(new QFStyledButton(QFStyledButton::SelectDirectory, ui->edtOptSetupConfigDirReadonly, ui->edtOptSetupConfigDirReadonly));
}

SPIMB040OptionsWidget::~SPIMB040OptionsWidget()
{
    delete ui;
}

void SPIMB040OptionsWidget::readSettings(ProgramOptions *options)
{
    ui->edtOptSetupDir->setText(options->getConfigValue("spimb040/optsetup_directory", options->getGlobalConfigFileDirectory()).toString());
    ui->edtOptSetupConfigDir->setText(options->getConfigValue("spimb040/optsetup_config_directory", options->getConfigFileDirectory()).toString());
    ui->edtOptSetupConfigDirReadonly->setText(options->getConfigValue("spimb040/optsetup_config_directory_readonly", options->getGlobalConfigFileDirectory()).toString());
    ui->cmbStyle->setCurrentIndex(ui->cmbStyle->findText(options->getConfigValue("spimb040/style", ProgramOptions::getInstance()->getStyle()).toString(), Qt::MatchExactly ));
    ui->cmbStylesheet->setCurrentIndex(ui->cmbStylesheet->findText(options->getConfigValue("spimb040/stylesheet", ProgramOptions::getInstance()->getStylesheet()).toString(), Qt::MatchExactly ));
    emit styleChanged(getStyle(), getStylesheet());
}

void SPIMB040OptionsWidget::writeSettings(ProgramOptions *options)
{
    options->setConfigValue("spimb040/optsetup_directory", ui->edtOptSetupDir->text());
    options->setConfigValue("spimb040/optsetup_config_directory", ui->edtOptSetupConfigDir->text());
    options->setConfigValue("spimb040/optsetup_config_directory_readonly", ui->edtOptSetupConfigDirReadonly->text());
    options->setConfigValue("spimb040/style", ui->cmbStyle->currentText());
    options->setConfigValue("spimb040/stylesheet", ui->cmbStylesheet->currentText());
}

QString SPIMB040OptionsWidget::getStylesheet()
{
    return ui->cmbStylesheet->itemData(ui->cmbStylesheet->currentIndex()).toString();
}

QString SPIMB040OptionsWidget::getStyle()
{
    return ui->cmbStyle->currentText();
}

void SPIMB040OptionsWidget::on_cmbStylesheet_currentIndexChanged( int index ) {
    QString fn=ui->cmbStylesheet->itemData(index).toString();
    QFile f(fn);
    QString qss="";
    if (f.open(QFile::ReadOnly)) {
        QTextStream s(&f);
        qss=s.readAll();
    }
    this->setStyleSheet(qss);

    emit styleChanged(getStyle(), getStylesheet());
}


void SPIMB040OptionsWidget::on_cmbStyle_currentIndexChanged( const QString & text ) {
    parentWidget()->setStyle(QStyleFactory::create(text));
    parentWidget()->setPalette(parentWidget()->style()->standardPalette());
    emit styleChanged(getStyle(), getStylesheet());
}

void SPIMB040OptionsWidget::on_cmbStyle_highlighted( const QString & text ) {
    //on_cmbStyle_currentIndexChanged(text);
}

void SPIMB040OptionsWidget::reloadStylesheets(bool forSure)
{
    QString oldStyle=ui->cmbStyle->currentText();
    QString oldStyleSheet=ui->cmbStylesheet->currentText();
    if (forSure || !ui->cmbStyle->hasFocus()) {
        bool widVisible=ui->cmbStyle->isVisible(); if (widVisible) ui->cmbStyle->setUpdatesEnabled(false);
        ui->cmbStyle->clear();
        ui->cmbStyle->addItems(QStyleFactory::keys());
        ui->cmbStyle->setCurrentIndex(ui->cmbStyle->findText(oldStyle, Qt::MatchContains));
        if (widVisible) ui->cmbStyle->setUpdatesEnabled(true);
    }


    // find all available stylesheets
    if (forSure || !ui->cmbStylesheet->hasFocus()) {
        bool widVisible=ui->cmbStylesheet->isVisible(); if (widVisible) ui->cmbStylesheet->setUpdatesEnabled(false);
        QDir dir(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/spimb040/stylesheets/");
        QStringList filters, sl;
        dir.cd("stylesheets");
        filters.clear();
        filters << "*.qss";
        disconnect(ui->cmbStylesheet, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cmbStylesheet_currentIndexChanged(int)));
        ui->cmbStylesheet->clear();
        sl=dir.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            ui->cmbStylesheet->addItem(sl[i].remove(".qss", Qt::CaseInsensitive), dir.absoluteFilePath(sl[i]));
        }


        dir=QDir(ProgramOptions::getInstance()->getAssetsDirectory());
        dir.cd("stylesheets");
        sl.clear();
        sl=dir.entryList(filters, QDir::Files);
        for (int i=0; i<sl.size(); i++) {
            ui->cmbStylesheet->addItem(sl[i].remove(".qss", Qt::CaseInsensitive), dir.absoluteFilePath(sl[i]));
        }
        connect(ui->cmbStylesheet, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cmbStylesheet_currentIndexChanged(int)));

        ui->cmbStylesheet->setCurrentIndex( ui->cmbStylesheet->findText(oldStyleSheet));
        if (widVisible) ui->cmbStylesheet->setUpdatesEnabled(true);
    }

    //QTimer::singleShot(120000, this, SLOT(reloadStylesheets())); // rescan directory every 2 minutes
}
