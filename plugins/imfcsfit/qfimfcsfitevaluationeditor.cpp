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

#include "qfimfcsfitevaluationeditor.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "qfrdrfcsdatainterface.h"
#include <iostream>
#include <cfloat>
#include "qfimfcsfitevaluation.h"
#include "cpptools.h"
#include "statistics_tools.h"
#include <QThread>
#include "qmoretextobject.h"
#include "qmodernprogresswidget.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "qfimfcssetparamfromfiledialog.h"
#include "statistics_tools.h"
#include "qfmathtools.h"





QFImFCSFitEvaluationEditor::QFImFCSFitEvaluationEditor(QFPluginServices *services, QFEvaluationPropertyEditor *propEditor, QWidget *parent):
    QFFitResultsByIndexEvaluationEditorWithWidgets("imfcsfitevaleditor/", propEditor, services, parent, true, true, tr("pixel"))
{
    //menuTools=propEditor->addMenu("Tools", 0);

    createWidgets();
    btnFirstRun->setText(tr("avg."));

    actFitAllFilesThreadedWriter=new QAction(QIcon(":/fcsfit/fit_fitcurrentrunallfiles.png"), tr("Fit All &Files this %1 (newMT)").arg(m_runName), this);
    actFitAllFilesThreadedWriter->setToolTip(tr("fast multi-threaded: perform a fit for all files, but fit in each file only the currently displayed %1").arg(m_runName));
    connect (actFitAllFilesThreadedWriter, SIGNAL(triggered()), this, SLOT(fitAllFilesThreadedWriter()));

    actFitAllThreadedWriter=new QAction(QIcon(":/imfcsfit/fit_fitall.png"), tr("Fit Everything (newMT)"), this);
    actFitAllThreadedWriter->setToolTip(tr("fast multi-threaded: perform a fit for all files, and all %1s therein (everything)").arg(m_runName));
    connect (actFitAllThreadedWriter, SIGNAL(triggered()), this, SLOT(fitEverythingThreadedWriter()));

    actFitAllRunsThreadedWriter=new QAction(QIcon(":/imfcsfit/fit_fitallruns.png"), tr("Fit All %1s, this file (newMT)").arg(m_runName), this);
    actFitAllRunsThreadedWriter->setToolTip(tr("fast multi-threaded: perform a fit for all %1s, in the current file").arg(m_runName));
    connect (actFitAllRunsThreadedWriter, SIGNAL(triggered()), this, SLOT(fitAllRunsThreadedWriter()));

    populateFitButtons();

}

QFImFCSFitEvaluationEditor::~QFImFCSFitEvaluationEditor()
{
    //dtor
}


void QFImFCSFitEvaluationEditor::getPlotData(QFRawDataRecord *rec, int index, QList<QFGetPlotdataInterface::GetPlotDataItem> &plots, int option, const QString &optionName)
{
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(rec);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (data&&eval) {

        if (option>3) {
            if (optionName==tr("FCS Diffusion Law: tauD vs. Aeff")) {
            } else if (optionName==tr("FCS Diffusion Law: Aeff/D vs. Aeff")) {
            } else if (optionName==tr("FCS Diffusion Law: tauD vs. w_{xy}^2")) {
            } else if (optionName==tr("FCS Diffusion Law: Aeff/D vs. w_{xy}^2")) {
            } else if (optionName==tr("FCS MSD: tauD vs. d^2")) {
            } else if (optionName==tr("FCS MSD: d^2/D vs. d^2")) {
            }
        } else {

            double norm=1;
            QVector<double> acftau, acf;
            acftau=arrayToVector(data->getCorrelationT(), data->getCorrelationN());;
            QFFitFunction* ffunc=eval->getFitFunction(rec);
            int datacut_min=datacut->get_min();
            int datacut_max=datacut->get_max();

            try {
                if (data->getCorrelationN()>0) {

                    long N=data->getCorrelationN();
                    int runAvgWidth=11;
                    double* tauvals=data->getCorrelationT();
                    double* corrdata=NULL;

                    if (eval->getCurrentIndex()<0) {
                        corrdata=data->getCorrelationMean();
                    } else {
                        if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                            corrdata=data->getCorrelationRun(eval->getCurrentIndex());
                        } else {
                            corrdata=data->getCorrelationMean();
                        }
                    }
                    //double* weights=eval->allocWeights(NULL, rec, eval->getCurrentIndex(), datacut_min, datacut_max);


                    /////////////////////////////////////////////////////////////////////////////////
                    // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
                    // we are working with a complete set of parameters
                    /////////////////////////////////////////////////////////////////////////////////
                    double* fullParams=eval->allocFillParameters(rec, index, ffunc);
                    double* errors=eval->allocFillParameterErrors(rec, index, ffunc);
                    ffunc->calcParameter(fullParams, errors);

                    for (int i=0; i<acftau.size(); i++) {
                        acf<<ffunc->evaluate(acftau[i], fullParams);
                    }

                    /////////////////////////////////////////////////////////////////////////////////
                    // try and find particle number for normalized CF
                    /////////////////////////////////////////////////////////////////////////////////
                    bool hasN=false;
                    bool has1N=false;
                    double pN=0, peN=0;
                    double p1N=0, pe1N=0;
                    for (int i=0;i<ffunc->paramCount(); i++) {
                        QFFitFunction::ParameterDescription d=ffunc->getDescription(i);
                        if (ffunc->isParameterVisible(i, fullParams)) {
                            if (d.id=="n_particle") {
                                hasN=true;
                                pN=fullParams[i];
                                if (errors) peN=errors[i];
                                break;
                            } else if (d.id=="1n_particle") {
                                has1N=true;
                                p1N=fullParams[i];
                                if (errors) pe1N=errors[i];
                            }
                        }
                    }

                    if (!hasN && has1N) {
                        pN=1.0/p1N;
                        peN=qfErrorDiv(1, 0, p1N, pe1N);
                        hasN=true;
                    }
                    if (hasN && corrdata) {
                        norm=pN;
                    }

                    /////////////////////////////////////////////////////////////////////////////////
                    // clean memory
                    /////////////////////////////////////////////////////////////////////////////////
                    qfFree(fullParams);
                    qfFree(errors);
                    //qfFree(weights);
                }
            } catch(std::exception& E) {
                services->log_error(tr("error during plotting, error message: %1\n").arg(E.what()));
            }




            QFGetPlotdataInterface::GetPlotDataItem item;
            item.x=acftau;
            item.y=arrayToVector(data->getCorrelationRun(index), data->getCorrelationN());
            item.averageGroupIndex=0;
            bool ok=true;
            double* w=eval->allocWeights(&ok, rec, index, true);
            if (ok && w) {
                item.yerrors=arrayToVector(w, data->getCorrelationN());
            }
            if (w) qfFree(w);
            item.name=rec->getName()+": "+data->getCorrelationRunName(index);
            if (option==2 || option==3) {
                item.name+=tr(", normalized");

                for (int i=0; i<item.y.size(); i++) {
                    item.y[i]=item.y[i]*norm;
                }
                for (int i=0; i<item.yerrors.size(); i++) {
                    item.yerrors[i]=item.yerrors[i]*norm;
                }

            }
            item.name=QString("\\verb{%1}").arg(item.name);
            plots.append(item);

            if (option==1 || option==3) {
                item.xerrors.clear();
                item.yerrors.clear();
                item.averageGroupIndex=1;
                item.y=acf;
                item.name=rec->getName()+": "+data->getCorrelationRunName(index)+tr(": fit");
                if (option==3) {
                    item.name=rec->getName()+": "+data->getCorrelationRunName(index)+tr(": normalized fit");
                    for (int i=0; i<item.y.size(); i++) {
                        item.y[i]=item.y[i]*norm;
                    }
                    for (int i=0; i<item.yerrors.size(); i++) {
                        item.yerrors[i]=item.yerrors[i]*norm;
                    }
                }
                item.name=QString("\\verb{%1}").arg(item.name);
                plots.append(item);
            }
        }

    }
}

