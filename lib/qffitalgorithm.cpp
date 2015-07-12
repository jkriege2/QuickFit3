/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
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
#include "qftools.h"
#include "statistics_tools.h"
#include "qfglobalfittool.h"

//#define DEBUG_QFFitAlgorithm


void QFFitAlgorithm::Functor::evaluateJacobian(double* evalout, const double* params) const {
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::Functor::evaluateJacobian ... run numerical evalJac!";
#endif
    evaluateJacobianNum(evalout, params);
}

void QFFitAlgorithm::Functor::evaluateJacobianNum(double *evalout, const double *params, double h) const
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::Functor::evaluateJacobianNum(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    int pcount=get_paramcount();
    uint64_t outcount=get_evalout();
    uint64_t i=0;
    QVector<double> dat0=duplicateArrayV(params, pcount);
    for (uint64_t io=0; io<outcount*pcount; io++) {
        evalout[io]=0;
    }

    QVector<double> centerout(outcount, 0.0);
    for (int ip=0; ip<pcount; ip++) {
        QVector<double> dat=dat0;
        dat[ip]=dat0[ip]+2.0*h;
        evaluate(centerout.data(), dat.data());
        for (uint64_t io=0; io<outcount; io++) {
            evalout[ip*outcount+io]=evalout[ip*outcount+io]-centerout[io];
        }

        dat[ip]=dat0[ip]+h;
        evaluate(centerout.data(), dat.data());
        for (uint64_t io=0; io<outcount; io++) {
            evalout[ip*outcount+io]=evalout[ip*outcount+io]+8.0*centerout[io];
        }

        dat[ip]=dat0[ip]-h;
        evaluate(centerout.data(), dat.data());
        for (uint64_t io=0; io<outcount; io++) {
            evalout[ip*outcount+io]=evalout[ip*outcount+io]-8.0*centerout[io];
        }

        dat[ip]=dat0[ip]-2.0*h;
        evaluate(centerout.data(), dat.data());
        for (uint64_t io=0; io<outcount; io++) {
            evalout[ip*outcount+io]=evalout[ip*outcount+io]+centerout[io];
        }

    }


    for (uint64_t io=0; io<outcount*pcount; io++) {
        evalout[io]=evalout[io]/(12.0*h);
    }
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


void QFFitAlgorithm::FitResult::addNumberList(QString resultName, const double* value, int items, QString unit) {
    QFRawDataRecord::evaluationResult r;

    r.type=QFRawDataRecord::qfrdreNumberVector;
    r.dvec.clear();
    for (int i=0; i<items; i++) r.dvec.append(value[i]);
    r.unit=unit;

    params[resultName]=r;
}


void QFFitAlgorithm::FitResult::addNumberMatrix(QString resultName, const double* value, int columns, int rows, QString unit) {
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
                s=s+QLocale::system().toString(r.dvec[i]);
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
                s=s+QLocale::system().toString(r.dvec[i]);
            }
            return s+") "+r.unit;
        }
        case QFRawDataRecord::qfrdreNumberError: return QObject::tr("(%1 +/- %2) %3").arg(r.dvalue).arg(r.derror).arg(r.unit);
        case QFRawDataRecord::qfrdreString: return r.svalue;
        default: return ("");
    }
    return QString("");
}




