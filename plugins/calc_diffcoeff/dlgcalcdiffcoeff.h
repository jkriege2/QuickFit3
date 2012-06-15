#ifndef DLGCALCDIFFCOEFF_H
#define DLGCALCDIFFCOEFF_H

#include <QDialog>
#include "qftablemodel.h"
#include "qftools.h"

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
        
        void updatePlot();
        void readSettings();
        void writeSettings();
    private:
        Ui::DlgCalcDiffCoeff *ui;
        QFEDiffusionCoefficientCalculator *plugin;

        QFTableModel* tab;
        int c_temp, c_D, c_visc, c_density;
        QVector<double> temp, D, visc, density;
};

#endif // DLGCALCDIFFCOEFF_H
