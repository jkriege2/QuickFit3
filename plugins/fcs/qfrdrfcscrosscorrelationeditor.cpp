/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-12-05 21:55:50 +0100 (Fr, 05 Dez 2014) $  (revision $Rev: 3660 $)

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

#include "qfrdrfcscrosscorrelationeditor.h"
#include "qfrdrfcsdata.h"
#include "qfrawdatapropertyeditor.h"


QFRDRFCSCrossCorrelationEditorRunsModel::QFRDRFCSCrossCorrelationEditorRunsModel(QFRDRFCSCrossCorrelationEditor *editor, QObject* parent):
    QAbstractTableModel(parent)
{
    current=NULL;
    this->editor=editor;
}

void QFRDRFCSCrossCorrelationEditorRunsModel::setCurrent(QFRawDataRecord* current) {
    this->current=current;
    reset();
};


QVariant QFRDRFCSCrossCorrelationEditorRunsModel::data(const QModelIndex &index, int role) const {
    if (!current || !index.isValid()) return QVariant();
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (role==Qt::BackgroundColorRole) {
        QPalette pal;
        if (index.row()>0) {
            int incl=editor->isIncluded(current, index.row()-1);
            if (incl==0) return pal.color(QPalette::Window).darker(130);
            else if (incl==2) return pal.color(QPalette::Window).darker(110);
            return pal.color(QPalette::Base);
        }
        return pal.color(QPalette::Base);
    } else if (role==Qt::ToolTipRole) {
        QPalette pal;
        if (index.row()>0) {
            int incl=editor->isIncluded(current, index.row()-1);
            if (incl==0) return tr("this run is <u>excluded in <b>all</b> RDRs</u> in the group");
            else if (incl==2) return tr("this run is <u>excluded in <b>some</b> RDRs</u> in the group");
            return tr("this run is <u>included in <b>all</b> RDRs</u> in the group");
        }
        return pal.color(QPalette::Base);
    } else if (role==Qt::DisplayRole) {
        if (index.row()==0) return tr("none");
        return m->getCorrelationRunName(index.row()-1);//tr("Run %1").arg(index.row()-1);
    }
    return QVariant();
}

Qt::ItemFlags QFRDRFCSCrossCorrelationEditorRunsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRFCSCrossCorrelationEditorRunsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

int QFRDRFCSCrossCorrelationEditorRunsModel::rowCount(const QModelIndex &parent) const {
    if (!current) return 0;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (m) return 1+m->getCorrelationRuns();
    return 1;
}

int QFRDRFCSCrossCorrelationEditorRunsModel::columnCount(const QModelIndex &parent) const {
    return 1;
}


void QFRDRFCSCrossCorrelationEditor::excludeRuns() {
    if (!current) return;
    QFRDRFCSData* mc=qobject_cast<QFRDRFCSData*>(current);
    if (mc) {
        QList<QFRDRFCSData *> ms=getRecordsInGroup();
        for (int j=0; j<ms.size(); j++) {
            QFRDRFCSData* m=ms[j];
            if (m) {
                QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
                for (int i=0; i<l.size(); i++) {
                    //std::cout<<"adding run "<<l[i].row()-1<<std::endl;
                    m->leaveoutAddRun(l[i].row()-1);
                }
                m->recalculateCorrelations();
            }
        }
    }
}

void QFRDRFCSCrossCorrelationEditor::includeRuns() {
    if (!current) return;
    QFRDRFCSData* mc=qobject_cast<QFRDRFCSData*>(current);
    if (mc) {
        QList<QFRDRFCSData *> ms=getRecordsInGroup();
        for (int j=0; j<ms.size(); j++) {
            QFRDRFCSData* m=ms[j];
            if (m) {
                QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
                for (int i=0; i<l.size(); i++) {
                    //std::cout<<"adding run "<<l[i].row()-1<<std::endl;
                    m->leaveoutRemoveRun(l[i].row()-1);
                }
                m->recalculateCorrelations();
            }
        }
    }
}

int QFRDRFCSCrossCorrelationEditor::isIncluded(QFRawDataRecord *current, int index) const
{
    int res=0;
    QList<QFRDRFCSData *> l=getRecordsInGroup(current);
    for (int i=0; i<l.size(); i++) {
        if (!l[i]->leaveoutRun(index)) {
            if (res==0) res=1;
        } else {
            if (res==1) res=2;
        }
    }
    return res;
}



