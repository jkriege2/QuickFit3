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

#include "qfhistogramview.h"
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
#include "qftools.h"
#include "matlabtools.h"
#include "csvtools.h"
#include "programoptions.h"
#include <QProgressDialog>
#include "datatools.h"
#include "jkqtptools.h"
#include "qffitalgorithm.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunction.h"
#include "qffitfunctionmanager.h"
#include "qffitfunctionplottools.h"


QFHistogramView::QFHistogramView(QWidget *parent) :
    QWidget(parent)
{
    connectParameterWidgetsCounter=0;
    histLabel="";

    createWidgets();
}

QFHistogramView::~QFHistogramView() {
    for (int i=0; i<histograms.size(); i++) {
        if (histograms[i].data && !histograms[i].external) qfFree(histograms[i].data);
    }
    histograms.clear();
}

void QFHistogramView::createWidgets() {

    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widHist=this;
    layHist=new QGridLayout();
    widHist->setLayout(layHist);

    // HISTOGRAM SETTINGS/////////////////////////////////////////////////////////////////////
    QHBoxLayout* coll;
    grpHistogramSettings=new QGroupBox(tr("histogram style"), this);
    flHistSet=new QFormLayout();
    grpHistogramSettings->setLayout(flHistSet);
    spinHistogramBins=new QSpinBox(this);
    spinHistogramBins->setRange(5,99999);
    spinHistogramBins->setValue(50);
    coll=new QHBoxLayout();
    coll->addWidget(spinHistogramBins);
    coll->addStretch();

    QToolButton* tb;
    actHelp=createActionAndButton(tb, QIcon(":/lib/help.png"), tr("show help for histogram widget"), this);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    coll->addWidget(tb);
    actPrintReport=createActionAndButton(tb, QIcon(":/lib/printreport.png"), tr("Print histogram report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    coll->addWidget(tb);
    actSaveReport=createActionAndButton(tb, QIcon(":/lib/savereport.png"), tr("Save histogram report"), this);
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




    edtHistogramRelaxedRangePercent=new QDoubleSpinBox(this);
    edtHistogramRelaxedRangePercent->setRange(0,100);
    edtHistogramRelaxedRangePercent->setSuffix(" %");
    edtHistogramRelaxedRangePercent->setValue(5);
    edtHistogramRelaxedRangePercentUp=new QDoubleSpinBox(this);
    edtHistogramRelaxedRangePercentUp->setRange(0,100);
    edtHistogramRelaxedRangePercentUp->setSuffix(" %");
    edtHistogramRelaxedRangePercentUp->setValue(5);
    flHistSet->addRow(tr("# bins:"), coll);
    QHBoxLayout* layHistogram=new QHBoxLayout();
    chkLogHistogram=new QCheckBox(tr("log-scale"), grpHistogramSettings);
    chkNormalizedHistograms=new QCheckBox(tr("normalized"), grpHistogramSettings);
    chkKey=new QCheckBox(tr("show key"),this);
    chkKey->setChecked(true);
    layHistogram->addWidget(chkLogHistogram);
    layHistogram->addSpacing(15);
    layHistogram->addWidget(chkNormalizedHistograms);
    layHistogram->addSpacing(15);
    layHistogram->addWidget(chkKey);
    layHistogram->addStretch();
    flHistSet->addRow(tr(""), layHistogram);

    chkHistogramRangeAuto=new QRadioButton("auto", grpHistogramSettings);
    chkHistogramRangeAuto->setChecked(true);
    chkHistogramRangeRelaxAuto=new QRadioButton("relaxed auto: ", grpHistogramSettings);
    chkHistogramRangeManual=new QRadioButton("manual", grpHistogramSettings);
    QHBoxLayout* layradAuto=new QHBoxLayout();
    layradAuto->addWidget(chkHistogramRangeManual);
    layradAuto->addWidget(chkHistogramRangeAuto);
    layradAuto->addWidget(chkHistogramRangeRelaxAuto);
    layradAuto->addWidget(edtHistogramRelaxedRangePercent);
    layradAuto->addWidget(new QLabel(tr("  up:")));
    layradAuto->addWidget(edtHistogramRelaxedRangePercentUp);
    layradAuto->addStretch();
    flHistSet->addRow(tr("range:"), layradAuto);
    edtHistogramMin=new QFDoubleEdit(this);
    edtHistogramMin->setCheckBounds(false, false);
    edtHistogramMin->setValue(0);
    edtHistogramMax=new QFDoubleEdit(this);
    edtHistogramMax->setCheckBounds(false, false);
    edtHistogramMax->setValue(10);
    coll=new QHBoxLayout();
    coll->addWidget(edtHistogramMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtHistogramMax,1);
    coll->addStretch();
    coll->setContentsMargins(0,0,0,0);
    flHistSet->addRow(QString(""), coll);


    cmbFitFunction=new QFFitFunctionComboBox(this);
    flHistSet->addRow(tr("distribution fit:"), cmbFitFunction);



    // HISTOGRAM PLOTS ///////////////////////////////////////////////////////////////////////
    splitterHistogram=new QVisibleHandleSplitter(this);
    pltParamHistogram=new QFPlotter(this);
    pltParamHistogram->setMinimumWidth(100);
    pltParamHistogram->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
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
    tabHistogramParameters->setCellCreate(0, 0, tr("data points N"));
    tabHistogramParameters->setCellCreate(1, 0, tr("average"));
    tabHistogramParameters->setCellCreate(2, 0, tr("median"));
    tabHistogramParameters->setCellCreate(3, 0, tr("std. dev. &sigma;"));
    tabHistogramParameters->setCellCreate(4, 0, tr("norm. median abs. dev. NMAD"));
    tabHistogramParameters->setCellCreate(5, 0, tr("min"));
    tabHistogramParameters->setCellCreate(6, 0, tr("25% quantile"));
    tabHistogramParameters->setCellCreate(7, 0, tr("75% quantile"));
    tabHistogramParameters->setCellCreate(8, 0, tr("max"));
    tabHistogramParameters->setCellCreate(9, 0, tr("skewness &gamma;<sub>1</sub>"));
    tabHistogramParameters->setCellCreate(10, 0, tr("invalid values"));
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
    splitterHistogram->addWidget(pltParamHistogram);
    splitterHistogram->addWidget(widHTab);
    splitterHistogram->setStretchFactor(0,1);
    splitterHistogram->setStretchFactor(1,0);
    layHist->addWidget(splitterHistogram, 0, 0);
    layHist->addWidget(grpHistogramSettings, 0, 1);
    layHist->setColumnStretch(0,5);
    layHist->setContentsMargins(0,0,0,0);

    //connectParameterWidgets(true);

    setSpaceSavingMode(true);
    cmbFitFunction->updateFitFunctions("gen_pdf_,gen_,gendist_,dist_");
    cmbFitFunction->setCurrentFitFunction("gen_gaussian_sqrte");
    cmbFitFunction->setUpdatesEnabled(true);
    chkKey->setChecked(false);

}


void QFHistogramView::setSpaceSavingMode(bool enabled)
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

void QFHistogramView::addSettingsWidget(const QString& label, QWidget* widget) {
    flHistSet->addRow(label, widget);
}

bool QFHistogramView::getLog() const  {
    return chkLogHistogram->isChecked();
}

void QFHistogramView::setLog(bool enabled) {
    chkLogHistogram->setChecked(enabled);
}


int QFHistogramView::getBins() const {
    return spinHistogramBins->value();
}

void QFHistogramView::setBins(int bins) {
    spinHistogramBins->setValue(bins);
}


bool QFHistogramView::getNormalized() const {
    return chkNormalizedHistograms->isChecked();
}

void QFHistogramView::setNormalized(bool norm) {
    chkNormalizedHistograms->setChecked(norm);
}


bool QFHistogramView::getAutorange() const {
    return chkHistogramRangeAuto->isChecked();
}

void QFHistogramView::setAutorange(bool autorange) {
    chkHistogramRangeAuto->setChecked(autorange);
}


double QFHistogramView::getMin() const {
    return edtHistogramMin->value();
}

void QFHistogramView::setMin(double min) {
    edtHistogramMin->setValue(min);
}


double QFHistogramView::getMax() const {
    return edtHistogramMax->value();
}

void QFHistogramView::setMax(double max) {
    edtHistogramMax->setValue(max);
}

void QFHistogramView::connectParameterWidgets(bool connectTo) {
    if (connectTo) {
        //connectParameterWidgetsCounter--;
        //if (connectParameterWidgetsCounter<=0) {
        //    connectParameterWidgetsCounter=0;
            connect(chkLogHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(chkNormalizedHistograms, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
            connect(chkHistogramRangeAuto, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(chkHistogramRangeRelaxAuto, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(chkHistogramRangeManual, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(edtHistogramMin, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
            connect(edtHistogramRelaxedRangePercent, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
            connect(edtHistogramRelaxedRangePercentUp, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
            connect(edtHistogramMax, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
            connect(chkKey, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
            connect(cmbFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(histogramSettingsChanged()));
        //}
    } else {
        //connectParameterWidgetsCounter++;
        disconnect(chkLogHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkNormalizedHistograms, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkHistogramRangeAuto, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkHistogramRangeRelaxAuto, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkHistogramRangeManual, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMin, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramRelaxedRangePercent, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramRelaxedRangePercentUp, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMax, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkKey, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(cmbFitFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(histogramSettingsChanged()));
    }
}

void QFHistogramView::readSettings(QSettings& settings, const QString& prefix) {
    connectParameterWidgets(false);
    loadSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");
    setBins(settings.value(prefix+QString("bins"), 50).toInt());
    setNormalized(settings.value(prefix+QString("norm"), true).toBool());
    setLog(settings.value(prefix+QString("log"), false).toBool());
    setAutorange(settings.value(prefix+QString("rauto"), true).toBool());
    chkKey->setChecked(settings.value(prefix+QString("showkey"), true).toBool());
    cmbFitFunction->setCurrentFitFunction(settings.value(prefix+QString("distfit"), "gen_gaussian_sqrte").toString());
    edtHistogramRelaxedRangePercent->setValue(settings.value(prefix+QString("rangepercent"), 5).toDouble());
    edtHistogramRelaxedRangePercentUp->setValue(settings.value(prefix+QString("rangepercentup"), 5).toDouble());
    if (chkHistogramRangeManual->isChecked()) {
        setMin(settings.value(prefix+QString("rmin"), 0).toDouble());
        setMax(settings.value(prefix+QString("rmax"), 10).toDouble());
    }
    connectParameterWidgets(true);
}

void QFHistogramView::writeSettings(QSettings& settings, const QString& prefix) {
    saveSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");

    settings.setValue(prefix+QString("bins"), getBins());
    settings.setValue(prefix+QString("norm"), getNormalized());
    settings.setValue(prefix+QString("log"), getLog());
    settings.setValue(prefix+QString("rauto"), getAutorange());
    settings.setValue(prefix+QString("showkey"), chkKey->isChecked());
    settings.setValue(prefix+QString("distfit"), cmbFitFunction->currentFitFunctionID());
    settings.setValue(prefix+QString("rangepercent"), edtHistogramRelaxedRangePercent->value());
    settings.setValue(prefix+QString("rangepercentup"), edtHistogramRelaxedRangePercentUp->value());
    if (chkHistogramRangeManual->isChecked()) {
        settings.setValue(prefix+QString("rmin"), getMin());
        settings.setValue(prefix+QString("rmax"), getMax());
    }
}

void QFHistogramView::writeQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{
    current->setQFProperty(prefix+QString("bins_%1").arg(param), getBins(), false, false);
    current->setQFProperty(prefix+QString("norm_%1").arg(param), getNormalized(), false, false);
    current->setQFProperty(prefix+QString("log_%1").arg(param), getLog(), false, false);
    current->setQFProperty(prefix+QString("rauto_%1").arg(param), getAutorange(), false, false);
    current->setQFProperty(prefix+QString("showkey_%1").arg(param), chkKey->isChecked(), false, false);
    current->setQFProperty(prefix+QString("distfit_%1").arg(param), cmbFitFunction->currentFitFunctionID(), false, false);
    current->setQFProperty(prefix+QString("rangepercent_%1").arg(param), edtHistogramRelaxedRangePercent->value(), false, false);
    current->setQFProperty(prefix+QString("rangepercentup_%1").arg(param), edtHistogramRelaxedRangePercentUp->value(), false, false);
    if (chkHistogramRangeManual->isChecked()) {
        current->setQFProperty(prefix+QString("rmin_%1").arg(param), getMin(), false, false);
        current->setQFProperty(prefix+QString("rmax_%1").arg(param), getMax(), false, false);
    }
}

void QFHistogramView::readQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{
    setBins(current->getQFPropertyHirarchy2(prefix+QString("bins_%1").arg(param), prefix+QString("bins_%1_%2").arg(egroup).arg(param), 50).toInt());
    setNormalized(current->getQFPropertyHirarchy2(prefix+QString("norm_%1").arg(param), prefix+QString("norm_%1_%2").arg(egroup).arg(param), true).toBool());
    setLog(current->getQFPropertyHirarchy2(prefix+QString("log_%1").arg(param), prefix+QString("log_%1_%2").arg(egroup).arg(param), false).toBool());
    setAutorange(current->getQFPropertyHirarchy2(prefix+QString("rauto_%1").arg(param), prefix+QString("rauto_%1_%2").arg(egroup).arg(param), true).toBool());
    chkKey->setChecked(current->getQFPropertyHirarchy2(prefix+QString("showkey_%1").arg(param), prefix+QString("showkey_%1_%2").arg(egroup).arg(param), true).toBool());
    cmbFitFunction->setCurrentFitFunction(current->getQFPropertyHirarchy2(prefix+QString("distfit_%1").arg(param), prefix+QString("distfit_%1_%2").arg(egroup).arg(param), "gen_gaussian_sqrte").toString());
    edtHistogramRelaxedRangePercent->setValue(current->getQFPropertyHirarchy2(prefix+QString("rangepercent_%1").arg(param), prefix+QString("rangepercent_%1_%2").arg(egroup).arg(param), 5).toDouble());
    edtHistogramRelaxedRangePercentUp->setValue(current->getQFPropertyHirarchy2(prefix+QString("rangepercentup_%1").arg(param), prefix+QString("rangepercentup_%1_%2").arg(egroup).arg(param), 5).toDouble());
    if (chkHistogramRangeManual->isChecked()) {
        setMin(current->getQFPropertyHirarchy2(prefix+QString("rmin_%1").arg(param), prefix+QString("rmin_%1_%2").arg(egroup).arg(param), 0).toDouble());
        setMax(current->getQFPropertyHirarchy2(prefix+QString("rmax_%1").arg(param), prefix+QString("rmax_%1_%2").arg(egroup).arg(param), 10).toDouble());
    }
}


void QFHistogramView::clear() {
    for (int i=0; i<histograms.size(); i++) {
        if (histograms[i].data && !histograms[i].external) qfFree(histograms[i].data);
    }
    histograms.clear();
    updateHistogram(true);
}

int QFHistogramView::addHistogram(QString name, double* data, int32_t size, bool external) {
    QFHistogramView::Histogram h;
    h.data=data;
    if (!external) h.data=duplicateArray(data, size);
    h.name=name;
    h.size=size;
    h.external=external;
    histograms.append(h);

    return histograms.size()-1;
}

int QFHistogramView::addCopiedHistogram(QString name, const double *data, int32_t size) {
    QFHistogramView::Histogram h;
    h.data=duplicateArray(data, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms.append(h);

    return histograms.size()-1;
}

void QFHistogramView::setHistogram(int i, QString name, double* data, int32_t size, bool external) {
    if (i<0 || i>=histograms.size()) return;
    QFHistogramView::Histogram h=histograms[i];
    if (h.data && !h.external) qfFree(h.data);
    h.data=data;
    if (!external) h.data=duplicateArray(data, size);
    h.name=name;
    h.size=size;
    h.external=external;
    histograms[i]=h;
}

void QFHistogramView::setCopiedHistogram(int i, QString name, const double *data, int32_t size)
{
    if (i<0 || i>=histograms.size()) return;
    QFHistogramView::Histogram h=histograms[i];
    if (h.data && !h.external) qfFree(h.data);
    h.data=duplicateArray(data, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms[i]=h;
}

void QFHistogramView::removeHistogram(int i) {
    if (i<0 || i>=histograms.size()) return;
    if (histograms[i].data && !histograms[i].external) qfFree(histograms[i].data);
    histograms.removeAt(i);
}

int QFHistogramView::histogramCount() const {
    return histograms.size();
}

void QFHistogramView::setHistogramXLabel(const QString label, bool update)
{
    histLabel=label;
    if (update) replotHistogram();
}

/** \brief replot histogram */
void QFHistogramView::replotHistogram() {
    pltParamHistogram->set_doDrawing(false);
    pltParamHistogram->getYAxis()->set_logAxis(chkLogHistogram->isChecked());
    if (chkLogHistogram->isChecked()) {
        pltParamHistogram->get_plotter()->setY(0.1, qMax(1.0 ,pltParamHistogram->getYMax()));
    }
    pltParamHistogram->getXAxis()->set_axisLabel(histLabel);
    if (chkNormalizedHistograms->isChecked()) pltParamHistogram->getYAxis()->set_axisLabel(tr("normalized frequency"));
    else pltParamHistogram->getYAxis()->set_axisLabel(tr("frequency"));
    pltParamHistogram->zoomToFit(true, true, false, !chkLogHistogram->isChecked());
    pltParamHistogram->set_doDrawing(true);
    pltParamHistogram->update_plot();
}

/** \brief recalculate histogram over all pixels */
void QFHistogramView::updateHistogram(bool replot, int which) {

    edtHistogramMin->setEnabled(chkHistogramRangeManual->isChecked());
    edtHistogramMax->setEnabled(chkHistogramRangeManual->isChecked());
    edtHistogramRelaxedRangePercent->setEnabled(chkHistogramRangeRelaxAuto->isChecked());
    edtHistogramRelaxedRangePercentUp->setEnabled(chkHistogramRangeRelaxAuto->isChecked());

    pltParamHistogram->set_doDrawing(false);
    tvHistogramParameters->setModel(NULL);
    tabHistogramParameters->setReadonly(false);

    pltParamHistogram->get_plotter()->set_showKey(chkKey->isChecked());

    JKQTPdatastore* ds=pltParamHistogram->getDatastore();
    if (which<0) {
        pltParamHistogram->clearGraphs(true);
        ds->clear();
        for (int r=0; r<tabHistogramParameters->rowCount(); r++) {
            for (int c=0; c<histograms.size(); c++)  {
                tabHistogramParameters->setCellCreate(r, c+1, QVariant());
            }
        }
    } else if (which<histograms.size()){
        QFHistogramView::Histogram hist=histograms[which];
        for (int i=pltParamHistogram->getGraphCount()-1; i>=0; i--) {
            if (pltParamHistogram->getGraph(i)->get_title().contains(hist.name)) {
                pltParamHistogram->deleteGraph(i, true);
            }
        }
        QString prefix=QString("hist%1_").arg(which);
        ds->deleteAllPrefixedColumns(prefix);
        for (int r=0; r<tabHistogramParameters->rowCount(); r++) {
            tabHistogramParameters->setCellCreate(r, which+1, QVariant());
        }
    }



    //qDebug()<<"***** updateHistogram()   "<<plteImageData<<plteImageSize;

    QList<size_t> barYs;
    QList<int> barHistID;
    QList<JKQTPboxplotHorizontalGraph*> bars;
    double allhist_max=0;

    int histStart=0;
    int histEnd=histograms.size();
    if (which>=0) {
        histStart=which;
        histEnd=which+1;
    }
    double amin=0;
    double amax=0;
    double ramin=0;
    double ramax=0;
    bool first=true;
    for (int hh=histStart; hh<histEnd; hh++) {
        //qDebug()<<hh<<histograms.size();
        if (hh>=0 && hh<histograms.size()) {
            QFHistogramView::Histogram hist=histograms[hh];
            if (hist.data && (hist.size>0)) {
                int imageSize=hist.size;
                double* datahist=(double*)qfMalloc(imageSize*sizeof(double));
                long long datasize=0;
                for (register long long i=0; i<imageSize; i++) {
                    const double v=hist.data[i];
                    if (statisticsFloatIsOK(v)) {
                        datahist[datasize]=v;
                        datasize++;
                    }
                }
                double dmin, dmax, q5min,q5max;
                statisticsSort(datahist, datasize);
                //datasize=statisticsFilterGoodFloat(datahist, datasize);
                dmin=statisticsSortedMin(datahist, datasize);
                dmax=statisticsSortedMax(datahist, datasize);
                q5min=statisticsSortedQuantile(datahist, datasize, edtHistogramRelaxedRangePercent->value()/100.0);
                q5max=statisticsSortedQuantile(datahist, datasize, (100.0-edtHistogramRelaxedRangePercentUp->value())/100.0);
                if (first) {
                    amin=dmin;
                    amax=dmax;
                    ramin=q5min;
                    ramax=q5max;
                } else {
                    amin=qMin(amin, dmin);
                    amax=qMax(amax, dmax);
                    ramin=qMin(ramin, q5min);
                    ramax=qMax(ramax, q5max);
                }
                first=false;
                qfFree(datahist);
            }
        }
    }
    for (int hh=histStart; hh<histEnd; hh++) {
        //qDebug()<<hh<<histograms.size();
        if (hh>=0 && hh<histograms.size()) {
            QFHistogramView::Histogram hist=histograms[hh];
            if (hist.data && (hist.size>0)) {
                int imageSize=hist.size;
                double* datahist=(double*)qfMalloc(imageSize*sizeof(double));
                int32_t datasize=0;
                double mmin=edtHistogramMin->value();
                double mmax=edtHistogramMax->value();
                if (chkHistogramRangeAuto->isChecked() && hh==0) {
                    for (register int32_t i=0; i<imageSize; i++) {
                        const double v=hist.data[i];
                        if (statisticsFloatIsOK(v) && v>=amin && v<=amax) {
                            datahist[i]=v;
                            datasize++;
                        }
                    }
                } else if (chkHistogramRangeRelaxAuto->isChecked() && hh==0){
                    for (register int32_t i=0; i<imageSize; i++) {
                        const double v=hist.data[i];
                        if (statisticsFloatIsOK(v) && v>=ramin && v<=ramax) {
                            datahist[i]=v;
                            datasize++;
                        }
                    }
                } else {
                    for (register int32_t i=0; i<imageSize; i++) {
                        const double v=hist.data[i];
                        if (statisticsFloatIsOK(v)&&(v>=mmin) && (v<=mmax)) {
                            datahist[datasize]=v;
                            datasize++;
                        }
                    }
                }


                statisticsSort(datahist, datasize);
                double dmean, dstd, dmin, dmax, dmedian, dq25, dq75, dskew, dnmad;
                dmean=statisticsAverageVariance(dstd, datahist, datasize);
                dstd=sqrt(dstd);
                dmin=statisticsSortedMin(datahist, datasize);
                dmax=statisticsSortedMax(datahist, datasize);
                dmedian=statisticsSortedMedian(datahist, datasize);
                dq25=statisticsSortedQuantile(datahist, datasize, 0.25);
                dq75=statisticsSortedQuantile(datahist, datasize, 0.75);
                dskew=statisticsSkewness(datahist, datasize);
                dnmad=statisticsSortedNMAD(datahist, datasize);
                tabHistogramParameters->setCellCreate(0, hh+1, datasize);
                tabHistogramParameters->setCellCreate(1, hh+1, dmean);
                tabHistogramParameters->setCellCreate(2, hh+1, dmedian);
                tabHistogramParameters->setCellCreate(3, hh+1, dstd);
                tabHistogramParameters->setCellCreate(4, hh+1, dnmad);
                tabHistogramParameters->setCellCreate(5, hh+1, dmin);
                tabHistogramParameters->setCellCreate(6, hh+1, dq25);
                tabHistogramParameters->setCellCreate(7, hh+1, dq75);
                tabHistogramParameters->setCellCreate(8, hh+1, dmax);
                tabHistogramParameters->setCellCreate(9, hh+1, dskew);
                tabHistogramParameters->setCellCreate(10, hh+1, imageSize-datasize);
                int rowcnt=11;
                tabHistogramParameters->setColumnTitle(hh+1, hist.name);

                if (chkHistogramRangeAuto->isChecked() && hh==0) {
                    connectParameterWidgets(false);
                    edtHistogramMin->setValue(amin);
                    edtHistogramMax->setValue(amax);
                    connectParameterWidgets(true);
                }
                if (chkHistogramRangeRelaxAuto->isChecked() && hh==0) {
                    connectParameterWidgets(false);
                    edtHistogramMin->setValue(ramin);
                    edtHistogramMax->setValue(ramax);
                    connectParameterWidgets(true);
                }

                long histBins=spinHistogramBins->value();
                double* histX=(double*)qfMalloc(histBins*sizeof(double));
                double* histY=(double*)qfMalloc(histBins*sizeof(double));


                if (chkHistogramRangeAuto->isChecked() && hh==0) {
                    statisticsHistogramRanged<double, double>(datahist, datasize, amin, amax, histX, histY, histBins, chkNormalizedHistograms->isChecked());
                } else if (chkHistogramRangeRelaxAuto->isChecked() && hh==0) {
                    statisticsHistogramRanged<double, double>(datahist, datasize, ramin, ramax, histX, histY, histBins, chkNormalizedHistograms->isChecked());
                } else {
                    statisticsHistogramRanged<double, double>(datahist, datasize, mmin, mmax, histX, histY, histBins, chkNormalizedHistograms->isChecked());
                }

                QFFitAlgorithm* alg=QFFitAlgorithmManager::getInstance()->createAlgorithm("fit_lmfit");
                QFFitFunction* ff=cmbFitFunction->createCurrentInstance();
                QList<QFFitFunction*> addFF;
                QVector<double> p;
                if (alg && ff && histX && histY && histBins>0) {
                    QVector<double> perr;
                    QVector<bool> fix;
                    for (int i=0; i<ff->paramCount(); i++) {
                        perr.append(0);
                        p.append(ff->getDescription(i).initialValue);
                        if (ff->getDescription(i).id=="offset") {
                            fix.append(true);
                        } else {
                            fix.append(ff->getDescription(i).initialFix);
                        }
                    }
                    //qDebug()<<"init: "<<p<<fix;
                    ff->estimateInitial(p.data(), histX, histY, histBins, fix.data());
                    //qDebug()<<"esti: "<<p<<fix;
                    for (int i=0; i<ff->paramCount(); i++) {
                        if (ff->getDescription(i).id=="offset") {
                            p[i]=0;
                        }
                    }
                    QVector<double> pinit=p;
                    alg->fit(p.data(), perr.data(), histX, histY, NULL, histBins, ff, pinit.constData(), fix.data());
                    //qDebug()<<"fit:  "<<p<<fix<<histX[0]<<histY[0]<<histBins;
                    for (int i=0; i<ff->paramCount(); i++) {
                        QFFitFunction::ParameterDescription d=ff->getDescription(i);
                        if (ff->isParameterVisible(i, p.data())) {
                            tabHistogramParameters->setCellCreate(rowcnt, hh+1, p[i]);
                            tabHistogramParameters->setCellCreate(rowcnt, 0, tr("fit: %1").arg(d.label));
                            rowcnt++;
                        }
                    }
                    if (ff->getAdditionalPlotCount(p)>0) {
                        for (unsigned int i=0; i<ff->getAdditionalPlotCount(p); i++) {
                            addFF.append(cmbFitFunction->createCurrentInstance());
                        }
                    }
                }


                if (alg) delete alg;


                if (hh==0) mainHistogramMax=statisticsMax(histY, histBins);
                double barY=mainHistogramMax*1.1;
                if (mainHistogramMax>allhist_max) allhist_max=mainHistogramMax;

                QString prefix=QString("hist%1_").arg(hh);

                size_t pltcPHHistogramX=ds->addCopiedColumn(histX, histBins, prefix+"histX");
                size_t pltcPHHistogramY=ds->addCopiedColumn(histY, histBins, prefix+"histY");
                size_t pltcPHBarY=ds->addCopiedColumn(&barY, 1, prefix+"barY");
                barYs.append(pltcPHBarY);
                barHistID.append(hh);
                size_t pltcPHBarMean=ds->addCopiedColumn(&dmean, 1, prefix+"mean");;
                /*size_t pltcPHBarStd=*/ds->addCopiedColumn(&dstd, 1, prefix+"stddev");;
                size_t pltcPHBarMedian=ds->addCopiedColumn(&dmedian, 1, prefix+"median");;
                size_t pltcPHBarMin=ds->addCopiedColumn(&dmin, 1, prefix+"min");;
                size_t pltcPHBarMax=ds->addCopiedColumn(&dmax, 1, prefix+"max");;
                size_t pltcPHBarQ25=ds->addCopiedColumn(&dq25, 1, prefix+"quant25");;
                size_t pltcPHBarQ75=ds->addCopiedColumn(&dq75, 1, prefix+"quant75");;

                JKQTPboxplotHorizontalGraph* plteParamHistogramBoxplot=new JKQTPboxplotHorizontalGraph(pltParamHistogram->get_plotter());
                bars.append(plteParamHistogramBoxplot);
                plteParamHistogramBoxplot->set_boxWidth(mainHistogramMax*0.08);
                if (chkLogHistogram->isChecked()) {
                    plteParamHistogramBoxplot->set_boxWidth(mainHistogramMax*0.08*double(hh+1));
                }
                plteParamHistogramBoxplot->set_maxColumn(pltcPHBarMax);
                plteParamHistogramBoxplot->set_minColumn(pltcPHBarMin);
                plteParamHistogramBoxplot->set_medianColumn(pltcPHBarMedian);
                plteParamHistogramBoxplot->set_meanColumn(pltcPHBarMean);
                plteParamHistogramBoxplot->set_percentile25Column(pltcPHBarQ25);
                plteParamHistogramBoxplot->set_percentile75Column(pltcPHBarQ75);
                plteParamHistogramBoxplot->set_posColumn(pltcPHBarY);
                //plteParamHistogramBoxplot->set_color(QColor("blue"));
                plteParamHistogramBoxplot->set_title(tr("boxplot (%1)").arg(hist.name));
                plteParamHistogramBoxplot->set_lineWidth(1);
                plteParamHistogramBoxplot->set_color(defaultColors.value(hh, plteParamHistogramBoxplot->get_color()));
                pltParamHistogram->addGraph(plteParamHistogramBoxplot);

                JKQTPbarHorizontalGraph* plteParamHistogram=new JKQTPbarHorizontalGraph(pltParamHistogram->get_plotter());
                QColor fill=plteParamHistogramBoxplot->get_color();
                fill.setAlphaF(0.7);
                plteParamHistogram->set_fillColor(fill);
                plteParamHistogram->set_color(plteParamHistogramBoxplot->get_color());
                plteParamHistogram->set_xColumn(pltcPHHistogramX);
                plteParamHistogram->set_yColumn(pltcPHHistogramY);
                plteParamHistogram->set_width(1);
                /*if (!chkHistogramRangeAuto->isChecked()) {
                    plteParamHistogram->set_width(1.0/(double)histograms.size());
                    plteParamHistogram->set_shift(hh*1.0/(double)histograms.size());
                //}*/
                plteParamHistogram->set_title(tr("histogram (%1)").arg(hist.name));
                pltParamHistogram->addGraph(plteParamHistogram);

                if (ff && p.size()>0) {
                    JKQTPxQFFitFunctionLineGraph* plteFit=new JKQTPxQFFitFunctionLineGraph(pltParamHistogram->get_plotter());
                    plteFit->set_fitFunction(ff, true);
                    plteFit->set_params(p);
                    plteFit->set_color(plteParamHistogramBoxplot->get_color());
                    plteFit->set_drawLine(true);
                    plteFit->set_style(Qt::SolidLine);
                    plteFit->set_lineWidth(1.5);
                    plteFit->set_title(tr("fit (%1): %2").arg(hist.name).arg(ff->shortName()));
                    pltParamHistogram->addGraph(plteFit);
                    if (addFF.size()>0) {
                        for (int i=0; i<addFF.size(); i++) {
                            if (addFF[i]) {
                                JKQTPxQFFitFunctionLineGraph* plteFitSub=new JKQTPxQFFitFunctionLineGraph(pltParamHistogram->get_plotter());
                                plteFitSub->set_fitFunction(addFF[i], true);
                                QVector<double> ps=p;
                                addFF[i]->transformParametersForAdditionalPlot(i, ps);
                                plteFitSub->set_params(ps);
                                plteFitSub->set_color(plteParamHistogramBoxplot->get_color().darker());
                                plteFitSub->set_drawLine(true);
                                plteFitSub->set_style(Qt::DashLine);
                                plteFitSub->set_lineWidth(1);
                                plteFitSub->set_title("");//tr("fit (%1): %2").arg(hist.name).arg(ff->shortName()));
                                pltParamHistogram->addGraph(plteFitSub);
                            }
                        }
                    }
                } else {
                    if (ff) delete ff;
                }

                qfFree(histX);
                qfFree(histY);
                qfFree(datahist);

            }
        }
    }

    if (!chkLogHistogram->isChecked()) {
        for (int i=0; i<barYs.size(); i++) {
            ds->set(barYs[i], 0, allhist_max*(1.0+double(barHistID[i]+1)*0.1));
            bars[i]->set_boxWidth(allhist_max*0.08);
        }
    } else {
        for (int i=0; i<barYs.size(); i++) {
            ds->set(barYs[i], 0, allhist_max+pow(10.0, (double(barHistID[i]+floor(log(allhist_max)/log(10.0)))*0.1)));
            bars[i]->set_boxWidth(pow(10.0, floor(allhist_max)/*+double(i)/double(barYs.size())*/));
        }
    }

    tabHistogramParameters->setReadonly(true);
    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->resizeColumnsToContents();
    pltParamHistogram->set_doDrawing(true);
    if (replot) {
        replotHistogram();
    }
    //qDebug()<<"updateHistogram ... end";
}

void QFHistogramView::histogramSettingsChanged(bool update) {
    edtHistogramMin->setEnabled(!chkHistogramRangeAuto->isChecked());
    edtHistogramMax->setEnabled(!chkHistogramRangeAuto->isChecked());
    if (update) updateHistogram(true);
    emit settingsChanged();
}

void QFHistogramView::printReport()
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

void QFHistogramView::saveReport()
{
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString currentSaveDirectory="";
    currentSaveDirectory=ProgramOptions::getConfigValue(QString("QFHistogramView/last_report_directory"), currentSaveDirectory).toString();

    QString fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                currentSaveDirectory,
                                          #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
                                              tr("PDF File (*.pdf);;PostScript File (*.ps);;Open Document file (*.odf);;HTML file (*.html)"));
                                          #else
                                              tr("PDF File (*.pdf);;Open Document file (*.odf);;HTML file (*.html)"));
                                          #endif



    if (!fn.isEmpty()) {
        currentSaveDirectory=QFileInfo(fn).absolutePath();
        ProgramOptions::setConfigValue(QString("QFHistogramView/last_report_directory"), currentSaveDirectory);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QProgressDialog progress(tr("Exporting ..."), "", 0,100,NULL);
        progress.setWindowModality(Qt::WindowModal);
        //progress.setHasCancel(false);
        progress.setLabelText(tr("saving report <br> to '%1' ...").arg(fn));
        progress.show();
        progress.setValue(50);

        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter();//QPrinter::HighResolution);
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(15,15,15,15,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setOutputFormat(QPrinter::PdfFormat);
        printer->setColorMode(QPrinter::Color);
        QTextDocument* doc=new QTextDocument();
        doc->setTextWidth(printer->pageRect().size().width());
        QTextCursor cur(doc);
        writeReport(cur, doc);
        if (fi.suffix().toLower()=="ps" || fi.suffix().toLower()=="pdf") {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
            if (fi.suffix().toLower()=="ps") printer->setOutputFormat(QPrinter::PostScriptFormat);
#endif
            printer->setColorMode(QPrinter::Color);
            printer->setOutputFileName(fn);
            doc->print(printer);
        } else if (fi.suffix().toLower()=="odf") {
            QTextDocumentWriter writer(fn, "odf");
            writer.write(doc);
        } else if ((fi.suffix().toLower()=="html")||(fi.suffix().toLower()=="htm")) {
            QTextDocumentWriter writer(fn, "html");
            writer.write(doc);
        }
        //qDebug()<<doc->toHtml();
        delete doc;
        delete printer;
        progress.accept();
        QApplication::restoreOverrideCursor();
    }
}

void QFHistogramView::copyData()
{
    QList<QVector<double> > data;
    QStringList headers;
    fillDataArray(data, headers);

    csvCopy(data, headers);
}

void QFHistogramView::copyDataMatlab()
{
    QList<QVector<double> > data;
    QStringList headers;
    fillDataArray(data, headers);

    matlabCopy(data);
}

void QFHistogramView::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("qfhistogram.html");
}

void QFHistogramView::saveData()
{
    QList<QVector<double> > data;
    QStringList headers;
    fillDataArray(data, headers);
    QStringList f=QFDataExportHandler::getFormats();
    QString lastDir=ProgramOptions::getConfigValue("QFHistogramView/lastDataDir", "").toString();
    QString selFilter=ProgramOptions::getConfigValue("QFHistogramView/lastDataFilter", "").toString();
    QString fn=qfGetSaveFileName(this, tr("Save data to file"), lastDir, f.join(";;"), &selFilter);
    if (fn.size()>0) {
        QFDataExportHandler::save(data, f.indexOf(selFilter), fn, headers, QStringList());
        ProgramOptions::setConfigValue("QFHistogramView/lastDataDir", lastDir);
        ProgramOptions::setConfigValue("QFHistogramView/lastDataFilter", selFilter);
    }
}

void QFHistogramView::fillDataArray(QList<QVector<double> > &data, QStringList& headers)
{
    data.clear();
    headers.clear();
    for (int i=0; i<histograms.size(); i++) {
        QVector<double> d(histograms[i].size, 0.0);
        memcpy(d.data(), histograms[i].data, histograms[i].size*sizeof(double));
        data.append(d);
        headers.append(histograms[i].name);
    }
}


void QFHistogramView::writeReport(QTextCursor& cursor, QTextDocument* document) {
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


    double w1=qMax(1,pltParamHistogram->width());
    double w2=qMax(1,tvHistogramParameters->width());
    double allwidth=qMax(1.0,w1+w2);
    cursor.insertBlock();
    cursor.insertText(tr("Parameter Histograms:\n"), fHeading2);
    QTextTable* table = cursor.insertTable(1, 2, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltParamHistogram->get_plotter()->draw(*painter, QRect(0,0,pltParamHistogram->width(),pltParamHistogram->height()));
        delete painter;
        double scale=0.5*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);

        tabCursor=table->cellAt(0, 1).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        //tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><nobr><span style=\"font-size: 7pt;\">%1</span></nobr></center>").arg(tvHistogramParameters->toHtml(1,true))));
        QPicture picT;
        painter=new JKQTPEnhancedPainter(&picT);
        tvHistogramParameters->paint(*painter);
        delete painter;
        scale=0.4*document->textWidth()/double(picT.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("statistics table:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, picT, QSizeF(picT.boundingRect().width(), picT.boundingRect().height())*scale);

    }
    cursor.movePosition(QTextCursor::End);
}

