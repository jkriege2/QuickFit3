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

#include "photonswidget.h"
#include "ui_photonswidget.h"
#include "readArrivalTimes.h"
#include<QVector>
#include "alexplots.h"
#include<QDebug>
#include"jkqtpelements.h"
#include <QMessageBox>


PhotonsWidget::PhotonsWidget(QWidget *parent, AlexData* data) :
    QWidget(parent),
    ui(new Ui::PhotonsWidget)
{
    ui->setupUi(this);

    ui->widgetIntesityTotal->hide(); // FIXME

    if(data==NULL) AlexEvalLog::warning("data not initialized");
    ad=data;

    ui->splitter->setChildrenCollapsible(false);
    ui->splitter->setStretchFactor(0,3);
    ui->splitter->setStretchFactor(1,1);

    ui->lineEditTTTRfile->setText(ad->TTTRfilename);

    ui->lineEditTTTRfile->addButton(edt=new JKStyledButton(JKStyledButton::SelectFile, ui->lineEditTTTRfile, ui->lineEditTTTRfile));
    edt->setFilter(tr("Supported Files(*.t3r *.txt *.csv);;PicoQuant TTTR files (*.t3r);;ASCII Files (*.csv *.txt);;All Files (*.*)"));
    edt->setInitPath(ad->currentDir);

    plot=ui->widgetIntensityPlot;
//    connect(ui->checkBoxLogscale, SIGNAL(toggled(bool)), plot->get_plotter()->getYAxis(), SLOT(set_logAxis(bool)));
    ui->checkBoxLogscale->setDisabled(true);
    connect(ui->pushButtonRead,SIGNAL(clicked()),this,SLOT(readT3Rfile()));
    connect(ui->doubleSpinBoxRangeStart,SIGNAL(valueChanged(double)),this,SLOT(getRangeStart(double)));
    connect(ui->doubleSpinBoxRangeStop,SIGNAL(valueChanged(double)),this,SLOT(getRangeStop(double)));
    connect(ui->pushButtonPlot,SIGNAL(clicked()),this,SLOT(plotIntensity()));
    connect(ui->pushButtonBackground,SIGNAL(clicked()),this,SLOT(backgroundAnalysis()));

}

PhotonsWidget::~PhotonsWidget()
{
    delete ui;
}

void PhotonsWidget::storeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"route", ui->comboBoxChannel->currentIndex());
    settings.setValue(prefix+"pin", ui->comboBoxPin->currentIndex());
    settings.setValue(prefix+"binwidth", ui->spinBoxBinwidth->value());
    settings.setValue(prefix+"visible_range", ui->doubleSpinBoxRange->value());
    settings.setValue(prefix+"splitterGeometry",ui->splitter->saveState());
    settings.setValue(prefix+"periodReconstruct",periodReconstruct);
    settings.setValue(prefix+"offsetReconstruct",offsetReconstruct);
}

void PhotonsWidget::loadSettings(QSettings &settings, const QString &prefix)
{
    ui->comboBoxChannel->setCurrentIndex(settings.value(prefix+"route_Dem", ui->comboBoxChannel->currentIndex()).toInt());
    ui->comboBoxPin->setCurrentIndex(settings.value(prefix+"pin_Dem", ui->comboBoxPin->currentIndex()).toInt());
    ui->spinBoxBinwidth->setValue(settings.value(prefix+"binwidth", ui->spinBoxBinwidth->value()).toInt());
    ui->doubleSpinBoxRange->setValue(settings.value(prefix+"visible_range", ui->doubleSpinBoxRange->value()).toDouble());
    periodReconstruct=settings.value(prefix+"periodReconstruct", 100e-6).toDouble();
    offsetReconstruct=settings.value(prefix+"offsetReconstruct", 100e-6).toDouble();
    ui->splitter->restoreState(settings.value(prefix+"splitterGeometry").toByteArray());
}

void PhotonsWidget::automatedRun(const QString &filename)
{
    readT3Rfile(filename);
    QString prefix=ad->currentDir+"/"+ad->name+"_";
    ui->widgetPlotAlternation->saveImage(prefix+"excitationPeriod",false);

}


