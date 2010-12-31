#include "qfprojecttreemodel.h"
#include "qfproject.h"

QFProjectTreeModel::QFProjectTreeModel(QObject* parent):
    QAbstractItemModel(parent)
{
    current=NULL;
}

QFProjectTreeModel::~QFProjectTreeModel()
{
    //dtor
}

void QFProjectTreeModel::init(QFProject* p) {
    if (current) disconnect(current, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));
    current=p;
    connect(current, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));
    reset();
}

int QFProjectTreeModel::columnCount ( const QModelIndex & parent ) const {
    //std::cout<<"columnCount(???)=1\n";
    return 1;
}

int QFProjectTreeModel:: rowCount ( const QModelIndex & parent) const {
    if (!current) return 0;

    nodeType nt=classifyIndex(parent);
    //std::cout<<"rowCount(parent_index="<<nodeType2String(nt).toStdString()<<", parent_row="<<parent.row()<<")\n";
    switch(nt) {
        case qfpntUnknown:
            //std::cout<<"=0\n";
            return 0;
        case qfpntRoot:
            //std::cout<<"=1\n";
            return 1;
        case qfpntProject: //
            //std::cout<<"=1\n";
            return 2;
        case qfpntRawDataDir:
            //std::cout<<tr("=%1\n").arg(rawData.size()).toStdString();
            return current->getRawDataCount(); //rawData.size();
        case qfpntRawDataRecord:
            //std::cout<<"=1\n";
            return 0;
        case qfpntEvaluationDir:
            //std::cout<<tr("=%1\n").arg(rawData.size()).toStdString();
            return current->getEvaluationCount(); //evaluations.size();
        case qfpntEvaluationRecord:
            //std::cout<<"=1\n";
            return 0;//getEvaluationByIndex(parent)->getDataRecordCount();
    };
    //std::cout<<"=0\n";
    return 0;
}

Qt::ItemFlags QFProjectTreeModel::flags(const QModelIndex &index) const {
    if (!current) return 0;

    nodeType nt=classifyIndex(index);
    //std::cout<<"flags(index="<<nodeType2String(nt).toStdString()<<", row="<<index.row()<<")\n";
    switch(nt) {
        case qfpntUnknown:
            return 0;
        case qfpntRoot:
            return 0;
        case qfpntProject:
            return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable;;
        case qfpntRawDataDir:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        case qfpntRawDataRecord:
            return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable;;
        case qfpntEvaluationDir:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        case qfpntEvaluationRecord:
            return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable;;
    }
    return 0;
 }

QVariant QFProjectTreeModel::data ( const QModelIndex & index, int role ) const {
    if (!current) return QVariant();

    nodeType nt=classifyIndex(index);
    //std::cout<<"data(index="<<nodeType2String(nt).toStdString()<<", row="<<index.row()<<")\n";

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(nt) {
            case qfpntUnknown:
                return QVariant();
            case qfpntRoot:
                return QVariant();
            case qfpntProject:
                return QVariant(current->getName());
            case qfpntRawDataDir:
                return QVariant(tr("Raw Data"));
            case qfpntRawDataRecord:
                if (current->getRawDataByNum(index.row())) return QVariant(current->getRawDataByNum(index.row())->getName());
            case qfpntEvaluationDir:
                return QVariant(tr("Evaluations"));
            case qfpntEvaluationRecord:
                if (current->getEvaluationByNum(index.row())) return QVariant(current->getEvaluationByNum(index.row())->getName());
        };
    } else if (role == Qt::DecorationRole) {
        switch(nt) {
            case qfpntUnknown:
                return QVariant();
            case qfpntRoot:
                return QVariant();
            case qfpntProject:
                return QVariant(QIcon(":/projecttree_folder.png"));
            case qfpntRawDataDir:
            case qfpntEvaluationDir:
                return QVariant(QIcon(":/projecttree_folderopen.png"));
            case qfpntRawDataRecord:
                if (current->getRawDataByNum(index.row())) return current->getRawDataByNum(index.row())->getSmallIcon();
            case qfpntEvaluationRecord:
                if (current->getEvaluationByNum(index.row())) return current->getEvaluationByNum(index.row())->getSmallIcon();
        };
    }
    return QVariant();
}

bool QFProjectTreeModel::setData (const QModelIndex &index, const QVariant &value, int role) {
    if (!current) return false;
    if (role != Qt::EditRole) return false;

    bool result = false;

    nodeType nt=classifyIndex(index);
    //std::cout<<"setData(index="<<nodeType2String(nt).toStdString()<<", row="<<index.row()<<")\n";

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(nt) {
            case qfpntUnknown:
                break;
            case qfpntRoot:
                break;
            case qfpntProject:
                current->setName(value.toString());
                result=true;
                break;
            case qfpntRawDataDir:
                break;
            case qfpntRawDataRecord:
                if (current->getRawDataByNum(index.row())) current->getRawDataByNum(index.row())->setName(value.toString());
                result=true;
                break;
            case qfpntEvaluationDir:
                break;
            case qfpntEvaluationRecord:
                if (current->getEvaluationByNum(index.row())) current->getEvaluationByNum(index.row())->setName(value.toString());
                result=true;
                break;
        };
    }
    /*if (result) {
        dataChange=true;
        emit wasChanged(dataChange);
        emit dataChanged(index, index);
    }*/
    return result;
}

