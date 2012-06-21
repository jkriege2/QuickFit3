#ifndef QFRDRRESULTSMODEL_H
#define QFRDRRESULTSMODEL_H

#include "lib_imexport.h"
#include <QAbstractTableModel>
#include <QStringList>

// forward declaration
class QFRawDataRecord;

/*! \brief class to display the results stored in QFRawDataRecord
    \ingroup qf3lib_project

    This model is meant to be used together with QFHTMLDelegate

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
            NameRole=Qt::UserRole+1,
            AvgRole=Qt::UserRole+2,
            SDRole=Qt::UserRole+3
        };
    public slots:
        void setResultFilter(QString filter);
        void setEvaluationFilter(QString filter);
        void setResultFilterNot(QString filter);
        void setEvaluationFilterNot(QString filter);
        void setEvaluationFilterUsesRegExp(bool use);
        void setResultFilterUsesRegExp(bool use);
    private slots:
        void resultsChanged();
    protected:
        QFRawDataRecord* record;
    private:

        /** \brief calculate average and standard deviation over all result for a given result name */
        void calcStatistics(QString resultName, double& average, double& stddev) const;

        /** \brief current list of all shown result names (updated on model reset, using calcResultNames() */
        QStringList lastResultNames;
        /** \brief current list of all shown result labels (updated on model reset, using calcResultNames() */
        QStringList lastResultLabels;
        /** \brief list of all columns/evaluation result sets in the model (as they are unsorted in the QFRawDataRecord), where one string is the ID and the second the label */
        QList<QPair<QString, QString> > lastResultSets;

        QString resultFilter;
        QString evaluationFilter;
        QString resultFilterNot;
        QString evaluationFilterNot;
        bool resultFilterRegExp;
        bool evaluationFilterRegExp;
};

#endif // QFRDRRESULTSMODEL_H
