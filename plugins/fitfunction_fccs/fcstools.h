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

#ifndef FCCSTOOLS_H
#define FCCSTOOLS_H

#include "math.h"
#include "qfmathtools.h"

inline double FCS_newVeff(double wxy, double wz) {
    return qfCube(QF_SQRTPI)*qfSqr(wxy)*wz;
}

inline double FCS_newVeffError(double wxy, double ewxy, double wz, double ewz){
    return 0;
}


inline double QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(double dz, double D, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*D*t+wg*wg+wr*wr);
    return exp(-2.0*dz*dz/dt_sigma)/dt_sigma*sqrt(2.0/M_PI);
}

inline double QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(double dx, double dy, double dz, double D, double t, double wg, double wr, double zg, double zr) {
    return QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(dx, D, t, wg, wr)*
           QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(dy, D, t, wg, wr)*
           QFFitFunctionsFCCSFWDiff2ColorCCF_corrfactor(dz, D, t, zg, zr);
}


inline double QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(double dz, double Gamma, double alpha, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*Gamma*pow(t, alpha)+wg*wg+wr*wr);
    return exp(-2.0*dz*dz/dt_sigma)/dt_sigma*sqrt(2.0/M_PI);
}

inline double QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(double dx, double dy, double dz, double Gamma, double alpha, double t, double wg, double wr, double zg, double zr) {
    return QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(dx, Gamma, alpha, t, wg, wr)*
           QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(dy, Gamma, alpha, t, wg, wr)*
           QFFitFunctionsFCCSFWADiff2ColorCCF_corrfactor(dz, Gamma, alpha, t, zg, zr);
}

inline double QFFitFunctionFCCSFWTriplet(int components, double t, double thetaT, double tauT, double thetaR=0.0, double tauR=0.0) {
    if (  (components==1 && fabs(thetaT)>1e-15 && fabs(tauT)>1e-15)
          ||(components==2 && (fabs(thetaT)>1e-15 && fabs(tauT)>1e-15) && (fabs(thetaR)<1e-15 || fabs(tauR)<1e-15))) {
        return (1.0-thetaT+thetaT*exp(-t/tauT))/(1.0-thetaT);
    } else if (components==2 && (fabs(thetaT)>1e-15 && fabs(tauT)>1e-15)& (fabs(thetaR)>1e-15 && fabs(tauR)>1e-15)) {
        return (1.0-thetaT-thetaR+thetaT*exp(-t/tauT)+thetaR*exp(-t/tauR))/(1.0-thetaT-thetaR);
    } else if (components==2 && (fabs(thetaT)<1e-15 || fabs(tauT)<1e-15)& (fabs(thetaR)>1e-15 && fabs(tauR)>1e-15)) {
        return (1.0-thetaR+thetaR*exp(-t/tauR))/(1.0-thetaR);
    } else {
        return 1.0;
    }
}

#endif // FCCSTOOLS_H
