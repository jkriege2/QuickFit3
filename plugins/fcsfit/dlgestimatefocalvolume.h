/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef DLGESTIMATEFOCALVOLUME_H
#define DLGESTIMATEFOCALVOLUME_H

#include <QDialog>
#include "ui_estimatefocalvolumedlg.h"
#include "programoptions.h"
#include "qfpluginservices.h"
#include "qfextensiontool.h"
#include "qfextension.h"
#include "qfextensionmanager.h"
#include "qfextensionreportingtool.h"

/*! \brief estimates the focal parameter \f$ w_{xy} \f$ from a given diffusion coefficient or concentration
    \ingroup qf3evalp_fcsfit

    The focus width is calculated in nanometers! The diffusion time \f$ \tau_D\f$ is given in microseconds.

    If a diffusion coefficient \f$ (D\pm\Delta D) \f$ in micron^2/s is known, this dialog calculates:
      \f[ w_{xy}=\sqrt{4\cdot D\cdot\tau_D\cdot10^{-6}}\cdot10^{3} \f]
      \f[ \Delta w_{xy}=\sqrt{\frac{\tau_D\cdot10^{-6}\cdot\Delta D^2}{D}+\frac{D\cdot\Delta\tau_D^2\cdot10^{-6}}{\tau_D}}\cdot10^{3} \f]
    If a concentration \f$ (C\pm\Delta C) \f$ in mMolar is known, this dialog calculates:
      \f[ w_{xy}=\sqrt[3]{\frac{N}{6.02214179\cdot\pi^{3/2}\cdot\gamma\cdot C}\cdot10^{10}} \f]
      \f[ \Delta w_{xy}=\sqrt{\left(\Delta N\cdot\frac{h}{3\cdot(N\cdot h)^{2/3}}\right)^2 + \left(\Delta\gamma\cdot\frac{N\cdot h}{3\cdot\gamma\cdot(N\cdot h)^{2/3}}\right)^2 + \left(\Delta C\cdot\frac{N\cdot h}{3\cdot C\cdot(N\cdot h)^{2/3}}\right)^2} \f]
      where \f[ h:=\frac{10^{10}}{\pi^{3/2}\cdot\gamma\cdot C\cdot 6.02214179} \f]
*/

class dlgEstimateFocalVolume : public QDialog, private Ui::dlgEstimateFocalVolume {
        Q_OBJECT
    public:
        dlgEstimateFocalVolume(ProgramOptions* settings, QWidget* parent=NULL);
        virtual ~dlgEstimateFocalVolume();

        void init(double particles, double particles_error, bool has_nparticles, double tauD, double tauD_error, bool has_tauD, double gamma, double gamma_error, bool has_gamma);
        double get_wxy() { return wxy; }
        double get_wxyerror() { return wxy_error; }

        void get_wxy(double particles, double particles_error, double tauD, double tauD_error, double gamma, double gamma_error, double &wxy, double &wxy_error);
        double get_wxy(double particles, double particles_error, double tauD, double tauD_error, double gamma, double gamma_error);
        double get_wxyerror(double particles, double particles_error, double tauD, double tauD_error, double gamma, double gamma_error);

        bool allRuns() const;



    protected:
        double particles;
        double particles_error;
        bool has_nparticles;
        double tauD;
        double tauD_error;
        bool has_tauD;
        double gamma;
        double gamma_error;
        bool has_gamma;
        double wxy;
        double wxy_error;
        ProgramOptions* settings;
        QFExtensionTool* toolDCalc;
        QFExtensionReportingTool* toolDCalcReport;
        QVariant DTool;


        void calc_from_C();
        void calc_from_D();
    protected slots:
        void on_buttonBox_accepted();

        void on_spinC_valueChanged(double d);
        void on_spinCError_valueChanged(double d);
        void on_spinD_valueChanged(double d);
        void on_spinDError_valueChanged(double d);
        void on_btnHelp_clicked();
        void on_btnDCalculator_clicked();

        void checkToolVal();
    private:
};

#endif // DLGESTIMATEFOCALVOLUME_H
