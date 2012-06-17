#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"


QFFCSFitEvaluation::QFFCSFitEvaluation(QFProject* parent):
    QFFitResultsByIndexEvaluation("fcs_,dls_,fccs_", parent)
{

    m_weighting=EqualWeighting;
    m_currentIndex=-1;

    if (m_fitFunctions.contains("fcs_diff")) {
        m_fitFunction="fcs_diff";
    } else if (m_fitFunctions.contains("fcs_diff1")) {
        m_fitFunction="fcs_diff1";
    } else if (m_fitFunctions.contains("fcs_multidiff")) {
        m_fitFunction="fcs_multidiff";
    }

    if (m_fitAlgorithms.contains("fit_levmar")) {
        m_fitAlgorithm="fit_levmar";
    }

}

QFFCSFitEvaluation::~QFFCSFitEvaluation() {
}


QString QFFCSFitEvaluation::getIndexName(QFRawDataRecord *rec, int index) {
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(rec);
    if (fcs) {
        return fcs->getCorrelationRunName(index);
    }
    return QFFitResultsByIndexEvaluation::getIndexName(rec, index);
}

void QFFCSFitEvaluation::intWriteDataAlgorithm(QXmlStreamWriter& w) const {
    if (m_weighting==EqualWeighting) w.writeAttribute("weighting", "equal");
    if (m_weighting==StdDevWeighting) w.writeAttribute("weighting", "stddev");
    if (m_weighting==RunErrorWeighting) w.writeAttribute("weighting", "runerror");
}

void QFFCSFitEvaluation::intReadDataAlgorithm(QDomElement& e) {
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        m_weighting=EqualWeighting;
        if (a=="stddev") m_weighting=StdDevWeighting;
        if (a=="runerror") m_weighting=RunErrorWeighting;
    }
}

QFEvaluationEditor* QFFCSFitEvaluation::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFFCSFitEvaluationEditor(services, propEditor, parent);
};

bool QFFCSFitEvaluation::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}

bool QFFCSFitEvaluation::hasSpecial(QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) {
    int run=getIndexFromEvaluationResultID(id);
    if (paramid=="count_rate") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf) {
            error=crintf->getRateStdDev(run)*1000;
            value=crintf->getRateMean(run)*1000;
            return true;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(run);
            error=scrintf->getSimpleCountrateVariance(run);
            return true;
        }

    } else if (paramid=="count_rate1") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf && crintf->getRateChannels()>0) {
            error=crintf->getRateStdDev(run, 0)*1000.0;
            value=crintf->getRateMean(run, 0)*1000.0;
            //qDebug()<<"getRateMean(run="<<run<<", ch=0) = "<<value<<" +/- "<<error;
            return true;
        }
    } else if (paramid=="count_rate2") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf && crintf->getRateChannels()>1) {
            error=crintf->getRateStdDev(run, 1)*1000.0;
            value=crintf->getRateMean(run, 1)*1000.0;
            //qDebug()<<"getRateMean(run="<<run<<", ch=1) = "<<value<<" +/- "<<error;
            return true;
        }
    }

    return false;
}

int QFFCSFitEvaluation::getIndexMin(QFRawDataRecord* r) {
    return -1;
}

int QFFCSFitEvaluation::getIndexMax(QFRawDataRecord* r) {
    if (!r) return -1;
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (!fcs) return 0;
    if (fcs->getCorrelationRuns()<=0) return -1;
    else return fcs->getCorrelationRuns()-1;
}



















/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////

