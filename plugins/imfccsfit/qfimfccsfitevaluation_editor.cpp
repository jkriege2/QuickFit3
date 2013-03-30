#include "qfimfccsfitevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfimfccsfitevaluation_item.h"
#include "ui_qfimfccsfitevaluation_editor.h"
#include "qffitalgorithm.h"
#include "qffitalgorithmmanager.h"
#include "qfimfccsparameterinputdelegate.h"
#include <QtGui>
#include <QtCore>
#include "tools.h"


QFImFCCSFitEvaluationEditor::QFImFCCSFitEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFImFCCSFitEvaluationEditor)
{
    updatingData=true;
    
    currentSaveDirectory="";
    
    // setup widgets
    ui->setupUi(this);
    ui->pltOverview->setRunSelectWidgetActive(true);
    connect(ui->pltOverview, SIGNAL(currentRunChanged(int)), this, SLOT(setCurrentRun(int)));
    ui->splitter->setChildrenCollapsible(false);
    ui->splitter->setStretchFactor(0,3);
    ui->splitter->setStretchFactor(1,1);
    QList<int> l;
    l<<3*ui->splitter->height()/4;
    l<<ui->splitter->height()/4;
    ui->splitter->setSizes(l);



    ui->tableView->setModel(NULL);
    ui->tableView->setItemDelegate(new QFImFCCSParameterInputDelegate(ui->tableView));
    ui->tableView->setTextElideMode(Qt::ElideMiddle);



    // set correlation/residual plots:
    ui->pltData->get_plotter()->set_gridPrinting(true);
    ui->pltData->get_plotter()->addGridPrintingPlotter(0,1,ui->pltResiduals->get_plotter());
    ui->pltData->set_displayToolbar(true);
    ui->pltData->set_toolbarAlwaysOn(true);
    ui->pltData->getXAxis()->set_logAxis(true);

    ui->pltResiduals->set_displayToolbar(false);
    ui->pltResiduals->getXAxis()->set_axisLabel(tr("lag time $\\tau$ [seconds]"));
    ui->pltResiduals->getXAxis()->set_labelFontSize(11);
    ui->pltResiduals->getXAxis()->set_tickLabelFontSize(10);
    ui->pltResiduals->getXAxis()->set_logAxis(true);
    ui->pltResiduals->getYAxis()->set_axisLabel(tr("residuals"));
    ui->pltResiduals->getYAxis()->set_labelFontSize(11);
    ui->pltResiduals->getYAxis()->set_tickLabelFontSize(10);
    ui->pltData->getXAxis()->set_axisLabel("");
    ui->pltData->getYAxis()->set_axisLabel(tr("correlation function $g(\\tau)$"));
    ui->pltData->getYAxis()->set_labelFontSize(11);
    ui->pltData->getYAxis()->set_tickLabelFontSize(10);
    ui->pltData->getXAxis()->set_drawMode1(JKQTPCADMticks);
    ui->pltData->getXAxis()->set_tickLabelFontSize(10);
    ui->pltResiduals->getXAxis()->set_drawMode1(JKQTPCADMcomplete);
    ui->pltResiduals->get_plotter()->setBorder(1,1,1,1);
    ui->pltData->get_plotter()->setBorder(1,1,1,1);
    ui->pltResiduals->synchronizeToMaster(ui->pltData, true, false);
    ui->pltData->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltData->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltResiduals->get_plotter()->set_useAntiAliasingForSystem(true);
    ui->pltResiduals->get_plotter()->set_useAntiAliasingForGraphs(true);
    ui->pltResiduals->set_displayMousePosition(false);
    ui->pltData->set_displayMousePosition(false);
    ui->pltData->get_plotter()->set_keyFontSize(9);
    ui->pltData->get_plotter()->set_keyXMargin(2);
    ui->pltData->get_plotter()->set_keyYMargin(2);
    ui->pltResiduals->get_plotter()->set_keyFontSize(9);
    ui->pltResiduals->get_plotter()->set_keyXMargin(2);
    ui->pltResiduals->get_plotter()->set_keyYMargin(2);
    ui->pltResiduals->useExternalDatastore(ui->pltData->getDatastore());
    ui->pltResiduals->setMinimumHeight(75);
    ui->pltData->setMinimumHeight(75);
    connect(ui->pltData, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMoved(double,double)));
    connect(ui->pltResiduals, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMoved(double,double)));
    connect(ui->pltData->get_plotter()->get_actZoomAll(), SIGNAL(triggered()), ui->pltResiduals, SLOT(zoomToFit()));



    
    // create progress dialog for evaluation
    dlgEvaluationProgress=new QProgressDialog(NULL);
    dlgEvaluationProgress->hide();
    dlgEvaluationProgress->setWindowModality(Qt::WindowModal);
    
    // connect widgets 
    connect(ui->btnEvaluateAll, SIGNAL(clicked()), this, SLOT(evaluateAll()));
    connect(ui->btnEvaluateCurrent, SIGNAL(clicked()), this, SLOT(evaluateCurrent()));
    
    updatingData=false;
}

