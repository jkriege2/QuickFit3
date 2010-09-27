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
    reset();
}

void QFRDRResultsModel::init(QFRawDataRecord* record) {
    setParent(record);
    this->record=record;
    disconnect();
    connect(record, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
}

int QFRDRResultsModel::rowCount(const QModelIndex &parent ) const {
    if (!record) return 0;
    QList<QString> l=calcResultNames();
    //std::cout<<"result row count="<<l.size()<<std::endl;
    return l.size();
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
        QList<QString> l=calcResultNames();
        QString en=record->resultsGetEvaluationName(index.column());
        if (index.row()<l.size()) if (record->resultsExists(en, l[index.row()])) {
            QFRawDataRecord::evaluationResult r=record->resultsGet(en, l[index.row()]);
            return QVariant(record->resultsGetAsString(en, l[index.row()]));
        }
    } else if (role==Qt::BackgroundRole) {
        QPalette pal;
        if (index.row()%2) return pal.color(QPalette::AlternateBase);
        return pal.color(QPalette::Base);
    }

    return QVariant();
}

QVariant QFRDRResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (!record) return QVariant();
    if (role==Qt::DisplayRole) {
        if (orientation==Qt::Horizontal) {
            return QVariant(record->resultsGetEvaluationName(section));
        } else {
            QList<QString> l=calcResultNames();
            if (section<l.size()) return QVariant(l[section]);
        }
    }
    return QVariant();
}

QList<QString> QFRDRResultsModel::calcResultNames() const {
    QList<QString> l;
    for (int i=0; i<record->resultsGetEvaluationCount(); i++) {
        QString en=record->resultsGetEvaluationName(i);
        for (int j=0; j<record->resultsGetCount(en); j++) {
            QString rn=record->resultsGetResultName(en, j);
            if (!l.contains(rn)) {
                l.append(rn);
            }
        }
    }
    return l;
}

