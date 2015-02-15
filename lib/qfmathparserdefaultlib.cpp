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
#include "statistics_tools.h"

#ifdef QFLIB_LIBRARY
#  include "qfpluginservices.h"
#  include "qfproject.h"
#  include "qfrawdatarecord.h"
#  include "qfevaluationitem.h"
#  include "qfrdrtableinterface.h"
#  include "qfrdrimagemask.h"
#  include "qfrdrrunselection.h"
#  include "qfrdrimageselectioninterface.h"
#  include "qfrdrimagetoruninterface.h"
#  include "qffitfunction.h"
#  include "qffitfunctionmanager.h"
#  include "qffitalgorithm.h"
#  include "qffitalgorithmmanager.h"
#  include "qffitfunctionparsed.h"
#endif


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

    p->addVariableString("stdlib_version", QString("0.4 (p%1)").arg(QFMATHPARSER_VERSION));


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
    p->addFunction("simplify", QFMathParser_DefaultLib::fSimplify);
    p->addFunction("split", QFMathParser_DefaultLib::fSplit);
    p->addFunction("join", QFMathParser_DefaultLib::fJoin);
    p->addFunction("removeemtystrings", QFMathParser_DefaultLib::fRemoveEmpty);

    p->addFunction("toupper", QFMathParser_DefaultLib::fToUpper);
    p->addFunction("length", QFMathParser_DefaultLib::fLength);
    p->addFunction("size", QFMathParser_DefaultLib::fLength);
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
    p->addFunction("mandersoverlap", QFMathParser_DefaultLib::fMandersOverlapCoeff);
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
    p->addFunction("containssubstr_caseinsensitive", QFMathParser_DefaultLib::fContainsSubStringLC);
    p->addFunction("contains_caseinsensitive", QFMathParser_DefaultLib::fContainsLC);
    p->addFunction("replace", QFMathParser_DefaultLib::fReplace);
    p->addFunction("multireplace", QFMathParser_DefaultLib::fReplaceMulti);

    p->addFunction("isnan", QFMathParser_DefaultLib::fIsNan);
    p->addFunction("isinf", QFMathParser_DefaultLib::fIsInf);
    p->addFunction("isfinite", QFMathParser_DefaultLib::fIsFinite);
    p->addFunction("isnumok", QFMathParser_DefaultLib::fIsFloatOK);
    p->addFunction("isvalid", QFMathParser_DefaultLib::fIsValid);
    p->addFunction("isvoid", QFMathParser_DefaultLib::fIsVoid);
    p->addFunction("isint", QFMathParser_DefaultLib::fIsInt);
    p->addFunction("isuint", QFMathParser_DefaultLib::fIsUInt);
    p->addFunction("isbool", QFMathParser_DefaultLib::fIsBool);
    p->addFunction("isstring", QFMathParser_DefaultLib::fIsString);
    p->addFunction("isnumber", QFMathParser_DefaultLib::fIsDouble);
    p->addFunction("isdouble", QFMathParser_DefaultLib::fIsDouble);
    p->addFunction("isdoublevector", QFMathParser_DefaultLib::fIsDoubleVector);
    p->addFunction("isnumbervector", QFMathParser_DefaultLib::fIsDoubleVector);
    p->addFunction("isboolvector", QFMathParser_DefaultLib::fIsBoolVector);
    p->addFunction("isstringvector", QFMathParser_DefaultLib::fIsStringVector);

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
    p->addFunction("returnnth", QFMathParser_DefaultLib::fReturnNth);
    p->addFunction("returnfirst", QFMathParser_DefaultLib::fReturnFirst);
    p->addFunction("returnfirstvalid", QFMathParser_DefaultLib::fReturnFirstValid);
    p->addFunction("returnlastvalid", QFMathParser_DefaultLib::fReturnLastValid);
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
    p->addFunction("int", QFMathParser_DefaultLib::fInt);
    p->addFunction("num", QFMathParser_DefaultLib::fNum);
    p->addFunction("double", QFMathParser_DefaultLib::fNum);
    p->addFunction("bool", QFMathParser_DefaultLib::fBool);
    p->addFunction("strvec", QFMathParser_DefaultLib::fStrVec);
    p->addFunction("boolvec", QFMathParser_DefaultLib::fBoolVec);
    p->addFunction("numvec", QFMathParser_DefaultLib::fNumVec);
    p->addFunction("intvec", QFMathParser_DefaultLib::fIntVec);
    p->addFunction("ensuresize", QFMathParser_DefaultLib::fEnsureVectorSize);
    p->addFunction("ensuresize_start", QFMathParser_DefaultLib::fEnsureVectorSizeStart);
    p->addFunction("histogram", QFMathParser_DefaultLib::fHistogram);
    p->addFunction("histogrambins", QFMathParser_DefaultLib::fHistogramBins);
    p->addFunction("rangedhistogram", QFMathParser_DefaultLib::fRangedHistogram);
    p->addFunction("rangedhistogrambins", QFMathParser_DefaultLib::fRangedHistogramBins);

    p->addFunction("regression", QFMathParser_DefaultLib::fRegression);
    p->addFunction("polyfit", QFMathParser_DefaultLib::fPolyFit);
    p->addFunction("weighted_regression", QFMathParser_DefaultLib::fWeightedRegression);
    p->addFunction("irls", QFMathParser_DefaultLib::fIRLS);
    p->addFunction("erroradd", QFMathParser_DefaultLib::fErrorAdd);
    p->addFunction("errorasub", QFMathParser_DefaultLib::fErrorSub);
    p->addFunction("errormul", QFMathParser_DefaultLib::fErrorMul);
    p->addFunction("errordiv", QFMathParser_DefaultLib::fErrorDiv);
    p->addFunction("errorpow", QFMathParser_DefaultLib::fErrorPow);

#ifdef QFLIB_LIBRARY
    p->addFunction("rdr_ids", QFMathParser_DefaultLib::fRDRIDs);
    p->addFunction("rdr_getresult", QFMathParser_DefaultLib::fRDRGetResult);
    p->addFunction("rdr_getresulterror", QFMathParser_DefaultLib::fRDRGetResultError);
    p->addFunction("rdr_getresultids", QFMathParser_DefaultLib::fRDRGetResultIDs);
    p->addFunction("rdr_getevaluationids", QFMathParser_DefaultLib::fRDRGetEvaluationIDs);
    p->addFunction("rdr_getname", QFMathParser_DefaultLib::fRDRGetName);
    p->addFunction("rdr_getfolder", QFMathParser_DefaultLib::fRDRGetFolder);
    p->addFunction("rdr_getgroup", QFMathParser_DefaultLib::fRDRGetGroup);
    p->addFunction("rdr_getrole", QFMathParser_DefaultLib::fRDRGetRole);
    p->addFunction("rdr_getdescription", QFMathParser_DefaultLib::fRDRGetDescription);
    p->addFunction("rdr_getproperty", QFMathParser_DefaultLib::fRDRGetProperty);
    p->addFunction("rdr_gettype", QFMathParser_DefaultLib::fRDRGetType);
    p->addFunction("rdr_getfiles", QFMathParser_DefaultLib::fRDRGetFiles);
    p->addFunction("rdr_getfiletypes", QFMathParser_DefaultLib::fRDRGetFileTypes);
    p->addFunction("rdr_getfiledescriptions", QFMathParser_DefaultLib::fRDRGetFileDescriptions);

    p->addFunction("eval_ids", QFMathParser_DefaultLib::fEvalIDs);
    p->addFunction("eval_getname", QFMathParser_DefaultLib::fEvalGetName);
    p->addFunction("eval_getdescription", QFMathParser_DefaultLib::fEvalGetDescription);
    p->addFunction("eval_getproperty", QFMathParser_DefaultLib::fEvalGetProperty);
    p->addFunction("eval_gettype", QFMathParser_DefaultLib::fEvalGetType);

    p->addFunction("project_getproperty", QFMathParser_DefaultLib::fProjectGetProperty);


    p->addFunction("table_column", fTable_column);
    p->addFunction("table_data", fTable_data);
    p->addFunction("table_numcolumn", fTable_numcolumn);
    p->addFunction("table_numdata", fTable_numdata);
    p->addFunction("table_cell", fTable_data);
    p->addFunction("table_columns", fTable_columns);
    p->addFunction("table_columntitles", fTable_columntitles);
    p->addFunction("table_rows", fTable_rows);
    p->addFunction("rdr_istable", fRDR_istable);


    p->addFunction("rdr_isoverview", fRDR_isoverview);
    p->addFunction("overview_width", fRDR_overviewwidth);
    p->addFunction("overview_height", fRDR_overviewheight);
    p->addFunction("overview_size", fRDR_overviewsize);
    p->addFunction("overview_channels", fRDR_overviewchannels);
    p->addFunction("overview_image", fRDR_overview);


    p->addFunction("rdr_ismask", fRDR_ismask);
    p->addFunction("mask_width", fRDR_maskwidth);
    p->addFunction("mask_height", fRDR_maskheight);
    p->addFunction("mask_size", fRDR_masksize);
    p->addFunction("mask_image", fRDR_mask);
    p->addFunction("mask_get", fRDR_mask);

    p->addFunction("rdr_isrunexclude", fRDR_isrunex);
    p->addFunction("rdr_isrunex", fRDR_isrunex);
    p->addFunction("runex_runs", fRDR_runex_runs);
    p->addFunction("runex_isexcluded", fRDR_runex_isexcluded);
    p->addFunction("runex_excluded", fRDR_runex_excluded);
    p->addFunction("runex_included", fRDR_runex_included);
    p->addFunction("runex_mask", fRDR_runex_mask);



    p->addFunction("rdr_isimageselection", fRDR_isimageselection);
    p->addFunction("imageselection_width", fRDR_imageselectionwidth);
    p->addFunction("imageselection_height", fRDR_imageselectionheight);
    p->addFunction("imageselection_size", fRDR_imageselectionsize);
    p->addFunction("imageselection_count", fRDR_imageselectioncount);
    p->addFunction("imageselection_names", fRDR_imageselectionnames);
    p->addFunction("imageselection_image", fRDR_imageselection);
    p->addFunction("imageselection_get", fRDR_imageselection);
    p->addFunction("imageselection_lastofname", fRDR_imageselectionlastofname);
    p->addFunction("imageselection_firstofname", fRDR_imageselectionfirstofname);

    p->addFunction("fitfunction_parameternames", fFitFunctionParameters);
    p->addFunction("fitfunction_isfitparameters", fFitFunctionIsFit);
    p->addFunction("fitfunction_init", fFitFunctionInit);
    p->addFunction("fitfunction_initfix", fFitFunctionInitFix);
    p->addFunction("fitfunction_ids", fFitFunctionIDs);
    p->addFunction("fitfunction_evaluate", fFitFunctionEval);
    p->addFunction("fitfunction_paramcount", fFitFunctionParamCount);
    p->addFunction("fitfunction_calcparameters", fFitFunctionCalc);
    p->addFunction("fitfunction_estimateparameters", fFitFunctionEstimate);
    p->addFunction("fitfunction_fit", fFitFunctionFit);

    p->addFunction("fitalgorithm_ids", fFitAlgorithmsIDs);

    p->addFunction("fit", fFit);
