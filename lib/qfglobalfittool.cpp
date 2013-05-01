#include "qfglobalfittool.h"
#include "qftools.h"


//#define DEBUG_GLOBALFIT
#undef DEBUG_GLOBALFIT




QFFitMultiQFFitFunctionFunctor::QFFitMultiQFFitFunctionFunctor():
    QFFitAlgorithm::Functor(0)
{
    m_paramCount=0;
}

QFFitMultiQFFitFunctionFunctor::~QFFitMultiQFFitFunctionFunctor()
{
    clear();
}

void QFFitMultiQFFitFunctionFunctor::addTerm(QFFitFunction *model, const double *currentParams, const bool *fixParams, const double *dataX, const double *dataY, const double *dataWeight, uint64_t M, double *paramsMin, double *paramsMax)
{
    subFunctorData sfd;
    sfd.f=new QFFitAlgorithm::FitQFFitFunctionFunctor(model, currentParams, fixParams, dataX, dataY, dataWeight, M);
    sfd.mapToLocal=(int*)calloc(sfd.f->get_paramcount(), sizeof(int));
    sfd.modelParams=duplicateArray(currentParams, model->paramCount());
    sfd.paramsMin=NULL;
    sfd.paramsMax=NULL;
    if (paramsMin) sfd.paramsMin=duplicateArray(paramsMin, model->paramCount());
    if (paramsMax) sfd.paramsMax=duplicateArray(paramsMax, model->paramCount());
    sfd.paramsFix=NULL;
    if (fixParams) duplicateArray(fixParams, model->paramCount());
    subFunctors.append(sfd);
    recalculateInternals();
}

void QFFitMultiQFFitFunctionFunctor::evaluate(double *evalout, const double *params) {
    int outCnt=0;
    for (int i=0; i<subFunctors.size(); i++) {
        double* eout=&(evalout[outCnt]);
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=subFunctors[i].f;
        int pcount=f->get_paramcount();
        double* pin=(double*)malloc(pcount*sizeof(double));

        for (int p=0; p<pcount; p++) {
            pin[p]=params[subFunctors[i].mapToLocal[p]];
        }

        f->evaluate(eout, pin);
        free(pin);

        outCnt+=subFunctors[i].f->get_evalout();
    }
}

