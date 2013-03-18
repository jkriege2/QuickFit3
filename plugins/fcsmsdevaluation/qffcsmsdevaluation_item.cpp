#include "qffcsmsdevaluation_item.h"
#include "qffcsmsdevaluation_editor.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qfmathtools.h"



#define sqr(x) ((x)*(x))

QFFCSMSDEvaluationItem::QFFCSMSDEvaluationItem(QFProject* parent):
    QFUsesResultsByIndexAndModelEvaluation(parent, true, false)
{
    currentIndex=-1;
    currentModel=1;
    currentWeights=0;
}

QFFCSMSDEvaluationItem::~QFFCSMSDEvaluationItem() {

}

QString QFFCSMSDEvaluationItem::getEvaluationResultID(int currentIndex, int model) const {
    if (currentIndex<0) return QString("%1_%2_m%3_runavg").arg(getType()).arg(getID()).arg(model);
    return QString("%1_%2_m%3_run%4").arg(getType()).arg(getID()).arg(model).arg(currentIndex);
}



void QFFCSMSDEvaluationItem::intWriteData(QXmlStreamWriter& w) {
    QFUsesResultsByIndexAndModelEvaluation::intWriteData(w);
    w.writeStartElement("msd_config");
    w.writeAttribute("current_weights", QString::number(currentWeights));
    w.writeEndElement();

}

void QFFCSMSDEvaluationItem::intReadData(QDomElement* e) {
    QFUsesResultsByIndexAndModelEvaluation::intReadData(e);

    QDomElement e1=e->firstChildElement("msd_config");
    currentWeights=e1.attribute("current_weights", "0").toInt();


}



QFEvaluationEditor* QFFCSMSDEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFFCSMSDEvaluationEditor(services, propEditor, parent);
}

bool QFFCSMSDEvaluationItem::isApplicable(QFRawDataRecord* record) {
    return record->inherits("QFRDRFCSDataInterface");
}


int QFFCSMSDEvaluationItem::getIndexMin(QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return -1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}

int QFFCSMSDEvaluationItem::getIndexMax(QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return data->getCorrelationRuns()-1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}



void QFFCSMSDEvaluationItem::setCurrentWeights(int index)
{
    currentWeights=index;
}





QString QFFCSMSDEvaluationItem::getCurrentModelName() const
{
    return getModelName(getCurrentModel());
}



int QFFCSMSDEvaluationItem::getCurrentWeights() const
{
    return currentWeights;
}

QStringList QFFCSMSDEvaluationItem::getFitTypes()
{
    QStringList sl;
    sl<<tr("LM line-fit");
    sl<<tr("linear regression");
    sl<<tr("robust regression (IRLS)");
    return sl;
}

void QFFCSMSDEvaluationItem::setTheory(int i, bool enabled, double pre, double D, double alpha) {
    setFitValue(QString("msd_theory%1_en").arg(i), (enabled)?1:0);
    setFitValue(QString("msd_theory%1_pre").arg(i), pre);
    setFitValue(QString("msd_theory%1_d").arg(i), D);
    setFitValue(QString("msd_theory%1_alpha").arg(i), alpha);
}

int QFFCSMSDEvaluationItem::getFitWidth() const {
    return getFitValue(QString("msd_fitwidth"));
}

bool QFFCSMSDEvaluationItem::getFitRangeLimited() const {
    return getFitValue(QString("msd_fitlimitedrange"));
}

void QFFCSMSDEvaluationItem::setFitRangeLimited(bool rangeLimit) {
    setFitValue(QString("msd_fitlimitedrange"), rangeLimit);
}

int QFFCSMSDEvaluationItem::getFitType() const
{
    return getFitValue(QString("msd_fittype"));
}

void QFFCSMSDEvaluationItem::setFitType(int type)
{
    setFitValue(QString("msd_fittype"), type);
}

void QFFCSMSDEvaluationItem::setFitWidth(int width) {
    setFitValue(QString("msd_fitwidth"), width);
}

bool QFFCSMSDEvaluationItem::getTheoryEnabled(int i) const {
    return getFitValue(QString("msd_theory%1_en").arg(i))!=0;
}

double QFFCSMSDEvaluationItem::getTheoryPre(int i) const {
    return getFitValue(getTheoryPreName(i));
}

QString QFFCSMSDEvaluationItem::getTheoryAlphaName(int i) const
{
    return QString("msd_theory%1_alpha").arg(i);
}

