#ifndef QFEGSLMathParserExtensionsFUNCTIONS_H
#define QFEGSLMathParserExtensionsFUNCTIONS_H

#include "qfmathparser.h"
#include "gsl/gsl_sf_coulomb.h"
#include "gsl/gsl_sf_debye.h"
#include "gsl/gsl_sf_dilog.h"
#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_sf_laguerre.h"
#include "gsl/gsl_sf_trig.h"

QFMATHPARSER_DEFINE_2PARAMLVEC_NUMERIC_FUNC(fmp_gsl_sf_hydrogenicR_1, "hydrogenicR1", gsl_sf_hydrogenicR_1)
QFMATHPARSER_DEFINE_4PARAMLVEC_NUMERIC_FUNC(fmp_gsl_sf_hydrogenicR, "hydrogenicR", gsl_sf_hydrogenicR)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_debye_1, "debye1", gsl_sf_debye_1)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_debye_2, "debye2", gsl_sf_debye_2)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_debye_3, "debye3", gsl_sf_debye_3)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_debye_4, "debye4", gsl_sf_debye_4)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_debye_5, "debye5", gsl_sf_debye_5)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_debye_6, "debye6", gsl_sf_debye_6)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_dilog, "dilog", gsl_sf_dilog)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_fact, "fact", gsl_sf_fact)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_doublefact, "doublefact", gsl_sf_doublefact)
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_lnfact, "lnfact", gsl_sf_lnfact )
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_lndoublefact, "lndoublefact", gsl_sf_lndoublefact )
QFMATHPARSER_DEFINE_2PARAMLVEC_NUMERIC_FUNC(fmp_gsl_sf_taylorcoeff, "taylorcoeff", gsl_sf_taylorcoeff  )
QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(fmp_gsl_sf_choose, "choose", gsl_sf_choose  )
QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(fmp_gsl_sf_lnchoose, "lnchoose", gsl_sf_lnchoose  )
QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(fmp_gsl_sf_beta, "beta", gsl_sf_beta  )
QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC(fmp_gsl_sf_lnbeta, "lnbeta", gsl_sf_lnbeta  )
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_lngamma , "lngamma", gsl_sf_lngamma )
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_gamma , "gamma", gsl_sf_gamma )

QFMATHPARSER_DEFINE_2PARAMLVEC_NUMERIC_FUNC(fmp_gsl_sf_laguerre_1, "laguerre1", gsl_sf_laguerre_1  )
QFMATHPARSER_DEFINE_2PARAMLVEC_NUMERIC_FUNC(fmp_gsl_sf_laguerre_2, "laguerre2", gsl_sf_laguerre_2  )
QFMATHPARSER_DEFINE_2PARAMLVEC_NUMERIC_FUNC(fmp_gsl_sf_laguerre_3, "laguerre3", gsl_sf_laguerre_3  )
QFMATHPARSER_DEFINE_3PARAMLVEC_NUMERIC_FUNC(fmp_gsl_sf_laguerre_n, "laguerre", gsl_sf_laguerre_n  )

QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_angle_restrict_symm , "angle_restrict_symm", gsl_sf_angle_restrict_symm  )
QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fmp_gsl_sf_angle_restrict_pos , "angle_restrict_pos", gsl_sf_angle_restrict_pos  )


#endif // QFEGSLMathParserExtensionsFUNCTIONS_H
