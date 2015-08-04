/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "optionsdialog.h"
#include "qfpluginservices.h"
#include <iostream>
#include "qfstyledbutton.h"
#include "qftools.h"

OptionsDialog::OptionsDialog(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);
    labMath->setText("g_\\text{gg}^\\text{A}(\\tau)=\\frac{1}{N}\\cdot\\left(1+\\frac{\\tau}{\\tau_D}\\right)^{-1}\\cdot\\left(1+\\frac{\\tau}{\\gamma^2\\tau_D}\\right)^{-1/2}");
    QFStyledButton* btn=new QFStyledButton(QFStyledButton::SelectDirectory, edtUserFitFunctions, edtUserFitFunctions);
    edtUserFitFunctions->addButton(btn);
    btn=new QFStyledButton(QFStyledButton::SelectDirectory, edtUserSettings, edtUserSettings);
    edtUserSettings->addButton(btn);
    btn=new QFStyledButton(QFStyledButton::SelectDirectory, edtGlobalSettings, edtGlobalSettings);
    edtGlobalSettings->addButton(btn);
    btn=new QFStyledButton(QFStyledButton::SelectDirectory, edtTempFolder, edtTempFolder);
    edtTempFolder->addButton(btn);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);

    label_9->setVisible(false);
    label_10->setVisible(false);
    label_11->setVisible(false);

    chkChildWindowsStayOnTop->setVisible(false);
    chkHelpWindowsStayOnTop->setVisible(false);
    chkProjectWindowsStayOnTop->setVisible(false);

    connect(cmbStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(styleChanged(QString)));
    connect(cmbStylesheet, SIGNAL(currentIndexChanged(QString)), this, SLOT(stylesheetChanged(QString)));
}

OptionsDialog::~OptionsDialog()
{
    //dtor
}

void OptionsDialog::stylesheetChanged( const QString & text ) {
    //qDebug()<<"setTyleSheet("<<text<<")";
    QString fn=QString(m_options->getAssetsDirectory()+"/stylesheets/%1.qss").arg(text);
    QFile f(fn);
    QString qss="";
    if (f.open(QFile::ReadOnly)) {
        QTextStream s(&f);
        qss=s.readAll();
    }

    //std::cout<<qss.toStdString()<<std::endl;
    this->setStyleSheet(qss);
}

//void OptionsDialog::on_cmbStylesheet_highlighted( const QString & /*text*/ ) {
//    //on_cmbStylesheet_currentIndexChanged(text);
//}

void OptionsDialog::styleChanged( const QString & text ) {
//        QStyle* s=QStyleFactory::create(text);
//        if (s!=NULL) this->setStyle(s);

    QApplication::setStyle(QStyleFactory::create(text));

    QApplication::setPalette(QApplication::style()->standardPalette());
}

//void OptionsDialog::on_cmbStyle_highlighted( const QString & text ) {
//    on_cmbStyle_currentIndexChanged(text);
//}

void OptionsDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(ProgramOptions::getInstance()->getAssetsDirectory()+QString("/help/qf3_settings.html"));
}

void OptionsDialog::on_listWidget_currentRowChanged(int currentRow)
{
    stackedWidget->setCurrentIndex(currentRow);
}

void OptionsDialog::on_spinMath_valueChanged(int value)
{
    labMath->getMathText()->set_fontSize(value);
    labMath->getMathText()->useSTIX();
    labMath->setMath("g_\\text{gg}^\\text{A}(\\tau)=\\frac{1}{N}\\cdot\\frac{1}{\\sqrt{1+\\frac{\\tau}{\\tau_D}}}");

}

void OptionsDialog::updateFontExample()
{
    labFontExample->setText("Test Example");
    labFontExample->setFont(QFont(cmbHelpFont->currentFont().family(), spinHelpFontsize->value()));
}


