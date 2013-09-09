#include "qfrdrtable2dhistogramdialog.h"
#include "ui_qfrdrtable2dhistogramdialog.h"
#include "statistics_tools.h"

QFRDRTable2DHistogramDialog::QFRDRTable2DHistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTable2DHistogramDialog)
{
    selectedX=-1;
    ui->setupUi(this);
}

QFRDRTable2DHistogramDialog::~QFRDRTable2DHistogramDialog()
{
    delete ui;
}

void QFRDRTable2DHistogramDialog::setTable(QFTablePluginModel *table)
{
    this->table=table;
}


void QFRDRTable2DHistogramDialog::setSelectedColumn(int col, int colY)
{
    selectedX=col;
    selectedY=colY;
}

void QFRDRTable2DHistogramDialog::init() {
    ui->cmbInputX->clear();
    ui->cmbInputY->clear();
    ui->cmbOutput->clear();
    ui->cmbOutput->addItem(tr("--- add column ---"), -1);
    for (int i=0; i<table->columnCount(); i++) {
        ui->cmbInputX->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
        ui->cmbInputY->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
        ui->cmbOutput->addItem(table->headerData(i, Qt::Horizontal).toString(), QVariant(i));
    }
    ui->cmbOutput->setCurrentIndex(0);
    ui->cmbInputX->setCurrentIndex(ui->cmbInputX->findData(selectedX));
    ui->cmbInputY->setCurrentIndex(ui->cmbInputY->findData(selectedY));
    updateHistograms();
    calcAutoBandwidth();
    calcAutoBinwidth();
    calcAutoBins();
    calcAutoBandwidthY();
    calcAutoBinwidthY();
    calcAutoBinsY();
    updateHistograms();
}

QMap<int, QVector<double> > QFRDRTable2DHistogramDialog::getHistograms() const
{
    return histograms;
}

QMap<int, QVector<QVariant> > QFRDRTable2DHistogramDialog::getHistograms2() const
{
    return histograms2;
}

QMap<int, QString> QFRDRTable2DHistogramDialog::getHeaderNames() const
{
    return headerNames;
}

