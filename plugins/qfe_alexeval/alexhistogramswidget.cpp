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

#include "alexhistogramswidget.h"
#include "ui_alexhistogramswidget.h"
#include "analysis.h"
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_histogram2d.h>
#include"parameters.h"
#include "jkqtptools.h"
#include <QMessageBox>
#include<QFileDialog>
#include<QDebug>

ALEXHistogramsWidget::ALEXHistogramsWidget(QWidget *parent, AlexData* data) :
    QWidget(parent),
    ui(new Ui::ALEXHistogramsWidget)
{
    ui->setupUi(this);
    if(data==NULL) qFatal("ALEXHistogramsWidget: data not initialized");
    ad=data;
    pltSelectionRect=NULL;
    relativeLevels=false;
//    plotRange.setDefault();

    actZoomRange=new QFActionWithNoMenuRole(QIcon(":/images/jkqtp_zoomall.png"), tr("zoom full histogram"), this);
    connect(actZoomRange, SIGNAL(triggered()), this, SLOT(zoomFull2DHistogram()));
    ui->widPlotDensity->addAction(actZoomRange);
    ui->widPlotDensity->updateToolbarActions();

    //ui->widPlotPrHist->synchronizeToMaster(ui->widPlotScatter, true, false);
//    connect(ui->widPlotHistS, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), ui->widPlotPrHist, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));
//    connect(ui->widPlotPrHist, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), ui->widPlotDensity, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));
    connect(ui->widPlotDensity, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), ui->widPlotHistS, SLOT(synchronizeYAxis(double,double,double,double,JKQtPlotter*)));
    connect(ui->widPlotHistS, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), ui->widPlotDensity, SLOT(synchronizeYAxis(double,double,double,double,JKQtPlotter*)));
    connect(ui->widPlotPrHist, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), ui->widPlotDensity, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));
    connect(ui->widPlotDensity, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), ui->widPlotPrHist, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));

    ui->chkSelect->setChecked(true);

    connect(ui->pushButtonSave,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->widPlotDensity,SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)),this,SLOT(selectRangeRect(double,double,double,double)));
    connect(ui->pushButtonImportBackground,SIGNAL(clicked()),this,SLOT(setBackground()));

    ui->comboBoxPlot->addItem("Contour Plot");
    ui->comboBoxPlot->addItem("t_D-t_A");
    ui->comboBoxPlot->addItem("none");

    dlgGMM = new DialogGMMparams(this);
    for(int i = 0;i<dlgGMM->getModels().size();++i) {
        ui->comboBoxModel->addItem(dlgGMM->getModels().at(i).getName());
    }
    on_pushButtonGMMreset_clicked();

    ui->doubleSpinBoxScale->setDisabled(true);

}

ALEXHistogramsWidget::~ALEXHistogramsWidget()
{
    delete ui;
}

void ALEXHistogramsWidget::storeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"doubleSpinBoxBackgroundDemDex",ui->doubleSpinBoxBackgroundDemDex->value());
    settings.setValue(prefix+"doubleSpinBoxBackgroundDemAex",ui->doubleSpinBoxBackgroundDemAex->value());
    settings.setValue(prefix+"doubleSpinBoxBackgroundAemDex",ui->doubleSpinBoxBackgroundAemDex->value());
    settings.setValue(prefix+"doubleSpinBoxBackgroundAemAex",ui->doubleSpinBoxBackgroundAemAex->value());
    settings.setValue(prefix+"doubleSpinBoxCrosstalk",ui->doubleSpinBoxCrosstalk->value());
    settings.setValue(prefix+"doubleSpinBoxDirectExc",ui->doubleSpinBoxDirectExc->value());
    settings.setValue(prefix+"doubleSpinBoxScale",ui->doubleSpinBoxScale->value());
    settings.setValue(prefix+"spinBoxBins",ui->spinBoxBins->value());
    settings.setValue(prefix+"plotRangeContour.minP",plotRangeContour.minP);
    settings.setValue(prefix+"plotRangeContour.maxP",plotRangeContour.maxP);
    settings.setValue(prefix+"plotRangeContour.minS",plotRangeContour.minS);
    settings.setValue(prefix+"plotRangeContour.maxS",plotRangeContour.maxS);
    settings.setValue(prefix+"comboBoxPalette",ui->comboBoxPalette->currentIndex());
    settings.setValue(prefix+"comboBoxPlot",ui->comboBoxPlot->currentIndex());
    settings.setValue(prefix+"comboBoxModel",ui->comboBoxModel->currentIndex());
    settings.beginWriteArray(prefix+"contourLevels");
    for (int i = 0; i < contourLevels.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("contourLevel", contourLevels.at(i));
    }
    settings.endArray();
    settings.setValue(prefix+"splitterGeometry",ui->splitter->saveState());
    settings.setValue(prefix+"splitterGeometry2",ui->splitter_2->saveState());
    settings.setValue(prefix+"splitterGeometry3",ui->splitter_3->saveState());
    settings.setValue(prefix+"splitterGeometry4",ui->splitter_4->saveState());
}

