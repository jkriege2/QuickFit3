#ifndef QFRDRCURVEPARSERFUNCTIONS_H
#define QFRDRCURVEPARSERFUNCTIONS_H

#include "qfmathparser.h"
#include "qfrdrcurvesinterface.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"

QFMATHPARSER_DEFINE_RDRIS_FUNC(fRDR_iscurve, rdr_iscurve, QFRDRCurvesInterface)
void fCurve_getCount(qfmpResult &res,const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fCurve_getDataX(qfmpResult &res,const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fCurve_getDataXError(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
void fCurve_getDataY(qfmpResult &res,const qfmpResult* params, unsigned int  n, QFMathParser* p);
void fCurve_getDataYError(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);

#endif // QFRDRCURVEPARSERFUNCTIONS_H
