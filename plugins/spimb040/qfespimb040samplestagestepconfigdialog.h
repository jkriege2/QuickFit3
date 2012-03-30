#ifndef QFESPIMB040SAMPLESTAGESTEPCONFIGDIALOG_H
#define QFESPIMB040SAMPLESTAGESTEPCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class QFESPIMB040SampleStageStepConfigDialog;
}

class QFESPIMB040SampleStageStepConfigDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESPIMB040SampleStageStepConfigDialog(double x, double y, double z, QWidget *parent = 0);
        ~QFESPIMB040SampleStageStepConfigDialog();
        
        double x() const;
        double y() const;
        double z() const;
    private:
        Ui::QFESPIMB040SampleStageStepConfigDialog *ui;
};

#endif // QFESPIMB040SAMPLESTAGESTEPCONFIGDIALOG_H