void ALEXHistogramsWidget::loadSettings(QSettings &settings, const QString &prefix)
{
    ui->splitter->restoreState(settings.value(prefix+"splitterGeometry").toByteArray());
    ui->splitter_2->restoreState(settings.value(prefix+"splitterGeometry2").toByteArray());
    ui->splitter_3->restoreState(settings.value(prefix+"splitterGeometry3").toByteArray());
    ui->splitter_4->restoreState(settings.value(prefix+"splitterGeometry4").toByteArray());
    ui->doubleSpinBoxBackgroundDemDex->setValue(settings.value(prefix+"doubleSpinBoxBackgroundDemDex",ui->doubleSpinBoxBackgroundDemDex->value()).toDouble());
    ui->doubleSpinBoxBackgroundDemAex->setValue(settings.value(prefix+"doubleSpinBoxBackgroundDemAex",ui->doubleSpinBoxBackgroundDemAex->value()).toDouble());
    ui->doubleSpinBoxBackgroundAemDex->setValue(settings.value(prefix+"doubleSpinBoxBackgroundAemDex",ui->doubleSpinBoxBackgroundAemDex->value()).toDouble());
    ui->doubleSpinBoxBackgroundAemAex->setValue(settings.value(prefix+"doubleSpinBoxBackgroundAemAex",ui->doubleSpinBoxBackgroundAemAex->value()).toDouble());
    ui->doubleSpinBoxCrosstalk->setValue(settings.value(prefix+"doubleSpinBoxCrosstalk",ui->doubleSpinBoxCrosstalk->value()).toDouble());
    ui->doubleSpinBoxDirectExc->setValue(settings.value(prefix+"doubleSpinBoxDirectExc",ui->doubleSpinBoxDirectExc->value()).toDouble());
    ui->doubleSpinBoxScale->setValue(settings.value(prefix+"doubleSpinBoxScale",ui->doubleSpinBoxScale->value()).toDouble());
    ui->spinBoxBins->setValue(settings.value(prefix+"spinBoxBins",ui->spinBoxBins->value()).toInt());
    plotRangeContour.minP=settings.value(prefix+"plotRangeContour.minP",-0.1).toDouble();
    plotRangeContour.maxP=settings.value(prefix+"plotRangeContour.maxP",+1.1).toDouble();
    plotRangeContour.minS=settings.value(prefix+"plotRangeContour.minS",-0.1).toDouble();
    plotRangeContour.maxS=settings.value(prefix+"plotRangeContour.maxS",+1.1).toDouble();
    plotRangeContour.check();
    int size = settings.beginReadArray(prefix+"contourLevels");
    contourLevels.clear();
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        contourLevels.append(settings.value("contourLevel").toDouble());
    }
    settings.endArray();
    ui->comboBoxPlot->setCurrentIndex(settings.value(prefix+"comboBoxPlot",0).toInt());
    ui->comboBoxPalette->setCurrentIndex(settings.value(prefix+"comboBoxPalette",0).toInt());
    ui->comboBoxModel->setCurrentIndex(settings.value(prefix+"comboBoxModel",0).toInt());

}

void ALEXHistogramsWidget::evaluate()
{
    on_pushButtonEval_clicked();
}

void ALEXHistogramsWidget::on_pushButtonEval_clicked()
{
    qDebug("ALEXHistogramsWidget::evaluate");
    if(ad->isEmpty()||ad->isEmptyBurst()) {AlexEvalLog::warning("no data"); return;}
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    getFRETparameters();
    int res=analysisFRET(ad->burstVectorDual,ad->FRETparams);
    if (res) AlexEvalLog::warning("Error in FRET analysis");
    writeStatistics();
    drawPlots();

    QApplication::restoreOverrideCursor();
}

void ALEXHistogramsWidget::drawPlots()
{
    if(ad->isEmptyBurst()) return;
    qDebug("ALEXHistogramsWidget::drawPlots");

    // density plot
    plotDensity();

    // pr histogram
    plotHistogramPR();

    // S histogram
    plotHistogramS();

    switch(ui->comboBoxPlot->currentIndex()) {
    case 0:
        plotContour();
        break;
    case 1:
        plotTgTr();
        break;
    default:
        AlexEvalLog::warning("Error: Plot not implemented");
        break;
    }


}