QString QFFCSMSDEvaluationItem::getTheoryDName(int i) const
{
    return QString("msd_theory%1_d").arg(i);
}

QString QFFCSMSDEvaluationItem::getTheoryPreName(int i) const
{
    return QString("msd_theory%1_pre").arg(i);
}

double QFFCSMSDEvaluationItem::getTheoryD(int i) const {
    return getFitValue(getTheoryDName(i));
}

double QFFCSMSDEvaluationItem::getTheoryAlpha(int i) const {
    return getFitValue(getTheoryAlphaName(i));
}





/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////

double* QFFCSMSDEvaluationItem::allocWeights(bool* weightsOKK, QFRawDataRecord* record_in, int run_in, int data_start, int data_end) const {
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


QVector<double> QFFCSMSDEvaluationItem::getMSD(QFRawDataRecord *record, int index, int model) const {
    QVector<double> res;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        res=getFitValueNumberArray(record, index, "msd");
    }
    return res;
}

QVector<double> QFFCSMSDEvaluationItem::getMSDTaus(QFRawDataRecord *record, int index, int model) const {
    QVector<double> res;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {
        res=getFitValueNumberArray(record, index, "msd_tau");
    }
    return res;
}

double QFFCSMSDEvaluationItemfMSD( double t, const double *p )
{
    const double D=p[0];
    const double a=p[1];
    return 6.0*D*pow(t, a);
}

double QFFCSMSDEvaluationItemfMSD_lin( double t, const double *p )
{
    const double D=p[0];
    const double a=p[1];
    return log(6.0*D)+a*log(t);
}

double QFFCSMSDEvaluationItemfMSD_D=0;
double QFFCSMSDEvaluationItemfMSD_alpha=0;

double QFFCSMSDEvaluationItemfMSD_linfixD( double t, const double *p )
{
    const double D=QFFCSMSDEvaluationItemfMSD_D;
    const double a=p[0];
    return log(6.0*D)+a*log(t);
}

double QFFCSMSDEvaluationItemfMSD_linfixalpha( double t, const double *p )
{
    const double D=p[0];
    const double a=QFFCSMSDEvaluationItemfMSD_alpha;
    return log(6.0*D)+a*log(t);
}


void QFFCSMSDEvaluationItem::calcMSDFit(double &alpha_out_param, bool fixAlpha, double &D_out_param, bool fixD, QFRawDataRecord *record, int index, int model, double prefactor, int range_min, int range_max, int fit_type) const
{
    double alpha_out=alpha_out_param;
    double D_out=D_out_param/prefactor*6.0;
    QVector<double> distTau=getMSDTaus(record, index, model);
    QVector<double> dist=getMSD(record, index, model);
    QVector<double> ldist, ltau;
    for (int i=0; i<dist.size(); i++) {
        ldist<<log(dist[i]);
        ltau<<log(distTau[i]);
    }

    if (distTau.size()>1 && dist.size()>1 ) {
        distTau=distTau.mid(range_min, range_max-range_min+1);
        dist=dist.mid(range_min, range_max-range_min+1);
        ldist=ldist.mid(range_min, range_max-range_min+1);
        ltau=ltau.mid(range_min, range_max-range_min+1);
        for (int i=ltau.size()-1; i>=0; i--) {
            if (!(QFFloatIsOK(ltau[i]) && QFFloatIsOK(ldist[i]) && QFFloatIsOK(dist[i]) && QFFloatIsOK(distTau[i]))) {
                distTau.remove(i);
                dist.remove(i);
                ldist.remove(i);
                ltau.remove(i);
            }
        }


        if (fit_type==0) { // lm fit

            if (!fixAlpha && !fixD) {
                double pout[2];
                int n_par = 2; // number of parameters
                pout[1]=alpha_out;
                pout[0]=D_out;
                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.maxcall=500;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, dist.size(), distTau.data(), ldist.data(), QFFCSMSDEvaluationItemfMSD_lin, &control, &status );
                alpha_out=pout[1];
                D_out=pout[0];
            } else if (fixAlpha && !fixD) {
                double pout[1];
                int n_par = 1; // number of parameters
                pout[0]=D_out;
                QFFCSMSDEvaluationItemfMSD_alpha=alpha_out;
                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.maxcall=500;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, dist.size(), distTau.data(), ldist.data(), QFFCSMSDEvaluationItemfMSD_linfixalpha, &control, &status );
                D_out=pout[0];
            } else if (!fixAlpha && fixD) {
                double pout[1];
                int n_par = 1; // number of parameters
                pout[0]=alpha_out;
                QFFCSMSDEvaluationItemfMSD_D=D_out;
                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.maxcall=500;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                lmcurve_fit( n_par, pout, dist.size(), distTau.data(), ldist.data(), QFFCSMSDEvaluationItemfMSD_linfixD, &control, &status );
                alpha_out=pout[0];
            }

        } else if (fit_type==1) { // simple regression
            double a=log(6.0*D_out), b=alpha_out;
            statisticsLinearRegression(ltau.data(), ldist.data(), ldist.size(), a, b, fixD, fixAlpha);
            D_out=(exp(a)/6.0);
            alpha_out=b;
        } else if (fit_type==2) { // robust regression
            double a=log(6.0*D_out), b=alpha_out;
            statisticsIterativelyReweightedLeastSquaresRegression(ltau.data(), ldist.data(), ldist.size(), a, b, 1.1, 200, fixD, fixAlpha);
            D_out=(exp(a)/6.0);
            alpha_out=b;
        }
    }
    if (QFFloatIsOK(D_out)) D_out_param=D_out*6.0/prefactor;
    if (QFFloatIsOK(alpha_out)) alpha_out_param=alpha_out;
}