void PhotonsWidget::plotIntensity()
{ 
    if(ad->isEmpty()) readT3Rfile();
    if(ad->isEmpty()) {AlexEvalLog::warning("no TTTR data"); return;}
    qDebug("plotIntensity");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    plot->set_doDrawing(false);

    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    ds->clear();
    plot->get_plotter()->clearGraphs(true);

    plot->setObjectName("timetrace");
    plot->setGrid(false);
    plot->get_plotter()->getXAxis()->set_axisLabel("time in s"); // $x$ axis with greek letters $\\sqrt{\\alpha\\cdot\\beta}$
    plot->get_plotter()->getYAxis()->set_axisLabel("countrate in kHz");

    // calculate binned intensity
    binwidth=ui->spinBoxBinwidth->value()*1e-3; // to sec
    int nbins=(int)(ad->measurementDuration()/binwidth);
    qDebug()<<"nbins="<<nbins;
    gsl_histogram * histDonor=gsl_histogram_calloc(nbins);
    gsl_histogram * histAcceptor=gsl_histogram_calloc(nbins);
    gsl_histogram_set_ranges_uniform (histDonor, ad->start, ad->stop);
    gsl_histogram_set_ranges_uniform (histAcceptor, ad->start, ad->stop);
    for (int ii=0;ii<ad->photons.size();++ii) {
#ifdef PHOTONMACRO
        if(isDem(ad->photons.at(ii))) gsl_histogram_increment (histDonor, ad->photons.at(ii).time);
        if(isAem(ad->photons.at(ii))) gsl_histogram_increment (histAcceptor, ad->photons.at(ii).time);
#else
        if(ad->photons.at(ii).isDem()) gsl_histogram_increment (histDonor, ad->photons.at(ii).time);
        if(ad->photons.at(ii).isAem()) gsl_histogram_increment (histAcceptor, ad->photons.at(ii).time);
#endif
    }

    gsl_histogram_scale(histDonor,(1e-3)/binwidth); // convert bins to burstrate in kHz
    gsl_histogram_scale(histAcceptor,(-1e-3)/binwidth);

    // store data in plot
    plot->get_plotter()->setGrid(false);
    unsigned long long nrows=(unsigned long long) histDonor->n;
    size_t xColumn=ds->addCopiedColumn(histDonor->range, nrows, "time"); // adds column (copies values!) and returns column ID
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
    double width=1.0/(double)yColumns.size();
    double s=-1.0/2.0+width/2.0;
    for (int i=0; i<yColumns.size(); i++) {
        JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(plot->get_plotter());
        g->set_title(titles[i]);
        g->set_xColumn(xColumn);
        g->set_yColumn(yColumns[i]);
        g->set_shift(s);
        g->set_width(width);
        g->set_style(Qt::NoPen);
        g->set_fillColor(colors.at(i));
        s=s+width;
        g->set_datarange_start(0);
        g->set_datarange_end(ui->doubleSpinBoxRange->value()/binwidth);
        plot->addGraph(g);
    }
    // connect to slider
    disconnect(ui->scrollPhotons, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    ui->scrollPhotons->setMaximum(2*(ui->doubleSpinBoxRangeStop->value()-ui->doubleSpinBoxRangeStart->value())/ui->doubleSpinBoxRange->value());
//    ui->scrollPhotons->setMinimum(0);
    ui->scrollPhotons->setValue(0);
    connect(ui->scrollPhotons, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));

    plot->get_plotter()->set_keyPosition(JKQTPkeyOutsideTopRight); // set legend position
    plot->zoom(ui->doubleSpinBoxRangeStart->value(),ui->doubleSpinBoxRangeStart->value() + ui->doubleSpinBoxRange->value(),-50,50);
    plot->set_doDrawing(true);
    plot->update_plot(); // draw the plot

//    qDebug("comparison of QVector methods pop_back and remove");
//    int n = ad->markerTimeAexc.size();
//    while(ad->markerTimeAexc.last()>0.1) ad->markerTimeAexc.pop_back();
//    qDebug("start remove");
//    while(ad->markerTimeDexc.last()>0.1) ad->markerTimeDexc.remove(ad->markerTimeDexc.size()-1);
//    qDebug()<<ad->markerTimeAexc.size();
//    qDebug("finished");

    QApplication::restoreOverrideCursor();
}

