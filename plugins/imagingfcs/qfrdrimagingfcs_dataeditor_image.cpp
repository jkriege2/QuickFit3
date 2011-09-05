#include "qfrdrimagingfcs_dataeditor.h"
#include "qfrdrimagingfcs_data.h"
#include <QDebug>








QFRDRImagingFCSImageEditor::QFRDRImagingFCSImageEditor(QFPluginServices* services, QWidget* parent):
    QFRawDataEditor(services, parent)
{
    createWidgets();
}

QFRDRImagingFCSImageEditor::~QFRDRImagingFCSImageEditor()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    plteOverviewSelectedData=NULL;
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
    plteOverviewExcludedData=NULL;
    if (plteImageData) free(plteImageData);
    plteImageData=NULL;
}


void QFRDRImagingFCSImageEditor::excludeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutAddRun(run);
        }
        m->recalcCorrelations();
    }
    replotData();
}

void QFRDRImagingFCSImageEditor::includeRuns() {
    if (!current) return;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        QSetIterator<int32_t> i(selected);
        while (i.hasNext()) {
             int32_t run=i.next();
             m->leaveoutRemoveRun(run);
        }
        m->recalcCorrelations();
    }
    replotData();
}


void QFRDRImagingFCSImageEditor::createWidgets() {
    QLabel* l;
    QVBoxLayout* lb=new QVBoxLayout(this);
    setLayout(lb);
    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    lb->addWidget(splitter);

    QWidget* w=new QWidget(this);
    QVBoxLayout* vbl=new QVBoxLayout();
    w->setLayout(vbl);

    QGroupBox* wimg=new QGroupBox(tr(" image plots "), this);
    vbl->addWidget(wimg);
    QFormLayout* gli=new QFormLayout(this);
    wimg->setLayout(gli);
    cmbColorbar=new QComboBox(wimg);
    QStringList sl=JKQTFPimagePlot_getPalettes();
    for (int i=0; i<sl.size(); i++) {
        cmbColorbar->addItem(JKQTFPimagePlot_getPaletteIcon(i), sl[i]);
    }
    gli->addRow(tr("color bar:"), cmbColorbar);

    QGroupBox* wcp=new QGroupBox(tr(" correlation plots "), this);
    vbl->addWidget(wcp);
    QFormLayout* gl=new QFormLayout(this);
    wcp->setLayout(gl);
    chkDisplayAverage=new QCheckBox(w);
    chkDisplayAverage->setChecked(true);
    gl->addRow(tr("display &average:"), chkDisplayAverage);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), this, SLOT(replotData()));

    cmbAverageStyle=new QComboBox(w);
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbAverageStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageStyle, SLOT(setEnabled(bool)));

    cmbAverageErrorStyle=new QComboBox(w);
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbAverageErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), cmbAverageErrorStyle, SLOT(setEnabled(bool)));

    gl->addRow(tr("average &options:"), cmbAverageStyle);
    connect(chkDisplayAverage, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
    connect(cmbAverageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    gl->addRow("", cmbAverageErrorStyle);
    connect(cmbAverageErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));


    cmbRunStyle=new QComboBox(w);
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));

    cmbRunErrorStyle=new QComboBox(w);
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));

    gl->addRow((labRunOptions=new QLabel(tr("run &options:"), w)), cmbRunStyle);
    labRunOptions->setBuddy(cmbRunStyle);
    connect(cmbRunStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    gl->addRow("", cmbRunErrorStyle);
    connect(cmbRunErrorStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));

    chkLogTauAxis=new QCheckBox("", w);
    gl->addRow((l=new QLabel(tr("log &tau;-axis:"))), chkLogTauAxis);
    l->setTextFormat(Qt::RichText);
    connect(chkLogTauAxis, SIGNAL(clicked()), this, SLOT(replotData()));




    /*btnDontUse=new QPushButton(tr("exclude sel."), w);
    connect(btnDontUse, SIGNAL(clicked()), this, SLOT(excludeRuns()));
    gl->addWidget(btnDontUse, row, 1);
    btnUse=new QPushButton(tr("include sel."), w);
    connect(btnUse, SIGNAL(clicked()), this, SLOT(includeRuns()));
    gl->addWidget(btnUse, row, 2);
    row++;*/




    QWidget* wpltOverview=new QWidget(this);
    QVBoxLayout* lpltOverview=new QVBoxLayout();
    wpltOverview->setLayout(lpltOverview);
    pltOverview=new JKQTFastPlotter(wpltOverview);
    lpltOverview->addWidget(new QLabel(tr("Overview:")));
    lpltOverview->addWidget(pltOverview, 1);
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
    plteImageData=NULL;
    plteOverviewSize=0;










    QWidget* wpltImage=new QWidget(this);
    QVBoxLayout* lpltImage=new QVBoxLayout();
    wpltImage->setLayout(lpltImage);
    pltImage=new JKQTFastPlotter(wpltImage);
    lpltImage->addWidget((labParamImage=new QLabel(tr("Parameter Image:"))));
    lpltImage->addWidget(pltImage, 1);
    pltImage->set_aspectRatio(1);
    pltImage->set_maintainAspectRatio(true);
    pltImage->set_xAxisLabelVisible(false);
    pltImage->set_yAxisLabelVisible(false);
    pltImage->set_tickFontSize(8);
    pltImage->set_tickFontName(font().family());
    fo=font();
    fo.setPointSizeF(pltImage->get_tickFontSize());
    fm= QFontMetricsF(fo);
    pltImage->set_plotBorderBottom(2.0*fm.ascent());
    pltImage->set_plotBorderLeft(fm.width("00000"));
    pltImage->set_drawGrid(false);
    connect(pltImage, SIGNAL(clicked(double,double,Qt::KeyboardModifiers)), this,SLOT(previewClicked(double,double,Qt::KeyboardModifiers)));

    plteImage=new JKQTFPimagePlot(pltImage, NULL, JKQTFP_double, 1, 1, 0, 1, 0, 1, JKQTFP_GRAY);
    pltImage->addPlot(plteImage);
    connect(cmbColorbar, SIGNAL(currentIndexChanged(int)), plteImage, SLOT(set_palette(int)));

    col=QColor("red");
    col.setAlphaF(0.5);
    plteImageSelected=new JKQTFPimageOverlayPlot(pltImage, NULL, 1, 1, 0, 1, 0, 1, col);
    pltImage->addPlot(plteImageSelected);

    col=QColor("blue");
    col.setAlphaF(0.5);
    plteImageExcluded=new JKQTFPimageOverlayPlot(pltImage, NULL, 1, 1, 0, 1, 0, 1, col);
    pltImage->addPlot(plteImageExcluded);

    plteImageData=NULL;







    plotter = new JKQtPlotter(true, this);
    plotter->get_plotter()->set_showKey(true);
    plotterResid = new JKQtPlotter(true, this);
    plotterResid->get_plotter()->set_showKey(true);


    plotter->get_plotter()->set_gridPrinting(true);
    plotter->get_plotter()->addGridPrintingPlotter(0,1,plotterResid->get_plotter());
    plotter->set_displayToolbar(true);
    plotterResid->set_displayToolbar(false);
    plotterResid->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    plotterResid->getXAxis()->set_labelFontSize(11);
    plotterResid->getXAxis()->set_tickLabelFontSize(10);
    plotterResid->getYAxis()->set_axisLabel(tr("residuals"));
    plotterResid->getYAxis()->set_labelFontSize(11);
    plotterResid->getYAxis()->set_tickLabelFontSize(10);
    plotter->getXAxis()->set_axisLabel("");
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->getYAxis()->set_labelFontSize(11);
    plotter->getYAxis()->set_tickLabelFontSize(10);
    plotter->getXAxis()->set_drawMode1(JKQTPCADMticks);
    plotter->getXAxis()->set_tickLabelFontSize(10);
    plotterResid->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    plotterResid->get_plotter()->setBorder(1,1,1,1);
    plotter->get_plotter()->setBorder(1,1,1,1);
    plotterResid->synchronizeToMaster(plotter, true, false);
    plotter->get_plotter()->set_useAntiAliasingForSystem(true);
    plotter->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotterResid->get_plotter()->set_useAntiAliasingForSystem(true);
    plotterResid->get_plotter()->set_useAntiAliasingForGraphs(true);
    plotter->set_displayMousePosition(false);
    plotterResid->set_displayMousePosition(false);
    plotter->get_plotter()->set_keyFontSize(9);
    plotter->get_plotter()->set_keyXMargin(2);
    plotter->get_plotter()->set_keyYMargin(2);
    plotterResid->get_plotter()->set_keyFontSize(9);
    plotterResid->get_plotter()->set_keyXMargin(2);
    plotterResid->get_plotter()->set_keyYMargin(2);
    plotterResid->useExternalDatastore(plotter->getDatastore());



    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout(this);
    lp->setContentsMargins(0,0,0,0);
    wp->setLayout(lp);
    lp->addWidget(plotterResid,1);
    sliders=new DataCutSliders(this);
    connect(sliders, SIGNAL(slidersChanged(int , int , int, int)), this, SLOT(slidersChanged(int, int, int, int)));
    lp->addWidget(sliders);


    splitterTopBot=new QVisibleHandleSplitter(Qt::Vertical, this);
    splitterTop=new QVisibleHandleSplitter(Qt::Horizontal, this);
    splitterTop->addWidget(wpltImage);
    splitterTop->addWidget(wpltOverview);
    splitterTopBot->addWidget(splitterTop);
    splitterTopBot->addWidget(plotter);
    splitterTopBot->addWidget(wp);


    vbl->addWidget(new QWidget(this), 1);
    splitter->addWidget(splitterTopBot);
    splitter->addWidget(w);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0,5);
    splitter->setStretchFactor(1,1);

};

void QFRDRImagingFCSImageEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) disconnect(old, 0, this, 0);
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
        sliders->disableSliderSignals();
        sliders->set_min(0);
        sliders->set_max(m->getCorrelationN());
        sliders->set_userMin(current->getProperty("fcsimimeditor_datacut_min", 0).toInt());
        sliders->set_userMax(current->getProperty("fcsimimeditor_datacut_max", m->getCorrelationN()).toInt());
        sliders->enableSliderSignals();
    } else {
//        runs.setCurrent(current);
    }

    //readSettings();

};


void QFRDRImagingFCSImageEditor::previewClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

    int xx=(int)floor(x);
    int yy=(int)floor(y);

    int idx=m->xyToRun(xx, yy);

    if (modifiers==Qt::ControlModifier) {
        if (selected.contains(idx)) selected.remove(idx);
        else selected.insert(idx);
    } else {
        selected.clear();
        selected.insert(idx);
    }
    replotData();
}

void QFRDRImagingFCSImageEditor::rawDataChanged() {
    replotImage();
    replotData();
};

void QFRDRImagingFCSImageEditor::slidersChanged(int userMin, int userMax, int min, int max) {
    if (!current) return;
    current->setQFProperty("fcsimimeditor_datacut_min", userMin, false, false);
    current->setQFProperty("fcsimimeditor_datacut_max", userMax, false, false);
    //replotData();

    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        plotterResid->clearGraphs();
        ds->clear();
        return;
    }

    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    plotterResid->set_doDrawing(false);
    plotterResid->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    plotter->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());
    plotterResid->get_plotter()->setGraphsDataRange(sliders->get_userMin(), sliders->get_userMax());

    QApplication::restoreOverrideCursor();
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    plotter->update_plot();
    plotterResid->set_doDrawing(true);
    plotterResid->set_emitSignals(true);
    plotterResid->update_plot();

}

