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

#include "qfgeneralfitfunctionbase.h"
#include "statistics_tools.h"
#include "qfmathtools.h"



QFFCSFitFunctionBase::QFFCSFitFunctionBase(double def_wxy):
    QFFitFunction()
{
    ids_found=false;
    est_defaultwxy=def_wxy;
}

bool QFFCSFitFunctionBase::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix) const
{
    getParameterIDs();
    bool ok=false;
    double o=0;
    if (id_offset>=0) {
        ok=true;
        if (N>3 && dataY && params) params[id_offset]=statisticsAverage(&(dataY[N-4]),4);
    }
    if (id_N>=0) {
        if (id_offset>=0 && params) o=params[id_offset];
        ok=true;
        int i=0;
        if (N>0 && dataX && dataY && params) {
            while (i<N && dataX[i]<1e-5) {
                i++;
            }
            params[id_N]=1.0/(dataY[i]-o);
        }
    }
    if (id_tauD1>=0 || id_D1>=0) {
        ok=true;
        int i=0;
        if (N>0 && dataX && dataY && params) {
            while (i<N && dataX[i]<1e-5) {
                i++;
            }
            double NN=dataY[i];
            double off=dataY[N-1];
            if (N>3) off=statisticsAverage(&(dataY[N-4]),4);
            while (i<N-1 && (dataY[i]>(NN-off)/2.0 || dataY[i+1]>(NN-off)/2.0)) {
                i++;
            }

            if (id_D1>=0 && DisFitParam && i<N) {
                double wxy=est_defaultwxy;
                if (params && id_wxy>=0) wxy=params[id_wxy]*1e-3;
                params[id_D1]=wxy*wxy/dataX[i];
                if (id_D2>=0) {
                    params[id_D2]=params[id_D1]*10.0;
                }
            }
            if (id_tauD1>=0 && !DisFitParam && i<N) {
                params[id_tauD1]=dataX[i]*1e6;
                if (id_tauD2>=0) {
                    params[id_tauD2]=params[id_tauD1]/10.0;
                }
            }
        }
    }
    if (ok) return true;
    else return QFFitFunction::estimateInitial(params, dataX, dataY, N, fix);
}

