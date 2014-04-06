#ifndef QFESPIMLIGHTSHEETEVALUATIONCOPYCONFIGDIALOG_H
#define QFESPIMLIGHTSHEETEVALUATIONCOPYCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class QFESPIMLightSheetEvaluationCopyConfigDialog;
}

class QFESPIMLightSheetEvaluationCopyConfigDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFESPIMLightSheetEvaluationCopyConfigDialog(QWidget *parent = 0);
        ~QFESPIMLightSheetEvaluationCopyConfigDialog();

        bool getSaveFit() const;
        bool getShiftCenter() const;
        bool getSaveNormalized() const;
        int getOutput() const;
        int getUnits() const;
    private:
        Ui::QFESPIMLightSheetEvaluationCopyConfigDialog *ui;
};

#endif // QFESPIMLIGHTSHEETEVALUATIONCOPYCONFIGDIALOG_H
