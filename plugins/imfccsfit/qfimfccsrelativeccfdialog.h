#ifndef QFIMFCCSRELATIVECCFDIALOG_H
#define QFIMFCCSRELATIVECCFDIALOG_H

#include <QWidget>
#include "qfimfccsfitevaluation_item.h"
#include "qfplotter.h"
#include "qmodernprogresswidget.h"

namespace Ui {
    class QFImFCCSRelativeCCFDialog;
}

class QFImFCCSRelativeCCFDialog : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSRelativeCCFDialog(QWidget *parent = 0);
        ~QFImFCCSRelativeCCFDialog();

        QFRawDataRecord* getACF() const;
        QFRawDataRecord* getCCF() const;

        static bool calculateRelCCF(QFRawDataRecord* acf, QFRawDataRecord* ccf, double** rel, double** rel_error, int &w, int &h, int avgCount, bool showErrorMessage=false);

    protected:
        void closeEvent(QCloseEvent *event);
    protected slots:
        void cmbACF_currentIndexChanged(int index);
        void cmbCCF_currentIndexChanged(int index);
        void replotImages();
        void addResult();
        void on_btnNextACF_clicked();
        void on_btnNextFileSameRole_clicked();
        void on_btnStoreDataAllSameRole_clicked();
        void on_btnStoreDataAll_clicked();
        void on_btnHelp_clicked();
    private:
        Ui::QFImFCCSRelativeCCFDialog *ui;
        QFImFCCSMatchRDRFunctor* matchFunctor;
        JKQTPColumnMathImage* plt;

        bool nextACF();
        bool nextFileSameRole();

        bool isACF(const QFRawDataRecord* rec);
        bool isCCF(const QFRawDataRecord* rec);
};

#endif // QFIMFCCSRELATIVECCFDIALOG_H
