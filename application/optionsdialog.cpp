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
}

OptionsDialog::~OptionsDialog()
{
    //dtor
}

void OptionsDialog::on_cmbStylesheet_currentIndexChanged( const QString & text ) {
    QString fn=QString(m_options->getAssetsDirectory()+"/stylesheets/%1.qss").arg(text);
    QFile f(fn);
    f.open(QFile::ReadOnly);
    QTextStream s(&f);
    QString qss=s.readAll();
    //std::cout<<qss.toStdString()<<std::endl;
    this->setStyleSheet(qss);
}

void OptionsDialog::on_cmbStylesheet_highlighted( const QString & text ) {
    //on_cmbStylesheet_currentIndexChanged(text);
}

void OptionsDialog::on_cmbStyle_currentIndexChanged( const QString & text ) {
//        QStyle* s=QStyleFactory::create(text);
//        if (s!=NULL) this->setStyle(s);

    QApplication::setStyle(QStyleFactory::create(text));

    QApplication::setPalette(QApplication::style()->standardPalette());
}

void OptionsDialog::on_cmbStyle_highlighted( const QString & text ) {
    on_cmbStyle_currentIndexChanged(text);
}

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


void OptionsDialog::open(ProgramOptions* options) {
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
    chkChildWindowsStayOnTop->setChecked(options->getChildWindowsStayOnTop());
    chkUserSaveAfterFirstEdit->setChecked(options->getUserSaveAfterFirstEdit());
    chkProjectWindowsStayOnTop->setChecked(options->getProjectWindowsStayOnTop());
    chkHelpWindowsStayOnTop->setChecked(options->getHelpWindowsStayOnTop());
    chkDebugMessages->setChecked(options->debugLogVisible());
    spinProxyPort->setValue(options->getProxyPort());
    edtProxyHost->setText(options->getProxyHost());
    cmbProxyType->setCurrentIndex(qBound(0,options->getProxyType(),2));
    chkUpdates->setChecked(options->getConfigValue("quickfit/checkupdates", true).toBool());
    cmbWindowHeader->setCurrentIndex(options->getConfigValue("quickfit/windowheadermode", 1).toInt());
    QDir dhome(options->getHomeQFDirectory());
    if (!QDir(dhome.absolutePath()+"/userfitfunctions").exists()) dhome.mkdir("userfitfunctions");
    edtUserFitFunctions->setText(options->getConfigValue("quickfit/user_fitfunctions", options->getHomeQFDirectory()+"/userfitfunctions/").toString());
    edtUserSettings->setText(options->getHomeQFDirectory());
    spinMath->setValue(options->getConfigValue("quickfit/math_pointsize", 14).toInt());
    on_spinMath_valueChanged(spinMath->value());


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
        options->setConfigValue("quickfit/user_fitfunctions", edtUserFitFunctions->text());
        options->setHomeQFDirectory(edtUserSettings->text());
        options->setConfigValue("quickfit/math_pointsize", spinMath->value());
        options->setConfigValue("quickfit/windowheadermode", cmbWindowHeader->currentIndex());
        {
            QDir dir(edtUserFitFunctions->text());
            if (!dir.exists()) dir.mkpath(dir.absolutePath());
        }
        {
            QDir dir(edtUserSettings->text());
            if (!dir.exists()) dir.mkpath(dir.absolutePath());
        }

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