QFRawDataRecord* QFProjectTreeModel::getRawDataByIndex(const QModelIndex& index) const {
    if (!current) return NULL;
    nodeType nt=classifyIndex(index);
    //std::cout<<"getRawDataByIndex(index="<<nodeType2String(nt).toStdString()<<", row="<<index.row()<<")\n";
    if (nt==qfpntRawDataRecord) {
        return current->getRawDataByNum(index.row());
    }
    return NULL;
}

QFEvaluationItem* QFProjectTreeModel::getEvaluationByIndex(const QModelIndex& index) const {
    if (!current) return NULL;
    nodeType nt=classifyIndex(index);
    //std::cout<<"getEvaluationByIndex(index="<<nodeType2String(nt).toStdString()<<", row="<<index.row()<<")\n";
    if (nt==qfpntEvaluationRecord) {
        return current->getEvaluationByNum(index.row());
    }
    return NULL;
}

QModelIndex QFProjectTreeModel::index ( int row, int column, const QModelIndex & parent) const {
    if (!current) return QModelIndex();
    nodeType nt=classifyIndex(parent);
    //std::cout<<tr("index(row=%1, column=%2, parent=%3)\n").arg(row).arg(column).arg(nodeType2String(nt)).toStdString();
    switch(nt) {
        case qfpntUnknown: return QModelIndex();
        case qfpntRoot: return createIndex(0,0,qfpntProject);
        case qfpntProject: {
            if (row==0) return createIndex(row,0,qfpntRawDataDir);
            if (row==1) return createIndex(row,0,qfpntEvaluationDir);
        }
        case qfpntRawDataDir: return createIndex(row,0,qfpntRawDataRecord);
        case qfpntRawDataRecord: return QModelIndex();
        case qfpntEvaluationDir: return createIndex(row,0,qfpntEvaluationRecord);
        case qfpntEvaluationRecord: return QModelIndex();
    };
    return QModelIndex();
}

QModelIndex QFProjectTreeModel::index ( QFRawDataRecord* record, int column ) const {
    if (!current || !record || column>0) return QModelIndex();
    if (!current->rawDataExists(record)) return QModelIndex();
    return createIndex(current->getRawDataIndex(record), 0, qfpntRawDataRecord);
}

QModelIndex QFProjectTreeModel::index ( QFEvaluationItem* record, int column ) const {
    if (!current || !record || column>0) return QModelIndex();
    if (!current->evaluationExists(record)) return QModelIndex();
    return createIndex(current->getEvaluationIndex(record), 0, qfpntEvaluationRecord);
}

QModelIndex QFProjectTreeModel::parent ( const QModelIndex & index ) const {
    if (!current) return QModelIndex();
    nodeType nt=classifyIndex(index);
    //std::cout<<"parent(index="<<nodeType2String(nt).toStdString()<<", row="<<index.row()<<")\n";
    switch(nt) {
        case qfpntUnknown: return QModelIndex();
        case qfpntRoot: return QModelIndex();
        case qfpntProject: return QModelIndex();
        case qfpntRawDataDir: return createIndex(0, 0, qfpntProject);
        case qfpntRawDataRecord: return createIndex(0,0, qfpntRawDataDir);
        case qfpntEvaluationDir: return createIndex(0, 0, qfpntProject);
        case qfpntEvaluationRecord: return createIndex(1,0, qfpntEvaluationDir);
    };
    return QModelIndex();
}

QFProjectTreeModel::nodeType QFProjectTreeModel::classifyIndex(const QModelIndex& index) const {
    if (!index.isValid()) return qfpntRoot;

    switch(index.internalId()) {
        case qfpntUnknown: return qfpntUnknown;
        case qfpntRoot: return qfpntRoot;
        case qfpntProject: return qfpntProject;
        case qfpntRawDataDir: return qfpntRawDataDir;
        case qfpntRawDataRecord: return qfpntRawDataRecord;
        case qfpntEvaluationDir: return qfpntEvaluationDir;
        case qfpntEvaluationRecord: return qfpntEvaluationRecord;
    }
    return qfpntUnknown;
}

QString QFProjectTreeModel::nodeType2String( const QFProjectTreeModel::nodeType nt) const {
    switch(nt) {
        case qfpntUnknown: return "unknown";
        case qfpntRoot: return "root";
        case qfpntProject: return "project";
        case qfpntRawDataDir: return "rawDataDir";
        case qfpntRawDataRecord: return "rawDataRecord";
        case qfpntEvaluationDir: return "evaluationDir";
        case qfpntEvaluationRecord: return "evaluationRecord";
    };
    return "invalid";
}

void QFProjectTreeModel::projectChanged(bool data) {
    reset();
}

