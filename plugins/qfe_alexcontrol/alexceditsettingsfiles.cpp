#include "alexceditsettingsfiles.h"
#include "ui_alexceditsettingsfiles.h"
#include "qftools.h"


ALEXCEditSettingsFiles::ALEXCEditSettingsFiles(const QString &channels, const QString &timing, const QString &ports, QWidget *parent):
    QFDialog(parent),
    ui(new Ui::ALEXCEditSettingsFiles)
{
    ui->setupUi(this);
    ui->edtChannels->setPlainText(readFile(channels));
    ui->edtTiming->setPlainText(readFile(timing));
    ui->edtPorts->setPlainText(readFile(ports));
}

ALEXCEditSettingsFiles::~ALEXCEditSettingsFiles()
{
    delete ui;
}

QString ALEXCEditSettingsFiles::getChannels() const
{
    return ui->edtChannels->toPlainText();
}

QString ALEXCEditSettingsFiles::getTiming() const
{
    return ui->edtTiming->toPlainText();
}

QString ALEXCEditSettingsFiles::getPorts() const
{
    return ui->edtPorts->toPlainText();
}
