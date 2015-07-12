/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
        void on_btnCHelp4_clicked();
        void on_btnCHelp5_clicked();
        void on_btnEditGivenD_toggled(bool checked);
        void on_cmbShapeType_currentIndexChanged(int index);
        void on_cmbParticlePropsType_currentIndexChanged(int index);
        void showHelp();
        void on_btnSendEmailGivenD_clicked();
    protected:
        Ui::DlgCalcDiffCoeff *ui;
        QFEDiffusionCoefficientCalculator *plugin;
        DlgComponentInfo* dlgInfo;

        QFTableModel* tab;
        int c_temp, c_D, c_visc, c_density, c_Dwater, c_Dsolution, c_Dsphere;
        QVector<double> temp, D, visc, density, Dwater, Dsolution, Dsphere;

        bool updating;
        QString userDDescriptor;

};

#endif // DLGCALCDIFFCOEFF_H
