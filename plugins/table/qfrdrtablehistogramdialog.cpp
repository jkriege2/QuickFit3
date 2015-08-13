/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qfrdrtablehistogramdialog.h"
#include "ui_qfrdrtablehistogramdialog.h"
#include "statistics_tools.h"

QFRDRTableHistogramDialog::QFRDRTableHistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableHistogramDialog)
{
    selected=-1;
    ui->setupUi(this);
    ui->edtBandwidth->setCheckBounds(false, false);
    ui->edtRangeMin->setCheckBounds(false, false);
    ui->edtRangeMax->setCheckBounds(false, false);
    ui->edtBinWidth->setCheckBounds(false, false);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFRDRTableHistogramDialog::~QFRDRTableHistogramDialog()
{
    delete ui;
}

void QFRDRTableHistogramDialog::setTable(QFTablePluginModel *table)
{
    this->table=table;
}


void QFRDRTableHistogramDialog::setSelectedColumn(int col)
{
    selected=col;
}

void QFRDRTableHistogramDialog::init() {
    ui->cmbInput->clear();
    ui->cmbOutput->clear();
    ui->cmbOutput->addItem(tr("--- add column ---"), -1);
    for (int i=0; i<table->columnCount(); i++) {
        ui->cmbInput->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
        ui->cmbOutput->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
    }
    ui->cmbOutput->setCurrentIndex(0);
    ui->cmbInput->setCurrentIndex(ui->cmbInput->findData(selected));
    updateHistograms();
    calcAutoBandwidth();
    calcAutoBinwidth();
    calcAutoBins();
    updateHistograms();
}

QMap<int, QVector<double> > QFRDRTableHistogramDialog::getHistograms() const
{
    return histograms;
}

QMap<int, QString> QFRDRTableHistogramDialog::getHeaderNames() const
{
    return headerNames;
}

void QFRDRTableHistogramDialog::updateHistograms(bool estimateBinWidth)
{
    histograms.clear();
    headerNames.clear();

    ui->plotter->set_doDrawing(false);
    ui->plotter->clearGraphs(true);
    ui->plotter->getDatastore()->clear();

    int col_input=ui->cmbInput->itemData(ui->cmbInput->currentIndex()).toInt();
    //qDebug()<<"col_input="<<col_input<<"   itemData="<<ui->cmbInput->itemData(ui->cmbInput->currentIndex());
    QVector<double> data=table->getColumnDataAsNumbers(col_input);
    if (data.size()>0) {
        int col_start=ui->cmbOutput->itemData(ui->cmbOutput->currentIndex()).toInt();
        if (col_start<0) {
            col_start=table->columnCount();
        }
        //qDebug()<<"col_start="<<col_start<<"   ui->cmbOutput->currentIndex()="<<ui->cmbOutput->currentIndex()<<"   itemData="<<ui->cmbOutput->itemData(ui->cmbOutput->currentIndex(), -1);

        int datasize=statisticsFilterGoodFloat(data.data(), data.size());
        statisticsSort(data.data(), datasize);
        double dataMin=statisticsSortedMin(data.data(), datasize);
        double dataMax=statisticsSortedMax(data.data(), datasize);
        double dataAvg=statisticsAverage(data.data(), datasize);
        double dataSD=statisticsStdDev(data.data(), datasize);
        double dataQ25=statisticsSortedQuantile(data.data(), datasize, 0.25);
        double dataQ75=statisticsSortedQuantile(data.data(), datasize, 0.75);
        stat.max=dataMax;
        stat.min=dataMin;
        stat.median=statisticsSortedMedian(data.data(), datasize);
        stat.mean=dataAvg;
        stat.q25=dataQ25;
        stat.q75=dataQ75;
        stat.var=dataSD*dataSD;
        stat.values=datasize;

        ui->labStatistics->setText(tr("N=%1,   &lang;X&rang;=%2,   &sigma;<sub>X</sub>=%3<br>min(X)=%4,   max(X)=%5,  q<sub>25%</sub>=%6,  q<sub>75%</sub>=%7")
                                   .arg(datasize).arg(dataAvg).arg(dataSD).arg(dataMin).arg(dataMax).arg(dataQ25).arg(dataQ75));

        double hmin=0;
        double hmax=0;
        if (ui->chkRangeMin->isChecked()) {
            hmin=dataMin;
            ui->edtRangeMin->setValue(hmin);
        } else {
            hmin=ui->edtRangeMin->value();
        }
        if (ui->chkRangeMax->isChecked()) {
            hmax=dataMax;
            ui->edtRangeMax->setValue(hmax);
        } else {
            hmax=ui->edtRangeMax->value();
        }

        double binWidth=0;
        if (estimateBinWidth) {
            binWidth=2.0*(dataQ75-dataQ25)/cbrt(datasize);
            ui->edtBinWidth->setValue(binWidth);
            ui->spinBins->setValue((hmax-hmin)/binWidth);
        } else {
            if (ui->chkAutoBinWidth->isChecked()) {
                binWidth=(hmax-hmin)/double(ui->spinBins->value());
                ui->edtBinWidth->setValue(binWidth);
            } else {
                binWidth=ui->edtBinWidth->value();
                ui->spinBins->setValue((hmax-hmin)/binWidth);
            }
        }

        QVector<double> histX, histY;
        histX.resize(ui->spinBins->value());
        histY.resize(ui->spinBins->value());

        if (ui->cmbMethod->currentIndex()==0) statisticsHistogramRanged(data.data(), datasize, hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==1) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DGaussian, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==2) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DCauchy, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==3) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DPicard, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==4) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DEpanechnikov, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==5) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DUniform, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==6) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DTriangle, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==7) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DQuartic, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==8) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DTriweight, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==9) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DTricube, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());
        else if (ui->cmbMethod->currentIndex()==10) statisticsKDEHistogramRanged(data.data(), datasize, statisticsKernel1DCosine, ui->edtBandwidth->value(), hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());



        QString hname="";
        if (ui->chkNormalized->isChecked()) {
            statisticsNormalize(histY.data(), histY.size());
            hname+=tr("normalized");
        }
        if (ui->chkCumulative->isChecked()) {
            statisticsAccumulateHistogram(histY.data(), histY.size());
            hname+=tr(" accumulated");
        }



        headerNames[col_start]=tr("hist(%1): bins").arg(ui->cmbInput->currentText());
        if (ui->chkCumulative->isChecked()) headerNames[col_start+1]=tr("hist(%1): %2 frequency").arg(ui->cmbInput->currentText()).arg(hname);
        else headerNames[col_start+1]=tr("hist(%1): %2 frequency").arg(ui->cmbInput->currentText()).arg(hname);
        histograms[col_start]=histX;
        histograms[col_start+1]=histY;

        int c_bins=ui->plotter->getDatastore()->addCopiedColumn(histX.data(), histX.size(), tr("bins"));
        int c_hist=ui->plotter->getDatastore()->addCopiedColumn(histY.data(), histY.size(), tr("%2 histogram").arg(hname));
        JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(ui->plotter->get_plotter());
        g->set_xColumn(c_bins);
        g->set_yColumn(c_hist);
        g->set_title("");
        ui->plotter->addGraph(g);
        ui->plotter->getYAxis()->set_axisLabel(tr("%2 frequency").arg(hname));
    }
    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();

}

