/** \file qfmathparser.cpp
 *  \ingroup qfmp
 */

#define COMPILING_THIS_QFMATHPARSER
#include "qfmathparser.h" // class's header file
#include <iostream>
#include <float.h>
#include <QDebug>
#include <stdint.h>
#include <limits>
#include <algorithm>

/******************************************************************************************
 * default-Funktionen für den Parser
 ******************************************************************************************/


namespace QFMathParser_Private {
    qfmpResult fdoubleToQString(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpString;
      if (n!=1) p->qfmpError(QObject::tr("num2str accepts 1 argument"));
      if (params[0].type!=qfmpDouble) p->qfmpError(QObject::tr("num2str needs double argument"));
      r.str=doubleToQString(params[0].num);
      return r;
    }


    qfmpResult fIntToStr(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpString;
      if (n!=1) p->qfmpError(QObject::tr("int2str accepts 1 argument"));
      if (params[0].type!=qfmpDouble) p->qfmpError(QObject::tr("int2str needs double argument"));
      r.str=QString::number(params[0].toInteger());
      return r;
    }

    qfmpResult fIntToBinStr(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpString;
      if (n!=1) p->qfmpError(QObject::tr("int2bin accepts 1 argument"));
      if (params[0].type!=qfmpDouble) p->qfmpError(QObject::tr("int2bin needs double argument, but argument is %1").arg(params[0].toTypeString()));
      r.str=QString("0b")+QString::number(params[0].toInteger(), 2);
      return r;
    }

    qfmpResult fIntToHexStr(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpString;
      if (n!=1) p->qfmpError(QObject::tr("int2hex accepts 1 argument"));
      if (params[0].type!=qfmpDouble) p->qfmpError(QObject::tr("int2hex needs double argument"));
      r.str=QString("0x")+QString::number(params[0].toInteger(), 16).toUpper();
      return r;
    }

    qfmpResult fIntToOctStr(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpString;
      if (n!=1) p->qfmpError(QObject::tr("int2oct accepts 1 argument"));
      if (params[0].type!=qfmpDouble) p->qfmpError(QObject::tr("int2oct needs double argument"));
      r.str=QString("0o")+QString::number(params[0].toInteger(), 8);
      return r;
    }

    qfmpResult fboolToQString(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpString;
      if (n!=1) p->qfmpError(QObject::tr("bool2str accepts 1 argument"));
      if (params[0].type!=qfmpBool) p->qfmpError(QObject::tr("bool2str needs bool argument"));
      r.str=(r.boolean)?"true":"false";
      return r;
    }


    qfmpResult fToSystemPathSeparator(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpString;
      if (n!=1) p->qfmpError(QObject::tr("tosystempathseparator accepts 1 argument"));
      if (params[0].type!=qfmpString) p->qfmpError(QObject::tr("tosystempathseparator needs string argument"));
      r.str="";
      for (int i=0; i<params[0].str.size(); i++) {
          QChar ch=params[0].str[i];
          if (ch=='/' || ch=='\\') ch=QDir::separator();
          r.str+=ch;
      }

      return r;
    }

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

    QVector<double> QFMathParser_sort(const QVector<double>& value) {
        QVector<double> m=value;
        qSort(m);
        return m;
    }

    bool compareGreaterThan(const double &s1, const double &s2)
     {
         return s1 > s2;
     }

    QVector<double> QFMathParser_dsort(const QVector<double>& value) {
        QVector<double> m=value;
        qSort(m.begin(), m.end(), compareGreaterThan);
        return m;
    }

    QVector<double> QFMathParser_shuffle(const QVector<double>& value) {
        std::vector<double> mm=value.toStdVector();
        std::random_shuffle( mm.begin(), mm.end() );
        return QVector<double>::fromStdVector(mm);
    }


    QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(fSort, sort, QFMathParser_sort)
    QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(fDSort, dsort, QFMathParser_dsort)
    QFMATHPARSER_DEFINE_1PARAM_NUMERICVEC_FUNC(fShuffle, shuffle, QFMathParser_shuffle)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMean, sum, qfstatisticsAverage)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSum, sum, qfstatisticsSum)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fCount, count, qfstatisticsCount)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fProd, prod, qfstatisticsProd)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fSum2, sum2, qfstatisticsSum2)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fVar, var, qfstatisticsVariance)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fStd, std, qfstatisticsStd)
    QFMATHPARSER_DEFINE_1PARAM_VECTONUM_FUNC(fMedian, median, qfstatisticsMedian)
    QFMATHPARSER_DEFINE_2PARAM1VEC_VECTONUM_FUNC(fQuantile, quantile, qfstatisticsQuantile)


    QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC_SIMPLE(fYn, yn)
    QFMATHPARSER_DEFINE_2PARAM2VEC_NUMERIC_FUNC_SIMPLE(fJn, jn)
    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fSlit, slit, qfSlit)
    QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC_SIMPLE(fFMod, fmod)
    QFMATHPARSER_DEFINE_2PARAM12VEC_NUMERIC_FUNC_SIMPLE(fATan2, atan2)
    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fGauss, gaussnn, qfGaussSqrtE)
    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fGaussDist, gauss, qfGaussNormSqrtE)
    QFMATHPARSER_DEFINE_2PARAM1VEC_NUMERIC_FUNC(fRoundSig, roundsig, roundError)

    QString QFMathParser_trimm(const QString& value) {
        return value.trimmed();
    }
    QString QFMathParser_tolower(const QString& value) {
        return value.toLower();
    }
    QString QFMathParser_toupper(const QString& value) {
        return value.toUpper();
    }

    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fTrimm, trimm, QFMathParser_trimm)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fToLower, tolower, QFMathParser_tolower)
    QFMATHPARSER_DEFINE_1PARAM_STRING_FUNC(fToUpper, toupper, QFMathParser_toupper)


    qfmpResult fSRand(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpDouble;
      //if (n!=1) p->qfmpError(QObject::tr("srand accepts 1 argument"));
      if (n>=0 && params[0].type!=qfmpDouble) p->qfmpError(QObject::tr("srand needs double argument"));
      r.num=0;
      //srand((unsigned int)params[0].num);
      if (n>=0) p->get_rng()->seed(uint32_t(params[0].num));
      else p->get_rng()->seed();
      return r;
    }

    qfmpResult fRand(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpDouble;
      if (n>2) p->qfmpError(QObject::tr("rand accepts 0, 1 or 2 arguments"));
      //r.num=double(rand())/double(RAND_MAX);
      if (n==0) r.num=p->get_rng()->rand();
      else if (n==2) r.num=p->get_rng()->rand()*(params[1].num-params[0].num)+params[0].num;
      else r.num=p->get_rng()->rand(params[0].num);
      return r;
    }

    qfmpResult fRandNorm(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpDouble;
      double mean=0;
      double var=1;
      if (n>2) p->qfmpError(QObject::tr("randnorm accepts 0,1 or 2 argument"));
      if (n>0 && (params[0].type!=qfmpDouble)) p->qfmpError(QObject::tr("randnorm needs a double as first argument (if any)"));
      if (n>1 && (params[1].type!=qfmpDouble)) p->qfmpError(QObject::tr("randnorm needs a double as second argument (if any)"));
      if (n==1) {
          var=params[0].num;
      } else if (n>1) {
          mean=params[0].num;
          var=params[1].num;
      }
      r.num=p->get_rng()->randNorm(mean, var);
      return r;
    }

    qfmpResult fRandInt(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      if (n>2) p->qfmpError(QObject::tr("randint accepts 0, 1 or 2 argument"));
      r.type=qfmpDouble;
      if (n==0) r.num=p->get_rng()->randInt();
      else if (n==2) r.num=p->get_rng()->randInt()*((uint32_t)params[1].num-(uint32_t)params[0].num)+(uint32_t)params[0].num;
      else r.num=p->get_rng()->randInt((uint32_t)params[0].num);
      return r;
    }








    qfmpResult fMin(const qfmpResult* params, unsigned int  n, QFMathParser* p){
      qfmpResult r;
      r.type=qfmpDouble;
      if (n==1 && params[0].type==qfmpDoubleVector) {
          r.num=qfstatisticsMin(params[0].numVec);
      } else if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
          r.num=fmin(params[0].num, params[1].num);
      } else {
          p->qfmpError(QObject::tr("min(...) takes two number or 1 number vector argument"));
          r.isValid=false;
          return r;
      }
      return r;
    }

    qfmpResult fMax(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.type=qfmpDouble;
        if (n==1 && params[0].type==qfmpDoubleVector) {
            r.num=qfstatisticsMax(params[0].numVec);
        } else if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
            r.num=fmax(params[0].num, params[1].num);
        } else {
            p->qfmpError(QObject::tr("max(...) takes two number or 1 number vector argument"));
            r.isValid=false;
            return r;
        }
        return r;
    }

    qfmpResult fLength(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.type=qfmpDouble;
        if (n!=1) {
            p->qfmpError(QObject::tr("length(x) need one argument"));
            r.isValid=false;
            return r;
        }
        r.num=params[0].length();
        return r;
    }

    qfmpResult fRemove(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.isValid=false;
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].convertsToIntVector()) {
            QVector<int> ii=params[1].asIntVector();
            QVector<double> dat=params[0].numVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            for (int i=0; i<dat.size(); i++) {
                if (!ii.contains(i)) {
                    r.numVec<<dat[i];
                }
            }
        } else {
            p->qfmpError(QObject::tr("remove(x, idx) need one number vector and one number or number vector argument"));
        }
        return r;
    }

    qfmpResult fDot(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.isValid=false;
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector && params[0].numVec.size()==params[1].numVec.size()) {
            double sum=0;
            for (int i=0; i<params[0].numVec.size(); i++) {
                sum+=params[0].numVec[i]*params[1].numVec[i];
            }
            return qfmpResult(sum);
        } else {
            p->qfmpError(QObject::tr("dot(x, y) needs two number vector arguments of equal size"));
        }
        return r;
    }

    qfmpResult fConcat(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.isValid=false;
        if (n>0) {
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            for (unsigned int i=0; i<n; i++) {
                if (params[i].convertsToVector()) {
                    r.numVec+=params[i].asVector();
                } else {
                    p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more number vector or number arguments"));
                    return p->getInvalidResult();
                }
            }
        } else {
            p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more number vector or number arguments"));
            return p->getInvalidResult();
        }
        return r;
    }

    qfmpResult fRemoveAll(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.isValid=false;
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {
            const QVector<double>& dat=params[0].numVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]!=params[1].num) {
                    r.numVec<<dat[i];
                }
            }
        } else {
            p->qfmpError(QObject::tr("removeall(x, value) need one number vector and one number argument"));
        }
        return r;
    }

    qfmpResult fReverse(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.type=qfmpDoubleVector;
        if (n==1 && params[0].type==qfmpDoubleVector) {
            r=params[0].numVec;
            for (int i=0; i<r.numVec.size(); i++) {
                r.numVec[i]=params[0].numVec[r.numVec.size()-i-1];
            }
        } else {
            p->qfmpError(QObject::tr("reverse(x) need one number vector argument"));
            r.isValid=false;
            return r;
        }
        return r;
    }


}

void QFMathParser::addStandardVariables(){
    addVariableDouble("pi", M_PI);
    addVariableDouble("e", M_E);
    addVariableDouble("sqrt2", sqrt(2));
    addVariableString("version", "0.2");
    addVariableDouble("log2e", M_LOG2E);
    addVariableDouble("log10e", M_LOG10E);
    addVariableDouble("ln2", M_LN2);
    addVariableDouble("ln10", M_LN10);
    addVariableDouble("h", 6.6260689633E-34);
    addVariableDouble("hbar", 1.05457162853E-34);
    addVariableDouble("epsilon0", 8.854187817E-12);
    addVariableDouble("mu0", 12.566370614E-7);
    addVariableDouble("c", 299792458);
    addVariableDouble("ce", 1.60217648740E-19);
    addVariableDouble("muB", 927.40091523E-26);
    addVariableDouble("muB_eV", 5.788381755579E-5);
    addVariableDouble("muN", 5.0507832413E-27);
    addVariableDouble("muN_eV", 3.152451232645E-8);
    addVariableDouble("me", 9.1093821545E-31);
    addVariableDouble("mp", 1.67262163783E-27);
    addVariableDouble("mn", 1.67492721184E-27);
    addVariableDouble("NA", 6.0221417930E23);
    addVariableDouble("kB", 1.380650424E-23);
    addVariableDouble("kB_eV", 8.61734315E-5);

    for (int i=0; i<externalGlobalVariables.size(); i++) {
        addVariable(externalGlobalVariables[i].first, externalGlobalVariables[i].second);
    }

}