void PhotonsWidget::getRangeStart(double startTime)
{
    ad->start=startTime;
    ui->doubleSpinBoxRangeStop->setMinimum(startTime+ui->doubleSpinBoxRange->singleStep());
}

void PhotonsWidget::getRangeStop(double endTime)
{
    ad->stop=endTime;
    ui->doubleSpinBoxRangeStart->setMaximum(endTime-ui->doubleSpinBoxRange->singleStep());
}



void PhotonsWidget::readT3Rfile(const QString file)
{
    QString inFileName;
    if(!file.isEmpty()) inFileName=file;
    else {
        inFileName =ui->lineEditTTTRfile->text();
        if(inFileName.isEmpty()) {
            AlexEvalLog::warning("filename empty");
            emit edt->click();
            //QCoreApplication::processEvents();
            inFileName=ui->lineEditTTTRfile->text();
            if(inFileName.isEmpty()) return;
        }
    }
    // empty datastore and grid
    ui->widgetPlotAlternation->get_plotter()->getDatastore()->clear();
    ui->widgetPlotAlternation->get_plotter()->clearGraphs(true);

    ad->clear();
    ui->lineEditTTTRfile->setText(inFileName);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ad->setName(inFileName);
    this->parentWidget()->setWindowTitle("ALEXEval: "+inFileName);
    AlexEvalLog::text(ad->TTTRfilename+"\t"+ad->iniFilename+"\t"+ad->name);

    if(inFileName.endsWith(".csv")||inFileName.endsWith(".txt")) {
        QList<QVector<double> > data;
        data.append(ad->arrivalTimeCh1);
        data.append(ad->arrivalTimeCh2);
        readCSVtoList(data, inFileName);
        Photon p;
        for(int i=0;i<data.first().size();++i) {
            p.time=data.at(0).at(i);
            p.flags=(unsigned char) QString::number(data.at(1).at(i)).toInt(NULL,2);
//            qDebug()<<p.flags;
            ad->photons.append(p);
        }
        ad->start=qMax(0.0,ad->photons.first().time-1e-6);
        ad->stop = qMin(ad->max_measurementtime,ad->photons.last().time+3e-6);
        qDebug()<<ad->start<<"\t"<<ad->stop;
//        ui->doubleSpinBoxRangeStop->setMaximum(ad->stop);
//        ui->doubleSpinBoxRangeStop->setValue(ad->stop);
//        ui->doubleSpinBoxRangeStart->setMinimum(ad->start);
//        ui->doubleSpinBoxRangeStart->setValue(ad->start);

//        ad->markerTimeDexc.append(ad->start);
//        ad->markerTimeAexc.append(ad->stop-2e-6);
//        ad->markerTimeDexc.append(ad->stop-1e-6);
//        ad->markerTimeAexc.append(ad->stop);
//        QApplication::restoreOverrideCursor();
//        return;
        if((ad->photons.isEmpty())) {
            AlexEvalLog::warning("Error reading file " + inFileName);
            QApplication::restoreOverrideCursor();
            return;
        }
    } else if (inFileName.endsWith(".t3r")) { /// .t3r file
        Route0=(ChannelMappings) ui->comboBoxChannel->currentIndex();
        Pin4=(ChannelMappings)ui->comboBoxPin->currentIndex();
        AlexEvalLog::text("Route0="+QString::number(Route0)+"Pin4="+QString::number(Pin4));
        // assign channels and read in TTTR file
        int res;
        if        (Route0==Acceptor&&Pin4==Acceptor) {
            res=readTTTRfile(ad->photons,ad->markerTimeAexc,ad->markerTimeDexc,ad->TTTRfilename,AcceptorEm,AcceptorEx);
        } else if (Route0==Acceptor&&Pin4==Donor) {
            res=readTTTRfile(ad->photons,ad->markerTimeDexc,ad->markerTimeAexc,ad->TTTRfilename,AcceptorEm,DonorEx);
        } else if (Route0==Donor&&Pin4==Acceptor) {
            res=readTTTRfile(ad->photons,ad->markerTimeAexc,ad->markerTimeDexc,ad->TTTRfilename,DonorEm,AcceptorEx);
        } else if (Route0==Donor&&Pin4==Donor) {
            res=readTTTRfile(ad->photons,ad->markerTimeDexc,ad->markerTimeAexc,ad->TTTRfilename,DonorEm,DonorEx);
        } else {
            AlexEvalLog::warning("invalid channel assignment");
            QApplication::restoreOverrideCursor();
            return;
        }

        if(res||(ad->photons.isEmpty())) {
            AlexEvalLog::warning("Error reading t3r file");
            QApplication::restoreOverrideCursor();
            return;
        }
    } else {
        AlexEvalLog::warning("unknown file format");
        QApplication::restoreOverrideCursor();
        return;

    }

    QString result; result=result.sprintf(
                "found %i  photons",
                ad->photons.size());
    AlexEvalLog::text(result);

    ad->stop = qMax(ad->photons.last().time,ad->photons.last().time); // CHECK THIS
    ad->start= qMin(ad->photons.first().time,ad->photons.first().time);
    ui->doubleSpinBoxRangeStop->setMaximum(ad->stop);
    ui->doubleSpinBoxRangeStop->setValue(ad->stop);
    ui->doubleSpinBoxRangeStart->setMinimum(ad->start);
    ui->doubleSpinBoxRangeStart->setValue(ad->start);


    // there should be approximately the same number of marker in each channel.
    // A 20% deviation is allowed and missing markers may be filled in later. If the deviation is larger
    // than 20%, the user is informed and presented a dialog to choose if he wants to reconstruct
    // the marker events or if the .t3r file is a FRET measurement with single wavelenght excitation
    if(ad->markerTimeAexc.isEmpty()||ad->markerTimeAexc.size()>ad->markerTimeDexc.size()*1.2||
            ad->markerTimeDexc.size()>ad->markerTimeAexc.size()*1.2) {
        if(ad->markerTimeDexc.size()>1) {
            periodReconstruct=ad->getExcitationPeriodDonor();
        } else if(ad->markerTimeAexc.size()>1) {
            periodReconstruct=ad->getExcitationPeriodAcceptor();
        }

        QApplication::restoreOverrideCursor();

        DialogReconstructMarkerRecords* dlg = new DialogReconstructMarkerRecords(this);
        dlg->setPeriod(periodReconstruct);
        dlg->setOffset(offsetReconstruct);
        dlg->setInfoText(ad->markerTimeDexc.size(),ad->markerTimeAexc.size());
        dlg->exec();
        if(dlg->result()==QDialog::Rejected) {
            ad->clear();
            return;
        }

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        method = dlg->getChannel();
        switch (method) {
        case FullConstruction:
            periodReconstruct=dlg->getFullDonorPeriod();
            offsetReconstruct=dlg->getFullOffset();
            constructMarkerEvents(ad->markerTimeDexc,ad->markerTimeAexc,periodReconstruct,offsetReconstruct,ad->stop);
            break;
        case SingleWavelengthFRET:
            ad->markerTimeAexc.clear();
            ad->markerTimeDexc.clear();
            ad->start=ad->photons.first().time-1e-6;
            ad->stop = ad->photons.last().time+3e-6;
            ad->markerTimeDexc.append(ad->start);
            ad->markerTimeAexc.append(ad->stop-2e-6);
            ad->markerTimeDexc.append(ad->stop-1e-6);
            ad->markerTimeAexc.append(ad->stop);
            ui->widgetPlotAlternation->get_plotter()->getDatastore()->clear();
            ui->widgetPlotAlternation->get_plotter()->clearGraphs(true);
            ad->method=AlexData::swFRET;
            updateInfo(false);
            checkAlternationPeriods(ad->photons, ad->markerTimeDexc, ad->markerTimeAexc);
            QApplication::restoreOverrideCursor();
            return;
        case Donor:
            checkMarker(ad->markerTimeDexc,dlg->getPeriod());
            estimateMarkerEvents(ad->markerTimeAexc, ad->markerTimeDexc, dlg->getOffset());
            break;
        case Acceptor:
            checkMarker(ad->markerTimeAexc,dlg->getPeriod());
            estimateMarkerEvents(ad->markerTimeDexc, ad->markerTimeAexc, dlg->getOffset());
        default:
            AlexEvalLog::warning("unknown selection for reconstruction");
        }
        ad->method=AlexData::ALEX;
        qDebug("period: %.11f",dlg->getPeriod());
        qDebug("offset: %.11f",dlg->getOffset());

//        for(int i=0;i<20;++i) {
//            qDebug("%f\t%f",ad->markerTimeDexc.at(i)*1e6,ad->markerTimeAexc.at(i)*1e6);
//        }

//        setRange(ad->arrivalTimeDonor,ad->markerTimeDexc.first(),ad->markerTimeAexc.last());
//        setRange(ad->arrivalTimeAcceptor,ad->markerTimeDexc.first(),ad->markerTimeAexc.last());

    } else {
        ad->method=AlexData::ALEX;
    }
    checkAlternationPeriods(ad->photons, ad->markerTimeDexc, ad->markerTimeAexc);

    updateInfo(true);

    plotAlternation(ui->widgetPlotAlternation,ad);

    QApplication::restoreOverrideCursor();
}



