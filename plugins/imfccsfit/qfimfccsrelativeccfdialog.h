#ifndef QFIMFCCSRELATIVECCFDIALOG_H
#define QFIMFCCSRELATIVECCFDIALOG_H

#include <QDialog>
#include "qfimfccsfitevaluation_item.h"

namespace Ui {
    class QFImFCCSRelativeCCFDialog;
}

class QFImFCCSRelativeCCFDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSRelativeCCFDialog(QWidget *parent = 0);
        ~QFImFCCSRelativeCCFDialog();

        QFRawDataRecord* getACF() const;
        QFRawDataRecord* getCCF() const;

    protected slots:
        void on_cmbACF_currentIndexChanged(int index);
    private:
        Ui::QFImFCCSRelativeCCFDialog *ui;
        QFImFCCSMatchRDRFunctor* matchFunctor;
};

#endif // QFIMFCCSRELATIVECCFDIALOG_H
