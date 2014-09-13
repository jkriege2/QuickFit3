/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-12 18:00:48 +0200 (Fr, 12 Sep 2014) $  (revision $Rev: 3461 $)

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

#include "qfecalculatorlibrary.h"
#include "statistics_tools.h"
#include <QTextDocument>
#include <QTextDocumentFragment>

template<int clearplots, bool barX, bool barY, int errorstyle>
qfmpResult fErrorbars(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  if (dlg) {
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          //qDebug()<<"current_figure="<<fid;
          if (clearplots!=0) fig->clearPlots();
          fig->startAddingPlots();
          unsigned int i=0;
          while (i<n) {
              QVector<double> X, Y, Xerr, Yerr;
              QString spec="";
              QString label="";
              X=params[i].asVector();
              i++;
              if (i<n) {
                  Y=params[i].asVector();
                  i++;
              }
              if (barX && i<n && params[i].type==qfmpDoubleVector) {
                  Xerr=params[i].asVector();
                  i++;
              } else if (barX && i<n && params[i].type==qfmpDouble) {
                  Xerr.fill(params[i].num, qMax(X.size(),Y.size()));
                  i++;
              }
              if (barY && i<n && params[i].type==qfmpDoubleVector) {
                  Yerr=params[i].asVector();
                  i++;
              } else if (barY && i<n && params[i].type==qfmpDouble) {
                  Yerr.fill(params[i].num, qMax(X.size(),Y.size()));
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  spec=params[i].str;
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  label=params[i].str;
                  i++;
              }

              if (X.size()>0 && Y.size()>0) {
                  fig->addErrorPlot(X, Y, Xerr, Yerr, spec, label, (JKQTPerrorPlotstyle)errorstyle);
              }
          }
          fig->endAddingPlots();
          if (clearplots) fig->fitRanges();
      }
  }
  return qfmpResult::voidResult();
}


template<int clearplots, bool logX, bool logY>
qfmpResult fPlot(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  if (dlg) {
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          //qDebug()<<"current_figure="<<fid;
          if (clearplots!=0) fig->clearPlots();
          fig->startAddingPlots();
          fig->setLogX(logX);
          fig->setLogY(logY);
          unsigned int i=0;
          while (i<n) {
              QVector<double> X, Y;
              QString spec="";
              QString label="";
              X=params[i].asVector();
              i++;
              if (i<n) {
                  Y=params[i].asVector();
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  spec=params[i].str;
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  label=params[i].str;
                  i++;
              }

              if (X.size()>0 && Y.size()>0) {
                  fig->addPlot(X, Y, spec, label);
              }
          }
          fig->endAddingPlots();
          if (clearplots) fig->fitRanges();
      }
  }
  return qfmpResult::voidResult();
}



template<int clearplots, bool logX, bool logY>
qfmpResult fBar(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  if (dlg) {
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          //qDebug()<<"current_figure="<<fid;
          if (clearplots!=0) fig->clearPlots();
          fig->startAddingPlots();
          if (logX) fig->setLogX(true);
          if (logY) fig->setLogY(true);
          unsigned int i=0;
          while (i<n) {
              QVector<double> X, Y;
              QString spec="";
              QString label="";
              X=params[i].asVector();
              i++;
              if (i<n && params[i].convertsToVector()) {
                  Y=params[i].asVector();
                  i++;
              } else {
                  Y=X;
                  X.resize(Y.size());
                  for (int i=0; i<X.size(); i++) X[i]=i+1;
              }
              if (i<n && params[i].type==qfmpString) {
                  spec=params[i].str;
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  label=params[i].str;
                  i++;
              }

              if (X.size()>0 && Y.size()>0) {
                  fig->addBarPlot(X, Y, spec, label);
              }
          }
          fig->endAddingPlots();
          if (clearplots) fig->fitRanges();
      }
  }
  return qfmpResult::voidResult();
}



template<int clearplots>
qfmpResult fHist(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  if (dlg) {
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          //qDebug()<<"current_figure="<<fid;
          QString f="addhist";
          if (clearplots) f="hist";
          if (clearplots!=0) fig->clearPlots();
          fig->startAddingPlots();
          QVector<double> D;
          QVector<double> range;
          uint32_t bins=11;
          QString spec="";
          QString label="";
          int i=1;
          if (n>=1) {
              D=params[0].asVector();
          }
          if (n>1 && params[i].type==qfmpDoubleVector && params[i].numVec.size()==2) {
              range=params[i].numVec;
              i++;
          }
          if (n>1 && params[i].isUInt()) {
              bins=params[i].toUInt();
              i++;
          }
          if (i<n && params[i].type==qfmpString) {
              spec=params[i].str;
              i++;
          }
          if (i<n && params[i].type==qfmpString) {
              label=params[i].str;
              i++;
          }
          QVector<double> X, Y;
          X.resize(bins);
          Y.resize(bins);
          if (range.size()!=2) statisticsHistogram(D.data(), D.size(), X.data(), Y.data(), bins);
          else statisticsHistogramRanged(D.data(), D.size(), range[0], range[1], X.data(), Y.data(), bins);

          if (X.size()>0 && Y.size()>0) {
              fig->addBarPlot(X, Y, spec, label);
          }

          fig->endAddingPlots();
          if (clearplots) fig->fitRanges();
      }
  }
  return qfmpResult::voidResult();
}


