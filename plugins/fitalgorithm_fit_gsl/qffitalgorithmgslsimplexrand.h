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

#ifndef QFFitAlgorithmGSLSimplexRand_H
#define QFFitAlgorithmGSLSimplexRand_H
#include "qffitalgorithm.h"
#include "qffitalgorithmgslsimplex.h"

/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_gsl


*/
class QFFitAlgorithmGSLSimplexRand: public QFFitAlgorithmGSLSimplex {

    public:
        /** \brief class constructor */
        QFFitAlgorithmGSLSimplexRand();
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("GSL: Randomized Simplex Minimizer (derivative free)"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("GSL: Randomized Simplex Minimizer"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_gsl_simplex_rand"); }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
};


#endif // QFFitAlgorithmGSLSimplexRand_H
