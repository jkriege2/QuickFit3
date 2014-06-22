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

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QBitmap>
#include "qpaintengine_p.h"
#include "qpaintengine_cairo_p.h"
#include <QDebug>

#ifdef Q_WS_X11
#include "qt_x11_p.h"
#include <QtGui/qx11info_x11.h>
#include <X11/Xutil.h>
#include <cairo/cairo-xlib.h>

extern Drawable qt_x11Handle(const QPaintDevice *pd); // in qpaintdevice_x11.cpp
extern const QX11Info *qt_x11Info(const QPaintDevice *pd);

#endif

#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <cairo/cairo-ps.h>
#include <cairo/cairo-svg.h>

#include "brushpattern.cpp"

class DrawItem
{
public:
  typedef enum { DrawPath, DrawText } DrawType;
  DrawItem(DrawType _type, bool _fill, bool _stroke, bool _antialias)
    :type(_type), fill(_fill), stroke(_stroke), antialias(_antialias) {}

  DrawType type;
  bool fill;
  bool stroke;
  bool antialias;
};

class PathDrawItem : public DrawItem
{
public:
  PathDrawItem(const QPainterPath &_path, bool fill, bool stroke, bool antialias)
    :DrawItem(DrawPath, fill, stroke, antialias), path(_path) {}

  const QPainterPath &path;
};

class QCairoPaintEnginePrivate : public QPaintEnginePrivate
{
public:
  QCairoPaintEnginePrivate();
  ~QCairoPaintEnginePrivate();

private:
  Q_DECLARE_PUBLIC(QCairoPaintEngine)
  void init();
  void end();

  void updateMatrix();
  void updateClip();
  void updatePen();
  void updateBrush();
  void updateBrushOrigin();
  void updateComposition();
  void updatePenWidth();
  void updateBrushInternal(const QBrush &brush, bool pen);

  void drawPath(const QPainterPath &path, bool fill);
  void drawPicture(const QPaintDevice &pd, const QRectF &r, const QRectF &sr);
  void updatePath(const QPainterPath &path);
  void drawInternal(const DrawItem &item, bool adjustMatrix);
  void drawPathInternal(const PathDrawItem &item);

  cairo_t* cr;
  cairo_pattern_t* penPattern;
  cairo_pattern_t* penMask;
  cairo_pattern_t* brushPattern;
  cairo_pattern_t* brushMask;
  QPaintEngine::DirtyFlags dirtyFlags;
  QBitmap qBrushMask;
};

static QImage adjustImage(const QImage &img)
{
  QImage dst(img);

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
  if (img.format() == QImage::Format_Mono)
    dst = img.convertToFormat(QImage::Format_MonoLSB);
#else
  if (img.format() == QImage::Format_MonoLSB)
    dst = img.convertToFormat(QImage::Format_Mono);
#endif
  else if (img.format() == QImage::Format_Indexed8)
    dst = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
  else if (img.format() == QImage::Format_ARGB32)
    dst = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

  return dst;
}

