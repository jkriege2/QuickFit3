/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFENHANCEDCOMBOBOX_H
#define QFENHANCEDCOMBOBOX_H

#include <QComboBox>
#include "lib_imexport.h"
#include <QVariant>


/*! \brief enhanced QComboBox
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFEnhancedComboBox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFEnhancedComboBox(QWidget *parent = 0);
        QVariant currentData(int role=Qt::UserRole) const;

    public slots:
        void setCurrentData(const QVariant& data, int role=Qt::UserRole);
    signals:

    public slots:

};

#endif // QFENHANCEDCOMBOBOX_H
