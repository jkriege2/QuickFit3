#ifndef QFEVALUATIONRESULTSMODEL_H
#define QFEVALUATIONRESULTSMODEL_H

#include "lib_imexport.h"
#include <QAbstractTableModel>
#include <QStringList>
#include <QPointer>
#include <QRegExp>

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
            NameRole=Qt::UserRole+1,
            AvgRole=Qt::UserRole+2,
            SDRole=Qt::UserRole+3,
            SumRole=Qt::UserRole+4,
            Sum2Role=Qt::UserRole+5,
            CountRole=Qt::UserRole+6
        };


    public slots:
        void resultsChanged(QFRawDataRecord* record=NULL, const QString& evaluationName=QString(""), const QString& resultName=QString(""));
        void setResultFilter(QString filter);
        void setFilesFilter(QString filter);
        void setResultFilterNot(QString filter);
        void setFilesFilterNot(QString filter);
        void setFilesFilterUsesRegExp(bool use);
        void setResultFilterUsesRegExp(bool use);
    protected:
        QFEvaluationItem* evaluation;
        QString evalFilter;

        void calcStatistics(QString resultName, double& average, double& stddev) const;
    private:

        /** \brief calculate average and standard deviation over all result for a given result name */
        //void calcStatistics(QString resultName, double& average, double& stddev) const;

        /** \brief current list of all shown result names (updated on model reset, using calcResultNames() */
        QStringList lastResultNames;
        QStringList lastResultLabels;
        QList<QPair<QPointer<QFRawDataRecord>, QString> > lastResults;
        QMap<QString, QString> resultGroups;

        QString resultFilter;
        QString filesFilter;
        QString resultFilterNot;
        QString filesFilterNot;
        bool resultFilterRegExp;
        bool filesFilterRegExp;
};

#endif // QFEVALUATIONRESULTSMODEL_H
