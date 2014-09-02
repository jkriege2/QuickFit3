/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFRDRTABLEPARSERFUNCTIONS_H
#define QFRDRTABLEPARSERFUNCTIONS_H

#include "qfmathparser.h"
#include <cmath>
#include "statistics_tools.h"
#include "qfmathtools.h"
#include "qftablepluginmodel.h"
#include "qftablemodel.h"

struct QFMathParserData {
    QFTablePluginModel* model;
    int row;
    int column;
};

void addQFRDRTableFunctions(QFMathParser* parser, QStringList *names=NULL, bool columnMode=false);

void fQFRDRTableEditor_data(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_column(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_columnStr(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_columnBool(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);

void fQFRDRTableEditor_row(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_rowStr(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_rowBool(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_coltitles(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_coltitle(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_colindexbytitle(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);

void fQFRDRTableEditor_columnUnique(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColAvg(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColSum(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColSum2(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColVar(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColStd(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColMin(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColMax(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColMedian(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);
void fQFRDRTableEditor_indexedColQuantile(qfmpResult& res, const qfmpResult* params, unsigned int n, QFMathParser* p);

void fQFRDRTableEditor_dataleft(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);

void fQFRDRTableEditor_colavg(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsum(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsum2(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colvar(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colstd(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colmedian(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colquantile(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colmin(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colmax(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);

void fQFRDRTableEditor_colimagewidth(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colimageheight(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_collength(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);




void fQFRDRTableEditor_colsavg(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colssum(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colscount(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colssum2(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsvar(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsstd(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsmedian(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsquantile(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsmin(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fQFRDRTableEditor_colsmax(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);

#endif // QFRDRTABLEPARSERFUNCTIONS_H
