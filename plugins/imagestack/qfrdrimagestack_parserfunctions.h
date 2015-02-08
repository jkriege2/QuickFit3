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
