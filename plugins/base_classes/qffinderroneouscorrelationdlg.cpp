#include "qffinderroneouscorrelationdlg.h"
#include "ui_qffinderroneouscorrelationdlg.h"
#include "programoptions.h"

QFFindErroneousCorrelationDlg::QFFindErroneousCorrelationDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFindErroneousCorrelationDlg)
{
    ui->setupUi(this);
    qRegisterMetaType<QList<int> >("QList<int>");
}

QFFindErroneousCorrelationDlg::~QFFindErroneousCorrelationDlg()
{
    ProgramOptions::setConfigValue("QFFindErroneousCorrelationDlg/chkClearMask", ui->chkClearMask->isChecked());
    ProgramOptions::setConfigValue("QFFindErroneousCorrelationDlg/spinHighQuantile", 100.0-ui->spinHighQuantile->value());
    ProgramOptions::setConfigValue("QFFindErroneousCorrelationDlg/spinLowQuantile", ui->spinLowQuantile->value());
    ProgramOptions::setConfigValue("QFFindErroneousCorrelationDlg/spinLagMax", ui->spinLagMax->value());
    ProgramOptions::setConfigValue("QFFindErroneousCorrelationDlg/spinLagMin", ui->spinLagMin->value());
    delete ui;
}

void QFFindErroneousCorrelationDlg::init(QFRDRRunSelectionsInterface *runselection, QFRDRFCSDataInterface *fcs)
{
    this->runselection=runselection;
    this->fcs=fcs;

    if (runselection) {
        initSelection.clear();
        for (int i=0; i<runselection->leaveoutGetRunCount(); i++) {
            if (runselection->leaveoutRun(i)) initSelection<<i;
        }
    }

    ui->spinLagMin->setRange(0, fcs->getCorrelationN());
    ui->spinLagMax->setRange(0, fcs->getCorrelationN());
    ui->spinLagMin->setValue(5);
    ui->spinLagMax->setValue(10);

    ui->chkClearMask->setChecked(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/chkClearMask", false).toBool());
    ui->spinHighQuantile->setValue(100-ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinHighQuantile", 95).toDouble());
    ui->spinLowQuantile->setValue(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinLowQuantile", 5).toDouble());
    ui->spinLagMax->setValue(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinLagMax", 10).toInt());
    ui->spinLagMin->setValue(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinLagMin", 5).toInt());

}

QList<int> QFFindErroneousCorrelationDlg::getInitSelection() const
{
    return initSelection;
}

QList<int> QFFindErroneousCorrelationDlg::getMaskedRuns() const
{
    return mask;
}

bool QFFindErroneousCorrelationDlg::isPreview() const
{
    return ui->chkPreview->isChecked();
}

bool QFFindErroneousCorrelationDlg::doClearOldMask() const
{
    return ui->chkClearMask->isChecked();
}

void QFFindErroneousCorrelationDlg::on_spinLagMin_valueChanged(int value)
{
    performCalc();
    ui->spinLagMax->setMinimum(value);
}

void QFFindErroneousCorrelationDlg::on_spinLagMax_valueChanged(int value)
{
    performCalc();
    ui->spinLagMin->setMaximum(value);
}

void QFFindErroneousCorrelationDlg::on_spinHighQuantile_valueChanged(double value)
{
    performCalc();
}

void QFFindErroneousCorrelationDlg::on_spinLowQuantile_valueChanged(double value)
{
    performCalc();
}

void QFFindErroneousCorrelationDlg::performCalc()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (fcs) {
        int l0=ui->spinLagMin->value();
        int l1=ui->spinLagMax->value();
        double* T=fcs->getCorrelationT();

        ui->labLagTimes->setText(tr("&tau;<sub>min</sub> ... &tau;<sub>max</sub> = %1s ... %2s").arg(doubleToUnitString(T[l0],5,true,'.',true)).arg(doubleToUnitString(T[l1],5,true,'.',true)));

        QVector<double> data;
        for (int i=0; i<fcs->getCorrelationRuns(); i++) {
            double value=0;
            double* ccf=fcs->getCorrelationRun(i);
            for (int l=l0; l<=l1; l++) {
                value+=ccf[l];
            }
            data<<value/double(l1-l1+1);
        }
        double ql=qfstatisticsQuantile(data, ui->spinLowQuantile->value()/100.0);
        double qh=qfstatisticsQuantile(data, (100.0-ui->spinHighQuantile->value())/100.0);
        mask.clear();
        for (int i=0; i<data.size(); i++) {
            if (ui->spinLowQuantile->value()>0.0 && data[i]<=ql)  mask<<i;
            if (ui->spinHighQuantile->value()-100.0<100.0 && data[i]>qh) mask<<i;
        }

        if (ui->chkClearMask->isChecked()) emit setSelection(mask, true);
        else {
            QList<int> m=initSelection;
            m.append(mask);
            emit setSelection(m, true);
        }

        ui->labStatistics->setText(tr("masking %1 out of %2 runs").arg(mask.size()).arg(data.size()));

    }
    QApplication::restoreOverrideCursor();
}
