#include "qfrdrphotoncounts_dataeditor.h"
#include "qfrdrphotoncounts_data.h"
#include "qfrdrphotoncountsinterface.h"



QFRDRPhotonCountsDataEditorChannelsModel::QFRDRPhotonCountsDataEditorChannelsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    current=NULL;
}

void QFRDRPhotonCountsDataEditorChannelsModel::setCurrent(QFRawDataRecord* current) {
    this->current=current;
    reset();
};


QVariant QFRDRPhotonCountsDataEditorChannelsModel::data(const QModelIndex &index, int role) const {
    if (!current || !index.isValid()) return QVariant();
    QFRDRPhotonCountsInterface* m=qobject_cast<QFRDRPhotonCountsInterface*>(current);
    if (role==Qt::BackgroundColorRole) {
        QPalette pal;
        if (index.row()>0) {
            return pal.color(QPalette::Base);
        }
        return pal.color(QPalette::Base);
    } else if (role==Qt::DisplayRole) {
        if (index.row()==0) return tr("none");
        return m->getPhotonCountsChannelName(index.row()-1);//tr("Run %1").arg(index.row()-1);
    }
    return QVariant();
}



Qt::ItemFlags QFRDRPhotonCountsDataEditorChannelsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRPhotonCountsDataEditorChannelsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

int QFRDRPhotonCountsDataEditorChannelsModel::rowCount(const QModelIndex &parent) const {
    if (!current) return 0;
    QFRDRPhotonCountsInterface* m=qobject_cast<QFRDRPhotonCountsInterface*>(current);
    if (m) return 1+m->getPhotonCountsChannels();
    return 1;
}

int QFRDRPhotonCountsDataEditorChannelsModel::columnCount(const QModelIndex &parent) const {
    return 1;
}









QFRDRPhotonCountsDataEditor::QFRDRPhotonCountsDataEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    createWidgets();
}

QFRDRPhotonCountsDataEditor::~QFRDRPhotonCountsDataEditor()
{
    //dtor
}

