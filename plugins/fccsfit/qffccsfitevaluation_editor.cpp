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

#include "qffccsfitevaluation_editor.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qffccsfitevaluation_item.h"
#include "ui_qffccsfitevaluation_editor.h"
#include "qffitalgorithm.h"
#include "qffitalgorithmmanager.h"
#include "qffccsparameterinputdelegate.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtCore>
#include "cpptools.h"
#include "qmoretextobject.h"
#include "dlgqfprogressdialog.h"
#include "qffitfunctionmanager.h"
#include "dlgfccsfitallconfiguration.h"
#include "qffitfunctionconfigforglobalfittools.h"
#include "dlgfccsresetsetparameter.h"

QFFCCSFitEvaluationEditor::QFFCCSFitEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent):
    QFEvaluationEditor(services, propEditor, parent),
    ui(new Ui::QFFCCSFitEvaluationEditor)
{
    updatingData=true;
    
    currentSaveDirectory="";
    

    // setup fit progress dialog
    dlgFitProgress = new dlgQFFitAlgorithmProgressDialog(this);
    dlgFitProgressReporter=new dlgQFFitAlgorithmProgressDialogReporter(dlgFitProgress);

    // setup widgets
    ui->setupUi(this);
    connect(ui->pltOverview, SIGNAL(currentRunChanged(int)), this, SLOT(setCurrentRun(int)));
    ui->datacut->set_allowCopyToAll(true);
    ui->datacut->set_copyToFilesEnabled(true);
    ui->datacut->set_copyToRunsEnabled(false);
    ui->datacut->set_runsName(tr("runs"));

    ui->cmbDisplayData->setDefaultSymbol(JKQTPcross, 5);
    ui->cmbDisplayData->addUsedSymbol(JKQTPcross, 5, true);
    ui->cmbDisplayData->addUsedSymbol(JKQTPcross, 5, false);
    ui->cmbDisplayData->addUsedSymbol(JKQTPcross, 10, true);
    ui->cmbDisplayData->addUsedSymbol(JKQTPcross, 10, false);
    ui->cmbDisplayData->addUsedSymbol(JKQTPcross, 15, true);
    ui->cmbDisplayData->addUsedSymbol(JKQTPcross, 15, false);
    ui->cmbDisplayData->addUsedSymbol(JKQTPfilledCircle, 5, true);
    ui->cmbDisplayData->addUsedSymbol(JKQTPfilledCircle, 5, false);
    ui->cmbDisplayData->addUsedSymbol(JKQTPfilledCircle, 10, true);
    ui->cmbDisplayData->addUsedSymbol(JKQTPfilledCircle, 10, false);
    ui->cmbDisplayData->addUsedSymbol(JKQTPfilledCircle, 15, true);
    ui->cmbDisplayData->addUsedSymbol(JKQTPfilledCircle, 15, false);
    ui->cmbDisplayData->addUsedSymbol(JKQTPdot, 10, false);




    ui->tableView->setModel(NULL);
    ui->tableView->setItemDelegate(new QFFCCSParameterInputDelegate(ui->tableView));
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
    //ui->pltData->get_plotter()->set_keyXMargin(2);
    //ui->pltData->get_plotter()->set_keyYMargin(2);
    ui->pltResiduals->get_plotter()->set_keyFontSize(9);
    //ui->pltResiduals->get_plotter()->set_keyXMargin(2);
    //ui->pltResiduals->get_plotter()->set_keyYMargin(2);
    ui->pltResiduals->useExternalDatastore(ui->pltData->getDatastore());
    ui->pltResiduals->setMinimumHeight(75);
    ui->pltData->setMinimumHeight(75);
    connect(ui->pltData, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMoved(double,double)));
    connect(ui->pltResiduals, SIGNAL(plotMouseMove(double,double)), this, SLOT(plotMouseMoved(double,double)));
    connect(ui->pltData->get_plotter()->get_actZoomAll(), SIGNAL(triggered()), ui->pltResiduals, SLOT(zoomToFit()));


    menuEvaluation=propEditor->addOrFindMenu(tr("&Evaluation"), 0);
    menuFCCSFit=propEditor->addOrFindMenu(tr("&Tools"), 0);

    actFitCurrent=new QAction(QIcon(":/fccs_fit/fit_fitcurrent.png"), tr("Fit &Current"), this);
    connect(actFitCurrent, SIGNAL(triggered()), this, SLOT(fitCurrent()));
    ui->btnEvaluateCurrent->setDefaultAction(actFitCurrent);
    menuEvaluation->addAction(actFitCurrent);

    actFitAllRunsMT=new QAction(QIcon(":/fccs_fit/fit_fitallruns.png"), tr("Fit &All Runs (MT)"), this);
    connect(actFitAllRunsMT, SIGNAL(triggered()), this, SLOT(fitAllRunsThreaded()));
    actFitAllRunsMT->setEnabled(false);
    ui->btnEvaluateCurrentAllRuns->addAction(actFitAllRunsMT);
    menuEvaluation->addAction(actFitAllRunsMT);

    actFitAllRuns=new QAction(QIcon(":/fccs_fit/fit_fitallruns.png"), tr("Fit &All Runs"), this);
    connect(actFitAllRuns, SIGNAL(triggered()), this, SLOT(fitAllRuns()));
    ui->btnEvaluateCurrentAllRuns->addAction(actFitAllRuns);
    ui->btnEvaluateCurrentAllRuns->setDefaultAction(actFitAllRuns);
    menuEvaluation->addAction(actFitAllRuns);


    actFitAllFilesetsAllPixels=new QAction(QIcon(":/imfccsfit/fit_fitall.png"), tr("Fit All Filesets && Pixels"), this);
    connect(actFitAllFilesetsAllPixels, SIGNAL(triggered()), this, SLOT(fitAllFilesetsAllPixels()));
    menuEvaluation->addAction(actFitAllFilesetsAllPixels);

    actResetCurrent=new QAction(tr("&Reset Current"), this);
    actResetCurrent->setToolTip(tr("reset the currently displayed file (and runs) to the initial parameters\nThis deletes all fit results stored for the current file."));
    connect(actResetCurrent, SIGNAL(triggered()), this, SLOT(resetCurrent()));
    ui->btnClearCurrent->setDefaultAction(actResetCurrent);
    menuEvaluation->addSeparator();
    menuEvaluation->addAction(actResetCurrent);

    actResetAllPixelsInAllFilesets=new QAction(tr("Reset All &Runs in all filesets"), this);
    actResetAllPixelsInAllFilesets->setToolTip(tr("reset all runs to the initial parameters in all files fitted so far."));
    connect(actResetAllPixelsInAllFilesets, SIGNAL(triggered()), this, SLOT(resetAllPixelsInAllFilesets()));
    connect(ui->btnClearEverything, SIGNAL(clicked()), this, SLOT(resetAllPixelsInAllFilesets()));
    menuEvaluation->addAction(actResetAllPixelsInAllFilesets);

    actResetAllRuns=new QAction(tr("Reset All &Runs"), this);
    actResetAllRuns->setToolTip(tr("reset all runs to the initial parameters in the current file.\nThis deletes all fit results stored for all runs in the current file."));
    connect(actResetAllRuns, SIGNAL(triggered()), this, SLOT(resetAllPixels()));
    ui->btnClearAllPixels->setDefaultAction(actResetAllRuns);
    menuEvaluation->addAction(actResetAllRuns);

    actCopyToInitial=new QAction(tr("Copy to &Initial"), this);
    actCopyToInitial->setToolTip(tr("copy the currently displayed fit parameters to the set of initial parameters,\n so they are used by files/runs that were not fit yet."));
    connect(actCopyToInitial, SIGNAL(triggered()), this, SLOT(copyToInitial()));
    ui->btnCopyToInitial->setDefaultAction(actCopyToInitial);
    menuEvaluation->addAction(actCopyToInitial);

    actCheckFilesets=new QAction(tr("Check fitted filesets"), this);
    actCheckFilesets->setToolTip(tr("removes all fitted filesets, for which no fit results exist."));
    connect(actCheckFilesets, SIGNAL(triggered()), this, SLOT(checkFitFileSets()));
    menuEvaluation->addAction(actCheckFilesets);


    actSaveReport=new QAction(QIcon(":/fccs_fit/fit_savereport.png"), tr("&Save Report"), this);
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    ui->btnSaveReport->setDefaultAction(actSaveReport);
    menuEvaluation->addSeparator();
    menuEvaluation->addAction(actSaveReport);

    actPrintReport=new QAction(QIcon(":/fccs_fit/fit_printreport.png"), tr("&Print Report"), this);
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
    ui->btnPrintReport->setDefaultAction(actPrintReport);
    menuEvaluation->addAction(actPrintReport);

    /*QMenu* m=menuFCCSFit->addMenu(tr("configure evaluation for ..."));

    actConfigureForNormalFCCS=new QAction(tr("FCCS: normal diffusion"), this);
    connect(actConfigureForNormalFCCS, SIGNAL(triggered()), this, SLOT(configureForSPFCCS()));
    m->addAction(actConfigureForNormalFCCS);
    actConfigureFor2CFCCS=new QAction(tr("FCCS: 2-component normal diffusion"), this);
    connect(actConfigureFor2CFCCS, SIGNAL(triggered()), this, SLOT(configureFor2CSPIMFCCS()));
    m->addAction(actConfigureFor2CFCCS);

    actConfigureForAnomalousFCCS=new QAction(tr("FCCS: anomalous diffusion"), this);
    connect(actConfigureForAnomalousFCCS, SIGNAL(triggered()), this, SLOT(configureForASPFCCS()));
    m->addAction(actConfigureForAnomalousFCCS);*/

    menuFCCSFit->addSeparator();
    actGuess=new QAction(tr("Guess file sets ..."), this);
    connect(actGuess, SIGNAL(triggered()), this, SLOT(guessFromCurrentFileSet()));
    menuFCCSFit->addAction(actGuess);


    menuFCCSFit->addSeparator();
    actLoadGLobalFitConfig=new QAction(tr("&load global fit config ..."), this);
    connect(actLoadGLobalFitConfig, SIGNAL(triggered()), this, SLOT(loadGlobalFitConfig()));
    menuFCCSFit->addAction(actLoadGLobalFitConfig);
    actSaveGLobalFitConfig=new QAction(tr("&save global fit config ..."), this);
    connect(actSaveGLobalFitConfig, SIGNAL(triggered()), this, SLOT(saveGlobalFitConfig()));
    menuFCCSFit->addAction(actSaveGLobalFitConfig);

    actSetResetFitParameters=new QAction(tr("set/reset fit &parameters ..."), this);
    connect(actSetResetFitParameters, SIGNAL(triggered()), this, SLOT(setParameterInRDRs()));
    menuFCCSFit->addAction(actSetResetFitParameters);
    menuFCCSFit->addSeparator();

    
    // connect widgets 
    connect(ui->pltData, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(zoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(ui->pltResiduals, SIGNAL(zoomChangedLocally(double,double,double,double,JKQtPlotter*)), this, SLOT(zoomChangedLocally(double,double,double,double,JKQtPlotter*)));
    connect(ui->datacut, SIGNAL(copyUserMinToAll(int)), this, SLOT(slidersCopyUserMinToAll(int)));
    connect(ui->datacut, SIGNAL(copyUserMaxToAll(int)), this, SLOT(slidersCopyUserMaxToAll(int)));
    connect(ui->datacut, SIGNAL(copyUserMinMaxToAll(int,int)), this, SLOT(slidersCopyUserMinMaxToAll(int,int)));
/*    connect(ui->datacut, SIGNAL(copyUserMinToAll(int)), this, SLOT(copyUserMinToAll(int)));
    connect(ui->datacut, SIGNAL(copyUserMaxToAll(int)), this, SLOT(copyUserMaxToAll(int)));
    connect(ui->datacut, SIGNAL(copyUserMinMaxToAll(int,int)), this, SLOT(copyUserMinMaxToAll(int,int)));
    connect(ui->datacut, SIGNAL(copyUserMinToAllRuns(int)), this, SLOT(copyUserMinToAllRuns(int)));
    connect(ui->datacut, SIGNAL(copyUserMaxToAllRuns(int)), this, SLOT(copyUserMaxToAllRuns(int)));
    connect(ui->datacut, SIGNAL(copyUserMinMaxToAllRuns(int,int)), this, SLOT(copyUserMinMaxToAllRuns(int,int)));*/

    updatingData=false;
    ui->splitterPlotTable->setStretchFactor(0,1);
    ui->splitterPlotTable->setStretchFactor(1,1);
    ui->splitterPlots->setStretchFactor(0,3);
    ui->splitterPlots->setStretchFactor(1,1);
}

QFFCCSFitEvaluationEditor::~QFFCCSFitEvaluationEditor()
{
    delete ui;
}

int QFFCCSFitEvaluationEditor::getUserMin(int index)
{
    QFFCCSFitEvaluationItem* item=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!current) return 0;
    return getUserMin(item->getFitFile(0), index);
}

