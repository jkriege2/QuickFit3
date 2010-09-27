#ifndef QFRDRRESULTSMODEL_H
#define QFRDRRESULTSMODEL_H

#include <QAbstractTableModel>

// forward declaration
class QFRawDataRecord;

/*! \brief class to display the results stored in QFRawDataRecord

*/
class QFRDRResultsModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRResultsModel();
        /** Default destructor */
        virtual ~QFRDRResultsModel();

        void init(QFRawDataRecord* record);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    private slots:
        void resultsChanged();
    protected:
        QFRawDataRecord* record;
    private:
        QList<QString> calcResultNames() const;
};

#endif // QFRDRRESULTSMODEL_H
