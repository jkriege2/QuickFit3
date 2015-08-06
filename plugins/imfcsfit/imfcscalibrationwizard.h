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
        QFPlotter* getPltWxy() const;

        void enableStep2();
        void hideStep01(bool hide=true);

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