int QFFCCSFitEvaluationEditor::getUserMax(int index)
{
    QFFCCSFitEvaluationItem* item=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!current) return 0;
    return getUserMax(item->getFitFile(0), index);
}

int QFFCCSFitEvaluationEditor::getUserMin(QFRawDataRecord *rec, int index)
{
    return getUserMin(rec, index, ui->datacut->get_userMin());
}

int QFFCCSFitEvaluationEditor::getUserMax(QFRawDataRecord *rec, int index)
{
    return getUserMax(rec, index, ui->datacut->get_userMax());
}

void QFFCCSFitEvaluationEditor::zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender) {
    Q_UNUSED(newymin)
    Q_UNUSED(newymax)

    if (sender==ui->pltData) {
        ui->pltResiduals->setX(newxmin, newxmax);
        ui->pltResiduals->update_plot();
    }
}

void QFFCCSFitEvaluationEditor::checkFitFileSets()
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    data->checkAndCleanFitFileSets();
    QApplication::restoreOverrideCursor();

}

void QFFCCSFitEvaluationEditor::on_spinFitLocalGlobals_valueChanged(int value)
{
    if (!current) return;
    current->setQFProperty("localGlobalFitIterations", value, false, false);

}

void QFFCCSFitEvaluationEditor::on_spinFitRepeats_valueChanged(int value)
{
    if (!current) return;
    current->setQFProperty("repeatFit", value, false, false);
}

void QFFCCSFitEvaluationEditor::on_chkDontFitMasked_toggled(bool checked)
{
    if (!current) return;
    current->setQFProperty("dontFitMaskedPixels", checked, false, false);
}

void QFFCCSFitEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    // called when this widget should be connected to a new QFEvaluationItem

    QFFCCSFitEvaluationItem* item=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    QFFCCSFitEvaluationItem* item_old=qobject_cast<QFFCCSFitEvaluationItem*>(old);
    if (ui->tableView->model()) {
        disconnect(ui->tableView->model(), 0, this, 0);
    }
    ui->tableView->setModel(NULL);
    ui->pltOverview->setRDR(0);
    ui->lstFileSets->setModel(NULL);

    if (old!=NULL && item_old!=NULL) {
        /* disconnect item_old and clear all widgets here */
        disconnect(item_old, SIGNAL(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)), this, SLOT(highlightingChanged(QFRawDataRecord*, QFRawDataRecord*)));
        //disconnect(ui->btnAddFile, SIGNAL(clicked()), item_old, SLOT(addFitFile()));
        //disconnect(ui->btnRemoveFile, SIGNAL(clicked()), item_old, SLOT(removeFitFile()));
        disconnect(item, SIGNAL(fileChanged(int,QFRawDataRecord*)), this, SLOT(fileChanged(int,QFRawDataRecord*)));
        disconnect(ui->datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        disconnect(item_old->getParameterInputTableModel(), SIGNAL(fitParamChanged()), this, SLOT(displayEvaluation()));
        disconnect(item_old->getParameterInputTableModel(), SIGNAL(modelRebuilt()), this, SLOT(setParameterTableSpans()));
        disconnect(ui->widFitErrorEstimate, SIGNAL(parametersChanged()), this, SLOT(errorEstimateModeChanged()));
    }



    if (item) {
        updatingData=true;
        buildGlobalConfigs(item);
        ui->cmbFitAlgorithm->setCurrentAlgorithm(item->getFitAlgorithm()->id());
        ui->cmbWeight->setCurrentWeight(item->getFitDataWeighting());
        ui->cmbDisplayData->setCurrentIndex(item->getProperty("FCCSFit/datadisplay", 1).toInt());
        ui->cmbErrorDisplay->setCurrentIndex(item->getProperty("FCCSFit/errordisplay", 0).toInt());
        ui->chkGrid->setChecked(item->getProperty("FCCSFit/grid", true).toBool());
        ui->chkKey->setChecked(item->getProperty("FCCSFit/key", true).toBool());
        ui->chkSaveStrings->setChecked(!item->getProperty("dontSaveFitResultMessage", true).toBool());
        ui->chkDontFitMasked->setChecked(item->getProperty("dontFitMaskedPixels", true).toBool());
        ui->spinFitRepeats->setValue(item->getProperty("repeatFit", 3).toInt());
        ui->spinFitLocalGlobals->setValue(item->getProperty("localGlobalFitIterations", 0).toInt());
        ui->lstFileSets->setModel(item->getFileSetsModel());
        if (ui->lstFileSets->model()->rowCount()>0) {
            ui->lstFileSets->setCurrentIndex(ui->lstFileSets->model()->index(0,0));
            filesSetActivated(ui->lstFileSets->currentIndex());
        }
        ui->widFitErrorEstimate->readSettings(item);
        ui->tableView->setModel(item->getParameterInputTableModel());
        connect(item->getParameterInputTableModel(), SIGNAL(modelRebuilt()), this, SLOT(ensureCorrectParamaterModelDisplay()));
        setParameterTableSpans();

        /* connect widgets and fill with data from item here */
        //connect(ui->btnAddFile, SIGNAL(clicked()), item, SLOT(addFitFile()));
        //connect(ui->btnRemoveFile, SIGNAL(clicked()), item, SLOT(removeFitFile()));
        connect(item, SIGNAL(fileChanged(int,QFRawDataRecord*)), this, SLOT(fileChanged(int,QFRawDataRecord*)));
        connect(ui->datacut, SIGNAL(slidersChanged(int, int, int, int)), this, SLOT(slidersChanged(int, int, int, int)));
        connect(item->getParameterInputTableModel(), SIGNAL(fitParamChanged()), this, SLOT(displayEvaluation()));
        connect(ui->lstFileSets, SIGNAL(clicked(QModelIndex)), this, SLOT(filesSetActivated(QModelIndex)));
        connect(item->getParameterInputTableModel(), SIGNAL(modelRebuilt()), this, SLOT(setParameterTableSpans()));
        connect(ui->widFitErrorEstimate, SIGNAL(parametersChanged()), this, SLOT(errorEstimateModeChanged()));
        ui->pltOverview->setRDR(item->getFitFile(0));
        updatingData=false;
    }

    if (item && current->propertyExists("PRESET_FIT_MODEL")) {
        QString model=current->getProperty("PRESET_FIT_MODEL", "").toString();
        QStringList models=QFFitFunctionManager::getInstance()->getIDList();
        if (model.size()>0) {
            QList<QFFitFunctionConfigForGlobalFitInterface*> modelPlugins;
            for (int i=0; i<models.size(); i++) {
                QFFitFunctionConfigForGlobalFitInterface* intf=qobject_cast<QFFitFunctionConfigForGlobalFitInterface*>(QFFitFunctionManager::getInstance()->getPluginObject(QFFitFunctionManager::getInstance()->getPluginForID(models[i])));
                if (intf && !modelPlugins.contains(intf)) modelPlugins<<intf;
            }
            for (int i=0; i<modelPlugins.size(); i++) {
                QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig c=QFFitFunctionConfigForGlobalFitInterface_GlobalFitConfig_get(modelPlugins[i], model);
                if (c.shortLabel==model) {
                    configureFitFromGlobal(c, false);
                    break;
                }
            }

//
        }
        current->deleteProperty("PRESET_FIT_MODEL");
    }

    if (item && current->propertyExists("PRESET_FIT_MODELS_LIST")) {
        QString modelliststr=current->getProperty("PRESET_FIT_MODELS_LIST", "").toString();
        QString globalparamsstr=current->getProperty("PRESET_FIT_MODELS_GLOBALPARAMS_LIST", "").toString();
        QString rolesstr=current->getProperty("PRESET_FIT_MODELS_ROLES_LIST", "").toString();
        QStringList fitmodels=modelliststr.split(";");
        QStringList globalparams=globalparamsstr.split(";");
        QStringList roles=rolesstr.split(";");
        if (fitmodels.size()>0) {
            QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig c;
            c.models=fitmodels;
            c.roles=roles;
            for (int i=0; i<globalparams.size(); i++) {
                c.globalParams.append(constructQListWithMultipleItems(QStringList(globalparams[i]), fitmodels.size()));
            }

            configureFitFromGlobal(c, false);
        }
        current->deleteProperty("PRESET_FIT_MODELS_LIST");
        current->deleteProperty("PRESET_FIT_MODELS_GLOBALPARAMS_LIST");
        current->deleteProperty("PRESET_FIT_MODELS_ROLES_LIST");
    }

    ensureCorrectParamaterModelDisplay();
    displayEvaluation();
}