#endif



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





    /*QVector<double> QFMathParser_shuffleD(const QVector<double>& value) {
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
    }*/

    void fShuffle(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("shuffle"));
            r.setInvalid();
            return;
        }
        if(params[0].type==qfmpDoubleVector) {
            //r.setDoubleVec(QFMathParser_shuffleD(params[0].numVec));
            r.setDoubleVec(params[0].numVec);
            for (int i=r.numVec.size(); i>=1; i--) {
                int j=p->get_rng()->randInt(i);
                qSwap(r.numVec[i], r.numVec[j]);
            }
        } else if(params[0].type==qfmpStringVector) {
            //r.setStringVec(QFMathParser_shuffleS(params[0].strVec));
            r.setStringVec(params[0].strVec);
            for (int i=r.strVec.size(); i>=1; i--) {
                int j=p->get_rng()->randInt(i);
                qSwap(r.strVec[i], r.strVec[j]);
            }
        } else if(params[0].type==qfmpBoolVector) {
            //r.setBoolVec(QFMathParser_shuffleB(params[0].boolVec));
            r.setBoolVec(params[0].boolVec);
            for (int i=r.boolVec.size(); i>=1; i--) {
                int j=p->get_rng()->randInt(i);
                qSwap(r.boolVec[i], r.boolVec[j]);
            }
        } else {
            p->qfmpError(QObject::tr("%1(x) argument has to be a vector of numbers/booleans/strings").arg("shuffle"));
            r.setInvalid();
        }
        return;
    }


    void fIsValid(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isvalid"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].isValid);
    }

    void fIsVoid(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isvoid"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpVoid);
    }


    void fIsInt(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isint"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].isInteger());

    }

    void fIsUInt(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isuint"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].isUInt());


    }

    void fIsDouble(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isdouble"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpDouble);

    }

    void fIsString(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isstring"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpString);
    }

    void fIsBool(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isbool"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpBool);
    }

    void fIsVector(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isvector"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpDoubleVector || params[0].type==qfmpBoolVector || params[0].type==qfmpStringVector);
    }

    void fIsBoolVector(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isboolvector"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpBoolVector);
    }

    void fIsDoubleVector(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isdoublevector"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpDoubleVector);
    }

    void fIsStringVector(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        if (n!=1) {
            p->qfmpError(QObject::tr("%1(x) needs exacptly 1 argument").arg("isstringbool"));
            r.setInvalid();
            return;
        }
        r.setBoolean(params[0].type==qfmpStringVector);
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
      if (n>=1 && params[0].type!=qfmpDouble) p->qfmpError(QObject::tr("srand needs double argument"));
      r.num=0;
      //srand((unsigned int)params[0].num);
      if (n>=1) p->get_rng()->seed(uint32_t(params[0].num));
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
        r.setDouble(params[0].length());
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

    void fContainsLC(qfmpResult& r, const qfmpResult *params, unsigned int  n, QFMathParser* p){
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
            r.boolean=params[0].strVec.contains(params[1].str, Qt::CaseInsensitive);
            r.isValid=true;
        } else if (n==2 && params[0].type==qfmpString && params[1].type==qfmpString) {
            const QString& dat=params[0].str;
            r.type=qfmpBool;
            r.boolean=dat.contains(params[1].str, Qt::CaseInsensitive);
        } else {
            p->qfmpError(QObject::tr("contains_caseinsensitive(x, value) needs two arguments: one vector x and a corresponding element value (or vector)"));
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

    void fContainsSubStringLC(qfmpResult& r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setInvalid();

        if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpString) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpBoolVector;
            r.boolVec.clear();
            for (int i=0; i<dat.size(); i++) {
                bool f=false;
                if (dat[i].contains(params[1].str, Qt::CaseInsensitive)) {
                    f=true;
                    break;
                }
                r.boolVec.append(f);
            }
        } else if (n==2 && params[0].type==qfmpString && params[1].type==qfmpString) {
            const QString& dat=params[0].str;
            r.type=qfmpBool;
            r.boolean=dat.contains(params[1].str, Qt::CaseInsensitive);
        } else  if (n==2 && params[0].type==qfmpStringVector && params[1].type==qfmpStringVector) {
            const QStringList& dat=params[0].strVec;
            r.type=qfmpDoubleVector;
            r.numVec.clear();
            r.isValid=true;
            for (int i=0; i<dat.size(); i++) {
                int add=-1;
                for (int j=0; j<params[1].strVec.size(); j++) {
                    QString v=params[1].strVec[j];
                    if (dat[i].contains(v, Qt::CaseInsensitive)) {
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
                if (params[0].str.contains(params[1].strVec[j], Qt::CaseInsensitive)) {
                    r.num=j;
                    break;
                }
            }
        }  else {
            p->qfmpError(QObject::tr("containssubstr_caseinsensitive(x, value) needs two arguments: stringVec/string, string/string or stringvec/string_vec"));
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

    void fReturnNth(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
        res.setInvalid();

        if (n<2) {
            p->qfmpError("returnnth(n,x1,x2,x3,...) needs at least two arguments");
            res.setInvalid();
            return;
        }
        int d=0;
        if (params[0].isInteger()) {
            d=params[0].toInteger();
        } else {
            p->qfmpError("returnnth(n,x1,x2,x3,...) needs an integer as first argument");
            res.setInvalid();
            return;
        }
        if (d<0 || d>=(long long)n-1) {
            p->qfmpError("returnnth(n,x0,x1,x2,...) parameter n has to index one of the elements x0,x1,...");
            res.setInvalid();
            return;
        }
        res=params[d+1];
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
        } else if (idxb.size()>0) {
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
                    for (int i=0; i<params[1].strVec.size(); i++) {
                        bv.append(rx.indexIn(params[1].strVec[i])>=0);
                    }
                    r.setBoolVec(bv);
                } else if (params[1].type==qfmpString) {
                    r.setBoolean(rx.indexIn(params[1].str)>=0);
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
                    for (int i=0; i<params[1].strVec.size(); i++) {
                        bv.append(rx.indexIn(params[1].strVec[i]));
                    }
                    r.setDoubleVec(bv);
                } else if (params[1].type==qfmpString) {
                    r.setDouble(rx.indexIn(params[1].str));
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

    void fReplace(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        r.setInvalid();
        if (n>0) r.set(params[0]);
        if (n==3 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble && params[2].type==qfmpDouble) {
            for (int i=0; i<r.numVec.size(); i++) {
                if (r.numVec[i]==params[1].num) r.numVec[i]=params[2].num;
            }
        } else if (n==3 && params[0].type==qfmpStringVector && params[1].type==qfmpString && params[2].type==qfmpString) {
            for (int i=0; i<r.strVec.size(); i++) {
                if (r.strVec[i]==params[1].str) r.strVec[i]=params[2].str;
            }
        } else if (n==3 && params[0].type==qfmpBoolVector && params[1].type==qfmpBool && params[2].type==qfmpBool) {
            for (int i=0; i<r.boolVec.size(); i++) {
                if (r.boolVec[i]==params[1].boolean) r.boolVec[i]=params[2].boolean;
            }
        } else if (n==3 && params[0].type==qfmpString && params[1].type==qfmpString && params[2].type==qfmpString) {
            r.str=r.str.replace(params[1].str, params[2].str);
        } else {
            p->qfmpError(QObject::tr("replace(x, old_value,new_value) needs 3 arguments: one vector x and two corresponding element values, or 3 strings"));
            r.setInvalid();
            return;
        }
    }

    void fReplaceMulti(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        const QString errorm=QObject::tr("multireplace(x, old_value,new_value,old2,new2,...) needs 3,5,7,... arguments: one vector x and pairs of corresponding element values, or 3/5/7/... strings");
        r.setInvalid();
        if (n>0) r.set(params[0]);
        if (n>=3 && n%2==1 && params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble && params[2].type==qfmpDouble) {
            for (unsigned int j=1; j<n; j+=2) {
                if (params[j].type!=qfmpDouble || params[j+1].type!=qfmpDouble) {
                    p->qfmpError(errorm);
                    r.setInvalid();
                    return;
                }
                for (int i=0; i<r.numVec.size(); i++) {
                    if (r.numVec[i]==params[j].num) r.numVec[i]=params[j+1].num;
                }
            }
        } else if (n>=3 && n%2==1 && params[0].type==qfmpStringVector && params[1].type==qfmpString && params[2].type==qfmpString) {
            for (unsigned int j=1; j<n; j+=2) {
                if (params[j].type!=qfmpString || params[j+1].type!=qfmpString) {
                    p->qfmpError(errorm);
                    r.setInvalid();
                    return;
                }
                for (int i=0; i<r.strVec.size(); i++) {
                    if (r.strVec[i]==params[j].str) r.strVec[i]=params[j+1].str;
                }
            }
        } else if (n>=3 && n%2==1 && params[0].type==qfmpBoolVector && params[1].type==qfmpBool && params[2].type==qfmpBool) {
            for (unsigned int j=1; j<n; j+=2) {
                if (params[j].type!=qfmpBool || params[j+1].type!=qfmpBool) {
                    p->qfmpError(errorm);
                    r.setInvalid();
                    return;
                }
                for (int i=0; i<r.boolVec.size(); i++) {
                    if (r.boolVec[i]==params[j].boolean) r.boolVec[i]=params[j+1].boolean;
                }
            }
        } else if (n>=3 && n%2==1 && params[0].type==qfmpString && params[1].type==qfmpString && params[2].type==qfmpString) {
            for (unsigned int j=1; j<n; j+=2) {
                if (params[j].type!=qfmpString || params[j+1].type!=qfmpString) {
                    p->qfmpError(errorm);
                    r.setInvalid();
                    return;
                }

                r.str=r.str.replace(params[j].str, params[j+1].str);
            }
        } else {
            p->qfmpError(errorm);
            r.setInvalid();
            return;
        }
    }

    void fSplit(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        r.setInvalid();
        if (n==2 && params[0].type==qfmpString && params[1].type==qfmpString) {
            r.setStringVec(params[0].str.split(params[1].str));
        } else {
            p->qfmpError(QObject::tr("split(x, param) needs 2 string arguments"));
            r.setInvalid();
            return;
        }
    }

    void fRemoveEmpty(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        r.setInvalid();
        if (n==1 && params[0].type==qfmpStringVector) {
            r.setStringVec(QStringList());
            for (int i=0; i<params[0].strVec.size(); i++) {
                if (!params[0].strVec[i].isEmpty()) r.strVec<<params[0].strVec[i];
            }
        } else {
            p->qfmpError(QObject::tr("removeemptystrings(x) needs 1 string vector arguments"));
            r.setInvalid();
            return;
        }
    }

    void fJoin(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        QString sep="";
        r.setString("");
        if (n==2) {
            if (params[1].type==qfmpString) {
                sep=params[1].str;
            } else {
                p->qfmpError(QObject::tr("join(x, separator) needs a string as second argument"));
                r.setInvalid();
                return;
            }
        }
        if (n>0 && params[0].type==qfmpStringVector) {
            r.setString(params[0].strVec.join(sep));
        } else {
            p->qfmpError(QObject::tr("join(x, separator) needs a string vector as first argument"));
            r.setInvalid();
            return;
        }
    }





    void fReturnFirstValid(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        res.setInvalid();

        if (n<1) {
            p->qfmpError("returnfirstvalid(x1,x2,x3,...) needs at least one argument");
            return ;
        }
        for (unsigned int i=0; i<n; i++) {
            if (params[i].isValid && params[i].type!=qfmpVoid) {
                res=params[i];
                return;
            }
        }
    }


    void fReturnLastValid(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        res.setInvalid();

        if (n<1) {
            p->qfmpError("returnlastvalid(x1,x2,x3,...) needs at least one argument");
            return ;
        }
        for ( long i=(long)n-1; i>=0; i--) {
            if (params[i].isValid && params[i].type!=qfmpVoid) {
                res=params[i];
                return;
            }
        }
    }

    void fType(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        //qDebug()<<"fVarname "<<nodes<<n<<p;
        QString iname="type";
        if (n<=0) {
            r.setInvalid();
            p->qfmpError(QObject::tr("%1(...) needs at least 1 argument").arg(iname));
            return;
        }
        if (n==1) {
            r.setString(params[0].typeName());
        } else if (n>=1) {
            r.setStringVec(n, "");
            for (unsigned int i=0; i<n; i++) {
                r.strVec[i]=params[i].typeName();
            }
        }
    }

    void fStrVec(qfmpResult &res, const qfmpResult* nodes, unsigned int n, QFMathParser *p)
    {
        const QString iname="strvec";
        const QString tname="string";
        res.setStringVec();
        for (unsigned int i=0; i<n; i++) {
            if (nodes[i].convertsToStringVector()) {
                res.strVec<<nodes[i].asStrVector();
            } else if (nodes[i].type==qfmpVoid || !nodes[i].isValid) {
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3/%3 vector in argument %2").arg(iname).arg(i+1).arg(tname));
                return;
            }
        }
    }

    void fNumVec(qfmpResult &res, const qfmpResult* nodes, unsigned int n, QFMathParser *p)
    {
        const QString iname="numvec";
        const QString tname="number";
        res.setDoubleVec();
        for (unsigned int i=0; i<n; i++) {
            if (nodes[i].convertsToVector()) {
                res.numVec<<nodes[i].asVector();
            } else if (nodes[i].type==qfmpVoid || !nodes[i].isValid) {
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3/%3 vector in argument %2").arg(iname).arg(i+1).arg(tname));
                return;
            }
        }
    }
    void fIntVec(qfmpResult &res, const qfmpResult* nodes, unsigned int n, QFMathParser *p)
    {
        const QString iname="intvec";
        const QString tname="integer";
        res.setDoubleVec();
        for (unsigned int i=0; i<n; i++) {
            if (nodes[i].convertsToVector()) {
                QVector<int> iv=nodes[i].asIntVector();
                for (int j=0; j<iv.size(); j++) res.numVec<<iv[j];
            } else if (nodes[i].type==qfmpVoid || !nodes[i].isValid) {
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3/%3 vector in argument %2").arg(iname).arg(i+1).arg(tname));
                return;
            }
        }
    }
    void fInt(qfmpResult &res, const qfmpResult* nodes, unsigned int n, QFMathParser *p)
    {
        const QString iname="int";
        const QString tname="integer";
        if (n==0) {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(...) needs at least one argument").arg(iname));
            return;
        } else if (n==1) {
            int i=0;
            if (nodes[i].type==qfmpDouble || nodes[i].type==qfmpBool) {
                res.setDouble(nodes[i].toInteger());
            } else if (nodes[i].type==qfmpDoubleVector || nodes[i].type==qfmpBoolVector) {
                QVector<int> iv=nodes[i].asIntVector();
                res.setDoubleVec();
                for (int j=0; j<iv.size(); j++) res.numVec<<iv[j];
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3 in argument %2").arg(iname).arg(1).arg(tname));
                return;
            }
        } else {
            res.setDoubleVec();
            for (unsigned int i=0; i<n; i++) {
                if (nodes[i].type==qfmpDouble || nodes[i].type==qfmpBool) {
                    res.numVec<<nodes[i].toInteger();
                } else if (nodes[i].convertsToIntVector()) {
                    QVector<int> iv=nodes[i].asIntVector();
                    res.setDoubleVec();
                    for (int j=0; j<iv.size(); j++) res.numVec<<iv[j];
                } else {
                    res.setInvalid();
                    p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3/%3 vector in argument %2").arg(iname).arg(i+1).arg(tname));
                    return;
                }
            }
        }
    }
    void fNum(qfmpResult &res, const qfmpResult* nodes, unsigned int n, QFMathParser *p)
    {
        const QString iname="num";
        const QString tname="number";
        if (n==0) {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(...) needs at least one argument").arg(iname));
            return;
        } else if (n==1) {
            int i=0;
            if (nodes[i].type==qfmpDouble || nodes[i].type==qfmpBool) {
                res.setDouble(nodes[i].asNumber());
            } else if (nodes[i].convertsToVector()) {
                res.setDoubleVec(nodes[i].asVector());
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3 in argument %2").arg(iname).arg(1).arg(tname));
                return;
            }
        } else {
            res.setDoubleVec();
            for (unsigned int i=0; i<n; i++) {
                if (nodes[i].type==qfmpDouble || nodes[i].type==qfmpBool) {
                    res.numVec<<nodes[i].asNumber();
                } else if (nodes[i].convertsToVector()) {
                    res.numVec<<nodes[i].asVector();
                } else {
                    res.setInvalid();
                    p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3/%3 vector in argument %2").arg(iname).arg(i+1).arg(tname));
                    return;
                }
            }
        }
    }
    void fBool(qfmpResult &res, const qfmpResult* nodes, unsigned int n, QFMathParser *p)
    {
        const QString iname="bool";
        const QString tname="boolean";
        if (n==0) {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(...) needs at least one argument").arg(iname));
            return;
        } else if (n==1) {
            int i=0;
            if (nodes[i].type==qfmpDouble || nodes[i].type==qfmpBool) {
                res.setBoolean(nodes[i].asBool());
            } else if (nodes[i].type==qfmpDoubleVector || nodes[i].type==qfmpBoolVector || nodes[i].convertsToBoolVector()) {
                res.setBoolVec(nodes[i].asBoolVector());
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3 in argument %2").arg(iname).arg(1).arg(tname));
                return;
            }
        } else {
            res.setBoolVec();
            for (unsigned int i=0; i<n; i++) {
                if (nodes[i].type==qfmpDouble || nodes[i].type==qfmpBool) {
                    res.boolVec<<nodes[i].asBool();
                } else if (nodes[i].convertsToIntVector()) {
                    res.boolVec<<nodes[i].asBoolVector();
                } else {
                    res.setInvalid();
                    p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3/%3 vector in argument %2").arg(iname).arg(i+1).arg(tname));
                    return;
                }
            }
        }
    }
    void fBoolVec(qfmpResult &res, const qfmpResult* nodes, unsigned int n, QFMathParser *p)
    {
        const QString iname="boolvec";
        const QString tname="boolean";
        res.setBoolVec();
        for (unsigned int i=0; i<n; i++) {
            if (nodes[i].convertsToBoolVector()) {
                res.boolVec<<nodes[i].asBoolVector();
            } else if (nodes[i].type==qfmpVoid || !nodes[i].isValid) {
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(...) encountered a datatype, which cannot be converted to a %3/%3 vector in argument %2").arg(iname).arg(i+1).arg(tname));
                return;
            }
        }
    }


    void fEnsureVectorSize(qfmpResult &res, const qfmpResult* params, unsigned int n, QFMathParser *p)
    {
        const QString iname="ensuresize";

        if (n==3 && (params[0].type==qfmpDoubleVector && params[1].isInteger() && params[2].type==qfmpDouble)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.numVec.size()>ts) {
                res.numVec.resize(ts);
            } else if (res.numVec.size()<ts) {
                while (res.numVec.size()<ts) res.numVec.append(params[2].num);
            }
        } else if (n==3 && (params[0].type==qfmpStringVector && params[1].isInteger() && params[2].type==qfmpString)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.strVec.size()>ts) {
                while (res.strVec.size()>ts) res.strVec.removeLast();
            } else if (res.strVec.size()<ts) {
                while (res.strVec.size()<ts) res.strVec.append(params[2].str);
            }
        } else if (n==3 && (params[0].type==qfmpString && params[1].isInteger() && params[2].type==qfmpString && params[2].str.size()==1)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.str.size()>ts) {
                res.str=res.str.left(ts);
            } else if (res.str.size()<ts) {
                while (res.str.size()<ts) res.str+=params[2].str;
            }
        } else if (n==3 && (params[0].type==qfmpBoolVector && params[1].isInteger() && params[2].type==qfmpBool)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.boolVec.size()>ts) {
                res.boolVec.resize(ts);
            } else if (res.boolVec.size()<ts) {
                while (res.boolVec.size()<ts) res.boolVec.append(params[2].boolean);
            }
        } else {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(x,desiredsize,defaultvalue) needs three arguments: one vector, one integer and one argument copatible with x").arg(iname));
            return;
        }

    }



    void fEnsureVectorSizeStart(qfmpResult &res, const qfmpResult* params, unsigned int n, QFMathParser *p)
    {
        const QString iname="ensuresize_start";

        if (n==3 && (params[0].type==qfmpDoubleVector && params[1].isInteger() && params[2].type==qfmpDouble)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.numVec.size()>ts) {
                while (res.numVec.size()>ts) res.numVec.remove(0);
            } else if (res.numVec.size()<ts) {
                while (res.numVec.size()<ts) res.numVec.prepend(params[2].num);
            }
        } else if (n==3 && (params[0].type==qfmpStringVector && params[1].isInteger() && params[2].type==qfmpString)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.strVec.size()>ts) {
                while (res.strVec.size()>ts) res.strVec.removeFirst();
            } else if (res.strVec.size()<ts) {
                while (res.strVec.size()<ts) res.strVec.prepend(params[2].str);
            }
        } else if (n==3 && (params[0].type==qfmpString && params[1].isInteger() && params[2].type==qfmpString && params[2].str.size()==1)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.str.size()>ts) {
                res.str=res.str.right(ts);
            } else if (res.str.size()<ts) {
                while (res.str.size()<ts) res.str=params[2].str+res.str;
            }
        } else if (n==3 && (params[0].type==qfmpBoolVector && params[1].isInteger() && params[2].type==qfmpBool)) {
            int ts=params[1].toInteger();
            res=params[0];
            if (res.boolVec.size()>ts) {
                while (res.boolVec.size()>ts) res.boolVec.remove(0);
            } else if (res.boolVec.size()<ts) {
                while (res.boolVec.size()<ts) res.boolVec.prepend(params[2].boolean);
            }
        } else {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(x,desiredsize,defaultvalue) needs three arguments: one vector, one integer and one argument copatible with x").arg(iname));
            return;
        }
    }





    void fHistogram(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        const QString iname="histogram";
        int bins=11;
        if ((n==1 || n==2 || n==3) && params[0].type==qfmpDoubleVector) {
            bool norm=false;
            if (n==2 && params[1].isUInt())  {
                bins=qMax((uint32_t)2,params[1].toUInt());
            } else if (n==2 && params[1].type==qfmpBool)  {
                norm=params[1].boolean;
            } else if (n==3 && params[1].isUInt() && params[2].type==qfmpBool)  {
                bins=qMax((uint32_t)2,params[1].toUInt());
                norm=params[2].boolean;
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(data[,bins][,normalize]) needs an unsigned integer for bins and a boolean for normalize (both optional)").arg(iname));
                return;
            }
            QVector<double> temp;
            temp.resize(bins);
            res.setDoubleVec(bins);
            statisticsHistogram(params[0].numVec.constData(), params[0].numVec.size(), temp.data(), res.numVec.data(), bins, norm);
        } else {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(data[,bins][,normalize]) needs at least one number vector argument").arg(iname));
            return;
        }
    }

    void fHistogramBins(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        const QString iname="histogrambins";
        int bins=11;
        if ((n==1 || n==2 || n==3) && params[0].type==qfmpDoubleVector) {
            bool norm=false;
            if (n==2 && params[1].isUInt())  {
                bins=qMax((uint32_t)2,params[1].toUInt());
            } else if (n==2 && params[1].type==qfmpBool)  {
                norm=params[1].boolean;
            } else if (n==3 && params[1].isUInt() && params[2].type==qfmpBool)  {
                bins=qMax((uint32_t)2,params[1].toUInt());
                norm=params[2].boolean;
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(data[,bins][,normalize]) needs an unsigned integer for bins and a boolean for normalize (both optional)").arg(iname));
                return;
            }
            QVector<double> temp;
            temp.resize(bins);
            res.setDoubleVec(bins);
            statisticsHistogram(params[0].numVec.constData(), params[0].numVec.size(), res.numVec.data(), temp.data(), bins, norm);
        } else {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(data[,bins][,normalize]) needs at least one number vector argument").arg(iname));
            return;
        }

    }










    void fRangedHistogram(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        const QString iname="rangedhistogram";
        int bins=11;
        if ((n==3 || n==4 || n==5) && params[0].type==qfmpDoubleVector && params[1].type==qfmpDouble && params[2].type==qfmpDouble) {
            bool norm=false;
            double rmin=params[1].num;
            double rmax=params[2].num;
            if (n==4 && params[3].isUInt())  {
                bins=qMax((uint32_t)2,params[3].toUInt());
            } else if (n==4 && params[3].type==qfmpBool)  {
                norm=params[3].boolean;
            } else if (n==5 && params[3].isUInt() && params[4].type==qfmpBool)  {
                bins=qMax((uint32_t)2,params[3].toUInt());
                norm=params[4].boolean;
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1(data,min,max[,bins][,normalize]) needs an unsigned integer for bins and a boolean for normalize (both optional)").arg(iname));
                return;
            }
            QVector<double> temp;
            temp.resize(bins);
            res.setDoubleVec(bins);
            statisticsHistogramRanged(params[0].numVec.constData(), params[0].numVec.size(), rmin, rmax, temp.data(), res.numVec.data(), bins, norm);
        } else {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1(data,min,max[,bins][,normalize]) needs at least one number vector argument").arg(iname));
            return;
        }
    }

    void fRangedHistogramBins(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
    {
        const QString iname="rangedhistogrambins";
        int bins=11;
        int paramoffset=0;
        if (n>0 && params[0].type==qfmpDoubleVector) paramoffset=1;
        if ((n==2+paramoffset || n==3+paramoffset || n==4+paramoffset) && params[paramoffset+0].type==qfmpDouble && params[paramoffset+1].type==qfmpDouble) {
            bool norm=false;
            double rmin=params[paramoffset].num;
            double rmax=params[paramoffset+1].num;
            if (n==paramoffset+3 && params[paramoffset+2].isUInt())  {
                bins=qMax((uint32_t)2,params[paramoffset+2].toUInt());
            } else if (n==paramoffset+3 && params[paramoffset+2].type==qfmpBool)  {
                norm=params[paramoffset+2].boolean;
            } else if (n==paramoffset+4 && params[paramoffset+2].isUInt() && params[paramoffset+3].type==qfmpBool)  {
                bins=qMax((uint32_t)2,params[paramoffset+2].toUInt());
                norm=params[paramoffset+3].boolean;
            } else {
                res.setInvalid();
                p->qfmpError(QObject::tr("%1([data,]min,max[,bins][,normalize]) needs an unsigned integer for bins and a boolean for normalize (both optional)").arg(iname));
                return;
            }
            QVector<double> temp;
            temp.resize(bins);
            res.setDoubleVec(bins);
            QVector<double> dummy;
            dummy<<0<<1;
            statisticsHistogramRanged(dummy.constData(), dummy.size(), rmin, rmax, res.numVec.data(), temp.data(), bins, norm);
        } else {
            res.setInvalid();
            p->qfmpError(QObject::tr("%1([data,]min,max[,bins][,normalize]) needs at least two number arguments").arg(iname));
            return;
        }

    }
















    void fPolyFit(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=3) {
            p->qfmpError("polyfit(X,Y,n) needs 3 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].convertsToVector() && params[1].convertsToVector())) {
            p->qfmpError("polyfit(X,Y,n) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!= params[1].length()) {
            p->qfmpError("polyfit(X,Y,n) arguments X and Y have to have the same length");
            r.setInvalid();
            return;

        }
        if (!(params[2].isInteger())) {
            p->qfmpError("polyfit(X,Y,n) argument n has to be a positive integer number");
            r.setInvalid();
            return;

        }

        QVector<double> X=params[0].asVector();
        QVector<double> Y=params[1].asVector();
        int32_t np=params[2].toInteger();

        if (np<=0) {
            p->qfmpError("polyfit(X,Y,n) argument n has to be a positive integer number");
            r.setInvalid();
            return;

        }

        r.numVec.clear();
        r.numVec.resize(np+1);
        for (int i=0; i<np+1; i++) r.numVec[i]=0.0;
        bool ok=statisticsPolyFit(X.data(), Y.data(), qMin(X.size(), Y.size()), np, r.numVec.data());

    }


    void fRegression(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=2 && n!=4) {
            p->qfmpError("regression(X,Y) needs 2 or 4 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].convertsToVector() && params[1].convertsToVector())) {
            p->qfmpError("regression(X,Y) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!= params[1].length()) {
            p->qfmpError("regression(X,Y) arguments X and Y have to have the same length");
            r.setInvalid();
            return;

        }

        double a, b;
        bool fixA=false;
        bool fixB=false;
        if (n==4) {
            if (!(params[2].type==qfmpString && params[3].type==qfmpDouble)) {
                p->qfmpError(QObject::tr("regression(X,Y,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number, but parameters are %1 and %2").arg(params[2].toTypeString()).arg(params[3].toTypeString()));
                r.setInvalid();
                return;

            }
            if (params[2].str.toLower()=="a") {
                fixA=true;
                a=params[3].num;
            } else if (params[2].str.toLower()=="b") {
                fixB=true;
                b=params[3].num;
            } else {
                p->qfmpError(QObject::tr("regression(X,Y,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number 'a' or 'b', but was %1").arg(params[2].toTypeString()));
                r.setInvalid();
                return;

            }
        }
        QVector<double> X=params[0].asVector();
        QVector<double> Y=params[1].asVector();
        statisticsLinearRegression(X.data(), Y.data(), qMin(X.size(), Y.size()), a, b, fixA, fixB);

        r.numVec.clear();
        r.numVec<<a;
        r.numVec<<b;

    }

    void fWeightedRegression(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=3 && n!=5) {
            p->qfmpError("weighted_regression(X,Y,W) needs 3 or 5 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].convertsToVector() && params[1].convertsToVector() && params[2].convertsToVector())) {
            p->qfmpError("weighted_regression(X,Y,W) arguments X, Y and W have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!= params[1].length() || params[0].length()!= params[2].length() || params[2].length()!= params[2].length()) {
            p->qfmpError("weighted_regression(X,Y,W) arguments X, Y and W have to have the same length");
            r.setInvalid();
            return;

        }

        double a, b;
        bool fixA=false;
        bool fixB=false;
        if (n==5) {
            if (!(params[3].type==qfmpString && params[4].type==qfmpDouble)) {
                p->qfmpError("weighted_regression(X,Y,W,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number");
                r.setInvalid();
                return;

            }
            if (params[3].str.toLower()=="a") {
                fixA=true;
                a=params[4].num;
            } else if (params[3].str.toLower()=="b") {
                fixB=true;
                b=params[4].num;
            } else {
                p->qfmpError(QObject::tr("weighted_regression(X,Y,W,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number 'a' or 'b', but was %1").arg(params[3].toTypeString()));
                r.setInvalid();
                return;

            }
        }
        QVector<double> X=params[0].asVector();
        QVector<double> Y=params[1].asVector();
        QVector<double> W=params[2].asVector();
        statisticsLinearWeightedRegression(X.data(), Y.data(), W.data(), qMin(X.size(), Y.size()), a, b, fixA, fixB);

        r.numVec.clear();
        r.numVec<<a;
        r.numVec<<b;

    }

    void fIRLS(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n<2 && n>6) {
            p->qfmpError("irls(X,Y) needs between 2 and 6 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].convertsToVector() && params[1].convertsToVector())) {
            p->qfmpError("irls(X,Y) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!= params[1].length()) {
            p->qfmpError("irls(X,Y) arguments X and Y have to have the same length");
            r.setInvalid();
            return;

        }

        double a=0, b=0;
        bool fixA=false;
        bool fixB=false;
        double Lp=1.1;
        int iterations=100;
        int fixOffset=2;
        if (n>=3) {
            if (params[2].type==qfmpDouble) {
                fixOffset=3;
                Lp=params[2].asNumber();
                if (n>=4) {
                    if (params[3].type==qfmpDouble) {
                        fixOffset=4;
                        iterations=params[3].asNumber();
                    }
                }
            }
        }

        if ((int64_t)n==fixOffset+2) {
            if (!(params[fixOffset+0].type==qfmpString && params[fixOffset+1].type==qfmpDouble)) {
                p->qfmpError("irls(X,Y,...,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number");
                r.setInvalid();
                return;

            }
            if (params[fixOffset+0].str.toLower()=="a") {
                fixA=true;
                a=params[fixOffset+1].num;
            } else if (params[fixOffset+0].str.toLower()=="b") {
                fixB=true;
                b=params[fixOffset+1].num;
            } else {
                p->qfmpError(QObject::tr("irls(X,Y,...,'fixParameter',fixValue) argument fixParameter has to be a string and fixValue a number 'a' or 'b', but was %1").arg(params[fixOffset+0].toTypeString()));
                r.setInvalid();
                return;

            }
        }
        QVector<double> X=params[0].asVector();
        QVector<double> Y=params[1].asVector();
        statisticsIterativelyReweightedLeastSquaresRegression(X.data(), Y.data(), qMin(X.size(), Y.size()), a, b, Lp, iterations, fixA, fixB);

        r.numVec.clear();
        r.numVec<<a;
        r.numVec<<b;

    }






















    void fErrorAdd(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=2) {
            p->qfmpError("erroradd(X,Y) needs 2 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector)) {
            p->qfmpError("erroradd(X,Y) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!=2 || params[1].length()!=2) {
            p->qfmpError("erroradd(X,Y) arguments X and Y have to have length 2");
            r.setInvalid();
            return;

        }

        double X=params[0].numVec[0];
        double Xe=params[0].numVec[1];
        double Y=params[1].numVec[0];
        double Ye=params[1].numVec[1];

        r.numVec.clear();
        r.numVec<<X+Y;
        r.numVec<<sqrt(Xe*Xe+Ye*Ye);

    }

    void fErrorSub(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=2) {
            p->qfmpError("errorsub(X,Y) needs 2 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector)) {
            p->qfmpError("errorsub(X,Y) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!=2 || params[1].length()!=2) {
            p->qfmpError("errorsub(X,Y) arguments X and Y have to have length 2");
            r.setInvalid();
            return;

        }

        double X=params[0].numVec[0];
        double Xe=params[0].numVec[1];
        double Y=params[1].numVec[0];
        double Ye=params[1].numVec[1];

        r.numVec.clear();
        r.numVec<<X-Y;
        r.numVec<<sqrt(Xe*Xe+Ye*Ye);

    }




    void fErrorMul(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=2) {
            p->qfmpError("errormul(X,Y) needs 2 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector)) {
            p->qfmpError("errormul(X,Y) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!=2 || params[1].length()!=2) {
            p->qfmpError("errormul(X,Y) arguments X and Y have to have length 2");
            r.setInvalid();
            return;

        }

        double X=params[0].numVec[0];
        double Xe=params[0].numVec[1];
        double Y=params[1].numVec[0];
        double Ye=params[1].numVec[1];

        r.numVec.clear();
        r.numVec<<X*Y;
        r.numVec<<sqrt(qfSqr(Y*Xe)+qfSqr(X*Ye));

    }

    void fErrorDiv(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=2) {
            p->qfmpError("errordiv(X,Y) needs 2 arguments");
            r.setInvalid();
            return;

        }
        if (!(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector)) {
            p->qfmpError("errordiv(X,Y) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;

        }
        if (params[0].length()!=2 || params[1].length()!=2) {
            p->qfmpError("errordiv(X,Y) arguments X and Y have to have length 2");
            r.setInvalid();
            return;

        }

        double X=params[0].numVec[0];
        double Xe=params[0].numVec[1];
        double Y=params[1].numVec[0];
        double Ye=params[1].numVec[1];

        r.numVec.clear();
        r.numVec<<X/Y;
        r.numVec<<sqrt(qfSqr(Xe/Y)+qfSqr(X*Ye/Y/Y));

    }
    void fErrorPow(qfmpResult &r, const qfmpResult* params, unsigned int  n, QFMathParser* p){
        r.setDoubleVec();
        if (n!=2) {
            p->qfmpError("errorpow(X,Y) needs 2 arguments");
            r.setInvalid();
            return;
        }
        if (!(params[0].type==qfmpDoubleVector && params[1].type==qfmpDoubleVector)) {
            p->qfmpError("errorpow(X,Y) arguments X and Y have to be of type number vector");
            r.setInvalid();
            return;
        }
        if (params[0].length()!=2 || params[1].length()!=2) {
            p->qfmpError("errorpow(X,Y) arguments X and Y have to have length 2");
            r.setInvalid();
            return;
        }

        double X=params[0].numVec[0];
        double Xe=params[0].numVec[1];
        double Y=params[1].numVec[0];
        double Ye=params[1].numVec[1];

        r.numVec.clear();
        r.numVec<<pow(X,Y);
        r.numVec<<sqrt(qfSqr(Xe*Y*pow(X,Y-1.0))+qfSqr(Ye*pow(X,Y)*log(Y)));
    }


















#ifdef QFLIB_LIBRARY
    void fRDRIDs(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            QList<int> ids=p->getRawDataIDList();
            r.setDoubleVec(ids.size(), -1);
            QStringList t;
            if (n==1) {
                if (params[0].type==qfmpString) {
                    t<<params[0].str;
                } else if (params[0].type==qfmpStringVector) {
                    t=params[0].strVec;
                } else {
                    parser->qfmpError(QObject::tr("rdr_ids(types) needs a string, or string vector as first argument"));
                    r.setInvalid();
                    return;
                }
            }
            if (t.size()==0) {
                for (int i=0; i<ids.size(); i++) {
                    r.numVec[i]=ids[i];
                }
            } else {
                r.setDoubleVec();
                for (int i=0; i<ids.size(); i++) {
                    QFRawDataRecord* rdr=p->getRawDataByID(ids[i]);
                    if (rdr && t.contains(rdr->getType())) {
                        r.numVec<<ids[i];
                    }
                }
            }
        }
    }
    void fEvalIDs(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            QList<int> ids=p->getEvaluationIDList();
            r.setDoubleVec(ids.size(), -1);
            QStringList t;
            if (n==1) {
                if (params[0].type==qfmpString) {
                    t<<params[0].str;
                } else if (params[0].type==qfmpStringVector) {
                    t=params[0].strVec;
                } else {
                    parser->qfmpError(QObject::tr("eval_ids(types) needs a string, or string vector as first argument"));
                    r.setInvalid();
                    return;
                }
            }
            if (t.size()==0) {
                for (int i=0; i<ids.size(); i++) {
                    r.numVec[i]=ids[i];
                }
            } else {
                r.setDoubleVec();
                for (int i=0; i<ids.size(); i++) {
                    QFEvaluationItem* eval=p->getEvaluationByID(ids[i]);
                    if (eval && t.contains(eval->getType())) {
                        r.numVec<<ids[i];
                    }
                }
            }
        }
    }
    void fRDRGetResult(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            QString eval="";
            QString result="";
            QVariant def;
            if (n>=3 && params[0].type==qfmpDouble && params[1].type==qfmpString && params[2].type==qfmpString) {
                rdrID=params[0].toInteger();
                eval=params[1].str;
                result=params[2].str;
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                //qDebug()<<"fRDRGetResult"<<rdrID<<eval<<result;
                if (rdr) {
                    QVariant v=rdr->resultsGetAsQVariantNoError(eval, result);
                    if (v.isValid())  r.setVariant(v);
                    else r.setVariant(def);
                    //qDebug()<<"  -> "<<v;
                } else {
                    r.setVariant(def);
                }
                //qDebug()<<"  => "<<r.toTypeString();
            } else {
                parser->qfmpError(QObject::tr("rdr_getresult(rdr, evalID, resultID[, default]) needs an integer and two string arguments"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetResultIDs(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            QString eval="";
            if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpString) {
                rdrID=params[0].toInteger();
                eval=params[1].str;
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                r.setStringVec();
                if (rdr) {
                    r.strVec=rdr->resultsGetResultNames(eval);
                }
                qSort(r.strVec);
            } else {
                parser->qfmpError(QObject::tr("rdr_getresultids(rdr, evalID) needs an integer and one string argument"));
                r.setInvalid();
                return;
            }
        }
    }

    void fRDRGetEvaluationIDs(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                r.setStringVec();
                if (rdr) {
                    r.strVec=rdr->resultsGetEvaluationNames();
                }
                qSort(r.strVec);
            } else {
                parser->qfmpError(QObject::tr("rdr_getevaluationids(rdr) needs an integer argument"));
                r.setInvalid();
                return;
            }
        }
    }

    void fRDRGetResultError(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            QString eval="";
            QString result="";
            if (n==3 && params[0].type==qfmpDouble && params[1].type==qfmpString && params[2].type==qfmpString) {
                rdrID=params[0].toInteger();
                eval=params[1].str;
                result=params[2].str;
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setVariant(rdr->resultsGetAsQVariantOnlyError(eval, result));
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getresulterror(rdr, evalID, resultID) needs an integer and two string arguments"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetName(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setString(rdr->getName());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getname(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetType(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setString(rdr->getType());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_gettype(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetFolder(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setString(rdr->getFolder());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getfolder(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetGroup(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setString(rdr->getGroupName());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getgroup(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetRole(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setString(rdr->getRole());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getrole(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetDescription(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setString(rdr->getDescription());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getdescription(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetFiles(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setStringVec(rdr->getFiles());
                } else {
                    r.setStringVec();
                }
            } else if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpString) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setStringVec(rdr->getFilesForType(params[1].str));
                } else {
                    r.setStringVec();
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getfiles(rdr[, file_type]) needs one integer and an optional string argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetFileTypes(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setStringVec(rdr->getFilesTypes());
                } else {
                    r.setStringVec();
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_filetypes(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetFileDescriptions(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                rdrID=params[0].toInteger();
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setStringVec(rdr->getFilesDescriptions());
                } else {
                    r.setStringVec();
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_filedescriptions(rdr) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fRDRGetProperty(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int rdrID=-1;
            QString prop="";
            QVariant def;
            if (n>=2 && params[0].type==qfmpDouble && params[1].type==qfmpString ) {
                rdrID=params[0].toInteger();
                prop=params[1].str;
                if (n==3) {
                    def=params[2].asVariant();
                }
                QFRawDataRecord* rdr=p->getRawDataByID(rdrID);
                if (rdr) {
                    r.setVariant(rdr->getQFProperty(prop, def));
                } else {
                    r.setVariant(def);
                }
            } else {
                parser->qfmpError(QObject::tr("rdr_getproperty(rdr, propertyID) needs an integer and a string argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fProjectGetProperty(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            QString prop="";
            QVariant def;
            if (n>=1 && params[0].type==qfmpString ) {
                prop=params[0].str;
                if (n==2) {
                    def=params[1].asVariant();
                }

                r.setVariant(p->getQFProperty(prop, def));

            } else {
                parser->qfmpError(QObject::tr("project_getproperty(propertyID) needs at least a string as first argument"));
                r.setInvalid();
                return;
            }
        }
    }

    void fEvalGetName(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFEvaluationItem* eval=p->getEvaluationByID(evalID);
                if (eval) {
                    r.setString(eval->getName());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("eval_getname(eval) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fEvalGetType(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFEvaluationItem* eval=p->getEvaluationByID(evalID);
                if (eval) {
                    r.setString(eval->getType());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("eval_gettype(eval) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fEvalGetDescription(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFEvaluationItem* eval=p->getEvaluationByID(evalID);
                if (eval) {
                    r.setString(eval->getDescription());
                } else {
                    r.setString("");
                }
            } else {
                parser->qfmpError(QObject::tr("eval_getdescription(eval) needs one integer argument"));
                r.setInvalid();
                return;
            }
        }
    }
    void fEvalGetProperty(qfmpResult &r, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        r.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            QString prop="";
            QVariant def;
            if (n>=2 && params[0].type==qfmpDouble && params[1].type==qfmpString ) {
                evalID=params[0].toInteger();
                prop=params[1].str;
                if (n==3) {
                    def=params[2].asVariant();
                }
                QFEvaluationItem* eval=p->getEvaluationByID(evalID);
                if (eval) {
                    r.setVariant(eval->getQFProperty(prop, def));
                } else {
                    r.setVariant(def);
                }
            } else {
                parser->qfmpError(QObject::tr("eval_getproperty(eval, propertyID) needs an integer and a string argument"));
                r.setInvalid();
                return;
            }
        }
    }





















    void fRDR_istable(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                res.setBoolean(rdr);
            } else {
                parser->qfmpError(QObject::tr("rdr_istable(rdrid) needs one integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }

    void fTable_column(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
                evalID=params[0].toInteger();
                int col=params[1].toInteger()-1;
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                if (rdr && col>=0 && col<rdr->tableGetColumnCount()) {
                    res.setVariant(QVariantList(rdr->tableGetColumnData(col)));
                }
            } else {
                parser->qfmpError(QObject::tr("table_column(rdrid, col) needs two integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }


    void fTable_data(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==3 && params[0].type==qfmpDouble && params[1].type==qfmpDouble && params[2].type==qfmpDouble) {
                evalID=params[0].toInteger();
                int col=params[1].toInteger()-1;
                int row=params[2].toInteger()-1;
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                if (rdr && col>=0 && col<rdr->tableGetColumnCount()) {
                    res.setVariant(rdr->tableGetData(row,col));
                }
            } else {
                parser->qfmpError(QObject::tr("table_data(rdrid, col, row) needs three integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }







    void fTable_numcolumn(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
                evalID=params[0].toInteger();
                int col=params[1].toInteger()-1;
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                if (rdr && col>=0 && col<rdr->tableGetColumnCount()) {
                    res.setDoubleVec(rdr->tableGetColumnDataAsDouble(col));
                } else {
                    res.setDoubleVec();
                }
            } else {
                parser->qfmpError(QObject::tr("table_numcolumn(rdrid, col) needs two integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }


    void fTable_numdata(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==3 && params[0].type==qfmpDouble && params[1].type==qfmpDouble && params[2].type==qfmpDouble) {
                evalID=params[0].toInteger();
                int col=params[1].toInteger()-1;
                int row=params[2].toInteger()-1;
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                if (rdr && col>=0 && col<rdr->tableGetColumnCount()) {
                    res.setDouble(rdr->tableGetData(row,col).toDouble());
                } else {
                    res.setDouble(NAN);
                }
            } else {
                parser->qfmpError(QObject::tr("table_numdata(rdrid, col, row) needs three integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }




    void fTable_columns(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->tableGetColumnCount());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("table_columnss(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }


    void fTable_columntitles(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setStringVec();
                    for (int i=0; i<rdr->tableGetColumnCount(); i++) {
                        res.strVec<<rdr->tableGetColumnTitle(i);
                    }
                } else {
                    res.setStringVec();
                }
            } else {
                parser->qfmpError(QObject::tr("table_columntitles(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }


    void fTable_rows(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRTableInterface* rdr=dynamic_cast<QFRDRTableInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->tableGetRowCount());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("table_rows(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }
















    void fRDR_isoverview(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageToRunInterface* rdr=dynamic_cast<QFRDRImageToRunInterface*>(p->getRawDataByID(evalID));
                res.setBoolean(rdr);
            } else {
                parser->qfmpError(QObject::tr("rdr_isoverview(rdrid) needs one integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }


    void fRDR_overviewwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageToRunInterface* rdr=dynamic_cast<QFRDRImageToRunInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageFromRunsWidth());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("overview_width(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_overviewheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageToRunInterface* rdr=dynamic_cast<QFRDRImageToRunInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageFromRunsHeight());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("overview_height(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_overviewsize(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageToRunInterface* rdr=dynamic_cast<QFRDRImageToRunInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageFromRunsHeight()*rdr->getImageFromRunsWidth());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("overview_size(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }
    void fRDR_overviewchannels(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageToRunInterface* rdr=dynamic_cast<QFRDRImageToRunInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageFromRunsChannels());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("overview_channels(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }
    void fRDR_overview(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==1 && params[0].type==qfmpDouble)||(n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble)) {
                evalID=params[0].toInteger();
                QFRDRImageToRunInterface* rdr=dynamic_cast<QFRDRImageToRunInterface*>(p->getRawDataByID(evalID));
                int ch=0;
                if (n==2) ch=params[1].toInteger();
                if (rdr) {
                    res.setDoubleVec(rdr->getImageFromRunsPreview(ch), rdr->getImageFromRunsHeight()*rdr->getImageFromRunsWidth());
                } else {
                    res.setDoubleVec();
                }
            } else {
                parser->qfmpError(QObject::tr("overview_image(rdrid[, channel=0]) needs one or two integer argument"));
                res.setInvalid();
                return;
            }
        }
    }









    void fRDR_ismask(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageMaskInterface* rdr=dynamic_cast<QFRDRImageMaskInterface*>(p->getRawDataByID(evalID));
                res.setBoolean(rdr);
            } else {
                parser->qfmpError(QObject::tr("rdr_ismask(rdrid) needs one integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }


    void fRDR_maskwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageMaskInterface* rdr=dynamic_cast<QFRDRImageMaskInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->maskGetWidth());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("mask_width(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_maskheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageMaskInterface* rdr=dynamic_cast<QFRDRImageMaskInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->maskGetHeight());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("mask_height(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_masksize(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageMaskInterface* rdr=dynamic_cast<QFRDRImageMaskInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->maskGetHeight()*rdr->maskGetWidth());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("mask_size(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }
    void fRDR_mask(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==1 && params[0].type==qfmpDouble)) {
                evalID=params[0].toInteger();
                QFRDRImageMaskInterface* rdr=dynamic_cast<QFRDRImageMaskInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setBoolVec(rdr->maskGet(), rdr->maskGetWidth()*rdr->maskGetHeight());
                } else {
                    res.setBoolVec();
                }
            } else {
                parser->qfmpError(QObject::tr("mask_get(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }




    void fRDR_isrunex(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRRunSelectionsInterface* rdr=dynamic_cast<QFRDRRunSelectionsInterface*>(p->getRawDataByID(evalID));
                res.setBoolean(rdr);
            } else {
                parser->qfmpError(QObject::tr("rdr_isrunexclude(rdrid) needs one integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_runex_runs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==1 && params[0].type==qfmpDouble)) {
                evalID=params[0].toInteger();
                QFRDRRunSelectionsInterface* rdr=dynamic_cast<QFRDRRunSelectionsInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->leaveoutGetRunCount());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("runex_runs(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    };
    void fRDR_runex_isexcluded(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble)) {
                evalID=params[0].toInteger();
                int run=evalID=params[1].toInteger();
                QFRDRRunSelectionsInterface* rdr=dynamic_cast<QFRDRRunSelectionsInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setBoolean(rdr->leaveoutRun(run));
                }
            } else {
                parser->qfmpError(QObject::tr("runex_isexcluded(rdrid, run) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    };
    void fRDR_runex_excluded(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==1 && params[0].type==qfmpDouble)) {
                evalID=params[0].toInteger();
                QFRDRRunSelectionsInterface* rdr=dynamic_cast<QFRDRRunSelectionsInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDoubleVec();
                    for (int i=0; i<rdr->leaveoutGetRunCount(); i++) {
                        if (rdr->leaveoutRun(i)) res.numVec<<i;
                    }
                } else {
                    res.setDoubleVec();
                }
            } else {
                parser->qfmpError(QObject::tr("runex_included(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }
    void fRDR_runex_included(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==1 && params[0].type==qfmpDouble)) {
                evalID=params[0].toInteger();
                QFRDRRunSelectionsInterface* rdr=dynamic_cast<QFRDRRunSelectionsInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDoubleVec();
                    for (int i=0; i<rdr->leaveoutGetRunCount(); i++) {
                        if (!rdr->leaveoutRun(i)) res.numVec<<i;
                    }
                } else {
                    res.setDoubleVec();
                }
            } else {
                parser->qfmpError(QObject::tr("runex_included(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }
    void fRDR_runex_mask(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser) {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==1 && params[0].type==qfmpDouble)) {
                evalID=params[0].toInteger();
                QFRDRRunSelectionsInterface* rdr=dynamic_cast<QFRDRRunSelectionsInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setBoolVec(rdr->leaveoutGetRunCount());
                    for (int i=0; i<rdr->leaveoutGetRunCount(); i++) {
                        res.boolVec[i]=rdr->leaveoutRun(i);
                    }
                } else {
                    res.setBoolVec();
                }
            } else {
                parser->qfmpError(QObject::tr("runex_mask(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }




    void fRDR_isimageselection(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                res.setBoolean(rdr);
            } else {
                parser->qfmpError(QObject::tr("rdr_isimageselection(rdrid) needs one integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }


    void fRDR_imageselectionwidth(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageSelectionWidth());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_width(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_imageselectionheight(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageSelectionHeight());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_height(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_imageselectionsize(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageSelectionHeight()*rdr->getImageSelectionWidth());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_size(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_imageselectioncount(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setDouble(rdr->getImageSelectionCount());
                } else {
                    res.setDouble(0);
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_count(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_imageselectionnames(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if (n==1 && params[0].type==qfmpDouble) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setStringVec(rdr->getImageSelectionCount());
                    for (int i=0; i<rdr->getImageSelectionCount(); i++) {
                        res.strVec[i]=rdr->getImageSelectionName(i);
                    }
                } else {
                    res.setStringVec(0);
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_names(rdrid) needs one integer argument"));
                res.setInvalid();
                return;
            }
        }
    }

    void fRDR_imageselectionlastofname(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==2 && params[0].type==qfmpDouble && params[1].type==qfmpString) ) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setBoolVec();
                    for (int i=rdr->getImageSelectionCount()-1; i>=0; i--) {
                        if (rdr->getImageSelectionName(i)==params[1].str){
                            res.setBoolVec(rdr->loadImageSelection(params[1].toInteger()), rdr->getImageSelectionWidth()*rdr->getImageSelectionHeight());
                            return;
                        }
                    }
                } else {
                    res.setBoolVec();
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_lastofname(rdrid, name) needs one integer and one string argument"));
                res.setInvalid();
                return;
            }
        }
    }
    void fRDR_imageselectionfirstofname(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==2 && params[0].type==qfmpDouble && params[1].type==qfmpString) ) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setBoolVec();
                    for (int i=0; i<rdr->getImageSelectionCount(); i++) {
                        if (rdr->getImageSelectionName(i)==params[1].str){
                            res.setBoolVec(rdr->loadImageSelection(params[1].toInteger()), rdr->getImageSelectionWidth()*rdr->getImageSelectionHeight());
                            return;
                        }
                    }
                } else {
                    res.setBoolVec();
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_firstofname(rdrid, name) needs one integer and one string argument"));
                res.setInvalid();
                return;
            }
        }
    }
    void fRDR_imageselection(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        QFProject* p=QFPluginServices::getInstance()->getCurrentProject();
        if (p)  {
            int evalID=-1;
            if ((n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) ) {
                evalID=params[0].toInteger();
                QFRDRImageSelectionInterface* rdr=dynamic_cast<QFRDRImageSelectionInterface*>(p->getRawDataByID(evalID));
                if (rdr) {
                    res.setBoolVec(rdr->loadImageSelection(params[1].toInteger()), rdr->getImageSelectionWidth()*rdr->getImageSelectionHeight());
                } else {
                    res.setBoolVec();
                }
            } else {
                parser->qfmpError(QObject::tr("imageselection_get(rdrid, selid) needs two integer arguments"));
                res.setInvalid();
                return;
            }
        }
    }

    void fFitFunctionParameters(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==1 && params[0].type==qfmpString ) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res.setStringVec();
                if (ff) {
                    res.setStringVec(ff->getParameterIDs());
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_parameternames(ffid) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_parameternames(ffid) needs one string argument"));
                res.setInvalid();
                return;
            }

    }

    void fFitFunctionIsFit(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==1 && params[0].type==qfmpString ) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res.setBoolVec();
                if (ff) {
                    res.setBoolVec();
                    for (int i=0; i<ff->paramCount(); i++) {
                        res.boolVec<<ff->getDescription(i).fit;
                    }
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_fitparameters(ffid) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_fitparameters(ffid) needs one string argument"));
                res.setInvalid();
                return;
            }

    }

    void fFitFunctionInit(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==1 && params[0].type==qfmpString ) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res.setBoolVec();
                if (ff) {
                    res.setDoubleVec();
                    for (int i=0; i<ff->paramCount(); i++) {
                        res.numVec<<ff->getDescription(i).initialValue;
                    }
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_init(ffid) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_init(ffid) needs one string argument"));
                res.setInvalid();
                return;
            }

    }


    void fFitFunctionInitFix(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==1 && params[0].type==qfmpString ) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res.setBoolVec();
                if (ff) {
                    res.setBoolVec();
                    for (int i=0; i<ff->paramCount(); i++) {
                        res.boolVec<<ff->getDescription(i).initialFix;
                    }
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_initfix(ffid) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_initfix(ffid) needs one string argument"));
                res.setInvalid();
                return;
            }

    }

    void fFitFunctionParamCount(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==1 && params[0].type==qfmpString ) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res.setDouble(0);
                if (ff) {
                    res.setDouble(ff->paramCount());
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_paramcount(ffid) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_paramcount(ffid) needs one string argument"));
                res.setInvalid();
                return;
            }

    }

    void fFitFunctionEval(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==3 && params[0].type==qfmpString && (params[1].type==qfmpDouble || params[1].type==qfmpDoubleVector) && params[2].type==qfmpDoubleVector ) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res.setInvalid();
                if (ff) {
                    if (params[1].type==qfmpDouble) {
                        res.setDouble(ff->evaluate(params[1].num, params[2].numVec));
                    } else if (params[1].type==qfmpDoubleVector) {
                        res.setDoubleVec(ff->multiEvaluate(params[1].numVec, params[2].numVec));
                    }
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_eval(ffid, x, params) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_eval(ffid, x, params) needs three arguments [string, number/number_vector, number_vector]"));
                res.setInvalid();
                return;
            }

    }



    void fFitFunctionEstimate(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==3 && params[0].type==qfmpString &&  params[1].type==qfmpDoubleVector &&  params[2].type==qfmpDoubleVector) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res=params[1];
                if (ff) {
                    res.setDoubleVec();
                    for (int i=0; i<ff->paramCount(); i++) {
                        res.numVec<<ff->getDescription(i).initialValue;
                    }
                    if (params[1].numVec.size()!=params[2].numVec.size() || params[1].numVec.size()<1 || params[2].numVec.size()<1) {
                        parser->qfmpError(QObject::tr("fitfunction_estimateparameters(ffid, dataX, dataY) the vectors dataX and dataY have to have the same number of entries and each at least one entry"));
                        res.setInvalid();
                        return;
                    }
                    ff->estimateInitial(res.numVec.data(), params[1].numVec.data(), params[2].numVec.data(), params[1].numVec.size());
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_estimateparameters(ffid, dataX, dataY) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_estimateparameters(ffid, dataX, dataY) needs three arguments [string, number_vector, number_vector]"));
                res.setInvalid();
                return;
            }

    }


    void fFitFunctionCalc(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==2 && params[0].type==qfmpString &&  params[1].type==qfmpDoubleVector ) {
                QString ffid=params[0].str;
                QFFitFunction* ff=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid);
                res=params[1];
                if (ff) {
                    res.setDoubleVec(params[1].numVec);
                    ff->calcParameter(res.numVec);
                    delete ff;
                } else {
                    parser->qfmpError(QObject::tr("fitfunction_calcparameters(ffid, params) specified fit function not available"));
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_calcparameters(ffid, params) needs three arguments [string, number/number_vector, number_vector]"));
                res.setInvalid();
                return;
            }

    }

    void fFitFunctionIDs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();

            if (n==0) {
                res.setStringVec(QFPluginServices::getInstance()->getFitFunctionManager()->getModelIDs());
            } else {
                parser->qfmpError(QObject::tr("fitfunction_ids() needs no arguments"));
                res.setInvalid();
                return;
            }

    }


    void fFitAlgorithmsIDs(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
             if (n==0) {
                res.setStringVec(QFPluginServices::getInstance()->getFitAlgorithmManager()->getIDList());
            } else {
                parser->qfmpError(QObject::tr("fitalgorithm_ids() needs no arguments"));
                res.setInvalid();
                return;
            }

    }


    void fFitFunctionFit(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
            if (n>=5 && params[0].type==qfmpString && params[1].type==qfmpString &&  params[2].type==qfmpDoubleVector ) {
                QString ffid=params[1].str;
                QString faid=params[0].str;
                QScopedPointer<QFFitFunction> ff(QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(ffid));
                QScopedPointer<QFFitAlgorithm> fa(QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(faid));
                if (ff && fa) {
                    QVector<bool> fix;
                    QVector<double> dx, dy, w, pmin, pmax;
                    for (int i=0; i<ff->paramCount(); i++) {
                        fix<<ff->getDescription(i).initialFix;
                        pmin<<ff->getDescription(i).minValue;
                        pmax<<ff->getDescription(i).maxValue;
                    }
                    bool hasMin=false;
                    bool hasMax=false;
                    if (n>=6 && params[3].type==qfmpBoolVector) {
                        fix=params[3].boolVec;
                        if (fix.size()<ff->paramCount()) {
                            parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams, fix, dataX, dataY[, weight]) parameter 'fix' has too few entries (available: %1, required: %2)").arg(fix.size()).arg(ff->paramCount()));
                            res.setInvalid();
                            return;
                        }
                        if (params[4].type==qfmpDoubleVector && params[5].type==qfmpDoubleVector) {
                            dx=params[4].numVec;
                            dy=params[5].numVec;
                            if (n>=7 && params[6].type==qfmpDoubleVector) {
                                w=params[6].numVec;
                            } else if (n>=7 && params[6].type!=qfmpDoubleVector) {
                                parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams, fix, dataX, dataY[, weight]) parameter 'weight' has to be number vectors"));
                                res.setInvalid();
                                return;
                            }
                        } else {
                            parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams, fix, dataX, dataY[, weight]) parameters 'dataX'/'dataY' have to be number vectors"));
                            res.setInvalid();
                            return;
                        }
                    } else if (n>=5 && params[3].type==qfmpDoubleVector && params[4].type==qfmpDoubleVector) {
                        dx=params[3].numVec;
                        dy=params[4].numVec;
                        if (n>=6 && params[5].type==qfmpDoubleVector) {
                            w=params[5].numVec;
                        } else if (n>=6 && params[5].type!=qfmpDoubleVector) {
                            parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams, fix, dataX, dataY[, weight]) parameter 'weight' has to be number vectors"));
                            res.setInvalid();
                            return;
                        }
                    } else {
                        parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams, fix, dataX, dataY[, weight]) parameters 'dataX'/'dataY' have to be number vectors"));
                        res.setInvalid();
                        return;
                    }
                    QVector<double> pout=params[2].numVec;
                    QVector<double> eout(pout.size());
                    int Ndata=qMin(dx.size(), dy.size());
                    if (w.size()<=0) {
                        for (int i=0; i<Ndata; i++) w<<1;
                    }
                    if (Ndata<=1) {
                        parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams[, fix], dataX, dataY[, weight]) needs at least one entry in dataX/dataY"));
                        res.setInvalid();
                        return;
                    }
                    if (w.size()<Ndata) {
                        parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams[, fix], dataX, dataY[, weight]) needs the same number of entries in dataX/dataY and weight"));
                        res.setInvalid();
                        return;
                    }
                    if (fix.size()<ff->paramCount()) {
                        parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams, fix, dataX, dataY[, weight]) parameter 'fix' has too few entries (available: %1, required: %2)").arg(fix.size()).arg(ff->paramCount()));
                        res.setInvalid();
                        return;
                    }
                    if (params[2].numVec.size()<ff->paramCount()) {
                        parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams, fix, dataX, dataY[, weight]) parameter 'initparams' has too few entries (available: %1, required: %2)").arg(params[2].numVec.size()).arg(ff->paramCount()));
                        res.setInvalid();
                        return;
                    }

                    fa->fit(pout.data(), eout.data(), dx.data(), dy.data(), w.data(), Ndata, ff.data(), params[2].numVec.data(), fix.data(), pmin.data(), pmax.data());
                    res.setDoubleVec(pout);
                } else {
                    if (!fa) {
                        parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY[, weight]) specified fit algorithm not available"));
                        res.setInvalid();
                        return;
                    }
                    if (!ff) {
                        parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY[, weight]) specified fit function not available"));
                        res.setInvalid();
                        return;
                    }
                }
            } else {
                parser->qfmpError(QObject::tr("fitfunction_fit(fitalg, ffid, initparams[, fix], dataX, dataY[, weight]) needs at least five arguments [string, string, number_vector, [boolean_vector], number_vector, number_vector, [number_vector]]"));
                res.setInvalid();
                return;
            }

    }

    void fFit(qfmpResult &res, QFMathParser::qfmpNode **nodes, unsigned int n, QFMathParser *parser)
    {
        res.setInvalid();
        if (n>=6) {
            qfmpResult paramsFitAlg, paramsParams, paramsInitParams, paramsDataX, paramsDataY, paramsWeights;
            nodes[0]->evaluate(paramsFitAlg);
            nodes[2]->evaluate(paramsParams);
            nodes[3]->evaluate(paramsInitParams);
            nodes[4]->evaluate(paramsDataX);
            nodes[5]->evaluate(paramsDataY);
            if (n>=7) nodes[6]->evaluate(paramsWeights);
            if (paramsFitAlg.type==qfmpString && paramsParams.type==qfmpStringVector && paramsInitParams.type==qfmpDoubleVector && paramsDataX.type==qfmpDoubleVector && paramsDataY.type==qfmpDoubleVector ) {
                QScopedPointer<QFFitAlgorithm> fa(QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(paramsFitAlg.str));
                QStringList paramnames=paramsParams.strVec;
                QVector<double> initP=paramsInitParams.numVec;
                if (paramnames.size()!=initP.size()) {
                    parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY[, weight]) parameters 'parameters' and 'initparams' don't have the name number of entries"));
                    res.setInvalid();
                    return;
                }
                if (paramsDataX.numVec.size()!=paramsDataY.numVec.size() || paramsDataX.numVec.size()<1 || paramsDataY.numVec.size()<1) {
                    parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY[, weight]) parameters 'dataX' and 'dataY' don't have the name number of entries, and/or have less than 1 entry"));
                    res.setInvalid();
                    return;
                }
                if (n>=7 && paramsWeights.isValid && paramsWeights.numVec.size()!=paramsDataY.numVec.size()) {
                    parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY, weight) parameters 'dataX', 'dataY' and 'weights' don't have the name number of entries, and/or have less than 1 entry"));
                    res.setInvalid();
                    return;
                }
                QScopedPointer<QFFitFunctionQFMathparserNode> ff(new QFFitFunctionQFMathparserNode(parser, nodes[1], paramnames, initP));
                if (fa && ff) {
                    QVector<bool> fix;
                    QVector<double> pmin, pmax;
                    for (int i=0; i<initP.size(); i++) {
                        fix<<false;
                        pmin<<-DBL_MAX;
                        pmax<<DBL_MAX;
                    }


                    QVector<double> pout=initP;
                    QVector<double> eout(pout.size());
                    QVector<double> w;




                    if (n>=7 && paramsWeights.isValid) {
                        w=paramsWeights.numVec;
                    } else {
                        w.fill(1, paramsDataX.numVec.size());
                    }

                    fa->fit(pout.data(), eout.data(), paramsDataX.numVec.data(), paramsDataY.numVec.data(), w.data(), paramsDataX.numVec.size(), ff.data(), initP.data(), fix.data(), pmin.data(), pmax.data());
                    res.setDoubleVec(pout);
                } else {
                    if (!fa) {
                        parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY[, weight]) specified fit algorithm not available"));
                    } else {
                        parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY[, weight]) could not construct fit function"));
                    }
                    res.setInvalid();
                    return;
                }
            } else {
                parser->qfmpError(QObject::tr("fit(fitalg, fitfunction, parameters, initparams, dataX, dataY[, weight]) needs at least five arguments [string, expression, string_vector, number_vector, number_vector, number_vector, [number_vector]]"));
                res.setInvalid();
                return;
            }
        }
    }

#endif
}




