/*
Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfcurvefitevaluation.h"
#include "qfcurvefitevaluationeditor.h"
#include "QFRDRCurvesInterface.h"
#include "qffitfunction.h"
#include "qffcstools.h"

QFCurveFitEvaluation::QFCurveFitEvaluation(QFProject* parent):
    QFFitResultsByIndexEvaluation("", parent)
{

    m_weighting=EqualWeighting;
    m_fitFunction="gen_line";

    if (m_fitAlgorithms.contains("fit_lmfit")) {
        m_fitAlgorithm="fit_lmfit";
    } else {
        if (m_fitAlgorithms.contains("fit_levmar")) {
            m_fitAlgorithm="fit_levmar";
        }
    }

}

QFCurveFitEvaluation::~QFCurveFitEvaluation() {
}


QString QFCurveFitEvaluation::getIndexName(const QFRawDataRecord *rec, int index) const {
    QFRDRCurvesInterface* data=qobject_cast<QFRDRCurvesInterface*>(rec);
    if (data) {
        return data->curvesGetName(index);
    }
    return QFFitResultsByIndexEvaluation::getIndexName(rec, index);
}

void QFCurveFitEvaluation::intWriteDataAlgorithm(QXmlStreamWriter& w) const {
    w.writeAttribute("weighting", dataWeightToString(getFitDataWeighting()));
}

void QFCurveFitEvaluation::intReadDataAlgorithm(QDomElement& e) {
    if (e.hasAttribute("weighting")) {
        QString a=e.attribute("weighting").toLower();
        setFitDataWeighting(stringToDataWeight(a));
    }
}

QFEvaluationEditor* QFCurveFitEvaluation::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFCurveFitEvaluationEditor(services, propEditor, parent);
};

bool QFCurveFitEvaluation::isApplicable(const QFRawDataRecord *record) const {
    if (record->inherits("QFRDRCurvesInterface")) {
        const QFRDRCurvesInterface* c=dynamic_cast<const QFRDRCurvesInterface*>(record);
        if (c) return c->curvesGetCount()>0;
    }
    return false;
}

//bool QFCurveFitEvaluation::hasSpecial(const QFRawDataRecord* r, const QString& id, const QString& paramid, double& value, double& error) const {
//    int run=getIndexFromEvaluationResultID(id);
//    return qfFCSHasSpecial(r, run, paramid, value, error);

//}

//bool QFCurveFitEvaluation::overrideFitFunctionPreset(const QFRawDataRecord *r, QString paramName, double &value) const
//{
//    if (qfFCSOverrideFitFunctionPreset(this, r, paramName, value)) {
//        return true;
//    }
//    return QFFitResultsByIndexEvaluation::overrideFitFunctionPreset(r, paramName, value);
//}

//bool QFCurveFitEvaluation::overrideFitFunctionPresetError(const QFRawDataRecord *r, QString paramName, double &value) const
//{
//    if (qfFCSOverrideFitFunctionPresetError(this, r, paramName, value)) {
//        return true;
//    }
//    return QFFitResultsByIndexEvaluation::overrideFitFunctionPresetError(r, paramName, value);
//}

//bool QFCurveFitEvaluation::overrideFitFunctionPresetFix(const QFRawDataRecord *r, QString paramName, bool &value) const
//{
//    if (qfFCSOverrideFitFunctionPresetFix(this, r, paramName, value)) {
//        return true;
//    }
//    return QFFitResultsByIndexEvaluation::overrideFitFunctionPresetFix(r, paramName, value);
//}

int QFCurveFitEvaluation::getIndexMin(const QFRawDataRecord */*r*/) const {
    return 0;
}

int QFCurveFitEvaluation::getIndexMax(const QFRawDataRecord *r) const {
    if (!r) return -1;
    QFRDRCurvesInterface* curves=qobject_cast<QFRDRCurvesInterface*>(r);
    if (!curves) return 0;
    int cnt=curves->curvesGetCount();
    if (cnt<=0) return 0;
    else return cnt-1;
}




















