/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFMATHPARSERDEFAULTLIB_H
#define QFMATHPARSERDEFAULTLIB_H

#include "qfmathparsertools.h"


/******************************************************************************************
 * default-functions for farser
 ******************************************************************************************/


namespace QFMathParser_DefaultLib {

    QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(fdoubleToQString, num2str, doubleToQString)
    QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(fIntToStr, int2str, intToQString)
    QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(fIntToBinStr, int2bin, intToBinaryQString)
    QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(fIntToHexStr, int2hex, intToHexQString)
    QFMATHPARSER_DEFINE_1PARAM_NUM2STRING_FUNC(fIntToOctStr, int2oct, intToOctQString)
    QFMATHPARSER_DEFINE_1PARAM_BOOL2STRING_FUNC(fboolToQString, bool2str, boolToQString)

    QFMATHPARSER_DEFINE_1PARAM_STRING2NUM_FUNC(fStringToNum, str2num, QStringToDouble)
    QFMATHPARSER_DEFINE_1PARAM_STRING2NUM_FUNC(fCStringToNum, cstr2num, CQStringToDouble)
    QFMATHPARSER_DEFINE_1PARAM_STRING2BOOL_FUNC(fStringToBool, str2bool, QStringToBool)

    QFMATHPARSER_DEFINE_1PARAM_NUM2BOOL_FUNC(fIsNan, isnan, std::isnan)
    QFMATHPARSER_DEFINE_1PARAM_NUM2BOOL_FUNC(fIsInf, isinf, std::isinf)
    QFMATHPARSER_DEFINE_1PARAM_NUM2BOOL_FUNC(fIsFinite, isfinite, std::isfinite)
    QFMATHPARSER_DEFINE_1PARAM_NUM2BOOL_FUNC(fIsFloatOK, isnumok, QFFloatIsOK)

