/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qffcsmsdevaluation_item.h"
#include "qffcsmsdevaluation_editor.h"
#include "qfrdrfcsdatainterface.h"
#include "qfmathtools.h"
#include "qffcstools.h"
#include "imfcstools.h"


#define sqr(x) ((x)*(x))

QFFCSMSDEvaluationItem::QFFCSMSDEvaluationItem(QFProject* parent):
    QFUsesResultsByIndexAndModelEvaluation(parent, true, false)
{
    currentIndex=-1;
    currentModel=1;
}

QFFCSMSDEvaluationItem::~QFFCSMSDEvaluationItem() {

}

void QFFCSMSDEvaluationItem::setPresetProperty(const QString &id, const QVariant &data, bool usereditable, bool visible)
{
    QString uid=id.toUpper();
    QString lid=id.toLower();
    if (uid=="PRESET_GAMMA" || uid=="PRESET_FOCUS_STRUCT_FAC") {
        setInitFitValue("focus_struct_fac", data.toDouble());
    } else if (uid=="PRESET_FOCUS_WIDTH") {
        setInitFitValue("focus_width", data.toDouble());
    } else if (lid=="preset_focus_height") {
        setInitFitValue("focus_height", data.toDouble());
    } else if (lid=="preset_pixel_size") {
        setInitFitValue("pixel_size", data.toDouble());
    } else if (lid=="preset_n_particle") {
        setInitFitValue("n_particle", data.toDouble());
    } else if (lid=="preset_nonfl_theta1") {
        setInitFitValue("nonfl_theta1", data.toDouble());
    } else if (lid=="preset_nonfl_tau1") {
        setInitFitValue("nonfl_tau1", data.toDouble());
    } else if (lid=="preset_nonfl_theta2") {
        setInitFitValue("nonfl_theta2", data.toDouble());
    } else if (lid=="preset_nonfl_tau2") {
        setInitFitValue("nonfl_tau2", data.toDouble());
    } else {
        QFUsesResultsByIndexAndModelEvaluation::setPresetProperty(id, data, usereditable, visible);
    }
}

QString QFFCSMSDEvaluationItem::getEvaluationResultID(int currentIndex, int model) const {
    if (currentIndex<0) return QString("%1_%2_m%3_runavg").arg(getType()).arg(getID()).arg(model);
    return QString("%1_%2_m%3_run%4").arg(getType()).arg(getID()).arg(model).arg(currentIndex);
}



void QFFCSMSDEvaluationItem::intWriteData(QXmlStreamWriter& w) const {
    QFUsesResultsByIndexAndModelEvaluation::intWriteData(w);
    w.writeStartElement("msd_config");
    w.writeAttribute("current_weights", QString::number(m_weighting));
    w.writeEndElement();

}

void QFFCSMSDEvaluationItem::intReadData(QDomElement* e) {
    QFUsesResultsByIndexAndModelEvaluation::intReadData(e);

    QDomElement e1=e->firstChildElement("msd_config");
    m_weighting=indexToWeight( e1.attribute("current_weights", "0").toInt());


}



QFEvaluationEditor* QFFCSMSDEvaluationItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFFCSMSDEvaluationEditor(services, propEditor, parent);
}

bool QFFCSMSDEvaluationItem::isApplicable(const QFRawDataRecord *record) const {
    return record->inherits("QFRDRFCSDataInterface");
}


int QFFCSMSDEvaluationItem::getIndexMin(const QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return -1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}

int QFFCSMSDEvaluationItem::getIndexMax(const QFRawDataRecord *r) const {
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(r);
    if (data) {
        return data->getCorrelationRuns()-1;
    }
    return QFUsesResultsByIndexAndModelEvaluation::getIndexMin(r);
}








QString QFFCSMSDEvaluationItem::getCurrentModelName() const
{
    return getModelName(getCurrentModel());
}




