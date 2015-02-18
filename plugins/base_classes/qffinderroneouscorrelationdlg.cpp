/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



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

    /*ui->spinLagMin->setRange(0, fcs->getCorrelationN());
    ui->spinLagMax->setRange(0, fcs->getCorrelationN());
    ui->spinLagMin->setValue(5);
    ui->spinLagMax->setValue(10);*/

    ui->chkClearMask->setChecked(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/chkClearMask", false).toBool());
    ui->spinHighQuantile->setValue(100-ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinHighQuantile", 95).toDouble());
    ui->spinLowQuantile->setValue(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinLowQuantile", 5).toDouble());
    ui->spinLagMax->setValue(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinLagMax", 15).toDouble());
    ui->spinLagMin->setValue(ProgramOptions::getConfigValue("QFFindErroneousCorrelationDlg/spinLagMin", 5).toDouble());

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

void QFFindErroneousCorrelationDlg::on_spinLagMin_editingFinished()
{
    performCalc();
    //ui->spinLagMax->setMinimum(value);
}

void QFFindErroneousCorrelationDlg::on_spinLagMax_editingFinished()
{
    performCalc();
    //ui->spinLagMin->setMaximum(value);
}

void QFFindErroneousCorrelationDlg::on_spinHighQuantile_editingFinished()
{
    performCalc();
}

void QFFindErroneousCorrelationDlg::on_spinLowQuantile_editingFinished()
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

        ui->labLagTimes->clear();
        //ui->labLagTimes->setText(tr("&tau;<sub>min</sub> ... &tau;<sub>max</sub> = %1s ... %2s").arg(doubleToUnitString(T[l0],5,true,'.',true)).arg(doubleToUnitString(T[l1],5,true,'.',true)));

        QVector<double> data;
        for (int i=0; i<fcs->getCorrelationRuns(); i++) {
            double value=0;
            double* ccf=fcs->getCorrelationRun(i);
            double cnt=0;
            for (int l=0; l<=fcs->getCorrelationN(); l++) {
                if (T[l]>=ui->spinLagMin->value()*1e-6 && T[l]<=ui->spinLagMax->value()*1e-6) {
                    value+=ccf[l];
                    cnt++;
                }
            }
            data<<value/cnt;
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
