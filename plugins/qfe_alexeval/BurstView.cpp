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

#include "BurstView.h"
#include "ui_BurstView.h"
#include "gsl/gsl_histogram.h"
#include "analysis.h"
#include <QDebug>

// need this for edges graph
#define SmallestGreaterZeroCompare_xvsgz() if ((xvsgz>10.0*DBL_MIN)&&((smallestGreaterZero<10.0*DBL_MIN) || (xvsgz<smallestGreaterZero))) smallestGreaterZero=xvsgz;

BurstView::BurstView(QWidget *parent, AlexData *data) :
    QWidget(parent),
    ui(new Ui::BurstView)
{
    ui->setupUi(this);
    if(data==NULL) AlexEvalLog::warning("data not initialized");
    ad=data;

    ui->tableBursts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableBursts->setSortingEnabled(false);
    ui->tableBursts->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableBursts->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableBursts->setShowGrid(true);
//    ui->tableBursts->setStyleSheet("QTableView {selection-background-color: red;}");

    m_BurstTableHeader<<"start time in s"<<"duration in ms"<<"PR"<<"S"<<"Burst Size"<<"n_Dem_Dex"<<"n_Aem_Dex"<<"n_Dem_Aex"<<"n_Aem_Aex"<<"burstrate in kHz"<<"PR raw"<<"S raw"<<"type"<<"duration ch1"<<"duration ch2"<<"energy transfer"<<"startIdx"<<"startIdxCh1"<<"startIdxCh2";
    ui->tableBursts->setColumnCount(m_BurstTableHeader.size());
    ui->tableBursts->setHorizontalHeaderLabels(m_BurstTableHeader);

    connect( ui->tableBursts, SIGNAL( cellDoubleClicked (int, int) ),
             this, SLOT( cellSelected( int, int ) ) );
}

void BurstView::refresh()
{
    fillTable();
}

BurstView::~BurstView()
{
    delete ui;
}

void BurstView::storeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"comboBoxPlotType",ui->comboBoxPlotType->currentIndex());
    settings.setValue(prefix+"doubleSpinBoxBinwidth",ui->doubleSpinBoxBinwidth->value());
    settings.setValue(prefix+"doubleSpinBoxOffset",ui->doubleSpinBoxOffset->value());
    settings.setValue(prefix+"spinBoxAlpha",ui->spinBoxAlpha->value());
    settings.setValue(prefix+"spinBoxAlphaBurst",ui->spinBoxAlphaBurst->value());
    settings.setValue(prefix+"splitterGeometry",ui->splitter->saveState());
    settings.setValue(prefix+"splitterGeometry2",ui->splitter_2->saveState());
}

void BurstView::loadSettings(QSettings &settings, const QString &prefix)
{
    ui->comboBoxPlotType->setCurrentIndex(settings.value(prefix+"comboBoxPlotType", ui->comboBoxPlotType->currentIndex()).toInt());
    ui->doubleSpinBoxBinwidth->setValue(settings.value(prefix+"doubleSpinBoxBinwidth", ui->doubleSpinBoxBinwidth->value()).toDouble());
    ui->doubleSpinBoxOffset->setValue(settings.value(prefix+"doubleSpinBoxOffset", ui->doubleSpinBoxOffset->value()).toDouble());
    ui->spinBoxAlpha->setValue(settings.value(prefix+"spinBoxAlpha", ui->spinBoxAlpha->value()).toInt());
    ui->spinBoxAlphaBurst->setValue(settings.value(prefix+"spinBoxAlphaBurst", ui->spinBoxAlphaBurst->value()).toInt());
    ui->splitter->restoreState(settings.value(prefix+"splitterGeometry").toByteArray());
    ui->splitter_2->restoreState(settings.value(prefix+"splitterGeometry2").toByteArray());
}

void BurstView::cellSelected(int row, int column)
{
//    QMessageBox::information(this, "",
    qDebug()<<
                            "Cell at row "+QString::number(row)+
                             " column "+QString::number(column)+
                " was double clicked.";
    if(ui->comboBoxPlotType->currentIndex()==0)
        plotIntensity();
    else if(ui->comboBoxPlotType->currentIndex()==1)
        plotTTTR();
    }



