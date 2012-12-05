#include "qffitalgorithm.h"
#include "qftools.h"
#include <QMessageBox>
#include <QObject>
#include <cfloat>
#include <cstdio>
#include <iostream>
#include "qfrawdatarecord.h"

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
    QFFitAlgorithm::Functor(M)
{
    m_model=model;
    m_dataX=dataX;
    m_dataY=dataY;
    m_dataWeight=dataWeight;
    m_M=M;
    m_N=model->paramCount();
    functorFromModel=NULL;
    modelFromFunctor=NULL;

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
    functorFromModel=(int*)calloc(m_N, sizeof(int));
    modelFromFunctor=(int*)calloc(m_paramCount, sizeof(int));
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
    m_modelParams=(double*)calloc(m_N, sizeof(double));
    for (int i=0; i<m_N; i++) {
        m_modelParams[i]=currentParams[i];
    }
};

QFFitAlgorithm::FitQFFitFunctionFunctor::~FitQFFitFunctionFunctor() {
    free(functorFromModel);
    free(modelFromFunctor);
    free(m_modelParams);
}

double* QFFitAlgorithm::FitQFFitFunctionFunctor::createMappedArrayForFunctor(const double* modelData) {
    double* result=(double*)calloc(m_paramCount, sizeof(double));

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
    for (register int i=0; i<ecount; i++) {
        v = ( m_dataY[i] - m_model->evaluate(m_dataX[i], m_modelParams) ) / m_dataWeight[i];
        if (!QFFloatIsOK(v)) {
            if (i>0) v=evalout[i-1];
            else v=0;
        }
        evalout[i]=v;
    }
    //mapArrayFromModelToFunctor(params, m_modelParams);
}

void QFFitAlgorithm::FitQFFitFunctionFunctor::evaluateJacobian(double* evalout, const double* params) {
    mapArrayFromFunctorToModel(m_modelParams, params);
    register int pcount=get_paramcount();
    register int ecount=get_evalout();
    double* p=(double*)calloc(m_N, sizeof(double));
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
    free(p);
}



QFFitAlgorithm::FitResult QFFitAlgorithm::fit(double* paramsOut, double* paramErrorsOut, const double* dataX, const double* dataY, const double* dataWeight, uint64_t N, QFFitFunction* model, const double* initialParams, const bool* fixParams, const double* paramsMin, const double* paramsMax) {
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
        ppparamsMin=(double*)calloc(model->paramCount(), sizeof(double));
        for (int i=0; i<model->paramCount(); i++) {
            ppparamsMin[i]=-DBL_MAX;
        }
        pparamsMin=ppparamsMin;
    }
    if (paramsMax==NULL) {
        ppparamsMax=(double*)calloc(model->paramCount(), sizeof(double));
        for (int i=0; i<model->paramCount(); i++) {
            ppparamsMax[i]=DBL_MAX;
        }
        pparamsMax=ppparamsMax;
    }
    if (dataWeight==NULL) {
        dddataWeight=(double*)calloc(N, sizeof(double));
        for (uint64_t i=0; i<N; i++) {
            dddataWeight[i]=1.0;
        }
        ddataWeight=dddataWeight;
    }
    if (fixParams==NULL) {
        ppparamsFix=(bool*)calloc(model->paramCount(), sizeof(bool));
        for (int i=0; i<model->paramCount(); i++) {
            ppparamsFix[i]=false;
        }
        pparamsFix=ppparamsFix;
    }

    QFFitAlgorithm::FitQFFitFunctionFunctor fm(model, initialParams, pparamsFix, dataX, dataY, ddataWeight, N);

    //qDebug()<<"fm.get_paramcount()="<<fm.get_paramcount();

    double* tparamsMin=fm.createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm.createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)calloc(fm.get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)calloc(fm.get_paramcount(), sizeof(double));
    double* tinitialParams=fm.createMappedArrayForFunctor(initialParams);

    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, &fm, tparamsMin, tparamsMax);

    for (int i=0; i<model->paramCount(); i++) {
        paramsOut[i]=initialParams[i];
        paramErrorsOut[i]=0;
    }
    fm.mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    fm.mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    free(tparamsMax);
    free(tparamsMin);
    free(tparamErrorsOut);
    free(tparamsOut);
    free(tinitialParams);

    if (ppparamsMin==NULL) free(ppparamsMin);
    if (ppparamsMax==NULL) free(ppparamsMax);
    if (ppparamsFix==NULL) free(ppparamsFix);
    if (dddataWeight==NULL) free(dddataWeight);
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
        ppparamsMin=(double*)calloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsMin[i]=-DBL_MAX;
        }
        pparamsMin=ppparamsMin;
    }
    if (paramsMax==NULL) {
        ppparamsMax=(double*)calloc(model->get_paramcount(), sizeof(double));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsMax[i]=DBL_MAX;
        }
        pparamsMax=ppparamsMax;
    }

    if (fixParams==NULL) {
        ppparamsFix=(bool*)calloc(model->get_paramcount(), sizeof(bool));
        for (int i=0; i<model->get_paramcount(); i++) {
            ppparamsFix[i]=false;
        }
        pparamsFix=ppparamsFix;
    }



    QFFitAlgorithm::FitQFOptimizeFunctionFunctor fm(model, initialParams, pparamsFix);
    double* tparamsMin=fm.createMappedArrayForFunctor(pparamsMin);
    double* tparamsMax=fm.createMappedArrayForFunctor(pparamsMax);
    double* tparamsOut=(double*)calloc(fm.get_paramcount(), sizeof(double));
    double* tparamErrorsOut=(double*)calloc(fm.get_paramcount(), sizeof(double));
    double* tinitialParams=fm.createMappedArrayForFunctor(initialParams);

    result=intFit(tparamsOut, tparamErrorsOut, tinitialParams, &fm, tparamsMin, tparamsMax);

    for (int i=0; i<model->get_paramcount(); i++) {
        paramsOut[i]=initialParams[i];
        paramErrorsOut[i]=0;
    }
    fm.mapArrayFromFunctorToModel(paramsOut, tparamsOut);
    fm.mapArrayFromFunctorToModel(paramErrorsOut, tparamErrorsOut);

    free(tparamsMax);
    free(tparamsMin);
    free(tparamErrorsOut);
    free(tparamsOut);
    free(tinitialParams);

    if (ppparamsMin==NULL) free(ppparamsMin);
    if (ppparamsMax==NULL) free(ppparamsMax);
    if (ppparamsFix==NULL) free(ppparamsFix);
    return result;
}

bool QFFitAlgorithm::displayConfig() {
    QMessageBox::information(NULL, name(), QObject::tr("No configuration dialog is supplied for this optimization algorithm!"));
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
    functorFromModel=(int*)calloc(m_N, sizeof(int));
    modelFromFunctor=(int*)calloc(m_paramCount, sizeof(int));
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
    m_modelParams=(double*)calloc(m_N, sizeof(double));
    for (int i=0; i<m_N; i++) {
        m_modelParams[i]=currentParams[i];
    }
};

QFFitAlgorithm::FitQFOptimizeFunctionFunctor::~FitQFOptimizeFunctionFunctor() {
    free(functorFromModel);
    free(modelFromFunctor);
    free(m_modelParams);
}

double* QFFitAlgorithm::FitQFOptimizeFunctionFunctor::createMappedArrayForFunctor(const double* modelData) {
    double* result=(double*)calloc(m_paramCount, sizeof(double));

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
