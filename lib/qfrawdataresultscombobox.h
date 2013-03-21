#ifndef QFRAWDATARESULTSCOMBOBOX_H
#define QFRAWDATARESULTSCOMBOBOX_H

#include <QComboBox>
#include "qfrawdatarecord.h"
#include "lib_imexport.h"


/*! \brief combobox that displays a list of all evaluation results in a specified QFRawDataRecord and result group (see e.g. QFResultsGroupComboBox)
    \ingroup qf3lib_tools

    Basically this combobox is filled by a call to QFRawDataRecord::resultsCalcNamesEvalIDsAndLabels().

  */
class QFLIB_EXPORT QFRawDataResultsComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRawDataResultsComboBox(QWidget *parent = 0);
        QString currentResult() const;
        QString currentEvaluationID() const;
        QString evaluationGroup() const;
        void setResultGroupFilters(QStringList& resultGroupFilters);
        QFRawDataRecord* getRDR() const;
    signals:
        void currentResultChanged(QString result, QString evalID);
    public slots:
        void setRDR(QFRawDataRecord *rdr);
        void setEvaluationGroup(const QString& group);
        void setCurrentResult(const QString& result);
        void setEvaluationIDFilter(const QString& evalID);
    protected slots:
        void myCurrentIndexChanged(int i);
        void refill();

    protected:
        QPointer<QFRawDataRecord> rdr;
        QString egroup;
        QString evaluationIDFilter;
        QStringList resultGroupFilters;
        QMap<int, QString> evalIDs;
};

#endif // QFRAWDATARESULTSCOMBOBOX_H
