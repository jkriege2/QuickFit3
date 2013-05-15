#include "qffitfunctiondlsg2indext.h"

#include <cmath>
#include "qfmathtools.h"
#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))
#define pow4(x) ((x)*(x)*(x)*(x))
#define pow5(x) ((x)*(x)*(x)*(x)*(x))


QFFitFunctionDLSG2inDExt::QFFitFunctionDLSG2inDExt() {
   //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialFix, initialValue, minValue, maxValue, inc, absMin, absMax
   addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 8,            1,        8,        1,   1,      1);
#define DLSG2_n_components 0
   addParameter(FloatNumber,  "epsilon",                 "baseline",                                              "&epsilon;",                "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0,            -1e50,    1e50,     0.01);
#define DLSG2_epsilon 1

   addParameter(FloatNumber,  "frac1",                   "fraction of component 1",                               "&alpha;<sub>1</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0 );
#define DLSG2_frac1 2
   addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff1 3
   addParameter(FloatNumber,  "frac2",                   "fraction of component 2",                               "&alpha;<sub>2</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
#define DLSG2_frac2 4
   addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff2 5
   addParameter(FloatNumber,  "frac3",                   "fraction of component 3",                               "&alpha;<sub>3</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
#define DLSG2_frac3 6
   addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff3 7
   addParameter(FloatNumber,  "frac4",                   "fraction of component 4",                               "&alpha;<sub>4</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
#define DLSG2_frac4 8
   addParameter(FloatNumber,  "diff_coeff4",             "diffusion coefficient of species 4",                    "D<sub>4</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff4 9
   addParameter(FloatNumber,  "frac5",                   "fraction of component 5",                               "&alpha;<sub>5</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0 );
#define DLSG2_frac5 10
   addParameter(FloatNumber,  "diff_coeff5",             "diffusion coefficient of species 5",                    "D<sub>5</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff5 11
   addParameter(FloatNumber,  "frac6",                   "fraction of component 6",                               "&alpha;<sub>6</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
#define DLSG2_frac6 12
   addParameter(FloatNumber,  "diff_coeff6",             "diffusion coefficient of species 2",                    "D<sub>6</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff6 13
   addParameter(FloatNumber,  "frac7",                   "fraction of component 7",                               "&alpha;<sub>7</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
#define DLSG2_frac7 14
   addParameter(FloatNumber,  "diff_coeff7",             "diffusion coefficient of species 7",                    "D<sub>7</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff7 15
   addParameter(FloatNumber,  "frac8",                   "fraction of component 8",                               "&alpha;<sub>8</sub>",      "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 1,            0,        1e10,     0.1, 0);
#define DLSG2_frac8 16
   addParameter(FloatNumber,  "diff_coeff8",             "diffusion coefficient of species 8",                    "D<sub>8</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, true, 500,          0,        1e50,     1    );
#define DLSG2_diff_coeff8 17

   addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 1,            -10,      10,       0.1  );
#define DLSG2_offset 18

   addParameter(FloatNumber,  "dls_angle",               "scattering angle",                                      "&Theta;<sub>scatter</sub>", "°",         "°",                      false,     true,        false,              QFFitFunction::EditError,    false, 90,          0,        1e4,      1    );
#define DLSG2_angle 19
   addParameter(FloatNumber,  "refractive_index",        "refractive index",                                      "n",                        "",           "",                       false,     true,        false,              QFFitFunction::EditError,    false, 1.33,        0,        1e4,      1    );
#define DLSG2_refractive 20
   addParameter(FloatNumber,  "wavelength",              "scattering light wavelength",                           "&lambda;",                 "nm",         "nm",                     false,     true,        false,              QFFitFunction::EditError,    false, 488,         0,        1e4,      1    );
#define DLSG2_wavelength 21
   addParameter(FloatNumber,  "q_square",                "q square",                                              "q<sup>2</sup>",            "1/nm²",       "nm<sup>-2</sup>",        false,    false,        false,              QFFitFunction::DisplayError, false, 500,          -1e50,    1e50,     1    );
#define DLSG2_diff_qsquare 22

   addParameter(FloatNumber,  "tau1",                    "correlation time 1",                                    "&tau;<sub>1</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 100,          1e-10,    1e50,     1    );
#define DLSG2_tau1 23
   addParameter(FloatNumber,  "tau2",                    "correlation time 2",                                    "&tau;<sub>2</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 1000,         1e-10,    1e50,     1    );
#define DLSG2_tau2 24
   addParameter(FloatNumber,  "tau3",                    "correlation time 3",                                    "&tau;<sub>3</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 10000,        1e-10,    1e50,     1    );
#define DLSG2_tau3 25
   addParameter(FloatNumber,  "tau4",                    "correlation time 4",                                    "&tau;<sub>4</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 100000,       1e-10,    1e50,     1    );
#define DLSG2_tau4 26
   addParameter(FloatNumber,  "tau5",                    "correlation time 5",                                    "&tau;<sub>5</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 100,          1e-10,    1e50,     1    );
#define DLSG2_tau5 27
   addParameter(FloatNumber,  "tau6",                    "correlation time 6",                                    "&tau;<sub>6</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 1000,         1e-10,    1e50,     1    );
#define DLSG2_tau6 28
   addParameter(FloatNumber,  "tau7",                    "correlation time 7",                                    "&tau;<sub>7</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 10000,        1e-10,    1e50,     1    );
#define DLSG2_tau7 29
   addParameter(FloatNumber,  "tau8",                    "correlation time 8",                                    "&tau;<sub>8</sub>",        "µs",         "&mu;s",                  false,    false,        false,              QFFitFunction::DisplayError, false, 100000,       1e-10,    1e50,     1    );
#define DLSG2_tau8 30

}

double QFFitFunctionDLSG2inDExt::evaluate(double t, const double* data) const {
   const int comp=data[DLSG2_n_components];
   const double epsilon=data[DLSG2_epsilon];
   const double D1=data[DLSG2_diff_coeff1];
   const double frac1=data[DLSG2_frac1];
   const double D2=data[DLSG2_diff_coeff2];
   const double frac2=data[DLSG2_frac2];
   const double D3=data[DLSG2_diff_coeff3];
   const double frac3=data[DLSG2_frac3];
   const double D4=data[DLSG2_diff_coeff4];
   const double frac4=data[DLSG2_frac4];
   const double D5=data[DLSG2_diff_coeff5];
   const double frac5=data[DLSG2_frac5];
   const double D6=data[DLSG2_diff_coeff6];
   const double frac6=data[DLSG2_frac6];
   const double D7=data[DLSG2_diff_coeff7];
   const double frac7=data[DLSG2_frac7];
   const double D8=data[DLSG2_diff_coeff8];
   const double frac8=data[DLSG2_frac8];
   const double offset=data[DLSG2_offset];

   const double n=data[DLSG2_refractive];
   const double theta=data[DLSG2_angle]/180*M_PI;
   const double lambda=data[DLSG2_wavelength]/1e3;
   const double q=4.0*M_PI*n/lambda*sin(theta/2.0);
   const double q2=sqr(q);

   double g1=frac1*exp(-q2*D1*t);
   if (comp>1) g1+=frac2*exp(-q2*D2*t);
   if (comp>2) g1+=frac3*exp(-q2*D3*t);
   if (comp>3) g1+=frac4*exp(-q2*D4*t);

   return offset+epsilon+sqr(g1);
}


void QFFitFunctionDLSG2inDExt::calcParameter(double* data, double* error) const {
   const int comp=data[DLSG2_n_components];
   const double D1=data[DLSG2_diff_coeff1];
   double eD1=0;
   const double D2=data[DLSG2_diff_coeff2];
   double eD2=0;
   const double D3=data[DLSG2_diff_coeff3];
   double eD3=0;
   const double D4=data[DLSG2_diff_coeff4];
   double eD4=0;
   const double D5=data[DLSG2_diff_coeff5];
   double eD5=0;
   const double D6=data[DLSG2_diff_coeff6];
   double eD6=0;
   const double D7=data[DLSG2_diff_coeff7];
   double eD7=0;
   const double D8=data[DLSG2_diff_coeff8];
   double eD8=0;

   const double n=data[DLSG2_refractive];
   double en=0;
   const double theta=data[DLSG2_angle]/180*M_PI;
   double etheta=0;
   const double lambda=data[DLSG2_wavelength]/1e3;
   double elambda=0;
   if (error) {
      en=error[DLSG2_refractive];
      etheta=error[DLSG2_angle]/180*M_PI;
      elambda=error[DLSG2_wavelength]/1e3;
      eD1=error[DLSG2_diff_coeff1];
      eD2=error[DLSG2_diff_coeff2];
      eD3=error[DLSG2_diff_coeff3];
      eD4=error[DLSG2_diff_coeff4];
      eD5=error[DLSG2_diff_coeff5];
      eD6=error[DLSG2_diff_coeff6];
      eD7=error[DLSG2_diff_coeff7];
      eD8=error[DLSG2_diff_coeff8];
   }

   const double q=4.0*M_PI*n/lambda*sin(theta/2.0);

   data[DLSG2_diff_qsquare]=sqr(q);
   double eq=0;
   if (error) {
      eq=sqrt(sqr(en*4.0*M_PI/lambda*sin(theta/2.0))+sqr(elambda*4.0*M_PI*n/lambda/lambda*sin(theta/2.0))+sqr(etheta*2.0*M_PI/lambda*cos(theta/2.0)));
      error[DLSG2_diff_qsquare]=eq*2*q;
   }

   data[DLSG2_tau1]=0;
   data[DLSG2_tau2]=0;
   data[DLSG2_tau3]=0;
   data[DLSG2_tau4]=0;
   data[DLSG2_tau5]=0;
   data[DLSG2_tau6]=0;
   data[DLSG2_tau7]=0;
   data[DLSG2_tau8]=0;
   if (error) {
      error[DLSG2_tau1]=0;
      error[DLSG2_tau2]=0;
      error[DLSG2_tau3]=0;
      error[DLSG2_tau4]=0;
      error[DLSG2_tau5]=0;
      error[DLSG2_tau6]=0;
      error[DLSG2_tau7]=0;
      error[DLSG2_tau8]=0;

   }

   data[DLSG2_tau1]=1.0/q/q/D1;
   if (error) error[DLSG2_tau1]=sqrt(sqr(eq*2.0/(cube(q)*D1))+sqr(eD1/(sqr(q)*sqr(D1))));

   if (comp>1) {
      data[DLSG2_tau2]=1.0/q/q/D2;
      if (error) error[DLSG2_tau2]=sqrt(sqr(eq*2.0/(cube(q)*D2))+sqr(eD2/(sqr(q)*sqr(D2))));

      if (comp>2) {
         data[DLSG2_tau3]=1.0/q/q/D3;
         if (error) error[DLSG2_tau3]=sqrt(sqr(eq*2.0/(cube(q)*D3))+sqr(eD3/(sqr(q)*sqr(D3))));

         if (comp>3) {
            data[DLSG2_tau4]=1.0/q/q/D4;
            if (error) error[DLSG2_tau4]=sqrt(sqr(eq*2.0/(cube(q)*D4))+sqr(eD4/(sqr(q)*sqr(D4))));

            if (comp>4) {
               data[DLSG2_tau5]=1.0/q/q/D5;
               if (error) error[DLSG2_tau5]=sqrt(sqr(eq*2.0/(cube(q)*D5))+sqr(eD5/(sqr(q)*sqr(D5))));

               if (comp>5) {
                  data[DLSG2_tau6]=1.0/q/q/D6;
                  if (error) error[DLSG2_tau6]=sqrt(sqr(eq*2.0/(cube(q)*D6))+sqr(eD6/(sqr(q)*sqr(D6))));

                  if (comp>6) {
                     data[DLSG2_tau7]=1.0/q/q/D7;
                     if (error) error[DLSG2_tau7]=sqrt(sqr(eq*2.0/(cube(q)*D7))+sqr(eD7/(sqr(q)*sqr(D7))));

                     if (comp>7) {
                        data[DLSG2_tau8]=1.0/q/q/D8;
                        if (error) error[DLSG2_tau8]=sqrt(sqr(eq*2.0/(cube(q)*D8))+sqr(eD8/(sqr(q)*sqr(D8))));

                     }
                  }
               }
            }
         }
      }
   }

}

bool QFFitFunctionDLSG2inDExt::isParameterVisible(int parameter, const double* data) const {
   int comp=data[DLSG2_n_components];
   switch(parameter) {
   case DLSG2_frac2:  case DLSG2_tau2:  case DLSG2_diff_coeff2: return comp>1;
   case DLSG2_frac3:  case DLSG2_tau3:  case DLSG2_diff_coeff3: return comp>2;
   case DLSG2_frac4:  case DLSG2_tau4:  case DLSG2_diff_coeff4: return comp>3;
   case DLSG2_frac5:  case DLSG2_tau5:  case DLSG2_diff_coeff5: return comp>4;
   case DLSG2_frac6:  case DLSG2_tau6:  case DLSG2_diff_coeff6: return comp>5;
   case DLSG2_frac7:  case DLSG2_tau7:  case DLSG2_diff_coeff7: return comp>6;
   case DLSG2_frac8:  case DLSG2_tau8:  case DLSG2_diff_coeff8: return comp>7;
   }
   return true;
}
