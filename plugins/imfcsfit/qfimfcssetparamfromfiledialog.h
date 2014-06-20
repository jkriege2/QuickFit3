#ifndef QFIMFCSSETPARAMFROMFILEDIALOG_H
#define QFIMFCSSETPARAMFROMFILEDIALOG_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qfplotter.h"
#include "qfimfcsfitevaluation.h"
#include "qfimagetransform.h"

namespace Ui {
    class QFImFCSSetParamFromFileDialog;
}



class QFImFCSSetParamFromFileDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFImFCSSetParamFromFileDialog(int width, int heighth, QFImFCSFitEvaluation* eval, QStringList parameters, QStringList parameterIDs, QWidget *parent = 0);
        ~QFImFCSSetParamFromFileDialog();
        QFRawDataRecord* getRDR() const;
        QString getEvalID() const;
        QString getResultID() const;
        QString getParameter() const;
        QVector<double> getData(bool *ok=NULL) const;
    protected slots:
        void replotOvr();
        void on_cmbParamter_currentIndexChanged(int index);
        void updateDataFromRDR();
        void on_btnHelp_clicked();
        void on_btnLoadFile_clicked();
    private:
        Ui::QFImFCSSetParamFromFileDialog *ui;
        QStringList parameterIDs;
        JKQTPColumnMathImage* plt;
        int datawidth;
        int dataheight;
        QVector<double> data;
};

#endif // QFIMFCSSETPARAMFROMFILEDIALOG_H
