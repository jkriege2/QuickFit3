#ifndef QFRDRTABLEPARSERFUNCTIONS_H
#define QFRDRTABLEPARSERFUNCTIONS_H

#include "jkmathparser.h"
#include <cmath>
#include "statistics_tools.h"
#include "qfmathtools.h"
#include "qftablepluginmodel.h"
#include "qftablemodel.h"

struct jkMathParserData {
    QFTablePluginModel* model;
    int row;
    int column;
};

void addQFRDRTableFunctions(jkMathParser* parser, QStringList *names=NULL);

jkMathParser::jkmpResult fQFRDRTableEditor_data(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);

jkMathParser::jkmpResult fQFRDRTableEditor_dataleft(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);

jkMathParser::jkmpResult fQFRDRTableEditor_colavg(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colsum(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colsum2(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colvar(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colstd(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colmedian(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colquantile(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colmin(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);
jkMathParser::jkmpResult fQFRDRTableEditor_colmax(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p);


#endif // QFRDRTABLEPARSERFUNCTIONS_H