void QFFCCSFitEvaluationEditor::resultsChanged() {
    /* some other evaluation or the user changed the results stored in the current raw data record,
       so redisplay */
    ensureCorrectParamaterModelDisplay();
    displayEvaluation();
}

void QFFCCSFitEvaluationEditor::readSettings() {
    // read widget settings
    if (!settings) return;
    currentSaveDirectory=settings->getQSettings()->value(QString("FCCS_fit/editor/lastSaveDirectory"), currentSaveDirectory).toString();
    if (current) {
        loadSplitter(*(settings->getQSettings()), ui->splitterPlots, QString("QFFCCSFitEvaluationEditor%1/splitterPlots").arg(current->getID()));
        loadSplitter(*(settings->getQSettings()), ui->splitter_2, QString("QFFCCSFitEvaluationEditor%1/splitter_2").arg(current->getID()));
        loadSplitter(*(settings->getQSettings()), ui->splitterPlotTable, QString("QFFCCSFitEvaluationEditor%1/splitterPlotTable").arg(current->getID()));
    }
}

void QFFCCSFitEvaluationEditor::writeSettings() {
    // write widget settings
    if (!settings) return;
    settings->getQSettings()->setValue(QString("FCCS_fit/editor/lastSaveDirectory"), currentSaveDirectory);
    if (current) {
        saveSplitter(*(settings->getQSettings()), ui->splitterPlots, QString("QFFCCSFitEvaluationEditor%1/splitterPlots").arg(current->getID()));
        saveSplitter(*(settings->getQSettings()), ui->splitter_2, QString("QFFCCSFitEvaluationEditor%1/splitter_2").arg(current->getID()));
        saveSplitter(*(settings->getQSettings()), ui->splitterPlotTable, QString("QFFCCSFitEvaluationEditor%1/splitterPlotTable").arg(current->getID()));
    }
}

void QFFCCSFitEvaluationEditor::showFitAlgorithmHelp()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();
    int ppid=services->getFitAlgorithmManager()->getPluginForID(pid);
    QFFitAlgorithm* algorithm=data->getFitAlgorithm(pid);
    QString help=services->getFitAlgorithmManager()->getPluginHelp(ppid, pid);
    if (QFile::exists(help) && algorithm) {
        QFPluginServices::getInstance()->displayHelpWindow(help);
    } else {
        QMessageBox::information(this, tr("FCCS Fit"), tr("No Online-Help for this fit algorithm available."));
    }
}

void QFFCCSFitEvaluationEditor::configFitAlgorithm()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
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

void QFFCCSFitEvaluationEditor::fitAlgorithmChanged(int /*model*/)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    data->setFitAlgorithm(pid);
    /*populateFitButtons(data->getFitAlgorithm()->isThreadSafe());
    actFitAllThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());
    actFitAllFilesThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());
    actFitAllRunsThreaded->setEnabled(data->getFitAlgorithm()->isThreadSafe());*/
    QApplication::restoreOverrideCursor();
}

void QFFCCSFitEvaluationEditor::displayOptionsChanged()
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString pid=ui->cmbFitAlgorithm->currentFitAlgorithmID();

    current->setQFProperty("FCCSFit/datadisplay", ui->cmbDisplayData->currentIndex());
    current->setQFProperty("FCCSFit/errordisplay",ui->cmbErrorDisplay->currentIndex());
    current->setQFProperty("FCCSFit/grid",ui->chkGrid->isChecked());
    current->setQFProperty("FCCSFit/key",ui->chkKey->isChecked());

    displayData();
    QApplication::restoreOverrideCursor();
}

void QFFCCSFitEvaluationEditor::setCurrentRun(int run)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    data->setCurrentIndex(run);

    QFRawDataRecord* currentRecord=data->getFitFile(0);

    if (currentRecord) {
        ui->datacut->disableSliderSignals();
        ui->datacut->set_min(getUserRangeMin(currentRecord, data->getCurrentIndex()));
        ui->datacut->set_max(getUserRangeMax(currentRecord, data->getCurrentIndex()));
        ui->datacut->set_userMin(getUserMin(currentRecord, data->getCurrentIndex(), getUserRangeMin(currentRecord, data->getCurrentIndex())));
        ui->datacut->set_userMax(getUserMax(currentRecord, data->getCurrentIndex(), getUserRangeMax(currentRecord, data->getCurrentIndex())));
        ui->datacut->enableSliderSignals();
    }

    displayEvaluation();
    QApplication::restoreOverrideCursor();
}

void QFFCCSFitEvaluationEditor::configureFor2CSPIMFCCS()
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;

    if (data->getFitFileCount()<3) {
        while (data->getFitFileCount()<3) {
            data->addFitFile();
        }
    } else if (data->getFitFileCount()>3) {
        while (data->getFitFileCount()>3) {
            data->removeFitFile();
        }
    }

    data->setFitFunction(0, "fccs_fw_diff2colortcacfg");
    data->setFitFunction(1, "fccs_fw_diff2colortcacfr");
    data->setFitFunction(2, "fccs_fw_diff2colortcccf");
    data->clearLinkParameters();

    QStringList globals;
    globals<<"concentration_a"<<"concentration_b"<<"concentration_ab"
          <<"diff_coeff_a"  <<"diff_coeff_b"  <<"diff_coeff_ab"
         <<"diff_coeff2_a"  <<"diff_coeff2_b"  <<"diff_coeff2_ab"
        <<"diff_rho2_a"  <<"diff_rho2_b"  <<"diff_rho2_ab"
          <<"n_nonfluorescent"<<"nonfl_tau_a"<<"nonfl_theta_a"<<"nonfl_tau_b"<<"nonfl_theta_b"<<"nonfl_tau_ab"<<"nonfl_theta_ab"
          <<"crosstalk"<<"focus_distance_x"  <<"focus_distance_y"  <<"focus_distance_z"
          <<"focus_width1"  <<"focus_width2"  <<"focus_struct_fac1"  <<"focus_struct_fac2"
          <<"count_rate1"<<"count_rate2"<<"background1"<<"background2";

    for (int g=0; g<globals.size(); g++) {
        data->setLinkParameter(0, globals[g], g);
        data->setLinkParameter(1, globals[g], g);
        data->setLinkParameter(2, globals[g], g);
    }
}

