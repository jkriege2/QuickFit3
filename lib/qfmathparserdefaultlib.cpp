/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfmathparser.h"
#include <typeinfo>

#include "qfmathparserdefaultlib.h"
#include <QDateTime>
#include <QDate>
#include <QTime>


#define QFMATHPARSER_REGISTER_FUNC(FName, NAME_IN_PARSER) p->addFunction(#NAME_IN_PARSER, QFMathParser_DefaultLib::FName);


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
    p->addFunction("randvec_seed", QFMathParser_DefaultLib::fRandVecSeed);
    p->addFunction("randintvec_seed", QFMathParser_DefaultLib::fRandIntVecSeed);
    p->addFunction("randnormvec_seed", QFMathParser_DefaultLib::fRandNormVecSeed);
    p->addFunction("randboolvec_seed", QFMathParser_DefaultLib::fRandBoolVecSeed);
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
    p->addFunction("diff", QFMathParser_DefaultLib::fDiff);
    p->addFunction("cumsum", QFMathParser_DefaultLib::fCumSum);
    p->addFunction("cumprod", QFMathParser_DefaultLib::fCumProd);
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
    p->addFunction("findfirst", QFMathParser_DefaultLib::fFindFirst);
    p->addFunction("select", QFMathParser_DefaultLib::fSelect);
    p->addFunction("concat", QFMathParser_DefaultLib::fConcat);
    p->addFunction("deg2rad", QFMathParser_DefaultLib::fDegToRad, NULL, qfDegToRad);
    p->addFunction("rad2deg", QFMathParser_DefaultLib::fRadToDeg, NULL, qfRadToDeg);
    p->addFunction("dot", QFMathParser_DefaultLib::fDot);
    p->addFunction("alltrue", QFMathParser_DefaultLib::fAllTrue);
    p->addFunction("allfalse", QFMathParser_DefaultLib::fAllFalse);
    p->addFunction("anytrue", QFMathParser_DefaultLib::fAnyTrue);
    p->addFunction("anyfalse", QFMathParser_DefaultLib::fAnyFalse);
    p->addFunction("countoccurences", QFMathParser_DefaultLib::fCountOccurences);
    p->addFunction("mad", QFMathParser_DefaultLib::fMAD);
    p->addFunction("nmad", QFMathParser_DefaultLib::fNMAD);
    p->addFunction("trapz", QFMathParser_DefaultLib::fTrapz);
    p->addFunction("trapez", QFMathParser_DefaultLib::fTrapz);

    p->addFunction("containssubstring", QFMathParser_DefaultLib::fContainsSubString);
    p->addFunction("contains", QFMathParser_DefaultLib::fContains);

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
    p->addFunction("indexedmad", QFMathParser_DefaultLib::fIndexedMAD);
    p->addFunction("indexednmad", QFMathParser_DefaultLib::fIndexedNMAD);

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
    p->addFunction("filterfinite", QFMathParser_DefaultLib::fFilterFinite);
    p->addFunction("filternumok", QFMathParser_DefaultLib::fFilterFinite);

    p->addFunction("regexpcap", QFMathParser_DefaultLib::fRegExpCaptureNCS);
    p->addFunction("regexpcontains", QFMathParser_DefaultLib::fRegExpContainsNCS);
    p->addFunction("regexpindexin", QFMathParser_DefaultLib::fRegExpIndexInNCS);

    p->addFunction("regexpcap_minimal", QFMathParser_DefaultLib::fRegExpCaptureMNCS);
    p->addFunction("regexpcontains_minimal", QFMathParser_DefaultLib::fRegExpContainsMNCS);
    p->addFunction("regexpindexin_minimal", QFMathParser_DefaultLib::fRegExpIndexInMNCS);

    p->addFunction("regexpcap_cs", QFMathParser_DefaultLib::fRegExpCaptureCS);
    p->addFunction("regexpcontains_cs", QFMathParser_DefaultLib::fRegExpContainsCS);
    p->addFunction("regexpindexin_cs", QFMathParser_DefaultLib::fRegExpIndexInCS);

    p->addFunction("regexpcap_cs_minimal", QFMathParser_DefaultLib::fRegExpCaptureMCS);
    p->addFunction("regexpcontains_cs_minimal", QFMathParser_DefaultLib::fRegExpContainsMCS);
    p->addFunction("regexpindexin_cs_minimal", QFMathParser_DefaultLib::fRegExpIndexInMCS);



    p->addFunction("str2bool", QFMathParser_DefaultLib::fStringToBool);
    p->addFunction("str2num", QFMathParser_DefaultLib::fStringToNum);
    p->addFunction("cstr2num", QFMathParser_DefaultLib::fCStringToNum);



    p->addFunction("now", QFMathParser_DefaultLib::fNow);
    p->addFunction("datenum", QFMathParser_DefaultLib::fDateNum);
    p->addFunction("datetimenum", QFMathParser_DefaultLib::fDateTimeNum);
    p->addFunction("date2str", QFMathParser_DefaultLib::fDate2Str);
    p->addFunction("datetime2str", QFMathParser_DefaultLib::fDateTime2Str);
    p->addFunction("datediff2secs", QFMathParser_DefaultLib::fDatediff2Secs);
    p->addFunction("datediff2mins", QFMathParser_DefaultLib::fDatediff2Mins);
    p->addFunction("datediff2hours", QFMathParser_DefaultLib::fDatediff2Hours);
    p->addFunction("datediff2days", QFMathParser_DefaultLib::fDatediff2Days);



    p->addFunction("dateday", QFMathParser_DefaultLib::fDateDay);
    p->addFunction("datemonth", QFMathParser_DefaultLib::fDateMonth);
    p->addFunction("dateyear", QFMathParser_DefaultLib::fDateYear);
    p->addFunction("datehour", QFMathParser_DefaultLib::fDateHour);
    p->addFunction("datemin", QFMathParser_DefaultLib::fDateMin);
    p->addFunction("datesec", QFMathParser_DefaultLib::fDateSec);

    p->addFunction("varname", QFMathParser_DefaultLib::fVarname);




    QFMATHPARSER_REGISTER_FUNC(fextractfilename, extract_filename)
    QFMATHPARSER_REGISTER_FUNC(fextractfilebasename, extract_basename)

    QFMATHPARSER_REGISTER_FUNC(fextractfilefullbasenam, extract_complete_basename)
    QFMATHPARSER_REGISTER_FUNC(fextractfileext, extract_ext)
    QFMATHPARSER_REGISTER_FUNC(fextractfilefullext, extract_complete_ext)

    QFMATHPARSER_REGISTER_FUNC(fextractfilepath, extract_file_path)
    QFMATHPARSER_REGISTER_FUNC(fextractfileabspath, extract_absolute_path)
    QFMATHPARSER_REGISTER_FUNC(fextractfileabsfilepath, extract_absolute_file_path)


    QFMATHPARSER_REGISTER_FUNC(fReadFile, readfile)
    QFMATHPARSER_REGISTER_FUNC(fescapify, escapify)
    QFMATHPARSER_REGISTER_FUNC(fdeescapify, deescapify)
    QFMATHPARSER_REGISTER_FUNC(fcleanStringForFilename, clean_string_for_filename)


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












    void fRandVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
      bool ok=true;
      if (n>4 || n<=1) { ok=false; p->qfmpError(QObject::tr("randvec_seed accepts 2, 3 or 4 arguments")); }
      if (n>0 && (params[0].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randvec_seed needs a double as first argument (if any)")); }
      if (n>1 && (params[1].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randvec_seed needs a double as second argument (if any)")); }
      if (n>2 && (params[2].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randvec_seed needs a double as third argument (if any)")); }
      if (n>4 && (params[3].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randvec_seed needs a double as fourth argument (if any)")); }
      int items=params[0].toInteger();
      if (items<=0) { ok=false; p->qfmpError(QObject::tr("randvec_seed's first argument has to be a positive integer number")); }
      if (ok) {
          r.setDoubleVec(items, 0);

          if (n==2) {
              MTRand rng;
              rng.seed(params[1].num);
              for (int i=0; i<items; i++)
                r.numVec[i]=rng.rand();
          } else if (n==4) {
              MTRand rng;
              rng.seed(params[3].num);
              for (int i=0; i<items; i++)
                r.numVec[i]=rng.rand()*(params[2].num-params[1].num)+params[1].num;
          } else if (n==3){
              MTRand rng;
              rng.seed(params[2].num);
              for (int i=0; i<items; i++)
                r.numVec[i]=rng.rand(params[1].num);
          }
      } else {
        r.setInvalid();
      }
    }


    void fRandIntVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        bool ok=true;
        if (n>4 || n<=1) { ok=false; p->qfmpError(QObject::tr("randintvec_seed accepts 2, 3 or 4 arguments")); }
        if (n>0 && (params[0].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randintvec_seed needs a double as first argument (if any)")); }
        if (n>1 && (params[1].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randintvec_seed needs a double as second argument (if any)")); }
        if (n>2 && (params[2].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randintvec_seed needs a double as third argument (if any)")); }
        if (n>4 && (params[3].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randintvec_seed needs a double as fourth argument (if any)")); }
        int items=params[0].toInteger();
        if (items<=0) { ok=false; p->qfmpError(QObject::tr("randintvec_seed's first argument has to be a positive integer number")); }
        if (ok) {
            r.setDoubleVec(items, 0);

            if (n==2) {
                MTRand rng;
                rng.seed(params[1].num);
                for (int i=0; i<items; i++)
                  r.numVec[i]=rng.randInt();
            } else if (n==4) {
                MTRand rng;
                rng.seed(params[3].num);
                for (int i=0; i<items; i++)
                  r.numVec[i]=rng.randInt()*((uint32_t)params[2].num-(uint32_t)params[1].num)+(uint32_t)params[1].num;
            } else if (n==3){
                MTRand rng;
                rng.seed(params[2].num);
                for (int i=0; i<items; i++)
                  r.numVec[i]=rng.randInt((uint32_t)params[1].num);
            }
        } else {
          r.setInvalid();
        }
    }

    void fRandNormVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        bool ok=true;
        if (n>4 || n<=1) { ok=false; p->qfmpError(QObject::tr("randnormvec_seed accepts 2, 3 or 4 arguments")); }
        if (n>0 && (params[0].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randnormvec_seed needs a double as first argument (if any)")); }
        if (n>1 && (params[1].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randnormvec_seed needs a double as second argument (if any)")); }
        if (n>2 && (params[2].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randnormvec_seed needs a double as third argument (if any)")); }
        if (n>4 && (params[3].type!=qfmpDouble)) { ok=false;  p->qfmpError(QObject::tr("randnormvec_seed needs a double as fourth argument (if any)")); }
        int items=params[0].toInteger();
        if (items<=0) { ok=false; p->qfmpError(QObject::tr("randnormvec_seed's first argument has to be a positive integer number")); }
        if (ok) {
            MTRand rng;

            r.setDoubleVec(items, 0);
            double mean=0;
            double var=1;
            if (n==3) {
                var=params[1].num;
                rng.seed(params[2].num);
            } else if (n==2) {
                rng.seed(params[1].num);
            } else if (n>3) {
                mean=params[1].num;
                var=params[2].num;
                rng.seed(params[3].num);
            }

            for (int i=0; i<items; i++)
              r.numVec[i]=rng.randNorm(mean, var);
        } else {
          r.setInvalid();
        }

}



    void fRandBoolVecSeed(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        bool ok=true;
      if (n>3 || n<=1) { ok=false; p->qfmpError(QObject::tr("randboolvec_seed accepts 2 or 3 arguments")); }
      if (n>0 && (params[0].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randboolvec_seed needs a double as first argument (if any)")); }
      if (n>1 && (params[1].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randboolvec_seed needs a double as second argument (if any)")); }
      if (n>2 && (params[2].type!=qfmpDouble)) { ok=false; p->qfmpError(QObject::tr("randboolvec_seed needs a double as third argument (if any)")); }
      int items=params[0].toInteger();
      double pr=0.5;
      double seed=params[1].num;
      if (n>2) {
          pr=params[1].num;
          seed=params[2].num;
      }
      if (items<=0) { ok=false; p->qfmpError(QObject::tr("randboolvec_seed's first argument has to be a positive integer number")); }
      if (pr<0 || pr>1) { ok=false; p->qfmpError(QObject::tr("randboolvec_seed's second argument has to be a number between 0 and 1")); }
      if (ok) {
          r.setBoolVec(items, false);
          MTRand rng;
          rng.seed(seed);
          for (int i=0; i<items; i++)
            r.boolVec[i]=(rng.rand()<=pr);
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

    void fLength(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("length(x) need one argument"));
            r.setInvalid();
            return;
        }
        r.num=params[0].length();
    }

    void fRemove(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
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
            r.setInvalid();
            return;
        }
    }

    void fDot(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector && params[0].numVec.size()==params[1].numVec.size()) {
            double sum=0;
            for (int i=0; i<params[0].numVec.size(); i++) {
                sum+=params[0].numVec[i]*params[1].numVec[i];
            }
            r.setDouble(sum);
        } else {
            p->qfmpError(QObject::tr("dot(x, y) needs two number vector arguments of equal size"));
            r.setInvalid();
            return;
        }
    }

    void fConcat(qfmpResult &r,const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
        if (n>0) {
            if (params[0].type==qfmpDouble || params[0].type==qfmpDoubleVector) {
                r.type=qfmpDoubleVector;
                r.numVec.clear();
                for (unsigned int i=0; i<n; i++) {
                    if (params[i].convertsToVector()) {
                        r.numVec+=params[i].asVector();
                    } else {
                        p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more vectors or vectorelements as arguments (all have to have the same type)"));
                        r.setInvalid();
                        return;
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
                        r.setInvalid();
                        return;
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
                        r.setInvalid();
                        return;
                    }
                }
            }
        } else {
            p->qfmpError(QObject::tr("concat(x1, x2, ...) needs one or more vectors or vectorelements as arguments (all have to have the same type)"));
            r.setInvalid();
            return;
        }
    }

    void fRemoveAll(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
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
            r.setInvalid();
            return;
        }

    }


    void fFilterFinite(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
        if (n==1 && params[0].type==qfmpDoubleVector) {
            const QVector<double>& dat=params[0].numVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (QFFloatIsOK(dat[i])) {
                    r.numVec<<dat[i];
                }
            }
        } else {
            p->qfmpError(QObject::tr("filterfinite(x) need one number vector argument"));
            r.setInvalid();
            return;
        }
    }

    void fFind(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
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
            r.setInvalid();
            return;
        }
    }


    void fFindFirst(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {
            const QVector<double>& dat=params[0].numVec;
            r.type=qfmpDouble;
            r.num=-1;
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]==params[1].num) {
                    r.num=i;
                    break;
                }
            }
        } else if (n==2 && params[0].type==qfmpBoolVector && params[1].type==qfmpBool) {
            const QVector<bool>& dat=params[0].boolVec;
            r.type=qfmpDoubleVector;
            r.type=qfmpDouble;
            r.num=-1;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]==params[1].boolean) {
                    r.num=i;
                    break;
                }
            }
        } else if (n==1 && params[0].type==qfmpBoolVector) {
            const QVector<bool>& dat=params[0].boolVec;
            r.type=qfmpDoubleVector;
            r.type=qfmpDouble;
            r.num=-1;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]) {
                    r.num=i;
                    break;
                }
            }
        } else if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpDoubleVector;
            r.type=qfmpDouble;
            r.num=-1;
            for (int i=0; i<dat.size(); i++) {
                if (dat[i]==params[1].str) {
                    r.num=i;
                    break;
                }
            }
        } else  if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {
            const QVector<double>& dat=params[0].numVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int j=0; j<params[1].numVec.size(); j++) {
                int add=-1;
                double v=params[1].numVec[j];
                for (int i=0; i<dat.size(); i++) {
                    if (dat[i]==v) {
                        add=i;
                        break;
                    }
                }
                r.numVec.append(add);
            }
        } else  if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpStringVector) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int j=0; j<params[1].strVec.size(); j++) {
                int add=-1;
                QString v=params[1].strVec[j];
                for (int i=0; i<dat.size(); i++) {
                    if (dat[i]==v) {
                        add=i;
                        break;
                    }
                }
                r.numVec.append(add);
            }
        } else  if (n==2 && params[0].type==qfmpBoolVector && params[1].type==qfmpBoolVector) {
            const QVector<bool>& dat=params[0].boolVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int j=0; j<params[1].numVec.size(); j++) {
                int add=-1;
                bool v=params[1].boolVec[j];
                for (int i=0; i<dat.size(); i++) {
                    if (dat[i]==v) {
                        add=i;
                        break;
                    }
                }
                r.numVec.append(add);
            }
        } else {
            p->qfmpError(QObject::tr("findfirst(x, value=true) needs two arguments: one vector x and a corresponding element value (or vector), if only a boolean vector is given, the function uses value=true"));
            r.setInvalid();
            return;
        }
    }


    void fContains(qfmpResult& r, const qfmpResult *params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble) {
            r.type=qfmpBool;
            r.boolean=params[0].numVec.contains(params[1].num);
            r.isValid=true;
        } else if (n==2 && params[0].type==qfmpBoolVector && params[1].type==qfmpBool) {
            r.type=qfmpBool;
            r.boolean=params[0].boolVec.contains(params[1].boolean);
            r.isValid=true;
        } else if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            r.type=qfmpBool;
            r.boolean=params[0].strVec.contains(params[1].str);
            r.isValid=true;
        } else if (n==2 && params[0].type==qfmpString && params[1].type==qfmpString) {
            const QString& dat=params[0].str;
            r.type=qfmpBool;
            r.boolean=dat.contains(params[1].str);
        } else {
            p->qfmpError(QObject::tr("contains(x, value) needs two arguments: one vector x and a corresponding element value (or vector)"));
            r.setInvalid();
            return;
        }
    }

    void fContainsSubString(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();

        if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpBoolVector;
            r.boolVec.clear();
            for (int i=0; i<dat.size(); i++) {
                bool f=false;
                if (dat[i].contains(params[1].str)) {
                    f=true;
                    break;
                }
                r.boolVec.append(f);
            }
        } else if (n==2 && params[0].type==qfmpString && params[1].type==qfmpString) {
            const QString& dat=params[0].str;
            r.type=qfmpBool;
            r.boolean=dat.contains(params[1].str);
        } else  if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpStringVector) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                int add=-1;
                for (int j=0; j<params[1].strVec.size(); j++) {
                    QString v=params[1].strVec[j];
                    if (dat[i].contains(v)) {
                        add=j;
                        break;
                    }
                }
                r.numVec.append(add);
            }
        } else  if (n==2 && params[0].type==qfmpString && params[1].type==qfmpStringVector) {
            r.type=qfmpDouble;
            r.num=-1;
            r.isValid=true;
            for (int j=0; j<params[1].strVec.size(); j++) {
                if (params[0].str.contains(params[1].strVec[j])) {
                    r.num=j;
                    break;
                }
            }
        }  else {
            p->qfmpError(QObject::tr("containssubstr(x, value) needs two arguments: stringVec/string, string/string or stringvec/string_vec"));
            r.setInvalid();
            return;
        }
    }

    void fSelect(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
        if (n==2 && params[0].type==qfmpDoubleVector && params[1].type==qfmpBoolVector) {
            const QVector<double>& dat=params[0].numVec;
            if (dat.size()!=params[1].boolVec.size()) {
                p->qfmpError(QObject::tr("find(x, criterion) vectors x and criterion have different size"));
                r.setInvalid();
                return ;
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
                r.setInvalid();
                return ;
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
                r.setInvalid();
                return ;
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
            r.setInvalid();
            return;
        }
    }
     void fReverse(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();
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
            r.setInvalid();
            return ;
        }
    }



    void fUnique(qfmpResult &res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        res.setInvalid();

        if (n!=1) {
            p->qfmpError("unique(data) needs 1 argument");
            return ;
        }
        if ((params[0].type==qfmpDouble) || (params[0].type==qfmpString) || (params[0].type==qfmpBool)) {
            res=params[0];
            return;
        }

        if (params[0].type==qfmpDoubleVector) {
            QVector<double> out;
            const QVector<double>& in=params[0].numVec;

            for (int i=0; i<in.size(); i++) {
                if (!out.contains(in[i])) out<<in[i];
            }

            res.setDoubleVec(out);
            return;
        }

        if (params[0].type==qfmpStringVector) {
            QStringList out;
            const QStringList& in=params[0].strVec;

            for (int i=0; i<in.size(); i++) {
                if (!out.contains(in[i])) out<<in[i];
            }

            res.setStringVec(out);
            return;
        }

        if (params[0].type==qfmpBoolVector) {
            QVector<bool> out;
            const QVector<bool>& in=params[0].boolVec;

            for (int i=0; i<in.size(); i++) {
                if (!out.contains(in[i])) out<<in[i];
                if (out.size()>1) break;
            }

            res.setBoolVec(out);
            return;
        }
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
                return qfmpResult(qfUniqueApplyFunction<double, double, qfDoubleVectorToDoubleFunc>(d, params[1].asVector(), qfstatisticsStd<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction<double, bool, qfDoubleVectorToDoubleFunc>(d, params[1].asBoolVector(), qfstatisticsStd<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction<double, QString, qfDoubleVectorToDoubleFunc>(d, params[1].asStrVector(), qfstatisticsStd<QVector<double> >));
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



    qfmpResult fIndexedMAD(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedmad(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsMADS<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsMADS<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsMADS<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedmad(data, index) needs a number vector as data argument and an equal sized index array of any type");
        }

        return res;
    }

    qfmpResult fIndexedNMAD(const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        qfmpResult res=qfmpResult::invalidResult();

        if (n!=2) p->qfmpError("indexedmad(data, index) needs 2 argument");
        else if (((params[0].type==qfmpDoubleVector)||(params[0].type==qfmpDouble)) && params[0].length()==params[1].length()) {
            QVector<double> d=params[0].asVector();

            if (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asVector(), qfstatisticsNMADS<QVector<double> >));
            }
            if (params[1].type==qfmpBool || params[1].type==qfmpBoolVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asBoolVector(), qfstatisticsNMADS<QVector<double> >));
            }
            if (params[1].type==qfmpString || params[1].type==qfmpStringVector) {
                return qfmpResult(qfUniqueApplyFunction(d, params[1].asStrVector(), qfstatisticsNMADS<QVector<double> >));
            }
        } else {
            p->qfmpError("indexedmad(data, index) needs a number vector as data argument and an equal sized index array of any type");
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






    void fRegExpCapture(qfmpResult& r, const qfmpResult *params, unsigned int n, QFMathParser *p, bool minimal, Qt::CaseSensitivity casesens)
    {
        r.setInvalid();
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
                rx.setMinimal(minimal);
                rx.setCaseSensitivity(casesens);
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
                    r.setString(params[1].str);
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

    void fRegExpContains(qfmpResult& r, const qfmpResult *params, unsigned int n, QFMathParser *p, bool minimal, Qt::CaseSensitivity casesens)
    {
        if (n!=2) {\
            p->qfmpError(QObject::tr("regexpcontains(regexp, strings) needs 2 arguments"));\
            r.setInvalid();\
            return; \
        }\
        if(params[1].type==qfmpStringVector || params[1].type==qfmpString) {\
            if(params[0].type==qfmpString) {\
                QRegExp rx(params[0].str);
                rx.setMinimal(minimal);
                rx.setCaseSensitivity(casesens);
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

    void fRegExpIndexIn(qfmpResult& r, const qfmpResult *params, unsigned int n, QFMathParser *p, bool minimal, Qt::CaseSensitivity casesens)
    {
        if (n!=2) {\
            p->qfmpError(QObject::tr("regexpindexin(regexp, strings) needs 2 arguments"));\
            r.setInvalid();\
            return; \
        }\
        if(params[1].type==qfmpStringVector || params[1].type==qfmpString) {\
            if(params[0].type==qfmpString) {\
                QRegExp rx(params[0].str);
                rx.setMinimal(minimal);
                rx.setCaseSensitivity(casesens);
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


    void fTrapz(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        res.setInvalid();

        if (n==1) {
            if (params[0].type==qfmpDoubleVector) {
                res.setDouble(qfstatisticsTrapz(params[0].numVec));
            } else {
                p->qfmpError("trapz(Y) needs 1 vector of numbers");
            }

        } else if (n==2) {
            if (params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector) {
                res.setDouble(qfstatisticsTrapzXY(params[0].numVec, params[1].numVec));
            } else {
                p->qfmpError("trapz(X,Y) needs 2 vectors of numbers");
            }
        } else {
            p->qfmpError("trapz(Y)/trapz(X,Y) needs 1 or 2 argument");
        }
    }





    void fDateNum(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datenum";
        r.setInvalid();
        if (n<1||n>2) {
            p->qfmpError(QObject::tr("%1(date[, formatstring]) needs 1 or 2 arguments").arg(iname));
            return;
        }
        if (n>=1) {
            QString format="yyyy-MM-dd";
            if (n==2 && params[1].type==qfmpString) {
                format=params[1].str;
            } else if (n==2) {
                p->qfmpError(QObject::tr("%1(x, formatstring) 2nd argument has to be a string").arg(iname));
                return;
            }

            if(params[0].type==qfmpString) {
                r.setDouble(QDateTime::fromString(params[0].str, format).toMSecsSinceEpoch());
                return;
            } else if(params[0].type==qfmpStringVector) {
                r.setDoubleVec(params[0].strVec.size());
                for (int i=0; i<params[0].strVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromString(params[0].strVec[i], format).toMSecsSinceEpoch();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a string or string vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDateTimeNum(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datetimenum";
        r.setInvalid();
        if (n<1||n>2) {
            p->qfmpError(QObject::tr("%1(date[, formatstring]) needs 1 or 2 arguments").arg(iname));
            return;
        }
        if (n>=1) {
            QDateTime dat;
            QString format="yyyy-MM-ddTHH:mm:ss";
            if (n==2 && params[1].type==qfmpString) {
                format=params[1].str;
                return;
            } else if (n==2) {
                p->qfmpError(QObject::tr("%1(x, formatstring) 2nd argument has to be a string").arg(iname));
                return;
            }

            if(params[0].type==qfmpString) {
                //qDebug()<<params[0].str<<QDateTime::fromString(params[0].str, format)<<format;
                r.setDouble(QDateTime::fromString(params[0].str, format).toMSecsSinceEpoch());
                return;
            } else if(params[0].type==qfmpStringVector) {
                r.setDoubleVec(params[0].strVec.size());
                for (int i=0; i<params[0].strVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromString(params[0].strVec[i], format).toMSecsSinceEpoch();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a string or string vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }


    void fDate2Str(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="date2str";
        r.setInvalid();
        if (n<1||n>2) {
            p->qfmpError(QObject::tr("%1(date[, formatstring]) needs 1 or 2 arguments").arg(iname));
            return;
        }
        if (n>=1) {
            QDateTime dat;
            QString format="yyyy-MM-dd";
            if (n==2 && params[1].type==qfmpString) {
                format=params[1].str;
            } else if (n==2) {
                p->qfmpError(QObject::tr("%1(x, formatstring) 2nd argument has to be a string").arg(iname));
                return;
            }

            if(params[0].type==qfmpDouble) {
                r.setString(QDateTime::fromMSecsSinceEpoch(params[0].num).toString(format));
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setStringVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.strVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).toString(format);
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }


    void fDateTime2Str(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="date2str";
        r.setInvalid();
        if (n<1||n>2) {
            p->qfmpError(QObject::tr("%1(date[, formatstring]) needs 1 or 2 arguments").arg(iname));
            return;
        }
        if (n>=1) {
            QDateTime dat;
            QString format="yyyy-MM-ddTHH:mm:ss";
            if (n==2 && params[1].type==qfmpString) {
                format=params[1].str;
            } else if (n==2) {
                p->qfmpError(QObject::tr("%1(x, formatstring) 2nd argument has to be a string").arg(iname));
                return;
            }

            if(params[0].type==qfmpDouble) {
                r.setString(QDateTime::fromMSecsSinceEpoch(params[0].num).toString(format));
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setStringVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.strVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).toString(format);
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDatediff2Secs(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datediff2secs";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(params[0].num/1e3);
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=params[0].numVec[i]/1e3;
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDatediff2Mins(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datediff2mins";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(params[0].num/1e3/60.0);
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=params[0].numVec[i]/1e3/60.0;
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDatediff2Hours(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datediff2hours";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(params[0].num/1e3/60.0/60.0);
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=params[0].numVec[i]/1e3/60.0/60.0;
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDatediff2Days(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datediff2hours";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(params[0].num/1e3/60.0/60.0/24.0);
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=params[0].numVec[i]/1e3/60.0/60.0/24.0;
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fNow(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="now";
        r.setInvalid();
        if (n!=0) {
            p->qfmpError(QObject::tr("%1() needs no arguments").arg(iname));
            return;
        }
        r.setDouble(QDateTime::currentDateTime().toMSecsSinceEpoch());
    }


    void fDateDay(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="dateday";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(QDateTime::fromMSecsSinceEpoch(params[0].num).date().day());
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).date().day();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDateMonth(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datemonth";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(QDateTime::fromMSecsSinceEpoch(params[0].num).date().month());
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).date().month();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDateYear(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="dateyear";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(QDateTime::fromMSecsSinceEpoch(params[0].num).date().year());
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).date().year();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }

    void fDateHour(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datehour";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(QDateTime::fromMSecsSinceEpoch(params[0].num).time().hour());
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).time().hour();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }
    void fDateMin(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datemin";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(QDateTime::fromMSecsSinceEpoch(params[0].num).time().minute());
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).time().minute();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }


    void fDateSec(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        QString iname="datesec";
        r.setInvalid();
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(date) needs 1 argument").arg(iname));
            return;
        }
        if (n>=1) {

            if(params[0].type==qfmpDouble) {
                r.setDouble(QDateTime::fromMSecsSinceEpoch(params[0].num).time().second());
                return ;
            } else if(params[0].type==qfmpDoubleVector) {
                r.setDoubleVec(params[0].numVec.size());
                for (int i=0; i<params[0].numVec.size(); i++) {
                    r.numVec[i]=QDateTime::fromMSecsSinceEpoch(params[0].numVec[i]).time().second();
                }
                return;
            } else {
                p->qfmpError(QObject::tr("%1(x) argument has to be a double or double vector").arg(iname));
                return;
            }
            return;
        }
        return;
    }


    void fVarname(qfmpResult& r,  QFMathParser::qfmpNode** nodes, unsigned int n, QFMathParser* p) {
        //qDebug()<<"fVarname "<<nodes<<n<<p;
        QString iname="varname";
        r.setInvalid();
        if (n<=0) {
            p->qfmpError(QObject::tr("%1(...) needs at least 1 argument").arg(iname));
            return;
        }
        if (n>=1) {
            r.setStringVec(n, "");
            for (unsigned int i=0; i<n; i++) {
                QFMathParser::qfmpVariableNode* vn=dynamic_cast<QFMathParser::qfmpVariableNode*>(nodes[i]);
                if (vn) {
                    r.strVec[i]=vn->getName();
                } else {
                    r.setInvalid();
                    p->qfmpError(QObject::tr("%1(...) needs variable names as input arguments, %2-th argument is of different type (%3)").arg(iname).arg(i+1).arg(typeid(*nodes[i]).name()));
                    return;
                }
            }
            return;
        }
        return;
    }



}