QFRDRFCSCrossCorrelationEditor::QFRDRFCSCrossCorrelationEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    runs=new QFRDRFCSCrossCorrelationEditorRunsModel(this, this);
    createWidgets();
}

QFRDRFCSCrossCorrelationEditor::~QFRDRFCSCrossCorrelationEditor()
{
    //dtor
}

QList<QFRDRFCSData *> QFRDRFCSCrossCorrelationEditor::getRecordsInGroup(QFRawDataRecord* current, QStringList *roles, QList<QColor> *graph_colors) const
{
    QList<QFRawDataRecord *> li;
    QList<QFRDRFCSData*> l;
    QList<QColor> c;
    QStringList r;
    if (current) {
        QFProject* p=current->getProject();
        if (p) {
            li=current->getGroupMembers();
            for (int i=0; i<li.size(); i++) {
                QFRDRFCSData* f=qobject_cast<QFRDRFCSData*>(li[i]);
                if (f) {
                    l<<f;
                    r<<f->getRole();
                }
            }
            QString fccs="", acf1="", acf2="";
            for (int i=0; i<r.size(); i++) {
                if (r[i].toUpper().startsWith("FCCS")) fccs=r[i];
                if (r[i].toUpper().startsWith("ACF")) {
                    if (acf1.isEmpty()) acf1=r[i];
                    else if (acf2.isEmpty()) acf2=r[i];
                }
            }
            if (qfReadFirstInt(acf1)>qfReadFirstInt(acf2)) {
                qSwap(acf1, acf2);
            }
            bool isFCCS=!fccs.isEmpty() && !acf1.isEmpty() && !acf2.isEmpty();

            for (int i=0; i<l.size(); i++) {
                QFRDRFCSData* f=l[i];
                QColor col=getCycleColor(i, 2*li.size()-1, 1,1);
                if (isFCCS) {
                    if (f->getRole()==fccs) col=QColor("blue");
                    else if (f->getRole()==acf1) col=QColor("darkgreen");
                    else if (f->getRole()==acf2) col=QColor("red");
                }
                c<<col;
            }
        }
    }
    if (roles) *roles=r;
    if (graph_colors) *graph_colors=c;
    return l;
}

QList<QFRDRFCSData *> QFRDRFCSCrossCorrelationEditor::getRecordsInGroup(QStringList *roles, QList<QColor> *graph_colors) const
{
    return getRecordsInGroup(current, roles, graph_colors);
}

