#ifndef IMFCSCALIBRATIONWIZARD_H
#define IMFCSCALIBRATIONWIZARD_H

#include <QWidget>
#include "qfplotter.h"

namespace Ui {
    class ImFCSCalibrationWizard;
}

class ImFCSCalibrationWizard : public QWidget
{
        Q_OBJECT
        
    public:
        explicit ImFCSCalibrationWizard(QWidget *parent = 0);
        ~ImFCSCalibrationWizard();
        QFPlotter* getPltD() const;

    signals:
        void run1();
        void run2();
        void run3();
        void run4();
        void run5();
        void showTutorial();
        void loadFile();
        void correlate();
    protected:
        void closeEvent(QCloseEvent *event);
    protected slots:
        void on_btnReset_clicked();
        void on_btnUnlock_clicked();
        void on_btnHelp_clicked();
        void on_btnClose_clicked();
        void on_btnStep1_clicked();
        void on_btnStep2_clicked();
        void on_btnStep3_clicked();
        void on_btnStep4_clicked();
        void on_btnStep5_clicked();
        void on_btnLoadFile_clicked();
        void on_btnCorrelate_clicked();

    private:
        Ui::ImFCSCalibrationWizard *ui;
};

#endif // IMFCSCALIBRATIONWIZARD_H
