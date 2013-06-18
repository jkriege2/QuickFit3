#ifndef QFEDEFAULTMATHPARSEREXTENSIONFUNCTIONS_H
#define QFEDEFAULTMATHPARSEREXTENSIONFUNCTIONS_H

#include "qfmathparser.h"

qfmpResult fRegression(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fWeightedRegression(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fIRLS(const qfmpResult* params, unsigned int  n, QFMathParser* p);



qfmpResult fErrorAdd(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fErrorSub(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fErrorMul(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fErrorDiv(const qfmpResult* params, unsigned int  n, QFMathParser* p);
qfmpResult fErrorPow(const qfmpResult* params, unsigned int  n, QFMathParser* p);

#endif // QFEDEFAULTMATHPARSEREXTENSIONFUNCTIONS_H