void ALEXHistogramsWidget::save()
{
    QString prefix=ad->currentDir+"/"+ad->name+"_";
    AlexEvalLog::text("save data: " + prefix);

    QMessageBox msgBox;
    msgBox.setText("It seems that the data has been saved earlier.\n");
    msgBox.setInformativeText("Do you want to overwrite the files?");
//    "Do you want to overwrite the files or store them with a different name?");
    msgBox.addButton(QMessageBox::Yes );
//    msgBox.addButton(QMessageBox::Save );
    msgBox.addButton(QMessageBox::No );
//    msgBox.addButton(QMessageBox::Cancel );

    if(ad->isEmptyBurst()) AlexEvalLog::warning("run evaluation first/no data to save");
    else {
        int ret=QMessageBox::Yes;
        if(QFile::exists(prefix+FILENAME_ALEXDATA)) {
            ret = msgBox.exec(); // ask user if he wants to overwrite the files
        }
        if(ret==QMessageBox::Yes) {
            SaveBursts(ad->burstVectorCh1,ad->burstSearchParamsCh1,prefix+FILENAME_BURSTDONOR);
            SaveBursts(ad->burstVectorCh2,ad->burstSearchParamsCh2,prefix+FILENAME_BURSTACCEPTOR);
            ad->save(prefix+FILENAME_ALEXDATA);
            ui->widPlot->saveImage(prefix+ui->comboBoxPlot->currentText()+FILE_EXTENSION_PLOT, false);
            ui->widPlotDensity->saveImage(prefix+"density"+FILE_EXTENSION_PLOT, false);
            ui->widPlotHistS->saveImage(prefix+"HistS"+FILE_EXTENSION_PLOT, false);
            ui->widPlotPrHist->saveImage(prefix+"HistP"+FILE_EXTENSION_PLOT,false);

            // save parameters to ini file
            ad->saveIni(prefix+FILENAME_INI);
        }

    }
}

void ALEXHistogramsWidget::selectRange(double xmin, double xmax , double ymin , double ymax )
{
    qDebug("selectRange");
    ad->FRETparams.MinS=qMin(ymin,ymax);
    ad->FRETparams.MaxS=qMax(ymin,ymax);
    ad->FRETparams.MinP=qMin(xmin,xmax);
    ad->FRETparams.MaxP=qMax(xmin,xmax);
    writeStatistics();
    plotHistogramPR();
    plotHistogramS();
}
void ALEXHistogramsWidget::selectRangeRect(double x, double y , double width , double height )
{
    qDebug()<<"selectRangeRect  x="<<x<<"  y="<<y<<"  width="<<width<<"  height="<<height;
    ad->FRETparams.MinS=qMin(y,y+height);
    ad->FRETparams.MaxS=qMax(y,y+height);
    ad->FRETparams.MinP=qMin(x,x+width);
    ad->FRETparams.MaxP=qMax(x,x+width);
    if (pltSelectionRect) {
        pltSelectionRect->set_bottomleftrectangle(x,y,width,height);
//        pltSelectionRect->set_alpha(ui->spinBoxAlpha->value());
        QColor fillColor=pltSelectionRect->get_fillColor();
        fillColor.setAlpha(ui->spinBoxAlpha->value());
        pltSelectionRect->set_fillColor(fillColor);
        ui->widPlotDensity->update_plot();
    }
//    qDebug()<<ad->FRETparams.toString();
    writeStatistics();
    plotHistogramPR();
    plotHistogramS();
    if(ui->comboBoxPlot->currentIndex()==1) {
        ui->widPlot->get_plotter()->deleteGraph(vRangeGraphTgTr);
        JKQTPverticalRange* gr = new JKQTPverticalRange(ui->widPlot->get_plotter());
        gr->set_rangeMin(ad->FRETparams.MinP);
        gr->set_rangeMax(ad->FRETparams.MaxP);
        gr->set_invertedRange(true);
        QColor col = QColor(Qt::lightGray);
        col.setAlpha(ui->spinBoxAlpha->value());
        gr->set_fillColor(col);
        gr->set_style(Qt::NoPen);
        gr->set_plotCenterLine(false);
        vRangeGraphTgTr = ui->widPlot->addGraph(gr);
        ui->widPlot->update();
    }
}

void ALEXHistogramsWidget::setPlotRange(const Range_t &range, JKQtPlotter *plotter)
{
    plotter->zoom(range.minP,range.maxP,range.minS,range.maxS);
}



