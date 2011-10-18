#include "qfespimb00histogrammaskdialog.h"
#include "ui_qfespimb00histogrammaskdialog.h"

QFESPIMB00HistogramMaskDialog::QFESPIMB00HistogramMaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESPIMB00HistogramMaskDialog)
{
    ui->setupUi(this);
}

QFESPIMB00HistogramMaskDialog::~QFESPIMB00HistogramMaskDialog()
{
    delete ui;
}

int QFESPIMB00HistogramMaskDialog::mode() const {
    return ui->cmbMode->currentIndex();
}
void QFESPIMB00HistogramMaskDialog::setMode(int m) {
    ui->cmbMode->setCurrentIndex(m);
}

int QFESPIMB00HistogramMaskDialog::pixels() const {
    return ui->spinPixels->value();
}
void QFESPIMB00HistogramMaskDialog::setPixels(int pixels, int maxPixels) {
    ui->spinPixels->setMaximum(maxPixels);
    ui->spinPixels->setMinimum(0);
    ui->spinPixels->setValue(pixels);
}
