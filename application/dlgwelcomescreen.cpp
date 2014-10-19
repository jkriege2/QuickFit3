#include "dlgwelcomescreen.h"
#include "ui_dlgwelcomescreen.h"
#include "programoptions.h"
#include "qfversion.h"

DlgWelcomeScreen::DlgWelcomeScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgWelcomeScreen)
{
    ui->setupUi(this);
    ui->chkShow->setChecked(options->getConfigValue("quickfit/welcomescreen", true).toBool());
    ui->labQF->setText(tr("QuickFit %1 <small>SVN: %2 - compiledate: %3</small>").arg(qfInfoVersion()).arg(qfInfoSVNVersion()).arg(qfInfoCompileDate()));
}

DlgWelcomeScreen::~DlgWelcomeScreen()
{
    ProgramOptions::setConfigValue("quickfit/welcomescreen", ui->chkShow->isChecked());
    delete ui;
}

void DlgWelcomeScreen::on_chkShow_toggled(bool enabled)
{
    ProgramOptions::setConfigValue("quickfit/welcomescreen", ui->chkShow->isChecked());
}

void DlgWelcomeScreen::on_btnClose_clicked()
{
    close();
}
