/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "qfglobalfittool.h"
#include "qftools.h"
#include "qfmathtools.h"
#include "statistics_tools.h"
#define DEBUG_GLOBALFIT
//#undef DEBUG_GLOBALFIT




QFFitMultiQFFitFunctionFunctor::QFFitMultiQFFitFunctionFunctor():
    QFFitAlgorithm::Functor(0)
{
    doRecalculateInternals=true;
    m_paramCount=0;
}

QFFitMultiQFFitFunctionFunctor::~QFFitMultiQFFitFunctionFunctor()
{
    clear();
}

void QFFitMultiQFFitFunctionFunctor::addTerm(QFFitFunction *model, const double *currentParams, const bool *fixParams, const double *dataX, const double *dataY, const double *dataWeight, uint64_t M, const double *paramsMin, const double *paramsMax)
{
    subFunctorData sfd;

    //qDebug()<<"QFFitMultiQFFitFunctionFunctor::addTerm():";
    //qDebug()<<"   dataX="<<arrayToString(dataX, M);
    //qDebug()<<"   dataY="<<arrayToString(dataY, M);

    sfd.f=new QFFitAlgorithm::FitQFFitFunctionFunctor(model, currentParams, fixParams, dataX, dataY, dataWeight, M);
    sfd.mapToLocal=(int*)qfCalloc(sfd.f->get_paramcount(), sizeof(int));
    sfd.modelParams=duplicateArray(currentParams, model->paramCount());
    sfd.paramsMin=NULL;
    sfd.paramsMax=NULL;
    if (paramsMin) sfd.paramsMin=duplicateArray(paramsMin, model->paramCount());
    if (paramsMax) sfd.paramsMax=duplicateArray(paramsMax, model->paramCount());
    sfd.paramsFix=NULL;
    if (fixParams) sfd.paramsFix=duplicateArray(fixParams, model->paramCount());
    subFunctors.append(sfd);
    recalculateInternals();
}

void QFFitMultiQFFitFunctionFunctor::evaluate(double *evalout, const double *params) const {
    int outCnt=0;
    for (int i=0; i<subFunctors.size(); i++) {
        double* eout=&(evalout[outCnt]);
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=subFunctors[i].f;
        int pcount=f->get_paramcount();
        double* pin=(double*)qfMalloc(pcount*sizeof(double));

        for (int p=0; p<pcount; p++) {
            pin[p]=params[subFunctors[i].mapToLocal[p]];
        }

        f->evaluate(eout, pin);
        qfFree(pin);

        outCnt+=subFunctors[i].f->get_evalout();
    }
    //if (m_evalout!=outCnt) qDebug()<<"QFFitMultiQFFitFunctionFunctor::evaluate:   m_evalout="<<m_evalout<<"   outCnt="<<outCnt;
}

void QFFitMultiQFFitFunctionFunctor::evaluateJacobian(double *evalout, const double *params) const {
    int outCnt=0;
    for (int i=0; i<subFunctors.size(); i++) {
        double* eout=&(evalout[outCnt]);
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=subFunctors[i].f;
        int pcount=f->get_paramcount();
        double* pin=(double*)qfMalloc(pcount*sizeof(double));

        for (int p=0; p<pcount; p++) {
            pin[p]=params[subFunctors[i].mapToLocal[p]];
        }

        f->evaluateJacobian(eout, pin);
        qfFree(pin);

        outCnt+=(subFunctors[i].f->get_evalout()*pcount);
    }
}

bool QFFitMultiQFFitFunctionFunctor::get_implementsJacobian() const
{
    /*if (subFunctors.size()<=0) return false;
    bool implements=true;
    for (int i=0; i<subFunctors.size(); i++) {
        implements=implements&&subFunctors[i].f->get_implementsJacobian();
    }
    return implements;*/
    return false;
}

int QFFitMultiQFFitFunctionFunctor::get_paramcount() const
{
    return m_paramCount;
}

void QFFitMultiQFFitFunctionFunctor::setAllLinks(const QList<QList<QPair<int, int> > > &linkedParams)
{
    this->linkedParams=linkedParams;
    recalculateInternals();
}

void QFFitMultiQFFitFunctionFunctor::addGlobalParam(const QList<QPair<int, int> > &links)
{
    linkedParams.append(links);
    recalculateInternals();
}

