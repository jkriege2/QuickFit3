#ifndef QFEDEFAULTMATHPARSEREXTENSIONFUNCTIONS_H
#define QFEDEFAULTMATHPARSEREXTENSIONFUNCTIONS_H

#include "qfmathparser.h"

qfmpResult fRegression(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fWeightedRegression(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fIRLS(const qfmpResult* params, unsigned int  n, QFMathParser* p);


#endif // QFEDEFAULTMATHPARSEREXTENSIONFUNCTIONS_H