void ALEXHistogramsWidget::writeStatistics()
{
    FRETstatistics_t FRETstatistics;
    int nBursts=statisticsFRET(FRETstatistics, ad->burstVectorDual,ad->FRETparams);
    QString text= //tr("P= %1±%2\n").arg(Pmean, 0, 'f', 3).arg(Pstd,0 ,'f',3);
            "P= "  + QString::number(FRETstatistics.Pmean,'f',3) + // P=%.3f&plusmn%.3f\nS=%.3f&plusmn%.3f // &plusmn on Win: Alt+0177
            "&plusmn;"     + QString::number(FRETstatistics.Pstd/sqrt(FRETstatistics.n)  ,'f',3) +
            "<br>S = "+ QString::number(FRETstatistics.Smean ,'f',3) +
            "&plusmn;"     + QString::number(FRETstatistics.Sstd/sqrt(FRETstatistics.n)  ,'f',3) +
            "<br>in the rectangle defined by "+ QString::number(ad->FRETparams.MinP,'f',2) +
            "&lt;=P&lt;="+ QString::number(ad->FRETparams.MaxP,'f',2) +
            "<br>and "+ QString::number(ad->FRETparams.MinS ,'f',2) +
            "&lt;=S &lt;=" + QString::number(ad->FRETparams.MaxS ,'f',2) +
            "<br>STD(P) = "+ QString::number(FRETstatistics.Pstd ,'f',4) +
            "<br>STD(S) = "+ QString::number(FRETstatistics.Sstd ,'f',4) +
            "<br>burstrate = "+ QString::number(FRETstatistics.meanBurstRate*1e-3,'f',2) +
            "<br>burst duration = "+ QString::number(FRETstatistics.meanBurstDuration ,'f',2) +
            "<br>number of photons = "+ QString::number(FRETstatistics.meanBurstSize ,'f',2) +
            "<br> with " + QString::number(nBursts) +
            " bursts.<br>"
            "<br> burst count (classification by DCBS Extended)"
            "<br> Dual : " + QString::number(FRETstatistics.nDual) +
            "<br> Donly: " + QString::number(FRETstatistics.nDonly) +
            "<br> Aonly: " + QString::number(FRETstatistics.nAonly) +
            "<br> mean burst size: " + QString::number(FRETstatistics.meanBurstSize,'i') +
            "<br> mean burst size Donly: " + QString::number(FRETstatistics.meanBurstSizeD,'i') +
            "<br> mean burst size Aonly: " + QString::number(FRETstatistics.meanBurstSizeA,'i') +
            "<br><br>Range: " + QString::number(ad->rangeALEX.minP) + "&lt;=P&lt;=" + QString::number(ad->rangeALEX.maxP) + "<br>     " +
            QString::number(ad->rangeALEX.minS) + "&lt;=S&lt;=" + QString::number(ad->rangeALEX.maxS) +
            "";
    ui->textBrowserStatistics->setHtml(text);
}


void ALEXHistogramsWidget::plotDensity()
{
    plot=ui->widPlotDensity;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    plot->get_plotter()->clearGraphs(true);
    ds->clear();
    plot->setGrid(false);
//    plot->get_plotter()->set_plotLabel("ALEX density plot");
    plot->get_plotter()->getXAxis()->set_axisLabel("P");
    plot->get_plotter()->getYAxis()->set_axisLabel("S");

    int nbins=ui->spinBoxBins->value();
    gsl_histogram2d * histAlex=gsl_histogram2d_calloc_uniform(nbins,nbins,ad->rangeALEX.minP,ad->rangeALEX.maxP,ad->rangeALEX.minS,ad->rangeALEX.maxS);
    for(int i=0; i<ad->burstVectorDual.size();++i)
        gsl_histogram2d_increment(histAlex, ad->burstVectorDual.at(i).proximityRatio, ad->burstVectorDual.at(i).stoichiometryRatio);

//    qDebug()<<histAlex->ny<<histAlex->nx;
    size_t colHist2D=ds->addCopiedImageAsColumnTranspose(histAlex->bin, histAlex->ny, histAlex->nx, tr("2Dhist"));
//    ds->getColumn(colHist2D).exchange(0, 1.0/0.0);

    JKQTPColumnMathImage* densityplot=new JKQTPColumnMathImage(plot->get_plotter()); // bin(i,j) = bin[i * ny + j].
    densityplot->set_x(ad->rangeALEX.minP);
    densityplot->set_y(ad->rangeALEX.minS);
    plot->setAbsoluteX(ad->rangeALEX.minP,ad->rangeALEX.maxP);
    plot->setAbsoluteY(ad->rangeALEX.minS,ad->rangeALEX.maxS);
    densityplot->set_width(ad->rangeALEX.width());
    densityplot->set_height(ad->rangeALEX.heigth());
    densityplot->set_Nx(histAlex->nx);
    densityplot->set_Ny(histAlex->ny);
    densityplot->set_palette(ui->comboBoxPalette->colorPalette());
    densityplot->set_imageColumn(colHist2D);
    densityplot->set_imageName(tr("#bursts"));
//    densityplot->set_infColor(Qt::transparent);
    densityplot->set_nanColor(Qt::transparent);

    plot->addGraph(densityplot);
    gsl_histogram2d_free(histAlex);

    // selection
    QColor fill=QColor("skyblue");
    fill.setAlphaF(0.5);
    pltSelectionRect=new JKQTPgeoRectangle(plot->get_plotter(), 0,0,0,0, QColor("black"), 1, Qt::DotLine, fill);
    pltSelectionRect->set_bottomleftrectangle(ad->FRETparams.MinP,ad->FRETparams.MinS,ad->FRETparams.MaxP-ad->FRETparams.MinP,ad->FRETparams.MaxS-ad->FRETparams.MinS);
    plot->addGraph(pltSelectionRect);
    setPlotRange(ad->rangeALEX,plot);
}