void QFRDRFCSCrossCorrelationEditor::createWidgets() {
    correlationMaskTools=new QFCorrelationMaskTools(this);
    connect(correlationMaskTools, SIGNAL(rawDataChanged()), this, SLOT(rawDataChangedRecalc()));

    QVBoxLayout* l=new QVBoxLayout();
    setLayout(l);
    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    l->addWidget(splitter);

    QWidget* w=new QWidget(this);
    QGridLayout* gl=new QGridLayout();
    w->setLayout(gl);
    cmbAverageErrors=new QComboBox(w);
    gl->addWidget(new QLabel(tr("display average:")), 0, 0);
    gl->addWidget(cmbAverageErrors, 0, 1);
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no average"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no errors"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_elines.png"), tr("with error lines"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_ebars.png"), tr("with error bars"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_elinesbars.png"), tr("with lines and bars"));
    cmbAverageErrors->addItem(QIcon(":/libqf3widgets/plot_epoly.png"), tr("with error polygons"));
    cmbAverageErrors->addItem(QIcon(":/libqf3widgets/plot_epolybars.png"), tr("with polygons and bars"));
    connect(cmbAverageErrors, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData(int)));

    cmbRunDisplay=new QComboBox(w);
    gl->addWidget(new QLabel(tr("display runs:")), 1, 0);
    gl->addWidget(cmbRunDisplay, 1, 1);
    cmbRunDisplay->addItem(tr("average only"));
    cmbRunDisplay->addItem(tr("selected run"));
    connect(cmbRunDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(runsModeChanged(int)));

    cmbRunErrors=new QComboBox(w);
    gl->addWidget(new QLabel(tr("run errors:")), 2, 0);
    gl->addWidget(cmbRunErrors, 2, 1);
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no errors"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_elines.png"), tr("with error lines"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_ebars.png"), tr("with error bars"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_elinesbars.png"), tr("with lines and bars"));
    cmbRunErrors->addItem(QIcon(":/libqf3widgets/plot_epoly.png"), tr("with error polygons"));
    cmbRunErrors->addItem(QIcon(":/libqf3widgets/plot_epolybars.png"), tr("with polygons and bars"));
    connect(cmbRunErrors, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData(int)));

    lstRunsSelect=new QListView(w);
    lstRunsSelect->setModel(runs);
    lstRunsSelect->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(lstRunsSelect->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(lstRunsSelect->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &, const QModelIndex &)));


    gl->addWidget(new QLabel(tr("select runs to display:")), 3, 0, 1, 2);
    gl->addWidget(lstRunsSelect, 4, 0, 3, 2);
    gl->setRowStretch(4, 2);
    btnDontUse=new QPushButton(tr("exclude selected"), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    gl->addWidget(btnDontUse, 7, 0, 1, 2);
    btnUse=new QPushButton(tr("include selected"), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    gl->addWidget(btnUse, 8, 0, 1, 2);

    chkLogTauAxis=new QCheckBox("", w);
    gl->addWidget(new QLabel(tr("log-scale on<br>lag time axis:")), 9, 0);
    gl->addWidget(chkLogTauAxis, 9, 1);
    connect(chkLogTauAxis, SIGNAL(toggled(bool)), this, SLOT(replotData()));

    grpInfo=new QGroupBox(tr("Info"), w);
    QGridLayout* ggl=new QGridLayout();
    grpInfo->setLayout(ggl);
    ggl->addWidget(new QLabel(tr("# Runs = ")), 0, 0);
    labRuns=new QLabel(grpInfo);
    ggl->addWidget(labRuns, 0,1);
    ggl->addWidget(new QLabel(tr("# Points = ")), 1, 0);
    labCorrelationPoints=new QLabel(grpInfo);
    ggl->addWidget(labCorrelationPoints, 1,1);
    gl->addWidget(grpInfo, 10,0,1,2);

    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout();
    wp->setLayout(lp);
    plotter = new QFPlotter(true, this);
    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    lp->addWidget(plotter);
    sliders=new DataCutSliders(this);
    connect(sliders, SIGNAL(slidersChanged(int , int , int, int)), this, SLOT(slidersChanged(int, int, int, int)));
    lp->addWidget(sliders);

    splitter->addWidget(wp);
    splitter->addWidget(w);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0,5);
    splitter->setStretchFactor(1,1);

    actCopyNormalizedACF=new QAction(tr("copy normalized CFs to table"), this);
    connect(actCopyNormalizedACF, SIGNAL(triggered()), this, SLOT(copyNormalizedACFs()));
    actCorrectOffset=new QAction(tr("correct CFs for offset"), this);
    connect(actCorrectOffset, SIGNAL(triggered()), this, SLOT(correctOffset()));

    menuMask=propertyEditor->addMenu("&Selection/Mask", 0);
    correlationMaskTools->registerMaskToolsToMenu(menuMask);
    menuMask->addSeparator();
    correlationMaskTools->registerCorrelationToolsToMenu(menuMask);

    menuData=propertyEditor->addOrFindMenu(tr("&Data"), -1);
    //menuData->addAction(actCopyNormalizedACF);
    menuData->addAction(actCorrectOffset);


}

void QFRDRFCSCrossCorrelationEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        writeSettings();
        disconnect(old, 0, this, 0);
    }
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    correlationMaskTools->setRDR(current);
    if (m) {
        readSettings();
        cmbRunDisplay->setCurrentIndex(m->getProperty("FCS_RUN_DISPLAY", ProgramOptions::getConfigValue("fcsdataeditor/run_display", 0)).toInt());
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        runs->setCurrent(current);
        sliders->disableSliderSignals();
        sliders->set_min(0);
        sliders->set_max(m->getCorrelationN());
        sliders->set_userMin(current->getProperty("fcscorreditor_datacut_min", 0).toInt());
        sliders->set_userMax(current->getProperty("fcscorreditor_datacut_max", m->getCorrelationN()).toInt());
        sliders->enableSliderSignals();

    } else {
//        runs.setCurrent(current);
    }

    //readSettings();

}

