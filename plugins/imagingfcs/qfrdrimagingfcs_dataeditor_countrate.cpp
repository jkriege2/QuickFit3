#include "qfrdrimagingfcs_dataeditor_countrate.h"
#include "qfrdrimagingfcs_data.h"
#include <QDebug>
#include "qfrawdatapropertyeditor.h"

void QFRDRImagingFCSDataEditorCountrate::excludeRuns() {
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

void QFRDRImagingFCSDataEditorCountrate::includeRuns() {
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





QFRDRImagingFCSDataEditorCountrate::QFRDRImagingFCSDataEditorCountrate(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent):
    QFRawDataEditor(services, propEditor, parent)
{
    createWidgets();
}

QFRDRImagingFCSDataEditorCountrate::~QFRDRImagingFCSDataEditorCountrate()
{
    if (plteOverviewSelectedData) free(plteOverviewSelectedData);
    plteOverviewSelectedData=NULL;
    if (plteOverviewExcludedData) free(plteOverviewExcludedData);
    plteOverviewExcludedData=NULL;
}

void QFRDRImagingFCSDataEditorCountrate::createWidgets() {
    imFCSTools=new QFRDRImagingFCSEditTools(this);
    menuImagingFCSTools=propertyEditor->addOrFindMenu(tr("ImagingFCS Tools"));
    imFCSTools->registerToMenu(menuImagingFCSTools);

    int row=0;
    QLabel* l;
    QVBoxLayout* lb=new QVBoxLayout(this);
    setLayout(lb);
    splitter=new QVisibleHandleSplitter(Qt::Horizontal, this);
    lb->addWidget(splitter);

    QWidget* w=new QWidget(this);
    QGridLayout* gl=new QGridLayout(this);
    w->setLayout(gl);
    cmbVideo=new QComboBox(w);

    gl->addWidget(new QLabel(tr("video:"), w), row, 0);
    gl->addWidget(cmbVideo, row, 1,1,2);
    connect(cmbVideo, SIGNAL(currentIndexChanged(int)), this, SLOT(replotData()));
    row++;


    cmbRunStyle=new JKQTPLinePlotStyleComboBox(w);
    /*cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_lines.png"), tr("lines"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_points.png"), tr("points"));
    cmbRunStyle->addItem(QIcon(":/imaging_fcs/fcsplot_pointslines.png"), tr("lines+points"));*/

    cmbRunErrorStyle=new JKQTPerrorPlotstyleComboBox(w);
    /*cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_enone.png"), tr("none"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_ebars.png"), tr("bars"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elines.png"), tr("lines"));
    cmbRunErrorStyle->addItem(QIcon(":/imaging_fcs/fcsplot_elinesbars.png"), tr("lines+bars"));*/

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
    plteOverview->setVisible(true);
    pltOverview->addPlot(plteOverview);
    plteOverviewRGB=new JKQTFPRGBImageOverlayPlot(pltOverview, NULL, JKQTFP_uint16, 1, 1, 0, 1, 0, 1);
    plteOverviewRGB->setVisible(false);
    pltOverview->addPlot(plteOverviewRGB);

    QColor col("orange");
    col.setAlphaF(0.5);
    plteOverviewSelected=new JKQTFPimageOverlayPlot(pltOverview, NULL, 1, 1, 0, 1, 0, 1, col);
    pltOverview->addPlot(plteOverviewSelected);

    col=QColor("darkblue");
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

    grpInfo=new QGroupBox(tr("Info"), w);
    QGridLayout* ggl=new QGridLayout(grpInfo);
    grpInfo->setLayout(ggl);
    ggl->addWidget(new QLabel(tr("# Pixels = ")), 0, 0);
    labRuns=new QLabel(grpInfo);
    ggl->addWidget(labRuns, 0,1);
    ggl->addWidget(new QLabel(tr("# Intensity Samples = ")), 1, 0);
    labCorrelationPoints=new QLabel(grpInfo);
    ggl->addWidget(labCorrelationPoints, 1,1);
    ggl->addWidget(new QLabel(tr("# Segments = ")), 2, 0);
    labSegments=new QLabel(grpInfo);
    ggl->addWidget(labSegments, 2,1);
    ggl->addWidget(new QLabel(tr("# Measurement Duration = ")), 3, 0);
    labDuration=new QLabel(grpInfo);
    ggl->addWidget(labDuration, 3,1);
    gl->addWidget(grpInfo, row,0,1,3);

    QWidget* wp=new QWidget(this);
    QVBoxLayout* lp=new QVBoxLayout(this);
    wp->setLayout(lp);
    plotter = new QFPlotter(true, this);
    plotter->get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    plotter->get_plotter()->set_showKey(true);
    plotter->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    plotter->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    plotter->get_plotter()->set_useAntiAliasingForGraphs(false);
    plotter->get_plotter()->set_useAntiAliasingForSystem(true);

    lp->addWidget(plotter);

    splitter->addWidget(wp);
    splitter->addWidget(w);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setStretchFactor(0,5);
    splitter->setStretchFactor(1,1);



};

void QFRDRImagingFCSDataEditorCountrate::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        disconnect(old, 0, this, 0);
        disconnect(old, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));        
    }
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) {
        runs.setCurrent(current);
        QStringList sl;

        for (int i=0; i<m->getImageStackCount(); i++) {
            sl<<m->getImageStackDescription(i);
        }

        cmbVideo->clear();
        cmbVideo->addItems(sl);
        cmbVideo->setCurrentIndex(0);
        connect(current, SIGNAL(rawDataChanged()), this, SLOT(rawDataChanged()));
    } else {
//        runs.setCurrent(current);
    }
    imFCSTools->setRDR(current);

    //readSettings();

};