double* QFFCSFitEvaluation::allocWeights(bool* weightsOKK, QFRawDataRecord* record_in, int run_in, int data_start, int data_end) {
    if (weightsOKK) *weightsOKK=false;
    QFRawDataRecord* record=record_in;
    if (!record_in) record=getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    //JKQTPdatastore* ds=pltData->getDatastore();
    //JKQTPdatastore* dsres=pltResiduals->getDatastore();
    //QFFitFunction* ffunc=getFitFunction();
    int run=run_in;
    if (run<=-100) run=getCurrentIndex();

    int N=data->getCorrelationN();

    QFFCSFitEvaluation::DataWeight weighting=getFitDataWeighting();
    double* weights=(double*)malloc(N*sizeof(double));
    bool weightsOK=false;
    if (weighting==QFFCSFitEvaluation::StdDevWeighting) {
        double* std=data->getCorrelationStdDev();
        weightsOK=true;
        for (int i=0; i<N; i++) {
            weights[i]=std[i];
            if ((data_start>=0) && (data_end>=0)) {
                if ((i>=data_start)&&(i<=data_end)) {
                    if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                        weightsOK=false;
                        break;
                    }
                };
            } else {
                if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                    weightsOK=false;
                    break;
                };
            }
        }
    }
    if (weighting==QFFCSFitEvaluation::RunErrorWeighting) {
        double* std=NULL;
        if (run>=0) std=data->getCorrelationRunError(run);
        else std=data->getCorrelationStdDev();
        weightsOK=true;
        for (int i=0; i<N; i++) {
            weights[i]=std[i];
            if ((data_start>=0) && (data_end>=0)) {
                if ((i>=data_start)&&(i<=data_end)) {
                    if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                        weightsOK=false;
                        break;
                    }
                };
            } else {
                if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                    weightsOK=false;
                    break;
                };
            }
        }
    }
    if (!weightsOK) {
        for (int i=0; i<N; i++) weights[i]=1;
        if (weighting==QFFCSFitEvaluation::EqualWeighting) weightsOK=true;
    }
    if (weightsOKK) *weightsOKK=weightsOK;
    return weights;
}