void ALEXHistogramsWidget::plotTgTr()
{
    if(ad->isEmptyBurst()) return;
    qDebug("plot T_g-T_r");
    plot=ui->widPlot;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    plot->get_plotter()->clearGraphs(true);
    ds->clear();
    plot->setGrid(false);
//    plot->get_plotter()->set_plotLabel("");
    plot->get_plotter()->get_yAxis()->set_axisLabel("duration t_D-t_A in ms");
    plot->get_plotter()->get_xAxis()->set_axisLabel("proximity ratio P");

    int nbins=ui->spinBoxBins->value();
    gsl_histogram2d * histAlex=gsl_histogram2d_calloc_uniform(nbins,nbins,ad->FRETparams.MinP,ad->FRETparams.MaxP,ad->FRETparams.MinTgTr,ad->FRETparams.MaxTgTr);
    for(int i=0; i<ad->burstVectorDual.size();++i)
        gsl_histogram2d_increment(histAlex, ad->burstVectorDual.at(i).proximityRatio, (ad->burstVectorDual.at(i).durationDonor-ad->burstVectorDual.at(i).durationAcceptor)*1e3);

    size_t colHist2D=ds->addCopiedImageAsColumnTranspose(histAlex->bin, histAlex->ny, histAlex->nx, tr("2Dhist"));

    JKQTPColumnMathImage* tgMtr=new JKQTPColumnMathImage(plot->get_plotter()); // bin(i,j) = bin[i * ny + j].
    tgMtr->set_x(ad->rangeALEX.minP);
    tgMtr->set_y(ad->FRETparams.MinTgTr);
    plot->setAbsoluteX(ad->rangeALEX.minP,ad->rangeALEX.maxP);
    plot->setAbsoluteY(ad->FRETparams.MinTgTr,ad->FRETparams.MaxTgTr);
    tgMtr->set_width(ad->rangeALEX.maxP-ad->rangeALEX.minP);
    tgMtr->set_height(ad->FRETparams.MaxTgTr-ad->FRETparams.MinTgTr);
    tgMtr->set_Nx(histAlex->nx);
    tgMtr->set_Ny(histAlex->ny);
    tgMtr->set_palette(JKQTPMathImageINVERTED_OCEAN);
    tgMtr->set_imageColumn(colHist2D);
    tgMtr->set_imageName(tr("#bursts"));
    tgMtr->set_nanColor(Qt::transparent);

    plot->addGraph(tgMtr);
    plot->zoomToFit();

    JKQTPverticalRange* gr = new JKQTPverticalRange(plot->get_plotter());
    gr->set_rangeMin(ad->FRETparams.MinP);
    gr->set_rangeMax(ad->FRETparams.MaxP);
    gr->set_invertedRange(true);
    QColor col = QColor(Qt::lightGray);
    col.setAlpha(ui->spinBoxAlpha->value());
    gr->set_fillColor(col);
    gr->set_style(Qt::NoPen);
    gr->set_plotCenterLine(false);
    vRangeGraphTgTr = plot->addGraph(gr);

    gsl_histogram2d_free(histAlex);
}

void ALEXHistogramsWidget:: plotContour()
{
    if(ad->isEmptyBurst()) return;
    plot=ui->widPlot;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    // empty datastore and grid
    plot->get_plotter()->clearGraphs(true);
    ds->clear();
    plot->get_plotter()->setGrid(false);
//    plot->get_plotter()->set_plotLabel("ALEX density plot");
    plot->get_plotter()->getXAxis()->set_axisLabel("P");
    plot->get_plotter()->getYAxis()->set_axisLabel("S");

    int nbins=ui->spinBoxBins->value();
    gsl_histogram2d * histAlex=gsl_histogram2d_calloc_uniform(nbins,nbins,plotRangeContour.minP,plotRangeContour.maxP,plotRangeContour.minS,plotRangeContour.maxS);
    for(int i=0; i<ad->burstVectorDual.size();++i)
        gsl_histogram2d_increment(histAlex, ad->burstVectorDual.at(i).proximityRatio, ad->burstVectorDual.at(i).stoichiometryRatio);
    size_t colHist2D=ds->addCopiedImageAsColumnTranspose(histAlex->bin, histAlex->ny, histAlex->nx, tr("2Dhist"));

    JKQTPContour* cp=new JKQTPContour(plot->get_plotter()); // bin(i,j) = bin[i * ny + j].
    cp->set_x(plotRangeContour.minP);
    cp->set_y(plotRangeContour.minS);
    plot->setAbsoluteX(ad->rangeALEX.minP,ad->rangeALEX.maxP);
    plot->setAbsoluteY(ad->rangeALEX.minS,ad->rangeALEX.maxS);
    cp->set_width(plotRangeContour.width());
    cp->set_height(plotRangeContour.heigth());
    cp->set_palette(ui->comboBoxPalette->colorPalette());
    cp->set_Nx(histAlex->nx);
    cp->set_Ny(histAlex->ny);
    cp->set_imageColumn(colHist2D);
    cp->set_nanColor(Qt::transparent);
    cp->set_contourLevels(contourLevels);
    cp->set_relativeLevels(relativeLevels);

    plot->get_plotter()->set_showKey(true);

    plot->addGraph(cp);
    gsl_histogram2d_free(histAlex); // possible segmentation faults when calling draw() <-- but not if storing data in datastore
    setPlotRange(plotRangeContour,plot);
}

