#include "qfpfitfunctionsfcsdistribution.h"
#include "qffitfunctionfcsdistributiongaussianD.h"
#include "qffitfunctionfcsdistributionloggaussian.h"
#include "qffitfunctionfcsdistributiongaussian.h"
#include "qffitfunctionfcsdistributiongaussianint.h"

QStringList QFPFitFunctionsFCSDistribution::getIDs() const {
    QStringList res;
    res<<"fcs_dist_norm"<<"fcs_dist_norm_d"<<"fcs_dist_lognorm";//<<"fcs_dist_int_norm";
    return res;
}

QFFitFunction* QFPFitFunctionsFCSDistribution::get(QString id, QObject* parent) const  {
    if (id=="fcs_dist_lognorm") {
        return new QFFitFunctionFCSDistributionLogGaussian();
    } else if (id=="fcs_dist_norm") {
            return new QFFitFunctionFCSDistributionGaussian();
    } else if (id=="fcs_dist_int_norm") {
            return new QFFitFunctionFCSDistributionIntGaussian();
    } else if (id=="fcs_dist_norm_d") {
        return new QFFitFunctionFCSDistributionGaussianD();
    }

    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunctions_fcsdistribution, QFPFitFunctionsFCSDistribution)
