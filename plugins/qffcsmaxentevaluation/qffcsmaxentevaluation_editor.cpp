#include "qffcsmaxentevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qffcsmaxentevaluation_item.h"

#include <QtGui>
#include <QtCore>

QFFCSMaxEntEvaluationEditor::QFFCSMaxEntEvaluationEditor(QFPluginServices* services, QWidget* parent):
    QFFCSByIndexAndModelEvaluationEditor(services, parent)
{
    dataEventsEnabled=false;
    createWidgets();
    dataEventsEnabled=true;
}

QFFCSMaxEntEvaluationEditor::~QFFCSMaxEntEvaluationEditor()
{
}


void QFFCSMaxEntEvaluationEditor::createWidgets() {
    edtAlpha=new NumberEdit(this);
    edtAlpha->setRange(0, DBL_MAX);
    edtAlpha->setCheckBounds(true, false);
    edtAlpha->setDecimals(10);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&alpha; = </i>"), edtAlpha);

////////////////////////////////////////
    edtNdist=new QSpinBox(this);
    edtNdist->setRange(10, INT_MAX);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&Ndist = </i>"), edtNdist);
    edtNumIter=new QSpinBox(this);
    edtNumIter->setRange(10, INT_MAX);
    flAlgorithmParams->addRow(tr("MaxEnt: <i>&Iterations/Step = </i>"), edtNumIter);
////////////////////////////////////////




    cmbWeights=new QComboBox(this);
    cmbWeights->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    cmbWeights->setEditable(false);
    cmbWeights->addItem(tr("equal weights"));
    cmbWeights->addItem(tr("standard deviation"));
    cmbWeights->addItem(tr("per run errors"));
    cmbWeights->setMaximumWidth(150);
    cmbWeights->setMinimumWidth(150);
    QLabel* l=new QLabel(tr("&Weight Model: "), this);
    l->setBuddy(cmbWeights);
    hblModel->addWidget(l);
    hblModel->addWidget(cmbWeights);
    hblModel->addStretch();


    tbPlot=new QToolBar(QString("tbPlot"), this);
    tbPlot->setIconSize(QSize(16,16));
    tbPlot->addAction(pltData->get_plotter()->get_actSavePlot());
    pltData->get_plotter()->get_actSavePlot()->setToolTip(tr("save the correlation and residuals plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlot->addAction(pltData->get_plotter()->get_actPrint());
    pltData->get_plotter()->get_actPrint()->setToolTip(tr("print the correlation and residuals plot"));
    tbPlot->addAction(pltData->get_plotter()->get_actCopyPixelImage());
    pltData->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the correlation and residuals plot as an image to the system clipboard"));
    tbPlot->addSeparator();
    tbPlot->addAction(pltData->get_plotter()->get_actZoomAll());
    tbPlot->addAction(pltData->get_plotter()->get_actZoomIn());
    tbPlot->addAction(pltData->get_plotter()->get_actZoomOut());

    toolbar->removeAction(pltData->get_plotter()->get_actSavePlot());
    toolbar->removeAction(pltData->get_plotter()->get_actPrint());
    toolbar->removeAction(pltData->get_plotter()->get_actCopyPixelImage());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomAll());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomIn());
    toolbar->removeAction(pltData->get_plotter()->get_actZoomOut());
    plotLayout->insertWidget(0, tbPlot);


    pltDistribution=new JKQtPlotter(true, this);
    pltDistribution->useExternalDatastore(pltData->getDatastore());
    pltDistribution->set_displayToolbar(false);
    pltDistribution->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    pltDistribution->getXAxis()->set_labelFontSize(11);
    pltDistribution->getXAxis()->set_tickLabelFontSize(10);
    pltDistribution->getXAxis()->set_logAxis(true);
    pltDistribution->getYAxis()->set_axisLabel(tr("MaxEnt distribution"));
    pltDistribution->getYAxis()->set_labelFontSize(11);
    pltDistribution->getYAxis()->set_tickLabelFontSize(10);
    pltDistribution->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    pltDistribution->get_plotter()->setBorder(1,1,1,1);
    pltDistribution->get_plotter()->set_useAntiAliasingForSystem(true);
    pltDistribution->get_plotter()->set_useAntiAliasingForGraphs(true);
    pltDistribution->set_displayMousePosition(false);
    pltDistribution->get_plotter()->set_keyFontSize(9);
    pltDistribution->get_plotter()->set_keyXMargin(2);
    pltDistribution->get_plotter()->set_keyYMargin(2);


    tbPlotDistribution=new QToolBar(QString("tbPlotDistribution"), this);
    tbPlotDistribution->setIconSize(QSize(16,16));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actSavePlot());
    pltDistribution->get_plotter()->get_actSavePlot()->setToolTip(tr("save the distribution plot as an image (PNG, JPEG, PDF, SVG, ...)"));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actPrint());
    pltDistribution->get_plotter()->get_actPrint()->setToolTip(tr("print the distribution plot "));
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actCopyPixelImage());
    pltDistribution->get_plotter()->get_actCopyPixelImage()->setToolTip(tr("copy the distribution plot as an image to the system clipboard"));
    tbPlotDistribution->addSeparator();
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomAll());
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomIn());
    tbPlotDistribution->addAction(pltDistribution->get_plotter()->get_actZoomOut());


    QWidget* wPltDist=new QWidget(this);
    QVBoxLayout* layPltDist=new QVBoxLayout(this);
    wPltDist->setLayout(layPltDist);
    layPltDist->setContentsMargins(0,0,0,0);
    layPltDist->addWidget(tbPlotDistribution);
    layPltDist->addWidget(pltDistribution);
    splitMorePLot->addWidget(wPltDist);

    splitMorePLot->setCollapsible(splitMorePLot->indexOf(pltDistribution), false);

}










void QFFCSMaxEntEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    QFFCSByIndexAndModelEvaluationEditor::connectWidgets(current, old);

    if (old) {
        disconnect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));
        disconnect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        disconnect(edtNdist, SIGNAL(valueChanged(int)),this,SLOT(NdistChanged(int)));
        disconnect(edtNumIter, SIGNAL(valueChanged(int)),this,SLOT(NumIterChanged(int)));
    }

    QFFCSMaxEntEvaluationItem* item=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    //QFFCSMaxEntEvaluationItem* item_old=qobject_cast<QFFCSMaxEntEvaluationItem*>(old);
    if (item) {
        dataEventsEnabled=false;

        edtAlpha->setValue(item->getAlpha());
        connect(edtAlpha, SIGNAL(valueChanged(double)), this, SLOT(alphaChanged(double)));

        edtNdist->setValue(item->getNdist());
        connect(edtNdist, SIGNAL(valueChanged(int)), this, SLOT(NdistChanged(int)));

        edtNumIter->setValue(item->getNumIter());
        connect(edtNumIter, SIGNAL(valueChanged(int)), this, SLOT(NumIterChanged(int)));

        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));

        dataEventsEnabled=true;
    }

}


void QFFCSMaxEntEvaluationEditor::readSettings() {
    QFFCSByIndexAndModelEvaluationEditor::readSettings();
};

void QFFCSMaxEntEvaluationEditor::writeSettings() {
    QFFCSByIndexAndModelEvaluationEditor::writeSettings();

}



void QFFCSMaxEntEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    QFFCSByIndexAndModelEvaluationEditor::highlightingChanged(formerRecord, currentRecord);

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    //QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(currentRecord);

    if (data && eval) {
        bool oldde=dataEventsEnabled;
        dataEventsEnabled=false;
        edtAlpha->setValue(eval->getAlpha());
        cmbWeights->setCurrentIndex(eval->getCurrentWeights());
        edtNdist->setRange(10,data->getCorrelationN()); //qMax(0,data->getCorrelationN())
        edtNdist->setValue(eval->getNdist());
        edtNumIter->setRange(1,10000); //qMax(0,data->getCorrelationN())
        edtNumIter->setValue(eval->getNumIter());
        dataEventsEnabled=oldde;
    }

}


