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

#include "qffcsweightingtools.h"
#include "qfrawdatarecord.h"
#include "qfrdrfcsdatainterface.h"
#include "statistics_tools.h"

QFFCSWeightingTools::QFFCSWeightingTools()
{
    m_weighting=QFFCSWeightingTools::EqualWeighting;
}

QFFCSWeightingTools::~QFFCSWeightingTools()
{
}

void QFFCSWeightingTools::setFitDataWeighting(QFFCSWeightingTools::DataWeight weighting)
{
    m_weighting=weighting;
}

void QFFCSWeightingTools::setFitDataWeighting(int weighting)
{
    setFitDataWeighting(indexToWeight(weighting));
}

QFFCSWeightingTools::DataWeight QFFCSWeightingTools::getFitDataWeighting() const
{
    return m_weighting;
}

QString QFFCSWeightingTools::dataWeightToString(QFFCSWeightingTools::DataWeight weight)
{
    if (weight==StdDevWeighting) return "stddev";
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

QString QFFCSWeightingTools::dataWeightToName(QFFCSWeightingTools::DataWeight weight, const QString &runname)
{
    if (weight==StdDevWeighting) return "stddev";
    if (weight==RunErrorWeighting) return QString("per ")+runname;
    if (weight==RunningStdDev3Weight) return QString("SD 3-")+runname+QString("s");;
    if (weight==RunningStdDev5Weight) return QString("SD 5-")+runname+QString("s");;
    if (weight==RunningStdDev7Weight) return QString("SD 7-")+runname+QString("s");;
    if (weight==RunningStdDev11Weight) return QString("SD 11-")+runname+QString("s");;
    if (weight==Poly2Weight) return QString("dev. from poly(2)");
    if (weight==Poly3Weight) return QString("dev. from poly(3)");
    if (weight==Poly5Weight) return QString("dev. from poly(5)");
    return "equal";
}

QFFCSWeightingTools::DataWeight QFFCSWeightingTools::stringToDataWeight(QString weight)
{
    QString w=weight.toLower().trimmed();
    if (w=="1" || w=="stddev") return StdDevWeighting;
    if (w=="2" || w=="runerror") return RunErrorWeighting;
    if (w=="3" || w=="runsd3") return RunningStdDev3Weight;
    if (w=="4" || w=="runsd5") return RunningStdDev5Weight;
    if (w=="5" || w=="runsd7") return RunningStdDev5Weight;
    if (w=="6" || w=="runsd11") return RunningStdDev11Weight;
    if (w=="7" || w=="poly2") return Poly2Weight;
    if (w=="8" || w=="poly3") return Poly3Weight;
    if (w=="9" || w=="poly5") return Poly5Weight;
    return EqualWeighting;
}

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

double *QFFCSWeightingTools::allocWeights(bool *weightsOKK, const QFRawDataRecord *record_in, int run_in, int data_start, int data_end) const
{
    QVector<double> weights=allocVecWeights(weightsOKK, record_in, run_in, data_start, data_end);

    if (weights.size()>0) {
        double* w=(double*)qfMalloc(weights.size()*sizeof(double));
        copyArray(w, weights.constData(), weights.size());
        //qDebug()<<"QFFCSWeightingTools::allocWeights: "<<w<<weights.size();
        return w;
    }
    //qDebug()<<"QFFCSWeightingTools::allocWeights: "<<NULL;
    if (weightsOKK) weightsOKK=false;
    return NULL;
}

QVector<double> QFFCSWeightingTools::allocVecWeights(bool *weightsOKK, const QFRawDataRecord *record_in, int run_in, int data_start, int data_end) const
{
    QVector<double> weights;


    if (weightsOKK) *weightsOKK=false;
    const QFRawDataRecord* record=record_in;
    //if (!record_in) record=getHighlightedRecord();
    const QFRDRFCSDataInterface* data=dynamic_cast<const QFRDRFCSDataInterface*>(record);

    int run=run_in;
    //if (run<=-100) run=getCurrentIndex();

    int N=0;
    if (data) N=data->getCorrelationN();

    //qDebug()<<"allocWeights w="<<getFitDataWeighting();
    if (N<=0) return QVector<double>();

    QFFCSWeightingTools::DataWeight weighting=getFitDataWeighting();
    weights.resize(N);//double* weights=(double*)qfMalloc(N*sizeof(double));
    bool weightsOK=false;
    if (data&&weights.size()>0 && weighting==QFFCSWeightingTools::StdDevWeighting) {
        double* std=data->getCorrelationStdDev();
        //qDebug()<<N<<arrayToString(std, N);
        if (std) {
            weightsOK=true;
            for (int i=0; i<N; i++) {
                weights[i]=std[i];
            }
            CHECK_WEIGHT
        }
        //qDebug()<<weightsOK;
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::RunErrorWeighting) {
        double* std=NULL;
        if (run>=0) std=data->getCorrelationRunError(run);
        else std=data->getCorrelationStdDev();
        //qDebug()<<N<<arrayToString(std, N);
        if (std) {
            weightsOK=true;
            for (int i=0; i<N; i++) {
                weights[i]=std[i];
            }
            CHECK_WEIGHT
        }
        //qDebug()<<weightsOK;
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::RunningStdDev3Weight) {
        double* corrdat=NULL;
        if (run>=0) corrdat=data->getCorrelationRun(run);
        else if (run==-1) corrdat=data->getCorrelationMean();
        if (corrdat) {
            weightsOK=true;
            RUN_STD(3)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::RunningStdDev5Weight) {
        double* corrdat=NULL;
        if (run>=0) corrdat=data->getCorrelationRun(run);
        else if (run==-1) corrdat=data->getCorrelationMean();
        if (corrdat) {
            weightsOK=true;
            RUN_STD(5)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::RunningStdDev7Weight) {
        double* corrdat=NULL;
        if (run>=0) corrdat=data->getCorrelationRun(run);
        else if (run==-1) corrdat=data->getCorrelationMean();
        if (corrdat) {
            weightsOK=true;
            RUN_STD(7)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::RunningStdDev11Weight) {
        double* corrdat=NULL;
        if (run>=0) corrdat=data->getCorrelationRun(run);
        else if (run==-1) corrdat=data->getCorrelationMean();
        if (corrdat) {
            weightsOK=true;
            RUN_STD(11)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::Poly2Weight) {
        double* taudat=data->getCorrelationT();
        double* corrdat=NULL;
        if (run>=0) corrdat=data->getCorrelationRun(run);
        else if (run==-1) corrdat=data->getCorrelationMean();
        if (corrdat) {
            weightsOK=true;
            POLY_WEIGHT(2)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::Poly3Weight) {
        double* taudat=data->getCorrelationT();
        double* corrdat=NULL;
        if (run>=0) corrdat=data->getCorrelationRun(run);
        else if (run==-1) corrdat=data->getCorrelationMean();
        if (corrdat) {
            weightsOK=true;
            POLY_WEIGHT(3)
            CHECK_WEIGHT
        }
    }
    if (data&&weights.size()>0&&!weightsOK&& weighting==QFFCSWeightingTools::Poly5Weight) {
        double* taudat=data->getCorrelationT();
        double* corrdat=NULL;
        if (run>=0) corrdat=data->getCorrelationRun(run);
        else if (run==-1) corrdat=data->getCorrelationMean();
        if (corrdat) {
            weightsOK=true;
            POLY_WEIGHT(5)
            CHECK_WEIGHT
        }
    }
    if (!weightsOK && weights.size()>0) {
        double ww=1.0;

        for (int i=0; i<N; i++) weights[i]=ww;
        if (weighting==QFFCSWeightingTools::EqualWeighting) weightsOK=true;
    }
    //qDebug()<<"allocWeights weightsOK="<<weightsOK<<weights<<weighting;

    if (weightsOKK) *weightsOKK=weightsOK;
    //qDebug()<<"QFFCSWeightingTools::allocVecWeights "<<weights.size();
    return weights;

}

QFFCSWeightingTools::DataWeight QFFCSWeightingTools::indexToWeight(int index)
{
    if (index<0 || index>QFFCSWeightingTools::DataWeightingMax) return QFFCSWeightingTools::EqualWeighting;
    return (QFFCSWeightingTools::DataWeight)index;

}
















QFFCSWeightingCombobox::QFFCSWeightingCombobox(QWidget *parent):
    QComboBox(parent)
{
    setEditable(false);
    clear();
    addItem(tr("equal weights"));
    addItem(tr("standard deviation"));
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

QFFCSWeightingTools::DataWeight QFFCSWeightingCombobox::currentWeight() const
{
    return indexToWeight(currentIndex());
}

void QFFCSWeightingCombobox::setCurrentWeight(QFFCSWeightingTools::DataWeight weight)
{
    setCurrentIndex((int)weight);
}

QFFCSWeightingTools::DataWeight QFFCSWeightingCombobox::indexToWeight(int index)
{
    return QFFCSWeightingTools::indexToWeight(index);
}

void QFFCSWeightingCombobox::currentIdxChanged(int idx)
{
    emit currentWeightChanged(indexToWeight(idx));
}