void QFFCCSFitEvaluationEditor::onConfigureGlobalItemClicked()
{
    QAction* act=qobject_cast<QAction*>(sender());
    int idx=actsGlobalConfig.indexOf(act);
    //qDebug()<<"sender()="<<sender()<<" type="<<sender()->metaObject()->className()<<"   act="<<act<<"   idx="<<idx;
    if (act&&idx>=0&&idx<globalConfig.size()) {
        bool loadParams=false;
        if (globalConfig[idx].paramValues.size()>0) {
            loadParams=QMessageBox::question(this, tr("load global fit configuration ..."), tr("The configuration file also contains fit parameter presets and ranges.\nShould these also be loaded?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
        }
        configureFitFromGlobal(globalConfig[idx], loadParams);
    }
}

void QFFCCSFitEvaluationEditor::configureFitFromGlobal(const QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig &config, bool loadParams)
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;

    if (!data) return;

    if (data->getFitFileCount()<config.models.size()) {
        while (data->getFitFileCount()<config.models.size()) {
            data->addFitFile();
        }
    } else if (data->getFitFileCount()>config.models.size()) {
        while (data->getFitFileCount()>config.models.size()) {
            data->removeFitFile();
        }
    }

    for (int i=0; i<config.models.size(); i++) {
        data->setFitFunction(i, config.models[i]);
    }


    if (config.roles.size()>0) {
        QFRawDataRecord* rdr=data->getFitFile(0);
        if (rdr) {
            int group=rdr->getGroup();
            QList<QFRawDataRecord*> l=rdr->getGroupMembers();
            for (int i=0; i<config.roles.size(); i++) {
                QString r=config.roles.value(i, "").toLower();
                if (!r.isEmpty()) {
                    int found=-1;
                    for (int j=0; j<l.size(); j++) {
                        if (l[j] && l[j]->getRole().toLower().startsWith(r)) {
                            found=j;
                            break;
                        }
                    }
                    if (found>=0) {
                        data->setFitFile(i, l[found]);
                        l.removeAt(found);
                    }
                }
            }
        }
    }
    data->clearLinkParameters();

    for (int i=0; i<config.globalParams.size(); i++) {
        QList<QStringList> g=config.globalParams[i];
        for (int j=0; j<g.size(); j++) {
            QStringList sl=g[j];
            if (sl.size()>0) {
                for (int s=0; s<sl.size(); s++) {
                    if (!sl[s].isEmpty()) {
                        data->setLinkParameter(j, sl[s], i);
                    }
                }
            }
        }
    }

    if (config.fixes.size()>0) {
        for (int i=0; i<config.fixes.size(); i++) {
            QFRawDataRecord* rdr=data->getFitFile(i);
            QFFitFunction* ff=data->getFitFunction(i);
            if (ff && rdr && config.fixes[i].size()>0) {
                for (int j=0; j<ff->paramCount(); j++) {
                    if (ff->getDescription(j).fit) {
                        data->setFitFix(rdr, data->getCurrentIndex(), ff->getParameterID(j), config.fixes[i].contains(ff->getParameterID(j)));
                        data->setInitFitFix(ff->getParameterID(j), config.fixes[i].contains(ff->getParameterID(j)), rdr);
                    }
                }
            }
        }
    }

    if (loadParams && config.singleFixes.size()>0) {
        for (int i=0; i<config.singleFixes.size(); i++) {
            QFRawDataRecord* rdr=data->getFitFile(i);
            QFFitFunction* ff=data->getFitFunction(i);
            if (ff && rdr && config.singleFixes[i].size()>0) {
                QMapIterator<QString, bool> it(config.singleFixes[i]);
                while (it.hasNext()) {
                    it.next();
                    if (ff->hasParameter(it.key())) {
                        data->setInitFitFix(it.key(), it.value(), rdr);
                        data->setFitFix(rdr, data->getCurrentIndex(), it.key(), it.value());
                    }
                }
            }
        }
    }
    if (loadParams && config.paramValues.size()>0) {
        for (int i=0; i<config.paramValues.size(); i++) {
            QFRawDataRecord* rdr=data->getFitFile(i);
            QFFitFunction* ff=data->getFitFunction(i);
            if (ff && rdr && config.paramValues[i].size()>0) {
                QMapIterator<QString, QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter> it(config.paramValues[i]);
                while (it.hasNext()) {
                    it.next();
                    if (ff->hasParameter(it.key())) {
                        data->setInitFitValue(it.key(), it.value().value, it.value().error, rdr);
                        data->setFitValue(rdr, data->getCurrentIndex(), it.key(), it.value().value);
                        data->setFitError(rdr, data->getCurrentIndex(), it.key(), it.value().error);
                        if (it.value().setRange) {
                            data->setFitMin(it.key(), it.value().rangeMin, rdr);
                            data->setFitMax(it.key(), it.value().rangeMax, rdr);
                        }
                    }
                }
            }
        }
    }

}

QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig QFFCCSFitEvaluationEditor::getCurrentGlobalFitConfig() const
{
    QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig config;
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return config;

    for (int i=0; i<data->getFitFileCount(); i++) {
        QString r="";
        if (data->getFitFile(i)) {
            r=data->getFitFile(i)->getRole();
        }
        config.models.append(data->getFitFunctionID(i));
        config.roles.append(r);
    }

    for (int i=0; i<data->getLinkParameterCount(); i++) {

        QList<QStringList> l;
        QStringList sl;
        for (int j=0; j<data->getFitFileCount(); j++) {
            l<<sl;
        }
        config.globalParams<<l;
    }

    for (int i=0; i<data->getFitFileCount(); i++) {
        QFFitFunction* ff=data->getFitFunction(i);
        QFRawDataRecord* rdr=data->getFitFile(i);
        if (ff && rdr) {
            QStringList fl;
            QMap<QString, QFFitFunctionConfigForGlobalFitInterface::GlobalFitParameter> params;
            for (int j=0; j<ff->paramCount(); j++) {
                const QString param=ff->getParameterID(j);
                const int global=data->getLinkParameter(i, param);
                if (global>=0 && global<config.globalParams.size()) {
                    config.globalParams[global].operator[](i).append(param);
                }
                if (data->getFitFix(rdr, data->getEvaluationResultID(data->getCurrentIndex(), rdr), param)) {
                    fl<<param;
                }
                params[param].value=data->getFitValue(rdr, data->getEvaluationResultID(data->getCurrentIndex(), rdr), param);
                params[param].error=data->getFitError(rdr, data->getEvaluationResultID(data->getCurrentIndex(), rdr), param);
                params[param].rangeMin=data->getFitMin(param, rdr);
                params[param].rangeMax=data->getFitMax(param, rdr);
            }
            config.fixes<<fl;
            config.paramValues<<params;
        }
    }

    return config;
}

void QFFCCSFitEvaluationEditor::configureForSPFCCS() {
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;

    if (data->getFitFileCount()<3) {
        while (data->getFitFileCount()<3) {
            data->addFitFile();
        }
    } else if (data->getFitFileCount()>3) {
        while (data->getFitFileCount()>3) {
            data->removeFitFile();
        }
    }

    data->setFitFunction(0, "fccs_fw_diff2coloracfg");
    data->setFitFunction(1, "fccs_fw_diff2coloracfr");
    data->setFitFunction(2, "fccs_fw_diff2colorccf");
    data->clearLinkParameters();

    QStringList globals;
    globals<<"concentration_a"<<"concentration_b"<<"concentration_ab"
          <<"diff_coeff_a"  <<"diff_coeff_b"  <<"diff_coeff_ab"
          <<"n_nonfluorescent"<<"nonfl_tau_a"<<"nonfl_theta_a"<<"nonfl_tau_b"<<"nonfl_theta_b"<<"nonfl_tau_ab"<<"nonfl_theta_ab"
          <<"crosstalk"<<"focus_distance_x"  <<"focus_distance_y"  <<"focus_distance_z"
          <<"focus_width1"  <<"focus_width2"  <<"focus_struct_fac1"  <<"focus_struct_fac2"
          <<"count_rate1"<<"count_rate2"<<"background1"<<"background2";



    for (int g=0; g<globals.size(); g++) {
        data->setLinkParameter(0, globals[g], g);
        data->setLinkParameter(1, globals[g], g);
        data->setLinkParameter(2, globals[g], g);
    }

}

void QFFCCSFitEvaluationEditor::configureForASPFCCS() {
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;

    if (data->getFitFileCount()<3) {
        while (data->getFitFileCount()<3) {
            data->addFitFile();
        }
    } else if (data->getFitFileCount()>3) {
        while (data->getFitFileCount()>3) {
            data->removeFitFile();
        }
    }

    data->setFitFunction(0, "fccs_fw_adiff2coloracfg");
    data->setFitFunction(1, "fccs_fw_adiff2coloracfr");
    data->setFitFunction(2, "fccs_fw_adiff2colorccf");
    data->clearLinkParameters();

    QStringList globals;
    globals<<"concentration_a"<<"concentration_b"<<"concentration_ab"
           <<"diff_acoeff_a"  <<"diff_acoeff_b"  <<"diff_acoeff_ab"
           <<"diff_alpha_a"  <<"diff_alpha_b"  <<"diff_alpha_ab"
           <<"n_nonfluorescent"<<"nonfl_tau_a"<<"nonfl_theta_a"<<"nonfl_tau_b"<<"nonfl_theta_b"<<"nonfl_tau_ab"<<"nonfl_theta_ab"
           <<"crosstalk"<<"focus_distance_x"  <<"focus_distance_y"  <<"focus_distance_z"
           <<"focus_width1"  <<"focus_width2"  <<"focus_struct_fac1"  <<"focus_struct_fac2"
           <<"count_rate1"<<"count_rate2"<<"background1"<<"background2";

    for (int g=0; g<globals.size(); g++) {
        data->setLinkParameter(0, globals[g], g);
        data->setLinkParameter(1, globals[g], g);
        data->setLinkParameter(2, globals[g], g);
    }

}

void QFFCCSFitEvaluationEditor::filesSetActivated(const QModelIndex &idx)
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    data->setFitFileSet(idx.row());
}

void QFFCCSFitEvaluationEditor::ensureCorrectParamaterModelDisplay()
{
    QElapsedTimer t;
    t.start();
    //qDebug()<<"ensureCorrectParamaterModelDisplay";
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    setUpdatesEnabled(false);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked| QAbstractItemView::SelectedClicked| QAbstractItemView::EditKeyPressed| QAbstractItemView::EditKeyPressed|QAbstractItemView::CurrentChanged);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QHeaderView* h=ui->tableView->verticalHeader();
    if (h) {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        h->setSectionResizeMode(QHeaderView::Fixed);
        #else
        h->setResizeMode(QHeaderView::Fixed);
        #endif
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
                #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                h->setSectionResizeMode(i, QHeaderView::Interactive);
                #else
                h->setResizeMode(i, QHeaderView::Interactive);
                #endif

               //qDebug()<<"  resizing h"<<i<<" to contents done "<<t.elapsed()<<"ms";
           }
           //h->setResizeMode(i, QHeaderView::Interactive);
           if (i>0 && eval) {
               int ii=(i-1)%(eval->getParameterInputTableModel()->getColsPerRDR());
               //qDebug()<<"  resizing h"<<i<<" to contents "<<t.elapsed()<<"ms";
               if (ii==0) h->resizeSection(i, 125);
               else if (ii==1) h->resizeSection(i, 50);
               else if (ii==2) h->resizeSection(i, 50);
               else if (ii==eval->getParameterInputTableModel()->getColsPerRDR()-2) h->resizeSection(i, 24);
               else if (ii==eval->getParameterInputTableModel()->getColsPerRDR()-1) h->resizeSection(i, 75);
               else h->resizeSection(i, 75);
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
   setParameterVisibility();
   setUpdatesEnabled(true);
   //qDebug()<<"ensureCorrectParamaterModelDisplay done "<<t.elapsed()<<"ms";

}

void QFFCCSFitEvaluationEditor::fileChanged(int num, QFRawDataRecord *file)
{
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (num==0) ui->pltOverview->setRDR(file);
    if (!eval) return;

    ui->datacut->disableSliderSignals();
    ui->datacut->set_min(0);
    ui->datacut->set_max(getUserRangeMax(file, eval->getCurrentIndex()));
    ui->datacut->set_userMin(getUserMin(0));
    ui->datacut->set_userMax(getUserMax(getUserRangeMax(file, eval->getCurrentIndex())));
    ui->datacut->enableSliderSignals();
    displayData();
}

void QFFCCSFitEvaluationEditor::plotMouseMoved(double x, double y)
{
    ui->labPosition->setText(tr("current position: (%1s, %2)").arg(floattohtmlstr(x, 3, true, 1e-18).c_str()).arg(y));
}

void QFFCCSFitEvaluationEditor::displayEvaluation() {
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);

    displayData();
    eval->getParameterInputTableModel()->rebuildModel();

}

