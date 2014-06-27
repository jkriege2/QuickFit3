#include "qfrdrimagingfcscopycorrasrdrdialog.h"
#include "ui_qfrdrimagingfcscopycorrasrdrdialog.h"
#include "programoptions.h"

QFRDRImagingFCSCopyCorrAsRDRDialog::QFRDRImagingFCSCopyCorrAsRDRDialog(bool isFCCS, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSCopyCorrAsRDRDialog)
{
    ui->setupUi(this);
    ui->chkCopyACF->setEnabled(isFCCS);
    ui->chkCopyACF->setChecked(true);
    if (ProgramOptions::getInstance()) {
        ui->edtSelName->setText(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/selName", ui->edtSelName->text()).toString());
        ui->chkCopyACF->setChecked(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/copyACF", ui->chkCopyACF->isChecked()).toBool());
        ui->chkCopyAllRuns->setChecked(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCopyAllRuns", ui->chkCopyAllRuns->isChecked()).toBool());
        ui->chkCreateNamedSel->setChecked(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCreateNamedSel", ui->chkCreateNamedSel->isChecked()).toBool());

    }
}

QFRDRImagingFCSCopyCorrAsRDRDialog::~QFRDRImagingFCSCopyCorrAsRDRDialog()
{
    if (ProgramOptions::getInstance()) {
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/selName", ui->edtSelName->text());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/copyACF", ui->chkCopyACF->isChecked());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCopyAllRuns", ui->chkCopyAllRuns->isChecked());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSCopyCorrAsRDRDialog/chkCreateNamedSel", ui->chkCreateNamedSel->isChecked());
    }
    delete ui;
}

bool QFRDRImagingFCSCopyCorrAsRDRDialog::getCopyACF() const
{
    return ui->chkCopyACF->isChecked();
}

bool QFRDRImagingFCSCopyCorrAsRDRDialog::getCopyAllRuns() const
{
    return ui->chkCopyAllRuns->isChecked();
}

bool QFRDRImagingFCSCopyCorrAsRDRDialog::getCreateSelection() const
{
    return ui->chkCreateNamedSel->isChecked();
}

QString QFRDRImagingFCSCopyCorrAsRDRDialog::getSelectionName() const
{
    return ui->edtSelName->text();
}
