#include "qffitresultsbyindexevaluationfittools.h"
#include "qffitresultsbyindexevaluation.h"


QFFitStatistics QFFitResultsByIndexEvaluationFitToolsBase::calcFitStatistics(bool storeAsResults, QFFitFunction *ffunc, long N, double *tauvals, double *corrdata, double *weights, int datacut_min, int datacut_max, double *fullParams, double *errors, bool *paramsFix, int runAvgWidth, int residualHistogramBins, QFRawDataRecord *record, int run)
{
    QFFitStatistics result= ffunc->calcFitStatistics(N, tauvals, corrdata, weights, datacut_min, datacut_max, fullParams, errors, paramsFix, runAvgWidth, residualHistogramBins);

    QFFitResultsByIndexEvaluation* t=dynamic_cast<QFFitResultsByIndexEvaluation*>(this);
    if (record && t) {
        if (storeAsResults) {

            QString param="";
            QString eid= t->getEvaluationResultID(run, record);
            t->setFitResultValue(record, eid, param="fitstat_chisquared", result.residSqrSum);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

            t->setFitResultValue(record, eid, param="fitstat_chisquared_weighted", result.residWeightSqrSum);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

            t->setFitResultValue(record, eid, param="fitstat_residavg", result.residAverage);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("residual average"), QString("&lang;E&rang;"));

            t->setFitResultValue(record, eid, param="fitstat_residavg_weighted", result.residWeightAverage);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

            t->setFitResultValue(record, eid, param="fitstat_residstddev", result.residStdDev);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

            t->setFitResultValue(record, eid, param="fitstat_residstddev_weighted", result.residWeightStdDev);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

            t->setFitResultValue(record, eid, param="fitstat_fitparams", result.fitparamN);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("fit params"));

            t->setFitResultValue(record, eid, param="fitstat_datapoints", result.dataSize);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("datapoints"));

            t->setFitResultValue(record, eid, param="fitstat_dof", result.degFreedom);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("degrees of freedom"));

            t->setFitResultValue(record, eid, param="fitstat_r2", result.Rsquared);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("R squared"), QObject::tr("R<sup>2</sup>"));

            t->setFitResultValue(record, eid, param="fitstat_tss", result.TSS);
            t->setFitResultGroup(record, eid, param, QObject::tr("fit statistics"));
            t->setFitResultLabel(record, eid, param, QObject::tr("total sum of squares"));

        }
    }

    return result;
}

QFFitResultsByIndexEvaluationFitToolsBase::QFFitResultsByIndexEvaluationFitToolsBase()
{
    mutexThreadedFit=new QMutex(QMutex::Recursive);
}

QFFitResultsByIndexEvaluationFitToolsBase::~QFFitResultsByIndexEvaluationFitToolsBase()
{
    delete mutexThreadedFit;
}

