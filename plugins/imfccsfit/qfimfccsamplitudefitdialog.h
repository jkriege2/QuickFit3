#ifndef QFImFCCSAmplitudeFitDialog_H
#define QFImFCCSAmplitudeFitDialog_H

#include <QWidget>
#include "qfimfccsfitevaluation_item.h"
#include "qfplotter.h"
#include "qmodernprogresswidget.h"

namespace Ui {
    class QFImFCCSAmplitudeFitDialog;
}

class QFImFCCSAmplitudeFitDialog : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSAmplitudeFitDialog(QWidget *parent = 0);
        ~QFImFCCSAmplitudeFitDialog();

        QFRawDataRecord* getACF0() const;
        QFRawDataRecord* getACF1() const;
        QFRawDataRecord* getCCF() const;

        bool calculateRelCCF(QFRawDataRecord *acf0, QFRawDataRecord *acf1, QFRawDataRecord *ccf, double **rel_out0, double **rel_error_out0, double **rel_out1, double **rel_error_out1, int&w, int& h, bool showErrorMessage=false) const;

    protected slots:
        void cmbACF_currentIndexChanged(int index);
        void cmbCCF_currentIndexChanged(int index);
        void replotImages();
        void addResult();
        void on_btnNextCCF_clicked();
        void on_btnStoreDataAll_clicked();
        void on_btnHelp_clicked();
        void on_cmbAmplitudeSource_currentIndexChanged(int index);
    protected:
        void closeEvent(QCloseEvent * event);
    private:
        Ui::QFImFCCSAmplitudeFitDialog *ui;
        QFImFCCSMatchRDRFunctor* matchFunctor;
        JKQTPColumnMathImage* plt;

        bool nextCCF();
        bool prevCCF();

        bool isACF(const QFRawDataRecord* rec);
        bool isCCF(const QFRawDataRecord* rec);
};

#endif // QFImFCCSAmplitudeFitDialog_H
