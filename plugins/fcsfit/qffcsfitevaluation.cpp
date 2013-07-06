#include "qffcsfitevaluation.h"
#include "qffcsfitevaluationeditor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"
#include "qffcstools.h"

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


QString QFFCSFitEvaluation::getIndexName(QFRawDataRecord *rec, int index) const {
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(rec);
    if (fcs) {
        return fcs->getCorrelationRunName(index);
    }
    return QFFitResultsByIndexEvaluation::getIndexName(rec, index);
}

void QFFCSFitEvaluation::intWriteDataAlgorithm(QXmlStreamWriter& w) const {
    w.writeAttribute("weighting", dataWeightToString(getFitDataWeighting()));
}

void QFFCSFitEvaluation::intReadDataAlgorithm(QDomElement& e) {
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        setFitDataWeighting(stringToDataWeight(a));
    }
}

QFEvaluationEditor* QFFCSFitEvaluation::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFFCSFitEvaluationEditor(services, propEditor, parent);
};

bool QFFCSFitEvaluation::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}

bool QFFCSFitEvaluation::hasSpecial(const QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const {
    int run=getIndexFromEvaluationResultID(id);
    return qfFCSHasSpecial(r, run, paramid, value, error);
    /*if (paramid=="count_rate") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf) {
            error=crintf->getRateStdDev(run)*1000.0;
            value=crintf->getRateMean(run)*1000.0;
            return true;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(run,0)*1000.0;
            error=scrintf->getSimpleCountrateVariance(run,0)*1000.0;
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
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(run,0)*1000.0;
            error=scrintf->getSimpleCountrateVariance(run,0)*1000.0;
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
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0 && scrintf->getSimpleCountrateChannels()>1) {
            value=scrintf->getSimpleCountrateAverage(run,1)*1000.0;
            error=scrintf->getSimpleCountrateVariance(run,1)*1000.0;
            return true;
        }

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
    } else if (paramid=="dls_angle") {
        if (!r) return false;
        if (!(r->propertyExists("ANGLE [°]") || r->propertyExists("ANGLE"))) return false;
        double angle_deg=r->getProperty("ANGLE [°]", r->getProperty("ANGLE", 90.0).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="wavelength") {
        if (!r) return false;
        if (!(r->propertyExists("WAVELENGTH [nm]") || r->propertyExists("WAVELENGTH"))) return false;
        double angle_deg=r->getProperty("WAVELENGTH [nm]", r->getProperty("WAVELENGTH", 532.0).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="refractive_index") {
        if (!r) return false;
        if (!(r->propertyExists("REFRACTIVE_INDEX"))) return false;
        double angle_deg=r->getProperty("REFRACTIVE_INDEX", 1.33).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="viscosity") {
        if (!r) return false;
        if (!(r->propertyExists("VISCOSITY [cp]") || r->propertyExists("VISCOSITY"))) return false;
        double angle_deg=r->getProperty("VISCOSITY [cp]", r->getProperty("VISCOSITY", 0.86).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="temperature") {
        if (!r) return false;
        if (!(r->propertyExists("TEMPERATURE [K]") || r->propertyExists("TEMPERATURE"))) return false;
        double angle_deg=r->getProperty("TEMPERATURE [K]", r->getProperty("TEMPERATURE", 396).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="focus_distance") {
        if (!r) return false;
        double deltax=r->getProperty("DCCF_DELTAX", 0.0).toDouble();
        double deltay=r->getProperty("DCCF_DELTAY", 0.0).toDouble();
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double width=r->getProperty("PIXEL_WIDTH", -1).toDouble();
        double height=r->getProperty("PIXEL_HEIGHT", -1).toDouble();
        if (width<=0 || height<=0 || !(r->propertyExists("DCCF_DELTAX") && r->propertyExists("DCCF_DELTAY"))) return false;
        value=sqrt(qfSqr(bin*width*deltax)+qfSqr(bin*height*deltay));
        error=0;
        return true;
    } else if (paramid=="focus_distancex"||paramid=="focus_distance_x") {
        if (!r) return false;
        double deltax=r->getProperty("DCCF_DELTAX", 0.0).toDouble();
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double width=r->getProperty("PIXEL_WIDTH", -1).toDouble();
        if (width<=0 || !(r->propertyExists("DCCF_DELTAX"))) return false;
        value=bin*width*deltax;
        error=0;
        return true;
    } else if (paramid=="focus_distancey"||paramid=="focus_distance_y") {
        if (!r) return false;
        double deltay=r->getProperty("DCCF_DELTAY", 0.0).toDouble();
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double height=r->getProperty("PIXEL_HEIGHT", -1).toDouble();
        if (height<=0 || !(r->propertyExists("DCCF_DELTAY"))) return false;
        value=bin*height*deltay;
        error=0;
        return true;
    }

    return false;*/
}

bool QFFCSFitEvaluation::overrideFitFunctionPreset(QFRawDataRecord *r, QString paramName, double &value) const
{
    if (qfFCSOverrideFitFunctionPreset(this, r, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexEvaluation::overrideFitFunctionPreset(r, paramName, value);
}

bool QFFCSFitEvaluation::overrideFitFunctionPresetError(QFRawDataRecord *r, QString paramName, double &value) const
{
    if (qfFCSOverrideFitFunctionPresetError(this, r, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexEvaluation::overrideFitFunctionPresetError(r, paramName, value);
}

bool QFFCSFitEvaluation::overrideFitFunctionPresetFix(QFRawDataRecord *r, QString paramName, bool &value) const
{
    if (qfFCSOverrideFitFunctionPresetFix(this, r, paramName, value)) {
        return true;
    }
    return QFFitResultsByIndexEvaluation::overrideFitFunctionPresetFix(r, paramName, value);
}

int QFFCSFitEvaluation::getIndexMin(QFRawDataRecord* r) const {
    return -1;
}

int QFFCSFitEvaluation::getIndexMax(QFRawDataRecord* r) const {
    if (!r) return -1;
    QFRDRFCSDataInterface* fcs=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (!fcs) return 0;
    if (fcs->getCorrelationRuns()<=0) return -1;
    else return fcs->getCorrelationRuns()-1;
}



















/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////


void QFFCSFitEvaluation::doFit(QFRawDataRecord* record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter* dlgFitProgress, bool doLog) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    QFFitFunction* ffunc=getFitFunction(record);
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

        if (doLog) QFPluginLogTools::log_text(tr("running fit with '%1' (%2) and model '%3' (%4) on raw data record '%5', run %6 ... \n").arg(falg->name()).arg(falg->id()).arg(ffunc->name()).arg(ffunc->id()).arg(record->getName()).arg(runname));

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
        if (doLog) QFPluginLogTools::log_text(tr("   - tau:  (%1)\n").arg(dt));
        if (doLog) QFPluginLogTools::log_text(tr("   - corr: (%1)\n").arg(dc));*/


        if (doLog) QFPluginLogTools::log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(cut_low).arg(cut_up).arg(cut_N));
        bool weightsOK=false;
        weights=allocWeights(&weightsOK, record, run, cut_low, cut_up);
        if (!weightsOK && doLog) QFPluginLogTools::log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));

        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=allocFillParameters(record, run);
        double* initialparams=allocFillParameters(record, run);
        double* errors=allocFillParameterErrors(record, run);
        double* errorsI=allocFillParameterErrors(record, run);
        double* paramsMin=allocFillParametersMin(record);
        double* paramsMax=allocFillParametersMax(record);
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

                    for (int i=0; i<ffunc->paramCount(); i++) {
                        if (!(ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit)) {
                            errors[i]=errorsI[i];
                        }
                    }

                    ffunc->calcParameter(params, errors);
                    ffunc->sortParameter(params, errors, paramsFix);
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

                    if (doLog) QFPluginLogTools::log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(doFitThread->getDeltaTime()).arg(result.fitOK?tr("success"):tr("no convergence")));
                    if (doLog) QFPluginLogTools::log_text(tr("   - result-message: %1\n").arg(result.messageSimple));
                    if (doLog) QFPluginLogTools::log_text(tr("   - initial params         (%1)\n").arg(iparams));
                    if (doLog) QFPluginLogTools::log_text(tr("   - output params          (%1)\n").arg(oparams));
                    if (doLog) QFPluginLogTools::log_text(tr("   - output params, rounded (%1)\n").arg(orparams));


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
                        QFFitStatistics fit_stat=calcFitStatistics(true, ffunc, N, taudata, corrdata, weights, cut_low, cut_up, params, errors, paramsFix, 11, 25, record, run);
                        fit_stat.free();
                    }

                    //record->enableEmitResultsChanged(false);
                    //emit resultsChanged();
                } else {
                    if (doLog) QFPluginLogTools::log_warning(tr("   - fit canceled by user!!!\n"));
                }
            } else {
                if (doLog) QFPluginLogTools::log_error(tr("   - there are not enough datapoints for the fit (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount));
            }
            //set_doEmitPropertiesChanged(true);
            //set_doEmitResultsChanged(true);
           // record->enableEmitResultsChanged(false);
            //emitPropertiesChanged();
            //emitResultsChanged();

            if (epc) set_doEmitPropertiesChanged(true);
            if (erc) set_doEmitResultsChanged(true);
            if (rerc) record->enableEmitResultsChanged(true);
        } catch(std::exception& E) {
            if (doLog) QFPluginLogTools::log_error(tr("error during fitting, error message: %1\n").arg(E.what()));
        }

        // clean temporary parameters
        //delete doFitThread;
        free(weights);
        free(params);
        free(initialparams);
        free(errors);
        free(errorsI);
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


/*QFFitStatistics QFFCSFitEvaluation::calcFitStatistics(bool saveAsResults, QFFitFunction* ffunc, long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errors, bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record, int run) {
    QFFitStatistics result= ffunc->calcFitStatistics(N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins);

    if (record) {
        if (saveAsResults) {
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
}*/










void QFFCSFitEvaluation::doFitForMultithread(QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFPluginLogService *logservice) const
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
        if (!weightsOK && logservice) {
            logservice->log_warning(tr("fitting file '%1', run %2:\n   weights have invalid values => setting all weights to 1\n").arg(record->getName()).arg(run));
        }

        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=allocFillParameters(record, run, ffunc);
        double* initialparams=allocFillParameters(record, run, ffunc);
        double* errors=allocFillParameterErrors(record, run, ffunc);
        double* errorsI=allocFillParameterErrors(record, run, ffunc);
        double* paramsMin=allocFillParametersMin(record, ffunc);
        double* paramsMax=allocFillParametersMax(record, ffunc);
        bool* paramsFix=allocFillFix(record, run, ffunc);
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
                #if QT_VERSION >= 0x040800
                    double deltaTime=double(tstart.nsecsElapsed())/1.0e6;
                #else
                    double deltaTime=double(tstart.elapsed());
                #endif

                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (!(ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit)) {
                        errors[i]=errorsI[i];
                    }
                }

                ffunc->calcParameter(params, errors);
                ffunc->sortParameter(params, errors, paramsFix);
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
                        record->resultsSetNumberError(evalID, fpid, params[i], errors[i], unit);
                        record->resultsSetGroup(evalID, fpid, tr("fit results"));
                        record->resultsSetLabel(evalID, fpid, ffunc->getDescription(pid).name, ffunc->getDescription(pid).label);
                        record->resultsSetSortPriority(evalID, fpid, true);

                        record->resultsSetBoolean(evalID, ffid, paramsFix[i]);
                        record->resultsSetGroup(evalID,ffid, tr("fit results"));
                        record->resultsSetLabel(evalID, ffid, ffunc->getDescription(pid).name+tr(", fix"), ffunc->getDescription(pid).label+tr(", fix"));
                        record->resultsSetSortPriority(evalID,  ffid, true);
                    }
                }

                record->resultsSetEvaluationGroup(evalID, egroup);
                record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                record->resultsSetEvaluationGroupIndex(evalID, run);
                record->resultsSetEvaluationDescription(evalID, QString(""));


                record->resultsSetString(evalID, "fit_model_name", ffunc->id());
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: model"));

                record->resultsSetString(evalID, "fitalg_name", falg->id());
                record->resultsSetGroup(evalID, param, group);
                record->resultsSetLabel(evalID, param, tr("fit: algorithm"));

                record->resultsSetNumber(evalID, "fitalg_runtime", deltaTime, "msecs");
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
                    param="";
                    //record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                    switch(it.value().type) {
                        case QFRawDataRecord::qfrdreNumber:
                            record->resultsSetNumber(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().unit);
                            break;
                        case QFRawDataRecord::qfrdreNumberError:
                            record->resultsSetNumberError(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().derror, it.value().unit);
                            break;
                        case QFRawDataRecord::qfrdreInteger:
                            record->resultsSetInteger(evalID, param=("fitalg_"+it.key()), it.value().ivalue, it.value().unit);
                            break;
                        case QFRawDataRecord::qfrdreBoolean:
                            record->resultsSetBoolean(evalID, param=("fitalg_"+it.key()), it.value().bvalue);
                            break;
                        case QFRawDataRecord::qfrdreString:
                            record->resultsSetString(evalID, param=("fitalg_"+it.key()), it.value().svalue);
                            break;

                        case QFRawDataRecord::qfrdreBooleanVector:
                        case QFRawDataRecord::qfrdreBooleanMatrix:
                        case QFRawDataRecord::qfrdreNumberVector:
                        case QFRawDataRecord::qfrdreNumberMatrix:
                        case QFRawDataRecord::qfrdreNumberErrorVector:
                        case QFRawDataRecord::qfrdreNumberErrorMatrix:
                        case QFRawDataRecord::qfrdreIntegerVector:
                        case QFRawDataRecord::qfrdreIntegerMatrix:
                            record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
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

                    record->resultsSetNumber(evalID, param="fitstat_chisquared", result.residSqrSum);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

                    record->resultsSetNumber(evalID, param="fitstat_chisquared_weighted", result.residWeightSqrSum);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

                    record->resultsSetNumber(evalID, param="fitstat_residavg", result.residAverage);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("residual average"), QString("&lang;E&rang;"));

                    record->resultsSetNumber(evalID, param="fitstat_residavg_weighted", result.residWeightAverage);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

                    record->resultsSetNumber(evalID, param="fitstat_residstddev", result.residStdDev);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

                    record->resultsSetNumber(evalID, param="fitstat_residstddev_weighted", result.residWeightStdDev);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

                    record->resultsSetInteger(evalID, param="fitstat_fitparams", result.fitparamN);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("fit params"));

                    record->resultsSetInteger(evalID, param="fitstat_datapoints", result.dataSize);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("datapoints"));

                    record->resultsSetInteger(evalID, param="fitstat_dof", result.degFreedom);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("degrees of freedom"));

                    record->resultsSetNumber(evalID, param="fitstat_r2", result.Rsquared);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("R squared"), tr("R<sup>2</sup>"));

                    record->resultsSetNumber(evalID, param="fitstat_tss", result.TSS);
                    record->resultsSetGroup(evalID, param, tr("fit statistics"));
                    record->resultsSetLabel(evalID, param, tr("total sum of squares"));

                    result.free();
                }

            } else {
                if (logservice) logservice->log_error(tr("fitting file '%3', run %4:\n   there are not enough datapoints for the fit (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount).arg(record->getName()).arg(run));
            }

            /*if (epc) set_doEmitPropertiesChanged(true);
            if (erc) set_doEmitResultsChanged(true);
            if (rerc) record->enableEmitResultsChanged(true);*/
        } catch(std::exception& E) {
            if (logservice) logservice->log_error(tr("fitting file '%2', run %3:\n   error during fitting, error message: %1\n").arg(E.what()).arg(record->getName()).arg(run));
        }

        // clean temporary parameters

        free(weights);
        free(params);
        free(initialparams);
        free(errors);
        free(paramsFix);
        free(paramsMax);
        free(paramsMin);
        free(errorsI);
    }

    if (ffunc) delete ffunc;
    if (falg) delete falg;

}