void QFFCSMSDEvaluationItem::calcMSDFits(QVector<double> &taus_out, QVector<double> &alpha_out, QVector<double> &D_out, QFRawDataRecord *record, int index, int model, int evalWidth, int evalShift, int first, int fit_type, QVector<double>*tau_start, QVector<double>*tau_end) const
{
    QVector<double> distTau=getMSDTaus(record, index, model);
    QVector<double> dist=getMSD(record, index, model);
    taus_out.clear();
    alpha_out.clear();
    D_out.clear();
    if (distTau.size()>1 && dist.size()>1 && evalWidth>=3) {
        for (int i=first; i<=distTau.size()-1 ; i+=qMax(1,evalShift)) {
            double* t=(double*)calloc(evalWidth, sizeof(double));
            double* lt=(double*)calloc(evalWidth, sizeof(double));
            double* d=(double*)calloc(evalWidth, sizeof(double));
            int cnt=0;
            for (int j=i; j<=qMin(distTau.size()-1, i+evalWidth-1); j++) {
                t[cnt]=distTau[j];
                lt[cnt]=log(distTau[j]);
                d[cnt]=log(dist[j]);
                cnt++;
            }

            if (cnt>=evalWidth) {
                if (fit_type==0) { // lm fit

                    double pout[2];
                    int n_par = 2; // number of parameters
                    int m_dat = evalWidth; // number of data pairs
                    pout[1]=(d[0]-d[cnt-1])/(log(t[0])-log(t[cnt-1]));
                    pout[0]=(d[0]-pout[1]*log(t[0]))/6.0;
                    lm_status_struct status;
                    lm_control_struct control = lm_control_double;
                    control.maxcall=500;
                    control.printflags = 0; // monitor status (+1) and parameters (+2)
                    lmcurve_fit( n_par, pout, m_dat, t, d, QFFCSMSDEvaluationItemfMSD_lin, &control, &status );

                    if (QFFloatIsOK(pout[0]) && QFFloatIsOK(pout[1])) {
                        taus_out.append(t[cnt/2]);
                        D_out.append(pout[0]);
                        alpha_out.append(pout[1]);
                        if (tau_start) tau_start->append(t[0]);
                        if (tau_end) tau_end->append(t[evalWidth-1]);
                    }
                } else if (fit_type==1) { // simple regression
                    double a=0, b=0;
                    statisticsLinearRegression(lt, d, evalWidth, a, b);
                    if (QFFloatIsOK(exp(a)/6.0) && QFFloatIsOK(b)) {
                        taus_out.append(t[cnt/2]);
                        D_out.append(exp(a)/6.0);
                        alpha_out.append(b);
                        if (tau_start) tau_start->append(t[0]);
                        if (tau_end) tau_end->append(t[evalWidth-1]);
                    }
                } else if (fit_type==2) { // robust regression
                    double a=0, b=0;
                    statisticsIterativelyReweightedLeastSquaresRegression(lt, d, evalWidth, a, b, 1.1, 100);
                    if (QFFloatIsOK(exp(a)/6.0) && QFFloatIsOK(b)) {
                        taus_out.append(t[cnt/2]);
                        D_out.append(exp(a)/6.0);
                        alpha_out.append(b);
                        if (tau_start) tau_start->append(t[0]);
                        if (tau_end) tau_end->append(t[evalWidth-1]);
                    }
                }


            }

            free(lt);
            free(t);
            free(d);
        }
    }
}





