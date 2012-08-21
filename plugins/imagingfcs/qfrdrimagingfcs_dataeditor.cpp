#include "qfrdrimagingfcs_dataeditor.h"
#include "qfrdrimagingfcs_data.h"
#include <QDebug>


void QFRDRImagingFCSDataEditor::excludeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
        for (int i=0; i<l.size(); i++) {
            //std::cout<<"adding run "<<l[i].row()-1<<std::endl;
            m->leaveoutAddRun(l[i].row()-1);
        }
        m->recalcCorrelations();
    }
    replotData();
}

void QFRDRImagingFCSDataEditor::includeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    QModelIndexList l=lstRunsSelect->selectionModel()->selectedRows();
    if (m) {
        for (int i=0; i<l.size(); i++) {
            //std::cout<<"removing run "<<l[i].row()-1<<std::endl;
            m->leaveoutRemoveRun(l[i].row()-1);
        }
        m->recalcCorrelations();
    }
    replotData();

}





QFRDRImagingFCSDataEditor::QFRDRImagingFCSDataEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRImagingFCSDataEditor::~QFRDRImagingFCSDataEditor()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    plteOverviewSelectedData=NULL;
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
    plteOverviewExcludedData=NULL;
}

void QFRDRImagingFCSDataEditor::createWidgets() {
    int row=0;
    QLabel* l;
    QVBoxLayout* lb=new QVBoxLayout(this);
    setLayout(lb);
    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    lb->addWidget(splitter);

    QWidget* w=new QWidget(this);
    QGridLayout* gl=new QGridLayout(this);
    w->setLayout(gl);
    chkDisplayAverage=new QCheckBox(w);
    chkDisplayAverage->setChecked(true);
    gl->addWidget((l=new QLabel(tr("display &average:"), w)), row, 0);
    l->setBuddy(chkDisplayAverage);
    gl->addWidget(chkDisplayAverage, row, 1);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));
    row++;

    cmbAverageStyle=new QComboBox(w);
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));
    cmbAverageStyle->addItem(QIcon(":/libqf3widgets/plot_epoly.png"), tr("with error polygons"));
    cmbAverageStyle->addItem(QIcon(":/libqf3widgets/plot_epolybars.png"), tr("with polygons and bars"));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageStyle, SLOT(setEnabled(bool)));

    cmbAverageErrorStyle=new QComboBox(w);
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));
    cmbAverageErrorStyle->addItem(QIcon(":/libqf3widgets/plot_epoly.png"), tr("with error polygons"));
    cmbAverageErrorStyle->addItem(QIcon(":/libqf3widgets/plot_epolybars.png"), tr("with polygons and bars"));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageErrorStyle, SLOT(setEnabled(bool)));

    gl->addWidget((l=new QLabel(tr("average &options:"), w)), row, 0);
    l->setBuddy(cmbAverageStyle);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
    gl->addWidget(cmbAverageStyle, row, 1);
    connect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    gl->addWidget(cmbAverageErrorStyle, row, 2);
    connect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    row++;


    cmbRunDisplay=new QComboBox(w);
    gl->addWidget(new QLabel(tr("display pixels:")), row, 0);
    gl->addWidget(cmbRunDisplay, row, 1);
    cmbRunDisplay->addItem(tr("no pixels"));
    cmbRunDisplay->addItem(tr("all pixels"));
    cmbRunDisplay->addItem(tr("all pixels (highlighted)"));
    cmbRunDisplay->addItem(tr("selected pixel"));
    cmbRunDisplay->setCurrentIndex(1);
    connect(cmbRunDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(runsModeChanged(int)));
    row++;

    cmbRunStyle=new QComboBox(w);
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbRunErrorStyle=new QComboBox(w);
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));

    gl->addWidget((labRunOptions=new QLabel(tr("pixel &options:"), w)), row, 0);
    labRunOptions->setBuddy(cmbRunStyle);
    gl->addWidget(cmbRunStyle, row, 1);
    connect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    gl->addWidget(cmbRunErrorStyle, row, 2);
    connect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    row++;



    lstRunsSelect=new QListView(w);
    lstRunsSelect->setModel(&runs);
    lstRunsSelect->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(lstRunsSelect->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    //connect(lstRunsSelect->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &, const QModelIndex &)));


    gl->addWidget(new QLabel(tr("select pixels to display:")), row, 0, 1, 3);
    row++;
    gl->addWidget(lstRunsSelect, row, 0, 3, 3);
    gl->setRowStretch(row, 2);
    row+=3;



    pltOverview=new JKQTFastPlotter(w);
    pltOverview->set_aspectRatio(1);
    pltOverview->set_maintainAspectRatio(true);
    pltOverview->set_xAxisLabelVisible(false);
    pltOverview->set_yAxisLabelVisible(false);
    pltOverview->set_tickFontSize(6);
    pltOverview->set_tickFontName(font().family());
    QFont fo=font();
    fo.setPointSizeF(pltOverview->get_tickFontSize());
    QFontMetricsF fm(fo);
    pltOverview->set_plotBorderBottom(2.0*fm.ascent());
    pltOverview->set_plotBorderLeft(fm.width("00000"));
    pltOverview->set_drawGrid(false);
    connect(pltOverview, SIGNAL(clicked(double,double,Qt::KeyboardModifiers)), this,SLOT(previewClicked(double,double,Qt::KeyboardModifiers)));
    gl->addWidget(new QLabel(tr("overview:")), row, 0, 1, 3);
    row++;
    gl->addWidget(pltOverview, row, 0, 2, 3);
    gl->setRowStretch(row, 2);
    row+=2;

    plteOverview=new JKQTFPimagePlot(pltOverview, NULL, JKQTFP_uint16, 1, 1, 0, 1, 0, 1, JKQTFP_GRAY);
    pltOverview->addPlot(plteOverview);

    QColor col("red");
    col.setAlphaF(0.5);
    plteOverviewSelected=new JKQTFPimageOverlayPlot(pltOverview, NULL, 1, 1, 0, 1, 0, 1, col);
    pltOverview->addPlot(plteOverviewSelected);

    col=QColor("blue");
    col.setAlphaF(0.5);
    plteOverviewExcluded=new JKQTFPimageOverlayPlot(pltOverview, NULL, 1, 1, 0, 1, 0, 1, col);
    pltOverview->addPlot(plteOverviewExcluded);


    plteOverviewSelectedData=NULL;
    plteOverviewExcludedData=NULL;
    plteOverviewSelectedSize=0;


    btnDontUse=new QPushButton(tr("exclude sel."), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    gl->addWidget(btnDontUse, row, 1);
    btnUse=new QPushButton(tr("include sel."), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    gl->addWidget(btnUse, row, 2);
    row++;

    chkLogTauAxis=new QCheckBox("", w);
    gl->addWidget((l=new QLabel(tr("log &tau;-axis:"))), row, 0);
    l->setTextFormat(Qt::RichText);
    gl->addWidget(chkLogTauAxis, row, 1);
    connect(chkLogTauAxis, SIGNAL(toggled(bool)), this, SLOT(replotData()));
    row++;

    grpInfo=new QGroupBox(tr("Info"), w);
    QGridLayout* ggl=new QGridLayout(grpInfo);
    grpInfo->setLayout(ggl);
    ggl->addWidget(new QLabel(tr("# Pixels = ")), 0, 0);
    labRuns=new QLabel(grpInfo);
    ggl->addWidget(labRuns, 0,1);
    ggl->addWidget(new QLabel(tr("# Points = ")), 1, 0);
    labCorrelationPoints=new QLabel(grpInfo);
    ggl->addWidget(labCorrelationPoints, 1,1);
    gl->addWidget(grpInfo, row,0,1,3);

    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout(this);
    wp->setLayout(lp);
    plotter = new JKQtPlotter(true, this);
    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotter->get_plotter()->set_showKey(false);
    plotter->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->get_plotter()->set_useAntiAliasingForGraphs(false);
    plotter->get_plotter()->set_useAntiAliasingForSystem(true);

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

void QFRDRImagingFCSDataEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        disconnect(old, 0, this, 0);
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        runs.setCurrent(current);
        sliders->disableSliderSignals();
        sliders->set_min(0);
        sliders->set_max(m->getCorrelationN());
        sliders->set_userMin(current->getProperty("imfcs_correditor_datacut_min", 0).toInt());
        sliders->set_userMax(current->getProperty("imfcs_correditor_datacut_max", m->getCorrelationN()).toInt());
        sliders->enableSliderSignals();
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    } else {
//        runs.setCurrent(current);
    }

    //readSettings();

};

void QFRDRImagingFCSDataEditor::selectionChanged(const QModelIndex & current, const QModelIndex & previous ) {
    if (cmbRunDisplay->currentIndex()>1) replotData();
}

void QFRDRImagingFCSDataEditor::selectionChanged(const QItemSelection & current, const QItemSelection & previous ) {
    replotData();
}

void QFRDRImagingFCSDataEditor::previewClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;
    if (cmbRunDisplay->currentIndex()<=1) return;

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int idx=m->xyToRun(xx, yy);

    if (modifiers==Qt::ControlModifier) {
        lstRunsSelect->selectionModel()->setCurrentIndex(runs.index(idx+1, 0), QItemSelectionModel::Toggle);
    } else {
        lstRunsSelect->selectionModel()->setCurrentIndex(runs.index(idx+1, 0), QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Current);
    }
    replotOverview();
}

