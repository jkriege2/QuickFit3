#include "qfrdrfcscorrelationeditor.h"
#include "qfrdrfcsdata.h"


QFRDRFCSCorrelationEditor::runsModel::runsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    current=NULL;
}

void QFRDRFCSCorrelationEditor::runsModel::setCurrent(QFRawDataRecord* current) {
    this->current=current;
    reset();
};


QVariant QFRDRFCSCorrelationEditor::runsModel::data(const QModelIndex &index, int role) const {
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
        return m->getCorrelationRunName(index.row()-1);//tr("Run %1").arg(index.row()-1);
    }
    return QVariant();
}

void QFRDRFCSCorrelationEditor::excludeRuns() {
    if (!current) return;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (m) {
        QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
        for (int i=0; i<l.size(); i++) {
            //std::cout<<"adding run "<<l[i].row()-1<<std::endl;
            m->leaveoutAddRun(l[i].row()-1);
        }
        m->recalculateCorrelations();
    }
}

void QFRDRFCSCorrelationEditor::includeRuns() {
    if (!current) return;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
    if (m) {
        for (int i=0; i<l.size(); i++) {
            //std::cout<<"removing run "<<l[i].row()-1<<std::endl;
            m->leaveoutRemoveRun(l[i].row()-1);
        }
        m->recalculateCorrelations();
    }
}


Qt::ItemFlags QFRDRFCSCorrelationEditor::runsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRFCSCorrelationEditor::runsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

int QFRDRFCSCorrelationEditor::runsModel::rowCount(const QModelIndex &parent) const {
    if (!current) return 0;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (m) return 1+m->getCorrelationRuns();
    return 1;
}

int QFRDRFCSCorrelationEditor::runsModel::columnCount(const QModelIndex &parent) const {
    return 1;
}





QFRDRFCSCorrelationEditor::QFRDRFCSCorrelationEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    createWidgets();
}

QFRDRFCSCorrelationEditor::~QFRDRFCSCorrelationEditor()
{
    //dtor
}

void QFRDRFCSCorrelationEditor::createWidgets() {
    QVBoxLayout* l=new QVBoxLayout(this);
    setLayout(l);
    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    l->addWidget(splitter);

    QWidget* w=new QWidget(this);
    QGridLayout* gl=new QGridLayout(this);
    w->setLayout(gl);
    cmbAverageErrors=new QComboBox(w);
    gl->addWidget(new QLabel(tr("display average:")), 0, 0);
    gl->addWidget(cmbAverageErrors, 0, 1);
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no average"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no errors"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_elines.png"), tr("with error lines"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_ebars.png"), tr("with error bars"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_elinesbars.png"), tr("with lines and bars"));
    connect(cmbAverageErrors, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData(int)));

    cmbRunDisplay=new QComboBox(w);
    gl->addWidget(new QLabel(tr("display runs:")), 1, 0);
    gl->addWidget(cmbRunDisplay, 1, 1);
    cmbRunDisplay->addItem(tr("no runs"));
    cmbRunDisplay->addItem(tr("all runs"));
    cmbRunDisplay->addItem(tr("all runs (highlighted)"));
    cmbRunDisplay->addItem(tr("used runs"));
    cmbRunDisplay->addItem(tr("selected run"));
    connect(cmbRunDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(runsModeChanged(int)));

    cmbRunErrors=new QComboBox(w);
    gl->addWidget(new QLabel(tr("run errors:")), 2, 0);
    gl->addWidget(cmbRunErrors, 2, 1);
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no errors"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_elines.png"), tr("with error lines"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_ebars.png"), tr("with error bars"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_elinesbars.png"), tr("with lines and bars"));
    connect(cmbRunErrors, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData(int)));

    lstRunsSelect=new QListView(w);
    lstRunsSelect->setModel(&runs);
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
    connect(chkLogTauAxis, SIGNAL(clicked()), this, SLOT(replotData()));

    grpInfo=new QGroupBox(tr("Info"), w);
    QGridLayout* ggl=new QGridLayout(grpInfo);
    grpInfo->setLayout(ggl);
    ggl->addWidget(new QLabel(tr("# Runs = ")), 0, 0);
    labRuns=new QLabel(grpInfo);
    ggl->addWidget(labRuns, 0,1);
    ggl->addWidget(new QLabel(tr("# Points = ")), 1, 0);
    labCorrelationPoints=new QLabel(grpInfo);
    ggl->addWidget(labCorrelationPoints, 1,1);
    gl->addWidget(grpInfo, 10,0,1,2);

    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout(this);
    wp->setLayout(lp);
    plotter = new JKQtPlotter(true, this);
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

};

void QFRDRFCSCorrelationEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) disconnect(old, 0, this, 0);
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (m) {
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        runs.setCurrent(current);
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

};

void QFRDRFCSCorrelationEditor::selectionChanged(const QModelIndex & current, const QModelIndex & previous ) {
    replotData();
}

void QFRDRFCSCorrelationEditor::selectionChanged(const QItemSelection & current, const QItemSelection & previous ) {
    replotData();
}

void QFRDRFCSCorrelationEditor::runsModeChanged(int c) {
    if (cmbRunDisplay->currentIndex()<=1) lstRunsSelect->setEnabled(false);
    else lstRunsSelect->setEnabled(true);
    replotData();
}

void QFRDRFCSCorrelationEditor::rawDataChanged() {
    replotData();
};

void QFRDRFCSCorrelationEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    current->setQFProperty("fcscorreditor_datacut_min", userMin, false, false);
    current->setQFProperty("fcscorreditor_datacut_max", userMax, false, false);
    replotData();
}

void QFRDRFCSCorrelationEditor::replotData(int dummy) {
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
    }
    //std::cout<<"repainting ... 1\n";
    if (m->getCorrelationN()>0) {
        size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");
    //std::cout<<"repainting ... 2\n";


        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        if (cmbAverageErrors->currentIndex()>=1) {
            size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), "cmean");
            size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), "cstddev");
            JKQTPerrorPlotstyle styl=JKQTPnoError;
            switch (cmbAverageErrors->currentIndex()) {
                case 2: styl=JKQTPerrorLines; break;
                case 3: styl=JKQTPerrorBars; break;
                case 4: styl=JKQTPerrorBarsLines; break;
            }


            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
            g->set_color(QColor("blue"));
            g->set_errorColor(QColor("blue").lighter());
            g->set_lineWidth(2);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(tr("average run"));
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
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i).toStdString());
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i).toStdString());
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                g->set_lineWidth(1);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_title(tr("run %1").arg(i));
                g->set_datarange_start(sliders->get_userMin());
                g->set_datarange_end(sliders->get_userMax());

                g->set_yErrorColumn(c_rune);
                g->set_yErrorStyle(runstyle);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_errorColor(g->get_errorColor().lighter());
                g->set_errorWidth(1);

                plotter->addGraph(g);
            }
        } else if (cmbRunDisplay->currentIndex()==2) {
           //////////////////////////////////////////////////////////////////////////////////
           // Plot ALL RUNS (left out runs in gray, standard runs in black, highlighted run in red)
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i).toStdString());
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i).toStdString());
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                g->set_lineWidth(1);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_title(tr("run %1").arg(i));
                g->set_datarange_start(sliders->get_userMin());
                g->set_datarange_end(sliders->get_userMax());

                g->set_yErrorColumn(c_rune);
                g->set_yErrorStyle(runstyle);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_errorWidth(1);

                if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0)))
                        g->set_color(QColor("red"));
                else {
                    if (!m->leaveoutRun(i)) {
                        g->set_color(QColor("black"));
                    } else {
                        g->set_color(QColor("grey"));
                    }
                }
                g->set_errorColor(g->get_errorColor().lighter());
                plotter->addGraph(g);
            }
        } else if (cmbRunDisplay->currentIndex()==3) {
           //////////////////////////////////////////////////////////////////////////////////
           // Plot only used runs
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                if (!m->leaveoutRun(i)) {
                    size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i).toStdString());
                    size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i).toStdString());
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_title(tr("run %1").arg(i));
                    g->set_datarange_start(sliders->get_userMin());
                    g->set_datarange_end(sliders->get_userMax());

                    g->set_yErrorColumn(c_rune);
                    g->set_yErrorStyle(runstyle);
                    g->set_xErrorStyle(JKQTPnoError);
                    g->set_errorColor(g->get_errorColor().lighter());
                    g->set_errorWidth(1);

                    plotter->addGraph(g);
                }
            }
        } else if (cmbRunDisplay->currentIndex()==4) {
           //////////////////////////////////////////////////////////////////////////////////
           // Plot only highlighted run
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                    size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i).toStdString());
                    size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i).toStdString());
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_title(tr("run %1").arg(i));
                    g->set_color(QColor("red"));
                    g->set_datarange_start(sliders->get_userMin());
                    g->set_datarange_end(sliders->get_userMax());

                    g->set_yErrorColumn(c_rune);
                    g->set_yErrorStyle(runstyle);
                    g->set_xErrorStyle(JKQTPnoError);
                    g->set_errorColor(g->get_errorColor().lighter());
                    g->set_errorWidth(1);

                    plotter->addGraph(g);
                }
            }
        }
    //std::cout<<"repainting ... 4\n";

        plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(), false);
    //std::cout<<"repainting ... 5\n";
        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    //std::cout<<"repainting ... 6\n";
        plotter->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [{\\mu}s]"));
        plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    //std::cout<<"repainting ... 7\n";
        plotter->zoomToFit(true, true, false,false);
    //std::cout<<"repainting ... 8\n";
    }
    plotter->set_doDrawing(true);
    //std::cout<<"repainting ... 9\n";
    plotter->set_emitSignals(true);
    //std::cout<<"repainting ... 10\n";
    plotter->update_plot();
    //std::cout<<" done!\n";
};


