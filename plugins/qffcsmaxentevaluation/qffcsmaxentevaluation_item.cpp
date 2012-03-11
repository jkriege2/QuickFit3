#include "qffcsmaxentevaluation_item.h"
#include "qffcsmaxentevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qfmathtools.h"
#include "libb040mem.h"


#define sqr(x) ((x)*(x))

QFFCSMaxEntEvaluationItem::QFFCSMaxEntEvaluationItem(QFProject* parent):
    QFUsesResultsByIndexAndModelEvaluation(parent, true, false)
{
    currentIndex=-1;
    currentModel=0;
    currentWeights=0;
}

QFFCSMaxEntEvaluationItem::~QFFCSMaxEntEvaluationItem() {

}

QString QFFCSMaxEntEvaluationItem::getEvaluationResultID(int currentIndex, int model) const {
    if (currentIndex<0) return QString("%1_%2_m%3_runavg").arg(getType()).arg(getID()).arg(model);
    return QString("%1_%2_m%3_run%4").arg(getType()).arg(getID()).arg(model).arg(currentIndex);
}



void QFFCSMaxEntEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    QFUsesResultsByIndexAndModelEvaluation::intWriteData(w);
    w.writeStartElement("maxent_config");
    w.writeAttribute("current_weights", QString::number(currentWeights));
    w.writeEndElement();

}

void QFFCSMaxEntEvaluationItem::intReadData(QDomElement* e) {
    QFUsesResultsByIndexAndModelEvaluation::intReadData(e);

    QDomElement e1=e->firstChildElement("maxent_config");
    currentWeights=e1.attribute("current_weights", "0").toInt();


}



QFEvaluationEditor* QFFCSMaxEntEvaluationItem::createEditor(QFPluginServices* services, QWidget* parent) {
    return new QFFCSMaxEntEvaluationEditor(services, parent);
}

bool QFFCSMaxEntEvaluationItem::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}


int QFFCSMaxEntEvaluationItem::getIndexMin(QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return -1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}

int QFFCSMaxEntEvaluationItem::getIndexMax(QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return data->getCorrelationRuns()-1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}



void QFFCSMaxEntEvaluationItem::setCurrentWeights(int index)
{
    currentWeights=index;
}





QString QFFCSMaxEntEvaluationItem::getCurrentModelName() const
{
    return getModelName(getCurrentModel());
}



int QFFCSMaxEntEvaluationItem::getCurrentWeights() const
{
    return currentWeights;
}

void QFFCSMaxEntEvaluationItem::setAlpha(double alpha) {
    setFitValue("maxent_alpha", alpha);
}

double QFFCSMaxEntEvaluationItem::getAlpha() const {
    return getFitValue("maxent_alpha");
}



/*
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
void QFFCSMaxEntEvaluationItem::setNdist(uint32_t Ndist) {
    setFitValue("maxent_Ndist", Ndist);
}

double QFFCSMaxEntEvaluationItem::getNdist() const {
    return getFitValue("maxent_Ndist");
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
*/






























/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////

double* QFFCSMaxEntEvaluationItem::allocWeights(bool* weightsOKK, QFRawDataRecord* record_in, int run_in, int data_start, int data_end) const {
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


QVector<double> QFFCSMaxEntEvaluationItem::getDistribution(QFRawDataRecord *record, int index, int model) const {
    QVector<double> res;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        res=getFitValueNumberArray(record, index, "maxent_distribution");
    }
    return res;
}

QVector<double> QFFCSMaxEntEvaluationItem::getDistributionTaus(QFRawDataRecord *record, int index, int model) const {
    QVector<double> res;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        res=getFitValueNumberArray(record, index, "maxent_tau");
    }
    return res;
}