bool QFFCSFitFunctionBase::estimateParameterFromXY(double &newparam, int param, double x, double y, const double *params) const
{
    getParameterIDs();
    if (param>=0) {
        if (param==id_offset) {
            newparam=y;
            return true;
        }
        if (param==id_N) {
            newparam=1.0/y;
            return true;
        }
        if (param==id_C) {
            double wxy=est_defaultwxy;
            if (params && id_wxy>=0) wxy=params[id_wxy]*1e-3;
            double Veff=pow(M_PI, 3.0/2.0)*6.0*wxy*wxy;
            if (params && id_Veff>=0) Veff=params[id_Veff];
            if (params && id_Aeff>=0) Veff=params[id_Aeff];
            newparam=(1.0/y)/Veff;
            return true;
        }
        if (param==id_tauD1 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
        if (param==id_tauD2 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
        if (param==id_tauD3 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
        if (param==id_tauD4 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
        if (param==id_D1 && DisFitParam) {
            double wxy=est_defaultwxy;
            if (params && id_wxy>=0) wxy=params[id_wxy]*1e-3;
            newparam=wxy*wxy/x;
            return true;
        }
        if (param==id_D2 && DisFitParam) {
            double wxy=est_defaultwxy;
            if (params && id_wxy>=0) wxy=params[id_wxy]*1e-3;
            newparam=wxy*wxy/x;
            return true;
        }
        if (param==id_D3 && DisFitParam) {
            double wxy=est_defaultwxy;
            if (params && id_wxy>=0) wxy=params[id_wxy]*1e-3;
            newparam=wxy*wxy/x;
            return true;
        }
        if (param==id_D4 && DisFitParam) {
            double wxy=est_defaultwxy;
            if (params && id_wxy>=0) wxy=params[id_wxy]*1e-3;
            newparam=wxy*wxy/x;
            return true;
        }
        if (param==id_nonfl_tau1 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
        if (param==id_nonfl_tau2 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
        if (param==id_nonfl_tau3 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
        if (param==id_nonfl_tau4 && !DisFitParam) {
            newparam=x*1e6;
            return true;
        }
    }
    return QFFitFunction::estimateParameterFromXY(newparam, param, x, y, params);
}

void QFFCSFitFunctionBase::getParameterIDs() const
{
    if (ids_found) return;

    id_offset=getParameterNum("offset");
    //if (id_offset>=0 && !getDescription(id_offset).userEditable) id_offset=-1;

    id_N=getParameterNum("n_particle");
    if (id_N>=0 && !getDescription(id_N).userEditable) id_N=-1;
    id_wxy=getParameterNum("focus_width");
    //if (id_wxy>=0 && !getDescription(id_wxy).userEditable) id_wxy=-1;
    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_tauD1, "diff_tau1", "diff_tau");
    if (id_tauD1>=0 && !getDescription(id_tauD1).userEditable) id_tauD1=-1;
    id_D1=getParameterNum("diff_coeff1");
    if (id_D1>=0 && !getDescription(id_D1).userEditable) id_D1=-1;

    id_tauD2=getParameterNum("diff_tau2");
    if (id_tauD2>=0 && !getDescription(id_tauD2).userEditable) id_tauD2=-1;
    id_D2=getParameterNum("diff_coeff2");
    if (id_D2>=0 && !getDescription(id_D2).userEditable) id_D2=-1;
    id_tauD3=getParameterNum("diff_tau3");
    if (id_tauD3>=0 && !getDescription(id_tauD3).userEditable) id_tauD3=-1;
    id_D3=getParameterNum("diff_coeff3");
    if (id_D3>=0 && !getDescription(id_D3).userEditable) id_D3=-1;
    id_tauD4=getParameterNum("diff_tau4");
    if (id_tauD4>=0 && !getDescription(id_tauD4).userEditable) id_tauD4=-1;
    id_D4=getParameterNum("diff_coeff4");
    if (id_D4>=0 && !getDescription(id_D4).userEditable) id_D4=-1;

    DisFitParam=false;
    if (id_D1>=0) {
        DisFitParam=getDescription(id_D1).fit;
    }

    id_nonfl_tau1=getParameterNum("nonfl_tau1");
    //if (id_nonfl_tau1>=0 && !getDescription(id_nonfl_tau1).userEditable) id_nonfl_tau1=-1;
    id_nonfl_tau2=getParameterNum("nonfl_tau2");
    //if (id_nonfl_tau2>=0 && !getDescription(id_nonfl_tau2).userEditable) id_nonfl_tau2=-1;
    id_nonfl_tau3=getParameterNum("nonfl_tau3");
    //if (id_nonfl_tau3>=0 && !getDescription(id_nonfl_tau3).userEditable) id_nonfl_tau3=-1;
    id_nonfl_tau4=getParameterNum("nonfl_tau4");
    //if (id_nonfl_tau4>=0 && !getDescription(id_nonfl_tau4).userEditable) id_nonfl_tau4=-1;


    id_Veff=getParameterNum("focus_volume");
    //if (id_Veff>=0 && !getDescription(id_Veff).userEditable) id_Veff=-1;
    id_C=getParameterNum("concentration");
    if (id_C>=0 && !getDescription(id_C).userEditable) id_C=-1;
    id_Aeff=getParameterNum("effective_area");
    //if (id_Aeff>=0 && !getDescription(id_Aeff).userEditable) id_Aeff=-1;


    ids_found=true;
}



QFDistributionFitFunctionBase::QFDistributionFitFunctionBase(double width_factor, bool logX, bool logY):
    QFFitFunction()
{
    ids_found=false;
    this->width_factor=width_factor;
    this->logX=logX;
    this->logY=logY;
}

bool QFDistributionFitFunctionBase::estimateInitial(double *params, const double *dX, const double *dY, long N, const bool *fix) const
{
    getParameterIDs();
    int c=-1;
    if (id_components>=0 && params) c=params[id_components];
    if ((c<0 || c>=3) && id_amplitude>=0 && id_position>=0 && id_width>=0 && id_amplitude2>=0 && id_position2>=0 && id_width2>=0 && id_amplitude3>=0 && id_position3>=0 && id_width3>=0) {
        if (params && dX && dY && N>0) {
            StatisticsScopedPointer<double> dataX, dataY;
            if (logX) dataX=statisticsDuplicateAndApply(dX, N, log);
            else dataX=statisticsDuplicateArray(dX, N);
            if (logY) dataY=statisticsDuplicateAndApply(dY, N, log);
            else dataY=statisticsDuplicateArray(dY, N);
            double pW=0;
            double pB=0;
            double pH=0;
            double pP=0;
            double pW2=0;
            double pH2=0;
            double pP2=0;
            double pW3=0;
            double pH3=0;
            double pP3=0;
            if (statistics3PeakFind(pP, pW, pP2, pW2, pP3, pW3, dataX.data(), dataY.data(), N, 0.0, (double)NAN, &pB, &pH, &pH2, &pH3)) {
                double dx=0;
                statisticsMinDistance(dataX.data(), N, &dx);
                if (dx>0) {
                    pW=qMax(pW,6.0*dx);
                    pW2=qMax(pW2,6.0*dx);
                    pW3=qMax(pW3,6.0*dx);
                }
                if (id_offset>=0) params[id_offset]=pB;
                if (id_components>=0) params[id_components]=1;
                params[id_amplitude]=pH;
                if (logX) {
                    params[id_position]=pow(10.0,pP);
                } else {
                    params[id_position]=pP;
                }
                params[id_width]=pW*width_factor;
                if (statisticsFloatIsOK(pP2)) {
                    params[id_amplitude2]=pH2;
                    if (logX) {
                        params[id_position2]=pow(10.0,pP2);
                    } else {
                        params[id_position2]=pP2;
                    }
                    //params[id_position2]=pP2;
                    params[id_width2]=pW2*width_factor;
                    if (id_components>=0) params[id_components]=2;
                }
                if (statisticsFloatIsOK(pP3)) {
                    params[id_amplitude3]=pH3;
                    //params[id_position2]=pP3;
                    if (logX) {
                        params[id_position3]=pow(10.0,pP3);
                    } else {
                        params[id_position3]=pP3;
                    }

                    params[id_width3]=pW3*width_factor;
                    if (id_components>=0) params[id_components]=3;
                }
                return true;
            } else {
                return false;
            }
        }
        return true;
    } else if ((c<0 || c>=2) && id_amplitude>=0 && id_position>=0 && id_width>=0 && id_amplitude2>=0 && id_position2>=0 && id_width2>=0) {
        if (params && dX && dY && N>0) {
            StatisticsScopedPointer<double> dataX, dataY;
            if (logX) dataX=statisticsDuplicateAndApply(dX, N, log);
            else dataX=statisticsDuplicateArray(dX, N);
            if (logY) dataY=statisticsDuplicateAndApply(dY, N, log);
            else dataY=statisticsDuplicateArray(dY, N);
            double pW=0;
            double pB=0;
            double pH=0;
            double pP=0;
            double pW2=0;
            double pH2=0;
            double pP2=0;
            if (statistics2PeakFind(pP, pW, pP2, pW2, dataX.data(), dataY.data(), N, 0.0, (double)NAN, &pB, &pH, &pH2)) {
                double dx=0;
                statisticsMinDistance(dataX.data(), N, &dx);
                if (dx>0) {
                    pW=qMax(pW,6.0*dx);
                    pW2=qMax(pW2,6.0*dx);
                }
                if (id_offset>=0) params[id_offset]=pB;
                params[id_amplitude]=pH;
                if (logX) {
                    params[id_position]=pow(10.0,pP);
                } else {
                    params[id_position]=pP;
                }
                params[id_width]=pW*width_factor;
                if (id_components>=0) params[id_components]=1;
                if (statisticsFloatIsOK(pP2)) {
                    params[id_amplitude2]=pH2;
                    if (logX) {
                        params[id_position2]=pow(10.0,pP2);
                    } else {
                        params[id_position2]=pP2;
                    }
                    params[id_width2]=pW2*width_factor;
                    if (id_components>=0) params[id_components]=2;
                }
                return true;
            } else {
                return false;
            }
        }
        return true;
    } else if (id_amplitude>=0 && id_position>=0 && id_width>=0) {
        if (params && dX && dY && N>0) {
            StatisticsScopedPointer<double> dataX, dataY;
            if (logX) dataX=statisticsDuplicateAndApply(dX, N, log);
            else dataX=statisticsDuplicateArray(dX, N);
            if (logY) dataY=statisticsDuplicateAndApply(dY, N, log);
            else dataY=statisticsDuplicateArray(dY, N);
            double pW=0;
            double pB=0;
            double pH=0;
            double pP=statisticsPeakFind(pW, dataX.data(), dataY.data(), N, 0.0, (double)NAN, &pB, &pH);
            if (statisticsFloatIsOK(pP)) {
                double dx=0;
                statisticsMinDistance(dataX.data(), N, &dx);
                if (dx>0) {
                    pW=qMax(pW,6.0*dx);
                }
                if (id_offset>=0) params[id_offset]=pB;
                params[id_amplitude]=pH;
                if (logX) {
                    params[id_position]=pow(10.0,pP);
                } else {
                    params[id_position]=pP;
                }
                params[id_width]=pW*width_factor;
                if (id_components>=0) params[id_components]=1;
                return true;
            } else {
                return false;
            }
        }
        return true;
    }

    return QFFitFunction::estimateInitial(params, dX, dY, N, fix);
}

bool QFDistributionFitFunctionBase::estimateParameterFromXY(double &newparam, int param, double x, double y, const double *params) const
{
    getParameterIDs();
    if (param>=0) {
        if (param==id_offset) {
            newparam=y;
            return true;
        }
        if (param==id_amplitude || param==id_amplitude2 || param==id_amplitude3 || param==id_amplitude4 || param==id_amplitude5 || param==id_amplitude6) {
            newparam=y;
            if (params && id_offset>=0) newparam=y-params[id_offset];
            return true;
        }
        if (param==id_position || param==id_position2 || param==id_position3 || param==id_position4 || param==id_position5 || param==id_position6) {
            newparam=x;
            return true;
        }

    }
    return QFFitFunction::estimateParameterFromXY(newparam, param, x, y, params);
}

void QFDistributionFitFunctionBase::getParameterIDs() const
{
    if (ids_found) return;

    QFFITFUNCTRIONBASE_GETPARAMID(id_offset, "offset");
    QFFITFUNCTRIONBASE_GETPARAMID(id_components, "components");

    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_amplitude, "amplitude", "amplitude1");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude2, "amplitude2");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude3, "amplitude3");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude4, "amplitude4");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude5, "amplitude5");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude6, "amplitude6");

    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_position, "position", "position1");
    QFFITFUNCTRIONBASE_GETPARAMID(id_position2, "position2");
    QFFITFUNCTRIONBASE_GETPARAMID(id_position3, "position3");
    QFFITFUNCTRIONBASE_GETPARAMID(id_position4, "position4");
    QFFITFUNCTRIONBASE_GETPARAMID(id_position5, "position5");
    QFFITFUNCTRIONBASE_GETPARAMID(id_position6, "position6");


    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_width, "width", "width1");
    QFFITFUNCTRIONBASE_GETPARAMID(id_width2, "width2");
    QFFITFUNCTRIONBASE_GETPARAMID(id_width3, "width3");
    QFFITFUNCTRIONBASE_GETPARAMID(id_width4, "width4");
    QFFITFUNCTRIONBASE_GETPARAMID(id_width5, "width5");
    QFFITFUNCTRIONBASE_GETPARAMID(id_width6, "width6");
    //qDebug()<<"QFDistributionFitFunctionBase id_offset="<<id_offset<<" id_components="<<id_components<<" id_amplitude="<<id_amplitude<<id_amplitude2<<id_amplitude3<<4<<id_amplitude5<<id_amplitude6<<" id_position="<<id_position<<id_position2<<id_position3<<id_position4<<id_position5<<id_position6<<" id_width="<<id_width<<id_width2<<id_width3<<id_width4<<id_width5<<id_width6;

    ids_found=true;
}