void QFMathParser::addStandardFunctions(){
    addFunction("sinc", QFMathParser_Private::fSinc);
    addFunction("asin", QFMathParser_Private::fASin);
    addFunction("acos", QFMathParser_Private::fACos);
    addFunction("atan", QFMathParser_Private::fATan);
    addFunction("atan2", QFMathParser_Private::fATan2);
    addFunction("sin", QFMathParser_Private::fSin);
    addFunction("cos", QFMathParser_Private::fCos);
    addFunction("tan", QFMathParser_Private::fTan);
    addFunction("sinh", QFMathParser_Private::fSinh);
    addFunction("cosh", QFMathParser_Private::fCosh);
    addFunction("tanh", QFMathParser_Private::fTanh);
    addFunction("log", QFMathParser_Private::fLog);
    addFunction("log2", QFMathParser_Private::fLog2);
    addFunction("log10", QFMathParser_Private::fLog10);
    addFunction("exp", QFMathParser_Private::fExp);
    addFunction("sqrt", QFMathParser_Private::fSqrt);
    addFunction("cbrt", QFMathParser_Private::fCbrt);
    addFunction("sqr", QFMathParser_Private::fSqr);
    addFunction("abs", QFMathParser_Private::fAbs);
    addFunction("erf", QFMathParser_Private::fErf);
    addFunction("erfc", QFMathParser_Private::fErfc);
    addFunction("lgamma", QFMathParser_Private::flGamma);
    addFunction("tgamma", QFMathParser_Private::ftGamma);
    addFunction("j0", QFMathParser_Private::fJ0);
    addFunction("j1", QFMathParser_Private::fJ1);
    addFunction("jn", QFMathParser_Private::fJn);
    addFunction("y0", QFMathParser_Private::fY0);
    addFunction("y1", QFMathParser_Private::fY1);
    addFunction("yn", QFMathParser_Private::fYn);
    addFunction("rand", QFMathParser_Private::fRand);
    addFunction("randint", QFMathParser_Private::fRandInt);
    addFunction("randnorm", QFMathParser_Private::fRandNorm);
    addFunction("srand", QFMathParser_Private::fSRand);
    addFunction("ceil", QFMathParser_Private::fCeil);
    addFunction("floor", QFMathParser_Private::fFloor);
    addFunction("trunc", QFMathParser_Private::fTrunc);
    addFunction("round", QFMathParser_Private::fRound);
    addFunction("fmod", QFMathParser_Private::fFMod);
    addFunction("min", QFMathParser_Private::fMin);
    addFunction("max", QFMathParser_Private::fMax);
    addFunction("int2bin", QFMathParser_Private::fIntToBinStr);
    addFunction("int2oct", QFMathParser_Private::fIntToOctStr);
    addFunction("int2hex", QFMathParser_Private::fIntToHexStr);
    addFunction("int2str", QFMathParser_Private::fIntToStr);
    addFunction("num2str", QFMathParser_Private::fdoubleToQString);
    addFunction("bool2str", QFMathParser_Private::fboolToQString);
    addFunction("gaussnn", QFMathParser_Private::fGauss);
    addFunction("gauss", QFMathParser_Private::fGaussDist);
    addFunction("slit", QFMathParser_Private::fSlit);
    addFunction("theta", QFMathParser_Private::fTheta);
    addFunction("tanc", QFMathParser_Private::fTanc);
    addFunction("sigmoid", QFMathParser_Private::fSigmoid);
    addFunction("sign", QFMathParser_Private::fSign);
    addFunction("roundsig", QFMathParser_Private::fRoundSig);
    addFunction("tosystempathseparator", QFMathParser_Private::fToSystemPathSeparator);
    addFunction("trimm", QFMathParser_Private::fTrimm);
    addFunction("tolower", QFMathParser_Private::fToLower);
    addFunction("toupper", QFMathParser_Private::fToUpper);
    addFunction("length", QFMathParser_Private::fLength);
    addFunction("sort", QFMathParser_Private::fSort);
    addFunction("dsort", QFMathParser_Private::fDSort);
    addFunction("count", QFMathParser_Private::fCount);
    addFunction("sum", QFMathParser_Private::fSum);
    addFunction("sum2", QFMathParser_Private::fSum2);
    addFunction("mean", QFMathParser_Private::fMean);
    addFunction("std", QFMathParser_Private::fStd);
    addFunction("var", QFMathParser_Private::fVar);
    addFunction("median", QFMathParser_Private::fMedian);
    addFunction("quantile", QFMathParser_Private::fQuantile);
    addFunction("remove", QFMathParser_Private::fRemove);
    addFunction("removeall", QFMathParser_Private::fRemoveAll);
    addFunction("shuffle", QFMathParser_Private::fShuffle);
    addFunction("reverse", QFMathParser_Private::fReverse);
    addFunction("concat", QFMathParser_Private::fConcat);
    addFunction("deg2rad", QFMathParser_Private::fDegToRad);
    addFunction("rad2deg", QFMathParser_Private::fRadToDeg);
    addFunction("dot", QFMathParser_Private::fDot);

    for (int i=0; i<externalGlobalFunctions.size(); i++) {
        addFunction(externalGlobalFunctions[i].first, externalGlobalFunctions[i].second);
    }
}






















QString QFMathParser::resultTypeToString(qfmpResultType type)
{
    switch(type) {
        case qfmpDouble: return QObject::tr("number");
        case qfmpDoubleVector: return QObject::tr("number vector");
        case qfmpBool: return QObject::tr("bool");
        case qfmpString: return QObject::tr("string");
        case qfmpVoid: return QObject::tr("void");
    }
    return QObject::tr("invalid");
}

QString QFMathParser::resultToString(const qfmpResult& r)
{
    return r.toString();
}

QString QFMathParser::resultToTypeString(const qfmpResult& r)
{
    return r.toTypeString();
}

qfmpResult QFMathParser::getInvalidResult()
{
    qfmpResult res;
    res.isValid=false;
    return res;
}

void QFMathParser::qfmpError(QString st)
{
    //qDebug()<<"##ERROR: "<<st;
    lastError.append(st);
    errors++;
}

QString QFMathParser::tokentostring(QFMathParser::qfmpTokenType token) {
    switch(token) {
        case END: return "END";
        case PRINT: return "PRINT (;)";
        case COMMA: return "COMMA (,)";
	    case STRING_DELIM: return "STRING_DELIM (' or \")";
	    case NAME: return "NAME";
	    case NUMBER: return "NUMBER";
	    case PLUS: return "PLUS (+)";
	    case MINUS: return "MINUS (-)";
	    case MUL: return "MUL (*)";
	    case DIV: return "DIV (/)";
	    case MODULO: return "MODULO (%)";
	    case ASSIGN: return "ASSIGN (=)";
        case LPARENTHESE: return "LPARENTHESE '('";
        case RPARENTHESE: return "RPARENTHESE ')'";
        case LBRACKET: return "LBRACKET '['";
        case COLON: return "COLON ':'";
        case RBRACKET: return "RBRACKET ']'";
        case LBRACE: return "LBRACE '{'";
        case RBRACE: return "RBRACE '}'";
        case POWER: return "POWER (^)";
        case TILDE: return "TILDE (~)";
        case FACTORIAL_LOGIC_NOT: return "FACTORIAL_LOGIC_NOT (!)";
	    case LOGIC_NOT: return "LOGIC_NOT (!/not)";
	    case LOGIC_AND: return "LOGIC_AND (&&/and)";
	    case LOGIC_OR: return "LOGIC_OR (||/or)";
        case BINARY_AND: return "BINARY_AND (&)";
        case BINARY_OR: return "BINARY_OR (|)";
        case LOGIC_XOR: return "LOGIC_XOR (xor)";
	    case LOGIC_NOR: return "LOGIC_NOR (nor)";
	    case LOGIC_NAND: return "LOGIC_NAND (nand)";
	    case LOGIC_TRUE: return "LOGIC_TRUE (true)";
	    case LOGIC_FALSE: return "LOGIC_FALSE (false)";
	    case COMP_EQUALT: return "COMP_EQUALT (==)";
	    case COMP_UNEQUAL: return "COMP_UNEQUAL (!=)";
	    case COMP_GREATER: return "COMP_GREATER (>)";
	    case COMP_SMALLER: return "COMP_SMALLER (<)";
	    case COMP_GEQUAL: return "COMP_GEQUAL (>=)";
	    case COMP_SEQUAL: return "COMP_SEQUAL (<=)";
    }
    return "unknown";
}

QString QFMathParser::currenttokentostring() {
    return tokentostring(CurrentToken);
}



/******************************************************************************************
 * jkMathParser
 ******************************************************************************************/

// class constructor
QFMathParser::QFMathParser() {
    //qDebug()<<"constructing QFMathParser";
    environment.setParent(this);
    //qDebug()<<"constructing QFMathParser: adding functions";
    addStandardFunctions();
    //qDebug()<<"constructing QFMathParser: adding variables";
    addStandardVariables();
    //qDebug()<<"constructing QFMathParser: resetting errors";
    resetErrors();
    //qDebug()<<"done";
}


// class destructor
QFMathParser::~QFMathParser()
{
	clearFunctions();
    clearVariables();
}

bool QFMathParser::hasErrorOccured() const
{
    return errors>0;
}


void QFMathParser::addVariableDouble(const QString &name, double v)
{
    //qDebug()<<"addVariableDouble("<<name<<v<<")";
    environment.addVariableDouble(name, v);
}

void QFMathParser::addVariableString(const QString& name, const QString &v)
{
    environment.addVariableString(name, v);
}

void QFMathParser::addVariableBoolean(const QString &name, bool v)
{
    environment.addVariableBoolean(name, v);
}

void QFMathParser::addVariable(const QString& name, const qfmpResult& result)
{
    environment.addVariable(name, result);

}

void QFMathParser::setVariable(const QString& name, const qfmpResult& result)
{
    environment.setVariable(name, result);

}

void QFMathParser::deleteVariable(const QString &name)
{
    environment.deleteVariable(name);
}


QString QFMathParser::printVariables() {
    return environment.printVariables();

}

QList<QPair<QString, QFMathParser::qfmpVariable> > QFMathParser::getVariables()
{
    return environment.getVariables();
}

QString QFMathParser::printFunctions()
{
    return environment.printFunctions();
}

QList<QPair<QString, QFMathParser::qfmpFunctionDescriptor> > QFMathParser::getFunctions()
{
    return environment.getFunctions();
}

QList<QPair<QString, QFMathParser::qfmpEvaluateFunc> > QFMathParser::externalGlobalFunctions= QList<QPair<QString, QFMathParser::qfmpEvaluateFunc> >() ;
QList<QPair<QString, qfmpResult> > QFMathParser::externalGlobalVariables= QList<QPair<QString, qfmpResult> >();


void QFMathParser::addGlobalFunction(const QString &name, QFMathParser::qfmpEvaluateFunc function)
{
    externalGlobalFunctions.append(qMakePair(name, function));
}

void QFMathParser::addGlobalVariable(const QString &name, double value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, const QVector<double> &value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, const QString &value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}

void QFMathParser::addGlobalVariable(const QString &name, bool value)
{
    externalGlobalVariables.append(qMakePair(name, qfmpResult(value)));
}



void QFMathParser::clearFunctions()
{
    environment.clearFunctions();
}


void QFMathParser::clearVariables(){
    environment.clearVariables();
}

// gibt den aktuellen Wert einer Variablen zurück
qfmpResult QFMathParser::getVariable(QString name)
{
    return environment.getVariable(name);
}

qfmpResult QFMathParser::getVariableOrInvalid(QString name)
{
    if (environment.variableExists(name)) return environment.getVariable(name);
    qfmpResult r;
    r.isValid=false;
    return r;
}



void QFMathParser::enterBlock()
{
    environment.enterBlock();
}

void QFMathParser::leaveBlock()
{
    environment.leaveBlock();
}


QChar QFMathParser::peekStream(QTextStream *stream)
{
    if (!stream) return QChar(0);
    qint64 p=stream->pos();
    QChar c=QChar(0);
    if (!stream->atEnd()) {
        stream->operator>>(c);
        stream->seek(p);
    }
    //qDebug()<<"peek "<<c;
    return c;
}

void QFMathParser::putbackStream(QTextStream *stream, QChar ch)
{
    if (ch!=0 && stream) stream->seek(stream->pos()-1);
}

bool QFMathParser::getFromStream(QTextStream *stream, QChar &c)
{
    if (!stream) return false;
    c=QChar(0);
    if (stream->atEnd()) {
        //qDebug()<<"read "<<c<<": false";
        return false;
    }
    stream->operator>>(c);
    //qDebug()<<"read "<<c<<": true";
    return true;
}



void QFMathParser::addVariable(const QString &name, const QFMathParser::qfmpVariable& value)
{
    environment.addVariable(name, value);
}


// wertet eine Funktion aus
qfmpResult QFMathParser::evaluateFunction(const QString& name, const QVector<qfmpResult> &params)
{
  return environment.evaluateFunction(name, params);

}

void QFMathParser::addFunction(const QString& name, qfmpEvaluateFunc function) {
  qfmpFunctionDescriptor f;
  f.function=function;
  f.name=name;
  f.type=QFMathParser::functionC;
  environment.setFunction(name, f);
}

void QFMathParser::addFunction(const QString &name, QStringList parameterNames, QFMathParser::qfmpNode *function)
{
    environment.addFunction(name, parameterNames, function);
}


