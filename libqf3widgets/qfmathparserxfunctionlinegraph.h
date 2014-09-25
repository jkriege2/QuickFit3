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


#ifndef QFMathParserXFunctionLineGraph_H
#define QFMathParserXFunctionLineGraph_H
#include <QString>
#include <QPainter>
#include <QPair>
#include "jkqtptools.h"
#include "tools.h"
#include "libwid_imexport.h"
#include "jkqtpelements.h"
#include "qfmathparser.h"

// forward declarations
class JKQtBasePlotter;


struct QFMathParserXFunctionLineGraphFunctionData {
    QFMathParser* parser;
    QFMathParser::qfmpNode* node;
    QFMathParser::ByteCodeProgram byteCode;
    bool useByteCode;
    int varcount;
    double* x;
};

/*! \brief This implements line plots where the data is taken from a user supplied function \f$ y=f(x) \f$ The function is defined as a string and parsed by QFMathParser
    \ingroup jkqtplotter_plots

    Additional function parameters may be given in the vector parameters. They are accessible in the function as \c p1 , \c p2 , \c p3 , ...
    Parameters may also be given from a data column. Then first the params from the column and the the parameters from the vector are numbered.
 */
class QFWIDLIB_EXPORT QFMathParserXFunctionLineGraph : public JKQTPxFunctionLineGraph
{
        Q_OBJECT
    public:
        explicit QFMathParserXFunctionLineGraph(JKQtBasePlotter *parent = 0);
        

        /** \brief class destructor */
        virtual ~QFMathParserXFunctionLineGraph();

        GET_SET_MACRO(QString, function)

        GET_SET_MACRO(QString, errorFunction)
    protected:


        /** \brief which plot style to use from the parent plotter (via JKQtPlotterBase::getPlotStyle() and JKQtPlotterBase::getNextStyle() ) */
        QString function;
        QFMathParserXFunctionLineGraphFunctionData fdata;

        QString errorFunction;
        QFMathParserXFunctionLineGraphFunctionData efdata;

        GET_SET_MACRO_I(jkqtpPlotFunctionType, plotFunction, clearData())
        GET_SET_MACRO_I(void*, params, clearData())
        GET_SET_MACRO(jkqtpPlotFunctionType, errorPlotFunction)
        GET_SET_MACRO(void*, errorParams)

        /** \brief fill the data array with data from the function plotFunction */
        virtual void createPlotData(bool collectParams=true);

        double dummyX, dummyEX;

};

#endif // QFMathParserXFunctionLineGraph_H