template<int clearplots, bool logX, bool logY>
qfmpResult fScatter(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  if (dlg) {
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          //qDebug()<<"current_figure="<<fid;
          if (clearplots!=0) fig->clearPlots();
          fig->startAddingPlots();
          if (logX) fig->setLogX(true);
          if (logY) fig->setLogY(true);
          unsigned int i=0;
          while (i<n) {
              QVector<double> X, Y, S;
              QString spec="";
              QString label="";
              X=params[i].asVector();
              i++;
              if (i<n) {
                  Y=params[i].asVector();
                  i++;
              }
              if (i<n) {
                  S=params[i].asVector();
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  spec=params[i].str;
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  label=params[i].str;
                  i++;
              }

              if (X.size()>0 && Y.size()>0) {
                  fig->addScatterPlot(X, Y, S, spec, label);
              }
          }
          fig->endAddingPlots();
          if (clearplots) fig->fitRanges();
      }
  }
  return qfmpResult::voidResult();
}


template<int clearplots, bool logX, bool logY>
qfmpResult fColorScatter(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  if (dlg) {
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          //qDebug()<<"current_figure="<<fid;
          if (clearplots!=0) fig->clearPlots();
          fig->startAddingPlots();
          if (logX) fig->setLogX(true);
          if (logY) fig->setLogY(true);
          unsigned int i=0;
          while (i<n) {
              QVector<double> X, Y, S;
              QString spec="";
              QString label="";
              QString pal="Matlab";
              X=params[i].asVector();
              i++;
              if (i<n) {
                  Y=params[i].asVector();
                  i++;
              }
              if (i<n) {
                  S=params[i].asVector();
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  spec=params[i].str;
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  pal=params[i].str;
                  i++;
              }
              if (i<n && params[i].type==qfmpString) {
                  label=params[i].str;
                  i++;
              }

              if (X.size()>0 && Y.size()>0) {
                  fig->addColorScatterPlot(X, Y, S, spec, label, pal);
              }
          }
          fig->endAddingPlots();
          if (clearplots) fig->fitRanges();
      }
  }
  return qfmpResult::voidResult();
}

qfmpResult fFigure(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  if (dlg) {
      if (n==0) return qfmpResult(fid);
      else if (n==1 && params[0].isInteger()) {
          fid=params[0].toInteger();
      } else {
          p->qfmpError(QObject::tr("figure()/figure(n) require none or one integer argument"));
          return qfmpResult::invalidResult();
      }
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
      }
  }
  return qfmpResult(fid);
}

template<int axis>
qfmpResult fLabel(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  QString f="label";
  if (axis==0) f="xlabel";
  if (axis==1) f="ylabel";

  if (dlg) {
      QString l;
      if (n==1 && params[0].type==qfmpString) {
          l=params[0].str;
      } else {
          p->qfmpError(QObject::tr("%1(n) require one string argument").arg(f));
          return qfmpResult::invalidResult();
      }
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          if (axis==-1) {
              fig->getPlotter()->get_plotter()->set_plotLabel(l);
          } else if (axis==0) {
            fig->getPlotter()->getXAxis()->set_axisLabel(l);
          } else if (axis==1) {
            fig->getPlotter()->getYAxis()->set_axisLabel(l);
          }

      }
  }
  return qfmpResult::voidResult();
}

template<int axis>
qfmpResult fLim(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();
  QString f="xlim";
  if (axis==0) f="xlim";
  if (axis==1) f="ylim";

  if (dlg) {
      double mi=0;
      double ma=0;
      if (n==1 && params[0].type==qfmpDoubleVector && params[0].numVec.size()>1) {
          mi=params[0].numVec[0];
          ma=params[0].numVec[1];
      } else if (n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
          mi=params[0].num;
          ma=params[1].num;
      } else {
          p->qfmpError(QObject::tr("%1([min,max])/%1(min,max) require one number-vector, or two number-arguments").arg(f));
          return qfmpResult::invalidResult();
      }
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          if (axis==0) {
            fig->getPlotter()->getXAxis()->setRange(mi,ma);
          } else if (axis==1) {
            fig->getPlotter()->getYAxis()->setRange(mi,ma);
          }

      }
  }
  return qfmpResult::voidResult();
}


