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

#ifndef QFGENERALFITFUNCTIONBASE_H
#define QFGENERALFITFUNCTIONBASE_H
#include "qfpluginfitfunction.h"
#include "qffitfunction.h"
#include "lib_imexport.h"

#define QFFITFUNCTRIONBASE_GETPARAMID(idvar, pid) {\
    idvar=getParameterNum(pid);\
    }
#define QFFITFUNCTRIONBASE_GETPARAMIDALT(idvar, pid, pid_alt) {\
    idvar=getParameterNum(pid);\
    if (idvar<0) idvar=getParameterNum(pid_alt);\
    }
#define QFFITFUNCTRIONBASE_GETPARAMID_CHECKEDITABLE(idvar, pid) {\
    idvar=getParameterNum(pid);\
    if (idvar>=0 && !getDescription(idvar).userEditable) idvar=-1;\
    }


/** \brief a base class for FCS fit functions that implements parameter estimation, based on generic parameter names */
class QFLIB_EXPORT QFFCSFitFunctionBase : public QFFitFunction
{
    public:
        QFFCSFitFunctionBase(double def_wxy_micrometers=0.25);

        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL) const;
        virtual bool estimateParameterFromXY(double& newparam, int param, double x, double y, const double* params=NULL) const;

    protected:
        void getParameterIDs() const;

        mutable bool ids_found;
        mutable int id_N;
        mutable int id_C;
        mutable int id_Veff;
        mutable int id_Aeff;
        mutable int id_wxy;
        mutable int id_tauD1;
        mutable int id_tauD2;
        mutable int id_tauD3;
        mutable int id_tauD4;
        mutable int id_D1;
        mutable int id_D2;
        mutable int id_D3;
        mutable int id_D4;
        mutable int id_offset;
        mutable int id_nonfl_tau1;
        mutable int id_nonfl_tau2;
        mutable int id_nonfl_tau3;
        mutable int id_nonfl_tau4;
        mutable bool DisFitParam;
        double est_defaultwxy;

};


/** \brief a base class for FCS fit functions that implements parameter estimation, based on generic parameter names */
class QFLIB_EXPORT QFDLSFitFunctionBase : public QFFitFunction
{
    public:
        QFDLSFitFunctionBase();

        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL) const;
        virtual bool estimateParameterFromXY(double& newparam, int param, double x, double y, const double* params=NULL) const;

    protected:
        void getParameterIDs() const;

        mutable bool ids_found;
        mutable int id_tauD1;
        mutable int id_tauD2;
        mutable int id_tauD3;
        mutable int id_tauD4;
        mutable int id_offset;
        mutable int id_epsilon;
        mutable int id_amplitude;

};
class QFLIB_EXPORT QFFCSLargeFocusFitFunctionBase : public QFFCSFitFunctionBase
{
    public:
        QFFCSLargeFocusFitFunctionBase(double def_wxy_micrometers=0.5):
            QFFCSFitFunctionBase(def_wxy_micrometers)
        {
        }
};


/** \brief a base class for Distribution/Peak fit functions that implements parameter estimation, based on generic parameter names */
class QFLIB_EXPORT QFDistributionFitFunctionBase : public QFFitFunction
{
    public:
        QFDistributionFitFunctionBase(double width_factor=1.0/2.3548, bool logX=false, bool logY=false);

        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL) const;
        virtual bool estimateParameterFromXY(double& newparam, int param, double x, double y, const double* params=NULL) const;

    protected:
        void getParameterIDs() const;

        mutable bool ids_found;
        mutable int id_amplitude;
        mutable int id_amplitude2;
        mutable int id_amplitude3;
        mutable int id_amplitude4;
        mutable int id_amplitude5;
        mutable int id_amplitude6;
        mutable int id_position;
        mutable int id_position2;
        mutable int id_position3;
        mutable int id_position4;
        mutable int id_position5;
        mutable int id_position6;
        mutable int id_width;
        mutable int id_width2;
        mutable int id_width3;
        mutable int id_width4;
        mutable int id_width5;
        mutable int id_width6;
        mutable int id_offset;
        mutable int id_components;

        double width_factor;
        bool logY;
        bool logX;

};



/** \brief a base class for Distribution/Peak fit functions that implements parameter estimation, based on generic parameter names */
class QFLIB_EXPORT QFCDFFitFunctionBase : public QFFitFunction
{
    public:
        QFCDFFitFunctionBase(double range_factor=1.0/5.0, bool logX=false, bool logY=false);

        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL) const;
        virtual bool estimateParameterFromXY(double& newparam, int param, double x, double y, const double* params=NULL) const;

    protected:
        void getParameterIDs() const;

        mutable bool ids_found;
        mutable int id_amplitude;
        mutable int id_position;
        mutable int id_width;
        mutable int id_offset;
        mutable int id_components;
        mutable int id_max;
        mutable int id_rate;
        mutable int id_base;

        double range_factor;
        bool logY;
        bool logX;

};




/** \brief a base class for decaying fit functions that implements parameter estimation, based on generic parameter names */
class QFLIB_EXPORT QFDecayFitFunctionBase : public QFFitFunction
{
    public:
        QFDecayFitFunctionBase();

        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL) const;
        virtual bool estimateParameterFromXY(double& newparam, int param, double x, double y, const double* params=NULL) const;

    protected:
        void getParameterIDs() const;

        mutable bool ids_found;
        mutable int id_amplitude;
        mutable int id_position;
        mutable int id_offset;
        mutable int id_max;
        mutable int id_rate;
        mutable int id_base;
        mutable int id_lifetime;
        mutable int id_lifetime2;
        mutable int id_lifetime3;
        mutable int id_lifetime4;
        mutable int id_amplitude2;
        mutable int id_amplitude3;
        mutable int id_amplitude4;

        double range_factor;

};

#endif // QFGENERALFITFUNCTIONBASE_H