QFFitAlgorithm::FitQFFitFunctionFunctor::FitQFFitFunctionFunctor(QFFitFunction *model, const double* currentParams, const bool* fixParams, const double* dataX, const double* dataY, const double* dataWeight, uint64_t M):
    QFFitAlgorithm::FitFunctionFunctor(dataX, dataY, dataWeight,M)
{
    m_model=model;
    fitLogY=false,
    m_N=model->paramCount();
    functorFromModel=NULL;
    modelFromFunctor=NULL;
    enableParameterTransforms=true;

    m_modelparamsFix=duplicateArray(fixParams, m_N);
    hasParameterTransforms=false;

    // now we calculate the mapping of the data
    m_paramCount=0;
    for (int i=0; i<m_N; i++) {
        QFFitFunction::ParameterDescription d=model->getDescription(i);
        if (!fixParams[i]) {
            if (d.fit && model->isParameterVisible(i, currentParams)) {
                paramTransforms<<d.parameterType;
                m_paramCount++;
                hasParameterTransforms=hasParameterTransforms||(d.parameterType!=QFFitFunction::StandardParameter);
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

double* QFFitAlgorithm::FitQFFitFunctionFunctor::createMappedArrayForFunctor(const double* modelData) const {
    double* result=(double*)qfCalloc(m_paramCount, sizeof(double));

    for (register int i=0; i<m_paramCount; i++) {
        result[i]=modelData[modelFromFunctor[i]];
    }

    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(result);
    return result;
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::mapArrayFromModelToFunctor(double* functorData, const double* modelData) const {
    for (register int i=0; i<m_paramCount; i++) {
        functorData[i]=modelData[modelFromFunctor[i]];
    }
    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(functorData);
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::mapArrayFromFunctorToModel(double* modelData, const double* functorData) const {
    for (register int i=0; i<m_paramCount; i++) {
        modelData[modelFromFunctor[i]]=functorData[i];
    }
    if (enableParameterTransforms&&hasParameterTransforms) backtransfromParameters(modelData);
}



void QFFitAlgorithm::FitQFFitFunctionFunctor::evaluate(double* evalout, const double* params) const {
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);


    register double v;
    register int ecount=get_evalout();
    QVector<double> evals(ecount, 0.0);
    m_model->multiEvaluate(evals.data(), m_dataX, ecount, mpt);
    if (m_dataWeight) {
        if (!fitLogY) {
            for (register int i=0; i<ecount; i++) {
                v = ( m_dataF[i] -  evals[i] ) / m_dataWeight[i];
                if (!QFFloatIsOK(v)) {
                    v=0;
                }
                evalout[i]=v;
            }
        } else {
            for (register int i=0; i<ecount; i++) {
                v = ( log(m_dataF[i]) -  log(evals[i]) ) /log(m_dataWeight[i]);
                if (!QFFloatIsOK(v)) {
                    v=0;
                }
                evalout[i]=v;
            }
        }
    } else{
        if (!fitLogY) {
            for (register int i=0; i<ecount; i++) {
                v = ( m_dataF[i] -  evals[i] );
                if (!QFFloatIsOK(v)) {
                    v=0;
                }
                evalout[i]=v;
            }
        } else {
            for (register int i=0; i<ecount; i++) {
                v = ( log(m_dataF[i]) -  log(evals[i]) );
                if (!QFFloatIsOK(v)) {
                    v=0;
                }
                evalout[i]=v;
            }
        }
    }

    qfFree(mpt);
    //mapArrayFromModelToFunctor(params, m_modelParams);
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::evaluateModelOnly(double *evalout, const double *params) const
{
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);


    register double v;
    register int ecount=get_evalout();
    QVector<double> evals(ecount, 0.0);
    m_model->multiEvaluate(evals.data(), m_dataX, ecount, mpt);
    for (register int i=0; i<ecount; i++) {
        v = evals[i];
        if (!QFFloatIsOK(v)) {
            v=0;
        }
        evalout[i]=v;
    }
    qfFree(mpt);
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::evaluateJacobian(double* evalout, const double* params) const {
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitQFFitFunctionFunctor::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);
    const int pcount=get_paramcount();
    const int ecount=get_evalout();
    double* p=(double*)qfCalloc(m_N, sizeof(double));
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitQFFitFunctionFunctor::evaluateJacobian  m_N="<<m_N<<"  pcount="<<pcount<<"  ecound="<<ecount<<"  m_dataWeight="<<m_dataWeight<<"  fitLogY="<<fitLogY<<"\n"; std::cout.flush();
#endif
    if (m_dataWeight) {
        for (register int i=0; i<ecount; i++) {
            const int offset=i*pcount;
            for (register int j=0; j<m_N; j++) { p[j]=0; }
            m_model->evaluateDerivatives(p, m_dataX[i], mpt);
            if (!fitLogY) {
                for ( int j=0; j<pcount; j++) {
                    evalout[offset+j]=-1.0*p[modelFromFunctor[j]]/m_dataWeight[i];
                }
            } else {
                double eval=m_model->evaluate(m_dataX[i], mpt);
                for ( int j=0; j<pcount; j++) {
                    evalout[offset+j]=-1.0/eval*p[modelFromFunctor[j]]/log(m_dataWeight[i]);
                }
            }
        }
    } else {
        for (register int i=0; i<ecount; i++) {
            const int offset=i*pcount;
            for (register int j=0; j<m_N; j++) { p[j]=0; }
            m_model->evaluateDerivatives(p, m_dataX[i], mpt);
            if (!fitLogY) {
                for ( int j=0; j<pcount; j++) {
                    evalout[offset+j]=-1.0*p[modelFromFunctor[j]];
                }
            } else {
                double eval=m_model->evaluate(m_dataX[i], mpt);
                for ( int j=0; j<pcount; j++) {
                    evalout[offset+j]=-1.0/eval*p[modelFromFunctor[j]];
                }
            }
        }
    }
    qfFree(p);
    qfFree(mpt);
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::transfromParameters(double *params) const
{
    if (enableParameterTransforms&&hasParameterTransforms) {
        register int pcount=get_paramcount();
        for (int i=0; i<pcount; i++) {
            switch (paramTransforms[i]) {
                case QFFitFunction::LogParameter: params[i]=log(params[i]); break;
                case QFFitFunction::CustomTramsformParameter: params[i]=m_model->customTransform(i, params[i]); break;
                default: break;
            }
        }
    }
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::backtransfromParameters(double *params) const
{
    if (enableParameterTransforms&&hasParameterTransforms) {
        register int pcount=get_paramcount();
        for (int i=0; i<pcount; i++) {
            switch (paramTransforms[i]) {
                case QFFitFunction::LogParameter: params[i]=exp(params[i]); break;
                case QFFitFunction::CustomTramsformParameter: params[i]=m_model->customBackTransform(i, params[i]); break;
                default: break;
            }
        }
    }
}

QVector<double> QFFitAlgorithm::FitQFFitFunctionFunctor::transfromParametersCopy(const double *params) const
{
    QVector<double> p=duplicateArrayV(params, get_paramcount());
    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(p.data());
    return p;
}

QVector<double> QFFitAlgorithm::FitQFFitFunctionFunctor::backtransfromParametersCopy(const double *params) const
{
    QVector<double> p=duplicateArrayV(params, get_paramcount());
    if (enableParameterTransforms&&hasParameterTransforms) backtransfromParameters(p.data());
    return p;
}



bool QFFitAlgorithm::functorHasWeights(const QFFitAlgorithm::Functor *f)
{
    if (f) {
        return f->isWeightedLSQ();
    } else {
        return false;
    }
//    {
//        const QFFitAlgorithm::FitFunctionFunctor* ff=dynamic_cast<const QFFitAlgorithm::FitFunctionFunctor*>(f);
//        if (ff ) return (ff->getDataWeight()!=NULL);
//    }
//    {
//        const QFFitAlgorithm::FitFunctionFunctor2D* ff=dynamic_cast<const QFFitAlgorithm::FitFunctionFunctor2D*>(f);
//        if (ff ) return (ff->getDataWeight()!=NULL);
//    }
//    {
//        const QFFitAlgorithm::FitFunctionFunctor3D* ff=dynamic_cast<const QFFitAlgorithm::FitFunctionFunctor3D*>(f);
//        if (ff ) return (ff->getDataWeight()!=NULL);
//    }

//    return false;
}

bool QFFitAlgorithm::functorAllWeightsOne(const QFFitAlgorithm::Functor *f)
{
    if (!f) return false;

    const QFFitMultiQFFitFunctionFunctor* ff=dynamic_cast<const QFFitMultiQFFitFunctionFunctor*>(f);
    if (ff ) {
        return ff->areAllWeightsOne();
    }

    uint64_t N=0;
    const double* w=functorGetWeights(f, &N);
    if (w && N>0) {
        bool all1=true;
        for (uint64_t i=0; i<N; i++) {
            if (w[i]!=1.0) return false;
        }
        return true;
    }
    return false;
}

const double *QFFitAlgorithm::functorGetWeights(const QFFitAlgorithm::Functor *f, uint64_t* N)
{
    {
        const QFFitAlgorithm::FitFunctionFunctor* ff=dynamic_cast<const QFFitAlgorithm::FitFunctionFunctor*>(f);
        if (ff ) {
            if (N) *N=ff->getDataPoints();
            return ff->getDataWeight();
        }
    }
    {
        const QFFitAlgorithm::FitFunctionFunctor2D* ff=dynamic_cast<const QFFitAlgorithm::FitFunctionFunctor2D*>(f);
        if (ff ) {
            if (N) *N=ff->getDataPoints();
            return ff->getDataWeight();
        }
    }
    {
        const QFFitAlgorithm::FitFunctionFunctor3D* ff=dynamic_cast<const QFFitAlgorithm::FitFunctionFunctor3D*>(f);
        if (ff ) {
            if (N) *N=ff->getDataPoints();
            return ff->getDataWeight();
        }
    }

    return NULL;
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

QFFitAlgorithm::FitResult QFFitAlgorithm::fit(double *paramsOut, double *paramErrorsOut, const double *dataX, const double *dataY, const double *dataWeight, uint64_t N, QFFitFunction *model, const double *initialParams, const bool *fixParams, const double *paramsMin, const double *paramsMax, bool fitLogY, double* chi2_out, QVector<double> *cov_matrix) {
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
    fm.setFitLogY(fitLogY);

    fm.setBootstrappingEnabled(false);

    //qDebug()<<"fm.get_paramcount()="<<fm.get_paramcount();

    double* tparamsMin=fm.createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm.createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tinitialParams=fm.createMappedArrayForFunctor(initialParams);

    double chi2=0.0;
    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, &fm, tparamsMin, tparamsMax);
    chi2=fm.calculateChi2(tparamsOut);


    if (m_errorEstimateModeFit==fpeBootstrapping) {
        //qDebug()<<"BS: "<<m_bootstrapFraction<<m_bootstrapDistortion<<m_bootstrapRepeats;
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

            double localChi2=0;
            QFFitAlgorithm::FitResult resultbs=intFit(bsParams, bsErrors, bsInitParams, &fm, tparamsMin, tparamsMax);
            //if (resultbs.params.contains("error_sum")) localChi2=resultbs.params["error_sum"].getAsDouble();
            localChi2=fm.calculateChi2(tparamsOut);
            if (chi2>0 && localChi2>0 && localChi2<chi2) {
                chi2=localChi2;
                //result=resultbs;
            }
            long its=resultbs.params.contains("iterations")?resultbs.params["iterations"].getAsDouble():0;
            long fevals=resultbs.params.contains("function_evals")?resultbs.params["function_evals"].getAsDouble():0;
            long jevals=resultbs.params.contains("jacobian_evals")?resultbs.params["jacobian_evals"].getAsDouble():0;
            long devals=resultbs.params.contains("derivative_evals")?resultbs.params["derivative_evals"].getAsDouble():0;
            if (its>0 || result.params.contains("iterations")) result.addInteger("iterations", its+result.params.value("iterations").getAsDouble());
            if (fevals>0 || result.params.contains("function_evals")) result.addInteger("function_evals", fevals+result.params.value("function_evals").getAsDouble());
            if (jevals>0 || result.params.contains("jacobian_evals")) result.addInteger("jacobian_evals", jevals+result.params.value("jacobian_evals").getAsDouble());
            if (devals>0 || result.params.contains("derivative_evals")) result.addInteger("derivative_evals", devals+result.params.value("derivative_evals").getAsDouble());
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
                tparamErrorsOut[j]=(bsParamSqrSum[j]-bsParamSum[j]*bsParamSum[j]/N)/(N-1.0);
                if (tparamErrorsOut[j]<0.0) {
                    tparamErrorsOut[j]=0;
                } else {
                    tparamErrorsOut[j]=sqrt( tparamErrorsOut[j]);
                }
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
        if (paramErrorsOut) paramErrorsOut[i]=0;
    }
    fm.mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    if (paramErrorsOut) fm.mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    if (cov_matrix) {
        QVector<double> COV(fm.get_paramcount()*fm.get_paramcount());
        if (result.params.contains("covariance_matrix") && result.params["covariance_matrix"].getVectorMatrixItems()==COV.size()) {
            COV=result.params["covariance_matrix"].getAsDoubleVector();
            //qDebug()<<"fit(): read cov matrix: "<<COV;
        } else {
            //if ( result.addNumberMatrix("covariance_matrix", COV.data(), model->get_paramcount(), model->get_paramcount());)
            QVector<double> J(fm.get_evalout()*fm.get_paramcount());
            fm.evaluateJacobian(J.data(), paramsOut);
            if (QFFitAlgorithm::functorHasWeights(&fm)) statisticsGetFitProblemCovMatrix(COV.data(), J.data(), fm.get_evalout(), fm.get_paramcount());
            else statisticsGetFitProblemVarCovMatrix(COV.data(), J.data(), fm.get_evalout(), fm.get_paramcount(), chi2);


            result.addNumberMatrix("covariance_matrix", COV.data(), fm.get_paramcount(), fm.get_paramcount());
            //qDebug()<<"fit(): calc cov matrix: "<<COV;
        }

        cov_matrix->clear();
        (*cov_matrix)=COV;
    }
    if (chi2_out) {
        *chi2_out=chi2;
        result.addNumber("error_sum", chi2);
    }

    qfFree(tparamsMax);
    qfFree(tparamsMin);
    qfFree(tparamErrorsOut);
    qfFree(tparamsOut);
    qfFree(tinitialParams);

    if (ppparamsMin!=NULL) qfFree(ppparamsMin);
    if (ppparamsMax!=NULL) qfFree(ppparamsMax);
    if (ppparamsFix!=NULL) qfFree(ppparamsFix);
    if (dddataWeight!=NULL) qfFree(dddataWeight);
    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fit2D(double *paramsOut, double *paramErrorsOut, const double *dataX, const double *dataY, const double *dataF, const double *dataWeight, uint64_t N, QFFitFunction2D *model, const double *initialParams, const bool *fixParams, const double *paramsMin, const double *paramsMax, bool fitLogY)
{
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

    QFFitAlgorithm::FitQFFitFunction2DFunctor fm(model, initialParams, pparamsFix, dataX, dataY, dataF, ddataWeight, N);
    fm.setFitLogY(fitLogY);

    fm.setBootstrappingEnabled(false);

    //qDebug()<<"fm.get_paramcount()="<<fm.get_paramcount();

    double* tparamsMin=fm.createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm.createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tinitialParams=fm.createMappedArrayForFunctor(initialParams);

    double chi2=0.0;
    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, &fm, tparamsMin, tparamsMax);
    if (result.params.contains("error_sum")) chi2=result.params["error_sum"].getAsDouble();

    if (m_errorEstimateModeFit==fpeBootstrapping) {
        //qDebug()<<"BS: "<<m_bootstrapFraction<<m_bootstrapDistortion<<m_bootstrapRepeats;
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

            double localChi2=0;
            QFFitAlgorithm::FitResult resultbs=intFit(bsParams, bsErrors, bsInitParams, &fm, tparamsMin, tparamsMax);
            if (resultbs.params.contains("error_sum")) localChi2=resultbs.params["error_sum"].getAsDouble();
            if (chi2>0 && localChi2>0 && localChi2<chi2) {
                chi2=localChi2;
                //result=resultbs;
            }
            long its=resultbs.params.contains("iterations")?resultbs.params["iterations"].getAsDouble():0;
            long fevals=resultbs.params.contains("function_evals")?resultbs.params["function_evals"].getAsDouble():0;
            long jevals=resultbs.params.contains("jacobian_evals")?resultbs.params["jacobian_evals"].getAsDouble():0;
            long devals=resultbs.params.contains("derivative_evals")?resultbs.params["derivative_evals"].getAsDouble():0;
            if (its>0 || result.params.contains("iterations")) result.addInteger("iterations", its+result.params.value("iterations").getAsDouble());
            if (fevals>0 || result.params.contains("function_evals")) result.addInteger("function_evals", fevals+result.params.value("function_evals").getAsDouble());
            if (jevals>0 || result.params.contains("jacobian_evals")) result.addInteger("jacobian_evals", jevals+result.params.value("jacobian_evals").getAsDouble());
            if (devals>0 || result.params.contains("derivative_evals")) result.addInteger("derivative_evals", devals+result.params.value("derivative_evals").getAsDouble());
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
                tparamErrorsOut[j]=(bsParamSqrSum[j]-bsParamSum[j]*bsParamSum[j]/N)/(N-1.0);
                if (tparamErrorsOut[j]<0.0) {
                    tparamErrorsOut[j]=0;
                } else {
                    tparamErrorsOut[j]=sqrt( tparamErrorsOut[j]);
                }
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
        if (paramErrorsOut) paramErrorsOut[i]=0;
    }
    fm.mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    if (paramErrorsOut) fm.mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    qfFree(tparamsMax);
    qfFree(tparamsMin);
    qfFree(tparamErrorsOut);
    qfFree(tparamsOut);
    qfFree(tinitialParams);

    if (ppparamsMin!=NULL) qfFree(ppparamsMin);
    if (ppparamsMax!=NULL) qfFree(ppparamsMax);
    if (ppparamsFix!=NULL) qfFree(ppparamsFix);
    if (dddataWeight!=NULL) qfFree(dddataWeight);
    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fit3D(double *paramsOut, double *paramErrorsOut, const double *dataX, const double *dataY, const double *dataZ, const double *dataF, const double *dataWeight, uint64_t N, QFFitFunction3D *model, const double *initialParams, const bool *fixParams, const double *paramsMin, const double *paramsMax, bool fitLogY)
{
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

    QFFitAlgorithm::FitQFFitFunction3DFunctor fm(model, initialParams, pparamsFix, dataX, dataY, dataZ, dataF, ddataWeight, N);
    fm.setFitLogY(fitLogY);

    fm.setBootstrappingEnabled(false);

    //qDebug()<<"fm.get_paramcount()="<<fm.get_paramcount();

    double* tparamsMin=fm.createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm.createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)qfCalloc(fm.get_paramcount(), sizeof(double));
    double* tinitialParams=fm.createMappedArrayForFunctor(initialParams);

    double chi2=0.0;
    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, &fm, tparamsMin, tparamsMax);
    if (result.params.contains("error_sum")) chi2=result.params["error_sum"].getAsDouble();

    if (m_errorEstimateModeFit==fpeBootstrapping) {
        //qDebug()<<"BS: "<<m_bootstrapFraction<<m_bootstrapDistortion<<m_bootstrapRepeats;
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

            double localChi2=0;
            QFFitAlgorithm::FitResult resultbs=intFit(bsParams, bsErrors, bsInitParams, &fm, tparamsMin, tparamsMax);
            if (resultbs.params.contains("error_sum")) localChi2=resultbs.params["error_sum"].getAsDouble();
            if (chi2>0 && localChi2>0 && localChi2<chi2) {
                chi2=localChi2;
                //result=resultbs;
            }
            long its=resultbs.params.contains("iterations")?resultbs.params["iterations"].getAsDouble():0;
            long fevals=resultbs.params.contains("function_evals")?resultbs.params["function_evals"].getAsDouble():0;
            long jevals=resultbs.params.contains("jacobian_evals")?resultbs.params["jacobian_evals"].getAsDouble():0;
            long devals=resultbs.params.contains("derivative_evals")?resultbs.params["derivative_evals"].getAsDouble():0;
            if (its>0 || result.params.contains("iterations")) result.addInteger("iterations", its+result.params.value("iterations").getAsDouble());
            if (fevals>0 || result.params.contains("function_evals")) result.addInteger("function_evals", fevals+result.params.value("function_evals").getAsDouble());
            if (jevals>0 || result.params.contains("jacobian_evals")) result.addInteger("jacobian_evals", jevals+result.params.value("jacobian_evals").getAsDouble());
            if (devals>0 || result.params.contains("derivative_evals")) result.addInteger("derivative_evals", devals+result.params.value("derivative_evals").getAsDouble());
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
                tparamErrorsOut[j]=(bsParamSqrSum[j]-bsParamSum[j]*bsParamSum[j]/N)/(N-1.0);
                if (tparamErrorsOut[j]<0.0) {
                    tparamErrorsOut[j]=0;
                } else {
                    tparamErrorsOut[j]=sqrt( tparamErrorsOut[j]);
                }
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
        if (paramErrorsOut) paramErrorsOut[i]=0;
    }
    fm.mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    if (paramErrorsOut) fm.mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    qfFree(tparamsMax);
    qfFree(tparamsMin);
    qfFree(tparamErrorsOut);
    qfFree(tparamsOut);
    qfFree(tinitialParams);

    if (ppparamsMin!=NULL) qfFree(ppparamsMin);
    if (ppparamsMax!=NULL) qfFree(ppparamsMax);
    if (ppparamsFix!=NULL) qfFree(ppparamsFix);
    if (dddataWeight!=NULL) qfFree(dddataWeight);
    return result;
}



QFFitAlgorithm::FitResult QFFitAlgorithm::lsqMinimize(double* paramsOut, double* paramErrorsOut, Functor* model, const double* initialParams, const bool* fixParams,  const double* paramsMin,  const double* paramsMax) {
    QFFitAlgorithm::FitResult result;

    const double* pparamsMin=paramsMin;
    double* ppparamsMin=NULL;
    const double* pparamsMax=paramsMax;
    double* ppparamsMax=NULL;
    const bool* pparamsFix=fixParams;
    bool* ppparamsFix=NULL;
    //std::cout<<"QFFitAlgorithm::lsqMinimize  1\n"; std::cout.flush();
    if (paramsMin==NULL) {
        ppparamsMin=(double*)qfCalloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsMin[i]=-DBL_MAX;
        }
        pparamsMin=ppparamsMin;
    }
    //std::cout<<"QFFitAlgorithm::lsqMinimize  2\n"; std::cout.flush();
    if (paramsMax==NULL) {
        ppparamsMax=(double*)qfCalloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsMax[i]=DBL_MAX;
        }
        pparamsMax=ppparamsMax;
    }
    //std::cout<<"QFFitAlgorithm::lsqMinimize  3\n"; std::cout.flush();


    if (fixParams==NULL) {
        ppparamsFix=(bool*)qfCalloc(model->get_paramcount(), sizeof(bool));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsFix[i]=false;
        }
        pparamsFix=ppparamsFix;
    }
    //std::cout<<"QFFitAlgorithm::lsqMinimize  4\n"; std::cout.flush();



    QFFitAlgorithm::FitQFOptimizeFunctionFunctor* fm;
    QFFitAlgorithm::FunctorBootstrapInterface* fmbs=dynamic_cast<QFFitAlgorithm::FunctorBootstrapInterface*>(model);
    if (fmbs) {
        fm=new QFFitAlgorithm::FitQFOptimizeFunctionBootstrapFunctor(model, initialParams, pparamsFix);
    } else {
        fm=new QFFitAlgorithm::FitQFOptimizeFunctionFunctor(model, initialParams, pparamsFix);
    }
    fmbs=dynamic_cast<QFFitAlgorithm::FunctorBootstrapInterface*>(fm);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  5\n"; std::cout.flush();

    double* tparamsMin=fm->createMappedArrayForFunctor(pparamsMin);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  6\n"; std::cout.flush();

    double* tparamsMax=fm->createMappedArrayForFunctor(pparamsMax);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  7\n"; std::cout.flush();
    double* tparamsOut=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
    //std::cout<<"QFFitAlgorithm::lsqMinimize  8\n"; std::cout.flush();
    double* tparamErrorsOut=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
    //std::cout<<"QFFitAlgorithm::lsqMinimize  9\n"; std::cout.flush();
    double* tinitialParams=fm->createMappedArrayForFunctor(initialParams);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  10\n"; std::cout.flush();

    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, fm, tparamsMin, tparamsMax);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  11\n"; std::cout.flush();


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
        if (paramErrorsOut) paramErrorsOut[i]=0;
    }
    //std::cout<<"QFFitAlgorithm::lsqMinimize  12\n"; std::cout.flush();

    fm->mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  13\n"; std::cout.flush();
    if (paramErrorsOut) fm->mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  14\n"; std::cout.flush();

    qfFree(tparamsMax);
    qfFree(tparamsMin);
    qfFree(tparamErrorsOut);
    qfFree(tparamsOut);
    qfFree(tinitialParams);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  15\n"; std::cout.flush();


    if (ppparamsMin) qfFree(ppparamsMin);
    if (ppparamsMax) qfFree(ppparamsMax);
    if (ppparamsFix) qfFree(ppparamsFix);
    //std::cout<<"QFFitAlgorithm::lsqMinimize  16\n"; std::cout.flush();

    delete fm;
    //std::cout<<"QFFitAlgorithm::lsqMinimize  17\n"; std::cout.flush();

    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fmin(double *paramsOut, QFFitAlgorithm::FunctorForFMin *model, const double *initialParams, const bool *fixParams, const double *paramsMin, const double *paramsMax)
{
    QFFitAlgorithm::FitResult result;
    if (!get_implementsMinimize()) {
        result.fitOK=false;
        result.message=result.messageSimple=QObject::tr("fmin(): this fit algorithm does not allow general objective function minimization");
        return result;
    }

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



    QFFitAlgorithm::FunctorForFMinFixAdaptor* fm;
    fm=new QFFitAlgorithm::FunctorForFMinFixAdaptor(model, initialParams, pparamsFix);

    double* tparamsMin=fm->createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm->createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)qfCalloc(fm->get_paramcount(), sizeof(double));
    double* tinitialParams=fm->createMappedArrayForFunctor(initialParams);

    result=intMinimize(tparamsOut, tinitialParams, fm, tparamsMin, tparamsMax);

    for (int i=0; i<model->get_paramcount(); i++) {
        paramsOut[i]=initialParams[i];
    }
    fm->mapArrayFromFunctorToModel(paramsOut, tparamsOut);

    qfFree(tparamsMax);
    qfFree(tparamsMin);
    qfFree(tparamsOut);
    qfFree(tinitialParams);

    if (ppparamsMin) qfFree(ppparamsMin);
    if (ppparamsMax) qfFree(ppparamsMax);
    if (ppparamsFix) qfFree(ppparamsFix);
    delete fm;


    return result;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fmin(double *paramsOut, QFFitAlgorithm::FMinFunctionP *model, int Nparams, const double *initialParams, QFFitAlgorithm::FMinFunctionDerivP *dFdp, const bool *fixParams, const double *paramsMin, const double *paramsMax, void *userData)
{
    QFFitAlgorithm::FunctorForFMinFromCFunc* m=new FunctorForFMinFromCFunc(Nparams, model, dFdp);
    m->setUserData(userData);
    QFFitAlgorithm::FitResult res=fmin(paramsOut, m, initialParams, fixParams, paramsMin, paramsMax);
    delete m;
    return res;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::fmin(double *paramsOut, QFFitAlgorithm::FMinFunctionNoUserDataP *model, int Nparams, const double *initialParams, QFFitAlgorithm::FMinFunctionNoUserDataDerivP *dFdp, const bool *fixParams, const double *paramsMin, const double *paramsMax, void *userData)
{
    QFFitAlgorithm::FunctorForFMinFromCFunc* m=new FunctorForFMinFromCFunc(Nparams, model, dFdp);
    m->setUserData(userData);
    QFFitAlgorithm::FitResult res=fmin(paramsOut, m, initialParams, fixParams, paramsMin, paramsMax);
    delete m;
    return res;
}

bool QFFitAlgorithm::displayConfig() {
    QMessageBox::information(NULL, name(), QObject::tr("No configuration dialog is supplied for this optimization algorithm!"));
    return false;
}

QFFitAlgorithm::FitResult QFFitAlgorithm::intMinimize(double */*paramsOut*/, const double */*initialParams*/, QFFitAlgorithm::FunctorForFMin */*model*/, const double */*paramsMin*/, const double */*paramsMax*/)
{
    QFFitAlgorithm::FitResult result;
    result.fitOK=false;
    result.message=result.messageSimple=QObject::tr("intMinimize(): this fit algorithm does not allow general objective function minimization");
    return result;
}

bool QFFitAlgorithm::isDeprecated() const
{
    return false;
}

















QFFitAlgorithm::FitQFOptimizeFunctionFunctor::FitQFOptimizeFunctionFunctor(Functor *model, const double *currentParams, const bool *fixParams):
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

            printf("   mapping m=%2d -> f=%2d\n", i, pid);
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

double* QFFitAlgorithm::FitQFOptimizeFunctionFunctor::createMappedArrayForFunctor(const double* modelData) const {
    double* result=(double*)qfCalloc(m_paramCount, sizeof(double));

    for (register int i=0; i<m_paramCount; i++) {
        result[i]=modelData[modelFromFunctor[i]];
    }

    return result;
}

void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::mapArrayFromModelToFunctor(double* functorData, const double* modelData) const {
    for (register int i=0; i<m_paramCount; i++) {
        functorData[i]=modelData[modelFromFunctor[i]];
    }
}

void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::mapArrayFromFunctorToModel(double* modelData, const double* functorData) const {
    for (int i=0; i<m_paramCount; i++) {
        modelData[modelFromFunctor[i]]=functorData[i];
        //std::cout<<"FitQFOptimizeFunctionFunctor::mapArrayFromFunctorToModel: "<<i<<"modelData[modelFromFunctor["<<i<<"]="<<modelFromFunctor[i]<<"]=functorData[i]\n"; std::cout.flush();
    }
}


void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::evaluate(double* evalout, const double* params) const {
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);
    m_model->evaluate(evalout, mpt);
    qfFree(mpt);
}

void QFFitAlgorithm::FitQFOptimizeFunctionFunctor::evaluateJacobian(double* evalout, const double* params) const {
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitQFOptimizeFunctionFunctor::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);
    double* e_out=(double*)qfMalloc(m_model->get_evalout()*m_model->get_paramcount()*sizeof(double));
    m_model->evaluateJacobian(e_out, mpt);
    const int pcnt=get_paramcount();
    const int m_pcnt=m_model->get_paramcount();
    for (uint64_t i=0; i<m_model->get_evalout(); i++) {
        for (int p=0; p<pcnt; p++) {
            evalout[i*pcnt+p]=e_out[i*m_pcnt+modelFromFunctor[p]];
        }
    }
    qfFree(mpt);
    qfFree(e_out);
}

QFFitAlgorithm::FitFunctionFunctor::FitFunctionFunctor(const double *dataX, const double *dataF, const double *dataWeight, uint64_t M):
    QFFitAlgorithm::Functor(M)
{
    m_bootstrapEnabled=false;
    m_bootstrapFraction=0.7;
    m_dataX=i_dataX=dataX;
    m_dataF=i_dataF=dataF;
    m_dataWeight=i_dataWeight=dataWeight;
    m_M=i_M=M;
    m_evalout=M;
}

QFFitAlgorithm::FitFunctionFunctor::~FitFunctionFunctor()
{
}

bool QFFitAlgorithm::FitFunctionFunctor::isWeightedLSQ() const
{
    return (m_dataWeight!=NULL);
}

bool QFFitAlgorithm::FitFunctionFunctor::areAllWeightsOne() const
{
    if (m_dataWeight) {

        for (uint64_t i=0; i<m_M; i++) {
            if(m_dataWeight[i]!=1.0) return false;
        }
        return true;
    }
    return false;
}

const double * QFFitAlgorithm::FitFunctionFunctor::getDataX() const
{
    return m_dataX;
}

const double * QFFitAlgorithm::FitFunctionFunctor::getDataF() const
{
    return m_dataF;
}

const double * QFFitAlgorithm::FitFunctionFunctor::getDataWeight() const
{
    return m_dataWeight;
}

QVector<double> QFFitAlgorithm::FitFunctionFunctor::getDataXV() const
{
    QVector<double> d;
    if (!m_dataX) return d;
    for (uint64_t i=0; i<m_M; i++) {
        d.append(m_dataX[i]);
    }
    return d;
}

QVector<double> QFFitAlgorithm::FitFunctionFunctor::getDataFV() const
{
    QVector<double> d;
    if (!m_dataF) return d;
    for (uint64_t i=0; i<m_M; i++) {
        d.append(m_dataF[i]);
    }
    return d;
}

QVector<double> QFFitAlgorithm::FitFunctionFunctor::getDataWeightV() const
{
    QVector<double> d;
    if (!m_dataWeight) return d;
    for (uint64_t i=0; i<m_M; i++) {
        d.append(m_dataWeight[i]);
    }
    return d;
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

void QFFitAlgorithm::FitFunctionFunctor::setDataF(const double *data)
{
    i_dataF=data;
    if (m_bootstrapEnabled) m_dataF=data;
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
        while (bootstrapIDs.size()>mBS && bootstrapIDs.size()>get_paramcount()+2) bootstrapIDs.removeLast();
        qSort(bootstrapIDs);
        //qDebug()<<"mBS="<<mBS<<"  IDS="<<bootstrapIDs;
        //qDebug()<<m_bootstrapFraction<<get_paramcount()<<bootstrapIDs.size()<<"/"<<i_M;
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
        if (i_dataF) bootstrappedF.resize(mBS);
        if (i_dataWeight) bootstrappedW.resize(mBS);
        m_dataX=NULL;
        m_dataF=NULL;
        m_dataWeight=NULL;
        if (i_dataX) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedX[i]=i_dataX[bootstrapIDs[i]];
            }
            m_dataX=bootstrappedX.constData();
        }
        if (i_dataF) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedF[i]=i_dataF[bootstrapIDs[i]];
            }
            m_dataF=bootstrappedF.constData();
        }
        if (i_dataWeight) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedW[i]=i_dataWeight[bootstrapIDs[i]];
            }
            m_dataWeight=bootstrappedW.constData();
        }
        m_M=qMin(bootstrappedX.size(), bootstrappedF.size());
    } else {
        bootstrappedX.clear();
        bootstrappedF.clear();
        bootstrappedW.clear();
        m_dataX=i_dataX;
        m_dataF=i_dataF;
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

void QFFitAlgorithm::FitFunctionFunctor::evaluateJacobianNum(double *evalout, const double *params, double h) const
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitFunctionFunctor::evaluateJacobianNum(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    Functor::evaluateJacobianNum(evalout, params, h);
    if (m_dataWeight) {
        for (uint64_t i=0; i<m_M; i++) {
            if (m_dataWeight[i]>0){
                for (int j=0; j<get_paramcount(); j++) {
                    evalout[i*get_paramcount()+j] = evalout[i*get_paramcount()+j]/m_dataWeight[i];
                }
            }
        }
    }
}

double QFFitAlgorithm::FitFunctionFunctor::calculateChi2(const double* params) const
{
    QVector<double> eout(get_evalout());
    evaluate(eout.data(), params);
    double chi2=0;
    for (uint64_t i=0; i<get_evalout(); i++) {
        chi2 += qfSqr(eout[i]);
    }
    return chi2;
}

QFFitAlgorithm::IRLSFunctorAdaptor::IRLSFunctorAdaptor( FitFunctionFunctor *functor, double irls_parameter):
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

void QFFitAlgorithm::IRLSFunctorAdaptor::evaluate(double *evalout, const double *params) const
{
    irls_functor->evaluate(evalout, params);
}

void QFFitAlgorithm::IRLSFunctorAdaptor::evaluateJacobian(double *evalout, const double *params) const
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::IRLSFunctorAdaptor::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
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
    return dynamic_cast< QFFitAlgorithm::FunctorBootstrapInterface*>(m_model);
}


