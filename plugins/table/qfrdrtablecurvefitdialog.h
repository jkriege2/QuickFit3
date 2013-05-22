#ifndef QFRDRTABLECURVEFITDIALOG_H
#define QFRDRTABLECURVEFITDIALOG_H

#include <QDialog>

namespace Ui {
    class QFRDRTableCurveFitDialog;
}

class QFRDRTableCurveFitDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableCurveFitDialog(QWidget *parent = 0);
        ~QFRDRTableCurveFitDialog();
        
    private:
        Ui::QFRDRTableCurveFitDialog *ui;
};

#endif // QFRDRTABLECURVEFITDIALOG_H
