#include "qfmathparser.h"

#include "qfmathparserdefaultlib.h"

bool QFMathParser_DefaultLib::hasDefaultVariables(QFMathParser* p)
{
    return p->functionExists("pi")
            && p->functionExists("e")
            && p->functionExists("log10e")
            && p->functionExists("hbar")
            && p->functionExists("muB");
}

void QFMathParser_DefaultLib::addDefaultVariables(QFMathParser* p)
{

    p->addVariableString("stdlib_version", QString("0.3 (p%1)").arg(QFMATHPARSER_VERSION));


    p->addVariableDouble("pi", M_PI);
    p->addVariableDouble("e", M_E);
    p->addVariableDouble("sqrt2", sqrt(2));
    p->addVariableDouble("log2e", M_LOG2E);
    p->addVariableDouble("log10e", M_LOG10E);
    p->addVariableDouble("ln2", M_LN2);
    p->addVariableDouble("ln10", M_LN10);
    p->addVariableDouble("h", 6.6260689633E-34);
    p->addVariableDouble("hbar", 1.05457162853E-34);
    p->addVariableDouble("epsilon0", 8.854187817E-12);
    p->addVariableDouble("mu0", 12.566370614E-7);
    p->addVariableDouble("c", 299792458);
    p->addVariableDouble("ce", 1.60217648740E-19);
    p->addVariableDouble("muB", 927.40091523E-26);
    p->addVariableDouble("muB_eV", 5.788381755579E-5);
    p->addVariableDouble("muN", 5.0507832413E-27);
    p->addVariableDouble("muN_eV", 3.152451232645E-8);
    p->addVariableDouble("me", 9.1093821545E-31);
    p->addVariableDouble("mp", 1.67262163783E-27);
    p->addVariableDouble("mn", 1.67492721184E-27);
    p->addVariableDouble("NA", QF_NAVOGADRO);
    p->addVariableDouble("kB", QF_K_BOLTZ);
    p->addVariableDouble("kB_eV", 8.61734315E-5);
}

bool QFMathParser_DefaultLib::hasDefaultFunctions(QFMathParser* p)
{
    return p->functionExists("sinc")
            && p->functionExists("erf")
            && p->functionExists("polyval")
            && p->functionExists("randvec")
            && p->functionExists("bool2str");
}


