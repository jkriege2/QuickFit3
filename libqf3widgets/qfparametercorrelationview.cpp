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
    connectParameterWidgets(true);
}

void QFParameterCorrelationView::writeSettings(QSettings &settings, const QString &prefix)
{
    saveSplitter(settings, splitterHistogram, prefix+"splitterhistogramSizes");
}

void QFParameterCorrelationView::writeQFProperties(QFProperties *current, const QString &prefix, const QString &egroup, const QString &param)
{
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
        connect(spinHistogramBins1, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        connect(chkHistogramRangeAuto1, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMin1, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMax1, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));

        connect(spinHistogramBins2, SIGNAL(valueChanged(int)), this, SLOT(histogramSettingsChanged()));
        connect(chkHistogramRangeAuto2, SIGNAL(toggled(bool)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMin2, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
        connect(edtHistogramMax2, SIGNAL(valueChanged(double)), this, SLOT(histogramSettingsChanged()));
    } else {
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
    pltParamHistogram->set_doDrawing(false);
    pltParamHistogram->getYAxis()->set_logAxis(false);
    pltParamHistogram->getXAxis()->set_axisLabel(histLabelX);
    pltParamHistogram->getYAxis()->set_axisLabel(histLabelY);
    pltParamHistogram->zoomToFit(true, true, false, false);
    pltParamHistogram->set_doDrawing(true);
    pltParamHistogram->update_plot();
}

void QFParameterCorrelationView::updateCorrelation(bool replot, int which)
{



    edtHistogramMin1->setEnabled(!chkHistogramRangeAuto1->isChecked());
    edtHistogramMax1->setEnabled(!chkHistogramRangeAuto1->isChecked());
    edtHistogramMin2->setEnabled(!chkHistogramRangeAuto2->isChecked());
    edtHistogramMax2->setEnabled(!chkHistogramRangeAuto2->isChecked());

    pltParamHistogram->set_doDrawing(false);
    tvHistogramParameters->setModel(NULL);
    tabHistogramParameters->setReadonly(false);

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
        CorrelationItem hist=histograms[which];
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
                double* datahist=(double*)malloc(imageSize*sizeof(double));
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


                JKQTPxyLineGraph* g=new JKQTPxyLineGraph(pltParamHistogram->get_plotter());
                // TODO: ADD SCATTE RPLOT!

                pltParamHistogram->addGraph(g);

            }
        }
    }


    tabHistogramParameters->setReadonly(true);
    tvHistogramParameters->setModel(tabHistogramParameters);
    tvHistogramParameters->resizeColumnsToContents();
    pltParamHistogram->set_doDrawing(true);
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


    double w1=qMax(1,pltParamHistogram->width());
    double w2=qMax(1,tvHistogramParameters->width());
    double allwidth=qMax(1.0,w1+w2);
    cursor.insertBlock();
    cursor.insertText(tr("Parameter Correlations:\n"), fHeading2);
    QTextTable* table = cursor.insertTable(1, 2, tableFormat1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        QPainter* painter=new QPainter(&pic);
        pltParamHistogram->get_plotter()->draw(*painter, QRect(0,0,pltParamHistogram->width(),pltParamHistogram->height()));
        delete painter;
        double scale=0.5*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale);

        tabCursor=table->cellAt(0, 1).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        //tabCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<center><nobr><span style=\"font-size: 7pt;\">%1</span></nobr></center>").arg(tvHistogramParameters->toHtml(1,true))));
        QPicture picT;
        painter=new QPainter(&picT);
        tvHistogramParameters->paint(*painter);
        delete painter;
        scale=0.4*document->textWidth()/double(picT.boundingRect().width());
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
    splitterHistogram->addWidget(pltParamHistogram);
    splitterHistogram->addWidget(widHTab);
    layHist->addWidget(splitterHistogram, 0, 0);
    layHist->addWidget(grpHistogramSettings, 0, 1);
    layHist->setColumnStretch(0,5);
    layHist->setContentsMargins(0,0,0,0);

    //connectParameterWidgets(true);

    setSpaceSavingMode(true);
}


