#include "qfevaluationresultsmodel.h"
#include "qfevaluationitem.h"
#include "qfproject.h"

QFEvaluationResultsModel::QFEvaluationResultsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    evaluation=NULL;
    resultFilter="";
    filesFilter="";
    resultFilterRegExp=false;
    filesFilterRegExp=false;
}

QFEvaluationResultsModel::~QFEvaluationResultsModel()
{
    //dtor
}

void QFEvaluationResultsModel::init(QFEvaluationItem* evaluation, QString evalFilter) {
    disconnect(this->evaluation, SIGNAL(resultsChanged(QFRawDataRecord* , const QString& , const QString& )), this, SLOT(resultsChanged(QFRawDataRecord* , const QString& , const QString& )));
    setParent(evaluation);
    this->evaluation=evaluation;
    this->evalFilter=evalFilter;
    resultsChanged();
    if (evaluation) connect(evaluation, SIGNAL(resultsChanged(QFRawDataRecord* , const QString& , const QString& )), this, SLOT(resultsChanged(QFRawDataRecord* , const QString& , const QString& )));
}

void QFEvaluationResultsModel::resultsChanged(QFRawDataRecord* record, const QString& evaluationName, const QString& resultName) {
    int column=-1;
    int row=-1;
    if (record!=NULL && !evaluationName.isEmpty() && !resultName.isEmpty()) {
        QPair<QPointer<QFRawDataRecord>, QString> colPair=qMakePair((QPointer<QFRawDataRecord>)record, evaluationName);
        column=lastResults.indexOf(colPair);
        if (column>=0) {
            row=lastResultNames.indexOf(resultName);
            if (row>=0) {
                QModelIndex idx=index(row, column);
                QModelIndex idx1=index(lastResults.size(), column);
                emit dataChanged(idx, idx);
                emit dataChanged(idx1, idx1);
            }
        }


    }
    if ( (column<0) || (row<0) ) {
        QTime t;
        t.start();
        //qDebug()<<"--- QFEvaluationResultsModel::resultsChanged()";
        if (evaluation) {
            //lastResultNames=evaluation->getProject()->rdrCalcMatchingResultsNames(evalFilter);
            lastResults=evaluation->getProject()->rdrCalcMatchingResults(evalFilter);
            QList<QPair<QString, QString> > l=evaluation->getProject()->rdrCalcMatchingResultsNamesAndLabels(evalFilter);
            lastResultNames.clear();
            lastResultLabels.clear();
            resultGroups.clear();
            for (int i=0; i<l.size(); i++) {
                lastResultNames.append(l[i].second);
                if (l[i].first.isEmpty()) lastResultLabels.append(l[i].second);
                else lastResultLabels.append(l[i].first);
            }
            /*for (int i=0; i<lastResults.size(); i++) {
                if (lastResults[i].first) {
                    lastResults[i].first->resultsGetGroup(lastResults[i].second, )
                }
            }*/
        } else {
            lastResultNames.clear();
            lastResultLabels.clear();
            lastResults.clear();
        }

        if (!filesFilter.isEmpty()) {
            QRegExp rx(filesFilter, Qt::CaseInsensitive, (filesFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
            for (int i=lastResults.size()-1; i>=0; i--) {
                QString n=lastResults[i].first->getName()+": "+lastResults[i].second;
                if (rx.indexIn(n)<0) {
                    lastResults.removeAt(i);
                }
            }
        }
        if (!resultFilter.isEmpty()) {
            QRegExp rx(resultFilter, Qt::CaseInsensitive, (resultFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
            for (int i=lastResultLabels.size()-1; i>=0; i--) {
                if (rx.indexIn(lastResultLabels[i])<0) {
                    lastResultNames.removeAt(i);
                    lastResultLabels.removeAt(i);
                }
            }
        }

        reset();
        //qDebug()<<"--- QFEvaluationResultsModel::resultsChanged() DONE: "<<t.elapsed();
    }

}

void QFEvaluationResultsModel::setResultFilter(QString filter)
{
    resultFilter=filter;
    resultsChanged();
}

void QFEvaluationResultsModel::setFilesFilter(QString filter)
{
    filesFilter=filter;
    resultsChanged();
}

void QFEvaluationResultsModel::setFilesFilterUsesRegExp(bool use)
{
    filesFilterRegExp=use;
    resultsChanged();
}

void QFEvaluationResultsModel::setResultFilterUsesRegExp(bool use)
{
    resultFilterRegExp=use;
    resultsChanged();
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
            } else return tr("Average %3 StdDev").arg(QChar(0xB1));
        } else {
            if (section<lastResultLabels.size()) return QVariant(lastResultLabels[section]);

        }
    }
    return QVariant();
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
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ( (r.type!=QFRawDataRecord::qfrdreNumberVector) && (r.type!=QFRawDataRecord::qfrdreNumberMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreNumberErrorVector) && (r.type!=QFRawDataRecord::qfrdreNumberErrorMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreIntegerVector) && (r.type!=QFRawDataRecord::qfrdreIntegerMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreStringVector) && (r.type!=QFRawDataRecord::qfrdreStringMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreBooleanVector) && (r.type!=QFRawDataRecord::qfrdreBooleanMatrix) ) {
                        return QVariant(record->resultsGetAsString(en, rname).replace("+/-", "&plusmn;").replace(" um", " &mu;m").replace(" usecs", " &mu;s").replace(" usec", " &mu;s"));
                    } else {
                        if (r.getVectorMatrixItems()<=10) {
                            return QVariant(record->resultsGetAsString(en,rname).replace("+/-", "&plusmn;").replace(" um", " &mu;m").replace(" usecs", " &mu;s").replace(" usec", " &mu;s"));
                        } else {
                            /*if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                                || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                                double var=0;
                                double mean=qfstatisticsAverageVariance(var, r.dvec);
                                return QVariant(QString("(%1 &plusmn; %2) %3<br>[").arg(mean).arg(sqrt(var)).arg(r.unit)+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                            } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                                double var=0;
                                double mean=qfstatisticsAverageVariance(var, r.ivec);
                                return QVariant(QString("(%1 &plusmn; %2) %3<br>[").arg(mean).arg(sqrt(var)).arg(r.unit)+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                            } else {*/
                                return QVariant(QString("[")+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                            //}
                        }


                        //QString s= QString("[")+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]");
                        //return s;
                    }
                }
            }else if (resI==lastResults.size()) {
                double average=0;
                double stddev=0;
                QString rname=lastResultNames[resNameI];
                calcStatistics(rname, average, stddev);
                return QVariant(QString("%1 %3 %2").arg(roundWithError(average, stddev)).arg(roundError(stddev)).arg(QChar(0xB1)));
            }
        }
    } else if ((role==Qt::ToolTipRole)||(role==Qt::StatusTipRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResults.size()) {
                QFRawDataRecord* record=lastResults[resI].first;
                QString en=lastResults[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        double var=0;
                        double mean=qfstatisticsAverageVariance(var, r.dvec);
                        return QVariant(QString("mean: %1<br>S.D.: %2<br>count: %3<br><i>&nbsp;&nbsp;&nbsp;%4</i>").arg(mean).arg(sqrt(var)).arg(r.getVectorMatrixItems()).arg(QFRawDataRecord::evaluationResultType2String(r.type)));
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        double var=0;
                        double mean=qfstatisticsAverageVariance(var, r.ivec);
                        return QVariant(QString("mean: %1<br>S.D.: %2<br>count: %3<br><i>&nbsp;&nbsp;&nbsp;%4</i>").arg(mean).arg(sqrt(var)).arg(r.getVectorMatrixItems()).arg(QFRawDataRecord::evaluationResultType2String(r.type)));
                    }
                    return QVariant(QString("[")+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                }
            }
        }
    } else if ((role==AvgRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResults.size()) {
                QFRawDataRecord* record=lastResults[resI].first;
                QString en=lastResults[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return qfstatisticsAverage(r.dvec);
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return qfstatisticsAverage(r.ivec);
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return record->resultsGetAsDouble(en, rname);
                    }
                }
            }
        }
    } else if ((role==SDRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResults.size()) {
                QFRawDataRecord* record=lastResults[resI].first;
                QString en=lastResults[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return sqrt(qfstatisticsVariance(r.dvec));
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return sqrt(qfstatisticsAverage(r.ivec));
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return record->resultsGetErrorAsDouble(en, rname);
                    } else {
                        if (r.type==QFRawDataRecord::qfrdreString) return record->resultsGetAsString(en, rname);
                    }
                }
            }
        }

    } else if ((role==ValueRole)||(role==Qt::EditRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResults.size()) {
                QFRawDataRecord* record=lastResults[resI].first;
                QString en=lastResults[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    return record->resultsGetAsQVariant(en, rname);
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
