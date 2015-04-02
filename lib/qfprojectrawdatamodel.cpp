/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfprojectrawdatamodel.h"

#include "qfrawdatarecord.h"

QFProjectRawDataModel::QFProjectRawDataModel(QObject* parentObj, QFProject* parent):
    QAbstractListModel(parentObj)
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QFProjectRawDataModel::projectChanged(bool data) {
    Q_UNUSED(data);
    QTime t;
    t.start();
    //qDebug()<<"QFProjectRawDataModel::projectChanged()";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
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
    Q_UNUSED(index);
    Qt::ItemFlags defaultFlags =Qt::ItemIsSelectable|Qt::ItemIsEnabled;
    return defaultFlags;
}

QVariant QFProjectRawDataModel::headerData(int section, Qt::Orientation orientation, int role) const {
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}


int QFProjectRawDataModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    if (item!=NULL) {
        return item->getRawDataCount();
    }
    return 0;
};

int QFProjectRawDataModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return 1;
};