void OptionsDialog::open(ProgramOptions* options) {
    disconnect(cmbStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(styleChanged(QString)));
    disconnect(cmbStylesheet, SIGNAL(currentIndexChanged(QString)), this, SLOT(stylesheetChanged(QString)));

    spnMaxThreads->setRange(1,100);
    spnMaxThreads->setValue(options->getMaxThreads());
    m_options=options;

    // find all available translations ... program is already in english, so add "en" by default (there is not en.qm file!)
    QDir dir(options->getAssetsDirectory());
    dir.cd("translations");
    QStringList filters;
    filters << "*.qm";

    cmbLanguage->clear();
    cmbLanguage->addItem("en");
    QStringList sl=qfDirListFilesRecursive(dir, filters);//dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        QString s=sl[i];
        int idx=s.indexOf(".");
        if (cmbLanguage->findText(s.left(idx))<0) {
            cmbLanguage->addItem(s.left(idx));
        }
    }
    cmbLanguage->setCurrentIndex( cmbLanguage->findText(options->getLanguageID()));

    cmbStyle->addItems(QStyleFactory::keys());
    cmbStyle->setCurrentIndex(cmbStyle->findText(options->getStyle(), Qt::MatchContains));
    spinAutosave->setValue(options->getAutosave());
    //chkAskSaveNewProject->setChecked(options->getUserSaveAfterFirstEdit());
    chkChildWindowsStayOnTop->setChecked(options->getChildWindowsStayOnTop());
    chkUserSaveAfterFirstEdit->setChecked(options->getUserSaveAfterFirstEdit());
    chkProjectWindowsStayOnTop->setChecked(options->getProjectWindowsStayOnTop());
    chkHelpWindowsStayOnTop->setChecked(options->getHelpWindowsStayOnTop());
    chkDebugMessages->setChecked(options->debugLogVisible());
    spinProxyPort->setValue(options->getProxyPort());
    edtProxyHost->setText(options->getProxyHost());
    cmbProxyType->setCurrentIndex(qBound(0,options->getProxyType(),2));
    chkUpdates->setChecked(options->getConfigValue("quickfit/checkupdates", true).toBool());
    chkStartupScreen->setChecked(options->getConfigValue("quickfit/welcomescreen", true).toBool());
    cmbWindowHeader->setCurrentIndex(options->getConfigValue("quickfit/windowheadermode", 1).toInt());
    QDir dhome(options->getHomeQFDirectory());
    if (!QDir(dhome.absolutePath()+"/userfitfunctions").exists()) dhome.mkdir("userfitfunctions");
    edtUserFitFunctions->setText(options->getConfigValue("quickfit/user_fitfunctions", options->getHomeQFDirectory()+"/userfitfunctions/").toString());
    edtUserSettings->setText(options->getHomeQFDirectory());
    edtGlobalSettings->setText(options->getGlobalConfigFileDirectory());
    edtTempFolder->setText(options->getConfigValue("quickfit/temp_folder", QFileInfo(qfGetTempFilename()).absolutePath()).toString());
    chkDefaultTempFolder->setChecked(options->getConfigValue("quickfit/temp_folder_default", true).toBool());
    spinHelpFontsize->setValue(options->getConfigValue("quickfit/help_pointsize", 11).toInt());
    spinMath->setValue(options->getConfigValue("quickfit/math_pointsize", 14).toInt());
    cmbHelpFont->setCurrentFont(QFont(options->getConfigValue("quickfit/help_font", font().family()).toString()));

    cmbFileDialog->setCurrentIndex(0);
    if (!ProgramOptions::getConfigValue("quickfit/native_file_dialog", true).toBool()) {
        cmbFileDialog->setCurrentIndex(1);
    }
    on_spinMath_valueChanged(spinMath->value());
    updateFontExample();


    // find all available stylesheets
    dir.cd("../stylesheets");
    filters.clear();
    filters << "*.qss";

    cmbStylesheet->clear();
    sl=qfDirListFilesRecursive(dir, filters);//dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        cmbStylesheet->addItem(sl[i].remove(".qss", Qt::CaseInsensitive));
    }
    cmbStylesheet->setCurrentIndex( cmbStylesheet->findText(options->getStylesheet()));

    connect(cmbStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(styleChanged(QString)));
    connect(cmbStylesheet, SIGNAL(currentIndexChanged(QString)), this, SLOT(stylesheetChanged(QString)));

    stylesheetChanged( cmbStylesheet->currentText() );
    styleChanged(cmbStyle->currentText());

    for (int i=0; i<m_plugins.size(); i++) {
        m_plugins[i]->readSettings(options);
    }

    if (exec() == QDialog::Accepted ){
        options->setMaxThreads(spnMaxThreads->value());
        options->setLanguageID(cmbLanguage->currentText());
        options->setStylesheet(cmbStylesheet->currentText());
        options->setStyle(cmbStyle->currentText());
        options->setAutosave(spinAutosave->value());
        options->setChildWindowsStayOnTop(chkChildWindowsStayOnTop->isChecked());
        options->setUserSaveAfterFirstEdit(chkUserSaveAfterFirstEdit->isChecked());
        options->setHelpWindowsStayOnTop(chkHelpWindowsStayOnTop->isChecked());
        options->setProjectWindowsStayOnTop(chkProjectWindowsStayOnTop->isChecked());
        options->setDebugLogVisible(chkDebugMessages->isChecked());
        options->setProxyHost(edtProxyHost->text());
        options->setProxyPort(spinProxyPort->value());
        options->setProxyType(cmbProxyType->currentIndex());
        options->setConfigValue("quickfit/checkupdates", chkUpdates->isChecked());
        options->setConfigValue("quickfit/welcomescreen", chkStartupScreen->isChecked());
        options->setConfigValue("quickfit/user_fitfunctions", edtUserFitFunctions->text());
        options->setHomeQFDirectory(edtUserSettings->text());
        options->setGlobalConfigFileDirectory(edtGlobalSettings->text());
        options->setConfigValue("quickfit/math_pointsize", spinMath->value());
        options->setConfigValue("quickfit/help_pointsize", spinHelpFontsize->value());
        options->setConfigValue("quickfit/help_font", cmbHelpFont->currentFont().family());
        options->setConfigValue("quickfit/windowheadermode", cmbWindowHeader->currentIndex());

        options->setConfigValue("quickfit/temp_folder", edtTempFolder->text());
        options->setConfigValue("quickfit/temp_folder_default", chkDefaultTempFolder->isChecked());

        //options->setUserSaveAfterFirstEdit(chkAskSaveNewProject->isChecked());

        {
            QDir dir(edtUserFitFunctions->text());
            if (!dir.exists()) dir.mkpath(dir.absolutePath());
        }
        {
            QDir dir(edtUserSettings->text());
            if (!dir.exists()) dir.mkpath(dir.absolutePath());
        }
        {
            QDir dir(edtGlobalSettings->text());
            if (!dir.exists()) dir.mkpath(dir.absolutePath());
        }
        options->setConfigValue("quickfit/native_file_dialog", (cmbFileDialog->currentIndex()==0));

        for (int i=0; i<m_plugins.size(); i++) {
            m_plugins[i]->writeSettings(options);
        }
        options->writeSettings();
    }
}

void OptionsDialog::setPlugins(const QList<QFPluginOptionsDialogInterface *> &plugins)
{
    m_plugins.clear();
    for (int i=0; i<plugins.size(); i++) {
        QFPluginOptionsWidget* w=plugins[i]->createOptionsWidget(this);
        //tabWidget->addTab(w, QIcon(plugins[i]->pluginOptionsIcon()), plugins[i]->pluginOptionsName());
        stackedWidget->addWidget(w);
        QListWidgetItem* lw=new QListWidgetItem(QIcon(plugins[i]->pluginOptionsIcon()), plugins[i]->pluginOptionsName(), listWidget);
        listWidget->addItem(lw);
        m_plugins.append(w);
    }
}
