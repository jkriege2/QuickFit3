#include "qfevaluationresultsmodel.h"
#include "qfevaluationitem.h"
#include "qfproject.h"

QFEvaluationResultsModel::QFEvaluationResultsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    evaluation=NULL;
}

QFEvaluationResultsModel::~QFEvaluationResultsModel()
{
    //dtor
}

void QFEvaluationResultsModel::init(QFEvaluationItem* evaluation, QString evalFilter) {
    disconnect(this->evaluation, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    setParent(evaluation);
    this->evaluation=evaluation;
    this->evalFilter=evalFilter;
    if (evaluation) connect(evaluation, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    resultsChanged();
}

void QFEvaluationResultsModel::resultsChanged() {
    if (evaluation) {
        //lastResultNames=evaluation->getProject()->rdrCalcMatchingResultsNames(evalFilter);
        lastResults=evaluation->getProject()->rdrCalcMatchingResults(evalFilter);
        QList<QPair<QString, QString> > l=evaluation->getProject()->rdrCalcMatchingResultsNamesAndLabels(evalFilter);
        lastResultNames.clear();
        lastResultLabels.clear();
        for (int i=0; i<l.size(); i++) {
            lastResultNames.append(l[i].second);
            if (l[i].first.isEmpty()) lastResultLabels.append(l[i].second);
            else lastResultLabels.append(l[i].first);
        }
    } else {
        lastResultNames.clear();
        lastResultLabels.clear();
        lastResults.clear();
    }
    reset();
}

int QFEvaluationResultsModel::rowCount(const QModelIndex &parent) const {
    if (!evaluation) {
        return 0;
    }
    return lastResults.size()+1;
}

int QFEvaluationResultsModel::columnCount(const QModelIndex &parent) const {
    if (!evaluation) {
        return 0;
    }
    return lastResultNames.size();
}

Qt::ItemFlags QFEvaluationResultsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFEvaluationResultsModel::data(const QModelIndex &index, int role) const {
    int resNameI=index.column();
    int resI=index.row();
    if (!evaluation || !index.isValid()) return QVariant();
    if (role==Qt::DisplayRole) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResults.size()) {
                QFRawDataRecord* record=lastResults[resI].first;
                QString en=lastResults[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    if (record->resultsExists(en, rname)) {
                        QFRawDataRecord::evaluationResult r=record->resultsGet(en, rname);
                        return QVariant(record->resultsGetAsString(en, rname));
                    }
                }
            }else if (resI==lastResults.size()) {
                double average=0;
                double stddev=0;
                QString rname=lastResultNames[resNameI];
                calcStatistics(rname, average, stddev);
                return QVariant(QString("%1 +/- %2").arg(roundWithError(average, stddev)).arg(roundError(stddev)));
            }
        }
    } else if ((role==ValueRole)||(role==Qt::EditRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResults.size()) {
                QFRawDataRecord* record=lastResults[resI].first;
                QString en=lastResults[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    if (record->resultsExists(en, rname)) {
                        return record->resultsGetAsQVariant(en, rname);
                    }
                }
            }
        }
    } else if (role==NameRole) {
        if (resNameI<lastResultNames.size()) {
            return QVariant(lastResultNames[resNameI]);
        }
    }

    return QVariant();
}

QVariant QFEvaluationResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!evaluation) return QVariant();
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Vertical) {
            if (section<lastResults.size()) {
                if (lastResults[section].first) {
                    return QVariant(lastResults[section].first->getName()+": "+lastResults[section].second);
                } else {
                    return QVariant();
                }
            } else return tr("Average +/- StdDev");
        } else {
            if (section<lastResultLabels.size()) return QVariant(lastResultLabels[section]);

        }
    }
    return QVariant();
}



void QFEvaluationResultsModel::calcStatistics(QString resultName, double& average, double& stddev) const {
    average=0;
    stddev=0;
    double sum=0;
    double sum2=0;
    double count=0;

    for (register int i=0; i<lastResults.size(); i++) {
        QFRawDataRecord* record=lastResults[i].first;
        QString en=lastResults[i].second;
        if (record) {
            bool ok=false;
            double value=record->resultsGetAsDouble(en, resultName, &ok);
            if (ok) {
                sum=sum+value;
                sum2=sum2+value*value;
                count=count+1;
            }
        }

    }

    if (count>0) {
        average=sum/count;
        if (count>1) {
            stddev=sqrt(sum2/count-sum*sum/count/count);
        }
    }
}