QFImFCCSFitEvaluationEditor::~QFImFCCSFitEvaluationEditor()
{
    delete ui;
    delete dlgEvaluationProgress;
}

void QFImFCCSFitEvaluationEditor::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender) {
    if (sender==ui->pltData) {
        ui->pltResiduals->setX(newxmin, newxmax);
        ui->pltResiduals->update_plot();
    }
}

void QFImFCCSFitEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFImFCCSFitEvaluationItem* item=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    QFImFCCSFitEvaluationItem* item_old=qobject_cast<QFImFCCSFitEvaluationItem*>(old);
    if (ui->tableView->model()) {
        disconnect(ui->tableView->model(), 0, this, 0);
    }
    ui->tableView->setModel(NULL);
    ui->pltOverview->setRDR(0);

    if (old!=NULL && item_old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        disconnect(ui->btnAddFile, SIGNAL(clicked()), item_old, SLOT(addFitFile()));
        disconnect(ui->btnRemoveFile, SIGNAL(clicked()), item_old, SLOT(removeFitFile()));
        disconnect(item, SIGNAL(fileChanged(int,QFRawDataRecord*)), this, SLOT(fileChanged(int,QFRawDataRecord*)));
    }



    if (item) {
        updatingData=true;

        ui->cmbFitAlgorithm->setCurrentAlgorithm(item->getFitAlgorithm()->id());
        ui->cmbWeight->setCurrentWeight(item->getFitDataWeighting());
        ui->cmbDisplayData->setCurrentIndex(item->getProperty("imFCCSFit/datadisplay", 1).toInt());
        ui->cmbErrorDisplay->setCurrentIndex(item->getProperty("imFCCSFit/errordisplay", 0).toInt());
        ui->chkGrid->setChecked(item->getProperty("imFCCSFit/grid", true).toBool());
        ui->chkKey->setChecked(item->getProperty("imFCCSFit/key", true).toBool());

        ui->tableView->setModel(item->getParameterInputTableModel());
        connect(item->getParameterInputTableModel(), SIGNAL(modelRebuilt()), this, SLOT(ensureCorrectParamaterModelDisplay()));

        /* connect widgets and fill with data from item here */
        connect(item, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        connect(ui->btnAddFile, SIGNAL(clicked()), item, SLOT(addFitFile()));
        connect(ui->btnRemoveFile, SIGNAL(clicked()), item, SLOT(removeFitFile()));
        connect(item, SIGNAL(fileChanged(int,QFRawDataRecord*)), this, SLOT(fileChanged(int,QFRawDataRecord*)));
        ui->pltOverview->setRDR(item->getFitFile(0));
        updatingData=false;
    }

    ensureCorrectParamaterModelDisplay();
    displayData();
    displayEvaluation();
}

void QFImFCCSFitEvaluationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    ensureCorrectParamaterModelDisplay();
    displayData();
    displayEvaluation();    
}

void QFImFCCSFitEvaluationEditor::readSettings() {
    // read widget settings
    if (!settings) return;
    currentSaveDirectory=settings->getQSettings()->value(QString("imfccs_fit/editor/lastSaveDirectory"), currentSaveDirectory).toString();
    if (current) {
        loadSplitter(*(settings->getQSettings()), ui->splitter, QString("QFImFCCSFitEvaluationEditor%1/splitter_1").arg(current->getID()));
        loadSplitter(*(settings->getQSettings()), ui->splitter_2, QString("QFImFCCSFitEvaluationEditor%1/splitter_2").arg(current->getID()));
        loadSplitter(*(settings->getQSettings()), ui->splitter_3, QString("QFImFCCSFitEvaluationEditor%1/splitter_3").arg(current->getID()));
    }
}

void QFImFCCSFitEvaluationEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("imfccs_fit/editor/lastSaveDirectory"), currentSaveDirectory);
    if (current) {
        saveSplitter(*(settings->getQSettings()), ui->splitter, QString("QFImFCCSFitEvaluationEditor%1/splitter_1").arg(current->getID()));
        saveSplitter(*(settings->getQSettings()), ui->splitter_2, QString("QFImFCCSFitEvaluationEditor%1/splitter_2").arg(current->getID()));
        saveSplitter(*(settings->getQSettings()), ui->splitter_3, QString("QFImFCCSFitEvaluationEditor%1/splitter_3").arg(current->getID()));
    }
}

void QFImFCCSFitEvaluationEditor::showFitAlgorithmHelp()
{
    if (!current) return;
    QFImFCCSFitEvaluationItem* data=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();
    int ppid=services->getFitAlgorithmManager()->getPluginForID(pid);
    QFFitAlgorithm* algorithm=data->getFitAlgorithm(pid);
    QString help=services->getFitAlgorithmManager()->getPluginHelp(ppid, pid);
    if (QFile::exists(help) && algorithm) {
        QFPluginServices::getInstance()->displayHelpWindow(help);
    } else {
        QMessageBox::information(this, tr("imFCCS Fit"), tr("No Online-Help for this fit algorithm available."));
    }
}

void QFImFCCSFitEvaluationEditor::configFitAlgorithm()
{
    if (!current) return;
    QFImFCCSFitEvaluationItem* data=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    if (!data) return;
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();
    QFFitAlgorithm* algorithm=data->getFitAlgorithm(pid);
    if (algorithm) {
        data->restoreQFFitAlgorithmParameters(algorithm);
        if (algorithm->displayConfig()) {
            data->storeQFFitAlgorithmParameters(algorithm);
        }
    }
}

void QFImFCCSFitEvaluationEditor::fitAlgorithmChanged(int model)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();
    QFImFCCSFitEvaluationItem* data=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    data->setFitAlgorithm(pid);
    /*populateFitButtons(data->getFitAlgorithm()->isThreadSafe());
    actFitAllThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());
    actFitAllFilesThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());
    actFitAllRunsThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());*/
    QApplication::restoreOverrideCursor();
}

void QFImFCCSFitEvaluationEditor::displayOptionsChanged()
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();

    current->setQFProperty("imFCCSFit/datadisplay", ui->cmbDisplayData->currentIndex());
    current->setQFProperty("imFCCSFit/errordisplay",ui->cmbErrorDisplay->currentIndex());
    current->setQFProperty("imFCCSFit/grid",ui->chkGrid->isChecked());
    current->setQFProperty("imFCCSFit/key",ui->chkKey->isChecked());

    displayData();
    QApplication::restoreOverrideCursor();
}

void QFImFCCSFitEvaluationEditor::setCurrentRun(int run)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFImFCCSFitEvaluationItem* data=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    qDebug()<<data->getCurrentIndex();
    data->setCurrentIndex(run);
    qDebug()<<run<<data->getCurrentIndex();
    displayData();
    displayEvaluation();
    QApplication::restoreOverrideCursor();
}

void QFImFCCSFitEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    // this slot is called when the user selects a new record in the raw data record list on the RHS of this widget in the evaluation dialog
    
    QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    QString resultID=QString(current->getType()+QString::number(current->getID())).toLower();
    QFRawDataRecord* data=currentRecord; // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(currentRecord);
    disconnect(formerRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData()));

    if (data) { // if we have a valid object, update
        connect(currentRecord, SIGNAL(rawDataChanged()), this, SLOT(displayData())); // redisplay data, if data changed

        updatingData=true;
        // assign values to widgets here 
        ui->pltOverview->setRDR(currentRecord);
        updatingData=false;
    }
    
    // ensure that data of new highlighted record is displayed
    displayEvaluation();
    displayData();
}

