#include "qfrdrresultsmodel.h"
#include "qfrawdatarecord.h"


QFRDRResultsModel::QFRDRResultsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    record=NULL;
    resultFilter="";
    evaluationFilter="";
    resultFilterRegExp=false;
    evaluationFilterRegExp=false;
}

QFRDRResultsModel::~QFRDRResultsModel()
{
    //dtor
}

template <class T1>
bool QFRDRResultsModel_StringPairCaseInsensitiveCompareSecond(const QPair<T1, QString> &s1, const QPair<T1, QString> &s2) {
    return s1.second.toLower() < s2.second.toLower();
}

template <class T1>
bool QFRDRResultsModel_StringPairCaseInsensitiveCompareFirst(const QPair<QString, T1> &s1, const QPair<QString, T1> &s2) {
    return s1.first.toLower() < s2.first.toLower();
}

void QFRDRResultsModel::resultsChanged() {
    lastResultNames.clear();
    lastResultLabels.clear();
    lastResultSets.clear();

    QTime t;
    t.start();

    if (record) {
        //qDebug()<<"--- QFRDRResultsModel (rec:"<<record->getName()<<")";

        /*if (record) lastResultNames=record->resultsCalcNames();
        else lastResultNames.clear();*/
        QList<QPair<QString, QString> > l=record->resultsCalcNamesAndLabels();
        for (int i=0; i<l.size(); i++) {
            lastResultNames.append(l[i].second);
            if (l[i].first.isEmpty()) lastResultLabels.append(l[i].second);
            else lastResultLabels.append(l[i].first);
        }


        for (int section=0; section<record->resultsGetEvaluationCount(); section++) {
            QString en=record->resultsGetEvaluationName(section);
            QString description=record->resultsGetEvaluationDescription(en);
            lastResultSets.append(qMakePair(en, description));
        }

        if (lastResultSets.size()>0) {
            qSort(lastResultSets.begin(), lastResultSets.end(), QFRDRResultsModel_StringPairCaseInsensitiveCompareSecond<QString>);
        }

        if (!evaluationFilter.isEmpty()) {
            QRegExp rx(evaluationFilter, Qt::CaseInsensitive, (evaluationFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
            for (int i=lastResultSets.size()-1; i>=0; i--) {
                QString n=lastResultSets[i].second;
                if (rx.indexIn(n)<0) {
                    lastResultSets.removeAt(i);
                }
            }
        }
        if (!resultFilter.isEmpty()) {
            QRegExp rx(resultFilter, Qt::CaseInsensitive,(resultFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
            for (int i=lastResultLabels.size()-1; i>=0; i--) {
                if (rx.indexIn(lastResultLabels[i])<0) {
                    lastResultNames.removeAt(i);
                    lastResultLabels.removeAt(i);
                }
            }
        }
    }
    reset();
    //qDebug()<<"--- QFRDRResultsModel ... done "<<t.elapsed();
}

QVariant QFRDRResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!record) return QVariant();
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Horizontal) {
            if (section<lastResultSets.size()) return QVariant(lastResultSets[section].second);
            else return tr("Avg %1 StdDev").arg(QChar(0xB1)); //tr("&lang;val&rang; &plusmn; &sigma<sub>val</sub>");
        } else {
            if (section<lastResultLabels.size()) return QVariant(lastResultLabels[section]);
        }
    }
    return QVariant();
}

