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

QFParameterCorrelationView::QFParameterCorrelationView(QWidget *parent) :
    QWidget(parent)
{
    connectParameterWidgetsCounter=0;
    histLabelX="";
    histLabelY="";
    createWidgets();
}

QFParameterCorrelationView::~QFParameterCorrelationView()
{
    for (int i=0; i<histograms.size(); i++) {
        if (histograms[i].data1 && !histograms[i].external) free(histograms[i].data1);
        if (histograms[i].data2 && !histograms[i].external) free(histograms[i].data2);
    }
    histograms.clear();
}

void QFParameterCorrelationView::readSettings(QSettings &settings, const QString &prefix)
{
    connectParameterWidgets(false);
    loadSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");
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
    current->setQFProperty(prefix+QString("symbol_%1_%2").arg(egroup).arg(param), cmbSymbol->currentIndex(), false, false);
    current->setQFProperty(prefix+QString("symbsize_%1_%2").arg(egroup).arg(param), spinSymbolSize->value(), false, false);
    current->setQFProperty(prefix+QString("bins1_%1_%2").arg(egroup).arg(param), getHistBins1(), false, false);
    current->setQFProperty(prefix+QString("rauto1_%1_%2").arg(egroup).arg(param), getAutorange1(), false, false);
    if (!getAutorange1()) {
        current->setQFProperty(prefix+QString("rmin1_%1_%2").arg(egroup).arg(param), getMin1(), false, false);
        current->setQFProperty(prefix+QString("rmax1_%1_%2").arg(egroup).arg(param), getMax1(), false, false);
    }
    current->setQFProperty(prefix+QString("bins2_%1_%2").arg(egroup).arg(param), getHistBins2(), false, false);
    current->setQFProperty(prefix+QString("rauto2_%1_%2").arg(egroup).arg(param), getAutorange2(), false, false);
    if (!getAutorange1()) {
        current->setQFProperty(prefix+QString("rmin2_%1_%2").arg(egroup).arg(param), getMin2(), false, false);
        current->setQFProperty(prefix+QString("rmax2_%1_%2").arg(egroup).arg(param), getMax2(), false, false);
    }

}

void QFParameterCorrelationView::readQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{
    cmbSymbol->setCurrentIndex(current->getProperty(prefix+QString("symbol_%1_%2").arg(egroup).arg(param)).toInt());
    spinSymbolSize->setValue(current->getProperty(prefix+QString("symbsize_%1_%2").arg(egroup).arg(param)).toInt());
    setHistBins1(current->getProperty(prefix+QString("bins1_%1_%2").arg(egroup).arg(param), 50).toInt());
    setAutorange1(current->getProperty(prefix+QString("rauto1_%1_%2").arg(egroup).arg(param), true).toBool());
    if (!getAutorange1()) {
        setMin1(current->getProperty(prefix+QString("rmin1_%1_%2").arg(egroup).arg(param), 0).toDouble());
        setMax1(current->getProperty(prefix+QString("rmax1_%1_%2").arg(egroup).arg(param), 10).toDouble());
    }

    setHistBins2(current->getProperty(prefix+QString("bins2_%1_%2").arg(egroup).arg(param), 50).toInt());
    setAutorange2(current->getProperty(prefix+QString("rauto2_%1_%2").arg(egroup).arg(param), true).toBool());
    if (!getAutorange2()) {
        setMin2(current->getProperty(prefix+QString("rmin2_%1_%2").arg(egroup).arg(param), 0).toDouble());
        setMax2(current->getProperty(prefix+QString("rmax2_%1_%2").arg(egroup).arg(param), 10).toDouble());
    }
}

