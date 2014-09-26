/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "qffitalgorithm.h"
#include "qftools.h"
#include <QMessageBox>
#include <QObject>
#include <cfloat>
#include <cstdio>
#include <iostream>
#include "qfrawdatarecord.h"
#include "../extlibs/MersenneTwister.h"

void QFFitAlgorithm::Functor::evaluateJacobian(double* evalout, const double* params) {
}



void QFFitAlgorithm::FitResult::addNumber(QString resultName, double value, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreNumber;
    r.dvalue=value;
    r.unit=unit;

    params[resultName]=r;
}

void QFFitAlgorithm::FitResult::addNumberList(QString resultName, QVector<double>& value, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreNumberVector;
    r.dvec=value;
    r.unit=unit;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addNumberMatrix(QString resultName, QVector<double>& value, int columns, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreNumberMatrix;
    r.dvec=value;
    r.columns=columns;
    r.unit=unit;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addNumberList(QString resultName, double* value, int items, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreNumberVector;
    r.dvec.clear();
    for (int i=0; i<items; i++) r.dvec.append(value[i]);
    r.unit=unit;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addNumberMatrix(QString resultName, double* value, int columns, int rows, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreNumberVector;
    r.dvec.clear();
    for (int i=0; i<rows*columns; i++) r.dvec.append(value[i]);
    r.columns=columns;
    r.unit=unit;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addNumberError(QString resultName, double value, double error, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreNumberError;
    r.dvalue=value;
    r.derror=error;
    r.unit=unit;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addInteger(QString resultName, int64_t value, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreInteger;
    r.ivalue=value;
    r.unit=unit;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addString(QString resultName, QString value) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreString;
    r.svalue=value;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addBoolean(QString resultName, bool value) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreBoolean;
    r.bvalue=value;

    params[resultName]=r;
}

QString  QFFitAlgorithm::FitResult::getAsString(QString resultName) {
    QFRawDataRecord::evaluationResult r=params[resultName];
    switch(r.type) {
        case QFRawDataRecord::qfrdreBoolean: if (r.bvalue) return QObject::tr("true"); else return QObject::tr("false");
        case QFRawDataRecord::qfrdreInteger: return QObject::tr("%1 %2").arg(r.ivalue).arg(r.unit);
        case QFRawDataRecord::qfrdreNumber: return QObject::tr("%1 %2").arg(r.dvalue).arg(r.unit);
        case QFRawDataRecord::qfrdreNumberVector: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) s=s+"; ";
                s=s+QString::number(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case QFRawDataRecord::qfrdreNumberMatrix: {
            QString s="(";
            for (int i=0; i<r.dvec.size(); i++) {
                if (i>0) {
                    if (i%r.columns==0) s=s+";; ";
                    else s=s+"; ";
                }
                s=s+QString::number(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case QFRawDataRecord::qfrdreNumberError: return QObject::tr("(%1 +/- %2) %3").arg(r.dvalue).arg(r.derror).arg(r.unit);
        case QFRawDataRecord::qfrdreString: return r.svalue;
        default: return ("");
    }
    return QString("");
}




QFFitAlgorithm::FitQFFitFunctionFunctor::FitQFFitFunctionFunctor(QFFitFunction* model, const double* currentParams, const bool* fixParams, const double* dataX, const double* dataY, const double* dataWeight, uint64_t M):
    QFFitAlgorithm::FitFunctionFunctor(dataX, dataY, dataWeight,M)
{
    m_model=model;
    m_N=model->paramCount();
    functorFromModel=NULL;
    modelFromFunctor=NULL;

    m_modelparamsFix=duplicateArray(fixParams, m_N);

    // now we calculate the mapping of the data
    m_paramCount=0;
    for (int i=0; i<m_N; i++) {
        if (!fixParams[i]) {
            QFFitFunction::ParameterDescription d=model->getDescription(i);
            if (d.fit && model->isParameterVisible(i, currentParams)) {
                m_paramCount++;
            }
        }
    }
    functorFromModel=(int*)qfCalloc(m_N, sizeof(int));
    modelFromFunctor=(int*)qfCalloc(m_paramCount, sizeof(int));
    int pid=0;
    for (int i=0; i<m_N; i++) {
        functorFromModel[i]=-1;
        if (!fixParams[i]) {
            QFFitFunction::ParameterDescription d=model->getDescription(i);
            if (d.fit && model->isParameterVisible(i, currentParams)) {
                functorFromModel[i]=pid;
                modelFromFunctor[pid]=i;
                //printf("   mapping m=%2d -> f=%2d [%s]\n", i, pid, d.id.toStdString().c_str());
                pid++;
            }
        }
    }
    m_modelParams=(double*)qfCalloc(m_N, sizeof(double));
    for (int i=0; i<m_N; i++) {
        m_modelParams[i]=currentParams[i];
    }
};

QFFitAlgorithm::FitQFFitFunctionFunctor::~FitQFFitFunctionFunctor() {
    qfFree(functorFromModel);
    qfFree(modelFromFunctor);
    qfFree(m_modelParams);
    qfFree(m_modelparamsFix);
}

double* QFFitAlgorithm::FitQFFitFunctionFunctor::createMappedArrayForFunctor(const double* modelData) {
    double* result=(double*)qfCalloc(m_paramCount, sizeof(double));

    for (register int i=0; i<m_paramCount; i++) {
        result[i]=modelData[modelFromFunctor[i]];
    }

    return result;
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::mapArrayFromModelToFunctor(double* functorData, const double* modelData) {
    for (register int i=0; i<m_paramCount; i++) {
        functorData[i]=modelData[modelFromFunctor[i]];
    }
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::mapArrayFromFunctorToModel(double* modelData, const double* functorData) {
    for (register int i=0; i<m_paramCount; i++) {
        modelData[modelFromFunctor[i]]=functorData[i];
    }
}



void QFFitAlgorithm::FitQFFitFunctionFunctor::evaluate(double* evalout, const double* params) {
    mapArrayFromFunctorToModel(m_modelParams, params);
    /*std::cout<<"N="<<m_N<<" Q="<<m_paramCount<<" M="<<get_evalout()<<"  P = ( ";
    for (register int i=0; i<m_N; i++) {
        if (i>0) std::cout<<", ";
        std::cout<<m_modelParams[i];
    }*/
    /*std::cout<<"P = ( ";
    for (register int i=0; i<m_paramCount; i++) {
        if (i>0) std::cout<<", ";
        std::cout<<params[i];
    }
    std::cout<<" )\n";*/
    register double v;
    register int ecount=get_evalout();
    QVector<double> evals(ecount, 0.0);
    m_model->multiEvaluate(evals.data(), m_dataX, ecount, m_modelParams);
    for (register int i=0; i<ecount; i++) {
        v = ( m_dataY[i] -  evals[i] ) / m_dataWeight[i];
        if (!QFFloatIsOK(v)) {
            v=0;
        }
        evalout[i]=v;
    }
    //mapArrayFromModelToFunctor(params, m_modelParams);
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::evaluateJacobian(double* evalout, const double* params) {
    mapArrayFromFunctorToModel(m_modelParams, params);
    register int pcount=get_paramcount();
    register int ecount=get_evalout();
    double* p=(double*)qfCalloc(m_N, sizeof(double));
    for (register int i=0; i<ecount; i++) {
        register int offset=i*pcount;
        for (register int j=0; j<m_N; j++) { p[j]=0; }
        m_model->evaluateDerivatives(p, m_dataX[i], m_modelParams);
        for (register int j=0; j<pcount; j++) {
            evalout[offset+j]=-1.0*p[modelFromFunctor[j]]/m_dataWeight[i];
        }
    }
    //std::cout<<"jac"<<doublevectortostr(params, pcount)<<"=    [ecount="<<ecount<<", pcount="<<pcount<<"";
    //std::cout<<"modelFromFunctor"<<intvectortostr(modelFromFunctor, pcount)<<" ]\n";
    //std::cout<<doublearraytostr(evalout, pcount, ecount)<<std::endl;
    qfFree(p);
}



QFFitAlgorithm::QFFitAlgorithm()
{
    m_bootstrapFraction=0.6;
    m_bootstrapDistortion=0.075;
    m_reporter=NULL;
    m_bootstrapRepeats=20;
    m_errorEstimateModeFit=fpeAlgorithm;
    m_bootstrapFraction=0.7;
}

QFFitAlgorithm::~QFFitAlgorithm()
{

}

void QFFitAlgorithm::setBootstrapRepeats(int repeats)
{
    m_bootstrapRepeats=repeats;
}

int QFFitAlgorithm::getBootstrapRepeats() const
{
    return m_bootstrapRepeats;
}

void QFFitAlgorithm::setBootstrapFraction(double fraction)
{
    m_bootstrapFraction=fraction;
}

double QFFitAlgorithm::getBootstrapFraction() const
{
    return m_bootstrapFraction;
}

void QFFitAlgorithm::setBootstrapDistortion(double distortion)
{
    m_bootstrapDistortion=distortion;
}

double QFFitAlgorithm::getBootstrapDistortion() const
{
    return m_bootstrapDistortion;
}

void QFFitAlgorithm::setErrorEstimateModeFit(QFFitAlgorithm::FitParameterErrorEstimates mode)
{
    m_errorEstimateModeFit=mode;
}

void QFFitAlgorithm::setErrorEstimateModeFit(int mode)
{
    setErrorEstimateModeFit(QFFitAlgorithm::FitParameterErrorEstimates(mode));
}

void QFFitAlgorithm::setErrorEstimateModeFit(QFFitAlgorithm::FitParameterErrorEstimates mode, int repeats)
{
    setErrorEstimateModeFit(mode);
    m_bootstrapRepeats=repeats;
}

void QFFitAlgorithm::setErrorEstimateModeFit(QFFitAlgorithm::FitParameterErrorEstimates mode, int repeats, double bootstrapFraction)
{
    setErrorEstimateModeFit(mode, repeats);
    m_bootstrapFraction=bootstrapFraction;
}

void QFFitAlgorithm::setErrorEstimateModeFit(QFFitAlgorithm::FitParameterErrorEstimates mode, int repeats, double bootstrapFraction, double distortion)
{
    setErrorEstimateModeFit(mode, repeats, bootstrapFraction);
    m_bootstrapDistortion=distortion;
}

QFFitAlgorithm::FitParameterErrorEstimates QFFitAlgorithm::getErrorEstimateModeFit() const
{
    return m_errorEstimateModeFit;
}

void QFFitAlgorithm::readErrorEstimateParametersFit(const QFProperties *props, const QString &prefix)
{
    setErrorEstimateModeFit(props->getProperty(prefix+"FIT_ERRORESTIMATE_MODE", getErrorEstimateModeFit()).toInt());
    setBootstrapDistortion(props->getProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_DISTORTION", getBootstrapDistortion()).toDouble());
    setBootstrapFraction(props->getProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_FRACTION", getBootstrapFraction()).toDouble());
    setBootstrapRepeats(props->getProperty(prefix+"FIT_ERRORESTIMATE_BOOTSTRAP_REPEATS", getBootstrapRepeats()).toDouble());
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fit(double *paramsOut, double *paramErrorsOut, const double *dataX, const double *dataY, const double *dataWeight, uint64_t N, QFFitFunction *model, const double *initialParams, const bool *fixParams, const double *paramsMin, const double *paramsMax) {
    QFFitAlgorithm::FitResult result;
    const double* pparamsMin=paramsMin;
    double* ppparamsMin=NULL;
    const double* pparamsMax=paramsMax;
    double* ppparamsMax=NULL;
    const double* ddataWeight=dataWeight;
    double* dddataWeight=NULL;
    const bool* pparamsFix=fixParams;
    bool* ppparamsFix=NULL;
    if (paramsMin==NULL) {
        ppparamsMin=(double*)qfCalloc(model->paramCount(), sizeof(double));
        for (int i=0; i<model->paramCount(); i++) {
            ppparamsMin[i]=-DBL_MAX;
        }
        pparamsMin=ppparamsMin;
    }
    if (paramsMax==NULL) {
        ppparamsMax=(double*)qfCalloc(model->paramCount(), sizeof(double));
        for (int i=0; i<model->paramCount(); i++) {
            ppparamsMax[i]=DBL_MAX;
        }
        pparamsMax=ppparamsMax;
    }
    if (dataWeight==NULL) {
        dddataWeight=(double*)qfCalloc(N, sizeof(double));
        for (uint64_t i=0; i<N; i++) {
            dddataWeight[i]=1.0;
        }
        ddataWeight=dddataWeight;
    }
    if (fixParams==NULL) {
        ppparamsFix=(bool*)qfCalloc(model->paramCount(), sizeof(bool));
        for (int i=0; i<model->paramCount(); i++) {
            ppparamsFix[i]=false;
        }
        pparamsFix=ppparamsFix;
    }

    QFFitAlgorithm::FitQFFitFunctionFunctor fm(model, initialParams, pparamsFix, dataX, dataY, ddataWeight, N);

    fm.setBootstrappingEnabled(false);

    //qDebug()<<"fm.get_paramcount()="<<fm.get_paramcount();

    double* tparamsMin=fm.createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm.createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tinitialParams=fm.createMappedArrayForFunctor(initialParams);

    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, &fm, tparamsMin, tparamsMax);

    if (m_errorEstimateModeFit==fpeBootstrapping) {
        fm.setBootstrappingFraction(m_bootstrapFraction, false);
        fm.setBootstrappingEnabled(true, false);

        double* bsParams=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
        double* bsErrors=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
        double* bsParamSum=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
        double* bsParamSqrSum=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
        double* bsInitParams=duplicateArray(tparamsOut, fm.get_paramcount());
        MTRand rng;

        // initialize vectors and distor last fit results for initial fit parameter values,
        // ensure that distorted values are within the range of allowed values!
        for (int j=0; j<fm.get_paramcount(); j++) {
            bsParamSum[j]=0.0;
            bsParamSqrSum[j]=0.0;
            if (m_bootstrapDistortion>0.0) {
                bsInitParams[j]=tparamsOut[j]*(1.0+(rng.rand()-0.5)*2.0*m_bootstrapDistortion);
                if (bsInitParams[j]<=tparamsMin[j] || bsInitParams[j]>=tparamsMax[j]) bsInitParams[j]=tparamsOut[j];
            } else {
                bsInitParams[j]=tparamsOut[j];
            }
        }
        //qDebug()<<"BS>> fit_result: "<<arrayToString(tparamsOut, fm.get_paramcount());
        //qDebug()<<"BS>> init_array: "<<arrayToString(bsInitParams, fm.get_paramcount());
        double N=0;
        for (int i=0; i<m_bootstrapRepeats; i++) {
            fm.prepareBootstrapSelection();
            intFit(bsParams, bsErrors, bsInitParams, &fm, tparamsMin, tparamsMax);
            bool ok=true;
            for (int j=0; j<fm.get_paramcount(); j++) {
                const double p=bsParams[j];
                if (!QFFloatIsOK(p)) {
                    ok=false;
                    break;
                }
            }
            if (ok) {
                for (int j=0; j<fm.get_paramcount(); j++) {
                    const double p=bsParams[j];
                    bsParamSum[j]=bsParamSum[j]+p;
                    bsParamSqrSum[j]=bsParamSqrSum[j]+p*p;
                }
                N++;
            }
            //qDebug()<<"BS>>   step "<<i<<" ok="<<ok<<"     p="<<arrayToString(bsParams, fm.get_paramcount());
        }

        if (N>1) {
            for (int j=0; j<fm.get_paramcount(); j++) {
                tparamErrorsOut[j]=sqrt((bsParamSqrSum[j]-bsParamSum[j]*bsParamSum[j]/N)/(N-1.0));
            }
        } else {
            for (int j=0; j<fm.get_paramcount(); j++) {
                tparamErrorsOut[j]=0;
            }
        }
        result.addString("errorEstimateMode", "bootstrapping");
        result.addInteger("bootstrapRepeats", m_bootstrapRepeats);
        result.addInteger("bootstrapSuccessfulRepeats", N);
        result.addNumber("bootstrapFraction", m_bootstrapFraction);
        result.addNumber("bootstrapDistortion", m_bootstrapDistortion);
        result.addNumberList("bootstrapInitialparams", bsInitParams, fm.get_paramcount());
        qfFree(bsParams);
        qfFree(bsErrors);
        qfFree(bsParamSum);
        qfFree(bsParamSqrSum);
        qfFree(bsInitParams);
        fm.setBootstrappingEnabled(false, true);
    } else {
        result.addString("errorEstimateMode", "by_fit_algorithm");
    }

    for (int i=0; i<model->paramCount(); i++) {
        paramsOut[i]=initialParams[i];
        paramErrorsOut[i]=0;
    }
    fm.mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    fm.mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    qfFree(tparamsMax);
    qfFree(tparamsMin);
    qfFree(tparamErrorsOut);
    qfFree(tparamsOut);
    qfFree(tinitialParams);

    if (ppparamsMin==NULL) qfFree(ppparamsMin);
    if (ppparamsMax==NULL) qfFree(ppparamsMax);
    if (ppparamsFix==NULL) qfFree(ppparamsFix);
    if (dddataWeight==NULL) qfFree(dddataWeight);
    return result;
}


QFFitAlgorithm::FitResult QFFitAlgorithm::optimize(double* paramsOut, double* paramErrorsOut, Functor* model, const double* initialParams, const bool* fixParams,  const double* paramsMin,  const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    const double* pparamsMin=paramsMin;
    double* ppparamsMin=NULL;
    const double* pparamsMax=paramsMax;
    double* ppparamsMax=NULL;
    const bool* pparamsFix=fixParams;
    bool* ppparamsFix=NULL;
    if (paramsMin==NULL) {
        ppparamsMin=(double*)qfCalloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsMin[i]=-DBL_MAX;
        }
        pparamsMin=ppparamsMin;
    }
    if (paramsMax==NULL) {
        ppparamsMax=(double*)qfCalloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsMax[i]=DBL_MAX;
        }
        pparamsMax=ppparamsMax;
    }

    if (fixParams==NULL) {
        ppparamsFix=(bool*)qfCalloc(model->get_paramcount(), sizeof(bool));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsFix[i]=false;
        }
        pparamsFix=ppparamsFix;
    }



    QFFitAlgorithm::FitQFOptimizeFunctionFunctor* fm;
    QFFitAlgorithm::FunctorBootstrapInterface* fmbs=dynamic_cast<QFFitAlgorithm::FunctorBootstrapInterface*>(model);
    if (fmbs) {
        fm=new QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor(model, initialParams, pparamsFix);
    } else {
        fm=new QFFitAlgorithm::FitQFOptimizeFunctionFunctor(model, initialParams, pparamsFix);
    }
    fmbs=dynamic_cast<QFFitAlgorithm::FunctorBootstrapInterface*>(fm);

    double* tparamsMin=fm->createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm->createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
    double* tinitialParams=fm->createMappedArrayForFunctor(initialParams);

    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, fm, tparamsMin, tparamsMax);


    //qDebug()<<"### OPTIMIZE: "<<m_errorEstimateModeFit<<fmbs;
    if (m_errorEstimateModeFit==fpeBootstrapping && fmbs) {
        fmbs->setBootstrappingFraction(m_bootstrapFraction, false);
        fmbs->setBootstrappingEnabled(true, false);

        double* bsParams=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
        double* bsErrors=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
        double* bsParamSum=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
        double* bsParamSqrSum=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
        double* bsInitParams=duplicateArray(tparamsOut, fm->get_paramcount());
        MTRand rng;

        // initialize vectors and distor last fit results for initial fit parameter values,
        // ensure that distorted values are within the range of allowed values!
        for (int j=0; j<fm->get_paramcount(); j++) {
            bsParamSum[j]=0.0;
            bsParamSqrSum[j]=0.0;
            if (m_bootstrapDistortion>0.0) {
                bsInitParams[j]=tparamsOut[j]*(1.0+(rng.rand()-0.5)*2.0*m_bootstrapDistortion);
                if (bsInitParams[j]<=tparamsMin[j] || bsInitParams[j]>=tparamsMax[j]) bsInitParams[j]=tparamsOut[j];
            } else {
                bsInitParams[j]=tparamsOut[j];
            }
        }
        //qDebug()<<"BS>> fit_result: "<<arrayToString(tparamsOut, fm->get_paramcount());
        //qDebug()<<"BS>> init_array: "<<arrayToString(bsInitParams, fm->get_paramcount());
        double N=0;
        for (int i=0; i<m_bootstrapRepeats; i++) {
            fmbs->prepareBootstrapSelection();
            intFit(bsParams, bsErrors, bsInitParams, fm, tparamsMin, tparamsMax);
            bool ok=true;
            for (int j=0; j<fm->get_paramcount(); j++) {
                const double p=bsParams[j];
                if (!QFFloatIsOK(p)) {
                    ok=false;
                    break;
                }
            }
            if (ok) {
                for (int j=0; j<fm->get_paramcount(); j++) {
                    const double p=bsParams[j];
                    bsParamSum[j]=bsParamSum[j]+p;
                    bsParamSqrSum[j]=bsParamSqrSum[j]+p*p;
                }
                N++;
            }
            //qDebug()<<"BS>>   step "<<i<<" ok="<<ok<<"     p="<<arrayToString(bsParams, fm->get_paramcount());
        }

        if (N>1) {
            for (int j=0; j<fm->get_paramcount(); j++) {
                tparamErrorsOut[j]=sqrt((bsParamSqrSum[j]-bsParamSum[j]*bsParamSum[j]/N)/(N-1.0));
            }
        } else {
            for (int j=0; j<fm->get_paramcount(); j++) {
                tparamErrorsOut[j]=0;
            }
        }
        result.addString("errorEstimateMode", "bootstrapping");
        result.addInteger("bootstrapRepeats", m_bootstrapRepeats);
        result.addInteger("bootstrapSuccessfulRepeats", N);
        result.addNumber("bootstrapFraction", m_bootstrapFraction);
        result.addNumber("bootstrapDistortion", m_bootstrapDistortion);
        result.addNumberList("bootstrapInitialparams", bsInitParams, fm->get_paramcount());
        qfFree(bsParams);
        qfFree(bsErrors);
        qfFree(bsParamSum);
        qfFree(bsParamSqrSum);
        qfFree(bsInitParams);
        fmbs->setBootstrappingEnabled(false, true);
    } else {
        result.addString("errorEstimateMode", "by_fit_algorithm");
    }


    for (int i=0; i<model->get_paramcount(); i++) {
        paramsOut[i]=initialParams[i];
        paramErrorsOut[i]=0;
    }
    fm->mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    fm->mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    qfFree(tparamsMax);
    qfFree(tparamsMin);
    qfFree(tparamErrorsOut);
    qfFree(tparamsOut);
    qfFree(tinitialParams);

    if (ppparamsMin) qfFree(ppparamsMin);
    if (ppparamsMax) qfFree(ppparamsMax);
    if (ppparamsFix) qfFree(ppparamsFix);
    delete fm;
    return result;
}

bool QFFitAlgorithm::displayConfig() {
    QMessageBox::information(NULL, name(), QObject::tr("No configuration dialog is supplied for this optimization algorithm!"));
    return false;
}

bool QFFitAlgorithm::isDeprecated() const
{
    return false;
}

















QFFitAlgorithm::FitQFOptimizeFunctionFunctor::FitQFOptimizeFunctionFunctor(Functor* model, const double *currentParams, const bool *fixParams):
    QFFitAlgorithm::Functor(model->get_evalout())
{
    m_model=model;
    m_N=model->get_paramcount();
    functorFromModel=NULL;
    modelFromFunctor=NULL;

    // now we calculate the mapping of the data
    m_paramCount=0;
    for (int i=0; i<m_N; i++) {
        if (!fixParams[i]) {
            m_paramCount++;
        }
    }
    functorFromModel=(int*)qfCalloc(m_N, sizeof(int));
    modelFromFunctor=(int*)qfCalloc(m_paramCount, sizeof(int));
    int pid=0;
    for (int i=0; i<m_N; i++) {
        functorFromModel[i]=-1;
        if (!fixParams[i]) {
            functorFromModel[i]=pid;
            modelFromFunctor[pid]=i;
            //printf("   mapping m=%2d -> f=%2d [%s]\n", i, pid, d.id.toStdString().c_str());
            pid++;
        }
    }
    m_modelParams=(double*)qfCalloc(m_N, sizeof(double));
    for (int i=0; i<m_N; i++) {
        m_modelParams[i]=currentParams[i];
    }
};

QFFitAlgorithm::FitQFOptimizeFunctionFunctor::~FitQFOptimizeFunctionFunctor() {
    qfFree(functorFromModel);
    qfFree(modelFromFunctor);
    qfFree(m_modelParams);
}

double* QFFitAlgorithm::FitQFOptimizeFunctionFunctor::createMappedArrayForFunctor(const double* modelData) {
    double* result=(double*)qfCalloc(m_paramCount, sizeof(double));

    for (register int i=0; i<m_paramCount; i++) {
        result[i]=modelData[modelFromFunctor[i]];
    }

    return result;
}

void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::mapArrayFromModelToFunctor(double* functorData, const double* modelData) {
    for (register int i=0; i<m_paramCount; i++) {
        functorData[i]=modelData[modelFromFunctor[i]];
    }
}

void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::mapArrayFromFunctorToModel(double* modelData, const double* functorData) {
    for (register int i=0; i<m_paramCount; i++) {
        modelData[modelFromFunctor[i]]=functorData[i];
    }
}


void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::evaluate(double* evalout, const double* params) {
    mapArrayFromFunctorToModel(m_modelParams, params);
    m_model->evaluate(evalout, m_modelParams);
}

void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::evaluateJacobian(double* evalout, const double* params) {
    mapArrayFromFunctorToModel(m_modelParams, params);
    m_model->evaluateJacobian(evalout, m_modelParams);
}

QFFitAlgorithm::FitFunctionFunctor::FitFunctionFunctor(const double *dataX, const double *dataY, const double *dataWeight, uint64_t M):
    QFFitAlgorithm::Functor(M)
{
    m_bootstrapEnabled=false;
    m_bootstrapFraction=0.7;
    m_dataX=i_dataX=dataX;
    m_dataY=i_dataY=dataY;
    m_dataWeight=i_dataWeight=dataWeight;
    m_M=i_M=M;
    m_evalout=M;
}

QFFitAlgorithm::FitFunctionFunctor::~FitFunctionFunctor()
{
}

const double * QFFitAlgorithm::FitFunctionFunctor::getDataX() const
{
    return m_dataX;
}

const double * QFFitAlgorithm::FitFunctionFunctor::getDataY() const
{
    return m_dataY;
}

const double * QFFitAlgorithm::FitFunctionFunctor::getDataWeight() const
{
    return m_dataWeight;
}

uint64_t QFFitAlgorithm::FitFunctionFunctor::getDataPoints() const
{
    return m_M;
}

void QFFitAlgorithm::FitFunctionFunctor::setDataX(const double *data)
{
    i_dataX=data;
    if (m_bootstrapEnabled) m_dataX=data;
}

void QFFitAlgorithm::FitFunctionFunctor::setDataY(const double *data)
{
    i_dataY=data;
    if (m_bootstrapEnabled) m_dataY=data;
}

void QFFitAlgorithm::FitFunctionFunctor::setDataWeight(const double *data)
{
    i_dataWeight=data;
    if (m_bootstrapEnabled) m_dataWeight=data;
}

void QFFitAlgorithm::FitFunctionFunctor::setDataPoints(uint64_t data)
{
    i_M=data;
    if (m_bootstrapEnabled) m_M=m_evalout=data;
}

void QFFitAlgorithm::FitFunctionFunctor::prepareBootstrapSelection()
{
    int64_t mBS=qRound64(double(i_M)*m_bootstrapFraction);
    if (mBS<get_paramcount()+1) mBS=get_paramcount()+1;

    if (m_bootstrapEnabled) {
        bootstrapIDs.clear();
        for (uint64_t i=0; i<i_M; i++) bootstrapIDs.append(i);
        qfShuffleInplace(bootstrapIDs);
        while (bootstrapIDs.size()>mBS) bootstrapIDs.removeLast();
        qSort(bootstrapIDs);
        //qDebug()<<"mBS="<<mBS<<"  IDS="<<bootstrapIDs;
    } else {
        bootstrapIDs.clear();
    }
    reapplyBootstrapselection();
}

void QFFitAlgorithm::FitFunctionFunctor::reapplyBootstrapselection()
{
    uint64_t mBS=bootstrapIDs.size();

    if (m_bootstrapEnabled && mBS>0) {
        if (i_dataX) bootstrappedX.resize(mBS);
        if (i_dataY) bootstrappedY.resize(mBS);
        if (i_dataWeight) bootstrappedW.resize(mBS);
        m_dataX=NULL;
        m_dataY=NULL;
        m_dataWeight=NULL;
        if (i_dataX) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedX[i]=i_dataX[bootstrapIDs[i]];
            }
            m_dataX=bootstrappedX.constData();
        }
        if (i_dataY) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedY[i]=i_dataY[bootstrapIDs[i]];
            }
            m_dataY=bootstrappedY.constData();
        }
        if (i_dataWeight) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedW[i]=i_dataWeight[bootstrapIDs[i]];
            }
            m_dataWeight=bootstrappedW.constData();
        }
        m_M=qMin(bootstrappedX.size(), bootstrappedY.size());
    } else {
        bootstrappedX.clear();
        bootstrappedY.clear();
        bootstrappedW.clear();
        m_dataX=i_dataX;
        m_dataY=i_dataY;
        m_dataWeight=i_dataWeight;
        m_M=i_M;
    }
    m_evalout=m_M;

}

void QFFitAlgorithm::FitFunctionFunctor::setBootstrappingEnabled(bool enabled, bool prepBootstrapping)
{
    m_bootstrapEnabled=enabled;
    if (enabled && prepBootstrapping) prepareBootstrapSelection();
}

bool QFFitAlgorithm::FitFunctionFunctor::getBootstrappingEnabled() const
{
    return m_bootstrapEnabled;
}

void QFFitAlgorithm::FitFunctionFunctor::setBootstrappingFraction(double fraction, bool prepBootstrapping)
{
    m_bootstrapFraction=qBound<double>(0.0, fraction, 1.0);
    if (m_bootstrapEnabled && prepBootstrapping) prepareBootstrapSelection();
}


double QFFitAlgorithm::FitFunctionFunctor::getBootstrappingFraction() const
{
    return m_bootstrapFraction;
}

QFFitAlgorithm::IRLSFunctorAdaptor::IRLSFunctorAdaptor(QFFitAlgorithm::FitFunctionFunctor *functor, double irls_parameter):
    Functor(functor->get_evalout())
{
    this->irls_parameter=irls_parameter;
    this->irls_functor=functor;
    const uint64_t N=functor->getDataPoints();
    irls_weights=(double*)qfMalloc(N*sizeof(double));
    for (uint64_t i=0; i<N; i++) {
        irls_weights[i]=1.0/double(N);
    }
    oldWeights=irls_functor->getDataWeight();
    irls_functor->setDataWeight(irls_weights);
    irls_functor->reapplyBootstrapselection();
}

QFFitAlgorithm::IRLSFunctorAdaptor::~IRLSFunctorAdaptor() {
    irls_functor->setDataWeight(oldWeights);
    irls_functor->reapplyBootstrapselection();
    qfFree(irls_weights);
}

void QFFitAlgorithm::IRLSFunctorAdaptor::evaluate(double *evalout, const double *params)
{
    irls_functor->evaluate(evalout, params);
}

void QFFitAlgorithm::IRLSFunctorAdaptor::evaluateJacobian(double *evalout, const double *params)
{
    irls_functor->evaluateJacobian(evalout, params);
}

int QFFitAlgorithm::IRLSFunctorAdaptor::get_paramcount() const {
    return irls_functor->get_paramcount();
}

bool QFFitAlgorithm::IRLSFunctorAdaptor::get_implementsJacobian() const {
    return irls_functor->get_implementsJacobian();
}

void QFFitAlgorithm::IRLSFunctorAdaptor::irlsReweight(const double *params)
{
    double* err=(double*)qfMalloc(irls_functor->get_evalout()*sizeof(double));

    if (irls_functor->get_evalout()>=irls_functor->getDataPoints()) {
        // calculate error vector (f(xi,p)-yi)/sigmai
        irls_functor->evaluate(err, params);
        for (uint64_t i=0; i<irls_functor->get_evalout(); i++) {
            // calculate unweighted error f(xi,p)-yi
            err[i]=err[i]*irls_weights[i];
            // calculate new weight
            irls_weights[i]=pow(fabs(err[i]), (irls_parameter-2.0)/2.0);
        }
    }

    qfFree(err);
}




QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor::FitQFOptimizeFunctionBootstrapFunctor(QFFitAlgorithm::Functor *model, const double *currentParams, const bool *fixParams):
    FitQFOptimizeFunctionFunctor(model, currentParams, fixParams)
{

}

void QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor::prepareBootstrapSelection()
{
    if (getModelAsBootstrap())
        getModelAsBootstrap()->prepareBootstrapSelection();
}

void QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor::reapplyBootstrapselection()
{
    if (getModelAsBootstrap())
        getModelAsBootstrap()->reapplyBootstrapselection();
}

void QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor::setBootstrappingEnabled(bool enabled, bool prepBootstrapping)
{
    if (getModelAsBootstrap())
        getModelAsBootstrap()->setBootstrappingEnabled(enabled, prepBootstrapping);
}

void QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor::setBootstrappingFraction(double fraction, bool prepBootstrapping)
{
    if (getModelAsBootstrap())
        getModelAsBootstrap()->setBootstrappingFraction(fraction, prepBootstrapping);
}

QFFitAlgorithm::FunctorBootstrapInterface *QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor::getModelAsBootstrap() const
{
    return dynamic_cast<QFFitAlgorithm::FunctorBootstrapInterface*>(m_model);
}