void QFRDRImagingFCSImageEditor::replotImage() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImageEditor::replotOverview() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    pltOverview->set_doDrawing(false);

    if (!m) {
        plteOverview->set_image(NULL, JKQTFP_uint16, 1, 1);
        plteOverviewSelected->set_data(NULL, 1, 1);
    } else {
        uint16_t* ov=m->getDataImagePreview();
        double w=m->getDataImageWidth();
        double h=m->getDataImageHeight();
        double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));

        pltOverview->setXRange(0,w);
        pltOverview->setYRange(0,h);
        pltOverview->set_xTickDistance(dx);
        pltOverview->set_yTickDistance(dy);
        pltOverview->set_aspectRatio(w/h);

        if (plteOverviewSize<w*h) {
            plteOverviewSize=w*h;
            plteOverviewSelectedData=(bool*)realloc(plteOverviewSelectedData, plteOverviewSize*sizeof(bool));
            plteOverviewExcludedData=(bool*)realloc(plteOverviewExcludedData, plteOverviewSize*sizeof(bool));
        }

        plteOverviewSelected->set_data(plteOverviewSelectedData, m->getDataImageWidth(), m->getDataImageHeight());
        plteOverviewSelected->set_xmax(w);
        plteOverviewSelected->set_ymax(h);
        plteOverviewExcluded->set_data(plteOverviewExcludedData, m->getDataImageWidth(), m->getDataImageHeight());
        plteOverviewExcluded->set_xmax(w);
        plteOverviewExcluded->set_ymax(h);
        plteOverview->set_image(ov, JKQTFP_uint16, m->getDataImageWidth(), m->getDataImageHeight());
        plteOverview->set_xmax(w);
        plteOverview->set_ymax(h);
        if (plteOverviewSelectedData) {
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                int x=m->runToX(i);
                int y=m->runToY(i);
                int idx=y*m->getDataImageWidth()+x;
                plteOverviewSelectedData[idx]=selected.contains(i);
                plteOverviewExcludedData[idx]=m->leaveoutRun(i);
            }
        }
    }

    pltOverview->set_doDrawing(true);
    pltOverview->update_data();
    QApplication::restoreOverrideCursor();
}