void ALEXHistogramsWidget::plotHistogramPR()
{
    // pr histogram
    plot=ui->widPlotPrHist;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    plot->get_plotter()->clearGraphs(true);
    ds->clear();
    plot->get_plotter()->getXAxis()->set_axisLabel("P");
    plot->get_plotter()->getYAxis()->set_axisLabel("frequency");
    int nbins=ui->spinBoxBins->value();
    gsl_histogram * histPr=gsl_histogram_calloc_uniform(nbins,ad->rangeALEX.minP,ad->rangeALEX.maxP);
    gsl_histogram * histPrSelected=gsl_histogram_calloc_uniform(nbins,ad->rangeALEX.minP,ad->rangeALEX.maxP);
    for(int i=0;i<ad->burstVectorDual.size();++i) {
        gsl_histogram_increment(histPr,ad->burstVectorDual.at(i).proximityRatio);
        if(!ad->burstVectorDual.at(i).isMasked(ad->FRETparams))
            gsl_histogram_increment(histPrSelected,ad->burstVectorDual.at(i).proximityRatio);
    }

    qDebug("plot PR histogram");
    unsigned long long nrows=histPr->n;
    size_t xColumn=ds->addCopiedColumn(histPr->range, nrows, "P"); // adds column and returns column ID
    size_t yColumn=ds->addCopiedColumn(histPr->bin, nrows, "all");
    size_t yColumnFiltered=ds->addCopiedColumn(histPrSelected->bin, nrows, "selected");
    gsl_histogram_free(histPr);
    gsl_histogram_free(histPrSelected);

    JKQTPbarHorizontalGraph* g=new JKQTPbarHorizontalGraph(plot->get_plotter()); // g->set_title("pr");
    g->set_xColumn(xColumn);
    g->set_yColumn(yColumn);
    g->set_shift(0);
    g->set_width(1);
    g->set_style(Qt::NoPen);
    g->set_fillColor(QColor(DISTRIBUTION_COLOR));
    plot->addGraph(g);

    g=new JKQTPbarHorizontalGraph(plot->get_plotter());
    g->set_xColumn(xColumn);
    g->set_yColumn(yColumnFiltered);
    g->set_shift(0);
    g->set_width(1);
    g->set_style(Qt::NoPen);
    g->set_fillColor(QColor(DISTRIBUTION_COLOR_SELECTED));
    plot->addGraph(g);



//    double area=0;
//    for(int i=0;i<histPrSelected->n;++i) {
//        area+=fabs(histPrSelected->range[i+1]-histPrSelected->range[i])*histPrSelected->bin[i];
//    }
//    QVector<double> gauss,gauss_x;
//    gauss.reserve(nbins);
//    int k=0;
//    double mu=model.getGMM()->getMu(k,0);
//    double sigma=model.getGMM()->getGauss(k).getSigma()(0,0);

//    double norm=area/sqrt(2*M_PI)/sigma;
//    qDebug()<<"plot gauss\nmu="<<mu<<" sigma="<<sigma<<"norm="<<norm<<"area="<<area;
//    double x_min=histPrSelected->range[0];
//    double x_max=histPrSelected->range[histPrSelected->n];
//    int npoints=200;
//    double step=(x_max-x_min)/(npoints-1);
//    for(int i=0;i<npoints;++i) {
//        gauss_x.append(x_min+i*step);
//    }
//    for(int i=0;i<npoints;++i) {
//        gauss.append(norm*exp(-pow((gauss_x.at(i)-mu),2)/sigma/sigma/2));
//    }
//    size_t xColumnGauss=ds->addCopiedColumn(gauss_x.data(), gauss_x.size(), "gauss P");
//    size_t yColumnGauss=ds->addCopiedColumn(gauss.data(), gauss.size(), "gauss fit");

//    JKQTPxyLineGraph* gg=new JKQTPxyLineGraph(plot->get_plotter());
//    gg->set_xColumn(xColumnGauss);
//    gg->set_yColumn(yColumnGauss);
//    gg->set_style(Qt::SolidLine);
//    gg->set_color(QColor("black"));
//    plot->addGraph(gg);

    plot->get_plotter()->zoomToFit();

//    JKQTPverticalRange *gr;
//    gr = new JKQTPverticalRange(plot->get_plotter());
//    gr->set_rangeMin(ad->FRETparams.MinP);
//    gr->set_rangeMax(ad->FRETparams.MaxP);
//    gr->set_invertedRange(true);
//    QColor col = QColor(Qt::lightGray);
//    col.setAlpha(ui->spinBoxAlpha->value());
//    gr->set_fillColor(col);
//    gr->set_style(Qt::NoPen);
//    gr->set_plotCenterLine(false);
//    plot->addGraph(gr);
}

