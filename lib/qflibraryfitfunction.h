/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFLibraryFitFunction_H
#define QFLibraryFitFunction_H

#include "qffitfunction.h"
#include <QString>
#include <QList>
#include <QStringList>
#include <cfloat>
#include <cmath>
#include "qfmathtools.h"
#include "lib_imexport.h"
#include <QLibrary>

class QFLibraryFitFunction_private;

class QFLIB_EXPORT QFLibraryFitFunction : public QFFitFunction
{
    public:
        explicit QFLibraryFitFunction(QLibrary* library);
        virtual ~QFLibraryFitFunction();

        /*! \brief help file for the fit fuction (HTML) */
        QString helpFile() const ;

        /*! \brief \c true, if the fit functions is good for use */
        bool isValid() const;

        /*! \brief return a string, describing the last error */
        QString lastError() const;

        /*! \brief return a string, describing the available features of this fit functions */
        QString features() const;

        /*! \copydoc QFFitFunction::category()  */
        virtual QString category() const;
        /*! \copydoc QFFitFunction::name()  */
        virtual QString name() const;
        /*! \copydoc QFFitFunction::shortName()  */
        virtual QString shortName() const ;
        /*! \copydoc QFFitFunction::id()  */
        virtual QString id() const;

        /*! \copydoc QFFitFunction::evaluate()  */
        virtual double evaluate(double x, const double* parameters) const ;

        /*! \copydoc QFFitFunction::multiEvaluate()  */
        virtual void multiEvaluate(double* y, const double* x, uint64_t N, const double* parameters) const;

        /*! \copydoc QFFitFunction::evaluateDerivatives()  */
        virtual void evaluateDerivatives(double* derivatives, double x, const double* parameters) const ;

        /*! \copydoc QFFitFunction::get_implementsDerivatives()  */
        virtual bool get_implementsDerivatives()const ;

        /*! \copydoc QFFitFunction::calcParameter()  */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const ;

        /*! \copydoc QFFitFunction::sortParameter()  */
        virtual void sortParameter(double* parameterValues, double* error=NULL, bool* fix=NULL) const ;

        /*! \copydoc QFFitFunction::isParameterVisible()  */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const ;

        /*! \copydoc QFFitFunction::getAdditionalPlotCount()  */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()  */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) ;

        /*! \copydoc QFFitFunction::estimateInitial()  */
        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL);


    private:
        QFLibraryFitFunction_private* d;

};

#endif // QFLibraryFitFunction_H