/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////


void QFCurveFitEvaluation::doFit(QFRawDataRecord* record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFFitAlgorithmReporter* dlgFitProgress, bool doLog, bool guessOnly) {
    QFRDRCurvesInterface* data=qobject_cast<QFRDRCurvesInterface*>(record);
    QFFitFunction* ffunc=getFitFunction(record);
    QFFitAlgorithm* falg=getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;


    QVector<double> x,y,err;
    x=data->curvesGetX(run);
    y=data->curvesGetY(run);
    long N=qMin(x.size(), y.size());
    int rangeMinDatarange=0;
    int rangeMaxDatarange=N;
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    int fitrepeats=qBound(1,getProperty("FIT_REPEATS", 1).toInt(),1000);

    restoreQFFitAlgorithmParameters(falg);
    falg->readErrorEstimateParametersFit(this);

    //QFFCSFitEvaluation::DataWeight weighting=getFitDataWeighting();

    if (N>0) {
        falg->setReporter(dlgFitProgress);
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);
        dlgFitProgress->reportStatus(tr("setting up ..."));
        dlgFitProgress->setProgressMax(100);
        dlgFitProgress->setProgress(0);

        if (guessOnly) {
            if (doLog) QFPluginLogTools::log_text(tr("running parameter-guess for model '%1' (%2) on raw data record '%3', curve %4 ... \n").arg(ffunc->name()).arg(ffunc->id()).arg(record->getName()).arg(runname));
        } else {
            if (doLog) QFPluginLogTools::log_text(tr("running fit with '%1' (%2) and model '%3' (%4) on raw data record '%5', curve %6 ... \n").arg(falg->name()).arg(falg->id()).arg(ffunc->name()).arg(ffunc->id()).arg(record->getName()).arg(runname));
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


        if (doLog) QFPluginLogTools::log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(cut_low).arg(cut_up).arg(cut_N));
        bool weightsOK=false;
        err=allocVecWeights(&weightsOK, record, run);
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

            double* px=x.data();
            double* py=y.data();
            double* perr=err.data();

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

                        doFitThread->init(falg, params, errors, &px[cut_low], &py[cut_low], &perr[cut_low], cut_N, ffunc, initialparams, paramsFix, paramsMin, paramsMax, fitrepeats);
                        doFitThread->setGuessParamsOnly(guessOnly);
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

                    doFitThread->init(falg, params, errors, &px[cut_low], &py[cut_low], &perr[cut_low], cut_N, ffunc, initialparams, paramsFix, paramsMin, paramsMax, fitrepeats);
                    doFitThread->setGuessParamsOnly(guessOnly);
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

                    if (!guessOnly) {
                        if (doLog) QFPluginLogTools::log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(doFitThread->getDeltaTime()).arg(result.fitOK?tr("success"):tr("no convergence")));
                        if (doLog) QFPluginLogTools::log_text(tr("   - result-message: %1\n").arg(result.messageSimple));
                    }
                    if (doLog) QFPluginLogTools::log_text(tr("   - initial params         (%1)\n").arg(iparams));
                    if (doLog) QFPluginLogTools::log_text(tr("   - output params          (%1)\n").arg(oparams));
                    if (doLog) QFPluginLogTools::log_text(tr("   - output params, rounded (%1)\n").arg(orparams));


                    QString evalID=getEvaluationResultID(ffunc->id(), run);
                    QString param;
                    QString group="fit properties";
                    QString egroup=QString("%1%2__%3__%4").arg(getType()).arg(getID()).arg(falg->id()).arg(ffunc->id());
                    //QString egrouplabel=QString("%1%2: %3, %4").arg(getType()).arg(getID()).arg(falg->shortName()).arg(ffunc->shortName());
                    QString egrouplabel=QString("#%4 \"%1\": %2, %3").arg(getName()).arg(falg->shortName()).arg(ffunc->shortName()).arg(getID());
                    record->resultsSetNumber(evalID, "fitparam_g0", ffunc->evaluate(0, params));
                    record->resultsSetGroup(evalID, "fitparam_g0", tr("fit results"));
                    record->resultsSetLabel(evalID, "fitparam_g0", tr("g(0)"));
                    record->resultsSetSortPriority(evalID, "fitparam_g0", true);



                    record->resultsSetEvaluationGroup(evalID, egroup);
                    record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                    record->resultsSetEvaluationGroupIndex(evalID, run);
                    record->resultsSetEvaluationDescription(evalID, QString(""));

                    if (!guessOnly) {

                        record->resultsSetInteger(evalID, param="fit_used_curve", run);
                        record->resultsSetGroup(evalID, param, group);
                        record->resultsSetLabel(evalID, param, tr("fit: used curves"));

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
                    } else {
                        record->resultsSetString(evalID, "fitalg_message", doFitThread->getGuessMessage());
                        record->resultsSetGroup(evalID, param, group);
                        record->resultsSetLabel(evalID, param, tr("guess: message"));

                        record->resultsSetString(evalID, "fitalg_messageHTML", doFitThread->getGuessMessage());
                        record->resultsSetGroup(evalID, param, group);
                        record->resultsSetLabel(evalID, param, tr("guess: message (markup)"));

                        record->resultsSetBoolean(evalID, "fitalg_success", doFitThread->getGuessOK());
                        record->resultsSetGroup(evalID, param, group);
                        record->resultsSetLabel(evalID, param, tr("guess: success"));

                    }


                    {
                        QFFitStatistics fit_stat=calcFitStatistics(true, ffunc, N, px, py, perr, cut_low, cut_up, params, errors, paramsFix, 11, 25, record, run);
                        fit_stat.free();
                    }

                    //record->enableEmitResultsChanged(false);
                    //emit resultsChanged();
                } else {
                    if (guessOnly) {
                        if (doLog) QFPluginLogTools::log_warning(tr("   - parameter-guess canceled by user!!!\n"));
                    } else {
                        if (doLog) QFPluginLogTools::log_warning(tr("   - fit canceled by user!!!\n"));
                    }
                }
            } else {
                if (doLog) QFPluginLogTools::log_error(tr("   - there are not enough datapoints for the fit/parameter-guess (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount));
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
            if (doLog) QFPluginLogTools::log_error(tr("error during fitting/guessing, error message: %1\n").arg(E.what()));
        }

        // clean temporary parameters
        //delete doFitThread;
        qfFree(params);
        qfFree(initialparams);
        qfFree(errors);
        qfFree(errorsI);
        qfFree(paramsFix);
        qfFree(paramsMax);
        qfFree(paramsMin);

        //displayModel(false);
        //replotData();
        //QApplication::restoreOverrideCursor();
        //dlgFitProgress->done();
        falg->setReporter(NULL);
    }
}