void QFFitMultiQFFitFunctionFunctor::addToGlobalParam(int param, QPair<int, int> link)
{
#ifdef DEBUG_GLOBALFIT
    qDebug()<<"addToGlobalParam("<<param<<link.first<<link.second<<")  linkedParams.size()="<<linkedParams.size();
#endif
    if (param>=0 && param<linkedParams.size()) {
        if (!linkedParams[param].contains(link)) linkedParams[param].append(link);
    }
    recalculateInternals();
}

void QFFitMultiQFFitFunctionFunctor::addToGlobalParam(int param, int term, int localParam)
{
    addToGlobalParam(param, qMakePair(term, localParam));
}

void QFFitMultiQFFitFunctionFunctor::setGlobalParamCount(int count)
{
    linkedParams.clear();
    QList<QPair<int, int> > empty;
    while (linkedParams.size()<count) {
        linkedParams.append(empty);
    }
    recalculateInternals();
}


void QFFitMultiQFFitFunctionFunctor::recalculateInternals()
{
    if (!doRecalculateInternals) return;
#ifdef DEBUG_GLOBALFIT
    QString dbg0="||";
    QString dbg ="||";
    QString dbg1="||";
    QString dbg2="||";
#endif
    m_linkedParamsCount=0;
    m_evalout=0;
   // QMap<int, int> global_reorder;
    for (int i=0; i<linkedParams.size(); i++) {
        //if (!isLinkedSingle(i)) {
            m_linkedParamsCount++;
            paramNames.append(QString("Global%1").arg(m_linkedParamsCount-1));
#ifdef DEBUG_GLOBALFIT
            dbg +=QString(" G%1 |").arg(m_linkedParamsCount-1,2,10,QLatin1Char(' '));
            dbg1+=QString("%1 |").arg(m_linkedParamsCount-1,4,10,QLatin1Char(' '));
            dbg2+=QString("     |");
            dbg0+=QString("      ");
#endif
        //}
    }
#ifdef DEBUG_GLOBALFIT
    dbg0+=QString("#| ");
    dbg +=QString("#| ");
    dbg1+=QString("#| ");
    dbg2+=QString("#| ");
#endif



    m_paramCount=0;
    paramNames.clear();
    int paramIdx=m_linkedParamsCount;
#ifdef DEBUG_GLOBALFIT
    int cnt=m_linkedParamsCount;
#endif
    for (int i=0; i<subFunctors.size(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=subFunctors[i].f;
        m_evalout+=f->get_evalout();
        int pcount=f->get_paramcount();
        int realp=0;
        for (int p=0; p<pcount; p++) {
            int idx=getLinkID(i, f->mapFromFunctorToModel(p));
#ifdef DEBUG_GLOBALFIT
            qDebug()<<"cheking "<<i<<p<<f->mapFromFunctorToModel(p)<<" ["<<m_linkedParamsCount<<"]  => "<<idx;
#endif
            if (idx>=0 && idx<m_linkedParamsCount) {
                subFunctors[i].mapToLocal[p]=idx;
#ifdef DEBUG_GLOBALFIT
                dbg0+=QString("F%1               |").arg(i,2,10,QLatin1Char(' '));
                dbg +=QString(" G%1              |").arg(idx,2,10,QLatin1Char(' '));
                //dbg1+=QString("%1              |").arg(cnt,4,10,QLatin1Char(' '));
                dbg1+=QString("                  |");
                dbg2+=QString("%1 |").arg(f->getModel()->getParameterID(f->mapFromFunctorToModel(p)).leftJustified(17,' ', true));
                cnt++;
#endif
            } else {
                subFunctors[i].mapToLocal[p]=paramIdx;
                paramIdx++;
                realp++;
                paramNames.append(QString("Model%1: ").arg(i)+f->getModel()->getParameterID(f->mapFromFunctorToModel(p)));
#ifdef DEBUG_GLOBALFIT
                dbg0+=QString("F%1             |").arg(i,2,10,QLatin1Char(' '));
                dbg+= QString(" L%1(%2)        |").arg(p,2,10,QLatin1Char(' ')).arg(f->mapFromFunctorToModel(p),2,10,QLatin1Char(' '));
                dbg1+=QString("%1            |").arg(paramIdx-1,4,10,QLatin1Char(' '));
                dbg2+=QString("%1 |").arg(f->getModel()->getParameterID(f->mapFromFunctorToModel(p)).leftJustified(15,' ', true));
                cnt++;
#endif
            }
        }
        subFunctors[i].localParams=realp;
        m_paramCount+=realp;
#ifdef DEBUG_GLOBALFIT
        qDebug()<<"subFunctors["<<i<<"]: localParams="<<realp<<" name="<<f->getModel()->name();
#endif
    }
    m_paramCount+=m_linkedParamsCount;
#ifdef DEBUG_GLOBALFIT
    qDebug()<<"m_paramCount        = "<<m_paramCount;
    qDebug()<<"m_linkedParamsCount = "<<m_linkedParamsCount;
    qDebug()<<"outputParams = "<<get_evalout();
    qDebug()<<dbg0;
    qDebug()<<dbg;
    qDebug()<<dbg1;
    qDebug()<<dbg2;
#endif

}

void QFFitMultiQFFitFunctionFunctor::clear()
{
    for (int i=0; i<subFunctors.size(); i++) {
        if (subFunctors[i].f) delete subFunctors[i].f;
        //subFunctors[i].f=NULL;
        if (subFunctors[i].mapToLocal) qfFree(subFunctors[i].mapToLocal);
        if (subFunctors[i].paramsMin) qfFree(subFunctors[i].paramsMin);
        if (subFunctors[i].paramsMax) qfFree(subFunctors[i].paramsMax);
        if (subFunctors[i].paramsFix) qfFree(subFunctors[i].paramsFix);
        if (subFunctors[i].modelParams) qfFree(subFunctors[i].modelParams);
    }
    subFunctors.clear();
    m_paramCount=0;
    m_linkedParamsCount=0;
    linkedParams.clear();
}

bool QFFitMultiQFFitFunctionFunctor::isLinkedSingle(int idx) const
{
    if (idx<0 || idx>=linkedParams.size()) return true;
    return linkedParams[idx].size()<=1;
}

int QFFitMultiQFFitFunctionFunctor::getLinkID(int subID, int paramID)
{
    for (int i=0; i<linkedParams.size(); i++) {
        if (linkedParams[i].contains(qMakePair(subID, paramID))) return i;
    }
    return -1;
}

void QFFitMultiQFFitFunctionFunctor::setDoRecalculateInternals(bool enabled)
{
    doRecalculateInternals=enabled;
    if (enabled) recalculateInternals();
}

bool QFFitMultiQFFitFunctionFunctor::isWeightedLSQ() const
{
    for (int i=0; i<subFunctors.size(); i++) {
        if (!subFunctors[i].f || !subFunctors[i].f->isWeightedLSQ()) return false;
    }
    return true;
}

bool QFFitMultiQFFitFunctionFunctor::areAllWeightsOne() const
{
    for (int i=0; i<subFunctors.size(); i++) {
        if (subFunctors[i].f) {
            const double* w=subFunctors[i].f->getDataWeight();
            uint64_t N=subFunctors[i].f->getDataPoints();
            for (uint64_t i=0; i<N; i++) {
                if (w[i]!=1.0) return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void QFFitMultiQFFitFunctionFunctor::prepareBootstrapSelection()
{
    for (int i=0; i<subFunctors.size(); i++) {
        subFunctors[i].f->setBootstrappingEnabled(bsEnabled, false);
        subFunctors[i].f->setBootstrappingFraction(bsFraction, false);
        subFunctors[i].f->prepareBootstrapSelection();
    }
}

void QFFitMultiQFFitFunctionFunctor::reapplyBootstrapselection()
{
    for (int i=0; i<subFunctors.size(); i++) {
        subFunctors[i].f->setBootstrappingEnabled(bsEnabled, false);
        subFunctors[i].f->setBootstrappingFraction(bsFraction, false);
        subFunctors[i].f->reapplyBootstrapselection();
    }
}

void QFFitMultiQFFitFunctionFunctor::setBootstrappingEnabled(bool enabled, bool prepBootstrapping)
{
    bsEnabled=enabled;
    if (bsEnabled&&prepBootstrapping) prepareBootstrapSelection();
}

void QFFitMultiQFFitFunctionFunctor::setBootstrappingFraction(double fraction, bool prepBootstrapping)
{
    bsFraction=fraction;
    if (bsEnabled&&prepBootstrapping) prepareBootstrapSelection();
}

void QFFitMultiQFFitFunctionFunctor::evaluateJacobianNum(double *evalout, const double *params, double h) const
{
    int outCnt=0;
    for (int i=0; i<subFunctors.size(); i++) {
        double* eout=&(evalout[outCnt]);
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=subFunctors[i].f;
        int pcount=f->get_paramcount();
        double* pin=(double*)qfMalloc(pcount*sizeof(double));

        for (int p=0; p<pcount; p++) {
            pin[p]=params[subFunctors[i].mapToLocal[p]];
        }

        f->evaluateJacobianNum(eout, pin, h);
        qfFree(pin);

        outCnt+=(subFunctors[i].f->get_evalout()*pcount);
    }
}

double QFFitMultiQFFitFunctionFunctor::calculateChi2(const double *params) const
{
    QVector<double> eout(get_evalout());
    evaluate(eout.data(), params);
    double chi2=0;
    for (uint64_t i=0; i<get_evalout(); i++) {
        chi2 += qfSqr(eout[i]);
    }
    return chi2;
}

QFGlobalFitTool::QFGlobalFitTool(QFFitAlgorithm *algorithm)
{
    m_algorithm=algorithm;
    m_repeatFit=1;
    m_globalLocalRepeats=0;
    functor=new QFFitMultiQFFitFunctionFunctor();
}

QFGlobalFitTool::~QFGlobalFitTool()
{
    delete functor;
    qfClearContainer(localFunctors);
}

void QFGlobalFitTool::addTerm(QFFitFunction *model, const double *currentParams, const bool *fixParams, const double *dataX, const double *dataY, const double *dataWeight, uint64_t M, const double *paramsMin, const double *paramsMax)
{
    functor->addTerm(model, currentParams, fixParams, dataX, dataY, dataWeight, M, paramsMin, paramsMax);
}

void QFGlobalFitTool::setAllLinks(const QList<QList<QPair<int, int> > > &linkedParams)
{
    functor->setAllLinks(linkedParams);
}

void QFGlobalFitTool::addGlobalParam(const QList<QPair<int, int> > &links)
{
    functor->addGlobalParam(links);
}

void QFGlobalFitTool::addToGlobalParam(int param, QPair<int, int> link)
{
    functor->addToGlobalParam(param, link);
}

void QFGlobalFitTool::addToGlobalParam(int param, int term, int localParam)
{
    functor->addToGlobalParam(param, qMakePair(term, localParam));
}

void QFGlobalFitTool::setGlobalParamCount(int count)
{
    functor->setGlobalParamCount(count);
}

void QFGlobalFitTool::clear()
{
    functor->clear();
}

void QFGlobalFitTool::setRepeats(int fitRepeat)
{
    m_repeatFit=fitRepeat;
}

void QFGlobalFitTool::setGlobalLocalRepeats(int fitRepeat)
{
    m_globalLocalRepeats=fitRepeat;
}

void QFGlobalFitTool::setDoRecalculateInternals(bool enabled)
{
    functor->setDoRecalculateInternals(enabled);
}

void QFGlobalFitTool::createLocalFitFunctors()
{
    qfClearContainer(localFunctors, true);
    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        QFFitFunction* ff=functor->getSubFunctor(i)->getModel();
        const double* dataX=functor->getSubFunctor(i)->getDataX();
        const double* dataY=functor->getSubFunctor(i)->getDataF();
        const double* dataW=functor->getSubFunctor(i)->getDataWeight();
        uint64_t dataN=functor->getSubFunctor(i)->getDataPoints();
        const double* param=functor->getSubFunctor(i)->getModelParams();
         bool* fixParams=functor->getSubFunctor(i)->getModelParamsFix();

        int freeParams=0;
        for (int p=0; p<ff->paramCount(); p++) {
            fixParams[p]=fixParams[p]||(functor->getLinkID(i, p)>=0);
            if (!fixParams[p]) freeParams++;
        }

        if (freeParams>0) {
            QFFitAlgorithm::FitQFFitFunctionFunctor* lf=new QFFitAlgorithm::FitQFFitFunctionFunctor(ff, param, fixParams, dataX, dataY, dataW, dataN);
            localFunctors.append(lf);
        } else {
            localFunctors.append(NULL);
        }
    }
}

QFFitAlgorithm::FitResult QFGlobalFitTool::fit(const QList<double *> &paramsOut, const QList<double *> &paramErrorsOut, const QList<double *> &initialParams, const QList<double *> &paramErrorsIn, QFBasicFitStatistics *fitstat, QVector<double> *rawFitResults_io, QVector<double> *rawFitResultsErrors_io, double *chi2_io, QVector<double> *cov_matrix_io, QStringList *paramNames_io, QVector<double> *fitX_io, QVector<double> *fitY_io, QVector<double> *fitW_io, QVector<double> *fitM_io) {
    QVector<double> rawFitResults;
    QVector<double> rawFitResultsErrors;
    double chi2;
    QVector<double> cov_matrix;
    QStringList paramNames;
    QVector<double> fitX;
    QVector<double> fitY;
    QVector<double> fitW;
    QVector<double> fitM;

    QFFitAlgorithm::FitResult result;
    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=functor->getSubFunctor(i);
        int pcount=f->get_paramcount();
        copyArray(paramsOut[i], initialParams[i], pcount);
        copyArray(paramErrorsOut[i], paramErrorsIn[i], pcount);
    }
    if (m_globalLocalRepeats<=0) {
        qDebug()<<"fit global ...";
        result= fitGlobal(paramsOut, paramErrorsOut, &rawFitResults, &rawFitResultsErrors, &chi2, &cov_matrix, &paramNames, &fitX, &fitY, &fitW, &fitM);
    } else {

        for (int gl=0; gl<m_globalLocalRepeats; gl++) {
            qDebug()<<"fit local ...";
            fitLocal(paramsOut, paramErrorsOut);
            qDebug()<<"fit global ...";
            result=fitGlobal(paramsOut, paramErrorsOut, &rawFitResults, &rawFitResultsErrors, &chi2, &cov_matrix, &paramNames, &fitX, &fitY, &fitW, &fitM);
        }
    }

    if (fitstat) {
        qDebug()<<"calc stat ...";
        //qDebug()<<"calc global fit stat fitX.size()="<<fitX.size()<<"  fitM.size()="<<fitM.size()<<"  fitY.size()="<<fitY.size()<<"  fitW.size()="<<fitW.size()<<"  rawFitResults.size()="<<rawFitResults.size()<<"  cov_matrix.size()="<<cov_matrix.size();
        (*fitstat)=calculateBasicFitStatistics(fitX.size(), fitX.data(), fitM.data(), fitY.data(), fitW.data(), -1,-1, rawFitResults.size(), rawFitResults.data(), rawFitResultsErrors.data(), cov_matrix, 200, true);
    }

    qDebug()<<"save output ...";
    if (rawFitResults_io) *rawFitResults_io=rawFitResults;
    if (rawFitResultsErrors_io) *rawFitResultsErrors_io=rawFitResultsErrors;
    if (chi2_io) *chi2_io=chi2;
    if (cov_matrix_io) *cov_matrix_io=cov_matrix;
    if (paramNames_io) *paramNames_io=paramNames;
    if (fitX_io) *fitX_io=fitX;
    if (fitY_io) *fitY_io=fitY;
    if (fitW_io) *fitW_io=fitW;
    if (fitM_io) *fitM_io=fitM;

    return result;
}

QList<QFFitAlgorithm::FitResult> QFGlobalFitTool::fitLocal(const QList<double *> &paramsOut, const QList<double *> &paramErrorsOut) {
    QList<QFFitAlgorithm::FitResult> res;
    for (int i=0; i<localFunctors.size(); i++) {

        if (localFunctors[i]) {
            double* params=paramsOut[i];
            double* errors=paramErrorsOut[i];
            for (int c=0; c<m_repeatFit; c++) {
                QFFitAlgorithm::FitResult r=m_algorithm->lsqMinimize(params, errors, localFunctors[i], params, localFunctors[i]->getModelParamsFix(), functor->getParamsMin(i), functor->getParamsMax(i));
                if (c==0) res.append(r);
                else res[i]=r;
            }
        }
    }
    return res;
}

QFFitAlgorithm::FitResult QFGlobalFitTool::fitGlobal(const QList<double *> &paramsOut, const QList<double *> &paramErrorsOut, QVector<double> *rawFitResults, QVector<double> *rawFitResultsErrors, double *chi2_out, QVector<double> *cov_matrix, QStringList* paramNames, QVector<double> *fitX, QVector<double> *fitY, QVector<double> *fitW, QVector<double> *fitM)
{
    int debugCnt=0;
    QFFitAlgorithm::FitResult result;
    int ppcount=functor->get_paramcount();
    qDebug()<<"fitglobal "<<(debugCnt++);
    double* params=(double*)qfCalloc(ppcount,sizeof(double));
    qDebug()<<"fitglobal "<<(debugCnt++);
    double* errors=(double*)qfCalloc(ppcount,sizeof(double));
    qDebug()<<"fitglobal "<<(debugCnt++);
    double* pmin=(double*)qfCalloc(ppcount,sizeof(double));
    qDebug()<<"fitglobal "<<(debugCnt++);
    double* pmax=(double*)qfCalloc(ppcount,sizeof(double));
    qDebug()<<"fitglobal "<<(debugCnt++);
    bool* fix=(bool*)qfCalloc(ppcount,sizeof(bool));
    qDebug()<<"fitglobal "<<(debugCnt++);
    for (int i=0; i<ppcount; i++) fix[i]=true;
    bool minmax=true;

    if (paramNames) (*paramNames)=functor->getParamNames();
    qDebug()<<"fitglobal "<<(debugCnt++);

    // fill parameter vector
    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=functor->getSubFunctor(i);
        int pcount=f->get_paramcount();
        double* pi=paramsOut[i];
        double* err=paramErrorsOut[i];
        double* mi=functor->getParamsMin(i);
        double* ma=functor->getParamsMax(i);
        bool* fi=functor->getParamsFix(i);
#ifdef DEBUG_GLOBALFIT
        qDebug()<<"\n\n\n\n\n\n\n\n\n\n\n\n\n";
        qDebug()<<"F"<<i<<": "<<f->getModel()->name()<<":\n   "<<listToString( f->getModel()->getParameterIDs(), true)<<"\n   "<<arrayToString(paramsOut[i], f->getModel()->paramCount());
        if (fi) qDebug()<<"\n   "<<arrayToString(fi, f->getModel()->paramCount());
#endif
        //double* po=paramsOut[i];
        for (int p=0; p<pcount; p++) {
            int modelIdx=f->mapFromFunctorToModel(p);
            int idx=functor->mapSubFunctorToGlobal(i, p);
#ifdef DEBUG_GLOBALFIT
            qDebug()<<"<-F"<<i<<":   idx="<<idx<<"/"<<ppcount<<"     modelIdx="<<modelIdx<<"/"<<f->getModelParamsCount()<<"  pi="<<pi<<"   err="<<err;
#endif
            if (idx>=0 && idx<ppcount) {
                params[idx]=pi[modelIdx];
                errors[idx]=err[modelIdx];
                minmax=minmax&&(mi&&ma);
                if (mi) pmin[idx]=mi[modelIdx];
                if (ma) pmax[idx]=ma[modelIdx];
                if (fi) {
                    fix[idx]=fi[modelIdx];
                } else {
                    fix[idx]=false;
                }
#ifdef DEBUG_GLOBALFIT
            } else {
                qDebug()<<"ERROR: invalid idx="<<idx<<"   max="<<ppcount;
#endif
            }
        }
    }
#ifdef DEBUG_GLOBALFIT
    qDebug()<<"BEFORE OPTIMIZE : minmax="<<minmax;
    qDebug()<<"  params = "<<arrayToString(params, ppcount);
    qDebug()<<"  errors = "<<arrayToString(errors, ppcount);
    qDebug()<<"  pmin   = "<<arrayToString(pmin, ppcount);
    qDebug()<<"  pmax   = "<<arrayToString(pmax, ppcount);
    qDebug()<<"  fix    = "<<arrayToString(fix, ppcount);
#endif

    if (minmax) {
        for (int i=0; i<m_repeatFit; i++) {
            qDebug()<<"fitglobal "<<(debugCnt++)<<"lsqMinimize() ...";
            result=m_algorithm->lsqMinimize(params, errors, functor, params, fix, pmin, pmax);
            qDebug()<<"fitglobal "<<(debugCnt++)<<"lsqMinimize() ... DONE";
        }
    } else {
        for (int i=0; i<m_repeatFit; i++) {
            qDebug()<<"fitglobal "<<(debugCnt++)<<"lsqMinimize() ...";
            result=m_algorithm->lsqMinimize(params, errors, functor, params, fix, NULL, NULL);
            qDebug()<<"fitglobal "<<(debugCnt++)<<"lsqMinimize() ... DONE";
        }
    }
#ifdef DEBUG_GLOBALFIT
    qDebug()<<"AFTER OPTIMIZE: ";//("<<res.messageSimple<<")";
    qDebug()<<"  params = "<<arrayToString(params, ppcount);
    qDebug()<<"  errors = "<<arrayToString(errors, ppcount);
    qDebug()<<"  pmin   = "<<arrayToString(pmin, ppcount);
    qDebug()<<"  pmax   = "<<arrayToString(pmax, ppcount);
    qDebug()<<"  fix    = "<<arrayToString(fix, ppcount);
#endif

    qDebug()<<"fitglobal "<<(debugCnt++)<<"chi2";
    double chi2=functor->calculateChi2(params);
    qDebug()<<"fitglobal "<<(debugCnt++)<<"rawRes ...";
    if (rawFitResults) {
        rawFitResults->clear();
        for (int i=0; i<ppcount; i++) {
            rawFitResults->append(params[i]);
        }
    }
    qDebug()<<"fitglobal "<<(debugCnt++)<<"rawResErr ...";

    if (rawFitResultsErrors) {
        rawFitResultsErrors->clear();
        for (int i=0; i<ppcount; i++) {
            rawFitResultsErrors->append(errors[i]);
        }
    }


    // write out parameter vector
    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=functor->getSubFunctor(i);
        int pcount=f->get_paramcount();
        //double* pi=initialParams[i];
        double* err=paramErrorsOut[i];
        double* po=paramsOut[i];
#ifdef DEBUG_GLOBALFIT
        qDebug()<<"->F"<<i<<":   err="<<err<<"  po="<<po;
#endif
        //copyArray(po, f->getModelParams(), f->getModelParamsCount());
        //copyArrayOrDefault(err, paramErrorsIn[i], f->getModelParamsCount(), 0.0);
        for (int p=0; p<pcount; p++) {
            int modelIdx=f->mapFromFunctorToModel(p);
            int idx=functor->mapSubFunctorToGlobal(i, p);
#ifdef DEBUG_GLOBALFIT
            qDebug()<<"->F"<<i<<":   idx="<<idx<<"/"<<ppcount<<"     modelIdx="<<modelIdx<<"/"<<f->getModelParamsCount();
            if (idx>=0 && idx<ppcount) {
#endif
                po[modelIdx]=params[idx];
                err[modelIdx]=errors[idx];
#ifdef DEBUG_GLOBALFIT
            } else {
                qDebug()<<"ERROR: invalid idx="<<idx<<"   max="<<ppcount;
            }
#endif
        }
        qDebug()<<"fitglobal "<<(debugCnt++)<<"writeparam ...";

    }

    qDebug()<<"fitglobal "<<(debugCnt++)<<"det XYMW ...";

    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        if (fitX) fitX->operator<<(functor->getSubFunctor(i)->getDataXV());
        if (fitY) fitY->operator<<(functor->getSubFunctor(i)->getDataFV());
        if (fitW) fitW->operator<<(functor->getSubFunctor(i)->getDataWeightV());
        if (fitM) {
            QVector<double> m;
            m.resize(functor->getSubFunctor(i)->get_evalout());
            functor->getSubFunctor(i)->evaluateModelOnly(m.data(), paramsOut[i]);
            fitM->operator<<(m);
        }
    }

    qDebug()<<"fitglobal "<<(debugCnt++)<<"COV ...";

    if (cov_matrix) {
        QVector<double> COV(functor->get_paramcount()*functor->get_paramcount());
        if (result.params.contains("covariance_matrix") && result.params["covariance_matrix"].getVectorMatrixItems()==COV.size()) {
            COV=result.params["covariance_matrix"].getAsDoubleVector();
            //qDebug()<<"fit(): read cov matrix: "<<COV;
        } else {
            //if ( result.addNumberMatrix("covariance_matrix", COV.data(), model->get_paramcount(), model->get_paramcount());)
            QVector<double> J(functor->get_evalout()*functor->get_paramcount());
            functor->evaluateJacobian(J.data(), params);
            bool hasWeights=true;
            for (int i=0; i<functor->getSubFunctorCount(); i++) {
                if (!QFFitAlgorithm::functorHasWeights(functor->getSubFunctor(i))) {
                    hasWeights=false;
                    break;
                }
            }
            if (hasWeights) statisticsGetFitProblemCovMatrix(COV.data(), J.data(), functor->get_evalout(), functor->get_paramcount());
            else statisticsGetFitProblemVarCovMatrix(COV.data(), J.data(), functor->get_evalout(), functor->get_paramcount(), chi2);


            result.addNumberMatrix("covariance_matrix", COV.data(), functor->get_paramcount(), functor->get_paramcount());
            //qDebug()<<"fit(): calc cov matrix: "<<COV;
        }

        cov_matrix->clear();
        (*cov_matrix)=COV;
    }
    if (chi2_out) {
        *chi2_out=chi2;
    }
    result.addNumber("error_sum", chi2);


    qDebug()<<"fitglobal "<<(debugCnt++)<<"FREE ...";

    qfFree(params);
    qfFree(errors);
    qfFree(pmin);
    qfFree(pmax);
    qfFree(fix);
#ifdef DEBUG_GLOBALFIT
    qDebug()<<"return";
#endif
    return result;
}