void QFRDRImagingFCSDataEditor::runsModeChanged(int c) {
    labRunOptions->setEnabled(cmbRunDisplay->currentIndex()>0);
    cmbRunStyle->setEnabled(cmbRunDisplay->currentIndex()>0);
    cmbRunErrorStyle->setEnabled(cmbRunDisplay->currentIndex()>0);
    if (cmbRunDisplay->currentIndex()<=1) lstRunsSelect->setEnabled(false);
    else lstRunsSelect->setEnabled(true);
    replotData();
}

void QFRDRImagingFCSDataEditor::rawDataChanged() {
    replotData();
}

void QFRDRImagingFCSDataEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    current->setQFProperty("imfcs_correditor_datacut_min", userMin, false, false);
    current->setQFProperty("imfcs_correditor_datacut_max", userMax, false, false);
    //replotData();

    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        ds->clear();
        return;
    }

    qDebug()<<"sliderChanged()";
    bool dodraw=plotter->get_doDrawing();
    bool doemit=plotter->get_emitSignals();
    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    plotter->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());

    if (doemit) {
        plotter->set_doDrawing(true);
        plotter->update_plot();
    }
    if (dodraw) plotter->set_emitSignals(true);

    QApplication::restoreOverrideCursor();
    qDebug()<<"sliderChanged() ... done";

}