QFFitStatistics QFFCSMaxEntEvaluationItem::calcFitStatistics(QFRawDataRecord *record, int index, int model, double *taus, double* modelValsIn, uint32_t N, uint32_t MaxEntParams, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {

        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);

        double* modelVals=modelValsIn;
        bool freeModelVals=false;
        if (!modelVals) {
            modelVals=(double*)malloc(N*sizeof(double));
            QVector<double> dist=getDistribution(record, index, model);
            QVector<double> distTau=getDistributionTaus(record, index, model);
            evaluateModel(record, index, model, taus, modelVals, N, distTau.data(), dist.data(), dist.size());
            freeModelVals=true;
        }
        //double* modelVals=(double*)malloc(N*sizeof(double));
        //evaluateModel(record, index, model, taus, modelVals, N);
        double* weights=allocWeights(NULL, record, index);

        QFFitStatistics result= calculateFitStatistics(N, taus, modelVals, corrdata, weights, datacut_min, datacut_max, MaxEntParams, runAvgWidth, residualHistogramBins);

        if (record) {
            if (hasResults(record, index)) {
                QString param="";
                setFitResultValue(record, index, model, param="fitstat_chisquared", result.residSqrSum);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font>"));

                setFitResultValue(record, index,model,  param="fitstat_chisquared_weighted", result.residWeightSqrSum);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("weighted chi squared"), QString("<font size=\"+2\">&chi;<sup>2</sup></font> (weighted)"));

                setFitResultValue(record, index, model, param="fitstat_residavg", result.residAverage);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("residual average"), QString("&lang;E&rang;"));

                setFitResultValue(record, index, model, param="fitstat_residavg_weighted", result.residWeightAverage);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("weighted residual average"), QString("&lang;E&rang; (weighted)"));

                setFitResultValue(record, index, model, param="fitstat_residstddev", result.residStdDev);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; "));

                setFitResultValue(record, index, model, param="fitstat_residstddev_weighted", result.residWeightStdDev);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("weighted residual stddev"), QString("&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;  (weighted)"));

                setFitResultValue(record, index, model, param="fitstat_fitparams", result.fitparamN);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("fit params"));

                setFitResultValue(record, index, model, param="fitstat_datapoints", result.dataSize);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("datapoints"));

                setFitResultValue(record, index, model, param="fitstat_dof", result.degFreedom);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("degrees of freedom"));

                setFitResultValue(record, index, model, param="fitstat_r2", result.Rsquared);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("R squared"), tr("R<sup>2</sup>"));

                setFitResultValue(record, index, model, param="fitstat_tss", result.TSS);
                setFitResultGroup(record, index, model, param, tr("fit statistics"));
                setFitResultLabel(record, index, model, param, tr("total sum of squares"));

            }
        }

        if (freeModelVals) free(modelVals);

        return result;
    }
    return QFFitStatistics();

}

















////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENT THE FIT MODEL, THE FIT AND ALL THE PARAMETERS REQUIRED FOR THOSE
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QFFCSMaxEntEvaluationItem::evaluateModel(QFRawDataRecord *record, int index, int model, double *taus, double *output, uint32_t N, double* distribution_tau, double* distribution, uint32_t distributionN) const {
    if (!taus || !output) return;
    //qDebug()<<"evalModel(N="<<N<<"  distributionN="<<distributionN<<")";
    if (model==0) { // 3D diffusion model

        // first we read the stored fit parameters:
        double gamma=getFitValue(record, index, model, "focus_struct_fac");
        double trip_tau=getFitValue(record, index, model, "trip_tau")*1e-6; // stored in microseconds!!!
        double trip_theta=getFitValue(record, index, model, "trip_theta");
        double N_particle=1;

        // now we evaluate the model
        for (uint32_t i=0; i<N; i++) {
            double trip_factor=(1.0-trip_theta+trip_theta*exp(-taus[i]/trip_tau))/(1.0-trip_theta);
            //output[i]=trip_factor*1.0/N/(1+taus[i]/100e-6)/sqrt(1+taus[i]/100e-6/sqr(gamma));
            register double sum=0;
            if (distribution_tau && distribution && distributionN>0) {
                for (register uint32_t j=0; j<distributionN; j++) {
                    sum=sum+distribution[j]/(1+taus[i]/distribution_tau[j])/sqrt(1+taus[i]/distribution_tau[j]/sqr(gamma));

                }
            } else {
                sum=1.0;
            }
            output[i]=sum/N_particle*trip_factor;
        }
    }
}

QString QFFCSMaxEntEvaluationItem::getModelName(int model) const {
    if (model==0) return tr("FCS: 3D diffusion");
    return "";
}

bool QFFCSMaxEntEvaluationItem::getParameterDefault(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QFUsesResultsEvaluation::FitParameterDefault &defaultValue) const {
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
       /* if (parameterID=="n_particle") {
            defaultValue.value=8;
            return true;
        }*/
    }

    if (parameterID=="maxent_alpha") {
        defaultValue.value=0.001;
        return true;
    }

    return false;
}