void QFGlobalFitTool::evalueChi2Landscape(double *chi2Landscape, int paramXFile, int paramXID, const QVector<double> &paramXValues, int paramYFile, int paramYID, const QVector<double> &paramYValues, const QList<double *>& initialParams)
{
    int ppcount=functor->get_paramcount();
    double* params=(double*)qfCalloc(ppcount,sizeof(double));

    // fill parameter vector
    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=functor->getSubFunctor(i);
        int pcount=f->get_paramcount();
        double* pi=initialParams[i];
        //double* po=paramsOut[i];
        for (int p=0; p<pcount; p++) {
            int modelIdx=f->mapFromFunctorToModel(p);
            int idx=functor->mapSubFunctorToGlobal(i, p);
            params[idx]=pi[modelIdx];
        }
    }

    int idxX=functor->mapSubFunctorToGlobal(paramXFile, paramXID);
    int idxY=functor->mapSubFunctorToGlobal(paramYFile, paramYID);
    int idx=0;
    double* d=(double*)qfCalloc(functor->get_evalout(), sizeof(double));

    for (int y=0; y<paramYValues.size(); y++) {
        for (int x=0; x<paramXValues.size(); x++) {
            params[idxX]=paramXValues[x];
            params[idxY]=paramYValues[y];

            functor->evaluate(d, params);

            double chi2=0;
            //qDebug()<<"---------------------------------------------------------------------------";
            //qDebug()<<"-- x,y = "<<x<<y;
            //qDebug()<<"-- x,y = "<<paramXValues[x]<<paramYValues[y];
            //qDebug()<<"-- evalout_count = "<<functor->get_evalout();
            //qDebug()<<"---------------------------------------------------------------------------";
            for (uint64_t i=0; i<functor->get_evalout(); i++)  {
                chi2=chi2+qfSqr(d[i]);
                //qDebug()<<d[i]<<qfSqr(d[i])<<chi2;
            }
            chi2Landscape[idx]=chi2;

            idx++;
        }
    }

    qfFree(d);
}