void QFImFCCSFitEvaluationEditor::ensureCorrectParamaterModelDisplay()
{
    QElapsedTimer t;
    t.start();
    //qDebug()<<"ensureCorrectParamaterModelDisplay";
    QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    setUpdatesEnabled(false);
    ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QHeaderView* h=ui->tableView->verticalHeader();
    if (h) {
        h->setResizeMode(QHeaderView::Fixed);
        QFontMetrics fm(h->font());
        int height=qMax(16, (int)ceil(double(fm.height())*1.5));
        //qDebug()<<"resizing verticals to "<<height;
        for (int i=0; i<h->count(); i++) {
            h->resizeSection(i, height);
        }
    }
    h=ui->tableView->horizontalHeader();
   if (h) {
       int width=qMax(200, ui->tableView->width()/4);
       if (eval) width=qMax(width, ui->tableView->width()/eval->getParameterInputTableModel()->columnCount());
       //ui->tableView->resizeColumnsToContents();
       for (int i=0; i<h->count(); i++) {
           if (i==0) {
               //qDebug()<<"  resizing h"<<i<<" to contents "<<t.elapsed()<<"ms";
               ui->tableView->resizeColumnToContents(i);
               h->setResizeMode(i, QHeaderView::Interactive);
               //qDebug()<<"  resizing h"<<i<<" to contents done "<<t.elapsed()<<"ms";
           }
           //h->setResizeMode(i, QHeaderView::Interactive);
           if (i>0 && eval) {
               int ii=(i-1)%(eval->getParameterInputTableModel()->getColsPerRDR());
               //qDebug()<<"  resizing h"<<i<<" to contents "<<t.elapsed()<<"ms";
               if (ii==0) h->resizeSection(i, 250);
               else if (ii==1) h->resizeSection(i, 75);
               else if (ii==2) h->resizeSection(i, 50);
               else if (ii==3) h->resizeSection(i, 24);
               else if (ii==4) h->resizeSection(i, 75);
               //qDebug()<<"  resizing h"<<i<<" to contents done "<<t.elapsed()<<"ms";
               //h->setResizeMode(i, QHeaderView::Interactive);
               /*
               if (ii==3)  h->resizeSection(i, 20);
               else if (ii==0 && ui->tableView->columnWidth(i)>2*ui->tableView->width()/3) h->resizeSection(i, qMax(200, 2*ui->tableView->width()/3));
               else if (ii==0 && ui->tableView->columnWidth(i)<200) h->resizeSection(i, 200);*/

               //qDebug()<<"resizeH: ii="<<i<<" width="<<width<<"  files="<<eval->getNumberOfFitFiles();
           }
       }
   }
   setUpdatesEnabled(true);
   //qDebug()<<"ensureCorrectParamaterModelDisplay done "<<t.elapsed()<<"ms";

}

void QFImFCCSFitEvaluationEditor::fileChanged(int num, QFRawDataRecord *file)
{
    if (num==0) ui->pltOverview->setRDR(file);
}

void QFImFCCSFitEvaluationEditor::plotMouseMoved(double x, double y)
{
    ui->labPosition->setText(tr("current position: (%1s, %2)").arg(floattohtmlstr(x, 3, true, 1e-18).c_str()).arg(y));
}

void QFImFCCSFitEvaluationEditor::displayEvaluation() {
}