void QFRDRFCSCorrelationEditor::readSettings() {
    if (!settings) return;
    //std::cout<<"--QFRDRFCSCorrelationEditor::readSettings()\n";
    plotter->loadSettings(*(settings->getQSettings()), QString("fcsdataeditor/corrplot"));
    //splitter->restoreState(settings->getQSettings()->value(QString("fcsdataeditor/corrsplitterSizes")).toByteArray());
    chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("fcsdataeditor/log_tau_axis"), true).toBool());
    cmbAverageErrors->setCurrentIndex(settings->getQSettings()->value(QString("fcsdataeditor/error_display"), 2).toInt());
    cmbRunErrors->setCurrentIndex(settings->getQSettings()->value(QString("fcsdataeditor/run_error_display"), 0).toInt());
    cmbRunDisplay->setCurrentIndex(settings->getQSettings()->value(QString("fcsdataeditor/run_display"), 0).toInt());
    loadSplitter(*(settings->getQSettings()), splitter, "fcsdataeditor/corrsplitterSizes");
};


void QFRDRFCSCorrelationEditor::writeSettings() {
    if (!settings) return;
    //std::cout<<"--QFRDRFCSCorrelationEditor::writeSettings()\n";
    //plotter->saveSettings(*(settings->getQSettings()), QString("fcsdataeditor/corrplot"));
    //settings->getQSettings()->setValue(QString("fcsdataeditor/corrsplitterSizes"), splitter->saveState());
    settings->getQSettings()->setValue(QString("fcsdataeditor/log_tau_axis"), chkLogTauAxis->isChecked());
    settings->getQSettings()->setValue(QString("fcsdataeditor/error_display"), cmbAverageErrors->currentIndex());
    settings->getQSettings()->setValue(QString("fcsdataeditor/run_error_display"), cmbRunErrors->currentIndex());
    settings->getQSettings()->setValue(QString("fcsdataeditor/run_display"), cmbRunDisplay->currentIndex());
    saveSplitter(*(settings->getQSettings()), splitter, "fcsdataeditor/corrsplitterSizes");
};



