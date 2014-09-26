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

#ifndef QFFITALGORITHMGSLSTEEPEST_DESCENT_H
#define QFFITALGORITHMGSLSTEEPEST_DESCENT_H

#include "qffitalgorithmgslderivbase.h"

class QFFitAlgorithmGSLSteepestDescent : public QFFitAlgorithmGSLDerivativeBase
{
    public:
        QFFitAlgorithmGSLSteepestDescent(): QFFitAlgorithmGSLDerivativeBase(gsl_multimin_fdfminimizer_steepest_descent) { setParameter("max_iterations", 2000); }
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Steepest Descent"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Steepest Descent"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_steepest_descent"); }
};
#endif // QFFITALGORITHMGSLSTEEPEST_DESCENT_H
