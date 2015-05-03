/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-11-25 17:03:47 +0100 (Di, 25 Nov 2014) $  (revision $Rev: 3644 $)

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

#include "qffitfunctionbase.h"



QString QFFitFunctionBase::shortName() const
{
    return name();
}

QString QFFitFunctionBase::category() const
{
    return QString();
}

bool QFFitFunctionBase::isDeprecated() const
{
    return false;
}

bool QFFitFunctionBase::get_implementsDerivatives() const
{
    return false;
}

void QFFitFunctionBase::calcParameter(double */*parameterValues*/, double */*error*/) const
{

}

void QFFitFunctionBase::sortParameter(double */*parameterValues*/, double */*error*/, bool */*fix*/) const
{

}

bool QFFitFunctionBase::isParameterVisible(int /*parameter*/, const double */*parameterValues*/) const
{
    return true;
}

unsigned int QFFitFunctionBase::getAdditionalPlotCount(const double */*params*/)
{
    return 0;
}

QString QFFitFunctionBase::transformParametersForAdditionalPlot(int /*plot*/, double */*params*/)
{
    return QString();
}

double QFFitFunctionBase::customTransform(int /*parameter*/, double value) const
{
    return value;
}

double QFFitFunctionBase::customBackTransform(int /*parameter*/, double value) const
{
    return value;
}

QVector<double> QFFitFunctionBase::getInitialParamValues() const
{
    QVector<double> p;
    for (int i=0; i<paramCount(); i++) {
        p<<getDescription(i).initialValue;
    }
    return p;
}

QVector<double> QFFitFunctionBase::getInitialParamMins() const
{
    QVector<double> p;
    for (int i=0; i<paramCount(); i++) {
        p<<getDescription(i).minValue;
    }
    return p;
}

QVector<double> QFFitFunctionBase::getInitialParamMaxs() const
{
    QVector<double> p;
    for (int i=0; i<paramCount(); i++) {
        p<<getDescription(i).maxValue;
    }
    return p;
}

QVector<double> QFFitFunctionBase::getParamAbsMins() const
{
    QVector<double> p;
    for (int i=0; i<paramCount(); i++) {
        p<<getDescription(i).absMinValue;
    }
    return p;
}

QVector<double> QFFitFunctionBase::getParamAbsMaxs() const
{
    QVector<double> p;
    for (int i=0; i<paramCount(); i++) {
        p<<getDescription(i).absMaxValue;
    }
    return p;
}

QVector<bool> QFFitFunctionBase::getInitialParamFixes() const
{
    QVector<bool> p;
    for (int i=0; i<paramCount(); i++) {
        p<<getDescription(i).initialFix;
    }
    return p;
}
