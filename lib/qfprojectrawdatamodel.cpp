#include "qfprojectrawdatamodel.h"

#include "qfrawdatarecord.h"

QFProjectRawDataModel::QFProjectRawDataModel(QFProject* parent):
    QAbstractListModel(parent)
{
    item = parent;
    defaultMimeType="application/qf2.ids.int.list";
    connect(item, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));
}

QFProjectRawDataModel::~QFProjectRawDataModel()
{
    disconnect(item, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));
}

void QFProjectRawDataModel::setProject(QFProject* parent) {
    if (item) disconnect(item, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));

    item = parent;
    if (item) connect(item, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));
    reset();
}

void QFProjectRawDataModel::projectChanged(bool data) {
    reset();
}

QVariant QFProjectRawDataModel::data(const QModelIndex &index, int role) const {
    if ((!index.isValid()) || (item==NULL)) return QVariant();
    if (role==Qt::DisplayRole) {
        if (index.internalId()>=0) {
            QFRawDataRecord* rec=item->getRawDataByNum(index.row());
            return QVariant(rec->getName());
        } else {
            if (index.row()==0) return QVariant(tr("Raw Data"));
        }
    } else if (role==Qt::DecorationRole) {
        if (index.internalId()>=0) {
            QFRawDataRecord* rec=item->getRawDataByNum(index.row());
            return QVariant(rec->getSmallIcon());
        }
    } else if (role==Qt::UserRole) {
        if (index.internalId()>=0) {
            QFRawDataRecord* rec=item->getRawDataByNum(index.row());
            return QVariant(rec->getID());
        }
    }
    return QVariant();
}

Qt::ItemFlags QFProjectRawDataModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags =Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    return defaultFlags;
}

QVariant QFProjectRawDataModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}


int QFProjectRawDataModel::rowCount(const QModelIndex &parent) const {
    if (item!=NULL) {
        return item->getRawDataCount();
    }
    return 0;
};

int QFProjectRawDataModel::columnCount(const QModelIndex &parent) const {
    return 1;
};



