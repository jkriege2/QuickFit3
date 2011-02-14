#include "qfrdrresultsmodel.h"
#include "qfrawdatarecord.h"


QFRDRResultsModel::QFRDRResultsModel():
    QAbstractTableModel(NULL)
{
    record=NULL;
}

QFRDRResultsModel::~QFRDRResultsModel()
{
    //dtor
}

void QFRDRResultsModel::resultsChanged() {
    lastResultNames=calcResultNames();
    reset();
}

void QFRDRResultsModel::init(QFRawDataRecord* record) {
    disconnect();
    setParent(record);
    this->record=record;
    lastResultNames=calcResultNames();
    connect(record, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    reset();
}

int QFRDRResultsModel::rowCount(const QModelIndex &parent ) const {
    if (!record) return 0;
    //QList<QString>& l=lastResultNames;
    //std::cout<<"result row count="<<l.size()<<std::endl;
    return lastResultNames.size();
}

int QFRDRResultsModel::columnCount(const QModelIndex &parent) const {
    if (!record) return 0;
    //std::cout<<"result column count="<<record->resultsGetEvaluationCount()<<std::endl;
    return record->resultsGetEvaluationCount();
}

Qt::ItemFlags QFRDRResultsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRResultsModel::data(const QModelIndex &index, int role) const {
    if (!record || !index.isValid()) return QVariant();
    if (role==Qt::DisplayRole) {
        //QList<QString>& l=lastResultNames;//calcResultNames();
        if (index.row()<lastResultNames.size()) {
            QString en=record->resultsGetEvaluationName(index.column());
            if (record->resultsExists(en, lastResultNames[index.row()])) {
                QFRawDataRecord::evaluationResult r=record->resultsGet(en, lastResultNames[index.row()]);
                return QVariant(record->resultsGetAsString(en, lastResultNames[index.row()]));
            }
        }
    } else if (role==ValueRole) {
        if (index.row()<lastResultNames.size()) {
            QString en=record->resultsGetEvaluationName(index.column());
            if (record->resultsExists(en, lastResultNames[index.row()])) {
                return record->resultsGetAsQVariant(en, lastResultNames[index.row()]);
            }
        }
    }

    return QVariant();
}

QVariant QFRDRResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!record) return QVariant();
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Horizontal) {
            return QVariant(record->resultsGetEvaluationName(section));
        } else {
            //QList<QString>& l=lastResultNames;//calcResultNames();
            if (section<lastResultNames.size()) return QVariant(lastResultNames[section]);
        }
    }
    return QVariant();
}

QList<QString> QFRDRResultsModel::calcResultNames() const {
    QList<QString> l;
    int evalCount=record->resultsGetEvaluationCount();
    for (unsigned int i=0; i<evalCount; i++) {
        QString en=record->resultsGetEvaluationName(i);
        int jmax=record->resultsGetCount(en);
        for (unsigned int j=0; j<jmax; j++) {
            QString rn=record->resultsGetResultName(en, j);
            if (!l.contains(rn)) {
                l.append(rn);
            }
        }
    }
    return l;
}

