#ifndef DLGCALCDIFFCOEFF_H
#define DLGCALCDIFFCOEFF_H

#include <QDialog>
#include "qftablemodel.h"
#include "qftools.h"
#include "qfpluginservices.h"
#include <QSettings>
#include <QRegExp>

class QFEDiffusionCoefficientCalculator;

namespace Ui {
    class DlgCalcDiffCoeff;
}

class DlgCalcDiffCoeff : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgCalcDiffCoeff(QFEDiffusionCoefficientCalculator *plg, QWidget *parent = 0);
        ~DlgCalcDiffCoeff();

    protected slots:
        void updateD();
        void updateGivenD();
        void on_btnSaveGivenD_clicked();
        void updatePlot();
        void readSettings();
        void writeSettings();
        void readSamples();
    private:
        Ui::DlgCalcDiffCoeff *ui;
        QFEDiffusionCoefficientCalculator *plugin;

        QFTableModel* tab;
        int c_temp, c_D, c_visc, c_density;
        QVector<double> temp, D, visc, density;

        bool updating;
};

#endif // DLGCALCDIFFCOEFF_H
