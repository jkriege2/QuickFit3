#include "optionswidget.h"
#include "ui_optionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"
#include "qfstyledbutton.h"
#include "qfpluginservices.h"

OptionsWidget::OptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::OptionsWidget)
{
    ui->setupUi(this);
    QFStyledButton* btn=new QFStyledButton(QFStyledButton::SelectDirectory, ui->edtUserDatabase, ui->edtUserDatabase);
    ui->edtUserDatabase->addButton(btn);
}

OptionsWidget::~OptionsWidget()
{
    delete ui;
}

void OptionsWidget::readSettings(ProgramOptions *options)
{
    ui->edtUserDatabase->setText(options->getConfigValue("qfe_spectraviewer/user_database", QFPluginServices::getInstance()->getConfigFileDirectory()+"/plugins/qfe_spectraviewer/").toString());
}

void OptionsWidget::writeSettings(ProgramOptions *options)
{

    options->setConfigValue("qfe_spectraviewer/user_database", ui->edtUserDatabase->text());
}