void QFFCSFitEvaluation::calcChi2Landscape(double *chi2Landscape, int paramXFile, int paramXID, const QVector<double> &paramXValues, int paramYFile, int paramYID, const QVector<double> &paramYValues, const QList<QFRawDataRecord *> &records, int run, int defaultMinDatarange, int defaultMaxDatarange)
{
    QFRawDataRecord* record=records.value(0,NULL);
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);

    QFFitFunction* ffunc=getFitFunction(record);
    QFFitAlgorithm* falg=getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;

    int rangeMinDatarange=0;
    int rangeMaxDatarange=data->getCorrelationN();
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    restoreQFFitAlgorithmParameters(falg);

    //QFFCSFitEvaluation::DataWeight weighting=getFitDataWeighting();

    if (data->getCorrelationN()>0) {
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);


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

        bool weightsOK=false;
        weights=allocWeights(&weightsOK, record, run, cut_low, cut_up);

        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=allocFillParameters(record, run);
        double* errors=allocFillParameterErrors(record, run);
        bool* paramsFix=allocFillFix(record, run);
        for (int i=0; i<ffunc->paramCount(); i++) {
            paramsFix[i]=false;
        }



        ffunc->calcParameter(params, errors);

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



        QFFitAlgorithm::FitQFFitFunctionFunctor fm(ffunc, params, paramsFix, &taudata[cut_low], &corrdata[cut_low], &weights[cut_low], cut_N);
        double* pm=(double*)malloc(fm.get_paramcount()*sizeof(double));
        int idxX=paramXID;
        int idxY=paramYID;
        int idx=0;
        double* d=(double*)calloc(fm.get_evalout(), sizeof(double));

        for (int y=0; y<paramYValues.size(); y++) {
            for (int x=0; x<paramXValues.size(); x++) {
                params[idxX]=paramXValues[x];
                params[idxY]=paramYValues[y];

                fm.mapArrayFromModelToFunctor(pm, params);
                fm.evaluate(d, pm);

                double chi2=0;
                for (int i=0; i<fm.get_evalout(); i++)  {
                    chi2=chi2+qfSqr(d[i]);
                }
                chi2Landscape[idx]=chi2;

                idx++;
            }
        }

        free(d);
        free(pm);


        free(weights);
        free(params);
        free(errors);
         free(paramsFix);


    }
}