void QFCurveFitEvaluation::doFitForMultithread(QFFitAlgorithm *falg, QFFitFunction *ffunc, QFRawDataRecord *record, int run, int defaultMinDatarange, int defaultMaxDatarange, QFPluginLogService *logservice, bool guessOnly) const
{
    QFRDRCurvesInterface* data=qobject_cast<QFRDRCurvesInterface*>(record);
    //QFFitFunction* ffunc=createFitFunction();
    //QFFitAlgorithm* falg=createFitAlgorithm(NULL);

    if ((!ffunc)||(!data)||(!falg)) {
        //if (ffunc) delete ffunc;
        //if (falg) delete falg;
        return;
    }

    QVector<double> x,y,err;
    x=data->curvesGetX(run);
    y=data->curvesGetY(run);
    long N=qMin(x.size(), y.size());
    int rangeMinDatarange=0;
    int rangeMaxDatarange=N;
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    int fitrepeats=qBound(1,getProperty("FIT_REPEATS", 1).toInt(),1000);

    //restoreQFFitAlgorithmParameters(falg);
    falg->readErrorEstimateParametersFit(this);


    if (N>0) {
        falg->setReporter(NULL);
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);

        double* px=x.data();
        double* py=y.data();

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
        bool weightsOK=false;
        err=allocVecWeights(&weightsOK, record, run);
        if (!weightsOK && logservice) {
            logservice->log_warning(tr("fitting file '%1', curve %2:\n   weights have invalid values => setting all weights to 1\n").arg(record->getName()).arg(run));
        }
        double* perr=err.data();


        // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
        // we are working with a complete set of parameters
        double* params=allocFillParameters(record, run, ffunc);
        double* initialparams=allocFillParameters(record, run, ffunc);
        double* errors=allocFillParameterErrors(record, run, ffunc);
        double* errorsI=allocFillParameterErrors(record, run, ffunc);
        double* paramsMin=allocFillParametersMin(record, ffunc);
        double* paramsMax=allocFillParametersMax(record, ffunc);
        bool* paramsFix=allocFillFix(record, run, ffunc);
        QVector<double> COV;
        locker.unlock();


        try {


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
                double* init=duplicateArray(initialparams, ffunc->paramCount());
                QFFitAlgorithm::FitResult result;
                QString guessm="";
                bool guessok=false;

                if (guessOnly) {
                    guessm=tr("parameter-guessing failed");
                    if (ffunc->estimateInitial(init, &px[cut_low], &py[cut_low], cut_N, paramsFix)) {
                        for (int i=0; i<ffunc->paramCount(); i++) {
                            if (!paramsFix || !paramsFix[i]) {
                                params[i]=init[i];
                            }
                            errors[i]=0;
                        }
                        guessm=tr("parameter-guessing successfull");
                        guessok=true;
                    }
                } else {
                    for (int rep=0; rep<fitrepeats; rep++) {
                        result=falg->fit(params, errors, &px[cut_low], &py[cut_low], &perr[cut_low], cut_N, ffunc, init, paramsFix, paramsMin, paramsMax, false, NULL, &COV);
                        copyArray(init, params, ffunc->paramCount());
                    }
                }
                qfFree(init);

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
                        record->resultsSetLabel(evalID, fpid, pid+": "+ffunc->getDescription(pid).name, ffunc->getDescription(pid).label);
                        record->resultsSetSortPriority(evalID, fpid, true);

                        record->resultsSetBoolean(evalID, ffid, paramsFix[i]);
                        record->resultsSetGroup(evalID,ffid, tr("fit results"));
                        record->resultsSetLabel(evalID, ffid, "fix_"+pid+": "+ffunc->getDescription(pid).name+tr(", fix"), ffunc->getDescription(pid).label+tr(", fix"));
                        record->resultsSetSortPriority(evalID,  ffid, true);
                    }
                }
                record->resultsSetNumber(evalID, "fitparam_g0", ffunc->evaluate(0, params));
                record->resultsSetGroupLabelsAndSortPriority(evalID, "fitparam_g0", tr("fit results"), tr("g(0)"), tr("g(0)"), true);


                record->resultsSetEvaluationGroup(evalID, egroup);
                record->resultsSetEvaluationGroupLabel(egroup, egrouplabel);
                record->resultsSetEvaluationGroupIndex(evalID, run);
                record->resultsSetEvaluationDescription(evalID, QString(""));

                if (!guessOnly) {
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
                } else {
                    record->resultsSetString(evalID, "fitalg_message", guessm);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("guess: message"));

                    record->resultsSetString(evalID, "fitalg_messageHTML", guessm);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("guess: message (markup)"));

                    record->resultsSetBoolean(evalID, "fitalg_success", guessok);
                    record->resultsSetGroup(evalID, param, group);
                    record->resultsSetLabel(evalID, param, tr("guess: success"));
                }




                {
                    QFFitStatistics result= ffunc->calcFitStatistics(N, px, py, perr, cut_low, cut_up, params, errors, paramsFix, 11, 25, COV);
                    record->resultsSetBasicFitStatistics(result, evalID, "fitstat_", tr("fit statistics"));
                    result.free();
                }

            } else {
                if (logservice) logservice->log_error(tr("fitting file '%3', curve %4:\n   there are not enough datapoints for the fit (%1 datapoints, but %2 fit parameters!)\n").arg(cut_N).arg(fitparamcount).arg(record->getName()).arg(run));
            }

         } catch(std::exception& E) {
            if (logservice) logservice->log_error(tr("fitting file '%2', curve %3:\n   error during fitting, error message: %1\n").arg(E.what()).arg(record->getName()).arg(run));
        }

        // clean temporary parameters

        qfFree(params);
        qfFree(initialparams);
        qfFree(errors);
        qfFree(paramsFix);
        qfFree(paramsMax);
        qfFree(paramsMin);
        qfFree(errorsI);
    }

    //if (ffunc) delete ffunc;
    //if (falg) delete falg;

}

