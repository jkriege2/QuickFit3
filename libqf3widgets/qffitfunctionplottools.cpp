/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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
    subfunction=-1;
}

JKQTPxQFFitFunctionLineGraph::~JKQTPxQFFitFunctionLineGraph()
{
    delete intParam;
    if (this->ownsFunction && this->fitFunction) delete this->fitFunction;
}

void JKQTPxQFFitFunctionLineGraph::set_fitFunction(QFFitFunction *fitFunction, bool ownsFunction)
{
    if (this->ownsFunction && this->fitFunction) delete this->fitFunction;
    this->fitFunction=fitFunction;
    this->ownsFunction=ownsFunction;
}

void JKQTPxQFFitFunctionLineGraph::collectParameters()
{
    JKQTPxFunctionLineGraph::collectParameters();
    intParam->function=fitFunction;
    intParam->param=iparams;
    intParam->scaleX=scaleX;
    intParam->offsetX=offsetX;
    if (subfunction>=0) {
        fitFunction->transformParametersForAdditionalPlot(subfunction, intParam->param);
    }
    set_plotFunction(JKQTPxQFFitFunctionLineGraph_func);
    set_params((void*)intParam);
}

/*void JKQTPxQFFitFunctionLineGraph::createPlotData(bool collectParams)
{
    intParam->function=fitFunction;
    intParam->param=paramsVector;
    intParam->scaleX=scaleX;
    intParam->offsetX=offsetX;
    set_plotFunction(JKQTPxQFFitFunctionLineGraph_func);
    set_params((void*)intParam);

    JKQTPxFunctionLineGraph::createPlotData(false);
}*/
