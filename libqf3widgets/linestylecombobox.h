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


#ifndef LineStyleComboBox_H
#define LineStyleComboBox_H

#include <QComboBox>
#include <QColor>
#include <QVariant>
#include <QPen>
#include "libwid_imexport.h"

/*! \brief a QComboBox variant that allows to select line styles
    \ingroup tools_qt
*/
class QFWIDLIB_EXPORT LineStyleComboBox : public QComboBox {
        Q_OBJECT
    public:
        /** Default constructor */
        LineStyleComboBox(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~LineStyleComboBox();

        /** \brief insert a given line style at the given position */
        void insertLineStyle(int index, Qt::PenStyle style, const QString &name);

        /** \brief insert a given line style at the end of the list */
        inline void addLineStyle(Qt::PenStyle style, const QString &name) {
            insertLineStyle(styleCount(), style, name);
        };


        /** \brief return the number of line styles in the list */
        inline int styleCount() const {
            return count();
        };

        /** \brief set the current item to the given line style (and add it, if it is not there) */
        inline void setCurrentLineStyle(Qt::PenStyle style) {
            int idx=findData((int)style);
            if (idx>=0) setCurrentIndex(idx);
        }

        /** \brief return the currently selected line style */
        inline Qt::PenStyle currentLineStyle() const {
            return lineStyle(currentIndex());
        }

        /** \brief return the line style of the given item index */
        Qt::PenStyle lineStyle(int index) const {
            return (Qt::PenStyle)(itemData(index).toInt());
        };
    signals:
        void activated(Qt::PenStyle style);
        void highlighted(Qt::PenStyle style);

    protected slots:
        void emitActivated(int i) {
            emit activated(lineStyle(i));
        };
        void emitHighlighted(int i)  {
            emit highlighted(lineStyle(i));
        };
    private:
};

#endif // LineStyleComboBox_H
