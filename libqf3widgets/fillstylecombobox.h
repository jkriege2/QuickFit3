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


#ifndef FillStyleComboBox_H
#define FillStyleComboBox_H

#include <QComboBox>
#include <QColor>
#include <QVariant>
#include <QBrush>
#include "libwid_imexport.h"

/*! \brief a QComboBox variant that allows to select line styles
    \ingroup tools_qt
*/
class QFWIDLIB_EXPORT FillStyleComboBox : public QComboBox {
        Q_OBJECT
    public:
        /** Default constructor */
        FillStyleComboBox(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~FillStyleComboBox();

        /** \brief insert a given line style at the given position */
        void insertFillStyle(int index, Qt::BrushStyle style, const QString &name);

        /** \brief insert a given line style at the end of the list */
        inline void addFillStyle(Qt::BrushStyle style, const QString &name) {
            insertFillStyle(styleCount(), style, name);
        };


        /** \brief return the number of line styles in the list */
        inline int styleCount() const {
            return count();
        };

        /** \brief set the current item to the given line style (and add it, if it is not there) */
        inline void setCurrentFillStyle(Qt::BrushStyle style) {
            int idx=findData((int)style);
            if (idx>=0) setCurrentIndex(idx);
        }

        /** \brief return the currently selected line style */
        inline Qt::BrushStyle currentFillStyle() const {
            return fillStyle(currentIndex());
        }

        /** \brief return the line style of the given item index */
        Qt::BrushStyle fillStyle(int index) const {
            return (Qt::BrushStyle)(itemData(index).toInt());
        };
    signals:
        void activated(Qt::BrushStyle style);
        void highlighted(Qt::BrushStyle style);

    protected slots:
        void emitActivated(int i) {
            emit activated(fillStyle(i));
        };
        void emitHighlighted(int i)  {
            emit highlighted(fillStyle(i));
        };
    private:
};

#endif // FillStyleComboBox_H