void QFFCCSFitEvaluationEditor::displayData() {
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);


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
    ds->clear();
    QList<QColor> cols;
    cols<<QColor("darkgreen")<<QColor("red")<<QColor("blue")<<QColor("darkorange")<<QColor("purple")<<QColor("maroon");
    //qDebug()<<"\n\n\n\n ### displayData() eval="<<eval<<" ###";
    if (eval) {
        for (int file=0; file<eval->getFitFileCount(); file++) {
            QFFitFunction* ff=eval->getFitFunction(file);
            QFRawDataRecord* rec=eval->getFitFile(file);

            QFRDRFCSDataInterface* fcs=dynamic_cast<QFRDRFCSDataInterface*>(rec);

            double* data=fcs->getCorrelationRun(eval->getCurrentIndex());
            double* tau=fcs->getCorrelationT();
            bool wOK=false;
            double* sigma=eval->allocWeights(&wOK, rec, eval->getCurrentIndex());//fcs->getCorrelationRunError(eval->getCurrentIndex());
            if ((!wOK  || eval->getFitDataWeighting()==QFFCSWeightingTools::EqualWeighting) && sigma) {
                qfFree(sigma);
                sigma=NULL;
            }
            long N=fcs->getCorrelationN();
            //qDebug()<<"file="<<file<<" I="<<eval->getCurrentIndex()<<":   tau="<<tau[0]<<" d="<<data[0]<<N;
            if (data && tau) {
                size_t c_tau=ds->addCopiedColumn(tau, N, tr("file%1: tau [s]").arg(file+1));
                size_t c_data=ds->addCopiedColumn(data, N, tr("file%1: g(tau)").arg(file+1));
                int c_error=-1;
                if (sigma &&  eval->getFitDataWeighting()!=QFFCSWeightingTools::EqualWeighting) c_error=ds->addCopiedColumn(sigma, N, tr("file%1: errors").arg(file+1));
                //qDebug()<<c_tau<<c_data<<c_error;
                JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(ui->pltData->get_plotter());
                QString plotname=QString("\\verb{")+rec->getName()+QString(": ")+fcs->getCorrelationRunName(eval->getCurrentIndex())+QString("}");
                g->set_title(plotname);
                g->set_xColumn(c_tau);
                g->set_yColumn(c_data);
                g->set_xErrorColumn(-1);
                g->set_yErrorColumn(c_error);
                g->set_color(cols.value(file, g->get_color()));
                g->set_fillColor(g->get_color().lighter());
                QColor ec=g->get_color().lighter();
                g->set_errorColor(ec);
                ec.setAlphaF(0.5);
                g->set_errorFillColor(ec);
                g->set_symbol(ui->cmbDisplayData->getSymbol());
                g->set_drawLine(ui->cmbDisplayData->getDrawLine());
                g->set_symbolSize(ui->cmbDisplayData->getSymbolSize());
                g->set_lineWidth(1);
                g->set_xErrorStyle(JKQTPnoError);
                g->set_yErrorStyle(ui->cmbErrorDisplay->getErrorStyle());
                g->set_datarange_start(ui->datacut->get_userMin());
                g->set_datarange_end(ui->datacut->get_userMax());
                ui->pltData->get_plotter()->addGraph(g);

                //qDebug()<<ff<<ff->name();

                double* params=eval->allocFillParameters(rec, eval->getCurrentIndex(), ff);
                double* err=eval->allocFillParameterErrors(rec, eval->getCurrentIndex(), ff);
                bool* fix=eval->allocFillFix(rec, eval->getCurrentIndex(), ff);
                QVector<double> paramsV;
                for (int i=0; i<ff->paramCount(); i++) {
                    paramsV<<params[i];
                    //qDebug()<<ff->getParameterID(i)<<" = "<<params[i];
                }
                JKQTPxQFFitFunctionLineGraph* g_fit=new JKQTPxQFFitFunctionLineGraph(ui->pltData);
                g_fit->set_title(tr("fit: %1").arg(plotname));
                g_fit->set_fitFunction(ff, false);
                g_fit->set_params(paramsV);
                g_fit->set_color(g->get_color());
                g_fit->set_style(Qt::DashLine);
                g_fit->set_lineWidth(2);
                g_fit->set_minSamples(30);
                g_fit->set_maxRefinementDegree(4);
                g_fit->set_plotRefinement(true);
                ui->pltData->get_plotter()->addGraph(g_fit);

                QFFitStatistics fstat=ff->calcFitStatistics(N, tau, data, sigma, ui->datacut->get_userMin(), ui->datacut->get_userMax(), params, err, fix, 3, 100,QVector<double>(), 200, false, NULL, false);

                size_t c_resid=ds->addCopiedColumn(fstat.residuals, tr("file%1: residuals").arg(file+1));
                size_t c_residw=ds->addCopiedColumn(fstat.residuals_weighted, tr("file%1: weighted residuals").arg(file+1));
                size_t c_residat=ds->addCopiedColumn(fstat.tau_runavg, tr("file%1: tau for avg. res.").arg(file+1));
                size_t c_resida=ds->addCopiedColumn(fstat.residuals_runavg, tr("file%1: avg. residuals").arg(file+1));
                size_t c_residaw=ds->addCopiedColumn(fstat.residuals_runavg_weighted, tr("file%1: weighted avg. residuals").arg(file+1));
                JKQTPxyLineGraph* g_res=new JKQTPxyLineGraph();
                g_res->set_xColumn(c_tau);
                g_res->set_yColumn(c_resid);
                g_res->set_color(g->get_color());
                g_res->set_fillColor(g->get_fillColor());
                g_res->set_symbol(ui->cmbDisplayData->getSymbol());
                g_res->set_symbolSize(ui->cmbDisplayData->getSymbolSize());
                g_res->set_lineWidth(1);
                g_res->set_drawLine(ui->cmbDisplayData->getDrawLine());
                g_res->set_datarange_start(ui->datacut->get_userMin());
                g_res->set_datarange_end(ui->datacut->get_userMax());
                ui->pltResiduals->get_plotter()->addGraph(g_res);
                JKQTPxyLineGraph* g_resa=new JKQTPxyLineGraph();
                g_resa->set_xColumn(c_residat);
                g_resa->set_yColumn(c_resida);
                g_resa->set_color(g->get_color());
                g_resa->set_drawLine(true);
                g_resa->set_symbol(JKQTPnoSymbol);
                ui->pltResiduals->get_plotter()->addGraph(g_resa);
                fstat.free();




                qfFree(params);
                qfFree(err);
                qfFree(fix);
            }
            if (sigma) qfFree(sigma);

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





void QFFCCSFitEvaluationEditor::fitCurrent() {
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg) return;
    QList<QFRawDataRecord*> records=eval->getFitFiles();
    if (records.size()<=0) return;

    falg->setReporter(dlgFitProgressReporter);
    QString runname=tr("average");
    if (eval->getCurrentIndex()>=0) runname=QString::number(eval->getCurrentIndex());
    dlgFitProgress->reportSuperStatus(tr("fit run %1<br>using algorithm '%2' \n").arg(runname).arg(falg->name()));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(100);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    dlgFitProgress->setAllowCancel(true);
    dlgFitProgress->display();
    QApplication::processEvents();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    eval->doFit(records, eval->getCurrentIndex(), getUserMin(records[0], eval->getCurrentIndex(), ui->datacut->get_userMin()), getUserMax(records[0], eval->getCurrentIndex(), ui->datacut->get_userMax()), dlgFitProgressReporter, ProgramOptions::getConfigValue(eval->getType()+"/log", false).toBool());
    for (int i=0; i<records.size(); i++) {
        records[i]->enableEmitResultsChanged(true);
    }

    dlgFitProgress->reportSuperStatus(tr("fit done ... updating user interface\n"));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(100);
    displayEvaluation();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
    falg->setReporter(NULL);
    QApplication::processEvents();
    current->emitResultsChanged();
    eval->getParameterInputTableModel()->rebuildModel();
}

void QFFCCSFitEvaluationEditor::fitAllRunsThreaded()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg->isThreadSafe()) {
        fitAllRuns();
        return;
    }
    if (!falg) return;

    dlgQFProgressDialog* dlgTFitProgress=new dlgQFProgressDialog(this);
    dlgTFitProgress->reportTask(tr("fit all runs in the current file<br>using algorithm '%1' \n").arg(falg->name()));
    dlgTFitProgress->setProgressMax(100);
    dlgTFitProgress->setProgress(0);
    dlgTFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgTFitProgress->display();


    int items=0;
    int thread=0;
    dlgTFitProgress->reportStatus("creating thread objects ...");
    QApplication::processEvents();
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    //QList<QFFitResultsByIndexEvaluationFitThread*> threads;
    int threadcount=qMax(2,ProgramOptions::getInstance()->getMaxThreads());
    if (ProgramOptions::getConfigValue(eval->getType()+"/overrule_threads", false).toBool()) {
        threadcount=qMax(2,ProgramOptions::getConfigValue(eval->getType()+"/threads", 1).toInt());
    }
    /*for (int i=0; i<threadcount; i++) {
        threads.append(new QFFitResultsByIndexEvaluationFitThread(true, this));
    }


    // enqueue the jobs in the several threads and finally make sure they know when to stop
    dlgTFitProgress->reportStatus("distributing jobs ...");
    QApplication::processEvents();
    //for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=eval->getHighlightedRecord();
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);

        if (record ) {
            record->disableEmitResultsChanged();
            int runmax=eval->getIndexMax(record);
            int runmin=eval->getIndexMin(record);
            items=items+runmax-runmin+1;
            for (int run=runmin; run<=runmax; run++) {
                bool doall=!current->getProperty("LEAVEOUTMASKED", false).toBool();
                //qDebug()<<doall;
                if (run<=runmax && (doall || (!doall && rsel && !rsel->leaveoutRun(run)))) {
                    threads[thread]->addJob(eval, record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()));
                    thread++;
                    if (thread>=threadcount) thread=0;
                }
            }
        }
    //}
    dlgTFitProgress->setProgressMax(items+5);


    // start all threads and wait for them to finish
    for (int i=0; i<threadcount; i++) {
        threads[i]->start();
    }
    bool finished=false;
    bool canceled=false;
    QTime time;
    time.start();
    while (!finished) {


        // check whether all threads have finished and collect progress info
        finished=true;
        int jobsDone=0;
        for (int i=0; i<threadcount; i++) {
            finished=finished&&threads[i]->isFinished();
            jobsDone=jobsDone+threads[i]->getJobsDone();
        }
        dlgTFitProgress->setProgress(jobsDone);
        if (!canceled) {
            double runtime=double(time.elapsed())/1.0e3;
            double timeperfit=runtime/double(jobsDone);
            double estimatedRuntime=double(items)*timeperfit;
            double remaining=estimatedRuntime-runtime;
            dlgTFitProgress->reportStatus(tr("processing fits in %3 threads ... %1/%2 done\nruntime: %4:%5       remaining: %6:%7 [min:secs]       %8 fits/sec").arg(jobsDone).arg(items).arg(threadcount).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));
        }
        QApplication::processEvents();

        // check for user canceled
        if (!canceled && dlgTFitProgress->isCanceled()) {
            dlgTFitProgress->reportStatus("sending all threads the CANCEL signal");
            for (int i=0; i<threadcount; i++) {
                threads[i]->cancel(false);
            }
            canceled=true;
        }
    }


    // free memory
    for (int i=0; i<threadcount; i++) {
        threads[i]->cleanJobs();
        delete threads[i];
    }*/

    dlgTFitProgress->reportStatus(tr("fit done ... updating user interface\n"));
    dlgTFitProgress->setProgress(items+2);
    QApplication::processEvents();

    /*for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        if (record ) {
            record->enableEmitResultsChanged(true);
        }
    }*/
    current->emitResultsChanged();

    dlgTFitProgress->setProgress(items+3);
    QApplication::processEvents();
    displayEvaluation();
    dlgTFitProgress->setProgress(items+5);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
    dlgTFitProgress->done();
    delete dlgTFitProgress;
    eval->getParameterInputTableModel()->rebuildModel();
}

