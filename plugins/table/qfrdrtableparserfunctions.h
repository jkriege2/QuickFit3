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

qfmpResult fQFRDRTableEditor_data(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_column(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_columnStr(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_columnBool(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_columnUnique(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColAvg(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColSum(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColSum2(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColVar(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColStd(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColMin(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColMax(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColMedian(const qfmpResult* params, unsigned int n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_indexedColQuantile(const qfmpResult* params, unsigned int n, QFMathParser* p);

qfmpResult fQFRDRTableEditor_dataleft(const qfmpResult* params, unsigned int  n, QFMathParser* p);

qfmpResult fQFRDRTableEditor_colavg(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colsum(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colsum2(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colvar(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colstd(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colmedian(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colquantile(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colmin(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colmax(const qfmpResult* params, unsigned int  n, QFMathParser* p);

qfmpResult fQFRDRTableEditor_colimagewidth(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_colimageheight(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fQFRDRTableEditor_collength(const qfmpResult* params, unsigned int  n, QFMathParser* p);


#endif // QFRDRTABLEPARSERFUNCTIONS_H
