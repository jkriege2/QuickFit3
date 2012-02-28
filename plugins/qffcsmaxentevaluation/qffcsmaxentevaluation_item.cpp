#include "qffcsmaxentevaluation_item.h"
#include "qffcsmaxentevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qfmathtools.h"

#define sqr(x) ((x)*(x))

QFFCSMaxEntEvaluationItem::QFFCSMaxEntEvaluationItem(QFProject* parent):
    QFUsesResultsByIndexEvaluation(parent, true, false)
{
    currentIndex=-1;
    currentModel=0;
    currentWeights=0;
}

QFFCSMaxEntEvaluationItem::~QFFCSMaxEntEvaluationItem() {

}

QString QFFCSMaxEntEvaluationItem::getEvaluationResultID(int currentIndex) {
    if (currentIndex<0) return QString("%1_%2_m%3_runavg").arg(getType()).arg(getID()).arg(getCurrentModelName());
    return QString("%1_%2_m%3_run%4").arg(getType()).arg(getID()).arg(getCurrentModelName()).arg(currentIndex);
}


void QFFCSMaxEntEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    QFUsesResultsByIndexEvaluation::intWriteData(w);
    w.writeStartElement("maxent_config");
    w.writeAttribute("current_index", QString::number(currentIndex));
    w.writeAttribute("current_model", QString::number(currentModel));
    w.writeAttribute("current_weights", QString::number(currentWeights));

}

void QFFCSMaxEntEvaluationItem::intReadData(QDomElement* e) {
    QFUsesResultsByIndexEvaluation::intReadData(e);

    QDomElement e1=e->firstChildElement("maxent_config");
    currentIndex=e1.attribute("current_index", "0").toInt();
    currentModel=e1.attribute("current_model", "0").toInt();
    currentWeights=e1.attribute("current_weights", "0").toInt();


}

bool QFFCSMaxEntEvaluationItem::getParameterDefault(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QFUsesResultsEvaluation::FitParameterDefault &defaultValue) {
    if (currentModel==0) {
        if (parameterID=="trip_tau") {
            defaultValue.value=3;
            return true;
        }
        if (parameterID=="trip_theta") {
            defaultValue.value=0.2;
            return true;
        }
        if (parameterID=="focus_struct_fac") {
            defaultValue.value=6;
            return true;
        }
    }

    if (parameterID=="alpha") {
        defaultValue.value=0.001;
        return true;
    }
    return false;
}


QFEvaluationEditor* QFFCSMaxEntEvaluationItem::createEditor(QFPluginServices* services, QWidget* parent) {
    return new QFFCSMaxEntEvaluationEditor(services, parent);
}

bool QFFCSMaxEntEvaluationItem::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}

void QFFCSMaxEntEvaluationItem::doEvaluation(QFRawDataRecord* record, int index) {
    record->disableEmitResultsChanged();

    setFitResultValueBool(record, index, "evaluation_completed", true);

    record->enableEmitResultsChanged(true);
}

void QFFCSMaxEntEvaluationItem::setCurrentWeights(int index)
{
    currentWeights=index;
}

void QFFCSMaxEntEvaluationItem::setCurrentIndex(int index) {
    currentIndex=index;
}

int QFFCSMaxEntEvaluationItem::getCurrentIndex() const {
    if (currentIndex<-1) return -1;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(getHighlightedRecord());
    if (data) {
        if (currentIndex>=data->getCorrelationRuns()) return data->getCorrelationRuns()-1;
    }
    return currentIndex;
}


void QFFCSMaxEntEvaluationItem::setCurrentModel(int index)
{
    currentModel=index;
}

int QFFCSMaxEntEvaluationItem::getCurrentModel() const
{
    return currentModel;
}



QString QFFCSMaxEntEvaluationItem::getParameterName(int model, int id) const {
    if (model==0) {
        if (id==0) return tr("triplet decay time [&mu;s]");
        if (id==1) return tr("triplet fraction [0..1]");
        if (id==1) return tr("axial ratio");
    }
    return QString();
}

int QFFCSMaxEntEvaluationItem::getParameterCount(int model) const {
    if (model==0) return 3;
    return 0;
}

QString QFFCSMaxEntEvaluationItem::getParameterID(int model, int id) {
    if (model==0) {
        if (id==0) return tr("trip_tau");
        if (id==1) return tr("trip_theta");
        if (id==1) return tr("focus_struct_fac");
    }
    return QString("m%1_p%2").arg(model).arg(id);
}

QString QFFCSMaxEntEvaluationItem::getCurrentModelName() const
{
    return getModelName(getCurrentModel());
}

QString QFFCSMaxEntEvaluationItem::getModelName(int model) const {
    if (model==0) return tr("FCS: 3D diffusion");
    return "";
}

int QFFCSMaxEntEvaluationItem::getModelCount() const
{
    return 1;
}

int QFFCSMaxEntEvaluationItem::getCurrentWeights() const
{
    return currentWeights;
}


































/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////

