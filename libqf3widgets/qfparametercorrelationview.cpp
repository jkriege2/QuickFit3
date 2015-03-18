/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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


#include "qfparametercorrelationview.h"
#include "statistics_tools.h"
#include <QDebug>
#include <math.h>
#include "jkqttools.h"
#include "statistics_tools.h"
#include "jkqtpelements.h"
#include <QHeaderView>
#include "qftools.h"
#include "qmoretextobject.h"
#include <QTextTableFormat>
#include <QTextFrameFormat>
#include <QTextLength>
#include<QTextCursor>
#include <QTextTable>
#include <QPicture>
#include <QTextDocumentFragment>
#include "programoptions.h"
#include "datatools.h"
#include <QButtonGroup>
#include "qfversion.h"
#if (QT_VERSION > QT_VERSION_CHECK(5, 2, 0))
#include <QPdfWriter>
#endif
#include "qfpluginservices.h"

QFParameterCorrelationView::QFParameterCorrelationView(QWidget *parent) :
    QWidget(parent)
{
    connectParameterWidgetsCounter=0;
    histLabelX="";
    histLabelY="";
    histLabelColor="";
    currentRangeSelection_xmin=0;
    currentRangeSelection_xmax=0;
    currentRangeSelection_ymin=0;
    currentRangeSelection_ymax=0;

    createWidgets();
    setColorCodingMode(ccmNone);
}

QFParameterCorrelationView::~QFParameterCorrelationView()
{
    for (int i=0; i<histograms.size(); i++) {
        if (histograms[i].data1 && !histograms[i].external) qfFree(histograms[i].data1);
        if (histograms[i].data2 && !histograms[i].external) qfFree(histograms[i].data2);
    }
    histograms.clear();
}

void QFParameterCorrelationView::readSettings(QSettings &settings, const QString &prefix)
{
    connectParameterWidgets(false);
    loadSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");
    chkScatterPlot->setChecked(settings.value(prefix+"/show_scatter", false).toBool());
    cmb2DHistogram->setCurrentIndex(settings.value(prefix+"/show_2dhist", 2).toInt());
    cmbSymbol->setCurrentIndex(settings.value(prefix+"/symbol", JKQTPfilledCircle).toInt());
    spinSymbolSize->setValue(settings.value(prefix+"/symbolsize", 5).toInt());
    connectParameterWidgets(true);
}

void QFParameterCorrelationView::writeSettings(QSettings &settings, const QString &prefix)
{
    saveSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");
    settings.setValue(prefix+"/symbol", cmbSymbol->currentIndex());
    settings.value(prefix+"/symbolsize", spinSymbolSize->value());
}

void QFParameterCorrelationView::writeQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{
    current->setQFProperty(prefix+QString("show_scatter_%1").arg(param), chkScatterPlot->isChecked(), false, false);
    current->setQFProperty(prefix+QString("show_2dhist_%1").arg(param), cmb2DHistogram->currentIndex(), false, false);
    current->setQFProperty(prefix+QString("hist2dbins1_%1").arg(param), spin2DHistogramBins1->value(), false, false);
    current->setQFProperty(prefix+QString("hist2dbins2_%1").arg(param), spin2DHistogramBins2->value(), false, false);
    current->setQFProperty(prefix+QString("showkeys_%1").arg(param), chkKey->isChecked(), false, false);

    current->setQFProperty(prefix+QString("symbol_%1").arg(param), cmbSymbol->currentIndex(), false, false);
    current->setQFProperty(prefix+QString("symbsize_%1").arg(param), spinSymbolSize->value(), false, false);
    current->setQFProperty(prefix+QString("bins1_%1").arg(param), spinHistogramBins1->value(), false, false);
    current->setQFProperty(prefix+QString("rauto1_%1").arg(param), chkHistogramRangeAuto1->isChecked(), false, false);
    current->setQFProperty(prefix+QString("rautorel1_%1").arg(param), chkHistogramRangeRelaxAuto1->isChecked(), false, false);
    if (chkHistogramRangeManual1->isChecked()) {
        current->setQFProperty(prefix+QString("rmin1_%1").arg(param), edtHistogramMin1->value(), false, false);
        current->setQFProperty(prefix+QString("rmax1_%1").arg(param), edtHistogramMax2->value(), false, false);
    }
    current->setQFProperty(prefix+QString("rrmin1_%1").arg(param), spinHistogramQL1->value(), false, false);
    current->setQFProperty(prefix+QString("rrmax1_%1").arg(param), spinHistogramQU1->value(), false, false);

    current->setQFProperty(prefix+QString("bins2_%1").arg(param), spinHistogramBins2->value(), false, false);
    current->setQFProperty(prefix+QString("rauto2_%1").arg(param), chkHistogramRangeAuto2->isChecked(), false, false);
    current->setQFProperty(prefix+QString("rautorel2_%1").arg(param), chkHistogramRangeRelaxAuto2->isChecked(), false, false);
    if (chkHistogramRangeManual2->isChecked()) {
        current->setQFProperty(prefix+QString("rmin2_%1").arg(param), edtHistogramMin2->value(), false, false);
        current->setQFProperty(prefix+QString("rmax2_%1").arg(param), edtHistogramMax2->value(), false, false);
    }
    current->setQFProperty(prefix+QString("rrmin2_%1").arg(param), spinHistogramQL2->value(), false, false);
    current->setQFProperty(prefix+QString("rrmax2_%1").arg(param), spinHistogramQU2->value(), false, false);

    current->setQFProperty(prefix+QString("colorscale_%1").arg(param), cmbColorScale->currentIndex(), false, false);
    current->setQFProperty(prefix+QString("colrauto_%1").arg(param), chkColorRangeAuto->isChecked(), false, false);
    current->setQFProperty(prefix+QString("colrautorel_%1").arg(param), chkColorRangeRelaxAuto->isChecked(), false, false);
    if (chkColorRangeManual->isChecked()) {
        current->setQFProperty(prefix+QString("colrmin_%1").arg(param), edtColorMin->value(), false, false);
        current->setQFProperty(prefix+QString("colrmax_%1").arg(param), edtColorMax->value(), false, false);
    }
    current->setQFProperty(prefix+QString("colrrmin2_%1").arg(param), spinColQL->value(), false, false);
    current->setQFProperty(prefix+QString("colrrmax2_%1").arg(param), spinColQU->value(), false, false);


}

void QFParameterCorrelationView::readQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{

    chkScatterPlot->setChecked(current->getQFPropertyHirarchy2(prefix+QString("show_scatter_%1").arg(param), prefix+QString("show_scatter_%1_%2").arg(egroup).arg(param), true).toBool());
    cmb2DHistogram->setCurrentIndex(current->getQFPropertyHirarchy2(prefix+QString("show_2dhist_%1").arg(param), prefix+QString("show_2dhist_%1_%2").arg(egroup).arg(param), 0).toInt());
    spin2DHistogramBins1->setValue(current->getQFPropertyHirarchy2(prefix+QString("hist2dbins1_%1").arg(param), prefix+QString("hist2dbins1_%1_%2").arg(egroup).arg(param), 30).toInt());
    spin2DHistogramBins2->setValue(current->getQFPropertyHirarchy2(prefix+QString("hist2dbins2_%1").arg(param), prefix+QString("hist2dbins2_%1_%2").arg(egroup).arg(param), 30).toInt());
    chkKey->setChecked(current->getQFPropertyHirarchy2(prefix+QString("showkeys_%1").arg(param), prefix+QString("showkeys_%1_%2").arg(egroup).arg(param), true).toBool());



    cmbSymbol->setCurrentIndex(current->getQFPropertyHirarchy2(prefix+QString("symbol_%1").arg(param), prefix+QString("symbol_%1_%2").arg(egroup).arg(param), JKQTPfilledCircle).toInt());
    spinSymbolSize->setValue(current->getQFPropertyHirarchy2(prefix+QString("symbsize_%1").arg(param), prefix+QString("symbsize_%1_%2").arg(egroup).arg(param), 3).toInt());
    spinHistogramBins1->setValue(current->getQFPropertyHirarchy2(prefix+QString("bins1_%1").arg(param), prefix+QString("bins1_%1_%2").arg(egroup).arg(param), 30).toInt());
    chkHistogramRangeAuto1->setChecked(current->getQFPropertyHirarchy2(prefix+QString("rauto1_%1").arg(param), prefix+QString("rauto1_%1_%2").arg(egroup).arg(param), true).toBool());
    chkHistogramRangeRelaxAuto1->setChecked(current->getQFPropertyHirarchy2(prefix+QString("rautorel1_%1").arg(param), prefix+QString("rautorel1_%1_%2").arg(egroup).arg(param), false).toBool());
    if (chkHistogramRangeManual1->isChecked()) {
        edtHistogramMin1->setValue(current->getQFPropertyHirarchy2(prefix+QString("rmin1_%1").arg(param), prefix+QString("rmin1_%1_%2").arg(egroup).arg(param), 0).toDouble());
        edtHistogramMax1->setValue(current->getQFPropertyHirarchy2(prefix+QString("rmax1_%1").arg(param), prefix+QString("rmax1_%1_%2").arg(egroup).arg(param), 10).toDouble());
    }
    spinHistogramQL1->setValue(current->getQFPropertyHirarchy2(prefix+QString("rrmin1_%1").arg(param), prefix+QString("rrmin1_%1_%2").arg(egroup).arg(param), 5).toDouble());
    spinHistogramQU1->setValue(current->getQFPropertyHirarchy2(prefix+QString("rrmax1_%1").arg(param), prefix+QString("rrmax1_%1_%2").arg(egroup).arg(param), 5).toDouble());

    spinHistogramBins2->setValue(current->getQFPropertyHirarchy2(prefix+QString("bins2_%1").arg(param), prefix+QString("bins2_%1_%2").arg(egroup).arg(param), 30).toInt());
    chkHistogramRangeAuto2->setChecked(current->getQFPropertyHirarchy2(prefix+QString("rauto2_%1").arg(param), prefix+QString("rauto2_%1_%2").arg(egroup).arg(param), true).toBool());
    chkHistogramRangeRelaxAuto2->setChecked(current->getQFPropertyHirarchy2(prefix+QString("rautorel2_%1").arg(param), prefix+QString("rautorel2_%1_%2").arg(egroup).arg(param), false).toBool());
    if (chkHistogramRangeManual2->isChecked()) {
        edtHistogramMin2->setValue(current->getQFPropertyHirarchy2(prefix+QString("rmin2_%1").arg(param), prefix+QString("rmin2_%1_%2").arg(egroup).arg(param), 0).toDouble());
        edtHistogramMax2->setValue(current->getQFPropertyHirarchy2(prefix+QString("rmax2_%1").arg(param), prefix+QString("rmax2_%1_%2").arg(egroup).arg(param), 10).toDouble());
    }
    spinHistogramQL1->setValue(current->getQFPropertyHirarchy2(prefix+QString("rrmin2_%1").arg(param), prefix+QString("rrmin2_%1_%2").arg(egroup).arg(param), 5).toDouble());
    spinHistogramQU1->setValue(current->getQFPropertyHirarchy2(prefix+QString("rrmax2_%1").arg(param), prefix+QString("rrmax2_%1_%2").arg(egroup).arg(param), 5).toDouble());



    cmbColorScale->setCurrentIndex(current->getQFPropertyHirarchy2(prefix+QString("colorscale_%1").arg(param), prefix+QString("colorscale_%1_%2").arg(egroup).arg(param), JKQTPMathImageMATLAB).toInt());
    chkColorRangeAuto->setChecked(current->getQFPropertyHirarchy2(prefix+QString("colrauto_%1").arg(param), prefix+QString("colrauto_%1_%2").arg(egroup).arg(param), true).toBool());
    chkColorRangeRelaxAuto->setChecked(current->getQFPropertyHirarchy2(prefix+QString("colrautorel_%1").arg(param), prefix+QString("colrautorel_%1_%2").arg(egroup).arg(param), false).toBool());
    if (chkColorRangeManual->isChecked()) {
        edtColorMin->setValue(current->getQFPropertyHirarchy2(prefix+QString("colrmin_%1").arg(param), prefix+QString("colrmin_%1_%2").arg(egroup).arg(param), 0).toDouble());
        edtColorMax->setValue(current->getQFPropertyHirarchy2(prefix+QString("colrmax_%1").arg(param), prefix+QString("colrmax_%1_%2").arg(egroup).arg(param), 10).toDouble());
    }
    spinColQL->setValue(current->getQFPropertyHirarchy2(prefix+QString("colrrmin2_%1").arg(param), prefix+QString("colrrmin2_%1_%2").arg(egroup).arg(param), 5).toDouble());
    spinColQU->setValue(current->getQFPropertyHirarchy2(prefix+QString("colrrmax2_%1").arg(param), prefix+QString("colrrmax2_%1_%2").arg(egroup).arg(param), 5).toDouble());
}

