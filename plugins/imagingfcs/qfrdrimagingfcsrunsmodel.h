#ifndef QFRDRIMAGINGFCSRUNSMODEL_H
#define QFRDRIMAGINGFCSRUNSMODEL_H

#include <QAbstractTableModel>
#include "qfrawdatarecord.h"

/*! \brief This model contains all runs in a QFRawDataRecord
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImagingFCSRunsModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSRunsModel(QObject *parent = 0);
        void setCurrent(QFRawDataRecord* current);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    protected:
        QFRawDataRecord* current;
};


#endif // QFRDRIMAGINGFCSRUNSMODEL_H