void QFRDRFCSCrossCorrelationEditor::selectionChanged(const QModelIndex & current, const QModelIndex & previous ) {
    replotData();
}

void QFRDRFCSCrossCorrelationEditor::selectionChanged(const QItemSelection & current, const QItemSelection & previous ) {
    replotData();
}

void QFRDRFCSCrossCorrelationEditor::runsModeChanged(int c) {
    if (cmbRunDisplay->currentIndex()==0) lstRunsSelect->setEnabled(false);
    else lstRunsSelect->setEnabled(true);
    replotData();
}

void QFRDRFCSCrossCorrelationEditor::rawDataChanged() {
    replotData();
}

void QFRDRFCSCrossCorrelationEditor::rawDataChangedRecalc()
{
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
     //qDebug()<<"rawDataChangedRecalc()  m="<<m;
     if (m) m->recalculateCorrelations();
     replotData();
     runs->setCurrent(current);
}

void QFRDRFCSCrossCorrelationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    current->setQFProperty("fcscorreditor_datacut_min", userMin, false, false);
    current->setQFProperty("fcscorreditor_datacut_max", userMax, false, false);
    replotData();
}

void QFRDRFCSCrossCorrelationEditor::copyNormalizedACFs()
{

}

void QFRDRFCSCrossCorrelationEditor::correctOffset()
{
    if (!current) return;
    bool ok=false;
    double offset=QInputDialog::getDouble(this, tr("FCS Offset correction"), tr("enter the offset, which should be subtracted (NOTE: A reload of the project will be required!)"), current->getProperty("CORR_OFFSET", 0.0).toDouble(),-2147483647,2147483647,5, &ok);
    if (ok)  {
        current->setQFProperty("CF_CORRECT_OFFSET", offset, true, true);
        if (QMessageBox::information(this, tr("corect correlation offset"), tr("The project needs to be saved and reloaded for the offset correction to take effect!\nReload project now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
            QFPluginServices::getInstance()->reloadCurrentProject();
        }

    }
}



void QFRDRFCSCrossCorrelationEditor::replotData(int dummy) {
    //std::cout<<"repainting ...";
    //writeSettings();
    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        ds->clear();
        return;
    }
    //writeSettings();
    plotter->set_doDrawing(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    plotter->set_emitSignals(false);
    plotter->clearGraphs();
    ds->clear();
    //m->getProject()->setIntProperty();
    labRuns->setText(QString::number(m->getCorrelationRuns()));
    labCorrelationPoints->setText(QString::number(m->getCorrelationN()));
    JKQTPerrorPlotstyle runstyle=JKQTPnoError;
    switch (cmbRunErrors->currentIndex()) {
        case 1: runstyle=JKQTPerrorLines; break;
        case 2: runstyle=JKQTPerrorBars; break;
        case 3: runstyle=JKQTPerrorBarsLines; break;
        case 4: runstyle=JKQTPerrorPolygons; break;
        case 5: runstyle=JKQTPerrorBarsPolygons; break;
    }
    //std::cout<<"repainting ... 1\n";

    QList<QFRDRFCSData* > ms;
    QStringList roles;
    QList<QColor> graph_colors;

    ms=getRecordsInGroup(current, &roles, &graph_colors);

    QList<JKQTPgraph*> topGraphs;
    for (int msi=0; msi<ms.size(); msi++){
        QFRDRFCSData* m=ms[msi];
        if (m->getCorrelationN()>0) {
            size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");
            //std::cout<<"repainting ... 2\n";


            //////////////////////////////////////////////////////////////////////////////////
            // Plot average + error markers
            //////////////////////////////////////////////////////////////////////////////////
            if (cmbAverageErrors->currentIndex()>=1) {
                size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), QString("%1: cmean").arg(roles[msi]));
                size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), QString("%1: cstddev").arg(roles[msi]));
                JKQTPerrorPlotstyle styl=JKQTPnoError;
                switch (cmbAverageErrors->currentIndex()) {
                    case 2: styl=JKQTPerrorLines; break;
                    case 3: styl=JKQTPerrorBars; break;
                    case 4: styl=JKQTPerrorBarsLines; break;
                    case 5: styl=JKQTPerrorPolygons; break;
                    case 6: styl=JKQTPerrorBarsPolygons; break;
                }


                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                g->set_color(graph_colors[msi]);
                QColor errc=g->get_color().lighter();
                g->set_errorColor(errc);
                errc.setAlphaF(0.5);
                g->set_errorFillColor(errc);
                g->set_lineWidth(2);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_mean);
                g->set_yErrorColumn(c_std);
                g->set_title(tr("%1: average run").arg(roles[msi]));
                g->set_yErrorStyle(styl);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_datarange_start(sliders->get_userMin());
                g->set_datarange_end(sliders->get_userMax());
                plotter->addGraph(g);
            }
            //std::cout<<"repainting ... 3\n";

            //////////////////////////////////////////////////////////////////////////////////
            // Plot ALL RUNS
            //////////////////////////////////////////////////////////////////////////////////
            if (cmbRunDisplay->currentIndex()==1) {
                //////////////////////////////////////////////////////////////////////////////////
                // Plot only highlighted run
                //////////////////////////////////////////////////////////////////////////////////
                for (int i=0; i<m->getCorrelationRuns(); i++) {
                    if (lstRunsSelect->selectionModel()->isSelected(runs->index(i+1, 0))) {
                        size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("%2: run %1").arg(i).arg(roles[msi]));
                        size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("%2: run error %1").arg(i).arg(roles[msi]));
                        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                        g->set_lineWidth(1);
                        g->set_xColumn(c_tau);
                        g->set_yColumn(c_run);
                        g->set_title(tr("%2: run %1").arg(i).arg(roles[msi]));
                        g->set_color(graph_colors[msi]);
                        g->set_datarange_start(sliders->get_userMin());
                        g->set_datarange_end(sliders->get_userMax());

                        g->set_yErrorColumn(c_rune);
                        g->set_yErrorStyle(runstyle);
                        g->set_xErrorStyle(JKQTPnoError);
                        QColor errc=g->get_color().lighter();
                        g->set_errorColor(errc);
                        errc.setAlphaF(0.5);
                        g->set_errorFillColor(errc);
                        g->set_errorWidth(1);
                        g->set_style(Qt::DashLine);

                        //plotter->addGraph(g);
                        topGraphs<<g;
                    }
                }
            }

        }
    }

    plotter->addGraphs(topGraphs);
    plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    plotter->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->zoomToFit(true, true, false,false);
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotter->update_plot();
}