void PhotonsWidget::backgroundAnalysis()
{
    if(ad->isEmpty()) readT3Rfile();
    if(ad->isEmpty()) {AlexEvalLog::warning("no TTTR data"); return;}
    qDebug("backgroundAnalysis");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ad->setRange();

    if(background(ad->background,ad->photons)==EXIT_FAILURE) AlexEvalLog::warning("error backgroundAnalysis");

    double t_Dex=ad->getExcitationPeriodDonor();
    double t_Aex=ad->getExcitationPeriodAcceptor();
    QString infoText="Background countrates\ntotal=%1 kHz\nDem_Dex=%2 kHz\nAem_Dex=%3 kHz\nDem_Aex=%4 kHz\nAem_Aex=%5 kHz\n\nnumber of photons\ndonor\t%6\nacceptor\t%7\n\n#donor marker=%8\n#acceptor marker=%9\nDex period=%10us\nDex duration=%11us\nAex duration=%12us";
    infoText=infoText.arg(ad->background.burstrate()*1e-3,0,'f',3);
//    infoText=infoText.arg((ad->background.n_Dem_Dex/ad->measurementDuration())*1e-3,0,'f',3); // FIXME: maybe take only the respective excitation lengths
//    infoText=infoText.arg((ad->background.n_Aem_Dex/ad->measurementDuration())*1e-3,0,'f',3);
//    infoText=infoText.arg((ad->background.n_Dem_Aex/ad->measurementDuration())*1e-3,0,'f',3);
//    infoText=infoText.arg((ad->background.n_Aem_Aex/ad->measurementDuration())*1e-3,0,'f',3);
    if(ad->calculateBackground()==EXIT_FAILURE) AlexEvalLog::warning("PhotonsWidget: error calculateBackground");
    infoText=infoText.arg((ad->FRETparams.bg_Dem_Dex)*1e-3,0,'f',3); // take only the respective excitation lengths
    infoText=infoText.arg((ad->FRETparams.bg_Aem_Dex)*1e-3,0,'f',3);
    infoText=infoText.arg((ad->FRETparams.bg_Dem_Aex)*1e-3,0,'f',3);
    infoText=infoText.arg((ad->FRETparams.bg_Aem_Aex)*1e-3,0,'f',3);
    infoText=infoText.arg(ad->arrivalTimeCh1.size());
    infoText=infoText.arg(ad->arrivalTimeCh2.size());
    infoText=infoText.arg(ad->markerTimeDexc.size());
    infoText=infoText.arg(ad->markerTimeAexc.size());
    infoText=infoText.arg(t_Dex*1e6);
    infoText=infoText.arg(ad->getExcitationDurationDonor()*1e6);
    infoText=infoText.arg(ad->getExcitationDurationAcceptor()*1e6);
    ui->textBrowser->setText(infoText);

    emit backgroundCalculated();

    QApplication::restoreOverrideCursor();
}