QStringList QFFCSMSDEvaluationItem::getFitTypes()
{
    QStringList sl;
    sl<<tr("LM line-fit");
    sl<<tr("linear regression");
    sl<<tr("robust regression (IRLS)");
    return sl;
}

void QFFCSMSDEvaluationItem::setTheory(int i, bool enabled, double pre, double D, double alpha, QFRawDataRecord* record, int run) {
    //qDebug()<<getTheoryAlphaName(i, record, run)<<" = "<<alpha;
    setFitResultValue(record, run, getCurrentModel(), QString("msd_theory%1_en").arg(i), (enabled)?1:0);
    setFitResultValue(record, run, getCurrentModel(), getTheoryPreName(i, record, run), pre);
    setFitResultValue(record, run, getCurrentModel(), getTheoryDName(i, record, run), D);
    setFitResultValue(record, run, getCurrentModel(), getTheoryAlphaName(i, record, run), alpha);
    if (i>0) {
        setFitResultValue(record, run, getCurrentModel(), getTheoryIntersectName(i-1, record, run), getTheoryIntersect(i-1, record, run));
    }
}

int QFFCSMSDEvaluationItem::getFitWidth(QFRawDataRecord* record, int run) const {
    return getFitValue(record, run, getCurrentModel(), QString("msd_fitwidth"));
}

bool QFFCSMSDEvaluationItem::getFitRangeLimited(QFRawDataRecord* record, int run) const {
    return getFitValue(record, run, getCurrentModel(), QString("msd_fitlimitedrange"));
}

void QFFCSMSDEvaluationItem::setFitRangeLimited(bool rangeLimit, QFRawDataRecord* record, int run) {
    setFitValue(record, run, getCurrentModel(), QString("msd_fitlimitedrange"), rangeLimit);
}

int QFFCSMSDEvaluationItem::getFitType(QFRawDataRecord* record, int run) const
{
    return getFitValue(record, run, getCurrentModel(), QString("msd_fittype"));
}

void QFFCSMSDEvaluationItem::setFitType(int type, QFRawDataRecord* record, int run)
{
    setFitValue(record, run, getCurrentModel(), QString("msd_fittype"), type);
}



void QFFCSMSDEvaluationItem::setFitWidth(int width, QFRawDataRecord* record, int run) {
    setFitValue(record, run, getCurrentModel(), QString("msd_fitwidth"), width);
}

bool QFFCSMSDEvaluationItem::getTheoryEnabled(int i, QFRawDataRecord* record, int run) const {
    return getFitValue(record, run, getCurrentModel(), QString("msd_theory%1_en").arg(i))!=0;
}

double QFFCSMSDEvaluationItem::getTheoryPre(int i, QFRawDataRecord* record, int run) const {
    return getFitValue(record, run, getCurrentModel(), getTheoryPreName(i, record, run));
}

QString QFFCSMSDEvaluationItem::getTheoryAlphaName(int i, QFRawDataRecord* record, int run) const
{
    return QString("msd_theory%1_alpha").arg(i);
}

QString QFFCSMSDEvaluationItem::getTheoryDName(int i, QFRawDataRecord* record, int run) const
{
    return QString("msd_theory%1_d").arg(i);
}

QString QFFCSMSDEvaluationItem::getTheoryPreName(int i, QFRawDataRecord* record, int run) const
{
    return QString("msd_theory%1_pre").arg(i);
}

QString QFFCSMSDEvaluationItem::getTheoryIntersectName(int i, QFRawDataRecord *record, int run) const
{
    return QString("msd_theory%1_%2_intersect").arg(i).arg(i+1);
}

double QFFCSMSDEvaluationItem::getTheoryD(int i, QFRawDataRecord* record, int run) const {
    return getFitValue(record, run, getCurrentModel(), getTheoryDName(i, record, run));
}

double QFFCSMSDEvaluationItem::getTheoryAlpha(int i, QFRawDataRecord* record, int run) const {
    //qDebug()<<"read: "<<getTheoryAlphaName(i, record, run)<<" == "<<getFitValue(record, run, getCurrentModel(), getTheoryAlphaName(i, record, run));
    return getFitValue(record, run, getCurrentModel(), getTheoryAlphaName(i, record, run));
}