void BurstView::fillTable()
{
    ui->tableBursts->clearContents();
    if(ad->isEmptyBurst()) return;
    qDebug("BurstView::fillTable");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->tableBursts->setRowCount(ad->burstVectorDual.size());

    // if you want to add items, change the headers in m_BurstTableHeader first (see constructor of BurstView).
    for(int i=0; i<ad->burstVectorDual.size();++i ) {

        int k=0;
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).startTime)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).duration*1e3)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).proximityRatio)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).stoichiometryRatio)));

        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).numberOfPhotons)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).n_Dem_Dex)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).n_Aem_Dex)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).n_Dem_Aex)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).n_Aem_Aex)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).burstrate_kHz())));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).proximityRatioRaw)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).stoichiometryRatioRaw)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(ad->burstVectorDual.at(i).typeName()));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).durationDonor*1e3)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).durationAcceptor*1e3)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).energyTransfer)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).startIdx)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).startIdxDonor)));
        ui->tableBursts->setItem(i, k++, new QTableWidgetItem(QString::number(ad->burstVectorDual.at(i).startIdxAcceptor)));
    }
    qDebug()<<ui->tableBursts->rowCount();
//    for(int i=0; i<ui->tableBursts->columnCount();++i) {
    ui->tableBursts->resizeColumnsToContents();

    QApplication::restoreOverrideCursor();
}

void BurstView::plotIntensity()
{
    qDebug("BurstView: plotIntensity");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    JKQtPlotter * plot = ui->widPlotBurst;

    BurstDual burstDisplayed = ad->burstVectorDual.at(ui->tableBursts->selectedRanges().first().topRow());
    double offset = ui->doubleSpinBoxOffset->value()*1e-3;
    burstDisplayed.startTime=burstDisplayed.startTime-offset;
    if(((burstDisplayed.startTime)<ad->start) )  burstDisplayed.startTime=ad->start;
    burstDisplayed.duration+=2*offset;
    if(((burstDisplayed.endTime())>= ad->stop) )  burstDisplayed.duration=ad->stop-burstDisplayed.startTime;
    while( (burstDisplayed.startIdx>0) && (ad->photons.at(burstDisplayed.startIdx).time>burstDisplayed.startTime) )  burstDisplayed.startIdx--;
    while((burstDisplayed.endIdx()<ad->photons.size()) && ad->photons.at(burstDisplayed.endIdx()).time<burstDisplayed.endTime())  burstDisplayed.numberOfPhotons++;

    double binwidth=ui->doubleSpinBoxBinwidth->value()*1e-6;
    int nbins=(int)(burstDisplayed.duration/binwidth+1);
    gsl_histogram * histDonor = gsl_histogram_alloc (nbins); // donor emission channel
    gsl_histogram * histAcceptor = gsl_histogram_alloc (nbins); // acceptor emission channel
    gsl_histogram_set_ranges_uniform (histDonor, burstDisplayed.startTime, burstDisplayed.endTime() );
    gsl_histogram_set_ranges_uniform (histAcceptor, burstDisplayed.startTime, burstDisplayed.endTime());

    for(int i=burstDisplayed.startIdx; i<burstDisplayed.endIdx();++i) {
#ifdef PHOTONMACRO
        if(isDem(ad->photons.at(i))) gsl_histogram_increment (histDonor, ad->photons.at(i).time);
        if(isAem(ad->photons.at(i))) gsl_histogram_increment (histAcceptor, ad->photons.at(i).time);
#else
        if(ad->photons.at(i).isDem()) gsl_histogram_increment (histDonor, ad->photons.at(i).time);
        if(ad->photons.at(i).isAem()) gsl_histogram_increment (histAcceptor, ad->photons.at(i).time);
#endif
    }

//    qDebug("t in s\tDonor emission\tAcceptor emission\ttotal emission");
//    for (size_t i=0; i<histDonor->n; ++i)
//        qDebug("%lg\t%lg\t%lg\t%lg", histDonor->range[i], histDonor->bin[i],histAcceptor->bin[i],histDonor->bin[i]+histAcceptor->bin[i]);

    gsl_histogram_scale(histAcceptor,-1.0);

    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    ds->clear();
    plot->get_plotter()->clearGraphs(true);

    plot->setObjectName("timetrace");
    plot->get_plotter()->getXAxis()->set_axisLabel("time in s"); // $x$ axis with greek letters $\\sqrt{\\alpha\\cdot\\beta}$
    plot->get_plotter()->getYAxis()->set_axisLabel("number of photons");

    // store data in plot
    plot->get_plotter()->setGrid(false);
    unsigned long long nrows=(unsigned long long) histDonor->n;
//    for()
    size_t xColumn=ds->addCopiedColumn(histDonor->range, nrows, "time in s"); // adds column (copies values!) and returns column ID
    QVector<size_t> yColumns;
    yColumns.push_back(ds->addCopiedColumn(histDonor->bin, nrows, "Donor"));
    yColumns.push_back(ds->addCopiedColumn(histAcceptor->bin, nrows, "Acceptor"));
    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);
    QStringList titles;
    titles<<"Donor"<<"Acceptor";
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_DONOR));
    colors.append(QColor(COUNTRATE_COLOR_ACCEPTOR));
    // plot
    double width=1.0;
    for (int i=0; i<yColumns.size(); i++) {
        JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(plot->get_plotter());
        g->set_title(titles[i]);
        g->set_xColumn(xColumn);
        g->set_yColumn(yColumns[i]);
        g->set_width(width);
        g->set_style(Qt::NoPen);
        g->set_fillColor(colors[i]);
        plot->addGraph(g);
    }
    plot->zoomToFit();
    plot->get_plotter()->set_keyPosition(JKQTPkeyInsideTopRight); // set legend position

    plotMarker(plot,burstDisplayed.startTime,burstDisplayed.endTime(),qMax(plot->getXMax(),plot->getYMax()));

    QApplication::restoreOverrideCursor();
}