void QFRDRImagingFCSDataEditorCountrate::selectionChanged(const QModelIndex & current, const QModelIndex & previous ) {
    replotData();
}

void QFRDRImagingFCSDataEditorCountrate::selectionChanged(const QItemSelection & current, const QItemSelection & previous ) {
    replotData();
}

void QFRDRImagingFCSDataEditorCountrate::previewClicked(double x, double y, Qt::KeyboardModifiers modifiers) {
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (!m) return;

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

void QFRDRImagingFCSDataEditorCountrate::runsModeChanged(int c) {
    labRunOptions->setEnabled(true);
    cmbRunStyle->setEnabled(true);
    cmbRunErrorStyle->setEnabled(true);
    lstRunsSelect->setEnabled(true);
    replotData();
}

void QFRDRImagingFCSDataEditorCountrate::rawDataChanged() {
    replotData();
}


void QFRDRImagingFCSDataEditorCountrate::replotOverview() {
     //qDebug()<<"replotOverview";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    bool doDraw=pltOverview->get_doDrawing();
    pltOverview->set_doDrawing(false);

    if (!m) {
        plteOverview->set_image(NULL, JKQTFP_uint16, 1, 1);
        plteOverviewRGB->set_image(NULL, JKQTFP_uint16, 1, 1);
        plteOverviewSelected->set_data(NULL, 0, 0);
        plteOverviewExcluded->set_data(NULL, 0, 0);
    } else {
        double* ov=m->getImageFromRunsPreview(0);
        double* ov2=m->getImageFromRunsPreview(1);
        int channels=m->getImageFromRunsChannelsAdvised();
        double w=m->getImageFromRunsWidth();
        double h=m->getImageFromRunsHeight();
        double dx=1;
        if (w>1) dx=pow(10.0,floor(log(w)/log(10.0)));
        double dy=1;
        if (h>1) dy=pow(10.0,floor(log(h)/log(10.0)));

        if (channels==2 && current && current->getProperty("INTERNAL_DUALVIEW_MODE_SWITCHEDCHANNEL", false).toBool()) qSwap(ov, ov2);

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
        if (channels==1) {
            plteOverview->set_image(ov, JKQTFP_double, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
            plteOverviewRGB->setVisible(false);
            plteOverview->setVisible(true);
        } else if (channels>1) {
            plteOverviewRGB->set_image(ov2, JKQTFP_double, ov, JKQTFP_double, m->getImageFromRunsWidth(), m->getImageFromRunsHeight());
            plteOverview->setVisible(false);
            plteOverviewRGB->setVisible(true);

        }
        plteOverview->set_xmax(w);
        plteOverview->set_ymax(h);
        plteOverviewRGB->set_xmax(w);
        plteOverviewRGB->set_ymax(h);
        if (plteOverviewSelectedData && plteOverviewExcludedData) {
            for (int i=0; i<m->getCorrelationRuns(); i++) {
                int x=m->runToX(i);
                int y=m->runToY(i);
                int idx=y*m->getImageFromRunsWidth()+x;
                plteOverviewSelectedData[idx]=lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0));
                plteOverviewExcludedData[idx]=m->leaveoutRun(i);
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

void QFRDRImagingFCSDataEditorCountrate::replotData(int dummy) {
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
    plotter->clearGraphs();
    plotter->get_plotter()->clearOverlayElement(true);
    ds->clear();


    int vidW=m->getImageStackWidth(cmbVideo->currentIndex());
    int vidH=m->getImageStackHeight(cmbVideo->currentIndex());
    int frames=m->getImageStackFrames(cmbVideo->currentIndex());
    int channels=m->getImageStackChannels(cmbVideo->currentIndex());
    double duration=m->getMeasurementDuration();
    double exposure=m->getFrameTime();
    int segments=m->getProperty("SEGMENTS", 0).toInt();
    int sumframes=m->getProperty("VIDEO_AVGFRAMES", 0).toInt();

    //qDebug()<<cmbVideo->currentIndex()<<cmbVideo->currentText()<<vidW<<vidH<<frames;

    labRuns->setText(QString("%1x%2 = %3").arg(vidW).arg(vidH).arg(m->getCorrelationRuns()));
    labCorrelationPoints->setText(tr("%1 à exp: %2ms (=%3frames)").arg(frames).arg(exposure*double(sumframes)).arg(sumframes));
    labSegments->setText(tr("%1 à %2s").arg(segments).arg(duration/double(segments)));
    labDuration->setText(QString("%1s").arg(duration));

    if (vidW*vidH==m->getCorrelationRuns()) {
        if (duration>0 && segments>0) {

            for (int i=0; i<segments; i++) {
                double t=double(i)*duration/double(segments);
                if (m->segmentUsed(i)) {
                    JKQTPoverlayVerticalLine* ovl=new JKQTPoverlayVerticalLine(t, tr("seg.%1  t=%2s").arg(i+1).arg(t), plotter->get_plotter());
                    ovl->set_color(QColor("black"));
                    ovl->set_lineWidth(2);
                    ovl->set_lineStyle(Qt::DotLine);
                    ovl->set_fontSize(12);
                    plotter->get_plotter()->addOverlayElement(ovl);
                } else {
                    JKQTPoverlayVerticalRange* ovl=new JKQTPoverlayVerticalRange(t, t+duration/double(segments), tr("seg.%1  t=%2s").arg(i+1).arg(t), plotter->get_plotter());
                    QColor fill=QColor("darkgray");
                    fill.setAlphaF(0.6);
                    ovl->set_color(QColor("darkgray"));
                    ovl->set_fillColor(fill);
                    ovl->set_lineWidth(2);
                    ovl->set_lineStyle(Qt::DotLine);
                    ovl->set_fontSize(12);
                    plotter->get_plotter()->addOverlayElement(ovl);
                }
            }
        }




        JKQTPerrorPlotstyle runerrorstyle=cmbRunErrorStyle->getErrorStyle();
        /*switch (cmbRunErrorStyle->currentIndex()) {
            case 1: runerrorstyle=JKQTPerrorBars; break;
            case 2: runerrorstyle=JKQTPerrorLines; break;
            case 3: runerrorstyle=JKQTPerrorBarsLines; break;
            case 4: runerrorstyle=JKQTPerrorPolygons; break;
            case 5: runerrorstyle=JKQTPerrorBarsPolygons; break;
        }*/
        /*bool runLine=(cmbRunStyle->currentIndex()!=1);
        JKQTPgraphSymbols runSymbol=JKQTPcross;
        if (cmbRunStyle->currentIndex()==0) runSymbol=JKQTPnoSymbol;*/
        bool runLine=cmbRunStyle->getDrawLine();
        JKQTPgraphSymbols runSymbol=cmbRunStyle->getSymbol();

        //qDebug()<<frames<<m->getImageStackTUnitFactor(cmbVideo->currentIndex());
        size_t c_tau=ds->addLinearColumn(frames, 0, frames*m->getImageStackTUnitFactor(cmbVideo->currentIndex()), tr("time [%1]").arg(m->getImageStackTUnitName(cmbVideo->currentIndex())));

        plotter->get_xAxis()->set_axisLabel(tr("time $t$ [%1]").arg(m->getImageStackTUnitName(cmbVideo->currentIndex())));
        plotter->get_yAxis()->set_axisLabel(tr("intensity $I(t)$ [A.U. or kHz]"));


        double cnt=0;
        for (int i=0; i<m->getCorrelationRuns(); i++) {
            if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                cnt++;
            }
        }
        double cc=0;
        for (int i=0; i<m->getCorrelationRuns(); i++) {
            if (lstRunsSelect->selectionModel()->isSelected(runs.index(i+1, 0))) {
                for (int c=0; c<channels; c++) {
                    double* d=m->getImageStack(cmbVideo->currentIndex(),0, c);
                    size_t c_run=ds->addCopiedColumn(&(d[i]),frames, vidW*vidH, QString("pixel %1 %2").arg(i).arg(m->getCorrelationRunName(i)));
                    size_t c_rune=-1;
                    JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph();
                    g->set_lineWidth(1);
                    g->set_xColumn(c_tau);
                    g->set_yColumn(c_run);
                    g->set_drawLine(runLine);
                    g->set_symbol(runSymbol);
                    g->set_title(tr("run %1: %2").arg(i).arg(m->getCorrelationRunName(i)));
                    QColor col;
                    if (cnt>1) {
                        col.setHsvF(cc/cnt,1.0,1.0);
                        if (c%3==0) g->set_color(col);
                        else g->set_color(col.darker(150));
                        if (c%3==1) g->set_style(Qt::DashLine);
                        if (c%3==2) g->set_style(Qt::DashDotLine);
                    } else  {
                        col=QColor("green");
                        if (c==1) col=QColor("red");
                        if (c==2) col=QColor("blue");
                        if (c==3) col=QColor("magenta");
                        g->set_color(col);
                        g->set_style(Qt::SolidLine);
                    }



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


                    if (cmbVideo->currentText().toLower().contains("uncorrected") || cmbVideo->currentText().toLower().contains("uncorr")) {
                        double background=m->getProperty("BASELINE", 0).toDouble();
                        double bleachTime=0;
                        double bleachAmplitude=0;
                        double bleachTime2=0;
                        double bleachAmplitude2=0;
                        double bleachFactor=0;
                        double bleachFactor3=0;
                        double bleachOffset=0;
                        bool hasBTime=false, hasBA=false, hasBTime2=false, hasBA2=false, hasBFactor=false, hasBFactor3=false, hasBOffset=false;
                        for (int id=0; id<m->getOverviewImageCount(); id++) {
                            QString ft=m->getOverviewImageID(id);
                            QString name=m->getOverviewImageName(id);
                            double* d=m->getOverviewImage(id);
                            //qDebug()<<"ft="<<ft<<"   name="<<name<<"   d="<<d<<"        "<<m->getOverviewImageHeight(id)<<" x "<<m->getOverviewImageWidth(id);
                            if (d && m->getOverviewImageHeight(id)>=vidH && m->getOverviewImageWidth(id)>=vidW) {
                                if (ft=="background" || ft=="background frame") background=d[i];
                                if (ft=="display_image") {
                                    if (name.toLower().contains(tr("bleach amplitude 2"))) {
                                        bleachAmplitude2=m->readValueFromPossiblySplitImage(d, m->getOverviewImageWidth(id), m->getOverviewImageHeight(id), i, c>0);
                                        hasBA2=true;
                                    } else if (name.toLower().contains(tr("bleach time 2 file"))) {
                                        bleachTime2=m->readValueFromPossiblySplitImage(d, m->getOverviewImageWidth(id), m->getOverviewImageHeight(id), i, c>0);
                                        hasBTime2=true;
                                    } else if (name.toLower().contains(tr("bleach amplitude"))) {
                                        bleachAmplitude=m->readValueFromPossiblySplitImage(d, m->getOverviewImageWidth(id), m->getOverviewImageHeight(id), i, c>0);
                                        hasBA=true;
                                    } else if (name.toLower().contains(tr("bleach time file"))) {
                                        bleachTime=m->readValueFromPossiblySplitImage(d, m->getOverviewImageWidth(id), m->getOverviewImageHeight(id), i, c>0);
                                        hasBTime=true;
                                    } else if (name.toLower().contains(tr("bleach polynomial factor file"))) {
                                        bleachFactor=m->readValueFromPossiblySplitImage(d, m->getOverviewImageWidth(id), m->getOverviewImageHeight(id), i, c>0);
                                        hasBFactor=true;
                                    } else if (name.toLower().contains(tr("bleach polynomial factor 3 file"))) {
                                        bleachFactor3=m->readValueFromPossiblySplitImage(d, m->getOverviewImageWidth(id), m->getOverviewImageHeight(id), i, c>0);
                                        hasBFactor3=true;
                                    } else if (name.toLower().contains(tr("bleach polynomial shift file")) || name.toLower().contains(tr("bleach polynomial offset file"))) {
                                        bleachOffset=m->readValueFromPossiblySplitImage(d, m->getOverviewImageWidth(id), m->getOverviewImageHeight(id), i, c>0);
                                        hasBOffset=true;
                                    }
                                }
                            }
                        }
//                        qDebug()<<"background="<<background;
//                        qDebug()<<"bleachAmplitude="<<hasBA<<bleachAmplitude;
//                        qDebug()<<"bleachTime="<<hasBTime<<bleachTime;
//                        qDebug()<<"bleachFactor="<<hasBFactor<<bleachFactor;
//                        qDebug()<<"bleachFactor3="<<hasBFactor3<<bleachFactor3;
//                        qDebug()<<"bleachOffset="<<hasBOffset<<bleachOffset;
                        if (hasBTime && hasBA) {
                            double* d=(double*)malloc(frames*sizeof(double));
                            double timeFactor=m->getProperty("FRAME_COUNT", 1).toDouble()/double(frames);
                            if (hasBFactor&&hasBFactor3&&hasBOffset) {
                                for (int tt=0; tt<frames; tt++) {
                                    double time=double(tt)*timeFactor;
                                    d[tt]=background+bleachAmplitude*exp(-(bleachOffset+time+bleachFactor*qfSqr(time)+bleachFactor3*qfCube(time))/bleachTime);
                                }
                            } else if (hasBFactor&&hasBOffset) {
                                for (int tt=0; tt<frames; tt++) {
                                    double time=double(tt)*timeFactor;
                                    d[tt]=background+bleachAmplitude*exp(-(bleachOffset+time+bleachFactor*qfSqr(time))/bleachTime);
                                }
                            } else if (hasBTime2 && hasBA2) {
                                for (int tt=0; tt<frames; tt++) {
                                    double time=double(tt)*timeFactor;
                                    d[tt]=background+bleachAmplitude*exp(-time/bleachTime)+bleachAmplitude2*exp(-time/bleachTime2);
                                }
                            } else {
                                for (int tt=0; tt<frames; tt++) {
                                    double time=double(tt)*timeFactor;
                                    d[tt]=background+bleachAmplitude*exp(-time/bleachTime);
                                }
                            }
                            size_t c_bleach=ds->addColumn(d, frames, tr("bleach correction fit"));
                            JKQTPxyLineGraph* gl=new JKQTPxyLineGraph(plotter->get_plotter());
                            gl->set_xColumn(c_tau);
                            gl->set_yColumn(c_bleach);
                            gl->set_color(col.darker());
                            gl->set_symbol(JKQTPnoSymbol);
                            gl->set_drawLine(true);
                            gl->set_lineWidth(2);
                            gl->set_style(Qt::DashLine);
                            if (hasBFactor3&&hasBFactor&&hasBOffset) {
                                gl->set_title(tr("run %1: exp(pol3) fit (b=%2, A=%3, o=%5, f=%6, f_3=%7, \\tau_B=%4 s)").arg(i).arg(background).arg(floattolatexstr(bleachAmplitude, 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(floattolatexstr(bleachTime/m->getProperty("FRAME_COUNT", 1).toDouble()*m->getMeasurementDuration(), 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(bleachOffset).arg(bleachFactor).arg(bleachFactor3));
                            } else if (hasBFactor&&hasBOffset) {
                                gl->set_title(tr("run %1: exp(pol2) fit (b=%2, A=%3, o=%5, f=%6, \\tau_B=%4 s)").arg(i).arg(background).arg(floattolatexstr(bleachAmplitude, 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(floattolatexstr(bleachTime/m->getProperty("FRAME_COUNT", 1).toDouble()*m->getMeasurementDuration(), 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(bleachOffset).arg(bleachFactor));
                            } else if (hasBTime2 && hasBA2) {
                                gl->set_title(tr("run %1: dblexp fit (b=%2, A=%3, \\tau_B=%4 s, A2=%5, \\tau_{B2}=%6 s)").arg(i).arg(background).arg(floattolatexstr(bleachAmplitude, 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(floattolatexstr(bleachTime/m->getProperty("FRAME_COUNT", 1).toDouble()*m->getMeasurementDuration(), 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(floattolatexstr(bleachAmplitude2, 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(floattolatexstr(bleachTime2/m->getProperty("FRAME_COUNT", 1).toDouble()*m->getMeasurementDuration(), 2, true, 1e-16, 1e-3, 1e5).c_str()));
                            } else {
                                gl->set_title(tr("run %1: exp fit (b=%2, A=%3, \\tau_B=%4 s)").arg(i).arg(background).arg(floattolatexstr(bleachAmplitude, 2, true, 1e-16, 1e-3, 1e5).c_str()).arg(floattolatexstr(bleachTime/m->getProperty("FRAME_COUNT", 1).toDouble()*m->getMeasurementDuration(), 2, true, 1e-16, 1e-3, 1e5).c_str()));
                            }
                            plotter->addGraph(gl);
                        }
                        if (background!=0) {
                            JKQTPgeoInfiniteLine* gb=new JKQTPgeoInfiniteLine(plotter->get_plotter(), 0, background, 1,0,col.darker(),2,Qt::DotLine);
                            gb->set_title(tr("run %1: background (b=%2)").arg(i).arg(background));
                            plotter->addGraph(gb);
                        }
                    }

                }
                cc++;
            }
        }

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


void QFRDRImagingFCSDataEditorCountrate::readSettings() {
    if (!settings) return;
    plotter->loadSettings(*(settings->getQSettings()), QString("imfcsdataeditor/corrplot"));
    cmbRunStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsdataeditor/run_style"), 0).toInt());
    cmbRunErrorStyle->setCurrentIndex(settings->getQSettings()->value(QString("imfcsdataeditor/run_error_style"), 0).toInt());
    loadSplitter(*(settings->getQSettings()), splitter, "imfcsdataeditor/corrsplitterSizes");
}


void QFRDRImagingFCSDataEditorCountrate::writeSettings() {
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfcsdataeditor/run_style"), cmbRunStyle->currentIndex());
    settings->getQSettings()->setValue(QString("imfcsdataeditor/run_error_style"), cmbRunErrorStyle->currentIndex());
    saveSplitter(*(settings->getQSettings()), splitter, "imfcsdataeditor/corrsplitterSizes");
}