static cairo_surface_t* qtPaintDeviceToCairoSurface(const QPaintDevice &pd)
{
  const QCairoPaintDevice* cpd=dynamic_cast<const QCairoPaintDevice*>(&pd);
  cairo_surface_t *surface = NULL;
  if (cpd) {
      QCairoPaintDevice::CairoFileType ft=cpd->getFileType();
      QSizeF s=cpd->getFileSizeMM();
      if (ft==QCairoPaintDevice::cftPDF14) {
          qDebug()<<  "Cairo-PDF1.4";
          surface = cairo_pdf_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_pdf_surface_restrict_to_version(surface, CAIRO_PDF_VERSION_1_4);
      } else if (ft==QCairoPaintDevice::cftPDF15) {
          qDebug()<<  "Cairo-PDF1.5";
          surface = cairo_pdf_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_pdf_surface_restrict_to_version(surface, CAIRO_PDF_VERSION_1_5);
      } else if (ft==QCairoPaintDevice::cftPS2) {
          qDebug()<<  "Cairo-PS2";
          surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
      } else if (ft==QCairoPaintDevice::cftPS3) {
          qDebug()<<  "Cairo-PS3";
          surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
      } else if (ft==QCairoPaintDevice::cftEPS2) {
          qDebug()<<  "Cairo-EPS2";
          surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
          cairo_ps_surface_set_eps(surface, 1);
      } else if (ft==QCairoPaintDevice::cftEPS3) {
          qDebug()<<  "Cairo-EPS3";
          surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
          cairo_ps_surface_set_eps(surface, 1);
      } else if (ft==QCairoPaintDevice::cftSVG11) {
          qDebug()<<  "Cairo-SVG11";
          surface = cairo_svg_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_svg_surface_restrict_to_version(surface, CAIRO_SVG_VERSION_1_1);
      } else if (ft==QCairoPaintDevice::cftSVG12) {
          qDebug()<<  "Cairo-SVG12";
          surface = cairo_svg_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
          cairo_svg_surface_restrict_to_version(surface, CAIRO_SVG_VERSION_1_2);
      }
  } else {
      switch (pd.devType())
      {
          case QInternal::Widget:
              {
#ifdef Q_WS_X11
                  const QX11Info *xinfo = qt_x11Info(&pd);
                  surface = cairo_xlib_surface_create(xinfo->display(), qt_x11Handle(&pd),
                                                      (Visual*)xinfo->visual(), pd.width(), pd.height());
#endif
                  break;
              }

          case QInternal::Pixmap:
              {
#ifdef Q_WS_X11
                  const QX11Info *xinfo = qt_x11Info(&pd);
                  const QPixmap &pixmap = static_cast<const QPixmap&>(pd);
                  if (pixmap.isQBitmap())
                      surface = cairo_xlib_surface_create_for_bitmap(xinfo->display(), qt_x11Handle(&pd),
                                                                     ScreenOfDisplay(xinfo->display(), xinfo->screen()), pd.width(), pd.height());
                  else
                  {
                      Visual *v = (Visual*)xinfo->visual();
#ifndef QT_NO_XRENDER
                      XVisualInfo vinfo;
                      if (X11->use_xrender && pixmap.hasAlphaChannel())
                      {
                          if (XMatchVisualInfo(xinfo->display(), xinfo->screen(), pixmap.depth(), TrueColor, &vinfo))
                              v = vinfo.visual;
                      }
#endif
                      surface = cairo_xlib_surface_create(xinfo->display(), qt_x11Handle(&pd), v, pd.width(), pd.height());
                  }
#endif
                  break;
              }

          case QInternal::Image:
              {
                  static const int qtImageFormatToCairoFormat[QImage::NImageFormats] =
                  {
                      -1,
    #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
                      -1, CAIRO_FORMAT_A1,
    #else
                      CAIRO_FORMAT_A1, -1,
    #endif
                      -1,
                      CAIRO_FORMAT_RGB24, -1, CAIRO_FORMAT_ARGB32, CAIRO_FORMAT_RGB16_565
                  };

                  const QImage &img = static_cast<const QImage&>(pd);
                  int format = qtImageFormatToCairoFormat[img.format()];
                  if (format != -1)
                      surface = cairo_image_surface_create_for_data((uchar*)img.bits(), (cairo_format_t)format,
                                                                    img.width(), img.height(), img.bytesPerLine());
                  break;
              }

          default:
              break;
      }
  }

  if (!surface)
    qWarning("qpaintDeviceToCairoSurface error");

  return surface;
}

static void releasePattern(cairo_pattern_t *pat)
{
    qDebug()<<"releasePattern";
  if (!pat)
    return;

  if (cairo_pattern_get_type(pat) == CAIRO_PATTERN_TYPE_SURFACE)
  {
    cairo_surface_t *surface;
    cairo_pattern_get_surface(pat, &surface);
    cairo_surface_destroy(surface);
  }

  cairo_pattern_destroy(pat);
}

static QPaintEngine::PaintEngineFeatures cairo_features()
{
  QPaintEngine::PaintEngineFeatures features =
      QPaintEngine::PrimitiveTransform
    | QPaintEngine::PatternTransform
    | QPaintEngine::PixmapTransform
    | QPaintEngine::PatternBrush
    | QPaintEngine::LinearGradientFill
    | QPaintEngine::RadialGradientFill
    //| QPaintEngine::ConicalGradientFill
    | QPaintEngine::AlphaBlend
    | QPaintEngine::PorterDuff
    | QPaintEngine::PainterPaths
    | QPaintEngine::Antialiasing
    | QPaintEngine::BrushStroke
    | QPaintEngine::ConstantOpacity
    | QPaintEngine::MaskedBrush
    //| QPaintEngine::PaintOutsidePaintEvent
    ;

  return features;
}

