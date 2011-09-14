#include "qfrdrresultsmodel.h"
#include "qfrawdatarecord.h"


QFRDRResultsModel::QFRDRResultsModel(QObject* parent):
    QAbstractTableModel(parent)
{
    record=NULL;
}

QFRDRResultsModel::~QFRDRResultsModel()
{
    //dtor
}

void QFRDRResultsModel::resultsChanged() {
    /*if (record) lastResultNames=record->resultsCalcNames();
    else lastResultNames.clear();*/
    QList<QPair<QString, QString> > l=record->resultsCalcNamesAndLabels();
    lastResultNames.clear();
    lastResultLabels.clear();
    for (int i=0; i<l.size(); i++) {
        lastResultNames.append(l[i].second);
        if (l[i].first.isEmpty()) lastResultLabels.append(l[i].second);
        else lastResultLabels.append(l[i].first);
    }
    reset();
}

void QFRDRResultsModel::init(QFRawDataRecord* record) {
    disconnect();
    setParent(record);
    this->record=record;
    QList<QPair<QString, QString> > l=record->resultsCalcNamesAndLabels();
    lastResultNames.clear();
    lastResultLabels.clear();
    for (int i=0; i<l.size(); i++) {
        lastResultNames.append(l[i].second);
        if (l[i].first.isEmpty()) lastResultLabels.append(l[i].second);
        else lastResultLabels.append(l[i].first);
    }
/*    if (record) lastResultNames=record->resultsCalcNames();
    else lastResultNames.clear();*/
    connect(record, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    reset();
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
    return record->resultsGetEvaluationCount()+1;
}

Qt::ItemFlags QFRDRResultsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRResultsModel::data(const QModelIndex &index, int role) const {
    if (!record || !index.isValid()) return QVariant();
    if (role==Qt::DisplayRole) {
        if (index.row()<lastResultNames.size()) {
            if (index.column()<record->resultsGetEvaluationCount()) {
                QString en=record->resultsGetEvaluationName(index.column());
                if (record->resultsExists(en, lastResultNames[index.row()])) {
                    QFRawDataRecord::evaluationResult r=record->resultsGet(en, lastResultNames[index.row()]);
                    return QVariant(record->resultsGetAsString(en, lastResultNames[index.row()]).replace("+/-", "&plusmn;").replace(" um", " &mu;m").replace(" usecs", " &mu;s").replace(" usec", " &mu;s"));
                }
            } else if (index.column()==record->resultsGetEvaluationCount()) {
                QString rname=lastResultNames[index.row()];
                double average=0;
                double stddev=0;
                calcStatistics(rname, average, stddev);
                return QVariant(QString("%1 &plusmn; %2").arg(roundWithError(average, stddev)).arg(roundError(stddev)));
            }
        }
    } else if ((role==ValueRole)||(role==Qt::EditRole)) {
        if (index.row()<lastResultNames.size()) {
            if (index.column()<record->resultsGetEvaluationCount()) {
                QString en=record->resultsGetEvaluationName(index.column());
                if (record->resultsExists(en, lastResultNames[index.row()])) {
                    return record->resultsGetAsQVariant(en, lastResultNames[index.row()]);
                }
            }
        }
    } else if (role==NameRole) {
        if (index.row()<lastResultNames.size()) {
            return QVariant(lastResultNames[index.row()]);
        }
    }

    return QVariant();
}

QVariant QFRDRResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!record) return QVariant();
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Horizontal) {
            if (section<record->resultsGetEvaluationCount()) return QVariant(record->resultsGetEvaluationDescription(record->resultsGetEvaluationName(section)));
            else return tr("Avg +/- StdDev"); //tr("&lang;val&rang; &plusmn; &sigma<sub>val</sub>");
        } else {
            if (section<lastResultLabels.size()) return QVariant(lastResultLabels[section]);
        }
    }
    return QVariant();
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
