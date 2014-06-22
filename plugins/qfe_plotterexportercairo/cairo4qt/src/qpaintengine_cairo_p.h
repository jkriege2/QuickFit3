/*
 *  This file is part of cairo4qt.
 *
 *  Copyright (c) 2007 dragchan <zgchan317@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef QPAINTENGINE_CAIRO_P_H
#define QPAINTENGINE_CAIRO_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qpaintengine.h"
#include "qcairopaintdevice.h"

class QCairoPaintEnginePrivate;

class QCairoPaintEngine : public QPaintEngine
{
public:
    QCairoPaintEngine();
    ~QCairoPaintEngine();

    bool begin(QPaintDevice *pdev);
    bool end();

    void updateState(const QPaintEngineState &state);

    void drawPoints(const QPointF *points, int pointCount);
    void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    void drawPath(const QPainterPath &path);
    void drawTextItem(const QPointF &p, const QTextItem &textItem);
    void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);
    void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s);
    void drawImage(const QRectF &r, const QImage &img, const QRectF &sr,
                   Qt::ImageConversionFlags flags = Qt::AutoColor);

    Type type() const;

protected:
    QCairoPaintEngine(QCairoPaintEnginePrivate &dptr);

private:
    Q_DECLARE_PRIVATE(QCairoPaintEngine)
    Q_DISABLE_COPY(QCairoPaintEngine)
};

#endif