QCairoPaintEngine::QCairoPaintEngine()
  :QPaintEngine(*(new QCairoPaintEnginePrivate), cairo_features())
{
    qDebug()<<"QCairoPaintEngine::QCairoPaintEngine()";
}

QCairoPaintEngine::QCairoPaintEngine(QCairoPaintEnginePrivate &dptr)
  :QPaintEngine(dptr, cairo_features())
{
    qDebug()<<"QCairoPaintEngine::QCairoPaintEngine(dptr)";
}

QCairoPaintEngine::~QCairoPaintEngine()
{
    qDebug()<<"QCairoPaintEngine::~QCairoPaintEngine()";
}

QPaintEngine::Type QCairoPaintEngine::type() const
{
  return QPaintEngine::User;
}

bool QCairoPaintEngine::begin(QPaintDevice *pdev)
{
   qDebug()<<"QCairoPaintEngine::begin";
  Q_D(QCairoPaintEngine);

  cairo_surface_t *surface = qtPaintDeviceToCairoSurface(*pdev);
  if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
  {
    qDebug()<<"CAIRO_ERROR (no surface): "<<cairo_status_to_string(cairo_surface_status(surface));
    cairo_surface_finish(surface);
    cairo_surface_destroy(surface);
    return false;
  }

  d->cr = cairo_create(surface);
  if (cairo_status(d->cr) != CAIRO_STATUS_SUCCESS)
  {
    qDebug()<<"CAIRO_ERROR (no create): "<<cairo_status_to_string(cairo_status(d->cr));
    cairo_destroy(d->cr);
    cairo_surface_finish(surface);
    cairo_surface_destroy(surface);
    d->cr = NULL;
    return false;
  }

  qDebug()<<"QCairoPaintEngine::begin: OK";

  return true;
}

bool QCairoPaintEngine::end()
{
    qDebug()<<"QCairoPaintEngine::end";
  Q_D(QCairoPaintEngine);
  d->end();
  qDebug()<<"QCairoPaintEngine::end: OK";

  return true;
}

void QCairoPaintEngine::drawPoints(const QPointF *points, int pointCount)
{
    qDebug()<<"QCairoPaintEngine::drawPoints";
  Q_D(QCairoPaintEngine);

  QPainterPath path;
  for (int i = 0; i < pointCount; i++)
  {
    path.moveTo(points[i]);
    path.lineTo(points[i].x() + 0.001, points[i].y());
  }

  d->updatePen();
  cairo_line_cap_t capBak = cairo_get_line_cap(d->cr);
  cairo_set_line_cap(d->cr, CAIRO_LINE_CAP_SQUARE);

  d->drawPath(path, false);

  cairo_set_line_cap(d->cr, capBak);
}

void QCairoPaintEngine::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
    qDebug()<<"QCairoPaintEngine::drawPolygon";
  Q_D(QCairoPaintEngine);

  QPolygonF pol;
  for (int i = 0; i < pointCount; i++)
    pol.append(points[i]);

  if (mode != QPaintEngine::PolylineMode)
    pol.append(points[0]);

  QPainterPath path;
  path.addPolygon(pol);
  path.setFillRule(mode == QPaintEngine::OddEvenMode ? Qt::OddEvenFill : Qt::WindingFill);

  d->drawPath(path, mode != QPaintEngine::PolylineMode);
}

void QCairoPaintEngine::drawPath(const QPainterPath &path)
{
    qDebug()<<"QCairoPaintEngine::drawPath";
  Q_D(QCairoPaintEngine);
  d->drawPath(path, true);
}

void QCairoPaintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    qDebug()<<"QCairoPaintEngine::drawTextItem";
  QPaintEngine::drawTextItem(p, textItem);
}

void QCairoPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
    qDebug()<<"QCairoPaintEngine::drawPixmap";
  Q_D(QCairoPaintEngine);
  d->drawPicture(pm, r, sr);
}

void QCairoPaintEngine::drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &s)
{
    qDebug()<<"QCairoPaintEngine::drawTiledPixmap";
  Q_D(QCairoPaintEngine);
  d->drawPicture(pm, r, QRectF(s, QSizeF()));
}