QFCDFFitFunctionBase::QFCDFFitFunctionBase(double range_factor, bool logX, bool logY)
{
    ids_found=false;
    this->range_factor=range_factor;
    this->logX=logX;
    this->logY=logY;
}

bool QFCDFFitFunctionBase::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix) const
{
    getParameterIDs();
    if (id_position>=0 || id_width>=0 || id_base>=0 || id_max>=0 || id_amplitude>=0 || id_offset>=0) {
        if (params && dataX && dataY && N>0) {
            StatisticsScopedPointer<double> dX, dY;
            if (logX) dX=statisticsDuplicateAndApply(dataX, N, log);
            else dX=statisticsDuplicateArray(dataX, N);
            if (logY) dY=statisticsDuplicateAndApply(dataY, N, log);
            else dY=statisticsDuplicateArray(dataY, N);
            statisticsSort2(dX.data(), dY.data(), N);

            double mi=0, ma=0;

            statisticsMinMax(dY.data(), N, mi, ma);

            double p=statisticsXatY50Sorted(dX.data(), dY.data(), N);
            double range=(dX[N-1]-dX[0])*range_factor;
            if (dY[N-1]-dY[0]<0) {
                range=range*-1.0;
            }

            if(id_base>=0) params[id_base]=mi;
            if(id_max>=0) params[id_max]=ma;
            if(id_amplitude>=0) params[id_amplitude]=ma-mi;
            if(id_offset>=0) params[id_offset]=mi;
            if (id_position>=0) params[id_position]=p;
            if (id_width>=0) params[id_width]=range*range_factor;
            if (id_rate>=0) params[id_rate]=range*range_factor;
        }
        return true;
    }

    return QFFitFunction::estimateInitial(params, dataX, dataY, N, fix);

}

