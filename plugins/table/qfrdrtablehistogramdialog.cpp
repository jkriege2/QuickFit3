#include "qfrdrtablehistogramdialog.h"
#include "ui_qfrdrtablehistogramdialog.h"
#include "statistics_tools.h"

QFRDRTableHistogramDialog::QFRDRTableHistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableHistogramDialog)
{
    selected=-1;
    ui->setupUi(this);
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
        statisticsHistogramRanged(data.data(), datasize, hmin, hmax, histX.data(), histY.data(), ui->spinBins->value());


        headerNames[col_start]=tr("hist(%1): bins").arg(ui->cmbInput->currentText());
        headerNames[col_start+1]=tr("hist(%1): frequency").arg(ui->cmbInput->currentText());
        histograms[col_start]=histX;
        histograms[col_start+1]=histY;

        int c_bins=ui->plotter->getDatastore()->addCopiedColumn(histX.data(), histX.size(), tr("bins"));
        int c_hist=ui->plotter->getDatastore()->addCopiedColumn(histY.data(), histY.size(), tr("histogram"));
        JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(ui->plotter->get_plotter());
        g->set_xColumn(c_bins);
        g->set_yColumn(c_hist);
        g->set_title(0);
        ui->plotter->addGraph(g);
    }
    ui->plotter->zoomToFit();
    ui->plotter->set_doDrawing(true);
    ui->plotter->update_plot();

}

void QFRDRTableHistogramDialog::on_edtRangeMax_valueChanged(double value)
{
    if (!ui->chkRangeMax->isChecked())   updateHistograms();
}

void QFRDRTableHistogramDialog::on_edtRangeMin_valueChanged(double value)
{
    if (!ui->chkRangeMin->isChecked())   updateHistograms();
}

void QFRDRTableHistogramDialog::on_edtBinWidth_valueChanged(double value)
{
    if (!ui->chkAutoBinWidth->isChecked())   updateHistograms();
}

void QFRDRTableHistogramDialog::on_spinBins_valueChanged(int value)
{
    if (ui->chkAutoBinWidth->isChecked()) updateHistograms();
}

