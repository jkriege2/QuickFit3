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


#ifndef QFFITFUNCTIONPLOTTOOLS_H
#define QFFITFUNCTIONPLOTTOOLS_H

#include "libwid_imexport.h"
#include <QVector>
#include <QString>
#include "jkqtpelements.h"
#include "qffitfunction.h"

struct JKQTPxQFFitFunctionLineGraphFunctionParams; // forward

class QFWIDLIB_EXPORT JKQTPxQFFitFunctionLineGraph: public JKQTPxFunctionLineGraph {
        Q_OBJECT
    public:

        /** \brief class constructor */
        JKQTPxQFFitFunctionLineGraph(JKQtBasePlotter* parent=NULL);
        ~JKQTPxQFFitFunctionLineGraph();



        GET_MACRO(QFFitFunction*, fitFunction)
        GET_SET_MACRO(bool, ownsFunction)
        GET_SET_MACRO(double, scaleX)
        GET_SET_MACRO(double, offsetX)

        void set_fitFunction(QFFitFunction* fitFunction, bool ownsFunction=true);
    protected:
        QFFitFunction* fitFunction;
        bool ownsFunction;

        double scaleX;
        double offsetX;

        JKQTPxQFFitFunctionLineGraphFunctionParams* intParam;

        virtual void collectParameters();
        //virtual void createPlotData( bool collectParams=true);


};

#endif // QFFITFUNCTIONPLOTTOOLS_H
