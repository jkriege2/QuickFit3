#include "qfrdrimagingfcscopydataselectdialog.h"
#include "ui_qfrdrimagingfcscopydataselectdialog.h"
#include "programoptions.h"

QFRDRImagingFCSCopyDataSelectDialog::QFRDRImagingFCSCopyDataSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSCopyDataSelectDialog)
{
    ui->setupUi(this);
    ui->chkParameterImage->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSCopyDataSelectDialog/param", true).toBool());
    ui->chkGOF->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSCopyDataSelectDialog/gof", true).toBool());
    ui->chkMask->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSCopyDataSelectDialog/mask", true).toBool());
    ui->chkOverview->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSCopyDataSelectDialog/overview", true).toBool());
    ui->chkPixelCoordinate->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSCopyDataSelectDialog/pixelcoordinate", true).toBool());
    ui->chkPixelNumber->setChecked(ProgramOptions::getConfigValue("QFRDRImagingFCSCopyDataSelectDialog/pixelnum", true).toBool());
}

QFRDRImagingFCSCopyDataSelectDialog::~QFRDRImagingFCSCopyDataSelectDialog()
{
    ProgramOptions::setConfigValue("QFRDRImagingFCSCopyDataSelectDialog/param", ui->chkParameterImage->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSCopyDataSelectDialog/gof", ui->chkGOF->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSCopyDataSelectDialog/mask", ui->chkMask->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSCopyDataSelectDialog/overview", ui->chkOverview->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSCopyDataSelectDialog/pixelcoordinate", ui->chkPixelCoordinate->isChecked());
    ProgramOptions::setConfigValue("QFRDRImagingFCSCopyDataSelectDialog/pixelnum", ui->chkPixelNumber->isChecked());

    delete ui;
}

bool QFRDRImagingFCSCopyDataSelectDialog::copyParam() const
{
    return ui->chkParameterImage->isChecked();
}

bool QFRDRImagingFCSCopyDataSelectDialog::copyOvf() const
{
    return ui->chkOverview->isChecked();
}

bool QFRDRImagingFCSCopyDataSelectDialog::copyParam2() const
{
    return ui->chkGOF->isChecked();
}

bool QFRDRImagingFCSCopyDataSelectDialog::copyMask() const
{
    return ui->chkMask->isChecked();
}

bool QFRDRImagingFCSCopyDataSelectDialog::copyID() const
{
    return ui->chkPixelNumber->isChecked();
}

bool QFRDRImagingFCSCopyDataSelectDialog::copyCoordinates() const
{
    return ui->chkPixelCoordinate->isChecked();
}
