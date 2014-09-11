/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfevaluationresultsmodel.h"
#include "qfevaluationitem.h"
#include "qfproject.h"
#include "qfrdrrunselection.h"

QFEvaluationResultsModel::QFEvaluationResultsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    evaluation=NULL;
    resultFilter="";
    filesFilter="";
    resultFilterNot="";
    filesFilterNot="";
    resultFilterRegExp=false;
    filesFilterRegExp=false;
    displayProperties.clear();
    showVectorMatrixAvg=true;
    extractRuns=true;
    removeUnusedRuns=false;
    mindRunsForVectorStat=true;
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
                QModelIndex idx=index(row, column+displayProperties.size());
                QModelIndex idx1=index(lastResults.size(), column+displayProperties.size());
                emit dataChanged(idx, idx);
                emit dataChanged(idx1, idx1);
            }
        }


    }
    if ( (column<0) || (row<0) ) {
        QTime t;
        t.start();
        //qDebug()()<<"--- QFEvaluationResultsModel::resultsChanged()";
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

        } else {
            lastResultNames.clear();
            lastResultLabels.clear();
            lastResults.clear();
        }

        if (extractRuns && removeUnusedRuns && evaluation) {
            for (int i=lastResults.size()-1; i>=0; i--) {
                int run=evaluation->getIndexFromEvaluationResultID(lastResults[i].second);
                QFRawDataRecord* rdr=lastResults[i].first;
                QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(rdr);
                bool ok=true;
                if (runsel && run>=0) {
                    ok=!runsel->leaveoutRun(run);
                    //qDebug()<<rdr->getName()<<run<<ok;
                }

                if (!ok) {
                    lastResults.removeAt(i);
                }
            }
        }

        if ((!filesFilter.isEmpty()) || (!filesFilterNot.isEmpty())) {
            bool usefilt=!filesFilter.isEmpty();
            bool usefiltN=!filesFilterNot.isEmpty();
            QRegExp rx(filesFilter, Qt::CaseInsensitive, (filesFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
            QRegExp rxN(filesFilterNot, Qt::CaseInsensitive, (filesFilterRegExp)?(QRegExp::RegExp):(QRegExp::Wildcard));
            for (int i=lastResults.size()-1; i>=0; i--) {
                QString n=lastResults[i].first->getName()+": "+lastResults[i].second;
                bool filt=rx.indexIn(n)>=0;
                bool filtN=rxN.indexIn(n)>=0;
                bool ok=(filt||(!usefilt)) && ((!filtN)||(!usefiltN));

                if (!ok) {
                    lastResults.removeAt(i);
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

        reset();
        //qDebug()()<<"--- QFEvaluationResultsModel::resultsChanged() DONE: "<<t.elapsed();
    }

}

void QFEvaluationResultsModel::setResultFilter(QString filter)
{
    resultFilter=filter;

    if (evaluation) evaluation->setQFProperty("RESULTS_FILTER", filter);

    resultsChanged();
}

void QFEvaluationResultsModel::setFilesFilter(QString filter)
{
    filesFilter=filter;
    if (evaluation) evaluation->setQFProperty("FILES_FILTER", filter);
    resultsChanged();
}

void QFEvaluationResultsModel::setResultFilterNot(QString filter)
{
    resultFilterNot=filter;
    if (evaluation) evaluation->setQFProperty("RESULTS_FILTERNOT", filter);
    resultsChanged();
}

void QFEvaluationResultsModel::setFilesFilterNot(QString filter)
{
    filesFilterNot=filter;
    if (evaluation) evaluation->setQFProperty("FILES_FILTERNOT", filter);
    resultsChanged();
}

void QFEvaluationResultsModel::setFilesFilterUsesRegExp(bool use)
{
    filesFilterRegExp=use;
    if (evaluation) evaluation->setQFProperty("FILES_FILTER_REGEXP", use);
    resultsChanged();
}

void QFEvaluationResultsModel::setResultFilterUsesRegExp(bool use)
{
    resultFilterRegExp=use;
    if (evaluation) evaluation->setQFProperty("RESULTS_FILTER_REGEXP", use);
    resultsChanged();
}

void QFEvaluationResultsModel::setDisplayProperties(const QStringList &props) {
    displayProperties=props;
    resultsChanged();
}

QVariant QFEvaluationResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {

    // EXTRACO_L1, EXTRACOL_2, ... EXTRACOL_E | PROP_1, PROP_2, ... PROP_P | RESULT_1, RESULT_2, ..., RESULT_N
    //     0           1               E-1        E       E+1        E+P-1    E+P        E+P+1         E+P+N-1

    int extraID=section;
    int propID=section-getExtraColumns();
    int resnameID=section-getExtraColumns()-displayProperties.size();


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
            if (resnameID<0) {
                if (propID<0) {
                    if (extraID==0 && extractRuns) return tr("index/run");
                } else {
                    return QVariant(tr("prop: %1").arg(displayProperties.value(propID, "???")));
                }
            } else {
                return QVariant(lastResultLabels.value(resnameID, "???"));
            }

        }
    }
    return QVariant();
}

void QFEvaluationResultsModel::setShowVectorMatrixAvg(bool show)
{
    showVectorMatrixAvg=show;
    reset();
}

void QFEvaluationResultsModel::setExtractIndexes(bool enabled)
{
    extractRuns=enabled;
    resultsChanged();
}

void QFEvaluationResultsModel::setRemoveUnusedIndexes(bool enabled)
{
    removeUnusedRuns=enabled;
    if (extractRuns) resultsChanged();
}

void QFEvaluationResultsModel::setMinUnusedIndexesForStatistics(bool enabled)
{
    mindRunsForVectorStat=enabled;
    resultsChanged();
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
    return lastResultNames.size()+displayProperties.size()+getExtraColumns();
}

Qt::ItemFlags QFEvaluationResultsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFEvaluationResultsModel::data(const QModelIndex &index, int role) const {
    // EXTRACO_L1, EXTRACOL_2, ... EXTRACOL_E | PROP_1, PROP_2, ... PROP_P | RESULT_1, RESULT_2, ..., RESULT_N
    //     0           1               E-1        E       E+1        E+P-1    E+P        E+P+1         E+P+N-1

    int extraID=index.column();
    int propID=index.column()-getExtraColumns();
    int resnameID=index.column()-getExtraColumns()-displayProperties.size();

    int resI=index.row();

    //qDebug()<<"QFEvaluationResultsModel::data resI="<<resI<<"/"<<lastResults.size()<<"  extraID="<<extraID<<"  propID="<<propID<<"  resnameID="<<resnameID;

    if (!evaluation || !index.isValid()) return QVariant();
    if (resnameID<0) {
        if (propID<0) {
            if (extraID==0 && extractRuns && resI<lastResults.size()) { // return run column data
                QString en=lastResults[resI].second;
                int run=evaluation->getIndexFromEvaluationResultID(en);
                if (role==Qt::DisplayRole || role==Qt::EditRole || role==ValueRole || (role==AvgRole) || (role==SumRole) || (role==MedianRole)) {
                    return run;
                } else if ((role==Qt::ToolTipRole)||(role==Qt::StatusTipRole)) {
                    return QVariant(tr("the results in this row belong to run %1").arg(run));
                } else if (role==Qt::BackgroundColorRole) {
                    return QColor("lightgrey");
                }
            }
        } else {
            if (role==Qt::DisplayRole || role==Qt::EditRole || role==ValueRole || (role==AvgRole) || (role==SumRole) || (role==MedianRole)) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QString propname=displayProperties.value(propID, "");
                    if (record && !propname.isEmpty()) {
                        return record->getProperty(propname, QVariant());
                    }
                } else if (resI==lastResults.size()) {
                    return QVariant();
                }
            } else if (role==CountRole) {
                return 1;
            } else if (role==SDRole) {
                return 0;
            } else if ((role==Qt::ToolTipRole)||(role==Qt::StatusTipRole)) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QString propname=displayProperties.value(propID, "");
                    QVariant v=record->getProperty(propname, QVariant());
                    return QVariant(tr("property: %1<br>contents: %2<br><i>&nbsp;&nbsp;&nbsp;%3</i>").arg(propname).arg(v.toString()).arg(v.typeName()));
                }
            } else if ((role==NameRole)) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QString propname=displayProperties.value(propID, "");
                    return propname;
                }
            } else if (role==Qt::BackgroundColorRole) {
                return QColor("lightgrey");
            }
        }
    } else {
        if (role==Qt::DisplayRole) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ( (r.type!=QFRawDataRecord::qfrdreNumberVector) && (r.type!=QFRawDataRecord::qfrdreNumberMatrix) &&
                             (r.type!=QFRawDataRecord::qfrdreNumberErrorVector) && (r.type!=QFRawDataRecord::qfrdreNumberErrorMatrix) &&
                             (r.type!=QFRawDataRecord::qfrdreIntegerVector) && (r.type!=QFRawDataRecord::qfrdreIntegerMatrix) &&
                             (r.type!=QFRawDataRecord::qfrdreStringVector) && (r.type!=QFRawDataRecord::qfrdreStringMatrix) &&
                             (r.type!=QFRawDataRecord::qfrdreBooleanVector) && (r.type!=QFRawDataRecord::qfrdreBooleanMatrix) ) {
                            return QVariant(record->resultsGetAsString(en, rname).replace("+/-", "&plusmn;").replace(" micron/s", " &mu;m/s").replace(" um", " &mu;m").replace(" usecs", " &mu;s").replace(" usec", " &mu;s"));
                        } else {
                            if (showVectorMatrixAvg && r.isNumberType()) {
                                QString u=r.unit;
                                return QVariant(QString("<font color=\"darkblue\"><i>(%2&plusmn;%3) %4</i></font><br><font size\"-2\">[%1]</font>").arg(QFRawDataRecord::evaluationResultType2String(r.type))
                                                .arg(data(index, QFEvaluationResultsModel::AvgRole).toDouble()).arg(data(index, QFEvaluationResultsModel::SDRole).toDouble())
                                                .arg(u.replace(" um", " &mu;m").replace(" micron/s", " &mu;m/s").replace(" usecs", " &mu;s").replace(" usec", " &mu;s")));
                            } else {
                                if (r.getVectorMatrixItems()<=10) {
                                    return QVariant(record->resultsGetAsString(en,rname).replace("+/-", "&plusmn;").replace(" um", " &mu;m").replace(" usecs", " &mu;s").replace(" usec", " &mu;s"));
                                } else {
                                    return QVariant(QString("[")+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]"));
                                }
                            }
                        }
                    }
                }else if (resI==lastResults.size()) {
                    double average=0;
                    double stddev=0;
                    QString rname=lastResultNames[resnameID];
                    calcStatistics(rname, average, stddev);
                    return QVariant(QString("%1 %3 %2").arg(roundWithError(average, stddev)).arg(roundError(stddev)).arg(QChar(0xB1)));
                }
            }
        } else if (role==Qt::BackgroundColorRole) {
            if (resI==lastResults.size()) {
                return QColor("lightsteelblue");
            }
        } else if ((role==Qt::ToolTipRole)||(role==Qt::StatusTipRole)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        QString common=tr("<small><font color=\"darkgrey\"><i><br><br>result name: %2<br>evaluation name: %1<br>group: %3</i></font></small>").arg(en).arg(rname).arg(r.group);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            double var=0;
                            double mean=qfstatisticsMaskedAverageVariance(var, mask, r.dvec);
                            return QVariant(QString("mean: %1<br>S.D.: %2<br>count: %3<br><i>&nbsp;&nbsp;&nbsp;%4</i>").arg(mean).arg(sqrt(var)).arg(r.getVectorMatrixItems()).arg(QFRawDataRecord::evaluationResultType2String(r.type))+common);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            double var=0;
                            double mean=qfstatisticsMaskedAverageVariance(var, mask, r.ivec);
                            return QVariant(QString("mean: %1<br>S.D.: %2<br>count: %3<br><i>&nbsp;&nbsp;&nbsp;%4</i>").arg(mean).arg(sqrt(var)).arg(r.getVectorMatrixItems()).arg(QFRawDataRecord::evaluationResultType2String(r.type))+common);
                        } else if ((r.type==QFRawDataRecord::qfrdreBooleanVector) || (r.type==QFRawDataRecord::qfrdreBooleanMatrix) ) {
                            int cnt=0;
                            qCount(r.bvec, true, cnt);
                            return QVariant(QString("[%2]<br>item: %1<br>true items: %3").arg(r.getVectorMatrixItems()).arg(QFRawDataRecord::evaluationResultType2String(r.type)).arg(cnt)+common);
                        } else if (  (r.type==QFRawDataRecord::qfrdreStringVector) || (r.type==QFRawDataRecord::qfrdreStringMatrix) ) {
                            return QVariant(QString("[%2]<br>count: %1").arg(r.getVectorMatrixItems()).arg(QFRawDataRecord::evaluationResultType2String(r.type))+common);
                        } else if ((r.type==QFRawDataRecord::qfrdreString)) {
                            return QVariant(tr("contents: %2<br><i>&nbsp;&nbsp;&nbsp;%1</i>").arg(QFRawDataRecord::evaluationResultType2String(r.type)).arg(data(index, Qt::DisplayRole).toString())+common);
                        }
                        return QVariant(QString("[")+QFRawDataRecord::evaluationResultType2String(r.type)+QString("]")+common);
                    }
                }
            }
        } else if ((role==AvgRole)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return qfstatisticsMaskedAverage(mask, r.dvec);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return qfstatisticsMaskedAverage(mask, r.ivec);
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            return record->resultsGetAsDouble(en, rname);
                        }
                    }
                }
            }
        } else if ((role==SumRole)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return qfstatisticsMaskedSum(mask, r.dvec);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return qfstatisticsMaskedSum(mask, r.ivec);
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            return record->resultsGetAsDouble(en, rname);
                        } else {
                            return 0.0;
                        }
                    }
                }
            }
        } else if ((role==Sum2Role)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return qfstatisticsMaskedSum2(mask, r.dvec);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return qfstatisticsMaskedSum2(mask, r.ivec);
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            double v= record->resultsGetAsDouble(en, rname);
                            return v*v;
                        } else {
                            return 0.0;
                        }
                    }
                }
            }
        } else if ((role==CountRole)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return qfstatisticsMaskedCount(mask, r.dvec);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return qfstatisticsMaskedCount(mask, r.ivec);
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            return 1;
                        } else {
                            return 0;
                        }
                    }
                }
            }
        } else if ((role==SDRole)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return sqrt(qfstatisticsMaskedVariance(mask, r.dvec));
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return sqrt(qfstatisticsMaskedVariance(mask, r.ivec));
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            return record->resultsGetErrorAsDouble(en, rname);
                        } else {
                            if (r.type==QFRawDataRecord::qfrdreString) return record->resultsGetAsString(en, rname);
                        }
                    }
                }
            }
        } else if ((role==MedianRole)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return qfstatisticsMaskedMedian(mask, r.dvec);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return qfstatisticsMaskedMedian(mask, r.ivec);
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            return record->resultsGetErrorAsDouble(en, rname);
                        } else {
                            if (r.type==QFRawDataRecord::qfrdreString) return record->resultsGetAsString(en, rname);
                        }
                    }
                }
            }
        } else if ((role==Quantile25Role)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return qfstatisticsMaskedQuantile(mask, r.dvec, 0.25);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return qfstatisticsMaskedQuantile(mask, r.ivec, 0.25);
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            return 0;
                        } else {
                            if (r.type==QFRawDataRecord::qfrdreString) return record->resultsGetAsString(en, rname);
                        }
                    }
                }
            }
        } else if ((role==Quantile75Role)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
                    QVector<bool> mask;
                    if (runsel && mindRunsForVectorStat) mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        const QFRawDataRecord::evaluationResult& r=record->resultsGet(en, rname);
                        if ((r.type==QFRawDataRecord::qfrdreNumberVector) || (r.type==QFRawDataRecord::qfrdreNumberErrorVector)
                            || (r.type==QFRawDataRecord::qfrdreNumberMatrix) || (r.type==QFRawDataRecord::qfrdreNumberErrorMatrix) ) {
                            return qfstatisticsMaskedQuantile(mask, r.dvec, 0.75);
                        } else if ((r.type==QFRawDataRecord::qfrdreIntegerVector) || (r.type==QFRawDataRecord::qfrdreIntegerMatrix) ) {
                            return qfstatisticsMaskedQuantile(mask, r.ivec, 0.75);
                        } else if (r.type==QFRawDataRecord::qfrdreNumber || r.type==QFRawDataRecord::qfrdreNumberError || r.type==QFRawDataRecord::qfrdreInteger || r.type==QFRawDataRecord::qfrdreBoolean) {
                            return 0;
                        } else {
                            if (r.type==QFRawDataRecord::qfrdreString) return record->resultsGetAsString(en, rname);
                        }
                    }
                }
            }
        } else if ((role==ValueRole)||(role==Qt::EditRole)) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    if (record) {
                        return record->resultsGetAsQVariant(en, rname);
                    }
                }
            }

        } else if (role==EvalNameRole) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QString en=lastResults[resI].second;
                    //QString rname=lastResultNames[resNameI];
                    return en;
                }
            }
        } else if (role==ResultNameRole) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    //QString en=lastResults[resI].second;
                    QString rname=lastResultNames[resnameID];
                    return rname;
                }
            }

        } else if (role==ResultIDRole) {
            if (resnameID<lastResultNames.size()) {
                if (resI<lastResults.size()) {
                    QFRawDataRecord* record=lastResults[resI].first;
                    if (record) {
                        return record->getID();
                    }
                }
            }
            return -1;

        } else if (role==NameRole) {
            if (resnameID<lastResultNames.size()) {
                return QVariant(lastResultNames[resnameID]);
            }
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
        QFRDRRunSelectionsInterface* runsel=dynamic_cast<QFRDRRunSelectionsInterface*>(record);
        QString en=lastResults[i].second;
        if (record) {
            QFRawDataRecord::evaluationResultType t=record->resultsGetType(en, resultName);
            if (t==QFRawDataRecord::qfrdreNumberVector||t==QFRawDataRecord::qfrdreNumberMatrix||t==QFRawDataRecord::qfrdreNumberErrorVector||t==QFRawDataRecord::qfrdreNumberErrorMatrix
                ||t==QFRawDataRecord::qfrdreIntegerVector||t==QFRawDataRecord::qfrdreIntegerMatrix) {

                QVector<double> d=record->resultsGetAsDoubleList(en, resultName);
                QVector<bool> mask;
                if (runsel && d.size()==runsel->leaveoutGetRunCount()) {
                    mask=QFRDRRunSelectionsInterface_getRunSelectionAsBoolVec(runsel);
                }
                if (mindRunsForVectorStat && mask.size()>=d.size()){
                    sum=sum+qfstatisticsMaskedSum(mask,d);
                    sum2=sum2+qfstatisticsMaskedSum2(mask,d);
                    count=count+qfstatisticsMaskedCount(mask,d);
                } else {
                    sum=sum+qfstatisticsSum(d);
                    sum2=sum2+qfstatisticsSum2(d);
                    count=count+qfstatisticsCount(d);
                }
                /*for (int di=0; di<d.size(); di++) {
                    sum=sum+d[di];
                    sum2=sum2+d[di]*d[di];
                }
                count=count+d.size();*/

            } else {
                bool ok=false;
                double value=record->resultsGetAsDouble(en, resultName, &ok);
                if (ok) {
                    sum=sum+value;
                    sum2=sum2+value*value;
                    count=count+1;
                }
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

int QFEvaluationResultsModel::getExtraColumns() const
{
    int extraColumns=0;
    if (extractRuns) extraColumns++;
    return extraColumns;
}
