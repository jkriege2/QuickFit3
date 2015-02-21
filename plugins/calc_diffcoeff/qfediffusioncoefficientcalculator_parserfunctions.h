
/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-12 18:00:48 +0200 (Fr, 12 Sep 2014) $  (revision $Rev: 3461 $)

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

#ifndef QFEDIFFUSIONCOEFFICIENTCALCULATOR_PARSERFUNCTIONS_H
#define QFEDIFFUSIONCOEFFICIENTCALCULATOR_PARSERFUNCTIONS_H


#include "qfmathparser.h"

void fCalcD_getViscosityWater(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCalcD_getDSphere(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCalcD_getDRotSphere(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCalcD_getTauDRotSphere(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);


#endif // QFEDIFFUSIONCOEFFICIENTCALCULATOR_PARSERFUNCTIONS_H
