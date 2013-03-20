#include "qfprojectrawdatamodel.h"

#include "qfrawdatarecord.h"

QFProjectRawDataModel::QFProjectRawDataModel(QFProject* parent):
    QAbstractListModel(parent)
{
    item = parent;
    defaultMimeType="application/qf2.ids.int.list";
    if (item) connect(item, SIGNAL(structureChanged()), this , SLOT(projectChanged()));
    //connect(item, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));
}

QFProjectRawDataModel::~QFProjectRawDataModel()
{
    if (item) disconnect(item, SIGNAL(structureChanged()), this , SLOT(projectChanged()));
    //disconnect(item, SIGNAL(wasChanged(bool)), this , SLOT(projectChanged(bool)));
}

void QFProjectRawDataModel::setProject(QFProject* parent) {
    if (item) disconnect(item, SIGNAL(structureChanged()), this , SLOT(projectChanged()));

    item = parent;
    if (item) connect(item, SIGNAL(structureChanged()), this , SLOT(projectChanged()));
    reset();
}

void QFProjectRawDataModel::projectChanged(bool data) {
    QTime t;
    t.start();
    //qDebug()<<"QFProjectRawDataModel::projectChanged()";
    reset();
    //qDebug()<<"QFProjectRawDataModel::projectChanged() end: "<<t.elapsed()<<" ms";
}

QVariant QFProjectRawDataModel::data(const QModelIndex &index, int role) const {
    if ((!index.isValid()) || (item==NULL)) return QVariant();
    if (role==Qt::DisplayRole) {
        QFRawDataRecord* rec=item->getRawDataByNum(index.row());
        if (rec) return QVariant(rec->getName());
    } else if (role==Qt::DecorationRole) {
        QFRawDataRecord* rec=item->getRawDataByNum(index.row());
        if (rec) return QVariant(rec->getSmallIcon());
    } else if (role==Qt::UserRole) {
        QFRawDataRecord* rec=item->getRawDataByNum(index.row());
        if (rec) return QVariant(rec->getID());
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



