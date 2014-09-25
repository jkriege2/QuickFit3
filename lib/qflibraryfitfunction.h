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

        QString helpFile() const ;

        bool isValid() const;

        QString lastError() const;

        QString features() const;

        /*! \copydoc QFFitFunction::()  */
        virtual QString name() const;
        /*! \copydoc QFFitFunction::()  */
        virtual QString shortName() const ;
        /*! \copydoc QFFitFunction::()  */
        virtual QString id() const;

        /*! \copydoc QFFitFunction::()  */
        virtual double evaluate(double x, const double* parameters) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual void multiEvaluate(double* y, const double* x, uint64_t N, const double* parameters) const;

        /*! \copydoc QFFitFunction::()  */
        virtual void evaluateDerivatives(double* derivatives, double x, const double* parameters) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual bool get_implementsDerivatives()const ;

        /*! \copydoc QFFitFunction::()  */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual void sortParameter(double* parameterValues, double* error=NULL, bool* fix=NULL) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const ;

        /*! \copydoc QFFitFunction::()  */
        virtual unsigned int getAdditionalPlotCount(const double* params);

        /*! \copydoc QFFitFunction::()  */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) ;

        /*! \copydoc QFFitFunction::()  */
        virtual bool estimateInitial(double* params, const double* dataX, const double* dataY, long N, const bool *fix=NULL);


    private:
        QFLibraryFitFunction_private* d;

};

#endif // QFLibraryFitFunction_H