void QFRDRResultsModel::init(QFRawDataRecord* record) {
    disconnect(this->record, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    setParent(record);
    this->record=record;
    connect(record, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    resultsChanged();
}

int QFRDRResultsModel::rowCount(const QModelIndex &parent ) const {
    if (!record) {
        //std::cout<<"result row count=0\n";
        return 0;
    }
    //QList<QString>& l=lastResultNames;
    //std::cout<<"result row count="<<lastResultNames.size()<<std::endl;
    return lastResultNames.size();
}

int QFRDRResultsModel::columnCount(const QModelIndex &parent) const {
    if (!record) {
        //std::cout<<"result column count=0\n" ;
        return 0;
    }
    //std::cout<<"result column count="<<record->resultsGetEvaluationCount()<<std::endl;
    //return record->resultsGetEvaluationCount()+1;
    return lastResultSets.size();
}

Qt::ItemFlags QFRDRResultsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRResultsModel::data(const QModelIndex &index, int role) const {
    int resNameI=index.row();
    int resI=index.column();
    if (!record || !index.isValid()) return QVariant();
    if (role==Qt::DisplayRole) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) { //record->resultsGetEvaluationCount()) {
                QString en=lastResultSets[resI].first;
                if (record->resultsExists(en, lastResultNames[resNameI])) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, lastResultNames[resNameI]);
                    if ( (r.type!=QFRawDataRecord::qfrdreNumberVector) && (r.type!=QFRawDataRecord::qfrdreNumberMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreNumberErrorVector) && (r.type!=QFRawDataRecord::qfrdreNumberErrorMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreIntegerVector) && (r.type!=QFRawDataRecord::qfrdreIntegerMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreStringVector) && (r.type!=QFRawDataRecord::qfrdreStringMatrix) &&
                         (r.type!=QFRawDataRecord::qfrdreBooleanVector) && (r.type!=QFRawDataRecord::qfrdreBooleanMatrix) ) {
                        return QVariant(record->resultsGetAsString(en, lastResultNames[resNameI]).replace("+/-", "&plusmn;").replace(" um", " &mu;m").replace(" usecs", " &mu;s").replace(" usec", " &mu;s"));
                    } else {
                        if (r.getVectorMatrixItems()<=10) return QVariant(record->resultsGetAsString(en, lastResultNames[resNameI]).replace("+/-", "&plusmn;").replace(" um", " &mu;m").replace(" usecs", " &mu;s").replace(" usec", " &mu;s"));
                        else {
                            /*if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                                || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                                double var=0;
                                double mean=qfstatisticsAverageVariance(var, r.dvec);
                                return QVariant(QString("(%1 &plusmn; %2) %3 [").arg(mean).arg(sqrt(var)).arg(r.unit)+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                            } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                                double var=0;
                                double mean=qfstatisticsAverageVariance(var, r.ivec);
                                return QVariant(QString("(%1 &plusmn; %2) %3 [").arg(mean).arg(sqrt(var)).arg(r.unit)+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                            } else {*/
                                return QVariant(QString("[")+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                            //}
                        }
                    }
                }
            } else if (resI==record->resultsGetEvaluationCount()) {
                QString rname=lastResultNames[resNameI];
                double average=0;
                double stddev=0;
                calcStatistics(rname, average, stddev);
                return QVariant(QString("%1 &plusmn; %2").arg(roundWithError(average, stddev)).arg(roundError(stddev)));
            }
        }
    } else if ((role==Qt::ToolTipRole)||(role==Qt::StatusTipRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
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
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
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
                    } else {
                        if (r.type==QFRawDataRecord::qfrdreString) return record->resultsGetAsString(en, rname);
                    }
                }
            }
        }
    } else if ((role==SDRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
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
                    }
                }
            }
        }

    } else if ((role==ValueRole)||(role==Qt::EditRole)) {
        if (resNameI<lastResultNames.size()) {
            //if (resI<record->resultsGetEvaluationCount()) {
                //QString en=record->resultsGetEvaluationName(resI);
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].first;
                if (record->resultsExists(en, lastResultNames[resNameI])) {
                    return record->resultsGetAsQVariant(en, lastResultNames[resNameI]);
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


void QFRDRResultsModel::setResultFilter(QString filter)
{
    resultFilter=filter;
    resultsChanged();
}

void QFRDRResultsModel::setEvaluationFilter(QString filter)
{
    evaluationFilter=filter;
    resultsChanged();
}

void QFRDRResultsModel::setEvaluationFilterUsesRegExp(bool use)
{
    evaluationFilterRegExp=use;
    resultsChanged();
}

void QFRDRResultsModel::setResultFilterUsesRegExp(bool use)
{
    resultFilterRegExp=use;
    resultsChanged();
}

void QFRDRResultsModel::calcStatistics(QString resultName, double& average, double& stddev) const {
    average=0;
    stddev=0;
    double sum=0;
    double sum2=0;
    double count=0;
    int evalcnt=record->resultsGetEvaluationCount();

    for (register int i=0; i<evalcnt; i++) {
        bool ok=false;
        double value=record->resultsGetAsDouble(record->resultsGetEvaluationName(i), resultName, &ok);
        if (ok) {
            sum=sum+value;
            sum2=sum2+value*value;
            count=count+1;
        }

    }

    if (count>0) {
        average=sum/count;
        if (count>1) {
            stddev=sqrt(sum2/count-sum*sum/count/count);
        }
    }
}