void QCairoPaintEngine::drawImage(const QRectF &r, const QImage &img, const QRectF &sr, Qt::ImageConversionFlags)
{
    qDebug()<<"QCairoPaintEngine::drawImage";
  Q_D(QCairoPaintEngine);
  d->drawPicture(adjustImage(img), r, sr);
}

void QCairoPaintEngine::updateState(const QPaintEngineState &state)
{
    qDebug()<<"QCairoPaintEngine::updateState";
  Q_D(QCairoPaintEngine);
  d->dirtyFlags |= state.state();
}

QCairoPaintEnginePrivate::QCairoPaintEnginePrivate()
{
    qDebug()<<"QCairoPaintEnginePrivate::QCairoPaintEnginePrivate()";
  init();
}

QCairoPaintEnginePrivate::~QCairoPaintEnginePrivate()
{
    qDebug()<<"QCairoPaintEnginePrivate::~QCairoPaintEnginePrivate()";
  end();
}

void QCairoPaintEnginePrivate::init()
{
    qDebug()<<"QCairoPaintEnginePrivate::init";
  cr = NULL;
  penPattern = penMask = NULL;
  brushPattern = brushMask = NULL;
  dirtyFlags = QPaintEngine::AllDirty;
  qBrushMask = QBitmap();
}

void QCairoPaintEnginePrivate::end()
{
    qDebug()<<"QCairoPaintEnginePrivate::end";
  releasePattern(penPattern);
  releasePattern(penMask);
  releasePattern(brushPattern);
  releasePattern(brushMask);

  if (cr)
  {
    cairo_surface_t *surface = cairo_get_target(cr);
    cairo_destroy(cr);
    cairo_surface_finish (surface);
    cairo_surface_destroy(surface);
  }

  init();
}

void QCairoPaintEnginePrivate::updateComposition()
{
    qDebug()<<"QCairoPaintEnginePrivate::updateComposition()";
  if (!(dirtyFlags & QPaintEngine::DirtyCompositionMode))
    return;

  dirtyFlags &= ~QPaintEngine::DirtyCompositionMode;

  Q_Q(QCairoPaintEngine);

  static const cairo_operator_t qtCompositionModeToCairoOp[QPainter::CompositionMode_Xor + 1] =
  {
    CAIRO_OPERATOR_OVER,
    CAIRO_OPERATOR_DEST_OVER,
    CAIRO_OPERATOR_CLEAR,
    CAIRO_OPERATOR_SOURCE,
    CAIRO_OPERATOR_DEST,
    CAIRO_OPERATOR_IN,
    CAIRO_OPERATOR_DEST_IN,
    CAIRO_OPERATOR_OUT,
    CAIRO_OPERATOR_DEST_OUT,
    CAIRO_OPERATOR_ATOP,
    CAIRO_OPERATOR_DEST_ATOP,
    CAIRO_OPERATOR_XOR,
  };

  cairo_set_operator(cr, qtCompositionModeToCairoOp[q->painter()->compositionMode()]);
}

#define DirtyPenWidth (QPaintEngine::DirtyFlag)0x8000

void QCairoPaintEnginePrivate::updateMatrix()
{
    qDebug()<<" QCairoPaintEnginePrivate::updateMatrix()";
  if (!(dirtyFlags & QPaintEngine::DirtyTransform))
    return;

  dirtyFlags &= ~QPaintEngine::DirtyTransform;

  Q_Q(QCairoPaintEngine);

  cairo_matrix_t cm;
  QMatrix qm = q->painter()->deviceMatrix();
  cairo_matrix_init(&cm, qm.m11(), qm.m12(), qm.m21(), qm.m22(), qm.dx(), qm.dy());
  cairo_set_matrix(cr, &cm);

  QPen pen = q->painter()->pen();
  if (pen.style() != Qt::NoPen && pen.widthF() == 0.)
    dirtyFlags |= DirtyPenWidth;
}

