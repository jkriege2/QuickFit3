/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "qfrdrimagingfcsrunsmodel.h"
#include "qfrdrimagingfcs_data.h"

QFRDRImagingFCSRunsModel::QFRDRImagingFCSRunsModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    current=NULL;
}




void QFRDRImagingFCSRunsModel::setCurrent(QFRawDataRecord* current) {
    QTime t;
    t.start();
    //qDebug()<<" QFRDRImagingFCSRunsModel::setCurrent()";
    this->current=current;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
    //qDebug()<<" QFRDRImagingFCSRunsModel::setCurrent() ... done "<<t.elapsed();
}


QVariant QFRDRImagingFCSRunsModel::data(const QModelIndex &index, int role) const {
    if (!current || !index.isValid()) return QVariant();
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (role==Qt::BackgroundColorRole) {
        QPalette pal;
        if (index.row()>0) {
            if (m->leaveoutRun(index.row()-1)) return pal.color(QPalette::Window).darker(130);
            return pal.color(QPalette::Base);
        }
        return pal.color(QPalette::Base);
    } else if (role==Qt::DisplayRole) {
        if (index.row()==0) return tr("none");
        return m->getCorrelationRunName(index.row()-1);//tr("Run %1").arg(index.row()-1);
    }
    return QVariant();
}

Qt::ItemFlags QFRDRImagingFCSRunsModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFRDRImagingFCSRunsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant();
}

int QFRDRImagingFCSRunsModel::rowCount(const QModelIndex &parent) const {
    if (!current) return 0;
    QFRDRImagingFCSData* m=qobject_cast<QFRDRImagingFCSData*>(current);
    if (m) return 1+m->getCorrelationRuns();
    return 1;
}

int QFRDRImagingFCSRunsModel::columnCount(const QModelIndex &parent) const {
    return 1;
}
