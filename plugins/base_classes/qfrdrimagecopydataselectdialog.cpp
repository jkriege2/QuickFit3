#include "qfrdrimagecopydataselectdialog.h"
#include "ui_qfrdrimagecopydataselectdialog.h"
#include "programoptions.h"

QFRDRImageCopyDataSelectDialog::QFRDRImageCopyDataSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImageCopyDataSelectDialog)
{
    ui->setupUi(this);
    ui->chkParameterImage->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/param", true).toBool());
    ui->chkGOF->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/gof", true).toBool());
    ui->chkMask->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/mask", true).toBool());
    ui->chkOverview->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/overview", true).toBool());
    ui->chkPixelCoordinate->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/pixelcoordinate", true).toBool());
    ui->chkPixelNumber->setChecked(ProgramOptions::getConfigValue("QFRDRImageCopyDataSelectDialog/pixelnum", true).toBool());
}

QFRDRImageCopyDataSelectDialog::~QFRDRImageCopyDataSelectDialog()
{
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/param", ui->chkParameterImage->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/gof", ui->chkGOF->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/mask", ui->chkMask->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/overview", ui->chkOverview->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/pixelcoordinate", ui->chkPixelCoordinate->isChecked());
    ProgramOptions::setConfigValue("QFRDRImageCopyDataSelectDialog/pixelnum", ui->chkPixelNumber->isChecked());

    delete ui;
}

bool QFRDRImageCopyDataSelectDialog::copyParam() const
{
    return ui->chkParameterImage->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyOvf() const
{
    return ui->chkOverview->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyParam2() const
{
    return ui->chkGOF->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyMask() const
{
    return ui->chkMask->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyID() const
{
    return ui->chkPixelNumber->isChecked();
}

bool QFRDRImageCopyDataSelectDialog::copyCoordinates() const
{
    return ui->chkPixelCoordinate->isChecked();
}
