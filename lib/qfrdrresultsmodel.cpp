#include "qfrdrresultsmodel.h"
#include "qfrawdatarecord.h"



QFRDRResultsModel::QFRDRResultsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    record=NULL;
    resultFilter="";
    evaluationFilter="";
    resultFilterNot="";
    evaluationFilterNot="";
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

void QFRDRResultsModel::resultsChanged(const QString& evalName, const QString& resultName,bool deleted) {
    bool updateAll=true;
    int updateRow=-1;
    int updateCol=-1;
    int updateRowEnd=-1;
    int updateColEnd=-1;
    if (!evalName.isEmpty()) {
        for (int i=0; i<lastResultSets.size(); i++) {
            QString en=lastResultSets[i].first;
            if (en==evalName) {
                if (resultName.isEmpty()) { // no resultName was given, so a complete column changed
                    updateRow=0;
                    updateRowEnd=rowCount()-1;
                    updateColEnd=updateCol=i;
                    updateAll=false;
                    break;
                } else {
                    int r=lastResultNames.indexOf(resultName);
                    if (r>=0) {
                        updateRowEnd=updateRow=r;
                        updateColEnd=updateCol=i;
                        updateAll=false;
                        break;
                    }
                }
            }
        }
    }

    if (updateAll && (!evalName.isEmpty()) && (!resultName.isEmpty()) && (!deleted)) {
        // so far we didn't detect that an existing cell just changed, so if it is an add cell operation,
        // we have to cope with that extra

        // let's first find out whether the changed result is visible with the current filters at all
        QString enLabel="", rnLabel="";
        if (fulfillsFilters(evalName, resultName, &enLabel, &rnLabel)) {
            //qDebug()<<"   enLabel="<<enLabel<<"   rnLabel="<<rnLabel;
            // now we try to find the place where the result has to be inserted
            bool insertRow=true;
            int rowPos=0;
            bool insertCol=true;
            int colPos=0;
            for (int r=0; r<rowCount(); r++) {
                QString hd=headerData(r, Qt::Vertical).toString();
                //qDebug()<<"r="<<r<<"   hd="<<hd;
                if (hd==rnLabel) {
                    insertRow=false;
                    rowPos=r;
                    break;
                } else if (rnLabel>hd) {
                    rowPos=r;
                }
            }
            for (int c=0; c<columnCount()-1; c++) {
                QString hd=headerData(c, Qt::Horizontal).toString();
                //qDebug()<<"c="<<c<<"   hd="<<hd;
                if (hd==enLabel) {
                    insertCol=false;
                    colPos=c;
                    break;
                } else if (enLabel>hd) {
                    colPos=c;
                }
            }
            if (columnCount()-1<=0) {
                colPos=0;
                insertCol=true;
            }
            if (rowCount()<=0) {
                rowPos=0;
                insertRow=true;
            }
            if (rowPos>=0 && colPos>=0) {
                if (insertRow) {
                    beginInsertRows(QModelIndex(), rowPos, rowPos);
                    lastResultNames.insert(rowPos, resultName);
                    lastResultLabels.insert(rowPos, rnLabel);
                    endInsertRows();
                    //qDebug()<<"QFRDRResultsModel::resultsChanged:   insertRow";
                }
                if (insertCol) {
                    beginInsertColumns(QModelIndex(), colPos, colPos);
                    lastResultSets.insert(colPos, qMakePair(evalName, enLabel));
                    endInsertColumns();
                    //qDebug()<<"QFRDRResultsModel::resultsChanged:   insertCol";
                }
                if (!insertRow && !insertCol) {
                    emit dataChanged(index(rowPos, colPos), index(rowPos, colPos));
                    //qDebug()<<"QFRDRResultsModel::resultsChanged:   dataChanged";
                }
                return;
            }
        }

    }

    if (!updateAll) {
        if (!deleted){
            // we only updated data in a single cell
            emit dataChanged(index(updateRow, updateCol), index(updateRowEnd, updateColEnd));
        } else {
            if (updateRow==updateRowEnd) {
                if (updateCol==0 && updateColEnd==columnCount()-1) { // we are removing a complete row
                    beginRemoveRows(QModelIndex(), updateRow, updateRow);
                    lastResultNames.removeAt(updateRow);
                    lastResultLabels.removeAt(updateRow);
                    endRemoveRows();
                    //qDebug()<<"QFRDRResultsModel::resultsChanged:   removeRows";
                } else if (updateCol==updateColEnd && updateRow==updateRowEnd) { // we are removing a single entry only
                    emit dataChanged(index(updateRow, updateCol), index(updateRowEnd, updateColEnd));
                    //qDebug()<<"QFRDRResultsModel::resultsChanged:   dataChanged";
                } else { // any other remove operations trigger a model reset
                    updateAll=true;
                }
            } else if (updateCol==updateColEnd) {
                if (updateRow==0 && updateRowEnd==rowCount()-1) { // we are removing a complete column
                    beginRemoveColumns(QModelIndex(), updateCol, updateCol);
                    lastResultSets.removeAt(updateCol);
                    endRemoveColumns();
                    //qDebug()<<"QFRDRResultsModel::resultsChanged:   removeCols";
                } else if (updateCol==updateColEnd && updateRow==updateRowEnd) { // we are removing a single entry only
                    emit dataChanged(index(updateRow, updateCol), index(updateRowEnd, updateColEnd));
                    //qDebug()<<"QFRDRResultsModel::resultsChanged:   dataChanged";
                } else { // any other remove operations trigger a model reset
                    updateAll=true;
                }
            }

        }
    }
    if (updateAll) { // there is something we couldn't cope with, so we have to rebuild the model
        //qDebug()<<"QFRDRResultsModel::resultsChanged:   reset";

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


            if ((!evaluationFilter.isEmpty()) || (!evaluationFilterNot.isEmpty())) {
                bool usefilt=!evaluationFilter.isEmpty();
                bool usefiltN=!evaluationFilterNot.isEmpty();
                QRegExp rx(evaluationFilter, Qt::CaseInsensitive, (evaluationFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
                QRegExp rxN(evaluationFilterNot, Qt::CaseInsensitive, (evaluationFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
                for (int i=lastResultSets.size()-1; i>=0; i--) {
                    QString n=lastResultSets[i].second;
                    bool filt=rx.indexIn(n)>=0;
                    bool filtN=rxN.indexIn(n)>=0;
                    bool ok=(filt||(!usefilt)) && ((!filtN)||(!usefiltN));

                    if (!ok) {
                        lastResultSets.removeAt(i);
                    }
                }
            }
            if ((!resultFilter.isEmpty()) || (!resultFilterNot.isEmpty())) {
                bool usefilt=!resultFilter.isEmpty();
                bool usefiltN=!resultFilterNot.isEmpty();
                QRegExp rx(resultFilter, Qt::CaseInsensitive, (resultFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
                QRegExp rxN(resultFilterNot, Qt::CaseInsensitive, (resultFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
                for (int i=lastResultLabels.size()-1; i>=0; i--) {
                    QString n=lastResultLabels[i];
                    bool filt=rx.indexIn(n)>=0;
                    bool filtN=rxN.indexIn(n)>=0;
                    bool ok=(filt||(!usefilt)) && ((!filtN)||(!usefiltN));

                    if (!ok) {
                        lastResultNames.removeAt(i);
                        lastResultLabels.removeAt(i);
                    }
                }
            }
        }
        reset();
        //qDebug()<<"--- QFRDRResultsModel ... done "<<t.elapsed();
    }
}

bool QFRDRResultsModel::fulfillsFilters(const QString &en, const QString &resultName, QString* enLabel, QString* rnLabel) {
    if (!record) return false;
    bool display=true;
    QString n=record->resultsGetEvaluationDescription(en);
    if (enLabel) *enLabel=n;
    if ((!evaluationFilter.isEmpty()) || (!evaluationFilterNot.isEmpty())) {
        bool usefilt=!evaluationFilter.isEmpty();
        bool usefiltN=!evaluationFilterNot.isEmpty();
        QRegExp rx(evaluationFilter, Qt::CaseInsensitive, (evaluationFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
        QRegExp rxN(evaluationFilterNot, Qt::CaseInsensitive, (evaluationFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));

        bool filt=rx.indexIn(n)>=0;
        bool filtN=rxN.indexIn(n)>=0;
        display=(filt||(!usefilt)) && ((!filtN)||(!usefiltN));

    }


    QString g=record->resultsGetGroup(en, resultName);
    n=record->resultsGetLabel(en, resultName);
    if (!g.isEmpty()) n=g+": "+n;
    if (rnLabel) *rnLabel=n;
    if ((!resultFilter.isEmpty()) || (!resultFilterNot.isEmpty())) {
        bool usefilt=!resultFilter.isEmpty();
        bool usefiltN=!resultFilterNot.isEmpty();
        QRegExp rx(resultFilter, Qt::CaseInsensitive, (resultFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
        QRegExp rxN(resultFilterNot, Qt::CaseInsensitive, (resultFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));

        bool filt=rx.indexIn(n)>=0;
        bool filtN=rxN.indexIn(n)>=0;
        display=display&&(filt||(!usefilt)) && ((!filtN)||(!usefiltN));
    }
    return display;
}


QVariant QFRDRResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!record) return QVariant();
    if (role==Qt::DisplayRole || role==Qt::ToolTipRole) {
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
    disconnect(this->record, SIGNAL(resultsChanged(QString,QString,bool)), this, SLOT(resultsChanged(QString,QString,bool)));
    setParent(record);
    this->record=record;
    connect(record, SIGNAL(resultsChanged(QString,QString,bool)), this, SLOT(resultsChanged(QString,QString,bool)));
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
    return lastResultSets.size()+1;
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
    } else if (role==Qt::BackgroundColorRole) {
        if (resI==columnCount()-1) {
            return QVariant(QColor("lightsteelblue"));
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
                    } else if ((r.type==QFRawDataRecord::qfrdreString)) {
                        return QVariant(tr("contents: %2<br><i>&nbsp;&nbsp;&nbsp;%1</i>").arg(QFRawDataRecord::evaluationResultType2String(r.type)).arg(data(index, Qt::DisplayRole).toString()));
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

    } else if ((role==CountRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return qfstatisticsCount(r.dvec);
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return qfstatisticsCount(r.ivec);
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return 1;
                    } else {
                        return 0;
                    }
                }
            }
        }

    } else if ((role==SumRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return qfstatisticsSum(r.dvec);
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return qfstatisticsSum(r.ivec);
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return record->resultsGetAsDouble(en, rname);
                    }
                }
            }
        }

    } else if ((role==Sum2Role)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return qfstatisticsSum2(r.dvec);
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return qfstatisticsSum2(r.ivec);
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        double v= record->resultsGetAsDouble(en, rname);
                        return v*v;
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
                        return sqrt(qfstatisticsVariance(r.ivec));
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return record->resultsGetErrorAsDouble(en, rname);
                    }
                }
            }
        }
    } else if ((role==MedianRole)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return qfstatisticsMedian(r.dvec);
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return qfstatisticsMedian(r.ivec);
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return record->resultsGetErrorAsDouble(en, rname);
                    }
                }
            }
        }
    } else if ((role==Quantile25Role)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return qfstatisticsQuantile(r.dvec, 0.25);
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return qfstatisticsQuantile(r.ivec, 0.25);
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return 0;
                    }
                }
            }
        }
    } else if ((role==Quantile75Role)) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].second;
                QString rname=lastResultNames[resNameI];
                if (record) {
                    const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                    if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                        || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                        return qfstatisticsQuantile(r.dvec, 0.75);
                    } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                        return qfstatisticsQuantile(r.ivec, 0.75);
                    } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                        return 0;
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

    } else if (role==EvalNameRole) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].first;
                return en;
                if (record->resultsExists(en, lastResultNames[resNameI])) {
                    return record->resultsGetAsQVariant(en, lastResultNames[resNameI]);
                }
            }
        }
    } else if (role==ResultNameRole) {
        if (resNameI<lastResultNames.size()) {
            if (resI<lastResultSets.size()) {
                QString en=lastResultSets[resI].first;
                return lastResultNames[resNameI];
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

void QFRDRResultsModel::setResultFilterNot(QString filter)
{
    resultFilterNot=filter;
    resultsChanged();
}

void QFRDRResultsModel::setEvaluationFilterNot(QString filter)
{
    evaluationFilterNot=filter;
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
        QString en=record->resultsGetEvaluationName(i);
        QFRawDataRecord::evaluationResultType t=record->resultsGetType(en, resultName);
        if (t==QFRawDataRecord::qfrdreNumberVector||t==QFRawDataRecord::qfrdreNumberMatrix||t==QFRawDataRecord::qfrdreNumberErrorVector||t==QFRawDataRecord::qfrdreNumberErrorMatrix
            ||t==QFRawDataRecord::qfrdreIntegerVector||t==QFRawDataRecord::qfrdreIntegerMatrix) {

            QVector<double> d=record->resultsGetAsDoubleList(en, resultName);
            for (int di=0; di<d.size(); di++) {
                sum=sum+d[di];
                sum2=sum2+d[di]*d[di];
            }
            count=count+d.size();

        } else {
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


