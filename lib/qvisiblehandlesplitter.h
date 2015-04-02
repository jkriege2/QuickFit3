/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QVISIBLEHANDLESPLITTER_H
#define QVISIBLEHANDLESPLITTER_H

#include <QSplitter>
#include <QSplitterHandle>
#include "lib_imexport.h"

/*! \brief a QSplitter that has a really visible Splitter handle
    \ingroup tools_qt


 */
class QFLIB_EXPORT QVisibleHandleSplitter : public QSplitter {
        Q_OBJECT
    public:
        explicit QVisibleHandleSplitter(Qt::Orientation orientation, QWidget *parent = 0);
        explicit QVisibleHandleSplitter(QWidget *parent = 0);
        void setDecorated(bool enabled);
        void setHoverEffect(bool enabled);
        void setGripWidth(int width);
    signals:

    public slots:

    protected:
        QSplitterHandle *createHandle();

};



/*! \brief a QSplitterHandle that is really visible Splitter handle (used by QVisibleHandleSplitter)
    \ingroup tools_qt


 */
class QFLIB_EXPORT QVisibleGripSplitterHandle: public QSplitterHandle {
        Q_OBJECT
    public:
        QVisibleGripSplitterHandle(Qt::Orientation orientation, QSplitter * parent=NULL);
        void setDecorated(bool enabled) { m_drawGrip=enabled; }
        bool decorated() const { return m_drawGrip; }
        void setHoverEffect(bool enabled) { m_lightup=enabled; }
        bool hoverEffect() const { return m_lightup; }
        void setGripWidth(int width) { m_gripHeight=width; }
        int gripWidth() const { return m_gripHeight; }
    protected:
        virtual void leaveEvent(QEvent* event);
        virtual void enterEvent(QEvent* event);
        virtual void paintEvent(QPaintEvent* event);
        int m_gripHeight;
        bool m_inside;
        bool m_lightup;
        bool m_drawGrip;
};

#endif // QVISIBLEHANDLESPLITTER_H