void BurstView::plotTTTR()
{
    qDebug("BurstView: plotTTTR");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    JKQtPlotter * plot = ui->widPlotBurst;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    ds->clear();
    plot->get_plotter()->clearGraphs(true);

    BurstDual burstDisplayed = ad->burstVectorDual.at(ui->tableBursts->selectedRanges().first().topRow());
    double burstStartTime=burstDisplayed.startTime;
    double burstEndTime=burstDisplayed.endTime();
    double offset = ui->doubleSpinBoxOffset->value()*1e-3+10e-6;
    burstDisplayed.startTime=burstDisplayed.startTime-offset;
    if(((burstDisplayed.startTime)<ad->start) )  burstDisplayed.startTime=ad->start;
    burstDisplayed.duration+=2*offset;
    if(((burstDisplayed.endTime())>= ad->stop) )  burstDisplayed.duration=ad->stop-burstDisplayed.startTime;
    while( (burstDisplayed.startIdx>0) && (ad->photons.at(burstDisplayed.startIdx).time>burstDisplayed.startTime) )  burstDisplayed.startIdx--;
    while((burstDisplayed.endIdx()<ad->photons.size()) && ad->photons.at(burstDisplayed.endIdx()).time<burstDisplayed.endTime())  burstDisplayed.numberOfPhotons++;

    plot->get_plotter()->getXAxis()->set_axisLabel("time in s");
    plot->get_plotter()->getYAxis()->clearAxisTickLabels();
    plot->get_plotter()->getYAxis()->set_drawMode1(JKQTPCADMline);
    plot->get_plotter()->setGrid(false);


    JKQTPPeakStreamGraph *peakGraph = new JKQTPPeakStreamGraph(plot->get_plotter());
    peakGraph->set_baseline(0);
    peakGraph->set_peakHeight(1.0);
    peakGraph->set_color(QColor(COUNTRATE_COLOR_DONOR));
    QVector<double> donor=ad->photons.vectorChannelMarker(DonorEm,burstDisplayed.startIdx,burstDisplayed.endIdx());
    peakGraph->set_dataColumn(ds->addCopiedColumn(donor.data(),donor.size(), "Donor"));
    plot->addGraph(peakGraph);

    peakGraph = new JKQTPPeakStreamGraph(plot->get_plotter());
    peakGraph->set_baseline(0);
    peakGraph->set_peakHeight(-1.0);
    peakGraph->set_color(QColor(COUNTRATE_COLOR_ACCEPTOR));
//    peakGraph->set_dataColumn(ds->addColumn(&(ad->arrivalTimeCh2[burst.startIdxAcceptor]), stopIdxA-burst.startIdxAcceptor, "Acceptor"));
    QVector<double> acceptor=ad->photons.vectorChannelMarker(AcceptorEm,burstDisplayed.startIdx,burstDisplayed.endIdx());
    peakGraph->set_dataColumn(ds->addCopiedColumn(acceptor.data(),acceptor.size(), "Acceptor"));
    plot->addGraph(peakGraph);

    plot->zoom(burstDisplayed.startTime,burstDisplayed.endTime(),-1,1);
    plot->zoomToFit(false);

    // plot marker events
    plotMarker(plot,burstDisplayed.startTime,burstDisplayed.endTime());


    //    JKQTPverticalRange to mark the real burst duration

    JKQTPverticalRange *gr;
    gr = new JKQTPverticalRange(plot->get_plotter());
    gr->set_rangeMin(burstStartTime);
    gr->set_rangeMax(burstEndTime);
    gr->set_invertedRange(true);
    QColor col = QColor(Qt::lightGray);
    col.setAlpha(ui->spinBoxAlphaBurst->value());
    gr->set_fillColor(col);
    gr->set_style(Qt::NoPen);
    gr->set_plotCenterLine(false);
    plot->addGraph(gr);

    plotBurstCorr(burstDisplayed);


    QApplication::restoreOverrideCursor();
}

