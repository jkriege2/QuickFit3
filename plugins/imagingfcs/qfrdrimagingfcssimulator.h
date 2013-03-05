#ifndef QFRDRIMAGINGFCSSIMULATOR_H
#define QFRDRIMAGINGFCSSIMULATOR_H

#include <QDialog>
#include "qfrdrimagingfcssimulationthread.h"

namespace Ui {
    class QFRDRImagingFCSSimulator;
}

class QFRDRImagingFCSSimulator : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRImagingFCSSimulator(QWidget *parent = 0);
        ~QFRDRImagingFCSSimulator();
        
        QString getSimulationFilename() const;
    protected slots:
        void on_btnHelp_clicked();
        void writeSettings() const;
        void readSettings();
        void on_btnRun_clicked();
        void on_btnCancel_clicked();
        void threadFinished();
    private:
        Ui::QFRDRImagingFCSSimulator *ui;

        enum DialogState {
            dsParameterInput=0,
            dsRunning=1,
            dsFinished=2
        };

        DialogState dstate;

        void setState(DialogState new_state);

        QString lastSimFile;

        QFRDRImagingFCSSimulationThread* sim;
};

#endif // QFRDRIMAGINGFCSSIMULATOR_H