QFFitStatistics QFFCSMSDEvaluationItem::calcFitStatistics(QFRawDataRecord *record, int index, int model, double *taus, double* modelValsIn, uint32_t N, uint32_t MaxEntParams, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins) {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {

        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);

        double* modelVals=modelValsIn;
        bool freeModelVals=false;
        if (!modelVals) {
            modelVals=(double*)malloc(N*sizeof(double));
            QVector<double> msd=getMSD(record, index, model);
            QVector<double> msdTau=getMSDTaus(record, index, model);
            evaluateModel(record, index, model, taus, modelVals, N, msdTau.data(), msd.data(), msd.size());
            freeModelVals=true;
        }
        //double* modelVals=(double*)malloc(N*sizeof(double));
        //
        //(record, index, model, taus, modelVals, N);
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
void QFFCSMSDEvaluationItem::evaluateModel(QFRawDataRecord *record, int index, int model, double *taus, double *modelEval, uint32_t N, double* msdTaus, double* msd, uint32_t Nmsd) const {
    if (!taus || !modelEval) return;
    //qDebug()<<"evalModel(N="<<N<<"  distributionN="<<distributionN<<")";

    bool ok=false;
    int first=-1, last=-1;
    double t_msdstart=0;
    double t_msdend=0;
    if (msdTaus && msd && Nmsd>1) {
        ok=true;
        t_msdstart=msdTaus[0];
        t_msdend=msdTaus[Nmsd-1];
        for (int i=0; i<N; i++) {
            if (taus[i]==t_msdstart) {
                first=i;
                break;
            }
        }
        for (int i=N-1; i>=0; i--) {
            if (taus[i]==t_msdend) {
                last=i;
                break;
            }
        }
        //qDebug()<<"" <<first<<" ... "<<last;
        if (first>0 && last>0) {
            int k=0;
            for (int i=first; i<=last; i++) {
                if (taus[i]!=msdTaus[k]) {
                    ok=false;
                    break;
                }
                k++;
            }
            //qDebug()<<first<<" ... "<<last<<"   ok="<<ok<<" @ k="<<k;
        } else {
            ok=false;
        }
    }
    for (uint32_t i=0; i<N; i++) {
        modelEval[i]=0;
    }

    if (ok) {
        if (model==0) { // simple 2D model without triplet

            // first we read the stored fit parameters:
            double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            double N_particle=getFitValue(record,index,model,"n_particle");;


            // now we evaluate the model
            if (msdTaus && msd && Nmsd>1) {

                int k=0;
                for (uint32_t i=first; i<=last; i++) {
                    const double tau=msdTaus[k];
                    const double ms=msd[k];
                    modelEval[i]=1.0/N_particle/(1.0+2.0/3.0*ms/sqr(wxy));
                    k++;
                }
            }
        } else if (model==1) { // simple 3D model without triplet

            // first we read the stored fit parameters:
            double gamma=getFitValue(record, index, model, "focus_struct_fac");
            double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            double N_particle=getFitValue(record,index,model,"n_particle");;

            int k=0;
            for (uint32_t i=first; i<=last; i++) {
                const double tau=msdTaus[k];
                const double ms=msd[k];
                modelEval[i]=1.0/N_particle/(1.0+2.0/3.0*ms/sqr(wxy))/sqrt(1.0+2.0/3.0*ms/sqr(wxy)/sqr(gamma));;
                k++;
            }

        }
    }
}





QString QFFCSMSDEvaluationItem::getModelName(int model) const {
    switch(model) {
        case 0: return tr("FCS: simple 2D model");
        case 1: return tr("FCS: simple 3D model");
        //case 2: return tr("FCS: simple 3D model as one optimization problem");
    }

    return "";
}


bool QFFCSMSDEvaluationItem::getParameterDefault(QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QFUsesResultsEvaluation::FitParameterDefault &defaultValue) const {

    for (int i=0; i<MSDTHEORYCOUNT; i++) {
        if (parameterID==QString("msd_theory%1_en").arg(i)) {
            defaultValue.value=0;            
            return true;
        }
        if (parameterID==QString("msd_theory%1_pre").arg(i)) {
            defaultValue.value=6;
            defaultValue.fix=true;
            return true;
        }
        if (parameterID==QString("msd_theory%1_d").arg(i)) {
            defaultValue.value=10;
            defaultValue.fix=false;
            return true;
        }
        if (parameterID==QString("msd_theory%1_alpha").arg(i)) {
            defaultValue.value=1;
            defaultValue.fix=false;
            if (i==1) defaultValue.value=0.75;
            if (i==2) defaultValue.value=0.5;
            if (i==3) defaultValue.value=0.66;
            return true;
        }
    }
    if (parameterID==QString("msd_fitwidth")) {
        defaultValue.value=10;
        return true;
    }
    if (parameterID==QString("msd_fittype")) {
        defaultValue.value=0;
        return true;
    }
    if (parameterID==QString("msd_fitlimitedrange")) {
        defaultValue.value=1;
        return true;
    }

    switch (currentModel) {
        case 0:
            if (parameterID=="n_particle") {
                defaultValue.value=1;
                return true;
                }
            if (parameterID=="focus_width") {
                defaultValue.value=250;
                return true;
            }
            break;
        case 1:
        //case 2:
            if (parameterID=="n_particle") {
                defaultValue.value=1;
                return true;
                }
            if (parameterID=="focus_width") {
                defaultValue.value=250;
                return true;
                }
            if (parameterID=="focus_struct_fac") {
                defaultValue.value=6;
                return true;
            }
            break;
     }


    return false;
}

struct msd_diff3d_params {
    double wxy;
    double gamma;
    double N;
    double gmeasured;
};


void lmfit_msd_diff3d(const double *par, int m_dat, const void *data, double *fvec, int *info) {
    msd_diff3d_params* p=(msd_diff3d_params*)data;


    const double wxy2 = sqr(p->wxy);
    const double gamma2 = sqr(p->gamma);
    const double N = p->N;
    const double gmeasured = p->gmeasured;
    const double x = par[0];

    const double f1=1.0+2.0/3.0*x/wxy2;
    const double f2=1.0+2.0/3.0*x/wxy2/gamma2;

    *fvec = f1*sqrt(f2)-1.0/(N*gmeasured);
}

struct msd_diff3dall_params {
    double wxy;
    double gamma;
    double N;
    double* gmeasured;
    int Nacf;
};


void lmfit_msd_diff3dall(const double *par, int m_dat, const void *data, double *fvec, int *info) {
    msd_diff3dall_params* p=(msd_diff3dall_params*)data;


    const double wxy2 = sqr(p->wxy);
    const double gamma2 = sqr(p->gamma);
    const double N = p->N;
    const double* gmeasured = p->gmeasured;
    const int Nacf=p->Nacf;


    for (int i=0; i<Nacf; i++) {
        const double x = par[i];
        const double f1=1.0+2.0/3.0*x/wxy2;
        const double f2=1.0+2.0/3.0*x/wxy2/gamma2;
        *fvec = f1*sqrt(f2)-1.0/(N*gmeasured[i]);
    }
}

void QFFCSMSDEvaluationItem::doFit(QFRawDataRecord* record, int index, int model, int defaultMinDatarange, int defaultMaxDatarange, int runAvgWidth, int residualHistogramBins) {
    bool doEmit=record->isEmitResultsChangedEnabled();
    bool thisDoEmitResults=get_doEmitResultsChanged();
    bool thisDoEmitProps=get_doEmitResultsChanged();
    set_doEmitResultsChanged(false);
    set_doEmitPropertiesChanged(false);
    record->disableEmitResultsChanged();

   // qDebug() << "START DEBUGGING 0: We enter the do fit Method with MODEL = " << model;



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
        getProject()->getServices()->log_text(tr("running MSD fit with model '%1' on raw data record '%2', run %3 ... \n").arg(getModelName(model)).arg(record->getName()).arg(index));
        // which datapoints should we actually use?
        int rangeMinDatarange=0;
        int rangeMaxDatarange=data->getCorrelationN();
        if (defaultMinDatarange>=0) rangeMinDatarange=defaultMinDatarange;
        if (defaultMaxDatarange>=0) rangeMaxDatarange=defaultMaxDatarange;
        getProject()->getServices()->log_text(tr("   - fit data range: %1...%2 (%3 datapoints)\n").arg(defaultMinDatarange).arg(defaultMaxDatarange).arg(defaultMaxDatarange-defaultMinDatarange));




        uint32_t N=data->getCorrelationN();
        double* taus=data->getCorrelationT();
        int Ndist=rangeMaxDatarange-rangeMinDatarange;
        double* distTaus=(double*)calloc(Ndist,sizeof(double));
        double* dist=(double*)calloc(Ndist,sizeof(double));
        double* modelEval=(double*)malloc(N*sizeof(double));
        bool weightsOK=false;
        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);
        double* weights=allocWeights(&weightsOK, record, index, rangeMinDatarange, rangeMaxDatarange);
        if (!weightsOK) getProject()->getServices()->log_warning(tr("   - weights have invalid values => setting all weights to 1\n"));

        bool fitSuccess=false;
        QString fitMessage, fitMessageHTML;


        //////////Load Model Parameters//////////////////////////////////////////////////////
        double gamma=getFitValue(record, index, model, "focus_struct_fac");
        double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
        double N_particle=getFitValue(record,index,model,"n_particle");
        QVector<double> msd=getMSD(record, index, model);
        QVector<double> msd_tau=getMSDTaus(record, index, model);


        QElapsedTimer time;
        time.start();

        /////////////////////////////////////////////////////////
        /// MSD Implementation ///////////////////////////////
        /////////////////////////////////////////////////////////
        if (model==0) {
            for (int i=0; i<Ndist; i++) {
                distTaus[i]=taus[rangeMinDatarange+i];
                dist[i]=3.0*sqr(wxy)/2.0*(1.0/N_particle/corrdata[rangeMinDatarange+i]-1.0);
            }
        } else if (model==1) {
            for (int i=0; i<Ndist; i++) {
                distTaus[i]=taus[rangeMinDatarange+i];
                const double meas_acf=corrdata[rangeMinDatarange+i];
                const double tau=taus[rangeMinDatarange+i];

                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)
                /*control.ftol=getParameter("ftol").toDouble();
                control.xtol=getParameter("xtol").toDouble();
                control.gtol=getParameter("gtol").toDouble();
                control.epsilon=getParameter("epsilon").toDouble();
                control.stepbound=getParameter("stepbound").toDouble();
                control.maxcall=getParameter("max_iterations").toInt();*/

                msd_diff3d_params d = {wxy, gamma, N_particle, meas_acf};
                double r=60.0*tau;
                if (msd.size()==Ndist && msd_tau.size()==Ndist && msd_tau[i]==distTaus[i]) {
                    r=msd[i];
                }

                lmmin(1, &r, 1, &d, lmfit_msd_diff3d, &control, &status, NULL );
                dist[i]=r;

            }
        /*} else if (model==2) {
            for (int i=0; i<Ndist; i++) {
                distTaus[i]=taus[rangeMinDatarange+i];
                if (msd.size()==Ndist && msd_tau.size()==Ndist && msd_tau[i]==distTaus[i]) {
                    dist[i]=msd[i];
                } else {
                    dist[i]=60.0*distTaus[i];
                }
            }


            lm_status_struct status;
            lm_control_struct control = lm_control_double;
            control.printflags = 0; // monitor status (+1) and parameters (+2)


            msd_diff3dall_params d = {wxy, gamma, N_particle, &corrdata[rangeMinDatarange], Ndist};


            lmmin(Ndist, dist, Ndist, &d, lmfit_msd_diff3dall, &control, &status, NULL );*/
        }

        fitSuccess=true;
        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////

        // duration measurement
        double duration=double(time.elapsed())/1000.0;

        getProject()->getServices()->log_text(tr("   - fit completed after %1 msecs with result %2\n").arg(duration).arg(fitSuccess?tr("success"):tr("error")));
        fitMessage=tr("MSD finished with %1").arg(fitSuccess?tr("success"):tr("an error"));
        fitMessageHTML=tr("<b>MSD finished with %1</b>").arg(fitSuccess?tr("success"):tr("an error"));

        // now store the results:
        QString param;
        setFitResultValueNumberArray(record, index, model, param="msd_tau", distTaus, Ndist, QString("seconds"));
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("MSD: lag times"), QString("MSD: lag times <i>&tau;</i>"));
        setFitResultSortPriority(record, index, model, param, true);

        setFitResultValueNumberArray(record, index, model, param="msd", dist, Ndist);
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("MSD"), QString("MSD: <i>&lang;r<sup>2</sup>(&tau;)&rang;</i>"));
        setFitResultSortPriority(record, index, model, param, true);



        // save all the default values for all fit parameters as results.
        for (int i=0; i<getParameterCount(model); i++) {
            param=getParameterID(model, i);
            double value=getFitValue(record, index, model, param);
            setFitResultValue(record, index, model, param, value, getParameterUnit(model, i, false));
            setFitResultGroup(record, index, model, param, tr("fit results"));
            setFitResultLabel(record, index, model, param, getParameterName(model, i, false), getParameterName(model, i, true));
            setFitResultSortPriority(record, index, model, param, true);
        }

        // you can overwrite certain of these parameters using code like this:
        /*setFitResultValue(record, index, model, param="focus_struct_fac", getFitValue(record, index, model, param));
        setFitResultGroup(record, index, model, param, tr("fit results"));
        setFitResultLabel(record, index, model, param, tr("focus structure factor"), QString("focus structure factor <i>&gamma;</i>"));
        setFitResultSortPriority(record, index, model, param, true);*/




        // store number of iterations ... you may also store more fit algorithm properties like this
        setFitResultValueString(record, index, model, param="used_model", getModelName(model));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used model name"), tr("used model name"));

        setFitResultValueInt(record, index, model, param="msd_N",Ndist);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MSD: number of lag times"), tr("MSD: number of lag times"));

        setFitResultValueInt(record, index, model, param="used_model_id", model);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used model id"), tr("used model id"));

        setFitResultValueInt(record, index, model, param="used_run", index);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("used run"), tr("used run"));

        setFitResultValue(record, index, model, param="runtime", duration, tr("seconds"));
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("fit runtime"), tr("fit runtime"));

        setFitResultValueString(record, index, model, param="fitalg_message", fitMessage);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MSD message"), tr("MSD message"));

        setFitResultValueString(record, index, model, param="fitalg_messageHTML", fitMessageHTML);
        setFitResultGroup(record, index, model, param, tr("fit properties"));
        setFitResultLabel(record, index, model, param, tr("MSD message"), tr("MSD message"));

        // CALCULATE FIT STATISTICS
        //   now we evaluate the model for the given distribution
        /////////////////////////
        /////////////////////////
        // I changed the 7th argument in the evaluateModel call from &(taus[rangeMindatarange]) to &(distTaus[rangeMinDatarange])
        // this is what the 7th and 8th argument used to be like: &(distTaus[rangeMinDatarange]), &(dist[rangeMinDatarange])
        // this was the last argument:  rangeMaxDatarange-rangeMinDatarange, now changed to Ndist
        ////////////////////////

        evaluateModel(record, index, model, taus, modelEval, N, distTaus,dist,Ndist);
        //   then we can call calcFitStatistics()
        QFFitStatistics fit_stat=calcFitStatistics(record, index, model, taus, corrdata, N,Ndist, rangeMinDatarange, rangeMaxDatarange, runAvgWidth, residualHistogramBins);
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


