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

void addQFRDRTableFunctions(QFMathParser* parser, QStringList *names=NULL);

QFMathParser::qfmpResult fQFRDRTableEditor_data(QFMathParser::qfmpResult* params, unsigned int n, QFMathParser* p);

QFMathParser::qfmpResult fQFRDRTableEditor_dataleft(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);

QFMathParser::qfmpResult fQFRDRTableEditor_colavg(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colsum(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colsum2(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colvar(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colstd(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colmedian(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colquantile(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colmin(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colmax(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);


#endif // QFRDRTABLEPARSERFUNCTIONS_H
