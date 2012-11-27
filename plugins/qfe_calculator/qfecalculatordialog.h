#ifndef QFECALCULATORDIALOG_H
#define QFECALCULATORDIALOG_H

#include <QDialog>
#include "jkmathparser.h"
#include "qftools.h"

class QFECalculator;

namespace Ui {
class QFECalculatorDialog;
}

class QFECalculatorDialog : public QDialog {
        Q_OBJECT

    public:
        explicit QFECalculatorDialog(QFECalculator* calc, QWidget *parent = 0);
        ~QFECalculatorDialog();
    protected slots:
        void showHelp();
        void showCache();
        void on_btnEvaluate_clicked();
        void on_btnClearHistory_clicked();
        void on_btnCLearCache_clicked();
    private:
        Ui::QFECalculatorDialog *ui;
        jkMathParser* parser;
        QFECalculator* calc;
};

#endif // QFECALCULATORDIALOG_H
