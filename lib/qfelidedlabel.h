/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFELIDEDLABEL_H
#define QFELIDEDLABEL_H
#include "lib_imexport.h"
#include <QLabel>

class QFLIB_EXPORT QFElidedLabel : public QLabel
{
        Q_OBJECT
        Q_PROPERTY(bool isElided READ isElided)
        Q_PROPERTY(Qt::TextElideMode m_elidedMode READ elidedMode() WRITE setElidedMode())
    public:
        explicit QFElidedLabel(QWidget *parent=0, Qt::WindowFlags f=0);
        explicit QFElidedLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
        bool isElided() const { return elided; }
        Qt::TextElideMode elidedMode() const;
    public slots:
        void setElided(bool elided);
        void setElidedMode(Qt::TextElideMode elidedMode);
    protected:
        void paintEvent(QPaintEvent *event);


    private:
        bool elided;
        Qt::TextElideMode m_elidedMode;
        
};

#endif // QFELIDEDLABEL_H