QFFitAlgorithm::FitFunctionFunctor3D::FitFunctionFunctor3D(const double *dataX, const double *dataY, const double *dataZ, const double *dataF, const double *dataWeight, uint64_t M):
    QFFitAlgorithm::Functor(M)
{
    m_bootstrapEnabled=false;
    m_bootstrapFraction=0.7;
    m_dataX=i_dataX=dataX;
    m_dataY=i_dataY=dataY;
    m_dataZ=i_dataZ=dataZ;
    m_dataF=i_dataF=dataF;
    m_dataWeight=i_dataWeight=dataWeight;
    m_M=i_M=M;
    m_evalout=M;
}

QFFitAlgorithm::FitFunctionFunctor3D::~FitFunctionFunctor3D()
{

}

const double *QFFitAlgorithm::FitFunctionFunctor3D::getDataX() const
{
    return m_dataX;
}

const double *QFFitAlgorithm::FitFunctionFunctor3D::getDataY() const
{
    return m_dataY;
}

const double *QFFitAlgorithm::FitFunctionFunctor3D::getDataZ() const
{
    return m_dataZ;
}

const double *QFFitAlgorithm::FitFunctionFunctor3D::getDataF() const
{
    return m_dataF;
}

const double *QFFitAlgorithm::FitFunctionFunctor3D::getDataWeight() const
{
    return m_dataWeight;
}

