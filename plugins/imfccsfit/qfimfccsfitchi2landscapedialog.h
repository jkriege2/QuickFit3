#ifndef QFIMFCCSFITCHI2LANDSCAPEDIALOG_H
#define QFIMFCCSFITCHI2LANDSCAPEDIALOG_H

#include <QDialog>
#include "qfimfccsfitchi2landscapedialog.h"
#include "qfimfccsfitevaluation_item.h"

namespace Ui {
    class QFImFCCSFitChi2LandscapeDialog;
}

class QFImFCCSFitChi2LandscapeDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSFitChi2LandscapeDialog(QFImFCCSFitEvaluationItem* item, QWidget *parent = 0);
        ~QFImFCCSFitChi2LandscapeDialog();
    protected slots:
        void updateInputs();
        void on_btnPlot_clicked();
        
    private:
        Ui::QFImFCCSFitChi2LandscapeDialog *ui;
        QFImFCCSFitEvaluationItem* item;
};

#endif // QFIMFCCSFITCHI2LANDSCAPEDIALOG_H
