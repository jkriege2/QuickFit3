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

#ifndef QFRDRIMAGESTACK_PARSERFUNCTIONS_H
#define QFRDRIMAGESTACK_PARSERFUNCTIONS_H
#include "qfmathparser.h"

void fStack_isstack(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fStack_getstacks(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fStack_getwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fStack_getheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fStack_getframes(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fStack_getchannels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fStack_getframe(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fStack_getstack(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
#endif // QFRDRIMAGESTACK_PARSERFUNCTIONS_H
