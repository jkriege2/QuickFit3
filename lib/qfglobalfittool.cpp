#include "qfglobalfittool.h"









QFFitMultiQFFitFunctionFunctor::QFFitMultiQFFitFunctionFunctor():
    QFFitAlgorithm::Functor(0)
{
    m_paramCount=0;
}

QFFitMultiQFFitFunctionFunctor::~QFFitMultiQFFitFunctionFunctor()
{
    clear();
}

void QFFitMultiQFFitFunctionFunctor::addTerm(QFFitFunction *model, const double *currentParams, const bool *fixParams, const double *dataX, const double *dataY, const double *dataWeight, uint64_t M)
{
    subFunctorData sfd;
    sfd.f=new QFFitAlgorithm::FitQFFitFunctionFunctor(model, currentParams, fixParams, dataX, dataY, dataWeight, M);
    sfd.mapToLocal=(int*)calloc(sfd.f->get_paramcount(), sizeof(int));
    subFunctors.append(sfd);
    recalculateInternals();
}

void QFFitMultiQFFitFunctionFunctor::evaluate(double *evalout, const double *params)
{
}

void QFFitMultiQFFitFunctionFunctor::evaluateJacobian(double *evalout, const double *params)
{
}

bool QFFitMultiQFFitFunctionFunctor::get_implementsJacobian() const
{
    if (subFunctors.size()<=0) return false;
    bool implements=true;
    for (int i=0; i<subFunctors.size(); i++) {
        implements=implements&&subFunctors[i].f->get_implementsJacobian();
    }
    return implements;
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
    if (param>=0 && param<linkedParams.size()) {
        if (!linkedParams[param].contains(link)) linkedParams[param].append(link);
    }
}

void QFFitMultiQFFitFunctionFunctor::recalculateInternals()
{
    m_linkedParamsCount=0;
    m_evalout=0;
    for (int i=0; i<linkedParams.size(); i++) {
        if (isLinkedSingle(i)) {
            m_linkedParamsCount++;

        }
    }

    m_paramCount=0;
    int paramIdx=m_linkedParamsCount;
    for (int i=0; i<subFunctors.size(); i++) {
        QFFitAlgorithm::FitQFFitFunctionFunctor* f=subFunctors[i].f;
        m_evalout+=f->get_evalout();
        int pcount=f->get_paramcount();
        int realp=0;
        for (int p=0; p<pcount; p++) {
            int idx=getLinkID(i, p);
            if (idx>=0) {
                subFunctors[i].mapToLocal[p]=idx;
            } else {
                subFunctors[i].mapToLocal[p]=paramIdx;
                paramIdx++;
                realp++;
            }
        }
        subFunctors[i].localParams=realp;
        m_paramCount+=realp;
    }

    m_paramCount+=m_linkedParamsCount;
}

void QFFitMultiQFFitFunctionFunctor::clear()
{
    for (int i=0; i<subFunctors.size(); i++) {
        delete subFunctors[i].f;
        free(subFunctors[i].mapToLocal);
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
