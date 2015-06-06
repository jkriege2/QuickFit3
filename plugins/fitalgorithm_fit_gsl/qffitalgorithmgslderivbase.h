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

#ifndef QFFitAlgorithmGSLDerivativeBase_H
#define QFFitAlgorithmGSLDerivativeBase_H
#include "qffitalgorithm.h"
#include "gsl/gsl_multimin.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_gsl


*/
class QFFitAlgorithmGSLDerivativeBase: public QFFitAlgorithm {
    protected:

        const gsl_multimin_fdfminimizer_type *T;

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);

    protected:
        /** \copydoc QFFitAlgorithm::intMinimize() */
        virtual FitResult intMinimize(double* paramsOut, const double* initialParams, FunctorForFMin* model, const double* paramsMin, const double* paramsMax);
    public:
        /** \copydoc QFFitAlgorithm::get_implementsMinimize() */
        inline virtual bool get_implementsMinimize() { return true; }

    public:
        /** \brief class constructor */
        QFFitAlgorithmGSLDerivativeBase(const gsl_multimin_fdfminimizer_type *T=NULL);
        /** \brief class destructor */
        virtual ~QFFitAlgorithmGSLDerivativeBase() {}
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool isThreadSafe() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFitAlgorithmGSLDerivativeBase_H
