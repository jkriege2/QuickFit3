#include "qfimfcsfitevaluation.h"
#include "qfimfcsfitevaluationeditor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qffitfunction.h"
#include "qftools.h"
#include "qffcstools.h"

QFImFCSFitEvaluation::QFImFCSFitEvaluation(QFProject* parent):
    QFFitResultsByIndexAsVectorEvaluation("fcs_,dls_,fccs_", parent)
{

    matchFunctor=new QFImFCSMatchRDRFunctor();
    m_weighting=EqualWeighting;
    m_currentIndex=-1;
    //mutexThreadedFit=new QMutex(QMutex::Recursive);

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
    //delete mutexThreadedFit;
    delete matchFunctor;
}


void QFImFCSFitEvaluation::intWriteDataAlgorithm(QXmlStreamWriter& w) const {
    w.writeAttribute("weighting", dataWeightToString(getFitDataWeighting()));
}

void QFImFCSFitEvaluation::intReadDataAlgorithm(QDomElement& e) {
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        setFitDataWeighting(stringToDataWeight(a));
    }
}

QFEvaluationEditor* QFImFCSFitEvaluation::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFImFCSFitEvaluationEditor(services, propEditor, parent);
};

bool QFImFCSFitEvaluation::isApplicable(QFRawDataRecord* record) {
    //return record->inherits("QFRDRFCSDataInterface") && record->inherits("QFRDRImageToRunInterface");
    return matchFunctor->matches(record);
}