void QFRDRPhotonCountsDataEditor::createWidgets() {
    QVBoxLayout* l=new QVBoxLayout(this);
    setLayout(l);

    /*QHBoxLayout* hl=new QHBoxLayout(this);
    hl->addWidget(new QLabel(tr("display mode:")));
    hl->addWidget(cmbRateDisplay);
    l->addLayout(hl);*/

    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    l->addWidget(splitter);

    QWidget* widOptionsRHS=new QWidget(this);
    QGridLayout* glOptionsRHS=new QGridLayout(this);
    int glRowCount=0;
    widOptionsRHS->setLayout(glOptionsRHS);


    cmbRunDisplay=new QComboBox(widOptionsRHS);
    glOptionsRHS->addWidget(new QLabel(tr("runs display mode:")), glRowCount, 0);
    glOptionsRHS->addWidget(cmbRunDisplay, glRowCount, 1);
    cmbRunDisplay->addItem(tr("all runs"));
    cmbRunDisplay->addItem(tr("all runs (highlighted)"));
    cmbRunDisplay->addItem(tr("selected run"));
    connect(cmbRunDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(runsModeChanged(int)));
    lstRunsSelect=new QListView(widOptionsRHS);
    lstRunsSelect->setModel(&runs);
    lstRunsSelect->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(lstRunsSelect->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(lstRunsSelect->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &, const QModelIndex &)));


    glRowCount++;
    glOptionsRHS->addWidget(new QLabel(tr("select runs to display:")), glRowCount, 0, 1, 2);
    glRowCount++;
    glOptionsRHS->addWidget(lstRunsSelect, glRowCount, 0, 3, 2);
    glRowCount+=3;

    chkDisplayStatistics=new QCheckBox("", widOptionsRHS);
    glOptionsRHS->addWidget(new QLabel(tr("display statistics:")), glRowCount, 0);
    glOptionsRHS->addWidget(chkDisplayStatistics, glRowCount, 1);
    connect(chkDisplayStatistics, SIGNAL(toggled(bool)), this, SLOT(replotData()));

    glRowCount++;
    chkDisplayAverage=new QCheckBox("", widOptionsRHS);
    glOptionsRHS->addWidget(new QLabel(tr("display average<br>in plot:")), glRowCount, 0);
    glOptionsRHS->addWidget(chkDisplayAverage, glRowCount, 1);
    connect(chkDisplayAverage, SIGNAL(clicked()), this, SLOT(replotData()));

    glRowCount++;
    chkIncludeRate0=new QCheckBox("", widOptionsRHS);
    glOptionsRHS->addWidget(new QLabel(tr("include 0 on<br>rate axis:")), glRowCount, 0);
    glOptionsRHS->addWidget(chkIncludeRate0, glRowCount, 1);
    connect(chkIncludeRate0, SIGNAL(clicked()), this, SLOT(replotData()));

    glRowCount++;
    cmbPlotStyle=new QComboBox(widOptionsRHS);
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_lines.png"), tr("lines"));
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_points.png"), tr("points"));
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_bars.png"), tr("bars"));
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_steps.png"), tr("steps"));
    cmbPlotStyle->addItem(QIcon(":/libqf3widgets/plot_impulses.png"), tr("impulses"));
    glOptionsRHS->addWidget(new QLabel(tr("plot style:")), glRowCount, 0);
    glOptionsRHS->addWidget(cmbPlotStyle, glRowCount, 1);
    connect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    connect(cmbPlotStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotDetail()));

    glRowCount++;
    spinBinning=new QSpinBox(widOptionsRHS);
    spinBinning->setRange(10,1000000);
    glOptionsRHS->addWidget(new QLabel(tr("#points in<br>binned curve:")), glRowCount, 0);
    glOptionsRHS->addWidget(spinBinning, glRowCount, 1);



    glRowCount++;
    grpInfo=new QGroupBox(tr("Info"), widOptionsRHS);
    QGridLayout* ggl=new QGridLayout(grpInfo);
    grpInfo->setLayout(ggl);
    ggl->addWidget(new QLabel(tr("# Runs = ")), 0, 0);
    labRuns=new QLabel(grpInfo);
    ggl->addWidget(labRuns, 0,1);
    ggl->addWidget(new QLabel(tr("# Points = ")), 1, 0);
    labRatePoints=new QLabel(grpInfo);
    ggl->addWidget(labRatePoints, 1,1);
    labRateData=new QLabel(this);
    ggl->addWidget(labRateData,2,0,2,2);
    glOptionsRHS->addWidget(grpInfo, glRowCount,0,1,2);
    glRowCount+=2;

    glOptionsRHS->addItem(new QSpacerItem(5,10,QSizePolicy::Minimum, QSizePolicy::Expanding), glRowCount, 0);
    glRowCount++;

    plotterBinned = new JKQtPlotter(true, this);
    ovlRange=new JKQTPoverlayVerticalRange(0,0,plotterBinned->get_plotter());
    ovlRange->set_color(QColor("red"));
    plotterBinned->get_plotter()->addOverlayElement(ovlRange);

    QWidget* widPlotterDetail=new QWidget(this);
    QVBoxLayout* vbPlotterDetail=new QVBoxLayout(this);
    widPlotterDetail->setLayout(vbPlotterDetail);
    spinDetailWidth=new QDoubleSpinBox(this);
    spinDetailWidth->setRange(0.001,1e9);
    spinDetailWidth->setDecimals(3);
    spinDetailWidth->setValue(10);
    spinDetailWidth->setSuffix(tr(" ms"));
    QHBoxLayout* hbPlotterDetailControl=new QHBoxLayout(this);
    hbPlotterDetailControl->addWidget(new QLabel(tr("detail width: "), this));
    hbPlotterDetailControl->addWidget(spinDetailWidth);
    hbPlotterDetailControl->addWidget(new QLabel(tr("     detail position: "), this));
    scrollDetail=new QScrollBar(Qt::Horizontal, this);
    hbPlotterDetailControl->addWidget(scrollDetail, 1);
    labScrollRange=new QLabel("", this);
    labScrollRange->setMinimumWidth(120);
    labScrollRange->setMaximumWidth(120);
    hbPlotterDetailControl->addWidget(labScrollRange);
    vbPlotterDetail->addLayout(hbPlotterDetailControl);
    plotterDetail = new JKQtPlotter(true, this);
    vbPlotterDetail->addWidget(plotterDetail);

    splitterPlots=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterPlots->addWidget(plotterBinned);
    splitterPlots->addWidget(widPlotterDetail);

    splitter->addWidget(splitterPlots);
    splitter->addWidget(widOptionsRHS);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0,5);
    splitter->setStretchFactor(1,1);
};

void QFRDRPhotonCountsDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
	// this functions connects to a new QFRawDataRecord which could also be NULL, so
	// you will have to connect and disconnect the signals! This is called whenever
	// a user navigates to the next record.
	// here is a basic skeleton:
	
    if (old) disconnect(old, 0, this, 0); // first disconnect from last record
    disconnect(spinBinning, SIGNAL(valueChanged(int)), this, SLOT(binningChanged(int)));
    disconnect(spinDetailWidth, SIGNAL(valueChanged(double)), this, SLOT(detailWidthChanged(double)));
    disconnect(scrollDetail, SIGNAL(valueChanged(int)), this, SLOT(detailPosChanged(int)));

    QFRDRPhotonCountsData* m=qobject_cast<QFRDRPhotonCountsData*>(current); // check whether we have the right QFRawDataRecord class
    if (m) {
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        runs.setCurrent(current);
        spinBinning->setValue(m->GetPhotonCountsBinnedNFactor());
        spinDetailWidth->setValue(m->getProperty("PCOUNTEDITOR_DETAILWIDTH", spinDetailWidth->value()).toDouble());
        scrollDetail->setValue(0);
        scrollDetail->setRange(0, m->getPhotonCountsN()-1-getDetailWidth());
    } else {
        spinBinning->setValue(1000);
        scrollDetail->setValue(-1);
        scrollDetail->setRange(-1, -1);
    }
    connect(spinBinning, SIGNAL(valueChanged(int)), this, SLOT(binningChanged(int)));
    connect(spinDetailWidth, SIGNAL(valueChanged(double)), this, SLOT(detailWidthChanged(double)));
    connect(scrollDetail, SIGNAL(valueChanged(int)), this, SLOT(detailPosChanged(int)));


}

void QFRDRPhotonCountsDataEditor::replotData(int dummy) {
    //std::cout<<"repainting ...";
    JKQTPdatastore* ds=plotterBinned->getDatastore();
    QFRDRPhotonCountsData* m=qobject_cast<QFRDRPhotonCountsData*>(current);
    if (!m) {
        plotterBinned->clearGraphs();
        ds->clear();
        return;
    }
    plotterBinned->set_emitSignals(false);
    plotterBinned->set_doDrawing(false);
    plotterBinned->clearGraphs();
    ds->clear();
    labRuns->setText(QString::number(m->getPhotonCountsChannels()));
    labRatePoints->setText(tr("%1 (binned: %2)").arg(m->getPhotonCountsN()).arg(m->getPhotonCountsBinnedN()));

    QString labText=tr("<table border='1' cellpadding='0' cellspacing='0'><thead><tr bgcolor='darkgrey'><th rowspan=\"2\">&nbsp;channel&nbsp;</th><th colspan=\"3\">&nbsp;rate statistics [kHz]&nbsp;</th></tr>"
                       "<tr bgcolor='darkgrey'><th>&nbsp;avg &plusmn; stdev.&nbsp;</th><th>&nbsp;min&nbsp;</th><th>&nbsp;max&nbsp;</th></tr></thead><tbody>");

    labText+=plotBinnedItem(m);

    labText+=tr("</tbody></table>");
    plotterBinned->getXAxis()->set_logAxis(false);
    plotterBinned->getYAxis()->set_logAxis(false);
    plotterBinned->getXAxis()->set_axisLabel(tr("time [s]"));
    plotterBinned->getYAxis()->set_axisLabel(tr("count rate [kHz]"));
    plotterBinned->zoomToFit(true, true, false,chkIncludeRate0->isChecked());
    if (chkDisplayStatistics->isChecked()) labRateData->setText(labText); else labRateData->setText("");
    plotterBinned->set_doDrawing(true);
    plotterBinned->set_emitSignals(true);
    plotterBinned->update_plot();
}