void BurstView::plotMarker(JKQtPlotter *plot, const double startTime, const double endTime, const double height)
{

    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();

    // search for start and end index of markers using startTime and endTime
    QVector<double> markerEvents;
    QVector<double>::iterator startMarker = qLowerBound(ad->markerTimeDexc.begin(),ad->markerTimeDexc.end(),startTime);
    --startMarker;
    int markerIdx=std::distance(ad->markerTimeDexc.begin(),startMarker);
    while((markerIdx<(ad->markerTimeDexc.size()-1))&&(ad->markerTimeDexc.at(markerIdx)<endTime)) {
        markerEvents.append(ad->markerTimeDexc.at(markerIdx));
        markerEvents.append(ad->markerTimeAexc.at(markerIdx));
        markerIdx++;
    }
    markerEvents.append(ad->markerTimeDexc.at(markerIdx));
    markerEvents.append(ad->markerTimeAexc.at(markerIdx));
    qDebug()<<"binary search start" << *startMarker; // seems to work!
    qDebug()<<"binary search end" <<  ad->markerTimeDexc.at(markerIdx);
    size_t xColumn=ds->addCopiedColumn(markerEvents.data(), markerEvents.size(), "MarkerEvents");

    double heightScale=2*height;
    double baseline=-height;
    QVector<double> heightD, heightA;
    for(int i=0;i<markerEvents.size();++i) {
        heightD.append(((i+1)%2)*heightScale); // alternates between 0 and 2. markerEvents starts with donor marker!
        heightA.append((i%2)*heightScale);
    }
    QVector<size_t> yColumns;
    yColumns.append(ds->addCopiedColumn(heightD.data(), heightD.size(), "DonorMarker")); // adds column (copies values!) and returns column ID
    yColumns.append(ds->addCopiedColumn(heightA.data(), heightA.size(), "AcceptorMarker"));

    // plot
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_DONOR));
    colors.append(QColor(COUNTRATE_COLOR_ACCEPTOR));
    for (int i=0; i<yColumns.size(); i++) {
        JKQTPbarHorizontalGraphEdges* g=new JKQTPbarHorizontalGraphEdges(plot->get_plotter());
        colors[i].setAlpha(ui->spinBoxAlpha->value());
        g->set_baseline(baseline);
        g->set_xColumn(xColumn);
        g->set_yColumn(yColumns.at(i));
        g->set_width(1.0);
        g->set_style(Qt::NoPen);
        g->set_fillColor(colors.at(i));
        plot->addGraph(g);
    }

}