void QFParameterCorrelationView::connectParameterWidgets(bool connectTo)
{
    if (connectTo) {
        connect(spinSymbolSize, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        connect(cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSettingsChanged()));

        connect(spinHistogramBins1, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        connect(chkHistogramRangeAuto1, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(edtHistogramMin1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(edtHistogramMax1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(chkHistogramRangeRelaxAuto1, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(chkHistogramRangeManual1, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(spinHistogramQL1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(spinHistogramQU1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));

        connect(spinHistogramBins2, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        connect(chkHistogramRangeAuto2, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(edtHistogramMin2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(edtHistogramMax2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(chkHistogramRangeRelaxAuto2, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(chkHistogramRangeManual2, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(spinHistogramQL2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(spinHistogramQU2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));


        connect(cmbColorScale, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSettingsChanged()));
        connect(chkColorRangeAuto, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(edtColorMin, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(edtColorMax, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(chkColorRangeManual, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(chkColorRangeRelaxAuto, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(spinColQL, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        connect(spinColQU, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));


        connect(spin2DHistogramBins1, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        connect(spin2DHistogramBins2, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        connect(chkScatterPlot, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(chkKey, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        connect(cmb2DHistogram, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSettingsChanged()));

    } else {
        disconnect(spinSymbolSize, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        disconnect(cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSettingsChanged()));

        disconnect(spinHistogramBins1, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        disconnect(chkHistogramRangeAuto1, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(edtHistogramMin1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(edtHistogramMax1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(chkHistogramRangeRelaxAuto1, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(chkHistogramRangeManual1, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(spinHistogramQL1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(spinHistogramQU1, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));

        disconnect(spinHistogramBins2, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        disconnect(chkHistogramRangeAuto2, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(edtHistogramMin2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(edtHistogramMax2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(chkHistogramRangeRelaxAuto2, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(chkHistogramRangeManual2, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(spinHistogramQL2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(spinHistogramQU2, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));

        disconnect(spin2DHistogramBins1, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        disconnect(spin2DHistogramBins2, SIGNAL(valueChanged(int)), this, SLOT(dataSettingsChanged()));
        disconnect(chkScatterPlot, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(chkKey, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(cmb2DHistogram, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSettingsChanged()));
        disconnect(cmbColorScale, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSettingsChanged()));
        disconnect(chkColorRangeAuto, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(edtColorMin, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(edtColorMax, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(chkColorRangeManual, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(chkColorRangeRelaxAuto, SIGNAL(toggled(bool)), this, SLOT(dataSettingsChanged()));
        disconnect(spinColQL, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));
        disconnect(spinColQU, SIGNAL(valueChanged(double)), this, SLOT(dataSettingsChanged()));

    }
}

void QFParameterCorrelationView::replotCorrelation()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    pltParamCorrelation->set_doDrawing(false);
    pltParamHistogramX->set_doDrawing(false);
    pltParamHistogramY->set_doDrawing(false);
    pltParamCorrelation->getYAxis()->set_logAxis(false);
    pltParamCorrelation->getXAxis()->set_axisLabel(histLabelX);
    pltParamCorrelation->getYAxis()->set_axisLabel(histLabelY);
    pltParamCorrelation->set_doDrawing(true);
    pltParamHistogramX->set_doDrawing(true);
    pltParamHistogramY->set_doDrawing(true);
    pltParamCorrelation->zoomToFit(true, true, false, false);
    pltParamHistogramX->zoomToFit(false, true, false, false);
    pltParamHistogramY->zoomToFit(true, false, false, false);
    pltParamCorrelation->update_plot();
    pltParamHistogramX->update_plot();
    pltParamHistogramY->update_plot();
    QApplication::restoreOverrideCursor();
}

void QFParameterCorrelationView::updateCorrelation(bool replot, int which)
{

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


    edtHistogramMin1->setEnabled(chkHistogramRangeManual1->isChecked());
    edtHistogramMax1->setEnabled(chkHistogramRangeManual1->isChecked());
    spinHistogramQL1->setEnabled(chkHistogramRangeRelaxAuto1->isChecked());
    spinHistogramQU1->setEnabled(chkHistogramRangeRelaxAuto1->isChecked());
    edtHistogramMin2->setEnabled(chkHistogramRangeManual2->isChecked());
    edtHistogramMax2->setEnabled(chkHistogramRangeManual2->isChecked());
    spinHistogramQL2->setEnabled(chkHistogramRangeRelaxAuto2->isChecked());
    spinHistogramQU2->setEnabled(chkHistogramRangeRelaxAuto2->isChecked());
    edtColorMin->setEnabled(chkColorRangeManual->isChecked());
    edtColorMax->setEnabled(chkColorRangeManual->isChecked());
    spinColQL->setEnabled(chkColorRangeRelaxAuto->isChecked());
    spinColQU->setEnabled(chkColorRangeRelaxAuto->isChecked());




    pltParamCorrelation->set_doDrawing(false);
    pltParamHistogramX->set_doDrawing(false);
    pltParamHistogramY->set_doDrawing(false);
    tvHistogramParameters->setModel(NULL);
    tabHistogramParameters->setReadonly(false);
    pltParamCorrelation->get_plotter()->set_showKey(chkKey->isChecked());
    pltParamHistogramX->get_plotter()->set_showKey(chkKey->isChecked());
    pltParamHistogramY->get_plotter()->set_showKey(chkKey->isChecked());

    JKQTPdatastore* ds=pltParamCorrelation->getDatastore();
    if (which<0) {
        pltParamCorrelation->clearGraphs(true);
        pltParamHistogramX->clearGraphs(true);
        pltParamHistogramY->clearGraphs(true);
        ds->clear();
        for (int r=0; r<tabHistogramParameters->rowCount(); r++) {
            for (int c=0; c<histograms.size(); c++)  {
                tabHistogramParameters->setCellCreate(r, c+1, QVariant());
            }
        }
    } else if (which<histograms.size()){
        CorrelationItem hist=histograms[which];
        for (int i=pltParamCorrelation->getGraphCount()-1; i>=0; i--) {
            if (pltParamCorrelation->getGraph(i)->get_title().contains(hist.name)) {
                pltParamCorrelation->deleteGraph(i, true);
            }
        }
        QString prefix=QString("hist%1_").arg(which);
        ds->deleteAllPrefixedColumns(prefix);
        prefix=QString("corr%1_").arg(which);
        ds->deleteAllPrefixedColumns(prefix);
        for (int r=0; r<tabHistogramParameters->rowCount(); r++) {
            tabHistogramParameters->setCellCreate(r, which+1, QVariant());
        }
    }



    //qDebug()<<"***** updateHistogram()   "<<plteImageData<<plteImageSize;


    int histStart=0;
    int histEnd=histograms.size();
    if (which>=0) {
        histStart=which;
        histEnd=which+1;
    }
    QList<JKQTPgraph*> graphs_img;
    QList<JKQTPgraph*> graphs_scatter;

    for (int hh=histStart; hh<histEnd; hh++) {
        //qDebug()<<hh<<histograms.size();
        if (hh>=0 && hh<histograms.size()) {
            CorrelationItem hist=histograms[hh];
            if (hist.data1 && hist.data2 && (hist.size>0)) {

                double dmean, dstd=0, dmin, dmax, dmedian, dq25, dq75, dskew;//, dql, dqu;
                double* d1=duplicateArray(hist.data1, hist.size);
                double* d2=duplicateArray(hist.data2, hist.size);
                double* d3=NULL;
                if (hist.data3) d3=duplicateArray(hist.data3, hist.size);
                else d3=duplicateArray(hist.data1, hist.size);

                double* d1s=duplicateArraySort(d1, hist.size);
                double* d2s=duplicateArraySort(d2, hist.size);
                double* d3s=duplicateArraySort(d3, hist.size);



                int imageSize=hist.size;
                int32_t datasize=0;
                double mmin1=edtHistogramMin1->value();
                double mmax1=edtHistogramMax1->value();
                if (chkHistogramRangeAuto1->isChecked()) {
                    mmin1=statisticsSortedMin(d1s, imageSize);
                    mmax1=statisticsSortedMax(d1s, imageSize);
                } else if (chkHistogramRangeRelaxAuto1->isChecked()) {
                    mmin1=statisticsSortedQuantile(d1s, imageSize, spinHistogramQL1->value()/100.0);
                    mmax1=statisticsSortedQuantile(d1s, imageSize, (100.0-spinHistogramQU1->value())/100.0);
                }


                double mmin2=edtHistogramMin2->value();
                double mmax2=edtHistogramMax2->value();
                if (chkHistogramRangeAuto2->isChecked()) {
                    mmin2=statisticsSortedMin(d2s, imageSize);
                    mmax2=statisticsSortedMax(d2s, imageSize);
                } else if (chkHistogramRangeRelaxAuto2->isChecked()) {
                    mmin2=statisticsSortedQuantile(d2s, imageSize, spinHistogramQL2->value()/100.0);
                    mmax2=statisticsSortedQuantile(d2s, imageSize, (100.0-spinHistogramQU2->value())/100.0);
                }


                double mmin3=edtColorMax->value();
                double mmax3=edtColorMax->value();
                if (hist.data3) {
                    if (chkColorRangeAuto->isChecked()) {
                        mmin3=statisticsSortedMin(d3s, imageSize);
                        mmax3=statisticsSortedMax(d3s, imageSize);
                    } else if (chkColorRangeRelaxAuto->isChecked()) {
                        mmin3=statisticsSortedQuantile(d3s, imageSize, spinColQL->value()/100.0);
                        mmax3=statisticsSortedQuantile(d3s, imageSize, (100.0-spinColQU->value())/100.0);
                    }
                }



                /*qDebug()<<mmin1<<mmax1;
                qDebug()<<mmin2<<mmax2;
                qDebug()<<mmin3<<mmax3;*/

                for (register int32_t i=0; i<imageSize; i++) {
                    const double v1=hist.data1[i];
                    const double v2=hist.data2[i];
                    double v3=0;
                    if(hist.data3!=NULL) v3=hist.data3[i];
                    if (statisticsFloatIsOK(v1)&&statisticsFloatIsOK(v2)&&((v1>=mmin1 && v1<=mmax1))
                                                                        &&((v2>=mmin2 && v2<=mmax2))
                                                                        &&((!hist.data3) || (v3>=mmin3 && v3<=mmax3 &&statisticsFloatIsOK(v3)))) {
                        /*qDebug()<<i<<": "<<v1<<mmin1<<mmax1<<chkHistogramRangeAuto1->isChecked()<<(v1>=mmin1)<<(v1<=mmax1);
                        qDebug()<<"     "<<v2<<mmin2<<mmax2<<chkHistogramRangeAuto2->isChecked()<<(v2>=mmin2)<<(v2<=mmax2);
                        qDebug()<<datasize;*/
                        d1[datasize]=v1;
                        d2[datasize]=v2;
                        d3[datasize]=v3;
                        datasize++;
                    }
                }

                double* d1r=duplicateArray(d1, datasize);
                double* d2r=duplicateArray(d2, datasize);
                double* d3r=duplicateArray(d3, datasize);

                int col=0;
                tabHistogramParameters->setCellCreate(col++, hh+1, statisticsCorrelationCoefficient(d1r, d2r, datasize)*100.0);
                double linfitA=0, linfitB=1;
                statisticsIterativelyReweightedLeastSquaresRegression(d1r, d2r, datasize, linfitA, linfitB);
                double linfitrA=0, linfitrB=1;
                statisticsLinearRegression(d1r, d2r, datasize, linfitrA, linfitrB);
                tabHistogramParameters->setCellCreate(col++, hh+1, linfitrA);
                tabHistogramParameters->setCellCreate(col++, hh+1, linfitrB);
                tabHistogramParameters->setCellCreate(col++, hh+1, linfitA);
                tabHistogramParameters->setCellCreate(col++, hh+1, linfitB);



                statisticsSort(d1, datasize);
                statisticsSort(d2, datasize);
                statisticsSort(d3, datasize);


                dmean=statisticsAverageVariance(dstd, d1, datasize);
                dstd=sqrt(dstd);
                dmin=statisticsSortedMin(d1, datasize);
                dmax=statisticsSortedMax(d1, datasize);
                dmedian=statisticsSortedMedian(d1, datasize);
                dq25=statisticsSortedQuantile(d1, datasize, 0.25);
                dq75=statisticsSortedQuantile(d1, datasize, 0.75);
                dskew=statisticsSkewness(d1, datasize);


                tabHistogramParameters->setCellCreate(col++, hh+1, datasize);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmean);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmedian);
                tabHistogramParameters->setCellCreate(col++, hh+1, dstd);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmin);
                tabHistogramParameters->setCellCreate(col++, hh+1, dq25);
                tabHistogramParameters->setCellCreate(col++, hh+1, dq75);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmax);
                tabHistogramParameters->setCellCreate(col++, hh+1, dskew);
                tabHistogramParameters->setCellCreate(col++, hh+1, imageSize-datasize);
                tabHistogramParameters->setColumnTitle(hh+1, hist.name);

                if (!chkHistogramRangeManual1->isChecked() && hh==0) {
                    connectParameterWidgets(false);
                    edtHistogramMin1->setValue(mmin1);
                    edtHistogramMax1->setValue(mmax1);
                    connectParameterWidgets(true);
                }

                long histBinsX=spinHistogramBins1->value();
                double* histXX=(double*)qfMalloc(histBinsX*sizeof(double));
                double* histXY=(double*)qfMalloc(histBinsX*sizeof(double));
                statisticsHistogramRanged<double, double>(d1r, datasize, mmin1, mmax1, histXX, histXY, histBinsX, false);


                dmean=statisticsAverageVariance(dstd, d2, datasize);
                dstd=sqrt(dstd);
                dmin=statisticsSortedMin(d2, datasize);
                dmax=statisticsSortedMax(d2, datasize);
                dmedian=statisticsSortedMedian(d2, datasize);
                dq25=statisticsSortedQuantile(d2, datasize, 0.25);
                dq75=statisticsSortedQuantile(d2, datasize, 0.75);
                //dql=statisticsSortedQuantile(d1, datasize, spinHistogramQL2->value()/100.0);
                //dqu=statisticsSortedQuantile(d1, datasize, (100.0-spinHistogramQU2->value())/100.0);
                dskew=statisticsSkewness(d2, datasize);
                tabHistogramParameters->setCellCreate(col++, hh+1, datasize);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmean);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmedian);
                tabHistogramParameters->setCellCreate(col++, hh+1, dstd);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmin);
                tabHistogramParameters->setCellCreate(col++, hh+1, dq25);
                tabHistogramParameters->setCellCreate(col++, hh+1, dq75);
                tabHistogramParameters->setCellCreate(col++, hh+1, dmax);
                tabHistogramParameters->setCellCreate(col++, hh+1, dskew);
                tabHistogramParameters->setCellCreate(col++, hh+1, imageSize-datasize);
                tabHistogramParameters->setColumnTitle(hh+1, hist.name);

                if (!chkHistogramRangeManual2->isChecked() && hh==0) {
                    connectParameterWidgets(false);
                    edtHistogramMin2->setValue(mmin2);
                    edtHistogramMax2->setValue(mmax2);
                    connectParameterWidgets(true);
                }

                long histBinsY=spinHistogramBins2->value();
                double* histYX=(double*)qfMalloc(histBinsY*sizeof(double));
                double* histYY=(double*)qfMalloc(histBinsY*sizeof(double));
                statisticsHistogramRanged<double, double>(d2r, datasize, mmin2, mmax2, histYX, histYY, histBinsY, false);

                long hist2DBinsX=spin2DHistogramBins1->value();
                long hist2DBinsY=spin2DHistogramBins2->value();
                double* hist2DX=(double*)qfMalloc(hist2DBinsX*sizeof(double));
                double* hist2DY=(double*)qfMalloc(hist2DBinsY*sizeof(double));
                double* hist2D=(double*)qfCalloc(hist2DBinsX*hist2DBinsY,sizeof(double));
                double* kde2D=(double*)qfCalloc(hist2DBinsX*hist2DBinsY,sizeof(double));
                double bwX=(mmax1-mmin1)/double(hist2DBinsX);
                double bwY=(mmax2-mmin2)/double(hist2DBinsY);
                if (cmb2DHistogram->currentIndex()==2) statistics2DKDEHistogramRanged(d1r, d2r, datasize, statisticsKernel2DGaussian, bwX, bwY, mmin1, mmax1, mmin2, mmax2, hist2DX, hist2DY, kde2D, hist2DBinsX, hist2DBinsY);
                if (cmb2DHistogram->currentIndex()==1) statisticsHistogram2DRanged(d1r, d2r, datasize, mmin1, mmax1, mmin2, mmax2, hist2DX, hist2DY, hist2D, hist2DBinsX, hist2DBinsY);

                //qDebug()<<"bwX="<<bwX;
                //qDebug()<<"bwY="<<bwY;
                //qDebug()<<"x="<<arrayToString(hist2DX, hist2DBinsX);
                //qDebug()<<"y="<<arrayToString(hist2DY, hist2DBinsY);

                if (cmb2DHistogram->currentIndex()>0) {
                    int g_i=-1;
                    if (cmb2DHistogram->currentIndex()==1) {
                        g_i=pltParamCorrelation->getDatastore()->addCopiedImageAsColumn(hist2D, hist2DBinsX, hist2DBinsY, tr("corr%1_2DHISTOGRAM").arg(hh));
                    }
                    if (cmb2DHistogram->currentIndex()==2) {
                        g_i=pltParamCorrelation->getDatastore()->addCopiedImageAsColumn(kde2D, hist2DBinsX, hist2DBinsY, tr("corr%1_2DDENSITY").arg(hh));
                    }
                    JKQTPColumnMathImage* g=new JKQTPColumnMathImage(pltParamCorrelation->get_plotter());
                    g->set_title(tr("density: %1").arg(hist.name));
                    g->set_imageColumn(g_i);
                    g->set_Nx(hist2DBinsX);
                    g->set_Ny(hist2DBinsY);
                    g->set_x(mmin1);
                    g->set_y(mmin2);
                    g->set_width(mmax1-mmin1);
                    g->set_height(mmax2-mmin2);
                    g->set_palette(JKQTPMathImageINVERTED_OCEAN);
                    g->set_autoImageRange(true);
                    pltParamCorrelation->addGraph(g);
                    graphs_img<<g;
                }

                QColor scatterColor=QColor("red");
                if (hh%5==1) scatterColor=QColor("darkorange");
                if (hh%5==2) scatterColor=QColor("gold");
                if (hh%5==3) scatterColor=QColor("deeppink");
                if (hh%5==4) scatterColor=QColor("purple");

                if (chkScatterPlot->isChecked()) {
                    JKQTPxyParametrizedScatterGraph* g=new JKQTPxyParametrizedScatterGraph(pltParamCorrelation->get_plotter());
                    int g_x=pltParamCorrelation->getDatastore()->addCopiedColumn(d1r, datasize, tr("corr%1_X").arg(hh));
                    int g_y=pltParamCorrelation->getDatastore()->addCopiedColumn(d2r, datasize, tr("corr%1_Y").arg(hh));
                    g->set_title(tr("scatter: %1").arg(hist.name));
                    g->set_xColumn(g_x);
                    g->set_yColumn(g_y);
                    g->set_symbol(cmbSymbol->getSymbol());
                    g->set_symbolSize(spinSymbolSize->value());
                    g->set_drawLine(false);
                    g->set_color(scatterColor);
                    g->set_fillColor(scatterColor.lighter());

                    if (hist.data3 && colorcoding==ccmBy3rdColumn) {
                        int g_c=pltParamCorrelation->getDatastore()->addCopiedColumn(d3r, datasize, tr("corr%1_C").arg(hh));
                        g->set_colorColumnContainsRGB(false);
                        g->set_colorColumn(g_c);
                        g->set_autoImageRange(chkColorRangeAuto->isChecked());
                        g->set_imageMin(mmin3);
                        g->set_imageMax(mmax3);
                        g->set_palette(cmbColorScale->currentIndex());
                        g->get_colorBarRightAxis()->set_axisLabel(histLabelColor);
                        g->get_colorBarTopAxis()->set_axisLabel(histLabelColor);

                    } else if (colorcoding==ccmXYGR) {
                        QVector<double> d3rc(datasize,0);
                        double xmin=0, xmax=0;
                        double ymin=0, ymax=0;
                        statisticsMinMax(d1r, datasize, xmin, xmax);
                        statisticsMinMax(d2r, datasize, ymin, ymax);

                        for (int i=0; i<d3rc.size(); i++) {
                            double x=d1r[i];
                            double y=d2r[i];
                            d3rc[i]=qRgb(floor(127.0+128.0*(y-ymin)/(ymax-ymin)),floor(127.0+128*(x-xmin)/(xmax-xmin)),0);
                        }
                        int g_c=pltParamCorrelation->getDatastore()->addCopiedColumn(d3rc.data(), d3rc.size(), tr("corr%1_Col_RGB").arg(hh));
                        g->set_colorColumnContainsRGB(true);
                        g->set_colorColumn(g_c);
                        g->get_colorBarRightAxis()->set_axisLabel(histLabelColor);
                        g->get_colorBarTopAxis()->set_axisLabel(histLabelColor);
                    } else if (colorcoding==ccmXYRB) {
                        QVector<double> d3rc(datasize,0);
                        double xmin=0, xmax=0;
                        double ymin=0, ymax=0;
                        statisticsMinMax(d1r, datasize, xmin, xmax);
                        statisticsMinMax(d2r, datasize, ymin, ymax);

                        for (int i=0; i<d3rc.size(); i++) {
                            double x=d1r[i];
                            double y=d2r[i];
                            d3rc[i]=qRgb(floor(255.0*(x-xmin)/(xmax-xmin)),0,floor(255.0*(y-ymin)/(ymax-ymin)));
                        }
                        int g_c=pltParamCorrelation->getDatastore()->addCopiedColumn(d3rc.data(), d3rc.size(), tr("corr%1_Col_RGB").arg(hh));
                        g->set_colorColumnContainsRGB(true);
                        g->set_colorColumn(g_c);
                        g->get_colorBarRightAxis()->set_axisLabel(histLabelColor);
                        g->get_colorBarTopAxis()->set_axisLabel(histLabelColor);
                    }


                    pltParamCorrelation->addGraph(g);
                    graphs_scatter<<g;
                }





                JKQTPbarHorizontalGraph* plteParamHistogramX=new JKQTPbarHorizontalGraph(pltParamHistogramX->get_plotter());
                int g_hxx=pltParamCorrelation->getDatastore()->addCopiedColumn(histXX, histBinsX, tr("hist%1_X_histogram_X").arg(hh));
                int g_hxy=pltParamCorrelation->getDatastore()->addCopiedColumn(histXY, histBinsX, tr("hist%1_X_histogram_Y").arg(hh));
                QColor fill=scatterColor;
                plteParamHistogramX->set_color(fill);
                fill.setAlphaF(0.7);
                plteParamHistogramX->set_fillColor(fill);
                plteParamHistogramX->set_xColumn(g_hxx);
                plteParamHistogramX->set_yColumn(g_hxy);
                plteParamHistogramX->set_width(1);
                plteParamHistogramX->set_title("");
                pltParamHistogramX->addGraph(plteParamHistogramX);


                JKQTPbarVerticalGraph* plteParamHistogramY=new JKQTPbarVerticalGraph(pltParamHistogramY->get_plotter());
                int g_hyx=pltParamCorrelation->getDatastore()->addCopiedColumn(histYX, histBinsY, tr("hist%1_Y_histogram_X").arg(hh));
                int g_hyy=pltParamCorrelation->getDatastore()->addCopiedColumn(histYY, histBinsY, tr("hist%1_Y_histogram_Y").arg(hh));
                fill=scatterColor;
                plteParamHistogramY->set_color(fill);
                fill.setAlphaF(0.7);
                plteParamHistogramY->set_fillColor(fill);
                plteParamHistogramY->set_yColumn(g_hyx);
                plteParamHistogramY->set_xColumn(g_hyy);
                plteParamHistogramY->set_width(1);
                plteParamHistogramY->set_title("");
                pltParamHistogramY->addGraph(plteParamHistogramY);


                JKQTPxFunctionLineGraph *plteRLinFit=new JKQTPxFunctionLineGraph(pltParamCorrelation->get_plotter());
                plteRLinFit->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
                QVector<double> linfitParams;
                linfitParams<<linfitrA<<linfitrB;
                plteRLinFit->set_params(linfitParams);
                plteRLinFit->set_color(scatterColor.darker());
                plteRLinFit->set_lineWidth(1.5);
                plteRLinFit->set_style(Qt::DotLine);
                plteRLinFit->set_title(tr("regression: f(x)= %1 + %2 \\cdot x").arg(doubleToLatexQString(linfitrA, 4)).arg(doubleToLatexQString(linfitrB, 4)));
                pltParamCorrelation->addGraph(plteRLinFit);

                JKQTPxFunctionLineGraph *plteLinFit=new JKQTPxFunctionLineGraph(pltParamCorrelation->get_plotter());
                plteLinFit->setSpecialFunction(JKQTPxFunctionLineGraph::Polynomial);
                linfitParams.clear();
                linfitParams<<linfitA<<linfitB;
                plteLinFit->set_params(linfitParams);
                plteLinFit->set_color(scatterColor.darker());
                plteLinFit->set_lineWidth(1.5);
                plteLinFit->set_style(Qt::DashLine);
                plteLinFit->set_title(tr("IRLS: f(x)= %1 + %2 \\cdot x").arg(doubleToLatexQString(linfitA, 4)).arg(doubleToLatexQString(linfitB, 4)));
                pltParamCorrelation->addGraph(plteLinFit);


                if (hist.data3) {
                    if (!chkColorRangeManual->isChecked() && hh==0) {
                        connectParameterWidgets(false);
                        edtColorMin->setValue(mmin3);
                        edtColorMax->setValue(mmax3);
                        connectParameterWidgets(true);
                    }
                }


                qfFree(histXX);
                qfFree(histXY);

                qfFree(histYX);
                qfFree(histYY);

                qfFree(hist2DX);
                qfFree(hist2DY);
                qfFree(hist2D);
                qfFree(kde2D);

                qfFree(d1);
                qfFree(d2);
                qfFree(d3);
                qfFree(d1r);
                qfFree(d2r);
                qfFree(d3r);
                qfFree(d1s);
                qfFree(d2s);
                qfFree(d3s);



            }
        }
    }

    for (int i=graphs_img.size()-1; i>=0; i--) {
        pltParamCorrelation->moveGraphBottom(graphs_img[i]);
    }


    tabHistogramParameters->setReadonly(true);
    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->resizeColumnsToContents();
    pltParamCorrelation->set_doDrawing(true);
    pltParamHistogramX->set_doDrawing(true);
    pltParamHistogramY->set_doDrawing(true);
    if (replot) {
        replotCorrelation();
    }

    QApplication::restoreOverrideCursor();
}

void QFParameterCorrelationView::addSettingsWidget(const QString &label, QWidget *widget)
{
    flHistSet->addRow(label, widget);
}


void QFParameterCorrelationView::clear()
{
    for (int i=0; i<histograms.size(); i++) {
        if (histograms[i].data1 && !histograms[i].external) qfFree(histograms[i].data1);
        if (histograms[i].data2 && !histograms[i].external) qfFree(histograms[i].data2);
    }
    histograms.clear();
    updateCorrelation(true);
}

int QFParameterCorrelationView::addCorrelation(QString name, double *data1, double *data2, int32_t size, bool external)
{
    QFParameterCorrelationView::CorrelationItem h;
    h.data1=data1;
    h.data2=data2;
    if (!external) {
        h.data1=duplicateArray(data1, size);
        h.data2=duplicateArray(data2, size);
    }
    h.name=name;
    h.size=size;
    h.external=external;
    histograms.append(h);

    return histograms.size()-1;
}

int QFParameterCorrelationView::addCorrelation(QString name, double *data1, double *data2, double *data3, int32_t size, bool external){
    QFParameterCorrelationView::CorrelationItem h;
    h.data1=data1;
    h.data2=data2;
    h.data3=data3;
    if (!external) {
        h.data1=duplicateArray(data1, size);
        h.data2=duplicateArray(data2, size);
        h.data3=duplicateArray(data3, size);
    }
    h.name=name;
    h.size=size;
    h.external=external;
    histograms.append(h);

    if (data3) setColorCodingMode(ccmBy3rdColumn);

    return histograms.size()-1;
}


int QFParameterCorrelationView::addCopiedCorrelation(QString name, const double *data1, const double *data2, int32_t size)
{
    QFParameterCorrelationView::CorrelationItem h;
    h.data1=duplicateArray(data1, size);
    h.data2=duplicateArray(data2, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms.append(h);

    return histograms.size()-1;
}

int QFParameterCorrelationView::addCopiedCorrelation(QString name, const double *data1, const double *data2, const double *data3, int32_t size)
{
    QFParameterCorrelationView::CorrelationItem h;
    h.data1=duplicateArray(data1, size);
    h.data2=duplicateArray(data2, size);
    h.data3=duplicateArray(data3, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms.append(h);

    return histograms.size()-1;
}

void QFParameterCorrelationView::setCorrelation(int i, QString name, double *data1, double *data2, int32_t size, bool external)
{
    if (i<0 || i>=histograms.size()) return;
    QFParameterCorrelationView::CorrelationItem h=histograms[i];
    if (h.data1 && !h.external) qfFree(h.data1);
    if (h.data2 && !h.external) qfFree(h.data2);
    h.data1=data1;
    h.data2=data2;
    if (!external) {
        h.data1=duplicateArray(data1, size);
        h.data2=duplicateArray(data2, size);
    }
    h.name=name;
    h.size=size;
    h.external=external;
    histograms[i]=h;
}

void QFParameterCorrelationView::setCorrelation(int i, QString name, double *data1, double *data2, double *data3, int32_t size, bool external)
{
    if (i<0 || i>=histograms.size()) return;
    QFParameterCorrelationView::CorrelationItem h=histograms[i];
    if (h.data1 && !h.external) qfFree(h.data1);
    if (h.data2 && !h.external) qfFree(h.data2);
    if (h.data3 && !h.external) qfFree(h.data3);
    h.data1=data1;
    h.data2=data2;
    h.data3=data3;
    if (!external) {
        h.data1=duplicateArray(data1, size);
        h.data2=duplicateArray(data2, size);
        h.data3=duplicateArray(data3, size);
    }
    h.name=name;
    h.size=size;
    h.external=external;
    histograms[i]=h;
    if (data3) setColorCodingMode(ccmBy3rdColumn);
}

void QFParameterCorrelationView::setCopiedCorrelation(int i, QString name, const double *data1, const double *data2, int32_t size)
{
    if (i<0 || i>=histograms.size()) return;
    QFParameterCorrelationView::CorrelationItem h=histograms[i];
    if (h.data1 && !h.external) qfFree(h.data1);
    if (h.data2 && !h.external) qfFree(h.data2);
    h.data1=duplicateArray(data1, size);
    h.data2=duplicateArray(data2, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms[i]=h;
}

void QFParameterCorrelationView::setCopiedCorrelation(int i, QString name, const double *data1, const double *data2, const double *data3, int32_t size)
{
    if (i<0 || i>=histograms.size()) return;
    QFParameterCorrelationView::CorrelationItem h=histograms[i];
    if (h.data1 && !h.external) qfFree(h.data1);
    if (h.data2 && !h.external) qfFree(h.data2);
    if (h.data3 && !h.external) qfFree(h.data3);
    h.data1=duplicateArray(data1, size);
    h.data2=duplicateArray(data2, size);
    h.data3=duplicateArray(data3, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms[i]=h;
    if (data3) setColorCodingMode(ccmBy3rdColumn);
}

void QFParameterCorrelationView::removeCorrelation(int i)
{
    if (i<0 || i>=histograms.size()) return;
    if (histograms[i].data1 && !histograms[i].external) qfFree(histograms[i].data1);
    if (histograms[i].data2 && !histograms[i].external) qfFree(histograms[i].data2);
    histograms.removeAt(i);
}

int QFParameterCorrelationView::CorrelationCount() const
{
    return histograms.size();
}

void QFParameterCorrelationView::setCorrelation1Label(const QString label, bool update)
{
    histLabelX=label;
    if (update) replotCorrelation();
}

void QFParameterCorrelationView::setCorrelation2Label(const QString label, bool update)
{
    histLabelY=label;
    if (update) replotCorrelation();
}

void QFParameterCorrelationView::setCorrelationColorLabel(const QString label, bool update)
{
    histLabelColor=label;
    if (update) replotCorrelation();
}

void QFParameterCorrelationView::writeReport(QTextCursor &cursor, QTextDocument *document)
{
    repaint();
    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);


    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    fHeading1.setFontPointSize(1.4*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    QTextCharFormat fHeading2=fText;
    fHeading2.setFontPointSize(1.2*fText.fontPointSize());
    fHeading2.setFontWeight(QFont::Bold);
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTableFormat tableFormat1=tableFormat;
    tableFormat1.setWidth(QTextLength(QTextLength::VariableLength, 1));


    double w1=qMax(1,pltParamCorrelation->width());
    double w2=qMax(1,tvHistogramParameters->width());
    double allwidth=qMax(1.0,w1+w2);
    cursor.insertBlock();
    cursor.insertText(tr("Parameter Correlations:\n"), fHeading2);
    QTextTable* table = cursor.insertTable(3,3, tableFormat1);
    //table->mergeCells(0,2,2,1);
    table->mergeCells(2,0,1,3);
    {
        double scale=1.0;
        QTextCursor tabCursor=table->cellAt(0,1).firstCursorPosition();
        {
            QPicture pic;
            JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
            pltParamCorrelation->get_plotter()->draw(*painter, QRect(0,0,pltParamCorrelation->width(),pltParamCorrelation->height()));
            delete painter;
            scale=0.7*document->textWidth()/double(pic.boundingRect().width());
            if (scale<=0) scale=1;
            insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.5);
        }

        tabCursor=table->cellAt(0, 0).firstCursorPosition();
        {
            QPicture pic;
            JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
            pltParamHistogramY->get_plotter()->draw(*painter, QRect(0,0,pltParamCorrelation->width(),pltParamCorrelation->height()));
            delete painter;
            insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.18);
        }
        tabCursor=table->cellAt(1, 1).firstCursorPosition();
        {
            QPicture pic;
            JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
            pltParamHistogramX->get_plotter()->draw(*painter, QRect(0,0,pltParamCorrelation->width(),pltParamCorrelation->height()));
            delete painter;
            insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.5);
        }

        tabCursor=table->cellAt(2,0).firstCursorPosition();
        //tabCursor.insertText(tr("\n"), fTextBoldSmall);
        //tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><nobr><span style=\"font-size: 7pt;\">%1</span></nobr></center>").arg(tvHistogramParameters->toHtml(1,true))));
        QPicture picT;
        QPainter* painter=new QPainter(&picT);
        tvHistogramParameters->paint(*painter);
        delete painter;
        scale=0.3*document->textWidth()/double(picT.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("statistics table:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, picT, QSizeF(picT.boundingRect().width(), picT.boundingRect().height())*scale, 0.3);

    }
    cursor.movePosition(QTextCursor::End);

}

void QFParameterCorrelationView::setSpaceSavingMode(bool enabled)
{
    laySplitterTable->removeWidget(grpHistogramSettings);
    layHist->removeWidget(grpHistogramSettings);
    if (enabled) {
        laySplitterTable->insertWidget(0, grpHistogramSettings);
    } else {
        layHist->addWidget(grpHistogramSettings, 0, 1);
        layHist->setColumnStretch(0,5);
    }
}

double QFParameterCorrelationView::getParam1Min() const
{
    return edtHistogramMin1->value();
}

double QFParameterCorrelationView::getParam1Max() const
{
    return edtHistogramMax1->value();
}

double QFParameterCorrelationView::getParam2Min() const
{
    return edtHistogramMin2->value();
}

double QFParameterCorrelationView::getParam2Max() const
{
    return edtHistogramMax2->value();
}

double QFParameterCorrelationView::getParamColorMin() const
{
    return edtColorMin->value();
}

double QFParameterCorrelationView::getParamColorMax() const
{
    return edtColorMax->value();
}

void QFParameterCorrelationView::setCorrelationHistogramStretch(int corrStretch, int histStretch)
{
    layPlots->setColumnStretch(0,histStretch);
    layPlots->setColumnStretch(1,corrStretch);
    layPlots->setRowStretch(1,corrStretch);
    layPlots->setRowStretch(2,histStretch);
}

double QFParameterCorrelationView::getCurrentRangeSelectionXMin() const
{
    return currentRangeSelection_xmin;
}
double QFParameterCorrelationView::getCurrentRangeSelectionXMax() const
{
    return currentRangeSelection_xmax;
}
double QFParameterCorrelationView::getCurrentRangeSelectionYMin() const
{
    return currentRangeSelection_ymin;
}
double QFParameterCorrelationView::getCurrentRangeSelectionYMax() const
{
    return currentRangeSelection_ymax;
}


void QFParameterCorrelationView::setRangeSelectionMode(bool enabled)
{
    currentRangeSelection_xmin=0;
    currentRangeSelection_xmax=0;
    currentRangeSelection_ymin=0;
    currentRangeSelection_ymax=0;
    plteRangeSelection->set_visible(enabled);

    if (enabled) {
        pltParamCorrelation->set_mouseActionMode(QFPlotter::RectangleEvents);
        connect(pltParamCorrelation, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(rangeSelectionRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    } else {
        pltParamCorrelation->set_mouseActionMode(QFPlotter::ZoomRectangle);
        disconnect(pltParamCorrelation, SIGNAL(userRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)), this, SLOT(rangeSelectionRectangleFinished(double,double,double,double,Qt::KeyboardModifiers)));
    }

    UpdateSelectionRangeSelected();
}

void QFParameterCorrelationView::setrangeSelection(double xmin, double xmax, double ymin, double ymax)
{
    currentRangeSelection_xmin=xmin;
    currentRangeSelection_xmax=xmax;
    currentRangeSelection_ymin=ymin;
    currentRangeSelection_ymax=ymax;
    UpdateSelectionRangeSelected();
}

void QFParameterCorrelationView::clearRangeSelection()
{
    currentRangeSelection_xmin=0;
    currentRangeSelection_xmax=0;
    currentRangeSelection_ymin=0;
    currentRangeSelection_ymax=0;
    UpdateSelectionRangeSelected();
}


void QFParameterCorrelationView::dataSettingsChanged(bool update)
{
    edtHistogramMin1->setEnabled(chkHistogramRangeManual1->isChecked());
    edtHistogramMax1->setEnabled(chkHistogramRangeManual1->isChecked());
    spinHistogramQL1->setEnabled(chkHistogramRangeRelaxAuto1->isChecked());
    spinHistogramQU1->setEnabled(chkHistogramRangeRelaxAuto1->isChecked());
    edtHistogramMin2->setEnabled(chkHistogramRangeManual2->isChecked());
    edtHistogramMax2->setEnabled(chkHistogramRangeManual2->isChecked());
    spinHistogramQL2->setEnabled(chkHistogramRangeRelaxAuto2->isChecked());
    spinHistogramQU2->setEnabled(chkHistogramRangeRelaxAuto2->isChecked());
    edtColorMin->setEnabled(chkColorRangeManual->isChecked());
    edtColorMax->setEnabled(chkColorRangeManual->isChecked());
    spinColQL->setEnabled(chkColorRangeRelaxAuto->isChecked());
    spinColQU->setEnabled(chkColorRangeRelaxAuto->isChecked());
    if (update) updateCorrelation(true);
    emit settingsChanged();
}

void QFParameterCorrelationView::showPlotPosition(double x, double y)
{
    labPlotPos->setText(tr("(%1, %2").arg(x).arg(y));
}

void QFParameterCorrelationView::setColorCodingMode(QFParameterCorrelationView::ColorCodingMode colorcoding)
{
    bool changed=(this->colorcoding!=colorcoding);
    this->colorcoding=colorcoding;

    labColorCodingHeader->setVisible(colorcoding==ccmBy3rdColumn);
    labColorCodingHeader1->setVisible(colorcoding==ccmBy3rdColumn);
    labColorCodingHeader2->setVisible(colorcoding==ccmBy3rdColumn);
    labColorCodingHeader3->setVisible(colorcoding==ccmBy3rdColumn);
    cmbColorScale->setVisible(colorcoding==ccmBy3rdColumn);
    widColorCodingHeader->setVisible(colorcoding==ccmBy3rdColumn);
    widColorCodingHeader2->setVisible(colorcoding==ccmBy3rdColumn);
    if (changed) updateCorrelation(true);
}





void QFParameterCorrelationView::printReport()
{
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTextDocument* doc=new QTextDocument();
    doc->setTextWidth(p->pageRect().size().width());
    QTextCursor cur(doc);
    writeReport(cur, doc);
    doc->print(p);
    delete p;
    delete doc;
    QApplication::restoreOverrideCursor();
}

void QFParameterCorrelationView::saveReport()
{
    QTextDocument* doc=new QTextDocument();
    QTextCursor cur(doc);
    writeReport(cur, doc);
    qfSaveReport(doc, QString("QuickFit %1 Parameter Correlation Report").arg(qfInfoVersionFull()), QString("QFParameterCorrelationView"), this);
    delete doc;

}





void QFParameterCorrelationView::createWidgets()
{

    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widHist=this;
    layHist=new QGridLayout();
    widHist->setLayout(layHist);
    QHBoxLayout* hbrange;

    // HISTOGRAM SETTINGS/////////////////////////////////////////////////////////////////////
    QHBoxLayout* coll;
    grpHistogramSettings=new QGroupBox(tr("plot style"), this);
    flHistSet=new QFormLayout();
    grpHistogramSettings->setLayout(flHistSet);



    coll=new QHBoxLayout();
    coll->addStretch();
    coll->setContentsMargins(0,0,0,0);
    QToolButton* tb;
    actHelp=createActionAndButton(tb, QIcon(":/lib/help.png"), tr("show help for histogram widget"), this);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    coll->addWidget(tb);

    actPrintReport=createActionAndButton(tb, QIcon(":/lib/printreport.png"), tr("Print Histogram Report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    coll->addWidget(tb);
    actSaveReport=createActionAndButton(tb, QIcon(":/lib/savereport.png"), tr("Save Histogram Report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    coll->addWidget(tb);


    actCopyData=createActionAndButton(tb, QIcon(":/lib/copy16.png"), tr("Copy data used for histogram"), this);
    connect(actCopyData, SIGNAL(triggered()), this, SLOT(copyData()));
    coll->addWidget(tb);
    actCopyDataMatlab=createActionAndButton(tb, QIcon(":/lib/copy16_matlab.png"), tr("Copy data used for histogram as Matlab script"), this);
    connect(actCopyDataMatlab, SIGNAL(triggered()), this, SLOT(copyDataMatlab()));
    coll->addWidget(tb);
    actSaveData=createActionAndButton(tb, QIcon(":/lib/savedata.png"), tr("Save data used for histogram"), this);
    connect(actSaveData, SIGNAL(triggered()), this, SLOT(saveData()));
    coll->addWidget(tb);

    flHistSet->addRow("", coll);


    cmbSymbol=new JKQTPSymbolComboBox(this);
    cmbSymbol->setCurrentSymbol(JKQTPfilledCircle);

    chkScatterPlot=new QCheckBox("", this);
    chkScatterPlot->setChecked(true);
    cmb2DHistogram=new QComboBox(this);
    cmb2DHistogram->addItem(tr("none"));
    cmb2DHistogram->addItem(tr("histogram"));
    cmb2DHistogram->addItem(tr("kernel density"));
    cmb2DHistogram->setCurrentIndex(2);
    coll=new QHBoxLayout();
    coll->addWidget(new QLabel(tr("scatter: ")));
    coll->addWidget(chkScatterPlot,1);
    coll->addSpacing(16);
    coll->addWidget(new QLabel(tr("density: ")));
    coll->addWidget(cmb2DHistogram,1);
    coll->addStretch();
    coll->setContentsMargins(0,0,0,0);
    flHistSet->addRow(tr("<b>plots:</b>"), coll);

    spin2DHistogramBins1=new QSpinBox(this);
    spin2DHistogramBins1->setRange(5, 1000);
    spin2DHistogramBins1->setValue(35);
    spin2DHistogramBins2=new QSpinBox(this);
    spin2DHistogramBins2->setRange(5, 1000);
    spin2DHistogramBins2->setValue(35);
    coll=new QHBoxLayout();
    coll->addWidget(new QLabel("x: "));
    coll->addWidget(spin2DHistogramBins1,1);
    coll->addWidget(new QLabel("    y: "));
    coll->addWidget(spin2DHistogramBins2,1);
    coll->addStretch();
    coll->setContentsMargins(0,0,0,0);
    flHistSet->addRow(tr("density bins:"), coll);
    connect(cmb2DHistogram, SIGNAL(currentIndexChanged(int)), this, SLOT(histTypeChanged(int)));
    histTypeChanged(cmb2DHistogram->currentIndex());


    flHistSet->addRow(tr("<b>symbol:</b>"), cmbSymbol);
    spinSymbolSize=new QSpinBox(this);
    spinSymbolSize->setRange(1,30);
    spinSymbolSize->setValue(4);
    flHistSet->addRow(tr("<b>symbol size:</b>"), spinSymbolSize);
    connect(chkScatterPlot, SIGNAL(toggled(bool)), cmbSymbol, SLOT(setEnabled(bool)));
    connect(chkScatterPlot, SIGNAL(toggled(bool)), spinSymbolSize, SLOT(setEnabled(bool)));

    chkKey=new QCheckBox(this);
    chkKey->setChecked(false);
    flHistSet->addRow(tr("<b>keys visible:</b>"), chkKey);



    spinHistogramBins1=new QSpinBox(this);
    spinHistogramBins1->setRange(5,99999);
    spinHistogramBins1->setValue(50);
    coll=new QHBoxLayout();
    coll->setContentsMargins(0,0,0,0);
    coll->addWidget(spinHistogramBins1);
    coll->addStretch();
    flHistSet->addRow(tr("<b>Data X:</b>"), new QWidget(this));
    flHistSet->addRow(tr("  # bins:"), coll);

    chkHistogramRangeManual1=new QRadioButton("manual", grpHistogramSettings);
    chkHistogramRangeAuto1=new QRadioButton("auto", grpHistogramSettings);
    chkHistogramRangeRelaxAuto1=new QRadioButton("relaxed:", grpHistogramSettings);
    chkHistogramRangeAuto1->setChecked(true);
    QButtonGroup* bg=new QButtonGroup(this);
    bg->addButton(chkHistogramRangeManual1);
    bg->addButton(chkHistogramRangeAuto1);
    bg->addButton(chkHistogramRangeRelaxAuto1);
    spinHistogramQL1=new QDoubleSpinBox(this);
    spinHistogramQL1->setRange(0,100);
    spinHistogramQL1->setSuffix("%");
    spinHistogramQL1->setValue(5);
    spinHistogramQU1=new QDoubleSpinBox(this);
    spinHistogramQU1->setRange(0,100);
    spinHistogramQU1->setSuffix("%");
    spinHistogramQU1->setValue(5);
    hbrange=new QHBoxLayout();
    hbrange->addWidget(chkHistogramRangeManual1);
    hbrange->addWidget(chkHistogramRangeAuto1);
    hbrange->addWidget(chkHistogramRangeRelaxAuto1);
    hbrange->addWidget(spinHistogramQL1);
    hbrange->addWidget(new QLabel("..."));
    hbrange->addWidget(spinHistogramQU1);
    hbrange->addStretch();
    flHistSet->addRow(tr("  range:"), hbrange);

    edtHistogramMin1=new QFDoubleEdit(this);
    edtHistogramMin1->setCheckBounds(false, false);
    edtHistogramMin1->setValue(0);
    edtHistogramMax1=new QFDoubleEdit(this);
    edtHistogramMax1->setCheckBounds(false, false);
    edtHistogramMax1->setValue(10);
    coll=new QHBoxLayout();
    coll->addWidget(edtHistogramMin1,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtHistogramMax1,1);
    coll->addStretch();
    coll->setContentsMargins(0,0,0,0);
    flHistSet->addRow(QString(""), coll);

    flHistSet->addRow(tr("<b>Data Y:</b>"), new QWidget(this));
    spinHistogramBins2=new QSpinBox(this);
    spinHistogramBins2->setRange(5,99999);
    spinHistogramBins2->setValue(50);
    coll=new QHBoxLayout();
    coll->addWidget(spinHistogramBins2);
    coll->addStretch();
    flHistSet->addRow(tr("  # bins:"), coll);

    chkHistogramRangeManual2=new QRadioButton("manual", grpHistogramSettings);
    chkHistogramRangeAuto2=new QRadioButton("auto", grpHistogramSettings);
    chkHistogramRangeRelaxAuto2=new QRadioButton("relaxed:", grpHistogramSettings);
    chkHistogramRangeAuto2->setChecked(true);
    bg=new QButtonGroup(this);
    bg->addButton(chkHistogramRangeManual2);
    bg->addButton(chkHistogramRangeAuto2);
    bg->addButton(chkHistogramRangeRelaxAuto2);

    spinHistogramQL2=new QDoubleSpinBox(this);
    spinHistogramQL2->setRange(0,100);
    spinHistogramQL2->setSuffix("%");
    spinHistogramQL2->setValue(5);
    spinHistogramQU2=new QDoubleSpinBox(this);
    spinHistogramQU2->setRange(0,100);
    spinHistogramQU2->setSuffix("%");
    spinHistogramQU2->setValue(5);
    hbrange=new QHBoxLayout();
    hbrange->addWidget(chkHistogramRangeManual2);
    hbrange->addWidget(chkHistogramRangeAuto2);
    hbrange->addWidget(chkHistogramRangeRelaxAuto2);
    hbrange->addWidget(spinHistogramQL2);
    hbrange->addWidget(new QLabel("..."));
    hbrange->addWidget(spinHistogramQU2);
    hbrange->addStretch();
    flHistSet->addRow(tr("  range:"), hbrange);

    edtHistogramMin2=new QFDoubleEdit(this);
    edtHistogramMin2->setCheckBounds(false, false);
    edtHistogramMin2->setValue(0);
    edtHistogramMax2=new QFDoubleEdit(this);
    edtHistogramMax2->setCheckBounds(false, false);
    edtHistogramMax2->setValue(10);
    coll=new QHBoxLayout();
    coll->addWidget(edtHistogramMin2,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtHistogramMax2,1);
    coll->addStretch();
    coll->setContentsMargins(0,0,0,0);
    flHistSet->addRow(QString(""), coll);





    flHistSet->addRow(labColorCodingHeader=new QLabel(tr("<b>Data Color:</b>")), new QWidget(this));
    cmbColorScale=new JKQTPMathImageColorPaletteComboBox(this);
    cmbColorScale->setCurrentIndex(JKQTPMathImageMATLAB);
    flHistSet->addRow(labColorCodingHeader1=new QLabel(tr("  color palette:")), cmbColorScale);
    chkColorRangeManual=new QRadioButton(tr("manual"), grpHistogramSettings);
    chkColorRangeAuto=new QRadioButton(tr("auto"), grpHistogramSettings);
    chkColorRangeRelaxAuto=new QRadioButton(tr("relaxed:"), grpHistogramSettings);
    bg=new QButtonGroup(this);
    bg->addButton(chkColorRangeManual);
    bg->addButton(chkColorRangeAuto);
    bg->addButton(chkColorRangeRelaxAuto);

    spinColQL=new QDoubleSpinBox(this);
    spinColQL->setRange(0,100);
    spinColQL->setSuffix("%");
    spinColQL->setValue(5);
    spinColQU=new QDoubleSpinBox(this);
    spinColQU->setRange(0,100);
    spinColQU->setSuffix("%");
    spinColQU->setValue(5);
    widColorCodingHeader=new QWidget(this);
    hbrange=new QHBoxLayout();
    hbrange->setContentsMargins(0,0,0,0);
    hbrange->addWidget(chkColorRangeManual);
    hbrange->addWidget(chkColorRangeAuto);
    hbrange->addWidget(chkColorRangeRelaxAuto);
    hbrange->addWidget(spinColQL);
    hbrange->addWidget(new QLabel("..."));
    hbrange->addWidget(spinColQU);
    hbrange->addStretch();
    widColorCodingHeader->setLayout(hbrange);
    flHistSet->addRow(labColorCodingHeader2=new QLabel(tr("  range:")), widColorCodingHeader);
    edtColorMin=new QFDoubleEdit(this);
    edtColorMin->setCheckBounds(false, false);
    edtColorMin->setValue(0);
    edtColorMax=new QFDoubleEdit(this);
    edtColorMax->setCheckBounds(false, false);
    edtColorMax->setValue(10);
    widColorCodingHeader2=new QWidget(this);
    coll=new QHBoxLayout();
    coll->addWidget(edtColorMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtColorMax,1);
    coll->addStretch();
    coll->setContentsMargins(0,0,0,0);
    widColorCodingHeader2->setLayout(coll);
    flHistSet->addRow(labColorCodingHeader3=new QLabel(""), widColorCodingHeader2);



    // HISTOGRAM PLOTS ///////////////////////////////////////////////////////////////////////
    splitterHistogram=new QVisibleHandleSplitter(this);
    QWidget* widPlotter=new QWidget(this);
    layPlots=new QGridLayout();
    widPlotter->setLayout(layPlots);

    labPlotPos=new QLabel(this);
    layPlots->addWidget(labPlotPos, 0, 1);

    pltParamCorrelation=new QFPlotter(this);
    pltParamCorrelation->setMinimumWidth(100);
    pltParamCorrelation->set_displayToolbar(false);
    pltParamCorrelation->set_displayMousePosition(false);
    connect(pltParamCorrelation, SIGNAL(plotMouseMove(double,double)), this, SLOT(showPlotPosition(double,double)));

    plteRangeSelection=new JKQTPoverlayRectangle(0,0,0,0, pltParamCorrelation->get_plotter());
    plteRangeSelection->set_color(QColor("black"));
    plteRangeSelection->set_lineWidth(1);
    plteRangeSelection->set_lineStyle(Qt::DashLine);
    plteRangeSelection->set_visible(false);
    plteRangeSelection->set_fillColor(Qt::transparent);
    pltParamCorrelation->get_plotter()->addOverlayElement(plteRangeSelection);

    pltParamHistogramX=new QFPlotter(false, this, pltParamCorrelation->getDatastore());
    pltParamHistogramX->setMinimumWidth(100);
    pltParamHistogramX->get_plotter()->synchronizeToMaster(pltParamCorrelation->get_plotter(), true, false);
    pltParamHistogramX->getYAxis()->set_inverted(true);
    pltParamHistogramX->getXAxis()->set_drawMode1(JKQTPCADMticks);
    pltParamHistogramX->get_plotter()->set_showKey(false);
    pltParamHistogramX->set_displayToolbar(false);
    pltParamHistogramX->set_displayMousePosition(false);
    connect(pltParamCorrelation, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), pltParamHistogramX, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));
    connect(pltParamHistogramX, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), pltParamCorrelation, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));
    connect(pltParamHistogramX, SIGNAL(plotMouseMove(double,double)), this, SLOT(showPlotPosition(double,double)));

    pltParamHistogramY=new QFPlotter(false, this, pltParamCorrelation->getDatastore());
    pltParamHistogramY->setMinimumWidth(100);
    pltParamHistogramY->get_plotter()->synchronizeToMaster(pltParamCorrelation->get_plotter(), false, true);
    pltParamHistogramY->getXAxis()->set_inverted(true);
    pltParamHistogramY->getYAxis()->set_drawMode1(JKQTPCADMticks);
    pltParamHistogramY->get_plotter()->set_showKey(false);
    pltParamHistogramY->set_displayToolbar(false);
    pltParamHistogramY->set_displayMousePosition(false);
    connect(pltParamCorrelation, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), pltParamHistogramY, SLOT(synchronizeYAxis(double,double,double,double,JKQtPlotter*)));
    connect(pltParamHistogramY, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), pltParamCorrelation, SLOT(synchronizeYAxis(double,double,double,double,JKQtPlotter*)));
    connect(pltParamHistogramY, SIGNAL(plotMouseMove(double,double)), this, SLOT(showPlotPosition(double,double)));


    pltParamHistogramX->get_plotter()->addGridPrintingPlotter(0,1,pltParamCorrelation->get_plotter());
    pltParamHistogramX->get_plotter()->addGridPrintingPlotter(1,1,pltParamHistogramY->get_plotter());

    layPlots->addWidget(pltParamCorrelation, 1,1);
    layPlots->addWidget(pltParamHistogramX, 2,1);
    layPlots->addWidget(pltParamHistogramY, 1,0);
    layPlots->setColumnStretch(0,1);
    layPlots->setColumnStretch(1,3);
    layPlots->setRowStretch(1,3);
    layPlots->setRowStretch(2,1);

    tvHistogramParameters=new QEnhancedTableView(this);
    tabHistogramParameters=new QFTableModel(this);
    tabHistogramParameters->setReadonly(false);
    tabHistogramParameters->clear();
    tabHistogramParameters->appendColumn();
    tabHistogramParameters->setColumnTitle(0, tr("parameter"));
    tabHistogramParameters->appendColumn();
    tabHistogramParameters->setColumnTitle(1, tr("complete histogram"));
    tabHistogramParameters->appendColumn();
    tabHistogramParameters->setColumnTitle(2, tr("selection histogram"));
    //for (int r=0; r<8; r++) tabHistogramParameters->appendRow();
    int col=0;
    tabHistogramParameters->setCellCreate(col++, 0, tr("correlation r<sub>X,Y</sub> [%]"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("regression offset"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("regression factor"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("IRLS regr. offset"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("IRLS regr. factor"));


    tabHistogramParameters->setCellCreate(col++, 0, tr("X: data points N"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: average"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: median"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: std. dev. &sigma;"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: min"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: 25% quantile"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: 75% quantile"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: max"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: skewness &gamma;<sub>1</sub>"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("X: invalid values"));

    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: data points N"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: average"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: median"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: std. dev. &sigma;"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: min"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: 25% quantile"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: 75% quantile"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: max"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: skewness &gamma;<sub>1</sub>"));
    tabHistogramParameters->setCellCreate(col++, 0, tr("Y: invalid values"));
    tabHistogramParameters->setReadonly(true);

    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->setItemDelegate(new QFHTMLDelegate(tvHistogramParameters));
    tvHistogramParameters->setAlternatingRowColors(true);
    QFontMetrics tvfm(font());
    tvHistogramParameters->verticalHeader()->setDefaultSectionSize((int)round((double)tvfm.height()*1.5));


    QWidget* widHTab=new QWidget(this);
    laySplitterTable=new QVBoxLayout();
    laySplitterTable->setContentsMargins(0,0,0,0);
    widHTab->setLayout(laySplitterTable);
    laySplitterTable->addWidget(tvHistogramParameters);


    splitterHistogram->setChildrenCollapsible(false);
    splitterHistogram->addWidget(widPlotter);
    splitterHistogram->addWidget(widHTab);
    layHist->addWidget(splitterHistogram, 0, 0);
    layHist->addWidget(grpHistogramSettings, 0, 1);
    splitterHistogram->setStretchFactor(0,1);
    splitterHistogram->setStretchFactor(1,0);
    layHist->setColumnStretch(0,5);
    layHist->setContentsMargins(0,0,0,0);

    //connectParameterWidgets(true);

    setSpaceSavingMode(true);
}

void QFParameterCorrelationView::histTypeChanged(int index)
{
    spin2DHistogramBins1->setEnabled(index>0);
    spin2DHistogramBins2->setEnabled(index>0);
}


void QFParameterCorrelationView::copyData()
{
    QList<QVector<double> > data;
    QStringList headers;
    fillDataArray(data, headers);

    csvCopy(data, headers);
}

void QFParameterCorrelationView::copyDataMatlab()
{
    QList<QVector<double> > data;
    QStringList headers;
    fillDataArray(data, headers);

    matlabCopy(data);
}

void QFParameterCorrelationView::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("qfparametercorrelation.html");
}

void QFParameterCorrelationView::saveData()
{
    QList<QVector<double> > data;
    QStringList headers;
    fillDataArray(data, headers);
    QStringList f=QFDataExportHandler::getFormats();
    QString lastDir=ProgramOptions::getConfigValue("QFParameterCorrelationView/lastDataDir", "").toString();
    QString selFilter=ProgramOptions::getConfigValue("QFParameterCorrelationView/lastDataFilter", "").toString();
    QString fn=qfGetSaveFileName(this, tr("Save data to file"), lastDir, f.join(";;"), &selFilter);
    if (fn.size()>0) {
        QFDataExportHandler::save(data, f.indexOf(selFilter), fn, headers, QStringList());
        ProgramOptions::setConfigValue("QFParameterCorrelationView/lastDataDir", lastDir);
        ProgramOptions::setConfigValue("QFParameterCorrelationView/lastDataFilter", selFilter);
    }
}

void QFParameterCorrelationView::fillDataArray(QList<QVector<double> > &data, QStringList& headers)
{
    data.clear();
    headers.clear();
    for (int i=0; i<histograms.size(); i++) {
        {
            QVector<double> d(histograms[i].size, 0.0);
            memcpy(d.data(), histograms[i].data1, histograms[i].size*sizeof(double));
            data.append(d);
        }
        {
            QVector<double> d(histograms[i].size, 0.0);
            memcpy(d.data(), histograms[i].data2, histograms[i].size*sizeof(double));
            data.append(d);
        }
        if (histograms[i].data3){
            QVector<double> d(histograms[i].size, 0.0);
            memcpy(d.data(), histograms[i].data3, histograms[i].size*sizeof(double));
            data.append(d);
        }
        headers.append(histLabelX+" ("+histograms[i].name+")");
        headers.append(histLabelY+" ("+histograms[i].name+")");
        if (histograms[i].data3){
            headers.append(histLabelColor+" ("+histograms[i].name+")");
        }
    }
}

void QFParameterCorrelationView::rangeSelectionRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers);
    currentRangeSelection_xmin=qMin(x, x+width);
    currentRangeSelection_xmax=qMax(x, x+width);
    currentRangeSelection_ymin=qMin(y, y+height);
    currentRangeSelection_ymax=qMax(y, y+height);
    UpdateSelectionRangeSelected();
}

void QFParameterCorrelationView::UpdateSelectionRangeSelected()
{
    plteRangeSelection->set_x1(currentRangeSelection_xmin);
    plteRangeSelection->set_y1(currentRangeSelection_ymin);
    plteRangeSelection->set_x2(currentRangeSelection_xmax);
    plteRangeSelection->set_y2(currentRangeSelection_ymax);
    pltParamCorrelation->update_overlays();
    //qDebug()<<currentRangeSelection_xmin<<currentRangeSelection_xmax<<currentRangeSelection_ymin<<currentRangeSelection_ymax;
    emit rangeSelected(currentRangeSelection_xmin, currentRangeSelection_xmax, currentRangeSelection_ymin, currentRangeSelection_ymax);
}