bool QFImFCSFitEvaluationEditor::getPlotDataSpecs(QStringList *optionNames, QList<QFGetPlotdataInterface::GetPlotPlotOptions> *listPlotOptions)
{
    if (optionNames) {
        *optionNames<<tr("data");
        *optionNames<<tr("data + fits");
        *optionNames<<tr("normalized data");
        *optionNames<<tr("normalized data + fits");
    }
    if (listPlotOptions) {
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("correlation curve $g(\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("correlation curve $g(\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. correlation curve $N\\cdot g(\\tau)$"), true, false);
        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("lag time \\tau [s]"), tr("norm. correlation curve $N\\cdot g(\\tau)$"), true, false);
    }
    if (false && current) {
        QFRawDataRecord* record=current->getHighlightedRecord();
        QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
        QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
        if (data && eval) {
            QFFitFunction* ff=eval->getFitFunction();
            if (ff) {
                bool hasTauD=ff->hasParameter("diff_tau1");
                QString unitTauD="";
                if (hasTauD) unitTauD=ff->getDescription("diff_tau1").unit;

                bool hasMSDTauD=ff->hasParameter("msd_time_aeff1");
                QString unitMSDTauD="";
                if (hasMSDTauD) unitMSDTauD=ff->getDescription("msd_time_aeff1").unit;

                bool hasWxy=ff->hasParameter("focus_width");
                QString unitWxy="";
                if (hasWxy) unitWxy=ff->getDescription("focus_width").unit;

                bool hasD=ff->hasParameter("diff_coeff1");
                QString unitD="";
                if (hasD) unitD=ff->getDescription("diff_coeff1").unit;

                bool hasAeff=ff->hasParameter("effective_area");
                QString unitAEff="";
                if (hasAeff) unitAEff=ff->getDescription("effective_area").unit;

                bool hasDist=ff->hasParameter("focus_distance");
                QString unitDist="";
                if (hasDist) unitDist=ff->getDescription("focus_distance").unit;

                if (hasTauD && hasAeff) {
                    if (optionNames) {
                        *optionNames<<tr("FCS Diffusion Law: tauD vs. Aeff");
                    }
                    if (listPlotOptions) {
                        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("effective focus area A_{eff} [{\\mu}m^2]"), tr("effective diffusion time \\tau_D [s]"), false, false);
                    }
                }
                if (hasD && hasAeff) {
                    if (optionNames) {
                        *optionNames<<tr("FCS Diffusion Law: Aeff/D vs. Aeff");
                    }
                    if (listPlotOptions) {
                        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("effective focus area A_{eff} [{\\mu}m^2]"), tr("effective diffusion time \\tau_D=A_{eff}/D [s]"), false, false);
                    }
                }
                if (hasTauD && hasWxy) {
                    if (optionNames) {
                        *optionNames<<tr("FCS Diffusion Law: tauD vs. w_{xy}^2");
                    }
                    if (listPlotOptions) {
                        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("effective focus area A_{eff}=w_{xy}^2 [{\\mu}m^2]"), tr("effective diffusion time \\tau_D [s]"), false, false);
                    }
                }
                if (hasD && hasWxy) {
                    if (optionNames) {
                        *optionNames<<tr("FCS Diffusion Law: Aeff/D vs. w_{xy}^2");
                    }
                    if (listPlotOptions) {
                        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("effective focus area A_{eff}=w_{xy}^2 [{\\mu}m^2]"), tr("effective diffusion time \\tau_D=w_{xy}^2/D [s]"), false, false);
                    }
                }
                if (hasTauD && hasDist) {
                    if (optionNames) {
                        *optionNames<<tr("FCS MSD: tauD vs. d^2");
                    }
                    if (listPlotOptions) {
                        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("diffusion time \\tau_D [s]"), tr("squared focus distance d^2 [{\\mu}m^2]"), false, false);
                    }
                }
                if (hasD && hasDist) {
                    if (optionNames) {
                        *optionNames<<tr("FCS MSD: d^2/D vs. d^2");
                    }
                    if (listPlotOptions) {
                        *listPlotOptions<<QFGetPlotdataInterface::GetPlotPlotOptions(tr("diffusion time \\tau_D=d^2/D [s]"), tr("squared focus distance d^2 [{\\mu}m^2]"), false, false);
                    }
                }
            }
        }
    }
    return true;
}

int QFImFCSFitEvaluationEditor::getUserRangeMax(QFRawDataRecord *rec, int /*index*/) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(rec);
    if (data) {
        return data->getCorrelationN()-1;
    }
    return 0;
}

int QFImFCSFitEvaluationEditor::getUserRangeMin(QFRawDataRecord */*rec*/, int /*index*/) {
    return 0;
}

void QFImFCSFitEvaluationEditor::createWidgets() {

    cmbWeights=new QFFCSWeightingCombobox(this);
    cmbWeights->setMaximumWidth(150);
    cmbWeights->setMinimumWidth(150);
    QLabel* l=new QLabel(tr("&Weight Model: "), this);
    l->setBuddy(cmbWeights);
    layAlgorithm->addSpacing(32);
    layAlgorithm->addWidget(l);
    layAlgorithm->addWidget(cmbWeights);
    layAlgorithm->addStretch();

    widFitErrorEstimate=new QFFitAlgorithmErrorEstimateModeWidget(this);
    l=new QLabel(tr("&Error Estimation: "), this);
    l->setBuddy(widFitErrorEstimate);
    layAfterAlgorithm->addSpacing(32);
    layAfterAlgorithm->addWidget(l);
    layAfterAlgorithm->addWidget(widFitErrorEstimate);
    layAfterAlgorithm->addStretch();



    spinRepeats=new QSpinBox(this);
    spinRepeats->setRange(1, 100);
    spinRepeats->setValue(1);
    spinRepeats->setToolTip(tr("repeats the fit the given number of times for every click of fit. The output of the first run is used as initial parameters for the second, ..."));
    int row=layButtons->rowCount();
    layButtons->addWidget(new QLabel(tr("repeats:"), this), row, 0);
    layButtons->addWidget(spinRepeats, row, 1);
    connect(spinRepeats, SIGNAL(valueChanged(int)), this, SLOT(repeatsChanged(int)));

    chkDontSaveFitResultMessage=new QCheckBox(tr("don't save fit messages"), this);
    chkDontSaveFitResultMessage->setToolTip(tr("this improves the speed of saving the project significantly!"));
    chkDontSaveFitResultMessage->setChecked(true);
    connect(chkDontSaveFitResultMessage, SIGNAL(toggled(bool)), this, SLOT(dontSaveFitResultMessageChanged(bool)));


    layButtons->addWidget(chkDontSaveFitResultMessage, layButtons->rowCount(),0,1,2);

    chkLeaveoutMasked=new QCheckBox(tr("don't fit masked pixels"), this);
    chkLeaveoutMasked->setToolTip(tr("this improves the speed of saving the project significantly!"));
    chkLeaveoutMasked->setChecked(false);
    connect(chkLeaveoutMasked, SIGNAL(toggled(bool)), this, SLOT(leavoutMasked(bool)));


    layButtons->addWidget(chkLeaveoutMasked, layButtons->rowCount(),0,1,2);

    pltOverview=new QFRDRImageToRunPreview(this);
    pltOverview->setMaskEditable(true);
    pltOverview->setSelectionEditable(true);
    tabResidulas->insertTab(0, pltOverview, tr("Overview"));
    tabResidulas->setCurrentIndex(0);


    actSetParameterFromFile=new QAction(tr("set fit parameter from file ..."), this)    ;
    connect(actSetParameterFromFile, SIGNAL(triggered()), this, SLOT(setFitParameterFromFile()));
    menuTools->addAction(actSetParameterFromFile);
    //menuTools->addAction(menuOverlays);

    splitPlot->setStretchFactor(0,9);
    splitPlot->setStretchFactor(1,3);
    splitPlot->setStretchFactor(2,4);
    splitModel->setStretchFactor(0,1);
    splitModel->setStretchFactor(1,0);

}


void QFImFCSFitEvaluationEditor::connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) {
    QFFitResultsByIndexEvaluationEditorWithWidgets::connectDefaultWidgets(current, old, false);

    QFImFCSFitEvaluation* fcs=qobject_cast<QFImFCSFitEvaluation*>(current);

    if (old!=NULL) {
        disconnect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
        disconnect(widFitErrorEstimate, SIGNAL(parametersChanged()), this, SLOT(errorEstimateModeChanged()));
    }



    if (fcs) {

        dataEventsEnabled=false;
        cmbWeights->setCurrentIndex(current->getProperty("weights", 0).toInt());
        widFitErrorEstimate->readSettings(fcs);
        spinRepeats->setValue(current->getProperty("FIT_REPEATS", 1).toInt());
        dataEventsEnabled=true;

        if (current->propertyExists("PRESET_FIT_MODEL")) {
            cmbModel->setCurrentFitFunction(current->getProperty("PRESET_FIT_MODEL", fcs->getFitFunctionID()).toString());
            current->deleteProperty("PRESET_FIT_MODEL");
        }


    }

    connect(cmbWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(weightsChanged(int)));
    connect(widFitErrorEstimate, SIGNAL(parametersChanged()), this, SLOT(errorEstimateModeChanged()));

    displayModel(true);
    replotData();

}


void QFImFCSFitEvaluationEditor::readSettings() {
    QFFitResultsByIndexEvaluationEditorWithWidgets::readSettings();
}

void QFImFCSFitEvaluationEditor::writeSettings() {
    QFFitResultsByIndexEvaluationEditorWithWidgets::writeSettings();
    settings->getQSettings()->setValue(QString("fitevaleditor_%1%2/weights").arg(current->getType()).arg(current->getID()), cmbWeights->currentIndex());
}

void QFImFCSFitEvaluationEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    disconnect(pltOverview, SIGNAL(currentRunChanged(int)), this, SLOT(overviewRunChanged(int)));
    disconnect(spinRun, SIGNAL(valueChanged(int)), pltOverview, SLOT(setCurrentRun(int)));
    if (eval) {
        dataEventsEnabled=false;
        cmbWeights->setCurrentWeight(eval->getFitDataWeighting());
        spinRepeats->setValue(current->getProperty("FIT_REPEATS", 1).toInt());
        widFitErrorEstimate->readSettings(current);
        chkDontSaveFitResultMessage->setChecked(current->getProperty("dontSaveFitResultMessage", true).toBool());
        chkLeaveoutMasked->setChecked(current->getProperty("LEAVEOUTMASKED", true).toBool());
        pltOverview->setRDR(currentRecord);
        connect(pltOverview, SIGNAL(currentRunChanged(int)), this, SLOT(overviewRunChanged(int)));
        connect(spinRun, SIGNAL(valueChanged(int)), pltOverview, SLOT(setCurrentRun(int)));

        dataEventsEnabled=true;
    }
    QFFitResultsByIndexEvaluationEditorWithWidgets::highlightingChanged(formerRecord, currentRecord);
}

void QFImFCSFitEvaluationEditor::displayModel(bool newWidget) {
    QFFitResultsByIndexEvaluationEditorWithWidgets::displayModel(newWidget);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (newWidget && eval) {
/*        QFFitFunction* ffunc=eval->getFitFunction();
        if (ffunc) {

            /////////////////////////////////////////////////////////////////////////////////////////////
            // create new parameter widgets
            /////////////////////////////////////////////////////////////////////////////////////////////
            btnCalibrateFocalVolume->setEnabled(false);
            bool has_particles=false;
            bool has_wxy=false;
            bool has_tauD=false;
            bool has_gamma=false;
            for (int i=0; i<ffunc->paramCount(); i++) {
                QString id=ffunc->getParameterID(i);
                if ((id.toLower()=="n_particle") || (id.toLower()=="1n_particle")) has_particles=true;
                if ((id.toLower()=="diff_tau1") || (id.toLower()=="diff_tau")) has_tauD=true;
                if ((id.toLower()=="focus_width")) has_wxy=true;
                if ((id.toLower()=="focus_struct_fac")) has_gamma=true;

            }
            btnCalibrateFocalVolume->setEnabled((has_tauD||(has_particles&&has_gamma))&&has_wxy);
        }*/
    }
}