QFMathParser::qfmpTokenType QFMathParser::getToken(){
    QChar ch=0;
    while(getFromStream(program, ch) && ch.isSpace()) {
		;
	}


    switch (ch.toLatin1()) {
		case 0:
		case -1:
			return CurrentToken=END;
        case ';':
            return CurrentToken=PRINT;
        case ':':
            return CurrentToken=COLON;
        case '*':
			return CurrentToken=MUL;
		case '/':
			return CurrentToken=DIV;
		case '%':
			return CurrentToken=MODULO;
		case '+':
			return CurrentToken=PLUS;
		case '-':
			return CurrentToken=MINUS;
        case '(':
            return CurrentToken=LPARENTHESE;
        case ')':
            return CurrentToken=RPARENTHESE;
        case '[':
            return CurrentToken=LBRACKET;
        case ']':
            return CurrentToken=RBRACKET;
        case '{':
            return CurrentToken=LBRACE;
        case '}':
            return CurrentToken=RBRACE;
        case ',':
            return CurrentToken=COMMA;
		case '"':
			return CurrentToken=STRING_DELIM;
		case '^':
			return CurrentToken=POWER;
        case '~':
            return CurrentToken=TILDE;
        case '!':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_UNEQUAL;
			// else
            putbackStream(program, ch1);
			return CurrentToken=FACTORIAL_LOGIC_NOT;
		}
		case '&':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='&') return CurrentToken=LOGIC_AND;
			// else
            putbackStream(program, ch1);
            return CurrentToken=BINARY_AND;
            //qfmpError(QObject::tr("undefined operator '&'; Did you mean LOGICAL_AND ('&&' / 'and')?"));
            break;
		}
		case '|':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='|') return CurrentToken=LOGIC_OR;
			// else
            putbackStream(program, ch1);
            return CurrentToken=BINARY_OR;
            //qfmpError(QObject::tr("undefined operator '|'; Did you mean LOGICAL_OR ('||' / 'or')?"));
            break;
		}
		case '=':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_EQUALT;
			// else
            putbackStream(program, ch1);
			return CurrentToken=ASSIGN;
		}
		case '>':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_GEQUAL;
			// else
            putbackStream(program, ch1);
			return CurrentToken=COMP_GREATER;
		}
		case '<':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='=') return CurrentToken=COMP_SEQUAL;
			// else
            putbackStream(program, ch1);
			return CurrentToken=COMP_SMALLER;
		}
		case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': case '.':{
            putbackStream(program, ch);
            //(*program) >> NumberValue;
            NumberValue=readNumber();
			return CurrentToken=NUMBER;
		}
		default:
            if (ch.isLetter() || (ch=='_')) { // try to recognize NAME, LOGIC_TRUE, LOGIC_FALSE, DIFF_LBRACKET
				StringValue=ch;
                while (getFromStream(program, ch) && (ch.isDigit()||ch.isLetter() || (ch=='_') || (ch=='.'))) {
                    if ((ch.isDigit()||ch.isLetter()) || (ch=='_') || (ch=='.')) {
                        StringValue.push_back(ch);
                    }
				}

                putbackStream(program, ch); // now put the last thing read back int the stream, as it
									  // could belong to the next token

				if (StringValue=="true") return CurrentToken=LOGIC_TRUE;
				if (StringValue=="false") return CurrentToken=LOGIC_FALSE;
                if (StringValue.toLower()=="nan") {
                    NumberValue=std::numeric_limits<double>::quiet_NaN();
                    return CurrentToken=NUMBER;
                }
                if (StringValue.toLower()=="inf") {
                    NumberValue=std::numeric_limits<double>::infinity();
                    return CurrentToken=NUMBER;
                }
                if (StringValue.toLower()=="ninf") {
                    NumberValue=-std::numeric_limits<double>::infinity();
                    return CurrentToken=NUMBER;
                }
                if (StringValue=="and") return CurrentToken=LOGIC_AND;
                if (StringValue=="or") return CurrentToken=LOGIC_OR;
                if (StringValue=="band") return CurrentToken=BINARY_AND;
                if (StringValue=="bor") return CurrentToken=BINARY_OR;
                if (StringValue=="xor") return CurrentToken=LOGIC_XOR;
				if (StringValue=="not") return CurrentToken=LOGIC_NOT;
				if (StringValue=="nor") return CurrentToken=LOGIC_NOR;
				if (StringValue=="nand") return CurrentToken=LOGIC_NAND;

				return CurrentToken=NAME;
			}
			// the parser has found an unknown token. an exception will be thrown
			//std::cout<<StringValue<<",   "<<ch<<std::endl;
            qfmpError(QString("unknown token currentCharacter='%1', currentString='%2'").arg(qchartostr(ch)).arg(StringValue));
            break;
	}
    return END;
}


QFMathParser::qfmpNode* QFMathParser::parse(QString prog){
    progStr=prog;
    program=new QTextStream(&progStr);
    QFMathParser::qfmpNode* res=NULL;
    QFMathParser::qfmpNodeList* resList=new QFMathParser::qfmpNodeList(this);
	while(true) {
		getToken();
        if (CurrentToken == END) {
            break;
        }
        res= logicalExpression(false);
        resList->add(res);
	}
	delete program;
    if (resList->getCount()==1) {
        //delete resList;
        resList->popFirst(false);
        return res;
    } else {
        return resList;
    }
}

qfmpResult QFMathParser::evaluate(QString prog) {
    QFMathParser::qfmpNode* res=parse(prog);
    qfmpResult r=res->evaluate();
    delete res;
    return r;
}

QFMathParser::qfmpNode* QFMathParser::logicalExpression(bool get){
    QFMathParser::qfmpNode* left=logicalTerm(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case LOGIC_OR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPor, left, logicalTerm(true), this, NULL);
				break;
            case LOGIC_XOR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPxor, left, logicalTerm(true), this, NULL);
				break;
			case LOGIC_NOR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPnor, left, logicalTerm(true), this, NULL);
				break;
			default:
				return left;
		}
}

QFMathParser::qfmpNode* QFMathParser::logicalTerm(bool get){
    QFMathParser::qfmpNode* left=compExpression(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
            case LOGIC_AND:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPand, left, compExpression(true), this, NULL);
                break;
            case LOGIC_NAND:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryBoolNode(qfmpLOPnand, left, compExpression(true), this, NULL);
				break;
			default:
				return left;
		}
}

QFMathParser::qfmpNode* QFMathParser::compExpression(bool get){
    QFMathParser::qfmpNode* left=mathExpression(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case COMP_EQUALT:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_UNEQUAL:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPnequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_GREATER:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPgreater, left, mathExpression(true), this, NULL);
				break;
			case COMP_SMALLER:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPlesser, left, mathExpression(true), this, NULL);
				break;
			case COMP_GEQUAL:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPgreaterequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_SEQUAL:
                left= (QFMathParser::qfmpNode*)new qfmpCompareNode(qfmpCOMPlesserequal, left, mathExpression(true), this, NULL);
				break;
			default:
				return left;
		}
}


QFMathParser::qfmpNode* QFMathParser::mathExpression(bool get){
    QFMathParser::qfmpNode* left=mathTerm(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case PLUS:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('+', left, mathTerm(true), this, NULL);
				break;
            case BINARY_OR:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('|', left, mathTerm(true), this, NULL);
                break;
            case MINUS:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('-', left, mathTerm(true), this, NULL);
				break;
			default:
				return left;
		}
}

QFMathParser::qfmpNode* QFMathParser::mathTerm(bool get){
    QFMathParser::qfmpNode* left=vectorPrimary(get);

	for(;;) // forever, do until you find anything else than a term
		switch(CurrentToken) {
			case MUL:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('*', left, vectorPrimary(true), this, NULL);
				break;
			case DIV:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('/', left, vectorPrimary(true), this, NULL);
				break;
			case MODULO:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('%', left, vectorPrimary(true), this, NULL);
				break;
            case BINARY_AND:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('&', left, vectorPrimary(true), this, NULL);
                break;
            default:
				return left;
		}
}


QFMathParser::qfmpNode* QFMathParser::vectorPrimary(bool get){

    QFMathParser::qfmpNode* res=NULL;
    if (get) getToken();

    if(CurrentToken==LBRACKET) { // found primary: [ expression, expression, ... ] i.e. a vector

        qfmpVectorList* vl=new qfmpVectorList(this, NULL);
        res=vl;
        getToken();
        while ((CurrentToken != RBRACKET)&&(CurrentToken!=END)) {
            QFMathParser::qfmpNode* parameter=logicalExpression(vl->getCount()>0);
            vl->add(parameter);
            if ((CurrentToken!=RBRACKET)&&(CurrentToken!=COMMA)&&(CurrentToken!=END)) {
                qfmpError(QString("']' or ',' expected, but '%1' found after %2 elements").arg(currenttokentostring()).arg(vl->getCount()));
                return NULL;
            }

        }


        if ( CurrentToken != RBRACKET ) {
            qfmpError(QString("']' expected, but '%1' found").arg(currenttokentostring()));
            return NULL;
        }
        getToken();
    } else { // vector construct start:delta:end and start:end


        res=primary(false);

        if (CurrentToken==COLON) {
            QFMathParser::qfmpNode* v1=res;
            QFMathParser::qfmpNode* v2=primary(true);
            if (CurrentToken==COLON) {
                res=(QFMathParser::qfmpNode*)new qfmpVectorConstructionNode(v1, primary(true), v2, this, NULL);
            } else {
                res=(QFMathParser::qfmpNode*)new qfmpVectorConstructionNode(v1, v2, NULL, this, NULL);
            }
        }
    }

    if (!res) res=new qfmpInvalidNode(this, NULL);

    return res;

}


QFMathParser::qfmpNode* QFMathParser::primary(bool get){
    QFMathParser::qfmpNode* res=NULL;

	if (get) getToken();

	switch(CurrentToken) {
		case NUMBER: {
            qfmpResult val;
            val.type=qfmpDouble;
			val.num=NumberValue;
			getToken();
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			break;
		}

		case NAME: {
                //jkMathParser::qfmpNode* def=NULL;
                QString varname=StringValue;
                getToken();
                if (CurrentToken == ASSIGN) { // assign a variable name
                    res=new qfmpVariableAssignNode(varname, logicalExpression(true), this, NULL);
                } else if (CurrentToken == LBRACKET) { // vector element access found
                    QFMathParser::qfmpNode* parameter=logicalExpression(true);
                    if ( CurrentToken != RBRACKET ) {
                        qfmpError(QObject::tr("']' expected, but '%1' found").arg(currenttokentostring()));
                        return NULL;
                    }
                    getToken();
                    if (CurrentToken == ASSIGN) { // assign a variable name
                        res=new qfmpVectorElementAssignNode(varname, parameter, logicalExpression(true), this, NULL);
                    } else {
                        res=new qfmpVectorAccessNode(varname, parameter, this, NULL);
                    }
                } else if (CurrentToken == LPARENTHESE) { // function found
                    //QFMathParser::qfmpNode** params=(QFMathParser::qfmpNode**)malloc(255*sizeof(QFMathParser::qfmpNode*));
                    QVector<QFMathParser::qfmpNode*> params;


                    getToken();
                    while ((CurrentToken != RPARENTHESE)&&(CurrentToken!=END)) {
                        QFMathParser::qfmpNode* parameter=logicalExpression(params.size()>0);
                        params.append(parameter);
                        if ((CurrentToken!=RPARENTHESE)&&(CurrentToken!=COMMA)&&(CurrentToken!=END)) {
                            qfmpError(QObject::tr("')' or ',' expected, but '%1' found").arg(currenttokentostring()));
                            return NULL;
                        }

                    }

                    if ( CurrentToken != RPARENTHESE ) {
                        qfmpError(QObject::tr("')' expected, but '%1' found").arg(currenttokentostring()));
                        return NULL;
                    }
                    getToken();
                    QString lvarname=varname.toLower();
                    if (lvarname=="if") {
                        if (params.size()!=3) {
                            qfmpError(QObject::tr("'if(decision, trueValue, falseValue)' expects 3 arguments, but '%1' found").arg(params.size()));
                            return NULL;
                        }
                        qfmpCasesNode* cn=new qfmpCasesNode(this, NULL);
                        cn->addCase(params[0], params[1]);
                        cn->setElse(params[2]);
                        res=cn;
                    } else if (lvarname=="cases") {
                        if (params.size()<3 || (params.size()%2==0)) {
                            qfmpError(QObject::tr("'cases(decision1, trueValue1, decision2, trueValue2, ..., elseValue)' expects 3, 5, 7 ... arguments, but '%1' found").arg(params.size()));
                            return NULL;
                        }
                        qfmpCasesNode* cn=new qfmpCasesNode(this, NULL);
                        for (int i=0; i<params.size()-1; i=i+2) {
                            cn->addCase(params[i], params[i+1]);
                        }
                        cn->setElse(params.last());
                        res=cn;
                    } else if (lvarname=="sum" || lvarname=="prod" || lvarname=="for") {
                        if (params.size()==1) {
                            if (lvarname!="for") {
                                res=new qfmpFunctionNode(varname, params, this, NULL);
                            } else {
                                qfmpError(QObject::tr("'for(NAME, start[, delta], end, expression)' expects 4 or 5 arguments, but '%1' found").arg(params.size()));
                                return NULL;
                            }
                        } else if (params.size()==3){
                            QString iName="";
                            qfmpVariableNode* vn=dynamic_cast<qfmpVariableNode*>(params[0]);
                            if (vn) {
                                iName=vn->getName();
                            } else {
                                qfmpError(QObject::tr("'%1(NAME, ..., expression)' expects a variable name as first argument").arg(lvarname));
                                return NULL;
                            }
                            res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[2], this, NULL);
                        } else if (params.size()==4){
                            QString iName="";
                            qfmpVariableNode* vn=dynamic_cast<qfmpVariableNode*>(params[0]);
                            if (vn) {
                                iName=vn->getName();
                            } else {
                                qfmpError(QObject::tr("'%1(NAME, ..., expression)' expects a variable name as first argument").arg(lvarname));
                                return NULL;
                            }
                            res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[2], NULL, params[3], this, NULL);
                        } else if (params.size()==5){
                            QString iName="";
                            qfmpVariableNode* vn=dynamic_cast<qfmpVariableNode*>(params[0]);
                            if (vn) {
                                iName=vn->getName();
                            } else {
                                qfmpError(QObject::tr("'%1(NAME, ..., expression)' expects a variable name as first argument").arg(lvarname));
                                return NULL;
                            }
                            res=new qfmpVectorOperationNode(lvarname, iName, params[1], params[3], params[2], params[4], this, NULL);
                        } else {
                            qfmpError(QObject::tr("'%2(NAME, start[, delta], end, expression)' expects 4 or 5 arguments and 'sum|prod|for(NAME, value_list, expression)' expects 3 arguments, but '%1' found").arg(params.size()).arg(lvarname));
                            return NULL;
                        }
                    } else if (CurrentToken==ASSIGN) { // function assignment
                        bool allParamsAreNames=true;
                        QStringList pnames;
                        for (int i=0; i<params.size(); i++) {
                            qfmpVariableNode* n=NULL;
                            if ((n=dynamic_cast<qfmpVariableNode*>(params[i]))) {
                                pnames<<n->getName();
                            } else {
                                allParamsAreNames=false;
                            }

                        }
                        //qDebug()<<"FASSIGN: "<<varname<<allParamsAreNames<<pnames;
                        if (allParamsAreNames) {
                            res=new qfmpFunctionAssignNode(varname, pnames, primary(true), this, NULL);
                        } else {
                            qfmpError(QObject::tr("malformed function assignmentfound, expected this form: FNAME(P1, P2, ...)=expression").arg(currenttokentostring()));
                            return NULL;
                        }
                    } else {
                        res=new qfmpFunctionNode(varname, params, this, NULL);
                    }

                } else {
                  res=(QFMathParser::qfmpNode*)new qfmpVariableNode(varname, this, NULL);
                }
                //res= def;
                break;

		}

		case STRING_DELIM: {// found primary: "data" == string constant
            qfmpResult val;
            val.type=qfmpString;
			val.str=readDelim('"');
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
		}

		case MINUS:  // found primary:  - primary
            res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('-', primary(true), this, NULL);
			break;

        case FACTORIAL_LOGIC_NOT:
        case LOGIC_NOT:
            res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('!', primary(true), this, NULL);
			break;

        case TILDE:
            res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('~', primary(true), this, NULL);
            break;

        case LPARENTHESE: { // found primary: ( expression )
            QFMathParser::qfmpNode* expr=logicalExpression(true);
            if (CurrentToken != RPARENTHESE) {
                qfmpError(QString("')' expected, but '%1' found").arg(currenttokentostring()));
                return NULL;
            }
			getToken(); // swallow ")"
			res= expr;
			break;
		}
        case LOGIC_TRUE: {// found 'true'
            qfmpResult val;
            val.type=qfmpBool;
			val.boolean=true;
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
        }
		case LOGIC_FALSE: {// found 'false'
            qfmpResult val;
            val.type=qfmpBool;
			val.boolean=false;
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
        }

		default:
            qfmpError(QObject::tr("primary expected, but '%1'' found").arg(currenttokentostring()));
            return new qfmpInvalidNode(this, NULL);

	}

    if (CurrentToken==POWER) {
        res=(QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('^', res, primary(true), this, NULL);
    }


    if (!res) res=new qfmpInvalidNode(this, NULL);

    return res;

}