qfmpResult fLogScale(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();

  if (dlg) {
      bool logX=false;
      bool logY=false;
      if (n==2 && params[0].type==qfmpBool && params[1].type==qfmpBool) {
          logX=params[0].boolean;
          logY=params[1].boolean;
      } else {
          p->qfmpError(QObject::tr("logscale(xlog,ylog) require two boolean arguments"));
          return qfmpResult::invalidResult();
      }
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          fig->getPlotter()->getXAxis()->set_logAxis(logX);
          fig->getPlotter()->getXAxis()->set_logAxis(logY);
      }
  }
  return qfmpResult::voidResult();
}


qfmpResult fGrid(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();

  if (dlg) {
      bool grid=false;
      QString style="";
      if (n==1 && params[0].type==qfmpBool) {
          grid=params[0].boolean;
      } else if (n==2 && params[0].type==qfmpBool && params[1].type==qfmpString) {
          grid=params[0].boolean;
          style=params[1].str;
      } else {
          p->qfmpError(QObject::tr("grid(enabled)/grid(enabled,\"style\") require one boolean and one optional string argument"));
          return qfmpResult::invalidResult();
      }
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          fig->startAddingPlots();
          fig->setGrid(grid, style);
          fig->endAddingPlots();
      }
  }
  return qfmpResult::voidResult();
}

template<int axes>
qfmpResult fTicks(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();

  QString f="ticks(xspacing,yspacing)";
  if (axes==0) f="xticks(spacing)";
  if (axes==1) f="yticks(spacing)";
  if (dlg) {
      double spacingX=0;
      double spacingY=0;
      if (axes<0 && n==2 && params[0].type==qfmpDouble && params[1].type==qfmpDouble) {
          spacingX=params[0].num;
          spacingY=params[1].num;
      } else if (axes<0 && n==1 && params[0].type==qfmpDouble) {
          spacingY=spacingX=params[0].num;
      } else if (axes==0 && n==1 && params[0].type==qfmpDouble) {
          spacingX=params[0].num;
      } else if (axes==1 && n==1 && params[0].type==qfmpDouble) {
          spacingY=params[0].num;
      } else {
          p->qfmpError(QObject::tr("%1: wrong number/type of arguments").arg(f));
          return qfmpResult::invalidResult();
      }
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          fig->startAddingPlots();
          if (axes<0 || axes==0) {
              fig->getPlotter()->getXAxis()->set_userTickSpacing(spacingX);
              fig->getPlotter()->getXAxis()->set_autoAxisSpacing(false);
          }
          if (axes<0 || axes==1) {
              fig->getPlotter()->getYAxis()->set_userTickSpacing(spacingY);
              fig->getPlotter()->getYAxis()->set_autoAxisSpacing(false);
          }
          fig->endAddingPlots();
      }
  }
  return qfmpResult::voidResult();
}

template<int axes>
qfmpResult fAutoTicks(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  int fid=p->getGeneraldata("QFECalculatorDialog/current_figure", int(-1)).toInt();

  QString f="autoticks(x,y)";
  if (axes==0) f="xautoticks(enabled)";
  if (axes==1) f="yautoticks(enabled)";
  if (dlg) {
      bool XAuto=true;
      double YAuto=true;
      if (axes<0 && n==2 && params[0].type==qfmpBool && params[1].type==qfmpBool) {
          XAuto=params[0].boolean;
          YAuto=params[1].boolean;
      } else if (axes<0 && n==1 && params[0].type==qfmpBool) {
          YAuto=XAuto=params[0].boolean;
      } else if (axes==0 && n==1 && params[0].type==qfmpBool) {
          XAuto=params[0].boolean;
      } else if (axes==1 && n==1 && params[0].type==qfmpBool) {
          YAuto=params[0].boolean;
      } else {
          p->qfmpError(QObject::tr("%1: wrong number/type of arguments").arg(f));
          return qfmpResult::invalidResult();
      }
      QFECalculatorPlotDialog* fig=dlg->getPlot(fid, &fid);
      if (fig) {
          p->setGeneralData("QFECalculatorDialog/current_figure", fid);
          fig->startAddingPlots();
          if (axes<0 || axes==0) {
              fig->getPlotter()->getXAxis()->set_autoAxisSpacing(XAuto);
          }
          if (axes<0 || axes==1) {
              fig->getPlotter()->getYAxis()->set_autoAxisSpacing(YAuto);
          }
          fig->endAddingPlots();
      }
  }
  return qfmpResult::voidResult();
}