void QFImFCSFitEvaluationEditor::replotData() {


    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    QFFCSWeightingTools* wdata=dynamic_cast<QFFCSWeightingTools*>(current.data());
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();

    if ((!eval)||(!data)) {
        pltData->clearGraphs();
        ds->clear();
        return;
    }

    //qDebug()<<" **** replotData()";
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
    dsres->clear();
    ds->clear();
    dsresh->clear();
    dsresc->clear();

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

    pltData->get_plotter()->set_showKey(chkKey->isChecked());
    pltResiduals->get_plotter()->set_showKey(chkKey->isChecked());
    pltResidualHistogram->get_plotter()->set_showKey(chkKey->isChecked());
    pltResidualCorrelation->get_plotter()->set_showKey(chkKey->isChecked());

    //qDebug()<<"   b "<<t.elapsed()<<" ms";
    t.start();

    //int errorStyle=cmbErrorStyle->currentIndex();
    int plotStyle=cmbPlotStyle->currentIndex();
    //int residualStyle=cmbResidualStyle->currentIndex();

    if (data->getCorrelationN()>0) {
        size_t c_tau=ds->addColumn(data->getCorrelationT(), data->getCorrelationN(), "tau");


        //////////////////////////////////////////////////////////////////////////////////
        // Plot average + error markers
        //////////////////////////////////////////////////////////////////////////////////
        size_t c_mean=0;
        QString graphName="";
        int c_std=-1;
        QString errorName="";
        if (eval->getCurrentIndex()<0) {
            c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
            graphName=tr("\\verb{%1} average").arg(record->getName());
            //c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
            //errorName=tr("stddev");
        } else {
            if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                c_mean=ds->addColumn(data->getCorrelationRun(eval->getCurrentIndex()), data->getCorrelationN(), QString("run"+QString::number(eval->getCurrentIndex())));
                graphName=tr("\\verb{%1} %2").arg(record->getName()).arg(data->getCorrelationRunName(eval->getCurrentIndex()));
                /*if (eval->getFitDataWeighting()==QFImFCSFitEvaluation::RunErrorWeighting) {
                    c_std=ds->addColumn(data->getCorrelationRunError(eval->getCurrentIndex()), data->getCorrelationN(), "cperpixelerror");
                    errorName=tr("per pixel");
                } else {
                    c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                    errorName=tr("stddev");
                }*/
            } else {
                c_mean=ds->addColumn(data->getCorrelationMean(), data->getCorrelationN(), "cmean");
                graphName=tr("\\verb{%1} average").arg(record->getName());
                //c_std=ds->addColumn(data->getCorrelationStdDev(), data->getCorrelationN(), "cstddev");
                //errorName=tr("stddev");
            }
        }
        if (wdata) {
            bool wok=false;
            double* weigm=wdata->allocWeights(&wok, record, eval->getCurrentIndex());
            if (wok && weigm && eval->getFitDataWeighting()!=QFFCSWeightingTools::EqualWeighting) {
                errorName=wdata->dataWeightToName(eval->getFitDataWeighting(), m_runName);
                c_std=ds->addCopiedColumn(weigm, data->getCorrelationN(), QString("cerr_")+wdata->dataWeightToString(eval->getFitDataWeighting()));
                qfFree(weigm);
                weigm=NULL;
            }
            if (weigm) qfFree(weigm);
        }
        JKQTPerrorPlotstyle styl=cmbErrorStyle->getErrorStyle();

        //qDebug()<<"   c "<<t.elapsed()<<" ms";
        t.start();

        JKQTPxyLineErrorGraph* g=new JKQTPxyLineErrorGraph(pltData->get_plotter());
        QColor gcolor=pltData->get_plotter()->get_graphColor();
        g->set_color(gcolor);
        g->set_errorColor(gcolor.lighter());
        QColor errfc=g->get_errorColor();
        errfc.setAlphaF(0.5);
        g->set_errorFillColor(errfc);
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
    //qDebug()<<"  replotData end  runtime = "<<t1.elapsed()<<" ms";

    //setUpdatesEnabled(updEn);
}



