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

QFMathParser::qfmpResult fQFRDRTableEditor_data(const QFMathParser::qfmpResult* params, unsigned int n, QFMathParser* p);

QFMathParser::qfmpResult fQFRDRTableEditor_dataleft(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);

QFMathParser::qfmpResult fQFRDRTableEditor_colavg(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colsum(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colsum2(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colvar(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colstd(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colmedian(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colquantile(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colmin(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);
QFMathParser::qfmpResult fQFRDRTableEditor_colmax(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p);


#endif // QFRDRTABLEPARSERFUNCTIONS_H
