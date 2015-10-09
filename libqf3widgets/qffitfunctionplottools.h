/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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


#ifndef QFFITFUNCTIONPLOTTOOLS_H
#define QFFITFUNCTIONPLOTTOOLS_H

#include "libwid_imexport.h"
#include <QVector>
#include <QString>
#include "jkqtpelements.h"
#include "qffitfunction.h"
#include "cpptools.h"

struct JKQTPxQFFitFunctionLineGraphFunctionParams; // forward

class QFWIDLIB_EXPORT JKQTPxQFFitFunctionLineGraph: public JKQTPxFunctionLineGraph {
        Q_OBJECT
    public:

        /** \brief class constructor */
        JKQTPxQFFitFunctionLineGraph(JKQtBasePlotter* parent=NULL);
        JKQTPxQFFitFunctionLineGraph(JKQtPlotter* parent=NULL);
        ~JKQTPxQFFitFunctionLineGraph();



        GET_MACRO(QFFitFunction*, fitFunction)
        GET_SET_MACRO(bool, ownsFunction)
        GET_SET_MACRO(double, scaleX)
        GET_SET_MACRO(double, offsetX)
        GET_SET_MACRO(int, subfunction)

        void set_fitFunction(QFFitFunction* fitFunction, bool ownsFunction=true);
        void set_fitFunction(const QString& fitFunction);
    protected:
        QFFitFunction* fitFunction;
        bool ownsFunction;

        double scaleX;
        double offsetX;
        int subfunction;

        JKQTPxQFFitFunctionLineGraphFunctionParams* intParam;

        virtual void collectParameters();
        //virtual void createPlotData( bool collectParams=true);


};

#endif // QFFITFUNCTIONPLOTTOOLS_H
