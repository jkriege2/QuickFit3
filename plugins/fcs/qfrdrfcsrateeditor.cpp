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

#include "qfrdrfcsrateeditor.h"
#include "qfrdrfcsdata.h"
#include "qfrawdatapropertyeditor.h"
#include "qfprdrfcs.h"

QFRDRFCSRateEditor::runsModel::runsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    current=NULL;
}

void QFRDRFCSRateEditor::runsModel::setCurrent(QFRawDataRecord* current) {
    this->current=current;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
};
QVariant QFRDRFCSRateEditor::runsModel::data(const QModelIndex &index, int role) const {
    if (!current || !index.isValid()) return QVariant();
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (role==Qt::BackgroundColorRole) {
        QPalette pal;
        if (index.row()>0) {
            if (m->leaveoutRun(index.row()-1)) return pal.color(QPalette::Window).darker(130);
            return pal.color(QPalette::Base);
        }
        return pal.color(QPalette::Base);
    } else if (role==Qt::DisplayRole) {
        if (index.row()==0) return tr("none");
        return tr("Run %1").arg(index.row()-1);
    }
    return QVariant();
}

void QFRDRFCSRateEditor::excludeRuns() {
    if (!current) return;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
    for (int i=0; i<l.size(); i++) {
        //std::cout<<"adding run "<<l[i].row()-1<<std::endl;
        m->leaveoutAddRun(l[i].row()-1);
    }
    m->recalculateCorrelations();
}

void QFRDRFCSRateEditor::includeRuns() {
    if (!current) return;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
    for (int i=0; i<l.size(); i++) {
        //std::cout<<"removing run "<<l[i].row()-1<<std::endl;
        m->leaveoutRemoveRun(l[i].row()-1);
    }
    m->recalculateCorrelations();
}


Qt::ItemFlags QFRDRFCSRateEditor::runsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRFCSRateEditor::runsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

int QFRDRFCSRateEditor::runsModel::rowCount(const QModelIndex &parent) const {
    if (!current) return 0;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    return 1+m->getRateRuns();
}

int QFRDRFCSRateEditor::runsModel::columnCount(const QModelIndex &parent) const {
    return 1;
}





QFRDRFCSRateEditor::QFRDRFCSRateEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRFCSRateEditor::~QFRDRFCSRateEditor()
{
    //dtor
}


