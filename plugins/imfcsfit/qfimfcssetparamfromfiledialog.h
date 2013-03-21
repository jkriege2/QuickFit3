#ifndef QFIMFCSSETPARAMFROMFILEDIALOG_H
#define QFIMFCSSETPARAMFROMFILEDIALOG_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qfplotter.h"
#include "qfimfcsfitevaluation.h"

namespace Ui {
    class QFImFCSSetParamFromFileDialog;
}

class QFImFCSSetParamFromFileDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFImFCSSetParamFromFileDialog(QFImFCSFitEvaluation* eval, QStringList parameters, QStringList parameterIDs, QWidget *parent = 0);
        ~QFImFCSSetParamFromFileDialog();
        QFRawDataRecord* getRDR() const;
        QString getEvalID() const;
        QString getResultID() const;
        QString getParameter() const;
        QVector<double> getData(bool *ok=NULL) const;
    protected slots:
        void replotOvr();
        void on_cmbParamter_currentIndexChanged(int index);
    private:
        Ui::QFImFCSSetParamFromFileDialog *ui;
        QStringList parameterIDs;
        JKQTPColumnMathImage* plt;
};

#endif // QFIMFCSSETPARAMFROMFILEDIALOG_H