void QFFCSFitEvaluation::doFit(QFRawDataRecord* record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter* dlgFitProgress) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFitFunction* ffunc=getFitFunction();
    QFFitAlgorithm* falg=getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;

    int rangeMinDatarange=0;
    int rangeMaxDatarange=data->getCorrelationN();
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    restoreQFFitAlgorithmParameters(falg);

    //QFFCSFitEvaluation::DataWeight weighting=getFitDataWeighting();

    if (data->getCorrelationN()>0) {
        falg->setReporter(dlgFitProgress);
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);
        dlgFitProgress->reportStatus(tr("setting up ..."));
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setProgress(0);

        getProject()->getServices()->log_text(tr("running fit with '%1' (%2) and model '%3' (%4) on raw data record '%5', run %6 ... \n").arg(falg->name()).arg(falg->id()).arg(ffunc->name()).arg(ffunc->id()).arg(record->getName()).arg(runname));

        long N=data->getCorrelationN();
        double* weights=NULL;
        double* taudata=data->getCorrelationT();
        double* corrdata=NULL;
        if (run<0) {
            corrdata=data->getCorrelationMean();
        } else {
            if (run<(int)data->getCorrelationRuns()) {
                corrdata=data->getCorrelationRun(run);
            } else {
                corrdata=data->getCorrelationMean();
            }
        }
        // we also have to care for the data cutting
        int cut_low=rangeMinDatarange;
        int cut_up=rangeMaxDatarange;
        if (cut_low<0) cut_low=0;
        if (cut_up>=N) cut_up=N-1;
        int cut_N=cut_up-cut_low+1;
        if (cut_N<0) {
            cut_low=0;
            cut_up=ffunc->paramCount()-1;
            if (cut_up>=N) cut_up=N-1;
            cut_N=cut_up+1;
        }

        /*QString dt, dc;
        for (int i=cut_low; i<=cut_up; i++) {
            dt+=QString(", %1").arg(taudata[i]);
            dc+=QString(", %1").arg(corrdata[i]);
        }
        getProject()->getServices()->log_text(tr("   - tau:  (%1)\n").arg(dt));
        getProject()->getServices()->log_text(tr("   - corr: (%1)\n").arg(dc));*/


        getProject()->getServices()->log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(cut_low).arg(cut_up).arg(cut_N));
        bool weightsOK=false;
        weights=allocWeights(&weightsOK, record, run, cut_low, cut_up);
        if (!weightsOK) getProject()->getServices()->log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));

        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=allocFillParameters(record, run);
        double* initialparams=allocFillParameters(record, run);
        double* errors=allocFillParameterErrors(record, run);
        double* paramsMin=allocFillParametersMin();
        double* paramsMax=allocFillParametersMax();
        bool* paramsFix=allocFillFix(record, run);



        try {
            set_doEmitPropertiesChanged(false);
            set_doEmitResultsChanged(false);
            record->disableEmitResultsChanged();

            ffunc->calcParameter(params, errors);
            ffunc->calcParameter(initialparams, errors);

            QString iparams="";
            QString oparams="";
            QString orparams="";
            int fitparamcount=0;
            for (int i=0; i<ffunc->paramCount(); i++) {
                if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                    if (!iparams.isEmpty()) iparams=iparams+";  ";
                    fitparamcount++;
                    iparams=iparams+QString("%1 = %2").arg(ffunc->getDescription(i).id).arg(params[i]);
                }
                //printf("  fit: %s = %lf +/m %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
            }


            if (cut_N>fitparamcount) {

                bool OK=false;

                if (dlgFitProgress) {
                    if (!dlgFitProgress->isCanceled()) {

                        dlgFitProgress->reportStatus(tr("fitting ..."));
                        dlgFitProgress->setProgressMax(100);
                        dlgFitProgress->setProgress(0);
                        doFitThread->init(falg, params, errors, &taudata[cut_low], &corrdata[cut_low], &weights[cut_low], cut_N, ffunc, initialparams, paramsFix, paramsMin, paramsMax);
                        doFitThread->start(QThread::HighPriority);
                        QTime t;
                        t.start();
                        while (!doFitThread->isFinished()) {
                            if (t.elapsed()>10) {
                                QApplication::processEvents(QEventLoop::AllEvents, 50);
                                if (dlgFitProgress->isCanceled()) {
                                  doFitThread->terminate();
                                  break;
                                }
                                t.start();
                            }
                        }
                        dlgFitProgress->setProgressFull();
                        dlgFitProgress->reportStatus(tr("calculating parameters, errors and storing data ..."));
                        OK=!dlgFitProgress->isCanceled();
                    }
                } else {
                    doFitThread->init(falg, params, errors, &taudata[cut_low], &corrdata[cut_low], &weights[cut_low], cut_N, ffunc, initialparams, paramsFix, paramsMin, paramsMax);
                    doFitThread->start(QThread::HighPriority);
                    QTime t;
                    t.start();
                    while (!doFitThread->isFinished()) {
                        if (t.elapsed()>10) {
                            QApplication::processEvents(QEventLoop::AllEvents, 50);
                            t.start();
                        }
                    }
                    OK=true;
                }
                //dlgFitProgress->setAllowCancel(false);


                if (OK) {
                    record->disableEmitResultsChanged();

                    QFFitAlgorithm::FitResult result=doFitThread->getResult();
                    ffunc->calcParameter(params, errors);
                    ffunc->sortParameter(params, errors);
                    ffunc->calcParameter(params, errors);

                    for (int i=0; i<ffunc->paramCount(); i++) {
                        if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                            if (!oparams.isEmpty()) oparams=oparams+";  ";

                            oparams=oparams+QString("%1 = %2+/-%3").arg(ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                        }
                        //printf("  fit: %s = %lf +/- %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                    }

                    // round errors and values
                    for (int i=0; i<ffunc->paramCount(); i++) {
                        errors[i]=roundError(errors[i], 2);
                        params[i]=roundWithError(params[i], errors[i], 2);
                    }
                    setFitResultValuesVisibleWithGroupAndLabel(record, run, params, errors, tr("fit results"), paramsFix, tr("fit results"), true);

                    for (int i=0; i<ffunc->paramCount(); i++) {
                        if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                            if (!orparams.isEmpty()) orparams=orparams+";  ";
                            orparams=orparams+QString("%1 = %2+/-%3").arg(ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                        }
                        //printf("  fit: %s = %lf +/- %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                    }

                    getProject()->getServices()->log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(doFitThread->getDeltaTime()).arg(result.fitOK?tr("success"):tr("no convergence")));
                    getProject()->getServices()->log_text(tr("   - result-message: %1\n").arg(result.messageSimple));
                    getProject()->getServices()->log_text(tr("   - initial params         (%1)\n").arg(iparams));
                    getProject()->getServices()->log_text(tr("   - output params          (%1)\n").arg(oparams));
                    getProject()->getServices()->log_text(tr("   - output params, rounded (%1)\n").arg(orparams));


                    QString evalID=getEvaluationResultID(ffunc->id(), run);
                    QString param;
                    QString group="fit properties";
                    QString egroup=QString("%1%2__%3__%4").arg(getType()).arg(getID()).arg(falg->id()).arg(ffunc->id());
                    //QString egrouplabel=QString("%1%2: %3, %4").arg(getType()).arg(getID()).arg(falg->shortName()).arg(ffunc->shortName());
                    QString egrouplabel=QString("#%4 \"%1\": %2, %3").arg(getName()).arg(falg->shortName()).arg(ffunc->shortName()).arg(getID());


                    record->resultsSetEvaluationGroup(evalID, egroup);
                    record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                    record->resultsSetEvaluationGroupIndex(evalID, run);
                    record->resultsSetEvaluationDescription(evalID, QString(""));

                    record->resultsSetInteger(evalID, param="fit_used_run", run);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: used runs"));

                    record->resultsSetString(evalID, "fit_model_name", ffunc->id());
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: model"));

                    record->resultsSetString(evalID, "fitalg_name", falg->id());
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: algorithm"));

                    record->resultsSetNumber(evalID, "fitalg_runtime", doFitThread->getDeltaTime(), "msecs");
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: runtime"));

                    record->resultsSetBoolean(evalID, "fitalg_success", result.fitOK);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: success"));

                    record->resultsSetString(evalID, "fitalg_message", result.messageSimple);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: message"));

                    record->resultsSetString(evalID, "fitalg_messageHTML", result.message);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: message (markup)"));

                    record->resultsSetInteger(evalID, "fit_datapoints", cut_N);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: datapoints"));

                    record->resultsSetInteger(evalID, "fit_cut_low", cut_low);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: first point"));

                    record->resultsSetInteger(evalID, "fit_cut_up", cut_up);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: last point"));


                    QMapIterator<QString, QFRawDataRecord::evaluationResult> it(result.params);
                    while (it.hasNext()) {
                        it.next();
                        record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                        record->resultsSetGroup(evalID, param, group);
                        record->resultsSetLabel(evalID, param, it.value().label, it.value().label_rich);
                    }


                    {
                        QFFitStatistics fit_stat=calcFitStatistics(ffunc, N, taudata, corrdata, weights, cut_low, cut_up, params, errors, paramsFix, 11, 25, record, run);
                        fit_stat.free();
                    }

                    record->enableEmitResultsChanged(false);
                    //emit resultsChanged();
                } else {
                    getProject()->getServices()->log_warning(tr("   - fit canceled by user!!!\n"));
                }
            } else {
                getProject()->getServices()->log_error(tr("   - there are not enough datapoints for the fit (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount));
            }
            set_doEmitPropertiesChanged(true);
            set_doEmitResultsChanged(true);
            record->enableEmitResultsChanged(false);
            //emitPropertiesChanged();
            //emitResultsChanged();
        } catch(std::exception& E) {
            getProject()->getServices()->log_error(tr("error during fitting, error message: %1\n").arg(E.what()));
        }

        // clean temporary parameters
        //delete doFitThread;
        free(weights);
        free(params);
        free(initialparams);
        free(errors);
        free(paramsFix);
        free(paramsMax);
        free(paramsMin);

        //displayModel(false);
        //replotData();
        //QApplication::restoreOverrideCursor();
        //dlgFitProgress->done();
        falg->setReporter(NULL);
    }
}