uint64_t QFFitAlgorithm::FitFunctionFunctor3D::getDataPoints() const
{
    return m_M;
}

bool QFFitAlgorithm::FitFunctionFunctor3D::isWeightedLSQ() const
{
    return (m_dataWeight!=NULL);
}

bool QFFitAlgorithm::FitFunctionFunctor3D::areAllWeightsOne() const
{
    if (m_dataWeight) {

        for (uint64_t i=0; i<m_M; i++) {
            if(m_dataWeight[i]!=1.0) return false;
        }
        return true;
    }
    return false;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setDataX(const double *data)
{
    i_dataX=data;
    if (m_bootstrapEnabled) m_dataX=data;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setDataY(const double *data)
{
    i_dataY=data;
    if (m_bootstrapEnabled) m_dataY=data;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setDataZ(const double *data)
{
    i_dataZ=data;
    if (m_bootstrapEnabled) m_dataZ=data;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setDataF(const double *data)
{
    i_dataF=data;
    if (m_bootstrapEnabled) m_dataF=data;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setDataWeight(const double *data)
{
    i_dataWeight=data;
    if (m_bootstrapEnabled) m_dataWeight=data;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setDataPoints(uint64_t data)
{
    i_M=data;
    if (m_bootstrapEnabled) m_M=m_evalout=data;
}

void QFFitAlgorithm::FitFunctionFunctor3D::prepareBootstrapSelection()
{
    int64_t mBS=qRound64(double(i_M)*m_bootstrapFraction);
    if (mBS<get_paramcount()+1) mBS=get_paramcount()+1;

    if (m_bootstrapEnabled) {
        bootstrapIDs.clear();
        for (uint64_t i=0; i<i_M; i++) bootstrapIDs.append(i);
        qfShuffleInplace(bootstrapIDs);
        while (bootstrapIDs.size()>mBS && bootstrapIDs.size()>get_paramcount()+2) bootstrapIDs.removeLast();
        qSort(bootstrapIDs);
        //qDebug()<<"mBS="<<mBS<<"  IDS="<<bootstrapIDs;
        //qDebug()<<m_bootstrapFraction<<get_paramcount()<<bootstrapIDs.size()<<"/"<<i_M;
    } else {
        bootstrapIDs.clear();
    }
    reapplyBootstrapselection();
}

void QFFitAlgorithm::FitFunctionFunctor3D::reapplyBootstrapselection()
{
    uint64_t mBS=bootstrapIDs.size();

    if (m_bootstrapEnabled && mBS>0) {
        if (i_dataX) bootstrappedX.resize(mBS);
        if (i_dataY) bootstrappedY.resize(mBS);
        if (i_dataZ) bootstrappedZ.resize(mBS);
        if (i_dataF) bootstrappedF.resize(mBS);
        if (i_dataWeight) bootstrappedW.resize(mBS);
        m_dataX=NULL;
        m_dataY=NULL;
        m_dataZ=NULL;
        m_dataF=NULL;
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
        if (i_dataZ) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedZ[i]=i_dataZ[bootstrapIDs[i]];
            }
            m_dataZ=bootstrappedZ.constData();
        }
        if (i_dataF) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedF[i]=i_dataF[bootstrapIDs[i]];
            }
            m_dataF=bootstrappedF.constData();
        }
        if (i_dataWeight) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedW[i]=i_dataWeight[bootstrapIDs[i]];
            }
            m_dataWeight=bootstrappedW.constData();
        }
        m_M=qMin(qMin(qMin(bootstrappedX.size(), bootstrappedY.size()), bootstrappedZ.size()), bootstrappedF.size());
    } else {
        bootstrappedX.clear();
        bootstrappedY.clear();
        bootstrappedW.clear();
        m_dataX=i_dataX;
        m_dataY=i_dataY;
        m_dataZ=i_dataZ;
        m_dataF=i_dataF;
        m_dataWeight=i_dataWeight;
        m_M=i_M;
    }
    m_evalout=m_M;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setBootstrappingEnabled(bool enabled, bool prepBootstrapping)
{
    m_bootstrapEnabled=enabled;
    if (enabled && prepBootstrapping) prepareBootstrapSelection();
}

bool QFFitAlgorithm::FitFunctionFunctor3D::getBootstrappingEnabled() const
{
    return m_bootstrapEnabled;
}

void QFFitAlgorithm::FitFunctionFunctor3D::setBootstrappingFraction(double fraction, bool prepBootstrapping)
{
    m_bootstrapFraction=qBound<double>(0.0, fraction, 1.0);
    if (m_bootstrapEnabled && prepBootstrapping) prepareBootstrapSelection();
}

double QFFitAlgorithm::FitFunctionFunctor3D::getBootstrappingFraction() const
{
    return m_bootstrapFraction;
}

void QFFitAlgorithm::FitFunctionFunctor3D::evaluateJacobianNum(double *evalout, const double *params, double h) const
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitFunctionFunctor3D::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    Functor::evaluateJacobianNum(evalout, params, h);
    if (m_dataWeight) {
        for (uint64_t i=0; i<m_M; i++) {
            if (m_dataWeight[i]>0){
                for (int j=0; j<get_paramcount(); j++) {
                    evalout[i*get_paramcount()+j] = evalout[i*get_paramcount()+j]/m_dataWeight[i];
                }
            }
        }
    }
}