void BurstView::plotBurstCorr(const BurstDual &burst)
{
    JKQtPlotter * plot = ui->widgetPlotCorr;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    ds->clear();
    plot->clearGraphs();
    QVector<double> ccorr;
    QVector<double> corrA;
    QVector<double> corrD;
    QVector<double> tau;
    QVector<double> donor=ad->photons.vectorChannelMarker(DonorEm,burst.startIdx,burst.endIdx());
    QVector<double> acceptor=ad->photons.vectorChannelMarker(AcceptorEm,burst.startIdx,burst.endIdx());
    crossCorrelateTOA(tau,ccorr,donor.data(),donor.size(),acceptor.data(),acceptor.size(),ad->FCSparams);
    correlateTOA(tau,corrA,acceptor.data(),acceptor.size(),ad->FCSparams);
    correlateTOA(tau,corrD,donor.data(),donor.size(),ad->FCSparams);


    plot->setObjectName("FCS");
    plot->setGrid(false);
    plot->get_plotter()->set_plotLabel("single burst FCS");
    plot->get_plotter()->getXAxis()->set_axisLabel("lag time $\\tau$"); // $x$ axis with greek letters $\\sqrt{\\alpha\\cdot\\beta}$
    plot->get_plotter()->getYAxis()->set_axisLabel("g($\\tau$)");
    plot->get_plotter()->getXAxis()->set_logAxis(true);

    size_t xColumn=ds->addCopiedColumn(tau.data(), tau.size(), "tau");
    QVector<size_t> yColumns;
    yColumns.push_back(ds->addCopiedColumn(corrD.data(), corrD.size(), "g_D"));
    yColumns.push_back(ds->addCopiedColumn(corrA.data(), corrA.size(), "g_A"));
    yColumns.push_back(ds->addCopiedColumn(ccorr.data(), ccorr.size(), "crosscorr"));
    QStringList titles;
    titles<<"auto corr D"<<"auto corr A"<<"cross corr DA";
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_DONOR));
    colors.append(QColor(COUNTRATE_COLOR_ACCEPTOR));
    colors.append(QColor(FCS_COLOR_CROSS));

    // line plots
    for (int i=0; i<yColumns.size(); i++) {
        JKQTPxyLineGraph*lg=new JKQTPxyLineGraph(plot->get_plotter());
        lg->set_drawLine(true);
        lg->set_xColumn(xColumn);
        lg->set_yColumn(yColumns.at(i));
        lg->set_color(colors.at(i));
        lg->set_title(titles.at(i));
        plot->addGraph(lg);
    }

    plot->zoomToFit();

}

void BurstView::plotMarkerOld()
{
    JKQtPlotter * plot = ui->widPlotBurst;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();

    int row = ui->tableBursts->selectedRanges().first().topRow();
    BurstDual burst = ad->burstVectorDual.at(row);

    //    QVector<double> markerDex;
    //    int markerIdx=0;
    //    while(ad->markerTimeDexc.value(markerIdx,burst.startTime)<burst.startTime) {markerIdx++;}
    //    qDebug()<<"linear search first" << ad->markerTimeDexc.at(markerIdx);
    //    markerIdx--;
    //    while(((markerIdx+1)<ad->markerTimeDexc.size()) && (ad->markerTimeDexc.at(markerIdx)<(burst.startTime+burst.duration)) ) {
    //        markerDex.append(ad->markerTimeDexc.at(markerIdx)-burst.startTime);
    //        markerEvents.append(ad->markerTimeDexc.at(markerIdx));
    //        markerEvents.append(ad->markerTimeAexc.at(markerIdx));
    //        markerIdx++;
    //    }
    //    markerIdx++;
    //    markerEvents.append(ad->markerTimeDexc.at(markerIdx));
    //    markerEvents.append(ad->markerTimeAexc.at(markerIdx));
    //    qDebug()<<"linear search last" << ad->markerTimeDexc.at(markerIdx);

    //    QVector<double> markerAex;
    //    markerIdx=0;
    //    while(ad->markerTimeAexc.value(markerIdx,burst.startTime)<burst.startTime) {markerIdx++;}
    //    while(ad->markerTimeAexc.at(markerIdx)<burst.startTime+burst.duration) {
    //        markerAex.append(ad->markerTimeAexc.at(markerIdx));
    //        markerIdx++;
    //    }


    // plot marker
    QVector<double> markerDex;
    int markerIdx=0;
    QVector<double>::iterator startMarker = qLowerBound(ad->markerTimeDexc.begin(),ad->markerTimeDexc.end(),burst.startTime);
    qDebug()<<"binary search" << *startMarker; // seems to work!
    while(ad->markerTimeDexc.value(markerIdx,burst.startTime)<burst.startTime) {markerIdx++;}
    qDebug()<<"linear search" << ad->markerTimeDexc.at(markerIdx);
    while(ad->markerTimeDexc.at(markerIdx)<burst.startTime+burst.duration) {
        markerDex.append(ad->markerTimeDexc.at(markerIdx)-burst.startTime);
        markerIdx++;
    }
    QVector<double> markerAex;
    markerIdx=0;
    while(ad->markerTimeAexc.value(markerIdx,burst.startTime)<burst.startTime) {markerIdx++;}
    while(ad->markerTimeAexc.at(markerIdx)<burst.startTime+burst.duration) {
        markerAex.append(ad->markerTimeAexc.at(markerIdx)-burst.startTime);
        markerIdx++;
    }

    QVector<size_t> markerColumns;
    markerColumns.push_back(ds->addCopiedColumn(markerDex.data(), markerDex.size(), "Donor marker"));
    markerColumns.push_back(ds->addCopiedColumn(markerAex.data(), markerAex.size(), "Acceptor marker"));
    QVector<double> peakHeight;
    peakHeight.push_back(-qAbs(plot->getYAxis()->getMin()));
    peakHeight.push_back(qAbs(plot->getYAxis()->getMax()));
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_DONOR));
    colors.append(QColor(COUNTRATE_COLOR_ACCEPTOR));


    for (int i=0; i<markerColumns.size(); i++) {
        JKQTPPeakStreamGraph *g = new JKQTPPeakStreamGraph(plot->get_plotter());
        g->set_baseline(0);
        g->set_peakHeight(peakHeight[i]);
        qDebug()<<"peakHeight"<<peakHeight[i];
        g->set_color(colors[i]);
        g->set_dataColumn(markerColumns.at(i));
        plot->addGraph(g);
        g->set_lineWidth(1);
    }

}