void QFFCSMaxEntEvaluationEditor::displayData() {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);

    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    JKQTPdatastore* dsdist=pltDistribution->getDatastore();

    if ((!eval)||(!data)) {
        pltData->clearGraphs();
        ds->clear();
        return;
    } else {

        //qDebug()<<" **** displayData()";
        QTime t, t1;
        t.start();
        t1.start();

        //bool updEn=updatesEnabled();
        //setUpdatesEnabled(false);

        pltResiduals->set_doDrawing(false);
        pltResiduals->set_emitSignals(false);
        pltResiduals->clearGraphs();
        pltData->set_doDrawing(false);
        pltData->set_emitSignals(false);
        pltData->clearGraphs();
        pltResidualHistogram->set_doDrawing(false);
        pltResidualHistogram->set_emitSignals(false);
        pltResidualHistogram->clearGraphs();
        pltResidualCorrelation->set_doDrawing(false);
        pltResidualCorrelation->set_emitSignals(false);
        pltResidualCorrelation->clearGraphs();
        pltDistribution->set_doDrawing(false);
        pltDistribution->set_emitSignals(false);
        pltDistribution->clearGraphs();
        dsres->clear();
        ds->clear();
        dsresh->clear();
        dsresc->clear();
        dsdist->clear();

        //qDebug()<<"   a "<<t.elapsed()<<" ms";
        t.start();

        pltResiduals->getXAxis()->set_logAxis(chkXLogScale->isChecked());
        pltData->getXAxis()->set_logAxis(chkXLogScale->isChecked());
        if (chkXLogScale->isChecked()) {
            pltData->getXAxis()->set_minorTicks(9);
            pltResiduals->getXAxis()->set_minorTicks(9);
        } else {
            pltData->getXAxis()->set_minorTicks(1);
            pltResiduals->getXAxis()->set_minorTicks(1);
        }
        pltResiduals->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltResiduals->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltData->getXAxis()->set_drawGrid(chkGrid->isChecked());
        pltData->getYAxis()->set_drawGrid(chkGrid->isChecked());
        pltData->getYAxis()->set_minTicks(5);
        pltResiduals->getYAxis()->set_minTicks(5);

        //qDebug()<<"   b "<<t.elapsed()<<" ms";
        t.start();

        int errorStyle=cmbErrorStyle->currentIndex();
        int plotStyle=cmbPlotStyle->currentIndex();
        //int residualStyle=cmbResidualStyle->currentIndex();

        if (data->getCorrelationN()>0) {
            size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");


            //////////////////////////////////////////////////////////////////////////////////
            // Plot average + error markers
            //////////////////////////////////////////////////////////////////////////////////
            size_t c_mean=0;
            QString graphName="";
            size_t c_std=0;
            QString errorName="";
            if (eval->getCurrentIndex()<0) {
                c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                graphName=tr("\\verb{%1} average").arg(record->getName());
                c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                errorName=tr("stddev");
            } else {
                if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                    c_mean=ds->addColumn(data->getCorrelationRun(eval->getCurrentIndex()), data->getCorrelationN(), QString("run"+QString::number(eval->getCurrentIndex())));
                    graphName=tr("\\verb{%1} %2").arg(record->getName()).arg(data->getCorrelationRunName(eval->getCurrentIndex()));
                    if (eval->getCurrentWeights()==2) {
                        c_std=ds->addColumn(data->getCorrelationRunError(eval->getCurrentIndex()), data->getCorrelationN(), "cperrunerror");
                        errorName=tr("per run");
                    } else {
                        c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                        errorName=tr("stddev");
                    }
                } else {
                    c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                    graphName=tr("\\verb{%1} average").arg(record->getName());
                    c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                    errorName=tr("stddev");
                }
            }
            JKQTPerrorPlotstyle styl=JKQTPnoError;
            switch (errorStyle) {
                case 1: styl=JKQTPerrorLines; break;
                case 2: styl=JKQTPerrorBars; break;
                case 3: styl=JKQTPerrorBarsLines; break;
            }
            //qDebug()<<"   c "<<t.elapsed()<<" ms";
            t.start();

            JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(pltData->get_plotter());
            QColor gcolor=pltData->get_plotter()->get_graphColor();
            g->set_color(gcolor);
            g->set_errorColor(gcolor.lighter());
            g->set_lineWidth(2);
            g->set_symbolSize(8);
            g->set_symbolWidth(1);
            g->set_xColumn(c_tau);
            g->set_yColumn(c_mean);
            g->set_yErrorColumn(c_std);
            g->set_title(graphName);
            g->set_yErrorStyle(styl);
            g->set_xErrorStyle(JKQTPnoError);
            g->set_datarange_start(datacut->get_userMin());
            g->set_datarange_end(datacut->get_userMax());
            // draw lines is default
            if (plotStyle==0) { // draw points
                g->set_drawLine(false);
                g->set_symbol(JKQTPcross);
            } else if (plotStyle==2) {
                g->set_symbol(JKQTPcross);
            }
            pltData->addGraph(g);
            //qDebug()<<"   d "<<t.elapsed()<<" ms";
            t.start();

            updateFitFunctions();
            //qDebug()<<"   e "<<t.elapsed()<<" ms";
            t.start();

            pltData->zoomToFit(true, true);
            pltResiduals->zoomToFit(false, true);
            pltDistribution->zoomToFit(true, true);

            pltResiduals->setX(pltData->getXMin(), pltData->getXMax());

            pltResidualHistogram->zoomToFit(true, true);
            pltResidualCorrelation->zoomToFit(true, true);
            //qDebug()<<"   f "<<t.elapsed()<<" ms";
            t.start();
        }


        pltResiduals->set_doDrawing(true);
        pltResiduals->set_emitSignals(true);
        pltData->set_doDrawing(true);
        pltData->set_emitSignals(true);
        pltResidualHistogram->set_doDrawing(true);
        pltResidualHistogram->set_emitSignals(true);
        pltResidualCorrelation->set_doDrawing(true);
        pltResidualCorrelation->set_emitSignals(true);
        pltDistribution->set_doDrawing(true);
        pltDistribution->set_emitSignals(true);
        //qDebug()<<"   g "<<t.elapsed()<<" ms";
        t.start();

        pltResiduals->update_plot();
        //qDebug()<<"   h "<<t.elapsed()<<" ms";
        t.start();
        pltData->update_plot();
        //qDebug()<<"   i "<<t.elapsed()<<" ms";
        t.start();
        pltResidualHistogram->update_plot();
        //qDebug()<<"   j "<<t.elapsed()<<" ms";
        t.start();
        pltResidualCorrelation->update_plot();
        //qDebug()<<"   k "<<t.elapsed()<<" ms";
        t.start();
        pltDistribution->update_plot();
        //qDebug()<<"   l "<<t.elapsed()<<" ms";
        t.start();
        //qDebug()<<"  displayData end  runtime = "<<t1.elapsed()<<" ms";

        //setUpdatesEnabled(updEn);

    }
}


