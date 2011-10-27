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
    qDebug()<<" QFRDRImagingFCSRunsModel::setCurrent()";
    this->current=current;
    reset();
    qDebug()<<" QFRDRImagingFCSRunsModel::setCurrent() ... done "<<t.elapsed();
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