    QString ptosystempathseparator(const QString& param);
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fToSystemPathSeparator,tosystempathseparator, ptosystempathseparator )

    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fSinc, sinc, qfSinc)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fTanc, tanc, qfTanc)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fSin, sin)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fCos, cos)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fTan, tan)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fExp, exp)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fLog, log)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fLog10, log10)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fLog2, log2)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fSqrt, sqrt)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fCbrt, cbrt)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fSqr, qfSqr)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fAbs, abs, fabs)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fASin, asin)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fACos, acos)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fATan, atan)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fSinh, sinh)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fCosh, cosh)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fTanh, tanh)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fASinh, asinh)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fACosh, acosh)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fATanh, atanh)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fErf, erf)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fErfc, erfc)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(flGamma, lgamma)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(ftGamma, tgamma)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fJ0, j0)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fJ1, j1)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fY0, y0)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fY1, y1)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fCeil, ceil)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fFloor, floor)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fTrunc, trunc)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC_SIMPLE(fRound, round)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fTheta, theta, qfTheta)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fSigmoid, sigmoid, qfSigmoid)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fSign, sign, qfSign)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fDegToRad, deg2rad, qfDegToRad)
    QFMATHPARSER_DEFINE_1PARAM_NUMERIC_FUNC(fRadToDeg, rad2deg, qfRadToDeg)
    QFMATHPARSER_DEFINE_2PARAM1NUM2VEC_VECTONUM_FUNC(fPolynom, polyval, qfEvalPolynomial)
    QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(fDerivPolynom, polyder, qfDerivePolynomial)

    template <class T>
    inline T QFMathParser_sort(const T& value) {
        T m=value;
        qSort(m);
        return m;
    }

    QFMATHPARSER_DEFINE_1PARAM_VECTOR_FUNC(fSort, sort, QFMathParser_sort)

    template <class T>
    inline bool compareGreaterThan(const T &s1, const T &s2)
     {
         return s1 > s2;
     }

    static inline QVector<double> QFMathParser_dsort(const QVector<double>& value) {
        QVector<double> m=value;
        qSort(m.begin(), m.end(), compareGreaterThan<double>);
        return m;
    }

    static inline QVector<bool> QFMathParser_dsort(const QVector<bool>& value) {
        QVector<bool> m=value;
        qSort(m.begin(), m.end(), compareGreaterThan<bool>);
        return m;
    }
    static inline QStringList QFMathParser_dsort(const QStringList& value) {
        QStringList m=value;
        qSort(m.begin(), m.end(), compareGreaterThan<QString>);
        return m;
    }


    QFMATHPARSER_DEFINE_1PARAM_VECTOR_FUNC(fDSort, dsort, QFMathParser_dsort)


    QVector<double> QFMathParser_shuffleD(const QVector<double>& value);
    QVector<bool> QFMathParser_shuffleB(const QVector<bool>& value);
    QStringList QFMathParser_shuffleS(const QStringList& value);

    void fIsValid(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsVoid(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsInt(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsUInt(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsDouble(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsString(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsBool(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsVector(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsBoolVector(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsDoubleVector(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fIsStringVector(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fShuffle(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fAllTrue(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fAllFalse(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fAnyTrue(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fAnyFalse(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fContains(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fContainsLC(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fReplace(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fReplaceMulti(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fCountOccurences(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fSplit(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fJoin(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRemoveEmpty(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMean, mean, qfstatisticsAverage)

    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSkewness, skewness, qfstatisticsSkewness)
    QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(fCentralMoment, moment, qfstatisticsCentralMoment)
    QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(fNonCentralMoment, ncmoment, qfstatisticsMoment)
    QFMATHPARSER_DEFINE_2PARAM2VEC_VECTONUM_FUNC(fCorrcoeff, corrcoeff, qfstatisticsCorrCoeff)

    QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(fDiff, diff, qfstatisticsDiff)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSum, sum, qfstatisticsSum)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fCount, count, qfstatisticsCount)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fProd, prod, qfstatisticsProd)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSum2, sum2, qfstatisticsSum2)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fVar, var, qfstatisticsVariance)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fStd, std, qfstatisticsStd)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMedian, median, qfstatisticsMedian)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMAD, mad, qfstatisticsMADS)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fNMAD, nmad, qfstatisticsNMADS)
    QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(fQuantile, quantile, qfstatisticsQuantile)

    QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(fCumSum, cumsum, qfstatisticsCumSum)
    QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(fCumProd, cumprod, qfstatisticsCumProd)

    QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC_SIMPLE(fYn, yn)
    QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC_SIMPLE(fJn, jn)
    static inline double fJn(double a, double b) {
        return jn(a,b);
    }
    static inline double fYn(double a, double b) {
        return yn(a,b);
    }

    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fSlit, slit, qfSlit)
    QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC_SIMPLE(fFMod, fmod)
    QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC_SIMPLE(fATan2, atan2)
    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fGauss, gaussnn, qfGaussSqrtE)
    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fGaussDist, gauss, qfGaussNormSqrtE)
    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fRoundSig, roundsig, roundError)

    static inline QString QFMathParser_trimm(const QString& value) {
        return value.trimmed();
    }
    static inline QString QFMathParser_simplify(const QString& value) {
        return value.simplified();
    }
    static inline QString QFMathParser_tolower(const QString& value) {
        return value.toLower();
    }
    static inline QString QFMathParser_toupper(const QString& value) {
        return value.toUpper();
    }


    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fTrimm, trimm, QFMathParser_trimm)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fSimplify, simplify, QFMathParser_simplify)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fToLower, tolower, QFMathParser_tolower)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fToUpper, toupper, QFMathParser_toupper)

    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fReadFile, readfile, readFile)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fescapify, escapify, escapify)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fdeescapify, deescapify, deescapify)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fcleanStringForFilename, clean_string_for_filename, cleanStringForFilename)


    static inline QString QFMathParser_extractfilename(const QString& value) {
        return QFileInfo(value).fileName();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfilename, extract_filename, QFMathParser_extractfilename)
    static inline QString QFMathParser_extractfilepath(const QString& value) {
        return QFileInfo(value).path();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfilepath, extract_file_path, QFMathParser_extractfilepath)
    static inline QString QFMathParser_extractfileabspath(const QString& value) {
        return QFileInfo(value).absolutePath();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfileabspath, extract_absolute_path, QFMathParser_extractfileabspath)
    static inline QString QFMathParser_extractfileabsfilepath(const QString& value) {
        return QFileInfo(value).absoluteFilePath();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfileabsfilepath, extract_absolute_file_path, QFMathParser_extractfileabsfilepath)
    static inline QString QFMathParser_extractfileext(const QString& value) {
        return QFileInfo(value).suffix();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfileext, extract_ext, QFMathParser_extractfileext)
    static inline QString QFMathParser_extractfilefullext(const QString& value) {
        return QFileInfo(value).completeSuffix();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfilefullext, extract_complete_ext, QFMathParser_extractfilefullext)
    static inline QString QFMathParser_extractfilebasename(const QString& value) {
        return QFileInfo(value).baseName();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfilebasename, extract_basename, QFMathParser_extractfilebasename)
    static inline QString QFMathParser_extractfilefullbasename(const QString& value) {
        return QFileInfo(value).completeBaseName();
    }
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fextractfilefullbasenam, extract_complete_basename, QFMathParser_extractfilefullbasename)

    qfmpResult fSRand(const qfmpResult* params, unsigned int  n, QFMathParser* p);

    static inline double fSRand(double rangeMax, QFMathParser* p) {
        p->get_rng()->seed(uint32_t(rangeMax));
        return 0;
    }

    qfmpResult fRand(const qfmpResult* params, unsigned int  n, QFMathParser* p);

    static inline double fRand(double rangeMin, double rangeMax, QFMathParser* p) {
        return p->get_rng()->rand()*(rangeMax-rangeMin)+rangeMin;
    }
    static inline double fRand(double rangeMax, QFMathParser* p) {
        return p->get_rng()->rand(rangeMax);
    }
    static inline double fRand(QFMathParser* p) {
        return p->get_rng()->rand();
    }

    qfmpResult fRandNorm(const qfmpResult* params, unsigned int  n, QFMathParser* p);

    static inline double fRandNorm(double mean, double var, QFMathParser* p) {
        return p->get_rng()->randNorm(mean,var);
    }
    static inline double fRandNorm(double var, QFMathParser* p) {
        return p->get_rng()->randNorm(0,var);
    }
    static inline double fRandNorm(QFMathParser* p) {
        return p->get_rng()->randNorm(0,1);
    }

    qfmpResult fRandInt(const qfmpResult* params, unsigned int  n, QFMathParser* p);

    static inline double fRandInt(double rangeMin, double rangeMax, QFMathParser* p) {
        return p->get_rng()->randInt()*((uint32_t)rangeMax-(uint32_t)rangeMin)+(uint32_t)rangeMin;
    }
    static inline double fRandInt(double rangeMax, QFMathParser* p) {
        return p->get_rng()->randInt((uint32_t)rangeMax);
    }
    static inline double fRandInt(QFMathParser* p) {
        return p->get_rng()->randInt();
    }




    void fEvalFitFunc(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRandVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fRandIntVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRandNormVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRandBoolVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRandVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fRandIntVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRandNormVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRandBoolVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fRandBool(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    static inline double fMin(double a,double b) {
        return (a<b)?a:b;
    }

    static inline double fMax(double a,double b) {
        return (a>b)?a:b;
    }


    qfmpResult fMin(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fMax(const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fLength(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRemove(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fReverse(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDot(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fConcat(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fRemoveAll(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fFind(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fFindFirst(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fContainsSubString(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fContainsSubStringLC(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fFilterFinite(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fSelect(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fUnique(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedAvg(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedVar(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedStd(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedSum(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedSum2(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedMedian(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedMin(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedMax(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedQuantile(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedSkewness(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedProd(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedCount(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedMoment(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedNonCentralMoment(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fIndexedCorrCoeff(const qfmpResult* params, unsigned int  n, QFMathParser* p) ;
    qfmpResult fIndexedMAD(const qfmpResult* params, unsigned int  n, QFMathParser* p) ;
    qfmpResult fIndexedNMAD(const qfmpResult* params, unsigned int  n, QFMathParser* p) ;
    qfmpResult fReturnLast(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fReturnFirst(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fReturnFirstValid(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fReturnLastValid(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fReturnNth(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fLast(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fFirst(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fItem(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fRunningAverage(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fTrapz(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fRegExpCapture(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p, bool minimal, Qt::CaseSensitivity casesens);

    inline void fRegExpCaptureMCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpCapture(r, params, n, p, true, Qt::CaseSensitive);
    }
    inline void fRegExpCaptureMNCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpCapture(r, params, n, p, true, Qt::CaseInsensitive);
    }
    inline void fRegExpCaptureCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpCapture(r, params, n, p, false, Qt::CaseSensitive);
    }
    inline void fRegExpCaptureNCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpCapture(r, params, n, p, false, Qt::CaseInsensitive);
    }


    void fRegExpContains(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p, bool minimal, Qt::CaseSensitivity casesens);

    inline void fRegExpContainsMCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpContains(r, params, n, p, true, Qt::CaseSensitive);
    }
    inline void fRegExpContainsMNCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpContains(r, params, n, p, true, Qt::CaseInsensitive);
    }
    inline void fRegExpContainsCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpContains(r, params, n, p, false, Qt::CaseSensitive);
    }
    inline void fRegExpContainsNCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpContains(r, params, n, p, false, Qt::CaseInsensitive);
    }

    void fRegExpIndexIn(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p, bool minimal, Qt::CaseSensitivity casesens);

    inline void fRegExpIndexInMCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpIndexIn(r, params, n, p, true, Qt::CaseSensitive);
    }
    inline void fRegExpIndexInMNCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpIndexIn(r, params, n, p, true, Qt::CaseInsensitive);
    }
    inline void fRegExpIndexInCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpIndexIn(r, params, n, p, false, Qt::CaseSensitive);
    }
    inline void fRegExpIndexInNCS(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        fRegExpIndexIn(r, params, n, p, false, Qt::CaseInsensitive);
    }

    void fDateNum(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateTimeNum(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDate2Str(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateTime2Str(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fDatediff2Secs(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDatediff2Mins(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDatediff2Hours(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDatediff2Days(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fNow(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateDay(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateMonth(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateYear(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateHour(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateMin(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fDateSec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fVarname(qfmpResult& res, QFMathParser::qfmpNode **nodes, unsigned int n, QFMathParser* p);
    void fType(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fStrVec(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fNumVec(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fBoolVec(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fInt(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fIntVec(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fNum(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fBool(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fEnsureVectorSize(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);
    void fEnsureVectorSizeStart(qfmpResult& res, const qfmpResult* nodes, unsigned int n, QFMathParser* p);

    void addDefaultFunctions(QFMathParser *p);
    bool hasDefaultFunctions(QFMathParser* p);
    void addDefaultVariables(QFMathParser *p);
    bool hasDefaultVariables(QFMathParser* p);



#ifdef QFLIB_LIBRARY
    void fRDRIDs(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetResult(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetResultError(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetName(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetType(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetFolder(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetGroup(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetDescription(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetRole(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetFiles(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetFileTypes(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetFileDescriptions(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDRGetProperty(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);

    void fEvalIDs(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fEvalGetName(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser);
    void fEvalGetType(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser);
    void fEvalGetProperty(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser);
    void fEvalGetDescription(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p);

    void fProjectGetProperty(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser);

    void fRDR_istable(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fTable_column(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fTable_data(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fTable_numcolumn(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fTable_numdata(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fTable_columns(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fTable_columntitles(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fTable_rows(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);


    void fRDR_isoverview(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_overviewwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_overviewheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_overviewsize(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_overviewchannels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_overview(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);

    void fRDR_ismask(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
    void fRDR_maskwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_maskheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_masksize(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_mask(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);

    void fRDR_isimageselection(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser);
    void fRDR_imageselectionwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_imageselectionheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_imageselectionsize(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_imageselectioncount(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_imageselectionnames(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_imageselectionlastofname(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_imageselectionfirstofname(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);
    void fRDR_imageselection(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p);

#endif

}


#endif // QFMATHPARSERDEFAULTLIB_H