void QFRDRFCSRateEditor::createWidgets() {


    QVBoxLayout* l=new QVBoxLayout();
    setLayout(l);

    /*QHBoxLayout* hl=new QHBoxLayout();
    hl->addWidget(new QLabel(tr("display mode:")));
    hl->addWidget(cmbRateDisplay);
    l->addLayout(hl);*/

    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    l->addWidget(splitter);

    QWidget* w=new QWidget(this);
    QGridLayout* gl=new QGridLayout();
    w->setLayout(gl);

    cmbRateDisplay=new QComboBox(this);
    cmbRateDisplay->addItem(tr("binned or none"));
    cmbRateDisplay->addItem(tr("binned or full"));// default
    cmbRateDisplay->addItem(tr("full or binned"));
    cmbRateDisplay->addItem(tr("none"));
    cmbRateDisplay->setCurrentIndex(1);
    connect(cmbRateDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData(int)));
    gl->addWidget(new QLabel(tr("data display mode:")), 1, 0);
    gl->addWidget(cmbRateDisplay, 1, 1);


    cmbRunDisplay=new QComboBox(w);
    gl->addWidget(new QLabel(tr("runs display mode:")), 2, 0);
    gl->addWidget(cmbRunDisplay, 2, 1);
    cmbRunDisplay->addItem(tr("all runs"));
    cmbRunDisplay->addItem(tr("all runs (highlighted)"));
    cmbRunDisplay->addItem(tr("used runs"));
    cmbRunDisplay->addItem(tr("selected run"));
    connect(cmbRunDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(runsModeChanged(int)));
    lstRunsSelect=new QListView(w);
    lstRunsSelect->setModel(&runs);
    lstRunsSelect->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(lstRunsSelect->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(lstRunsSelect->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &, const QModelIndex &)));


    gl->addWidget(new QLabel(tr("select runs to display:")), 3, 0, 1, 2);
    gl->addWidget(lstRunsSelect, 4, 0, 3, 2);
    btnDontUse=new QPushButton(tr("exclude selected"), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    gl->addWidget(btnDontUse, 7, 0, 1, 2);
    btnUse=new QPushButton(tr("include selected"), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    gl->addWidget(btnUse, 8, 0, 1, 2);

    chkDisplayStatistics=new QCheckBox("", w);
    gl->addWidget(new QLabel(tr("display statistics:")), 9, 0);
    gl->addWidget(chkDisplayStatistics, 9, 1);
    connect(chkDisplayStatistics, SIGNAL(toggled(bool)), this, SLOT(replotData()));

    chkDisplayAverage=new QCheckBox("", w);
    gl->addWidget(new QLabel(tr("display average<br>in plot:")), 10, 0);
    gl->addWidget(chkDisplayAverage, 10, 1);
    connect(chkDisplayAverage, SIGNAL(clicked()), this, SLOT(replotData()));

    chkIncludeRate0=new QCheckBox("", w);
    gl->addWidget(new QLabel(tr("include 0 on<br>rate axis:")), 11, 0);
    gl->addWidget(chkIncludeRate0, 11, 1);
    connect(chkIncludeRate0, SIGNAL(clicked()), this, SLOT(replotData()));

    chkOverlay=new QCheckBox("", w);
    gl->addWidget(new QLabel(tr("overlay other<br>count rates:")), 12, 0);
    gl->addWidget(chkOverlay, 12, 1);
    connect(chkOverlay, SIGNAL(clicked()), this, SLOT(replotData()));


    grpInfo=new QGroupBox(tr("Info"), w);
    QGridLayout* ggl=new QGridLayout();
    grpInfo->setLayout(ggl);
    ggl->addWidget(new QLabel(tr("# Runs = ")), 0, 0);
    labRuns=new QLabel(grpInfo);
    ggl->addWidget(labRuns, 0,1);
    ggl->addWidget(new QLabel(tr("# Points = ")), 1, 0);
    labRatePoints=new QLabel(grpInfo);
    ggl->addWidget(labRatePoints, 1,1);
    gl->addWidget(grpInfo, 13,0,1,2);

    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout();
    wp->setLayout(lp);
    plotter = new QFPlotter(true, this);
    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    lp->addWidget(plotter);

    labRateData=new QLabel(this);
    lp->addWidget(labRateData);
    /*sliders=new DataCutSliders(this);
    connect(sliders, SIGNAL(slidersChanged(int , int , int, int)), this, SLOT(slidersChanged(int, int, int, int)));
    lp->addWidget(sliders);*/

    splitter->addWidget(wp);
    splitter->addWidget(w);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0,5);
    splitter->setStretchFactor(1,1);

    QMenu* menuFCSTools=propertyEditor->addOrFindMenu(tr("FCS Tools"));
    QAction* actSetBackground=new QAction(tr("set background from this record ..."), this);
    connect(actSetBackground, SIGNAL(triggered()), this, SLOT(setBackrgoundFromThisRDR()));
    menuFCSTools->addAction(actSetBackground);

};

void QFRDRFCSRateEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) disconnect(old, 0, this, 0);
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);

    if (old) {
        writeSettings();
    }

    if (m) {
        readSettings();
        cmbRunDisplay->setCurrentIndex(m->getProperty("FCS_RATE_RUN_DISPLAY", ProgramOptions::getConfigValue("fcsdataeditor/raterun_display", 0)).toInt());
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        runs.setCurrent(current);
    } else {
//        runs.setCurrent(current);
    }
    //sliders->set_min(0);
    //sliders->set_max(m->getCorrelationN());

    //readSettings();

};

void QFRDRFCSRateEditor::selectionChanged(const QModelIndex & current, const QModelIndex & previous ) {
    replotData();
}

void QFRDRFCSRateEditor::selectionChanged(const QItemSelection & current, const QItemSelection & previous ) {
    replotData();
}

void QFRDRFCSRateEditor::runsModeChanged(int c) {
    lstRunsSelect->setEnabled((cmbRunDisplay->currentIndex()<=1) || (cmbRunDisplay->currentIndex()==3));
    replotData();
}

void QFRDRFCSRateEditor::rawDataChanged() {
    replotData();
}

