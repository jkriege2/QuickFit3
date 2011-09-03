#ifndef QFRDRRESULTSMODEL_H
#define QFRDRRESULTSMODEL_H

#include "lib_imexport.h"
#include <QAbstractTableModel>
#include <QStringList>

// forward declaration
class QFRawDataRecord;

/*! \brief class to display the results stored in QFRawDataRecord
    \ingroup qf3lib_project

*/
class QFLIB_EXPORT QFRDRResultsModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRResultsModel(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRDRResultsModel();

        void init(QFRawDataRecord* record);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        enum {
            ValueRole=Qt::UserRole,
            NameRole=Qt::UserRole+1
        };
    private slots:
        void resultsChanged();
    protected:
        QFRawDataRecord* record;
    private:

        /** \brief calculate average and standard deviation over all result for a given result name */
        void calcStatistics(QString resultName, double& average, double& stddev) const;

        /** \brief current list of all shown result names (updated on model reset, using calcResultNames() */
        QStringList lastResultNames;
};

#endif // QFRDRRESULTSMODEL_H