double QFMathParser::readNumber()
{


  double dfactor=1;
  bool isNumber=true;


  int i=0;
  QChar c;

  if (getFromStream(program, c)) {
   // check sign
    if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
    else if (c=='+') { isNumber=getFromStream(program, c); i++; }

    if (isNumber && c=='0') {
        isNumber=getFromStream(program, c); i++;
        if (c=='x' || c=='X') return dfactor*readHex();
        if (c=='o' || c=='O') return dfactor*readOct();
        if (c=='b' || c=='B') return dfactor*readBin();
    }
    putbackStream(program, c);
    return dfactor*readDec();

  }
  return 0;
}

double QFMathParser::readDec()
{

  double current_double=0;

  bool isMantissa=true;
  double dfactor=1;
  int mantissaPos=0;
  bool isNumber=true;
  bool foundDot=false;


  QString num="";
  int i=0;
  QChar c;

  if (getFromStream(program, c)) {
   // check sign
    if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
    else if (c=='+') { isNumber=getFromStream(program, c); i++; }

    while (isNumber) {
      if (!isMantissa) {
        switch(c.toLatin1()) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            num+=c;
            break;
          case '-':
            if (i==mantissaPos) {
              num+=c;
            } else {
              isNumber=false;
              putbackStream(program, c);
            }
            break;
          case '+':
            if (i!=mantissaPos) {
              isNumber=false;
              putbackStream(program, c);
            }
            break;
          default:
            putbackStream(program, c);
            isNumber=false;
        }
      }
      if (isMantissa) {
        switch(c.toLatin1()) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            num+=c;
            break;
          case '.':
            if (foundDot) {
              putbackStream(program, c);
              isNumber=false;
            } else {
              num+=c;
              foundDot=true;
            }
            break;
          case '-':
            if (i==mantissaPos) {
              dfactor=-1;
            } else {
              isNumber=false;
              putbackStream(program, c);
            }
            break;
          case '+':
            if (i==mantissaPos) {
              dfactor=1;
            } else {
              putbackStream(program, c);
              isNumber=false;
            }
            break;
          case 'e':
          case 'E':
            isMantissa=false;
            num+='e';
            mantissaPos=i+1;
            break;

          default:
            putbackStream(program, c);
            isNumber=false;
        }
      }
      i++;
      if (isNumber) isNumber=getFromStream(program, c);
    }
  }

    if (num.length()<=0) num="0";
    current_double = num.toDouble();
    current_double=(current_double)*dfactor;
    return current_double;
}

double QFMathParser::readHex() {
    double current_double=0;
    double dfactor=1;
    bool isNumber=true;

    QString num="";
    int i=0;
    QChar c;

    if (getFromStream(program, c)) {
        // check sign
        if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
        else if (c=='+') { isNumber=getFromStream(program, c); i++; }

        while (isNumber) {
            switch(c.toLatin1()) {
              case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'a':
                case 'B':
                case 'b':
                case 'C':
                case 'c':
                case 'D':
                case 'd':
                case 'E':
                case 'e':
                case 'F':
                case 'f':
                num+=c;
                break;
              default:
                putbackStream(program, c);
                isNumber=false;
            }
            i++;
            if (isNumber) isNumber=getFromStream(program, c);
        }
    }

      if (num.length()<=0) num="0";
      current_double=num.toInt(NULL, 16);
      current_double=(current_double)*dfactor;
      return current_double;
}

double QFMathParser::readOct() {
    double current_double=0;
    double dfactor=1;
    bool isNumber=true;

    QString num="";
    int i=0;
    QChar c;

    if (getFromStream(program, c)) {
        // check sign
        if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
        else if (c=='+') { isNumber=getFromStream(program, c); i++; }

        while (isNumber) {
            switch(c.toLatin1()) {
              case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                num+=c;
                break;
              default:
                putbackStream(program, c);
                isNumber=false;
            }
            i++;
            if (isNumber) isNumber=getFromStream(program, c);
        }
    }

      if (num.length()<=0) num="0";
      current_double=num.toInt(NULL, 8);
      current_double=(current_double)*dfactor;
      return current_double;
}

double QFMathParser::readBin() {

  double current_double=0;
  double dfactor=1;
  bool isNumber=true;

  QString num="";
  int i=0;
  QChar c;

  if (getFromStream(program, c)) {
      // check sign
      if (c=='-') { dfactor=-1; isNumber=getFromStream(program, c); i++; }
      else if (c=='+') { isNumber=getFromStream(program, c); i++; }

      while (isNumber) {
          switch(c.toLatin1()) {
            case '0':
            case '1':
              num+=c;
              break;
            default:
              putbackStream(program, c);
              isNumber=false;
          }
          i++;
          if (isNumber) isNumber=getFromStream(program, c);
      }
  }

    if (num.length()<=0) num="0";
    double fac=1;
    current_double=0;
    for (int i=num.size()-1; i>=0; i--) {
        if (num[i]=='1') current_double=current_double+fac;
        fac=fac*2.0;
    }
    current_double=(current_double)*dfactor;
    return current_double;
}

QString QFMathParser::readDelim(QChar delimiter){
    QString res="";
    QChar ch=0;

    while(getFromStream(program, ch)) {
		if (ch==delimiter ) {
            QChar ch1=peekStream(program);
		    if (ch1==delimiter) {
                    getFromStream(program, ch);
		            res=res+delimiter;
            } else {
                break;
            }
        } else if ((program) && (ch!=delimiter)) res=res+ch;
	}

	return res;
}









/******************************************************************************************
 * Klassenhierarchie, um Ausdrücke darzustellen
 ******************************************************************************************/