void QFRDRPhotonCountsDataEditor::replotDetail() {
    //std::cout<<"repainting ...";
    JKQTPdatastore* ds=plotterDetail->getDatastore();
    QFRDRPhotonCountsData* m=qobject_cast<QFRDRPhotonCountsData*>(current);
    if (!m) {
        plotterDetail->clearGraphs();
        ds->clear();
        return;
    }
    plotterDetail->set_emitSignals(false);
    plotterDetail->set_doDrawing(false);
    plotterDetail->clearGraphs();
    ds->clear();

    plotDetailItem(m);
    plotterDetail->getXAxis()->set_logAxis(false);
    plotterDetail->getYAxis()->set_logAxis(false);
    plotterDetail->getXAxis()->set_axisLabel(tr("time [s]"));
    plotterDetail->getYAxis()->set_axisLabel(tr("photons"));
    double mmin, mmax;
    for (int c=0; c<m->getPhotonCountsBinnedChannels(); c++) {
        double mi, ma;
        m->getPhotonCountsMinMax(c, mi, ma, QFRDRPhotonCountsData::Counts);
        if (c==0) {
            mmin=mi;
            mmax=ma;
        } else {
            if (mi<mmin) mmin=mi;
            if (ma>mmax) mmax=ma;
        }
    }
    plotterDetail->setY(mmin, mmax*1.05);
    plotterDetail->zoomToFit(true, false, false,chkIncludeRate0->isChecked());
    plotterDetail->set_doDrawing(true);
    plotterDetail->set_emitSignals(true);
    plotterDetail->update_plot();
    labScrollRange->setText(QString("%1 - %2 s").arg(plotterDetail->getXMin()).arg(plotterDetail->getXMax()));
    ovlRange->set_position(plotterDetail->getXMin());
    ovlRange->set_position2(plotterDetail->getXMax());
    plotterBinned->update_overlays();
}

void QFRDRPhotonCountsDataEditor::selectionChanged(const QModelIndex &current, const QModelIndex &previous) {
    replotData();
    replotDetail();
}

void QFRDRPhotonCountsDataEditor::selectionChanged(const QItemSelection &current, const QItemSelection &previous) {
    replotData();
    replotDetail();
}

void QFRDRPhotonCountsDataEditor::runsModeChanged(int c) {
    lstRunsSelect->setEnabled(cmbRunDisplay->currentIndex()>0);
    replotData();

}

void QFRDRPhotonCountsDataEditor::binningChanged(int bin) {
    QFRDRPhotonCountsData* m=qobject_cast<QFRDRPhotonCountsData*>(current);
    m->setPhotonCountsBinnedN(bin);
    replotData();
}

void QFRDRPhotonCountsDataEditor::detailWidthChanged(double width) {
    QFRDRPhotonCountsData* m=qobject_cast<QFRDRPhotonCountsData*>(current);
    if (m) {
        scrollDetail->setRange(0, m->getPhotonCountsN()-1-getDetailWidth());
        m->setQFProperty("PCOUNTEDITOR_DETAILWIDTH", width, false, false);
    } else  scrollDetail->setRange(-1, -1);

    replotDetail();
}

void QFRDRPhotonCountsDataEditor::detailPosChanged(int pos) {
    replotDetail();
}

void QFRDRPhotonCountsDataEditor::rawDataChanged() {
	// emitted when the raw data changes 
	QFRDRPhotonCountsData* m=qobject_cast<QFRDRPhotonCountsData*>(current);
    replotData();
    replotDetail();
}

void QFRDRPhotonCountsDataEditor::readSettings() {
	// read widget settings
    if (!settings) return;
    plotterBinned->loadSettings(*(settings->getQSettings()), QString("photoncounteditor/binnedplot"));
    plotterDetail->loadSettings(*(settings->getQSettings()), QString("photoncounteditor/detailplot"));
    spinBinning->setValue(settings->getQSettings()->value(QString("photoncounteditor/binning"), 1000).toInt());
    spinDetailWidth->setValue(settings->getQSettings()->value(QString("photoncounteditor/detail_width"), 10).toDouble());
    cmbRunDisplay->setCurrentIndex(settings->getQSettings()->value(QString("photoncounteditor/raterun_display"), 0).toInt());
    cmbPlotStyle->setCurrentIndex(settings->getQSettings()->value(QString("photoncounteditor/plotstyle"), 0).toInt());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("photoncounteditor/rateaverage_display"), true).toBool());
    chkDisplayStatistics->setChecked(settings->getQSettings()->value(QString("photoncounteditor/ratestatistics_display"), false).toBool());
    chkIncludeRate0->setChecked(settings->getQSettings()->value(QString("photoncounteditor/include_rate0"), true).toBool());
    loadSplitter(*(settings->getQSettings()), splitter, "photoncounteditor/splitterSizes");
    loadSplitter(*(settings->getQSettings()), splitterPlots, "photoncounteditor/plotsplitterSizes");

    replotData();
    replotDetail();
}


void QFRDRPhotonCountsDataEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("photoncounteditor/binning"), spinBinning->value());
    settings->getQSettings()->setValue(QString("photoncounteditor/detail_width"), spinDetailWidth->value());
    settings->getQSettings()->setValue(QString("photoncounteditor/raterun_display"), cmbRunDisplay->currentIndex());
    settings->getQSettings()->setValue(QString("photoncounteditor/plotstyle"), cmbPlotStyle->currentIndex());
    settings->getQSettings()->setValue(QString("photoncounteditor/rateaverage_display"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("photoncounteditor/ratestatistics_display"), chkDisplayStatistics->isChecked());
    settings->getQSettings()->setValue(QString("photoncounteditor/include_rate0"), chkIncludeRate0->isChecked());
    saveSplitter(*(settings->getQSettings()), splitter, "photoncounteditor/splitterSizes");
    saveSplitter(*(settings->getQSettings()), splitterPlots, "photoncounteditor/plotsplitterSizes");
}

int QFRDRPhotonCountsDataEditor::getDetailWidth() const {
     QFRDRPhotonCountsData* m=qobject_cast<QFRDRPhotonCountsData*>(current);
    if (m) return qMax(long(10), long(ceil(spinDetailWidth->value()/1000.0/m->getAverageT())));
    else return 10;
}