JKQTPbarHorizontalGraphEdges::JKQTPbarHorizontalGraphEdges(JKQtBasePlotter *parent):
    JKQTPbarHorizontalGraph(parent)
{
    color=QColor("black");
    fillColor=QColor("red");
    style=Qt::SolidLine;
    lineWidth=1;
    fillStyle=Qt::SolidPattern;
    width=0.9;
    shift=0;
    baseline=0;


    if (parent) { // get style settings from parent object
        parentPlotStyle=parent->getNextStyle();
        fillColor=parent->getPlotStyle(parentPlotStyle).color();
    }

}

void JKQTPbarHorizontalGraphEdges::draw(JKQTPEnhancedPainter &painter)
{
    if (parent==NULL) return;
    JKQTPdatastore* datastore=parent->getDatastore();
    if (datastore==NULL) return;
    if (!JKQTPIsOKFloat(baseline)) return;

    drawErrorsBefore(painter);

    QPen p=painter.pen();
    p.setColor(color);
    p.setWidthF(qMax(JKQTPLOTTER_ABS_MIN_LINEWIDTH, lineWidth*parent->get_lineWidthMultiplier()));
    p.setStyle(style);
    p.setJoinStyle(Qt::RoundJoin);

    QBrush b=painter.brush();
    b.setColor(fillColor);
    b.setStyle(fillStyle);

    int imax=qMin(datastore->getColumn(xColumn).getRows(), datastore->getColumn(yColumn).getRows());
    if(datastore->getColumn(xColumn).getRows()>datastore->getColumn(yColumn).getRows()) imax++;
    int imin=0;
    // interpret data ranges
    if (datarange_start>-1) {
        imin=mmin(datarange_start, imax);
    }
    if (datarange_end>-1) {
        imax=mmin(datarange_end+1, imax);
    }
    if (imax<imin) {
        int h=imin;
        imin=imax;
        imax=h;
    }
    if (imin<0) imin=0;
    if (imax<0) imax=0;
    painter.save();

    double x0=parent->x2p(0);
    if (parent->getXAxis()->get_logAxis()) x0=parent->x2p(parent->getXAxis()->getMin());
    double y0=parent->y2p(0+baseline);
    if (parent->getYAxis()->get_logAxis()) y0=parent->y2p(parent->getYAxis()->getMin()+baseline);

    for (int i=imin; i<imax-1; i++) {
        double xv,yv,xxv;
        xv = datastore->get(xColumn,i);
        if(imin==imax) {// only one x-value
            xxv=xv+1;
        } else {
            xxv=datastore->get(xColumn,i+1);
        }
        yv = datastore->get(yColumn,i);

        if (JKQTPIsOKFloat(xv) && JKQTPIsOKFloat(yv) && JKQTPIsOKFloat(xxv)) {
            double x=parent->x2p(xv+shift);
            double y=parent->y2p(yv+baseline);
            double xx=parent->x2p(xv+shift+(xxv-xv)*width);
            double yy=y0;
            if (yy<y) { yy=y; y=y0; }
//            qDebug()<<"x="<<x<<"xx="<<xx<<"y="<<y<<"yy="<<yy;
            painter.setBrush(b);
            painter.setPen(p);
            QRectF r(QPointF(x, y), QPointF(xx, yy));
            painter.drawRect(r);
        }
    }

    painter.restore();
    drawErrorsAfter(painter);
}