void QFFCSMaxEntEvaluationEditor::updateFitFunctions() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    int index=eval->getCurrentIndex();
    int model=eval->getCurrentModel();
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    JKQTPdatastore* dsdist=pltDistribution->getDatastore();


    if ((!eval)||(!data)) {
        return;
    }


    //qDebug()<<" **** updateFitFunctions";
    QTime t, t1;
    t.start();
    t1.start();

    int residualStyle=cmbResidualStyle->currentIndex();
    int residualHistogramBins=spinResidualHistogramBins->value();
    int datacut_min=datacut->get_userMin();
    int datacut_max=datacut->get_userMax();


    //bool updEn=updatesEnabled();
    //setUpdatesEnabled(false);
    try {

        if (data->getCorrelationN()>0) {
            int c_tau=ds->getColumnNum("tau");
            if (c_tau>=0) { // we only add a graph, if we have a column with tau values
                eval->set_doEmitPropertiesChanged(false);
                eval->set_doEmitResultsChanged(false);
                record->disableEmitResultsChanged();
                //qDebug()<<"    a "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // retrieve data and tau-values from rawdata record
                /////////////////////////////////////////////////////////////////////////////////
                long N=data->getCorrelationN();
                int runAvgWidth=11;
                double* tauvals=data->getCorrelationT();
                double* corrdata=NULL;

                if (index<0) {
                    corrdata=data->getCorrelationMean();
                } else {
                    if (index<(int)data->getCorrelationRuns()) {
                        corrdata=data->getCorrelationRun(index);
                    } else {
                        corrdata=data->getCorrelationMean();
                    }
                }

                double* weights=eval->allocWeights(NULL, record, index, datacut_min, datacut_max);

                int rangeMaxDatarange=getUserMax(record, index);
                int rangeMinDatarange=getUserMin(record, index);


                /////////////////////////////////////////////////////////////////////////////////
                // calculate fit statistics
                /////////////////////////////////////////////////////////////////////////////////
                record->disableEmitResultsChanged();
                double* modelVals=(double*)malloc(N*sizeof(double));
                QVector<double> dist=eval->getDistribution(record, index, model);
                QVector<double> distTau=eval->getDistributionTaus(record, index, model);
                eval->evaluateModel(record, index, model, tauvals, modelVals, N, distTau.data(), dist.data(), dist.size());

                // CALCULATE FIT STATISTICS
                QFFitStatistics fit_stat=eval->calcFitStatistics(record, index, model, tauvals, modelVals, N, dist.size(), rangeMinDatarange, rangeMaxDatarange, runAvgWidth, residualHistogramBins);

                record->enableEmitResultsChanged();


                size_t c_fit = ds->addCopiedColumn(fit_stat.fitfunc, N, "fit_model");
                //qDebug()<<"    f "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot fit model and additional function graphs
                /////////////////////////////////////////////////////////////////////////////////
                JKQTPxyLineGraph* g_fit=new JKQTPxyLineGraph(pltData->get_plotter());
                g_fit->set_drawLine(true);
                g_fit->set_title("fit function");
                g_fit->set_xColumn(c_tau);
                g_fit->set_yColumn(c_fit);
                g_fit->set_datarange_start(datacut->get_userMin());
                g_fit->set_datarange_end(datacut->get_userMax());
                pltData->addGraph(g_fit);
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_taures=c_tau;//dsres->addCopiedColumn(data->getCorrelationT(), N, "tau");
                size_t c_residuals=0;
                JKQTPxyLineGraph* g_residuals=new JKQTPxyLineGraph(pltResiduals->get_plotter());
                if (chkWeightedResiduals->isChecked()) {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals_weighted, N, "residuals_weighted");
                    g_residuals->set_title("weighted residuals");
                } else {
                    c_residuals=dsres->addCopiedColumn(fit_stat.residuals, N, "residuals");
                    g_residuals->set_title("residuals");
                }
                g_residuals->set_xColumn(c_taures);
                g_residuals->set_yColumn(c_residuals);
                g_residuals->set_symbolSize(8);
                g_residuals->set_symbolWidth(1);
                g_residuals->set_datarange_start(datacut->get_userMin());
                g_residuals->set_datarange_end(datacut->get_userMax());
                g_residuals->set_drawLine(true);
                if (residualStyle==0) { // draw points
                    g_residuals->set_drawLine(false);
                    g_residuals->set_symbol(JKQTPcross);
                } else if (residualStyle==2) {
                    g_residuals->set_symbol(JKQTPcross);
                }
                pltResiduals->addGraph(g_residuals);
                //qDebug()<<"    h "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals running average
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_tauresra=dsres->addCopiedColumn(fit_stat.tau_runavg, fit_stat.runAvgN, "tau_resid_runavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg_weighted, fit_stat.runAvgN, "residuals_runavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(fit_stat.residuals_runavg, fit_stat.runAvgN, "residuals_runavg");
                    g_residualsra->set_title("residuals, movAvg");
                }
                g_residualsra->set_xColumn(c_tauresra);
                g_residualsra->set_yColumn(c_residualsra);
                g_residualsra->set_symbol(JKQTPnoSymbol);
                g_residualsra->set_symbolSize(6);
                g_residualsra->set_symbolWidth(1);
                g_residualsra->set_color(g_residuals->get_color().darker());
                //g_residuals->set_datarange_start(datacut->get_userMin());
                //g_residuals->set_datarange_end(datacut->get_userMax());
                g_residualsra->set_drawLine(true);
                /*if (residualStyle==0) { // draw points
                    // always draw as lines
                    g_residualsra->set_symbol(JKQTPplus);
                } else if (residualStyle==2) {
                    g_residualsra->set_symbol(JKQTPplus);
                }*/
                pltResiduals->addGraph(g_residualsra);
                //qDebug()<<"    i "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals histogram
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualHistogramX=0;
                size_t c_residualHistogramY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fit_stat.rminw+fit_stat.residHistWBinWidth/2.0, fit_stat.rmaxw-fit_stat.residHistWBinWidth/2.0, "residualhist_weighted_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resWHistogram, residualHistogramBins, "residualhist_weighted_y");
                } else {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fit_stat.rmin+fit_stat.residHistBinWidth/2.0, fit_stat.rmax-fit_stat.residHistBinWidth/2.0, "residualhist_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fit_stat.resHistogram, residualHistogramBins, "residualhist_y");
                }
                JKQTPbarHorizontalGraph* g_residualsHistogram=new JKQTPbarHorizontalGraph(pltResidualHistogram->get_plotter());
                g_residualsHistogram->set_xColumn(c_residualHistogramX);
                g_residualsHistogram->set_yColumn(c_residualHistogramY);
                g_residualsHistogram->set_shift(0);
                g_residualsHistogram->set_width(1.0);
                pltResidualHistogram->addGraph(g_residualsHistogram);
                //qDebug()<<"    j "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // plot residuals correlations
                /////////////////////////////////////////////////////////////////////////////////
                size_t c_residualCorrelationX=dsresc->addLinearColumn(fit_stat.resN-1, 1, fit_stat.resN-1, "residualcorr_x");
                size_t c_residualCorrelationY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualCorrelationY=dsresc->addCopiedColumn(&(fit_stat.resWCorrelation[1]), fit_stat.resN-1, "residualcorr_weighted_y");
                } else {
                    c_residualCorrelationY=dsresh->addCopiedColumn(&(fit_stat.resCorrelation[1]), fit_stat.resN-1, "residualcorr_y");
                }
                JKQTPxyLineGraph* g_residualsCorrelation=new JKQTPxyLineGraph(pltResidualCorrelation->get_plotter());
                g_residualsCorrelation->set_xColumn(c_residualCorrelationX);
                g_residualsCorrelation->set_yColumn(c_residualCorrelationY);
                pltResidualCorrelation->addGraph(g_residualsCorrelation);
                //qDebug()<<"    k "<<t.elapsed()<<" ms";
                t.start();



                /////////////////////////////////////////////////////////////////////////////////
                // plot distribution
                /////////////////////////////////////////////////////////////////////////////////
                QVector<double> mem_tau=eval->getDistributionTaus(record, index, model);
                QVector<double> mem_dist=eval->getDistribution(record, index, model);
                int c_disttau=-1;
                int c_dist=-1;
                if (mem_tau.size()>0 && mem_dist.size()>0) {
                    c_disttau=dsdist->addCopiedColumn(mem_tau.data(), mem_tau.size(), "maxent_tau");
                    c_dist=dsdist->addCopiedColumn(mem_dist.data(), mem_dist.size(), "maxent_tau");;
                } else {
                    pltDistribution->setXY(pltData->getXMin(), pltData->getXMax(), pltData->getYMin(), pltData->getYMax());
                }
                JKQTPxyLineGraph* g_dist=new JKQTPxyLineGraph(pltDistribution->get_plotter());
                g_dist->set_drawLine(true);
                g_dist->set_title("MaxEnt distribution");
                g_dist->set_xColumn(c_disttau);
                g_dist->set_yColumn(c_dist);
                pltDistribution->addGraph(g_dist);
                //qDebug()<<"    g "<<t.elapsed()<<" ms";
                t.start();




                /////////////////////////////////////////////////////////////////////////////////
                // update display of fit results
                /////////////////////////////////////////////////////////////////////////////////
                QString txtFit="<font face=\"Arial\">";
                QString fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_messageHTML");
                if (fitResult.isEmpty()) fitResult=record->resultsGetAsString(eval->getEvaluationResultID(), "fitalg_message");

                if (!fitResult.isEmpty()) {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>%1</center></div><br>").arg(fitResult);
                } else {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>not fit yet</center></div><br>");
                }
                txtFit+=QString("<b>%1</b><cebter>").arg(tr("Fit Statistics:"));
                txtFit+=QString("<table border=\"0\" width=\"95%\">");
                //txtFit+=QString("<tr><td align=\"right\"></td><td align=\"left\"></td><td align=\"right\"></td><td align=\"left\"></td></tr>");
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residSqrSum).arg(fit_stat.residWeightSqrSum);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&lang;E&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &lang;E&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residAverage).arg(fit_stat.residWeightAverage);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                                "<td align=\"right\" valign=\"bottom\"> &radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.residStdDev).arg(fit_stat.residWeightStdDev);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">NP =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">NR =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.fitparamN).arg(fit_stat.dataSize);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">DF =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\"></td><td align=\"left\" valign=\"bottom\"></td>"
                                "</tr>").arg(fit_stat.degFreedom);
                txtFit+=QString("<tr>"
                                "<td align=\"right\" valign=\"bottom\">TSS  =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                                "<td></td>"
                                "<td align=\"right\" valign=\"bottom\">R<sup>2</sup> =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                                "</tr>").arg(fit_stat.TSS).arg(fit_stat.Rsquared);
                //qDebug()<<"    l "<<t.elapsed()<<" ms";
                t.start();

                txtFit+=QString("</table><br><font size=\"-1\"><i>Legend:</i>: &chi;<sup>2</sup>: sum error square, &lang;E&rang;: residual average, &radic;&lang;E2&rang;: residual stddev., <br>NP: number of fit parameters, NR: number of residuals, <br>DF: degrees of freedom, R<sup>2</sup>: coefficient of determination, <br>TSS: total sum of squares</font>");
                txtFit+=QString("</center></font>");
                fitStatisticsReport=txtFit;
                txtFitStatistics->setHtml(txtFit);

                //qDebug()<<"    m_presignals "<<t.elapsed()<<" ms";
                eval->set_doEmitPropertiesChanged(true);
                eval->set_doEmitResultsChanged(true);
                //record->enableEmitResultsChanged();

                //qDebug()<<"    m "<<t.elapsed()<<" ms";
                t.start();


                /////////////////////////////////////////////////////////////////////////////////
                // clean memory
                /////////////////////////////////////////////////////////////////////////////////
                free(weights);
                fit_stat.free();

                //qDebug()<<"    n "<<t.elapsed()<<" ms";
                t.start();
            }
        }
    } catch(std::exception& E) {
        services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
    }
    //setUpdatesEnabled(updEn);
    //qDebug()<<"    updateFitFunctions end   runtime = "<<t1.elapsed()<<"ms";

}