void QFFCCSFitEvaluationEditor::fitAllRuns()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg) return;
    QList<QFRawDataRecord*> records=eval->getFitFiles();
    if (records.size()<=0) return;
    QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(records[0]);

    falg->setReporter(dlgFitProgressReporter);
    int runmax=eval->getIndexMax(records[0]);
    int runmin=eval->getIndexMin(records[0]);
    QString runname=tr("average");
    if (eval->getCurrentIndex()>=0) runname=QString::number(eval->getCurrentIndex());
    dlgFitProgress->reportSuperStatus(tr("fit run %1<br>using algorithm '%2' \n").arg(runname).arg(falg->name()));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(runmax-runmin);
    dlgFitProgress->setProgress(0);
    dlgFitProgress->setSuperProgress(0);
    dlgFitProgress->setAllowCancel(true);
    dlgFitProgress->display();
    QApplication::processEvents();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


    QTime time;
    time.start();
    for (int run=runmin; run<=runmax; run++) {
        bool doall=true;//!current->getProperty("leaveoutMasked", false).toBool();
        if (doall || (!doall && rsel && !rsel->leaveoutRun(run))) {
            falg->setReporter(dlgFitProgressReporter);
            QString runname=tr("average");
            if (run>=0) runname=QString::number(run);
            double runtime=double(time.elapsed())/1.0e3;
            double timeperfit=runtime/double(run-runmin);
            double estimatedRuntime=double(runmax-runmin)*timeperfit;
            double remaining=estimatedRuntime-runtime;
            dlgFitProgress->reportSuperStatus(tr("fit run %1<br>using algorithm '%2' \nruntime: %3:%4       remaining: %5:%6 [min:secs]       %9 fits/sec").arg(runname).arg(falg->name()).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));

            //doFit(record, run);
            eval->doFit(records, run, getUserMin(records[0], run, ui->datacut->get_userMin()), getUserMax(records[0], run, ui->datacut->get_userMax()), dlgFitProgressReporter, ProgramOptions::getConfigValue(eval->getType()+"/log", false).toBool());

            dlgFitProgress->incSuperProgress();
            QApplication::processEvents();
            falg->setReporter(NULL);
            if (dlgFitProgress->isCanceled()) break;
        }
    }
    for (int i=0; i<records.size(); i++) {
        records[i]->enableEmitResultsChanged(true);
    }

    dlgFitProgress->reportSuperStatus(tr("fit done ... updating user interface\n"));
    dlgFitProgress->reportStatus("");
    dlgFitProgress->setProgressMax(100);
    dlgFitProgress->setSuperProgressMax(100);
    displayEvaluation();
    QApplication::restoreOverrideCursor();
    dlgFitProgress->done();
    falg->setReporter(NULL);
    QApplication::processEvents();
    current->emitResultsChanged();
    eval->getParameterInputTableModel()->rebuildModel();
}


void QFFCCSFitEvaluationEditor::fitAllFilesetsAllPixels()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg) return;

    int currentRun=eval->getCurrentIndex();
    QList<QList<QFRawDataRecord*> > filesets=eval->getFittedFiles();
    QList<QList<QFRawDataRecord*> > filesets_guessed=eval->getGuessedFiles();
    DlgFCCSFitAllConfiguration* dlg=new DlgFCCSFitAllConfiguration(this);
    if (dlg->exec()) {

        if (dlg->getFitGuessed()) filesets.append(filesets_guessed);

        int progMax=0;
        for (int fsIdx=0; fsIdx<filesets.size(); fsIdx++) {

            QList<QFRawDataRecord*> records=filesets[fsIdx];
            if (records.size()>0) {
                int runmax=eval->getIndexMax(records[0]);
                int runmin=eval->getIndexMin(records[0]);
                if (dlg->getFitAllRuns()) progMax=progMax+(runmax-runmin);
                else progMax++;
            }
        }

        falg->setReporter(dlgFitProgressReporter);
        dlgFitProgress->setSuperProgressMax(progMax);
        dlgFitProgress->setSuperProgress(0);
        dlgFitProgress->setAllowCancel(true);
        dlgFitProgress->display();
        for (int fsIdx=0; fsIdx<filesets.size(); fsIdx++) {

            QList<QFRawDataRecord*> records=filesets[fsIdx];
            if (records.size()>0) {
                //qDebug()<<"fitting "<<records[0]->getName();
                //for (int i=0; i<records.size(); i++) { eval->setFitFile(0, records[i]); }
                //QApplication::processEvents();
                QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(records[0]);

                int runmax=eval->getIndexMax(records[0]);
                int runmin=eval->getIndexMin(records[0]);
                if (!dlg->getFitAllRuns()) {
                    runmin=runmax=currentRun;
                }
                QString runname=tr("average");
                if (eval->getCurrentIndex()>=0) runname=QString::number(eval->getCurrentIndex());
                dlgFitProgress->reportSuperStatus(tr("fit run %1 in fileset %3<br>using algorithm '%2' \n").arg(runname).arg(falg->name()).arg(fsIdx+1));
                dlgFitProgress->reportStatus("");
                dlgFitProgress->setProgressMax(100);
                dlgFitProgress->setProgress(0);
                QApplication::processEvents();
                QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));


                QTime time;
                time.start();
                for (int run=runmin; run<=runmax; run++) {
                    bool doall=true;//!current->getProperty("leaveoutMasked", false).toBool();
                    if (doall || (!doall && rsel && !rsel->leaveoutRun(run))) {
                        falg->setReporter(dlgFitProgressReporter);
                        QString runname=tr("average");
                        if (run>=0) runname=QString::number(run);
                        double runtime=double(time.elapsed())/1.0e3;
                        double timeperfit=runtime/double(run-runmin);
                        double estimatedRuntime=double(runmax-runmin)*timeperfit;
                        double remaining=estimatedRuntime-runtime;
                        dlgFitProgress->reportSuperStatus(tr("fit run %1<br>using algorithm '%2' \nruntime: %3:%4       remaining: %5:%6 [min:secs]       %9 fits/sec").arg(runname).arg(falg->name()).arg(uint(int(runtime)/60),2,10,QChar('0')).arg(uint(int(runtime)%60),2,10,QChar('0')).arg(uint(int(remaining)/60),2,10,QChar('0')).arg(uint(int(remaining)%60),2,10,QChar('0')).arg(1.0/timeperfit,5,'f',2));

                        //doFit(record, run);
                        eval->doFit(records, run, getUserMin(records[0], run, ui->datacut->get_userMin()), getUserMax(records[0], run, ui->datacut->get_userMax()), dlgFitProgressReporter, ProgramOptions::getConfigValue(eval->getType()+"/log", false).toBool());

                        dlgFitProgress->incSuperProgress();
                        QApplication::processEvents();
                        falg->setReporter(NULL);
                        if (dlgFitProgress->isCanceled()) break;
                    }
                }
                for (int i=0; i<records.size(); i++) {
                    records[i]->enableEmitResultsChanged(true);
                }

            }
        }
        dlgFitProgress->reportSuperStatus(tr("fit done ... updating user interface\n"));
        dlgFitProgress->reportStatus("");
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setSuperProgressMax(100);
        displayEvaluation();
        QApplication::restoreOverrideCursor();
        dlgFitProgress->done();
        falg->setReporter(NULL);
        QApplication::processEvents();
        current->emitResultsChanged();
        eval->getParameterInputTableModel()->rebuildModel();
    }
    delete dlg;
}



void QFFCCSFitEvaluationEditor::resetCurrent()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    for (int f=0; f<eval->getFitFileCount(); f++) {
        QFRawDataRecord* rec=eval->getFitFile(f);
        eval->resetAllFitResultsCurrent(rec);
    }
    displayEvaluation();
    QApplication::restoreOverrideCursor();
}

void QFFCCSFitEvaluationEditor::resetAllPixels()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    for (int f=0; f<eval->getFitFileCount(); f++) {
        QFRawDataRecord* rec=eval->getFitFile(f);
        eval->resetAllFitResultsAllIndices(rec);
    }
    displayEvaluation();
    QApplication::restoreOverrideCursor();
}

void QFFCCSFitEvaluationEditor::resetAllPixelsInAllFilesets()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QList<QList<QFRawDataRecord* > > filesets=eval->getFittedFiles();
    for (int i=0; i<filesets.size(); i++) {
        for (int f=0; f<filesets[i].size(); f++) {
            QFRawDataRecord* rec=filesets[i].value(f);
            eval->resetAllFitResultsCurrent(rec);
        }
    }
    displayEvaluation();
    QApplication::restoreOverrideCursor();
}

void QFFCCSFitEvaluationEditor::copyToInitial()
{
    if (!current) return;
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!eval) return;

    eval->set_doEmitResultsChanged(false);
    eval->set_doEmitPropertiesChanged(false);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    for (int f=0; f<eval->getFitFileCount(); f++) {
        QFRawDataRecord* rec=eval->getFitFile(f);
        QFFitFunction* ffunc=eval->getFitFunction(f);

        double* params=eval->allocFillParameters(ffunc);


        for (int i=0; i<ffunc->paramCount(); i++) {
            QString id=ffunc->getParameterID(i);
            double value=eval->getFitValue(id, rec);
            double error=eval->getFitError(id, rec);
            bool fix=eval->getFitFix(id, rec);
            if (ffunc->isParameterVisible(i, params)) {
                eval->setInitFitFix(id, fix, rec);
                eval->setInitFitValue(id, value, error, rec);
                //qDebug()<<"set("<<rec->getID()<<") "<<id<<" = "<<value<<" +/- "<<error;
            };
        }
        qfFree(params);
    }


    eval->set_doEmitResultsChanged(true);
    eval->set_doEmitPropertiesChanged(true);
    eval->emitResultsChanged();

    QApplication::restoreOverrideCursor();
}

void QFFCCSFitEvaluationEditor::setParameterTableSpans()
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    for (int i=1; i<ui->tableView->model()->columnCount(); i=i+data->getParameterInputTableModel()->getColsPerRDR()) {
        ui->tableView->setSpan(0,i,1,data->getParameterInputTableModel()->getColsPerRDR());
        ui->tableView->setSpan(1,i,1,data->getParameterInputTableModel()->getColsPerRDR());
    }
    setParameterVisibility();
}

void QFFCCSFitEvaluationEditor::guessFromCurrentFileSet()
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    data->guessFileSets(data->getFitFiles(), true, true);
}

