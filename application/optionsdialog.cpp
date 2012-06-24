#include "optionsdialog.h"
#include "qfpluginservices.h"
#include <iostream>

OptionsDialog::OptionsDialog(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);
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


void OptionsDialog::open(ProgramOptions* options) {
    spnMaxThreads->setValue(options->getMaxThreads());
    m_options=options;

    // find all available translations ... program is already in english, so add "en" by default (there is not en.qm file!)
    QDir dir(options->getAssetsDirectory());
    dir.cd("translations");
    QStringList filters;
    filters << "*.qm";
    cmbLanguage->clear();
    cmbLanguage->addItem("en");
    QStringList sl=dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        QString s=sl[i];
        int idx=s.indexOf(".");
        if (cmbLanguage->findText(s.left(idx))<0) {
            cmbLanguage->addItem(s.left(idx), Qt::CaseInsensitive);
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


    // find all available stylesheets
    dir.cd("../stylesheets");
    filters.clear();
    filters << "*.qss";
    cmbStylesheet->clear();
    sl=dir.entryList(filters, QDir::Files);
    for (int i=0; i<sl.size(); i++) {
        cmbStylesheet->addItem(sl[i].remove(".qss"), Qt::CaseInsensitive);
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
        for (int i=0; i<m_plugins.size(); i++) {
            m_plugins[i]->writeSettings(options);
        }
    }
}

void OptionsDialog::setPlugins(const QList<QFPluginOptionsDialogInterface *> &plugins)
{
    m_plugins.clear();
    for (int i=0; i<plugins.size(); i++) {
        QFPluginOptionsWidget* w=plugins[i]->createOptionsWidget(this);
        tabWidget->addTab(w, QIcon(plugins[i]->pluginOptionsIcon()), plugins[i]->pluginOptionsName());
        m_plugins.append(w);
    }
}
