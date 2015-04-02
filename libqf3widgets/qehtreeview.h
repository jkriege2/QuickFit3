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


#ifndef QEHTREEVIEW_H
#define QEHTREEVIEW_H

#include <QTreeView>
#include "libwid_imexport.h"

/*! \brief this class extends the QTreeView by including signals that will be
           emitted by several internal events (key events ...).
    \ingroup qf3lib_widgets

  This is usefull, if you want to trigger actions by a keypress ...
*/

class QFWIDLIB_EXPORT QEHTreeView : public QTreeView {
        Q_OBJECT
    public:
        /** \brief Default constructor */
        QEHTreeView(QWidget * parent = NULL);
        /** \brief Default destructor */
        virtual ~QEHTreeView();
    signals:
        void keyPressed(QKeyEvent * event);
        void keyReleased(QKeyEvent * event);
    protected:
        virtual void keyPressEvent ( QKeyEvent * event );
        virtual void keyReleaseEvent ( QKeyEvent * event );
    private:
};

#endif // QEHTREEVIEW_H
