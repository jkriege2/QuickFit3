#include "qfimfcsfitevaluation.h"
#include "qfimfcsfitevaluationeditor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"


QFImFCSFitEvaluation::QFImFCSFitEvaluation(QFProject* parent):
    QFFitResultsByIndexAsVectorEvaluation("fcs_,dls_,fccs_", parent)
{

    m_weighting=EqualWeighting;
    m_currentIndex=-1;
    mutexThreadedFit=new QMutex(QMutex::Recursive);

    if (m_fitFunctions.contains("fcs_spim_diff")) {
        m_fitFunction="fcs_spim_diff";
    } /*else if (m_fitFunctions.contains("fcs_diff1")) {
        m_fitFunction="fcs_diff1";
    }*/

    if (m_fitAlgorithms.contains("fit_lmfit")) {
        m_fitAlgorithm="fit_lmfit";
    }

}

QFImFCSFitEvaluation::~QFImFCSFitEvaluation() {
    delete mutexThreadedFit;
}


void QFImFCSFitEvaluation::intWriteDataAlgorithm(QXmlStreamWriter& w) const {
    if (m_weighting==EqualWeighting) w.writeAttribute("weighting", "equal");
    if (m_weighting==StdDevWeighting) w.writeAttribute("weighting", "stddev");
    if (m_weighting==RunErrorWeighting) w.writeAttribute("weighting", "runerror");
}

void QFImFCSFitEvaluation::intReadDataAlgorithm(QDomElement& e) {
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        m_weighting=EqualWeighting;
        if (a=="stddev") m_weighting=StdDevWeighting;
        if (a=="runerror") m_weighting=RunErrorWeighting;
    }
}

QFEvaluationEditor* QFImFCSFitEvaluation::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFImFCSFitEvaluationEditor(services, propEditor, parent);
};

bool QFImFCSFitEvaluation::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface") && record->inherits("QFRDRImageToRunInterface");
}

bool QFImFCSFitEvaluation::hasSpecial(QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const {
    int run=getIndexFromEvaluationResultID(id);
    return hasSpecial(r, run, paramid, value, error);
}

bool QFImFCSFitEvaluation::hasSpecial(QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) const {
    if (paramid=="count_rate") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf) {
            error=crintf->getRateStdDev(index)*1000;
            value=crintf->getRateMean(index)*1000;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(index);
            error=scrintf->getSimpleCountrateVariance(index);
        }
        return true;

    } else if (paramid=="pixel_width") {
        if (!r) return false;
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double width=r->getProperty("PIXEL_WIDTH", -1).toDouble();
        if (width<=0) return false;
        value=bin*width;
        error=0;
        return true;
    } else if (paramid=="pixel_height") {
        if (!r) return false;
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double height=r->getProperty("PIXEL_HEIGHT", -1).toDouble();
        if (height<=0) return false;
        value=bin*height;
        error=0;
        return true;
    }
    return false;
}

int QFImFCSFitEvaluation::getIndexMin(QFRawDataRecord* r) const {
    return -1;
}

int QFImFCSFitEvaluation::getIndexMax(QFRawDataRecord* r) const {
    if (!r) return -1;
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (fcs->getCorrelationRuns()<=0) return -1;
    else return fcs->getCorrelationRuns()-1;
}


















/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////