void QFFitMultiQFFitFunctionFunctor::evaluateJacobian(double *evalout, const double *params) {
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
#ifdef DEBUG_GLOBALFIT
    QString dbg0="||";
    QString dbg ="||";
    QString dbg1="||";
    QString dbg2="||";
#endif
    m_linkedParamsCount=0;
    m_evalout=0;
    for (int i=0; i<linkedParams.size(); i++) {
        if (!isLinkedSingle(i)) {
            m_linkedParamsCount++;
#ifdef DEBUG_GLOBALFIT
            dbg +=QString(" G%1 |").arg(m_linkedParamsCount-1,2,10,QLatin1Char(' '));
            dbg1+=QString("%1 |").arg(m_linkedParamsCount-1,4,10,QLatin1Char(' '));
            dbg2+=QString("     |");
            dbg0+=QString("      ");
#endif
        }
    }
#ifdef DEBUG_GLOBALFIT
    dbg0+=QString("#| ");
    dbg +=QString("#| ");
    dbg1+=QString("#| ");
    dbg2+=QString("#| ");
#endif



    m_paramCount=0;
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
            if (idx>=0 && idx<m_linkedParamsCount) {
                subFunctors[i].mapToLocal[p]=idx;
#ifdef DEBUG_GLOBALFIT
                dbg0+=QString("F%1               |").arg(i,2,10,QLatin1Char(' '));
                dbg +=QString(" G%1              |").arg(idx,2,10,QLatin1Char(' '));
                //dbg1+=QString("%1              |").arg(cnt,4,10,QLatin1Char(' '));
                dbg1+=QString("                  |").arg(cnt,4,10,QLatin1Char(' '));
                dbg2+=QString("%1 |").arg(f->getModel()->getParameterID(f->mapFromFunctorToModel(p)).leftJustified(17,' ', true));
                cnt++;
#endif
            } else {
                subFunctors[i].mapToLocal[p]=paramIdx;
                paramIdx++;
                realp++;
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
        delete subFunctors[i].f;
        free(subFunctors[i].mapToLocal);
        free(subFunctors[i].paramsMin);
        free(subFunctors[i].paramsMax);
        free(subFunctors[i].paramsFix);
        free(subFunctors[i].modelParams);
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

QFGlobalFitTool::QFGlobalFitTool(QFFitAlgorithm *algorithm)
{
    m_algorithm=algorithm;
    functor=new QFFitMultiQFFitFunctionFunctor();
}

QFGlobalFitTool::~QFGlobalFitTool()
{
    delete functor;
}

void QFGlobalFitTool::addTerm(QFFitFunction *model, const double *currentParams, const bool *fixParams, const double *dataX, const double *dataY, const double *dataWeight, uint64_t M, double *paramsMin, double *paramsMax)
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

QFFitAlgorithm::FitResult QFGlobalFitTool::fit(QList<double *> paramsOut, QList<double *> paramErrorsOut, QList<double *> initialParams, QList<double *> paramErrorsIn)
{
    QFFitAlgorithm::FitResult res;
    int ppcount=functor->get_paramcount();
    double* params=(double*)calloc(ppcount,sizeof(double));
    double* errors=(double*)calloc(ppcount,sizeof(double));
    double* pmin=(double*)calloc(ppcount,sizeof(double));
    double* pmax=(double*)calloc(ppcount,sizeof(double));
    bool* fix=(bool*)calloc(ppcount,sizeof(bool));
    bool minmax=true;

    // fill parameter vector
    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=functor->getSubFunctor(i);
#ifdef DEBUG_GLOBALFIT
        qDebug()<<"F"<<i<<": "<<f->getModel()->name()<<": "<<listToString( f->getModel()->getParameterIDs(), true);
#endif
        int pcount=f->get_paramcount();
        double* pi=initialParams[i];
        double* err=paramErrorsOut[i];
        double* mi=functor->getParamsMin(i);
        double* ma=functor->getParamsMax(i);
        bool* fi=functor->getParamsFix(i);
        //double* po=paramsOut[i];
        for (int p=0; p<pcount; p++) {
            int modelIdx=f->mapFromFunctorToModel(p);
            int idx=functor->mapSubFunctorToGlobal(i, p);
#ifdef DEBUG_GLOBALFIT
            qDebug()<<"<-F"<<i<<":   idx="<<idx<<"/"<<ppcount<<"     modelIdx="<<modelIdx<<"/"<<f->getModelParamsCount();
            if (idx>=0 && idx<ppcount) {
#endif
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
            }
#endif
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
        res=m_algorithm->optimize(params, errors, functor, params, fix, pmin, pmax);
    } else {
        res=m_algorithm->optimize(params, errors, functor, params, fix, NULL, NULL);
    }
#ifdef DEBUG_GLOBALFIT
    qDebug()<<"AFTER OPTIMIZE: ("<<res.messageSimple<<")";
    qDebug()<<"  params = "<<arrayToString(params, ppcount);
    qDebug()<<"  errors = "<<arrayToString(errors, ppcount);
    qDebug()<<"  pmin   = "<<arrayToString(pmin, ppcount);
    qDebug()<<"  pmax   = "<<arrayToString(pmax, ppcount);
    qDebug()<<"  fix    = "<<arrayToString(fix, ppcount);
#endif


    // read out parameter vector
    for (int i=0; i<functor->getSubFunctorCount(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=functor->getSubFunctor(i);
        int pcount=f->get_paramcount();
        //double* pi=initialParams[i];
        double* err=paramErrorsOut[i];
        double* po=paramsOut[i];
        copyArray(po, f->getModelParams(), f->getModelParamsCount());
        copyArrayOrDefault(err, paramErrorsIn[i], f->getModelParamsCount(), 0.0);
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
            }
#endif
        }
    }
    free(params);
    free(errors);
    free(pmin);
    free(pmax);
    free(fix);
#ifdef DEBUG_GLOBALFIT
    qDebug()<<"return";
#endif
    return res;
}


QFGlobalThreadedFit::QFGlobalThreadedFit(QObject *parent): QThread(parent)
{
}

QFGlobalThreadedFit::~QFGlobalThreadedFit()
{
}

void QFGlobalThreadedFit::init(QFGlobalFitTool *model, QList<double *> paramsOut, QList<double *> paramErrorsOut, QList<double *> initialParams, QList<double *> paramErrorsIn)
{
    this->model=model;
    this->paramsOut=paramsOut;
    this->paramErrorsOut=paramErrorsOut;
    this->initialParams=initialParams;
    this->paramErrorsIn=paramErrorsIn;
    results.params.clear();
}

void QFGlobalThreadedFit::run()
{
    QTime tstart=QTime::currentTime();
    results=model->fit(paramsOut, paramErrorsOut, initialParams, paramErrorsIn);
    deltaTime=(double)tstart.msecsTo(QTime::currentTime());
}
