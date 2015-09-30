/*
Copyright (c) 2012-2015 by Sebastian Isbaner

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#include "BurstStatisticsView.h"
#include "ui_BurstStatisticsView.h"
#include "gsl/gsl_histogram2d.h"

BurstStatisticsView::BurstStatisticsView(QWidget *parent, AlexData* data) :
    QWidget(parent),
    ui(new Ui::BurstStatisticsView)
{
    ui->setupUi(this);
    if(data==NULL) AlexEvalLog::warning("data not initialized");
    ad=data;
    rangeTgTr=ad->FRETparams;
}

BurstStatisticsView::~BurstStatisticsView()
{
    delete ui;
}

void BurstStatisticsView::storeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"spinBoxBins",ui->spinBoxBins->value());
    settings.setValue(prefix+"spinBoxMaxBurstSize",ui->spinBoxMaxBurstSize->value());
    settings.setValue(prefix+"splitterGeometry",ui->splitter->saveState());
    settings.setValue(prefix+"splitterGeometry2",ui->splitter_2->saveState());
}

void BurstStatisticsView::loadSettings(QSettings &settings, const QString &prefix)
{
    ui->spinBoxBins->setValue(settings.value(prefix+"spinBoxBins", ui->spinBoxBins->value()).toInt());
    ui->spinBoxMaxBurstSize->setValue(settings.value(prefix+"spinBoxMaxBurstSize", ui->spinBoxMaxBurstSize->value()).toInt());
    ui->splitter->restoreState(settings.value(prefix+"splitterGeometry").toByteArray());
    ui->splitter_2->restoreState(settings.value(prefix+"splitterGeometry2").toByteArray());
}

void BurstStatisticsView::evaluate()
{
    on_pushButtonRefresh_clicked();
}

void BurstStatisticsView::on_spinBoxBins_editingFinished()
{
    qDebug("BurstView::on_spinBoxBins_editingFinished");
    on_pushButtonRefresh_clicked();
}


void BurstStatisticsView::plotStatistics()
{
    qDebug("BurstView: plotStatistics");

    JKQtPlotter * plot;
    JKQTPdatastore* ds;
    JKQTPbarHorizontalGraph* g;

    gsl_histogram * durationDistribution;
    gsl_histogram * burstSizeDistribution;
    gsl_histogram * burstRateDistribution;

    switch(ui->comboBoxSource->currentIndex()) {
    case 0: // all bursts
        durationDistribution = ad->burstDurationDistribution(ui->spinBoxBins->value());
        burstSizeDistribution = ad->burstSizeDistribution(ui->spinBoxBins->value(),ui->spinBoxMaxBurstSize->value());
        burstRateDistribution = ad->burstRateDistribution(ui->spinBoxBins->value());
        break;
    case 1:
        AlexEvalLog::warning("not yet implemented");
//        durationDistribution = fd->burstDurationDistribution(ui->spinBoxBins->value());
//        burstSizeDistribution = fd->burstSizeDistribution(ui->spinBoxBins->value());
//        burstRateDistribution = fd->burstRateDistribution(ui->spinBoxBins->value());
//        break;
        return;
    default:
        AlexEvalLog::warning("unknown selection");
        break;
    }


//    for(size_t i=0;i<durationDistribution->n;++i) durationDistribution->range *=1e3; // convert to ms


    plot = ui->widPlotDuration;
    ds = plot->get_plotter()->getDatastore();
    ds->clear();    // empty datastore and grid
    plot->get_plotter()->clearGraphs(true);
    plot->get_plotter()->getXAxis()->set_axisLabel("burst duration in ms");
    plot->get_plotter()->getYAxis()->set_axisLabel("frequency");

    g=new JKQTPbarHorizontalGraph(plot->get_plotter());
    g->set_xColumn(ds->addCopiedColumn(durationDistribution->range, durationDistribution->n, "duration"));
    g->set_yColumn(ds->addCopiedColumn(durationDistribution->bin, durationDistribution->n, "frequency"));
    g->set_style(Qt::NoPen);
    g->set_color(QColor(DISTRIBUTION_COLOR));
    plot->addGraph(g);
    plot->zoomToFit();


    plot = ui->widPlotSize;
    ds=plot->get_plotter()->getDatastore();
    ds->clear();
    plot->get_plotter()->clearGraphs(true);
    plot->get_plotter()->getXAxis()->set_axisLabel("burst size");
    plot->get_plotter()->getYAxis()->set_axisLabel("frequency");

    g=new JKQTPbarHorizontalGraph(plot->get_plotter());
    g->set_xColumn(ds->addCopiedColumn(burstSizeDistribution->range, burstSizeDistribution->n, "burst size"));
    g->set_yColumn(ds->addCopiedColumn(burstSizeDistribution->bin, burstSizeDistribution->n, "frequency"));
    g->set_style(Qt::NoPen);
    g->set_color(QColor(DISTRIBUTION_COLOR));
    plot->addGraph(g);
    plot->zoomToFit();


    plot = ui->widPlotRate;
    ds=plot->get_plotter()->getDatastore();
    ds->clear();
    plot->get_plotter()->clearGraphs(true);
    plot->get_plotter()->getXAxis()->set_axisLabel("burst rate in kHz");
    plot->get_plotter()->getYAxis()->set_axisLabel("frequency");

    g=new JKQTPbarHorizontalGraph(plot->get_plotter());
    g->set_xColumn(ds->addCopiedColumn(burstRateDistribution->range, burstRateDistribution->n, "burst rate"));
    g->set_yColumn(ds->addCopiedColumn(burstRateDistribution->bin, burstRateDistribution->n, "frequency"));
    g->set_style(Qt::NoPen);
    g->set_color(QColor(DISTRIBUTION_COLOR));
    plot->addGraph(g);
    plot->zoomToFit();

    gsl_histogram_free(durationDistribution);
    gsl_histogram_free(burstSizeDistribution);
    gsl_histogram_free(burstRateDistribution);

}

void BurstStatisticsView::plotTgTr()
{
    JKQtPlotter * plot;
    JKQTPdatastore* ds;
    plot=ui->widPlotTg;
    plot->get_plotter()->get_xAxis()->set_axisLabel("duration t_D-t_A in ms");
    plot->get_plotter()->get_yAxis()->set_axisLabel("proximity ratio P");
    ds=plot->get_plotter()->getDatastore();
    ds->clear();
    plot->get_plotter()->clearGraphs(true);
    int nbins=ui->spinBoxBins->value();

    gsl_histogram2d * histAlex=gsl_histogram2d_calloc_uniform(nbins,nbins,rangeTgTr.MinTgTr,rangeTgTr.MaxTgTr,rangeTgTr.MinP,rangeTgTr.MaxP);
    for(int i=0; i<ad->burstVectorDual.size();++i)
        gsl_histogram2d_increment(histAlex, (ad->burstVectorDual.at(i).durationDonor-ad->burstVectorDual.at(i).durationAcceptor)*1e3, ad->burstVectorDual.at(i).proximityRatio);

    size_t colHist2D=ds->addCopiedImageAsColumnTranspose(histAlex->bin, histAlex->ny, histAlex->nx, tr("2Dhist"));

    JKQTPColumnMathImage* tgMtr=new JKQTPColumnMathImage(plot->get_plotter()); // bin(i,j) = bin[i * ny + j].
    tgMtr->set_x(rangeTgTr.MinTgTr);
    tgMtr->set_y(rangeTgTr.MinP);
    tgMtr->set_width(rangeTgTr.MaxTgTr-rangeTgTr.MinTgTr);
    tgMtr->set_height(rangeTgTr.MaxP-rangeTgTr.MinP);
    tgMtr->set_Nx(histAlex->nx);
    tgMtr->set_Ny(histAlex->ny);
    tgMtr->set_palette(JKQTPMathImageINVERTED_OCEAN);
    tgMtr->set_imageColumn(colHist2D);
    tgMtr->set_imageName(tr("#bursts"));
    tgMtr->set_nanColor(Qt::transparent);

    plot->addGraph(tgMtr);
    plot->zoomToFit();
    gsl_histogram2d_free(histAlex);
}

void BurstStatisticsView::on_pushButtonRefresh_clicked()
{
    if(ad->isEmptyBurst()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    plotStatistics();
    plotTgTr();
    QApplication::restoreOverrideCursor();
}