void QFRDRImagingFCSImageEditor::replotData(int dummy) {
    //qDebug()<<"replot";
    JKQTPdatastore* ds=plotter->getDatastore();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) {
        plotter->clearGraphs();
        ds->clear();
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    plotter->set_doDrawing(false);
    plotter->set_emitSignals(false);
    sliders->set_min(0);
    sliders->set_max(m->getCorrelationN());
    plotter->clearGraphs();
    ds->clear();


    if (m->getCorrelationN()>0) {

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        JKQTPerrorPlotstyle runerrorstyle=JKQTPnoError;
        switch (cmbRunErrorStyle->currentIndex()) {
            case 1: runerrorstyle=JKQTPerrorBars; break;
            case 2: runerrorstyle=JKQTPerrorLines; break;
            case 3: runerrorstyle=JKQTPerrorBarsLines; break;
        }
        bool runLine=(cmbRunStyle->currentIndex()!=1);
        JKQTPgraphSymbols runSymbol=JKQTPcross;
        if (cmbRunStyle->currentIndex()==0) runSymbol=JKQTPnoSymbol;

        JKQTPerrorPlotstyle avgerrorstyle=JKQTPnoError;
        switch (cmbAverageErrorStyle->currentIndex()) {
            case 1: avgerrorstyle=JKQTPerrorBars; break;
            case 2: avgerrorstyle=JKQTPerrorLines; break;
            case 3: avgerrorstyle=JKQTPerrorBarsLines; break;
        }
        bool avgLine=(cmbAverageStyle->currentIndex()!=1);
        JKQTPgraphSymbols avgSymbol=JKQTPcross;
        if (cmbAverageStyle->currentIndex()==0) avgSymbol=JKQTPnoSymbol;



        size_t c_tau=ds->addColumn(m->getCorrelationT(), m->getCorrelationN(), "tau");


        //QTime t;
        //t.start();


       //////////////////////////////////////////////////////////////////////////////////
       // Plot ALL RUNS (left out runs in gray, standard runs in black, highlighted run in red)
       //////////////////////////////////////////////////////////////////////////////////
        for (int i=0; i<m->getCorrelationRuns(); i++) {
            if (selected.contains(i)) {
                size_t c_run=ds->addColumn(m->getCorrelationRun(i), m->getCorrelationN(), QString("run %1 %2").arg(i).arg(m->getCorrelationRunName(i)).toStdString());
                size_t c_rune=ds->addColumn(m->getCorrelationRunError(i), m->getCorrelationN(), QString("run error %1 %2").arg(i).arg(m->getCorrelationRunName(i)).toStdString());
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

                if (!m->leaveoutRun(i)) {
                    g->set_color(QColor("red"));
                } else {
                    g->set_color(QColor("grey"));
                }
                g->set_errorColor(g->get_color().lighter());
                plotter->addGraph(g);
            }
        }



        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        if (chkDisplayAverage->isChecked()) {
            size_t c_mean=ds->addColumn(m->getCorrelationMean(), m->getCorrelationN(), "cmean");
            size_t c_std=ds->addColumn(m->getCorrelationStdDev(), m->getCorrelationN(), "cstddev");

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
            g->set_color(QColor("blue"));
            g->set_errorColor(g->get_color().lighter());
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

        plotter->getXAxis()->set_logAxis(chkLogTauAxis->isChecked());
        plotter->zoomToFit(true, true, false,false);

        QApplication::restoreOverrideCursor();
    }
    plotter->set_doDrawing(true);
    plotter->set_emitSignals(true);
    //QTime t;
    //t.start();
    plotter->update_plot();
    //qDebug()<<"plotting in "<<t.elapsed()<<" ms";
    replotOverview();
    QApplication::restoreOverrideCursor();
};


void QFRDRImagingFCSImageEditor::readSettings() {
    if (!settings) return;
    plotter->loadSettings(*(settings->getQSettings()), QString("imfcsimageeditor/corrplot"));
    chkLogTauAxis->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/log_tau_axis"), true).toBool());
    chkDisplayAverage->setChecked(settings->getQSettings()->value(QString("imfcsimageeditor/display_avg"), true).toBool());
    cmbAverageStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_style"), 0).toInt());
    cmbAverageErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/avg_error_style"), 2).toInt());
    cmbColorbar->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/colorbar"), 0).toInt());
    cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_style"), 1).toInt());
    cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsimageeditor/run_error_style"), 1).toInt());
    loadSplitter(*(settings->getQSettings()), splitter, "imfcsimageeditor/corrsplitterSizes");
};


void QFRDRImagingFCSImageEditor::writeSettings() {
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfcsimageeditor/log_tau_axis"), chkLogTauAxis->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/display_avg"), chkDisplayAverage->isChecked());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/colorbar"), cmbColorbar->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_style"), cmbAverageStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/avg_error_style"), cmbAverageErrorStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_style"), cmbRunStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsimageeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    saveSplitter(*(settings->getQSettings()), splitter, "imfcsimageeditor/corrsplitterSizes");
};