void QFMathParser_DefaultLib::addDefaultFunctions(QFMathParser* p)
{
    p->addFunction("sinc", QFMathParser_DefaultLib::fSinc, NULL, qfSinc);
    p->addFunction("asin", QFMathParser_DefaultLib::fASin, NULL, asin);
    p->addFunction("acos", QFMathParser_DefaultLib::fACos, NULL, acos);
    p->addFunction("atan", QFMathParser_DefaultLib::fATan, NULL, atan);
    p->addFunction("atan2", QFMathParser_DefaultLib::fATan2, NULL, NULL, atan2);
    p->addFunction("sin", QFMathParser_DefaultLib::fSin, NULL, sin);
    p->addFunction("cos", QFMathParser_DefaultLib::fCos, NULL, cos);
    p->addFunction("tan", QFMathParser_DefaultLib::fTan, NULL, tan);
    p->addFunction("sinh", QFMathParser_DefaultLib::fSinh, NULL, sinh);
    p->addFunction("cosh", QFMathParser_DefaultLib::fCosh, NULL, cosh);
    p->addFunction("tanh", QFMathParser_DefaultLib::fTanh, NULL, tanh);
    p->addFunction("asinh", QFMathParser_DefaultLib::fASinh, NULL, sinh);
    p->addFunction("acosh", QFMathParser_DefaultLib::fACosh, NULL, cosh);
    p->addFunction("atanh", QFMathParser_DefaultLib::fATanh, NULL, tanh);
    p->addFunction("log", QFMathParser_DefaultLib::fLog, NULL, log);
    p->addFunction("log2", QFMathParser_DefaultLib::fLog2, NULL, log2);
    p->addFunction("log10", QFMathParser_DefaultLib::fLog10, NULL, log10);
    p->addFunction("exp", QFMathParser_DefaultLib::fExp, NULL, exp);
    p->addFunction("sqrt", QFMathParser_DefaultLib::fSqrt, NULL, sqrt);
    p->addFunction("cbrt", QFMathParser_DefaultLib::fCbrt, NULL, cbrt);
    p->addFunction("sqr", QFMathParser_DefaultLib::fSqr, NULL, qfSqr);
    p->addFunction("abs", QFMathParser_DefaultLib::fAbs, NULL, fabs);
    p->addFunction("erf", QFMathParser_DefaultLib::fErf, NULL, erf);
    p->addFunction("erfc", QFMathParser_DefaultLib::fErfc, NULL, erfc);
    p->addFunction("lgamma", QFMathParser_DefaultLib::flGamma, NULL, lgamma);
    p->addFunction("tgamma", QFMathParser_DefaultLib::ftGamma, NULL, tgamma);
    p->addFunction("j0", QFMathParser_DefaultLib::fJ0, NULL, j0);
    p->addFunction("j1", QFMathParser_DefaultLib::fJ1, NULL, j1);
    p->addFunction("jn", QFMathParser_DefaultLib::fJn, NULL, NULL, QFMathParser_DefaultLib::fJn);
    p->addFunction("y0", QFMathParser_DefaultLib::fY0, NULL, y0);
    p->addFunction("y1", QFMathParser_DefaultLib::fY1, NULL, y1);
    p->addFunction("yn", QFMathParser_DefaultLib::fYn, NULL, NULL, QFMathParser_DefaultLib::fYn);
    p->addFunction("rand", QFMathParser_DefaultLib::fRand, QFMathParser_DefaultLib::fRand, QFMathParser_DefaultLib::fRand, QFMathParser_DefaultLib::fRand);
    p->addFunction("randint", QFMathParser_DefaultLib::fRandInt, QFMathParser_DefaultLib::fRandInt, QFMathParser_DefaultLib::fRandInt, QFMathParser_DefaultLib::fRandInt);
    p->addFunction("randnorm", QFMathParser_DefaultLib::fRandNorm, QFMathParser_DefaultLib::fRandNorm, QFMathParser_DefaultLib::fRandNorm, QFMathParser_DefaultLib::fRandNorm);
    p->addFunction("randbool", QFMathParser_DefaultLib::fRandBool);
    p->addFunction("randvec", QFMathParser_DefaultLib::fRandVec);
    p->addFunction("randintvec", QFMathParser_DefaultLib::fRandIntVec);
    p->addFunction("randnormvec", QFMathParser_DefaultLib::fRandNormVec);
    p->addFunction("randboolvec", QFMathParser_DefaultLib::fRandBoolVec);
    p->addFunction("srand", QFMathParser_DefaultLib::fSRand, NULL, QFMathParser_DefaultLib::fSRand);
    p->addFunction("ceil", QFMathParser_DefaultLib::fCeil, NULL, ceil);
    p->addFunction("floor", QFMathParser_DefaultLib::fFloor, NULL, floor);
    p->addFunction("trunc", QFMathParser_DefaultLib::fTrunc, NULL, trunc);
    p->addFunction("round", QFMathParser_DefaultLib::fRound, NULL, round);
    p->addFunction("fmod", QFMathParser_DefaultLib::fFMod, NULL, NULL, fmod);
    p->addFunction("min", QFMathParser_DefaultLib::fMin,NULL,NULL,QFMathParser_DefaultLib::fMin);
    p->addFunction("max", QFMathParser_DefaultLib::fMax,NULL,NULL,QFMathParser_DefaultLib::fMax);
    p->addFunction("int2bin", QFMathParser_DefaultLib::fIntToBinStr);
    p->addFunction("int2oct", QFMathParser_DefaultLib::fIntToOctStr);
    p->addFunction("int2hex", QFMathParser_DefaultLib::fIntToHexStr);
    p->addFunction("int2str", QFMathParser_DefaultLib::fIntToStr);
    p->addFunction("num2str", QFMathParser_DefaultLib::fdoubleToQString);
    p->addFunction("bool2str", QFMathParser_DefaultLib::fboolToQString);
    p->addFunction("gaussnn", QFMathParser_DefaultLib::fGauss, NULL, qfGaussSqrtE, qfGaussSqrtE);
    p->addFunction("gauss", QFMathParser_DefaultLib::fGaussDist, NULL, qfGaussNormSqrtE, qfGaussNormSqrtE);
    p->addFunction("slit", QFMathParser_DefaultLib::fSlit, NULL, NULL,qfSlit);
    p->addFunction("theta", QFMathParser_DefaultLib::fTheta, NULL, qfTheta);
    p->addFunction("tanc", QFMathParser_DefaultLib::fTanc, NULL, qfTanc);
    p->addFunction("sigmoid", QFMathParser_DefaultLib::fSigmoid, NULL, qfSigmoid);
    p->addFunction("sign", QFMathParser_DefaultLib::fSign, NULL, qfSign);
    p->addFunction("roundsig", QFMathParser_DefaultLib::fRoundSig, NULL, roundError, roundError);
    p->addFunction("tosystempathseparator", QFMathParser_DefaultLib::fToSystemPathSeparator);
    p->addFunction("trimm", QFMathParser_DefaultLib::fTrimm);
    p->addFunction("tolower", QFMathParser_DefaultLib::fToLower);
    p->addFunction("toupper", QFMathParser_DefaultLib::fToUpper);
    p->addFunction("length", QFMathParser_DefaultLib::fLength);
    p->addFunction("sort", QFMathParser_DefaultLib::fSort);
    p->addFunction("dsort", QFMathParser_DefaultLib::fDSort);
    p->addFunction("count", QFMathParser_DefaultLib::fCount);
    p->addFunction("sum", QFMathParser_DefaultLib::fSum);
    p->addFunction("sum2", QFMathParser_DefaultLib::fSum2);
    p->addFunction("mean", QFMathParser_DefaultLib::fMean);
    p->addFunction("corrcoeff", QFMathParser_DefaultLib::fCorrcoeff);
    p->addFunction("skewness", QFMathParser_DefaultLib::fSkewness);
    p->addFunction("moment", QFMathParser_DefaultLib::fCentralMoment);
    p->addFunction("ncmoment", QFMathParser_DefaultLib::fNonCentralMoment);
    p->addFunction("std", QFMathParser_DefaultLib::fStd);
    p->addFunction("var", QFMathParser_DefaultLib::fVar);
    p->addFunction("median", QFMathParser_DefaultLib::fMedian);
    p->addFunction("quantile", QFMathParser_DefaultLib::fQuantile);
    p->addFunction("remove", QFMathParser_DefaultLib::fRemove);
    p->addFunction("removeall", QFMathParser_DefaultLib::fRemoveAll);
    p->addFunction("shuffle", QFMathParser_DefaultLib::fShuffle);
    p->addFunction("reverse", QFMathParser_DefaultLib::fReverse);
    p->addFunction("find", QFMathParser_DefaultLib::fFind);
    p->addFunction("select", QFMathParser_DefaultLib::fSelect);
    p->addFunction("concat", QFMathParser_DefaultLib::fConcat);
    p->addFunction("deg2rad", QFMathParser_DefaultLib::fDegToRad, NULL, qfDegToRad);
    p->addFunction("rad2deg", QFMathParser_DefaultLib::fRadToDeg, NULL, qfRadToDeg);
    p->addFunction("dot", QFMathParser_DefaultLib::fDot);
    p->addFunction("alltrue", QFMathParser_DefaultLib::fAllTrue);
    p->addFunction("allfalse", QFMathParser_DefaultLib::fAllFalse);
    p->addFunction("anytrue", QFMathParser_DefaultLib::fAnyTrue);
    p->addFunction("anyfalse", QFMathParser_DefaultLib::fAnyFalse);
    p->addFunction("contains", QFMathParser_DefaultLib::fContains);
    p->addFunction("countoccurences", QFMathParser_DefaultLib::fCountOccurences);

    p->addFunction("isnan", QFMathParser_DefaultLib::fIsNan);
    p->addFunction("isinf", QFMathParser_DefaultLib::fIsInf);
    p->addFunction("isfinite", QFMathParser_DefaultLib::fIsFinite);
    p->addFunction("isnumok", QFMathParser_DefaultLib::fIsFloatOK);

    p->addFunction("unique", QFMathParser_DefaultLib::fUnique);
    p->addFunction("indexedmean", QFMathParser_DefaultLib::fIndexedAvg);
    p->addFunction("indexedvar", QFMathParser_DefaultLib::fIndexedVar);
    p->addFunction("indexedstd", QFMathParser_DefaultLib::fIndexedStd);
    p->addFunction("indexedmin", QFMathParser_DefaultLib::fIndexedMin);
    p->addFunction("indexedmax", QFMathParser_DefaultLib::fIndexedMax);
    p->addFunction("indexedmedian", QFMathParser_DefaultLib::fIndexedMedian);
    p->addFunction("indexedsum", QFMathParser_DefaultLib::fIndexedSum);
    p->addFunction("indexedsum2", QFMathParser_DefaultLib::fIndexedSum2);
    p->addFunction("indexedquantile", QFMathParser_DefaultLib::fIndexedQuantile);

    p->addFunction("indexedskewness", QFMathParser_DefaultLib::fIndexedSkewness);
    p->addFunction("indexedprod", QFMathParser_DefaultLib::fIndexedProd);
    p->addFunction("indexedcount", QFMathParser_DefaultLib::fIndexedCount);
    p->addFunction("indexedmoment", QFMathParser_DefaultLib::fIndexedMoment);
    p->addFunction("indexedncmoment", QFMathParser_DefaultLib::fIndexedNonCentralMoment);
    p->addFunction("indexedcorrcoeff", QFMathParser_DefaultLib::fIndexedCorrCoeff);

    p->addFunction("polyval", QFMathParser_DefaultLib::fPolynom);
    p->addFunction("polyder", QFMathParser_DefaultLib::fDerivPolynom);

    p->addFunction("last", QFMathParser_DefaultLib::fLast);
    p->addFunction("first", QFMathParser_DefaultLib::fFirst);
    p->addFunction("returnlast", QFMathParser_DefaultLib::fReturnLast);
    p->addFunction("returnfirst", QFMathParser_DefaultLib::fReturnFirst);
    p->addFunction("item", QFMathParser_DefaultLib::fItem);

    p->addFunction("runningaverage", QFMathParser_DefaultLib::fRunningAverage);

    p->addFunction("regexpcap", QFMathParser_DefaultLib::fRegExpCapture);
    p->addFunction("regexpcontains", QFMathParser_DefaultLib::fRegExpContains);
    p->addFunction("regexpindexin", QFMathParser_DefaultLib::fRegExpIndexIn);


    p->addFunction("str2bool", QFMathParser_DefaultLib::fStringToBool);
    p->addFunction("str2num", QFMathParser_DefaultLib::fStringToNum);
    p->addFunction("cstr2num", QFMathParser_DefaultLib::fCStringToNum);
}


















