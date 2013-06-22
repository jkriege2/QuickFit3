#include "qffcsweightingtools.h"
#include "qfrawdatarecord.h"
#include "qfrdrfcsdatainterface.h"

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
    return "equal";
}

QFFCSWeightingTools::DataWeight QFFCSWeightingTools::stringToDataWeight(QString weight)
{
    QString w=weight.toLower().trimmed();
    if (w=="1" || w=="stddev") return StdDevWeighting;
    if (w=="2" || w=="runerror") return RunErrorWeighting;
    return EqualWeighting;
}

double *QFFCSWeightingTools::allocWeights(bool *weightsOKK, QFRawDataRecord *record_in, int run_in, int data_start, int data_end) const
{
    if (weightsOKK) *weightsOKK=false;
    QFRawDataRecord* record=record_in;
    //if (!record_in) record=getHighlightedRecord();
    QFRDRFCSDataInterface* data=qobject_cast<QFRDRFCSDataInterface*>(record);

    int run=run_in;
    //if (run<=-100) run=getCurrentIndex();

    int N=data->getCorrelationN();

    //qDebug()<<"allocWeights w="<<getFitDataWeighting();
    if (N<=0) return NULL;

    QFFCSWeightingTools::DataWeight weighting=getFitDataWeighting();
    double* weights=(double*)malloc(N*sizeof(double));
    bool weightsOK=false;
    if (data && weighting==QFFCSWeightingTools::StdDevWeighting) {
        double* std=data->getCorrelationStdDev();
        if (std) {
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
    }
    if (data && weighting==QFFCSWeightingTools::RunErrorWeighting) {
        double* std=NULL;
        if (run>=0) std=data->getCorrelationRunError(run);
        else std=data->getCorrelationStdDev();
        if (std) {
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
    }
    if (!weightsOK) {
        for (int i=0; i<N; i++) weights[i]=1.0/N;
        if (weighting==QFFCSWeightingTools::EqualWeighting) weightsOK=true;
    }
    //qDebug()<<"allocWeights weightsOK="<<weightsOK<<weights;

    if (weightsOKK) *weightsOKK=weightsOK;
    return weights;
}

QFFCSWeightingTools::DataWeight QFFCSWeightingTools::indexToWeight(int index)
{
    if (index<0 || index>=QFFCSWeightingTools::DataWeightingMax) return QFFCSWeightingTools::EqualWeighting;
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