void QFFCSMaxEntEvaluationItem::doFit(QFRawDataRecord* record, int index, int model, int defaultMinDatarange, int defaultMaxDatarange, int runAvgWidth, int residualHistogramBins) {
    bool doEmit=record->isEmitResultsChangedEnabled();
    bool thisDoEmitResults=get_doEmitResultsChanged();
    bool thisDoEmitProps=get_doEmitResultsChanged();
    set_doEmitResultsChanged(false);
    set_doEmitPropertiesChanged(false);
    record->disableEmitResultsChanged();


    /* IMPLEMENT THIS

      Ergebnisse können einfach mit einer der setFitResult... Methoden gespeichert werden:

        //                                          PARAMETERNAME           WERT
        setFitResultValueBool(record, index, model, "evaluation_completed", true);
        //                                      PARAMETERNAME   WERT   FEHLER und EINHEIT (z.B. "ms") dazu
        setFitResultValue(record, index, model, "my_parameter", value, error, unit);

    */

    // HERE IS A DUMMY IMPLEMENTATION THAT OUTPUTS A SIMPLE GAUSSIAN
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        getProject()->getServices()->log_text(tr("running MaxEnt fit with model '%1' on raw data record '%2', run %3 ... \n").arg(getModelName(model)).arg(record->getName()).arg(index));
        // which datapoints should we actually use?
        int rangeMinDatarange=0;
        int rangeMaxDatarange=data->getCorrelationN();
        if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
        if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;
        getProject()->getServices()->log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(defaultMinDatarange).arg(defaultMaxDatarange).arg(defaultMaxDatarange-defaultMinDatarange));


        //////////
        uint32_t Ndist=100; // default
        //////////

        uint32_t N=data->getCorrelationN();
        double* taus=data->getCorrelationT();
        double* distTaus=(double*)calloc(Ndist,sizeof(double));
        double* dist=(double*)calloc(Ndist,sizeof(double));
        double* modelEval=(double*)malloc(N*sizeof(double));
        bool weightsOK=false;
        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);
        double* weights=allocWeights(&weightsOK, record, index, rangeMinDatarange, rangeMaxDatarange);
        if (!weightsOK) getProject()->getServices()->log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));
        double alpha=getAlpha();
        bool fitSuccess=false;

        double kappa=getFitValue("focus_struct_fac");
        double tripTau=getFitValue("trip_tau")*1e-6;
        double tripTheta=getFitValue("trip_theta");

        QElapsedTimer time;
        time.start();

        /////////////////////////////////////////////////////////
        /// MaxEnt Implementation ///////////////////////////////
        /////////////////////////////////////////////////////////
        MaxEntB040 mem;
        mem.setData(taus,corrdata,weights,N,rangeMinDatarange,rangeMaxDatarange);
        mem.run(alpha,kappa,tripTau,tripTheta);
        mem.setDistTaus(distTaus);
        mem.setDistribution(dist);


        // reset all NAN to 0
        for (unsigned int i=0; i<Ndist; i++) {
            double d=dist[i];
            if (!QFFloatIsOK(dist[i])) dist[i]=0;
            qDebug()<<distTaus[i]<<" ,\t"<<d<<" ,\t"<<dist[i];
        }

        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