void QFRDRImagingFCSDataEditor::replotOverview() {
     //qDebug()<<"replotOverview";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    bool doDraw=pltOverview->get_doDrawing();
    pltOverview->set_doDrawing(false);

    if (!m) {
        plteOverview->set_image(NULL, JKQTFP_uint16, 1, 1);
        plteOverviewSelected->set_data(NULL, 0, 0);
        plteOverviewExcluded->set_data(NULL, 0, 0);
    } else {
        double* ov=m->getImageFromRunsPreview();
        double w=m->getImageFromRunsWidth();
        double h=m->getImageFromRunsHeight();
        double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));

        //qDebug()<<"replotOverview:  "<<w<<" x "<<h;

        pltOverview->setXRange(0,w);
        pltOverview->setYRange(0,h);
        pltOverview->set_xTickDistance(dx);
        pltOverview->set_yTickDistance(dy);
        pltOverview->set_aspectRatio(w/h);
        plteOverviewSelected->set_data(NULL, 0, 0);
        plteOverviewExcluded->set_data(NULL, 0, 0);

        if (plteOverviewSelectedSize!=w*h) {
            plteOverviewSelectedSize=w*h;
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSelectedSize*sizeof(bool));
            plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plteOverviewSelectedSize*sizeof(bool));
        }

        plteOverviewSelected->set_data(plteOverviewSelectedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        plteOverviewSelected->set_xmax(w);
        plteOverviewSelected->set_ymax(h);
        plteOverviewExcluded->set_data(plteOverviewExcludedData, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        plteOverviewExcluded->set_xmax(w);
        plteOverviewExcluded->set_ymax(h);
        plteOverview->set_image(ov, JKQTFP_double, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
        plteOverview->set_xmax(w);
        plteOverview->set_ymax(h);
        if (plteOverviewSelectedData && plteOverviewExcludedData) {
            if (cmbRunDisplay->currentIndex()<=1) {
                for (int i=0; i<m->getCorrelationRuns(); i++) {
                    int x=m->runToX(i);
                    int y=m->runToY(i);
                    int idx=y*m->getImageFromRunsWidth()+x;
                    plteOverviewSelectedData[idx]=false;
                    plteOverviewExcludedData[idx]=m->leaveoutRun(i);
                }
            } else {
                for (int i=0; i<m->getCorrelationRuns(); i++) {
                    int x=m->runToX(i);
                    int y=m->runToY(i);
                    int idx=y*m->getImageFromRunsWidth()+x;
                    plteOverviewSelectedData[idx]=lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0));
                    plteOverviewExcludedData[idx]=m->leaveoutRun(i);
                }
            }
        }
    }

    if (doDraw) {
        pltOverview->set_doDrawing(true);
        pltOverview->update_data();
    }
    QApplication::restoreOverrideCursor();
    //qDebug()<<"replotOverview ... DONE";
}

