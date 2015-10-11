/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFFITFUNCTIONGENERALSEGMENTEDLINE_H
#define QFFITFUNCTIONGENERALSEGMENTEDLINE_H

#include "qfpluginfitfunction.h"




/*! \brief QFFitFunction class for fit function
    \ingroup qf3fitfunp_fitfunction_general

*/
class QFFitFunctionGeneralSegmentedLine: public QFFitFunction {
    public:
        QFFitFunctionGeneralSegmentedLine();
        virtual ~QFFitFunctionGeneralSegmentedLine() {}
        /*! \copydoc QFFitFunction::name()   */
        virtual QString name() const { return QObject::tr("General: Segmented Line"); }
        /** \copydoc QFFitFunction::shortName() */
        virtual QString shortName() const { return name(); }
        /*! \copydoc QFFitFunction::id()   */
        virtual QString id() const { return QString("gen_segline"); }
        /*! \copydoc QFFitFunction::category()   */
        virtual QString category() const { return QObject::tr("General Fit Functions"); }

        /*! \copydoc QFFitFunction::evaluate()   */
        virtual double evaluate(double t, const double* parameters) const;

        /*! \copydoc QFFitFunction::calcParameter()   */
        virtual void calcParameter(double* parameterValues, double* error=NULL) const;

        /*! \copydoc QFFitFunction::isParameterVisible()   */
        virtual bool isParameterVisible(int parameter, const double* parameterValues) const;
        /*! \copydoc QFFitFunction::getAdditionalPlotCount()   */
        virtual unsigned int getAdditionalPlotCount(const double* params) const;

        /*! \copydoc QFFitFunction::transformParametersForAdditionalPlot()   */
        virtual QString transformParametersForAdditionalPlot(int plot, double* params) const;

        /*! \copydoc QFFitFunction::get_implementsDerivatives()   */
        virtual bool get_implementsDerivatives() const;



};

#endif // QFFITFUNCTIONGENERALSEGMENTEDLINE_H