/*
        // REPLACE THIS WITH THE ACTUAL MAXENT FIT!!!
        double T0=1e-4;
        double sigma=2e-5;
        double dSum=0;
        for (int i=rangeMinDatarange; i<rangeMaxDatarange; i++) {
            const double t=taus[i];
            dist[i]=exp(-0.5*sqr(t-T0)/sqr(sigma));
            dSum=dSum+dist[i];
        }
        for (int i=rangeMinDatarange; i<rangeMaxDatarange; i++) {
            dist[i]=dist[i]/dSum;
        }
        fitSuccess=true;
        // FIT CODE COMPLETE
*/

        // duration measurement
        double duration=double(time.elapsed())/1000.0;

        getProject()->getServices()->log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(duration).arg(fitSuccess?tr("success"):tr("no convergence")));

        // now store the results:
        QString param;
        setFitResultValueNumberArray(record, index, model, param="maxent_tau", distTaus, Ndist, QString("seconds"));
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt distribution: lag times"), QString("MaxEnt distribution: lag times <i>&tau;</i>"));
        setFitResultSortPriority(record, index, model, param, true);

        setFitResultValueNumberArray(record, index, model, param="maxent_distribution", dist, Ndist);
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt distribution"), QString("MaxEnt distribution: <i>p(&tau;)</i>"));
        setFitResultSortPriority(record, index, model, param, true);



        // save all the default values for all fit parameters as results.
        for (int i=0; i<getParameterCount(model); i++) {
            param=getParameterID(model, i);
            double value=getFitValue(record, index, model, param);
            //qDebug()<<"model param "<<i<<": "<<param<<" = "<<value;
            setFitResultValue(record, index, model, param, value, getParameterUnit(model, i, false));
            setFitResultGroup(record, index, model, param, tr("fit results"));
            setFitResultLabel(record, index, model, param, getParameterName(model, i, false), getParameterName(model, i, true));
            setFitResultSortPriority(record, index, model, param, true);
            //qDebug()<<"model param "<<i<<": "<<param<<" = "<< getFitValue(record, index, model, param)<<getParameterUnit(model, i, false);
        }

        // you con overwrite certain of these parameters using code like this:
        /*setFitResultValue(record, index, model, param="focus_struct_fac", getFitValue(record, index, model, param));
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("focus structure factor"), QString("focus structure factor <i>&gamma;</i>"));
        setFitResultSortPriority(record, index, model, param, true);*/




        // store number of iterations ... you may also store more fit algorithm properties like this
        setFitResultValue(record, index, model, param="maxent_alpha", alpha);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MaxEnt scaling parameter alpha"), tr("scaling parameter &alpha;"));

        setFitResultValueInt(record, index, model, param="maxent_iterations", 5);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("number of MaxEnt iterations"), tr("number of MaxEnt iterations"));

        setFitResultValueString(record, index, model, param="used_model", getModelName(model));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used model name"), tr("used model name"));

        setFitResultValueInt(record, index, model, param="used_model_id", model);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used model id"), tr("used model id"));

        setFitResultValueInt(record, index, model, param="used_run", index);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used run"), tr("used run"));

        setFitResultValue(record, index, model, param="runtime", duration, tr("seconds"));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("fit runtime"), tr("fit runtime"));

        // CALCULATE FIT STATISTICS
        //   now we evaluate the model for the given distribution
        evaluateModel(record, index, model, taus, modelEval, N, &(taus[rangeMinDatarange]), &(dist[rangeMinDatarange]), rangeMaxDatarange-rangeMinDatarange);
        //   then we can call calcFitStatistics()
        QFFitStatistics fit_stat=calcFitStatistics(record, index, model, taus, corrdata, N, rangeMaxDatarange-rangeMinDatarange, rangeMinDatarange, rangeMaxDatarange, runAvgWidth, residualHistogramBins);
        //   finally we have to free the memory allocated in the calcFitStatistics() result.
        fit_stat.free();

        free(dist);
        free(weights);
        free(modelEval);
        free(distTaus);

    }

    if (doEmit) record->enableEmitResultsChanged(true);
    set_doEmitResultsChanged(thisDoEmitResults);
    set_doEmitPropertiesChanged(thisDoEmitProps);
}


QString QFFCSMaxEntEvaluationItem::getParameterName(int model, int id, bool html) const {
    if (model==0) {
        if (id==0) return (html)?tr("triplet decay time &tau;<sub>T</sub> [&mu;s]"):tr("triplet decay time [microseconds]");
        if (id==1) return (html)?tr("triplet fraction &theta;<sub>T</sub> [0..1]"):tr("triplet fraction [0..1]");
        if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
    }
    return QString();
}

QString QFFCSMaxEntEvaluationItem::getParameterUnit(int model, int id, bool html) const {
    if (model==0) {
        if (id==0) return (html)?tr("&mu;s"):tr("microseconds");
        if (id==1) return QString("");
        if (id==2) return QString("");
    }
    return QString();
}
int QFFCSMaxEntEvaluationItem::getParameterCount(int model) const {
    if (model==0) return 3;
    return 0;
}

QString QFFCSMaxEntEvaluationItem::getParameterID(int model, int id) const {
    if (model==0) {
        if (id==0) return tr("trip_tau");
        if (id==1) return tr("trip_theta");
        if (id==2) return tr("focus_struct_fac");
    }
    return QString("m%1_p%2").arg(model).arg(id);
}

int QFFCSMaxEntEvaluationItem::getModelCount(QFRawDataRecord *r, int index) const {
    return 1;
}
