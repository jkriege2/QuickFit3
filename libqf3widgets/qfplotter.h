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


#ifndef QFPLOTTER_H
#define QFPLOTTER_H

#include "jkqtplotter.h"
#include "libwid_imexport.h"
#include <QToolBar>

class QFPlotterPrivate; // forward

class QFWIDLIB_EXPORT QFPlotter : public JKQtPlotter
{
        Q_OBJECT
    protected:
        QFPlotterPrivate* p;
    public:
        friend class QFPlotterPrivate;
        explicit QFPlotter(bool datastore_internal, QWidget* parent=NULL, JKQTPdatastore* datast=NULL);
        explicit QFPlotter(QWidget *parent = 0);

        QToolBar* createToolbar(QWidget* parent=NULL, const QString& name=QString(""));
        virtual void populateToolbar(QToolBar* toolbar) const;

        QAction* get_actCopyToTable() const;
        QAction* get_actHelp() const;
    signals:
        
    public slots:
    protected:
        void initQFPlotter();
        virtual void modifyContextMenu(QMenu* menu);

        
};

#endif // QFPLOTTER_H
