#ifndef QFFCCSRelativeCCFCrosstalkDialog_H
#define QFFCCSRelativeCCFCrosstalkDialog_H

#include <QWidget>
#include "qffccsfitevaluation_item.h"
#include "qfplotter.h"
#include "qmodernprogresswidget.h"

namespace Ui {
    class QFFCCSRelativeCCFCrosstalkDialog;
}

class QFFCCSRelativeCCFCrosstalkDialog : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFFCCSRelativeCCFCrosstalkDialog(QWidget *parent = 0);
        ~QFFCCSRelativeCCFCrosstalkDialog();

        QFRawDataRecord* getACF0() const;
        QFRawDataRecord* getACF1() const;
        QFRawDataRecord* getCCF() const;

        static bool calculateRelCCF(QFRawDataRecord *acf0, QFRawDataRecord *acf1, QFRawDataRecord *ccf, double **rel_out0, double **rel_error_out0, double **rel_out1, double **rel_error_out1, double** acf0Amplitude, double **acf0UCAmplitude, double** acf1Amplitude, double **acf1UCAmplitude, double **ccfAmplitude, double **ccfUCAmplitude, int&runs, int avgCount, double crosstalk, int crosstalkDir, bool showErrorMessage=false, int source=0, const QString& resultGroupACF0=QString(), const QString& resultGroupACF1=QString(), const QString& resultGroupCCF=QString(), const QString& param="fitparam_g0", bool subtractBackground=true);

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
        Ui::QFFCCSRelativeCCFCrosstalkDialog *ui;
        QFFCCSMatchRDRFunctor* matchFunctor;
        JKQTPColumnMathImage* plt;

        bool nextCCF();
        bool prevCCF();

        bool isACF(const QFRawDataRecord* rec);
        bool isCCF(const QFRawDataRecord* rec);
};

#endif // QFFCCSRelativeCCFCrosstalkDialog_H
