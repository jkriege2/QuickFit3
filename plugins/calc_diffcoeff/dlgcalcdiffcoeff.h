#ifndef DLGCALCDIFFCOEFF_H
#define DLGCALCDIFFCOEFF_H

#include <QDialog>
#include "qftablemodel.h"
#include "qftools.h"
#include "qfpluginservices.h"
#include <QSettings>
#include <QRegExp>
#include "dlgcomponentinfo.h"

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
        void updatePlot();
        void on_btnSaveGivenD_clicked();
        void on_btnDeleteGivenD_clicked();
        void redoPlot();
        void readSettings();
        void writeSettings();
        void readSamples();
        void readComponents();
        void updateReport(const QModelIndex& index);
        void on_btnUseVisc_clicked();
        void on_btnUseD_clicked();
        void on_btnCHelp1_clicked();
        void on_btnCHelp2_clicked();
        void on_btnCHelp3_clicked();
        void showHelp();
    protected:
        Ui::DlgCalcDiffCoeff *ui;
        QFEDiffusionCoefficientCalculator *plugin;
        DlgComponentInfo* dlgInfo;

        QFTableModel* tab;
        int c_temp, c_D, c_visc, c_density, c_Dwater, c_Dsolution;
        QVector<double> temp, D, visc, density, Dwater, Dsolution;

        bool updating;

};

#endif // DLGCALCDIFFCOEFF_H
