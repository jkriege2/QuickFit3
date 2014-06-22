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


#ifndef QFPLOTTERPRIVATE_H
#define QFPLOTTERPRIVATE_H

#include <QObject>
#include <QAction>
class QFPlotter; // forward

class QFPlotterPrivate : public QObject
{
        Q_OBJECT
    public:
        friend class QFPlotter;
        explicit QFPlotterPrivate( QFPlotter* plotter, QObject *parent = 0);
        
    signals:
        
    public slots:
        void copyToTable();
        void showHelp();

    protected:
        QAction* actCopyToTable;
        QAction* actHelp;
        QFPlotter* plotter;
        
};

#endif // QFPLOTTERPRIVATE_H