void QFFCSMaxEntEvaluationEditor::displayParameters() {
    QFFCSByIndexAndModelEvaluationEditor::displayParameters();
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    QFSimpleFitParameterEnumeratorInterface* peval=qobject_cast<QFSimpleFitParameterEnumeratorInterface*>(current);
    if ((!record)||(!eval)/*||(!data)*/) return;

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;
    edtNdist->setValue(eval->getNdist());
    edtNumIter->setValue(eval->getNumIter());
    dataEventsEnabled=oldde;


    if (eval->hasResults(record)) {
        datacut->setEnabled(false);
        edtNdist->setEnabled(false);
    } else {
        datacut->setEnabled(true);
        edtNdist->setEnabled(true);
    }

}




void QFFCSMaxEntEvaluationEditor::fitCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;



    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));

    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;

    // here we call doEvaluation to execute our evaluation for the current record only
    eval->doFit(record, eval->getCurrentIndex(), eval->getCurrentModel(), getUserMin(record, eval->getCurrentIndex()), getUserMax(record, eval->getCurrentIndex()), 11, spinResidualHistogramBins->value());    
    dataEventsEnabled=oldde;
    eval->emitResultsChanged(record);
    record->emitResultsChanged();
    ///////////
    qDebug()<< "The Ndist value after the doFit method has been called";
    qDebug()<< eval->getNdist();
    ////////////
    displayParameters();
    displayData();
    dlgEvaluationProgress->setValue(100);

    QApplication::restoreOverrideCursor();
}


