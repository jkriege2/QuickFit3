#ifndef IMFCSTOOLS_H
#define IMFCSTOOLS_H

double SPIMFCS_newVeff(double a, double wxy, double wz);
double SPIMFCS_newVeffError(double a, double wxy, double wz);

double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_pixelcorrfactor(double a, double dx, double D, double t, double wg, double wr) ;
double QFFitFunctionsSPIMFCCSFWDiff2ColorCCF_zcorrfactor(double dz, double D, double t, double wg, double wr) ;

#endif // IMFCSTOOLS_H
