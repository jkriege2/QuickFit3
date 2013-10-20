#include "qfrdrreplacedialog.h"
#include "programoptions.h"
#include "ui_qfrdrreplacedialog.h"
#include "qfpluginservices.h"

QFRDRReplaceDialog::QFRDRReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRReplaceDialog)
{
    ui->setupUi(this);
    ui->chkReplaceInName->setChecked(ProgramOptions::getConfigValue("QFRDRReplaceDialog/innames", true).toBool());
    ui->chkReplaceInFolders->setChecked(ProgramOptions::getConfigValue("QFRDRReplaceDialog/infolders", false).toBool());
    ui->chkCaseSensitive->setChecked(ProgramOptions::getConfigValue("QFRDRReplaceDialog/casesense", false).toBool());
    ui->edtFind->setText(ProgramOptions::getConfigValue("QFRDRReplaceDialog/find", "").toString());
    ui->edtReplace->setText(ProgramOptions::getConfigValue("QFRDRReplaceDialog/replace", "").toString());
}

QFRDRReplaceDialog::~QFRDRReplaceDialog()
{
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/innames", ui->chkReplaceInName->isChecked());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/infolders", ui->chkReplaceInFolders->isChecked());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/casesense", ui->chkCaseSensitive->isChecked());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/find", ui->edtFind->text());
    ProgramOptions::setConfigValue("QFRDRReplaceDialog/replace", ui->edtReplace->text());
    delete ui;
}

bool QFRDRReplaceDialog::getReplaceName() const
{
    return ui->chkReplaceInName->isChecked();
}

bool QFRDRReplaceDialog::getReplaceFolder() const
{
    return ui->chkReplaceInFolders->isChecked();
}

QString QFRDRReplaceDialog::getFindString() const
{
    return ui->edtFind->text();
}

QString QFRDRReplaceDialog::getReplaceString() const
{
    return ui->edtReplace->text();
}

bool QFRDRReplaceDialog::getCaseSensitivity() const
{
    return ui->chkCaseSensitive->isChecked();
}

void QFRDRReplaceDialog::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgrdrreplace.html");

}
