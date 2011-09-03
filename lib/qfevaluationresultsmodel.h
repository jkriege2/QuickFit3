#ifndef QFEVALUATIONRESULTSMODEL_H
#define QFEVALUATIONRESULTSMODEL_H

#include "lib_imexport.h"
#include <QAbstractTableModel>
#include <QStringList>
#include <QPointer>

// forward declaration
class QFRawDataRecord;
class QFEvaluationItem;
class QFProject;

/*! \brief class to display the results stored in all QFRawDataRecord for a given QFEvaluationItem
    \ingroup qf3lib_project

*/
class QFLIB_EXPORT QFEvaluationResultsModel : public QAbstractTableModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEvaluationResultsModel(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEvaluationResultsModel();

        void init(QFEvaluationItem* evaluation, QString evalFilter);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        enum {
            ValueRole=Qt::UserRole,
            NameRole=Qt::UserRole+1
        };
    public slots:
        void resultsChanged();
    protected:
        QFEvaluationItem* evaluation;
        QString evalFilter;

        void calcStatistics(QString resultName, double& average, double& stddev) const;
    private:

        /** \brief calculate average and standard deviation over all result for a given result name */
        //void calcStatistics(QString resultName, double& average, double& stddev) const;

        /** \brief current list of all shown result names (updated on model reset, using calcResultNames() */
        QStringList lastResultNames;
        QList<QPair<QPointer<QFRawDataRecord>, QString> > lastResults;
};

#endif // QFEVALUATIONRESULTSMODEL_H
