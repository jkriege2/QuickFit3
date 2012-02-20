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

QFHistogramView::QFHistogramView(QWidget *parent) :
    QWidget(parent)
{
    connectParameterWidgetsCounter=0;
    createWidgets();
}

QFHistogramView::~QFHistogramView() {
    clear();
}

void QFHistogramView::createWidgets() {

    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widHist=this;
    QGridLayout* layHist=new QGridLayout();
    widHist->setLayout(layHist);

    // HISTOGRAM SETTINGS/////////////////////////////////////////////////////////////////////
    grpHistogramSettings=new QGroupBox(tr("histogram style"), this);
    flHistSet=new QFormLayout(grpHistogramSettings);
    grpHistogramSettings->setLayout(flHistSet);
    spinHistogramBins=new QSpinBox(this);
    spinHistogramBins->setRange(5,99999);
    spinHistogramBins->setValue(100);
    flHistSet->addRow(tr("# bins:"), spinHistogramBins);
    chkLogHistogram=new QCheckBox("", grpHistogramSettings);
    flHistSet->addRow(tr("log-scale:"), chkLogHistogram);
    chkNormalizedHistograms=new QCheckBox("", grpHistogramSettings);
    flHistSet->addRow(tr("normalized:"), chkNormalizedHistograms);
    chkHistogramRangeAuto=new QCheckBox("auto", grpHistogramSettings);
    flHistSet->addRow(tr("range:"), chkHistogramRangeAuto);
    edtHistogramMin=new JKDoubleEdit(this);
    edtHistogramMin->setCheckBounds(false, false);
    edtHistogramMin->setValue(0);
    edtHistogramMax=new JKDoubleEdit(this);
    edtHistogramMax->setCheckBounds(false, false);
    edtHistogramMax->setValue(10);
    QHBoxLayout* coll=new QHBoxLayout();
    coll->addWidget(edtHistogramMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtHistogramMax,1);
    coll->setContentsMargins(0,0,0,0);
    flHistSet->addRow(QString(""), coll);

    // HISTOGRAM PLOTS ///////////////////////////////////////////////////////////////////////
    splitterHistogram=new QVisibleHandleSplitter(this);
    pltParamHistogram=new JKQtPlotter(this);
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
    for (int r=0; r<8; r++) tabHistogramParameters->appendRow();
    tabHistogramParameters->setCell(0, 0, tr("data points N"));
    tabHistogramParameters->setCell(1, 0, tr("average"));
    tabHistogramParameters->setCell(2, 0, tr("median"));
    tabHistogramParameters->setCell(3, 0, tr("std. dev. &sigma;"));
    tabHistogramParameters->setCell(4, 0, tr("min"));
    tabHistogramParameters->setCell(5, 0, tr("25% quantile"));
    tabHistogramParameters->setCell(6, 0, tr("75% quantile"));
    tabHistogramParameters->setCell(7, 0, tr("max"));
    tabHistogramParameters->setReadonly(true);

    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->setItemDelegate(new QFHTMLDelegate(tvHistogramParameters));
    tvHistogramParameters->setAlternatingRowColors(true);
    QFontMetrics tvfm(font());
    tvHistogramParameters->verticalHeader()->setDefaultSectionSize((int)round((double)tvfm.height()*1.5));



    splitterHistogram->addWidget(pltParamHistogram);
    splitterHistogram->addWidget(tvHistogramParameters);
    layHist->addWidget(splitterHistogram, 0, 0);
    layHist->addWidget(grpHistogramSettings, 0, 1);
    layHist->setColumnStretch(0,5);
    layHist->setContentsMargins(0,0,0,0);

    //connectParameterWidgets(true);

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
            connect(edtHistogramMin, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
            connect(edtHistogramMax, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        //}
    } else {
        //connectParameterWidgetsCounter++;
        disconnect(chkLogHistogram, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkNormalizedHistograms, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(spinHistogramBins, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkHistogramRangeAuto, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMin, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMax, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
    }
}

void QFHistogramView::readSettings(QSettings& settings, const QString& prefix) {
    connectParameterWidgets(false);
    loadSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");
    connectParameterWidgets(true);
}

void QFHistogramView::writeSettings(QSettings& settings, const QString& prefix) {
    saveSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");
}

void QFHistogramView::clear() {
    for (int i=0; i<histograms.size(); i++) {
        if (histograms[i].data) free(histograms[i].data);
    }
    histograms.clear();
    updateHistogram(true);
}

int QFHistogramView::addHistogram(QString name, double* data, int32_t size) {
    QFHistogramView::Histogram h;
    h.data=data;
    h.name=name;
    h.size=size;
    histograms.append(h);

    return histograms.size()-1;
}

void QFHistogramView::setHistogram(int i, QString name, double* data, int32_t size) {
    if (i<0 || i>=histograms.size()) return;
    QFHistogramView::Histogram h=histograms[i];
    if (h.data) free(h.data);
    h.data=data;
    h.name=name;
    h.size=size;
    histograms[i]=h;
}

void QFHistogramView::removeHistogram(int i) {
    if (i<0 || i>=histograms.size()) return;
    if (histograms[i].data) free(histograms[i].data);
    histograms.removeAt(i);
}

int QFHistogramView::histogramCount() const {
    return histograms.size();
}

/** \brief replot histogram */
void QFHistogramView::replotHistogram() {
    pltParamHistogram->set_doDrawing(false);
    pltParamHistogram->getYAxis()->set_logAxis(chkLogHistogram->isChecked());
    pltParamHistogram->zoomToFit(true, true, false, !chkLogHistogram->isChecked());
    pltParamHistogram->set_doDrawing(true);
    pltParamHistogram->update_plot();
}

/** \brief recalculate histogram over all pixels */
void QFHistogramView::updateHistogram(bool replot, int which) {

    edtHistogramMin->setEnabled(!chkHistogramRangeAuto->isChecked());
    edtHistogramMax->setEnabled(!chkHistogramRangeAuto->isChecked());

    pltParamHistogram->set_doDrawing(false);
    tvHistogramParameters->setModel(NULL);
    tabHistogramParameters->setReadonly(false);

    JKQTPdatastore* ds=pltParamHistogram->getDatastore();
    if (which<0) {
        pltParamHistogram->clearGraphs(true);
        ds->clear();
        for (int r=0; r<tabHistogramParameters->rowCount(); r++) {
            for (int c=0; c<histograms.size(); c++)  {
                tabHistogramParameters->setCell(r, c+1, QVariant());
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
            tabHistogramParameters->setCell(r, which+1, QVariant());
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
    for (int hh=histStart; hh<histEnd; hh++) {
        //qDebug()<<hh<<histograms.size();
        if (hh>=0 && hh<histograms.size()) {
            QFHistogramView::Histogram hist=histograms[hh];
            if (hist.data && (hist.size>0)) {
                int imageSize=hist.size;
                double* datahist=(double*)malloc(imageSize*sizeof(double));
                int32_t datasize=0;
                double mmin=edtHistogramMin->value();
                double mmax=edtHistogramMax->value();
                if (chkHistogramRangeAuto->isChecked()) {
                    for (register int32_t i=0; i<imageSize; i++) {
                        datahist[i]=hist.data[i];
                        datasize++;
                    }
                } else {
                    for (register int32_t i=0; i<imageSize; i++) {
                        if ((hist.data[i]>=mmin) && (hist.data[i]<=mmax)) {
                            datahist[datasize]=hist.data[i];
                            datasize++;
                        }
                    }
                }


                statisticsSort(datahist, datasize);
                double dmean, dstd, dmin, dmax, dmedian, dq25, dq75;
                dmean=statisticsAverageVariance(dstd, datahist, datasize);
                dstd=sqrt(dstd);
                dmin=statisticsSortedMin(datahist, datasize);
                dmax=statisticsSortedMax(datahist, datasize);
                dmedian=statisticsSortedMedian(datahist, datasize);
                dq25=statisticsSortedQuantile(datahist, datasize, 0.25);
                dq75=statisticsSortedQuantile(datahist, datasize, 0.75);
                tabHistogramParameters->setCell(0, hh+1, datasize);
                tabHistogramParameters->setCell(1, hh+1, dmean);
                tabHistogramParameters->setCell(2, hh+1, dmedian);
                tabHistogramParameters->setCell(3, hh+1, dstd);
                tabHistogramParameters->setCell(4, hh+1, dmin);
                tabHistogramParameters->setCell(5, hh+1, dq25);
                tabHistogramParameters->setCell(6, hh+1, dq75);
                tabHistogramParameters->setCell(7, hh+1, dmax);
                tabHistogramParameters->setColumnTitle(hh+1, hist.name);

                if (chkHistogramRangeAuto->isChecked()) {
                    connectParameterWidgets(false);
                    edtHistogramMin->setValue(dmin);
                    edtHistogramMax->setValue(dmax);
                    connectParameterWidgets(true);
                }

                long histBins=spinHistogramBins->value();
                double* histX=(double*)malloc(histBins*sizeof(double));
                double* histY=(double*)malloc(histBins*sizeof(double));


                if (chkHistogramRangeAuto->isChecked()) {
                    statisticsHistogram<double, double>(datahist, datasize, histX, histY, histBins, chkNormalizedHistograms->isChecked());
                } else {
                    statisticsHistogramRanged<double, double>(datahist, datasize, edtHistogramMin->value(), edtHistogramMax->value(), histX, histY, histBins, chkNormalizedHistograms->isChecked());
                }

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
                pltParamHistogram->addGraph(plteParamHistogramBoxplot);

                JKQTPbarHorizontalGraph* plteParamHistogram=new JKQTPbarHorizontalGraph(pltParamHistogram->get_plotter());
                QColor fill=plteParamHistogramBoxplot->get_color();
                fill.setAlphaF(0.7);
                plteParamHistogram->set_fillColor(fill);
                plteParamHistogram->set_xColumn(pltcPHHistogramX);
                plteParamHistogram->set_yColumn(pltcPHHistogramY);
                plteParamHistogram->set_width(1);
                /*if (!chkHistogramRangeAuto->isChecked()) {
                    plteParamHistogram->set_width(1.0/(double)histograms.size());
                    plteParamHistogram->set_shift(hh*1.0/(double)histograms.size());
                //}*/
                plteParamHistogram->set_title(tr("histogram (%1)").arg(hist.name));
                pltParamHistogram->addGraph(plteParamHistogram);

                free(histX);
                free(histY);
                free(datahist);
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


void QFHistogramView::writeReport(QTextCursor& cursor, QTextDocument* document) {
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
        QPainter* painter=new QPainter(&pic);
        pltParamHistogram->get_plotter()->draw(*painter, QRect(0,0,pltParamHistogram->width(),pltParamHistogram->height()));
        delete painter;
        double scale=0.5*document->textWidth()/pic.boundingRect().width();
        if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);

        tabCursor=table->cellAt(0, 1).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><nobr><span style=\"font-size: 7pt;\">%1</span></nobr></center>").arg(tvHistogramParameters->toHtml(1,true))));
    }
    cursor.movePosition(QTextCursor::End);
}