QString QFRDRPhotonCountsDataEditor::addItemToPlot(double* rateT, double* rate, unsigned long rateN, long datarange_start, long datarange_end, unsigned int rateRuns, QString binned, QFRDRPhotonCountsData* m, JKQtPlotter* parent, bool showAvg) {
    QString labText="";
    QString name=m->getName();

    JKQTPdatastore* ds=parent->getDatastore();
    size_t c_tau=ds->addColumn(rateT, rateN, "time");
    double tt[2];
    tt[0]=rateT[0];
    tt[1]=rateT[rateN-1];
    //size_t c_tau2=ds->addCopiedColumn(tt, 2, "time");
    QColor color;
    if (cmbRunDisplay->currentIndex()==0) { // plot all runs
        for (unsigned int i=0; i<rateRuns; i++) {
            size_t c_run=ds->addColumn(&(rate[i*rateN]), rateN, QString("%2 channel %1").arg(i).arg(binned));

            JKQTPxyGraph* g=createPlot(Qt::transparent, parent, color);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_run);
            g->set_datarange_start(datarange_start);
            g->set_datarange_end(datarange_end);
            g->set_title(tr("%2 channel %1").arg(i).arg(binned));
            parent->addGraph(g);
            if ((showAvg && chkDisplayAverage->isChecked()) || chkDisplayStatistics->isChecked()) {
                double mean=m->getPhotonCountsMean(i, QFRDRPhotonCountsInterface::KiloHertz);
                double stddev=m->getPhotonCountsStdDev(i, QFRDRPhotonCountsInterface::KiloHertz);
                double mi,ma;
                m->getPhotonCountsMinMax(i, mi, ma, QFRDRPhotonCountsInterface::KiloHertz);
                labText+=tr("<tr><td>&nbsp;<font color='%6'>&diams;&nbsp;</font>%1&nbsp;</td><td>&nbsp;%2 &plusmn; %3&nbsp;</td><td>&nbsp;%4&nbsp;</td><td>&nbsp;%5&nbsp;</td></tr>").arg(i).arg(mean).arg(stddev).arg(mi).arg(ma).arg(color.name());

                if (showAvg && chkDisplayAverage->isChecked()) {
                        JKQTPhorizontalRange* r=new JKQTPhorizontalRange(parent->get_plotter());
                        QColor c=color.darker().darker();
                        r->set_color(c);
                        r->set_centerColor(color.darker());
                        r->set_rangeMin(mean-stddev);
                        r->set_rangeMax(mean+stddev);
                        r->set_plotCenterLine(true);
                        r->set_rangeCenter(mean);
                        c.setAlphaF(0.2);
                        r->set_fillColor(c);
                        //r->set_title(tr("%2: channel %1: avg").arg(i).arg(name));
                        parent->addGraph(r);

                }
            }
        }
    } else if (cmbRunDisplay->currentIndex()==1) { // plot all runs with the current one selected
        for (unsigned int i=0; i<rateRuns; i++) {
            size_t c_run=ds->addColumn(&(rate[i*rateN]), rateN, QString("%2 channel %1").arg(i).arg(binned));
            if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                JKQTPxyGraph* g=createPlot(QColor("red"), parent, color);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_datarange_start(datarange_start);
                g->set_datarange_end(datarange_end);
                g->set_title(tr("%2 channel %1").arg(i).arg(binned));
                parent->addGraph(g);
                if ((showAvg&&chkDisplayAverage->isChecked()) || chkDisplayStatistics->isChecked()) {
                    double mean=m->getPhotonCountsMean(i, QFRDRPhotonCountsInterface::KiloHertz);
                    double stddev=m->getPhotonCountsStdDev(i, QFRDRPhotonCountsInterface::KiloHertz);
                    double mi,ma;
                    m->getPhotonCountsMinMax(i, mi, ma, QFRDRPhotonCountsInterface::KiloHertz);
                    labText+=tr("<tr><td>&nbsp;<font color='%6'>&diams;&nbsp;</font>%1&nbsp;</td><td>&nbsp;%2 &plusmn; %3&nbsp;</td><td>&nbsp;%4&nbsp;</td><td>&nbsp;%5&nbsp;</td></tr>").arg(i).arg(mean).arg(stddev).arg(mi).arg(ma).arg("red");

                    if (showAvg&&chkDisplayAverage->isChecked()) {
                        JKQTPhorizontalRange* r=new JKQTPhorizontalRange(parent->get_plotter());
                        QColor c=color.darker().darker();
                        r->set_color(color.darker().darker());
                        r->set_centerColor(color.darker());
                        r->set_rangeMin(mean-stddev);
                        r->set_rangeMax(mean+stddev);
                        r->set_plotCenterLine(true);
                        r->set_rangeCenter(mean);
                        c.setAlphaF(0.2);
                        r->set_fillColor(c);
                        //r->set_title(tr("%2: run %1: avg").arg(i).arg(name));
                        parent->addGraph(r);
                    }
                }
            } else {
                JKQTPxyGraph* g=createPlot(QColor("black"), parent, color);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_datarange_start(datarange_start);
                g->set_datarange_end(datarange_end);
                g->set_title(tr("%2 channel %1").arg(i).arg(binned));
            }
        }

    }  else if (cmbRunDisplay->currentIndex()==2) { // plot only current runs
        for (unsigned int i=0; i<rateRuns; i++) {
            if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                size_t c_run=ds->addColumn(&(rate[i*rateN]), rateN, QString("%2 channel %1").arg(i).arg(binned));

                JKQTPxyGraph* g=createPlot(Qt::transparent, parent, color);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_datarange_start(datarange_start);
                g->set_datarange_end(datarange_end);
                g->set_title(tr("%2 channel %1").arg(i).arg(binned));
                parent->addGraph(g);
                if ((showAvg&&chkDisplayAverage->isChecked()) || chkDisplayStatistics->isChecked()) {
                    double mean=m->getPhotonCountsMean(i, QFRDRPhotonCountsInterface::KiloHertz);
                    double stddev=m->getPhotonCountsStdDev(i, QFRDRPhotonCountsInterface::KiloHertz);
                    double mi,ma;
                    m->getPhotonCountsMinMax(i, mi, ma, QFRDRPhotonCountsInterface::KiloHertz);
                    labText+=tr("<tr><td>&nbsp;<font color='%6'>&diams;&nbsp;</font> %1&nbsp;</td><td>&nbsp;%2 &plusmn; %3&nbsp;</td><td>&nbsp;%4&nbsp;</td><td>&nbsp;%5&nbsp;</td></tr>").arg(i).arg(mean).arg(stddev).arg(mi).arg(ma).arg(color.name());
                    if (showAvg&&chkDisplayAverage->isChecked()) {
                        JKQTPhorizontalRange* r=new JKQTPhorizontalRange(parent->get_plotter());
                        QColor c=color.darker().darker();
                        r->set_color(color.darker().darker());
                        r->set_centerColor(color.darker());
                        r->set_rangeMin(mean-stddev);
                        r->set_rangeMax(mean+stddev);
                        r->set_plotCenterLine(true);
                        r->set_rangeCenter(mean);
                        c.setAlphaF(0.2);
                        r->set_fillColor(c);
                        //r->set_title(tr("%2: channel %1: avg").arg(i).arg(name));
                        parent->addGraph(r);

                    }
                }
            }
        }
    }
    return labText;
}