void QFRDRTable2DHistogramDialog::updateHistograms(bool estimateBinWidth)
{
    histograms.clear();
    headerNames.clear();

    ui->plotter->set_doDrawing(false);
    ui->plotter->clearGraphs(true);
    ui->plotter->getDatastore()->clear();

    int colX_input=ui->cmbInputX->itemData(ui->cmbInputX->currentIndex()).toInt();
    int colY_input=ui->cmbInputY->itemData(ui->cmbInputY->currentIndex()).toInt();
    //qDebug()<<"col_input="<<col_input<<"   itemData="<<ui->cmbInput->itemData(ui->cmbInput->currentIndex());
    QVector<double> dataX=table->getColumnDataAsNumbers(colX_input);
    QVector<double> dataY=table->getColumnDataAsNumbers(colY_input);
    if (dataX.size()>0 && dataY.size()>0) {
        int col_start=ui->cmbOutput->itemData(ui->cmbOutput->currentIndex()).toInt();
        if (col_start<0) {
            col_start=table->columnCount();
        }
        //qDebug()<<"col_start="<<col_start<<"   ui->cmbOutput->currentIndex()="<<ui->cmbOutput->currentIndex()<<"   itemData="<<ui->cmbOutput->itemData(ui->cmbOutput->currentIndex(), -1);

        stat.values=statisticsFilterGoodFloat(dataX.data(), dataX.size());
        QVector<double> dataXS=dataX;
        statisticsSort(dataXS.data(), stat.values);
        stat.min=statisticsSortedMin(dataXS.data(), stat.values);
        stat.max=statisticsSortedMax(dataXS.data(), stat.values);
        stat.mean=statisticsAverage(dataXS.data(), stat.values);
        stat.var=statisticsStdDev(dataXS.data(), stat.values);
        stat.q25=statisticsSortedQuantile(dataXS.data(), stat.values, 0.25);
        stat.q75=statisticsSortedQuantile(dataXS.data(), stat.values, 0.75);
        stat.median=statisticsSortedMedian(dataXS.data(), stat.values);

        ui->labStatistics->setText(tr("<b>X:</b> N=%1,   &lang;X&rang;=%2,   &sigma;<sub>X</sub>=%3<br>min(X)=%4,   max(X)=%5,  q<sub>25%</sub>=%6,  q<sub>75%</sub>=%7")
                                   .arg(stat.values).arg(stat.mean).arg(stat.var).arg(stat.min).arg(stat.max).arg(stat.q25).arg(stat.q75));

        stat.var=stat.var*stat.var;
        double hmin=0;
        double hmax=0;
        if (ui->chkRangeMin->isChecked()) {
            hmin=stat.min;
            ui->edtRangeMin->setValue(hmin);
        } else {
            hmin=ui->edtRangeMin->value();
        }
        if (ui->chkRangeMax->isChecked()) {
            hmax=stat.max;
            ui->edtRangeMax->setValue(hmax);
        } else {
            hmax=ui->edtRangeMax->value();
        }

        double binWidth=0;
        if (estimateBinWidth) {
            binWidth=2.0*(stat.q75-stat.q25)/cbrt(stat.values);
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








        statY.values=statisticsFilterGoodFloat(dataY.data(), dataY.size());
        QVector<double> dataYS=dataY;
        statisticsSort(dataYS.data(), statY.values);
        statY.min=statisticsSortedMin(dataYS.data(), statY.values);
        statY.max=statisticsSortedMax(dataYS.data(), statY.values);
        statY.mean=statisticsAverage(dataYS.data(), statY.values);
        statY.var=statisticsStdDev(dataYS.data(), statY.values);
        statY.q25=statisticsSortedQuantile(dataYS.data(), statY.values, 0.25);
        statY.q75=statisticsSortedQuantile(dataYS.data(), statY.values, 0.75);
        statY.median=statisticsSortedMedian(dataYS.data(), statY.values);

        ui->labStatistics->setText(ui->labStatistics->text()+tr("<br><b>Y:</b> N=%1,   &lang;X&rang;=%2,   &sigma;<sub>X</sub>=%3<br>min(X)=%4,   max(X)=%5,  q<sub>25%</sub>=%6,  q<sub>75%</sub>=%7")
                                   .arg(statY.values).arg(statY.mean).arg(statY.var).arg(statY.min).arg(statY.max).arg(statY.q25).arg(statY.q75));

        statY.var=statY.var*statY.var;
        double hminY=0;
        double hmaxY=0;
        if (ui->chkRangeMinY->isChecked()) {
            hminY=statY.min;
            ui->edtRangeMinY->setValue(hminY);
        } else {
            hminY=ui->edtRangeMinY->value();
        }
        if (ui->chkRangeMaxY->isChecked()) {
            hmaxY=statY.max;
            ui->edtRangeMaxY->setValue(hmaxY);
        } else {
            hmaxY=ui->edtRangeMaxY->value();
        }

        double binWidthY=0;
        if (estimateBinWidth) {
            binWidthY=2.0*(statY.q75-statY.q25)/cbrt(statY.values);
            ui->edtBinWidthY->setValue(binWidthY);
            ui->spinBinsY->setValue((hmaxY-hminY)/binWidthY);
        } else {
            if (ui->chkAutoBinWidthY->isChecked()) {
                binWidthY=(hmaxY-hminY)/double(ui->spinBinsY->value());
                ui->edtBinWidthY->setValue(binWidthY);
            } else {
                binWidthY=ui->edtBinWidthY->value();
                ui->spinBinsY->setValue((hmaxY-hminY)/binWidthY);
            }
        }





        QVector<double> histX, histY, histogram;
        histX.resize(ui->spinBins->value());
        histY.resize(ui->spinBinsY->value());
        histogram.resize(ui->spinBins->value()*ui->spinBinsY->value());

        if (ui->cmbMethod->currentIndex()==0) statisticsHistogram2DRanged(dataX.data(), dataY.data(), qMin(stat.values, statY.values), hmin, hmax, hminY, hmaxY, histX.data(), histY.data(), histogram.data(), ui->spinBins->value(), ui->spinBinsY->value(), ui->chkNormalized->isChecked());
        else if (ui->cmbMethod->currentIndex()==1) statistics2DKDEHistogramRanged(dataX.data(), dataY.data(), qMin(stat.values, statY.values), statisticsKernel2DGaussian, ui->edtBandwidth->value(), ui->edtBandwidthY->value(), hmin, hmax, hminY, hmaxY, histX.data(), histY.data(), histogram.data(), ui->spinBins->value(), ui->spinBinsY->value());
        else if (ui->cmbMethod->currentIndex()==2) statistics2DKDEHistogramRanged(dataX.data(), dataY.data(), qMin(stat.values, statY.values), statisticsKernel2DUniform, ui->edtBandwidth->value(), ui->edtBandwidthY->value(), hmin, hmax, hminY, hmaxY, histX.data(), histY.data(), histogram.data(), ui->spinBins->value(), ui->spinBinsY->value());



        QString hname="";
        if (ui->chkNormalized->isChecked()) {
            statisticsNormalize(histogram.data(), histogram.size());
            hname+=tr("normalized");
        }




        headerNames[col_start]=tr("hist(%1): binsX").arg(ui->cmbInputX->currentText());
        headerNames[col_start+1]=tr("hist(%1): binsY").arg(ui->cmbInputX->currentText());
        headerNames[col_start+2]=tr("hist(%1 vs. %2): %3 frequency").arg(ui->cmbInputX->currentText()).arg(ui->cmbInputY->currentText()).arg(hname);
        headerNames[col_start+3]=tr("hist(%1 vs. %2): parameter names").arg(ui->cmbInputX->currentText()).arg(ui->cmbInputY->currentText());
        headerNames[col_start+4]=tr("hist(%1 vs. %2): parameters").arg(ui->cmbInputX->currentText()).arg(ui->cmbInputY->currentText());
        histograms[col_start]=histX;
        histograms[col_start+1]=histY;
        histograms[col_start+2]=histogram;
        QVector<QVariant> d;
        d.clear();
        d<<tr("Nx")<<tr("Ny")<<tr("x")<<tr("y")<<tr("width")<<tr("height");
        histograms2[col_start+3]=d;
        d.clear();
        d<<ui->spinBins->value()<<ui->spinBinsY->value()<<stat.min<<statY.min<<(stat.max-stat.min)<<(statY.max-statY.min);
        histograms2[col_start+3]=d;

        int c_hist=ui->plotter->getDatastore()->addCopiedColumn(histogram.data(), histogram.size(), tr("%2 histogram").arg(hname));
        JKQTPColumnMathImage* g=new JKQTPColumnMathImage(ui->plotter->get_plotter());
        g->set_imageColumn(c_hist);
        g->set_palette(JKQTPMathImage::INVERTED_OCEAN);
        g->set_title("");
        g->set_x(stat.min);
        g->set_y(statY.min);
        g->set_width(stat.max-stat.min);
        g->set_height(statY.max-statY.min);
        g->set_Nx(ui->spinBins->value());
        g->set_Ny(ui->spinBinsY->value());
        ui->plotter->addGraph(g);
        ui->plotter->getYAxis()->set_axisLabel(tr("%2 frequency").arg(hname));
    }
    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();

}

void QFRDRTable2DHistogramDialog::on_edtRangeMax_valueChanged(double value)
{
    if (!ui->chkRangeMax->isChecked())   updateHistograms();
}

void QFRDRTable2DHistogramDialog::on_edtRangeMin_valueChanged(double value)
{
    if (!ui->chkRangeMin->isChecked())   updateHistograms();
}

void QFRDRTable2DHistogramDialog::on_edtBinWidth_valueChanged(double value)
{
    if (!ui->chkAutoBinWidth->isChecked())   updateHistograms();
}

void QFRDRTable2DHistogramDialog::on_spinBins_valueChanged(int value)
{
    if (ui->chkAutoBinWidth->isChecked()) updateHistograms();
}

void QFRDRTable2DHistogramDialog::on_edtRangeMaxY_valueChanged(double value)
{
    if (!ui->chkRangeMaxY->isChecked())   updateHistograms();

}

void QFRDRTable2DHistogramDialog::on_edtRangeMinY_valueChanged(double value)
{
    if (!ui->chkRangeMinY->isChecked())   updateHistograms();

}

void QFRDRTable2DHistogramDialog::on_edtBinWidthY_valueChanged(double value)
{
    if (!ui->chkAutoBinWidthY->isChecked())   updateHistograms();

}

void QFRDRTable2DHistogramDialog::on_spinBinsY_valueChanged(int value)
{
    if (ui->chkAutoBinWidthY->isChecked()) updateHistograms();

}

void QFRDRTable2DHistogramDialog::on_cmbMethod_currentIndexChanged(int current)
{
    ui->edtBandwidth->setEnabled(current>0);
    ui->btnAutoBandwidth->setEnabled(current>0);
    ui->edtBandwidthY->setEnabled(current>0);
    ui->btnAutoBandwidthY->setEnabled(current>0);
}

void QFRDRTable2DHistogramDialog::on_btnAutoBinwidth_clicked()
{
    calcAutoBinwidth();
    updateHistograms();
}

void QFRDRTable2DHistogramDialog::on_btnAutoBandwidth_clicked()
{
    calcAutoBandwidth();
    updateHistograms();
}

void QFRDRTable2DHistogramDialog::on_btnAutoBins_clicked()
{
    calcAutoBins();
    updateHistograms();
}

void QFRDRTable2DHistogramDialog::on_btnAutoBinwidthY_clicked()
{
    calcAutoBinwidthY();
    updateHistograms();

}

void QFRDRTable2DHistogramDialog::on_btnAutoBandwidthY_clicked()
{
    calcAutoBandwidthY();
    updateHistograms();

}

void QFRDRTable2DHistogramDialog::on_btnAutoBinsY_clicked()
{
    calcAutoBinsY();
    updateHistograms();

}

void QFRDRTable2DHistogramDialog::calcAutoBinwidth()
{
    ui->edtBinWidth->setValue(2.0*(stat.q75-stat.q25)/cbrt(stat.values));
}

void QFRDRTable2DHistogramDialog::calcAutoBandwidth()
{
    ui->edtBandwidth->setValue(1.06*stat.var/pow(stat.values, 1.0/5.0));
}

void QFRDRTable2DHistogramDialog::calcAutoBins()
{
    ui->spinBins->setValue(sqrt(stat.values));
}

void QFRDRTable2DHistogramDialog::calcAutoBinwidthY()
{

    ui->edtBinWidthY->setValue(2.0*(statY.q75-statY.q25)/cbrt(statY.values));
}

void QFRDRTable2DHistogramDialog::calcAutoBandwidthY()
{
    ui->edtBandwidthY->setValue(1.06*statY.var/pow(statY.values, 1.0/5.0));

}

void QFRDRTable2DHistogramDialog::calcAutoBinsY()
{
    ui->spinBinsY->setValue(sqrt(statY.values));

}