QFFitAlgorithm::FitQFFitFunction3DFunctor::FitQFFitFunction3DFunctor(QFFitFunction3D *model, const double *currentParams, const bool *fixParams, const double *dataX, const double *dataY, const double *dataZ, const double *dataF, const double *dataWeight, uint64_t M):
    QFFitAlgorithm::FitFunctionFunctor3D(dataX, dataY, dataZ, dataF, dataWeight,M)
{
    m_model=model;
    fitLogY=false,
    m_N=model->paramCount();
    functorFromModel=NULL;
    modelFromFunctor=NULL;
    enableParameterTransforms=true;

    m_modelparamsFix=duplicateArray(fixParams, m_N);
    hasParameterTransforms=false;

    // now we calculate the mapping of the data
    m_paramCount=0;
    for (int i=0; i<m_N; i++) {
        QFFitFunction::ParameterDescription d=model->getDescription(i);
        if (!fixParams[i]) {
            if (d.fit && model->isParameterVisible(i, currentParams)) {
                paramTransforms<<d.parameterType;
                m_paramCount++;
                hasParameterTransforms=hasParameterTransforms||(d.parameterType!=QFFitFunction::StandardParameter);
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
}

QFFitAlgorithm::FitQFFitFunction3DFunctor::~FitQFFitFunction3DFunctor()
{
    qfFree(functorFromModel);
    qfFree(modelFromFunctor);
    qfFree(m_modelParams);
    qfFree(m_modelparamsFix);
}

double *QFFitAlgorithm::FitQFFitFunction3DFunctor::createMappedArrayForFunctor(const double *modelData) const
{
    double* result=(double*)qfCalloc(m_paramCount, sizeof(double));

    for (register int i=0; i<m_paramCount; i++) {
        result[i]=modelData[modelFromFunctor[i]];
    }

    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(result);
    return result;
}

void QFFitAlgorithm::FitQFFitFunction3DFunctor::mapArrayFromModelToFunctor(double *functorData, const double *modelData) const
{
    for (register int i=0; i<m_paramCount; i++) {
        functorData[i]=modelData[modelFromFunctor[i]];
    }
    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(functorData);
}

void QFFitAlgorithm::FitQFFitFunction3DFunctor::mapArrayFromFunctorToModel(double *modelData, const double *functorData) const
{
    for (register int i=0; i<m_paramCount; i++) {
        modelData[modelFromFunctor[i]]=functorData[i];
    }
    if (enableParameterTransforms&&hasParameterTransforms) backtransfromParameters(modelData);
}

void QFFitAlgorithm::FitQFFitFunction3DFunctor::evaluate(double *evalout, const double *params) const
{
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);


    register double v;
    register int ecount=get_evalout();
    QVector<double> evals(ecount, 0.0);
    for (int i=0; i<ecount; i++) {
        evals[i]=m_model->evaluate(m_dataX[i], m_dataY[i], m_dataZ[i], mpt);
    }

    if (!fitLogY) {
        for (register int i=0; i<ecount; i++) {
            v = ( m_dataF[i] -  evals[i] ) / m_dataWeight[i];
            if (!QFFloatIsOK(v)) {
                v=0;
            }
            evalout[i]=v;
        }
    } else {
        for (register int i=0; i<ecount; i++) {
            v = ( log(m_dataF[i]) -  log(evals[i]) ) /log(m_dataWeight[i]);
            if (!QFFloatIsOK(v)) {
                v=0;
            }
            evalout[i]=v;
        }
    }
    qfFree(mpt);
    //mapArrayFromModelToFunctor(params, m_modelParams);
}

void QFFitAlgorithm::FitQFFitFunction3DFunctor::evaluateJacobian(double *evalout, const double *params) const
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitFunctionFunctor3D::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    evaluateJacobianNum(evalout, params);
}

void QFFitAlgorithm::FitQFFitFunction3DFunctor::transfromParameters(double *params) const
{
    if (enableParameterTransforms&&hasParameterTransforms) {
        register int pcount=get_paramcount();
        for (int i=0; i<pcount; i++) {
            switch (paramTransforms[i]) {
                case QFFitFunction::LogParameter: params[i]=log(params[i]); break;
                case QFFitFunction::CustomTramsformParameter: params[i]=m_model->customTransform(i, params[i]); break;
                default: break;
            }
        }
    }
}

void QFFitAlgorithm::FitQFFitFunction3DFunctor::backtransfromParameters(double *params) const
{
    if (enableParameterTransforms&&hasParameterTransforms) {
        register int pcount=get_paramcount();
        for (int i=0; i<pcount; i++) {
            switch (paramTransforms[i]) {
                case QFFitFunction::LogParameter: params[i]=exp(params[i]); break;
                case QFFitFunction::CustomTramsformParameter: params[i]=m_model->customBackTransform(i, params[i]); break;
                default: break;
            }
        }
    }
}

QVector<double> QFFitAlgorithm::FitQFFitFunction3DFunctor::transfromParametersCopy(const double *params) const
{
    QVector<double> p=duplicateArrayV(params, get_paramcount());
    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(p.data());
    return p;
}

QVector<double> QFFitAlgorithm::FitQFFitFunction3DFunctor::backtransfromParametersCopy(const double *params) const
{
    QVector<double> p=duplicateArrayV(params, get_paramcount());
    if (enableParameterTransforms&&hasParameterTransforms) backtransfromParameters(p.data());
    return p;
}



















QFFitAlgorithm::FitFunctionFunctor2D::FitFunctionFunctor2D(const double *dataX, const double *dataY, const double *dataF, const double *dataWeight, uint64_t M):
    QFFitAlgorithm::Functor(M)
{
    m_bootstrapEnabled=false;
    m_bootstrapFraction=0.7;
    m_dataX=i_dataX=dataX;
    m_dataY=i_dataY=dataY;
    m_dataF=i_dataF=dataF;
    m_dataWeight=i_dataWeight=dataWeight;
    m_M=i_M=M;
    m_evalout=M;
}

QFFitAlgorithm::FitFunctionFunctor2D::~FitFunctionFunctor2D()
{

}

bool QFFitAlgorithm::FitFunctionFunctor2D::isWeightedLSQ() const
{
    return (m_dataWeight!=NULL);
}

bool QFFitAlgorithm::FitFunctionFunctor2D::areAllWeightsOne() const
{
    if (m_dataWeight) {

        for (uint64_t i=0; i<m_M; i++) {
            if(m_dataWeight[i]!=1.0) return false;
        }
        return true;
    }
    return false;
}

const double *QFFitAlgorithm::FitFunctionFunctor2D::getDataX() const
{
    return m_dataX;
}

const double *QFFitAlgorithm::FitFunctionFunctor2D::getDataY() const
{
    return m_dataY;
}

const double *QFFitAlgorithm::FitFunctionFunctor2D::getDataF() const
{
    return m_dataF;
}

const double *QFFitAlgorithm::FitFunctionFunctor2D::getDataWeight() const
{
    return m_dataWeight;
}

uint64_t QFFitAlgorithm::FitFunctionFunctor2D::getDataPoints() const
{
    return m_M;
}

void QFFitAlgorithm::FitFunctionFunctor2D::setDataX(const double *data)
{
    i_dataX=data;
    if (m_bootstrapEnabled) m_dataX=data;
}

void QFFitAlgorithm::FitFunctionFunctor2D::setDataY(const double *data)
{
    i_dataY=data;
    if (m_bootstrapEnabled) m_dataY=data;
}

void QFFitAlgorithm::FitFunctionFunctor2D::setDataF(const double *data)
{
    i_dataF=data;
    if (m_bootstrapEnabled) m_dataF=data;
}

void QFFitAlgorithm::FitFunctionFunctor2D::setDataWeight(const double *data)
{
    i_dataWeight=data;
    if (m_bootstrapEnabled) m_dataWeight=data;
}

void QFFitAlgorithm::FitFunctionFunctor2D::setDataPoints(uint64_t data)
{
    i_M=data;
    if (m_bootstrapEnabled) m_M=m_evalout=data;
}

void QFFitAlgorithm::FitFunctionFunctor2D::prepareBootstrapSelection()
{
    int64_t mBS=qRound64(double(i_M)*m_bootstrapFraction);
    if (mBS<get_paramcount()+1) mBS=get_paramcount()+1;

    if (m_bootstrapEnabled) {
        bootstrapIDs.clear();
        for (uint64_t i=0; i<i_M; i++) bootstrapIDs.append(i);
        qfShuffleInplace(bootstrapIDs);
        while (bootstrapIDs.size()>mBS && bootstrapIDs.size()>get_paramcount()+2) bootstrapIDs.removeLast();
        qSort(bootstrapIDs);
        //qDebug()<<"mBS="<<mBS<<"  IDS="<<bootstrapIDs;
        //qDebug()<<m_bootstrapFraction<<get_paramcount()<<bootstrapIDs.size()<<"/"<<i_M;
    } else {
        bootstrapIDs.clear();
    }
    reapplyBootstrapselection();
}

void QFFitAlgorithm::FitFunctionFunctor2D::reapplyBootstrapselection()
{
    uint64_t mBS=bootstrapIDs.size();

    if (m_bootstrapEnabled && mBS>0) {
        if (i_dataX) bootstrappedX.resize(mBS);
        if (i_dataY) bootstrappedY.resize(mBS);
        if (i_dataF) bootstrappedF.resize(mBS);
        if (i_dataWeight) bootstrappedW.resize(mBS);
        m_dataX=NULL;
        m_dataY=NULL;
        m_dataF=NULL;
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
        if (i_dataF) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedF[i]=i_dataF[bootstrapIDs[i]];
            }
            m_dataF=bootstrappedF.constData();
        }
        if (i_dataWeight) {
            for (uint64_t i=0; i<mBS; i++) {
                bootstrappedW[i]=i_dataWeight[bootstrapIDs[i]];
            }
            m_dataWeight=bootstrappedW.constData();
        }
        m_M=qMin(qMin(bootstrappedX.size(), bootstrappedY.size()), bootstrappedF.size());
    } else {
        bootstrappedX.clear();
        bootstrappedY.clear();
        bootstrappedW.clear();
        m_dataX=i_dataX;
        m_dataY=i_dataY;
        m_dataF=i_dataF;
        m_dataWeight=i_dataWeight;
        m_M=i_M;
    }
    m_evalout=m_M;
}

