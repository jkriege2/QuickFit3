/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center
	
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

#ifndef IMFCSTOOLS_H
#define IMFCSTOOLS_H

#include <math.h>
#include "qfmathtools.h"

inline double SPIMFCS_newVeff(double a, double wxy, double wz) {
    return QF_SQRTPI*qfSqr(a)*wz/qfSqr(erf(a/wxy)+wxy/QF_SQRTPI/a*(exp(-qfSqr(a/wxy))-1.0));
}

inline double SPIMFCS_newAeff(double a, double wxy, double wz) {
    return qfSqr(a)/qfSqr(erf(a/wxy)+wxy/QF_SQRTPI/a*(exp(-qfSqr(a/wxy))-1.0));
}

inline double SPIMFCS_newAeff_xz(double a, double wxy, double wz) {
    return QF_SQRTPI*wz*a/(erf(a/wxy)+wxy/QF_SQRTPI/a*(exp(-qfSqr(a/wxy))-1.0));
}

inline double SPIMFCS_newVeffError(double a, double ea, double wxy, double ewxy, double wz, double ewz){
    return 0;
}

inline double SPIMFCS_newAeffError(double a, double ea, double wxy, double ewxy, double wz, double ewz){
    return 0;
}

inline double SPIMFCS_newAeffError_xz(double a, double ea, double wxy, double ewxy, double wz, double ewz){
    return 0;
}

inline double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(double a, double dx, double D, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*D*t+wg*wg+wr*wr);
    const double dt_sigma2=qfSqr(dt_sigma);
    return ( (a-dx)*erf(M_SQRT2*(a-dx)/dt_sigma)-2.0*dx*erf(M_SQRT2*dx/dt_sigma)+(a+dx)*erf(M_SQRT2*(a+dx)/dt_sigma)+
            dt_sigma/QF_SQRT2PI*(exp(-2.0*qfSqr(a-dx)/dt_sigma2)-2.0*exp(-2.0*dx*dx/dt_sigma2)+exp(-2.0*qfSqr(a+dx)/dt_sigma2)) )/
            (2.0*a*a);
}

inline double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(double dz, double D, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*D*t+wg*wg+wr*wr);
    return exp(-2.0*dz*dz/dt_sigma)/dt_sigma*sqrt(2.0/M_PI);
}

inline double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(double a, double dx, double dy, double dz, double D, double t, double wg, double wr, double zg, double zr) {
    return QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dx, D, t, wg, wr)*
           QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dy, D, t, wg, wr)*
           QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(dz, D, t, zg, zr);
}

inline double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor_2Dxy(double a, double dx, double dy, double D, double t, double wg, double wr, double zg, double zr) {
    return QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dx, D, t, wg, wr)*
           QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dy, D, t, wg, wr);
}

inline double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor_2Dxz(double a, double dx, double dz, double D, double t, double wg, double wr, double zg, double zr) {
    return QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dx, D, t, wg, wr)*
           QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(dz, D, t, zg, zr);
}

inline double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor_2Dyz(double a, double dx, double dy, double dz, double D, double t, double wg, double wr, double zg, double zr) {
    return QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dy, D, t, wg, wr)*
           QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_corrfactor(dz, D, t, zg, zr);
}

inline double QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_pixelcorrfactor(double a, double dx, double Gamma, double alpha, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*Gamma*pow(t, alpha)+wg*wg+wr*wr);
    const double dt_sigma2=qfSqr(dt_sigma);
    return ( (a-dx)*erf(M_SQRT2*(a-dx)/dt_sigma)-2.0*dx*erf(M_SQRT2*dx/dt_sigma)+(a+dx)*erf(M_SQRT2*(a+dx)/dt_sigma)+
            dt_sigma/QF_SQRT2PI*(exp(-2.0*qfSqr(a-dx)/dt_sigma2)-2.0*exp(-2.0*dx*dx/dt_sigma2)+exp(-2.0*qfSqr(a+dx)/dt_sigma2)) )/
            (2.0*a*a);
}

inline double QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(double dz, double Gamma, double alpha, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*Gamma*pow(t, alpha)+wg*wg+wr*wr);
    return exp(-2.0*dz*dz/dt_sigma)/dt_sigma*sqrt(2.0/M_PI);
}

inline double QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(double a, double dx, double dy, double dz, double Gamma, double alpha, double t, double wg, double wr, double zg, double zr) {
    return QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_pixelcorrfactor(a, dx, Gamma, alpha, t, wg, wr)*
           QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_pixelcorrfactor(a, dy, Gamma, alpha, t, wg, wr)*
           QFFitFunctionsSPIMFCCSFWADiff2ColorCCF_corrfactor(dz, Gamma, alpha, t, zg, zr);
}

inline double TIRFCS_newAeff(double a, double wxy) {
    return qfSqr(a)/qfSqr(erf(a/wxy)+wxy/QF_SQRTPI/a*(exp(-qfSqr(a/wxy))-1.0));
}


inline double TIRFCS_newAeffError(double a, double ea, double wxy, double ewxy){
    return 0;
}


inline double QFFitFunctionsTIRFCCSFWDiff2ColorCCF_pixelcorrfactor(double a, double dx, double D, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*D*t+wg*wg+wr*wr);
    const double dt_sigma2=qfSqr(dt_sigma);
    return ( (a-dx)*erf(M_SQRT2*(a-dx)/dt_sigma)-2.0*dx*erf(M_SQRT2*dx/dt_sigma)+(a+dx)*erf(M_SQRT2*(a+dx)/dt_sigma)+
            dt_sigma/QF_SQRT2PI*(exp(-2.0*qfSqr(a-dx)/dt_sigma2)-2.0*exp(-2.0*dx*dx/dt_sigma2)+exp(-2.0*qfSqr(a+dx)/dt_sigma2)) )/
            (2.0*a*a);
}



inline double QFFitFunctionsTIRFCCSFWDiff2ColorCCF_corrfactor_2Dxy(double a, double dx, double dy, double D, double t, double wg, double wr) {
    return QFFitFunctionsTIRFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dx, D, t, wg, wr)*
           QFFitFunctionsTIRFCCSFWDiff2ColorCCF_pixelcorrfactor(a, dy, D, t, wg, wr);
}


inline double QFFitFunctionsTIRFCCSFWADiff2ColorCCF_pixelcorrfactor(double a, double dx, double Gamma, double alpha, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*Gamma*pow(t, alpha)+wg*wg+wr*wr);
    const double dt_sigma2=qfSqr(dt_sigma);
    return ( (a-dx)*erf(M_SQRT2*(a-dx)/dt_sigma)-2.0*dx*erf(M_SQRT2*dx/dt_sigma)+(a+dx)*erf(M_SQRT2*(a+dx)/dt_sigma)+
            dt_sigma/QF_SQRT2PI*(exp(-2.0*qfSqr(a-dx)/dt_sigma2)-2.0*exp(-2.0*dx*dx/dt_sigma2)+exp(-2.0*qfSqr(a+dx)/dt_sigma2)) )/
            (2.0*a*a);
}


inline double QFFitFunctionsTIRFCCSFWADiff2ColorCCF_corrfactor_2Dxy(double a, double dx, double dy, double Gamma, double alpha, double t, double wg, double wr) {
    return QFFitFunctionsTIRFCCSFWADiff2ColorCCF_pixelcorrfactor(a, dx, Gamma, alpha, t, wg, wr)*
           QFFitFunctionsTIRFCCSFWADiff2ColorCCF_pixelcorrfactor(a, dy, Gamma, alpha, t, wg, wr);
}

#endif // IMFCSTOOLS_H