void QFRDRFCSRateEditor::rawDataChangedRecalc()
{    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
     //qDebug()<<"rawDataChangedRecalc()  m="<<m;
     if (m) m->recalculateCorrelations();
     replotData();

};

void QFRDRFCSRateEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    replotData();
}


void QFRDRFCSRateEditor::setBackrgoundFromThisRDR()
{
    QVector<double> cnt, cnt_sd;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (m) {
        unsigned int rateChannels=m->getRateChannels();
        for (unsigned int channel=0; channel<rateChannels; channel++) {
            cnt<<m->getSimpleCountrateAverage(-1, channel, false)*1000.0;
            cnt_sd<<m->getSimpleCountrateStdDev(-1, channel, false)*1000.0;
        }
        QFPRDRFCS* plug=dynamic_cast<QFPRDRFCS*>(services->getRawDataRecordFactory()->getPlugin("fcs"));
        if (plug) {
            plug->setBackgroundInFCS(cnt, cnt_sd);
        }
    }
}

QString QFRDRFCSRateEditor::plotItem(QFRDRFCSData* m) {
    QString labText="";
    QString name=m->getName();

    JKQTPdatastore* ds=plotter->getDatastore();

    for (int channel=0; channel<m->getRateChannels(); channel++) {
        double* rateT=m->getRateT();
        double* rate=m->getRate(channel);
        unsigned long rateN=m->getRateN();
        unsigned int rateRuns=m->getRateRuns();
        unsigned int rateChannels=m->getRateChannels();
        //qDebug()<<channel<<rateT<<rate<<rateN<<rateRuns<<rateChannels;
        QString binned="";
        QString channelName="";
        if (rateChannels>1) channelName=tr(" channel %1").arg(channel);
        if (cmbRateDisplay->currentIndex()==0) {
            rateT=m->getBinnedRateT();
            rate=m->getBinnedRate(channel);
            rateN=m->getBinnedRateN();
            binned=tr("bin ");
        } else if (cmbRateDisplay->currentIndex()==1) {
            if (m->getBinnedRateN()>0){
                rateT=m->getBinnedRateT();
                rate=m->getBinnedRate(channel);
                rateN=m->getBinnedRateN();
                binned=tr("bin ");
            }
        } else if (cmbRateDisplay->currentIndex()==2) {
            if (rateN<=0){
                rateT=m->getBinnedRateT();
                rate=m->getBinnedRate(channel);
                rateN=m->getBinnedRateN();
                binned=tr("bin ");
            }
        }

        if (rateN>0 && cmbRateDisplay->currentIndex()<3) {
            size_t c_tau=ds->addColumn(rateT, rateN, "time");
            double tt[2];
            tt[0]=rateT[0];
            tt[1]=rateT[rateN-1];
            //size_t c_tau2=ds->addCopiedColumn(tt, 2, "time");

            if (cmbRunDisplay->currentIndex()==0) { // plot all runs
                for (unsigned int i=0; i<rateRuns; i++) {
                    size_t c_run=ds->addColumn(&(rate[i*rateN]), rateN, QString("%3:%4 %2run %1").arg(i).arg(binned).arg(name).arg(channelName));

                    JKQTPxyLineGraph* g=new JKQTPxyLineGraph(plotter->get_plotter());
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_title(tr("\\verb{%3}%4 %2run %1").arg(i).arg(binned).arg(name).arg(channelName));
                    plotter->addGraph(g);
                    if (chkDisplayAverage->isChecked() || chkDisplayStatistics->isChecked()) {
                        double mean=m->getRateMean(i, channel);
                        double stddev=m->getRateStdDev(i, channel);
                        double mi,ma;
                        m->getRateMinMax(i, mi, ma, channel);
                        labText+=tr("<tr><td>&nbsp;<font color='%6'>&diams;&nbsp;</font>%7:%8 %1&nbsp;</td><td>&nbsp;%2 +/- %3&nbsp;</td><td>&nbsp;%4&nbsp;</td><td>&nbsp;%5&nbsp;</td></tr>").arg(i).arg(mean).arg(stddev).arg(mi).arg(ma).arg(g->get_color().name()).arg(name).arg(channelName);

                        if (chkDisplayAverage->isChecked()) {
                                JKQTPhorizontalRange* r=new JKQTPhorizontalRange(plotter->get_plotter());
                                QColor c=g->get_color().darker().darker();
                                r->set_color(c);
                                r->set_centerColor(g->get_color().darker());
                                r->set_rangeMin(mean-stddev);
                                r->set_rangeMax(mean+stddev);
                                r->set_plotCenterLine(true);
                                r->set_rangeCenter(mean);
                                c.setAlphaF(0.2);
                                r->set_fillColor(c);
                                //r->set_title(tr("%2:%3 run %1: avg").arg(i).arg(name).arg(channelName));
                                plotter->addGraph(r);

                        }
                    }
                }
            } else if (cmbRunDisplay->currentIndex()==1) { // plot all runs with the current one selected
                for (unsigned int i=0; i<rateRuns; i++) {
                    size_t c_run=ds->addColumn(&(rate[i*rateN]), rateN, QString("%2%3 run %1").arg(i).arg(binned).arg(channelName));
                    if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(plotter->get_plotter());
                        g->set_lineWidth(1);
                        if (channel==0) {
                            g->set_color(QColor("red"));
                        } else {
                            g->set_color(QColor("orange"));
                        }
                        g->set_xColumn(c_tau);
                        g->set_yColumn(c_run);
                        g->set_title(tr("\\verb{%3}:%4 %2run %1").arg(i).arg(binned).arg(name).arg(channelName));
                        plotter->addGraph(g);
                        if (chkDisplayAverage->isChecked() || chkDisplayStatistics->isChecked()) {
                            double mean=m->getRateMean(i, channel);
                            double stddev=m->getRateStdDev(i, channel);
                            double mi,ma;
                            m->getRateMinMax(i, mi, ma, channel);
                            labText+=tr("<tr><td>&nbsp;<font color='%6'>&diams;&nbsp;</font>%7:%8 %1&nbsp;</td><td>&nbsp;%2 +/- %3&nbsp;</td><td>&nbsp;%4&nbsp;</td><td>&nbsp;%5&nbsp;</td></tr>").arg(i).arg(mean).arg(stddev).arg(mi).arg(ma).arg("red").arg(name).arg(channelName);

                            if (chkDisplayAverage->isChecked()) {
                                JKQTPhorizontalRange* r=new JKQTPhorizontalRange(plotter->get_plotter());
                                QColor c=g->get_color().darker().darker();
                                r->set_color(g->get_color().darker().darker());
                                r->set_centerColor(g->get_color().darker());
                                r->set_rangeMin(mean-stddev);
                                r->set_rangeMax(mean+stddev);
                                r->set_plotCenterLine(true);
                                r->set_rangeCenter(mean);
                                c.setAlphaF(0.2);
                                r->set_fillColor(c);
                                //r->set_title(tr("%2:%3 run %1: avg").arg(i).arg(name).arg(channelName));
                                plotter->addGraph(r);
                            }
                        }
                    } else {
                        if (!m->leaveoutRun(i)) {
                            plotter->get_plotter()->addGraph(c_tau, c_run, tr("\\verb{%3}:%4 %2run %1").arg(i).arg(binned).arg(name).arg(channelName), JKQTPlines, QColor("black"), JKQTPnoSymbol, Qt::SolidLine, 1);
                        } else {
                            plotter->get_plotter()->addGraph(c_tau, c_run, tr("\\verb{%3}:%4 %2run %1").arg(i).arg(binned).arg(name).arg(channelName), JKQTPlines, QColor("grey"), JKQTPnoSymbol, Qt::SolidLine, 1);
                        }
                    }
                }
            } else if (cmbRunDisplay->currentIndex()==2) { // plot only selected runs
                for (unsigned int i=0; i<rateRuns; i++) {
                    if (!m->leaveoutRun(i)) {
                        size_t c_run=ds->addColumn(&(rate[i*rateN]), rateN, QString("%3 %2 run %1").arg(i).arg(binned).arg(channelName));

                        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(plotter->get_plotter());
                        g->set_lineWidth(1);
                        g->set_xColumn(c_tau);
                        g->set_yColumn(c_run);
                        g->set_title(tr("\\verb{%3}:%4 %2run %1").arg(i).arg(binned).arg(name).arg(channelName));
                        plotter->addGraph(g);
                        if (chkDisplayAverage->isChecked() || chkDisplayStatistics->isChecked()) {
                            double mean=m->getRateMean(i, channel);
                            double stddev=m->getRateStdDev(i, channel);
                            double mi,ma;
                            m->getRateMinMax(i, mi, ma, channel);
                            labText+=tr("<tr><td>&nbsp;<font color='%6'>&diams;&nbsp;</font>%7:%8 %1&nbsp;</td><td>&nbsp;%2 +/- %3&nbsp;</td><td>&nbsp;%4&nbsp;</td><td>&nbsp;%5&nbsp;</td></tr>").arg(i).arg(mean).arg(stddev).arg(mi).arg(ma).arg(g->get_color().name()).arg(name).arg(channelName);
                            if (chkDisplayAverage->isChecked()) {
                                JKQTPhorizontalRange* r=new JKQTPhorizontalRange(plotter->get_plotter());
                                QColor c=g->get_color().darker().darker();
                                r->set_color(g->get_color().darker().darker());
                                r->set_centerColor(g->get_color().darker());
                                r->set_rangeMin(mean-stddev);
                                r->set_rangeMax(mean+stddev);
                                r->set_plotCenterLine(true);
                                r->set_rangeCenter(mean);
                                c.setAlphaF(0.2);
                                r->set_fillColor(c);
                                //r->set_title(tr("%2:%3 run %1: avg").arg(i).arg(name).arg(channelName));
                                plotter->addGraph(r);

                            }
                        }
                    }
                }
            }  else if (cmbRunDisplay->currentIndex()==3) { // plot only current run
                for (unsigned int i=0; i<rateRuns; i++) {
                    if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                        size_t c_run=ds->addColumn(&(rate[i*rateN]), rateN, QString("%3 %2 run %1").arg(i).arg(binned).arg(channelName));

                        JKQTPxyLineGraph* g=new JKQTPxyLineGraph(plotter->get_plotter());
                        g->set_lineWidth(1);
                        g->set_xColumn(c_tau);
                        g->set_yColumn(c_run);
                        g->set_title(tr("\\verb{%3}:%4 %2run %1").arg(i).arg(binned).arg(name).arg(channelName));
                        plotter->addGraph(g);
                        if (chkDisplayAverage->isChecked() || chkDisplayStatistics->isChecked()) {
                            double mean=m->getRateMean(i, channel);
                            double stddev=m->getRateStdDev(i, channel);
                            double mi,ma;
                            m->getRateMinMax(i, mi, ma, channel);
                            labText+=tr("<tr><td>&nbsp;<font color='%6'>&diams;&nbsp;</font>%7:%8 %1&nbsp;</td><td>&nbsp;%2 +/- %3&nbsp;</td><td>&nbsp;%4&nbsp;</td><td>&nbsp;%5&nbsp;</td></tr>").arg(i).arg(mean).arg(stddev).arg(mi).arg(ma).arg(g->get_color().name()).arg(name).arg(channelName);
                            if (chkDisplayAverage->isChecked()) {
                                JKQTPhorizontalRange* r=new JKQTPhorizontalRange(plotter->get_plotter());
                                QColor c=g->get_color().darker().darker();
                                r->set_color(g->get_color().darker().darker());
                                r->set_centerColor(g->get_color().darker());
                                r->set_rangeMin(mean-stddev);
                                r->set_rangeMax(mean+stddev);
                                r->set_plotCenterLine(true);
                                r->set_rangeCenter(mean);
                                c.setAlphaF(0.2);
                                r->set_fillColor(c);
                                //r->set_title(tr("%2:%3 run %1: avg").arg(i).arg(name).arg(channelName));
                                plotter->addGraph(r);

                            }
                        }
                    }
                }
            }

        }
    }
    return labText;

}

