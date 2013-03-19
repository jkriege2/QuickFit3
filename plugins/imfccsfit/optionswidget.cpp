#include "optionswidget.h"
#include "ui_optionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"

OptionsWidget::OptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::OptionsWidget)
{
    ui->setupUi(this);
}

OptionsWidget::~OptionsWidget()
{
    delete ui;
}

void OptionsWidget::readSettings(ProgramOptions *options)
{
    ui->chkLog->setChecked(options->getConfigValue("imfccs_fit/log", false).toBool());
    ui->chkOverrule->setChecked(options->getConfigValue("imfccs_fit/overrule_threads", false).toBool());
    ui->spinThreads->setValue(options->getConfigValue("imfccs_fit/threads", 1).toInt());
}

void OptionsWidget::writeSettings(ProgramOptions *options)
{

    options->setConfigValue("imfccs_fit/log", ui->chkLog->isChecked());
    options->setConfigValue("imfccs_fit/overrule_threads", ui->chkOverrule->isChecked());
    options->setConfigValue("imfccs_fit/threads", ui->spinThreads->value());
}