void QFImFCCSFitEvaluationEditor::displayData() {
    if (!current) return;
    QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    JKQTPdatastore* ds=ui->pltData->getDatastore();
    ui->pltData->set_doDrawing(false);
    ui->pltResiduals->set_doDrawing(false);
    ui->pltData->clearGraphs();
    ui->pltResiduals->clearGraphs();
    ui->pltData->get_plotter()->set_showKey(ui->chkKey->isChecked());
    ui->pltResiduals->get_plotter()->set_showKey(ui->chkKey->isChecked());
    ui->pltData->get_plotter()->get_xAxis()->set_drawGrid(ui->chkGrid->isChecked());
    ui->pltData->get_plotter()->get_yAxis()->set_drawGrid(ui->chkGrid->isChecked());
    ui->pltResiduals->get_plotter()->get_xAxis()->set_drawGrid(ui->chkGrid->isChecked());
    ui->pltResiduals->get_plotter()->get_yAxis()->set_drawGrid(ui->chkGrid->isChecked());
    QList<QColor> cols;
    cols<<QColor("darkgreen")<<QColor("red")<<QColor("blue")<<QColor("darkorange")<<QColor("purple")<<QColor("maroon");
    if (eval) {
        for (int file=0; file<eval->getNumberOfFitFiles(); file++) {
            QFFitFunction* ff=eval->getFitFunction(file);
            QFRawDataRecord* rec=eval->getFitFile(file);

            QFRDRFCSDataInterface* fcs=dynamic_cast<QFRDRFCSDataInterface*>(rec);
            QFRDRImageToRunInterface* runintf=dynamic_cast<QFRDRImageToRunInterface*>(rec);

            double* data=fcs->getCorrelationRun(eval->getCurrentIndex());
            double* tau=fcs->getCorrelationT();
            double* sigma=fcs->getCorrelationRunError(eval->getCurrentIndex());
            long N=fcs->getCorrelationN();
            if (data && tau) {
                int c_tau=ds->addCopiedColumn(tau, N, tr("file%1: tau [s]").arg(file+1));
                int c_data=ds->addCopiedColumn(data, N, tr("file%1: g(tau)").arg(file+1));
                int c_error=-1;
                if (sigma) c_error=c_data=ds->addCopiedColumn(sigma, N, tr("file%1: errors").arg(file+1));
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(ui->pltData->get_plotter());
                QString plotname=QString("\\verb{")+rec->getName()+QString(": ")+fcs->getCorrelationRunName(eval->getCurrentIndex())+QString("}");
                g->set_title(plotname);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_data);
                g->set_yErrorColumn(c_error);
                g->set_color(cols.value(file, g->get_color()));
                QColor ec=g->get_color().lighter();
                g->set_errorColor(ec);
                ec.setAlphaF(0.5);
                g->set_errorFillColor(ec);
                g->set_symbol(ui->cmbDisplayData->getSymbol());
                g->set_drawLine(ui->cmbDisplayData->getDrawLine());
                g->set_yErrorStyle(ui->cmbErrorDisplay->getErrorStyle());
                ui->pltData->get_plotter()->addGraph(g);

                double* params=eval->allocFillParameters(rec, eval->getCurrentIndex(), ff);
                double* err=eval->allocFillParameterErrors(rec, eval->getCurrentIndex(), ff);
                bool* fix=eval->allocFillFix(rec, eval->getCurrentIndex(), ff);
                QVector<double> paramsV;
                for (int i=0; i<ff->paramCount(); i++) {
                    paramsV<<params[i];
                }
                JKQTPxQFFitFunctionLineGraph* g_fit=new JKQTPxQFFitFunctionLineGraph();
                g_fit->set_title(tr("fit: %1").arg(plotname));
                g_fit->set_fitFunction(ff, false);
                g_fit->set_paramsVector(paramsV);
                g_fit->set_color(g->get_color());
                g_fit->set_style(Qt::DashLine);
                g_fit->set_lineWidth(1);
                ui->pltData->get_plotter()->addGraph(g_fit);

                QFFitStatistics fstat=ff->calcFitStatistics(N, tau, data, sigma, 0, N-1, params, err, fix, 3, 100);

                int c_resid=ds->addCopiedColumn(fstat.residuals, N, tr("file%1: residuals").arg(file+1));
                int c_residw=ds->addCopiedColumn(fstat.residuals_weighted, N, tr("file%1: weighted residuals").arg(file+1));
                int c_residat=ds->addCopiedColumn(fstat.tau_runavg, fstat.runAvgN, tr("file%1: tau for avg. res.").arg(file+1));
                int c_resida=ds->addCopiedColumn(fstat.residuals_runavg, N, tr("file%1: avg. residuals").arg(file+1));
                int c_residaw=ds->addCopiedColumn(fstat.residuals_runavg_weighted, N, tr("file%1: weighted avg. residuals").arg(file+1));
                JKQTPxyLineGraph* g_res=new JKQTPxyLineGraph();
                g_res->set_xColumn(c_tau);
                g_res->set_yColumn(c_resid);
                g_res->set_color(g->get_color());
                g_res->set_symbol(ui->cmbDisplayData->getSymbol());
                g_res->set_drawLine(ui->cmbDisplayData->getDrawLine());
                ui->pltResiduals->get_plotter()->addGraph(g_res);
                JKQTPxyLineGraph* g_resa=new JKQTPxyLineGraph();
                g_resa->set_xColumn(c_residat);
                g_resa->set_yColumn(c_resida);
                g_resa->set_color(g->get_color());
                g_resa->set_drawLine(true);
                g_resa->set_symbol(JKQTPnoSymbol);
                ui->pltResiduals->get_plotter()->addGraph(g_resa);


                free(params);
                free(err);
                free(fix);
            }

            ui->pltData->zoomToFit();
            ui->pltResiduals->zoomToFit();
        }

    }
    ui->pltData->set_doDrawing(true);
    ui->pltResiduals->set_doDrawing(true);
    ui->pltData->update_plot();
    ui->pltResiduals->update_plot();
    QApplication::restoreOverrideCursor();
}


