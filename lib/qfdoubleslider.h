/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFDOUBLESLIDER_H
#define QFDOUBLESLIDER_H

#include <QSlider>

#include "lib_imexport.h"
#include "programoptions.h"

/*! \brief a slider that support double-values
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFDoubleSlider : public QSlider
{
        Q_OBJECT
    public:
        explicit QFDoubleSlider(QWidget *parent = 0);
        double dvalue() const;
    signals:
        void dsliderMoved(double value);
    public slots:
        void setDRange(double min, double max);
        void setDValue(double value);
        void setDMinimum(double min);
        void setDMaximum(double max);
    protected slots:
        void dsliderMovedSlot(int pos);
    protected:
        virtual void sliderChange(SliderChange change);
        double m_value;
        double d_min;
        double d_max;
        int m_steps;
        
};

CREATE_WIDGET_PROGRAMOPTIONS_QFLOADSTORE(QFDoubleSlider,setDValue,dvalue,double)

#endif // QFDOUBLESLIDER_H
