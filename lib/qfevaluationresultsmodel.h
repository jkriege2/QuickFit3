/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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
            ValueRole=Qt::UserRole,        /**< returns the value as a string, the way it should be displayed in the table (i.e. with shortened lists/matrices ...) */
            NameRole=Qt::UserRole+1,       /**< same as ResultNameRole, i.e. returns the resultName of the result displayed in the cell */
            AvgRole=Qt::UserRole+2,        /**< returns the average of all numeric values (for lists and matrices) in this cell */
            SDRole=Qt::UserRole+3,         /**< same as AvgRole, but retruns the standard deviation */
            SumRole=Qt::UserRole+4,        /**< same as AvgRole, but returns the sum (usefull when calculating avergae/SD over several cells) */
            Sum2Role=Qt::UserRole+5,       /**< same as AvgRole, but returns the sum of squares (usefull when calculating avergae/SD over several cells) */
            CountRole=Qt::UserRole+6,      /**< returns the number of values represented by the cell (i.e. items in list/matrix, or 1) */
            EvalNameRole=Qt::UserRole+7,   /**< returns the evaluation name of the result in the cell */
            ResultNameRole=Qt::UserRole+8, /**< returns the resultname of the result in the cell */
            ResultIDRole=Qt::UserRole+9,   /**< returns the ID (numeric) of the QFRawDataRecord, the result is stored in, or -1 */
            MedianRole=Qt::UserRole+10,    /**< same as AvgRole, but returns the median  */
            Quantile25Role=Qt::UserRole+11,/**< same as AvgRole, but returns the 25% quantile  */
            Quantile75Role=Qt::UserRole+12 /**< same as AvgRole, but returns the 75% quantile  */
        };


    public slots:
        void resultsChanged(QFRawDataRecord* record=NULL, const QString& evaluationName=QString(""), const QString& resultName=QString(""));
        void setResultFilter(QString filter);
        void setFilesFilter(QString filter);
        void setResultFilterNot(QString filter);
        void setFilesFilterNot(QString filter);
        void setFilesFilterUsesRegExp(bool use);
        void setResultFilterUsesRegExp(bool use);
        void setDisplayProperties(const QStringList& props);
        void setShowVectorMatrixAvg(bool show);
        void setExtractIndexes(bool enabled);
        void setRemoveUnusedIndexes(bool enabled);
        void setMinUnusedIndexesForStatistics(bool enabled);
    protected:
        QFEvaluationItem* evaluation;
        QString evalFilter;

        void calcStatistics(QString resultName, double& average, double& stddev) const;
        int getExtraColumns() const;
    private:

        /** \brief calculate average and standard deviation over all result for a given result name */
        //void calcStatistics(QString resultName, double& average, double& stddev) const;

        /** \brief current list of all shown result names (updated on model reset, using calcResultNames() */
        QStringList lastResultNames;
        QStringList lastResultLabels;
        QStringList displayProperties;
        QList<QPair<QPointer<QFRawDataRecord>, QString> > lastResults;
        QMap<QString, QString> resultGroups;
        bool showVectorMatrixAvg;

        QString resultFilter;
        QString filesFilter;
        QString resultFilterNot;
        QString filesFilterNot;
        bool resultFilterRegExp;
        bool filesFilterRegExp;

        bool extractRuns;
        bool removeUnusedRuns;
        bool mindRunsForVectorStat;
};

#endif // QFEVALUATIONRESULTSMODEL_H
