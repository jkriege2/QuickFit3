#ifndef QFMATHPARSERDEFAULTLIB_H
#define QFMATHPARSERDEFAULTLIB_H

#include "qfmathparsertools.h"


/******************************************************************************************
 * default-Funktionen für den Parser
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

    void fShuffle(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fAllTrue(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fAllFalse(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    void fAnyTrue(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fAnyFalse(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);

    void fContains(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);
    void fCountOccurences(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p);


    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMean, mean, qfstatisticsAverage)

    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSkewness, skewness, qfstatisticsSkewness)
    QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(fCentralMoment, moment, qfstatisticsCentralMoment)
    QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(fNonCentralMoment, ncmoment, qfstatisticsMoment)
    QFMATHPARSER_DEFINE_2PARAM2VEC_VECTONUM_FUNC(fCorrcoeff, corrcoeff, qfstatisticsCorrCoeff)

    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSum, sum, qfstatisticsSum)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fCount, count, qfstatisticsCount)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fProd, prod, qfstatisticsProd)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSum2, sum2, qfstatisticsSum2)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fVar, var, qfstatisticsVariance)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fStd, std, qfstatisticsStd)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMedian, median, qfstatisticsMedian)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMAD, mad, qfstatisticsMAD)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fNMAD, nmad, qfstatisticsNMAD)
    QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(fQuantile, quantile, qfstatisticsQuantile)


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
    static inline QString QFMathParser_tolower(const QString& value) {
        return value.toLower();
    }
    static inline QString QFMathParser_toupper(const QString& value) {
        return value.toUpper();
    }

    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fTrimm, trimm, QFMathParser_trimm)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fToLower, tolower, QFMathParser_tolower)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fToUpper, toupper, QFMathParser_toupper)


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


    qfmpResult fLength(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fRemove(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fReverse(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fDot(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fConcat(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fRemoveAll(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fFind(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fFindFirst(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fContains(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fContainsSubString(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fFilterFinite(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fSelect(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fUnique(const qfmpResult* params, unsigned int  n, QFMathParser* p);
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
    qfmpResult fReturnLast(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fReturnFirst(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fLast(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fFirst(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fItem(const qfmpResult* params, unsigned int  n, QFMathParser* p);
    qfmpResult fRunningAverage(const qfmpResult* params, unsigned int  n, QFMathParser* p);

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



    void addDefaultFunctions(QFMathParser *p);
    bool hasDefaultFunctions(QFMathParser* p);
    void addDefaultVariables(QFMathParser *p);
    bool hasDefaultVariables(QFMathParser* p);
}


#endif // QFMATHPARSERDEFAULTLIB_H