double* QFImFCSFitEvaluation::allocWeights(bool* weightsOKK, QFRawDataRecord* record_in, int run_in, int data_start, int data_end) const {
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

    QFImFCSFitEvaluation::DataWeight weighting=getFitDataWeighting();
    double* weights=(double*)malloc(N*sizeof(double));
    bool weightsOK=false;
    if (weighting==QFImFCSFitEvaluation::StdDevWeighting) {
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
    if (weighting==QFImFCSFitEvaluation::RunErrorWeighting) {
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
        if (weighting==QFImFCSFitEvaluation::EqualWeighting) weightsOK=true;
    }
    if (weightsOKK) *weightsOKK=weightsOK;
    return weights;
}



void QFImFCSFitEvaluation::doFit(QFRawDataRecord* record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter* dlgFitProgress) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFitFunction* ffunc=getFitFunction();
    QFFitAlgorithm* falg=getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;

    int rangeMinDatarange=0;
    int rangeMaxDatarange=data->getCorrelationN();
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    restoreQFFitAlgorithmParameters(falg);

    //QFImFCSFitEvaluation::DataWeight weighting=getFitDataWeighting();

    if (data->getCorrelationN()>0) {
        falg->setReporter(dlgFitProgress);
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);
        dlgFitProgress->reportStatus(tr("setting up ..."));
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setProgress(0);

        QFPluginLogTools::log_text(tr("running fit with '%1' (%2) and model '%3' (%4) on raw data record '%5', run %6 ... \n").arg(falg->name()).arg(falg->id()).arg(ffunc->name()).arg(ffunc->id()).arg(record->getName()).arg(runname));

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
        QFPluginLogTools::log_text(tr("   - tau:  (%1)\n").arg(dt));
        QFPluginLogTools::log_text(tr("   - corr: (%1)\n").arg(dc));*/


        QFPluginLogTools::log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(cut_low).arg(cut_up).arg(cut_N));
        bool weightsOK=false;
        weights=allocWeights(&weightsOK, record, run, cut_low, cut_up);
        if (!weightsOK) QFPluginLogTools::log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));

        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=allocFillParameters(record, run);
        double* initialparams=allocFillParameters(record, run);
        double* errors=allocFillParameterErrors(record, run);
        double* paramsMin=allocFillParametersMin();
        double* paramsMax=allocFillParametersMax();
        bool* paramsFix=allocFillFix(record, run);



        try {
            bool epc=get_doEmitPropertiesChanged();
            bool erc=get_doEmitResultsChanged();
            bool rerc=record->isEmitResultsChangedEnabled();
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
                    //record->disableEmitResultsChanged();

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

                    QFPluginLogTools::log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(doFitThread->getDeltaTime()).arg(result.fitOK?tr("success"):tr("no convergence")));
                    QFPluginLogTools::log_text(tr("   - result-message: %1\n").arg(result.messageSimple));
                    QFPluginLogTools::log_text(tr("   - initial params         (%1)\n").arg(iparams));
                    QFPluginLogTools::log_text(tr("   - output params          (%1)\n").arg(oparams));
                    QFPluginLogTools::log_text(tr("   - output params, rounded (%1)\n").arg(orparams));


                    QString evalID=transformResultID(getEvaluationResultID(ffunc->id(), run));
                    QString param;
                    QString group="fit properties";
                    QString egroup=QString("%1%2__%3__%4").arg(getType()).arg(getID()).arg(falg->id()).arg(ffunc->id());
                    //QString egrouplabel=QString("%1%2: %3, %4").arg(getType()).arg(getID()).arg(falg->shortName()).arg(ffunc->shortName());
                    QString egrouplabel=QString("#%4 \"%1\": %2, %3").arg(getName()).arg(falg->shortName()).arg(ffunc->shortName()).arg(getID());


                    record->resultsSetEvaluationGroup(evalID, egroup);
                    record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                    record->resultsSetEvaluationGroupIndex(evalID, run);
                    record->resultsSetEvaluationDescription(evalID, QString(""));


                    if (run<0) record->resultsSetString(evalID, "fit_model_name", ffunc->id());
                    else record->resultsSetInStringList(evalID, "fit_model_name", run, ffunc->id());
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: model"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_name", falg->id());
                    else record->resultsSetInStringList(evalID, "fitalg_name", run, falg->id());
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: algorithm"));

                    if (run<0) record->resultsSetNumber(evalID, "fitalg_runtime", doFitThread->getDeltaTime(), "msecs");
                    else record->resultsSetInNumberList(evalID, "fitalg_runtime", run, doFitThread->getDeltaTime(), "msecs");
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: runtime"));

                    if (run<0) record->resultsSetBoolean(evalID, "fitalg_success", result.fitOK);
                    else record->resultsSetInBooleanList(evalID, "fitalg_success", run, result.fitOK);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: success"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_message", result.messageSimple);
                    else record->resultsSetInStringList(evalID, "fitalg_message", run, result.messageSimple);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: message"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_messageHTML", result.message);
                    else record->resultsSetInStringList(evalID, "fitalg_messageHTML", run, result.message);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: message (markup)"));

                    if (run<0) record->resultsSetInteger(evalID, "fit_datapoints", cut_N);
                    else record->resultsSetInIntegerList(evalID, "fit_datapoints", run, cut_N);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: datapoints"));

                    if (run<0) record->resultsSetInteger(evalID, "fit_cut_low", cut_low);
                    else record->resultsSetInIntegerList(evalID, "fit_cut_low", run, cut_low);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: first point"));

                    if (run<0) record->resultsSetInteger(evalID, "fit_cut_up", cut_up);
                    else record->resultsSetInIntegerList(evalID, "fit_cut_up", run, cut_up);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: last point"));


                    QMapIterator<QString, QFRawDataRecord::evaluationResult> it(result.params);
                    while (it.hasNext()) {
                        it.next();
                        param="";
                        //record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                        switch(it.value().type) {
                            case QFRawDataRecord::qfrdreNumber:
                                if (run<0) record->resultsSetNumber(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().unit);
                                else record->resultsSetInNumberList(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreNumberError:
                                if (run<0) record->resultsSetNumberError(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().derror, it.value().unit);
                                else record->resultsSetInNumberErrorList(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().derror, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreInteger:
                                if (run<0) record->resultsSetInteger(evalID, param=("fitalg_"+it.key()), it.value().ivalue, it.value().unit);
                                else record->resultsSetInIntegerList(evalID, param=("fitalg_"+it.key()), run, it.value().ivalue, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreBoolean:
                                if (run<0) record->resultsSetBoolean(evalID, param=("fitalg_"+it.key()), it.value().bvalue);
                                else record->resultsSetInBooleanList(evalID, param=("fitalg_"+it.key()), run, it.value().bvalue, it.value().unit);
                                break;
                            case QFRawDataRecord::qfrdreString:
                                if (run<0) record->resultsSetString(evalID, param=("fitalg_"+it.key()), it.value().svalue);
                                else record->resultsSetInStringList(evalID, param=("fitalg_"+it.key()), run, it.value().svalue, it.value().unit);
                                break;

                            case QFRawDataRecord::qfrdreBooleanVector:
                            case QFRawDataRecord::qfrdreBooleanMatrix:
                            case QFRawDataRecord::qfrdreNumberVector:
                            case QFRawDataRecord::qfrdreNumberMatrix:
                            case QFRawDataRecord::qfrdreNumberErrorVector:
                            case QFRawDataRecord::qfrdreNumberErrorMatrix:
                            case QFRawDataRecord::qfrdreIntegerVector:
                            case QFRawDataRecord::qfrdreIntegerMatrix:
                                if (run<0) record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                                break;
                            default:
                                break;
                        }

                        if (!param.isEmpty()) {
                            record->resultsSetGroup(evalID, param, group);
                            record->resultsSetLabel(evalID, param, it.value().label, it.value().label_rich);
                        }
                    }


                    {
                        QFFitStatistics fit_stat=calcFitStatistics(true, ffunc, N, taudata, corrdata, weights, cut_low, cut_up, params, errors, paramsFix, 11, 25, record, run);
                        fit_stat.free();
                    }

                    //record->enableEmitResultsChanged(false);
                    //emit resultsChanged();
                } else {
                    QFPluginLogTools::log_warning(tr("   - fit canceled by user!!!\n"));
                }
            } else {
                QFPluginLogTools::log_error(tr("   - there are not enough datapoints for the fit (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount));
            }

            if (epc) set_doEmitPropertiesChanged(true);
            if (erc) set_doEmitResultsChanged(true);
            if (rerc) record->enableEmitResultsChanged(true);
            //emitPropertiesChanged();
            //emitResultsChanged();
        } catch(std::exception& E) {
            QFPluginLogTools::log_error(tr("error during fitting, error message: %1\n").arg(E.what()));
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




QFFitStatistics QFImFCSFitEvaluation::calcFitStatistics(bool storeAsResults, QFFitFunction* ffunc, long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errors, bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record, int run) {
    QFFitStatistics result= ffunc->calcFitStatistics(N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins);

    if (record) {
        if (storeAsResults) {
            QString param="";
            QString eid= getEvaluationResultID(run);
            setFitResultValue(record, run, param="fitstat_chisquared", result.residSqrSum);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

            setFitResultValue(record, run, param="fitstat_chisquared_weighted", result.residWeightSqrSum);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

            setFitResultValue(record, run, param="fitstat_residavg", result.residAverage);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("residual average"), QString("&lang;E&rang;"));

            setFitResultValue(record, run, param="fitstat_residavg_weighted", result.residWeightAverage);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

            setFitResultValue(record, run, param="fitstat_residstddev", result.residStdDev);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

            setFitResultValue(record, run, param="fitstat_residstddev_weighted", result.residWeightStdDev);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

            setFitResultValue(record, run, param="fitstat_fitparams", result.fitparamN);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("fit params"));

            setFitResultValue(record, run, param="fitstat_datapoints", result.dataSize);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("datapoints"));

            setFitResultValue(record, run, param="fitstat_dof", result.degFreedom);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("degrees of freedom"));

            setFitResultValue(record, run, param="fitstat_r2", result.Rsquared);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("R squared"), tr("R<sup>2</sup>"));

            setFitResultValue(record, run, param="fitstat_tss", result.TSS);
            setFitResultGroup(record, run, param, tr("fit statistics"));
            setFitResultLabel(record, run, param, tr("total sum of squares"));

        }
    }

    return result;
}

