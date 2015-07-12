/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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


#ifndef QCAIROPAINTENGINE_H
#define QCAIROPAINTENGINE_H

#include <QPaintEngine>

#include <cairo/cairo.h>

class QCairoPaintEngine : public QPaintEngine
{
    public:
        QCairoPaintEngine();
        virtual bool begin(QPaintDevice* pdev );
        virtual bool end();
        virtual Type type() const;
        virtual void updateState(const QPaintEngineState& state );

        //virtual void drawRects(const QRect *rects, int rectCount);
        //virtual void drawRects(const QRectF *rects, int rectCount);

        virtual void drawLines(const QLine *lines, int lineCount);
        virtual void drawLines(const QLineF *lines, int lineCount);

        //virtual void drawEllipse(const QRectF &r);
        //virtual void drawEllipse(const QRect &r);

        virtual void drawPath(const QPainterPath &path);

        virtual void drawPoints(const QPointF *points, int pointCount);

        virtual void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
        virtual void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode);

        virtual void drawPixmap(const QRectF& r, const QPixmap& pm, const QRectF& sr );
        virtual void drawTextItem(const QPointF &p, const QTextItem &textItem);
        virtual void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s);
        virtual void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,  Qt::ImageConversionFlags flags = Qt::AutoColor);

    protected:
        cairo_surface_t* surface;
        cairo_t* cr;
        cairo_pattern_t* brushpattern;
        cairo_pattern_t* penpattern;
        //QPaintEngineState cstate;
        QPen cpen;
        QBrush cbrush;
        QMatrix cmatrix;
        QFont cfont;
        QPaintEngine::DirtyFlags cdirtyflags;
        QPainterPath cclippath;
        QRegion cclipregion;
        bool cclipenabled;
        bool exportText;

        void updateMatrix();
        void updatePen();
        void updateBrush();
        void updateFont();
        void updateClip();
        void updatePath(const QPainterPath &path, bool &fill);
};

#endif // QCAIROPAINTENGINE_H