double QFFCSMSDEvaluationItem::getTheoryIntersect(int i, QFRawDataRecord *record, int run) const
{
    if (i<MSDTHEORYCOUNT-1) {
        double P1=getTheoryPre(i, record, run);
        double D1=getTheoryD(i, record, run);
        double a1=getTheoryAlpha(i, record, run);
        double P2=getTheoryPre(i+1, record, run);
        double D2=getTheoryD(i+1, record, run);
        double a2=getTheoryAlpha(i+1, record, run);
        return pow(P1*D1/P2/D2, 1.0/(a2-a1));
    }
    return 0;
}




/////////////////////////////////////////////////////////////////////
// FITTING AND READING DATA FOR FIT, FIT STATISTICS
/////////////////////////////////////////////////////////////////////



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
            double* t=(double*)qfCalloc(evalWidth, sizeof(double));
            double* lt=(double*)qfCalloc(evalWidth, sizeof(double));
            double* d=(double*)qfCalloc(evalWidth, sizeof(double));
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

            qfFree(lt);
            qfFree(t);
            qfFree(d);
        }
    }
}





QFFitStatistics QFFCSMSDEvaluationItem::calcFitStatistics(QFRawDataRecord *record, int index, int model, double *taus, double* modelValsIn, uint32_t N, uint32_t MaxEntParams, int datacut_min, int datacut_max, int runAvgWidth, int residualHistogramBins) {
    //qDebug()<<"calcFitStatistics model="<<model;
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);
    if (data) {

        double* corrdata=data->getCorrelationMean();
        if (index>=0) corrdata=data->getCorrelationRun(index);

        double* modelVals=modelValsIn;
        bool freeModelVals=false;
        if (!modelVals) {
            modelVals=(double*)qfMalloc(N*sizeof(double));
            QVector<double> msd=getMSD(record, index, model);
            QVector<double> msdTau=getMSDTaus(record, index, model);
            evaluateModel(record, index, model, taus, modelVals, N, msdTau.data(), msd.data(), msd.size());
            freeModelVals=true;
        }
        //double* modelVals=(double*)qfMalloc(N*sizeof(double));
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

        if (freeModelVals) qfFree(modelVals);

        return result;
    }
    return QFFitStatistics();

}

