void QFRDRFCSCrossCorrelationEditor::readSettings() {
    if (!settings) return;
    //std::cout<<"--QFRDRFCSCrossCorrelationEditor::readSettings()\n";
    plotter->loadSettings(*(settings->getQSettings()), QString("fcsdataeditor/corrplot"));
    //splitter->restoreState(settings->getQSettings()->value(QString("fcsdataeditor/corrsplitterSizes")).toByteArray());
    chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("fcsdataeditor/log_tau_axis"), true).toBool());
    cmbAverageErrors->setCurrentIndex(settings->getQSettings()->value(QString("fcsdataeditor/error_display"), 2).toInt());
    cmbRunErrors->setCurrentIndex(settings->getQSettings()->value(QString("fcsdataeditor/run_error_display"), 0).toInt());
    cmbRunDisplay->setCurrentIndex(settings->getQSettings()->value(QString("fcsdataeditor/run_display"), 0).toInt());
    loadSplitter(*(settings->getQSettings()), splitter, "fcsdataeditor/corrsplitterSizes");
}


void QFRDRFCSCrossCorrelationEditor::writeSettings() {
    if (!settings) return;
    //std::cout<<"--QFRDRFCSCrossCorrelationEditor::writeSettings()\n";
    //plotter->saveSettings(*(settings->getQSettings()), QString("fcsdataeditor/corrplot"));
    //settings->getQSettings()->setValue(QString("fcsdataeditor/corrsplitterSizes"), splitter->saveState());
    settings->getQSettings()->setValue(QString("fcsdataeditor/log_tau_axis"), chkLogTauAxis->isChecked());
    settings->getQSettings()->setValue(QString("fcsdataeditor/error_display"), cmbAverageErrors->currentIndex());
    settings->getQSettings()->setValue(QString("fcsdataeditor/run_error_display"), cmbRunErrors->currentIndex());
    settings->getQSettings()->setValue(QString("fcsdataeditor/run_display"), cmbRunDisplay->currentIndex());
    saveSplitter(*(settings->getQSettings()), splitter, "fcsdataeditor/corrsplitterSizes");
}



