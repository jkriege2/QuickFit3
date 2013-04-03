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

QFMathParser::qfmpResult fdoubleToQString(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpString;
  if (n!=1) p->qfmpError("doubleToQString accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("doubleToQString needs double argument");
  r.str=doubleToQString(params[0].num);
  return r;
}


QFMathParser::qfmpResult fIntToStr(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpString;
  if (n!=1) p->qfmpError("inttostr accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("inttostr needs double argument");
  r.str=QString::number(params[0].num);
  return r;
}

QFMathParser::qfmpResult fboolToQString(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpString;
  if (n!=1) p->qfmpError("boolToQString accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpBool) p->qfmpError("doubleToQString needs bool argument");
  r.str=(r.boolean)?"true":"false";
  return r;
}


QFMathParser::qfmpResult fToSystemPathSeparator(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
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


QFMathParser::qfmpResult fSinc(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sinc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sinc needs double argument");
  //r.num=sin(params[0].num)/params[0].num;

  r.num=qfSinc(params[0].num);

  return r;
}

QFMathParser::qfmpResult fTanc(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tanc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tanc needs double argument");
  r.num=(params[0].num==0)?1.0:tan(params[0].num)/params[0].num;
  return r;
}

QFMathParser::qfmpResult fSin(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sin accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sin needs double argument");
  r.num=sin(params[0].num);
  return r;
}

QFMathParser::qfmpResult fCos(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("cos accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("cos needs double argument");
  r.num=cos(params[0].num);
  return r;
}

QFMathParser::qfmpResult fTan(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tan accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tan needs double argument");
  r.num=tan(params[0].num);
  return r;
}

QFMathParser::qfmpResult fExp(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("exp accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("exp needs double argument");
  r.num=exp(params[0].num);
  return r;
}

QFMathParser::qfmpResult fLog(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("log accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("log needs double argument");
  r.num=log(params[0].num);
  return r;
}

QFMathParser::qfmpResult fLog10(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("log10 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("log10 needs double argument");
  r.num=log10(params[0].num);
  return r;
}

QFMathParser::qfmpResult fLog2(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("log2 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("log2 needs double argument");
  r.num=log2(params[0].num);
  return r;
}

QFMathParser::qfmpResult fSqrt(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sqrt accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sqrt needs double argument");
  r.num=sqrt(params[0].num);
  return r;
}
QFMathParser::qfmpResult fCbrt(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("cbrt accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("cbrt needs double argument");
  r.num=cbrt(params[0].num);
  return r;
}

QFMathParser::qfmpResult fSqr(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sqr accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sqr needs double argument");
  r.num=params[0].num*params[0].num;
  return r;
}

QFMathParser::qfmpResult fAbs(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("abs accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("abs needs double argument");
  r.num=fabs(params[0].num);
  return r;
}

QFMathParser::qfmpResult fIf(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  if (n!=3) p->qfmpError("If accepts 3 argument");
  if (params[0].type!=QFMathParser::qfmpBool) p->qfmpError("If needs bool as first argument");
  if (params[0].boolean) return params[1]; else return params[2];
}

QFMathParser::qfmpResult fASin(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("asin accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("asin needs double argument");
  r.num=asin(params[0].num);
  return r;
}

QFMathParser::qfmpResult fACos(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("acos accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("acos needs double argument");
  r.num=acos(params[0].num);
  return r;
}

QFMathParser::qfmpResult fATan(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("atan accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("atan needs double argument");
  r.num=atan(params[0].num);
  return r;
}

QFMathParser::qfmpResult fATan2(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("atan2 accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("atan2 needs double argument");
  r.num=atan2(params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fSinh(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sinh accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("sinh needs double argument");
  r.num=sinh(params[0].num);
  return r;
}

QFMathParser::qfmpResult fCosh(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("cosh accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("cosh needs double argument");
  r.num=cosh(params[0].num);
  return r;
}

QFMathParser::qfmpResult fTanh(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tanh accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tanh needs double argument");
  r.num=tanh(params[0].num);
  return r;
}

QFMathParser::qfmpResult fErf(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("erf accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("erf needs double argument");
  r.num=erf(params[0].num);
  return r;
}

QFMathParser::qfmpResult fErfc(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("erfc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("erfc needs double argument");
  r.num=erfc(params[0].num);
  return r;
}

QFMathParser::qfmpResult flGamma(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("lgamma accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("lgamma needs double argument");
  r.num=lgamma(params[0].num);
  return r;
}

QFMathParser::qfmpResult ftGamma(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("tgamma accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("tgamma needs double argument");
  r.num=tgamma(params[0].num);
  return r;
}

QFMathParser::qfmpResult fJ0(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("j0 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("j0 needs double argument");
  r.num=j0(params[0].num);
  return r;
}

QFMathParser::qfmpResult fJ1(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("j1 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("j1 needs double argument");
  r.num=j1(params[0].num);
  return r;
}

QFMathParser::qfmpResult fY0(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("y0 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("y0 needs double argument");
  r.num=y0(params[0].num);
  return r;
}

QFMathParser::qfmpResult fY1(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("y1 accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("y1 needs double argument");
  r.num=y1(params[0].num);
  return r;
}

QFMathParser::qfmpResult fYn(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("yn accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("yn needs double argument");
  r.num=yn((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fJn(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("jn accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("jn needs double argument");
  r.num=jn((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fSRand(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
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

QFMathParser::qfmpResult fRand(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n>2) p->qfmpError("rand accepts 0, 1 or 2 arguments");
  //r.num=double(rand())/double(RAND_MAX);
  if (n==0) r.num=p->get_rng()->rand();
  else if (n==2) r.num=p->get_rng()->rand()*(params[1].num-params[0].num)+params[0].num;
  else r.num=p->get_rng()->rand(params[0].num);
  return r;
}

QFMathParser::qfmpResult fRandNorm(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
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

QFMathParser::qfmpResult fRandInt(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  if (n>2) p->qfmpError("randint accepts 0, 1 or 2 argument");
  r.type=QFMathParser::qfmpDouble;
  if (n==0) r.num=p->get_rng()->randInt();
  else if (n==2) r.num=p->get_rng()->randInt()*((uint32_t)params[1].num-(uint32_t)params[0].num)+(uint32_t)params[0].num;
  else r.num=p->get_rng()->randInt((uint32_t)params[0].num);
  return r;
}

QFMathParser::qfmpResult fCeil(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("ceil accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("ceil needs double argument");
  r.num=ceil(params[0].num);
  return r;
}

QFMathParser::qfmpResult fFloor(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("floor accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("floor needs double argument");
  r.num=floor(params[0].num);
  return r;
}

QFMathParser::qfmpResult fTrunc(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("trunc accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("trunc needs double argument");
  r.num=trunc(params[0].num);
  return r;
}

QFMathParser::qfmpResult fRound(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("round accepts 1 argument");
  if (params[0].type!=QFMathParser::qfmpDouble) p->qfmpError("round needs double argument");
  r.num=round(params[0].num);
  return r;
}

QFMathParser::qfmpResult fFMod(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("fmod accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("fmod needs double argument");
  r.num=fmod((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fMin(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("min accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("min needs double argument");
  r.num=fmin((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult fMax(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("max accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("max needs double argument");
  r.num=fmax((int)params[0].num, params[1].num);
  return r;
}

QFMathParser::qfmpResult QFSPIMLightsheetEvaluationItem_fGauss(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("gauss accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("gauss needs double argument");
  r.num=exp(-2*params[0].num*params[0].num/params[1].num/params[1].num);
  return r;
}

QFMathParser::qfmpResult fSlit(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=2) p->qfmpError("slit accepts 2 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("slit needs double argument");
  r.num=((params[0].num>=-1.0*params[1].num/2)&&(params[0].num<=params[1].num/2))?1.0:0.0;
  return r;
}

QFMathParser::qfmpResult fTheta(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("theta accepts 1 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("theta needs double argument");
  r.num=(params[0].num>=0)?1.0:0.0;
  return r;
}

QFMathParser::qfmpResult fSigmoid(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFMathParser::qfmpResult r;
  r.type=QFMathParser::qfmpDouble;
  if (n!=1) p->qfmpError("sigmoid accepts 1 argument");
  if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("sigmoid needs double argument");
  r.num=1.0/(1+exp(-1.0*params[0].num));
  return r;
}

QFMathParser::qfmpResult fSign(QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p){
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
    qDebug()<<"ERROR: "<<st;
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

QString QFMathParser::currenttokentostring() {
    switch(CurrentToken) {
        case END: return "END";
        case PRINT: return "PRINT (;)";
        case PARAMETER_DIV: return "PARAMETER_DIV (,)";
	    case STRING_DELIM: return "STRING_DELIM (' or \")";
        case NAME: return QString("NAME (%1)").arg(StringValue);
        case NUMBER: return QString("NUMBER (%1)").arg(NumberValue);
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
QFMathParser::QFMathParser() {
    addStandardFunctions();
    addStandardVariables();
    resetErrors();
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
    addFunction("doubleToQString", fdoubleToQString);
    addFunction("boolToQString", fboolToQString);
	addFunction("gauss", QFSPIMLightsheetEvaluationItem_fGauss);
	addFunction("slit", fSlit);
	addFunction("theta", fTheta);
	addFunction("tanc", fTanc);
	addFunction("sigmoid", fSigmoid);
	addFunction("sign", fSign);
	addFunction("tosystempathseparator", fToSystemPathSeparator);
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

// fügt eine Variable hinzu. Der Speicher wird extern verwaltet
void QFMathParser::addVariableDouble(QString name, double* v)
{
    name=strip(name);
    qfmpVariable nv;
    nv.type=QFMathParser::qfmpDouble;
    nv.num=v;
    if (variableExists(name)) nv.internal=variables[name].internal; else nv.internal=false;
    variables[name]=nv;
}

void QFMathParser::addVariableString(QString name, QString* v)
{
    name=strip(name);
    qfmpVariable nv;
    nv.type=QFMathParser::qfmpString;
    nv.str=v;
    if (variableExists(name)) nv.internal=variables[name].internal; else nv.internal=false;
    variables[name]=nv;
}

void QFMathParser::addVariableBoolean(QString name, bool* v)
{
    name=strip(name);
    qfmpVariable nv;
    nv.type=QFMathParser::qfmpBool;
    nv.boolean=v;
    if (variableExists(name)) nv.internal=variables[name].internal; else nv.internal=false;
    variables[name]=nv;
}

void QFMathParser::addVariableDouble(QString name, double v)
{
    name=strip(name);
    qfmpVariable nv;
    nv.type=QFMathParser::qfmpDouble;
    nv.num=new double;//(double*)malloc(sizeof(double));
    nv.internal=true;
    *(nv.num)=v;
//  std::cout<<*(nv.num)<<std::endl;
    variables[name]=nv;
}

void QFMathParser::addVariableString(QString name, QString v)
{
    name=strip(name);
    qfmpVariable nv;
    nv.type=QFMathParser::qfmpString;
    nv.str=new QString;
    nv.internal=true;
    *(nv.str)=v;
    variables[strip(name)]=nv;
}

void QFMathParser::addVariableBoolean(QString name, bool v)
{
    name=strip(name);
    qfmpVariable nv;
    nv.type=QFMathParser::qfmpBool;
    nv.boolean=new bool;
    nv.internal=true;
    *(nv.boolean)=v;
    variables[name]=nv;
}

void QFMathParser::addVariable(QString name, QFMathParser::qfmpResult result)
{
    switch(result.type) {
        case qfmpDouble:
            addVariableDouble(name, result.num);
            break;
        case qfmpString:
            addVariableString(name, result.str);
            break;
        case qfmpBool:
            addVariableBoolean(name, result.boolean);
            break;
    }

}


QString QFMathParser::printVariables() {
    QString res="";
    if (variables.size()>0) {
        for(QMap<QString, qfmpVariable>::const_iterator i=variables.begin(); i!=variables.end(); ++i) {
            qfmpVariable v=variables[i.key()];
            res+="'"+i.key()+"'"+"\t\t";
            if (v.internal) res+="intern"; else res+="extern";
            res+="\t";
            if (v.type==QFMathParser::qfmpBool) res+="bool\t";
            if (v.type==QFMathParser::qfmpDouble) res+="double\t";
            if (v.type==QFMathParser::qfmpString) res+="string\t";

            res+="\n";

        }
    }
    qDebug()<<res;
    return res;
}

QList<QPair<QString, QFMathParser::qfmpVariable> > QFMathParser::getVariables()
{
    QList<QPair<QString, qfmpVariable> > result;
    if (variables.size()>0) {
        for(QMap<QString, qfmpVariable>::const_iterator i=variables.begin(); i!=variables.end(); ++i) {
            qfmpVariable v=variables[i.key()];
            result.push_back(qMakePair(i.key(), v));
        }
    }
    return result;
}


void QFMathParser::deleteVariable(QString name) {
    if (variableExists(name)) {
        qfmpVariable v=variables[name];
        if (v.internal) {
            if (v.type==QFMathParser::qfmpDouble) { delete v.num; }
            else if (v.type==QFMathParser::qfmpString) { delete v.str; }
            else if (v.type==QFMathParser::qfmpBool) { delete v.boolean; }
        }
        variables.erase(variables.find(name));
    }
}


void QFMathParser::clearVariables(){
    if (variables.size()>0) {
        for(QMap<QString, qfmpVariable>::const_iterator i=variables.begin(); i!=variables.end(); ++i) {
            qfmpVariable v=variables[i.key()];
            if (v.internal) {
                if (v.type==QFMathParser::qfmpDouble) { delete v.num; }
                else if (v.type==QFMathParser::qfmpString) { delete v.str; }
                else if (v.type==QFMathParser::qfmpBool) { delete v.boolean; }
            }

        }
        variables.clear();
    }
}

// gibt den aktuellen Wert einer Variablen zurück
QFMathParser::qfmpResult QFMathParser::getVariable(QString name)
{
    if (variableExists(name)) {
        qfmpVariable v=variables[name];
        QFMathParser::qfmpResult r;
        r.type=v.type;
        if (v.type==QFMathParser::qfmpDouble) {
            r.num=*(v.num);
        } else if (v.type==QFMathParser::qfmpString) {
            r.str=*(v.str);
        } else if (v.type==QFMathParser::qfmpBool) {
            r.boolean=*(v.boolean);
        }
        return r;
    } else {
        // error
        //std::cout <<"error for:  '"<<name<<"'"<<std::endl;
        qfmpError(QString("variable '%1' does not exist (getVariable)").arg(name));
    }
    QFMathParser::qfmpResult res;
    res.isValid=false;
    return res;
}

QFMathParser::qfmpResult QFMathParser::getVariableOrInvalid(QString name)
{
    if (variableExists(name)) {
        qfmpVariable v=variables[name];
        QFMathParser::qfmpResult r;
        r.type=v.type;
        if (v.type==QFMathParser::qfmpDouble) {
            r.num=*(v.num);
        } else if (v.type==QFMathParser::qfmpString) {
            r.str=*(v.str);
        } else if (v.type==QFMathParser::qfmpBool) {
            r.boolean=*(v.boolean);
        }
        return r;
    }
    QFMathParser::qfmpResult res;
    res.isValid=false;
    return res;
}

QFMathParser::qfmpVariable QFMathParser::getVariableDef(QString name)
{
  if (variableExists(name)) {
  	return variables[name];
  } else {
    // error
    //std::cout <<"error for:  '"<<name<<"'"<<std::endl;
    qfmpError(QString("variable '%1' does not exist (getVariableDef)").arg(name));
  }
  return qfmpVariable();
}

QFMathParser::qfmpEvaluateFunc QFMathParser::getFunctionDef(QString name){
  if (functionExists(name)) {
  	return functions[name].function;
  } else {
    // error
    //std::cout <<name<<std::endl;
    qfmpError(QString("function '%1' does not exist (getFunctionDef)").arg(name));
  }
  return NULL;
}

void QFMathParser::addTempVariable(QString name, QFMathParser::qfmpResult value) {
  qfmpTempVariable v;
  v.name=name;
  v.type=value.type;
  v.internal=true;
  if (v.type==QFMathParser::qfmpDouble) { v.num=new double; }
  else if (v.type==QFMathParser::qfmpString) { v.str=new QString; }
  else if (v.type==QFMathParser::qfmpBool) { v.boolean=new bool; }
  tempvariables.push_back(v);
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

void QFMathParser::setVariableDouble(QString name, double value) {
    QFMathParser::qfmpResult r;
    r.type=QFMathParser::qfmpDouble;
    r.num=value;
    setVariable(name, r);
}

void QFMathParser::setVariable(QString name, QFMathParser::qfmpResult value)
{
	bool nexist=!variableExists(name);
    qfmpVariable v=variables[name];
	v.type=value.type;
    if (value.type==QFMathParser::qfmpDouble) {
    if (nexist) {v.num=new double; v.internal=true;}
    *(v.num)=value.num;
  } else if (value.type==QFMathParser::qfmpString) {
    if (nexist) {v.str=new QString; v.internal=true;}
    *(v.str)=value.str;
  } else if (value.type==QFMathParser::qfmpBool) {
    if (nexist) {v.boolean=new bool; v.internal=true;}
    *(v.boolean)=value.boolean;
  }
  variables[name]=v;
}


// wertet eine Funktion aus
QFMathParser::qfmpResult QFMathParser::evaluateFunction(QString name, QFMathParser::qfmpResult* params, unsigned char n)
{
  if (functionExists(name)) {
    //std::cout <<"found"    ;
  	return functions[name].function(params, n, this);
  } else {
    // error
    //qfmpError("function does not exist")
    qfmpError(QString("function '%1' does not exist (evaluateFunction)").arg(name));
  }
  QFMathParser::qfmpResult res;
  res.isValid=false;
  return res;

}

void QFMathParser::addFunction(QString name, qfmpEvaluateFunc function) {
  qfmpFunctionDescriptor f;
  f.function=function;
  f.name=name;
  functions[name]=f;
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
            qfmpError("undefined operator '&'; Did you mean LOGICAL_AND ('&&' / 'and')?");
            break;
		}
		case '|':{
            QChar ch1=0;
            if (program) getFromStream(program, ch1);
			if (ch1=='|') return CurrentToken=LOGIC_OR;
			// else
            putbackStream(program, ch1);
            qfmpError("undefined operator '|'; Did you mean LOGICAL_OR ('||' / 'or')?");
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
                } else if (CurrentToken == LBRACKET) { // function found
                    //QFMathParser::qfmpNode** params=(QFMathParser::qfmpNode**)malloc(255*sizeof(QFMathParser::qfmpNode*));
                    QVector<QFMathParser::qfmpNode*> params;


                    getToken();
                    while ((CurrentToken != RBRACKET)&&(CurrentToken!=END)) {
                        QFMathParser::qfmpNode* parameter=logicalExpression(params.size()>0);
                        params.append(parameter);
                        if ((CurrentToken!=RBRACKET)&&(CurrentToken!=PARAMETER_DIV)&&(CurrentToken!=END)) {
                            qfmpError(QString("')' or ',' expected, but '%1' found").arg(currenttokentostring()));
                            return NULL;
                        }

                    }

                    if ( CurrentToken != RBRACKET ) {
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

		case LOGIC_NOT:
            res= (QFMathParser::qfmpNode*)new qfmpUnaryNode('!', primary(true), this, NULL);
			break;

		case LBRACKET: { // found primary ( expression )
            QFMathParser::qfmpNode* expr=logicalExpression(true);
            if (CurrentToken != RBRACKET) {
                qfmpError(QString("')' expected, but '%1' found").arg(currenttokentostring()));
                return NULL;
            }
			getToken(); // swallow ")"
			res= expr;
			break;
		}
		case FACTORIAL_LOGIC_NOT:
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
  function=p->getFunctionDef(name);
}

QFMathParser::qfmpResult QFMathParser::qfmpFunctionNode::evaluate() {
  QVector<QFMathParser::qfmpResult> data;
  if (child.size()>0) {
    for (int i=0; i<child.size(); i++) {
      if (child[i]) data.append(child[i]->evaluate());
      else data.append(QFMathParser::getInvalidResult());
    }
  }
//  jkMathParser::Result r= getParser()->evaluateFunction(fun, data,n);
  return function(data.data(),data.size(), parser);
}


QFMathParser::qfmpFunctionNode::~qfmpFunctionNode() {
  /*if (n>0) {
    for (int i=0; i<n; i++) {
      delete child[i];
    }
  }*/
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
}

QString QFMathParser::qfmpResult::toString() const
{
    switch(type) {
        case qfmpDouble: return doubleToQString(num);
        case qfmpString: return str;
        case qfmpBool: return boolToQString(boolean);
    }
    return "";

}

QString QFMathParser::qfmpResult::toTypeString() const
{
    switch(type) {
        case qfmpDouble: return doubleToQString(num)+QObject::tr(" [number]");
        case qfmpString: return str+QObject::tr(" [string]");
        case qfmpBool: return boolToQString(boolean)+QObject::tr(" [bool]");
    }
    return "";

}

QFMathParser::qfmpVariable::qfmpVariable()
{
    type=qfmpDouble;     /*!< \brief type of the variable */
    internal=false;           /*!< \brief this is an internal variable */
    str=NULL;        /*!< \brief this points to the variable data if \c type==qfmpString */
    num=NULL;             /*!< \brief this points to the variable data if \c type==qfmpDouble */
    boolean=NULL;
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
