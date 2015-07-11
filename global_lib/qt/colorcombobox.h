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


#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H

#include <QComboBox>
#include <QColor>
#include <QVariant>
#include "../lib_imexport.h"
#include <QAction>


/*! \brief This class is used to notify all QF3FilterCombobox in the application to update their contents,
           if e.g. a configuration is deleted or added
    \ingroup tools_qt
*/
class ColorComboBoxNotifier: public QObject {
        Q_OBJECT
    public:
        ColorComboBoxNotifier(QObject* parent=NULL): QObject(parent) {};
    public slots:
        void emitNewColor(QColor newColor) { emit doUpdate(newColor); };
    signals:
        void doUpdate(QColor newColor);
};



/*! \brief a QComboBox variant that allows to select colors
    \ingroup tools_qt
*/
class LIB_EXPORT ColorComboBox : public QComboBox {
        Q_OBJECT
    public:
        /** Default constructor */
        ColorComboBox(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~ColorComboBox();

        /** \brief insert a given color at the given position */
        void insertColor(int index, const QColor &color, const QString &name);

        /** \brief insert a given color at the end of the list */
        inline void addColor(const QColor &color, const QString &name) {
            insertColor(colorCount()-1, color, name);
        };

        /** \brief insert a given color at the end of the list */
        inline void addNamedColor(const QString &name) {
            insertColor(colorCount()-1, QColor(name), name);
        };

        /** \brief return the number of colors in the list */
        inline int colorCount() const {
            return count();
        };

        /** \brief set the current item to the given color (and add it, if it is not there) */
        inline void setCurrentColor(const QColor &color) {
            int idx=findData(color);
            if (idx>=0) setCurrentIndex(idx);
            else {
                addColor(color, color.name());
                setCurrentIndex(findData(color));
            }
        }

        /** \brief return the currently selected color */
        inline QColor currentColor() const {
            return color(currentIndex());
        }

        /** \brief return the color of the given item index */
        QColor color(int index) const {
            return  itemData(index).value<QColor>();
        };
    signals:
        void activated(const QColor &color);
        void highlighted(const QColor &color);
        void currentColorChanged(const QColor &color);

    protected slots:
        void emitActivated(int i);
        void emitCurrentColorChanged(int i);
        void emitHighlighted(int i)  {
            emit highlighted(color(i));
        };
        void loadColors();
        void storeColors();
        void addNewColor(QColor newColor);
        void newUserColor();
    protected:
        static ColorComboBoxNotifier* m_notifier;
        QAction* actNewColor;
};

#if defined QFWIDLIB_LIBRARY || defined IS_QUICKFIT3_PLUGIN
#  include "programoptions.h"
CREATE_WIDGET_PROGRAMOPTIONS_QFLOADSTORE(ColorComboBox,setCurrentColor,currentColor,QColor)
#endif

#endif // COLORCOMBOBOX_H
