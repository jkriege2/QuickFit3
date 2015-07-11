/*
  Name: sdfffunctions.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sdfffunctions.h
 * \ingroup hlseqgen_sdff_f
 *
 */

#ifndef SDFFFUNCTIONS_H
#define SDFFFUNCTIONS_H

#include <string>
#include <cmath>
#include <cstdio>
#include <stdint.h>




#include "tools.h"
#include "sequencererror.h"
#include "sdfftools.h"
#include "sdffparser.h"
#include "sdffnodes.h"
#include "sdffhwcommands.h"

#include "./mersenne/MersenneTwister.h"



namespace sequencer {
  namespace sdff_functions {
    /** \brief \c <tt>if(test_expression, true_expression, false_expresion)</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     *
     * \f[ \mbox{if}(t, a, b)=\left\{\begin{array}{ll}a & t=\mbox{true}\\b & \mbox{otherwise}\end{array}\right. \f]
     */
    sdff_result IF(SDFFparser* parser, std::string fname, SDFFnode** parameter, unsigned char paramcount);
    /** \brief check of \c if function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool IF_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c <tt>sum(i, start, stop, expression)</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     *
     * \f[ \mbox{sum}(i, i_0, i_1, \mbox{expr})=\sum_{i=i_0}^{i_1}\mbox{expr} \f]
     */
    sdff_result SUM(SDFFparser* parser, std::string fname, SDFFnode** parameter, unsigned char paramcount);
    /** \brief check of \c sum function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool SUM_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c <tt>prod(i, start, stop, expression)</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     *
     * \f[ \mbox{prod}(i, i_0, i_1, \mbox{expr})=\prod_{i=i_0}^{i_1}\mbox{expr} \f]
     */
    sdff_result PROD(SDFFparser* parser, std::string fname, SDFFnode** parameter, unsigned char paramcount);
    /** \brief check of \c prod function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool PROD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);







    /** \brief \c <tt>set("channel"|time, value[, delay=0])</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     *
     * This function sets the \c value of a channel (either given by the first parameter, or by a <tt>channel(..){...}</tt>
     * group. It may be delayed for \a delay microseconds.
     */
    sdff_result SET(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c set function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     */
    bool SET_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c <tt>ramp_lin("channel"|time, end_value, duration[, delay=0])</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     *
     * This function adds a linear ramp to an analog channel
     */
    sdff_result RAMPLIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c ramp_lin function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     */
    bool RAMPLIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c <tt>ramp_cos("channel"|time, end_value, duration[, delay=0])</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     *
     * This function adds a cosine ramp to an analog channel
     */
    sdff_result RAMPCOS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c ramp_cos function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     */
    bool RAMPCOS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c <tt>ramp_exp("channel"|time, end_value, duration, tau[, delay=0])</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     *
     * This function adds an exponential ramp to an analog channel
     */
    sdff_result RAMPEXP(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c ramp_exp function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     */
    bool RAMPEXP_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c <tt>ramp_sig("channel"|time, end_value, duration, tau[, delay=0])</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     *
     * This function adds a sigmoidal ramp to an analog channel
     */
    sdff_result RAMPSIG(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c ramp_sig function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     */
    bool RAMPSIG_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c <tt>pulse("channel"|time, value, duration[, delay=0])</tt> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     *
     * This function adds a ramp to an analog channel
     */
    sdff_result PULSE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c pulse function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sgf
     */
    bool PULSE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);










    /** \brief \c add_info_marker function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    sdff_result ADDINFOMARKER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c add_info_marker function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    bool ADDINFOMARKER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c add_warning_marker function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    sdff_result ADDWARNINGMARKER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c add_warning_marker function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    bool ADDWARNINGMARKER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c add_error_marker function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    sdff_result ADDERRORMARKER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c add_error_marker function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    bool ADDERRORMARKER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c write function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    sdff_result WRITE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c write function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    bool WRITE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c writeln function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    sdff_result WRITELN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c writeln function for SDFFparser
     * \ingroup hlseqgen_sdff_f_lf
     */
    bool WRITELN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c delta function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result DELTA(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c delta function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool DELTA_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c theta function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result THETA(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c theta function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool THETA_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c sign function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result SIGN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c sign function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool SIGN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c limit function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result LIMIT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c limit function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool LIMIT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c min function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result MIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c min function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool MIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c max function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result MAX(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c max function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool MAX_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c sin function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result SIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c sin function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool SIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c cos function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result COS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c cos function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool COS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c tan function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result TAN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c tan function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool TAN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c exp function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result EXP(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c exp(x-1) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool EXP_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c exp function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result EXPM1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c exp(x)-1 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool EXPM1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c acos function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ACOS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c acos function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ACOS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c asin function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ASIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c asin function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ASIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c atan function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ATAN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c atan function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ATAN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c ln=log<sub>e</sub> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result LOG(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c ln=log<sub>e</sub> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool LOG_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c log<sub>e</sub>(x+1) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result LOGP1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c log<sub>e</sub>(x+1) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool LOGP1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c lb=log<sub>2</sub> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result LOG2(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c lb=log<sub>2</sub> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool LOG2_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c log10=log<sub>10</sub> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result LOG10(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c log10=log<sub>10</sub> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool LOG10_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c sqrt(x)=\f$ \sqrt{x} \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result SQRT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c sqrt(x)=\f$ \sqrt{x} \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool SQRT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c sqrt(x)=\f$ \sqrt[3]{x} \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result CBRT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c sqrt(x)=\f$ \sqrt[3]{x} \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool CBRT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);


    /** \brief \c acosh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ACOSH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c acosh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ACOSH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c asinh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ASINH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c asinh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ASINH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c atanh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ATANH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c atanh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ATANH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);
    /** \brief \c sinh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result SINH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c sinh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool SINH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c cosh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result COSH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c cosh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool COSH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c tanh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result TANH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c tanh function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool TANH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);


    /** \brief \c erf(x)=\f$\frac{2}{\sqrt{\pi}}\cdot\int_0^xe^{-t^2}\;\mbox{d}t \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ERF(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c erf(x)=\f$\frac{2}{\sqrt{\pi}}\cdot\int_0^xe^{-t^2}\;\mbox{d}t \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ERF_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c erfc(x)=1-erf(x)=1-\f$\frac{2}{\sqrt{\pi}}\cdot\int_0^xe^{-t^2}\;\mbox{d}t \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ERFC(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c erfc(x)=1-erf(x)=1-\f$\frac{2}{\sqrt{\pi}}\cdot\int_0^xe^{-t^2}\;\mbox{d}t \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ERFC_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c tgamma(x)=\f$\int_0^\infty t^{x-1}e^{-t}\;\mbox{d}t \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result TGAMMA(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c tgamma(x)=\f$\int_0^\infty t^{x-1}e^{-t}\;\mbox{d}t \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool TGAMMA_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c j0 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result J0(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c j0 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool J0_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c j1 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result J1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c j1 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool J1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c jn function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result JN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c jn function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool JN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c y0 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result Y0(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c y0 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool Y0_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c y1 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result Y1(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c y1 function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool Y1_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c yn function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result YN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c yn function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool YN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c abs(x)=\f$ |x| \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ABS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c abs(x)=\f$ |x| \f$ function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ABS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c round(x) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result ROUND(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c round function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool ROUND_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c ceil(x) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result CEIL(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c ceil function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool CEIL_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c floor(x) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result FLOOR(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c floor function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool FLOOR_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c trunc(x) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result TRUNC(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c trunc function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool TRUNC_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c tostring function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result TOSTRING(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c tostring function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool TOSTRING_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c str2num function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result STR2NUM(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c str2num function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool STR2NUM_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c str2bool function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result STR2BOOL(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c str2bool function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool STR2BOOL_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c length function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result LENGTH(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c length function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool LENGTH_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c strget function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result STRGET(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c strget function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool STRGET_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c tolower function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result TOLOWER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c tolower function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool TOLOWER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c toupper function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result TOUPPER(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c toupper function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool TOUPPER_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c strip function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result STRIP(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c strip function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool STRIP_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c factorial(x)=x! function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    sdff_result FACTORIAL(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c factorial function for SDFFparser
     * \ingroup hlseqgen_sdff_f_mf
     */
    bool FACTORIAL_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c random function for SDFFparser which returns a random number between [0..1)
     * \ingroup hlseqgen_sdff_f_rngf
     */
    sdff_result RAND(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c random function for SDFFparser which returns a random number between [0..1)
     * \ingroup hlseqgen_sdff_f_rngf
     */
    bool RAND_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c random_seed function for SDFFparser which seed the random number generator
     * \ingroup hlseqgen_sdff_f_rngf
     */
    sdff_result RANDSEED(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c random_seed function for SDFFparser which seed the random number generator
     * \ingroup hlseqgen_sdff_f_rngf
     */
    bool RANDSEED_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief \c random_int(max) function for SDFFparser which returns a random integer number [0..max]
     * \ingroup hlseqgen_sdff_f_rngf
     */
    sdff_result RANDINT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of \c random_int(max) function for SDFFparser
     * \ingroup hlseqgen_sdff_f_rngf
     */
    bool RANDINT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>random_norm(mean=0, variance=1)</code> function for SDFFparser which returns a gaussian distributed
     *         random number
     * \ingroup hlseqgen_sdff_f_rngf
     */
    sdff_result RANDGAUSS(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>random_norm(mean=0, variance=1)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_rngf
     */
    bool RANDGAUSS_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>tohex(integer)</code> function for SDFFparser which converts an integer into a hex string (255 = "FF")
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result TOHEX(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>tohex(integer)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool TOHEX_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>tobin(integer)</code> function for SDFFparser which converts an integer into a hex string (255 = "11111111")
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result TOBIN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>tobin(integer)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool TOBIN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>char(0..255)</code> function for SDFFparser which returns the param's ASCII character
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result CHAR(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>char(0..255)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool CHAR_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>isdouble(val), isinteger(val), isboolean(val), isstring(val)</code> function for SDFFparser which returns true if the value is of the specified type
     * \ingroup hlseqgen_sdff_f_sf
     */
    sdff_result ISOFTYPE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>isdouble(val), isinteger(val), isboolean(val), isstring(val)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_sf
     */
    bool ISOFTYPE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>fopen(filename[, mode])</code> function for SDFFparser which opens a file with the specified mode (default is \c "w" )
     * \ingroup hlseqgen_sdff_f_ff
     *
     * This function returns the C FILE* pointer as integer. Other file functions use this as a file handle (first argument).
     */
    sdff_result FOPEN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>fopen(filename[, mode])</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_ff
     */
    bool FOPEN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>fclose(handle_int)</code> function for SDFFparser which closes a file (specified by the handle)
     * \ingroup hlseqgen_sdff_f_ff
     */
    sdff_result FCLOSE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>fclose(handle_int)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_ff
     */
    bool FCLOSE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>fwrite(handle_int, data[, data ...])</code> function for SDFFparser which write to a file (specified by the handle)
     * \ingroup hlseqgen_sdff_f_ff
     */
    sdff_result FWRITE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>fwrite(handle_int, data[, data ...])</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_ff
     */
    bool FWRITE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>fwrite(handle_int, data[, data ...])</code> function for SDFFparser which write to a file (specified by the handle) and adds a linebreak
     * \ingroup hlseqgen_sdff_f_ff
     */
    sdff_result FWRITELN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>fwrite(handle_int, data[, data ...])</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_ff
     */
    bool FWRITELN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>serial_open(port, ...)</code> function for SDFFparser which opens a serial interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SEROPEN(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>serial_open(port, ...)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SEROPEN_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>serial_close(port)</code> function for SDFFparser which closes a serial interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERCLOSE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>serial_close(port)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERCLOSE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>serial_write(port, data[, data ..])</code> and <code>serial_writeln(port, data[, data ..])</code> function for SDFFparser which writes to a serial interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERWRITE(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>serial_write(port, data[, data ..])</code> and <code>serial_writeln(port, data[, data ..])</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERWRITE_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>serial_read(port, byte_count|end_char)</code> function for SDFFparser which reads from a serial interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERREAD(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>serial_read(port, byte_count|end_char)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERREAD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);



    /** \brief <code>sergpib_send(device, data[, data ..])</code> function for SDFFparser which writes to a serial-to-GPIB interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERGPIBSEND(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>sergpib_send(device, data[, data ..])</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERGPIBSEND_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>sergpib_send_DCL(), sergpib_send_SDC(), sergpib_send_GET(), sergpib_send_GTL(), sergpib_send_LLO(),
     *         sergpib_send_PPC(), sergpib_send_PPU(), sergpib_send_UNT(), sergpib_send_UNL(), sergpib_send_SPD()
     *         sergpib_send_SPE(), sergpib_send_TCT(), sergpib_send_IFC()</code> function for SDFFparser which writes to
     *         a serial-to-GPIB interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERGPIBSENDCMD(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>sergpib_send_DCL(), sergpib_send_SDC(), sergpib_send_GET(), sergpib_send_GTL(), sergpib_send_LLO(),
     *         sergpib_send_PPC(), sergpib_send_PPU(), sergpib_send_UNT(), sergpib_send_UNL(), sergpib_send_SPD()
     *         sergpib_send_SPE(), sergpib_send_TCT(), sergpib_send_IFC()</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERGPIBSENDCMD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>sergpib_query(device, data[, data ..])</code> function for SDFFparser which writes to a serial-to-GPIB interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERGPIBQUERY(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>sergpib_query(device, data[, data ..])</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERGPIBQUERY_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>sergpib_read(device)</code> function for SDFFparser which reads from a serial-to-GPIB interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERGPIBREAD(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>sergpib_read(device)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERGPIBREAD_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>sergpib_config(interface_address, use_EOI, REN_talkstate, REN_readstate, send_LB, query_delay)</code> function for SDFFparser which reads from a serial-to-GPIB interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERGPIBCONFIG(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>sergpib_config(interface_address, use_EOI, REN_talkstate, REN_readstate, send_LB, query_delay)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERGPIBCONFIG_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);


    /** \brief <code>sergpib_configint(interrupt, device, send_GTL, send_UNL, command)</code> function for SDFFparser which reads from a serial-to-GPIB interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERGPIBCONFIGINT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>sergpib_configint(interrupt, device, send_GTL, send_UNL, command)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERGPIBCONFIGINT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);

    /** \brief <code>sergpib_enable_interrupt(interrupt), sergpib_disable_interrupt(interrupt)</code> function for SDFFparser which reads from a serial-to-GPIB interface
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    sdff_result SERGPIBENABLEINT(SDFFparser* parser, std::string fname, sdff_result* parameter, unsigned char paramcount);
    /** \brief check of <code>sergpib_enable_interrupt(interrupt), sergpib_disable_interrupt(interrupt)</code> function for SDFFparser
     * \ingroup hlseqgen_sdff_f_hwcf
     */
    bool SERGPIBENABLEINT_check(SDFFparser* parser, std::string fname, sdff_paramtype* parameter, unsigned char paramcount, bool returnused);





    /** \todo implement functions: <tt>gauss, copy, fmod,
     *        strfind, setbit, getbit,
     *        </tt>
     */
  }
}
#endif // SDFFFUNCTIONS_H


/*@}*/
