#ifndef QFRDRPROPERTYMODEL_H
#define QFRDRPROPERTYMODEL_H

#include <QAbstractTableModel>
#include "lib_imexport.h"
#include <QStringList>


// forward declaration
class QFRawDataRecord;


/*! \brief this class serves as a QAbstractTableModel for the properties associated with a
           QFRawDataRecord.
    \ingroup qf3lib_project
*/
class QFLIB_EXPORT QFRDRPropertyModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRPropertyModel();
        /** Default destructor */
        virtual ~QFRDRPropertyModel();

        void init(QFRawDataRecord* record);

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    private slots:
        void propertiesChanged(const QString& property, bool visible);
    protected:
        QFRawDataRecord* record;

        void updateModel(bool doReset=true);

        QStringList props;
};

#endif // QFRDRPROPERTYMODEL_H
