#ifndef QFECALCULATORDIALOG_H
#define QFECALCULATORDIALOG_H

#include <QDialog>
#include "qfmathparser.h"
#include "qftools.h"
#include "qffunctionreferencetool.h"

class QFECalculator;

namespace Ui {
class QFECalculatorDialog;
}

class QFECalculatorDialog : public QDialog {
        Q_OBJECT

    public:
        explicit QFECalculatorDialog(QFECalculator* calc, QWidget *parent = 0);
        ~QFECalculatorDialog();
        int getPrecision() const;
        QPlainTextEdit* getHistory() const;
    protected slots:
        void showHelp();
        void showCache();
        void on_btnEvaluate_clicked();
        void on_btnClearHistory_clicked();
        void on_btnClearCache_clicked();
        void delayedStartSearch();
        void on_edtExpression_textChanged(QString text);
        void on_chkMultiline_toggled(bool enabled);
    private:
        Ui::QFECalculatorDialog *ui;
        QFMathParser* parser;
        QFECalculator* calc;
        QFFunctionReferenceTool* functionRef;
        QStringList history;
        void setupParser(QFMathParser* parser) const;
};

#endif // QFECALCULATORDIALOG_H