QFFitStatistics QFFCSFitEvaluation::calcFitStatistics(QFFitFunction* ffunc, long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errors, bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record, int run) {
    QFFitStatistics result= ffunc->calcFitStatistics(N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins);

    if (record) {
        if (hasFit(record, run)) {
            QString param="";
            QString eid= getEvaluationResultID(run);
            setFitResultValue(record, eid, param="fitstat_chisquared", result.residSqrSum);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

            setFitResultValue(record, eid, param="fitstat_chisquared_weighted", result.residWeightSqrSum);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

            setFitResultValue(record, eid, param="fitstat_residavg", result.residAverage);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("residual average"), QString("&lang;E&rang;"));

            setFitResultValue(record, eid, param="fitstat_residavg_weighted", result.residWeightAverage);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

            setFitResultValue(record, eid, param="fitstat_residstddev", result.residStdDev);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

            setFitResultValue(record, eid, param="fitstat_residstddev_weighted", result.residWeightStdDev);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

            setFitResultValue(record, eid, param="fitstat_fitparams", result.fitparamN);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("fit params"));

            setFitResultValue(record, eid, param="fitstat_datapoints", result.dataSize);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("datapoints"));

            setFitResultValue(record, eid, param="fitstat_dof", result.degFreedom);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("degrees of freedom"));

            setFitResultValue(record, eid, param="fitstat_r2", result.Rsquared);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("R squared"), tr("R<sup>2</sup>"));

            setFitResultValue(record, eid, param="fitstat_tss", result.TSS);
            setFitResultGroup(record, eid, param, tr("fit statistics"));
            setFitResultLabel(record, eid, param, tr("total sum of squares"));

        }
    }

    return result;
}
