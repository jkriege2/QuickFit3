/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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



/** \file jkmathparser.cpp
 *  \ingroup jkmp
 */

#define COMPILING_THIS_JKMATHPARSER
#include "jkmathparser.h" // class's header file
#include <iostream>
#include <float.h>



/******************************************************************************************
 * default-Funktionen für den Parser
 ******************************************************************************************/

jkMathParser::jkmpResult fFloatToStr(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  std::ostringstream ost;
  r.type=jkMathParser::jkmpString;
  if (n!=1) p->jkmpError("floattostr accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("floattostr needs double argument");
  ost<<params[0].num;
  r.str=ost.str();
  return r;
}


jkMathParser::jkmpResult fIntToStr(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  std::ostringstream ost;
  r.type=jkMathParser::jkmpString;
  if (n!=1) p->jkmpError("inttostr accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("inttostr needs double argument");
  ost<<int64_t(params[0].num);
  r.str=ost.str();
  return r;
}

jkMathParser::jkmpResult fBoolToStr(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  std::ostringstream ost;
  r.type=jkMathParser::jkmpString;
  if (n!=1) p->jkmpError("booltostr accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpBool) p->jkmpError("floattostr needs bool argument");
  r.str=(r.boolean)?"true":"false";
  return r;
}


jkMathParser::jkmpResult fToSystemPathSeparator(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpString;
  if (n!=1) p->jkmpError("tosystempathseparator accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpString) p->jkmpError("tosystempathseparator needs string argument");
  r.str="";
  for (size_t i=0; i<params[0].str.size(); i++) {
      char ch=params[0].str[i];
      if (ch=='/' || ch=='\\') ch=PATHSEPARATOR_CHAR;
      r.str+=ch;
  }

  return r;
}


jkMathParser::jkmpResult fSetDefault(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpString;
  if (n!=2) p->jkmpError("setdefault accepts 2 argument");
  if (params[0].type!=jkMathParser::jkmpString) p->jkmpError("setdefault needs a string as first argument");
  r=params[1];
  if (p->variableExists(params[0].str)) {
    r=p->getVariable(params[0].str);
  }

  return r;
}


jkMathParser::jkmpResult fStrDate(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
    jkMathParser::jkmpResult r;
    r.type=jkMathParser::jkmpString;
    std::string f="%Y-%m-%d";
    if (n>1) p->jkmpError("strdate accepts 0 or 1 argumentment");
    if (n>0 && params[0].type!=jkMathParser::jkmpString) p->jkmpError("strdate needs a string as first argument");
    if (n>0) f=params[0].str;
    char re[1024];
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    strftime(re, 1024, f.c_str(), timeinfo);
    r.str=re;

    return r;
}

jkMathParser::jkmpResult fCMDParam(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpString;
  std::string def="";
  if (n<1 || n>2) p->jkmpError("cmdparam(name, default) accepts 1 or 2 argument");
  if (params[0].type!=jkMathParser::jkmpString) p->jkmpError("cmdparam needs a string as first argument");
  if (n>1 && params[1].type!=jkMathParser::jkmpString) p->jkmpError("cmdparam needs a string as second argument");
  if (n>1) def=params[1].str;

  r.str=p->getArgCVParam(params[0].str, def);
  return r;
}


jkMathParser::jkmpResult fSinc(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("sinc accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("sinc needs double argument");
  //r.num=sin(params[0].num)/params[0].num;

  double x=params[0].num;
  static double const    taylor_0_bound = 3*DBL_MIN ;
  static double const    taylor_2_bound = sqrt(taylor_0_bound);
  static double const    taylor_n_bound = sqrt(taylor_2_bound);

  double    result = 1;
  if    (fabs(x) >= taylor_n_bound)
  {
      result= sin(x)/x;
  }
  else
  {
      // approximation by taylor series in x at 0 up to order 0

      if    (fabs(x) >= taylor_0_bound)
      {
          double    x2 = x*x;

          // approximation by taylor series in x at 0 up to order 2
          result -= x2/static_cast<double>(6);

          if    (fabs(x) >= taylor_2_bound)
          {
              // approximation by taylor series in x at 0 up to order 4
              result += (x2*x2)/static_cast<double>(120);
          }
      }


  }

  r.num=result;

  return r;
}

jkMathParser::jkmpResult fTanc(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("tanc accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("tanc needs double argument");
  r.num=(params[0].num==0)?1.0:tan(params[0].num)/params[0].num;
  return r;
}

jkMathParser::jkmpResult fSin(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("sin accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("sin needs double argument");
  r.num=sin(params[0].num);
  return r;
}

jkMathParser::jkmpResult fCos(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("cos accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("cos needs double argument");
  r.num=cos(params[0].num);
  return r;
}

jkMathParser::jkmpResult fTan(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("tan accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("tan needs double argument");
  r.num=tan(params[0].num);
  return r;
}

jkMathParser::jkmpResult fExp(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("exp accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("exp needs double argument");
  r.num=exp(params[0].num);
  return r;
}

jkMathParser::jkmpResult fLog(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("log accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("log needs double argument");
  r.num=log(params[0].num);
  return r;
}

jkMathParser::jkmpResult fLog10(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("log10 accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("log10 needs double argument");
  r.num=log10(params[0].num);
  return r;
}

jkMathParser::jkmpResult fLog2(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("log2 accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("log2 needs double argument");
  r.num=log2(params[0].num);
  return r;
}

jkMathParser::jkmpResult fSqrt(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("sqrt accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("sqrt needs double argument");
  r.num=sqrt(params[0].num);
  return r;
}
jkMathParser::jkmpResult fCbrt(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("cbrt accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("cbrt needs double argument");
  r.num=cbrt(params[0].num);
  return r;
}

jkMathParser::jkmpResult fSqr(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("sqr accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("sqr needs double argument");
  r.num=params[0].num*params[0].num;
  return r;
}

jkMathParser::jkmpResult fAbs(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("abs accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("abs needs double argument");
  r.num=fabs(params[0].num);
  return r;
}

jkMathParser::jkmpResult fIf(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  if (n!=3) p->jkmpError("If accepts 3 argument");
  if (params[0].type!=jkMathParser::jkmpBool) p->jkmpError("If needs bool as first argument");
  if (params[0].boolean) return params[1]; else return params[2];
}

jkMathParser::jkmpResult fASin(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("asin accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("asin needs double argument");
  r.num=asin(params[0].num);
  return r;
}

jkMathParser::jkmpResult fACos(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("acos accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("acos needs double argument");
  r.num=acos(params[0].num);
  return r;
}

jkMathParser::jkmpResult fATan(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("atan accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("atan needs double argument");
  r.num=atan(params[0].num);
  return r;
}

jkMathParser::jkmpResult fATan2(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("atan2 accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("atan2 needs double argument");
  r.num=atan2(params[0].num, params[1].num);
  return r;
}

jkMathParser::jkmpResult fSinh(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("sinh accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("sinh needs double argument");
  r.num=sinh(params[0].num);
  return r;
}

jkMathParser::jkmpResult fCosh(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("cosh accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("cosh needs double argument");
  r.num=cosh(params[0].num);
  return r;
}

jkMathParser::jkmpResult fTanh(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("tanh accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("tanh needs double argument");
  r.num=tanh(params[0].num);
  return r;
}

jkMathParser::jkmpResult fErf(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("erf accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("erf needs double argument");
  r.num=erf(params[0].num);
  return r;
}

jkMathParser::jkmpResult fErfc(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("erfc accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("erfc needs double argument");
  r.num=erfc(params[0].num);
  return r;
}

jkMathParser::jkmpResult flGamma(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("lgamma accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("lgamma needs double argument");
  r.num=lgamma(params[0].num);
  return r;
}

jkMathParser::jkmpResult ftGamma(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("tgamma accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("tgamma needs double argument");
  r.num=tgamma(params[0].num);
  return r;
}

jkMathParser::jkmpResult fJ0(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("j0 accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("j0 needs double argument");
  r.num=j0(params[0].num);
  return r;
}

jkMathParser::jkmpResult fJ1(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("j1 accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("j1 needs double argument");
  r.num=j1(params[0].num);
  return r;
}

jkMathParser::jkmpResult fY0(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("y0 accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("y0 needs double argument");
  r.num=y0(params[0].num);
  return r;
}

jkMathParser::jkmpResult fY1(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("y1 accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("y1 needs double argument");
  r.num=y1(params[0].num);
  return r;
}

jkMathParser::jkmpResult fYn(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("yn accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("yn needs double argument");
  r.num=yn((int)params[0].num, params[1].num);
  return r;
}

jkMathParser::jkmpResult fJn(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("jn accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("jn needs double argument");
  r.num=jn((int)params[0].num, params[1].num);
  return r;
}

jkMathParser::jkmpResult fSRand(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("srand accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("srand needs double argument");
  r.num=0;
  srand((unsigned int)params[0].num);
  return r;
}

jkMathParser::jkmpResult fRand(jkMathParser::jkmpResult* /*params*/, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=0) p->jkmpError("rand accepts 0 argument");
  r.num=double(rand())/double(RAND_MAX);
  return r;
}

jkMathParser::jkmpResult fCeil(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("ceil accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("ceil needs double argument");
  r.num=ceil(params[0].num);
  return r;
}

jkMathParser::jkmpResult fFloor(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("floor accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("floor needs double argument");
  r.num=floor(params[0].num);
  return r;
}

jkMathParser::jkmpResult fTrunc(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("trunc accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("trunc needs double argument");
  r.num=trunc(params[0].num);
  return r;
}

jkMathParser::jkmpResult fRound(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("round accepts 1 argument");
  if (params[0].type!=jkMathParser::jkmpDouble) p->jkmpError("round needs double argument");
  r.num=round(params[0].num);
  return r;
}

jkMathParser::jkmpResult fFMod(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("fmod accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("fmod needs double argument");
  r.num=fmod((int)params[0].num, params[1].num);
  return r;
}

jkMathParser::jkmpResult fMin(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("min accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("min needs double argument");
  r.num=fmin((int)params[0].num, params[1].num);
  return r;
}

jkMathParser::jkmpResult fMax(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("max accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("max needs double argument");
  r.num=fmax((int)params[0].num, params[1].num);
  return r;
}

jkMathParser::jkmpResult QFSPIMLightsheetEvaluationItem_fGauss(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("gauss accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("gauss needs double argument");
  r.num=exp(-2*params[0].num*params[0].num/params[1].num/params[1].num);
  return r;
}

jkMathParser::jkmpResult fSlit(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=2) p->jkmpError("slit accepts 2 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("slit needs double argument");
  r.num=((params[0].num>=-1.0*params[1].num/2)&&(params[0].num<=params[1].num/2))?1.0:0.0;
  return r;
}

jkMathParser::jkmpResult fTheta(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("theta accepts 1 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("theta needs double argument");
  r.num=(params[0].num>=0)?1.0:0.0;
  return r;
}

jkMathParser::jkmpResult fSigmoid(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("sigmoid accepts 1 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("sigmoid needs double argument");
  r.num=1.0/(1+exp(-1.0*params[0].num));
  return r;
}

jkMathParser::jkmpResult fSign(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p){
  jkMathParser::jkmpResult r;
  r.type=jkMathParser::jkmpDouble;
  if (n!=1) p->jkmpError("sign accepts 1 argument");
  if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("sign needs double argument");
  r.num=0.0;
  if (params[0].num<0) { r.num=-1; }
  else if (params[0].num>0) { r.num=+1; }
  return r;
}



inline std::string strip(std::string s) {
  std::string r;
  for (size_t i=0; i<s.size(); i++){
    if ((s[i]!=' ')&&(s[i]!='\t')&&(s[i]!='\r')&&(s[i]!='\n')) {
      r+=s[i];
    }
  }
  return r;
}



std::string jkMathParser::tokentostring(jkMathParser::jkmpTokenType token) {
    switch(token) {
        case END: return "END";
        case PRINT: return "PRINT (;)";
        case PARAMETER_DIV: return "PARAMETER_DIV (,)";
	    case STRING_DELIM: return "STRING_DELIM (' or \")";
	    case NAME: return "NAME";
	    case NUMBER: return "NUMBER";
	    case PLUS: return "PLUS (+)";
	    case MINUS: return "MINUS (-)";
	    case MUL: return "MUL (*)";
	    case DIV: return "DIV (/)";
	    case MODULO: return "MODULO (%)";
	    case ASSIGN: return "ASSIGN (=)";
	    case LBRACKET: return "LBRACKET '('";
	    case RBRACKET: return "RBRACKET ')'";
	    case POWER: return "POWER (^)";
	    case FACTORIAL_LOGIC_NOT: return "FACTORIAL_LOGIC_NOT (!)";
	    case LOGIC_NOT: return "LOGIC_NOT (!/not)";
	    case LOGIC_AND: return "LOGIC_AND (&&/and)";
	    case LOGIC_OR: return "LOGIC_OR (||/or)";
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

std::string jkMathParser::currenttokentostring() {
    switch(CurrentToken) {
        case END: return "END";
        case PRINT: return "PRINT (;)";
        case PARAMETER_DIV: return "PARAMETER_DIV (,)";
	    case STRING_DELIM: return "STRING_DELIM (' or \")";
	    case NAME: return format("NAME (%s)", StringValue.c_str());
	    case NUMBER: return format("NUMBER (%lf)", NumberValue);
	    case PLUS: return "PLUS (+)";
	    case MINUS: return "MINUS (-)";
	    case MUL: return "MUL (*)";
	    case DIV: return "DIV (/)";
	    case MODULO: return "MODULO (%)";
	    case ASSIGN: return "ASSIGN (=)";
	    case LBRACKET: return "LBRACKET '('";
	    case RBRACKET: return "RBRACKET ')'";
	    case POWER: return "POWER (^)";
	    case FACTORIAL_LOGIC_NOT: return "FACTORIAL_LOGIC_NOT (!)";
	    case LOGIC_NOT: return "LOGIC_NOT (!/not)";
	    case LOGIC_AND: return "LOGIC_AND (&/and)";
	    case LOGIC_OR: return "LOGIC_OR (|/or)";
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



/******************************************************************************************
 * jkMathParser
 ******************************************************************************************/

// class constructor
jkMathParser::jkMathParser() {
    jkmathparser_exception_function=NULL;
    data=NULL;
    argc=0;
    argv=NULL;
    addStandardFunctions();
    addStandardVariables();
}

void jkMathParser::addStandardVariables(){
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
}

void jkMathParser::addStandardFunctions(){
	addFunction("sinc", fSinc);
	addFunction("asin", fASin);
	addFunction("acos", fACos);
	addFunction("atan", fATan);
	addFunction("atan2", fATan2);
	addFunction("sin", fSin);
	addFunction("cos", fCos);
	addFunction("tan", fTan);
	addFunction("sinh", fSinh);
	addFunction("cosh", fCosh);
	addFunction("tanh", fTanh);
	addFunction("log", fLog);
	addFunction("log2", fLog2);
	addFunction("log10", fLog10);
	addFunction("exp", fExp);
	addFunction("sqrt", fSqrt);
    addFunction("cbrt", fCbrt);
    addFunction("sqr", fSqr);
	addFunction("abs", fAbs);
	addFunction("if", fIf);
	addFunction("erf", fErf);
	addFunction("erfc", fErfc);
	addFunction("lgamma", flGamma);
	addFunction("tgamma", ftGamma);
	addFunction("j0", fJ0);
	addFunction("j1", fJ1);
	addFunction("jn", fJn);
	addFunction("y0", fY0);
	addFunction("y1", fY1);
	addFunction("yn", fYn);
	addFunction("rand", fRand);
	addFunction("srand", fSRand);
	addFunction("ceil", fCeil);
	addFunction("floor", fFloor);
	addFunction("trunc", fTrunc);
	addFunction("round", fRound);
	addFunction("fmod", fFMod);
	addFunction("min", fMin);
	addFunction("max", fMax);
	addFunction("inttostr", fIntToStr);
	addFunction("floattostr", fFloatToStr);
	addFunction("num2str", fFloatToStr);
	addFunction("booltostr", fBoolToStr);
	addFunction("bool2str", fBoolToStr);
	addFunction("gauss", QFSPIMLightsheetEvaluationItem_fGauss);
	addFunction("slit", fSlit);
	addFunction("theta", fTheta);
	addFunction("tanc", fTanc);
	addFunction("sigmoid", fSigmoid);
	addFunction("sign", fSign);
	addFunction("tosystempathseparator", fToSystemPathSeparator);
	addFunction("setdefault", fSetDefault);
    addFunction("strdate", fStrDate);
	addFunction("cmdparam", fCMDParam);
	addFunction("argv", fCMDParam);
}

// class destructor
jkMathParser::~jkMathParser()
{
	clearFunctions();
	clearVariables();
}

// fügt eine Variable hinzu. Der Speicher wird extern verwaltet
void jkMathParser::addVariableDouble(std::string name, double* v)
{
    name=strip(name);
    jkmpVariable nv;
    nv.type=jkMathParser::jkmpDouble;
    nv.num=v;
    if (variableExists(name)) nv.internal=variables[name].internal; else nv.internal=false;
    variables[name]=nv;
}

void jkMathParser::addVariableString(std::string name, std::string* v)
{
    name=strip(name);
    jkmpVariable nv;
    nv.type=jkMathParser::jkmpString;
    nv.str=v;
    if (variableExists(name)) nv.internal=variables[name].internal; else nv.internal=false;
    variables[name]=nv;
}

void jkMathParser::addVariableBoolean(std::string name, bool* v)
{
    name=strip(name);
    jkmpVariable nv;
    nv.type=jkMathParser::jkmpBool;
    nv.boolean=v;
    if (variableExists(name)) nv.internal=variables[name].internal; else nv.internal=false;
    variables[name]=nv;
}

void jkMathParser::addVariableDouble(std::string name, double v)
{
    name=strip(name);
    jkmpVariable nv;
    nv.type=jkMathParser::jkmpDouble;
    nv.num=new double;//(double*)malloc(sizeof(double));
    nv.internal=true;
    *(nv.num)=v;
//  std::cout<<*(nv.num)<<std::endl;
    variables[name]=nv;
}

void jkMathParser::addVariableString(std::string name, std::string v)
{
    name=strip(name);
    jkmpVariable nv;
    nv.type=jkMathParser::jkmpString;
    nv.str=new std::string;
    nv.internal=true;
    *(nv.str)=v;
    variables[strip(name)]=nv;
}

void jkMathParser::addVariableBoolean(std::string name, bool v)
{
    name=strip(name);
    jkmpVariable nv;
    nv.type=jkMathParser::jkmpBool;
    nv.boolean=new bool;
    nv.internal=true;
    *(nv.boolean)=v;
    variables[name]=nv;
}

void jkMathParser::addVariable(std::string name, jkMathParser::jkmpResult result)
{
    switch(result.type) {
        case jkmpDouble:
            addVariableDouble(name, result.num);
            break;
        case jkmpString:
            addVariableString(name, result.str);
            break;
        case jkmpBool:
            addVariableBoolean(name, result.boolean);
            break;
    }

}


void jkMathParser::printVariables() {
    if (variables.size()>0) {
        for(std::map<std::string, jkmpVariable>::const_iterator i=variables.begin(); i!=variables.end(); ++i) {
            jkmpVariable v=variables[i->first];
            std::cout<<"'"<<i->first.c_str()<<"'"<<"\t\t";
            if (v.internal) std::cout<<"intern"; else std::cout<<"extern";
            std::cout<<"\t";
            if (v.type==jkMathParser::jkmpBool) std::cout<<"bool\t";
            if (v.type==jkMathParser::jkmpDouble) std::cout<<"double\t";
            if (v.type==jkMathParser::jkmpString) std::cout<<"string\t";

            std::cout<<std::endl;

        }
    }
}

std::vector<std::pair<std::string, jkMathParser::jkmpVariable> > jkMathParser::getVariables()
{
    std::vector<std::pair<std::string, jkmpVariable> > result;
    if (variables.size()>0) {
        for(std::map<std::string, jkmpVariable>::const_iterator i=variables.begin(); i!=variables.end(); ++i) {
            jkmpVariable v=variables[i->first];
            result.push_back(make_pair(i->first, v));
        }
    }
    return result;
}


void jkMathParser::deleteVariable(std::string name) {
    if (variableExists(name)) {
        jkmpVariable v=variables[name];
        if (v.internal) {
            if (v.type==jkMathParser::jkmpDouble) { delete v.num; }
            else if (v.type==jkMathParser::jkmpString) { delete v.str; }
            else if (v.type==jkMathParser::jkmpBool) { delete v.boolean; }
        }
        variables.erase(variables.find(name));
    }
}


void jkMathParser::clearVariables(){
    if (variables.size()>0) {
        for(std::map<std::string, jkmpVariable>::const_iterator i=variables.begin(); i!=variables.end(); ++i) {
            jkmpVariable v=variables[i->first];
            if (v.internal) {
                if (v.type==jkMathParser::jkmpDouble) { delete v.num; }
                else if (v.type==jkMathParser::jkmpString) { delete v.str; }
                else if (v.type==jkMathParser::jkmpBool) { delete v.boolean; }
            }

        }
        variables.clear();
    }
}

// gibt den aktuellen Wert einer Variablen zurück
jkMathParser::jkmpResult jkMathParser::getVariable(std::string name)
{
    if (variableExists(name)) {
        jkmpVariable v=variables[name];
        jkMathParser::jkmpResult r;
        r.type=v.type;
        if (v.type==jkMathParser::jkmpDouble) {
            r.num=*(v.num);
        } else if (v.type==jkMathParser::jkmpString) {
            r.str=*(v.str);
        } else if (v.type==jkMathParser::jkmpBool) {
            r.boolean=*(v.boolean);
        }
        return r;
    } else {
        // error
        //std::cout <<"error for:  '"<<name<<"'"<<std::endl;
        jkmpError(format("variable '%s' does not exist (getVariable)", name.c_str()));
    }
    jkMathParser::jkmpResult res;
    res.isValid=false;
    return res;
}

jkMathParser::jkmpResult jkMathParser::getVariableOrInvalid(std::string name)
{
    if (variableExists(name)) {
        jkmpVariable v=variables[name];
        jkMathParser::jkmpResult r;
        r.type=v.type;
        if (v.type==jkMathParser::jkmpDouble) {
            r.num=*(v.num);
        } else if (v.type==jkMathParser::jkmpString) {
            r.str=*(v.str);
        } else if (v.type==jkMathParser::jkmpBool) {
            r.boolean=*(v.boolean);
        }
        return r;
    }
    jkMathParser::jkmpResult res;
    res.isValid=false;
    return res;
}

jkMathParser::jkmpVariable jkMathParser::getVariableDef(std::string name)
{
  if (variableExists(name)) {
  	return variables[name];
  } else {
    // error
    //std::cout <<"error for:  '"<<name<<"'"<<std::endl;
    jkmpError(format("variable '%s' does not exist (getVariableDef)", name.c_str()));
  }
  return jkmpVariable();
}

jkMathParser::jkmpEvaluateFunc jkMathParser::getFunctionDef(std::string name){
  if (functionExists(name)) {
  	return functions[name].function;
  } else {
    // error
    //std::cout <<name<<std::endl;
    jkmpError(format("function '%s' does not exist (getFunctionDef)", name.c_str()));
  }
  return NULL;
}

void jkMathParser::addTempVariable(std::string name, jkMathParser::jkmpResult value) {
  jkmpTempVariable v;
  v.name=name;
  v.type=value.type;
  v.internal=true;
  if (v.type==jkMathParser::jkmpDouble) { v.num=new double; }
  else if (v.type==jkMathParser::jkmpString) { v.str=new std::string; }
  else if (v.type==jkMathParser::jkmpBool) { v.boolean=new bool; }
  tempvariables.push_back(v);
}

void jkMathParser::setVariableDouble(std::string name, double value) {
    jkMathParser::jkmpResult r;
    r.type=jkMathParser::jkmpDouble;
    r.num=value;
    setVariable(name, r);
}

void jkMathParser::setVariable(std::string name, jkMathParser::jkmpResult value)
{
	bool nexist=!variableExists(name);
	jkmpVariable v=variables[name];
	v.type=value.type;
	if (value.type==jkMathParser::jkmpDouble) {
    if (nexist) {v.num=new double; v.internal=true;}
    *(v.num)=value.num;
  } else if (value.type==jkMathParser::jkmpString) {
    if (nexist) {v.str=new std::string; v.internal=true;}
    *(v.str)=value.str;
  } else if (value.type==jkMathParser::jkmpBool) {
    if (nexist) {v.boolean=new bool; v.internal=true;}
    *(v.boolean)=value.boolean;
  }
  variables[name]=v;
}


// wertet eine Funktion aus
jkMathParser::jkmpResult jkMathParser::evaluateFunction(std::string name, jkMathParser::jkmpResult* params, unsigned char n)
{
  if (functionExists(name)) {
    //std::cout <<"found"    ;
  	return functions[name].function(params, n, this);
  } else {
    // error
    //jkmpError("function does not exist")
    jkmpError(format("function '%s' does not exist (evaluateFunction)", name.c_str()));
  }
  jkMathParser::jkmpResult res;
  res.isValid=false;
  return res;

}

void jkMathParser::addFunction(std::string name, jkmpEvaluateFunc function) {
  jkmpFunctionDescriptor f;
  f.function=function;
  f.name=name;
  functions[name]=f;
}


jkMathParser::jkmpTokenType jkMathParser::getToken(){
	char ch=0;
	while(program->get(ch) && isspace(ch)) {
		;
	}


	switch (ch) {
		case 0:
		case -1:
			return CurrentToken=END;
		case ';':
			return CurrentToken=PRINT;
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
			return CurrentToken=LBRACKET;
		case ')':
			return CurrentToken=RBRACKET;
		case ',':
			return CurrentToken=PARAMETER_DIV;
		case '"':
			return CurrentToken=STRING_DELIM;
		case '^':
			return CurrentToken=POWER;
		case '!':{
			char ch1=0;
			if (*program) program->get(ch1);
			if (ch1=='=') return CurrentToken=COMP_UNEQUAL;
			// else
			program->putback(ch1);
			return CurrentToken=FACTORIAL_LOGIC_NOT;
		}
		case '&':{
			char ch1=0;
			if (*program) program->get(ch1);
			if (ch1=='&') return CurrentToken=LOGIC_AND;
			// else
			program->putback(ch1);
			jkmpError("undefined operator '&'; Did you mean LOGICAL_AND ('&&' / 'and')?");
		}
		case '|':{
			char ch1=0;
			if (*program) program->get(ch1);
			if (ch1=='|') return CurrentToken=LOGIC_OR;
			// else
			program->putback(ch1);
			jkmpError("undefined operator '|'; Did you mean LOGICAL_OR ('||' / 'or')?");
		}
		case '=':{
			char ch1=0;
			if (*program) program->get(ch1);
			if (ch1=='=') return CurrentToken=COMP_EQUALT;
			// else
			program->putback(ch1);
			return CurrentToken=ASSIGN;
		}
		case '>':{
			char ch1=0;
			if (*program) program->get(ch1);
			if (ch1=='=') return CurrentToken=COMP_GEQUAL;
			// else
			program->putback(ch1);
			return CurrentToken=COMP_GREATER;
		}
		case '<':{
			char ch1=0;
			if (*program) program->get(ch1);
			if (ch1=='=') return CurrentToken=COMP_SEQUAL;
			// else
			program->putback(ch1);
			return CurrentToken=COMP_SMALLER;
		}
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':{
			program->putback(ch);
			(*program) >> NumberValue;
			return CurrentToken=NUMBER;
		}
		default:
			if (isalpha(ch) || (ch=='_')) { // try to recognize NAME, LOGIC_TRUE, LOGIC_FALSE, DIFF_LBRACKET
				StringValue=ch;
				while (program->get(ch) && (isalnum(ch) || (ch=='_') || (ch=='.'))) {
                    if (isalnum(ch) || (ch=='_') || (ch=='.')) {
                        StringValue.push_back(ch);
                    }
				}

				program->putback(ch); // now put the last thing read back int the stream, as it
									  // could belong to the next token

				if (StringValue=="true") return CurrentToken=LOGIC_TRUE;
				if (StringValue=="false") return CurrentToken=LOGIC_FALSE;
				if (StringValue=="and") return CurrentToken=LOGIC_AND;
				if (StringValue=="or") return CurrentToken=LOGIC_OR;
				if (StringValue=="xor") return CurrentToken=LOGIC_XOR;
				if (StringValue=="not") return CurrentToken=LOGIC_NOT;
				if (StringValue=="nor") return CurrentToken=LOGIC_NOR;
				if (StringValue=="nand") return CurrentToken=LOGIC_NAND;

				return CurrentToken=NAME;
			}
			// the parser has found an unknown token. an exception will be thrown
			//std::cout<<StringValue<<",   "<<ch<<std::endl;
			jkmpError(format("unknown token currentCharacter='%s', currentString='%s'", chartostr(ch).c_str(), StringValue.c_str()));
	}
    return END;
}


jkMathParser::jkmpNode* jkMathParser::parse(std::string prog){
	program=new std::istringstream(prog);
	jkMathParser::jkmpNode* res=NULL;
	jkMathParser::jkmpNodeList* resList=new jkMathParser::jkmpNodeList(this);
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
        delete resList;
        return res;
    } else {
        return resList;
    }
}

jkMathParser::jkmpResult jkMathParser::evaluate(std::string prog) {
    jkMathParser::jkmpNode* res=parse(prog);
    jkMathParser::jkmpResult r=res->evaluate();
    delete res;
    return r;
}

jkMathParser::jkmpNode* jkMathParser::logicalExpression(bool get){
	jkMathParser::jkmpNode* left=logicalTerm(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case LOGIC_OR:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryBoolNode(jkmpLOPor, left, logicalTerm(true), this, NULL);
				break;
			case LOGIC_XOR:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryBoolNode(jkmpLOPxor, left, logicalTerm(true), this, NULL);
				break;
			case LOGIC_NOR:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryBoolNode(jkmpLOPnor, left, logicalTerm(true), this, NULL);
				break;
			default:
				return left;
		}
}

jkMathParser::jkmpNode* jkMathParser::logicalTerm(bool get){
	jkMathParser::jkmpNode* left=compExpression(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case LOGIC_AND:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryBoolNode(jkmpLOPand, left, compExpression(true), this, NULL);
				break;
			case LOGIC_NAND:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryBoolNode(jkmpLOPnand, left, compExpression(true), this, NULL);
				break;
			default:
				return left;
		}
}

jkMathParser::jkmpNode* jkMathParser::compExpression(bool get){
	jkMathParser::jkmpNode* left=mathExpression(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case COMP_EQUALT:
				left= (jkMathParser::jkmpNode*)new jkmpCompareNode(jkmpCOMPequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_UNEQUAL:
				left= (jkMathParser::jkmpNode*)new jkmpCompareNode(jkmpCOMPnequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_GREATER:
				left= (jkMathParser::jkmpNode*)new jkmpCompareNode(jkmpCOMPgreater, left, mathExpression(true), this, NULL);
				break;
			case COMP_SMALLER:
				left= (jkMathParser::jkmpNode*)new jkmpCompareNode(jkmpCOMPlesser, left, mathExpression(true), this, NULL);
				break;
			case COMP_GEQUAL:
				left= (jkMathParser::jkmpNode*)new jkmpCompareNode(jkmpCOMPgreaterequal, left, mathExpression(true), this, NULL);
				break;
			case COMP_SEQUAL:
				left= (jkMathParser::jkmpNode*)new jkmpCompareNode(jkmpCOMPlesserequal, left, mathExpression(true), this, NULL);
				break;
			default:
				return left;
		}
}


jkMathParser::jkmpNode* jkMathParser::mathExpression(bool get){
	jkMathParser::jkmpNode* left=mathTerm(get);

	for(;;) // forever, do until you find anything else than an expressions
		switch(CurrentToken) {
			case PLUS:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryArithmeticNode('+', left, mathTerm(true), this, NULL);
				break;
			case MINUS:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryArithmeticNode('-', left, mathTerm(true), this, NULL);
				break;
			default:
				return left;
		}
}

jkMathParser::jkmpNode* jkMathParser::mathTerm(bool get){
	jkMathParser::jkmpNode* left=primary(get);

	for(;;) // forever, do until you find anything else than a term
		switch(CurrentToken) {
			case MUL:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryArithmeticNode('*', left, primary(true), this, NULL);
				break;
			case DIV:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryArithmeticNode('/', left, primary(true), this, NULL);
				break;
			case MODULO:
				left= (jkMathParser::jkmpNode*)new jkmpBinaryArithmeticNode('%', left, primary(true), this, NULL);
				break;
			default:
				return left;
		}
}

jkMathParser::jkmpNode* jkMathParser::primary(bool get){
    jkMathParser::jkmpNode* res=NULL;

	if (get) getToken();

	switch(CurrentToken) {
		case NUMBER: {
			jkMathParser::jkmpResult val;
			val.type=jkMathParser::jkmpDouble;
			val.num=NumberValue;
			getToken();
			res= (jkMathParser::jkmpNode*)new jkmpConstantNode(val, this, NULL);
			break;
		}

		case NAME: {
			//jkMathParser::jkmpNode* def=NULL;
 			std::string varname=StringValue;
    	getToken();
 			if (CurrentToken == ASSIGN) { // assign a variable name
    			res=new jkmpVariableAssignNode(varname, logicalExpression(true), this, NULL);
 			} else if (CurrentToken == LBRACKET) { // function found
    			jkMathParser::jkmpNode** params=(jkMathParser::jkmpNode**)malloc(255*sizeof(jkMathParser::jkmpNode*));
    			unsigned char num=0;


          	getToken();
          	while ((CurrentToken != RBRACKET)&&(CurrentToken!=END)) {
          		jkMathParser::jkmpNode* parameter=logicalExpression(num>0);
          		params[num]=parameter;
          		num++;
             	if ((CurrentToken!=RBRACKET)&&(CurrentToken!=PARAMETER_DIV)&&(CurrentToken!=END))
                 jkmpError(format("')' or ',' expected, but '%s' found", currenttokentostring().c_str()));
          	}

    				if ( CurrentToken != RBRACKET ) jkmpError(format("')' expected, but '%s' found", currenttokentostring().c_str()));;
    				jkMathParser::jkmpNode** p=NULL;
            if (num>0) {
               p=(jkMathParser::jkmpNode**)malloc(sizeof(jkMathParser::jkmpNode*) * num);
               for (int i=0; i<num; i++) {
                 p[i]=params[i];
               }
            }
	    			res=new jkmpFunctionNode(varname, p, num, this, NULL);
	    			free(params);
	    			getToken();

  		} else {
     	  res=(jkMathParser::jkmpNode*)new jkmpVariableNode(varname, this, NULL);
 			}
			//res= def;
			break;

		}

		case STRING_DELIM: {// found primary: "data" == string constant
			jkMathParser::jkmpResult val;
			val.type=jkMathParser::jkmpString;
			val.str=readDelim('"');
			res= (jkMathParser::jkmpNode*)new jkmpConstantNode(val, this, NULL);
			getToken();
			break;
		}

		case MINUS:  // found primary:  - primary
			res= (jkMathParser::jkmpNode*)new jkmpUnaryNode('-', primary(true), this, NULL);
			break;

		case LOGIC_NOT:
			res= (jkMathParser::jkmpNode*)new jkmpUnaryNode('!', primary(true), this, NULL);
			break;

		case LBRACKET: { // found primary ( expression )
			jkMathParser::jkmpNode* expr=logicalExpression(true);
			if (CurrentToken != RBRACKET) jkmpError(format("')' expected, but '%s' found", currenttokentostring().c_str()));
			getToken(); // swallow ")"
			res= expr;
			break;
		}
		case FACTORIAL_LOGIC_NOT:
    case LOGIC_TRUE: {// found 'true'
			jkMathParser::jkmpResult val;
			val.type=jkMathParser::jkmpBool;
			val.boolean=true;
			res= (jkMathParser::jkmpNode*)new jkmpConstantNode(val, this, NULL);
			getToken();
			break;
   }
		case LOGIC_FALSE: {// found 'false'
			jkMathParser::jkmpResult val;
			val.type=jkMathParser::jkmpBool;
			val.boolean=false;
			res= (jkMathParser::jkmpNode*)new jkmpConstantNode(val, this, NULL);
			getToken();
			break;
   }

		default:
   		jkmpError("primary expected");

	}

	if (CurrentToken==POWER) {
		res=(jkMathParser::jkmpNode*)new jkmpBinaryArithmeticNode('^', res, primary(true), this, NULL);
	}

	return res;

}

std::string jkMathParser::readDelim(char delimiter){
	std::string res="";
	char ch=0;

 	while(program->get(ch)) {
		if (ch==delimiter ) {
		    char ch1=program->peek();
		    if (ch1==delimiter) {
		            program->get(ch);
		            res=res+delimiter;
            } else {
                break;
            }
		} else if ((*program) && (ch!=delimiter)) res=res+ch;
	}

	return res;
}









/******************************************************************************************
 * Klassenhierarchie, um Ausdrücke darzustellen
 ******************************************************************************************/
jkMathParser::jkmpUnaryNode::jkmpUnaryNode(char op, jkMathParser::jkmpNode* c, jkMathParser* p, jkMathParser::jkmpNode* par){
  child=c;
  child->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

jkMathParser::jkmpResult jkMathParser::jkmpUnaryNode::evaluate(){
  jkMathParser::jkmpResult c=child->evaluate();
  jkMathParser::jkmpResult res;
  res=c;

  switch(operation) {
    case '!':
         if (c.type==jkMathParser::jkmpBool) {
           res.boolean=!c.boolean;
           return res;
         } else parser->jkmpError("'!' only defined for bool");
         break;
    case '-':
         if (c.type==jkMathParser::jkmpDouble) {
           res.num=-c.num;
           return res;
         } else parser->jkmpError("'-' only defined for double");
         break;
     default: parser->jkmpError("unknown unary operation");
  }
  res.isValid=false;
  return res;
}







jkMathParser::jkmpBinaryArithmeticNode::jkmpBinaryArithmeticNode(char op, jkMathParser::jkmpNode* l, jkMathParser::jkmpNode* r, jkMathParser* p, jkMathParser::jkmpNode* par){
  left=l;
  right=r;
  left->setParent(this);
  right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

jkMathParser::jkmpResult jkMathParser::jkmpBinaryArithmeticNode::evaluate(){
  jkMathParser::jkmpResult l=left->evaluate();
  jkMathParser::jkmpResult r=right->evaluate();
  jkMathParser::jkmpResult res;

  switch(operation) {
    case '+':
         if (l.type==r.type) {
            if (l.type==jkMathParser::jkmpDouble) {
               res.type=jkMathParser::jkmpDouble;
               res.num=l.num+r.num;
               return res;
            }
            if (l.type==jkMathParser::jkmpString) {
               res.type=jkMathParser::jkmpString;
               res.str=l.str+r.str;
               return res;
            }
            parser->jkmpError("bool may not be added");
         } else parser->jkmpError("different Types while adding");
         break;
    case '-':
         if (l.type==r.type) {
            if (l.type==jkMathParser::jkmpDouble) {
               res.type=jkMathParser::jkmpDouble;
               res.num=l.num-r.num;
               return res;
            }
            parser->jkmpError("bool or string may not be subtractet");
         } else parser->jkmpError("different Types while subtracting");
         break;
    case '*':
         if (l.type==r.type) {
            if (l.type==jkMathParser::jkmpDouble) {
               res.type=jkMathParser::jkmpDouble;
               res.num=l.num*r.num;
               return res;
            }
            parser->jkmpError("bool or string may not be multiplied");
         } else parser->jkmpError("different Types while multiplying");
         break;
    case '/':
         if (l.type==r.type) {
            if (l.type==jkMathParser::jkmpDouble) {
               res.type=jkMathParser::jkmpDouble;
               res.num=l.num/r.num;
               return res;
            }
            parser->jkmpError("bool or string may not be divided");
         } else parser->jkmpError("different Types while dividing");
         break;
    case '%':
         if (l.type==r.type) {
            if (l.type==jkMathParser::jkmpDouble) {
               if ((l.num==floor(l.num)) && (r.num==floor(r.num)))  {
                   res.type=jkMathParser::jkmpDouble;
                   res.num=l.num/r.num;
                   return res;
               } else {
                   parser->jkmpError("modulo is only defined for integer arguments");
               }
            }
            parser->jkmpError("modulo is not defined for bool or string arguments");
         } else parser->jkmpError("different Types while calculating modulo");
         break;
    case '^':
         if (l.type==r.type) {
            if (l.type==jkMathParser::jkmpDouble) {
               res.type=jkMathParser::jkmpDouble;
               res.num=pow(l.num, r.num);
               return res;
            }
            parser->jkmpError("bool or string may not be taken to powers");
         } else parser->jkmpError("different types while taking to power");
         break;
     default: parser->jkmpError("unknown arithmetic operation");
  }
  res.isValid=false;
  return res;
}







jkMathParser::jkmpCompareNode::jkmpCompareNode(char op, jkMathParser::jkmpNode* l, jkMathParser::jkmpNode* r, jkMathParser* p, jkMathParser::jkmpNode* par){
  left=l;
  right=r;
  left->setParent(this);
  right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

jkMathParser::jkmpResult jkMathParser::jkmpCompareNode::evaluate(){
  jkMathParser::jkmpResult l=left->evaluate();
  jkMathParser::jkmpResult r=right->evaluate();
  jkMathParser::jkmpResult res;
  res.type=jkMathParser::jkmpBool;

  if (l.type!=r.type) parser->jkmpError("you can't compare different datatypes");

  switch(operation) {
    case jkmpCOMPequal:
        if (l.type==jkMathParser::jkmpDouble) {
           res.boolean=(l.num==r.num);
           return res;
        }
        if (l.type==jkMathParser::jkmpBool) {
           res.boolean=(l.boolean==r.boolean);
           return res;
        }
        if (l.type==jkMathParser::jkmpString) {
           res.boolean=(l.str==r.str);
           return res;
        }
        break;
    case jkmpCOMPnequal:
        if (l.type==jkMathParser::jkmpDouble) {
           res.boolean=(l.num!=r.num);
           return res;
        }
        if (l.type==jkMathParser::jkmpBool) {
           res.boolean=(l.boolean!=r.boolean);
           return res;
        }
        if (l.type==jkMathParser::jkmpString) {
           res.boolean=(l.str!=r.str);
           return res;
        }
        break;
    case jkmpCOMPgreater:
        if (l.type==jkMathParser::jkmpDouble) {
           res.boolean=(l.num>r.num);
           return res;
        }
        if (l.type==jkMathParser::jkmpBool) {
           res.boolean=(l.boolean>r.boolean);
           return res;
        }
        if (l.type==jkMathParser::jkmpString) {
           res.boolean=(l.str>r.str);
           return res;
        }
        break;
    case jkmpCOMPlesser:
        if (l.type==jkMathParser::jkmpDouble) {
           res.boolean=(l.num<r.num);
           return res;
        }
        if (l.type==jkMathParser::jkmpBool) {
           res.boolean=(l.boolean<r.boolean);
           return res;
        }
        if (l.type==jkMathParser::jkmpString) {
           res.boolean=(l.str<r.str);
           return res;
        }
        break;
    case jkmpCOMPgreaterequal:
        if (l.type==jkMathParser::jkmpDouble) {
           res.boolean=(l.num>=r.num);
           return res;
        }
        if (l.type==jkMathParser::jkmpBool) {
           res.boolean=(l.boolean>=r.boolean);
           return res;
        }
        if (l.type==jkMathParser::jkmpString) {
           res.boolean=(l.str>=r.str);
           return res;
        }
        break;
    case jkmpCOMPlesserequal:
        if (l.type==jkMathParser::jkmpDouble) {
           res.boolean=(l.num<=r.num);
           return res;
        }
        if (l.type==jkMathParser::jkmpBool) {
           res.boolean=(l.boolean<=r.boolean);
           return res;
        }
        if (l.type==jkMathParser::jkmpString) {
           res.boolean=(l.str<=r.str);
           return res;
        }
        break;
     default: parser->jkmpError("unknown compare operation");
  }
  res.isValid=false;
  return res;
}







jkMathParser::jkmpBinaryBoolNode::jkmpBinaryBoolNode(char op, jkMathParser::jkmpNode* l, jkMathParser::jkmpNode* r, jkMathParser* p, jkMathParser::jkmpNode* par){
  left=l;
  right=r;
  left->setParent(this);
  right->setParent(this);
  setParser(p);
  setParent(par);
  operation=op;
}

jkMathParser::jkmpResult jkMathParser::jkmpBinaryBoolNode::evaluate(){
  jkMathParser::jkmpResult l=left->evaluate();
  jkMathParser::jkmpResult r=right->evaluate();
  jkMathParser::jkmpResult res;

  if ((l.type!=jkMathParser::jkmpBool)||(r.type!=jkMathParser::jkmpBool)) parser->jkmpError("logical operations only for bool");

  switch(operation) {
    case jkmpLOPand:
        res.type=jkMathParser::jkmpBool;
        res.boolean=l.boolean&&r.boolean;
        return res;
        break;
    case jkmpLOPor:
        res.type=jkMathParser::jkmpBool;
        res.boolean=l.boolean||r.boolean;
        return res;
        break;
    case jkmpLOPnor:
        res.type=jkMathParser::jkmpBool;
        res.boolean=!(l.boolean||r.boolean);
        return res;
        break;
    case jkmpLOPxor:
        res.type=jkMathParser::jkmpBool;
        res.boolean=(l.boolean&& (!r.boolean))||(r.boolean&& (!l.boolean));
        return res;
        break;
    case jkmpLOPnand:
        res.type=jkMathParser::jkmpBool;
        res.boolean=!(l.boolean&&r.boolean);
        return res;
        break;
     default: parser->jkmpError("unknown error");
  }
  res.isValid=false;
  return res;
}




jkMathParser::jkmpVariableNode::jkmpVariableNode(std::string name, jkMathParser* p, jkMathParser::jkmpNode* par) {
  var=name;
  setParser(p);
  setParent(par);
};

jkMathParser::jkmpResult jkMathParser::jkmpVariableNode::evaluate() {
  return getParser()->getVariable(var);
};




void jkMathParser::jkmpNodeList::add(jkMathParser::jkmpNode* n){
  list.push_back(n);
}


jkMathParser::jkmpResult jkMathParser::jkmpNodeList::evaluate(){
  int n=getCount();
  jkMathParser::jkmpResult res;
  //std::cout<<"Nodelist.count()="<<n<<std::endl;
  if (n>0) {
     for (int i=0; i<n; i++) {
        res=list[i]->evaluate();
     }
     return res;
  } else parser->jkmpError("NodeList empty");
  res.isValid=false;
  return res;
}

jkMathParser::jkmpNodeList::~jkmpNodeList() {
/*  if (getCount()>0) {
     for (int i=0; i<getCount(); i++) {
        delete list[i];
     }
  };
  list.clear();*/
}


jkMathParser::jkmpVariableAssignNode::jkmpVariableAssignNode(std::string var, jkMathParser::jkmpNode* c, jkMathParser* p, jkMathParser::jkmpNode* par){
  child=c;
  child->setParent(this);
  setParser(p);
  setParent(par);
  variable=var;
//  std::cout<<"assign: "<<var<<std::endl;
}

jkMathParser::jkmpResult jkMathParser::jkmpVariableAssignNode::evaluate(){
  jkMathParser::jkmpResult res=child->evaluate();
//  std::cout<<"assign: "<<variable<<"    "<<res.num<<std::endl;
  getParser()->setVariable(variable, res);
  return res;
}

jkMathParser::jkmpFunctionNode::jkmpFunctionNode(std::string name, jkMathParser::jkmpNode** c, unsigned char num, jkMathParser* p, jkMathParser::jkmpNode* par) {
  child=c;
  n=num;
  fun=name;
  setParser(p);
  setParent(par);
  if (n>0) {
    for (int i=0; i<n; i++) {
      child[i]->setParent(this);
    }
  }
  function=p->getFunctionDef(name);
}

jkMathParser::jkmpResult jkMathParser::jkmpFunctionNode::evaluate() {
  jkMathParser::jkmpResult data[255];
  if (n>0) {
    for (int i=0; i<n; i++) {
      data[i]=child[i]->evaluate();
    }
  }
//  jkMathParser::jkmpResult r= getParser()->evaluateFunction(fun, data,n);
  return function(data,n, parser);
}


jkMathParser::jkmpFunctionNode::~jkmpFunctionNode() {
  /*if (n>0) {
    for (int i=0; i<n; i++) {
      delete child[i];
    }
  }*/
    if ((child!=NULL) && (n>0)) {
      for (int i=0; i<n; i++) {
	delete child[i];
      }
      free(child);
    }
}

jkMathParser::jkmpResult::jkmpResult()
{
    isValid=true;
    type=jkmpDouble;   /*!< \brief type of the result */
    str="";       /*!< \brief contains result if \c type==jkmpString */
    num=0;            /*!< \brief contains result if \c type==jkmpDouble */
    boolean=false;          /*!< \brief contains result if \c type==jkmpBool */
}

jkMathParser::jkmpVariable::jkmpVariable()
{
    type=jkmpDouble;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==jkmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==jkmpDouble */
    boolean=NULL;
}

void jkMathParser::setArgCV(int argc, char **argv) {
	this->argc=argc;
	this->argv=argv;
}

std::string jkMathParser::getArgCVParam(std::string name, std::string defaultResult) {
        if (!argv ) return defaultResult;
        if (argc<=1) return defaultResult;
	for (int i=1; i<argc; i++) {
		if (name==std::string(argv[i]) && i+1<argc) {
		    return std::string(argv[i+1]);
		}
	}
	return defaultResult;
}