qfmpResult fDisp(const qfmpResult* params, unsigned int  n, QFMathParser* p){
  QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  if (dlg) {
      QPlainTextEdit* h=dlg->getHistory();

      QString t="";
      for (int i=0; i<n; i++) {
          t=t+params[i].toString(dlg->getPrecision());
      }

      QTextCursor cur1(h->document());
      cur1.movePosition(QTextCursor::End);
      cur1.insertFragment(QTextDocumentFragment::fromPlainText(t+"\n"));

  }
  return qfmpResult::voidResult();
}

qfmpResult fDispNLB(const qfmpResult* params, unsigned int  n, QFMathParser* p){
    QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  if (dlg) {
      QPlainTextEdit* h=dlg->getHistory();

      QString t="";
      for (int i=0; i<n; i++) {
          t=t+params[i].toString(dlg->getPrecision());
      }

      QTextCursor cur1(h->document());
      cur1.movePosition(QTextCursor::End);
      cur1.insertFragment(QTextDocumentFragment::fromPlainText(t));

  }
  return qfmpResult::voidResult();
}

qfmpResult fDispType(const qfmpResult* params, unsigned int  n, QFMathParser* p){
    QFECalculatorDialog* dlg=(QFECalculatorDialog*)(p->getGeneraldata("QFECalculatorDialog/pointer", 0).toLongLong());
  if (dlg) {
      QPlainTextEdit* h=dlg->getHistory();

      QString t="";
      for (int i=0; i<n; i++) {
          t=t+params[i].toTypeString(dlg->getPrecision());
      }

      QTextCursor cur1(h->document());
      cur1.movePosition(QTextCursor::End);
      cur1.insertFragment(QTextDocumentFragment::fromPlainText(t+"\n"));

  }
  return qfmpResult::voidResult();
}




void registerQFECalculatorFunctions(QFMathParser *parser)
{
    parser->addFunction("grid", fGrid);
    parser->addFunction("ticks", fTicks<-1>);
    parser->addFunction("xticks", fTicks<0>);
    parser->addFunction("yticks", fTicks<1>);
    parser->addFunction("autoticks", fAutoTicks<-1>);
    parser->addFunction("xautoticks", fAutoTicks<0>);
    parser->addFunction("yautoticks", fAutoTicks<1>);
    parser->addFunction("logscale", fLogScale);
    parser->addFunction("label", fLabel<-1>);
    parser->addFunction("xlabel", fLabel<0>);
    parser->addFunction("ylabel", fLabel<1>);
    parser->addFunction("xlim", fLim<0>);
    parser->addFunction("ylim", fLim<1>);
    parser->addFunction("plot", fPlot<1,false,false>);
    parser->addFunction("addplot", fPlot<0,false,false>);

    parser->addFunction("bar", fBar<1,false,false>);
    parser->addFunction("addbar", fBar<0,false,false>);
    parser->addFunction("hist", fHist<1>);
    parser->addFunction("addhist", fHist<0>);

    parser->addFunction("scatter", fScatter<1,false,false>);
    parser->addFunction("addscatter", fScatter<0,false,false>);
    parser->addFunction("colorscatter", fColorScatter<1,false,false>);
    parser->addFunction("addcolorscatter", fColorScatter<0,false,false>);

    parser->addFunction("semilogx", fPlot<1,true,false>);
    parser->addFunction("addsemilogx", fPlot<0,true,false>);
    parser->addFunction("semilogy", fPlot<1,false,true>);
    parser->addFunction("addsemilogy", fPlot<0,false,true>);
    parser->addFunction("loglog", fPlot<1,true,true>);
    parser->addFunction("addloglog", fPlot<0,true,true>);
    parser->addFunction("errorbar", fErrorbars<1,true,true,JKQTPerrorBars>);
    parser->addFunction("adderrorbar", fErrorbars<0,true,true,JKQTPerrorBars>);
    parser->addFunction("yerrorbar", fErrorbars<1,false,true,JKQTPerrorBars>);
    parser->addFunction("addyerrorbar", fErrorbars<0,false,true,JKQTPerrorBars>);
    parser->addFunction("xerrorbar", fErrorbars<1,true,false,JKQTPerrorBars>);
    parser->addFunction("addxerrorbar", fErrorbars<0,true,false,JKQTPerrorBars>);
    parser->addFunction("figure", fFigure);
    parser->addFunction("write", fDispNLB);
    parser->addFunction("writeln", fDisp);
    parser->addFunction("disp", fDisp);
    parser->addFunction("print", fDisp);
    parser->addFunction("dispType", fDispType);
}
