/*
    Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "qfweightingtools.h"
#include "qfrawdatarecord.h"
#include "qfrdrcurvesinterface.h"
#include "statistics_tools.h"

QFCurveWeightingTools::QFCurveWeightingTools()
{
    m_weighting=QFCurveWeightingTools::EqualWeighting;
}

QFCurveWeightingTools::~QFCurveWeightingTools()
{
}

void QFCurveWeightingTools::setFitDataWeighting(QFCurveWeightingTools::DataWeight weighting)
{
    m_weighting=weighting;
}

void QFCurveWeightingTools::setFitDataWeighting(int weighting)
{
    setFitDataWeighting(indexToWeight(weighting));
}

QFCurveWeightingTools::DataWeight QFCurveWeightingTools::getFitDataWeighting() const
{
    return m_weighting;
}

bool QFCurveWeightingTools::isEqualWeights() const
{
    return m_weighting==EqualWeighting;
}

QString QFCurveWeightingTools::dataWeightToString(QFCurveWeightingTools::DataWeight weight)
{
    //if (weight==StdDevWeighting) return "stddev";
    if (weight==RunErrorWeighting) return "runerror";
    if (weight==RunningStdDev3Weight) return "runsd3";
    if (weight==RunningStdDev5Weight) return "runsd5";
    if (weight==RunningStdDev7Weight) return "runsd7";
    if (weight==RunningStdDev11Weight) return "runsd11";
    if (weight==Poly2Weight) return "poly2";
    if (weight==Poly3Weight) return "poly3";
    if (weight==Poly5Weight) return "poly5";
    return "equal";
}

QString QFCurveWeightingTools::dataWeightToName(QFCurveWeightingTools::DataWeight weight, const QString &/*runname*/)
{
    //if (weight==StdDevWeighting) return "stddev";
    if (weight==RunErrorWeighting) return QString("error");
    if (weight==RunningStdDev3Weight) return QString("SD 3-points");
    if (weight==RunningStdDev5Weight) return QString("SD 5-points");
    if (weight==RunningStdDev7Weight) return QString("SD 7-points");
    if (weight==RunningStdDev11Weight) return QString("SD 11-");
    if (weight==Poly2Weight) return QString("dev. from poly(2)");
    if (weight==Poly3Weight) return QString("dev. from poly(3)");
    if (weight==Poly5Weight) return QString("dev. from poly(5)");
    return "equal";
}

QFCurveWeightingTools::DataWeight QFCurveWeightingTools::stringToDataWeight(QString weight)
{
    QString w=weight.toLower().trimmed();
//    if (w=="1" || w=="stddev") return StdDevWeighting;
    if (w=="1" || w=="runerror") return RunErrorWeighting;
    if (w=="2" || w=="runsd3") return RunningStdDev3Weight;
    if (w=="3" || w=="runsd5") return RunningStdDev5Weight;
    if (w=="4" || w=="runsd7") return RunningStdDev5Weight;
    if (w=="5" || w=="runsd11") return RunningStdDev11Weight;
    if (w=="6" || w=="poly2") return Poly2Weight;
    if (w=="7" || w=="poly3") return Poly3Weight;
    if (w=="8" || w=="poly5") return Poly5Weight;
    return EqualWeighting;
}

/*
#define CHECK_WEIGHT \
    for (int i=0; i<N; i++) { \
        if ((data_start>=0) && (data_end>=0)) { \
            if ((i>=data_start)&&(i<=data_end)) { \
                if ((fabs(weights[i])<100.0*DBL_MIN)||(!QFFloatIsOK(weights[i]))) { \
                    weightsOK=false; \
                    break; \
                } \
            }; \
        } else { \
            if ((fabs(weights[i])<100*DBL_MIN)||(!QFFloatIsOK(weights[i]))) { \
                weightsOK=false; \
                break; \
            }; \
        } \
    }
*/


