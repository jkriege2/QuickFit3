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


#ifndef QENHANCEDCOMBOBOX_H
#define QENHANCEDCOMBOBOX_H

#include <QComboBox>
#include "libwid_imexport.h"
#include <QKeyEvent>

/** \brief this QComboox descendent implements features, missing in Qt
 *         with a possibly defined range
 *  \ingroup tools_qt
 *
 *  Added features are:
 *    - setReadonly() slot, which shows the combobox in the standardway, but prevents editing
 *    - editingFinished() signal, which is emitted when "return" or "enter" key is pressed
 *    .
 */
class QFWIDLIB_EXPORT QEnhancedComboBox : public QComboBox {
        Q_OBJECT
    public:
        explicit QEnhancedComboBox(QWidget *parent = 0);

        virtual void showPopup();
    signals:
        void editingFinished();
    public slots:
        void setReadOnly(bool readonly);


    protected:
        bool m_readonly;
        bool m_oldEditable;
        virtual void keyPressEvent ( QKeyEvent * e );
        virtual void keyReleaseEvent ( QKeyEvent * e );
        virtual void wheelEvent ( QWheelEvent * e );
};

#endif // QENHANCEDCOMBOBOX_H