void QFImFCCSFitEvaluationEditor::doEvaluation(QFRawDataRecord* record) {
    QApplication::processEvents();
    QApplication::processEvents();

    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record); //if (!data) return;
    QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);

    if (!eval) return;
    
    if (dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?
    
    /*
        DO YOUR EVALUATION HERE
    */

    services->log_text(tr("evaluation complete\n"));
    
    // write back fit results to record!
    record->disableEmitResultsChanged();
    record->resultsSetBoolean(eval->getEvaluationResultID(), "evaluation_completed", true);
    record->enableEmitResultsChanged();
    emit resultsChanged();
}




void QFImFCCSFitEvaluationEditor::evaluateCurrent() {
    /* EXECUTE AN EVALUATION FOR THE CURRENT RECORD ONLY */
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    if ((!eval)||(!record)/*||(!data)*/) return;

    
    
    dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
    
    dlgEvaluationProgress->setRange(0,100);
    dlgEvaluationProgress->setValue(50);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // here we call doEvaluation to execute our evaluation for the current record only
    doEvaluation(record);

    displayEvaluation();
    displayData();
    dlgEvaluationProgress->setValue(100);

    QApplication::restoreOverrideCursor();
}


void QFImFCCSFitEvaluationEditor::evaluateAll() {
    /* EXECUTE AN EVALUATION FOR ALL RECORDS */
    if (!current) return;

    QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    if (!eval) return;

    // get a list of all raw data records this evaluation is applicable to
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    dlgEvaluationProgress->setRange(0,recs.size());
    dlgEvaluationProgress->setValue(0);
    dlgEvaluationProgress->open();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // iterate through all records and all runs therein and do the fits
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i]; 
        // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
        QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
        if ((record)/*&&(data)*/) {
            dlgEvaluationProgress->setLabelText(tr("evaluate '%1' ...").arg(record->getName()));
            // here we call doEvaluation to execute our evaluation for the current record only
            doEvaluation(record);
        }
        dlgEvaluationProgress->setValue(i);
        // check whether the user canceled this evaluation
        if (dlgEvaluationProgress->wasCanceled()) break;
    }
    dlgEvaluationProgress->setValue(recs.size());
    displayEvaluation();
    displayData();
    QApplication::restoreOverrideCursor();
}



void QFImFCCSFitEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    QFRawDataRecord* record=current->getHighlightedRecord(); 
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFImFCCSFitEvaluationItem* eval=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
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

void QFImFCCSFitEvaluationEditor::saveReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                currentSaveDirectory,
                                tr("PDF File (*.pdf);;PostScript File (*.ps)"));

    if (!fn.isEmpty()) {
        currentSaveDirectory=QFileInfo(fn).absolutePath();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter();
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(15,15,15,15,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setOutputFormat(QPrinter::PdfFormat);
        if (fi.suffix().toLower()=="ps") printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setOutputFileName(fn);
        QTextDocument* doc=new QTextDocument();
        doc->setTextWidth(printer->pageRect().size().width());
        createReportDoc(doc);
        doc->print(printer);
        delete doc;
        delete printer;
        QApplication::restoreOverrideCursor();
    }
}

void QFImFCCSFitEvaluationEditor::printReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTextDocument* doc=new QTextDocument();
    doc->setTextWidth(p->pageRect().size().width());
    createReportDoc(doc);
    doc->print(p);
    delete p;
    delete doc;
    QApplication::restoreOverrideCursor();
}

void QFImFCCSFitEvaluationEditor::on_cmbWeight_currentWeightChanged(QFFCSWeightingTools::DataWeight weight)
{
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();
    current->getHighlightedRecord()->setQFProperty("algorithm", pid, false, false);
    QFImFCCSFitEvaluationItem* data=qobject_cast<QFImFCCSFitEvaluationItem*>(current);
    data->setFitDataWeighting(weight);
    QApplication::restoreOverrideCursor();
}

