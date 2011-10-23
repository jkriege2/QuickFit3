#include "qfpfitfunctionsfcsdistribution.h"

QStringList QFPFitFunctionsFCSDistribution::getIDs() const {
    QStringList res;
    //res<<"fcs_dist_lognorm";
    return res;
}

QFFitFunction* QFPFitFunctionsFCSDistribution::get(QString id, QObject* parent) const  {
    if (id=="fcs_dist_lognorm") {
        return new QFFitFunctionFCSDistributionLogGaussian();
    } 
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_fcsdistribution, QFPFitFunctionsFCSDistribution)