void QFFitAlgorithm::FitFunctionFunctor2D::setBootstrappingEnabled(bool enabled, bool prepBootstrapping)
{
    m_bootstrapEnabled=enabled;
    if (enabled && prepBootstrapping) prepareBootstrapSelection();
}

bool QFFitAlgorithm::FitFunctionFunctor2D::getBootstrappingEnabled() const
{
    return m_bootstrapEnabled;
}

void QFFitAlgorithm::FitFunctionFunctor2D::setBootstrappingFraction(double fraction, bool prepBootstrapping)
{
    m_bootstrapFraction=qBound<double>(0.0, fraction, 1.0);
    if (m_bootstrapEnabled && prepBootstrapping) prepareBootstrapSelection();
}

double QFFitAlgorithm::FitFunctionFunctor2D::getBootstrappingFraction() const
{
    return m_bootstrapFraction;
}

void QFFitAlgorithm::FitFunctionFunctor2D::evaluateJacobianNum(double *evalout, const double *params, double h) const
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitFunctionFunctor2D::evaluateJacobianNum(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    Functor::evaluateJacobianNum(evalout, params, h);
    if (m_dataWeight) {
        for (uint64_t i=0; i<m_M; i++) {
            if (m_dataWeight[i]>0){
                for (int j=0; j<get_paramcount(); j++) {
                    evalout[i*get_paramcount()+j] = evalout[i*get_paramcount()+j]/m_dataWeight[i];
                }
            }
        }
    }
}