void QFCurveFitEvaluation::doFitForMultithreadReturn(QFRawDataRecord::QFFitFitResultsStore& result, const QFRawDataRecord *record, int run, int /*defaultMinDatarange*/, int /*defaultMaxDatarange*/, QFPluginLogService */*logservice*/, bool /*guessOnly*/) const
{
    result.index=run;
    result.rdr=record;
    result.rdrRecID=-1;
    if (result.rdr) result.rdrRecID=result.rdr->getID();


}

void QFCurveFitEvaluation::createFitFunctionAndAlgorithm(QFFitAlgorithm *&falg, QFFitFunction *&ffunc, const QFRawDataRecord */*record*/, int /*run*/)
{
    ffunc=createFitFunction();
    falg=createFitAlgorithm();
    restoreQFFitAlgorithmParameters(falg);
}

void QFCurveFitEvaluation::calcChi2Landscape(double *chi2Landscape, int /*paramXFile*/, int paramXID, const QVector<double> &paramXValues, int /*paramYFile*/, int paramYID, const QVector<double> &paramYValues, const QList<QFRawDataRecord *> &records, int run, int defaultMinDatarange, int defaultMaxDatarange)
{
    QFRawDataRecord* record=records.value(0,NULL);
    QFRDRCurvesInterface* data=qobject_cast<QFRDRCurvesInterface*>(record);

    QFFitFunction* ffunc=getFitFunction(record);
    QFFitAlgorithm* falg=getFitAlgorithm();
    if ((!ffunc)||(!data)||(!falg)) return;

    QVector<double> x,y,err;
    x=data->curvesGetX(run);
    y=data->curvesGetY(run);
    err=data->curvesGetYError(run);
    long N=qMin(x.size(), y.size());
    int rangeMinDatarange=0;
    int rangeMaxDatarange=N;
    if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
    if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;

    restoreQFFitAlgorithmParameters(falg);

    //QFFCSFitEvaluation::DataWeight weighting=getFitDataWeighting();

    if (N>0) {
        QString runname=tr("average");
        if (run>=0) runname=QString::number(run);


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
        err=allocVecWeights(&weightsOK, record, run);

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



        QFFitAlgorithm::FitQFFitFunctionFunctor fm(ffunc, params, paramsFix, &x[cut_low], &y[cut_low], &err[cut_low], cut_N);
        double* pm=(double*)qfMalloc(fm.get_paramcount()*sizeof(double));
        int idxX=paramXID;
        int idxY=paramYID;
        int idx=0;
        double* d=(double*)qfCalloc(fm.get_evalout(), sizeof(double));

        for (int y=0; y<paramYValues.size(); y++) {
            for (int x=0; x<paramXValues.size(); x++) {
                params[idxX]=paramXValues[x];
                params[idxY]=paramYValues[y];

                fm.mapArrayFromModelToFunctor(pm, params);
                fm.evaluate(d, pm);

                double chi2=0;
                for (uint64_t i=0; i<fm.get_evalout(); i++)  {
                    chi2=chi2+qfSqr(d[i]);
                }
                chi2Landscape[idx]=chi2;

                idx++;
            }
        }

        qfFree(d);
        qfFree(pm);
        qfFree(params);
        qfFree(errors);
        qfFree(paramsFix);


    }
}
