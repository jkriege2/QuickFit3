#ifndef QFRDRTABLEFORMULADIALOG_H
#define QFRDRTABLEFORMULADIALOG_H

#include <QDialog>
#include "qfcompleterfromfile.h"
#include <QCompleter>

namespace Ui {
    class QFRDRTableFormulaDialog;
}

class QFRDRTableFormulaDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableFormulaDialog(QWidget *parent = 0);
        ~QFRDRTableFormulaDialog();

        QString getExpression() const;

   protected slots:
        void on_edtFormula_textChanged(QString text);
        void on_btnHelp_clicked();
        
    private:
        Ui::QFRDRTableFormulaDialog *ui;
        QFCompleterFromFile* compExpression;

};

#endif // QFRDRTABLEFORMULADIALOG_H