void QFImFCSFitEvaluationEditor::updateFitFunctions() {
    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    int run=eval->getCurrentIndex();
    JKQTPdatastore* ds=pltData->getDatastore();
    JKQTPdatastore* dsres=pltResiduals->getDatastore();
    JKQTPdatastore* dsresh=pltResidualHistogram->getDatastore();
    JKQTPdatastore* dsresc=pltResidualCorrelation->getDatastore();
    QFFitFunction* ffunc=eval->getFitFunction();
    //QFImFCSFitEvaluation::DataWeight weighting=eval->getFitDataWeighting();

    if (!ffunc) return;

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

                if (eval->getCurrentIndex()<0) {
                    corrdata=data->getCorrelationMean();
                } else {
                    if (eval->getCurrentIndex()<(int)data->getCorrelationRuns()) {
                        corrdata=data->getCorrelationRun(eval->getCurrentIndex());
                    } else {
                        corrdata=data->getCorrelationMean();
                    }
                }

                double* weights=eval->allocWeights(NULL, record, eval->getCurrentIndex());


                /////////////////////////////////////////////////////////////////////////////////
                // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
                // we are working with a complete set of parameters
                /////////////////////////////////////////////////////////////////////////////////
                double* fullParams=eval->allocFillParameters();
                double* errors=eval->allocFillParameterErrors();
                bool* paramsFix=eval->allocFillFix(record, eval->getCurrentIndex());
                ffunc->calcParameter(fullParams, errors);

                //double detCOV=eval->getFitValue(record, eval->getEvaluationResultID(eval->getCurrentIndex(), record), "fitstat_det_cov");
                double prange_width=eval->getFitValue(record, eval->getEvaluationResultID(eval->getCurrentIndex(), record), "fitstat_bayes_model_probability_paramrangesize");
                //qDebug()<<"read old:   "<<eval->getCurrentIndex()<<eval->getEvaluationResultID(eval->getCurrentIndex(), record);//<<detCOV<<prange_width;
                if (!QFFloatIsOK(prange_width) || fabs(prange_width)<1000.0*DBL_MIN || fabs(prange_width)>1e36) {
                    prange_width=200;
                }

                /////////////////////////////////////////////////////////////////////////////////
                // calculate fit statistics
                /////////////////////////////////////////////////////////////////////////////////
                record->disableEmitResultsChanged();
                QStringList pnames;
                QFFitStatistics fitStatistics=eval->calcFitStatistics(eval->hasFit(record, run), ffunc, N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins, record, run, "fitstat_", tr("fit statistics"), QVector<double>(), prange_width, true, &pnames);

                record->enableEmitResultsChanged();


                size_t c_fit = ds->addCopiedColumn(fitStatistics.fitfunc.data(), N, "fit_model");
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
                for (int i=0; i<(int)ffunc->getAdditionalPlotCount(fullParams); i++) {
                    double* params=eval->allocFillParameters();
                    QString name=ffunc->transformParametersForAdditionalPlot(i, params);
                    double* afitfunc=(double*)qfMalloc(N*sizeof(double));
                    for (int j=0; j<N; j++) {
                        afitfunc[j]=ffunc->evaluate(tauvals[j], params);
                    }
                    size_t c_afit=ds->addCopiedColumn(afitfunc, N, QString("add_fit_model_%1").arg(i));
                    JKQTPxyLineGraph* g_afit=new JKQTPxyLineGraph(pltData->get_plotter());
                    g_afit->set_drawLine(true);
                    g_afit->set_title(name);
                    g_afit->set_xColumn(c_tau);
                    g_afit->set_yColumn(c_afit);
                    g_afit->set_datarange_start(datacut->get_userMin());
                    g_afit->set_datarange_end(datacut->get_userMax());
                    pltData->addGraph(g_afit);
                    qfFree(params);
                    qfFree(afitfunc);
                }
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
                    c_residuals=dsres->addCopiedColumn(fitStatistics.residuals_weighted.data(), N, "residuals_weighted");
                    g_residuals->set_title("weighted residuals");
                } else {
                    c_residuals=dsres->addCopiedColumn(fitStatistics.residuals.data(), N, "residuals");
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
                size_t c_tauresra=dsres->addCopiedColumn(fitStatistics.tau_runavg.data(), fitStatistics.runAvgN, "tau_resid_pixelavg");
                size_t c_residualsra=0;
                JKQTPxyLineGraph* g_residualsra=new JKQTPxyLineGraph(pltResiduals->get_plotter());


                if (chkWeightedResiduals->isChecked()) {
                    c_residualsra=dsres->addCopiedColumn(fitStatistics.residuals_runavg_weighted.data(), fitStatistics.runAvgN, "residuals_pixelavg_weighted");
                    g_residualsra->set_title("weighted residuals, movAvg");
                } else {
                    c_residualsra=dsres->addCopiedColumn(fitStatistics.residuals_runavg.data(), fitStatistics.runAvgN, "residuals_pixelavg");
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
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fitStatistics.rminw+fitStatistics.residHistWBinWidth/2.0, fitStatistics.rmaxw-fitStatistics.residHistWBinWidth/2.0, "residualhist_weighted_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fitStatistics.resWHistogram.data(), residualHistogramBins, "residualhist_weighted_y");
                } else {
                    c_residualHistogramX=dsresh->addLinearColumn(residualHistogramBins, fitStatistics.rmin+fitStatistics.residHistBinWidth/2.0, fitStatistics.rmax-fitStatistics.residHistBinWidth/2.0, "residualhist_x");
                    c_residualHistogramY=dsresh->addCopiedColumn(fitStatistics.resHistogram.data(), residualHistogramBins, "residualhist_y");
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
                size_t c_residualCorrelationX=dsresc->addLinearColumn(fitStatistics.resN-1, 1, fitStatistics.resN-1, "residualcorr_x");
                size_t c_residualCorrelationY=0;
                if (chkWeightedResiduals->isChecked()) {
                    c_residualCorrelationY=dsresc->addCopiedColumn(&(fitStatistics.resWCorrelation[1]), fitStatistics.resN-1, "residualcorr_weighted_y");
                } else {
                    c_residualCorrelationY=dsresh->addCopiedColumn(&(fitStatistics.resCorrelation[1]), fitStatistics.resN-1, "residualcorr_y");
                }
                JKQTPxyLineGraph* g_residualsCorrelation=new JKQTPxyLineGraph(pltResidualCorrelation->get_plotter());
                g_residualsCorrelation->set_xColumn(c_residualCorrelationX);
                g_residualsCorrelation->set_yColumn(c_residualCorrelationY);
                pltResidualCorrelation->addGraph(g_residualsCorrelation);
                //qDebug()<<"    k "<<t.elapsed()<<" ms";
                t.start();

                /////////////////////////////////////////////////////////////////////////////////
                // update display of fit results
                /////////////////////////////////////////////////////////////////////////////////
                QString txtFit="<font face=\"Arial\">";
                QString fitResult=record->resultsGetAsString(eval->getEvaluationResultID(record), "fitalg_messageHTML", eval->getCurrentIndex(), true);

                if (!fitResult.isEmpty()) {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>%1</center></div><br>").arg(fitResult);
                } else {
                    txtFit+=txtFit+tr("<div style=\"border-style:solid\"><b>Fit Result Message:</b><center>not fit yet</center></div><br>");
                }
                txtFit+=QString("<b>%1</b><center>").arg(tr("Fit Statistics:"));
                //txtFit+=QString("<table border=\"0\" width=\"95%\">");

                txtFit+=fitStatistics.getAsHTMLTable(false);
                txtFit+=tr("<br><br>Parameters in matrix:<br><blockquote><tt>%1</tt></blockquote>").arg(pnames.join(", "));
                txtFit+=QString("<br><br>");
                txtFit+=fitStatistics.getHTMLExplanation();

                txtFit+=QString("</center></font>");
                t.start();

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
                qfFree(fullParams);
                qfFree(errors);
                qfFree(weights);
                qfFree(paramsFix);
                fitStatistics.free();

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

















void QFImFCSFitEvaluationEditor::weightsChanged(int /*model*/) {
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    current->setQFProperty("weights", cmbWeights->currentIndex(), false, false);
    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (data) {
        data->setFitDataWeighting(cmbWeights->currentWeight());
        /*if (cmbWeights->currentIndex()==0) data->setFitDataWeighting(QFImFCSFitEvaluation::EqualWeighting);
        else if (cmbWeights->currentIndex()==1) data->setFitDataWeighting(QFImFCSFitEvaluation::StdDevWeighting);
        else if (cmbWeights->currentIndex()==2) data->setFitDataWeighting(QFImFCSFitEvaluation::RunErrorWeighting);
        else data->setFitDataWeighting(QFImFCSFitEvaluation::EqualWeighting);*/
    }
    displayModel(true);
    replotData();
    QApplication::restoreOverrideCursor();
}

void QFImFCSFitEvaluationEditor::dontSaveFitResultMessageChanged(bool checked)
{
    if (!current) return;
    current->setQFProperty("dontSaveFitResultMessage", checked, false, false);
}

void QFImFCSFitEvaluationEditor::leavoutMasked(bool checked)
{
    if (!current) return;
    current->setQFProperty("LEAVEOUTMASKED", checked, false, false);
}

void QFImFCSFitEvaluationEditor::repeatsChanged(int r)
{
    if (!current) return;
    current->setQFProperty("FIT_REPEATS", r, false, false);
}

void QFImFCSFitEvaluationEditor::overviewRunChanged(int run)
{
    spinRun->setValue(run);
}






int QFImFCSFitEvaluationEditor::getUserMin(QFRawDataRecord* rec, int index) {
    return getUserMin(rec, index, datacut->get_userMin());
}

int QFImFCSFitEvaluationEditor::getUserMax(QFRawDataRecord* rec, int index) {
    return getUserMax(rec, index, datacut->get_userMax());
}

int QFImFCSFitEvaluationEditor::getUserMin() {
    return getUserMin(datacut->get_userMin());
}

int QFImFCSFitEvaluationEditor::getUserMax() {
    return getUserMax(datacut->get_userMax());
}

void QFImFCSFitEvaluationEditor::populateFitButtons(bool mulThreadEnabledInModel)
{
    QFFitResultsByIndexEvaluationEditorWithWidgets::populateFitButtons(mulThreadEnabledInModel);

    btnFitAll->addAction(actFitAllFilesThreadedWriter);
    btnFitRunsAll->addAction(actFitAllThreadedWriter);
    btnFitRunsCurrent->addAction(actFitAllRunsThreadedWriter);

    btnFitAll->setDefaultAction(actFitAllFilesThreadedWriter);
    btnFitRunsAll->setDefaultAction(actFitAllThreadedWriter);
    btnFitRunsCurrent->setDefaultAction(actFitAllRunsThreadedWriter);


}






















/////////////////////////////////////////////////////////////////////
// REPORT GENERATION
/////////////////////////////////////////////////////////////////////

void QFImFCSFitEvaluationEditor::createReportDoc(QTextDocument* document) {
    // make sure all widgets ahave the right size
    {   int trci=tabResidulas->currentIndex();
        for (int i=0;i<tabResidulas->count(); i++) {
            tabResidulas->setCurrentIndex(i);
        }
        tabResidulas->setCurrentIndex(trci);
    }

    int PicTextFormat=QTextFormat::UserObject + 1;
    QObject *picInterface = new QPictureTextObject;
    document->documentLayout()->registerHandler(PicTextFormat, picInterface);

    if (!current) return;
    if (!cmbModel) return;
    QFRawDataRecord* record=current->getHighlightedRecord();
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(record);
    QFImFCSFitEvaluation* eval=qobject_cast<QFImFCSFitEvaluation*>(current);
    if (!eval) return;
    if (!fcs) return;
    if (!dataImg) return;

    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* algorithm=eval->getFitAlgorithm();
    //int run=eval->getCurrentIndex();
    double* params=eval->allocFillParameters();

    QTextCursor cursor(document);
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
    QTextBlockFormat bfLeftNB=bfLeft;
    bfLeftNB.setNonBreakableLines(true);
    QTextBlockFormat bfRight;
    bfRight.setAlignment(Qt::AlignRight);
    QTextBlockFormat bfRightNB=bfRight;
    bfRightNB.setNonBreakableLines(true);
    QTextBlockFormat bfCenter;
    bfCenter.setAlignment(Qt::AlignHCenter);
    QTextBlockFormat bfCenterNB=bfCenter;
    bfCenterNB.setNonBreakableLines(true);

    fHeading1.setFontPointSize(2*fText.fontPointSize());
    fHeading1.setFontWeight(QFont::Bold);
    cursor.insertText(tr("FCS Fit Report:\n\n"), fHeading1);
    cursor.movePosition(QTextCursor::End);

    QTextTableFormat tableFormat;
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QTextTable* table = cursor.insertTable(4, 4, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText(tr("file:"), fTextBold);
    table->cellAt(0, 1).firstCursorPosition().insertText(record->getName(), fText);
    table->cellAt(0, 2).firstCursorPosition().insertText(tr("pixel:"), fTextBold);
    QString runname=tr("average");
    int run=eval->getCurrentIndex();
    if (run>=0) runname=QString("[%1, %2]").arg(dataImg->runToX(run)).arg(dataImg->runToY(run));

    table->cellAt(0, 3).firstCursorPosition().insertText(QString("%1").arg(runname), fText);
    table->cellAt(1, 0).firstCursorPosition().insertText(tr("fit algorithm:"), fTextBold);
    table->cellAt(1, 1).firstCursorPosition().insertText(algorithm->name(), fText);
    table->cellAt(1, 2).firstCursorPosition().insertText(tr("data range:"), fTextBold);
    table->cellAt(1, 3).firstCursorPosition().insertText(tr("%1 ... %2 / %3 ... %4").arg(datacut->get_userMin()).arg(datacut->get_userMax()).arg(datacut->get_min()).arg(datacut->get_max()), fText);
    table->cellAt(2, 0).firstCursorPosition().insertText(tr("fit function:"), fTextBold);
    table->cellAt(2, 1).firstCursorPosition().insertText(ffunc->name(), fText);
    table->cellAt(2, 2).firstCursorPosition().insertText(tr("data weighting:"), fTextBold);
    table->cellAt(2, 3).firstCursorPosition().insertText(cmbWeights->currentText(), fText);
    table->cellAt(3, 2).firstCursorPosition().insertText(tr("error estimates:"), fTextBold);
    table->cellAt(3, 3).firstCursorPosition().insertText(widFitErrorEstimate->toString(), fText);
    cursor.movePosition(QTextCursor::End);

    cursor.insertBlock(); cursor.insertBlock();
    cursor.insertText(tr("Plots:\n"), fTextBold);
    QTextTableFormat tableFormat1;
    tableFormat1.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat1.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::PercentageLength, 66);
    constraints << QTextLength(QTextLength::PercentageLength, 34);
    tableFormat1.setColumnWidthConstraints(constraints);
    table = cursor.insertTable(2, 2, tableFormat1);
    table->mergeCells(0,0,2,1);
    {
        QTextCursor tabCursor=table->cellAt(0, 0).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltData->get_plotter()->draw(*painter, QRect(0,0,pltData->width(),pltData->width()));
        delete painter;
        double scale=document->textWidth()*0.62/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("correlation data, model and residuals:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.62);
    }
    QApplication::processEvents();

    {
        QTextCursor tabCursor=table->cellAt(0, 1).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltResidualHistogram->get_plotter()->draw(*painter, QRect(0,0,pltResidualHistogram->width(),pltResidualHistogram->width()));
        delete painter;
        double scale=document->textWidth()*0.3/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual histogram:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
    }
    QApplication::processEvents();
    {
        QTextCursor tabCursor=table->cellAt(1, 1).firstCursorPosition();
        QPicture pic;
        JKQTPEnhancedPainter* painter=new JKQTPEnhancedPainter(&pic);
        pltResidualCorrelation->get_plotter()->draw(*painter, QRect(0,0,pltResidualCorrelation->width(),pltResidualCorrelation->width()));
        delete painter;
        double scale=document->textWidth()*0.3/pic.boundingRect().width();
        if (scale<=0) scale=1;
        tabCursor.insertText(tr("residual correlations:\n"), fTextBoldSmall);
        insertQPicture(tabCursor, PicTextFormat, pic, QSizeF(pic.boundingRect().width(), pic.boundingRect().height())*scale, 0.3);
    }
    cursor.movePosition(QTextCursor::End);
    QApplication::processEvents();

    int fitParamCount=0;
    for (int i=0; i<ffunc->paramCount(); i++) {
        if (ffunc->isParameterVisible(i, params)) fitParamCount++;
    }
    QApplication::processEvents();

    cursor.insertBlock(); cursor.insertBlock();
    if (eval->hasFit()) cursor.insertText(tr("Model Parameters (fit results):"), fTextBold);
    else cursor.insertText(tr("Model Parameters (initial values):"), fTextBold);
    QTextTableFormat tableFormat2;
    tableFormat2.setBorderStyle(QTextFrameFormat::BorderStyle_None);
    tableFormat2.setWidth(QTextLength(QTextLength::PercentageLength, 98));
    table = cursor.insertTable(ceil((double)fitParamCount/2.0)+1, 12, tableFormat2);
    QTextCursor tableCursor;
    QApplication::processEvents();

    tableCursor=table->cellAt(0, 0).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 2).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 5).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();

    tableCursor=table->cellAt(0, 6).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Parameter"), fTextBoldSmall);
    tableCursor=table->cellAt(0, 8).firstCursorPosition();
    tableCursor.setBlockFormat(bfRight);
    tableCursor.insertText(tr("Value"), fTextBoldSmall);
    if (algorithm->get_supportsBoxConstraints()) table->cellAt(0, 11).firstCursorPosition().insertText(tr("Range"), fTextBoldSmall);
    QApplication::processEvents();


    int rowStart=1;
    int colStart=0;
    for (int i=0; i<ffunc->paramCount(); i++) {
        QApplication::processEvents();
        QString id=ffunc->getParameterID(i);
        double error=roundError(eval->getFitError(id),2);
        double value=roundWithError(eval->getFitValue(id), error, 2);
        QString value_string=floattohtmlstr(value, 5, true).c_str();
        bool fix=eval->getFitFix(id);
        QFFitFunction::ParameterDescription d=ffunc->getDescription(id);
        QString range=QString("%1...%2").arg(QString(floattohtmlstr(d.minValue, 5, true).c_str())).arg(QString(floattohtmlstr(d.maxValue, 5, true).c_str()));
        if ((d.widgetType==QFFitFunction::IntCombo)&&((int)value>=0)&&((int)value<d.comboItems.size())) {
            value_string="<i>"+d.comboItems[(int)value]+"</i>";
        }
        if (ffunc->isParameterVisible(i, params)) {
            QString err="";
            if (d.displayError!=QFFitFunction::NoError) {
                err=QString("&plusmn;&nbsp;%1").arg(QString(floattohtmlstr(error, 5, true).c_str()));
            }

            tableCursor=table->cellAt(rowStart, colStart).firstCursorPosition();
            tableCursor.setBlockFormat(bfRightNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(d.label));
            tableCursor.insertText(" = ", fTextSmall);

            tableCursor=table->cellAt(rowStart, colStart+1).firstCursorPosition();
            tableCursor.setBlockFormat(bfLeftNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            if (d.fit) {
                tableCursor.insertText(tr("F"), fTextSmall);
            }
            if (!d.userEditable) {
                tableCursor.insertText(tr("C"), fTextSmall);
            }
            if (fix) {
                tableCursor.insertText(tr("X"), fTextSmall);
            }


            tableCursor=table->cellAt(rowStart, colStart+2).firstCursorPosition();
            tableCursor.setBlockFormat(bfRightNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(value_string)));

            tableCursor=table->cellAt(rowStart, colStart+3).firstCursorPosition();
            tableCursor.setBlockFormat(bfLeftNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(err)));

            tableCursor=table->cellAt(rowStart, colStart+4).firstCursorPosition();
            tableCursor.setBlockFormat(bfLeftNB);
            tableCursor.setBlockCharFormat(fTextSmall);
            tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(d.unitLabel)));

            if (algorithm->get_supportsBoxConstraints()) {
                tableCursor=table->cellAt(rowStart, colStart+5).firstCursorPosition();
                tableCursor.setBlockFormat(bfLeftNB);
                tableCursor.setBlockCharFormat(fTextSmall);
                tableCursor.insertFragment(QTextDocumentFragment::fromHtml(QString("<nobr>%1</nobr>").arg(range)));
            }
            rowStart++;
        };
        if (rowStart>=table->rows()) {
            rowStart=1;
            colStart+=6;
        }
    }
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.setBlockFormat(bfCenterNB);
    cursor.setBlockCharFormat(fTextSmall);
    cursor.insertFragment(QTextDocumentFragment::fromHtml(tr("<i><u>legend:</u> <b>F</b>: fit parameter, <b>X</b>: fixed parameter, <b>C</b>: calculated parameter</i>")));
    QApplication::processEvents();

    cursor.setBlockFormat(bfLeft);
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock(); cursor.insertBlock();
    QString htmlBot=fitStatisticsReport;
    htmlBot.replace("width=\"95%\"", "");
    cursor.insertFragment(QTextDocumentFragment::fromHtml(htmlBot));

}