void QFParameterCorrelationView::connectParameterWidgets(bool connectTo)
{
    if (connectTo) {
        connect(spinSymbolSize, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        connect(cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(histogramSettingsChanged()));

        connect(spinHistogramBins1, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        connect(chkHistogramRangeAuto1, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMin1, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMax1, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));

        connect(spinHistogramBins2, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        connect(chkHistogramRangeAuto2, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMin2, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMax2, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
    } else {
        disconnect(spinSymbolSize, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        disconnect(cmbSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(histogramSettingsChanged()));

        disconnect(spinHistogramBins1, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkHistogramRangeAuto1, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMin1, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMax1, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));

        disconnect(spinHistogramBins2, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        disconnect(chkHistogramRangeAuto2, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMin2, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        disconnect(edtHistogramMax2, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
    }
}

void QFParameterCorrelationView::replotCorrelation()
{
    pltParamCorrelation->set_doDrawing(false);
    pltParamHistogramX->set_doDrawing(false);
    pltParamHistogramY->set_doDrawing(false);
    pltParamCorrelation->getYAxis()->set_logAxis(false);
    pltParamCorrelation->getXAxis()->set_axisLabel(histLabelX);
    pltParamCorrelation->getYAxis()->set_axisLabel(histLabelY);
    pltParamCorrelation->zoomToFit(true, true, false, false);
    pltParamHistogramX->zoomToFit(false, true, false, false);
    pltParamHistogramY->zoomToFit(true, false, false, false);
    pltParamCorrelation->set_doDrawing(true);
    pltParamHistogramX->set_doDrawing(true);
    pltParamHistogramY->set_doDrawing(true);
    pltParamCorrelation->update_plot();
    pltParamHistogramX->update_plot();
    pltParamHistogramY->update_plot();
}

void QFParameterCorrelationView::updateCorrelation(bool replot, int which)
{



    edtHistogramMin1->setEnabled(!chkHistogramRangeAuto1->isChecked());
    edtHistogramMax1->setEnabled(!chkHistogramRangeAuto1->isChecked());
    edtHistogramMin2->setEnabled(!chkHistogramRangeAuto2->isChecked());
    edtHistogramMax2->setEnabled(!chkHistogramRangeAuto2->isChecked());

    pltParamCorrelation->set_doDrawing(false);
    tvHistogramParameters->setModel(NULL);
    tabHistogramParameters->setReadonly(false);

    JKQTPdatastore* ds=pltParamCorrelation->getDatastore();
    if (which<0) {
        pltParamCorrelation->clearGraphs(true);
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

    for (int hh=histStart; hh<histEnd; hh++) {
        //qDebug()<<hh<<histograms.size();
        if (hh>=0 && hh<histograms.size()) {
            CorrelationItem hist=histograms[hh];
            if (hist.data1 && hist.data2 && (hist.size>0)) {

                double* d1=duplicateArray(hist.data1, hist.size);
                double* d2=duplicateArray(hist.data1, hist.size);

                int imageSize=hist.size;
                int32_t datasize=0;
                double mmin1=edtHistogramMin1->value();
                double mmax1=edtHistogramMax1->value();
                double mmin2=edtHistogramMin2->value();
                double mmax2=edtHistogramMax2->value();

                for (register int32_t i=0; i<imageSize; i++) {
                    const double v1=hist.data1[i];
                    const double v2=hist.data2[i];
                    if (statisticsFloatIsOK(v1)&&statisticsFloatIsOK(v2)&&(chkHistogramRangeAuto1->isChecked() || (v1>=mmin1 && v1<=mmax1))
                                                                        &&(chkHistogramRangeAuto2->isChecked() || (v2>=mmin2 && v2<=mmax2))) {
                        d1[i]=v1;
                        d2[i]=v2;
                        datasize++;
                    }
                }




                statisticsSort(d1, datasize);
                statisticsSort(d2, datasize);
                double dmean, dstd, dmin, dmax, dmedian, dq25, dq75, dskew;
                int col=0;
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

                if (chkHistogramRangeAuto1->isChecked() && hh==0) {
                    connectParameterWidgets(false);
                    edtHistogramMin1->setValue(dmin);
                    edtHistogramMax1->setValue(dmax);
                    connectParameterWidgets(true);
                }

                double aminX=0, amaxX=0;
                long histBinsX=spinHistogramBins1->value();
                double* histXX=(double*)malloc(histBinsX*sizeof(double));
                double* histXY=(double*)malloc(histBinsX*sizeof(double));
                if (chkHistogramRangeAuto1->isChecked() && hh==0) {
                    statisticsHistogramRanged<double, double>(d1, datasize, aminX, amaxX, histXX, histXY, histBinsX, false);
                } else {
                    statisticsHistogramRanged<double, double>(d1, datasize, aminX, amaxX, histXX, histXY, histBinsX, false);
                }


                dmean=statisticsAverageVariance(dstd, d2, datasize);
                dstd=sqrt(dstd);
                dmin=statisticsSortedMin(d2, datasize);
                dmax=statisticsSortedMax(d2, datasize);
                dmedian=statisticsSortedMedian(d2, datasize);
                dq25=statisticsSortedQuantile(d2, datasize, 0.25);
                dq75=statisticsSortedQuantile(d2, datasize, 0.75);
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

                if (chkHistogramRangeAuto2->isChecked() && hh==0) {
                    connectParameterWidgets(false);
                    edtHistogramMin2->setValue(dmin);
                    edtHistogramMax2->setValue(dmax);
                    connectParameterWidgets(true);
                }

                double aminY=0, amaxY=0;
                long histBinsY=spinHistogramBins2->value();
                double* histYX=(double*)malloc(histBinsY*sizeof(double));
                double* histYY=(double*)malloc(histBinsY*sizeof(double));
                if (chkHistogramRangeAuto2->isChecked() && hh==0) {
                    statisticsHistogramRanged<double, double>(d2, datasize, aminY, amaxY, histYX, histYY, histBinsY, false);
                } else {
                    statisticsHistogramRanged<double, double>(d2, datasize, aminY, amaxY, histYX, histYY, histBinsY, false);
                }


                JKQTPxyLineGraph* g=new JKQTPxyLineGraph(pltParamCorrelation->get_plotter());
                int g_x=pltParamCorrelation->getDatastore()->addCopiedColumn(d1, datasize, tr("X"));
                int g_y=pltParamCorrelation->getDatastore()->addCopiedColumn(d2, datasize, tr("Y"));
                g->set_title(hist.name);
                g->set_xColumn(g_x);
                g->set_yColumn(g_y);
                g->set_symbol(cmbSymbol->getSymbol());
                g->set_symbolSize(spinSymbolSize->value());





                JKQTPbarHorizontalGraph* plteParamHistogramX=new JKQTPbarHorizontalGraph(pltParamHistogramX->get_plotter());
                int g_hxx=pltParamCorrelation->getDatastore()->addCopiedColumn(histXX, histBinsX, tr("X_histogram_X"));
                int g_hxy=pltParamCorrelation->getDatastore()->addCopiedColumn(histXY, histBinsX, tr("X_histogram_Y"));
                QColor fill=g->get_color();
                plteParamHistogramX->set_color(fill);
                fill.setAlphaF(0.7);
                plteParamHistogramX->set_fillColor(fill);
                plteParamHistogramX->set_xColumn(g_hxx);
                plteParamHistogramX->set_yColumn(g_hxy);
                plteParamHistogramX->set_width(1);
                plteParamHistogramX->set_title("");
                pltParamHistogramX->addGraph(plteParamHistogramX);


                JKQTPbarHorizontalGraph* plteParamHistogramY=new JKQTPbarHorizontalGraph(pltParamHistogramY->get_plotter());
                int g_hyx=pltParamCorrelation->getDatastore()->addCopiedColumn(histYX, histBinsX, tr("Y_histogram_X"));
                int g_hyy=pltParamCorrelation->getDatastore()->addCopiedColumn(histYY, histBinsX, tr("Y_histogram_Y"));
                fill=g->get_color();
                plteParamHistogramY->set_color(fill);
                fill.setAlphaF(0.7);
                plteParamHistogramY->set_fillColor(fill);
                plteParamHistogramY->set_xColumn(g_hyx);
                plteParamHistogramY->set_yColumn(g_hyy);
                plteParamHistogramY->set_width(1);
                plteParamHistogramY->set_title("");
                pltParamHistogramY->addGraph(plteParamHistogramY);



                free(histXX);
                free(histXY);

                free(histYX);
                free(histYY);

                free(d1);
                free(d2);

                pltParamCorrelation->addGraph(g);

            }
        }
    }


    tabHistogramParameters->setReadonly(true);
    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->resizeColumnsToContents();
    pltParamCorrelation->set_doDrawing(true);
    if (replot) {
        replotCorrelation();
    }


}

void QFParameterCorrelationView::addSettingsWidget(const QString &label, QWidget *widget)
{
    flHistSet->addRow(label, widget);
}

int QFParameterCorrelationView::getHistBins1() const
{
    return spinHistogramBins1->value();
}

void QFParameterCorrelationView::setHistBins1(int bins)
{
    spinHistogramBins1->setValue(bins);
}

int QFParameterCorrelationView::getHistBins2() const
{
    return spinHistogramBins1->value();
}

void QFParameterCorrelationView::setHistBins2(int bins)
{
    spinHistogramBins2->setValue(bins);
}

bool QFParameterCorrelationView::getAutorange1() const
{
    return chkHistogramRangeAuto1->isChecked();
}

void QFParameterCorrelationView::setAutorange1(bool autorange)
{
    chkHistogramRangeAuto1->setChecked(autorange);
}

bool QFParameterCorrelationView::getAutorange2() const
{
    return chkHistogramRangeAuto2->isChecked();
}

void QFParameterCorrelationView::setAutorange2(bool autorange)
{
    chkHistogramRangeAuto2->setChecked(autorange);
}

double QFParameterCorrelationView::getMin1() const
{
    return edtHistogramMin1->value();
}

void QFParameterCorrelationView::setMin1(double min)
{
    edtHistogramMin1->setValue(min);
}

double QFParameterCorrelationView::getMax1() const
{
    return edtHistogramMax1->value();
}

void QFParameterCorrelationView::setMax1(double max)
{
    edtHistogramMax1->setValue(max);
}

double QFParameterCorrelationView::getMin2() const
{
    return edtHistogramMin2->value();

}

void QFParameterCorrelationView::setMin2(double min)
{
    edtHistogramMin2->setValue(min);

}

double QFParameterCorrelationView::getMax2() const
{
    return edtHistogramMax2->value();

}

void QFParameterCorrelationView::setMax2(double max)
{
    edtHistogramMax2->setValue(max);
}

void QFParameterCorrelationView::clear()
{
    for (int i=0; i<histograms.size(); i++) {
        if (histograms[i].data1 && !histograms[i].external) free(histograms[i].data1);
        if (histograms[i].data2 && !histograms[i].external) free(histograms[i].data2);
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


int QFParameterCorrelationView::addCopiedCorrelation(QString name, const double *data1, const double *data2, int32_t size)
{
    QFParameterCorrelationView::CorrelationItem h;
    h.data1=duplicateArray(data1, size);
    h.data2=duplicateArray(data2, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms.append(h);

    return histograms.size()-1;}

void QFParameterCorrelationView::setCorrelation(int i, QString name, double *data1, double *data2, int32_t size, bool external)
{
    if (i<0 || i>=histograms.size()) return;
    QFParameterCorrelationView::CorrelationItem h=histograms[i];
    if (h.data1 && !h.external) free(h.data1);
    if (h.data2 && !h.external) free(h.data2);
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

void QFParameterCorrelationView::setCopiedCorrelation(int i, QString name, const double *data1, const double *data2, int32_t size)
{
    if (i<0 || i>=histograms.size()) return;
    QFParameterCorrelationView::CorrelationItem h=histograms[i];
    if (h.data1 && !h.external) free(h.data1);
    if (h.data2 && !h.external) free(h.data2);
    h.data1=duplicateArray(data1, size);
    h.data2=duplicateArray(data2, size);
    h.name=name;
    h.size=size;
    h.external=false;
    histograms[i]=h;
}

void QFParameterCorrelationView::removeCorrelation(int i)
{
    if (i<0 || i>=histograms.size()) return;
    if (histograms[i].data1 && !histograms[i].external) free(histograms[i].data1);
    if (histograms[i].data2 && !histograms[i].external) free(histograms[i].data2);
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

void QFParameterCorrelationView::writeReport(QTextCursor &cursor, QTextDocument *document)
{

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
    QTextTable* table = cursor.insertTable(2,3, tableFormat1);
    table->mergeCells(0,2,2,1);
    {
        QTextCursor tabCursor=table->cellAt(0,1).firstCursorPosition();
        {
            QPicture pic;
            QPainter* painter=new QPainter(&pic);
            pltParamCorrelation->get_plotter()->draw(*painter, QRect(0,0,pltParamCorrelation->width(),pltParamCorrelation->height()));
            delete painter;
            double scale=0.7*document->textWidth()/double(pic.boundingRect().width());
            if (scale<=0) scale=1;
            insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
        }

        tabCursor=table->cellAt(0, 0).firstCursorPosition();
        {
            QPicture pic;
            QPainter* painter=new QPainter(&pic);
            pltParamHistogramY->get_plotter()->draw(*painter, QRect(0,0,pltParamCorrelation->width(),pltParamCorrelation->height()));
            delete painter;
            double scale=0.25*document->textWidth()/double(pic.boundingRect().width());
            if (scale<=0) scale=1;
            insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
        }
        tabCursor=table->cellAt(1, 1).firstCursorPosition();
        {
            QPicture pic;
            QPainter* painter=new QPainter(&pic);
            pltParamHistogramX->get_plotter()->draw(*painter, QRect(0,0,pltParamCorrelation->width(),pltParamCorrelation->height()));
            delete painter;
            double scale=0.25*document->textWidth()/double(pic.boundingRect().width());
            if (scale<=0) scale=1;
            insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);
        }

        tabCursor=table->cellAt(0, 2).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        //tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><nobr><span style=\"font-size: 7pt;\">%1</span></nobr></center>").arg(tvHistogramParameters->toHtml(1,true))));
        QPicture picT;
        QPainter* painter=new QPainter(&picT);
        tvHistogramParameters->paint(*painter);
        delete painter;
        double scale=0.3*document->textWidth()/double(picT.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("statistics table:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, picT, QSizeF(picT.boundingRect().width(), picT.boundingRect().height())*scale);

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


void QFParameterCorrelationView::dataSettingsChanged(bool update)
{
    edtHistogramMin1->setEnabled(!chkHistogramRangeAuto1->isChecked());
    edtHistogramMax1->setEnabled(!chkHistogramRangeAuto1->isChecked());
    edtHistogramMin2->setEnabled(!chkHistogramRangeAuto2->isChecked());
    edtHistogramMax2->setEnabled(!chkHistogramRangeAuto2->isChecked());
    if (update) updateCorrelation(true);
    emit settingsChanged();
}

void QFParameterCorrelationView::createWidgets()
{

    //////////////////////////////////////////////////////////////////////////////////////////
    // HISTOGRAM TAB
    //////////////////////////////////////////////////////////////////////////////////////////
    QWidget* widHist=this;
    layHist=new QGridLayout();
    widHist->setLayout(layHist);

    // HISTOGRAM SETTINGS/////////////////////////////////////////////////////////////////////
    QHBoxLayout* coll;
    grpHistogramSettings=new QGroupBox(tr("histogram style"), this);
    flHistSet=new QFormLayout(grpHistogramSettings);
    grpHistogramSettings->setLayout(flHistSet);

    cmbSymbol=new JKQTPSymbolComboBox(this);
    flHistSet->addRow(tr("<b>symbol:</b>"), cmbSymbol);
    spinSymbolSize=new QSpinBox(this);
    spinSymbolSize->setRange(1,30);
    spinSymbolSize->setValue(5);
    flHistSet->addRow(tr("<b>symbol size:</b>"), spinSymbolSize);


    spinHistogramBins1=new QSpinBox(this);
    spinHistogramBins1->setRange(5,99999);
    spinHistogramBins1->setValue(50);
    coll=new QHBoxLayout();
    coll->addWidget(spinHistogramBins1);
    coll->addStretch();
    flHistSet->addRow(tr("<b>Data X:</b>"), new QWidget(this));
    flHistSet->addRow(tr("  # bins:"), coll);
    chkHistogramRangeAuto1=new QCheckBox("auto", grpHistogramSettings);
    flHistSet->addRow(tr("  range:"), chkHistogramRangeAuto1);
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
    flHistSet->addRow(tr("  # bins:"), coll);
    chkHistogramRangeAuto2=new QCheckBox("auto", grpHistogramSettings);
    flHistSet->addRow(tr("  range:"), chkHistogramRangeAuto2);
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




    // HISTOGRAM PLOTS ///////////////////////////////////////////////////////////////////////
    splitterHistogram=new QVisibleHandleSplitter(this);
    QWidget* widPlotter=new QWidget(this);
    QGridLayout* layPlots=new QGridLayout(this);
    widPlotter->setLayout(layPlots);

    pltParamCorrelation=new QFPlotter(this);
    pltParamCorrelation->setMinimumWidth(100);

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

    pltParamHistogramY=new QFPlotter(false, this, pltParamCorrelation->getDatastore());
    pltParamHistogramY->setMinimumWidth(100);
    pltParamHistogramY->get_plotter()->synchronizeToMaster(pltParamCorrelation->get_plotter(), true, false);
    pltParamHistogramY->getXAxis()->set_inverted(true);
    pltParamHistogramY->getYAxis()->set_drawMode1(JKQTPCADMticks);
    pltParamHistogramY->get_plotter()->set_showKey(false);
    pltParamHistogramY->set_displayToolbar(false);
    pltParamHistogramY->set_displayMousePosition(false);
    connect(pltParamCorrelation, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), pltParamHistogramY, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));
    connect(pltParamHistogramY, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), pltParamCorrelation, SLOT(synchronizeXAxis(double,double,double,double,JKQtPlotter*)));


    pltParamHistogramX->get_plotter()->addGridPrintingPlotter(0,1,pltParamCorrelation->get_plotter());
    pltParamHistogramX->get_plotter()->addGridPrintingPlotter(1,1,pltParamHistogramY->get_plotter());

    layPlots->addWidget(pltParamCorrelation, 0,1);
    layPlots->addWidget(pltParamHistogramX, 1,1);
    layPlots->addWidget(pltParamHistogramY, 0,0);
    layPlots->setColumnStretch(0,1);
    layPlots->setColumnStretch(1,3);
    layPlots->setRowStretch(0,3);
    layPlots->setRowStretch(1,1);

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
    tabHistogramParameters->setCellCreate(4, 0, tr("min"));
    tabHistogramParameters->setCellCreate(5, 0, tr("25% quantile"));
    tabHistogramParameters->setCellCreate(6, 0, tr("75% quantile"));
    tabHistogramParameters->setCellCreate(7, 0, tr("max"));
    tabHistogramParameters->setCellCreate(8, 0, tr("skewness &gamma;<sub>1</sub>"));
    tabHistogramParameters->setCellCreate(9, 0, tr("invalid values"));
    tabHistogramParameters->setReadonly(true);

    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->setItemDelegate(new QFHTMLDelegate(tvHistogramParameters));
    tvHistogramParameters->setAlternatingRowColors(true);
    QFontMetrics tvfm(font());
    tvHistogramParameters->verticalHeader()->setDefaultSectionSize((int)round((double)tvfm.height()*1.5));


    QWidget* widHTab=new QWidget(this);
    laySplitterTable=new QVBoxLayout(widHTab);
    laySplitterTable->setContentsMargins(0,0,0,0);
    widHTab->setLayout(laySplitterTable);
    laySplitterTable->addWidget(tvHistogramParameters);


    splitterHistogram->setChildrenCollapsible(false);
    splitterHistogram->addWidget(widPlotter);
    splitterHistogram->addWidget(widHTab);
    layHist->addWidget(splitterHistogram, 0, 0);
    layHist->addWidget(grpHistogramSettings, 0, 1);
    layHist->setColumnStretch(0,5);
    layHist->setContentsMargins(0,0,0,0);

    //connectParameterWidgets(true);

    setSpaceSavingMode(true);
}