void QCairoPaintEnginePrivate::updateClip()
{
    qDebug()<<"QCairoPaintEnginePrivate::updateClip()";
  if (!(dirtyFlags & (QPaintEngine::DirtyClipEnabled | QPaintEngine::DirtyClipPath | QPaintEngine::DirtyClipRegion)))
    return;

  dirtyFlags &= ~(QPaintEngine::DirtyClipEnabled | QPaintEngine::DirtyClipPath | QPaintEngine::DirtyClipRegion);

  Q_Q(QCairoPaintEngine);
  cairo_reset_clip(cr);

  #define CLIP_REGION(_rgn) \
  do \
  { \
    QRegion rgn = _rgn; \
    if (!rgn.isEmpty()) \
    { \
      QPainterPath path; \
      path.addRegion(rgn); \
      updatePath(path); \
      cairo_clip(cr); \
    } \
  } while (0)

  QRegion sys = q->systemClip();
  if (!sys.isEmpty())
  {
    cairo_matrix_t cm, cmBak;
    cairo_get_matrix(cr, &cmBak);
    cairo_matrix_init_identity(&cm);
    cairo_set_matrix(cr, &cm);
    CLIP_REGION(sys);
    cairo_set_matrix(cr, &cmBak);
  }

  if (q->painter()->hasClipping())
    CLIP_REGION(q->painter()->clipRegion());
}

void QCairoPaintEnginePrivate::updatePen()
{
    qDebug()<<"updatePen()";

    if (!(dirtyFlags & QPaintEngine::DirtyPen))
    return;

  dirtyFlags &= ~QPaintEngine::DirtyPen;

  Q_Q(QCairoPaintEngine);
  QPen pen = q->painter()->pen();

  bool nopen = (pen.style() == Qt::NoPen);
  updateBrushInternal(nopen ? QBrush(Qt::NoBrush) : pen.brush(), true);
  if (nopen)
    return;

  cairo_line_cap_t cap;
  switch (pen.capStyle())
  {
    case Qt::FlatCap:
      cap = CAIRO_LINE_CAP_BUTT;
      break;

    case Qt::SquareCap:
      cap = CAIRO_LINE_CAP_SQUARE;
      break;

    case Qt::RoundCap:
      cap = CAIRO_LINE_CAP_ROUND;
      break;

    default:
      break;
  }
  cairo_set_line_cap(cr, cap);

  cairo_line_join_t join;
  switch (pen.joinStyle())
  {
    case Qt::MiterJoin:
    case Qt::SvgMiterJoin:
      join = CAIRO_LINE_JOIN_MITER;
      break;

    case Qt::BevelJoin:
      join = CAIRO_LINE_JOIN_BEVEL;
      break;

    case Qt::RoundJoin:
      join = CAIRO_LINE_JOIN_ROUND;
      break;

    default:
      break;
  }
  cairo_set_line_join(cr, join);

  cairo_set_miter_limit(cr, pen.miterLimit() * 2);
  dirtyFlags |= DirtyPenWidth;
}

void QCairoPaintEnginePrivate::updatePenWidth()
{
    qDebug()<<"updatePenWidth()";
  if (!(dirtyFlags & DirtyPenWidth))
    return;

  dirtyFlags &= ~DirtyPenWidth;

  Q_Q(QCairoPaintEngine);

  QPen pen = q->painter()->pen();

  double width = pen.widthF();
  if (width == 0.)
  {
    width = 1.;

    QMatrix qm = q->painter()->deviceMatrix();
    if (qm.m12() == 0. && qm.m21() == 0. && qm.m11() == qm.m22())
      width = 1. / qAbs(qm.m11());
  }
  cairo_set_line_width(cr, width);

  QVector<qreal> dashes = pen.dashPattern();
  for (int i = 0; i < dashes.size(); i++)
    dashes[i] *= width;
  cairo_set_dash(cr, dashes.data(), dashes.size(), 0);
}

void QCairoPaintEnginePrivate::updateBrush()
{
    qDebug()<<"updateBrush()";

    if (!(dirtyFlags & QPaintEngine::DirtyBrush))
    return;

  dirtyFlags &= ~QPaintEngine::DirtyBrush;

  Q_Q(QCairoPaintEngine);
  updateBrushInternal(q->painter()->brush(), false);
  dirtyFlags |= QPaintEngine::DirtyBrushOrigin;
}

