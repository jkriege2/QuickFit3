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