namespace QFMathParser_DefaultLib {

    QString ptosystempathseparator(const QString& param) {
        QString r;
        for (int i=0; i<param.size(); i++) {
            QChar ch=param[i];
            if (ch=='/' || ch=='\\') ch=QDir::separator();
            r+=ch;
        }
        return r;
    }





    QVector<double> QFMathParser_shuffleD(const QVector<double>& value) {
        std::vector<double> mm=value.toStdVector();
        std::random_shuffle( mm.begin(), mm.end() );
        return QVector<double>::fromStdVector(mm);
    }
    QVector<bool> QFMathParser_shuffleB(const QVector<bool>& value) {
        std::vector<bool> mm=value.toStdVector();
        std::random_shuffle( mm.begin(), mm.end() );
        return QVector<bool>::fromStdVector(mm);
    }
    QStringList QFMathParser_shuffleS(const QStringList& value) {
        std::vector<QString> mm;
        for (int i=0; i<value.size(); i++) {
            mm.push_back(value[i]);
        }
        std::random_shuffle( mm.begin(), mm.end() );
        QStringList res;
        for (size_t i=0; i<mm.size(); i++) {
            res<<mm[i];
        }
        return res;
    }

    void fShuffle(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("shuffle"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpDoubleVector) {
            r.setDoubleVec(QFMathParser_shuffleD(params[0].numVec));
        } else if(params[0].type==qfmpStringVector) {
            r.setStringVec(QFMathParser_shuffleS(params[0].strVec));
        } else if(params[0].type==qfmpBoolVector) {
            r.setBoolVec(QFMathParser_shuffleB(params[0].boolVec));
        } else {
            p->qfmpError(QObject::tr("%1(x) argument has to be a vector of numbers/booleans/strings").arg("shuffle"));
            r.setInvalid();
        }
        return;
    }


    void fAllTrue(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("alltrue"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpBoolVector) {
            bool res=true;
            for (int i=0; i<params[0].boolVec.size(); i++) {
                if (!params[0].boolVec[i]) {
                    res=false;
                    break;
                }
            }
            r.setBoolVec(res);
        } else if(params[0].type==qfmpBool) {
            r.setBoolVec(params[0].boolean);
         } else {
            p->qfmpError(QObject::tr("%1(x) argument has to be a vector of booleans").arg("alltrue"));
            r.setInvalid();
        }
        return;
    }

    void fAllFalse(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("allfalse"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpBoolVector) {
            bool res=true;
            for (int i=0; i<params[0].boolVec.size(); i++) {
                if (params[0].boolVec[i]) {
                    res=false;
                    break;
                }
            }
            r.setBoolVec(res);
        } else if(params[0].type==qfmpBool) {
            r.setBoolVec(!params[0].boolean);
         } else {
            p->qfmpError(QObject::tr("%1(x) argument has to be a vector of booleans").arg("allfalse"));
            r.setInvalid();
        }
        return;
    }