void QCairoPaintEnginePrivate::updateBrushInternal(const QBrush &brush, bool pen)
{
    qDebug()<<"updateBrushInternal()";
  cairo_pattern_t *pat, *mask;
  pat = mask = NULL;

  switch (brush.style())
  {
    case Qt::SolidPattern:
    {
      const QColor &color(brush.color());
      pat = cairo_pattern_create_rgba(color.redF(), color.greenF(), color.blueF(), color.alphaF());
      break;
    }

    case Qt::LinearGradientPattern:
    case Qt::RadialGradientPattern:
    {
      if (brush.style() == Qt::LinearGradientPattern)
      {
        const QLinearGradient *lg = static_cast<const QLinearGradient*>(brush.gradient());
        pat = cairo_pattern_create_linear(lg->start().x(), lg->start().y(),
                lg->finalStop().x(), lg->finalStop().y());
      } else
      {
        const QRadialGradient *rg = static_cast<const QRadialGradient*>(brush.gradient());
        pat = cairo_pattern_create_radial(rg->focalPoint().x(), rg->focalPoint().y(), 0.,
                rg->center().x(), rg->center().y(), rg->radius());
      }

      QGradientStops gstops = brush.gradient()->stops();
      for (int i = 0; i < gstops.size(); i++)
      {
        QColor color = gstops.at(i).second;
        cairo_pattern_add_color_stop_rgba(pat, gstops.at(i).first,
          color.redF(), color.greenF(), color.blueF(), color.alphaF());
      }

      static const cairo_extend_t qtGradientSpreadToCairoPatternExtend[QGradient::RepeatSpread + 1] =
      {
        CAIRO_EXTEND_PAD,
        CAIRO_EXTEND_REFLECT,
        CAIRO_EXTEND_REPEAT,
      };
      cairo_pattern_set_extend(pat, qtGradientSpreadToCairoPatternExtend[brush.gradient()->spread()]);
      break;
    }

    case Qt::Dense1Pattern ... Qt::DiagCrossPattern:
    case Qt::TexturePattern:
    {
      QPixmap pixmap;
      if (brush.style() == Qt::TexturePattern)
        pixmap = brush.texture();
      else
      {
        extern QPixmap qt_pixmapForBrush(int brushStyle, bool invert); //in qbrush.cpp
        pixmap = qt_pixmapForBrush(brush.style(), true);
      }

      pat = cairo_pattern_create_for_surface(qtPaintDeviceToCairoSurface(pixmap));

      if (pixmap.isQBitmap())
      {
        mask = pat;

        const QColor &color(brush.color());
        pat = cairo_pattern_create_rgba(color.redF(), color.greenF(), color.blueF(), color.alphaF());
      } else
#if defined Q_WS_X11 && !defined QT_NO_XRENDER
      if (!(X11->use_xrender && pixmap.hasAlphaChannel()))
#endif
      {
        qBrushMask = pixmap.mask();
        if (!qBrushMask.isNull())
          mask = cairo_pattern_create_for_surface(qtPaintDeviceToCairoSurface(qBrushMask));
      }

      if (pat)
        cairo_pattern_set_extend(pat, CAIRO_EXTEND_REPEAT);

      if (mask)
        cairo_pattern_set_extend(mask, CAIRO_EXTEND_REPEAT);
      break;
    }

    default:
      break;
  }

  if (pen)
  {
    releasePattern(penPattern);
    releasePattern(penMask);
    penPattern = pat;
    penMask = mask;
  } else
  {
    releasePattern(brushPattern);
    releasePattern(brushMask);
    brushPattern = pat;
    brushMask = mask;
  }
}

void QCairoPaintEnginePrivate::updateBrushOrigin()
{
    qDebug()<<"updateBrushOrigin()";
  if (!(dirtyFlags & QPaintEngine::DirtyBrushOrigin))
    return;

  dirtyFlags &= ~QPaintEngine::DirtyBrushOrigin;

  Q_Q(QCairoPaintEngine);
  QPointF origin = q->painter()->brushOrigin();

  cairo_matrix_t cm;
  cairo_matrix_init_translate(&cm, -origin.x(), -origin.y());

  if (brushPattern)
    cairo_pattern_set_matrix(brushPattern, &cm);

  if (brushMask)
    cairo_pattern_set_matrix(brushMask, &cm);
}

