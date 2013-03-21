#ifndef QFRESULTSGROUPCOMBOBOX_H
#define QFRESULTSGROUPCOMBOBOX_H

#include <QCombobox>
#include "qfrawdatarecord.h"
#include "lib_imexport.h"


/*! \brief combobox that displays a list of all revaluation groups of a specified QFRawDataRecord
    \ingroup qf3lib_tools

  */
class QFLIB_EXPORT QFResultsGroupComboBox : public QComboBox {
        Q_OBJECT
    public:
        explicit QFResultsGroupComboBox(QWidget *parent = 0);

        QString currentEvaluationGroup() const;
    public slots:
        void setCurrentEvaluationGroup(const QString& group);
        void setRDR(QFRawDataRecord *rdr);
    signals:
        void currentEvaluationGroupChanged(QString egroup);
    protected slots:
        void myCurrentIndexChanged(int i);
        void refill();
    protected:
        QPointer<QFRawDataRecord> rdr;
        QStringList egroups;
        QStringList egnames;
};

#endif // QFRESULTSGROUPCOMBOBOX_H
