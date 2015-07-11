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


#ifndef QMORETEXTOBJECT_H
#define QMORETEXTOBJECT_H

#include <QTextObjectInterface>

class QTextDocument;
class QTextFormat;
class QPainter;
class QRectF;
class QSizeF;
class QPicture;
class QTextCursor;
#include "../lib_imexport.h"

#include <QObject>


/*! \defgroup tools_qt_QTextObjects Additional QTextObjectInterface implementations
    \ingroup tools_qt

*/

/** \brief QTextObjectInterface to plot a SVG file (taken from Qt help)
 *  \ingroup tools_qt
 *
 */
class LIB_EXPORT QSvgTextObject : public QObject, public QTextObjectInterface {
        Q_OBJECT
        Q_INTERFACES(QTextObjectInterface)

     public:
         QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
         void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

     protected:
};

bool insertSVG(QTextCursor cursor, int SvgTextFormat, QString fileName, double relativeWidth=-1);



/** \brief QTextObjectInterface to plot a QPicture
 *  \ingroup tools_qt
 *
 */
class LIB_EXPORT QPictureTextObject : public QObject, public QTextObjectInterface {
        Q_OBJECT
        Q_INTERFACES(QTextObjectInterface)

     public:
         QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
         void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

     protected:
};

LIB_EXPORT bool insertQPicture(QTextCursor cursor, int SvgTextFormat, const QPicture& picture, QSizeF size=QSizeF(), double relativeWidth=-1);
LIB_EXPORT bool insertQPicture(QTextCursor cursor, int SvgTextFormat, const QPicture& picture, QPointF translate, QSizeF size=QSizeF(), double relativeWidth=-1);


#endif // QMORETEXTOBJECT_H
