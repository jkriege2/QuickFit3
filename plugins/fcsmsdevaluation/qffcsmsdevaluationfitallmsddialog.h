#ifndef QFFCSMSDEvaluationFitAllMSDDialog_H
#define QFFCSMSDEvaluationFitAllMSDDialog_H

#include <QDialog>
#include "qffcsmsdevaluation_item.h"
#include <QVector>

namespace Ui {
    class QFFCSMSDEvaluationFitAllMSDDialog;
}

class QFFCSMSDEvaluationFitAllMSDDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSMSDEvaluationFitAllMSDDialog(QFFCSMSDEvaluationItem* evaluation, int theoryID=0, QWidget *parent = 0);
        ~QFFCSMSDEvaluationFitAllMSDDialog();

    protected slots:
        void saveResults();
        void on_btnFit_clicked();
        void replotGraph();
        void showHelp();
    protected:
        void connectSignals(bool connectS=true);
    private:
        Ui::QFFCSMSDEvaluationFitAllMSDDialog *ui;
        QFFCSMSDEvaluationItem* evaluation;
        int theoryID;
        QVector<double> dist, distTau;
        int getRangeMin();
        int getRangeMax();
};

#endif // QFFCSMSDEvaluationFitAllMSDDialog_H
