#include "qfrdrimagingfcspostprocessbinningdialog.h"
#include "ui_qfrdrimagingfcspostprocessbinningdialog.h"

QFRDRImagingFCSPostProcessBinningDialog::QFRDRImagingFCSPostProcessBinningDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSPostProcessBinningDialog)
{
    ui->setupUi(this);
}

QFRDRImagingFCSPostProcessBinningDialog::~QFRDRImagingFCSPostProcessBinningDialog()
{
    delete ui;
}

void QFRDRImagingFCSPostProcessBinningDialog::init(int bin, bool interleaved)
{
    ui->chkOther->setChecked(false);
    ui->chkInterleaved->setChecked(interleaved);
    ui->spinBin->setValue(bin);

}

int QFRDRImagingFCSPostProcessBinningDialog::getBin() const
{
    return ui->spinBin->value();
}

bool QFRDRImagingFCSPostProcessBinningDialog::getInterleaved() const
{
    return ui->chkInterleaved->isChecked();
}

bool QFRDRImagingFCSPostProcessBinningDialog::getOtherRDR() const
{
    return ui->chkOther->isChecked();
}


void QFRDRImagingFCSPostProcessBinningDialog::on_chkInterleaved_toggled(bool checked)
{
    if (checked) {
        ui->labExplanation->setText(tr("<i>The autocorrelation curves from N*N pixel patch are averaged, but the patches overlap. So the image size is retained.</i>"));
    } else {
        ui->labExplanation->setText(tr("<i>The autocorrelation curves from each N*N pixel patch are averaged. The final image size of the record is reduced to width/N * height/N</i>"));
    }
}
