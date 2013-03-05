#ifndef QFRDRIMAGINGFCSSIMULATOR_H
#define QFRDRIMAGINGFCSSIMULATOR_H

#include <QDialog>

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
    private:
        Ui::QFRDRImagingFCSSimulator *ui;
};

#endif // QFRDRIMAGINGFCSSIMULATOR_H
