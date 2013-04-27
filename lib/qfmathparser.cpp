/** \file qfmathparser.cpp
 *  \ingroup qfmp
 */

#define COMPILING_THIS_QFMATHPARSER
#include "qfmathparser.h" // class's header file
#include <iostream>
#include <float.h>
#include <QDebug>
#include <stdint.h>


/******************************************************************************************
 * default-Funktionen für den Parser
 ******************************************************************************************/

QFMathParser::qfmpResult fdoubleToQString(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpString;
  if (n!=1) p->qfmpError("doubletostring accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("doubletostring needs double argument");
  r.str=doubleToQString(params[0].num);
  return r;
}


QFMathParser::qfmpResult fIntToStr(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpString;
  if (n!=1) p->qfmpError("inttostr accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("inttostr needs double argument");
  r.str=QString::number(params[0].num);
  return r;
}

QFMathParser::qfmpResult fboolToQString(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpString;
  if (n!=1) p->qfmpError("booltostring accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpBool) p->qfmpError("booltostring needs bool argument");
  r.str=(r.boolean)?"true":"false";
  return r;
}


QFMathParser::qfmpResult fToSystemPathSeparator(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpString;
  if (n!=1) p->qfmpError("tosystempathseparator accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpString) p->qfmpError("tosystempathseparator needs string argument");
  r.str="";
  for (int i=0; i<params[0].str.size(); i++) {
      QChar ch=params[0].str[i];
      if (ch=='/' || ch=='\\') ch=QDir::separator();
      r.str+=ch;
  }

  return r;
}


QFMathParser::qfmpResult fSinc(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sinc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sinc needs double argument");
  //r.num=sin(params[0].num)/params[0].num;

  r.num=qfSinc(params[0].num);

  return r;
}

QFMathParser::qfmpResult fTanc(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tanc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tanc needs double argument");
  r.num=(params[0].num==0)?1.0:tan(params[0].num)/params[0].num;
  return r;
}

QFMathParser::qfmpResult fSin(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sin accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sin needs double argument");
  r.num=sin(params[0].num);
  return r;
}

QFMathParser::qfmpResult fCos(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("cos accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("cos needs double argument");
  r.num=cos(params[0].num);
  return r;
}

QFMathParser::qfmpResult fTan(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tan accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tan needs double argument");
  r.num=tan(params[0].num);
  return r;
}

QFMathParser::qfmpResult fExp(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("exp accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("exp needs double argument");
  r.num=exp(params[0].num);
  return r;
}

QFMathParser::qfmpResult fLog(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("log accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("log needs double argument");
  r.num=log(params[0].num);
  return r;
}

QFMathParser::qfmpResult fLog10(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("log10 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("log10 needs double argument");
  r.num=log10(params[0].num);
  return r;
}

QFMathParser::qfmpResult fLog2(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("log2 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("log2 needs double argument");
  r.num=log2(params[0].num);
  return r;
}

QFMathParser::qfmpResult fSqrt(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sqrt accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sqrt needs double argument");
  r.num=sqrt(params[0].num);
  return r;
}
QFMathParser::qfmpResult fCbrt(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("cbrt accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("cbrt needs double argument");
  r.num=cbrt(params[0].num);
  return r;
}

QFMathParser::qfmpResult fSqr(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sqr accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sqr needs double argument");
  r.num=params[0].num*params[0].num;
  return r;
}

QFMathParser::qfmpResult fAbs(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("abs accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("abs needs double argument");
  r.num=fabs(params[0].num);
  return r;
}

QFMathParser::qfmpResult fIf(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  if (n!=3) p->qfmpError("If accepts 3 argument");
  if (params[0].type!=QFMathParser::qfmpBool) p->qfmpError("If needs bool as first argument");
  if (params[0].boolean) return params[1]; else return params[2];
}

QFMathParser::qfmpResult fASin(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("asin accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("asin needs double argument");
  r.num=asin(params[0].num);
  return r;
}

QFMathParser::qfmpResult fACos(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("acos accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("acos needs double argument");
  r.num=acos(params[0].num);
  return r;
}

QFMathParser::qfmpResult fATan(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("atan accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("atan needs double argument");
  r.num=atan(params[0].num);
  return r;
}

QFMathParser::qfmpResult fATan2(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("atan2 accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("atan2 needs double argument");
  r.num=atan2(params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fSinh(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sinh accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sinh needs double argument");
  r.num=sinh(params[0].num);
  return r;
}

QFMathParser::qfmpResult fCosh(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("cosh accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("cosh needs double argument");
  r.num=cosh(params[0].num);
  return r;
}

QFMathParser::qfmpResult fTanh(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tanh accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tanh needs double argument");
  r.num=tanh(params[0].num);
  return r;
}

QFMathParser::qfmpResult fErf(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("erf accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("erf needs double argument");
  r.num=erf(params[0].num);
  return r;
}

QFMathParser::qfmpResult fErfc(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("erfc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("erfc needs double argument");
  r.num=erfc(params[0].num);
  return r;
}

QFMathParser::qfmpResult flGamma(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("lgamma accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("lgamma needs double argument");
  r.num=lgamma(params[0].num);
  return r;
}

QFMathParser::qfmpResult ftGamma(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tgamma accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tgamma needs double argument");
  r.num=tgamma(params[0].num);
  return r;
}

QFMathParser::qfmpResult fJ0(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("j0 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("j0 needs double argument");
  r.num=j0(params[0].num);
  return r;
}

QFMathParser::qfmpResult fJ1(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("j1 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("j1 needs double argument");
  r.num=j1(params[0].num);
  return r;
}

QFMathParser::qfmpResult fY0(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("y0 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("y0 needs double argument");
  r.num=y0(params[0].num);
  return r;
}

QFMathParser::qfmpResult fY1(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("y1 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("y1 needs double argument");
  r.num=y1(params[0].num);
  return r;
}

QFMathParser::qfmpResult fYn(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("yn accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("yn needs double argument");
  r.num=yn((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fJn(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("jn accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("jn needs double argument");
  r.num=jn((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fSRand(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  //if (n!=1) p->qfmpError("srand accepts 1 argument");
  if (n>=0 && params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("srand needs double argument");
  r.num=0;
  //srand((unsigned int)params[0].num);
  if (n>=0) p->get_rng()->seed(uint32_t(params[0].num));
  else p->get_rng()->seed();
  return r;
}

QFMathParser::qfmpResult fRand(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n>2) p->qfmpError("rand accepts 0, 1 or 2 arguments");
  //r.num=double(rand())/double(RAND_MAX);
  if (n==0) r.num=p->get_rng()->rand();
  else if (n==2) r.num=p->get_rng()->rand()*(params[1].num-params[0].num)+params[0].num;
  else r.num=p->get_rng()->rand(params[0].num);
  return r;
}

QFMathParser::qfmpResult fRandNorm(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  double mean=0;
  double var=1;
  bool ok=true;
  if (n>2) p->qfmpError("randnorm accepts 0,1 or 2 argument");
  if (n>0 && (params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("randnorm needs a double as first argument (if any)");
  if (n>1 && (params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("randnorm needs a double as second argument (if any)");
  if (n==1) {
      var=params[0].num;
  } else if (n>1) {
      mean=params[0].num;
      var=params[1].num;
  }
  r.num=p->get_rng()->randNorm(mean, var);
  return r;
}

QFMathParser::qfmpResult fRandInt(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  if (n>2) p->qfmpError("randint accepts 0, 1 or 2 argument");
  r.type=QFMathParser::qfmpDouble;
  if (n==0) r.num=p->get_rng()->randInt();
  else if (n==2) r.num=p->get_rng()->randInt()*((uint32_t)params[1].num-(uint32_t)params[0].num)+(uint32_t)params[0].num;
  else r.num=p->get_rng()->randInt((uint32_t)params[0].num);
  return r;
}

QFMathParser::qfmpResult fCeil(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("ceil accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("ceil needs double argument");
  r.num=ceil(params[0].num);
  return r;
}

QFMathParser::qfmpResult fFloor(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("floor accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("floor needs double argument");
  r.num=floor(params[0].num);
  return r;
}

QFMathParser::qfmpResult fTrunc(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("trunc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("trunc needs double argument");
  r.num=trunc(params[0].num);
  return r;
}

QFMathParser::qfmpResult fRound(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("round accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("round needs double argument");
  r.num=round(params[0].num);
  return r;
}

QFMathParser::qfmpResult fFMod(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("fmod accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("fmod needs double argument");
  r.num=fmod((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fMin(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("min accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("min needs double argument");
  r.num=fmin((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fMax(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("max accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("max needs double argument");
  r.num=fmax((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult QFSPIMLightsheetEvaluationItem_fGauss(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("gauss accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("gauss needs double argument");
  r.num=exp(-2*params[0].num*params[0].num/params[1].num/params[1].num);
  return r;
}

QFMathParser::qfmpResult fSlit(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("slit accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("slit needs double argument");
  r.num=((params[0].num>=-1.0*params[1].num/2)&&(params[0].num<=params[1].num/2))?1.0:0.0;
  return r;
}

QFMathParser::qfmpResult fTheta(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("theta accepts 1 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("theta needs double argument");
  r.num=(params[0].num>=0)?1.0:0.0;
  return r;
}

QFMathParser::qfmpResult fSigmoid(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sigmoid accepts 1 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("sigmoid needs double argument");
  r.num=1.0/(1+exp(-1.0*params[0].num));
  return r;
}

QFMathParser::qfmpResult fSign(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sign accepts 1 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("sign needs double argument");
  r.num=0.0;
  if (params[0].num<0) { r.num=-1; }
  else if (params[0].num>0) { r.num=+1; }
  return r;
}



inline QString strip(QString s) {
  QString r;
  for (int i=0; i<s.size(); i++){
    if ((s[i]!=' ')&&(s[i]!='\t')&&(s[i]!='\r')&&(s[i]!='\n')) {
      r+=s[i];
    }
  }
  return r;
}



QString QFMathParser::resultToString(const QFMathParser::qfmpResult& r)
{
    return r.toString();
}

QString QFMathParser::resultToTypeString(const QFMathParser::qfmpResult& r)
{
    return r.toTypeString();
}

QFMathParser::qfmpResult QFMathParser::getInvalidResult()
{
    qfmpResult res;
    res.isValid=false;
    return res;
}

void QFMathParser::qfmpError(QString st)
{
    //qDebug()<<"ERROR: "<<st;
    lastError.append(st);
    errors++;
}

QString QFMathParser::tokentostring(QFMathParser::qfmpTokenType token) {
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
        case LPARENTHESE: return "LPARENTHESE '('";
        case RPARENTHESE: return "RPARENTHESE ')'";
        case LBRACKET: return "LBRACKET '['";
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
}

void QFMathParser::addStandardFunctions(){
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
    addFunction("randint", fRandInt);
    addFunction("randnorm", fRandNorm);
    addFunction("srand", fSRand);
	addFunction("ceil", fCeil);
	addFunction("floor", fFloor);
	addFunction("trunc", fTrunc);
	addFunction("round", fRound);
	addFunction("fmod", fFMod);
	addFunction("min", fMin);
	addFunction("max", fMax);
	addFunction("inttostr", fIntToStr);
    addFunction("doubletostring", fdoubleToQString);
    addFunction("booltostring", fboolToQString);
	addFunction("gauss", QFSPIMLightsheetEvaluationItem_fGauss);
	addFunction("slit", fSlit);
	addFunction("theta", fTheta);
	addFunction("tanc", fTanc);
	addFunction("sigmoid", fSigmoid);
	addFunction("sign", fSign);
	addFunction("tosystempathseparator", fToSystemPathSeparator);
    /*addFunction("mean", fMean);
    addFunction("std", fStd);
    addFunction("var", fVar);
    addFunction("median", fMedian);
    addFunction("quantile", fQuantile);*/
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

void QFMathParser::addVariable(const QString& name, const QFMathParser::qfmpResult& result)
{
    environment.addVariable(name, result);

}

void QFMathParser::setVariable(const QString& name, const QFMathParser::qfmpResult& result)
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



void QFMathParser::clearFunctions()
{
    environment.clearFunctions();
}


void QFMathParser::clearVariables(){
    environment.clearVariables();
}

// gibt den aktuellen Wert einer Variablen zurück
QFMathParser::qfmpResult QFMathParser::getVariable(QString name)
{
    return environment.getVariable(name);
}

QFMathParser::qfmpResult QFMathParser::getVariableOrInvalid(QString name)
{
    if (environment.variableExists(name)) return environment.getVariable(name);
    QFMathParser::qfmpResult r;
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
QFMathParser::qfmpResult QFMathParser::evaluateFunction(const QString& name, const QVector<qfmpResult> &params)
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
			return CurrentToken=PARAMETER_DIV;
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
            //qfmpError("undefined operator '&'; Did you mean LOGICAL_AND ('&&' / 'and')?");
            break;
		}
		case '|':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='|') return CurrentToken=LOGIC_OR;
			// else
            putbackStream(program, ch1);
            return CurrentToken=BINARY_OR;
            //qfmpError("undefined operator '|'; Did you mean LOGICAL_OR ('||' / 'or')?");
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
		case '5': case '6': case '7': case '8': case '9':{
            putbackStream(program, ch);
			(*program) >> NumberValue;
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

QFMathParser::qfmpResult QFMathParser::evaluate(QString prog) {
    QFMathParser::qfmpNode* res=parse(prog);
    QFMathParser::qfmpResult r=res->evaluate();
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
    QFMathParser::qfmpNode* left=primary(get);

	for(;;) // forever, do until you find anything else than a term
		switch(CurrentToken) {
			case MUL:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('*', left, primary(true), this, NULL);
				break;
			case DIV:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('/', left, primary(true), this, NULL);
				break;
			case MODULO:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('%', left, primary(true), this, NULL);
				break;
            case BINARY_AND:
                left= (QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('&', left, mathTerm(true), this, NULL);
                break;
            default:
				return left;
		}
}

QFMathParser::qfmpNode* QFMathParser::primary(bool get){
    QFMathParser::qfmpNode* res=NULL;

	if (get) getToken();

	switch(CurrentToken) {
		case NUMBER: {
            QFMathParser::qfmpResult val;
            val.type=QFMathParser::qfmpDouble;
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
                } else if (CurrentToken == LPARENTHESE) { // function found
                    //QFMathParser::qfmpNode** params=(QFMathParser::qfmpNode**)malloc(255*sizeof(QFMathParser::qfmpNode*));
                    QVector<QFMathParser::qfmpNode*> params;


                    getToken();
                    while ((CurrentToken != RPARENTHESE)&&(CurrentToken!=END)) {
                        QFMathParser::qfmpNode* parameter=logicalExpression(params.size()>0);
                        params.append(parameter);
                        if ((CurrentToken!=RPARENTHESE)&&(CurrentToken!=PARAMETER_DIV)&&(CurrentToken!=END)) {
                            qfmpError(QString("')' or ',' expected, but '%1' found").arg(currenttokentostring()));
                            return NULL;
                        }

                    }

                    if ( CurrentToken != RPARENTHESE ) {
                        qfmpError(QString("')' expected, but '%1' found").arg(currenttokentostring()));
                        return NULL;
                    }
                    res=new qfmpFunctionNode(varname, params, this, NULL);
                    getToken();

                } else {
                  res=(QFMathParser::qfmpNode*)new qfmpVariableNode(varname, this, NULL);
                }
                //res= def;
                break;

		}

		case STRING_DELIM: {// found primary: "data" == string constant
            QFMathParser::qfmpResult val;
            val.type=QFMathParser::qfmpString;
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
            QFMathParser::qfmpResult val;
            val.type=QFMathParser::qfmpBool;
			val.boolean=true;
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
        }
		case LOGIC_FALSE: {// found 'false'
            QFMathParser::qfmpResult val;
            val.type=QFMathParser::qfmpBool;
			val.boolean=false;
            res= (QFMathParser::qfmpNode*)new qfmpConstantNode(val, this, NULL);
			getToken();
			break;
        }

		default:
            qfmpError("primary expected");
            return new qfmpInvalidNode(this, NULL);

	}

	if (CurrentToken==POWER) {
        res=(QFMathParser::qfmpNode*)new qfmpBinaryArithmeticNode('^', res, primary(true), this, NULL);
	}

    if (!res) res=new qfmpInvalidNode(this, NULL);

    return res;

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
QFMathParser::qfmpUnaryNode::qfmpUnaryNode(char op, QFMathParser::qfmpNode* c, QFMathParser* p, QFMathParser::qfmpNode* par){
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

QFMathParser::qfmpResult QFMathParser::qfmpUnaryNode::evaluate(){
  QFMathParser::qfmpResult c=child->evaluate();
  QFMathParser::qfmpResult res;
  res=c;

  switch(operation) {
    case '!':
         if (c.type==QFMathParser::qfmpBool) {
           res.boolean=!c.boolean;
           return res;
         } else parser->qfmpError("'!' only defined for bool");
         break;
      case '-':
           if (c.type==QFMathParser::qfmpDouble) {
             res.num=-c.num;
             return res;
           } else parser->qfmpError("'-' only defined for double");
           break;
      case '~':
           if (c.isInteger()) {
               res.num=double(~c.toInteger());
               return res;
           } else parser->qfmpError("'~' only defined for integer numbers");
           break;
     default: parser->qfmpError("unknown unary operation");
  }
  res.isValid=false;
  return res;
}







QFMathParser::qfmpBinaryArithmeticNode::qfmpBinaryArithmeticNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par){
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

QFMathParser::qfmpResult QFMathParser::qfmpBinaryArithmeticNode::evaluate(){
  QFMathParser::qfmpResult l;
  if (left) l=left->evaluate();
  QFMathParser::qfmpResult r;
  if (right) r=right->evaluate();
  QFMathParser::qfmpResult res;

  switch(operation) {
    case '+':
         if (l.type==r.type) {
            if (l.type==QFMathParser::qfmpDouble) {
               res.type=QFMathParser::qfmpDouble;
               res.num=l.num+r.num;
               return res;
            }
            if (l.type==QFMathParser::qfmpString) {
               res.type=QFMathParser::qfmpString;
               res.str=l.str+r.str;
               return res;
            }
            parser->qfmpError("bool may not be added");
         } else parser->qfmpError("different Types while adding");
         break;
    case '-':
         if (l.type==r.type) {
            if (l.type==QFMathParser::qfmpDouble) {
               res.type=QFMathParser::qfmpDouble;
               res.num=l.num-r.num;
               return res;
            }
            parser->qfmpError("bool or string may not be subtractet");
         } else parser->qfmpError("different Types while subtracting");
         break;
    case '*':
         if (l.type==r.type) {
            if (l.type==QFMathParser::qfmpDouble) {
               res.type=QFMathParser::qfmpDouble;
               res.num=l.num*r.num;
               return res;
            }
            parser->qfmpError("bool or string may not be multiplied");
         } else parser->qfmpError("different Types while multiplying");
         break;
    case '/':
         if (l.type==r.type) {
            if (l.type==QFMathParser::qfmpDouble) {
               res.type=QFMathParser::qfmpDouble;
               res.num=l.num/r.num;
               return res;
            }
            parser->qfmpError("bool or string may not be divided");
         } else parser->qfmpError("different Types while dividing");
         break;
    case '%':
         if (l.type==r.type) {
            if (l.type==QFMathParser::qfmpDouble) {
               if ((l.num==floor(l.num)) && (r.num==floor(r.num)))  {
                   res.type=QFMathParser::qfmpDouble;
                   res.num=l.num/r.num;
                   return res;
               } else {
                   parser->qfmpError("modulo is only defined for integer arguments");
               }
            }
            parser->qfmpError("modulo is not defined for bool or string arguments");
         } else parser->qfmpError("different Types while calculating modulo");
         break;
    case '^':
         if (l.type==r.type) {
            if (l.type==QFMathParser::qfmpDouble) {
               res.type=QFMathParser::qfmpDouble;
               res.num=pow(l.num, r.num);
               return res;
            }
            parser->qfmpError("bool or string may not be taken to powers");
         } else parser->qfmpError("different types while taking to power");
         break;

    case '&':
        if (!(l.isInteger() && r.isInteger())) {
            parser->qfmpError("lhs and rhs argument of binary AND have to be integer numbers");
            break;
        }
        res.type=QFMathParser::qfmpDouble;
        res.num=double(l.toInteger()&r.toInteger());
        return res;
        break;
    case '|':
        if (!(l.isInteger() && r.isInteger())) {
            parser->qfmpError("lhs and rhs argument of binary OR have to be integer numbers");
            break;
        }
        res.type=QFMathParser::qfmpDouble;
        res.num=double(l.toInteger()|r.toInteger());
        return res;
        break;


      default: parser->qfmpError("unknown arithmetic operation");
  }
  res.isValid=false;
  return res;
}







QFMathParser::qfmpCompareNode::qfmpCompareNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par){
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

QFMathParser::qfmpResult QFMathParser::qfmpCompareNode::evaluate(){
    QFMathParser::qfmpResult l;
    if (left) l=left->evaluate();
    QFMathParser::qfmpResult r;
    if (right) r=right->evaluate();  QFMathParser::qfmpResult res;
  res.type=QFMathParser::qfmpBool;

  if (l.type!=r.type) parser->qfmpError("you can't compare different datatypes");

  switch(operation) {
    case qfmpCOMPequal:
        if (l.type==QFMathParser::qfmpDouble) {
           res.boolean=(l.num==r.num);
           return res;
        }
        if (l.type==QFMathParser::qfmpBool) {
           res.boolean=(l.boolean==r.boolean);
           return res;
        }
        if (l.type==QFMathParser::qfmpString) {
           res.boolean=(l.str==r.str);
           return res;
        }
        break;
    case qfmpCOMPnequal:
        if (l.type==QFMathParser::qfmpDouble) {
           res.boolean=(l.num!=r.num);
           return res;
        }
        if (l.type==QFMathParser::qfmpBool) {
           res.boolean=(l.boolean!=r.boolean);
           return res;
        }
        if (l.type==QFMathParser::qfmpString) {
           res.boolean=(l.str!=r.str);
           return res;
        }
        break;
    case qfmpCOMPgreater:
        if (l.type==QFMathParser::qfmpDouble) {
           res.boolean=(l.num>r.num);
           return res;
        }
        if (l.type==QFMathParser::qfmpBool) {
           res.boolean=(l.boolean>r.boolean);
           return res;
        }
        if (l.type==QFMathParser::qfmpString) {
           res.boolean=(l.str>r.str);
           return res;
        }
        break;
    case qfmpCOMPlesser:
        if (l.type==QFMathParser::qfmpDouble) {
           res.boolean=(l.num<r.num);
           return res;
        }
        if (l.type==QFMathParser::qfmpBool) {
           res.boolean=(l.boolean<r.boolean);
           return res;
        }
        if (l.type==QFMathParser::qfmpString) {
           res.boolean=(l.str<r.str);
           return res;
        }
        break;
    case qfmpCOMPgreaterequal:
        if (l.type==QFMathParser::qfmpDouble) {
           res.boolean=(l.num>=r.num);
           return res;
        }
        if (l.type==QFMathParser::qfmpBool) {
           res.boolean=(l.boolean>=r.boolean);
           return res;
        }
        if (l.type==QFMathParser::qfmpString) {
           res.boolean=(l.str>=r.str);
           return res;
        }
        break;
    case qfmpCOMPlesserequal:
        if (l.type==QFMathParser::qfmpDouble) {
           res.boolean=(l.num<=r.num);
           return res;
        }
        if (l.type==QFMathParser::qfmpBool) {
           res.boolean=(l.boolean<=r.boolean);
           return res;
        }
        if (l.type==QFMathParser::qfmpString) {
           res.boolean=(l.str<=r.str);
           return res;
        }
        break;
     default: parser->qfmpError("unknown compare operation");
  }
  res.isValid=false;
  return res;
}







QFMathParser::qfmpBinaryBoolNode::qfmpBinaryBoolNode(char op, QFMathParser::qfmpNode* l, QFMathParser::qfmpNode* r, QFMathParser* p, QFMathParser::qfmpNode* par){
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

QFMathParser::qfmpResult QFMathParser::qfmpBinaryBoolNode::evaluate(){
    QFMathParser::qfmpResult l;
    if (left) l=left->evaluate();
    QFMathParser::qfmpResult r;
    if (right) r=right->evaluate();
    QFMathParser::qfmpResult res;

  if ((l.type!=QFMathParser::qfmpBool)||(r.type!=QFMathParser::qfmpBool)) parser->qfmpError("logical operations only for bool");
  else {
      switch(operation) {
          case qfmpLOPand:
              res.type=QFMathParser::qfmpBool;
              res.boolean=l.boolean&&r.boolean;
              return res;
              break;
          case qfmpLOPor:
              res.type=QFMathParser::qfmpBool;
              res.boolean=l.boolean||r.boolean;
              return res;
              break;
          case qfmpLOPnor:
              res.type=QFMathParser::qfmpBool;
              res.boolean=!(l.boolean||r.boolean);
              return res;
              break;
          case qfmpLOPxor:
              res.type=QFMathParser::qfmpBool;
              res.boolean=(l.boolean&& (!r.boolean))||(r.boolean&& (!l.boolean));
              return res;
              break;
          case qfmpLOPnand:
              res.type=QFMathParser::qfmpBool;
              res.boolean=!(l.boolean&&r.boolean);
              return res;
              break;

          default: parser->qfmpError("unknown error");
              break;
      }
  }
  res.isValid=false;
  return res;
}




QFMathParser::qfmpVariableNode::qfmpVariableNode(QString name, QFMathParser* p, QFMathParser::qfmpNode* par) {
  var=name;
  setParser(p);
  setParent(par);
};

QFMathParser::qfmpResult QFMathParser::qfmpVariableNode::evaluate() {
  return getParser()->getVariable(var);
};




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


QFMathParser::qfmpResult QFMathParser::qfmpNodeList::evaluate(){
  int n=getCount();
  QFMathParser::qfmpResult res;
  //std::cout<<"Nodelist.count()="<<n<<std::endl;
  if (n>0) {
     for (int i=0; i<n; i++) {
        if (list[i]) res=list[i]->evaluate();
     }
     return res;
  } else parser->qfmpError("NodeList empty");
  res.isValid=false;
  return res;
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

QFMathParser::qfmpVariableAssignNode::qfmpVariableAssignNode(QString var, QFMathParser::qfmpNode* c, QFMathParser* p, QFMathParser::qfmpNode* par){
  child=c;
  if (child) child->setParent(this);
  setParser(p);
  setParent(par);
  variable=var;
//  std::cout<<"assign: "<<var<<std::endl;
}

QFMathParser::qfmpResult QFMathParser::qfmpVariableAssignNode::evaluate(){
  QFMathParser::qfmpResult res;
  if (child) res=child->evaluate();
//  std::cout<<"assign: "<<variable<<"    "<<res.num<<std::endl;
  getParser()->setVariable(variable, res);
  return res;
}

QFMathParser::qfmpFunctionNode::qfmpFunctionNode(QString name, QVector<qfmpNode *> params, QFMathParser *p, qfmpNode *par) {
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

QFMathParser::qfmpResult QFMathParser::qfmpFunctionNode::evaluate() {
  QVector<QFMathParser::qfmpResult> data;
  if (child.size()>0) {
    for (int i=0; i<child.size(); i++) {
      if (child[i]) data.append(child[i]->evaluate());
      else data.append(QFMathParser::getInvalidResult());
    }
  }
  return parser->evaluateFunction(fun, data);
}


QFMathParser::qfmpFunctionNode::~qfmpFunctionNode() {

    if (child.size()>0) {
      for (int i=0; i<child.size(); i++) {
        if (child[i]) delete child[i];
      }
      child.clear();
    }
}

QFMathParser::qfmpResult::qfmpResult()
{
    isValid=true;
    type=qfmpDouble;   /*!< \brief type of the result */
    str="";       /*!< \brief contains result if \c type==qfmpString */
    num=0;            /*!< \brief contains result if \c type==qfmpDouble */
    boolean=false;          /*!< \brief contains result if \c type==qfmpBool */
    numVec.clear();
}

QString QFMathParser::qfmpResult::toString() const
{
    switch(type) {
        case qfmpDouble: return doubleToQString(num);
        case qfmpDoubleVector: return QString("[ ")+doubleVecToQString(numVec)+QString(" ]");
        case qfmpString: return str;
        case qfmpBool: return boolToQString(boolean);
    }
    return "";

}

QString QFMathParser::qfmpResult::toTypeString() const
{
    if (!isValid) return QObject::tr("[INVALID]");
    switch(type) {
        case qfmpDouble: return doubleToQString(num)+QObject::tr(" [number]");
        case qfmpDoubleVector: return QString("[ ")+doubleVecToQString(numVec)+QObject::tr(" ] [number vector]");
        case qfmpString: return str+QObject::tr(" [string]");
        case qfmpBool: return boolToQString(boolean)+QObject::tr(" [bool]");
    }
    return "";

}

int64_t QFMathParser::qfmpResult::toInteger() const
{
    if (type==qfmpDouble) return num;
    if (type==qfmpBool) return (boolean)?1:0;
    return 0;
}

bool QFMathParser::qfmpResult::isInteger() const
{
    return type==qfmpDouble;
}

void QFMathParser::qfmpResult::setDouble(double val)
{
    type=qfmpDouble;
    num=val;
}

void QFMathParser::qfmpResult::setBoolean(bool val)
{
    type=qfmpBool;
    boolean=val;
}

void QFMathParser::qfmpResult::setString(const QString &val)
{
    type=qfmpString;
    str=val;
}

void QFMathParser::qfmpResult::setDoubleVec(const QVector<double> &val)
{
    type=qfmpDoubleVector;
    numVec=val;
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

QFMathParser::qfmpResult QFMathParser::qfmpVariable::toResult() const
{
    QFMathParser::qfmpResult r;
    r.type=type;
    r.isValid=true;
    if (type==QFMathParser::qfmpDouble && num) {
        r.num=*(num);
    } else if (type==QFMathParser::qfmpString && str) {
        r.str=*(str);
    } else if (type==QFMathParser::qfmpBool && boolean) {
        r.boolean=*(boolean);
    } else if (type==QFMathParser::qfmpDoubleVector && numVec) {
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

void QFMathParser::qfmpVariable::set(const QFMathParser::qfmpResult &result)
{
    if (type==result.type) {
        switch (result.type) {
            case qfmpDouble: { if (!num) num=new double; internal=true; } *num=result.num; break;
            case qfmpDoubleVector: { if (!numVec) numVec=new QVector<double>; internal=true; } *numVec=result.numVec; break;
            case qfmpString: { if (!str) str=new QString; internal=true; } *str=result.str; break;
            case qfmpBool: { if (!boolean) boolean=new bool; internal=true; } *boolean=result.boolean; break;
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

QFMathParser::qfmpResult QFMathParser::qfmpFunctionDescriptor::evaluate(const QVector<qfmpResult> &parameters, QFMathParser* parent) const
{
    QFMathParser::qfmpResult r;
    r.isValid=true;
    if (type==QFMathParser::functionC) {
        return function(parameters.data(), parameters.size(), parent);
    }
    if (type==QFMathParser::functionNode) {
        if (parameterNames.size()!=parameters.size()) {
            parent->qfmpError(QObject::tr("function '%1' takes %2 parameters, but %3 parameters were given").arg(name).arg(parameterNames.size()).arg(parameters.size()));
        } else {
            if (parent && parameterNames.size()>0) {
                parent->enterBlock();
                for (int i=0; i<parameterNames.size(); i++) {
                    parent->addVariable(parameterNames[i], parameters[i]);
                }
                r=functionNode->evaluate();
                parent->leaveBlock();
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
    if (type==QFMathParser::functionNode) res=QString("%1(%2): node\n").arg(name).arg(parameterNames.join(", "));
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
}

void QFMathParser::executionEnvironment::leaveBlock()
{
    if (currentLevel>0) {
        currentLevel--;
        QStringList keys=variables.keys();
        for (int i=0; i<keys.size(); i++) {
            while (variables[keys[i]].last().first>currentLevel) {
                variables[keys[i]].last().second.clearMemory();
                variables[keys[i]].removeLast();
            }
            if (variables[keys[i]].isEmpty()) variables.remove(keys[i]);
        }

        keys=functions.keys();
        for (int i=0; i<keys.size(); i++) {
            while (functions[keys[i]].last().first>currentLevel) {
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
}

void QFMathParser::executionEnvironment::setVariableDouble(const QString &name, double result)
{
    //qDebug()<<"executionEnvironment::setVariableDouble("<<name<<result<<")";
    QFMathParser::qfmpResult r;
    r.setDouble(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableDoubleVec(const QString &name, const QVector<double> &result)
{
    QFMathParser::qfmpResult r;
    r.setDoubleVec(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableString(const QString &name, const QString &result)
{
    QFMathParser::qfmpResult r;
    r.setString(result);
    setVariable(name, r);
}

void QFMathParser::executionEnvironment::setVariableBoolean(const QString &name, bool result)
{
    QFMathParser::qfmpResult r;
    r.setBoolean(result);
    setVariable(name, r);

}

void QFMathParser::executionEnvironment::addVariableDouble(const QString &name, double result)
{
    //qDebug()<<"executionEnvironment::addVariableDouble("<<name<<result<<")";
    QFMathParser::qfmpResult r;
    r.setDouble(result);
    //qDebug()<<"  adding "<<r.toTypeString();
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableDoubleVec(const QString &name, const QVector<double> &result)
{
    QFMathParser::qfmpResult r;
    r.setDoubleVec(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableString(const QString &name, const QString &result)
{
    QFMathParser::qfmpResult r;
    r.setString(result);
    addVariable(name, r);
}

void QFMathParser::executionEnvironment::addVariableBoolean(const QString &name, bool result)
{
    QFMathParser::qfmpResult r;
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

QFMathParser::qfmpResult QFMathParser::executionEnvironment::getVariable(const QString &name) const
{
    QFMathParser::qfmpResult res;
    res.isValid=false;
    if (variables.contains(name) && variables[name].size()>0) {
        res=variables[name].last().second.toResult();
        res.isValid=true;
        return res;
    }
    if (parent) parent->qfmpError(QObject::tr("the variable '%1' does not exist").arg(name));
    return res;
}

QFMathParser::qfmpResult QFMathParser::executionEnvironment::evaluateFunction(const QString &name, const QVector<QFMathParser::qfmpResult> &parameters) const
{
    QFMathParser::qfmpResult res;
    res.isValid=false;
    if (functions.contains(name) && functions[name].size()>0) {
        res=functions[name].last().second.evaluate(parameters, parent);
        res.isValid=true;
    }
    if (parent) parent->qfmpError(QObject::tr("the function '%1' does not exist").arg(name));
    return res;

}

void QFMathParser::executionEnvironment::addVariable(const QString &name, const QFMathParser::qfmpResult &result)
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

        QList<QPair<int, qfmpVariable> > l;
        l.append(qMakePair(currentLevel, v));
        //qDebug()<<"   addidng 2";
        variables[name]=l;
        //qDebug()<<"   addidng 3";
    }
}

void QFMathParser::executionEnvironment::setVariable(const QString &name, const QFMathParser::qfmpResult &result)
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

}

void QFMathParser::executionEnvironment::clearVariables()
{
    variables.clear();
}

void QFMathParser::executionEnvironment::clearFunctions()
{
    functions.clear();
}