bool QFCDFFitFunctionBase::estimateParameterFromXY(double &newparam, int param, double x, double y, const double *params) const
{
    getParameterIDs();
    if (param>=0) {
        if (param==id_offset || param==id_base || param==id_max) {
            newparam=y;
            return true;
        }
        if (param==id_amplitude) {
            newparam=y;
            if (params && id_offset>=0) newparam=y-params[id_offset];
            return true;
        }
        if (param==id_position) {
            newparam=x;
            return true;
        }

    }
    return QFFitFunction::estimateParameterFromXY(newparam, param, x, y, params);

}

void QFCDFFitFunctionBase::getParameterIDs() const
{
    if (ids_found) return;

    QFFITFUNCTRIONBASE_GETPARAMID(id_max, "max");
    QFFITFUNCTRIONBASE_GETPARAMID(id_rate, "rate");
    QFFITFUNCTRIONBASE_GETPARAMID(id_base, "base");
    QFFITFUNCTRIONBASE_GETPARAMID(id_offset, "offset");
    QFFITFUNCTRIONBASE_GETPARAMID(id_components, "components");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude, "amplitude");
    QFFITFUNCTRIONBASE_GETPARAMID(id_position, "position");
    QFFITFUNCTRIONBASE_GETPARAMID(id_width, "width");

    ids_found=true;
}