void QFFCCSFitEvaluationEditor::on_cmbWeight_currentWeightChanged(QFFCSWeightingTools::DataWeight weight)
{
    if (!current) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    data->setFitDataWeighting(weight);
    displayData();
    QApplication::restoreOverrideCursor();
}

int QFFCCSFitEvaluationEditor::getUserRangeMin(QFRawDataRecord *rec, int index)
{
    Q_UNUSED(rec)
    Q_UNUSED(index)
    return 0;
}

int QFFCCSFitEvaluationEditor::getUserMin(QFRawDataRecord *rec, int index, int defaultMin)
{
    Q_UNUSED(index)
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return defaultMin;
    const QString resultID=data->getEvaluationResultID(-1, rec);
    const QString resultIDNew=QString("%1_%2").arg(data->getType()).arg(data->getID());

    // WORKROUND FOR OLD PROPERTY NAMES
    int defaultM=rec->getProperty(QString(resultID+"_datacut_min"), defaultMin).toInt();

    if (current) defaultM=current->getProperty("datacut_min", defaultM).toInt();

    defaultM=rec->getProperty(resultID+"_datacut_min", defaultM).toInt();
    defaultM=rec->getProperty(resultIDNew+"_datacut_min", defaultM).toInt();
    return defaultM;
}

int QFFCCSFitEvaluationEditor::getUserMax(QFRawDataRecord *rec, int /*index*/, int defaultMax)
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return defaultMax;
    const QString resultID=data->getEvaluationResultID(-1, rec);
    const QString resultIDNew=QString("%1_%2").arg(data->getType()).arg(data->getID());

    // WORKROUND FOR OLD PROPERTY NAMES
    int defaultM=rec->getProperty(QString(resultID+"_datacut_max"), defaultMax).toInt();

    if (current) defaultM=current->getProperty("datacut_max", defaultM).toInt();

    defaultM= rec->getProperty(resultID+"_datacut_max", defaultM).toInt();
    defaultM= rec->getProperty(resultIDNew+"_datacut_max", defaultM).toInt();
    return defaultM;
    //return rec->getProperty(resultID+"_datacut_max", defaultMax).toInt();
}

void QFFCCSFitEvaluationEditor::setUserMinMax(int userMin, int userMax)
{

    if (!current) return;
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    data->setQFProperty("datacut_max", userMax, false, false);
    data->setQFProperty("datacut_min", userMin, false, false);
    for (int i=0; i<data->getFitFileCount(); i++) {
        QFRawDataRecord* rdr=data->getFitFile(i);
        setUserMinMaxInternal(data, rdr, userMin, userMax);
    }

}

void QFFCCSFitEvaluationEditor::slidersChanged(int userMin, int userMax, int /*min*/, int /*max*/)
{
    if (!current) return;
    setUserMinMax(userMin, userMax);
    displayData();
}