    void fAnyTrue(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("anytrue"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpBoolVector) {
            bool res=false;
            for (int i=0; i<params[0].boolVec.size(); i++) {
                if (params[0].boolVec[i]) {
                    res=true;
                    break;
                }
            }
            r.setBoolVec(res);
        } else if(params[0].type==qfmpBool) {
            r.setBoolVec(params[0].boolean);
         } else {
            p->qfmpError(QObject::tr("%1(x) argument has to be a vector of booleans").arg("anytrue"));
            r.setInvalid();
        }
        return;
    }

    void fAnyFalse(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("anyfalse"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpBoolVector) {
            bool res=false;
            for (int i=0; i<params[0].boolVec.size(); i++) {
                if (!params[0].boolVec[i]) {
                    res=true;
                    break;
                }
            }
            r.setBoolVec(res);
        } else if(params[0].type==qfmpBool) {
            r.setBoolVec(!params[0].boolean);
         } else {
            p->qfmpError(QObject::tr("%1(x) argument has to be a vector of booleans").arg("anyfalse"));
            r.setInvalid();
        }
        return;
    }

    void fContains(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x, value) needs exacptly 2 arguments").arg("contains"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {
            r.setBoolean(params[0].numVec.contains(params[1].num));
        } else if(params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            r.setBoolean(params[0].strVec.contains(params[1].str));
        } else if(params[0].type==qfmpBoolVector && params[1].type==qfmpBool) {
            r.setBoolean(params[0].boolVec.contains(params[1].boolean));
        } else {
            p->qfmpError(QObject::tr("%1(x, value) argument 1 has to be a vector of numbers/booleans/strings and argument 2 the according item type number/string/boolean").arg("contains"));
            r.setInvalid();
        }
        return;
    }
    void fCountOccurences(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x, value) needs exacptly 2 arguments").arg("countoccurences"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {
            r.setBoolean(params[0].numVec.count(params[1].num));
        } else if(params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            r.setBoolean(params[0].strVec.count(params[1].str));
        } else if(params[0].type==qfmpBoolVector && params[1].type==qfmpBool) {
            r.setBoolean(params[0].boolVec.count(params[1].boolean));
        } else {
            p->qfmpError(QObject::tr("%1(x, value) argument 1 has to be a vector of numbers/booleans/strings and argument 2 the according item type number/string/boolean").arg("countoccurences"));
            r.setInvalid();
        }
        return;
    }

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






    void fRandVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
      bool ok=true;
      if (n>3 || n<=0) { ok=false; p->qfmpError(QObject::tr("randvec accepts 1, 2 or 3 arguments")); }
      if (n>0 && (params[0].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randvec needs a double as first argument (if any)")); }
      if (n>1 && (params[1].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randvec needs a double as second argument (if any)")); }
      if (n>2 && (params[2].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randvec needs a double as third argument (if any)")); }
      int items=params[0].toInteger();
      if (items<=0) { ok=false; p->qfmpError(QObject::tr("randvec's first argument has to be a positive integer number")); }
      if (ok) {
          r.setDoubleVec(items, 0);

          if (n==1) {
              for (int i=0; i<items; i++)
                r.numVec[i]=p->get_rng()->rand();
          } else if (n==2) {
              for (int i=0; i<items; i++)
                r.numVec[i]=p->get_rng()->rand()*(params[2].num-params[1].num)+params[1].num;
          } else {
              for (int i=0; i<items; i++)
                r.numVec[i]=p->get_rng()->rand(params[1].num);
          }
      } else {
        r.setInvalid();
      }


    }


    void fRandIntVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
      bool ok=true;
      if (n>3 || n<=0) { ok=false; p->qfmpError(QObject::tr("randintvec accepts 1, 2 or 3 arguments")); }
      if (n>0 && (params[0].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randintvec needs a double as first argument (if any)")); }
      if (n>1 && (params[1].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randintvec needs a double as second argument (if any)")); }
      if (n>2 && (params[2].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randintvec needs a double as third argument (if any)")); }
      int items=params[0].toInteger();
      if (items<=0) { ok=false; p->qfmpError(QObject::tr("randintvec's first argument has to be a positive integer number")); }
      if (ok) {
          r.setDoubleVec(items, 0);

          if (n==1) {
              for (int i=0; i<items; i++)
                r.numVec[i]=p->get_rng()->randInt();
          } else if (n==2) {
              for (int i=0; i<items; i++)
                r.numVec[i]=p->get_rng()->randInt()*((uint32_t)params[2].num-(uint32_t)params[1].num)+(uint32_t)params[1].num;
          } else {
              for (int i=0; i<items; i++)
                r.numVec[i]=p->get_rng()->randInt((uint32_t)params[1].num);
          }
      } else {
          r.setInvalid();
      }


    }
    void fRandNormVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        bool ok=true;
      if (n>3 || n<=0) { ok=false; p->qfmpError(QObject::tr("randnormvec accepts 1, 2 or 3 arguments")); }
      if (n>0 && (params[0].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randnormvec needs a double as first argument (if any)")); }
      if (n>1 && (params[1].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randnormvec needs a double as second argument (if any)")); }
      if (n>2 && (params[2].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randnormvec needs a double as third argument (if any)")); }
      int items=params[0].toInteger();
      if (items<=0) { ok=false; p->qfmpError(QObject::tr("randnormvec's first argument has to be a positive integer number")); }
      if (ok) {

          r.setDoubleVec(items, 0);
          double mean=0;
          double var=1;
          if (n==2) {
              var=params[1].num;
          } else if (n>2) {
              mean=params[1].num;
              var=params[2].num;
          }

          for (int i=0; i<items; i++)
            r.numVec[i]=p->get_rng()->randNorm(mean, var);
      } else {
          r.setInvalid();
      }

    }



    void fRandBoolVec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        bool ok=true;
      if (n>2 || n<=0) { ok=false; p->qfmpError(QObject::tr("randboolvec accepts 1 or 2 arguments")); }
      if (n>0 && (params[0].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randboolvec needs a double as first argument (if any)")); }
      if (n>1 && (params[1].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randboolvec needs a double as second argument (if any)")); }
      int items=params[0].toInteger();
      double pr=0.5;
      if (n>1) pr=params[1].num;
      if (items<=0) { ok=false; p->qfmpError(QObject::tr("randboolvec's first argument has to be a positive integer number")); }
      if (pr<0 || pr>1) { ok=false; p->qfmpError(QObject::tr("randboolvec's second argument has to be a number between 0 and 1")); }
      if (ok) {
          r.setBoolVec(items, false);

          for (int i=0; i<items; i++)
            r.boolVec[i]=(p->get_rng()->rand()<=pr);
      } else {
          r.setInvalid();
      }

    }

    void fRandBool(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        bool ok=true;
      if (n>1) { ok=false; p->qfmpError(QObject::tr("randbool accepts 0 or 1 arguments")); }
      if (n>0 && (params[0].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randboolvec needs a double as first argument (if any)")); }
      double pr=0.5;
      if (n>1) pr=params[0].num;
      if (pr<0 || pr>1) { ok=false; p->qfmpError(QObject::tr("randbool's second argument has to be a number between 0 and 1")); }
      if (ok) {
          r.setBoolean(p->get_rng()->rand()<=pr);
      } else {
          r.setInvalid();
      }
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
            r.setDoubleVec();
            for (int i=0; i<dat.size(); i++) {
                if (!ii.contains(i)) {
                    r.numVec<<dat[i];
                }
            }
        } else if (n==2 && params[0].type==qfmpStringVector && params[1].convertsToIntVector()) {
            QVector<int> ii=params[1].asIntVector();
            QStringList dat=params[0].strVec;
            r.setStringVec();
            for (int i=0; i<dat.size(); i++) {
                if (!ii.contains(i)) {
                    r.strVec<<dat[i];
                }
            }
        } else if (n==2 && params[0].type==qfmpBoolVector && params[1].convertsToIntVector()) {
            QVector<int> ii=params[1].asIntVector();
            QVector<bool> dat=params[0].boolVec;
            r.setBoolVec();
            for (int i=0; i<dat.size(); i++) {
                if (!ii.contains(i)) {
                    r.boolVec<<dat[i];
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
        r.isValid=true;
        if (n>0) {
            if (params[0].type==qfmpDouble || params[0].type==qfmpDoubleVector) {
                r.type=qfmpDoubleVector;
                r.numVec.clear();
                for (unsigned int i=0; i<n; i++) {
                    if (params[i].convertsToVector()) {
                        r.numVec+=params[i].asVector();
                    } else {
                        p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more vectors or vectorelements as arguments (all have to have the same type)"));
                        return p->getInvalidResult();
                    }
                }
            } else if (params[0].type==qfmpBool || params[0].type==qfmpBoolVector) {
                r.type=qfmpBoolVector;
                r.boolVec.clear();
                for (unsigned int i=0; i<n; i++) {
                    if (params[i].convertsToBoolVector()) {
                        r.boolVec+=params[i].asBoolVector();
                    } else {
                        p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more vectors or vectorelements as arguments (all have to have the same type)"));
                        return p->getInvalidResult();
                    }
                }
            } else if (params[0].type==qfmpString || params[0].type==qfmpStringVector) {
                r.type=qfmpStringVector;
                r.strVec.clear();
                for (unsigned int i=0; i<n; i++) {
                    if (params[i].convertsToStringVector()) {
                        r.strVec.append(params[i].asStrVector());
                    } else {
                        p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more vectors or vectorelements as arguments (all have to have the same type)"));
                        return p->getInvalidResult();
                    }
                }
            }
        } else {
            p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more vectors or vectorelements as arguments (all have to have the same type)"));
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
        } else if (n==2 && params[0].type==qfmpBoolVector && params[1].type==qfmpBool) {
            const QVector<bool>& dat=params[0].boolVec;
            r.type=qfmpBoolVector;
            r.boolVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]!=params[1].boolean) {
                    r.boolVec<<dat[i];
                }
            }
        } else if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpStringVector;
            r.strVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]!=params[1].str) {
                    r.strVec<<dat[i];
                }
            }
        } else {
            p->qfmpError(QObject::tr("removeall(x, value) need one number vector and one number argument"));
        }
        return r;
    }

    qfmpResult fFind(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.isValid=false;
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {
            const QVector<double>& dat=params[0].numVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]==params[1].num) {
                    r.numVec<<i;
                }
            }
        } else if (n==2 && params[0].type==qfmpBoolVector && params[1].type==qfmpBool) {
            const QVector<bool>& dat=params[0].boolVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]==params[1].boolean) {
                    r.numVec<<i;
                }
            }
        } else if (n==1 && params[0].type==qfmpBoolVector) {
            const QVector<bool>& dat=params[0].boolVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]) {
                    r.numVec<<i;
                }
            }
        } else if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]==params[1].str) {
                    r.numVec<<i;
                }
            }
        } else {
            p->qfmpError(QObject::tr("find(x, value=true) needs two arguments: one vector x and a corresponding element value, if only a boolean vector is given, the function uses value=true"));
        }
        return r;
    }


    qfmpResult fSelect(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.isValid=false;
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpBoolVector) {
            const QVector<double>& dat=params[0].numVec;
            if (dat.size()!=params[1].boolVec.size()) {
                p->qfmpError(QObject::tr("find(x, criterion) vectors x and criterion have different size"));
                return r;
            }
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (params[1].boolVec[i]) {
                    r.numVec<<dat[i];
                }
            }
        } else if (n==2 && params[0].type==qfmpBoolVector && params[1].type==qfmpBoolVector) {
            const QVector<bool>& dat=params[0].boolVec;
            if (dat.size()!=params[1].boolVec.size()) {
                p->qfmpError(QObject::tr("find(x, criterion) vectors x and criterion have different size"));
                return r;
            }
            r.type=qfmpBoolVector;
            r.boolVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (params[1].boolVec[i]) {
                    r.boolVec<<dat[i];
                }
            }

        } else if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpBoolVector) {
            const QStringList& dat=params[0].strVec;
            if (dat.size()!=params[1].boolVec.size()) {
                p->qfmpError(QObject::tr("find(x, criterion) vectors x and criterion have different size"));
                return r;
            }
            r.type=qfmpStringVector;
            r.strVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (params[1].boolVec[i]) {
                    r.strVec<<dat[i];
                }
            }
        } else {
            p->qfmpError(QObject::tr("find(x, criterion) needs two arguments, a vector x and a boolean vector criterion of same length"));
        }
        return r;
    }
    qfmpResult fReverse(const qfmpResult* params, unsigned int  n, QFMathParser* p){
        qfmpResult r;
        r.type=qfmpDoubleVector;
        if (n==1 && params[0].type==qfmpDoubleVector) {
            r.setDoubleVec(params[0].numVec);
            for (int i=0; i<r.numVec.size(); i++) {
                r.numVec[i]=params[0].numVec[r.numVec.size()-i-1];
            }
        } else if (n==1 && params[0].type==qfmpStringVector) {
            r.setStringVec(params[0].strVec);
            for (int i=0; i<r.strVec.size(); i++) {
                r.strVec[i]=params[0].strVec[r.strVec.size()-i-1];
            }
        } else if (n==1 && params[0].type==qfmpBoolVector) {
            r.setBoolVec(params[0].boolVec);
            for (int i=0; i<r.boolVec.size(); i++) {
                r.boolVec[i]=params[0].boolVec[r.boolVec.size()-i-1];
            }
        } else {
            p->qfmpError(QObject::tr("reverse(x) need one number vector argument"));
            r.isValid=false;
            return r;
        }
        return r;
    }



    qfmpResult fUnique(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();
        if (n!=1) {
            p->qfmpError("unique(data) needs 1 argument");
            return res;
        }
        if ((params[0].type==qfmpDouble) || (params[0].type==qfmpString) || (params[0].type==qfmpBool)) return params[0];

        if (params[0].type==qfmpDoubleVector) {
            QVector<double> out;
            const QVector<double>& in=params[0].numVec;

            for (int i=0; i<in.size(); i++) {
                if (!out.contains(in[i])) out<<in[i];
            }

            return qfmpResult(out);
        }

        if (params[0].type==qfmpStringVector) {
            QStringList out;
            const QStringList& in=params[0].strVec;

            for (int i=0; i<in.size(); i++) {
                if (!out.contains(in[i])) out<<in[i];
            }

            return qfmpResult(out);
        }

        if (params[0].type==qfmpBoolVector) {
            QVector<bool> out;
            const QVector<bool>& in=params[0].boolVec;

            for (int i=0; i<in.size(); i++) {
                if (!out.contains(in[i])) out<<in[i];
                if (out.size()>1) break;
            }

            return qfmpResult(out);
        }
        return res;
    }






    qfmpResult fIndexedAvg(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedmean(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsAverage<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsAverage<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsAverage<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedmean(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedVar(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedvar(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsVariance<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsVariance<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsVariance<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedvar(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedStd(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedstd(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsStd<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsStd<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsStd<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedstd(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedSum(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedsum(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsSum<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsSum<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsSum<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedsum(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedSum2(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedsum2(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsSum2<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsSum2<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsSum2<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedsum2(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }


    qfmpResult fIndexedMedian(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedmedian(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsMedian<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsMedian<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsMedian<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedmedian(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedMin(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedmin(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsMin<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsMin<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsMin<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedmin(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedMax(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedmax(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsMax<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsMax<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsMax<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedmax(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedQuantile(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=3) p->qfmpError("indexedquantile(data, index, quantile) needs 3 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length() && params[2].type==qfmpDouble) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsQuantile<QVector<double> >, params[2].num));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsQuantile<QVector<double> >, params[2].num));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsQuantile<QVector<double> >, params[2].num));
            }
        } else {
            p->qfmpError("indexedquantile(data, index, quantile) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }


    qfmpResult fIndexedSkewness(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedskewness(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsSkewness<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsSkewness<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsSkewness<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedskewness(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedProd(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedprod(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsProd<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsProd<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsProd<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedprod(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedCount(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedcount(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsCount<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsCount<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsCount<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedcount(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }


    qfmpResult fIndexedMoment(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=3) p->qfmpError("indexedmoment(data, index, order) needs 3 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length() && params[2].type==qfmpDouble) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsCentralMoment<QVector<double> >, params[2].toInteger()));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsCentralMoment<QVector<double> >, params[2].toInteger()));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsCentralMoment<QVector<double> >, params[2].toInteger()));
            }
        } else {
            p->qfmpError("indexedmoment(data, index, order) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedNonCentralMoment(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=3) p->qfmpError("indexedncmoment(data, index, order) needs 3 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length() && params[2].type==qfmpDouble) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsMoment<QVector<double> >, params[2].toInteger()));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsMoment<QVector<double> >, params[2].toInteger()));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsMoment<QVector<double> >, params[2].toInteger()));
            }
        } else {
            p->qfmpError("indexedncmoment(data, index, order) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedCorrCoeff(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=3) p->qfmpError("indexedcorrcoeff(dataX, dataY, index) needs 3 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && ((params[1].type==qfmpDoubleVector)||(params[1].type==qfmpDouble)) && params[0].length()==params[2].length() && params[1].length()==params[2].length()) {
            QVector<double> d=params[0].asVector();
            QVector<double> dy=params[1].asVector();

            if (params[2].type==qfmpDouble || params[2].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction2I(d, dy, params[2].asVector(), qfstatisticsCorrCoeff<QVector<double> >));
            }
            if (params[2].type==qfmpBool || params[2].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction2I(d, dy, params[2].asBoolVector(), qfstatisticsCorrCoeff<QVector<double> >));
            }
            if (params[2].type==qfmpString || params[2].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction2I(d, dy, params[2].asStrVector(), qfstatisticsCorrCoeff<QVector<double> >));
            }
        } else {
            p->qfmpError(QObject::tr("indexedcorrcoeff(dataX, dataY, index) needs a number vector as data argument and an equal sized index array of any type [len(dataX)=%1, len(dataY)=%2, len(idx)=%3]").arg(params[0].length()).arg(params[1].length()).arg(params[2].length()));
        }

        return res;
    }














    qfmpResult fReturnLast(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n<1) {
            p->qfmpError("returnlast(x1,x2,x3,...) needs at least one argument");
            return res;
        }
        return params[n-1];
    }


    qfmpResult fReturnFirst(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n<1) {
            p->qfmpError("returnfirst(x1,x2,x3,...) needs at least one argument");
            return res;
        }
        return params[0];
    }

    qfmpResult fLast(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=1) {
            p->qfmpError("last(x) needs one argument");
            return res;
        }
        if (params[0].type==qfmpDouble) return qfmpResult(params[0].num);
        if (params[0].type==qfmpDoubleVector && params[0].length()>0) return qfmpResult(params[0].numVec.last());
        if (params[0].type==qfmpBool) return qfmpResult(params[0].boolean);
        if (params[0].type==qfmpBoolVector && params[0].length()>0) return qfmpResult(params[0].boolVec.last());
        if (params[0].type==qfmpString) return qfmpResult(params[0].str.right(1));
        if (params[0].type==qfmpStringVector && params[0].length()>0) return qfmpResult(params[0].strVec.last());

        p->qfmpError("last(x): x had no entries or unrecognized type");
        return res;
    }


    qfmpResult fFirst(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=1) {
            p->qfmpError("first(x) needs one argument");
            return res;
        }
        if (params[0].type==qfmpDouble) return qfmpResult(params[0].num);
        if (params[0].type==qfmpDoubleVector && params[0].length()>0) return qfmpResult(params[0].numVec.first());
        if (params[0].type==qfmpBool) return qfmpResult(params[0].boolean);
        if (params[0].type==qfmpBoolVector && params[0].length()>0) return qfmpResult(params[0].boolVec.first());
        if (params[0].type==qfmpString) return qfmpResult(params[0].str.left(1));
        if (params[0].type==qfmpStringVector && params[0].length()>0) return qfmpResult(params[0].strVec.first());

        p->qfmpError("first(x): x had no entries or unrecognized type");
        return res;
    }


    qfmpResult fItem(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) {
            p->qfmpError("item(x, idx) needs two argument");
            return res;
        }
        QVector<int> idx=params[1].asIntVector();
        bool isNumber=true;
        QVector<bool> idxb=params[1].asBoolVector();
        if (idx.size()<=0 && idxb.size()<=0) {
            p->qfmpError("item(x, idx) parameter idx has to be a number, vector of numbers or vector of booleans");
            return res;
        } else if (idx.size()<=0 && idxb.size()>0) {
            isNumber=false;
            if (idxb.size()!=params[0].length()) {
                p->qfmpError(QObject::tr("item(x, boolidx) parameter boolidx has to be a vector of booleans with the same length as vector x [length(x)=%2, length(boolidx)=%1]").arg(idxb.size()).arg(params[0].length()));
                return res;
            }
        }
        if (isNumber) {
            if (params[0].type==qfmpDouble && idx.contains(0)) return qfmpResult(params[0].num);
            if (params[0].type==qfmpBool && idx.contains(0)) return qfmpResult(params[0].boolean);
            if (params[0].type==qfmpString) {
                QString r;
                for (int i=0; i<idx.size(); i++) {
                    if (i>=0 && i<params[0].str.size()) {
                        r+=params[0].str[idx[i]];
                    } else {
                        p->qfmpError(QObject::tr("item(x, idx) index i=%1 out of range").arg(idx[i]));
                        return res;
                    }
                }
                return qfmpResult(r);
            }
            if (params[0].type==qfmpStringVector) {
                QStringList r;
                for (int i=0; i<idx.size(); i++) {
                    if (i>=0 && i<params[0].strVec.size()) {
                        r<<params[0].strVec[idx[i]];
                    } else {
                        p->qfmpError(QObject::tr("item(x, idx) index i=%1 out of range").arg(idx[i]));
                        return res;
                    }
                }
                if (r.size()==1) return qfmpResult(r[0]);
                return qfmpResult(r);
            }
            if (params[0].type==qfmpBoolVector) {
                QVector<bool> r;
                for (int i=0; i<idx.size(); i++) {
                    if (i>=0 && i<params[0].boolVec.size()) {
                        r<<params[0].boolVec[idx[i]];
                    } else {
                        p->qfmpError(QObject::tr("item(x, idx) index i=%1 out of range").arg(idx[i]));
                        return res;
                    }
                }
                if (r.size()==1) return qfmpResult(r[0]);
                return qfmpResult(r);
            }
            if (params[0].type==qfmpDoubleVector) {
                QVector<double> r;
                for (int i=0; i<idx.size(); i++) {
                    if (i>=0 && i<params[0].numVec.size()) {
                        r<<params[0].numVec[idx[i]];
                    } else {
                        p->qfmpError(QObject::tr("item(x, idx) index i=%1 out of range").arg(idx[i]));
                        return res;
                    }
                }
                if (r.size()==1) return qfmpResult(r[0]);
                return qfmpResult(r);
            }

        } else {
            if (params[0].type==qfmpString) {
                QString r;
                for (int i=0; i<idxb.size(); i++) {
                    if (idxb[i]) {
                        r+=params[0].str[i];
                    }
                }
                return qfmpResult(r);
            }
            if (params[0].type==qfmpStringVector) {
                QStringList r;
                for (int i=0; i<idxb.size(); i++) {
                    if (idxb[i]) {
                        r<<params[0].strVec[i];
                    }
                }
                if (r.size()==1) return qfmpResult(r[0]);
                return qfmpResult(r);
            }
            if (params[0].type==qfmpBoolVector) {
                QVector<bool> r;
                for (int i=0; i<idxb.size(); i++) {
                    if (idxb[i]) {
                        r<<params[0].boolVec[i];
                    }
                }
                if (r.size()==1) return qfmpResult(r[0]);
                return qfmpResult(r);
            }
            if (params[0].type==qfmpDoubleVector) {
                QVector<double> r;
                for (int i=0; i<idxb.size(); i++) {
                    if (idxb[i]) {
                        r<<params[0].numVec[i];
                    }
                }
                if (r.size()==1) return qfmpResult(r[0]);
                return qfmpResult(r);
            }
        }

        p->qfmpError("item(x, idx): x had no entries or unrecognized type");
        return res;
    }

    qfmpResult fRunningAverage(const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) {
            p->qfmpError("runningaverage(x,n) needs two arguments");
            return res;
        }
        if (params[0].type==qfmpDoubleVector && params[1].isInteger() && params[1].toInteger()>0) {
            const QVector<double>& iv=params[0].numVec;
            int wid=params[1].toInteger();
            res.setDoubleVec(QVector<double>());
            for (int i=0; i<iv.size(); i+=wid) {
                double s=0;
                if (i+wid<iv.size()) {
                    for (int j=0; j<wid; j++) {
                        s+=iv[i+j];
                    }
                    res.numVec<<(s/double(wid));
                }
            }
            return res;
        } else {
            p->qfmpError("runningaverage(x,n) requires a number vector as first and an integer number >1 as second argument");
            return res;
        }
    }

    void fRegExpCapture(qfmpResult& r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n<2 || n>4) {\
            p->qfmpError(QObject::tr("regexpcap(regexp, strings, cap_id=1, default_string=\"\") needs 2 or 4 arguments"));\
            r.setInvalid();\
            return; \
        }\
        if(params[1].type==qfmpStringVector || params[1].type==qfmpString) {\
            if(params[0].type==qfmpString) {\
                int capid=1;
                QString defaultStr="";
                if (n>2) {
                    if(params[2].type==qfmpDouble && params[2].toInteger()>=0) {\
                        capid=params[2].toInteger();
                    } else {
                        p->qfmpError(QObject::tr("regexpcap(regexp, strings, cap_id, default_string) argument cap_id has to be a number >=0"));\
                        r.setInvalid();\
                        return;
                    }
                }
                if (n>3) {
                    if(params[3].type==qfmpString) {\
                        defaultStr=params[3].str;
                    } else {
                        p->qfmpError(QObject::tr("regexpcap(regexp, strings, cap_id, default_string) argument default_string has to be a string"));\
                        r.setInvalid();\
                        return;
                    }
                }
                QRegExp rx(params[0].str);
                rx.setMinimal(false);
                rx.setCaseSensitivity(Qt::CaseInsensitive);
                if (params[1].type==qfmpStringVector) {
                    r.setStringVec(params[1].strVec);
                    for (int i=0; i<r.strVec.size(); i++) {
                        if (rx.indexIn(r.strVec[i])<0) {
                            r.strVec[i]=defaultStr;
                        } else {
                            r.strVec[i]=rx.cap(capid);
                        }
                    }
                } else if (params[1].type==qfmpString) {
                    r.setStringVec(params[1].strVec);
                    if (rx.indexIn(r.str)<0) {
                        r.str=defaultStr;
                    } else {
                        r.str=rx.cap(capid);
                    }
                }
            } else {
                p->qfmpError(QObject::tr("regexpcap(regexp, strings, cap_id=1, default_string=\"\") argument regexp has to be a string"));\
                r.setInvalid();\
                return;
            }
        } else {\
            p->qfmpError(QObject::tr("regexpcap(regexp, strings, cap_id, default_string=\"\") argument strings has to be a vector of strings and a string"));\
            r.setInvalid();\
            return;
        }\
        return; \
    }

    void fRegExpContains(qfmpResult& r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=2) {\
            p->qfmpError(QObject::tr("regexpcontains(regexp, strings) needs 2 arguments"));\
            r.setInvalid();\
            return; \
        }\
        if(params[1].type==qfmpStringVector || params[1].type==qfmpString) {\
            if(params[0].type==qfmpString) {\
                QRegExp rx(params[0].str);
                rx.setMinimal(false);
                rx.setCaseSensitivity(Qt::CaseInsensitive);
                if (params[1].type==qfmpStringVector) {
                    QVector<bool> bv;
                    for (int i=0; i<r.strVec.size(); i++) {
                        bv.append(rx.indexIn(r.strVec[i])>=0);
                    }
                    r.setBoolVec(bv);
                } else if (params[1].type==qfmpString) {
                    r.setBoolean(rx.indexIn(r.str)>=0);
                }
            } else {
                p->qfmpError(QObject::tr("regexpcontains(regexp, strings) argument regexp has to be a string"));\
                r.setInvalid();\
                return;
            }
        } else {\
            p->qfmpError(QObject::tr("regexpcontains(regexp, strings) argument strings has to be a vector of strings and a string"));\
            r.setInvalid();\
            return;
        }\
        return; \
    }

    void fRegExpIndexIn(qfmpResult& r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=2) {\
            p->qfmpError(QObject::tr("regexpindexin(regexp, strings) needs 2 arguments"));\
            r.setInvalid();\
            return; \
        }\
        if(params[1].type==qfmpStringVector || params[1].type==qfmpString) {\
            if(params[0].type==qfmpString) {\
                QRegExp rx(params[0].str);
                rx.setMinimal(false);
                rx.setCaseSensitivity(Qt::CaseInsensitive);
                if (params[1].type==qfmpStringVector) {
                    QVector<double> bv;
                    for (int i=0; i<r.strVec.size(); i++) {
                        bv.append(rx.indexIn(r.strVec[i]));
                    }
                    r.setDoubleVec(bv);
                } else if (params[1].type==qfmpString) {
                    r.setDouble(rx.indexIn(r.str));
                }
            } else {
                p->qfmpError(QObject::tr("regexpindexin(regexp, strings) argument regexp has to be a string"));\
                r.setInvalid();\
                return;
            }
        } else {\
            p->qfmpError(QObject::tr("regexpindexin(regexp, strings) argument strings has to be a vector of strings and a string"));\
            r.setInvalid();\
            return;
        }\
        return; \
    }

}