void QFImFCSFitEvaluationEditor::setFitParameterFromFile()
{
    if (!dataEventsEnabled) return;
    if (!current) return;
    if (!current->getHighlightedRecord()) return;

    QFImFCSFitEvaluation* data=qobject_cast<QFImFCSFitEvaluation*>(current);
    QFRawDataRecord* record=data->getHighlightedRecord();
    QFRDRImageToRunInterface* dataImg=qobject_cast<QFRDRImageToRunInterface*>(record);
    if (data && dataImg) {
        QFFitFunction* ff=data->getFitFunction();
        double* d=data->allocFillParameters(record, data->getCurrentIndex(), ff);
        if (ff) {
            QStringList params;
            QStringList paramIDs;
            QStringList sl=ff->getParameterIDs();
            for (int i=0; i<sl.size(); i++) {
                if (ff->isParameterVisible(i, d)) {
                    params.append(ff->getDescription(sl[i]).name);
                    paramIDs<<sl[i];
                }
            }

            int w=dataImg->getImageFromRunsWidth();
            int h=dataImg->getImageFromRunsHeight();
            QFImFCSSetParamFromFileDialog* dlg=new QFImFCSSetParamFromFileDialog(w, h, data, params, paramIDs, this);
            if (dlg->exec()) {
                bool ok=false;
                QVector<double> d=dlg->getData(&ok);
                if (d.size()>w*h) {
                    int tooMany=d.size()-w*h;
                    //d.remove(s.size()-tooMany, tooMany);
                    d.resize(w*h);
                }
                double avg=qfstatisticsAverage(d);
                QString param=dlg->getParameter();
                if (ok && d.size()<w*h) {
                    ok=(QMessageBox::question(this, tr("set fit result from RDR/file/..."), tr("The size of read data is smaller than the required data size (new data size: %4,  required size: %1x%2 = %3).\n  Import data nevertheless?").arg(w).arg(h).arg(w*h).arg(d.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes);
                }
                //qDebug()<<ok<<param<<avg<<d.size()-1<<data->getIndexMax(current->getHighlightedRecord())<<d.value(0,0);
                if (ok) {// && d.size()-1==data->getIndexMax(current->getHighlightedRecord())) {
                    data->setFitResultValue(record,data->getEvaluationResultID(0, record), param, d);
                    data->setFitResultValue(record,data->getEvaluationResultID(-1, record), param, avg);
                    parameterValueChanged();
                }  else {
                    QMessageBox::information(this, tr("set fit result from RDR/file/..."), tr("An internal error occured, fit parameter value could not be set."));
                }
            }
            delete dlg;
        }
        if (d) qfFree(d);
    }
}

void QFImFCSFitEvaluationEditor::errorEstimateModeChanged()
{
    if (!current) return;
    widFitErrorEstimate->saveSettings(current);
}








































void QFImFCSFitEvaluationEditor::fitEverythingThreadedWriter() {
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg->isThreadSafe()) {
        fitRunsAll();
        return;
    }
    if ((!ffunc)||(!falg)) return;

    dlgQFProgressDialog* dlgTFitProgress=new dlgQFProgressDialog(this);
    dlgTFitProgress->reportTask(tr("fit all files and all %3s therein<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()).arg(m_runName));
    dlgTFitProgress->setProgressMax(100);
    dlgTFitProgress->setProgress(0);
    dlgTFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgTFitProgress->display();


    dlgTFitProgress->reportStatus("creating thread objects ...");
    QApplication::processEvents();
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    QList<QFFitResultsByIndexEvaluationFitSmartThread*> threads;
    QFFitResultsByIndexEvaluationFitSmartThread_Writer* writerthread=new QFFitResultsByIndexEvaluationFitSmartThread_Writer(current->getProject(), this);
    int threadcount=qMax(2,ProgramOptions::getInstance()->getMaxThreads());
    if (ProgramOptions::getConfigValue(eval->getType()+"/overrule_threads", false).toBool()) {
        threadcount=qMax(2,ProgramOptions::getConfigValue(eval->getType()+"/threads", 1).toInt());
    }
    for (int i=0; i<threadcount; i++) {
        QFFitResultsByIndexEvaluationFitSmartThread* t=new QFFitResultsByIndexEvaluationFitSmartThread(writerthread, true, this);
        connect (t, SIGNAL(sigLogError(QString)), this, SLOT(log_error(QString)));
        connect (t, SIGNAL(sigLogText(QString)), this, SLOT(log_text(QString)));
        connect (t, SIGNAL(sigLogWarning(QString)), this, SLOT(log_warning(QString)));
        threads.append(t);
    }


    // enqueue the jobs in the several threads and finally make sure they know when to stop
    dlgTFitProgress->reportStatus("distributing jobs ...");
    QApplication::processEvents();
    int items=0;
    int thread=0;
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);

        if (record ) {
            record->disableEmitResultsChanged();
            int runmax=eval->getIndexMax(record);
            int runmin=eval->getIndexMin(record);
            items=items+runmax-runmin+1;
            for (int run=runmin; run<=runmax; run++) {
                bool doall=!current->getProperty("leaveoutMasked", false).toBool();
                if (run<=runmax && (doall || (!doall && rsel && !rsel->leaveoutRun(run)))) {
                    threads[thread]->addJob(eval, record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()));
                    thread++;
                    if (thread>=threadcount) thread=0;
                }
            }
        }
        QApplication::processEvents();
    }
    dlgTFitProgress->setProgressMax(items+5);


    // start all threads and wait for them to finish
    writerthread->start();
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
    }
    writerthread->cancel();
    delete writerthread;

    dlgTFitProgress->reportStatus(tr("fit done ... updating user interface\n"));
    dlgTFitProgress->setProgress(items+2);
    QApplication::processEvents();

    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        if (record ) {
            record->enableEmitResultsChanged(true);
        }
    }
    current->emitResultsChanged();

    dlgTFitProgress->setProgress(items+3);
    QApplication::processEvents();
    displayModel(false);
    replotData();
    dlgTFitProgress->setProgress(items+5);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
    dlgTFitProgress->done();
    delete dlgTFitProgress;
}