double* QFFCSMaxEntEvaluationItem::allocWeights(bool* weightsOKK, QFRawDataRecord* record_in, int run_in, int data_start, int data_end) {
    if (weightsOKK) *weightsOKK=false;
    QFRawDataRecord* record=record_in;
    if (!record_in) record=getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    //JKQTPdatastore* ds=pltData->getDatastore();
    //JKQTPdatastore* dsres=pltResiduals->getDatastore();
    //QFFitFunction* ffunc=getFitFunction();
    int run=run_in;
    if (run<=-100) run=getCurrentIndex();

    int N=data->getCorrelationN();


    double* weights=(double*)malloc(N*sizeof(double));
    bool weightsOK=false;
    if (currentWeights==1) {
        double* std=data->getCorrelationStdDev();
        weightsOK=true;
        for (int i=0; i<N; i++) {
            weights[i]=std[i];
            if ((data_start>=0) && (data_end>=0)) {
                if ((i>=data_start)&&(i<=data_end)) {
                    if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                        weightsOK=false;
                        break;
                    }
                };
            } else {
                if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                    weightsOK=false;
                    break;
                };
            }
        }
    }
    if (currentWeights==2) {
        double* std=NULL;
        if (run>=0) std=data->getCorrelationRunError(run);
        else std=data->getCorrelationStdDev();
        weightsOK=true;
        for (int i=0; i<N; i++) {
            weights[i]=std[i];
            if ((data_start>=0) && (data_end>=0)) {
                if ((i>=data_start)&&(i<=data_end)) {
                    if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                        weightsOK=false;
                        break;
                    }
                };
            } else {
                if ((fabs(weights[i])<10000*DBL_MIN)||(!QFFloatIsOK(weights[i]))) {
                    weightsOK=false;
                    break;
                };
            }
        }
    }
    if (!weightsOK) {
        for (int i=0; i<N; i++) weights[i]=1;
        if (currentWeights==0) weightsOK=true;
    }
    if (weightsOKK) *weightsOKK=weightsOK;
    return weights;
}

void QFFCSMaxEntEvaluationItem::evaluateModel(QFRawDataRecord *record, int index, int model, double *taus, double *output, uint32_t N)
{
    for (int i=0; i<N; i++) {
        output[i]=1.0/8.0/(1+taus[i]/100e-6)/sqrt(1+taus[i]/100e-6/sqr(6.0));
    }
}

void QFFCSMaxEntEvaluationItem::getDistributionCopy(QFRawDataRecord *record, int index, int model, double *taus, double *dist) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        uint32_t N=getDistributionN(record, index, model);
        memcpy(taus, data->getCorrelationT(), N*sizeof(double));
        double T0=taus[N/5];
        double sigma=2*T0;
        for (int i=0; i<N; i++) {
            const double t=taus[i];
            dist[i]=exp(-0.5*sqr(t-T0)/sqr(sigma))/sigma/sqrt(2.0*M_PI);
        }
    }
}

uint32_t QFFCSMaxEntEvaluationItem::getDistributionN(QFRawDataRecord *record, int index, int model) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        return data->getCorrelationN();
    }
    return 0;
}



QFFitStatistics QFFCSMaxEntEvaluationItem::calcFitStatistics(QFRawDataRecord *record, int index, int model, double *taus, uint32_t N, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {

        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);

        double* modelVals=(double*)malloc(N*sizeof(double));
        evaluateModel(record, index, model, taus, modelVals, N);
        double* weights=allocWeights(NULL, record, index);

        QFFitStatistics result= calculateFitStatistics(N, taus, modelVals, corrdata, weights, datacut_min, datacut_max, getDistributionN(record, index, model), runAvgWidth, residualHistogramBins);

        if (record) {
            if (hasResults(record, index)) {
                QString param="";
                setFitResultValue(record, index, param="fitstat_chisquared", result.residSqrSum);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

                setFitResultValue(record, index, param="fitstat_chisquared_weighted", result.residWeightSqrSum);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

                setFitResultValue(record, index, param="fitstat_residavg", result.residAverage);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("residual average"), QString("&lang;E&rang;"));

                setFitResultValue(record, index, param="fitstat_residavg_weighted", result.residWeightAverage);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

                setFitResultValue(record, index, param="fitstat_residstddev", result.residStdDev);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

                setFitResultValue(record, index, param="fitstat_residstddev_weighted", result.residWeightStdDev);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

                setFitResultValue(record, index, param="fitstat_fitparams", result.fitparamN);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("fit params"));

                setFitResultValue(record, index, param="fitstat_datapoints", result.dataSize);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("datapoints"));

                setFitResultValue(record, index, param="fitstat_dof", result.degFreedom);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("degrees of freedom"));

                setFitResultValue(record, index, param="fitstat_r2", result.Rsquared);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("R squared"), tr("R<sup>2</sup>"));

                setFitResultValue(record, index, param="fitstat_tss", result.TSS);
                setFitResultGroup(record, index, param, tr("fit statistics"));
                setFitResultLabel(record, index, param, tr("total sum of squares"));

            }
        }

        return result;
    }
    return QFFitStatistics();

}




