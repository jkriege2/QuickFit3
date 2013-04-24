#include "qfrdrimagingfcsseriesdialog.h"
#include "ui_qfrdrimagingfcsseriesdialog.h"

QFRDRImagingFCSSeriesDialog::QFRDRImagingFCSSeriesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSSeriesDialog)
{
    ui->setupUi(this);
    ui->edtStart->setCheckBounds(false, false);
    ui->edtStop->setCheckBounds(false, false);
    ui->edtDelta->setCheckBounds(false, false);
    if (ProgramOptions::getInstance()) {
        ui->cmbPrameter->setCurrentIndex(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSSeriesDialog/parameter", 0).toInt());
        ui->edtStart->setValue(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSSeriesDialog/start", 1).toDouble());
        ui->edtStop->setValue(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSSeriesDialog/stop", 10).toDouble());
        ui->edtDelta->setValue(ProgramOptions::getInstance()->getQSettings()->value("QFRDRImagingFCSSeriesDialog/delta", 1).toDouble());

    }
}

QFRDRImagingFCSSeriesDialog::~QFRDRImagingFCSSeriesDialog()
{
    delete ui;
}

int QFRDRImagingFCSSeriesDialog::getParameter() const
{
    return ui->cmbPrameter->currentIndex();
}

QList<double> QFRDRImagingFCSSeriesDialog::getValues() const {
    QList<double> vals;
    if (ui->tabWidget->currentIndex()==0) {
        for (double i=ui->edtStart->value(); i<=ui->edtStop->value(); i=i+ui->edtDelta->value()) {
            vals.append(i);
        }
    }
    return vals;
}

void QFRDRImagingFCSSeriesDialog::setParameter(const QString &parameter)
{
    QString p=parameter.toLower().trimmed();
    if (p=="binning") setParameter(0);
    else setParameter(0);
}

void QFRDRImagingFCSSeriesDialog::closeEvent(QCloseEvent *event) {
    if (ProgramOptions::getInstance()) {
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSSeriesDialog/parameter", ui->cmbPrameter->currentIndex());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSSeriesDialog/start", ui->edtStart->value());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSSeriesDialog/stop", ui->edtStop->value());
        ProgramOptions::getInstance()->getQSettings()->setValue("QFRDRImagingFCSSeriesDialog/delta", ui->edtDelta->value());

    }
}

void QFRDRImagingFCSSeriesDialog::setParameter(int idx)
{
    ui->cmbPrameter->setCurrentIndex(idx);
}

void QFRDRImagingFCSSeriesDialog::setRange(double start, double end, double inc)
{
    ui->edtStart->setValue(start);
    ui->edtStop->setValue(end);
    ui->edtDelta->setValue(inc);
}