QFDecayFitFunctionBase::QFDecayFitFunctionBase()
{
    ids_found=false;
}

bool QFDecayFitFunctionBase::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix) const
{
    getParameterIDs();
    if (id_position>=0 || id_lifetime>=0 || id_base>=0 || id_max>=0 || id_amplitude>=0 || id_offset>=0) {

        if (params && dataX && dataY) {

            StatisticsScopedPointer<double> dY(statisticsDuplicateAndApply(dataY, N, log));


            double a=1;
            if (id_amplitude>=0) a=log(params[id_amplitude]);
            double b=-1;
            if (id_lifetime>=0) b=-1.0/params[id_lifetime];


            if (fix) statisticsLinearRegression(dataX, dY.data(), N, a, b, (id_amplitude>=0)?fix[id_amplitude]:false, (id_lifetime>=0)?fix[id_lifetime]:false);
            else statisticsLinearRegression(dataX, dY.data(), N, a, b, false, false);

            if (id_offset>=0) params[id_offset]=0;
            if (id_base>=0) params[id_base]=0;
            if (id_position>=0) params[id_position]=0;
            if (id_max>=0) params[id_max]=exp(a);
            if (id_amplitude>=0) params[id_amplitude]=exp(a);
            if (id_amplitude2>=0) params[id_amplitude2]=exp(a);
            if (id_lifetime>=0) params[id_lifetime]=-1.0/b;
            if (id_lifetime2>=0) params[id_lifetime2]=-1.0/b/10.0;
        }
        return true;
    }

    return QFFitFunction::estimateInitial(params, dataX, dataY, N, fix);
}

bool QFDecayFitFunctionBase::estimateParameterFromXY(double &newparam, int param, double x, double y, const double *params) const
{
    getParameterIDs();
    if (param>=0) {
        if (param==id_offset || param==id_base || param==id_max) {
            newparam=y;
            return true;
        }
        if (param==id_amplitude || param==id_amplitude2 || param==id_amplitude3 || param==id_amplitude4) {
            newparam=y;
            if (params && id_offset>=0) newparam=y-params[id_offset];
            return true;
        }
        if (param==id_lifetime || param==id_lifetime2 || param==id_lifetime3 || param==id_lifetime4) {
            newparam=x;
            return true;
        }
        if (param==id_position) {
            newparam=x;
            return true;
        }

    }
    return QFFitFunction::estimateParameterFromXY(newparam, param, x, y, params);
}

