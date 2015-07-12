/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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



#ifndef QFOVERLAYSTYLECOMBOBOX_H
#define QFOVERLAYSTYLECOMBOBOX_H

#include <QComboBox>
#include <QList>
#include <QColor>

class JKQTPOverlayImageEnhanced; // forward

class QFOverlayStyleCombobox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFOverlayStyleCombobox(QWidget *parent = 0);
        
        void setSelectedOverlayStyle(JKQTPOverlayImageEnhanced* plot) const;

    signals:
        
    public slots:
    protected:
        QList<QColor> selColors;
};

#endif // QFOVERLAYSTYLECOMBOBOX_H
