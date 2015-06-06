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

#ifndef QFFITALGORITHMGSLSIMPLEX_H
#define QFFITALGORITHMGSLSIMPLEX_H
#include "qffitalgorithm.h"
#include "gsl/gsl_multimin.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_gsl


*/
class QFFitAlgorithmGSLSimplex: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);

        const gsl_multimin_fminimizer_type *T;

    protected:
        /** \copydoc QFFitAlgorithm::intMinimize() */
        virtual FitResult intMinimize(double* paramsOut, const double* initialParams, FunctorForFMin* model, const double* paramsMin, const double* paramsMax);
    public:
        /** \copydoc QFFitAlgorithm::get_implementsMinimize() */
        inline virtual bool get_implementsMinimize() { return true; }
    public:
        /** \brief class constructor */
        QFFitAlgorithmGSLSimplex();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmGSLSimplex() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Simplex Minimizer (derivative free)"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Simplex Minimizer"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_simplex"); }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool isThreadSafe() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFITALGORITHMGSLSIMPLEX_H
