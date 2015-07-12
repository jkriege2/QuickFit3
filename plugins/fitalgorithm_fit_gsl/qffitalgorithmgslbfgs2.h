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

#ifndef QFFITALGORITHMGSLBFGS2_H
#define QFFITALGORITHMGSLBFGS2_H

#include "qffitalgorithmgslderivbase.h"

class QFFitAlgorithmGSLBFGS2 : public QFFitAlgorithmGSLDerivativeBase
{
    public:
        QFFitAlgorithmGSLBFGS2(): QFFitAlgorithmGSLDerivativeBase(gsl_multimin_fdfminimizer_vector_bfgs2) { }
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Broyden-Fletcher-Goldfarb-Shanno (BFGS) v2"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: BFGS Minimizer"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_bfgs2"); }
};

#endif // QFFITALGORITHMGSLBFGS2_H
