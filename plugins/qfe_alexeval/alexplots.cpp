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

#include "alexplots.h"
#include<QDebug>
#include<gsl/gsl_histogram2d.h>



void plotHist(JKQtPlotter * plot, gsl_histogram * hist )
{
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
//    if (plot->get_plotter()->get_plotLabel().isEmpty()) plot->get_plotter()->set_plotLabel("histogram");
    if(plot->get_plotter()->getYAxis()->get_axisLabel().isEmpty()) plot->get_plotter()->getYAxis()->set_axisLabel("frequency");

    qDebug("plotHist");
    unsigned long long nrows=hist->n;
    size_t xColumn=ds->addCopiedColumn(hist->range, nrows, "x"); // adds column (copies values!) and returns column ID
    size_t yColumn=ds->addCopiedColumn(hist->bin, nrows, "frequency");
    // plot
    JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(plot->get_plotter());
//    g->set_title("histogram");
    g->set_xColumn(xColumn);
    g->set_yColumn(yColumn);
    g->set_shift(0);
    g->set_width(1);
    g->set_style(Qt::NoPen);
    plot->addGraph(g);

    plot->get_plotter()->zoomToFit();
}


void plotAlternation(JKQtPlotter *plot, const AlexData * const ad)
{
    // simple estimate of alternation times
    double t1=ad->getExcitationDurationDonor();
    double t3=ad->getExcitationDurationAcceptor();
    qDebug()<<"donor period T1="<<t1*1e6<<"us\tacceptor period T3="<<t3*1e6<<"us";

    double binwidth=5e-6, alternationPeriod=(t1+t3);
    int nbins=(int)(alternationPeriod/binwidth+1);
    gsl_histogram * histDonor = gsl_histogram_alloc (nbins); // donor emission channel
    gsl_histogram * histAcceptor = gsl_histogram_alloc (nbins); // acceptor emission channel
    gsl_histogram_set_ranges_uniform (histDonor, 0, alternationPeriod*1.1);
    gsl_histogram_set_ranges_uniform (histAcceptor, 0, alternationPeriod*1.1);
    int last=0;
    int lastExc=0;
    while(ad->photons.at(last).time<ad->markerTimeDexc.at(lastExc)) ++last;
    while((lastExc<1e4)&&(lastExc<ad->markerTimeDexc.size()+1)) { // average first 10000 periods

        while ((last<ad->photons.size()) && ad->photons[last].time<ad->markerTimeDexc.at(lastExc+1)) {
#ifdef PHOTONMACRO
            if(isDem(ad->photons[last])) gsl_histogram_increment (histDonor, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
            if(isAem(ad->photons[last])) gsl_histogram_increment (histAcceptor, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
#else
            if(ad->photons[last].isDem()) gsl_histogram_increment (histDonor, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
            if(ad->photons[last].isAem()) gsl_histogram_increment (histAcceptor, ad->photons[last].time-ad->markerTimeDexc[lastExc]);
#endif
//            qDebug()<<ad->photons[last].time-ad->markerTimeDexc[lastExc];
            last++;
        }
        // end of excitation period
        lastExc++;
    }
#ifdef MYDEBUG
    qDebug("photons in the first %.2fms. Start with donor excitation period\nt in us\tD_em\tA_em\tsum",ad->markerTimeAexc[lastAexc]*1e3);
    for (size_t i=0; i<histDonor->n; i++) {
        qDebug("%lg\t%lg\t%lg\t%lg", histDonor->range[i]*1e6, histDonor->bin[i],histAcceptor->bin[i],histDonor->bin[i]+histAcceptor->bin[i]);
    }
    qDebug()<<"photons:\t"<<lastDem<<"\t"<<lastAem<<"\t"<<lastAem+lastDem;
#endif

    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    plot->get_plotter()->setGrid(false);

    qDebug("plotHist DA");
    unsigned long long nrows=(unsigned long long)histDonor->n;
    for(size_t i=0;i<histDonor->n;++i) histDonor->range[i] *= 1e6;
    plot->get_xAxis()->set_axisLabel("time in us");
    plot->get_yAxis()->set_axisLabel("countrate in Hz");
    gsl_histogram_scale(histDonor,(1.0)/(lastExc*binwidth)); // convert bins to countrate in Hz (counts averaged over lastDex excitation periods in bin of width binwidth)
    gsl_histogram_scale(histAcceptor,(1.0)/(lastExc*binwidth));

    size_t xColumn=ds->addCopiedColumn(histDonor->range, nrows, "time"); // adds column (copies values!) and returns column ID
    QVector<size_t> yColumns;
    yColumns.push_back(ds->addCopiedColumn(histDonor->bin, nrows, "Donor"));
    yColumns.push_back(ds->addCopiedColumn(histAcceptor->bin, nrows, "Acceptor"));
    QList<QColor> colors;
    colors.append(QColor(COUNTRATE_COLOR_DONOR));
    colors.append(QColor(COUNTRATE_COLOR_ACCEPTOR));
    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);

    // plot
    double width=0.5;
    double s=-0.5+width/2.0;
    for (int i=0; i<yColumns.size(); ++i) {
        JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(plot->get_plotter());
        g->set_xColumn(xColumn);
        g->set_yColumn(yColumns[i]);
        g->set_shift(s);
        g->set_width(width);
        g->set_style(Qt::NoPen);
        g->set_fillColor(colors.at(i));
        s=s+width;
        plot->addGraph(g);
    }
    plot->zoomToFit();


}


void plotDensity(JKQtPlotter * plot, const AlexData* const ad, const int nbins)
{
    // density plot
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    plot->get_plotter()->clearGraphs(true);
    plot->setGrid(false);
    plot->get_plotter()->set_plotLabel("ALEX density plot");
    plot->get_plotter()->getXAxis()->set_axisLabel("PR");
    plot->get_plotter()->getYAxis()->set_axisLabel("S");
    gsl_histogram2d * histAlex=gsl_histogram2d_calloc_uniform(nbins,nbins,-0.1,1.1,-0.1,1.1);
    for(int i=0; i<ad->burstVectorDual.size();++i)
        gsl_histogram2d_increment(histAlex, ad->burstVectorDual.at(i).proximityRatio, ad->burstVectorDual.at(i).stoichiometryRatio);

    size_t colHist2D=ds->addCopiedImageAsColumnTranspose(histAlex->bin, histAlex->ny, histAlex->nx, "hist2D");
    ds->getColumn(colHist2D).exchange(0, 1.0/0.0);

    JKQTPColumnMathImage* densityplot=new JKQTPColumnMathImage(plot->get_plotter()); // bin(i,j) = bin[i * ny + j].
    densityplot->set_x(-0.1);
    densityplot->set_y(-0.1);
    densityplot->set_width(1.2);
    densityplot->set_height(1.2);
    densityplot->set_Nx(histAlex->nx);
    densityplot->set_Ny(histAlex->ny);
    densityplot->set_palette(JKQTPMathImageHOT);
    densityplot->set_imageColumn(colHist2D);
    densityplot->set_imageName("#bursts");
    densityplot->set_infColor(Qt::transparent);
    densityplot->set_nanColor(Qt::transparent);
    plot->addGraph(densityplot);
    gsl_histogram2d_free(histAlex);
//    plot->get_plotter()->saveAsPixelImage(ad->currentDir + "/density.png");
}



QComboBoxChannel::QComboBoxChannel(QWidget *parent) : QComboBox(parent)
{
    addItem("Dem");
    addItem("Aem");
    addItem("Aex");
    addItem("Dex");
    addItem("Dem_Dex");
    addItem("Aem_Dex");
    addItem("Dem_Aex");
    addItem("Aem_Aex");
    addItem("All"); // FIXME
//    addItem("Dem+Aem_Aex");
//    addItem("Dex+Aem_Aex");
}

uint QComboBoxChannel::channel() const
{
    uint ch=0;
    switch(currentIndex()) {
    case 0:
        ch=DonorEm;
        break;
    case 1:
        ch=AcceptorEm;
        break;
    case 2:
        ch=AcceptorEx;
        break;
    case 3:
        ch=DonorEx;
        break;
    case 4:
        ch=DonorEm|DonorEx;
        break;
    case 5:
        ch=AcceptorEm|DonorEx;
        break;
    case 6:
        ch=DonorEm|AcceptorEx;
        break;
    case 7:
        ch=AcceptorEm|AcceptorEx;
        break;
    case 8:
        ch=0; // FIXME
        break;
    case 9:
        ch=DonorEm|AcceptorEx; // FIXME
        break;
    case 10:
        ch=DonorEx|AcceptorEx; // FIXME
        break;
    default:
        AlexEvalLog::warning("unknown channel selection");
        break;
    }
    return ch;
}

