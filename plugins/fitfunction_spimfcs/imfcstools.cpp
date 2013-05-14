#include "imfcstools.h"
#include "math.h"
#include "qfmathtools.h"

double SPIMFCS_newVeff(double a, double wxy, double wz) {
    const double sqpi=sqrt(M_PI);
    // calculate Veff
    return sqpi*qfSqr(a)*wz/qfSqr(erf(a/wxy)+wxy/sqpi/a*(exp(-qfSqr(a/wxy))-1.0));
}

double SPIMFCS_newVeffError(double a, double wxy, double wz) {

    return 0;
}

double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(double a, double dx, double D, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*D*t+wg*wg+wr*wr);
    const double dt_sigma2=qfSqr(dt_sigma);
    return ( (a-dx)*erf(M_SQRT2*(a-dx)/dt_sigma)+2.0*dx*erf(M_SQRT2*dx/dt_sigma)+(a+dx)*erf(M_SQRT2*(a+dx)/dt_sigma)+
            dt_sigma/QF_SQRT2PI*(exp(-2.0*qfSqr(a-dx)/dt_sigma2)-2.0*exp(-2.0*dx*dx/dt_sigma2)+exp(-2.0*qfSqr(a+dx)/dt_sigma2)) )/(2.0*a*a);
}

double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_zcorrfactor(double dz, double D, double t, double wg, double wr) {
    const double dt_sigma=sqrt(8.0*D*t+wg*wg+wr*wr);
    return exp(-2.0*dz*dz/dt_sigma)/dt_sigma/sqrt(2.0/M_PI);
}