#define CHECK_WEIGHT \
    for (int i=0; i<N; i++) { \
        if ((fabs(weights[i])<100*DBL_MIN)||(!QFFloatIsOK(weights[i]))) { \
            weightsOK=false; \
            break; \
        }; \
    }


#define RUN_STD(avg) \
    for (int i=0; i<N; i++) { \
        weights[i]=0; \
        int jstart=-avg/2; \
        int jend=avg/2; \
        while (i+jstart<0) { \
            jstart++; \
            jend++; \
        } \
        while (i+jend>=N) { \
            jstart--; \
            jend--; \
        } \
        if (i+jstart>=0 && i+jend<N) { \
            register double s=0, s2=0, cnt=0; \
            for (register int j=jstart; j<=jend; j++) { \
                s=s+corrdat[i+j]; \
                s2=s2+corrdat[i+j]*corrdat[i+j]; \
                cnt++; \
            } \
            weights[i]=sqrt((s2-s*s/double(cnt))/double(cnt-1)); \
        } \
    }

#define POLY_WEIGHT(P) { \
    int avg=P+5; \
    for (int i=0; i<N; i++) { \
        QVector<double> x,y; \
        weights[i]=0; \
        int jstart=-avg/2; \
        int jend=avg/2; \
        while (i+jend>=N) { \
            jstart--; \
            jend--; \
        } \
        while (i+jstart<0) { \
            jstart++; \
            jend++; \
        } \
        if (i+jstart>=0 && i+jend<=N) { \
            double s=0, s2=0; \
            for (int j=jstart; j<jend; j++) { \
                x<<log(taudat[i+j]); \
                y<<corrdat[i+j]; \
            } \
            QVector<double> p(P+1,0.0); \
            if (statisticsPolyFit<double>(x.data(), y.data(), x.size(), P, p.data())) { \
                register double cn=0, s2=0; \
                for (register int j=jstart; j<jend; j++) { \
                    const double ee=corrdat[i+j]-statisticsPolyEval<double>(log(taudat[i]), p.data(), P); \
                    cn=cn+1.0; \
                    s2=s2+ee*ee; \
                } \
                weights[i]=sqrt(s2/cn); \
            } else { \
                weights[i] = 0; \
            } \
        } \
    } \
}

//                const double lx=log(taudat[i]);
//                weights[i]=fabs(corrdat[i]-statisticsPolyEval<double>(lx, p.data(), P));

double *QFCurveWeightingTools::allocWeights(bool *weightsOKK, const QFRawDataRecord *record_in, int run_in, bool returnEmptyForNoWeights) const
{
    QFCurveWeightingTools::DataWeight weighting=getFitDataWeighting();
    if (returnEmptyForNoWeights && weighting==EqualWeighting) return NULL;
    QVector<double> weights=allocVecWeights(weightsOKK, record_in, run_in, returnEmptyForNoWeights);

    if (weights.size()>0) {
        double* w=(double*)qfMalloc(weights.size()*sizeof(double));
        copyArray(w, weights.constData(), weights.size());
        //qDebug()<<"QFFCSWeightingTools::allocWeights: "<<w<<weights.size();
        return w;
    }
    //qDebug()<<"QFFCSWeightingTools::allocWeights: "<<NULL;
    if (weightsOKK) *weightsOKK=false;
    return NULL;
}