void ALEXHistogramsWidget::plotHistogramS()
{
    // pr histogram
    plot=ui->widPlotHistS;
    JKQTPdatastore* ds=plot->get_plotter()->getDatastore();
    plot->get_plotter()->clearGraphs(true);
    ds->clear();
    plot->get_plotter()->getYAxis()->set_axisLabel("S");
    plot->get_plotter()->getXAxis()->set_axisLabel("frequency");
    int nbins=ui->spinBoxBins->value();
    gsl_histogram * histS=gsl_histogram_calloc_uniform(nbins,ad->rangeALEX.minP,ad->rangeALEX.maxP);
    gsl_histogram * histSFiltered=gsl_histogram_calloc_uniform(nbins,ad->rangeALEX.minP,ad->rangeALEX.maxP);
    for(int i=0;i<ad->burstVectorDual.size();++i) {
        gsl_histogram_increment(histS,ad->burstVectorDual.at(i).stoichiometryRatio);
        if(!ad->burstVectorDual.at(i).isMasked(ad->FRETparams))
            gsl_histogram_increment(histSFiltered,ad->burstVectorDual.at(i).stoichiometryRatio);
    }

    qDebug("plot S histogram");
    unsigned long long nrows=histS->n;
    size_t yColumn=ds->addCopiedColumn(histS->range, nrows, "s"); // adds column and returns column ID
    size_t xColumn=ds->addCopiedColumn(histS->bin, nrows, "all");
    size_t xColumnFiltered=ds->addCopiedColumn(histSFiltered->bin, nrows, "selected");
    gsl_histogram_free(histS);
    gsl_histogram_free(histSFiltered);

    JKQTPbarVerticalGraph* g=new JKQTPbarVerticalGraph(plot->get_plotter()); // g->set_title("S");
    g->set_xColumn(xColumn);
    g->set_yColumn(yColumn);
    g->set_shift(0);
    g->set_width(1);
    g->set_style(Qt::NoPen);
    g->set_fillColor(QColor(DISTRIBUTION_COLOR));
    plot->addGraph(g);

    g=new JKQTPbarVerticalGraph(plot->get_plotter());
    g->set_xColumn(xColumnFiltered);
    g->set_yColumn(yColumn);
    g->set_shift(0);
    g->set_width(1);
    g->set_style(Qt::NoPen);
    g->set_fillColor(QColor(DISTRIBUTION_COLOR_SELECTED));
    plot->addGraph(g);

    plot->get_plotter()->zoomToFit();

//    JKQTPhorizontalRange *gr;
//    gr = new JKQTPhorizontalRange(plot->get_plotter());
//    gr->set_rangeMin(ad->FRETparams.MinS);
//    gr->set_rangeMax(ad->FRETparams.MaxS);
//    gr->set_invertedRange(true);
//    QColor col = QColor(Qt::lightGray);
//    col.setAlpha(ui->spinBoxAlpha->value());
//    gr->set_fillColor(col);
//    gr->set_style(Qt::NoPen);
//    gr->set_plotCenterLine(false);
    //    plot->addGraph(gr);
}

void ALEXHistogramsWidget::fitGMM(GMMfit *gmm)
{
    QVector<double> p,s;
    p.reserve(ad->burstVectorDual.size());
    s.reserve(ad->burstVectorDual.size());
    for(int i=0;i<ad->burstVectorDual.size();++i) {
        if(ad->burstVectorDual.at(i).isMasked(ad->FRETparams)) continue;
        p.append(ad->burstVectorDual.at(i).proximityRatio);
        s.append(ad->burstVectorDual.at(i).stoichiometryRatio);
    }
    std::vector<double*> data;
    data.push_back(p.data());
    data.push_back(s.data());
    gmm->setData(data,p.size());
    gmm->fit();
    qDebug()<<QString::fromStdString(gmm->getOptions().toString());
}

//void ALEXHistogramsWidget::importBackground()
//{
//    setBackground();
//}

void ALEXHistogramsWidget::on_chkSelect_toggled(bool checked)
{
    if (checked) {
        ui->widPlotDensity->set_mouseActionMode(JKQtPlotter::RectangleEvents);
    } else {
        ui->widPlotDensity->set_mouseActionMode(JKQtPlotter::ZoomRectangle);
    }
}

void ALEXHistogramsWidget::zoomFull2DHistogram()
{
    setPlotRange(ad->rangeALEX,ui->widPlotDensity);
}