void QCairoPaintEnginePrivate::updatePath(const QPainterPath &path)
{
    qDebug()<<"updatePath()";
  cairo_new_path(cr);

  int start = -1, elmCount = path.elementCount();
  for (int index = 0; index < elmCount; index++)
  {
    const QPainterPath::Element elm = path.elementAt(index);
    switch (elm.type)
    {
      case QPainterPath::MoveToElement:
        cairo_move_to(cr, elm.x, elm.y);
        start = index;
        break;

      case QPainterPath::LineToElement:
        cairo_line_to(cr, elm.x, elm.y);
        break;

      case QPainterPath::CurveToElement:
        cairo_curve_to(cr, elm.x, elm.y,
            path.elementAt(index + 1).x, path.elementAt(index + 1).y,
            path.elementAt(index + 2).x, path.elementAt(index + 2).y);
        index += 2;
        break;

      default:
        break;
    }
  }

  if (start != -1 && start != elmCount - 1
        && path.elementAt(start).x == path.elementAt(elmCount - 1).x
        && path.elementAt(start).y == path.elementAt(elmCount - 1).y)
    cairo_close_path(cr);

  static const cairo_fill_rule_t qtFillRuleToCairoFillRule[Qt::WindingFill + 1] =
  {
    CAIRO_FILL_RULE_EVEN_ODD,
    CAIRO_FILL_RULE_WINDING,
  };
  cairo_set_fill_rule(cr, qtFillRuleToCairoFillRule[path.fillRule()]);
}

void QCairoPaintEnginePrivate::drawPath(const QPainterPath &path, bool fill)
{
    qDebug()<<"drawPath()";
  Q_Q(QCairoPaintEngine);

  bool antialias = q->painter()->renderHints() & QPainter::Antialiasing;
  drawInternal(PathDrawItem(path, fill, true, antialias), true);
}

