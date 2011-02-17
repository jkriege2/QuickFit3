#ifndef QFRDRRESULTSMODEL_H
#define QFRDRRESULTSMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

// forward declaration
class QFRawDataRecord;

/*! \brief class to display the results stored in QFRawDataRecord
    \ingroup qf3lib_project

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

        enum {
            ValueRole=Qt::UserRole
        };
    private slots:
        void resultsChanged();
    protected:
        QFRawDataRecord* record;
    private:
        /** \brief return a list of all result names shown in this model */
        virtual QList<QString> calcResultNames() const;

        /** \brief calculate average and standard deviation over all result for a given result name */
        void calcStatistics(QString resultName, double& average, double& stddev) const;

        /** \brief current list of all shown result names (updated on model reset, using calcResultNames() */
        QStringList lastResultNames;
};

#endif // QFRDRRESULTSMODEL_H