void QFImFCSFitEvaluationEditor::fitAllRunsThreadedWriter() {
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg->isThreadSafe()) {
        fitRunsCurrent();
        return;
    }
    if ((!ffunc)||(!falg)) return;

    dlgQFProgressDialog* dlgTFitProgress=new dlgQFProgressDialog(this);
    dlgTFitProgress->reportTask(tr("fit all %3s in the current file<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()).arg(m_runName));
    dlgTFitProgress->setProgressMax(100);
    dlgTFitProgress->setProgress(0);
    dlgTFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgTFitProgress->display();


    dlgTFitProgress->reportStatus("creating thread objects ...");
    QApplication::processEvents();
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    QList<QFFitResultsByIndexEvaluationFitSmartThread*> threads;
    QFFitResultsByIndexEvaluationFitSmartThread_Writer* writerthread=new QFFitResultsByIndexEvaluationFitSmartThread_Writer(current->getProject(), this);
    int threadcount=qMax(2,ProgramOptions::getInstance()->getMaxThreads());
    if (ProgramOptions::getConfigValue(eval->getType()+"/overrule_threads", false).toBool()) {
        threadcount=qMax(2,ProgramOptions::getConfigValue(eval->getType()+"/threads", 1).toInt());
    }
    for (int i=0; i<threadcount; i++) {
        threads.append(new QFFitResultsByIndexEvaluationFitSmartThread(writerthread,  true, this));
    }


    // enqueue the jobs in the several threads and finally make sure they know when to stop
    dlgTFitProgress->reportStatus("distributing jobs ...");
    QApplication::processEvents();
    int items=0;
    int thread=0;
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
    writerthread->start();
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
    }
    writerthread->cancel();
    delete writerthread;


    dlgTFitProgress->reportStatus(tr("fit done ... updating user interface\n"));
    dlgTFitProgress->setProgress(items+2);
    QApplication::processEvents();

    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        if (record ) {
            record->enableEmitResultsChanged(true);
        }
    }
    current->emitResultsChanged();

    dlgTFitProgress->setProgress(items+3);
    QApplication::processEvents();
    displayModel(false);
    replotData();
    dlgTFitProgress->setProgress(items+5);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
    dlgTFitProgress->done();
    delete dlgTFitProgress;
}

