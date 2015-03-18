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


#include "qfmathparserxfunctionlinegraph.h"
#include <QDebug>
#include <QElapsedTimer>
#include "jkqtplotter.h"


double QFMathParserXFunctionLineGraph_evaluate(double x, void* data) {
    QFMathParserXFunctionLineGraphFunctionData* d=(QFMathParserXFunctionLineGraphFunctionData*)data;
    if (d && d->useByteCode && d->parser && d->byteCode.size()>0) {
        *(d->x)=x;
        return d->parser->evaluateBytecode(d->byteCode);
    } else if (d && d->parser && d->node) {
        //d->parser->resetErrors();
        *(d->x)=x;
        qfmpResult r;
        d->node->evaluate(r);

        if (r.isUsableResult()) {
            if (r.type==qfmpBool) {
                return r.boolean?1.0:0.0;
            } else if (r.type==qfmpDouble) {
                return r.num;
            }
        }
    }
    return NAN;

}

double QFMathParserXFunctionLineGraph_evaluateBytecode(double x, void* data) {
    QFMathParserXFunctionLineGraphFunctionData* d=(QFMathParserXFunctionLineGraphFunctionData*)data;
    if (d && d->parser && d->byteCode.size()>0) {
        *(d->x)=x;
        return d->parser->evaluateBytecode(d->byteCode);
    }
    return NAN;

}

QFMathParserXFunctionLineGraph::QFMathParserXFunctionLineGraph(JKQtBasePlotter *parent):
    JKQTPxFunctionLineGraph(parent)
{
    fdata.parser=new QFMathParser();
    fdata.node=NULL;
    fdata.varcount=0;
    function="";
    parameterColumn=-1;
    set_params(&fdata);
    set_plotFunction(QFMathParserXFunctionLineGraph_evaluate);

    efdata.parser=new QFMathParser();
    efdata.node=NULL;
    efdata.varcount=0;
    errorFunction="";
    errorParameterColumn=-1;
    set_errorParams(&efdata);
    set_errorPlotFunction(QFMathParserXFunctionLineGraph_evaluate);
}

QFMathParserXFunctionLineGraph::~QFMathParserXFunctionLineGraph()
{
    if (fdata.node) delete fdata.node;
    delete fdata.parser;
    if (efdata.node) delete efdata.node;
    delete efdata.parser;
}

void QFMathParserXFunctionLineGraph::createPlotData(bool collectParams)
{
    Q_UNUSED(collectParams);
    collectParameters();
    QElapsedTimer timer;
    timer.start();
    for (int i=0; i<fdata.varcount; i++) {
        fdata.parser->deleteVariable(QString("p%1").arg(i+1));
    }
    fdata.varcount=0;
    fdata.x=&dummyX;
    if (parent && parameterColumn>=0) {

        JKQTPdatastore* datastore=parent->getDatastore();
        int imin=0;
        int imax=datastore->getColumn(parameterColumn).getRows();

        for (int i=imin; i<imax; i++) {
            double xv=datastore->get(parameterColumn,i);
            fdata.parser->addVariableDouble(QString("p%1").arg(fdata.varcount+1), xv);
            fdata.varcount=fdata.varcount+1;

        }
    }
    for (int i=0; i<iparams.size(); i++) {
        fdata.parser->addVariableDouble(QString("p%1").arg(fdata.varcount+1), iparams[i]);
        fdata.varcount=fdata.varcount+1;
    }
    fdata.parser->addVariable(QString("x"),  QFMathParser::qfmpVariable(&dummyX));
    if (fdata.node) delete fdata.node;
    qint64 t=timer.elapsed();
    //qDebug()<<"createPlotData():   adding variables: "<<t<<"ms";
    fdata.node=fdata.parser->parse(function);
    QFMathParser::ByteCodeEnvironment environment(fdata.parser);
    environment.init(fdata.parser);
    fdata.byteCode.clear();
    fdata.useByteCode=fdata.node->createByteCode(fdata.byteCode, &environment);
    //qDebug()<<"createPlotData():   parsing: "<<timer.elapsed()-t<<"ms";


    qint64 t0=timer.elapsed();
    for (int i=0; i<efdata.varcount; i++) {
        efdata.parser->deleteVariable(QString("p%1").arg(i+1));
    }
    efdata.varcount=0;
    efdata.x=&dummyEX;
    if (parent && errorParameterColumn>=0) {

        JKQTPdatastore* datastore=parent->getDatastore();
        int imin=0;
        int imax=datastore->getColumn(errorParameterColumn).getRows();

        for (int i=imin; i<imax; i++) {
            double xv=datastore->get(errorParameterColumn,i);
            efdata.parser->addVariableDouble(QString("p%1").arg(efdata.varcount+1), xv);
            efdata.varcount=efdata.varcount+1;

        }
    }
    for (int i=0; i<ierrorparams.size(); i++) {
        efdata.parser->addVariableDouble(QString("p%1").arg(efdata.varcount+1), ierrorparams[i]);
        efdata.varcount=efdata.varcount+1;
    }
    efdata.parser->addVariable(QString("x"), QFMathParser::qfmpVariable(&dummyEX));
    if (efdata.node) delete efdata.node;
    t=timer.elapsed();
    //qDebug()<<"createPlotData():   adding variables: "<<t-t0<<"ms";
    efdata.node=efdata.parser->parse(errorFunction);
    //qDebug()<<"createPlotData():   parsing: "<<timer.elapsed()-t<<"ms";
    QFMathParser::ByteCodeEnvironment eenvironment(efdata.parser);
    environment.init(efdata.parser);
    efdata.byteCode.clear();
    efdata.useByteCode=efdata.node->createByteCode(efdata.byteCode, &eenvironment);

    set_params(&fdata);
    set_plotFunction(QFMathParserXFunctionLineGraph_evaluate);
    set_errorParams(&efdata);
    set_errorPlotFunction(QFMathParserXFunctionLineGraph_evaluate);

    t=timer.elapsed();
    JKQTPxFunctionLineGraph::createPlotData(false);
    //qDebug()<<"createPlotData():   JKQTPxFunctionLineGraph::createPlotData():   "<<timer.elapsed()-t<<"ms";

    /*int count=0;
    doublePair* d=data;
    while (d!=NULL) {
        count++;
        d=d->next;
    }
    qDebug()<<"refined to "<<count<<" daatapoints";*/
}