void QFRDRImagingFCSDataEditor::replotData(int dummy) {
    //qDebug()<<"replot";
    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        ds->clear();
        //qDebug()<<"replot ... DONE";
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool doDraw=plotter->get_doDrawing();
    bool doEmit=plotter->get_emitSignals();
    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    plotter->clearGraphs();
    ds->clear();

    labRuns->setText(QString::number(m->getCorrelationRuns()));
    labCorrelationPoints->setText(QString::number(m->getCorrelationN()));

    if (m->getCorrelationN()>0) {


        JKQTPerrorPlotstyle runerrorstyle=JKQTPnoError;
        switch (cmbRunErrorStyle->currentIndex()) {
            case 1: runerrorstyle=JKQTPerrorBars; break;
            case 2: runerrorstyle=JKQTPerrorLines; break;
            case 3: runerrorstyle=JKQTPerrorBarsLines; break;
            case 4: runerrorstyle=JKQTPerrorPolygons; break;
            case 5: runerrorstyle=JKQTPerrorBarsPolygons; break;
        }
        bool runLine=(cmbRunStyle->currentIndex()!=1);
        JKQTPgraphSymbols runSymbol=JKQTPcross;
        if (cmbRunStyle->currentIndex()==0) runSymbol=JKQTPnoSymbol;

        JKQTPerrorPlotstyle avgerrorstyle=JKQTPnoError;
        switch (cmbAverageErrorStyle->currentIndex()) {
            case 1: avgerrorstyle=JKQTPerrorBars; break;
            case 2: avgerrorstyle=JKQTPerrorLines; break;
            case 3: avgerrorstyle=JKQTPerrorBarsLines; break;
            case 4: avgerrorstyle=JKQTPerrorPolygons; break;
            case 5: avgerrorstyle=JKQTPerrorBarsPolygons; break;
        }
        bool avgLine=(cmbAverageStyle->currentIndex()!=1);
        JKQTPgraphSymbols avgSymbol=JKQTPcross;
        if (cmbAverageStyle->currentIndex()==0) avgSymbol=JKQTPnoSymbol;



        size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");


        //QTime t;
        //t.start();

        QList<JKQTPgraph*> topGraphs;


       //////////////////////////////////////////////////////////////////////////////////
       // Plot ALL RUNS
       //////////////////////////////////////////////////////////////////////////////////
       if (cmbRunDisplay->currentIndex()==1) {
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("pixel %1 %2").arg(m->getCorrelationRunName(i)).arg(i));
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("pixel error %1 %2").arg(m->getCorrelationRunName(i)).arg(i));
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                g->set_lineWidth(1);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_datarange_start(sliders->get_userMin());
                g->set_datarange_end(sliders->get_userMax());
                g->set_yErrorColumn(c_rune);
                g->set_yErrorStyle(runerrorstyle);
                g->set_xErrorStyle(JKQTPnoError);
                QColor errc=g->get_color().lighter();
                g->set_errorColor(errc);
                errc.setAlphaF(0.5);
                g->set_errorFillColor(errc);
                g->set_errorWidth(1);
                g->set_drawLine(runLine);
                g->set_symbol(runSymbol);
                g->set_symbolSize(5);
                g->set_title(tr("run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));

                plotter->addGraph(g);
            }
        } else if (cmbRunDisplay->currentIndex()==2) {
           //////////////////////////////////////////////////////////////////////////////////
           // Plot ALL RUNS (left out runs in gray, standard runs in black, highlighted run in red)
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("pixel error %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                g->set_lineWidth(1);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_run);
                g->set_drawLine(runLine);
                g->set_symbol(runSymbol);
                g->set_title(tr("run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                g->set_datarange_start(sliders->get_userMin());
                g->set_datarange_end(sliders->get_userMax());
                g->set_yErrorColumn(c_rune);
                g->set_yErrorStyle(runerrorstyle);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_symbolSize(5);
                g->set_errorWidth(1);

                bool isTop=false;

                if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                        g->set_color(QColor("red"));
                        topGraphs.append(g);
                        isTop=true;
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
           // Plot only highlighted run
           //////////////////////////////////////////////////////////////////////////////////
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                    size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("pixel error %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_drawLine(runLine);
                    g->set_symbol(runSymbol);
                    g->set_title(tr("run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                    g->set_color(QColor("red"));
                    g->set_datarange_start(sliders->get_userMin());
                    g->set_datarange_end(sliders->get_userMax());

                    g->set_yErrorColumn(c_rune);
                    g->set_yErrorStyle(runerrorstyle);
                    g->set_xErrorStyle(JKQTPnoError);
                    QColor errc=g->get_color().lighter();
                    g->set_errorColor(errc);
                    errc.setAlphaF(0.5);
                    g->set_errorFillColor(errc);
                    g->set_symbolSize(5);
                    g->set_errorWidth(1);

                    plotter->addGraph(g);
                }
            }
        }

       //qDebug()<<"plots: "<<t.elapsed()<<" ms";


        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        if (chkDisplayAverage->isChecked()) {
            size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), "cmean");
            size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), "cstddev");

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
            g->set_color(QColor("blue"));
            QColor errc=g->get_color().lighter();
            g->set_errorColor(errc);
            errc.setAlphaF(0.5);
            g->set_errorFillColor(errc);
            g->set_lineWidth(2);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(tr("average"));
            g->set_yErrorStyle(avgerrorstyle);
            g->set_xErrorStyle(JKQTPnoError);
            g->set_datarange_start(sliders->get_userMin());
            g->set_datarange_end(sliders->get_userMax());
            g->set_drawLine(avgLine);
            g->set_symbolSize(5);
            g->set_symbol(avgSymbol);
            plotter->addGraph(g);
        }

        plotter->addGraphs(topGraphs);

        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotter->zoomToFit(true, true, false,false);

    }
    if (doDraw) {
        plotter->set_doDrawing(true);
        plotter->update_plot();
    }
    if (doEmit) {
        plotter->set_emitSignals(true);
    }
    //QTime t;
    //t.start();
    //qDebug()<<"plotting in "<<t.elapsed()<<" ms";
    //qDebug()<<"replot ... DONE!";
    replotOverview();
    QApplication::restoreOverrideCursor();
}