QFFitAlgorithm::FitQFFitFunction2DFunctor::FitQFFitFunction2DFunctor(QFFitFunction2D *model, const double *currentParams, const bool *fixParams, const double *dataX, const double *dataY, const double *dataF, const double *dataWeight, uint64_t M):
    QFFitAlgorithm::FitFunctionFunctor2D(dataX, dataY, dataF, dataWeight,M)
{
    m_model=model;
    fitLogY=false,
    m_N=model->paramCount();
    functorFromModel=NULL;
    modelFromFunctor=NULL;
    enableParameterTransforms=true;

    m_modelparamsFix=duplicateArray(fixParams, m_N);
    hasParameterTransforms=false;

    // now we calculate the mapping of the data
    m_paramCount=0;
    for (int i=0; i<m_N; i++) {
        QFFitFunction::ParameterDescription d=model->getDescription(i);
        if (!fixParams[i]) {
            if (d.fit && model->isParameterVisible(i, currentParams)) {
                paramTransforms<<d.parameterType;
                m_paramCount++;
                hasParameterTransforms=hasParameterTransforms||(d.parameterType!=QFFitFunction::StandardParameter);
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
}

QFFitAlgorithm::FitQFFitFunction2DFunctor::~FitQFFitFunction2DFunctor()
{
    qfFree(functorFromModel);
    qfFree(modelFromFunctor);
    qfFree(m_modelParams);
    qfFree(m_modelparamsFix);
}

double *QFFitAlgorithm::FitQFFitFunction2DFunctor::createMappedArrayForFunctor(const double *modelData) const
{
    double* result=(double*)qfCalloc(m_paramCount, sizeof(double));

    for (register int i=0; i<m_paramCount; i++) {
        result[i]=modelData[modelFromFunctor[i]];
    }

    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(result);
    return result;
}

void QFFitAlgorithm::FitQFFitFunction2DFunctor::mapArrayFromModelToFunctor(double *functorData, const double *modelData) const
{
    for (register int i=0; i<m_paramCount; i++) {
        functorData[i]=modelData[modelFromFunctor[i]];
    }
    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(functorData);
}

void QFFitAlgorithm::FitQFFitFunction2DFunctor::mapArrayFromFunctorToModel(double *modelData, const double *functorData) const
{
    for (register int i=0; i<m_paramCount; i++) {
        modelData[modelFromFunctor[i]]=functorData[i];
    }
    if (enableParameterTransforms&&hasParameterTransforms) backtransfromParameters(modelData);
}

void QFFitAlgorithm::FitQFFitFunction2DFunctor::evaluate(double *evalout, const double *params) const
{
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);


    register double v;
    register int ecount=get_evalout();
    QVector<double> evals(ecount, 0.0);
    for (int i=0; i<ecount; i++) {
        evals[i]=m_model->evaluate(m_dataX[i], m_dataY[i], mpt);
    }

    if (!fitLogY) {
        for (register int i=0; i<ecount; i++) {
            v = ( m_dataF[i] -  evals[i] ) / m_dataWeight[i];
            if (!QFFloatIsOK(v)) {
                v=0;
            }
            evalout[i]=v;
        }
    } else {
        for (register int i=0; i<ecount; i++) {
            v = ( log(m_dataF[i]) -  log(evals[i]) ) /log(m_dataWeight[i]);
            if (!QFFloatIsOK(v)) {
                v=0;
            }
            evalout[i]=v;
        }
    }
    //mapArrayFromModelToFunctor(params, m_modelParams);
    qfFree(mpt);
}

void QFFitAlgorithm::FitQFFitFunction2DFunctor::evaluateJacobian(double *evalout, const double *params) const
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FitFunctionFunctor2D::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<get_evalout()<<", pcnt="<<get_paramcount()<<")";
#endif
    evaluateJacobianNum(evalout, params);
}

void QFFitAlgorithm::FitQFFitFunction2DFunctor::transfromParameters(double *params) const
{
    if (enableParameterTransforms&&hasParameterTransforms) {
        register int pcount=get_paramcount();
        for (int i=0; i<pcount; i++) {
            switch (paramTransforms[i]) {
                case QFFitFunction::LogParameter: params[i]=log(params[i]); break;
                case QFFitFunction::CustomTramsformParameter: params[i]=m_model->customTransform(i, params[i]); break;
                default: break;
            }
        }
    }
}

void QFFitAlgorithm::FitQFFitFunction2DFunctor::backtransfromParameters(double *params) const
{
    if (enableParameterTransforms&&hasParameterTransforms) {
        register int pcount=get_paramcount();
        for (int i=0; i<pcount; i++) {
            switch (paramTransforms[i]) {
                case QFFitFunction::LogParameter: params[i]=exp(params[i]); break;
                case QFFitFunction::CustomTramsformParameter: params[i]=m_model->customBackTransform(i, params[i]); break;
                default: break;
            }
        }
    }
}

QVector<double> QFFitAlgorithm::FitQFFitFunction2DFunctor::transfromParametersCopy(const double *params) const
{
    QVector<double> p=duplicateArrayV(params, get_paramcount());
    if (enableParameterTransforms&&hasParameterTransforms) transfromParameters(p.data());
    return p;
}

QVector<double> QFFitAlgorithm::FitQFFitFunction2DFunctor::backtransfromParametersCopy(const double *params) const
{
    QVector<double> p=duplicateArrayV(params, get_paramcount());
    if (enableParameterTransforms&&hasParameterTransforms) backtransfromParameters(p.data());
    return p;
}













void QFFitAlgorithm::FunctorForFMin::evaluateJacobian(double *evalout, const double *params)
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FunctorForFMin::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<1<<", pcnt="<<get_paramcount()<<")";
#endif
    evaluateJacobianNum(evalout, params);
}