QVector<double> QFCurveWeightingTools::allocVecWeights(bool *weightsOKK, const QFRawDataRecord *record_in, int run_in, bool returnEmptyForNoWeights) const
{
    QVector<double> weights;


    if (weightsOKK) *weightsOKK=false;
    const QFRawDataRecord* record=record_in;
    //if (!record_in) record=getHighlightedRecord();
    const QFRDRCurvesInterface* data=dynamic_cast<const QFRDRCurvesInterface*>(record);

    int run=run_in;
    //if (run<=-100) run=getCurrentIndex();

    QVector<double> taudat, corrdat, err;
    long N=0;
    if (data) {
        taudat=data->curvesGetX(run);
        corrdat=data->curvesGetY(run);
        err=data->curvesGetYError(run);
        N=qMin(taudat.size(), corrdat.size());
    }

    //qDebug()<<"allocWeights w="<<getFitDataWeighting();
    if (N<=0) return QVector<double>();

    QFCurveWeightingTools::DataWeight weighting=getFitDataWeighting();
    if (returnEmptyForNoWeights && weighting==EqualWeighting) return QVector<double>();
    weights.resize(N);//double* weights=(double*)qfMalloc(N*sizeof(double));
    bool weightsOK=false;
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::RunErrorWeighting) {
        //qDebug()<<N<<arrayToString(std, N);
        if (N>0) {
            weightsOK=true;
            for (int i=0; i<N; i++) {
                weights[i]=err.value(i, 0);
            }
            CHECK_WEIGHT
        }
        //qDebug()<<weightsOK;
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::RunningStdDev3Weight) {
        if (N>0) {
            weightsOK=true;
            RUN_STD(3)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::RunningStdDev5Weight) {
        if (N>0) {
            weightsOK=true;
            RUN_STD(5)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::RunningStdDev7Weight) {
        if (N>0) {
            weightsOK=true;
            RUN_STD(7)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::RunningStdDev11Weight) {
        if (N>0) {
            weightsOK=true;
            RUN_STD(11)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::Poly2Weight) {
        if (N>0) {
            weightsOK=true;
            POLY_WEIGHT(2)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::Poly3Weight) {
        if (N>0) {
            weightsOK=true;
            POLY_WEIGHT(3)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFCurveWeightingTools::Poly5Weight) {
        if (N>0) {
            weightsOK=true;
            POLY_WEIGHT(5)
            CHECK_WEIGHT
        }
    }
    if (!weightsOK && weights.size()>0) {
        double ww=1.0;

        for (int i=0; i<N; i++) weights[i]=ww;
        if (weighting==QFCurveWeightingTools::EqualWeighting) weightsOK=true;
    }
    //qDebug()<<"allocWeights weightsOK="<<weightsOK<<weights<<weighting;

    if (weightsOKK) *weightsOKK=weightsOK;
    //qDebug()<<"QFFCSWeightingTools::allocVecWeights "<<weights.size();


    return weights;

}

QFCurveWeightingTools::DataWeight QFCurveWeightingTools::indexToWeight(int index)
{
    if (index<0 || index>QFCurveWeightingTools::DataWeightingMax) return QFCurveWeightingTools::EqualWeighting;
    return (QFCurveWeightingTools::DataWeight)index;

}
















QFCurveWeightingCombobox::QFCurveWeightingCombobox(QWidget *parent):
    QComboBox(parent)
{
    setEditable(false);
    clear();
    addItem(tr("equal weights"));
    addItem(tr("per run error"));
    addItem(tr("running SD, 3"));
    addItem(tr("running SD, 5"));
    addItem(tr("running SD, 7"));
    addItem(tr("running SD, 11"));
    addItem(tr("deviation from poly(2)"));
    addItem(tr("deviation from poly(3)"));
    addItem(tr("deviation from poly(5)"));
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIdxChanged(int)));
}

QFCurveWeightingTools::DataWeight QFCurveWeightingCombobox::currentWeight() const
{
    return indexToWeight(currentIndex());
}

void QFCurveWeightingCombobox::setCurrentWeight(QFCurveWeightingTools::DataWeight weight)
{
    setCurrentIndex((int)weight);
}

QFCurveWeightingTools::DataWeight QFCurveWeightingCombobox::indexToWeight(int index)
{
    return QFCurveWeightingTools::indexToWeight(index);
}

void QFCurveWeightingCombobox::currentIdxChanged(int idx)
{
    emit currentWeightChanged(indexToWeight(idx));
}
