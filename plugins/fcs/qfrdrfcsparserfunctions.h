/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
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

#ifndef QFRDRFCSPARSERFUNCTIONS_H
#define QFRDRFCSPARSERFUNCTIONS_H

#include "qfmathparser.h"

void fRDR_isfcs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fFCS_correlation(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fFCS_correlationerror(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fFCS_meancorrelation(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fFCS_meancorrelationerror(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fFCS_tau(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fFCS_runs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);

void fRDR_issimplecountrate(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fSCR_channels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fSCR_runs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fSCR_mean(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fSCR_std(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);

void fRDR_iscountrate(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCR_rate(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCR_time(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCR_runs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCR_channels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCR_mean(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
void fCR_std(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);

#endif // QFRDRFCSPARSERFUNCTIONS_H
