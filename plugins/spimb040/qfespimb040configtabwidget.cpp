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

#include "qfespimb040configtabwidget.h"
#include "ui_qfespimb040configtabwidget.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qfstyledbutton.h"

QPointer<QFESPIMB040ConfigTabWidget> QFESPIMB040ConfigTabWidget::instance=NULL;

QFESPIMB040ConfigTabWidget::QFESPIMB040ConfigTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFESPIMB040ConfigTabWidget)
{
    ui->setupUi(this);

    QFStyledButton* btn;
    ui->edtOptSetup->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtOptSetup, ui->edtOptSetup));
    btn->setFilter(tr("Optics Setups Files (*.optSetup);;All files (*.*"));
    ui->edtOptSetupGlobalConfig->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtOptSetupGlobalConfig, ui->edtOptSetupGlobalConfig));
    btn->setFilter(tr("Optics Setup Configuration Files (*.optSetup.ini *.ini);;Configuration Files (*.ini);;All files (*.*"));
    ui->edtOptSetupUserConfig->addButton(btn=new QFStyledButton(QFStyledButton::SelectFile, ui->edtOptSetupUserConfig, ui->edtOptSetupUserConfig));
    btn->setFilter(tr("Optics Setup Configuration Files (*.optSetup.ini *.ini);;Configuration Files (*.ini);;All files (*.*"));

    reloadStylesheets(true);

    if (!instance) instance=this;

}

QFESPIMB040ConfigTabWidget::~QFESPIMB040ConfigTabWidget()
{
    delete ui;
}


void QFESPIMB040ConfigTabWidget::loadSettings(QSettings& settings, QString prefix) {
    ui->cmbStyle->setCurrentIndex(ui->cmbStyle->findText(settings.value(prefix+"style", ProgramOptions::getInstance()->getStyle()).toString(), Qt::MatchExactly ));
    ui->cmbStylesheet->setCurrentIndex(ui->cmbStylesheet->findText(settings.value(prefix+"stylesheet", ProgramOptions::getInstance()->getStylesheet()).toString(), Qt::MatchExactly ));
    emit styleChanged(getStyle(), getStylesheet());
    ui->edtOptSetupGlobalConfig->setText(settings.value(prefix+"optsetup_config_filename_readonly", QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/spim_at_b040.optSetup.ini").toString());
    ui->edtOptSetupUserConfig->setText(settings.value(prefix+"optsetup_config_filename", QFPluginServices::getInstance()->getConfigFileDirectory()+"plugins/spimb040/spim_at_b040.optSetup.ini").toString());
    ui->edtOptSetup->setText(settings.value(prefix+"optsetup_filename",QFPluginServices::getInstance()->getGlobalConfigFileDirectory()+"/spim_at_b040.optSetup").toString());
}

QString QFESPIMB040ConfigTabWidget::getStylesheet()
{
    if (instance) return instance->ui->cmbStylesheet->itemData(instance->ui->cmbStylesheet->currentIndex()).toString();
    else return ProgramOptions::getInstance()->getStylesheet();
}

QString QFESPIMB040ConfigTabWidget::getStyle()
{
    if (instance) return instance->ui->cmbStyle->currentText();
    else return ProgramOptions::getInstance()->getStyle();
}


void QFESPIMB040ConfigTabWidget::storeSettings(QSettings& settings, QString prefix) const {
    settings.setValue(prefix+"style", ui->cmbStyle->currentText());
    settings.setValue(prefix+"stylesheet", ui->cmbStylesheet->currentText());
    settings.setValue(prefix+"optsetup_config_filename_readonly", ui->edtOptSetupGlobalConfig->text());
    settings.setValue(prefix+"optsetup_config_filename", ui->edtOptSetupUserConfig->text());
    settings.setValue(prefix+"optsetup_filename", ui->edtOptSetup->text());
}


void QFESPIMB040ConfigTabWidget::on_cmbStylesheet_currentIndexChanged( int index ) {
    QString fn=ui->cmbStylesheet->itemData(index).toString();
    QFile f(fn);
    f.open(QFile::ReadOnly);
    QTextStream s(&f);
    QString qss=s.readAll();
    this->setStyleSheet(qss);

    emit styleChanged(getStyle(), getStylesheet());
}


void QFESPIMB040ConfigTabWidget::on_cmbStyle_currentIndexChanged( const QString & text ) {
    parentWidget()->setStyle(QStyleFactory::create(text));
    parentWidget()->setPalette(parentWidget()->style()->standardPalette());
    emit styleChanged(getStyle(), getStylesheet());
}

void QFESPIMB040ConfigTabWidget::on_cmbStyle_highlighted( const QString & text ) {
    on_cmbStyle_currentIndexChanged(text);
}

void QFESPIMB040ConfigTabWidget::reloadStylesheets(bool forSure)
{
    QString oldStyle=ui->cmbStyle->currentText();
    QString oldStyleSheet=ui->cmbStylesheet->currentText();
    if (forSure || !ui->cmbStyle->hasFocus()) {
        ui->cmbStyle->setUpdatesEnabled(false);
        ui->cmbStyle->clear();
        ui->cmbStyle->addItems(QStyleFactory::keys());
        ui->cmbStyle->setCurrentIndex(ui->cmbStyle->findText(oldStyle, Qt::MatchContains));
        ui->cmbStyle->setUpdatesEnabled(true);
    }


    // find all available stylesheets
    if (forSure || !ui->cmbStylesheet->hasFocus()) {
        ui->cmbStylesheet->setUpdatesEnabled(false);
        QDir dir(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/spimb040/stylesheets/");
        QStringList filters, sl;
        dir.cd("stylesheets");
        filters.clear();
        filters << "*.qss";
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

        ui->cmbStylesheet->setCurrentIndex( ui->cmbStylesheet->findText(oldStyleSheet));
        ui->cmbStylesheet->setUpdatesEnabled(true);
    }

    //QTimer::singleShot(120000, this, SLOT(reloadStylesheets())); // rescan directory every 2 minutes
}

void QFESPIMB040ConfigTabWidget::on_btnAutosetConfigs_clicked()
{
    QString optset=ui->edtOptSetup->text();
    QFileInfo fi(optset);
    ui->edtOptSetupGlobalConfig->setText(optset+".ini");
    ui->edtOptSetupUserConfig->setText(QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/spimb040/"+fi.fileName()+".ini");

    QDir().mkpath(QFileInfo(ui->edtOptSetupGlobalConfig->text()).absolutePath());
    QDir().mkpath(QFileInfo(ui->edtOptSetupUserConfig->text()).absolutePath());
}

