#ifndef QFFCSMSDEVALUATIONGETNFROMFITS_H
#define QFFCSMSDEVALUATIONGETNFROMFITS_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
namespace Ui {
    class QFFCSMSDEvaluationGetNFromFits;
}

class QFFCSMSDEvaluationGetNFromFits : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSMSDEvaluationGetNFromFits(QFRawDataRecord* current, QWidget *parent = 0);
        ~QFFCSMSDEvaluationGetNFromFits();

        int getApplyTo() const;
        QString getEvalGroup() const;
        QString getParameterN() const;
        QString getParameterTripletTheta() const;
        QString getParameterTripletTau() const;
        bool getN() const;
        bool getTriplet() const;
    private:
        Ui::QFFCSMSDEvaluationGetNFromFits *ui;
};

#endif // QFFCSMSDEVALUATIONGETNFROMFITS_H
