#ifndef QFFCSFitChi2LandscapeDialog_H
#define QFFCSFitChi2LandscapeDialog_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qffitfunction.h"
#include "qffitresultsbyindexevaluation.h"
namespace Ui {
    class QFFCSFitChi2LandscapeDialog;
}

class QFFCSFitChi2LandscapeDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSFitChi2LandscapeDialog(QFFitResultsByIndexEvaluation* item, QFRawDataRecord *rec, int index, QFFitFunction* fitfunction, QWidget *parent = 0);
        ~QFFCSFitChi2LandscapeDialog();
    protected slots:
        void updateInputs();
        void on_btnPlot_clicked();
        
    private:
        Ui::QFFCSFitChi2LandscapeDialog *ui;
        QFFitResultsByIndexEvaluation* item;
        QFRawDataRecord* rec;
        int index;
        QFFitFunction* fitfunction;
};

#endif // QFFCSFitChi2LandscapeDialog_H
