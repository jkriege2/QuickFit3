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


#ifndef QKEYSEQUENCEEDIT_H
#define QKEYSEQUENCEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include "qenhancedlineedit.h"
#include "jkstyledbutton.h"
#include "libwid_imexport.h"
#include <QVariant>

/*! \brief enhanced QLineEdit which allows the user to edit a QKeySequence
    \ingroup tools_qt

    The new features are:
       - translates a pressed key combination into a text
       - switches off the context menu
       - adds a button to clear the widget
    .
 */
class QFWIDLIB_EXPORT QKeySequenceEdit : public QEnhancedLineEdit
{
        Q_OBJECT
    public:
        explicit QKeySequenceEdit(QWidget *parent = 0);
        
        QKeySequence getKeySequence() const;
        void setKeySequence(QKeySequence sequence);
    signals:
        
    protected slots:
        void clearThis();

    protected:
        virtual void keyPressEvent(QKeyEvent* event);
        virtual void contextMenuEvent ( QContextMenuEvent * event );
        QKeySequence currentKS;
        JKStyledButton* btnClear;

};

#endif // QKEYSEQUENCEEDIT_H