////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENT THE FIT MODEL, THE FIT AND ALL THE PARAMETERS REQUIRED FOR THOSE
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QFFCSMSDEvaluationItem::evaluateModel(QFRawDataRecord *record, int index, int model, double *taus, double *modelEval, uint32_t N, double* msdTaus, double* msd, uint32_t Nmsd) const {
    if (!taus || !modelEval) return;
    //qDebug()<<"evalModel(model="<<model<<")";

    bool ok=false;
    int first=-1, last=-1;
    double t_msdstart=0;
    double t_msdend=0;
    if (msdTaus && msd && Nmsd>1) {
        ok=true;
        t_msdstart=msdTaus[0];
        t_msdend=msdTaus[Nmsd-1];
        for (uint32_t i=0; i<N; i++) {
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
        if (first>=0 && last>0) {
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
            const double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            const double N_particle=getFitValue(record,index,model,"n_particle");;


            // now we evaluate the model
            if (msdTaus && msd && Nmsd>1) {

                int k=0;
                for (int32_t i=first; i<=last; i++) {
                    const double tau=msdTaus[k];
                    const double ms=msd[k];
                    modelEval[i]=1.0/N_particle/(1.0+2.0/3.0*ms/sqr(wxy));
                    k++;
                }
            }
        } else if (model==1) { // simple 3D model without triplet

            // first we read the stored fit parameters:
            const double gamma=getFitValue(record, index, model, "focus_struct_fac");
            const double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            const double N_particle=getFitValue(record,index,model,"n_particle");;

            int k=0;
            for (int32_t i=first; i<=last; i++) {
                const double tau=msdTaus[k];
                const double ms=msd[k];
                modelEval[i]=1.0/N_particle/(1.0+2.0/3.0*ms/sqr(wxy))/sqrt(1.0+2.0/3.0*ms/sqr(wxy)/sqr(gamma));;
                k++;
            }

        } else if (model==2) { // simple 2D model with triplet-correction

            // first we read the stored fit parameters:
            const double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            const double N_particle=getFitValue(record,index,model,"n_particle");;
            const double fT=getFitValue(record,index,model,"nonfl_theta1");
            const double tauT=getFitValue(record,index,model,"nonfl_tau1")*1e-6;

            int k=0;
            for (int32_t i=first; i<=last; i++) {
                const double tau=msdTaus[k];
                const double ms=msd[k];
                modelEval[i]=1.0/N_particle/(1.0+2.0/3.0*ms/sqr(wxy))*qfFCSTripletTerm(tau, fT, tauT);
                k++;
            }

        } else if (model==3) { // simple 3D model with triplet-correction

            // first we read the stored fit parameters:
            const double gamma=getFitValue(record, index, model, "focus_struct_fac");
            const double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            const double N_particle=getFitValue(record,index,model,"n_particle");;
            const double fT=getFitValue(record,index,model,"nonfl_theta1");
            const double tauT=getFitValue(record,index,model,"nonfl_tau1")*1e-6;

            int k=0;
            for (int32_t i=first; i<=last; i++) {
                const double tau=msdTaus[k];
                const double ms=msd[k];
                modelEval[i]=1.0/N_particle/(1.0+2.0/3.0*ms/sqr(wxy))/sqrt(1.0+2.0/3.0*ms/sqr(wxy)/sqr(gamma))*qfFCSTripletTerm(tau, fT, tauT);
                k++;
            }
        } else if (model==4) { // SPIM-FCS simple 3D model

            // first we read the stored fit parameters:
            const double wz=getFitValue(record, index, model, "focus_height")/1.0e3;
            const double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            const double N_particle=getFitValue(record,index,model,"n_particle");;
            const double a=getFitValue(record,index,model,"pixel_size")/1.0e3;

            const double Veff=SPIMFCS_newVeff(a, wxy, wz);
            const double C=N_particle/Veff;
            const double sqpi=sqrt(M_PI);
            const double pre=1.0/(sqpi*wz*qfSqr(a)*C);

            int k=0;
            for (int32_t i=first; i<=last; i++) {
                const double tau=msdTaus[k];
                const double ms=msd[k];
                const double fourdt=2.0/3.0*ms;
                const double sqfourdtw=sqrt(fourdt+wxy*wxy);
                modelEval[i]=pre*qfSqr(sqfourdtw/(sqpi*a)*(exp(-qfSqr(a/sqfourdtw))-1.0)+erf(a/sqfourdtw))/sqrt(1.0+fourdt/qfSqr(wz));
                k++;
            }

        } else if (model==5) { // camera TIR-FCS simple 2D model

            // first we read the stored fit parameters:
            const double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            const double N_particle=getFitValue(record,index,model,"n_particle");;
            const double a=getFitValue(record,index,model,"pixel_size")/1.0e3;

            const double Veff=TIRFCS_newAeff(a, wxy);
            const double C=N_particle/Veff;
            const double sqpi=sqrt(M_PI);
            const double pre=1.0/(qfSqr(a)*C);

            int k=0;
            for (int32_t i=first; i<=last; i++) {
                const double tau=msdTaus[k];
                const double ms=msd[k];
                const double fourdt=2.0/3.0*ms;
                const double sqfourdtw=sqrt(fourdt+wxy*wxy);
                modelEval[i]=pre*qfSqr(sqfourdtw/(sqpi*a)*(exp(-qfSqr(a/sqfourdtw))-1.0)+erf(a/sqfourdtw));
                k++;
            }
        } else if (model==6) { // simple 3D model with triplet+darkstate-correction

            // first we read the stored fit parameters:
            const double gamma=getFitValue(record, index, model, "focus_struct_fac");
            const double wxy=getFitValue(record, index, model, "focus_width")/1.0e3;
            const double N_particle=getFitValue(record,index,model,"n_particle");;
            const double fT=getFitValue(record,index,model,"nonfl_theta1");
            const double tauT=getFitValue(record,index,model,"nonfl_tau1")*1e-6;
            const double fT2=getFitValue(record,index,model,"nonfl_theta2");
            const double tauT2=getFitValue(record,index,model,"nonfl_tau2")*1e-6;

            int k=0;
            for (int32_t i=first; i<=last; i++) {
                const double tau=msdTaus[k];
                const double ms=msd[k];
                modelEval[i]=1.0/N_particle/(1.0+2.0/3.0*ms/sqr(wxy))/sqrt(1.0+2.0/3.0*ms/sqr(wxy)/sqr(gamma))*qfFCSTripletBlinkTerm(tau, fT, tauT, fT2, tauT2);
                k++;
            }

        }
    }
}





QString QFFCSMSDEvaluationItem::getModelName(int model) const {
    switch(model) {
        case 0: return tr("FCS: simple 2D model");
        case 1: return tr("FCS: simple 3D model");
        case 2: return tr("FCS: simple 2D model + triplet correction");
        case 3: return tr("FCS: simple 3D model + triplet correction");
        case 4: return tr("camera SPIM-FCS: simple 3D model");
        case 5: return tr("camera TIR-FCS: simple 2D model");
        case 6: return tr("FCS: simple 3D model + triplet/blink correction");
        //case 2: return tr("FCS: simple 3D model as one optimization problem");
    }

    return "";
}


bool QFFCSMSDEvaluationItem::getParameterDefault(const QFRawDataRecord *r, const QString &resultID, const QString &parameterID, QFUsesResultsEvaluation::FitParameterDefault &defaultValue) const {

    for (int i=0; i<MSDTHEORYCOUNT; i++) {
        if (parameterID==QString("msd_theory%1_en").arg(i)) {
            defaultValue.value=(i==0)?1:0;
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
        defaultValue.value=25;
        return true;
    }
    if (parameterID==QString("msd_fittype")) {
        defaultValue.value=2;
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
        case 2:
        //case 2:
            if (parameterID=="n_particle") {
                defaultValue.value=1;
                return true;
                }
            if (parameterID=="focus_width") {
                defaultValue.value=250;
                return true;
                }
            if (parameterID=="nonfl_theta1") {
                defaultValue.value=0.2;
                return true;
            }
            if (parameterID=="nonfl_tau1") {
                defaultValue.value=3;
                return true;
            }
            break;
        case 3:
        //case 2:
            if (parameterID=="n_particle") {
                defaultValue.value=1;
                return true;
                }
            if (parameterID=="focus_width") {
                defaultValue.value=250;
                return true;
                }
            if (parameterID=="nonfl_theta1") {
                defaultValue.value=0.2;
                return true;
            }
            if (parameterID=="nonfl_tau1") {
                defaultValue.value=3;
                return true;
            }
            if (parameterID=="focus_struct_fac") {
                defaultValue.value=6;
                return true;
            }

            break;
        case 4:
        //case 2:
            if (parameterID=="n_particle") {
                defaultValue.value=1;
                return true;
                }
            if (parameterID=="focus_width") {
                defaultValue.value=600;
                return true;
                }
            if (parameterID=="focus_height") {
                defaultValue.value=1200;
                return true;
            }
            if (parameterID=="pixel_size") {
                defaultValue.value=400;
                return true;
            }
            break;
        case 5:
        //case 2:
            if (parameterID=="n_particle") {
                defaultValue.value=1;
                return true;
                }
            if (parameterID=="focus_width") {
                defaultValue.value=600;
                return true;
                }
            if (parameterID=="pixel_size") {
                defaultValue.value=400;
                return true;
            }
            break;
        case 6:
        //case 2:
            if (parameterID=="n_particle") {
                defaultValue.value=1;
                return true;
                }
            if (parameterID=="focus_width") {
                defaultValue.value=250;
                return true;
                }
            if (parameterID=="nonfl_theta1") {
                defaultValue.value=0.2;
                return true;
            }
            if (parameterID=="nonfl_tau1") {
                defaultValue.value=3;
                return true;
            }
            if (parameterID=="nonfl_theta2") {
                defaultValue.value=0.2;
                return true;
            }
            if (parameterID=="nonfl_tau2") {
                defaultValue.value=100;
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


struct msd_diff3dspim_params {
    double wxy;
    double wz;
    double N;
    double a;
    double gmeasured;
};
void lmfit_msd_SPIMdiff3dall(const double *par, int m_dat, const void *data, double *fvec, int *info) {
    msd_diff3dspim_params* p=(msd_diff3dspim_params*)data;


    const double wz=p->wz;
    const double wxy=p->wxy;
    const double N=p->N;
    const double a=p->a;

    const double Veff=SPIMFCS_newVeff(a, wxy, wz);
    const double C=N/Veff;
    const double sqpi=sqrt(M_PI);
    const double pre=1.0/(sqpi*wz*qfSqr(a)*C);

    const double gmeasured = p->gmeasured;

    const double ms=par[0];
    const double fourdt=2.0/3.0*ms;
    const double sqfourdtw=sqrt(fourdt+wxy*wxy);
    *fvec = pre*qfSqr(sqfourdtw/(sqpi*a)*(exp(-qfSqr(a/sqfourdtw))-1.0)+erf(a/sqfourdtw))/sqrt(1.0+fourdt/qfSqr(wz))-gmeasured;
}

void lmfit_msd_TIRdiff3dall(const double *par, int m_dat, const void *data, double *fvec, int *info) {
    msd_diff3dspim_params* p=(msd_diff3dspim_params*)data;


    const double wxy=p->wxy;
    const double N=p->N;
    const double a=p->a;

    const double Veff=TIRFCS_newAeff(a, wxy);
    const double C=N/Veff;
    const double sqpi=sqrt(M_PI);
    const double pre=1.0/(qfSqr(a)*C);

    const double gmeasured = p->gmeasured;

    const double ms=par[0];
    const double fourdt=2.0/3.0*ms;
    const double sqfourdtw=sqrt(fourdt+wxy*wxy);
    *fvec = pre*qfSqr(sqfourdtw/(sqpi*a)*(exp(-qfSqr(a/sqfourdtw))-1.0)+erf(a/sqfourdtw))-gmeasured;
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

      Ergebnisse k�nnen einfach mit einer der setFitResult... Methoden gespeichert werden:

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
        double* distTaus=(double*)qfCalloc(Ndist,sizeof(double));
        double* dist=(double*)qfCalloc(Ndist,sizeof(double));
        double* modelEval=(double*)qfMalloc(N*sizeof(double));
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
        double wz=getFitValue(record, index, model, "focus_height")/1.0e3;
        double a_pixel=getFitValue(record, index, model, "pixel_size")/1.0e3;
        double N_particle=getFitValue(record,index,model,"n_particle");
        double fT=0;
        double tauT=1e-10;
        double fT2=0;
        double tauT2=1e-10;
        if (model==2||model==3||model==6) {
            fT=getFitValue(record,index,model,"nonfl_theta1");
            tauT=getFitValue(record,index,model,"nonfl_tau1")*1e-6;
        }
        if (model==6) {
            fT2=getFitValue(record,index,model,"nonfl_theta2");
            tauT2=getFitValue(record,index,model,"nonfl_tau2")*1e-6;
        }
        QVector<double> msd=getMSD(record, index, model);
        QVector<double> msd_tau=getMSDTaus(record, index, model);


        QElapsedTimer time;
        time.start();

        /////////////////////////////////////////////////////////
        /// MSD Implementation ///////////////////////////////
        /////////////////////////////////////////////////////////
        if ((model==0)||(model==2)) {
            for (int i=0; i<Ndist; i++) {
                double meas_acf=corrdata[rangeMinDatarange+i];
                const double tau=taus[rangeMinDatarange+i];
                if (model==2) {
                    meas_acf=meas_acf/qfFCSTripletTerm(tau, fT, tauT);
                }
                distTaus[i]=tau;
                dist[i]=3.0*sqr(wxy)/2.0*(1.0/N_particle/meas_acf-1.0);
            }
        } else if ((model==1) || (model==3) || (model==6)) {
            for (int i=0; i<Ndist; i++) {
                distTaus[i]=taus[rangeMinDatarange+i];
                double meas_acf=corrdata[rangeMinDatarange+i];
                const double tau=taus[rangeMinDatarange+i];

                if (model==3) {
                    double t=meas_acf;
                    meas_acf=meas_acf/qfFCSTripletTerm(tau, fT, tauT);
                    //qDebug()<<t<<meas_acf<<fT<<tauT;
                } else if (model==6) {
                    double t=meas_acf;
                    meas_acf=meas_acf/qfFCSTripletBlinkTerm(tau, fT, tauT, fT2, tauT2);
                    //qDebug()<<t<<meas_acf<<fT<<tauT;
                }

                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)

                msd_diff3d_params d = {wxy, gamma, N_particle, meas_acf};
                double r=3.0*sqr(wxy)/2.0*(1.0/N_particle/corrdata[rangeMinDatarange+i]-1.0);
                if (msd.size()==Ndist && msd_tau.size()==Ndist && msd_tau[i]==distTaus[i]) {
                    r=msd[i];
                }

                lmmin(1, &r, 1, &d, lmfit_msd_diff3d, &control, &status, NULL );
                dist[i]=r;

            }
        } else if ((model==4)||(model==5)) {
            for (int i=0; i<Ndist; i++) {
                distTaus[i]=taus[rangeMinDatarange+i];
                double meas_acf=corrdata[rangeMinDatarange+i];
                const double tau=taus[rangeMinDatarange+i];


                lm_status_struct status;
                lm_control_struct control = lm_control_double;
                control.printflags = 0; // monitor status (+1) and parameters (+2)


                msd_diff3dspim_params d = {wxy, wz, N_particle, a_pixel, meas_acf};
                double r=3.0*sqr(wxy)/2.0*(1.0/N_particle/corrdata[rangeMinDatarange+i]-1.0);
                if (msd.size()==Ndist && msd_tau.size()==Ndist && msd_tau[i]==distTaus[i]) {
                    r=msd[i];
                }

                if (model==4) lmmin(1, &r, 1, &d, lmfit_msd_SPIMdiff3dall, &control, &status, NULL );
                else if (model==5) lmmin(1, &r, 1, &d, lmfit_msd_TIRdiff3dall, &control, &status, NULL );
                dist[i]=r;

            }

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

        qfFree(dist);
        qfFree(weights);
        qfFree(modelEval);
        qfFree(distTaus);


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
        case 2: // simple 3D model, triplet corrected
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("focus size w<sub>xy</sub> [nm]"):tr("focus size [nm]");
            if (id==2) return (html)?tr("triplet fraction &theta;<sub>T</sub>"):tr("triplet fraction");
            if (id==3) return (html)?tr("triplet time &tau;<sub>T</sub> [&mu;s]"):tr("triplet time [&mu;s]");
            break;
        case 3: // simple 3D model, triplet corrected
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("focus size w<sub>xy</sub> [nm]"):tr("focus size [nm]");
            if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
            if (id==3) return (html)?tr("triplet fraction &theta;<sub>T</sub>"):tr("triplet fraction");
            if (id==4) return (html)?tr("triplet time &tau;<sub>T</sub> [&mu;s]"):tr("triplet time [&mu;s]");
            break;
        case 4: // SPIM-FCS 3D model
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("1/e<sup>2</sup>-focus width w<sub>xy</sub> [nm]"):tr("1/e^2 focus width [nm]");
            if (id==2) return (html)?tr("1/e<sup>2</sup>-focus height w<sub>z</sub> [nm]"):tr("1/e^2 focus height [nm]");
            if (id==3) return (html)?tr("pixel size a [nm]"):tr("pixel size [nm]");
            break;
        case 5: // TIR-FCS 3D model
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("1/e<sup>2</sup>-focus width w<sub>xy</sub> [nm]"):tr("1/e^2 focus width [nm]");
            if (id==2) return (html)?tr("pixel size a [nm]"):tr("pixel size [nm]");
            break;
        case 6: // simple 3D model, triplet/blink corrected
            if (id==0) return (html)?tr("particle number N"):tr("particle number");
            if (id==1) return (html)?tr("focus size w<sub>xy</sub> [nm]"):tr("focus size [nm]");
            if (id==2) return (html)?tr("axial ratio &gamma;"):tr("axial ratio");
            if (id==3) return (html)?tr("triplet fraction &theta;<sub>T</sub>"):tr("triplet fraction");
            if (id==4) return (html)?tr("triplet time &tau;<sub>T</sub> [&mu;s]"):tr("triplet time [&mu;s]");
            if (id==5) return (html)?tr("blink fraction &theta;<sub>dark</sub>"):tr("blink fraction");
            if (id==6) return (html)?tr("blink time &tau;<sub>dark</sub> [&mu;s]"):tr("blink time [&mu;s]");
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
            break;
        case 1:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
            if (id==2) return QString("");
            break;
        case 2:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
            if (id==2) return QString("");
            if (id==3) return tr("&mu;s");
            break;
        case 3:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
            if (id==2) return QString("");
            if (id==3) return QString("");
            if (id==4) return tr("&mu;s");
            break;
        case 4:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
            if (id==2) return QString("nm");
            if (id==3) return QString("nm");
            break;
        case 5:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
            if (id==2) return QString("nm");
            break;
        case 6:
            if (id==0) return QString("");
            if (id==1) return tr("nm");
            if (id==2) return QString("");
            if (id==3) return QString("");
            if (id==4) return tr("&mu;s");
            if (id==5) return QString("");
            if (id==6) return tr("&mu;s");
            break;
    }
    return QString();
}
int QFFCSMSDEvaluationItem::getParameterCount(int model) const {
    switch (model) {
        case 0:
            return 2;
        case 1:
            return 3;
        case 2:
            return 4;
        case 3:
            return 5;
        case 4:
            return 4;
        case 5:
            return 3;
        case 6:
            return 7;
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
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            if (id==2) return tr("focus_struct_fac");
            break;
        case 2:
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            if (id==2) return tr("nonfl_theta1");
            if (id==3) return tr("nonfl_tau1");
            break;
        case 3:
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            if (id==2) return tr("focus_struct_fac");
            if (id==3) return tr("nonfl_theta1");
            if (id==4) return tr("nonfl_tau1");
            break;
        case 4:
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            if (id==2) return tr("focus_height");
            if (id==3) return tr("pixel_size");
            break;
        case 5:
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            if (id==2) return tr("pixel_size");
            break;
        case 6:
            if (id==0) return tr("n_particle");
            if (id==1) return tr("focus_width");
            if (id==2) return tr("focus_struct_fac");
            if (id==3) return tr("nonfl_theta1");
            if (id==4) return tr("nonfl_tau1");
            if (id==5) return tr("nonfl_theta2");
            if (id==6) return tr("nonfl_tau2");
            break;
    }
    return QString("m%1_p%2").arg(model).arg(id);
}

int QFFCSMSDEvaluationItem::getModelCount(QFRawDataRecord *r, int index) const {
    return 7;
}
