#ifndef QFRDRTREECOMBOBOX_H
#define QFRDRTREECOMBOBOX_H
#include "qfprojecttreemodel.h"
#include <QComboBox>
#include "lib_imexport.h"
#include "qfproject.h"

class QFLIB_EXPORT QFRDRTreeComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRTreeComboBox(QWidget *parent = 0);
        ~QFRDRTreeComboBox();
        void init(QFProject* project);
        QFRawDataRecord* currentRDR() const;
        int currentRDRID() const;
    signals:
        void currentRDRChanged(QFRawDataRecord* record);
        void refilledEmpty();
        void refilled(bool full);
    public slots:
        void setCurrentRDR(const QFRawDataRecord* record);
        void setCurrentRDRID(int record);

    protected slots:
        virtual void myCurrentIndexChanged(int i);
    public slots:
        void setDisplayRole(bool enabled);
        void setDisplayGroup(bool enabled);
        void setDisplayGroupAsColor(bool enabled);
        void setRDRTypeFilter(const QString& type);
        void setRDRApplicableFilter(QFEvaluationItem* eval);
    protected:
        QFProjectTreeModel* projectTree;
        QFProject* project;
};

#endif // QFRDRTREECOMBOBOX_H