bool QFImFCSFitEvaluation::hasSpecial(const QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const {
    int run=getIndexFromEvaluationResultID(id);
    return hasSpecial(r, run, paramid, value, error);
}

bool QFImFCSFitEvaluation::hasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) const {
    return qfFCSHasSpecial(r, index, paramid, value, error);
    /*if (paramid=="count_rate") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf) {
            error=crintf->getRateStdDev(index)*1000.0;
            value=crintf->getRateMean(index)*1000.0;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(index)*1000.0;
            error=scrintf->getSimpleCountrateVariance(index)*1000.0;
        }
        if (crintf||scrintf) return true;
    } else if (paramid=="count_rate1") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf && crintf->getRateChannels()>0) {
            error=crintf->getRateStdDev(index, 0)*1000.0;
            value=crintf->getRateMean(index, 0)*1000.0;
            //qDebug()<<"getRateMean(run="<<run<<", ch=0) = "<<value<<" +/- "<<error;
            return true;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(index)*1000.0;
            error=scrintf->getSimpleCountrateVariance(index)*1000.0;
            return true;
        }
    } else if (paramid=="count_rate2") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf && crintf->getRateChannels()>1) {
            error=crintf->getRateStdDev(index, 1)*1000.0;
            value=crintf->getRateMean(index, 1)*1000.0;
            //qDebug()<<"getRateMean(run="<<run<<", ch=1) = "<<value<<" +/- "<<error;
            return true;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0 && scrintf->getSimpleCountrateChannels()>1) {
            value=scrintf->getSimpleCountrateAverage(index,1)*1000.0;
            error=scrintf->getSimpleCountrateVariance(index,1)*1000.0;
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
        //qDebug()<<deltax<<bin<<width;
        error=0;
        return true;
    } else if (paramid=="focus_distancey"||paramid=="focus_distance_y") {
        if (!r) return false;
        double deltay=r->getProperty("DCCF_DELTAY", 0.0).toDouble();
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double height=r->getProperty("PIXEL_HEIGHT", -1).toDouble();
        if (height<=0 || !(r->propertyExists("DCCF_DELTAY"))) return false;
        //qDebug()<<deltay<<bin<<height;
        value=bin*height*deltay;
        error=0;
        return true;
    }
    return false;*/
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




void QFImFCSFitEvaluation::doFit(QFRawDataRecord* record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter* dlgFitProgress, bool doLog) {
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

                    bool has_g0=false;
                    for (int i=0; i<ffunc->paramCount(); i++) {
                        if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                            if (!oparams.isEmpty()) oparams=oparams+";  ";
                            has_g0=has_g0||(ffunc->getDescription(i).id=="g0");

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


                    QString evalID=transformResultID(getEvaluationResultID(ffunc->id(), run));
                    QString param;
                    QString group="fit properties";
                    QString egroup=QString("%1%2__%3__%4").arg(getType()).arg(getID()).arg(falg->id()).arg(ffunc->id());
                    //QString egrouplabel=QString("%1%2: %3, %4").arg(getType()).arg(getID()).arg(falg->shortName()).arg(ffunc->shortName());
                    QString egrouplabel=QString("#%4 \"%1\": %2, %3").arg(getName()).arg(falg->shortName()).arg(ffunc->shortName()).arg(getID());


                    if (!has_g0) {
                        if (run<0) record->resultsSetNumber(evalID, "fitparam_g0", ffunc->evaluate(0, params));
                        else record->resultsSetInNumberList(evalID, "fitparam_g0", run, ffunc->evaluate(0, params));
                    }
                    record->resultsSetGroup(evalID, "fitparam_g0", tr("fit results"));
                    record->resultsSetLabel(evalID, "fitparam_g0", tr("g(0)"));
                    record->resultsSetSortPriority(evalID, "fitparam_g0", true);

                    record->resultsSetEvaluationGroup(evalID, egroup);
                    record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                    record->resultsSetEvaluationGroupIndex(evalID, run);
                    record->resultsSetEvaluationDescription(evalID, QString(""));


                    if (run<0) record->resultsSetString(evalID, "fit_model_name", ffunc->id());
                    else record->resultsSetInStringList(evalID, "fit_model_name", run, ffunc->id());
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: model"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_name", falg->id());
                    else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, "fitalg_name", run, falg->id());
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
                    else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, "fitalg_message", run, result.messageSimple);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("fit: message"));

                    if (run<0) record->resultsSetString(evalID, "fitalg_messageHTML", result.message);
                    else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, "fitalg_messageHTML", run, result.message);
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
                                else if (!getProperty("dontSaveFitResultMessage", false).toBool()) record->resultsSetInStringList(evalID, param=("fitalg_"+it.key()), run, it.value().svalue, it.value().unit);
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
                    if (doLog) QFPluginLogTools::log_warning(tr("   - fit canceled by user!!!\n"));
                }
            } else {
                if (doLog) QFPluginLogTools::log_error(tr("   - there are not enough datapoints for the fit (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount));
            }

            if (epc) set_doEmitPropertiesChanged(true);
            if (erc) set_doEmitResultsChanged(true);
            if (rerc) record->enableEmitResultsChanged(true);
            //emitPropertiesChanged();
            //emitResultsChanged();
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




QFFitStatistics QFImFCSFitEvaluation::calcFitStatistics(bool storeAsResults, QFFitFunction* ffunc, long N, double* tauvals, double* corrdata, double* weights, int datacut_min, int datacut_max, double* fullParams, double* errors, bool* paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord* record, int run) {
    QFFitStatistics result= ffunc->calcFitStatistics(N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins);

    if (record) {
        if (storeAsResults) {
            QString param="";
            QString eid= getEvaluationResultID(run, record);
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

QFImFCSMatchRDRFunctor *QFImFCSFitEvaluation::getMatchFunctor() const
{
    return matchFunctor;
}

bool QFImFCSFitEvaluation::overrideFitFunctionPreset(QFRawDataRecord* r, QString paramid, double &value) const {
    if (qfFCSOverrideFitFunctionPreset(this, r, paramid, value)) {
        //qDebug()<<"found override for "<<paramid<<" : "<<value;
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPreset(r, paramid, value);
    /*if (paramid=="focus_width") {
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
    if (paramid=="diff_coeff1") {
        if (propertyExists("PRESET_D1")) {
            double D=getProperty("PRESET_D1", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPreset(paramid, value);*/
}

bool QFImFCSFitEvaluation::overrideFitFunctionPresetError(QFRawDataRecord* r, QString paramid, double &value) const {
    /*if (paramid=="focus_width") {
        if (propertyExists("PRESET_FOCUS_WIDTH_ERROR")) {
            double fw=getProperty("PRESET_FOCUS_WIDTH_ERROR", 0).toDouble();
            if (fw<=0) return false;
            value=fw;
            return true;
        }
    }
    if (paramid=="focus_height" || paramid=="focus_hieght") {
        if (propertyExists("PRESET_FOCUS_HEIGHT_ERROR")) {
            double fw=getProperty("PRESET_FOCUS_HEIGHT_ERROR", 0).toDouble();
            if (fw<=0) return false;
            value=fw;
            return true;
        }
    }
    if (paramid=="diff_coeff1") {
        if (propertyExists("PRESET_D1_ERROR")) {
            double D=getProperty("PRESET_D1_ERROR", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetError(paramid, value);*/
    if (qfFCSOverrideFitFunctionPresetError(this, r, paramid, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetError(r, paramid, value);
}

bool QFImFCSFitEvaluation::overrideFitFunctionPresetFix(QFRawDataRecord* r, QString paramid, bool &value) const
{
    /*if (paramid=="focus_width") {
        if (propertyExists("PRESET_FOCUS_WIDTH_FIX")) {
            value=getProperty("PRESET_FOCUS_WIDTH_FIX", 0).toDouble();
            return true;
        }
    }
    if (paramid=="focus_height" || paramid=="focus_hieght") {
        if (propertyExists("PRESET_FOCUS_HEIGHT_FIX")) {
            value=getProperty("PRESET_FOCUS_HEIGHT_FIX", 0).toDouble();
            return true;
        }
    }
    if (paramid=="diff_coeff1") {
        if (propertyExists("PRESET_D1_FIX")) {
            value=getProperty("PRESET_D1_FIX", 0).toDouble();
            return true;
        }
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetFix(paramid, value);*/
    if (qfFCSOverrideFitFunctionPresetFix(this, r, paramid, value)) {
        return true;
    }
    return QFFitResultsByIndexAsVectorEvaluation::overrideFitFunctionPresetFix(r, paramid, value);

}




void QFImFCSFitEvaluation::doFitForMultithread(QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFPluginLogService *logservice) const {
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
                    //printf("  fit: %s = %lf +/- %lf\n", ffunc->getDescription(i).id.toStdString().c_str(), params[i], errors[i]);
                }

                ffunc->calcParameter(params, errors);
                ffunc->sortParameter(params, errors, paramsFix);
                ffunc->calcParameter(params, errors);

                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (ffunc->isParameterVisible(i, params) && (!paramsFix[i]) && ffunc->getDescription(i).fit) {
                        if (!oparams.isEmpty()) oparams=oparams+";  ";

                        oparams=oparams+QString("%1 = %2+/-%3").arg(ffunc->getDescription(i).id).arg(params[i]).arg(errors[i]);
                    } else {
                        errors[i]=errorsI[i];
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




                bool has_g0=false;
                for (int i=0; i<ffunc->paramCount(); i++) {
                    if (ffunc->isParameterVisible(i, params)) {
                        QString pid=ffunc->getParameterID(i);
                        has_g0=has_g0||(pid=="g0");
                        QString unit=ffunc->getDescription(pid).unit;
                        QString fpid=getFitParamID(pid);
                        QString ffid= getFitParamFixID(pid);
                        if (run<0) record->resultsSetNumberError(evalID, fpid, params[i], errors[i], unit);
                        else {
                            record->resultsSetInNumberErrorListAndBool(evalID, fpid, run, params[i], errors[i], unit, getParamNameLocalStore(fpid), true);
                        }
                        record->resultsSetGroupLabelsAndSortPriority(evalID, fpid, tr("fit results"), ffunc->getDescription(pid).name, ffunc->getDescription(pid).label, true);

                        if (run<0) record->resultsSetBoolean(evalID, ffid, paramsFix[i]);
                        else {
                            record->resultsSetInBooleanListAndBool(evalID, ffid, run, paramsFix[i], QString(""), getParamNameLocalStore(ffid), true);
                        }
                        record->resultsSetGroupLabelsAndSortPriority(evalID,ffid, tr("fit results"), ffunc->getDescription(pid).name+tr(", fix"), ffunc->getDescription(pid).label+tr(", fix"), true);
                    }
                }
                if (!has_g0) {
                    if (run<0) record->resultsSetNumber(evalID, "fitparam_g0", ffunc->evaluate(0, params));
                    else record->resultsSetInNumberListAndBool(evalID, "fitparam_g0", run, ffunc->evaluate(0, params), "", getParamNameLocalStore("fitparam_g0"), true);
                }
                record->resultsSetGroupLabelsAndSortPriority(evalID, "fitparam_g0", tr("fit results"), tr("g(0)"), tr("g(0)"), true);


                record->resultsSetEvaluationGroup(evalID, egroup);
                record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                record->resultsSetEvaluationGroupIndex(evalID, run);
                record->resultsSetEvaluationDescription(evalID, QString(""));


                if (run<0) record->resultsSetString(evalID, "fit_model_name", ffunc->id());
                else {
                    record->resultsSetInStringListAndBool(evalID, "fit_model_name", run, ffunc->id(), QString(""), getParamNameLocalStore("fit_model_name"), true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group, tr("fit: model"));

                if (run<0) record->resultsSetString(evalID, "fitalg_name", falg->id());
                else {
                    record->resultsSetInStringListAndBool(evalID, "fitalg_name", run, falg->id(), QString(""), getParamNameLocalStore("fitalg_name"), true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group,  tr("fit: algorithm"));

                if (run<0) record->resultsSetNumber(evalID, "fitalg_runtime", deltaTime, "msecs");
                else {
                    record->resultsSetInNumberListAndBool(evalID, "fitalg_runtime", run, deltaTime, "msecs", getParamNameLocalStore("fitalg_runtime"), true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group,  tr("fit: runtime"));

                if (run<0) record->resultsSetBoolean(evalID, "fitalg_success", result.fitOK);
                else {
                    record->resultsSetInBooleanListAndBool(evalID, "fitalg_success", run, result.fitOK, QString(""), getParamNameLocalStore("fitalg_success"), true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group,  tr("fit: success"));

                if (run<0) record->resultsSetString(evalID, "fitalg_message", result.messageSimple);
                else {
                    record->resultsSetInStringListAndBool(evalID, "fitalg_message", run, result.messageSimple, QString(""), getParamNameLocalStore("fitalg_message"), true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group,  tr("fit: message"));

                if (run<0) record->resultsSetString(evalID, "fitalg_messageHTML", result.message);
                else {
                    record->resultsSetInStringListAndBool(evalID, "fitalg_messageHTML", run, result.message, QString(""), getParamNameLocalStore("fitalg_messageHTML"), true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group,  tr("fit: message (markup)"));

                if (run<0) record->resultsSetInteger(evalID, "fit_datapoints", cut_N);
                else {
                    record->resultsSetInIntegerListAndBool(evalID, "fit_datapoints", run, cut_N, QString(""), getParamNameLocalStore("fit_datapoints"),  true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group, tr("fit: datapoints"));

                if (run<0) record->resultsSetInteger(evalID, "fit_cut_low", cut_low);
                else {
                    record->resultsSetInIntegerListAndBool(evalID, "fit_cut_low", run, cut_low, QString(""), getParamNameLocalStore("fit_cut_low"),  true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group, tr("fit: first point"));

                if (run<0) record->resultsSetInteger(evalID, "fit_cut_up", cut_up);
                else {
                    record->resultsSetInIntegerListAndBool(evalID, "fit_cut_up", run, cut_up, QString(""), getParamNameLocalStore("fit_cut_up"),  true);
                }
                record->resultsSetGroupAndLabels(evalID, param, group, tr("fit: last point"));


                QMapIterator<QString, QFRawDataRecord::evaluationResult> it(result.params);
                while (it.hasNext()) {
                    it.next();
                    param="";
                    //record->resultsSet(evalID, param=("fitalg_"+it.key()), it.value());
                    switch(it.value().type) {
                        case QFRawDataRecord::qfrdreNumber:
                            if (run<0) record->resultsSetNumber(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().unit);
                            else {
                                record->resultsSetInNumberListAndBool(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().unit, getParamNameLocalStore(param),  true);
                            }
                            break;
                        case QFRawDataRecord::qfrdreNumberError:
                            if (run<0) record->resultsSetNumberError(evalID, param=("fitalg_"+it.key()), it.value().dvalue, it.value().derror, it.value().unit);
                            else  {
                                record->resultsSetInNumberErrorListAndBool(evalID, param=("fitalg_"+it.key()), run, it.value().dvalue, it.value().derror, it.value().unit, getParamNameLocalStore(param),  true);
                            }
                            break;
                        case QFRawDataRecord::qfrdreInteger:
                            if (run<0) record->resultsSetInteger(evalID, param=("fitalg_"+it.key()), it.value().ivalue, it.value().unit);
                            else {
                                record->resultsSetInIntegerListAndBool(evalID, param=("fitalg_"+it.key()), run, it.value().ivalue, it.value().unit, getParamNameLocalStore(param),  true);
                            }
                            break;
                        case QFRawDataRecord::qfrdreBoolean:
                            if (run<0) record->resultsSetBoolean(evalID, param=("fitalg_"+it.key()), it.value().bvalue);
                            else {
                                record->resultsSetInBooleanListAndBool(evalID, param=("fitalg_"+it.key()), run, it.value().bvalue, it.value().unit, getParamNameLocalStore(param),  true);
                            }
                            break;
                        case QFRawDataRecord::qfrdreString:
                            if (run<0) record->resultsSetString(evalID, param=("fitalg_"+it.key()), it.value().svalue);
                            else {
                                record->resultsSetInStringListAndBool(evalID, param=("fitalg_"+it.key()), run, it.value().svalue, it.value().unit, getParamNameLocalStore(param),  true);
                            }
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
                        record->resultsSetGroupAndLabels(evalID, param, group, it.value().label, it.value().label_rich);
                    }
                }


                {
                    QFFitStatistics result= ffunc->calcFitStatistics(N, taudata, corrdata, weights, cut_low, cut_up, params, errors, paramsFix, 11, 25);

                    QString group=tr("fit statistics");
                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_chisquared", result.residSqrSum);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_chisquared", run, result.residSqrSum, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_chisquared_weighted", result.residWeightSqrSum);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_chisquared_weighted", run, result.residWeightSqrSum, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residavg", result.residAverage);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_residavg", run, result.residAverage, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("residual average"), QString("&lang;E&rang;"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residavg_weighted", result.residWeightAverage);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_residavg_weighted", run, result.residWeightAverage, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residstddev", result.residStdDev);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_residstddev", run, result.residStdDev, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_residstddev_weighted", result.residWeightStdDev);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_residstddev_weighted", run, result.residWeightStdDev, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

                    if (run<0) record->resultsSetInteger(evalID, param="fitstat_fitparams", result.fitparamN);
                    else {
                        record->resultsSetInIntegerListAndBool(evalID, param="fitstat_fitparams", run, result.fitparamN, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("fit params"));

                    if (run<0) record->resultsSetInteger(evalID, param="fitstat_datapoints", result.dataSize);
                    else {
                        record->resultsSetInIntegerListAndBool(evalID, param="fitstat_datapoints", run, result.dataSize, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("datapoints"));

                    if (run<0) record->resultsSetInteger(evalID, param="fitstat_dof", result.degFreedom);
                    else {
                        record->resultsSetInIntegerListAndBool(evalID, param="fitstat_dof", run, result.degFreedom, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("degrees of freedom"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_r2", result.Rsquared);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_r2", run, result.Rsquared, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("R squared"), tr("R<sup>2</sup>"));

                    if (run<0) record->resultsSetNumber(evalID, param="fitstat_tss", result.TSS);
                    else {
                        record->resultsSetInNumberListAndBool(evalID, param="fitstat_tss", run, result.TSS, QString(""), getParamNameLocalStore(param),  true);
                    }
                    record->resultsSetGroupAndLabels(evalID, param, group, tr("total sum of squares"));

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
        free(errorsI);
        free(paramsFix);
        free(paramsMax);
        free(paramsMin);
    }

    if (ffunc) delete ffunc;
    if (falg) delete falg;

}


bool QFImFCSMatchRDRFunctor::matches(const QFRawDataRecord *record) const
{
    if (!record) return false;
    return record->inherits("QFRDRFCSDataInterface") && record->inherits("QFRDRImageToRunInterface");
}



void QFImFCSFitEvaluation::calcChi2Landscape(double *chi2Landscape, int paramXFile, int paramXID, const QVector<double> &paramXValues, int paramYFile, int paramYID, const QVector<double> &paramYValues, const QList<QFRawDataRecord *> &records, int run, int defaultMinDatarange, int defaultMaxDatarange)
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
