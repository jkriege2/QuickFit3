#ifndef QFFCSMSDEvaluationFitAllMSDDialog_H
#define QFFCSMSDEvaluationFitAllMSDDialog_H

#include <QDialog>
#include "qffcsmsdevaluation_item.h"
#include <QVector>
#include "qfdoubleedit.h"
#include <QCheckBox>
#include "doubledatacutsliders.h"
#include <QGroupBox>
namespace Ui {
    class QFFCSMSDEvaluationFitAllMSDDialog;
}

class QFFCSMSDEvaluationFitAllMSDDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSMSDEvaluationFitAllMSDDialog(QFFCSMSDEvaluationItem* evaluation, QWidget *parent = 0);
        ~QFFCSMSDEvaluationFitAllMSDDialog();

    protected slots:
        void performFit();
        void replotGraph();
        void showHelp();
    protected:
        void connectSignals(bool connectS=true);
    private:
        Ui::QFFCSMSDEvaluationFitAllMSDDialog *ui;
        QFFCSMSDEvaluationItem* evaluation;
        QVector<double> dist, distTau;
        int getRangeMin(const QVector<double> &distTau, DoubleDataCutSliders *sliders);
        int getRangeMax(const QVector<double> &distTau, DoubleDataCutSliders *sliders);
        QList<QGroupBox*> groupBoxs;
        QList<QCheckBox*> fixAlphas, fixDs;
        QList<QFDoubleEdit*> Ds, Ps, Alphas;
        QList<DoubleDataCutSliders*> Ranges;
};

#endif // QFFCSMSDEvaluationFitAllMSDDialog_H