QFFitMultiQFFitFunctionFunctor *QFGlobalFitTool::getFunctor() const
{
    return functor;
}



QFGlobalThreadedFit::QFGlobalThreadedFit(QObject *parent): QThread(parent)
{
    this->chi2=NULL;
    this->COV=NULL;
    this->paramNames=NULL;
    this->fitstat=NULL;
    this->rawFitResults=NULL;
    this->rawFitResultsErrors=NULL;
}

QFGlobalThreadedFit::~QFGlobalThreadedFit()
{
}

void QFGlobalThreadedFit::init(QFGlobalFitTool *globalTool, QList<double *> paramsOut, QList<double *> paramErrorsOut, QList<double *> initialParams, QList<double *> paramErrorsIn, double *chi2, QFBasicFitStatistics *fitstat, QVector<double> *rawFitResults, QVector<double> *rawFitResultsErrors, QVector<double> *COV, QStringList *paramNames, QVector<double>* fitX, QVector<double>* fitY, QVector<double>* fitW, QVector<double>* fitM)
{
    this->globalTool=globalTool;
    this->paramsOut=paramsOut;
    this->paramErrorsOut=paramErrorsOut;
    this->initialParams=initialParams;
    this->paramErrorsIn=paramErrorsIn;
    this->chi2=chi2;
    this->COV=COV;
    this->paramNames=paramNames;
    this->fitX=fitX;
    this->fitY=fitY;
    this->fitW=fitW;
    this->fitM=fitM;
    this->fitstat=fitstat;
    this->rawFitResults=rawFitResults;
    this->rawFitResultsErrors=rawFitResultsErrors;
    results.params.clear();
}

void QFGlobalThreadedFit::run()
{
    QTime tstart=QTime::currentTime();
    results=globalTool->fit(paramsOut, paramErrorsOut, initialParams, paramErrorsIn, fitstat, rawFitResults, rawFitResultsErrors, chi2, COV, paramNames, fitX, fitY, fitW, fitM);
    deltaTime=(double)tstart.msecsTo(QTime::currentTime());
}