QString QFFCSMSDEvaluationItem::getParameterName(int model, int id, bool html) const {
    switch (model) {
        case 0: // simple 2D model
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("focus size w<sub>xy</sub> [nm]"):tr("focus size [nm]");
            break;
        case 1: // simple 3D model
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("focus size w<sub>xy</sub> [nm]"):tr("focus size [nm]");
            if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
            break;
        /*case 2: // simple 3D model, one optimization problem
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("focus size w<sub>xy</sub> [nm]"):tr("focus size [nm]");
            if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
            break;*/
    }
    return QString();
}


QString QFFCSMSDEvaluationItem::getParameterUnit(int model, int id, bool html) const {
    switch (model) {
        case 0:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
        case 1:
        //case 2:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
            if (id==2) return QString("");
    }
    return QString();
}
int QFFCSMSDEvaluationItem::getParameterCount(int model) const {
    switch (model) {
        case 0:
            return 2;
        case 1:
        //case 2:
            return 3;
    }
    return 0;
}

QString QFFCSMSDEvaluationItem::getParameterID(int model, int id) const {
    switch (model) {
        case 0:
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            break;
        case 1:
        //case 2:
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            if (id==2) return tr("focus_struct_fac");
            break;
    }
    return QString("m%1_p%2").arg(model).arg(id);
}

int QFFCSMSDEvaluationItem::getModelCount(QFRawDataRecord *r, int index) const {
    return 2;
}
