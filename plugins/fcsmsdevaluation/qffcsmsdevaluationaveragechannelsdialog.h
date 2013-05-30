#ifndef QFFCSMSDEVALUATIONAVERAGECHANNELSDIALOG_H
#define QFFCSMSDEVALUATIONAVERAGECHANNELSDIALOG_H

#include <QDialog>

namespace Ui {
    class QFFCSMSDEvaluationAverageChannelsDialog;
}

class QFFCSMSDEvaluationAverageChannelsDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSMSDEvaluationAverageChannelsDialog(QWidget *parent = 0);
        ~QFFCSMSDEvaluationAverageChannelsDialog();
        void init(int pointsToAvg, int rangeMin, int rangeMax);
        int getPoints() const;
        int getApplyTo() const;
    private:
        Ui::QFFCSMSDEvaluationAverageChannelsDialog *ui;
};

#endif // QFFCSMSDEVALUATIONAVERAGECHANNELSDIALOG_H