QFMathParser::qfmpUnaryNode::qfmpUnaryNode(char op, QFMathParser::qfmpNode* c, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  child=c;
  child->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpUnaryNode::~qfmpUnaryNode()
{
    if (child) delete child;
}

qfmpResult QFMathParser::qfmpUnaryNode::evaluate(){
  qfmpResult c=child->evaluate();
  qfmpResult res;
  res=c;

  switch(operation) {
    case '!':
         if (c.type==qfmpBool) {
           res.boolean=!c.boolean;
           return res;
         } else parser->qfmpError(QObject::tr("'!' only defined for bool"));
         break;
      case '-':
          if (c.type==qfmpDouble) {
            res.num=-c.num;
            return res;
          } else if (c.type==qfmpDoubleVector) {
              for (int i=0; i<res.numVec.size(); i++) res.numVec[i]=-c.numVec[i];
              return res;
           } else parser->qfmpError(QObject::tr("'-' only defined for numbers"));
           break;
      case '~':
           if (c.isInteger()) {
               res.num=double(~c.toInteger());
               return res;
           } else if (c.type==qfmpDoubleVector) {
               for (int i=0; i<res.numVec.size(); i++) res.numVec[i]=double(~int32_t(c.numVec[i]));
               return res;
           } else parser->qfmpError(QObject::tr("'~' only defined for integer numbers"));
           break;
     default: parser->qfmpError(QObject::tr("unknown unary operation"));
  }
  res.isValid=false;
  return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpUnaryNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpUnaryNode(operation, child->copy(), getParser(), par);
    return n;
}







QFMathParser::qfmpBinaryArithmeticNode::qfmpBinaryArithmeticNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  left=l;
  right=r;
  if (left) left->setParent(this);
  if (right) right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpBinaryArithmeticNode::~qfmpBinaryArithmeticNode()
{
    if (left) delete left;
    if (right) delete right;
}

qfmpResult QFMathParser::qfmpBinaryArithmeticNode::evaluate(){
  qfmpResult l;
  if (left) l=left->evaluate();
  qfmpResult r;
  if (right) r=right->evaluate();
  qfmpResult res;

  switch(operation) {
      case '+':
            return qfmpResult::add(l, r, getParser());
      case '-':
            return qfmpResult::sub(l, r, getParser());
      case '*':
            return qfmpResult::mul(l, r, getParser());
      case '/':
            return qfmpResult::div(l, r, getParser());
      case '%':
            return qfmpResult::mod(l, r, getParser());
      case '^':
          return qfmpResult::power(l, r, getParser());
      case '&':
          return qfmpResult::bitwiseand(l, r, getParser());
      case '|':
          return qfmpResult::bitwiseor(l, r, getParser());

      default: parser->qfmpError(QObject::tr("unknown arithmetic operation"));
  }
  res.isValid=false;
  return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpBinaryArithmeticNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpBinaryArithmeticNode(operation, left->copy(), right->copy(), getParser(), par);
    return n;
}







QFMathParser::qfmpCompareNode::qfmpCompareNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  left=l;
  right=r;
  if (left) left->setParent(this);
  if (right) right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpCompareNode::~qfmpCompareNode()
{
    if (left) delete left;
    if (right) delete right;
}

qfmpResult QFMathParser::qfmpCompareNode::evaluate(){
    qfmpResult l;
    if (left) l=left->evaluate();
    qfmpResult r;
    if (right) r=right->evaluate();  qfmpResult res;
  res.type=qfmpBool;

  if (l.type!=r.type) parser->qfmpError(QObject::tr("you can't compare different datatypes"));

  switch(operation) {
    case qfmpCOMPequal:
          if (l.type==qfmpDouble) {
             res.boolean=(l.num==r.num);
             return res;
          }
          if (l.type==qfmpDoubleVector) {
             res.boolean=(l.numVec==r.numVec);
             return res;
          }
        if (l.type==qfmpBool) {
           res.boolean=(l.boolean==r.boolean);
           return res;
        }
        if (l.type==qfmpString) {
           res.boolean=(l.str==r.str);
           return res;
        }
        break;
    case qfmpCOMPnequal:
        if (l.type==qfmpDouble) {
           res.boolean=(l.num!=r.num);
           return res;
        }
        if (l.type==qfmpDoubleVector) {
           res.boolean=(l.numVec!=r.numVec);
           return res;
        }
        if (l.type==qfmpBool) {
           res.boolean=(l.boolean!=r.boolean);
           return res;
        }
        if (l.type==qfmpString) {
           res.boolean=(l.str!=r.str);
           return res;
        }
        break;
    case qfmpCOMPgreater:
        if (l.type==qfmpDouble) {
           res.boolean=(l.num>r.num);
           return res;
        }
        if (l.type==qfmpBool) {
           res.boolean=(l.boolean>r.boolean);
           return res;
        }
        if (l.type==qfmpString) {
           res.boolean=(l.str>r.str);
           return res;
        }
        break;
    case qfmpCOMPlesser:
        if (l.type==qfmpDouble) {
           res.boolean=(l.num<r.num);
           return res;
        }
        if (l.type==qfmpBool) {
           res.boolean=(l.boolean<r.boolean);
           return res;
        }
        if (l.type==qfmpString) {
           res.boolean=(l.str<r.str);
           return res;
        }
        break;
    case qfmpCOMPgreaterequal:
        if (l.type==qfmpDouble) {
           res.boolean=(l.num>=r.num);
           return res;
        }
        if (l.type==qfmpBool) {
           res.boolean=(l.boolean>=r.boolean);
           return res;
        }
        if (l.type==qfmpString) {
           res.boolean=(l.str>=r.str);
           return res;
        }
        break;
    case qfmpCOMPlesserequal:
        if (l.type==qfmpDouble) {
           res.boolean=(l.num<=r.num);
           return res;
        }
        if (l.type==qfmpBool) {
           res.boolean=(l.boolean<=r.boolean);
           return res;
        }
        if (l.type==qfmpString) {
           res.boolean=(l.str<=r.str);
           return res;
        }
        break;
      default: parser->qfmpError(QObject::tr("compare operation between %1 and %2 not possible").arg(l.toTypeString()).arg(r.toTypeString()));
  }
  res.isValid=false;
  return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpCompareNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpCompareNode(operation, left->copy(), right->copy(), getParser(), par);
    return n;
}







QFMathParser::qfmpBinaryBoolNode::qfmpBinaryBoolNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  left=l;
  right=r;
  if (left) left->setParent(this);
  if (right) right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

QFMathParser::qfmpBinaryBoolNode::~qfmpBinaryBoolNode()
{
    if (left) delete left;
    if (right) delete right;
}

qfmpResult QFMathParser::qfmpBinaryBoolNode::evaluate(){
    qfmpResult l;
    if (left) l=left->evaluate();
    qfmpResult r;
    if (right) r=right->evaluate();
    qfmpResult res;

  if ((l.type!=qfmpBool)||(r.type!=qfmpBool)) parser->qfmpError(QObject::tr("logical operations only for bool"));
  else {
      switch(operation) {
          case qfmpLOPand:
              res.type=qfmpBool;
              res.boolean=l.boolean&&r.boolean;
              return res;
              break;
          case qfmpLOPor:
              res.type=qfmpBool;
              res.boolean=l.boolean||r.boolean;
              return res;
              break;
          case qfmpLOPnor:
              res.type=qfmpBool;
              res.boolean=!(l.boolean||r.boolean);
              return res;
              break;
          case qfmpLOPxor:
              res.type=qfmpBool;
              res.boolean=(l.boolean&& (!r.boolean))||(r.boolean&& (!l.boolean));
              return res;
              break;
          case qfmpLOPnand:
              res.type=qfmpBool;
              res.boolean=!(l.boolean&&r.boolean);
              return res;
              break;

          default: parser->qfmpError(QObject::tr("unknown error"));
              break;
      }
  }
  res.isValid=false;
  return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpBinaryBoolNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNode *n= new QFMathParser::qfmpBinaryBoolNode(operation, left->copy(), right->copy(), getParser(), par);
    return n;
}




QFMathParser::qfmpVariableNode::qfmpVariableNode(QString name, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
 {
  var=name;
  setParser(p);
  setParent(par);
};

qfmpResult QFMathParser::qfmpVariableNode::evaluate() {
    return getParser()->getVariable(var);
}

QFMathParser::qfmpNode *QFMathParser::qfmpVariableNode::copy(QFMathParser::qfmpNode *par)
{
return new QFMathParser::qfmpVariableNode(var, getParser(), par);};




void QFMathParser::qfmpNodeList::add(QFMathParser::qfmpNode* n){
    list.push_back(n);
}

QFMathParser::qfmpNode *QFMathParser::qfmpNodeList::popFirst(bool deleteObject)
{
    if (list.size()>0) {
        if (deleteObject) {
            delete list.first();
            list.removeFirst();
        } else {
            QFMathParser::qfmpNode * n=list.first();
            list.removeFirst();
            return n;
        }

    }
    return NULL;
}


qfmpResult QFMathParser::qfmpNodeList::evaluate(){
  int n=getCount();
  qfmpResult res;
  //std::cout<<"Nodelist.count()="<<n<<std::endl;
  if (n>0) {
     for (int i=0; i<n; i++) {
        if (list[i]) res=list[i]->evaluate();
     }
     return res;
  } else parser->qfmpError(QObject::tr("NodeList empty"));
  res.isValid=false;
  return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpNodeList::copy(QFMathParser::qfmpNode *par)
{

    QFMathParser::qfmpNodeList* n= new QFMathParser::qfmpNodeList(getParser());
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            n->add(list[i]->copy(n));
        }
    }

    n->setParent(par);
    return n;
}

QFMathParser::qfmpNodeList::~qfmpNodeList() {
/*  if (getCount()>0) {
     for (int i=0; i<getCount(); i++) {
        delete list[i];
     }
  };
  list.clear();*/
    if (list.size()>0) for (int i=0; i<list.size(); i++) {
        if (list[i]) delete list[i];
    }
    list.clear();
}


QFMathParser::qfmpVariableAssignNode::~qfmpVariableAssignNode()
{
    if (child) delete child;
}

QFMathParser::qfmpVariableAssignNode::qfmpVariableAssignNode(QString var, QFMathParser::qfmpNode* c, QFMathParser* p, QFMathParser::qfmpNode* par):
    qfmpNode(p, par)
{
  child=c;
  if (child) child->setParent(this);
  setParser(p);
  setParent(par);
  variable=var;
//  std::cout<<"assign: "<<var<<std::endl;
}

qfmpResult QFMathParser::qfmpVariableAssignNode::evaluate(){
  qfmpResult res;
  if (child) res=child->evaluate();
//  std::cout<<"assign: "<<variable<<"    "<<res.num<<std::endl;
  getParser()->setVariable(variable, res);
  return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpVariableAssignNode::copy(QFMathParser::qfmpNode *par)
{
    if (child) return new QFMathParser::qfmpVariableAssignNode(variable, child->copy(NULL), getParser(), par);
    else return new QFMathParser::qfmpVariableAssignNode(variable, NULL, getParser(), par);
}

QFMathParser::qfmpFunctionNode::qfmpFunctionNode(QString name, QVector<qfmpNode *> params, QFMathParser *p, qfmpNode *par):
    qfmpNode(p, par)
 {
  child=params;
  fun=name;
  setParser(p);
  setParent(par);
  if (child.size()>0) {
    for (int i=0; i<child.size(); i++) {
      if (child[i]) child[i]->setParent(this);
    }
  }
}

qfmpResult QFMathParser::qfmpFunctionNode::evaluate() {
  QVector<qfmpResult> data;
  //QString dbg=fun+"(";
  if (child.size()>0) {
    for (int i=0; i<child.size(); i++) {
      if (child[i]) data.append(child[i]->evaluate());
      else data.append(QFMathParser::getInvalidResult());
      //if (i>0) dbg+=", ";
      //dbg+=data.last().toString();
    }
  }
  //dbg+=")";
  //if (fun=="fib") //qDebug()<<dbg;
  return parser->evaluateFunction(fun, data);
}

QFMathParser::qfmpNode *QFMathParser::qfmpFunctionNode::copy(QFMathParser::qfmpNode *par)
{
    QVector<qfmpNode *> params;
    if (child.size()>0) {
        for (int i=0; i<child.size(); i++) {
            params<<child[i]->copy(NULL);
        }
    }

    return new QFMathParser::qfmpFunctionNode(fun, params, getParser(), par);
}


QFMathParser::qfmpFunctionNode::~qfmpFunctionNode() {

    if (child.size()>0) {
      for (int i=0; i<child.size(); i++) {
        if (child[i]) delete child[i];
      }
      child.clear();
    }
}

qfmpResult::qfmpResult()
{
    isValid=true;
    type=qfmpDouble;
    str="";
    num=0;
    boolean=false;
    numVec.clear();
}

qfmpResult::qfmpResult(double value)
{
    isValid=true;
    type=qfmpDouble;
    str="";
    num=value;
    boolean=false;
    numVec.clear();
}

qfmpResult::qfmpResult(QString value)
{
    isValid=true;
    type=qfmpString;
    str=value;
    num=0;
    boolean=false;
    numVec.clear();
}

qfmpResult::qfmpResult(bool value)
{
    isValid=true;
    type=qfmpBool;
    str="";
    num=0;
    boolean=value;
    numVec.clear();
}

qfmpResult::qfmpResult(const QVector<double>& value)
{
    isValid=true;
    type=qfmpDoubleVector;
    str="";
    num=0;
    boolean=false;
    numVec=value;
}

QString qfmpResult::toString() const
{
    switch(type) {
        case qfmpDouble: return doubleToQString(num);
        case qfmpDoubleVector: return QString("[ ")+doubleVecToQString(numVec)+QString(" ]");
        case qfmpString: return str;
        case qfmpBool: return boolToQString(boolean);
        case qfmpVoid: break;
    }
    return "";

}

QString qfmpResult::toTypeString() const
{
    if (!isValid) return QObject::tr("[INVALID]");
    switch(type) {
        case qfmpDouble: return doubleToQString(num)+QObject::tr(" [number]");
        case qfmpDoubleVector: return QString("[ ")+doubleVecToQString(numVec)+QObject::tr(" ] [number vector]");
        case qfmpString: return str+QObject::tr(" [string]");
        case qfmpBool: return boolToQString(boolean)+QObject::tr(" [bool]");
        case qfmpVoid: return boolToQString(boolean)+QObject::tr(" [void]");
    }
    return QFMathParser::resultTypeToString(type);

}

int32_t qfmpResult::toInteger() const
{
    if (type==qfmpDouble) return num;
    if (type==qfmpBool) return (boolean)?1:0;
    return 0;
}

uint32_t qfmpResult::toUInt() const
{
    if (type==qfmpDouble) return num;
    if (type==qfmpBool) return (boolean)?1:0;
    return 0;
}

bool qfmpResult::isInteger() const
{
    return (type==qfmpDouble)&&(fabs(num)<4294967296.0);
}

bool qfmpResult::isUInt() const
{
    return type==qfmpDouble&&(fabs(num)<2147483648.0);
}

int qfmpResult::length() const
{
    if (!isValid) return 0;
    switch(type) {
        case qfmpDouble: return 1;
        case qfmpDoubleVector: return numVec.size();
        case qfmpString: return str.size();
        case qfmpBool: return 1;
        case qfmpVoid: return 0;
    }
    return 0;
}

void qfmpResult::setDouble(double val)
{
    type=qfmpDouble;
    num=val;
}

void qfmpResult::setBoolean(bool val)
{
    type=qfmpBool;
    boolean=val;
}

void qfmpResult::setString(const QString &val)
{
    type=qfmpString;
    str=val;
}

void qfmpResult::setDoubleVec(const QVector<double> &val)
{
    type=qfmpDoubleVector;
    numVec=val;
}

QVector<double> qfmpResult::asVector() const
{
    if (type==qfmpDoubleVector) return numVec;
    else if (type==qfmpDouble) return QVector<double>(1, num);
    return QVector<double>();
}

bool qfmpResult::convertsToVector() const
{
    if (type==qfmpDoubleVector || type==qfmpDouble) return true;
    return false;
}

bool qfmpResult::convertsToIntVector() const
{
    return convertsToVector();
}

bool qfmpResult::isUsableResult() const
{
    return isValid && (type!=qfmpVoid);
}

double qfmpResult::asNumber() const
{
    if (type==qfmpDouble) return num;
    return NAN;
}

QString qfmpResult::asString() const
{
    if (type==qfmpString) return str;
    return QString();
}

bool qfmpResult::asBool() const
{
    if (type==qfmpBool) return boolean;
    if (type==qfmpDouble) return num!=0;
    return false;
}

qfmpResultType qfmpResult::getType() const
{
    return type;
}

qfmpResult qfmpResult::invalidResult()
{
    qfmpResult r;
    r.isValid=false;
    return r;
}

qfmpResult qfmpResult::voidResult()
{
    qfmpResult r;
    r.isValid=true;
    r.type=qfmpVoid;
    return r;
}

qfmpResult qfmpResult::add(const qfmpResult &l, const qfmpResult &r, QFMathParser* p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    /*qDebug()<<QString::number(tt, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDouble)<<8)+qfmpDouble, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDouble)<<8)+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint16_t(qfmpDoubleVector<<8))+qfmpDoubleVector, 16);
    //qDebug()<<QString::number((uint16_t(qfmpString)<<8)+qfmpString, 16);*/
    switch(tt) {
        case (uint16_t(qfmpDouble)<<8)+qfmpDouble:
            return qfmpResult(l.num+r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]+r.num;
            }
            return re;
            break;
        }
        case (uint16_t(qfmpDouble)<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num+r.numVec[i];
            }
            return re;
            break;
        }
        case (uint16_t(qfmpDoubleVector<<8))+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]+r.numVec[i];
            }
            return re;
            break;
        }
        case (uint16_t(qfmpString)<<8)+qfmpString:
            return qfmpResult(l.str+r.str);
            break;
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be added").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::sub(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(l.num-r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]-r.num;
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num-r.numVec[i];
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]-r.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be subtracted").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::mul(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(l.num*r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]*r.num;
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num*r.numVec[i];
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]*r.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be multiplied").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::div(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(l.num/r.num);
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]/r.num;
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.num/r.numVec[i];
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i]/r.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be divided").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::mod(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(double(l.toInteger()%r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])%r.toInteger();
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()%int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])%int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in mod operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::power(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(pow(l.num,r.num));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=pow(l.numVec[i],r.num);
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=pow(l.num,r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=pow(l.numVec[i],r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in power operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::bitwiseand(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(double(l.toInteger()&r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])&r.toInteger();
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()&int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])&int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise and operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::bitwiseor(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpDouble<<8)+qfmpDouble:
            return qfmpResult(double(l.toInteger()|r.toInteger()));
            break;
        case (qfmpDoubleVector<<8)+qfmpDouble: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(l.numVec.size());
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])|r.toInteger();
            }
            return re;
            break;
        }
        case (qfmpDouble<<8)+qfmpDoubleVector: {
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=l.toInteger()|int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        case (qfmpDoubleVector<<8)+qfmpDoubleVector: {
            if (l.length()!=r.length()) {
                if (p) p->qfmpError(QObject::tr("arguments have to have the same length, but lengthes were left=%1, right=%2").arg(l.length()).arg(r.length()));
                return invalidResult();
            }
            qfmpResult re;
            re.type=qfmpDoubleVector;
            re.numVec.resize(r.numVec.size());
            for (int i=0; i<r.numVec.size(); i++) {
                re.numVec[i]=int32_t(l.numVec[i])|int32_t(r.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in bitwise or operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::logicand(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            return qfmpResult(l.boolean&&r.boolean);
            break;

        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic and operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::logicor(const qfmpResult &l, const qfmpResult &r, QFMathParser *p)
{
    uint16_t lt=l.type;
    uint16_t rt=r.type;
    uint16_t tt=(lt<<8)+rt;
    //qDebug()<<QString::number(tt, 16);
    switch(tt) {
        case (qfmpBool<<8)+qfmpBool:
            return qfmpResult(l.boolean||r.boolean);
            break;

        default:
            if (p) p->qfmpError(QObject::tr("arguments of type %1 and %2 may not be used in logic or operation").arg(l.typeName()).arg(r.typeName()));
            break;
    }
    return invalidResult();
}

qfmpResult qfmpResult::logicnot(const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpBool: return qfmpResult(!l.boolean);
        default:
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in logic not operation").arg(l.typeName()));
            break;

    }

    return invalidResult();
}

qfmpResult qfmpResult::neg(const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpDouble: return qfmpResult(-l.num);
        case qfmpDoubleVector: {
            qfmpResult re=l;
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=l.numVec[i];
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in negation operation").arg(l.typeName()));
            break;

    }

    return invalidResult();
}

qfmpResult qfmpResult::bitwisenot(const qfmpResult &l, QFMathParser *p)
{
    switch(l.type) {
        case qfmpDouble: return qfmpResult(double(~l.toInteger()));
        case qfmpDoubleVector: {
            qfmpResult re=l;
            for (int i=0; i<l.numVec.size(); i++) {
                re.numVec[i]=~int32_t(l.numVec[i]);
            }
            return re;
            break;
        }
        default:
            if (p) p->qfmpError(QObject::tr("argument of type %1 may not be used in bitwise not operation").arg(l.typeName()));
            break;

    }

    return invalidResult();
}

QString qfmpResult::typeName() const
{
    return QFMathParser::resultTypeToString(type);
}


QVector<int> qfmpResult::asIntVector() const
{
    if (type==qfmpDouble) return QVector<int>(1, num);
    else if (type==qfmpDoubleVector) {
        QVector<int> ii(numVec.size(), 0);
        for (int i=0; i<numVec.size(); i++) {
            ii[i]=numVec[i];
        }
        return ii;
    }
    return QVector<int>();
}

QFMathParser::qfmpVariable::qfmpVariable()
{
    type=qfmpDouble;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
    numVec=NULL;
}

void QFMathParser::qfmpVariable::clearMemory()
{
    if (internal) {
        if (type==qfmpDouble && num) delete (num);
        if (type==qfmpBool && boolean) delete (boolean);
        if (type==qfmpString && str) delete (str);
        if (type==qfmpDoubleVector && numVec) delete (numVec);
        num=NULL;
        boolean=NULL;
        str=NULL;
        numVec=NULL;
    }
    internal=false;
}

qfmpResult QFMathParser::qfmpVariable::toResult() const
{
    qfmpResult r;
    r.type=type;
    r.isValid=true;
    if (type==qfmpDouble && num) {
        r.num=*(num);
    } else if (type==qfmpString && str) {
        r.str=*(str);
    } else if (type==qfmpBool && boolean) {
        r.boolean=*(boolean);
    } else if (type==qfmpDoubleVector && numVec) {
        r.numVec=*(numVec);
    } else {
        r.isValid=false;
    }
    return r;
}

bool QFMathParser::qfmpVariable::isInternal() const
{
    return internal;
}

void QFMathParser::qfmpVariable::set(const qfmpResult &result)
{
    if (type==result.type) {
        switch (result.type) {
            case qfmpDouble: { if (!num) num=new double; internal=true; } *num=result.num; break;
            case qfmpDoubleVector: { if (!numVec) numVec=new QVector<double>; internal=true; } *numVec=result.numVec; break;
            case qfmpString: { if (!str) str=new QString; internal=true; } *str=result.str; break;
            case qfmpBool: { if (!boolean) boolean=new bool; internal=true; } *boolean=result.boolean; break;
            case qfmpVoid: break;
        }
    } else {
        clearMemory();
        internal=true;
        type=result.type;
        switch (result.type) {
            case qfmpDouble: num=new double; *num=result.num; break;
            case qfmpDoubleVector: numVec=new QVector<double>; *numVec=result.numVec; break;
            case qfmpString: str=new QString; *str=result.str; break;
            case qfmpBool: boolean=new bool; *boolean=result.boolean; break;
            case qfmpVoid: break;
        }
    }
}




QStringList QFMathParser::getLastErrors() const
{
    return lastError;
}

int QFMathParser::getLastErrorCount()
{
    return errors;
}

void QFMathParser::resetErrors()
{
    lastError.clear();
    errors=0;
}


QString QFMathParser::getLastError() const
{
    if (lastError.isEmpty()) return QString();
    return lastError.last();
}

QString QFMathParser::getFirstError() const
{
    if (lastError.isEmpty()) return QString();
    return lastError.first();
}

QFMathParser::qfmpFunctionDescriptor::qfmpFunctionDescriptor()
{
    name="";
    type=QFMathParser::functionC;
    functionNode=NULL;
    parameterNames.clear();
}

void QFMathParser::qfmpFunctionDescriptor::clearMemory()
{
    if (type==QFMathParser::functionNode && functionNode) {
        delete functionNode;
        functionNode=NULL;
    }
}

qfmpResult QFMathParser::qfmpFunctionDescriptor::evaluate(const QVector<qfmpResult> &parameters, QFMathParser* parent) const
{
    qfmpResult r;
    r.isValid=true;
    if (type==QFMathParser::functionC) {
        return function(parameters.data(), parameters.size(), parent);
    }
    if (type==QFMathParser::functionNode) {
        if (parameterNames.size()!=parameters.size()) {
            parent->qfmpError(QObject::tr("function '%1' takes %2 parameters, but %3 parameters were given").arg(name).arg(parameterNames.size()).arg(parameters.size()));
        } else {
            if (parent && parameterNames.size()>0) {
                //qDebug()<<"  enter block ";
                parent->enterBlock();
                for (int i=0; i<parameterNames.size(); i++) {
                    parent->addVariable(parameterNames[i], parameters[i]);
                    //qDebug()<<"  adding "<<parameterNames[i]<<"="<<parameters[i].toString();
                }
                r=functionNode->evaluate();
                parent->leaveBlock();
                //qDebug()<<"  after block x="<<parent->getVariable("x").toTypeString();
                return r;
            } else {
                r=functionNode->evaluate();
                return r;
            }
        }
    }
    r.isValid=false;
    return r;
}

QString QFMathParser::qfmpFunctionDescriptor::toDefString() const
{
    QString res="";
    if (type==QFMathParser::functionNode) res=QString("%1(%2): node").arg(name).arg(parameterNames.join(", "));
    if (type==QFMathParser::functionC) res=QString("%1(?): C function").arg(name);
    return res;
}


QFMathParser::executionEnvironment::executionEnvironment(QFMathParser *parent)
{
    //qDebug()<<"executionEnvironment constructed parent="<<parent;
    currentLevel=0;
    this->parent=parent;
}

QFMathParser::executionEnvironment::~executionEnvironment()
{
    clear();
}

void QFMathParser::executionEnvironment::setParent(QFMathParser *parent)
{
    this->parent=parent;
}

void QFMathParser::executionEnvironment::enterBlock()
{
    currentLevel++;
    //qDebug()<<"**ENTER_BLOCK "<<currentLevel;
}

void QFMathParser::executionEnvironment::leaveBlock()
{
    if (currentLevel>0) {
        currentLevel--;
        QStringList keys=variables.keys();
        for (int i=0; i<keys.size(); i++) {
            while (!(variables[keys[i]].isEmpty())&&variables[keys[i]].last().first>currentLevel) {
                variables[keys[i]].last().second.clearMemory();
                variables[keys[i]].removeLast();
            }
            //if (keys[i]=="x") //qDebug()<<"**LEAVE_BLOCK "<<currentLevel+1<<": var:"<<keys[i]<<"   levels="<<variables[keys[i]].size();
            if (variables[keys[i]].isEmpty()) variables.remove(keys[i]);
        }

        keys=functions.keys();
        for (int i=0; i<keys.size(); i++) {
            while ((!functions[keys[i]].isEmpty()) && functions[keys[i]].last().first>currentLevel) {
                functions[keys[i]].last().second.clearMemory();
                functions[keys[i]].removeLast();
            }
            if (functions[keys[i]].isEmpty()) functions.remove(keys[i]);
        }
    } else {
        parent->qfmpError(QObject::tr("cannot leave toplevel block!"));
    }
 }

void QFMathParser::executionEnvironment::clear()
{
    clearVariables();
    clearFunctions();
    currentLevel=0;
}



void QFMathParser::executionEnvironment::addVariable(const QString &name, const QFMathParser::qfmpVariable &variable)
{
    //qDebug()<<"addVariable("<<name<<")";
    if (variables.contains(name) && variables[name].size()>0) {
        if (variables[name].last().first==currentLevel) {
            variables[name].last().second.clearMemory();
            int l=variables[name].last().first;
            variables[name].removeLast();
            variables[name].append(qMakePair(l, variable));
        } else {
            variables[name].append(qMakePair(currentLevel, variable));
        }
    } else {
        QList<QPair<int, qfmpVariable> > l;
        l.append(qMakePair(currentLevel, variable));
        variables[name]=l;
    }
    //if (name=="x") //qDebug()<<"**ADDED_VARIABLE "<<currentLevel<<": var:"<<name<<"   levels="<<variables[name].size();
}

void QFMathParser::executionEnvironment::setVariableDouble(const QString &name, double result)
{
    //qDebug()<<"executionEnvironment::setVariableDouble("<<name<<result<<")";
    qfmpResult r;
    r.setDouble(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableDoubleVec(const QString &name, const QVector<double> &result)
{
    qfmpResult r;
    r.setDoubleVec(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableString(const QString &name, const QString &result)
{
    qfmpResult r;
    r.setString(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableBoolean(const QString &name, bool result)
{
    qfmpResult r;
    r.setBoolean(result);
    setVariable(name, r);

}

void QFMathParser::executionEnvironment::addVariableDouble(const QString &name, double result)
{
    //qDebug()<<"executionEnvironment::addVariableDouble("<<name<<result<<")";
    qfmpResult r;
    r.setDouble(result);
    //qDebug()<<"  adding "<<r.toTypeString();
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableDoubleVec(const QString &name, const QVector<double> &result)
{
    qfmpResult r;
    r.setDoubleVec(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableString(const QString &name, const QString &result)
{
    qfmpResult r;
    r.setString(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableBoolean(const QString &name, bool result)
{
    qfmpResult r;
    r.setBoolean(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::deleteVariable(const QString &name)
{
    if (variables.contains(name)) {
        for (int i=0; i<variables[name].size(); i++) {
            qfmpVariable v=variables[name].at(i).second;
            v.clearMemory();
        }
    }
    variables.remove(name);
}

QString QFMathParser::executionEnvironment::printVariables() const
{
    QString res="";

    if (variables.size()>0) {

        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpVariable> > > itV(variables);
        while (itV.hasNext()) {
            itV.next();

            qfmpVariable v=itV.value().last().second;
            res+="'"+itV.key()+"'"+"\t\t";
            if (v.isInternal()) res+="intern"; else res+="extern";
            res+="\t"+v.toResult().toTypeString();
            res+="\n";

        }
    }
    return res;
}

QString QFMathParser::executionEnvironment::printFunctions() const
{
    QString res="";

    if (functions.size()>0) {

        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpFunctionDescriptor> > > itV(functions);
        while (itV.hasNext()) {
            itV.next();
            if (res.size()>0) res+="\n";

            res+=itV.value().last().second.toDefString();

            /*qfmpFunctionDescriptor v=itV.value().last().second;
            if (v.type==QFMathParser::functionNode) res+=QString("%1(%2): node\n").arg(itV.key()).arg(v.parameterNames.join(", "));
            if (v.type==QFMathParser::functionC) res+=QString("%1(?): C function").arg(itV.key());*/
        }
    }
    return res;
}

QList<QPair<QString, QFMathParser::qfmpVariable> > QFMathParser::executionEnvironment::getVariables() const
{
    QList<QPair<QString, QFMathParser::qfmpVariable> > res;

    if (variables.size()>0) {
        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpVariable> > > itV(variables);
        while (itV.hasNext()) {
            itV.next();
            res.append(qMakePair(itV.key(), itV.value().last().second));
        }
    }
    return res;
}

QList<QPair<QString, QFMathParser::qfmpFunctionDescriptor> > QFMathParser::executionEnvironment::getFunctions() const
{
    QList<QPair<QString, QFMathParser::qfmpFunctionDescriptor> > res;

    if (functions.size()>0) {
        QMapIterator<QString, QList<QPair<int, QFMathParser::qfmpFunctionDescriptor> > > itV(functions);
        while (itV.hasNext()) {
            itV.next();
            res.append(qMakePair(itV.key(), itV.value().last().second));
        }
    }
    return res;
}

void QFMathParser::executionEnvironment::setFunction(const QString &name, const QFMathParser::qfmpFunctionDescriptor &function)
{
    if (functions.contains(name) && functions[name].size()>0) {
        if (functions[name].last().first==currentLevel) {
            functions[name].last().second.clearMemory();
            int l=functions[name].last().first;
            functions[name].removeLast();
            functions[name].append(qMakePair(l, function));
        } else {
            functions[name].append(qMakePair(currentLevel, function));
        }
    } else {
        QList<QPair<int, qfmpFunctionDescriptor> > l;
        l.append(qMakePair(currentLevel, function));
        functions[name]=l;
    }
}

void QFMathParser::executionEnvironment::addFunction(const QString &name, QStringList parameterNames, QFMathParser::qfmpNode *function)
{
    QFMathParser::qfmpFunctionDescriptor fd;
    fd.type=functionNode;
    fd.parameterNames=parameterNames;
    fd.functionNode=function;
    fd.name=name;
    setFunction(name, fd);
}

qfmpResult QFMathParser::executionEnvironment::getVariable(const QString &name) const
{
    qfmpResult res;
    res.isValid=false;
    if (variables.contains(name) && variables[name].size()>0) {
        res=variables[name].last().second.toResult();
        res.isValid=true;
        return res;
    }
    if (parent) parent->qfmpError(QObject::tr("the variable '%1' does not exist").arg(name));
    return res;
}

qfmpResult QFMathParser::executionEnvironment::evaluateFunction(const QString &name, const QVector<qfmpResult> &parameters) const
{
    qfmpResult res;
    res.isValid=false;
    //qDebug()<<"searching function "<<name<<": "<<functions.contains(name);
    //if (functions.contains(name)) //qDebug()<<"   defs: "<<functions[name].size();
    if (functions.contains(name) && functions[name].size()>0) {
        res=functions[name].last().second.evaluate(parameters, parent);
        res.isValid=true;
        return res;
    }
    if (parent) parent->qfmpError(QObject::tr("the function '%1' does not exist").arg(name));
    return res;
}

void QFMathParser::executionEnvironment::addVariable(const QString &name, const qfmpResult &result)
{
    //qDebug()<<"executionEnvironment::addVariable("<<name<<result.toTypeString()<<")";
    bool add=true;
    if (variables.contains(name)) {
        //qDebug()<<"   contains "<<name;
        if (variables[name].size()>0) {
            //qDebug()<<"   contains: size="<<variables[name].size();
            if (variables[name].last().first==currentLevel) {
                //qDebug()<<"   contains: level="<<variables[name].last().first;
                variables[name].last().second.set(result);
                add=false;
                return;
            }
        }
    }
    if (add) {
        //qDebug()<<"   addidng 0";
        QFMathParser::qfmpVariable v;
        v.set(result);
        //qDebug()<<"   addidng 1";

        /*QList<QPair<int, qfmpVariable> > l;
        if (variables.contains(name)) {
            l=variables[name];
        }
        l.append(qMakePair(currentLevel, v));
        //qDebug()<<"   addidng 2";
        variables[name]=l;
        //qDebug()<<"   addidng 3";*/
        variables[name].append(qMakePair(currentLevel, v));
    }
    //if (name=="x") //qDebug()<<"**ADD_VARIABLE "<<currentLevel<<": var:"<<name<<"   levels="<<variables[name].size();
}

void QFMathParser::executionEnvironment::setVariable(const QString &name, const qfmpResult &result)
{
    if (variables.contains(name) && variables[name].size()>0) {
        variables[name].last().second.set(result);
    } else {
        QFMathParser::qfmpVariable v;
        v.set(result);
        QList<QPair<int, qfmpVariable> > l;
        l.append(qMakePair(currentLevel, v));
        variables[name]=l;
    }
    //if (name=="x") //qDebug()<<"**SET_VARIABLE "<<currentLevel<<": var:"<<name<<"   levels="<<variables[name].size();

}

void QFMathParser::executionEnvironment::clearVariables()
{
    variables.clear();
}

void QFMathParser::executionEnvironment::clearFunctions()
{
    functions.clear();
}

QFMathParser::qfmpFunctionAssignNode::~qfmpFunctionAssignNode()
{
    if (child) delete child;
}

QFMathParser::qfmpFunctionAssignNode::qfmpFunctionAssignNode(QString function, QStringList parameterNames, QFMathParser::qfmpNode *c, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    child=c;
    if (child) child->setParent(this);
    setParser(p);
    setParent(par);
    this->function=function;
    this->parameterNames=parameterNames;
}

qfmpResult QFMathParser::qfmpFunctionAssignNode::evaluate()
{
    getParser()->addFunction(function, parameterNames, child->copy(NULL));
    return qfmpResult::voidResult();
}

QFMathParser::qfmpNode *QFMathParser::qfmpFunctionAssignNode::copy(QFMathParser::qfmpNode *par)
{
    if (child) return new QFMathParser::qfmpFunctionAssignNode(function, parameterNames, child->copy(NULL), getParser(), par);
    else return new QFMathParser::qfmpFunctionAssignNode(function, parameterNames, NULL, getParser(), par);
}

QFMathParser::qfmpNode *QFMathParser::qfmpConstantNode::copy(QFMathParser::qfmpNode *par)
{
    return new QFMathParser::qfmpConstantNode(data, getParser(), par);
}

QFMathParser::qfmpNode *QFMathParser::qfmpInvalidNode::copy(QFMathParser::qfmpNode *par)
{
    return new QFMathParser::qfmpInvalidNode(getParser(), par);
}

qfmpResult QFMathParser::qfmpVectorList::evaluate()
{
    qfmpResult res, r;
    res.type=qfmpDoubleVector;
    res.numVec.clear();
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            r=list[i]->evaluate();
            if (r.isValid && r.type==qfmpDouble) {
                res.numVec.append(r.num);
            } else if (r.isValid && r.type==qfmpDoubleVector) {
                res.numVec+=r.numVec;
            } else {
                res.isValid=false;
                if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct [Val1, Val2, ...]: item %1 has the wrong type (not number or number vector!)").arg(i));
                break;
            }
        }
    }
    return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorList::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpNodeList* n= new QFMathParser::qfmpVectorList(getParser(), par);
    if (list.size()>0) {
        for (int i=0; i<list.size(); i++) {
            n->add(list[i]->copy(n));
        }
    }

    n->setParent(par);
    return n;

}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorConstructionNode::copy(QFMathParser::qfmpNode *par)
{
    if (step) {
        return new qfmpVectorConstructionNode(start->copy(NULL), step->copy(NULL), end->copy(NULL), getParser(), par);
    } else {
        return new qfmpVectorConstructionNode(start->copy(NULL), NULL, end->copy(NULL), getParser(), par);
    }
}

QFMathParser::qfmpVectorConstructionNode::qfmpVectorConstructionNode(QFMathParser::qfmpNode *start, QFMathParser::qfmpNode *end, QFMathParser::qfmpNode *step, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    this->start=start;
    this->end=end;
    this->step=step;
    setParser(p);
    setParent(par);
}

QFMathParser::qfmpVectorConstructionNode::~qfmpVectorConstructionNode()
{
    if (start) delete start;
    if (step) delete step;
    if (end) delete end;
}

qfmpResult QFMathParser::qfmpVectorConstructionNode::evaluate()
{
    qfmpResult res;
    res.isValid=false;
    res.type=qfmpDoubleVector;
    qfmpResult rstart=start->evaluate();
    qfmpResult rend=end->evaluate();
    if (rstart.type!=qfmpDouble || rend.type!=qfmpDouble) {
        if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct 'start[:delta]:end'' start, delta and end have to be numbers (start=%1, end=%2)").arg(rstart.toTypeString()).arg(rend.toTypeString()));
        return res;
    }
    double s=rstart.num;
    double e=rend.num;
    double d=1;
    if (step) {
        qfmpResult rstep=step->evaluate();
        if (rstep.type!=qfmpDouble) {
            if (getParser()) getParser()->qfmpError(QObject::tr("error in vector construct 'start:delta:end'' delta has to be numbers (start=%1, step=%2, end=%3)").arg(rstart.toTypeString()).arg(rstep.toTypeString()).arg(rend.toTypeString()));
            return res;
        }
        d=rstep.num;
    }
    res.isValid=true;
    if (d>0) {
        for (double t=s; t<=e; t=t+d) {
            res.numVec<<t;
        }
    } else if (d<0) {
        for (double t=s; t>=e; t=t+d) {
            res.numVec<<t;
        }
    }
    return res;
}

QFMathParser::qfmpCasesNode::qfmpCasesNode(QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    setParent(par);
    setParser(p);
    elseNode=NULL;
}

QFMathParser::qfmpCasesNode::~qfmpCasesNode()
{
    if (elseNode) delete elseNode;
    for (int i=0; i<casesNodes.size(); i++) {
        if (casesNodes[i].first) delete casesNodes[i].first;
        if (casesNodes[i].second) delete casesNodes[i].second;
    }
    casesNodes.clear();
}

void QFMathParser::qfmpCasesNode::setElse(QFMathParser::qfmpNode *elseNode)
{
    this->elseNode=elseNode;
}

void QFMathParser::qfmpCasesNode::addCase(QFMathParser::qfmpNode *decision, QFMathParser::qfmpNode *value)
{
    casesNodes.append(qMakePair(decision, value));
}

qfmpResult QFMathParser::qfmpCasesNode::evaluate()
{
    qfmpResult r;
    r.isValid=false;
    for (int i=0; i<casesNodes.size(); i++) {
        qfmpResult d=casesNodes[i].first->evaluate();
        if (d.type==qfmpBool) {
            if (d.boolean) return casesNodes[i].second->evaluate();
        } else {
            if (getParser()) getParser()->qfmpError(QObject::tr("decision statement %1 does not have type boolean").arg(i+1));
            return r;
        }
    }
    if (elseNode) r=elseNode->evaluate();
    return r;
}

QFMathParser::qfmpNode *QFMathParser::qfmpCasesNode::copy(QFMathParser::qfmpNode *par)
{
    QFMathParser::qfmpCasesNode* res=new QFMathParser::qfmpCasesNode(getParser(), par);
    for (int i=0; i<casesNodes.size(); i++) {
        QFMathParser::qfmpNode* n1=NULL;
        QFMathParser::qfmpNode* n2=NULL;
        if (casesNodes[i].first) n1=casesNodes[i].first->copy();
        if (casesNodes[i].second) n2=casesNodes[i].second->copy();
        res->addCase(n1, n2);
    }
    if (elseNode) res->setElse(elseNode->copy(res));
    return res;
}

QFMathParser::qfmpVectorOperationNode::qfmpVectorOperationNode(const QString &operationName, const QString &variableName, QFMathParser::qfmpNode *items, QFMathParser::qfmpNode *expression, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    this->operationName=operationName.toLower();
    this->variableName=variableName;
    this->expression=expression;
    this->items=items;
    this->start=NULL;
    this->end=NULL;
    this->delta=NULL;
}

QFMathParser::qfmpVectorOperationNode::qfmpVectorOperationNode(const QString &operationName, const QString &variableName, QFMathParser::qfmpNode *start, QFMathParser::qfmpNode *end, QFMathParser::qfmpNode *delta, QFMathParser::qfmpNode *expression, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)

{
    this->operationName=operationName.toLower();
    this->variableName=variableName;
    this->expression=expression;
    this->items=NULL;
    this->start=start;
    this->end=end;
    this->delta=delta;
}

QFMathParser::qfmpVectorOperationNode::~qfmpVectorOperationNode()
{
    if (expression) delete expression;
    if (items) delete items;
    if (start) delete start;
    if (end) delete end;
    if (delta) delete delta;
}

qfmpResult QFMathParser::qfmpVectorOperationNode::evaluate()
{
    qfmpResult r;
    r.isValid=false;
    QStringList strVec;
    QVector<double> numVec, itemVals;
    qfmpResultType resType=qfmpDouble;
    if (items) {
        qfmpResult ri=items->evaluate();
        if (ri.type==qfmpDouble) itemVals<<ri.num;
        else if (ri.type==qfmpDoubleVector) itemVals=ri.numVec;
        else {
            if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, ITEMS, EXPRESSION) expects a list of numbers as ITEMS").arg(operationName));
            return r;
        }
    } else {
        qfmpResult rs=start->evaluate();
        qfmpResult re=end->evaluate();
        double s=rs.num;
        double e=re.num;
        if (rs.type!=qfmpDouble) {
            if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, START, DELTA, END, EXPRESSION) expects a number for START").arg(operationName));
            return r;
        }
        if (re.type!=qfmpDouble) {
            if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, START, DELTA, END, EXPRESSION) expects a number for END").arg(operationName));
            return r;
        }
        double d=1;
        if (delta) {
            qfmpResult rd=delta->evaluate();
            if (rd.type==qfmpDouble) {
                d=rd.num;
            } else {
                if (getParser()) getParser()->qfmpError(QObject::tr("%1(NAME, START, DELTA, END, EXPRESSION) expects a number for DELTA").arg(operationName));
                return r;
            }

        }
        //qDebug()<<"for: t="<<s<<"; t<="<<e<<"; t=t+"<<d;
        for (double t=s; t<=e; t=t+d) {
            itemVals<<t;
        }
    }
    if (itemVals.size()<=0) {
        if (operationName=="for") {
            return qfmpResult(QVector<double>());
        } else {
            return qfmpResult(0.0);
        }
    }
    getParser()->enterBlock();
    for (int i=0; i<itemVals.size(); i++) {
        getParser()->addVariable(variableName, qfmpResult(itemVals[i]));
        qfmpResult thisr=expression->evaluate();
        if (i==0) {
            resType=thisr.type;
        } else if ((resType==qfmpString && resType!=thisr.type) || ((resType==qfmpDouble || resType==qfmpDoubleVector)&&(thisr.type!=qfmpDouble && thisr.type!=qfmpDoubleVector)) ) {
            if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to the same type in every iteration: expected %2, but found %3 in iteration %4").arg(operationName).arg(thisr.toTypeString()).arg(resultTypeToString(resType)).arg(i+1));
            return r;
        }
        switch(resType) {
            case qfmpDouble:
                numVec<<thisr.num;
                break;
            case qfmpDoubleVector:
                numVec+=thisr.num;
                break;
            case qfmpString:
                if (operationName=="sum") {
                    strVec<<thisr.str;
                } else {
                    if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to number: but found %2 in iteration %3").arg(operationName).arg(resultTypeToString(resType)).arg(i+1));
                    return r;
                }
                break;
            default:
                if (getParser()) getParser()->qfmpError(QObject::tr("EXPRESSION in %1(NAME, ..., EXPRESSION) has to evaluate to string or number: but found %2 in iteration %3").arg(operationName).arg(resultTypeToString(resType)).arg(i+1));
                return r;
        }
    }
    getParser()->leaveBlock();

    r.isValid=true;
    if (operationName=="for") {
        return qfmpResult(numVec);
    } else if (operationName=="sum") {
        if (resType==qfmpString) {
            return qfmpResult(strVec.join(""));
        } else {
            return qfmpResult(qfstatisticsSum(numVec));
        }
    } else if (operationName=="prod") {
        return qfmpResult(qfstatisticsProd(numVec));
    }

    return r;
}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorOperationNode::copy(QFMathParser::qfmpNode *par)
{
    if (items) {
        return new qfmpVectorOperationNode(operationName, variableName, items->copy(), expression->copy(), getParser(), par);
    } else {
        if (delta) return new qfmpVectorOperationNode(operationName, variableName, start->copy(), end->copy(), delta->copy(), expression->copy(), getParser(), par);
        else return new qfmpVectorOperationNode(operationName, variableName, start->copy(), end->copy(), NULL, expression->copy(), getParser(), par);
    }
}

QFMathParser::qfmpVectorElementAssignNode::~qfmpVectorElementAssignNode()
{
    if (index) delete index;
}

QFMathParser::qfmpVectorElementAssignNode::qfmpVectorElementAssignNode(QString var, QFMathParser::qfmpNode *index, QFMathParser::qfmpNode *expression, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpVariableAssignNode(var, expression, p, par)
{
    this->index=index;
}

qfmpResult QFMathParser::qfmpVectorElementAssignNode::evaluate()
{
    qfmpResult res, exp, idx, var;
    res.isValid=true;
    if (child) exp=child->evaluate();
    if (index) idx=index->evaluate();

    var=getParser()->getVariable(variable);
    if (idx.convertsToIntVector()) {
        QVector<int> ii=idx.asIntVector();
        if (ii.size()==0) {
            getParser()->qfmpError(QObject::tr("vector element assignment needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
            return getParser()->getInvalidResult();
        }
        if (var.convertsToVector() && exp.convertsToVector()) {
            QVector<double> dat=exp.asVector();
            if (ii.size()==1 && dat.size()==1) {
                if (ii[0]>=0 && ii[0]<var.numVec.size()) {
                    var.numVec[ii[0]]=dat[0];
                    getParser()->setVariable(variable, var);
                    res=qfmpResult(dat[0]);
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[0]).arg(variable).arg(var.numVec.size()));
                    return getParser()->getInvalidResult();
                }
            } else if (ii.size()>1 && dat.size()==1) {
                res.type=qfmpDoubleVector;
                res.numVec.clear();
                const double t=dat[0];
                for (int i=0; i<ii.size(); i++) {
                    if (ii[i]>=0 && ii[i]<var.numVec.size()) {
                        var.numVec[ii[i]]=t;
                        res.numVec<<t;
                    } else {
                        getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.numVec.size()));
                        return getParser()->getInvalidResult();
                    }
                }
                getParser()->setVariable(variable, var);
            } else if (ii.size()>1 && dat.size()==ii.size()) {
                res.type=qfmpDoubleVector;
                res.numVec.clear();
                for (int i=0; i<ii.size(); i++) {
                    if (ii[i]>=0 && ii[i]<var.numVec.size()) {
                        const double t=dat[i];
                        var.numVec[ii[i]]=t;
                        res.numVec<<t;
                    } else {
                        getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.numVec.size()));
                        return getParser()->getInvalidResult();
                    }
                }
                getParser()->setVariable(variable, var);
            } else {
                getParser()->qfmpError(QObject::tr("can only assign x[N elements]=(1 element), x[N elements]=(N elements), x[1 elements]=(1 elements)"));
                return getParser()->getInvalidResult();
            }
        } else if (exp.type==qfmpString && var.type==qfmpString){
            QString dat=exp.asString();
            if (ii.size()==1 && dat.size()==1) {
                if (ii[0]>=0 && ii[0]<var.str.size()) {
                    var.str[ii[0]]=dat[0];
                    getParser()->setVariable(variable, var);
                    res=qfmpResult(QString(dat[0]));
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but string %2 has only %3 elements").arg(ii[0]).arg(variable).arg(var.str.size()));
                    return getParser()->getInvalidResult();
                }
            } else if (ii.size()>1 && dat.size()==1) {
                res.type=qfmpString;
                res.str.clear();
                QChar t=dat[0];
                for (int i=0; i<ii.size(); i++) {
                    if (ii[i]>=0 && ii[i]<var.str.size()) {
                        var.str[ii[i]]=t;
                        res.str+=t;
                    } else {
                        getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but string %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.str.size()));
                        return getParser()->getInvalidResult();
                    }
                }
                getParser()->setVariable(variable, var);
            } else if (ii.size()>1 && dat.size()==ii.size()) {
                res.type=qfmpString;
                res.str.clear();
                for (int i=0; i<ii.size(); i++) {
                    if (ii[i]>=0 && ii[i]<var.str.size()) {
                        QChar t=dat[i];
                        var.str[ii[i]]=t;
                        res.str+=t;
                    } else {
                        getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to assign to element %1, but string %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.str.size()));
                        return getParser()->getInvalidResult();
                    }
                }
                getParser()->setVariable(variable, var);
            } else {
                getParser()->qfmpError(QObject::tr("can only assign x[N elements]=(1 element), x[N elements]=(N elements), x[1 elements]=(1 elements)"));
                return getParser()->getInvalidResult();
            }
        } else {
            getParser()->qfmpError(QObject::tr("vector element assignment needs an expression which evaluates to the same type as the variable (var: %1, expression: %2)").arg(resultTypeToString(var.type)).arg(exp.toTypeString()));
            return getParser()->getInvalidResult();
        }
    } else {
        getParser()->qfmpError(QObject::tr("vector element assignment needs number or number vector as index, but index is %1").arg(resultTypeToString(idx.type)));
        return getParser()->getInvalidResult();
    }

    return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorElementAssignNode::copy(QFMathParser::qfmpNode *par)
{
    return new QFMathParser::qfmpVectorElementAssignNode(variable, index->copy(NULL), child->copy(NULL), getParser(), par);
}

QFMathParser::qfmpVectorAccessNode::~qfmpVectorAccessNode()
{
    if (index) delete index;
}

QFMathParser::qfmpVectorAccessNode::qfmpVectorAccessNode(QString var, QFMathParser::qfmpNode *index, QFMathParser *p, QFMathParser::qfmpNode *par):
    qfmpNode(p, par)
{
    variable=var;
    this->index=index;
}

qfmpResult QFMathParser::qfmpVectorAccessNode::evaluate()
{
    qfmpResult res, idx, var;
    res.isValid=true;
    if (index) idx=index->evaluate();

    var=getParser()->getVariable(variable);
    if (var.type==qfmpDoubleVector) {
        if (idx.convertsToIntVector()) {
            QVector<int> ii=idx.asIntVector();
            if (ii.size()==0) {
                getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                return getParser()->getInvalidResult();
            }
            res.type=qfmpDoubleVector;
            res.numVec=QVector<double>(ii.size(), 0);
            for (int i=0; i<ii.size(); i++) {
                if (ii[i]>=0 && ii[i]<var.numVec.size()) {
                    res.numVec[i]=var.numVec[ii[i]];
                } else {
                    getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but vector %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.numVec.size()));
                    return getParser()->getInvalidResult();
                }
            }
            if (res.numVec.size()==1) return qfmpResult(res.numVec[0]);

        } else {
            getParser()->qfmpError(QObject::tr("vector element access needs number or number vector as index, but index is %1").arg(resultTypeToString(idx.type)));
            return getParser()->getInvalidResult();
        }
    } else if (var.type==qfmpString) {
            if (idx.convertsToIntVector()) {
                QVector<int> ii=idx.asIntVector();
                if (ii.size()==0) {
                    getParser()->qfmpError(QObject::tr("vector element access needs non-empty number vector as index, but index is %1").arg(idx.toTypeString()));
                    return getParser()->getInvalidResult();
                }
                res.type=qfmpString;
                res.str="";
                for (int i=0; i<ii.size(); i++) {
                    if (ii[i]>=0 && ii[i]<var.str.size()) {
                        res.str+=var.str[ii[i]];
                    } else {
                        getParser()->qfmpError(QObject::tr("OUT OF RANGE: trying to access element %1, but string %2 has only %3 elements").arg(ii[i]).arg(variable).arg(var.str.size()));
                        return getParser()->getInvalidResult();
                    }
                }

            } else {
                getParser()->qfmpError(QObject::tr("string character access needs number or number vector as index, but index is %1").arg(resultTypeToString(idx.type)));
                return getParser()->getInvalidResult();
            }
    } else {
        getParser()->qfmpError(QObject::tr("vector element/string character access is only possible if the variable is a number vector, but variable '%1' is of type %2").arg(variable).arg(resultTypeToString(var.type)));
        return getParser()->getInvalidResult();
    }

    return res;
}

QFMathParser::qfmpNode *QFMathParser::qfmpVectorAccessNode::copy(QFMathParser::qfmpNode *par)
{
    return new qfmpVectorAccessNode(variable, index->copy(NULL), getParser(), par);
}