void ALEXHistogramsWidget::setBackground()
{
    qDebug("import background");
    if((ad->background.n_Dem()>0)&&(ad->background.n_Aem()>0)) {
        ad->calculateBackground();
    } else if(QFile::exists(ad->currentDir+"/"+ad->name+"_" FILENAME_INI)) {
        std::string iniFile=(ad->currentDir+"/"+ad->name+"_" FILENAME_INI).toStdString();
        readFromIniBackground(ad->FRETparams,iniFile);
        AlexEvalLog::text("loaded FRET parameters from ini file " + QString::fromStdString(iniFile));

    } else {
        AlexEvalLog::warning("no background information found");
        QMessageBox msgBox;
        msgBox.setText("No background information found.");
        msgBox.setInformativeText("Do you want to load the background settings from a configuration file (*.ini)?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        QString file;
        switch (ret) {
          case QMessageBox::Yes:
            file =QFileDialog::getOpenFileName(this, tr("Open ini file"),
                                        ad->currentDir,
                                        tr("AlexEval ini files(*.ini);;All Files (*.*)"));
            readFromIniBackground(ad->FRETparams,file.toStdString());
              break;
          case QMessageBox::Cancel:// Cancel was clicked
              break;
          default:// should never be reached
            break;
        }
    }
    ui->doubleSpinBoxBackgroundDemDex->setValue(ad->FRETparams.bg_Dem_Dex);
    ui->doubleSpinBoxBackgroundAemDex->setValue(ad->FRETparams.bg_Aem_Dex);
    ui->doubleSpinBoxBackgroundDemAex->setValue(ad->FRETparams.bg_Dem_Aex);
    ui->doubleSpinBoxBackgroundAemAex->setValue(ad->FRETparams.bg_Aem_Aex);
}


#include "dialogTable.h"
void ALEXHistogramsWidget::on_pushButtonLevels_clicked()
{
    qDebug()<<"pushButton clicked";

    DialogTable *dialog = new DialogTable(this);

    QList<QString>  header;
    header<<"contour levels";
    dialog->setTableHeader(header);
    for(int i =0; i<contourLevels.size();++i) {
        dialog->setItem(i,0, QString::number(contourLevels.at(i)));
    }
    dialog->setWindowTitle("Set Contour Levels");
    dialog->exec();

    if(dialog->result()==QDialog::Accepted && (dialog->getRowCount()>0)) {
        contourLevels.clear();
        bool ok;
        double sum=0;
        for(int i =0; i<dialog->getRowCount();++i) {
            double val = dialog->getItemValue(i,0).toDouble(&ok);
            if(ok) contourLevels.append(val);
            sum+=val;
        }
        if(dialog->getAuto()) {
            relativeLevels=true;
        } else {
            relativeLevels=false;
        }
    }


    if(ui->comboBoxPlot->currentIndex()==0) plotContour();


//    fd->clear();
//    testCorrelator(fd->tau,fd->autoCorrDonor,ad->arrivalTimeDonor,ad->arrivalTimeDonor,ad->FCSparams);
    //    plotFCS();
}

void ALEXHistogramsWidget::on_pushButtonGMM_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


    GMMfitOptions options;
    options.nIterMax=100;
    options.threshold=1e-2;

    model=dlgGMM->getModels().at(ui->comboBoxModel->currentIndex());
    model.getGMM()->setOptions(options);
    fitGMM(model.getGMM());
    AlexEvalLog::text(model.toString());

    QString table= ui->plainTextEditGMM->toPlainText();
    table+=model.resultToString(ad->name);
    ui->plainTextEditGMM->setPlainText(table);

    QApplication::restoreOverrideCursor();
}

void ALEXHistogramsWidget::on_pushButtonGMMreset_clicked()
{
    GMMfitModel model;
    ui->plainTextEditGMM->setPlainText(model.resultToStringHeader());
}

void ALEXHistogramsWidget::on_pushButtonEdit_clicked()
{
    QStringList names;
    names<<"P"<<"S";
    dlgGMM->setDimensionName(names);
    dlgGMM->exec();
    ui->comboBoxModel->clear();
    for(int i = 0;i<dlgGMM->getModels().size();++i) {
        ui->comboBoxModel->addItem(dlgGMM->getModels().at(i).getName());
    }
}


void ALEXHistogramsWidget::on_comboBoxPalette_currentIndexChanged()
{
    if(ui->comboBoxPlot->currentIndex()==0) plotContour();
    plotDensity();
}

void ALEXHistogramsWidget::on_comboBoxPlot_currentIndexChanged()
{
    if(ui->comboBoxPlot->currentIndex()==0) plotContour();
    if(ui->comboBoxPlot->currentIndex()==1) plotTgTr();
//    if(ui->comboBoxPlot->currentIndex()==2) plotFCS();
}


void ALEXHistogramsWidget::getFRETparameters()
{
    ad->FRETparams.bg_Dem_Dex=ui->doubleSpinBoxBackgroundDemDex->value();
    ad->FRETparams.bg_Aem_Dex=ui->doubleSpinBoxBackgroundAemDex->value();
    ad->FRETparams.bg_Dem_Aex=ui->doubleSpinBoxBackgroundDemAex->value();
    ad->FRETparams.bg_Aem_Aex=ui->doubleSpinBoxBackgroundAemAex->value();
    ad->calculateDutyCycles();
    ad->FRETparams.crosstalk=ui->doubleSpinBoxCrosstalk->value()*1e-2; // convert from %
    ad->FRETparams.scaleAcceptor=ui->doubleSpinBoxScale->value();
    ad->FRETparams.directExc=ui->doubleSpinBoxDirectExc->value()*1e-2; // convert from %
//    AlexEvalLog::text(ad->FRETparams.toString());
    qDebug()<<ad->FRETparams.toString();
    ui->textBrowserFRETparams->setText(ad->FRETparams.toString());
}

void ALEXHistogramsWidget::automatedRun()
{
    evaluate();
    // FIXME
//    fit->options.nIterMax=50;
//    fit->options.threshold=1e-2;
//    fit->setModel((GMMfitData::Model) ui->comboBoxModel->currentIndex());
//    fitGMM();
    save();
}