void QFRDRFCSRateEditor::replotData(int dummy) {
    //std::cout<<"repainting ...";
    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        ds->clear();
        return;
    }
    //std::cout<<"*QFRDRFCSRateEditor::replotData()\n";
    //writeSettings();
    plotter->set_emitSignals(false);
    plotter->set_doDrawing(false);
    plotter->clearGraphs();
    ds->clear();
    //m->getProject()->setIntProperty();
    labRuns->setText(QString::number(m->getRateRuns()));
    labRatePoints->setText(QString::number(m->getRateN()));

/*
    cmbRateDisplay->addItem(tr("binned count rate or none"));// default
    cmbRateDisplay->addItem(tr("binned count rate or full count rate"));
    cmbRateDisplay->addItem(tr("full count rate or binned rate"));
    cmbRateDisplay->addItem(tr("full count rate or none"));
    cmbRateDisplay->addItem(tr("none"));
    cmbRateDisplay->setCurrentIndex(1);
*/
    QString labText=tr("<table border='1' cellpadding='0' cellspacing='0'><thead><tr bgcolor='darkgrey'><th>&nbsp;File: Run&nbsp;</th><th>&nbsp;Average Count Rate [kHz]&nbsp;</th><th>&nbsp;min rate [kHz]&nbsp;</th><th>&nbsp;max rate [kHz]&nbsp;</th></tr></thead><tbody>");

    labText+=plotItem(m);
    if (chkOverlay->isChecked()) {
        QFRawDataRecord* r=current;
        while (r->getNextOfSameType()!=current && r->getNextOfSameType()!=NULL) {
            r=r->getNextOfSameType();
            QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(r);
            labText+=plotItem(m);
        }
    }

    labText+=tr("</tbody></table>");
    plotter->getXAxis()->set_logAxis(false);
    plotter->getYAxis()->set_logAxis(false);
    plotter->getXAxis()->set_axisLabel(tr("time [s]"));
    plotter->getYAxis()->set_axisLabel(tr("count rate [kHz]"));
    plotter->zoomToFit(true, true, false,chkIncludeRate0->isChecked());
    if (chkDisplayStatistics->isChecked()) labRateData->setText(labText); else labRateData->setText("");
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotter->update_plot();
    //size_t gg=ds->addLinearColumn(10, 1, 10, "");
    //plotter->addGraph(gg, gg, "test");
    //std::cout<<" done!\n";
    //std::cout<<"*QFRDRFCSRateEditor::replotData() end\n";
};