void QFImFCSFitEvaluationEditor::fitAllFilesThreadedWriter()
{
    if (!current) return;
    if (!cmbModel) return;
    QFFitResultsByIndexEvaluation* eval=qobject_cast<QFFitResultsByIndexEvaluation*>(current);
    if (!eval) return;
    QFFitFunction* ffunc=eval->getFitFunction();
    QFFitAlgorithm* falg=eval->getFitAlgorithm();
    if (!falg->isThreadSafe()) {
        fitAll();
        return;
    }
    if ((!ffunc)||(!falg)) return;

    dlgQFProgressDialog* dlgTFitProgress=new dlgQFProgressDialog(this);
    dlgTFitProgress->reportTask(tr("fit the current %3 in all files<br>using model '%1'<br>and algorithm '%2' \n").arg(ffunc->name()).arg(falg->name()).arg(m_runName));
    dlgTFitProgress->setProgressMax(100);
    dlgTFitProgress->setProgress(0);
    dlgTFitProgress->setAllowCancel(true);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dlgTFitProgress->display();


    dlgTFitProgress->reportStatus("creating thread objects ...");
    QApplication::processEvents();
    QList<QPointer<QFRawDataRecord> > recs=eval->getApplicableRecords();
    QList<QFFitResultsByIndexEvaluationFitSmartThread*> threads;
    QFFitResultsByIndexEvaluationFitSmartThread_Writer* writerthread=new QFFitResultsByIndexEvaluationFitSmartThread_Writer(current->getProject(), this);
    int threadcount=qMax(2,ProgramOptions::getInstance()->getMaxThreads());
    if (ProgramOptions::getConfigValue(eval->getType()+"/overrule_threads", false).toBool()) {
        threadcount=qMax(2,ProgramOptions::getConfigValue(eval->getType()+"/threads", 1).toInt());
    }
    for (int i=0; i<threadcount; i++) {
        threads.append(new QFFitResultsByIndexEvaluationFitSmartThread(writerthread,  true, this));
    }


    // enqueue the jobs in the several threads and finally make sure they know when to stop
    dlgTFitProgress->reportStatus("distributing jobs ...");
    QApplication::processEvents();
    int items=0;
    int thread=0;
    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        QFRDRRunSelectionsInterface* rsel=qobject_cast<QFRDRRunSelectionsInterface*>(record);

        if (record ) {
            record->disableEmitResultsChanged();
            items=items+1;
            int run=eval->getCurrentIndex();
            bool doall=!current->getProperty("leaveoutMasked", false).toBool();
            if (doall || (!doall && rsel && !rsel->leaveoutRun(run))) {
                threads[thread]->addJob(eval, record, run, getUserMin(record, run, datacut->get_userMin()), getUserMax(record, run, datacut->get_userMax()));
                thread++;
                if (thread>=threadcount) thread=0;
            }
        }
    }
    dlgTFitProgress->setProgressMax(items+5);


    // start all threads and wait for them to finish
    writerthread->start();
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
    }
    writerthread->cancel();
    delete writerthread;


    dlgTFitProgress->reportStatus(tr("fit done ... updating user interface\n"));
    dlgTFitProgress->setProgress(items+2);
    QApplication::processEvents();

    for (int i=0; i<recs.size(); i++) {
        QFRawDataRecord* record=recs[i];
        if (record ) {
            record->enableEmitResultsChanged(true);
        }
    }
    current->emitResultsChanged();

    dlgTFitProgress->setProgress(items+3);
    QApplication::processEvents();
    displayModel(false);
    replotData();
    dlgTFitProgress->setProgress(items+5);
    QApplication::processEvents();
    QApplication::restoreOverrideCursor();
    dlgTFitProgress->done();
    delete dlgTFitProgress;
}