QString QFRDRPhotonCountsDataEditor::plotBinnedItem(QFRDRPhotonCountsData* m) {


    double* rateT=m->getPhotonCountsBinnedT();
    double* rate=m->getPhotonCountsBinned();
    unsigned long rateN=m->getPhotonCountsBinnedN();
    unsigned int rateRuns=m->getPhotonCountsBinnedChannels();
    QString binned=tr("bin ");


    return addItemToPlot(rateT, rate, rateN, -1, -1, rateRuns, binned, m, plotterBinned, true);
}

QString QFRDRPhotonCountsDataEditor::plotDetailItem(QFRDRPhotonCountsData* m) {

    double* rateT=m->getPhotonCountsT();
    double* rate=m->getPhotonCounts();
    unsigned long rateN=m->getPhotonCountsN();
    unsigned int rateRuns=m->getPhotonCountsChannels();
    QString binned="";

    return addItemToPlot(rateT, rate, rateN, scrollDetail->value(), scrollDetail->value()+getDetailWidth(),  rateRuns, binned, m, plotterDetail, false);
}

JKQTPxyGraph *QFRDRPhotonCountsDataEditor::createPlot(QColor color, JKQtPlotter* parent, QColor& colorOut) {
    JKQTPxyGraph* g=NULL;
    if (cmbPlotStyle->currentIndex()==0) {
        JKQTPxyLineGraph* g1=new JKQTPxyLineGraph(parent->get_plotter());
        g1->set_lineWidth(1);
        if (color!=Qt::transparent) g1->set_color(color);
        g1->set_drawLine(true);
        colorOut=g1->get_color();
        g=g1;
    } else if (cmbPlotStyle->currentIndex()==1) {
        JKQTPxyLineGraph* g1=new JKQTPxyLineGraph(parent->get_plotter());
        g1->set_lineWidth(1);
        if (color!=Qt::transparent) g1->set_color(color);
        g1->set_drawLine(false);
        g1->set_symbol(JKQTPplus);
        colorOut=g1->get_color();
        g=g1;
    } else if (cmbPlotStyle->currentIndex()==2) {
        JKQTPbarHorizontalGraph* g1=new JKQTPbarHorizontalGraph(parent->get_plotter());
        g1->set_lineWidth(1);
        if (color!=Qt::transparent) {
            QColor c=color;
            c.setAlphaF(0.5);
            g1->set_fillColor(c);
        } else {
            QColor c=g1->get_fillColor();
            c.setAlphaF(0.5);
            g1->set_fillColor(c);
        }
        g1->set_color(color.darker());
        colorOut=g1->get_fillColor();
        colorOut.setAlphaF(1);
        g=g1;
    } else if (cmbPlotStyle->currentIndex()==3) {
        JKQTPstepHorizontalGraph* g1=new JKQTPstepHorizontalGraph(parent->get_plotter());
        g1->set_lineWidth(1);
        if (color!=Qt::transparent) g1->set_color(color);
        g1->set_fillCurve(false);
        g1->set_drawLine(true);
        g1->set_valuesCentered(false);
        colorOut=g1->get_color();
        g=g1;
    } else if (cmbPlotStyle->currentIndex()==4) {
        JKQTPimpulsesVerticalGraph* g1=new JKQTPimpulsesVerticalGraph(parent->get_plotter());
        g1->set_lineWidth(3);
        if (color!=Qt::transparent) g1->set_color(color);
        colorOut=g1->get_color();
        g=g1;
    }

    return g;
}

