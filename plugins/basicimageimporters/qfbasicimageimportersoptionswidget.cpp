#include "qfbasicimageimportersoptionswidget.h"
#include "ui_qfbasicimageimportersoptionswidget.h"
#include "qfstyledbutton.h"
#include <QFile>
#include "programoptions.h"

QFBasicImageImportersOptionsWidget::QFBasicImageImportersOptionsWidget(QObject* pluginObject, QWidget *parent) :
    QFPluginOptionsWidget(pluginObject, parent),
    ui(new Ui::QFBasicImageImportersOptionsWidget)
{
    ui->setupUi(this);
    ui->edtLibTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::OpenFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
    ui->edtLibTIFFLogfile->addButton(new QFStyledButton(QFStyledButton::SelectFile, ui->edtLibTIFFLogfile, ui->edtLibTIFFLogfile));
}

QFBasicImageImportersOptionsWidget::~QFBasicImageImportersOptionsWidget()
{
    delete ui;
}

void QFBasicImageImportersOptionsWidget::readSettings(ProgramOptions *options)
{
    ui->chkLibTIFFLogging->setChecked(options->getConfigValue("importers_basicimages/libtiff/log", false).toBool());
    ui->edtLibTIFFLogfile->setText(options->getConfigValue("importers_basicimages/libtiff/logfile", options->getConfigFileDirectory()+"/importers_basicimages_libtiff.log").toString());
}

void QFBasicImageImportersOptionsWidget::writeSettings(ProgramOptions *options)
{
    options->setConfigValue("importers_basicimages/libtiff/log", ui->chkLibTIFFLogging->isChecked());
    options->setConfigValue("importers_basicimages/libtiff/logfile", ui->edtLibTIFFLogfile->text());
}

void QFBasicImageImportersOptionsWidget::on_btnClearLibTIFFLog_clicked()
{
    if (QFile::exists(ui->edtLibTIFFLogfile->text())) QFile::remove(ui->edtLibTIFFLogfile->text());
}
