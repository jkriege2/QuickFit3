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

#ifndef QFFitAlgorithmLMFitIRLSIRLS_H
#define QFFitAlgorithmLMFitIRLSIRLS_H
#include "qffitalgorithm.h"
#include "qffitalgorithmlmfit.h"
#include "qffitalgorithmlmfitbox.h"

/*! \brief QQFFitAlgorithm implementation
    \ingroup qf3fitfunp_fit_lmfit


*/
class QFFitAlgorithmLMFitIRLS: public QFFitAlgorithmLMFitBox {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmLMFitIRLS();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmLMFitIRLS() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("irls_lmfit: Iteratively reweightes Levenberg-Marquard fit (outlier-robust fit, with box-constraints)"); };
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("irls_lmfit: IRLS Levenberg-Marquard fit"); };
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_irls_lmfit"); };
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();

};


#endif // QFFitAlgorithmLMFitIRLSIRLS_H
