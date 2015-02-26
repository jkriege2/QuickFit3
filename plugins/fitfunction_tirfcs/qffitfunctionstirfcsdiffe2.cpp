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

#include "qffitfunctionstirfcsdiffe2.h"
#include <cmath>
#include "qftools.h"
#include "imfcstools.h"
#define sqr(x) qfSqr(x)
#define cube(x) qfCube(x)
#define pow4(x) qfPow4(x)
#define NAVOGADRO QF_NAVOGADRO

QFFitFunctionsTIRFCSDiffE2::QFFitFunctionsTIRFCSDiffE2() {
    //           type,         id,                        name,                                                    label,                      unit,          unitlabel,               fit,       userEditable, userRangeEditable, displayError,                initialValue, minValue, maxValue, inc, absMin, absMax
    addParameter(IntCombo,     "n_components",            "number of diffusing components",                        "components",               "",            "",                      false,     true,         false,             QFFitFunction::NoError,      false, 1,            1,        3,        1,   1,      2);
    #define FCSSDiff_n_components 0
    addParameter(FloatNumber,  "n_particle",              "particle number N",                                     "N",                        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 100,          1e-10,    1e5,      1,   0);
    #define FCSSDiff_n_particle 1
    addParameter(FloatNumber,  "1n_particle",             "1/particle number N",                                   "1/N",                      "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.01,         1e-10,    1e5,      0.1, 0);
    #define FCSSDiff_1n_particle 2

    addParameter(FloatNumber,  "diff_rho1",               "fraction of first component",                           "&rho;<sub>1</sub>",        "",            "",                      false,     false,        false,             QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1);
    #define FCSSDiff_diff_rho1 3
    addParameter(FloatNumber,  "diff_coeff1",             "diffusion coefficient of species 1",                    "D<sub>1</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff1 4
    addParameter(FloatNumber,  "diff_rho2",               "fraction of second component",                          "&rho;<sub>2</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho2 5
    addParameter(FloatNumber,  "diff_coeff2",             "diffusion coefficient of species 2",                    "D<sub>2</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff2 6
    addParameter(FloatNumber,  "diff_rho3",               "fraction of third component",                           "&rho;<sub>3</sub>",        "",            "",                      true,      true,         true,              QFFitFunction::DisplayError, false, 0.5,          0,        0.99999,  0.1, 0,      1  );
    #define FCSSDiff_diff_rho3 7
    addParameter(FloatNumber,  "diff_coeff3",             "diffusion coefficient of species 3",                    "D<sub>3</sub>",            "micron^2/s", "&mu;m<sup>2</sup>/s",    true,      true,         true,              QFFitFunction::DisplayError, false, 10,           1e-10,    1e50,     1    );
    #define FCSSDiff_diff_coeff3 8

    addParameter(FloatNumber,  "offset",                  "correlation offset",                                    "G<sub>&infin;</sub>",      "",           "",                       true,      true,         true,              QFFitFunction::DisplayError, true, 0,            -10,      10,       0.1  );
    #define FCSSDiff_offset 9
    addParameter(FloatNumber,  "focus_width",             "PSF: lateral radius (1/e^2 radius)",                     "w<sub>x,y</sub>",    "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 600,          0,        1e4,      10    );
    #define FCSSDiff_focus_width 10
    addParameter(FloatNumber,  "pixel_width",             "pixel width",                                           "a",                        "nm",         "nm",                     true,      true,         true,              QFFitFunction::EditError,    true, 400,          0,        1e4,      10    );
    #define FCSSDiff_pixel_width 11
    addParameter(FloatNumber,  "effective_area",            "focus: effective area",                               "A<sub>eff</sub>",          "micron^2",         "&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_focus_volume 12
    addParameter(FloatNumber,  "concentration",           "particle concentration in focus",                       "C<sub>all</sub>",          "particles/micron^2",         "particles/&mu;m<sup>2</sup>",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define FCSSDiff_concentration 13
    addParameter(FloatNumber,  "count_rate",              "count rate during measurement",                         "count rate",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError,    false, 0,            0,        1e50,     1    );
    #define FCSSDiff_count_rate 14
    addParameter(FloatNumber,  "background",              "background count rate during measurement",              "background",               "Hz",         "Hz",                     false,    true,         false,              QFFitFunction::EditError  ,  false, 0,            0,        1e50,     1    );
    #define FCSSDiff_background 15
    addParameter(FloatNumber,  "cpm",                     "photon counts per molecule",                            "cnt/molec",                "Hz",         "Hz",                     false,    false,        false,              QFFitFunction::DisplayError, false, 0,            0,        1e50,     1    );
    #define FCSSDiff_cpm 16


    addParameter(FloatNumber,  "msd_tau1",                "evaluate MSD at this time",                             "&tau;<sub>MSD</sub>",      "ms",         "ms",                     false,    true,         false,              QFFitFunction::NoError  ,   false, 1,            1e-10,        1e50,     1    );
    #define SPIMFCSADIFF_msd_tau 17
    addParameter(FloatNumber,  "msd_at_tau1",              "MSD1 evaluated at given time tau_MSD",                  "MSD1(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau1 18
    addParameter(FloatNumber,  "msd_at_tau2",              "MSD2 evaluated at given time tau_MSD",                  "MSD2(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau2 19
    addParameter(FloatNumber,  "msd_at_tau3",              "MSD3 evaluated at given time tau_MSD",                  "MSD3(&tau;<sub>MSD</sub>)",  "micron^2",         "&mu;m<sup>2</sup>",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_at_tau3 20
    addParameter(FloatNumber,  "msd_time_aeff1",              "time to leave effective focus area with MSD1",      "&tau;<sub>1</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff1 21
    addParameter(FloatNumber,  "msd_time_aeff2",              "time to leave effective focus area with MSD2",      "&tau;<sub>2</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff2 22
    addParameter(FloatNumber,  "msd_time_aeff3",              "time to leave effective focus area with MSD3",      "&tau;<sub>3</sub>(A<sub>eff</sub>)",  "ms",         "ms",             false,    false,        false,              QFFitFunction::DisplayError, false, 0.5,          0,        1e50,     1    );
    #define SPIMFCSADIFF_msd_time_aeff3 23

}

double QFFitFunctionsTIRFCSDiffE2::evaluate(double t, const double* data) const {
    const int comp=data[FCSSDiff_n_components];
    const double N=data[FCSSDiff_n_particle];
    const double D1=data[FCSSDiff_diff_coeff1];
    const double D2=data[FCSSDiff_diff_coeff2];
    const double rho2=data[FCSSDiff_diff_rho2];
    const double D3=data[FCSSDiff_diff_coeff3];
    const double rho3=data[FCSSDiff_diff_rho3];
    const double wxy=data[FCSSDiff_focus_width]/1.0e3;
    const double a=data[FCSSDiff_pixel_width]/1.0e3;
    const double offset=data[FCSSDiff_offset];
    const double background=data[FCSSDiff_background];
    const double cr=data[FCSSDiff_count_rate];
    double backfactor=sqr(cr-background)/sqr(cr);
    if (fabs(cr)<1e-15 || fabs(background)<1e-10) backfactor=1;

    double rho1=1;
    if (comp==2) {
        rho1=1.0-rho2;
    }
    if (comp==3) {
        rho1=1.0-rho2-rho3;
    }

    double cfac=0;

    if (comp>0) {
        cfac=rho1*QFFitFunctionsTIRFCCSFWDiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, D1, t, wxy, wxy);
    }

    if (comp>1) {
        cfac=cfac+rho2*QFFitFunctionsTIRFCCSFWDiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, D2, t, wxy, wxy);
    }

    if (comp>2) {
        cfac=cfac+rho3*QFFitFunctionsTIRFCCSFWDiff2ColorCCF_corrfactor_2Dxy(a, 0, 0, D3, t, wxy, wxy);
    }

    const double Veff=TIRFCS_newAeff(a, wxy);
    const double pre=1.0/sqr(a);
    return offset+pre/(N/Veff)*cfac*backfactor;
}

void QFFitFunctionsTIRFCSDiffE2::evaluateDerivatives(double* derivatives, double t, const double* data) const {
}

void QFFitFunctionsTIRFCSDiffE2::sortParameter(double *parameterValues, double *error, bool *fix) const {

    const int comp=parameterValues[FCSSDiff_n_components];
    QList<QPair<int, double> > dataForSort;

    if (comp>1) {
        if (!fix || (!fix[FCSSDiff_diff_coeff1]))dataForSort.append(qMakePair(FCSSDiff_diff_coeff1, parameterValues[FCSSDiff_diff_coeff1]));
        if (!fix || (!fix[FCSSDiff_diff_coeff2]))dataForSort.append(qMakePair(FCSSDiff_diff_coeff2, parameterValues[FCSSDiff_diff_coeff2]));
        if (comp>2) {
            if (!fix || (!fix[FCSSDiff_diff_coeff3]))dataForSort.append(qMakePair(FCSSDiff_diff_coeff3, parameterValues[FCSSDiff_diff_coeff3]));
        }

        if (dataForSort.size()>1) {
            QList<int> init, final;
            final=qfQPairListToFirstList(dataForSort);
            //QList<double> initvals=qfQPairListToSecondList(dataForSort);
            qSort(dataForSort.begin(), dataForSort.end(), qfQPairCompareSecondDec<int, double>);
            init=qfQPairListToFirstList(dataForSort);
            /*qDebug()<<"sorting: "<<id();
            qDebug()<<"   Ds:";
            qDebug()<<"      init  = "<<init;
            qDebug()<<"       vals = "<<qfSelectFromVector(init, parameterValues);
            qDebug()<<"       vals = "<<qfSelectFromVector(init, error);
            qDebug()<<"      final = "<<final;*/

            qfSortVectorByPermutation(init, final, parameterValues);
            if (error) qfSortVectorByPermutation(init, final, error);

            // now we sort the diffusion fractions, using that ID_rhoI=ID_coeffI-1:
            for (int i=0; i<qMin(init.size(), final.size()); i++) {
                init[i]=init[i]-1.0;
                final[i]=final[i]-1.0;
            }
            /*qDebug()<<"   rhos:";
            qDebug()<<"      init  = "<<init;
            if (error) qDebug()<<"       vals = "<<qfSelectFromVector(init, error);
            if (fix) qDebug()<<"       vals = "<<qfSelectFromVector(init, fix);
            qDebug()<<"      final = "<<final;*/

            qfSortVectorByPermutation(init, final, parameterValues);
            if (error) qfSortVectorByPermutation(init, final, error);
            if (fix) qfSortVectorByPermutation(init, final, fix);
            //qDebug()<<"   fix = "<<fix<<"   error = "<<error;
        }

    }
}

void QFFitFunctionsTIRFCSDiffE2::calcParameter(double* data, double* error) const {
    double N=data[FCSSDiff_n_particle];
    double eN=0;
    //double D1=data[FCSSDiff_diff_coeff1];
    double eD1=0;
    double wxy=data[FCSSDiff_focus_width]/1.0e3;
    double ewxy=0;
    double a=data[FCSSDiff_pixel_width]/1.0e3;
    double ea=0;
    //double offset=data[FCSSDiff_offset];
    double eoffset=0;

    int comp=data[FCSSDiff_n_components];
    double rho2=data[FCSSDiff_diff_rho2];
    double erho2=0;
    double rho3=data[FCSSDiff_diff_rho3];
    double erho3=0;
    double cps=data[FCSSDiff_count_rate];
    double ecps=0;
    double ecpm=0;
    double background=data[FCSSDiff_background];
    double ebackground=0;

    double tau_msd=data[SPIMFCSADIFF_msd_tau];
    double etau_msd=0;
    const double D1=data[FCSSDiff_diff_coeff1];
    const double D2=data[FCSSDiff_diff_coeff2];
    const double D3=data[FCSSDiff_diff_coeff3];

    if (error) {
        eN=error[FCSSDiff_n_particle];
        eD1=error[FCSSDiff_diff_coeff1];
        ewxy=error[FCSSDiff_focus_width]/1.0e3;
        ea=error[FCSSDiff_pixel_width]/1.0e3;
        eoffset=error[FCSSDiff_offset];
        erho2=error[FCSSDiff_diff_rho2];
        erho3=error[FCSSDiff_diff_rho3];
        ecps=error[FCSSDiff_count_rate];
        ecpm=error[FCSSDiff_cpm];
        ebackground=error[FCSSDiff_background];
    }


    // calculate rho1
    double rho1=1;
    double erho1=0;
    if (comp==2) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        if (rho2>1.0) {
            rho1=0;
            rho2=0.5;
        }
        rho1=1.0-rho2;
        erho1=erho2;
    }
    if (comp==3) {
        if (rho2>1.0) rho2=1.0;
        if (rho2<0.0) rho2=0.0;
        if (rho3>1.0) rho3=1.0;
        if (rho3<0.0) rho3=0.0;
        if (rho2+rho3>1.0) {
            rho1=0;
            rho2=rho3=0.5;
        }
        rho1=1.0-rho2-rho3;
        erho1=sqrt(erho2*erho2+erho3*erho3);
    }

    data[FCSSDiff_diff_rho1]=rho1;
    data[FCSSDiff_diff_rho2]=rho2;
    data[FCSSDiff_diff_rho3]=rho3;
    if (error) {
        error[FCSSDiff_diff_rho1]=erho1;
        error[FCSSDiff_diff_rho2]=erho2;
        error[FCSSDiff_diff_rho3]=erho3;
    }


    // calculate 1/N
    data[FCSSDiff_1n_particle]=1.0/N;
    if (error) error[FCSSDiff_1n_particle]=fabs(eN/N/N);

    // calculate Veff
    double Aeff=data[FCSSDiff_focus_volume]=TIRFCS_newAeff(a, wxy);;
    if (error) error[FCSSDiff_focus_volume]=TIRFCS_newAeffError(a, ea, wxy, ewxy);

    // calculate C = N / Veff
    if (data[FCSSDiff_focus_volume]!=0) {
        data[FCSSDiff_concentration]=N/data[FCSSDiff_focus_volume];
    } else {
        data[FCSSDiff_concentration]=0;
    }
    if (data[FCSSDiff_focus_volume]!=0 && error) {
        error[FCSSDiff_concentration]=sqrt(qfSqr(eN/data[FCSSDiff_focus_volume])+qfSqr(error[FCSSDiff_focus_volume]*N/qfSqr(data[FCSSDiff_focus_volume])));//N/data[FCSSDiff_focus_volume]
    }


    // calculate CPM = (CPS-background)/N
    data[FCSSDiff_cpm]=(cps-background)/N;
    error[FCSSDiff_cpm]=sqrt(sqr(ecps/N)+sqr(ebackground/N)+sqr(eN*(cps-background)/sqr(N)));

    data[SPIMFCSADIFF_msd_at_tau1]=6.0*D1*tau_msd*1e-3;
    if (error) error[SPIMFCSADIFF_msd_at_tau1]=0;
    data[SPIMFCSADIFF_msd_at_tau2]=6.0*D2*tau_msd*1e-3;
    if (error) error[SPIMFCSADIFF_msd_at_tau2]=0;
    data[SPIMFCSADIFF_msd_at_tau3]=6.0*D3*tau_msd*1e-3;
    if (error) error[SPIMFCSADIFF_msd_at_tau3]=0;
    data[SPIMFCSADIFF_msd_time_aeff1]=Aeff/(4.0*D1)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff1]=0;
    data[SPIMFCSADIFF_msd_time_aeff2]=Aeff/(4.0*D2)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff2]=0;
    data[SPIMFCSADIFF_msd_time_aeff3]=Aeff/(4.0*D3)*1e3;
    if (error) error[SPIMFCSADIFF_msd_time_aeff3]=0;
}

bool QFFitFunctionsTIRFCSDiffE2::isParameterVisible(int parameter, const double* data) const {
    int comp=data[FCSSDiff_n_components];
    switch(parameter) {
        case FCSSDiff_diff_rho1:  return comp>1;
        case FCSSDiff_diff_coeff1: return comp>0;
        case FCSSDiff_diff_rho2: case FCSSDiff_diff_coeff2: case SPIMFCSADIFF_msd_at_tau2: case SPIMFCSADIFF_msd_time_aeff2: return comp>1;
        case FCSSDiff_diff_rho3: case FCSSDiff_diff_coeff3: case SPIMFCSADIFF_msd_at_tau3: case SPIMFCSADIFF_msd_time_aeff3: return comp>2;
    }
    return true;
}

unsigned int QFFitFunctionsTIRFCSDiffE2::getAdditionalPlotCount(const double* params) {
    return 0;
}

QString QFFitFunctionsTIRFCSDiffE2::transformParametersForAdditionalPlot(int plot, double* params) {
    return "";
}