void QFRDRTableHistogramDialog::on_edtRangeMax_valueChanged(double /*value*/)
{
    if (!ui->chkRangeMax->isChecked())   updateHistograms();
}

void QFRDRTableHistogramDialog::on_edtRangeMin_valueChanged(double /*value*/)
{
    if (!ui->chkRangeMin->isChecked())   updateHistograms();
}

void QFRDRTableHistogramDialog::on_edtBinWidth_valueChanged(double /*value*/)
{
    if (!ui->chkAutoBinWidth->isChecked())   updateHistograms();
}

void QFRDRTableHistogramDialog::on_spinBins_valueChanged(int /*value*/)
{
    if (ui->chkAutoBinWidth->isChecked()) updateHistograms();
}

void QFRDRTableHistogramDialog::on_cmbMethod_currentIndexChanged(int current)
{
    ui->edtBandwidth->setEnabled(current>0);
    ui->btnAutoBandwidth->setEnabled(current>0);
}

void QFRDRTableHistogramDialog::on_btnAutoBinwidth_clicked()
{
    calcAutoBinwidth();
    updateHistograms();
}

void QFRDRTableHistogramDialog::on_btnAutoBandwidth_clicked()
{
    calcAutoBandwidth();
    updateHistograms();
}

void QFRDRTableHistogramDialog::on_btnAutoBins_clicked()
{
    calcAutoBins();
    updateHistograms();
}

void QFRDRTableHistogramDialog::calcAutoBinwidth()
{
    ui->edtBinWidth->setValue(2.0*(stat.q75-stat.q25)/cbrt(stat.values));
}

void QFRDRTableHistogramDialog::calcAutoBandwidth()
{
    ui->edtBandwidth->setValue(1.06*stat.var/pow(stat.values, 1.0/5.0));
}

void QFRDRTableHistogramDialog::calcAutoBins()
{
    ui->spinBins->setValue(sqrt(stat.values));
}