void QFRDRFCSRateEditor::readSettings() {
    //std::cout<<"QFRDRFCSRateEditor::readSettings()\n";
    if (!settings) return;
    //std::cout<<"QFRDRFCSRateEditor::readSettings()\n";
    plotter->loadSettings(*(settings->getQSettings()), QString("fcsdataeditor/rateplot"));
    //splitter->restoreState(settings->getQSettings()->value(QString("fcsdataeditor/ratesplitterSizes")).toByteArray());
    cmbRunDisplay->setCurrentIndex(settings->getQSettings()->value(QString("fcsdataeditor/raterun_display"), 0).toInt());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("fcsdataeditor/rateaverage_display"), true).toBool());
    chkDisplayStatistics->setChecked(settings->getQSettings()->value(QString("fcsdataeditor/ratestatistics_display"), false).toBool());
    chkIncludeRate0->setChecked(settings->getQSettings()->value(QString("fcsdataeditor/include_rate0"), true).toBool());
    chkOverlay->setChecked(settings->getQSettings()->value(QString("fcsdataeditor/rate_overlay"), false).toBool());
    loadSplitter(*(settings->getQSettings()), splitter, "fcsdataeditor/ratesplitterSizes");

    replotData();
};


void QFRDRFCSRateEditor::writeSettings() {
    //std::cout<<"QFRDRFCSRateEditor::writeSettings()\n";
    if (!settings) return;
    //plotter->saveSettings(*(settings->getQSettings()), QString("fcsdataeditor/rateplot"));
    //settings->getQSettings()->setValue(QString("fcsdataeditor/ratesplitterSizes"), splitter->saveState());
    settings->getQSettings()->setValue(QString("fcsdataeditor/raterun_display"), cmbRunDisplay->currentIndex());
    settings->getQSettings()->setValue(QString("fcsdataeditor/rateaverage_display"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("fcsdataeditor/ratestatistics_display"), chkDisplayStatistics->isChecked());
    settings->getQSettings()->setValue(QString("fcsdataeditor/include_rate0"), chkIncludeRate0->isChecked());
    settings->getQSettings()->setValue(QString("fcsdataeditor/rate_overlay"), chkOverlay->isChecked());
    saveSplitter(*(settings->getQSettings()), splitter, "fcsdataeditor/ratesplitterSizes");
};



