#ifndef QFFCSMSDEVALUATIONFITMSDDIALOG_H
#define QFFCSMSDEVALUATIONFITMSDDIALOG_H

#include <QDialog>
#include "qffcsmsdevaluation_item.h"
#include <QVector>

namespace Ui {
    class QFFCSMSDEvaluationFitMSDDialog;
}

class QFFCSMSDEvaluationFitMSDDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSMSDEvaluationFitMSDDialog(QFFCSMSDEvaluationItem* evaluation, int theoryID, QWidget *parent = 0);
        ~QFFCSMSDEvaluationFitMSDDialog();

    protected slots:
        void saveResults();
        void on_btnFit_clicked();
        void replotGraph();
        void showHelp();
    protected:
        void connectSignals(bool connectS=true);
    private:
        Ui::QFFCSMSDEvaluationFitMSDDialog *ui;
        QFFCSMSDEvaluationItem* evaluation;
        int theoryID;
        QVector<double> dist, distTau;
};

#endif // QFFCSMSDEVALUATIONFITMSDDIALOG_H
