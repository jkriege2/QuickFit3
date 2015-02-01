/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-12-05 10:57:15 +0100 (Fr, 05 Dez 2014) $  (revision $Rev: 3659 $)

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

#ifndef QFFITFUNCTIONND_H
#define QFFITFUNCTIONND_H

#include <stdint.h>
#include <QString>
#include <QList>
#include <QStringList>
#include <cfloat>
#include <cmath>
#include "qfmathtools.h"
#include "lib_imexport.h"
#include "qffitfunctionbase.h"

/*! \brief describes a virtual base class for 2D fitting functions that are used together with QFFitAlgorithm objects.
    \ingroup qf3lib_fitting

    Mathematically this evaluates a 2D function \f$ f(x,y, \vec{p})\in\mathbb{R} \f$ at a position \f$ (x,y)\in\mathbb{R}^2 \f$
    and of a parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$ . This evaluation is implemented in evaluate().

    \see QFFitFunction for further explanations

*/
class QFLIB_EXPORT QFFitFunction2D : public QFFitFunctionBase
{
    public:
        virtual ~QFFitFunction2D() {}

        /*! \brief evaluate the fitting function \f$ f(x,y, \vec{p}) \f$ at the position \f$ x,y \f$ with the given parameter vector \f$ \vec{p} \f$
            \param x position \f$ x \f$ where to evaluate the fit function
            \param y position \f$ y \f$ where to evaluate the fit function
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \return the fitting function \f$ f(x,y, \vec{p}) \f$ at the position \f$ x,y \f$ with the given parameter vector \f$ \vec{p} \f$
         */
        virtual double evaluate(double x, double y, const double* parameters) const =0;
        inline virtual double evaluate(double x, double y, const QVector<double>& parameters) const {
            return evaluate(x, y,  parameters.constData());
        }
        inline virtual double evaluate(const double* x, const QVector<double>& parameters) const {
            return evaluate(x[0], x[1], parameters.constData());
        }
        inline virtual double evaluate(const double* x, const double* parameters) const {
            return evaluate(x[0], x[1], parameters);
        }
        inline virtual double evaluate(const QVector<double>& x, const QVector<double>& parameters) const {
            return evaluate(x[0], x[1], parameters.constData());
        }
};





/*! \brief describes a virtual base class for 3D fitting functions that are used together with QFFitAlgorithm objects.
    \ingroup qf3lib_fitting

    Mathematically this evaluates a 3D function \f$ f(x, y, z, \vec{p})\in\mathbb{R} \f$ at a position \f$ (x,y,z)\in\mathbb{R}^3 \f$
    and of a parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$ . This evaluation is implemented in evaluate().

    \see QFFitFunction for further explanations

*/
class QFLIB_EXPORT QFFitFunction3D : public QFFitFunctionBase
{
    public:
        virtual ~QFFitFunction3D() {}

        /*! \brief evaluate the fitting function \f$ f(x,y,z, \vec{p}) \f$ at the position \f$ x,y,z \f$ with the given parameter vector \f$ \vec{p} \f$
            \param x position \f$ x \f$ where to evaluate the fit function
            \param y position \f$ y \f$ where to evaluate the fit function
            \param z position \f$ z \f$ where to evaluate the fit function
            \param parameters parameter vector \f$ \vec{p}\in\mathbb{R}^N \f$
            \return the fitting function \f$ f(x,y,z, \vec{p}) \f$ at the position \f$ x,y,z \f$ with the given parameter vector \f$ \vec{p} \f$
         */
        virtual double evaluate(double x, double y, double z, const double* parameters) const =0;
        inline virtual double evaluate(double x, double y, double z, const QVector<double>& parameters) const {
            return evaluate(x, y, z, parameters.constData());
        }
        inline virtual double evaluate(const double* x, const QVector<double>& parameters) const {
            return evaluate(x[0], x[1], x[2], parameters.constData());
        }
        inline virtual double evaluate(const double* x, const double* parameters) const {
            return evaluate(x[0], x[1], x[2], parameters);
        }
        inline virtual double evaluate(const QVector<double>& x, const QVector<double>& parameters) const {
            return evaluate(x[0], x[1], x[2], parameters.constData());
        }
};

#endif // QFFITFUNCTIONND_H
