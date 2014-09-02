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

#ifndef QFFitFunctionDLSG2inDIND_H
#define QFFitFunctionDLSG2inDIND_H
#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for a tri-exponential G2 autocorrelation function in DLS
    \ingroup qf3fitfunp_fitfunctions_dls

*/
class QFFitFunctionDLSG2inD: public QFFitFunction {
    public:
        QFFitFunctionDLSG2inD();
        virtual ~QFFitFunctionDLSG2inD() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QString("DLS: G2 autocorrelation (D)"); };
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return QObject::tr("DLS: G2 autocorrelation (D)"); };
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("dls_g2_ind"); };

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;

};

#endif // QFFitFunctionDLSG2inDIND_H