void QCairoPaintEnginePrivate::drawPicture(const QPaintDevice &pd, const QRectF &r, const QRectF &sr)
{
    qDebug()<<"drawPicture()";
  cairo_pattern_t *pat = cairo_pattern_create_for_surface(qtPaintDeviceToCairoSurface(pd));
  if (!pat)
    return;

  cairo_pattern_t *brushBak = brushPattern, *maskBak = brushMask;
  brushPattern = pat;
  brushMask = NULL;

  QBitmap qmask;
  if (pd.devType() == QInternal::Pixmap)
  {
    const QPixmap &pixmap =  static_cast<const QPixmap&>(pd);
    if (!pixmap.isQBitmap()
#if defined Q_WS_X11 && !defined QT_NO_XRENDER
        && !(X11->use_xrender && pixmap.hasAlphaChannel())
#endif
       )
    {
      qmask = pixmap.mask();
      if (!qmask.isNull())
        brushMask = cairo_pattern_create_for_surface(qtPaintDeviceToCairoSurface(qmask));
    }
  }

  cairo_matrix_t cm;
  cairo_matrix_init_identity(&cm);
  cairo_matrix_translate(&cm, r.x(), r.y());
  if (sr.isValid())
    cairo_matrix_scale(&cm, r.width() / sr.width(), r.height() / sr.height());
  cairo_matrix_translate(&cm, -sr.x(), -sr.y());
  cairo_matrix_invert(&cm);

  cairo_pattern_set_matrix(brushPattern, &cm);
  cairo_pattern_set_extend(brushPattern, CAIRO_EXTEND_REPEAT);

  if (brushMask)
  {
    cairo_pattern_set_matrix(brushMask, &cm);
    cairo_pattern_set_extend(brushMask, CAIRO_EXTEND_REPEAT);
  }

  QPaintEngine::DirtyFlags dirtyBrushBak = dirtyFlags & (QPaintEngine::DirtyBrush | QPaintEngine::DirtyBrushOrigin);
  dirtyFlags &= ~(QPaintEngine::DirtyBrush | QPaintEngine::DirtyBrushOrigin);

  QPainterPath path;
  path.addRect(r);

  Q_Q(QCairoPaintEngine);
  bool antialias = q->painter()->renderHints() & (QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  drawInternal(PathDrawItem(path, true, false, antialias), false);

  dirtyFlags |= dirtyBrushBak;

  releasePattern(brushPattern);
  releasePattern(brushMask);
  brushPattern = brushBak;
  brushMask = maskBak;
}

void QCairoPaintEnginePrivate::drawInternal(const DrawItem &item, bool adjustMatrix)
{
    qDebug()<<"drawInternal()";
  updateMatrix();
  updateClip();
  updateComposition();

  if (item.fill)
  {
    updateBrush();
    updateBrushOrigin();
  }

  if (item.stroke)
  {
    updatePen();
    updatePenWidth();
  }

  if (!(item.fill && brushPattern) && !(item.stroke && penPattern))
    return;

  cairo_set_antialias(cr, item.antialias ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);

  Q_Q(QCairoPaintEngine);

#ifdef Q_WS_X11
  cairo_matrix_t cmOld;
  adjustMatrix &= !item.antialias;

  if (adjustMatrix)
  {
    cairo_get_matrix(cr, &cmOld);

    cairo_matrix_t cm;
    cm = cmOld;
    cm.x0 += 0.5;
    cairo_set_matrix(cr, &cm);
  }
#endif

  double opacity = q->painter()->opacity();
  if (opacity != 1.)
  {
    cairo_save(cr);

    QRectF rect;
    if (item.type == DrawItem::DrawPath)
    {
      rect = static_cast<const PathDrawItem &>(item).path.controlPointRect();
      if (item.stroke && penPattern)
      {
        double width = cairo_get_line_width(cr);

        cairo_matrix_t cm;
        cairo_get_matrix(cr, &cm);
        if (q->painter()->pen().widthF() == 0. && (cm.xy || cm.yx || cm.xx != cm.yy))
        {
          double height = 1.;
          width = 1.;
          cairo_device_to_user_distance(cr, &width, &height);
          width = qMax(qAbs(width), qAbs(height));
        }

        rect.adjust(-width, -width, width, width);
      }
    } else
    {

    }

    QPainterPath boundingPath;
    boundingPath.addRect(rect);
    updatePath(boundingPath);
    cairo_clip(cr);

    cairo_push_group(cr);
  }

  if (item.type == DrawItem::DrawPath)
    drawPathInternal(static_cast<const PathDrawItem &>(item));
  else
  {
  }

  if (opacity != 1.)
  {
    cairo_pop_group_to_source(cr);
    cairo_paint_with_alpha(cr, opacity);

    cairo_restore(cr);
  }

#ifdef Q_WS_X11
  if (adjustMatrix)
    cairo_set_matrix(cr, &cmOld);
#endif

  cairo_surface_flush(cairo_get_target(cr));
}

void QCairoPaintEnginePrivate::drawPathInternal(const PathDrawItem &item)
{
    qDebug()<<"drawPathInternal()";
  updatePath(item.path);

  if (item.fill && brushPattern)
  {
    cairo_set_source(cr, brushPattern);
    if (brushMask)
    {
      cairo_save(cr);
      cairo_clip_preserve(cr);
      cairo_mask(cr, brushMask);
      cairo_restore(cr);
    } else
      cairo_fill_preserve(cr);
  }

  if (item.stroke && penPattern)
  {
    Q_Q(QCairoPaintEngine);
    cairo_set_source(cr, penPattern);
    if (penMask)
    {
      QPen pen = q->painter()->pen();
      QPainterPathStroker stroker;

      stroker.setWidth(pen.widthF());
      stroker.setCapStyle(pen.capStyle());
      stroker.setJoinStyle(pen.joinStyle());
      stroker.setMiterLimit(pen.miterLimit());
      stroker.setDashPattern(pen.dashPattern());

      QPainterPath spath = stroker.createStroke(item.path);

      cairo_save(cr);
      updatePath(spath);
      cairo_clip(cr);
      cairo_mask(cr, penMask);
      cairo_restore(cr);
    } else
    {
      cairo_matrix_t cm;
      cairo_get_matrix(cr, &cm);

      bool transPath = (q->painter()->pen().widthF() == 0. && (cm.xy || cm.yx || cm.xx != cm.yy));
      if (transPath)
      {
        cairo_matrix_t cmNew;
        cairo_matrix_init_identity(&cmNew);
        cairo_set_matrix(cr, &cmNew);

        QMatrix qm(cm.xx, cm.yx, cm.xy, cm.yy, cm.x0, cm.y0);
        QPainterPath dstPath = item.path * qm;
        updatePath(dstPath);
      }

      cairo_stroke_preserve(cr);

      if (transPath)
        cairo_set_matrix(cr, &cm);
    }
  }
}