void PhotonsWidget::updateInfo(bool hasMarker)
{
    QString infoText;
    ad->photons.countPhotons();

    if(hasMarker) {
        infoText=tr("total countrate=%1kHz\ndonor emission=%2kHz\nacceptor emission=%3kHz\n#donor photons=%4\n#acceptor photons=%5"
                "\n#donor marker=%6\n#acceptor marker=%7\n\nDex period=%10us\nDex duration=%11us\nAex duration=%12us"
                    "\nDem_Dex=%13kHz\nAem_Dex=%14kHz\nDem_Aex=%15kHz\nAem_Aex=%16kHz");
        infoText=infoText.arg((ad->photons.size())/(ad->stop-ad->markerTimeDexc.first())*1e-3,0,'f',3);
        infoText=infoText.arg(ad->photons.countrateDem()*1e-3,0,'f',3);
        infoText=infoText.arg(ad->photons.countrateAem()*1e-3,0,'f',3);
        infoText=infoText.arg(ad->photons.countrateAem()*1e-3,0,'f',3);
        infoText=infoText.arg(ad->photons.nDem());
        infoText=infoText.arg(ad->photons.nAem());
        infoText=infoText.arg(ad->markerTimeDexc.size());
        infoText=infoText.arg(ad->markerTimeAexc.size());
        infoText=infoText.arg(ad->getExcitationPeriodDonor()*1e6);
        infoText=infoText.arg(ad->getExcitationDurationDonor()*1e6);
        infoText=infoText.arg(ad->getExcitationDurationAcceptor()*1e6);
        infoText=infoText.arg(ad->photons.countrateDem_Dex()*1e-3,0,'f',4);
        infoText=infoText.arg(ad->photons.countrateAem_Dex()*1e-3,0,'f',4);
        infoText=infoText.arg(ad->photons.countrateDem_Aex()*1e-3,0,'f',4);
        infoText=infoText.arg(ad->photons.countrateAem_Aex()*1e-3,0,'f',4);
    } else {
        infoText=tr("total countrate=%1kHz\ndonor emission=%2kHz\nacceptor emission=%3kHz\n#donor photons=%4\n#acceptor photons=%5");
        infoText=infoText.arg((ad->photons.size())/(ad->stop-ad->markerTimeDexc.first())*1e-3,0,'f',3);
        infoText=infoText.arg(ad->photons.countrateDem()*1e-3,0,'f',3);
        infoText=infoText.arg(ad->photons.countrateAem()*1e-3,0,'f',3);
        infoText=infoText.arg(ad->photons.nDem());
        infoText=infoText.arg(ad->photons.nAem());
    }

    ui->textBrowser->setText(infoText);
    AlexEvalLog::text(infoText);
}

void PhotonsWidget::sliderMoved(int value)
{
    JKQtPlotter* plot=ui->widgetIntensityPlot;
    binwidth=ui->spinBoxBinwidth->value()*1e-3; // to sec
    int bins=ui->doubleSpinBoxRange->value()/binwidth;
    for(size_t i=0;i<plot->get_plotter()->getGraphCount();++i) {
        plot->get_plotter()->getGraph(i)->set_datarange_start(double(value)/2.0*bins);
        plot->get_plotter()->getGraph(i)->set_datarange_end((double(value)/2.0+1.0)*bins);
//        plot->get_plotter()->getGraph(i);
    }
    plot->zoomToFit(true,false);

}
