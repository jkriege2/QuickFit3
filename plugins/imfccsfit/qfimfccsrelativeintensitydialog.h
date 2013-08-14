#ifndef QFImFCCSRelativeIntensityDialog_H
#define QFImFCCSRelativeIntensityDialog_H

#include <QWidget>
#include "qfimfccsfitevaluation_item.h"
#include "qfplotter.h"
#include "qmodernprogresswidget.h"

namespace Ui {
    class QFImFCCSRelativeIntensityDialog;
}

class QFImFCCSRelativeIntensityDialog : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSRelativeIntensityDialog(QWidget *parent = 0);
        ~QFImFCCSRelativeIntensityDialog();

        QFRawDataRecord* getCCF() const;

        static bool calculateRelIntensity(QFRawDataRecord* ccf, double** rel, int &w, int &h, bool showErrorMessage=false);

    protected slots:
        void cmbCCF_currentIndexChanged(int index);
        void replotImages();
        void addResult();
        void on_btnNextACF_clicked();
        void on_btnStoreDataAll_clicked();
        void on_btnHelp_clicked();
    private:
        Ui::QFImFCCSRelativeIntensityDialog *ui;
        QFImFCCSMatchRDRFunctor* matchFunctor;
        JKQTPColumnMathImage* plt;

        bool nextACF();

        bool isCCF(const QFRawDataRecord* rec);
};

#endif // QFImFCCSRelativeIntensityDialog_H
