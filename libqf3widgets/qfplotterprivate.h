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


#ifndef QFPLOTTERPRIVATE_H
#define QFPLOTTERPRIVATE_H

#include <QObject>
#include <QAction>
#include "qfrdrcurvesinterface.h"
class QFPlotter; // forward
class QFRDRColumnGraphsInterface;
class JKQTPgraph;

class QFPlotterPrivate : public QObject
{
        Q_OBJECT
    public:
        friend class QFPlotter;
        explicit QFPlotterPrivate( QFPlotter* plotter, QObject *parent = 0);
        
    signals:
        
    public slots:
        void copyToTable();
        void copyToCurve();
        void showHelp();

    protected:
        QAction* actCopyToTable;
        QAction* actCopyToCurve;
        QAction* actHelp;
        QFPlotter* plotter;

        struct CurveData {
            QString name;
            int xcol;
            int ycol;
            int xerr;
            int yerr;
            QFRDRCurvesInterface::CurveType type;
        };
    private:
        void setErrorProps(int plotid, int graphid, QFRDRColumnGraphsInterface *cols, JKQTPgraph *g, QColor color, QMap<int, int> &columns);

};

#endif // QFPLOTTERPRIVATE_H