void QFFCSMaxEntEvaluationEditor::fitRunsCurrent() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;

    // iterate through all records and all runs therein and do the fits
    //for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=eval->getHighlightedRecord();
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            dlgEvaluationProgress->setRange(0,data->getCorrelationRuns());
            for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
                dlgEvaluationProgress->setValue(idx);
            }
        }
        record->emitResultsChanged();
        QApplication::processEvents();
    //}
    dataEventsEnabled=oldde;
    eval->emitResultsChanged(record);
    dlgEvaluationProgress->setValue(recs.size());
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}


void QFFCSMaxEntEvaluationEditor::fitAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            }
            record->emitResultsChanged();
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dataEventsEnabled=oldde;
    eval->emitResultsChanged();
    dlgEvaluationProgress->setValue(recs.size());
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFFCSMaxEntEvaluationEditor::fitRunsAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool oldde=dataEventsEnabled;
    dataEventsEnabled=false;


    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
        QApplication::processEvents();
        if ((record)&&(data)) {
            // here we call doEvaluation to execute our evaluation for the current record only

            //for (int idx=-1; idx<data->getCorrelationRuns(); idx++) {
                int idx=eval->getCurrentIndex();
                dlgEvaluationProgress->setLabelText(tr("evaluate '%1', run %2 ...").arg(record->getName()).arg(idx));
                QApplication::processEvents();
                eval->doFit(record, idx, eval->getCurrentModel(), getUserMin(record, idx), getUserMax(record, idx), 11, spinResidualHistogramBins->value());
                QApplication::processEvents();
                // check whether the user canceled this evaluation
                if (dlgEvaluationProgress->wasCanceled()) break;
            //}
                record->emitResultsChanged();
        }
        dlgEvaluationProgress->setValue(i);
        QApplication::processEvents();
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dataEventsEnabled=oldde;
    eval->emitResultsChanged();
    dlgEvaluationProgress->setValue(recs.size());
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCSMaxEntEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCSMaxEntEvaluationItem* eval=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    // we use this QTextCursor to write the document
    QTextCursor cursor(document);
    
    // here we define some generic formats
    QTextCharFormat fText=cursor.charFormat();
    fText.setFontPointSize(8);
    QTextCharFormat fTextSmall=fText;
    fTextSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fTextBold=fText;
    fTextBold.setFontWeight(QFont::Bold);
    QTextCharFormat fTextBoldSmall=fTextBold;
    fTextBoldSmall.setFontPointSize(0.85*fText.fontPointSize());
    QTextCharFormat fHeading1=fText;
    QTextBlockFormat bfLeft;
    bfLeft.setAlignment(Qt::AlignLeft);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);

    
    // insert heading
    cursor.insertText(tr("Evaluation Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    // insert table with some data
    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(2, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(QString::number(record->getID()));
    cursor.movePosition(QTextCursor::End);

}




void QFFCSMaxEntEvaluationEditor::alphaChanged(double alpha) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setAlpha(alpha);
}

void QFFCSMaxEntEvaluationEditor::NdistChanged(int Ndist) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setNdist(Ndist);
}

void QFFCSMaxEntEvaluationEditor::NumIterChanged(int NumIter) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    if (data) data->setNumIter(NumIter);
}

void QFFCSMaxEntEvaluationEditor::weightsChanged(int weights) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCSMaxEntEvaluationItem* data=qobject_cast<QFFCSMaxEntEvaluationItem*>(current);
    data->setCurrentWeights(weights);
    displayParameters();
    displayData();
    QApplication::restoreOverrideCursor();
}