bool QFImFCSFitEvaluation::overrideFitFunctionPreset(QString paramid, double &value) const {
    if (paramid=="focus_width") {
        if (propertyExists("PRESET_FOCUS_WIDTH")) {
            double fw=getProperty("PRESET_FOCUS_WIDTH", 0).toDouble();
            if (fw<=0) return false;
            value=fw;
            return true;
        }
    }
    if (paramid=="focus_height" || paramid=="focus_hieght") {
        if (propertyExists("PRESET_FOCUS_HEIGHT")) {
            double fw=getProperty("PRESET_FOCUS_HEIGHT", 0).toDouble();
            if (fw<=0) return false;
            value=fw;
            return true;
        }
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPreset(paramid, value);
}




void QFImFCSFitEvaluation::doFitForMultithread(QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFPluginLogService *logservice) const
{
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFitFunction* ffunc=createFitFunction(NULL);
    QFFitAlgorithm* falg=createFitAlgorithm(NULL);

    if ((!ffunc)||(!data)||(!falg)) {
        if (ffunc) delete ffunc;
        if (falg) delete falg;
        return;
    }

    int rangeMinDatarange=0;
    int rangeMaxDatarange=data->getCorrelationN();
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    restoreQFFitAlgorithmParameters(falg);


    if (data->getCorrelationN()>0) {
        falg->setReporter(NULL);
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);
        /*if (dlgFitProgress) {
            dlgFitProgress->reportStatus(tr("setting up ..."));
            dlgFitProgress->setProgressMax(100);
            dlgFitProgress->setProgress(0);
        }*/

        //if (logservice) logservice->log_text(tr("running fit with '%1' (%2) and model '%3' (%4) on raw data record '%5', run %6 ... \n").arg(falg->name()).arg(falg->id()).arg(ffunc->name()).arg(ffunc->id()).arg(record->getName()).arg(runname));

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


        QMutexLocker locker(mutexThreadedFit);
        //if (logservice) logservice->log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(cut_low).arg(cut_up).arg(cut_N));
        bool weightsOK=false;
        weights=allocWeights(&weightsOK, record, run, cut_low, cut_up);
        if (!weightsOK) QFPluginLogTools::log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));

        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=allocFillParameters(record, run);
        double* initialparams=allocFillParameters(record, run);
        double* errors=allocFillParameterErrors(record, run);
        double* paramsMin=allocFillParametersMin();
        double* paramsMax=allocFillParametersMax();
        bool* paramsFix=allocFillFix(record, run);
        locker.unlock();


        try {
            /*bool epc=get_doEmitPropertiesChanged();
            bool erc=get_doEmitResultsChanged();
            bool rerc=record->isEmitResultsChangedEnabled();
            set_doEmitPropertiesChanged(false);
            set_doEmitResultsChanged(false);
            record->disableEmitResultsChanged();*/

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
            }


            if (cut_N>fitparamcount) {


                QElapsedTimer tstart;
                tstart.start();
                QFFitAlgorithm::FitResult result=falg->fit(params, errors, &taudata[cut_low], &corrdata[cut_low], &weights[cut_low], cut_N, ffunc, initialparams, paramsFix, paramsMin, paramsMax);
                double deltaTime=double(tstart.nsecsElapsed())*1.0e6;

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





                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                        if (!orparams.isEmpty()) orparams=orparams+";  ";
                        orparams=orparams+QString("%1 = %2+/-%3").arg(ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                    }
                    //printf("  fit: %s = %lf +/- %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                }

                //if (logservice) logservice->log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(deltaTime).arg(result.fitOK?tr("success"):tr("no convergence")));
                //if (logservice) logservice->log_text(tr("   - result-message: %1\n").arg(result.messageSimple));
                //if (logservice) logservice->log_text(tr("   - initial params         (%1)\n").arg(iparams));
                //if (logservice) logservice->log_text(tr("   - output params          (%1)\n").arg(oparams));
                //if (logservice) logservice->log_text(tr("   - output params, rounded (%1)\n").arg(orparams));


                QString evalID=transformResultID(getEvaluationResultID(ffunc->id(), run));
                QString param;
                QString group=tr("fit properties");
                QString egroup=QString("%1%2__%3__%4").arg(getType()).arg(getID()).arg(falg->id()).arg(ffunc->id());
                //QString egrouplabel=QString("%1%2: %3, %4").arg(getType()).arg(getID()).arg(falg->shortName()).arg(ffunc->shortName());
                QString egrouplabel=QString("#%4 \"%1\": %2, %3").arg(getName()).arg(falg->shortName()).arg(ffunc->shortName()).arg(getID());



                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (ffunc->isParameterVisible(i, params)) {
                        QString pid=ffunc->getParameterID(i);
                        QString unit=ffunc->getDescription(pid).unit;
                        QString fpid=getFitParamID(pid);
                        QString ffid= getFitParamFixID(pid);
                        if (run<0) record->resultsSetNumberError(evalID, fpid, params[i], errors[i], unit);
                        else record->resultsSetInNumberErrorList(evalID, fpid, run, params[i], errors[i], unit);
                        record->resultsSetGroup(evalID, fpid, tr("fit results"));
                        record->resultsSetLabel(evalID, fpid, ffunc->getDescription(pid).name, ffunc->getDescription(pid).label);
                        record->resultsSetSortPriority(evalID, fpid, true);

                        if (run<0) record->resultsSetBoolean(evalID, ffid, paramsFix[i]);
                        else record->resultsSetInBooleanList(evalID, ffid, run, paramsFix[i]);
                        record->resultsSetGroup(evalID,ffid, tr("fit results"));
                        record->resultsSetLabel(evalID, ffid, ffunc->getDescription(pid).name+tr(", fix"), ffunc->getDescription(pid).label+tr(", fix"));
                        record->resultsSetSortPriority(evalID,  ffid, true);
                    }
                }

                record->resultsSetEvaluationGroup(evalID, egroup);
                record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                record->resultsSetEvaluationGroupIndex(evalID, run);
                record->resultsSetEvaluationDescription(evalID, QString(""));


                if (run<0) record->resultsSetString(evalID, "fit_model_name", ffunc->id());
                else record->resultsSetInStringList(evalID, "fit_model_name", run, ffunc->id());
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: model"));

                if (run<0) record->resultsSetString(evalID, "fitalg_name", falg->id());
                else record->resultsSetInStringList(evalID, "fitalg_name", run, falg->id());
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: algorithm"));

                if (run<0) record->resultsSetNumber(evalID, "fitalg_runtime", deltaTime, "msecs");
                else record->resultsSetInNumberList(evalID, "fitalg_runtime", run, deltaTime, "msecs");
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: runtime"));

                if (run<0) record->resultsSetBoolean(evalID, "fitalg_success", result.fitOK);
                else record->resultsSetInBooleanList(evalID, "fitalg_success", run, result.fitOK);
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: success"));

                if (run<0) record->resultsSetString(evalID, "fitalg_message", result.messageSimple);
                else record->resultsSetInStringList(evalID, "fitalg_message", run, result.messageSimple);
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: message"));

                if (run<0) record->resultsSetString(evalID, "fitalg_messageHTML", result.message);
                else record->resultsSetInStringList(evalID, "fitalg_messageHTML", run, result.message);
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: message (markup)"));

                if (run<0) record->resultsSetInteger(evalID, "fit_datapoints", cut_N);
                else record->resultsSetInIntegerList(evalID, "fit_datapoints", run, cut_N);
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: datapoints"));

                if (run<0) record->resultsSetInteger(evalID, "fit_cut_low", cut_low);
                else record->resultsSetInIntegerList(evalID, "fit_cut_low", run, cut_low);
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: first point"));

                if (run<0) record->resultsSetInteger(evalID, "fit_cut_up", cut_up);
                else record->resultsSetInIntegerList(evalID, "fit_cut_up", run, cut_up);
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: last point"));


                QMapIterator<QString, QFRawDataRecord::evaluationResult> it(result.params);
                while (it.hasNext()) {
                    it.next();
                    param="";
                    //record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                    switch(it.value().type) {
                        case QFRawDataRecord::qfrdreNumber:
                            if (run<0) record->resultsSetNumber(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().unit);
                            else record->resultsSetInNumberList(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().unit);
                            break;
                        case QFRawDataRecord::qfrdreNumberError:
                            if (run<0) record->resultsSetNumberError(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().derror, it.value().unit);
                            else record->resultsSetInNumberErrorList(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().derror, it.value().unit);
                            break;
                        case QFRawDataRecord::qfrdreInteger:
                            if (run<0) record->resultsSetInteger(evalID, param=("fitalg_"+it.key()), it.value().ivalue, it.value().unit);
                            else record->resultsSetInIntegerList(evalID, param=("fitalg_"+it.key()), run, it.value().ivalue, it.value().unit);
                            break;
                        case QFRawDataRecord::qfrdreBoolean:
                            if (run<0) record->resultsSetBoolean(evalID, param=("fitalg_"+it.key()), it.value().bvalue);
                            else record->resultsSetInBooleanList(evalID, param=("fitalg_"+it.key()), run, it.value().bvalue, it.value().unit);
                            break;
                        case QFRawDataRecord::qfrdreString:
                            if (run<0) record->resultsSetString(evalID, param=("fitalg_"+it.key()), it.value().svalue);
                            else record->resultsSetInStringList(evalID, param=("fitalg_"+it.key()), run, it.value().svalue, it.value().unit);
                            break;

                        case QFRawDataRecord::qfrdreBooleanVector:
                        case QFRawDataRecord::qfrdreBooleanMatrix:
                        case QFRawDataRecord::qfrdreNumberVector:
                        case QFRawDataRecord::qfrdreNumberMatrix:
                        case QFRawDataRecord::qfrdreNumberErrorVector:
                        case QFRawDataRecord::qfrdreNumberErrorMatrix:
                        case QFRawDataRecord::qfrdreIntegerVector:
                        case QFRawDataRecord::qfrdreIntegerMatrix:
                            if (run<0) record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                            break;
                        default:
                            break;
                    }

                    if (!param.isEmpty()) {
                        record->resultsSetGroup(evalID, param, group);
                        record->resultsSetLabel(evalID, param, it.value().label, it.value().label_rich);
                    }
                }


                {
                    QFFitStatistics result= ffunc->calcFitStatistics(N, taudata, corrdata, weights, cut_low, cut_up, params, errors, paramsFix, 11, 25);

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_chisquared", result.residSqrSum);
                    else record->resultsSetInNumberList(evalID, param="fitstat_chisquared", run, result.residSqrSum);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_chisquared_weighted", result.residWeightSqrSum);
                    else record->resultsSetInNumberList(evalID, param="fitstat_chisquared_weighted", run, result.residWeightSqrSum);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residavg", result.residAverage);
                    else record->resultsSetInNumberList(evalID, param="fitstat_residavg", run, result.residAverage);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("residual average"), QString("&lang;E&rang;"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residavg_weighted", result.residWeightAverage);
                    else record->resultsSetInNumberList(evalID, param="fitstat_residavg_weighted", run, result.residWeightAverage);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residstddev", result.residStdDev);
                    else record->resultsSetInNumberList(evalID, param="fitstat_residstddev", run, result.residStdDev);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residstddev_weighted", result.residWeightStdDev);
                    else record->resultsSetInNumberList(evalID, param="fitstat_residstddev_weighted", run, result.residWeightStdDev);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

                    if (run<0) record->resultsSetInteger(evalID, param="fitstat_fitparams", result.fitparamN);
                    else record->resultsSetInIntegerList(evalID, param="fitstat_fitparams", run, result.fitparamN);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("fit params"));

                    if (run<0) record->resultsSetInteger(evalID, param="fitstat_datapoints", result.dataSize);
                    else record->resultsSetInIntegerList(evalID, param="fitstat_datapoints", run, result.dataSize);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("datapoints"));

                    if (run<0) record->resultsSetInteger(evalID, param="fitstat_dof", result.degFreedom);
                    else record->resultsSetInIntegerList(evalID, param="fitstat_dof", run, result.degFreedom);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("degrees of freedom"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_r2", result.Rsquared);
                    else record->resultsSetInNumberList(evalID, param="fitstat_r2", run, result.Rsquared);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("R squared"), tr("R<sup>2</sup>"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_tss", result.TSS);
                    else record->resultsSetInNumberList(evalID, param="fitstat_tss", run, result.TSS);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("total sum of squares"));

                    result.free();
                }

            } else {
                QFPluginLogTools::log_error(tr("   - there are not enough datapoints for the fit (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount));
            }

            /*if (epc) set_doEmitPropertiesChanged(true);
            if (erc) set_doEmitResultsChanged(true);
            if (rerc) record->enableEmitResultsChanged(true);*/
        } catch(std::exception& E) {
            QFPluginLogTools::log_error(tr("error during fitting, error message: %1\n").arg(E.what()));
        }

        // clean temporary parameters

        free(weights);
        free(params);
        free(initialparams);
        free(errors);
        free(paramsFix);
        free(paramsMax);
        free(paramsMin);
    }

    if (ffunc) delete ffunc;
    if (falg) delete falg;

}