void QFRDRImagingFCSDataEditor::readSettings() {
    if (!settings) return;
    plotter->loadSettings(*(settings->getQSettings()), QString("imfcsdataeditor/corrplot"));
    chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("imfcsdataeditor/log_tau_axis"), true).toBool());
    //chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("imfcsdataeditor/display_avg"), true).toBool());
    chkDisplayAverage->setChecked(true);
    cmbAverageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsdataeditor/avg_style"), 0).toInt());
    cmbAverageErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsdataeditor/avg_error_style"), 2).toInt());
    //cmbRunDisplay->setCurrentIndex(settings->getQSettings()->value(QString("imfcsdataeditor/run_display"), 3).toInt());
    cmbRunDisplay->setCurrentIndex(3);
    cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsdataeditor/run_style"), 0).toInt());
    cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsdataeditor/run_error_style"), 0).toInt());
    loadSplitter(*(settings->getQSettings()), splitter, "imfcsdataeditor/corrsplitterSizes");
};


void QFRDRImagingFCSDataEditor::writeSettings() {
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfcsdataeditor/log_tau_axis"), chkLogTauAxis->isChecked());
    settings->getQSettings()->setValue(QString("imfcsdataeditor/display_avg"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("imfcsdataeditor/run_display"), cmbRunDisplay->currentIndex());

    settings->getQSettings()->setValue(QString("imfcsdataeditor/avg_style"), cmbAverageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsdataeditor/avg_error_style"), cmbAverageErrorStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsdataeditor/run_display"), cmbRunDisplay->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsdataeditor/run_style"), cmbRunStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsdataeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    saveSplitter(*(settings->getQSettings()), splitter, "imfcsdataeditor/corrsplitterSizes");
};