bool JKQTPbarHorizontalGraphEdges::getXMinMax(double& minx, double& maxx, double& smallestGreaterZero) {
    bool start=true;
    minx=0;
    maxx=0;
    smallestGreaterZero=0;

    if (parent==NULL) return false;

    JKQTPdatastore* datastore=parent->getDatastore();
    int imin=0;
    int imax=qMin(datastore->getColumn(xColumn).getRows(), datastore->getColumn(yColumn).getRows());
    if(datastore->getColumn(xColumn).getRows()>datastore->getColumn(yColumn).getRows()) imax++;
    // interpret data ranges
    if (datarange_start>-1) {
        imin=mmin(datarange_start, imax);
    }
    if (datarange_end>-1) {
        imax=mmin(datarange_end, imax);
    }
    if (imax<imin) {
        int h=imin;
        imin=imax;
        imax=h;
    }
    if (imin<0) imin=0;
    if (imax<0) imax=0;

//    qDebug()<<imin<<imax;

    double xv=0;
    for (int i=imin; i<imax; i++) {
        if((i==imax-1) && ((i-1)>=imin)) xv=datastore->get(xColumn,i-1) + shift + (datastore->get(xColumn,i)-datastore->get(xColumn,i-1))*width;
        else xv=datastore->get(xColumn,i)+shift;
//        qDebug()<<"  xv("<<i<<")="<<xv<<"        => maxx="<<maxx<<"minx="<<minx;
        if (start || xv>maxx) maxx=xv;
        if (start || xv<minx) minx=xv;
        start=false;
        double xvsgz;
        xvsgz=xv; SmallestGreaterZeroCompare_xvsgz();
//        qDebug()<<"  xv("<<i<<")="<<xv<<"        => maxx="<<maxx<<"minx="<<minx;
    }
//    qDebug()<<"maxx="<<maxx<<"minx="<<minx<<"  smallestGreaterZero="<<smallestGreaterZero;
    return !start;
}




bool JKQTPbarHorizontalGraphEdges::getYMinMax(double& miny, double& maxy, double& smallestGreaterZero) {
    bool start=true;
    miny=0;
    maxy=0;
    smallestGreaterZero=0;

    if (parent==NULL) return false;

    JKQTPdatastore* datastore=parent->getDatastore();
    int imin=0;
    int imax=qMin(datastore->getColumn(xColumn).getRows(), datastore->getColumn(yColumn).getRows());
    if(datastore->getColumn(xColumn).getRows()>datastore->getColumn(yColumn).getRows()) imax++;
    // interpret data ranges
    if (datarange_start>-1) {
        imin=mmin(datarange_start, imax);
    }
    if (datarange_end>-1) {
        imax=mmin(datarange_end, imax);
    }
    if (imax<imin) {
        int h=imin;
        imin=imax;
        imax=h;
    }
    if (imin<0) imin=0;
    if (imax<0) imax=0;

    for (int i=imin; i<imax-1; i++) {
        double yv=datastore->get(yColumn,i)+baseline;
        if (start || yv>maxy) maxy=yv;
        if (start || yv<miny) miny=yv;
        start=false;
        double xvsgz;
        xvsgz=yv; SmallestGreaterZeroCompare_xvsgz();
    }
    if (miny>0) miny=0;
    if (maxy<0) maxy=0;

    return !start;
}

