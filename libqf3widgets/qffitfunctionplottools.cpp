#include "qffitfunctionplottools.h"

struct JKQTPxQFFitFunctionLineGraphFunctionParams {
    QFFitFunction* function;
    QVector<double> param;
    double scaleX;
    double offsetX;
};

double JKQTPxQFFitFunctionLineGraph_func(double x, void* param) {
    JKQTPxQFFitFunctionLineGraphFunctionParams* p=(JKQTPxQFFitFunctionLineGraphFunctionParams*)param;
    if (!p || !p->function) return 0;
    return p->function->evaluate(x/p->scaleX-p->offsetX, p->param.data());
}

JKQTPxQFFitFunctionLineGraph::JKQTPxQFFitFunctionLineGraph(JKQtBasePlotter* parent):
    JKQTPxFunctionLineGraph(parent)
{
    intParam=new JKQTPxQFFitFunctionLineGraphFunctionParams;
    fitFunction=NULL;
    ownsFunction=false;
    scaleX=1;
    offsetX=0;
}

JKQTPxQFFitFunctionLineGraph::~JKQTPxQFFitFunctionLineGraph()
{
    delete intParam;
}

void JKQTPxQFFitFunctionLineGraph::set_fitFunction(QFFitFunction *fitFunction, bool ownsFunction)
{
    if (this->ownsFunction && this->fitFunction) delete this->fitFunction;
    this->fitFunction=fitFunction;
    this->ownsFunction=ownsFunction;
}

void JKQTPxQFFitFunctionLineGraph::createPlotData()
{
    intParam->function=fitFunction;
    intParam->param=paramsVector;
    intParam->scaleX=scaleX;
    intParam->offsetX=offsetX;
    set_plotFunction(JKQTPxQFFitFunctionLineGraph_func);
    set_params((void*)intParam);

    JKQTPxFunctionLineGraph::createPlotData();
}
