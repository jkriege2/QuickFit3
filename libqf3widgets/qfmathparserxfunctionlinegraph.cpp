#include "qfmathparserxfunctionlinegraph.h"
#include <QDebug>
#include <QElapsedTimer>
#include "jkqtplotter.h"


double QFMathParserXFunctionLineGraph_evaluate(double x, void* data) {
    QFMathParserXFunctionLineGraphFunctionData* d=(QFMathParserXFunctionLineGraphFunctionData*)data;
    if (d && d->parser && d->node) {
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

void QFMathParserXFunctionLineGraph::createPlotData()
{
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
    for (int i=0; i<parameters.size(); i++) {
        fdata.parser->addVariableDouble(QString("p%1").arg(fdata.varcount+1), parameters[i]);
        fdata.varcount=fdata.varcount+1;
    }
    fdata.parser->addVariable(QString("x"),  QFMathParser::qfmpVariable(&dummyX));
    if (fdata.node) delete fdata.node;
    qint64 t=timer.elapsed();
    //qDebug()<<"createPlotData():   adding variables: "<<t<<"ms";
    fdata.node=fdata.parser->parse(function);
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
    for (int i=0; i<errorParameters.size(); i++) {
        efdata.parser->addVariableDouble(QString("p%1").arg(efdata.varcount+1), errorParameters[i]);
        efdata.varcount=efdata.varcount+1;
    }
    efdata.parser->addVariable(QString("x"), QFMathParser::qfmpVariable(&dummyEX));
    if (efdata.node) delete efdata.node;
    t=timer.elapsed();
    //qDebug()<<"createPlotData():   adding variables: "<<t-t0<<"ms";
    efdata.node=efdata.parser->parse(errorFunction);
    //qDebug()<<"createPlotData():   parsing: "<<timer.elapsed()-t<<"ms";

    set_params(&fdata);
    set_plotFunction(QFMathParserXFunctionLineGraph_evaluate);
    set_errorParams(&efdata);
    set_errorPlotFunction(QFMathParserXFunctionLineGraph_evaluate);

    t=timer.elapsed();
    JKQTPxFunctionLineGraph::createPlotData();
    //qDebug()<<"createPlotData():   JKQTPxFunctionLineGraph::createPlotData():   "<<timer.elapsed()-t<<"ms";

    /*int count=0;
    doublePair* d=data;
    while (d!=NULL) {
        count++;
        d=d->next;
    }
    qDebug()<<"refined to "<<count<<" daatapoints";*/
}

