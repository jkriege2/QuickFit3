/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef JKQTPPARSEDFUNCTIONELEMENTS_H
#define JKQTPPARSEDFUNCTIONELEMENTS_H

#include <QString>
#include <QPainter>
#include <QPair>
#include "jkqtptools.h"
#include "../tools.h"
#include "../jkmathparser.h"
#include "../lib_imexport.h"
#include "jkqtpelements.h"

// forward declarations
class JKQtBasePlotter;


struct JKQTPxParsedFunctionLineGraphFunctionData {
    jkMathParser* parser;
    jkMathParser::jkmpNode* node;
    int varcount;
};


/*! \brief This implements line plots where the data is taken from a user supplied function \f$ y=f(x) \f$ The function is defined as a string and parsed by JKMathParser
    \ingroup jkqtplotter_plots

    Additional function parameters may be given in the vector parameters. They are accessible in the function as \c p1 , \c p2 , \c p3 , ...
    Parameters may also be given from a data column. Then first the params from the column and the the parameters from the vector are numbered.
 */
class LIB_EXPORT JKQTPxParsedFunctionLineGraph: public JKQTPxFunctionLineGraph {
        Q_OBJECT
    public:


        /** \brief class constructor */
        JKQTPxParsedFunctionLineGraph(JKQtBasePlotter* parent=NULL);

        /** \brief class destructor */
        ~JKQTPxParsedFunctionLineGraph();

        GET_SET_MACRO(QList<double>, parameters)
        GET_SET_MACRO(QString, function)
        GET_SET_MACRO(int, parameterColumn)

        GET_SET_MACRO(QList<double>, errorParameters)
        GET_SET_MACRO(QString, errorFunction)
        GET_SET_MACRO(int, errorParameterColumn)
    protected:


        /** \brief which plot style to use from the parent plotter (via JKQtPlotterBase::getPlotStyle() and JKQtPlotterBase::getNextStyle() ) */
        QString function;
        QList<double> parameters;
        JKQTPxParsedFunctionLineGraphFunctionData fdata;
        int parameterColumn;

        QString errorFunction;
        QList<double> errorParameters;
        JKQTPxParsedFunctionLineGraphFunctionData efdata;
        int errorParameterColumn;

        GET_SET_MACRO_I(jkqtpPlotFunctionType, plotFunction, clearData())
        GET_SET_MACRO_I(void*, params, clearData())
        GET_SET_MACRO(jkqtpPlotFunctionType, errorPlotFunction)
        GET_SET_MACRO(void*, errorParams)

        /** \brief fill the data array with data from the function plotFunction */
        virtual void createPlotData(bool collectParams=true);
};
#endif // JKQTPPARSEDFUNCTIONELEMENTS_H