void QFDecayFitFunctionBase::getParameterIDs() const
{
    if (ids_found) return;

    QFFITFUNCTRIONBASE_GETPARAMID(id_max, "max");
    QFFITFUNCTRIONBASE_GETPARAMID(id_rate, "rate");
    QFFITFUNCTRIONBASE_GETPARAMID(id_base, "base");
    QFFITFUNCTRIONBASE_GETPARAMID(id_offset, "offset");
    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_position, "position", "position1");
    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_amplitude, "amplitude", "amplitude1");
    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_lifetime, "lifetime", "lifetime1");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude2, "amplitude2");
    QFFITFUNCTRIONBASE_GETPARAMID(id_lifetime2, "lifetime2");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude3, "amplitude3");
    QFFITFUNCTRIONBASE_GETPARAMID(id_lifetime3, "lifetime3");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude4, "amplitude4");
    QFFITFUNCTRIONBASE_GETPARAMID(id_lifetime4, "lifetime4");

    ids_found=true;
}


QFDLSFitFunctionBase::QFDLSFitFunctionBase()
{
    ids_found=false;
}

bool QFDLSFitFunctionBase::estimateInitial(double *params, const double *dataX, const double *dataY, long N, const bool *fix) const
{
    getParameterIDs();
    bool ok=false;
    if (id_offset>=0) {
        ok=true;
        if (N>3 && dataY && params) params[id_offset]=statisticsAverage(&(dataY[N-4]),4);
    }
    if (id_epsilon>=0 || id_amplitude>=0) {
        ok=true;
        int i=0;
        double o=0;
        if (id_offset>=0 && params) o=params[id_offset];
        if (N>0 && dataX && dataY && params) {
            while (i<N && dataX[i]<5e-6) {
                i++;
            }
            if (id_epsilon>=0) params[id_epsilon]=dataY[i]-o;
            if (id_amplitude>=0) params[id_amplitude]=dataY[i]-o;
        }
    }
    if (id_tauD1>=0 ) {
        ok=true;
        int i=0;
        if (N>0 && dataX && dataY && params) {
            while (i<N && dataX[i]<1e-5) {
                i++;
            }
            double NN=dataY[i];
            double off=dataY[N-1];
            if (N>3) off=statisticsAverage(&(dataY[N-4]),4);
            while (i<N-1 && (dataY[i]>(NN-off)/2.0 || dataY[i+1]>(NN-off)/2.0)) {
                i++;
            }

            if (id_tauD1>=0 && i<N) {
                params[id_tauD1]=dataX[i]*1e6;
                if (id_tauD2>=0) {
                    params[id_tauD2]=params[id_tauD1]/10.0;
                }
            }
        }
    }
    if (ok) return true;
    else return QFFitFunction::estimateInitial(params, dataX, dataY, N, fix);

}

bool QFDLSFitFunctionBase::estimateParameterFromXY(double &newparam, int param, double x, double y, const double *params) const
{
    getParameterIDs();
    if (param>=0) {
        if (param==id_offset || param==id_epsilon || param==id_amplitude) {
            newparam=y;
            return true;
        }
        if (param==id_tauD1 || param==id_tauD2 || param==id_tauD3 || param==id_tauD4) {
            newparam=x*1e6;
        }
    }
    return QFFitFunction::estimateParameterFromXY(newparam, param, x, y, params);

}

void QFDLSFitFunctionBase::getParameterIDs() const
{
    if (ids_found) return;

    QFFITFUNCTRIONBASE_GETPARAMID(id_epsilon, "epsilon");
    QFFITFUNCTRIONBASE_GETPARAMID(id_amplitude, "amplitude");
    QFFITFUNCTRIONBASE_GETPARAMID(id_offset, "offset");
    QFFITFUNCTRIONBASE_GETPARAMIDALT(id_tauD1, "tau1", "tau");
    QFFITFUNCTRIONBASE_GETPARAMID(id_tauD2, "tau2");
    QFFITFUNCTRIONBASE_GETPARAMID(id_tauD3, "tau3");
    QFFITFUNCTRIONBASE_GETPARAMID(id_tauD4, "tau4");

    ids_found=true;
}