void QFFitAlgorithm::FunctorForFMin::evaluateJacobianNum(double *evalout, const double *params, double h)
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FunctorForFMin::evaluateJacobianNum(eout="<<evalout<<", p="<<params<<", ecnt="<<1<<", pcnt="<<get_paramcount()<<")";
#endif
    int pcount=get_paramcount();
    QVector<double> dat0=duplicateArrayV(params, pcount);
    for (int io=0; io<pcount; io++) {
        evalout[io]=0;
    }

    for (register int ip=0; ip<pcount; ip++) {
        QVector<double> dat=dat0;
        dat[ip]=dat0[ip]+2.0*h;
        evalout[ip]=evalout[ip]-evaluate(dat.data());

        dat[ip]=dat0[ip]+h;
        evalout[ip]=evalout[ip]+8.0*evaluate(dat.data());


        dat[ip]=dat0[ip]-h;
        evalout[ip]=evalout[ip]-8.0*evaluate(dat.data());


        dat[ip]=dat0[ip]-2.0*h;
        evalout[ip]=evalout[ip]+evaluate(dat.data());

    }


    for (int io=0; io<pcount; io++) {
        evalout[io]=evalout[io]/(12.0*h);
    }
}


QFFitAlgorithm::FunctorForFMinFixAdaptor::FunctorForFMinFixAdaptor(QFFitAlgorithm::FunctorForFMin *model, const double *currentParams, const bool *fixParams):
    QFFitAlgorithm::FunctorForFMin()
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
    m_fixParams=(bool*)qfCalloc(m_N, sizeof(bool));
    functorFromModel=(int*)qfCalloc(m_N, sizeof(int));
    modelFromFunctor=(int*)qfCalloc(m_paramCount, sizeof(int));
    int pid=0;
    for (int i=0; i<m_N; i++) {
        functorFromModel[i]=-1;
        m_fixParams[i]=fixParams[i];
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
}

QFFitAlgorithm::FunctorForFMinFixAdaptor::~FunctorForFMinFixAdaptor()
{
    qfFree(functorFromModel);
    qfFree(modelFromFunctor);
    qfFree(m_modelParams);
    qfFree(m_fixParams);
}

double *QFFitAlgorithm::FunctorForFMinFixAdaptor::createMappedArrayForFunctor(const double *modelData)
{
    double* result=(double*)qfCalloc(m_paramCount, sizeof(double));

    for (register int i=0; i<m_paramCount; i++) {
        result[i]=modelData[modelFromFunctor[i]];
    }

    return result;
}

void QFFitAlgorithm::FunctorForFMinFixAdaptor::mapArrayFromModelToFunctor(double *functorData, const double *modelData)
{
    for (register int i=0; i<m_paramCount; i++) {
        functorData[i]=modelData[modelFromFunctor[i]];
    }
}

void QFFitAlgorithm::FunctorForFMinFixAdaptor::mapArrayFromFunctorToModel(double *modelData, const double *functorData)
{
    for (register int i=0; i<m_paramCount; i++) {
        modelData[modelFromFunctor[i]]=functorData[i];
    }
}

double QFFitAlgorithm::FunctorForFMinFixAdaptor::evaluate(const double *params)
{
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);
    double res= m_model->evaluate(mpt);
    qfFree(mpt);
    return res;
}

void QFFitAlgorithm::FunctorForFMinFixAdaptor::evaluateJacobian(double *evalout, const double *params)
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FunctorForFMinFixAdaptor::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<1<<", pcnt="<<get_paramcount()<<")";
#endif
    double* mpt=duplicateArray(m_modelParams, m_N);
    mapArrayFromFunctorToModel(mpt, params);
    m_model->evaluateJacobian(evalout, mpt);
    qfFree(mpt);
}



QFFitAlgorithm::FunctorForFMinFromCFunc::FunctorForFMinFromCFunc(int Nparams, QFFitAlgorithm::FMinFunctionP *F):
    FunctorForFMin()
{
    this->Nparams=Nparams;
    this->F=F;
    this->dFdp=NULL;
    this->m_userData=NULL;
    this->FnoUser=NULL;
    this->dFdpNoUser=NULL;
}

QFFitAlgorithm::FunctorForFMinFromCFunc::FunctorForFMinFromCFunc(int Nparams, QFFitAlgorithm::FMinFunctionP *F, QFFitAlgorithm::FMinFunctionDerivP *dFdp):
    FunctorForFMin()
{
    this->Nparams=Nparams;
    this->F=F;
    this->dFdp=dFdp;
    this->m_userData=NULL;
    this->FnoUser=NULL;
    this->dFdpNoUser=NULL;
}

QFFitAlgorithm::FunctorForFMinFromCFunc::FunctorForFMinFromCFunc(int Nparams, QFFitAlgorithm::FMinFunctionNoUserDataP *F):
    FunctorForFMin()
{
    this->Nparams=Nparams;
    this->F=NULL;
    this->dFdp=NULL;
    this->m_userData=NULL;
    this->FnoUser=F;
    this->dFdpNoUser=NULL;
}

QFFitAlgorithm::FunctorForFMinFromCFunc::FunctorForFMinFromCFunc(int Nparams, QFFitAlgorithm::FMinFunctionNoUserDataP *F, QFFitAlgorithm::FMinFunctionNoUserDataDerivP *dFdp):
    FunctorForFMin()
{
    this->Nparams=Nparams;
    this->F=NULL;
    this->dFdp=NULL;
    this->m_userData=NULL;
    this->FnoUser=F;
    this->dFdpNoUser=dFdp;
}

double QFFitAlgorithm::FunctorForFMinFromCFunc::evaluate(const double *params)
{
    if (F) {
        return (*F)(params, m_userData);
    } else if (FnoUser) {
        return (*FnoUser)(params);
    }
    return NAN;
}

void QFFitAlgorithm::FunctorForFMinFromCFunc::evaluateJacobian(double *evalout, const double *params)
{
#ifdef DEBUG_QFFitAlgorithm
    qDebug()<<"QFFitAlgorithm::FunctorForFMinFromCFunc::evaluateJacobian(eout="<<evalout<<", p="<<params<<", ecnt="<<1<<", pcnt="<<get_paramcount()<<")";
#endif
    if (dFdp) {
        (*dFdp)(evalout, params, m_userData);
    } else if (dFdpNoUser) {
        (*dFdpNoUser)(evalout, params);
    }
}

int QFFitAlgorithm::FunctorForFMinFromCFunc::get_paramcount() const
{
    return Nparams;
}

bool QFFitAlgorithm::FunctorForFMinFromCFunc::get_implementsJacobian() const
{
    return ((F!=NULL && dFdp!=NULL) || (FnoUser!=NULL && dFdpNoUser!=NULL));
}

void QFFitAlgorithm::FunctorForFMinFromCFunc::setUserData(void *userData)
{
    m_userData=userData;
}

void *QFFitAlgorithm::FunctorForFMinFromCFunc::getUserData() const
{
    return m_userData;
}