void QFFCCSFitEvaluationEditor::slidersCopyUserMinToAll(int userMin)
{
    if (!current) return;
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    QList<QList<QFRawDataRecord* > > fitted=data->getFittedFiles();
    QList<QList<QFRawDataRecord* > > guessed=data->getGuessedFiles();
    bool setGuessed=false;
    if ( guessed.size()>0) {
        setGuessed=QMessageBox::question(this, tr("FCCS: copy Min to all files"), tr("Do you also want to set min/max for guessed filesets?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
    }
    for (int i=0; i<fitted.size(); i++) {
        for (int j=0; j<fitted[i].size(); j++) {
            QFRawDataRecord* rdr=fitted[i].at(j);
            setUserMinInternal(data, rdr, userMin);
        }
    }
    if (guessed.size()>0 && setGuessed) {
        for (int i=0; i<guessed.size(); i++) {
            for (int j=0; j<guessed[i].size(); j++) {
                QFRawDataRecord* rdr=guessed[i].at(j);
                setUserMinInternal(data, rdr, userMin);
            }
        }
    }
}

void QFFCCSFitEvaluationEditor::slidersCopyUserMaxToAll(int userMax)
{
    if (!current) return;
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    QList<QList<QFRawDataRecord* > > fitted=data->getFittedFiles();
    QList<QList<QFRawDataRecord* > > guessed=data->getGuessedFiles();
    bool setGuessed=false;
    if ( guessed.size()>0) {
        setGuessed=QMessageBox::question(this, tr("FCCS: copy Max to all files"), tr("Do you also want to set min/max for guessed filesets?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
    }
    for (int i=0; i<fitted.size(); i++) {
        for (int j=0; j<fitted[i].size(); j++) {
            QFRawDataRecord* rdr=fitted[i].at(j);
            setUserMaxInternal(data, rdr, userMax);
        }
    }
    if (guessed.size()>0 && setGuessed) {
        for (int i=0; i<guessed.size(); i++) {
            for (int j=0; j<guessed[i].size(); j++) {
                QFRawDataRecord* rdr=guessed[i].at(j);
                setUserMaxInternal(data, rdr, userMax);
            }
        }
    }
}

void QFFCCSFitEvaluationEditor::slidersCopyUserMinMaxToAll(int userMin, int userMax)
{
    if (!current) return;
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    QList<QList<QFRawDataRecord* > > fitted=data->getFittedFiles();
    QList<QList<QFRawDataRecord* > > guessed=data->getGuessedFiles();
    bool setGuessed=false;
    if ( guessed.size()>0) {
        setGuessed=QMessageBox::question(this, tr("imFCCS: copy Min/Max to all files"), tr("Do you also want to set min/max for guessed filesets?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
    }
    for (int i=0; i<fitted.size(); i++) {
        for (int j=0; j<fitted[i].size(); j++) {
            QFRawDataRecord* rdr=fitted[i].at(j);
            //qDebug()<<"slidersCopyUserMinMaxToAll("<<userMin<<userMax<<"):  "<<rdr->getName()<<rdr;
            setUserMinMaxInternal(data, rdr, userMin, userMax);
        }
    }
    if (guessed.size()>0 && setGuessed) {
        for (int i=0; i<guessed.size(); i++) {
            for (int j=0; j<guessed[i].size(); j++) {
                QFRawDataRecord* rdr=guessed[i].at(j);
                setUserMinMaxInternal(data, rdr, userMin, userMax);
            }
        }
    }
}

void QFFCCSFitEvaluationEditor::setUserMinMaxInternal(QFFCCSFitEvaluationItem *data, QFRawDataRecord *rdr, int userMin, int userMax)
{
    const QString resultIDNew=QString("%1_%2").arg(data->getType()).arg(data->getID());
    const QString resultID=data->getEvaluationResultID(-1, rdr);
    rdr->disableEmitPropertiesChanged();
    rdr->setQFProperty(resultIDNew+"_datacut_min", userMin, false, false);
    rdr->setQFProperty(resultIDNew+"_datacut_max", userMax, false, false);
    //qDebug()<<"   "<<rdr<<resultIDNew+"_datacut_min/max";
    rdr->enableEmitPropertiesChanged(true);
}

void QFFCCSFitEvaluationEditor::setUserMinInternal(QFFCCSFitEvaluationItem *data, QFRawDataRecord *rdr, int userMin)
{
    const QString resultIDNew=QString("%1_%2").arg(data->getType()).arg(data->getID());
    const QString resultID=data->getEvaluationResultID(-1, rdr);
    rdr->disableEmitPropertiesChanged();
    rdr->setQFProperty(resultIDNew+"_datacut_min", userMin, false, false);
    //qDebug()<<"   "<<rdr<<resultIDNew+"_datacut_min";
    rdr->enableEmitPropertiesChanged(true);
}

void QFFCCSFitEvaluationEditor::setUserMaxInternal(QFFCCSFitEvaluationItem *data, QFRawDataRecord *rdr, int userMax)
{
    const QString resultIDNew=QString("%1_%2").arg(data->getType()).arg(data->getID());
    const QString resultID=data->getEvaluationResultID(-1, rdr);
    rdr->disableEmitPropertiesChanged();
    rdr->setQFProperty(resultIDNew+"_datacut_max", userMax, false, false);
    //qDebug()<<"   "<<rdr<<resultIDNew+"_datacut_max";
    rdr->enableEmitPropertiesChanged(true);
}

int QFFCCSFitEvaluationEditor::getUserRangeMax(QFRawDataRecord *rec, int /*index*/)
{
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(rec);
    if (data) {
        return data->getCorrelationN()-1;
    }
    return 0;
}
void QFFCCSFitEvaluationEditor::on_chkSaveStrings_toggled(bool checked)
{
    if (!current) return;
    current->setQFProperty("dontSaveFitResultMessage", !checked, false, false);
}

void QFFCCSFitEvaluationEditor::on_btnEditRanges_toggled(bool /*enabled*/)
{
    setParameterVisibility();
    ensureCorrectParamaterModelDisplay();
}

void QFFCCSFitEvaluationEditor::setParameterVisibility()
{
    bool enabled=ui->btnEditRanges->isChecked();
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    //data->getParameterInputTableModel()->setEditRanges(enabled);
    for (int i=1; i<ui->tableView->model()->columnCount(); i=i+data->getParameterInputTableModel()->getColsPerRDR()) {
        ui->tableView->setColumnHidden(i+2, !enabled);
        ui->tableView->setColumnHidden(i+3, !enabled);
    }
}

void QFFCCSFitEvaluationEditor::on_btnAddFile_clicked()
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    data->addFitFile();
    setParameterTableSpans();
    ensureCorrectParamaterModelDisplay();
    //connect(ui->btnRemoveFile, SIGNAL(clicked()), item, SLOT(removeFitFile()));

}

void QFFCCSFitEvaluationEditor::on_btnRemoveFile_clicked()
{
    QFFCCSFitEvaluationItem* data=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!data) return;
    QStringList files;

    for (int i=0; i<data->getFitFileCount(); i++) {
        QFRawDataRecord* rdr=data->getFitFile(i);
        QFFitFunction* ff=data->getFitFunction(i);
        QString f=tr("file #%1").arg(i+1);
        if (rdr) {
            f=f+QString(": ")+rdr->getName();
        }
        files<<f;
    }

    bool ok=false;
    int id=files.indexOf(QInputDialog::getItem(this, tr("remove file"), tr("please select the file to remove [default: last file]:"), files, files.size()-1, false, &ok));
    if (ok) {
        data->removeFitFile(id);
        setParameterTableSpans();
        ensureCorrectParamaterModelDisplay();
    }
    //connect(ui->btnRemoveFile, SIGNAL(clicked()), item, SLOT(removeFitFile()));
}

void QFFCCSFitEvaluationEditor::loadGlobalFitConfig()
{
    if (!current) return;
    QString filename=qfGetOpenFileNameSet("QFFCCSFitEvaluationEditor/GlobalConfig_", this, tr("load global fit configuration ..."), ProgramOptions::getInstance()->getHomeQFDirectory()+"/globalfit_configs", tr("Global FCCS fit configs (*.gfc)"));
    if (QFile::exists(filename)) {
        QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig config;
        if (openGlobalFitConfig(filename, config)) {
            bool loadParams=false;
            if (config.paramValues.size()>0) {
                loadParams=QMessageBox::question(this, tr("load global fit configuration ..."), tr("The configuration file also contains fit parameter presets and ranges.\nShould these also be loaded?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
            }
            configureFitFromGlobal(config, loadParams);
        }
    }
}

void QFFCCSFitEvaluationEditor::saveGlobalFitConfig()
{
    if (!current) return;
    QDir(ProgramOptions::getInstance()->getHomeQFDirectory()).mkpath("globalfit_configs");
    QString filename=qfGetSaveFileNameSet("QFFCCSFitEvaluationEditor/GlobalConfig_", this, tr("save global fit configuration ..."), ProgramOptions::getInstance()->getHomeQFDirectory()+"/globalfit_configs", tr("Global FCCS fit configs (*.gfc)"));
    if (!filename.isEmpty()) {
        QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig config=getCurrentGlobalFitConfig();
        storeGlobalFitConfig(filename, config);
    }
}

void QFFCCSFitEvaluationEditor::setParameterInRDRs()
{
    QFFCCSFitEvaluationItem* imfccseval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if (!imfccseval) return;
    DlgFCCSResetSetParameter* dlg=new DlgFCCSResetSetParameter(this);
    QList<DlgFCCSResetSetParameter::Parameter> p;
    dlg->setHelpfile(QFPluginServices::getInstance()->getPluginHelpDirectory(imfccseval->getType())+"/ui.html#setresetparams");

    QFFCCSParameterInputTable* tab=imfccseval->getParameterInputTableModel();
    QList<QFFCCSParameterInputTable::FitParam> ps=tab->getFitParamList();
    for (int i=0; i<ps.size(); i++) {
        if (ps[i].isEditable && !ps[i].visibleIn.isEmpty()) {
            DlgFCCSResetSetParameter::Parameter pp;
            pp.id=ps[i].id;
            pp.label=ps[i].label;
            pp.value=imfccseval->getFitValue(pp.id, ps[i].visibleIn.toList().value(0, NULL));
            pp.error=imfccseval->getFitError(pp.id, ps[i].visibleIn.toList().value(0, NULL));
            pp.fix=imfccseval->getFitFix(pp.id, ps[i].visibleIn.toList().value(0, NULL));
            p.append(pp);
        }
    }

    dlg->setParameters(p);
    if (dlg->exec()) {
        if (dlg->setInInitial()) {
            QFRawDataRecord* r=NULL;
            if (dlg->operationValue()==DlgFCCSResetSetParameter::opSet) imfccseval->setInitFitValue(dlg->getParameter(), dlg->getValue(), 0.0, r);
            if (dlg->operationError()==DlgFCCSResetSetParameter::opSet) imfccseval->setInitFitError(dlg->getParameter(), dlg->getError(), r);
            if (dlg->operationFix()==DlgFCCSResetSetParameter::opSet) imfccseval->setInitFitFix(dlg->getParameter(), dlg->getFix(), r);
        }
        QList<QList<QFRawDataRecord*> > fitfilesets;
        QList<QFRawDataRecord*> currentFitFiles=imfccseval->getFitFiles();
        fitfilesets.append(imfccseval->getFitFiles());
        if (dlg->setInAll()) {
            fitfilesets=imfccseval->getFittedFiles();
        }
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QProgressDialog progress(tr("Setting fit parameters"), tr("&Cancel"), 0, fitfilesets.size(), this);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
        for (int i=0; i<fitfilesets.size(); i++) {
            imfccseval->addFittedFileSet(fitfilesets[i]);
            for (int j=0; j<fitfilesets[i].size(); j++) {
                QFFitFunction* ff=imfccseval->getFitFunction(j);
                QFRawDataRecord* r=fitfilesets[i].at(j);
                if (r && ff && ff->hasParameter(dlg->getParameter())) {
                    bool doEmit=r->isEmitResultsChangedEnabled();
                    r->disableEmitResultsChanged();
                    if (dlg->setAllIndices()) {
                        if (dlg->operationValue()==DlgFCCSResetSetParameter::opSet) imfccseval->setAllFitResultValue(r,  dlg->getParameter(), dlg->getValue());
                        else if (dlg->operationValue()==DlgFCCSResetSetParameter::opReset) imfccseval->resetAllDefaultFitValue(r,  dlg->getParameter());
                        if (dlg->operationError()==DlgFCCSResetSetParameter::opSet) imfccseval->setAllFitResultError(r,  dlg->getParameter(), dlg->getError());
                        if (dlg->operationFix()==DlgFCCSResetSetParameter::opSet) imfccseval->setAllFitResultFix(r,  dlg->getParameter(), dlg->getFix());
                        else if (dlg->operationFix()==DlgFCCSResetSetParameter::opReset) imfccseval->resetAllDefaultFitFix(r,  dlg->getParameter());
                    } else {
                        QString rid=imfccseval->getEvaluationResultID(ff->id());
                        if (dlg->operationValue()==DlgFCCSResetSetParameter::opSet) imfccseval->setFitResultValue(r, rid, dlg->getParameter(), dlg->getValue());
                        else if (dlg->operationValue()==DlgFCCSResetSetParameter::opReset) imfccseval->resetDefaultFitValue(r, rid, dlg->getParameter());
                        if (dlg->operationError()==DlgFCCSResetSetParameter::opSet) imfccseval->setFitResultError(r, rid, dlg->getParameter(), dlg->getError());
                        if (dlg->operationFix()==DlgFCCSResetSetParameter::opSet) imfccseval->setFitResultFix(r, rid, dlg->getParameter(), dlg->getFix());
                        else if (dlg->operationFix()==DlgFCCSResetSetParameter::opReset) imfccseval->resetDefaultFitFix(r, rid, dlg->getParameter());
                    }
                    if (doEmit) r->enableEmitResultsChanged(true);
                }
                QApplication::processEvents();
                if (progress.wasCanceled()) break;
            }
            QApplication::processEvents();
            progress.setValue(i);
            if (progress.wasCanceled()) break;
        }
        imfccseval->setFitFiles(currentFitFiles);
        QApplication::restoreOverrideCursor();

    }

    delete dlg;
}

void QFFCCSFitEvaluationEditor::buildGlobalConfigs(QFFCCSFitEvaluationItem *current)
{
    for (int i=0; i<actsGlobalConfig.size(); i++) {
        actsGlobalConfig[i]->disconnect();
        delete actsGlobalConfig[i];
    }
    for (int i=0; i<menusGlobalConfig.size(); i++) {
        menuFCCSFit->removeAction(menusGlobalConfig[i]->menuAction());
        delete menusGlobalConfig[i];
    }
    globalConfig.clear();
    actsGlobalConfig.clear();
    menusGlobalConfig.clear();

    QStringList models=current->getAvailableFitFunctions();
    QList<QFFitFunctionConfigForGlobalFitInterface*> modelPlugins;
    for (int i=0; i<models.size(); i++) {
        QFFitFunctionConfigForGlobalFitInterface* intf=qobject_cast<QFFitFunctionConfigForGlobalFitInterface*>(QFFitFunctionManager::getInstance()->getPluginObject(QFFitFunctionManager::getInstance()->getPluginForID(models[i])));
        if (intf && !modelPlugins.contains(intf)) modelPlugins<<intf;
    }
    for (int i=0; i<modelPlugins.size(); i++) {
        for (int j=0; j<modelPlugins[i]->getGlobalFitConfigCount(); j++) {
            QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig c=modelPlugins[i]->getGlobalFitConfig(j);
            QMenu* menu=NULL;
            for (int k=0; k<menusGlobalConfig.size(); k++) {
                if (menusGlobalConfig[k]->title()==c.groupLabel) {
                    menu=menusGlobalConfig[k];
                    break;
                }
            }
            if (!menu) {
                menu=menuFCCSFit->addMenu(c.groupLabel);
                menusGlobalConfig<<menu;
            }
            if (menu) {
                QAction * act=menu->addAction(c.menuEntryLabel);
                connect(act, SIGNAL(triggered()), this, SLOT(onConfigureGlobalItemClicked()));
                actsGlobalConfig<<act;
                globalConfig<<c;
            }
        }
    }
}

void QFFCCSFitEvaluationEditor::errorEstimateModeChanged()
{
    if (!current) return;
    ui->widFitErrorEstimate->saveSettings(current);
}




void QFFCCSFitEvaluationEditor::createReportDoc(QTextDocument* document) {
    if (!current) return;
    //QFRawDataRecord* record=current->getHighlightedRecord();
    // possibly to a qobject_cast<> to the data type/interface you are working with here: QFRDRMyInterface* data=qobject_cast<QFRDRMyInterface*>(record);
    QFFCCSFitEvaluationItem* eval=qobject_cast<QFFCCSFitEvaluationItem*>(current);
    if ((!eval)/*||(!record)||(!data)*/) return;

    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

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
    cursor.insertText(tr("imFCCS Fit Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    cursor.insertText(tr("run: %1/%2..%3\n").arg(eval->getCurrentIndex()).arg(eval->getIndexMin(eval->getFitFile(0))).arg(eval->getIndexMax(eval->getFitFile(0))), fTextBoldSmall);

    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(2,1, tableFormat);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        ui->pltData->get_plotter()->draw(*painter, QRect(0,0,ui->pltData->width(),ui->pltData->height()+ui->pltResiduals->height()));
        delete painter;
        double scale=0.9*document->textWidth()/double(pic.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation curves:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.9);

        tabCursor=table->cellAt(1,0).firstCursorPosition();
        tabCursor.insertText(tr("\n"), fTextBoldSmall);
        QPicture picT;
        painter=new JKQTPEnhancedPainter(&picT);
        ui->tableView->paint(*painter);
        delete painter;
        scale=0.95*document->textWidth()/double(picT.boundingRect().width());
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("fit results table:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, picT, QSizeF(picT.boundingRect().width(), picT.boundingRect().height())*scale, 0.95);
    }
    QApplication::processEvents();
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();




    // insert table with some data
    /*QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(2, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("raw data:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("ID:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(QString::number(record->getID()));*/
    cursor.movePosition(QTextCursor::End);
}
