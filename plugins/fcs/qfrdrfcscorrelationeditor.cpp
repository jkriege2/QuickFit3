/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfrdrfcscorrelationeditor.h"
#include "qfrdrfcsdata.h"
#include "qfrawdatapropertyeditor.h"


QFRDRFCSCorrelationEditorRunsModel::QFRDRFCSCorrelationEditorRunsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    current=NULL;
}

void QFRDRFCSCorrelationEditorRunsModel::setCurrent(QFRawDataRecord* current) {
    this->current=current;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
};


QVariant QFRDRFCSCorrelationEditorRunsModel::data(const QModelIndex &index, int role) const {
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


Qt::ItemFlags QFRDRFCSCorrelationEditorRunsModel::flags(const QModelIndex &/*index*/) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRFCSCorrelationEditorRunsModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const {
    return QVariant();
}

int QFRDRFCSCorrelationEditorRunsModel::rowCount(const QModelIndex &/*parent*/) const {
    if (!current) return 0;
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (m) return 1+m->getCorrelationRuns();
    return 1;
}

int QFRDRFCSCorrelationEditorRunsModel::columnCount(const QModelIndex &/*parent*/) const {
    return 1;
}





QFRDRFCSCorrelationEditor::QFRDRFCSCorrelationEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRFCSCorrelationEditor::~QFRDRFCSCorrelationEditor()
{
    //dtor
}

void QFRDRFCSCorrelationEditor::createWidgets() {
    correlationMaskTools=new QFCorrelationMaskTools(this);
    connect(correlationMaskTools, SIGNAL(maskChanged()), this, SLOT(rawDataChangedRecalc()));

    QVBoxLayout* l=new QVBoxLayout();
    setLayout(l);
    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    l->addWidget(splitter);

    QWidget* w=new QWidget(this);
    QGridLayout* gl=new QGridLayout();
    w->setLayout(gl);
    cmbAverageErrors=new QComboBox(w);
    int row=0;
    gl->addWidget(new QLabel(tr("display average:")), row, 0);
    gl->addWidget(cmbAverageErrors, row, 1);
    row++;
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no average"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no errors"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_elines.png"), tr("with error lines"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_ebars.png"), tr("with error bars"));
    cmbAverageErrors->addItem(QIcon(":/fcs/fcsplot_elinesbars.png"), tr("with lines and bars"));
    cmbAverageErrors->addItem(QIcon(":/libqf3widgets/plot_epoly.png"), tr("with error polygons"));
    cmbAverageErrors->addItem(QIcon(":/libqf3widgets/plot_epolybars.png"), tr("with polygons and bars"));
    connect(cmbAverageErrors, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData(int)));

    cmbRunDisplay=new QComboBox(w);
    gl->addWidget(new QLabel(tr("display runs:")), row, 0);
    gl->addWidget(cmbRunDisplay, row, 1);
    cmbRunDisplay->addItem(tr("no runs"));
    cmbRunDisplay->addItem(tr("all runs"));
    cmbRunDisplay->addItem(tr("all runs (highlighted)"));
    cmbRunDisplay->addItem(tr("used runs"));
    cmbRunDisplay->addItem(tr("selected run"));
    connect(cmbRunDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(runsModeChanged(int)));
    row++;

    cmbRunErrors=new QComboBox(w);
    gl->addWidget(new QLabel(tr("run errors:")), row, 0);
    gl->addWidget(cmbRunErrors, row, 1);
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_enone.png"), tr("no errors"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_elines.png"), tr("with error lines"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_ebars.png"), tr("with error bars"));
    cmbRunErrors->addItem(QIcon(":/fcs/fcsplot_elinesbars.png"), tr("with lines and bars"));
    cmbRunErrors->addItem(QIcon(":/libqf3widgets/plot_epoly.png"), tr("with error polygons"));
    cmbRunErrors->addItem(QIcon(":/libqf3widgets/plot_epolybars.png"), tr("with polygons and bars"));
    connect(cmbRunErrors, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData(int)));
    row++;

    chkKeepZoom=new QCheckBox(tr("retain when changing runs/files"), this);
    gl->addWidget(new QLabel(tr("zoom-level:")), row, 0);
    gl->addWidget(chkKeepZoom, row, 1);
    lstRunsSelect=new QListView(w);
    lstRunsSelect->setModel(&runs);
    lstRunsSelect->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(lstRunsSelect->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(lstRunsSelect->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &, const QModelIndex &)));
    row++;

    gl->addWidget(new QLabel(tr("select runs to display:")), row, 0, 1, 2);
    row++;
    gl->addWidget(lstRunsSelect, row, 0, 3, 2);
    gl->setRowStretch(row, 2);
    row+=3;
    btnDontUse=new QPushButton(tr("exclude selected"), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    gl->addWidget(btnDontUse, row, 0, 1, 2);
    row++;
    btnUse=new QPushButton(tr("include selected"), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    gl->addWidget(btnUse, row, 0, 1, 2);
    row++;

    chkLogTauAxis=new QCheckBox("", w);
    gl->addWidget(new QLabel(tr("log-scale on<br>lag time axis:")), row, 0);
    gl->addWidget(chkLogTauAxis, row, 1);
    connect(chkLogTauAxis, SIGNAL(toggled(bool)), this, SLOT(replotData()));
    row++;

    grpInfo=new QGroupBox(tr("Info"), w);
    QGridLayout* ggl=new QGridLayout();
    grpInfo->setLayout(ggl);
    ggl->addWidget(new QLabel(tr("# Runs = ")), 0, 0);
    labRuns=new QLabel(grpInfo);
    ggl->addWidget(labRuns, 0,1);
    ggl->addWidget(new QLabel(tr("# Points = ")), 1, 0);
    labCorrelationPoints=new QLabel(grpInfo);
    ggl->addWidget(labCorrelationPoints, 1,1);
    gl->addWidget(grpInfo, row,0,1,2);
    row++;

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

    actCopyNormalizedACF=new QFActionWithNoMenuRole(tr("copy normalized CFs to table"), this);
    connect(actCopyNormalizedACF, SIGNAL(triggered()), this, SLOT(copyNormalizedACFs()));
    actCorrectOffset=new QFActionWithNoMenuRole(tr("correct CFs for offset"), this);
    connect(actCorrectOffset, SIGNAL(triggered()), this, SLOT(correctOffset()));

    menuMask=propertyEditor->addMenu("&Selection/Mask", 0);
    correlationMaskTools->registerMaskToolsToMenu(menuMask);
    menuMask->addSeparator();
    correlationMaskTools->registerCorrelationToolsToMenu(menuMask);

    menuData=propertyEditor->addOrFindMenu(tr("&Data"), -1);
    //menuData->addAction(actCopyNormalizedACF);
    menuData->addAction(actCorrectOffset);


};

void QFRDRFCSCorrelationEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
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

void QFRDRFCSCorrelationEditor::selectionChanged(const QModelIndex & /*current*/, const QModelIndex & /*previous*/ ) {
    replotData();
}

void QFRDRFCSCorrelationEditor::selectionChanged(const QItemSelection & /*current*/, const QItemSelection & /*previous*/ ) {
    replotData();
}

void QFRDRFCSCorrelationEditor::runsModeChanged(int /*c*/) {
    if (cmbRunDisplay->currentIndex()<=1) lstRunsSelect->setEnabled(false);
    else lstRunsSelect->setEnabled(true);
    replotData();
}

void QFRDRFCSCorrelationEditor::rawDataChanged() {
    replotData();
}

void QFRDRFCSCorrelationEditor::rawDataChangedRecalc()
{
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
     //qDebug()<<"rawDataChangedRecalc()  m="<<m;
     if (m) m->recalculateCorrelations();
     replotData();
     runs.setCurrent(current);
};

void QFRDRFCSCorrelationEditor::slidersChanged(int userMin, int userMax, int /*min*/, int /*max*/) {
    if (!current) return;
    current->setQFProperty("fcscorreditor_datacut_min", userMin, false, false);
    current->setQFProperty("fcscorreditor_datacut_max", userMax, false, false);
    replotData();
}

void QFRDRFCSCorrelationEditor::correctBackground()
{
    QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
    if (!m) return;
    QFSelectRDRDialog* dlg=new QFSelectRDRDialog(new QFMatchRDRFunctorSelectType(current->getType()), this);
    QList<QFRawDataRecord*> grpmem=current->getGroupMembers();
    dlg->selectNone();

    if (grpmem.size()<=0) {
        dlg->selectAll();
    } else {
        for (int i=0; i<grpmem.size(); i++) {
            dlg->selectRDR(grpmem[i]);
        }
    }

    double B0=m->getProperty("BACKGROUND_INTENSITY1", m->getProperty("BACKGROUND1",
                                                       m->getProperty("BACKGROUND_INTENSITY",
                                                       m->getProperty("BACKGROUND",
                                                                      m->getProperty("BACKGROUND_KHZ1",
                                                                      m->getProperty("BACKGROUND_KHZ", 0).toDouble()).toDouble()*1000.0).toDouble()).toDouble()).toDouble()).toDouble();
    double B1=m->getProperty("BACKGROUND_INTENSITY2", m->getProperty("BACKGROUND2", m->getProperty("BACKGROUND_INTENSITY", m->getProperty("BACKGROUND",
                                                                                                                                   m->getProperty("BACKGROUND_KHZ2", m->getProperty("BACKGROUND_KHZ", 0).toDouble()).toDouble()*1000.0
                                                                                                                                   ).toDouble()).toDouble()).toDouble()).toDouble();


    QFDoubleEdit* edtB1=new QFDoubleEdit(dlg);
    edtB1->setValue(B0);
    dlg->addWidget("Background, channel 0 [kHz]", edtB1);
    QFDoubleEdit* edtB2=new QFDoubleEdit(dlg);
    edtB2->setValue(B1);
    dlg->addWidget("Background, channel 1 [kHz]", edtB2);
    QFDoubleEdit* edtOffset=new QFDoubleEdit(dlg);
    edtOffset->setValue(0);
    dlg->addWidget("correlation function offset", edtOffset);

    if (dlg->exec()) {
        B0=edtB1->value();
        B1=edtB2->value();
        double offset=edtOffset->value();
        QList<QFRawDataRecord*> lst=dlg->getSelectedRDRsp();
        for (int i=0; i<lst.size(); i++) {
            QFRDRFCSData* mi=qobject_cast<QFRDRFCSData*>(lst[i]);
            if (mi) {
//                double I0=mi->getRateMean(r, 0);
//                double I1=mi->getRateMean(r, 1);
            }
        }
    }
    delete dlg;
}

void QFRDRFCSCorrelationEditor::correctOffset()
{
    if (!current) return;
    bool ok=false;
    double offset=QInputDialog::getDouble(this, tr("FCS Offset correction"), tr("enter the offset, which should be subtracted"), current->getProperty("CORR_OFFSET", 0.0).toDouble(),-2147483647,2147483647,5, &ok);
    if (ok)  {
        double oldoff=current->getProperty("CF_CORRECT_OFFSET", 0.0).toDouble();
        current->setQFProperty("CF_CORRECT_OFFSET", offset, true, true);
        /*if (QMessageBox::information(this, tr("corect correlation offset"), tr("The project needs to be saved and reloaded for the offset correction to take effect!\nReload project now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
            QFPluginServices::getInstance()->reloadCurrentProject();
        }*/
        QFRDRFCSData* m=qobject_cast<QFRDRFCSData*>(current);
        if (m) {
            for (int r=-1; r<m->getCorrelationRuns(); r++) {
                double* d=m->getCorrelationRun(r);
                int64_t N=m->getCorrelationN();
                for (int64_t i=0; i<N; i++) {
                    d[i]=d[i]+oldoff-offset;
                }
            }
        } else {
            if (QMessageBox::information(this, tr("corect correlation offset"), tr("The project needs to be saved and reloaded for the offset correction to take effect!\nReload project now?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) {
                QFPluginServices::getInstance()->reloadCurrentProject();
            }
        }

    }
}



void QFRDRFCSCorrelationEditor::replotData(int /*dummy*/) {
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
    if (m->getCorrelationN()>0) {
        size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");
    //std::cout<<"repainting ... 2\n";

        QList<JKQTPgraph*> topGraphs;

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
                case 5: styl=JKQTPerrorPolygons; break;
                case 6: styl=JKQTPerrorBarsPolygons; break;
            }


            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
            g->set_color(QColor("darkblue"));
            QColor errc=g->get_color().lighter();
            g->set_errorColor(errc);
            errc.setAlphaF(0.5);
            g->set_errorFillColor(errc);
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
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i));
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i));
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                g->set_lineWidth(1);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_title(tr("run %1").arg(i));
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

                plotter->addGraph(g);
            }
        } else if (cmbRunDisplay->currentIndex()==2) {
           //////////////////////////////////////////////////////////////////////////////////
           // Plot ALL RUNS (left out runs in gray, standard runs in black, highlighted run in red)
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i));
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i));
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
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

                bool isTop=false;

                if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                    g->set_color(QColor("red"));
                    isTop=true;
                    topGraphs.append(g);;
                } else {
                    if (!m->leaveoutRun(i)) {
                        g->set_color(QColor("black"));
                    } else {
                        g->set_color(QColor("grey"));
                    }
                }
                QColor errc=g->get_color().lighter();
                g->set_errorColor(errc);
                errc.setAlphaF(0.5);
                g->set_errorFillColor(errc);
                if (!isTop) plotter->addGraph(g);
            }
        } else if (cmbRunDisplay->currentIndex()==3) {
           //////////////////////////////////////////////////////////////////////////////////
           // Plot only used runs
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                if (!m->leaveoutRun(i)) {
                    size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i));
                    size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i));
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_title(tr("run %1").arg(i));
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

                    plotter->addGraph(g);
                }
            }
        } else if (cmbRunDisplay->currentIndex()==4) {
           //////////////////////////////////////////////////////////////////////////////////
           // Plot only highlighted run
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                    size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1").arg(i));
                    size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1").arg(i));
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(plotter->get_plotter());
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_title(tr("run %1").arg(i));
                    //g->set_color(QColor("red"));
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

                    plotter->addGraph(g);
                }
            }
        }

       plotter->addGraphs(topGraphs);
    //std::cout<<"repainting ... 4\n";

        //plotter->zoomToFit(true, true, !chkLogTauAxis->isChecked(), false);
    //std::cout<<"repainting ... 5\n";
        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
    //std::cout<<"repainting ... 6\n";
        plotter->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
        plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    //std::cout<<"repainting ... 7\n";
        if (!chkKeepZoom->isChecked()) plotter->zoomToFit(true, true, false,false);
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
    chkKeepZoom->setChecked(settings->getQSettings()->value(QString("fcsdataeditor/chkKeepZoom"), false).toBool());
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
    settings->getQSettings()->setValue(QString("fcsdataeditor/chkKeepZoom"), chkKeepZoom->isChecked());
    settings->getQSettings()->setValue(QString("fcsdataeditor/error_display"), cmbAverageErrors->currentIndex());
    settings->getQSettings()->setValue(QString("fcsdataeditor/run_error_display"), cmbRunErrors->currentIndex());
    settings->getQSettings()->setValue(QString("fcsdataeditor/run_display"), cmbRunDisplay->currentIndex());
    saveSplitter(*(settings->getQSettings()), splitter, "fcsdataeditor/corrsplitterSizes");
};



