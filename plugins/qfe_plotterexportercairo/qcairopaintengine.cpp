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


#include "qcairopaintengine.h"

#include <cairo/cairo-pdf.h>
#include <cairo/cairo-ps.h>
#include <cairo/cairo-svg.h>
#include "qcairopaintdevice.h"
#include <QDebug>


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


static void releasePattern(cairo_pattern_t *pat)
{
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

QCairoPaintEngine::QCairoPaintEngine():
    QPaintEngine(cairo_features())
{
    surface=NULL;
    cr=NULL;
    brushpattern=NULL;
    penpattern=NULL;
    exportText=false;
    end();
}

bool QCairoPaintEngine::begin(QPaintDevice *pd)
{
    if (surface) {
        end();
    }

    const QCairoPaintDevice* cpd=dynamic_cast<const QCairoPaintDevice*>(pd);
    surface = NULL;
    if (cpd) {
        exportText=cpd->getExportText();
        QCairoPaintDevice::CairoFileType ft=cpd->getFileType();
        QSizeF s=cpd->getFileSizeMM();
        //qDebug()<<ft;
        if (ft==QCairoPaintDevice::cftPDF14) {
            //qDebug()<<  "Cairo-PDF1.4";
            surface = cairo_pdf_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_pdf_surface_restrict_to_version(surface, CAIRO_PDF_VERSION_1_4);
        } else if (ft==QCairoPaintDevice::cftPDF15) {
            //Debug()<<  "Cairo-PDF1.5"<<s<<s.width()/25.4*72.0<<s.height()/25.4*72.0;
            surface = cairo_pdf_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_pdf_surface_restrict_to_version(surface, CAIRO_PDF_VERSION_1_5);
        } else if (ft==QCairoPaintDevice::cftPS2) {
           //qDebug()<<  "Cairo-PS2";
            surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
        } else if (ft==QCairoPaintDevice::cftPS3) {
           //qDebug()<<  "Cairo-PS3";
            surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
        } else if (ft==QCairoPaintDevice::cftEPS2) {
           //qDebug()<<  "Cairo-EPS2";
            surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
            cairo_ps_surface_set_eps(surface, 1);
        } else if (ft==QCairoPaintDevice::cftEPS3) {
           //qDebug()<<  "Cairo-EPS3";
            surface = cairo_ps_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_ps_surface_restrict_to_level(surface, CAIRO_PS_LEVEL_2);
            cairo_ps_surface_set_eps(surface, 1);
        } else if (ft==QCairoPaintDevice::cftSVG11) {
           //qDebug()<<  "Cairo-SVG11";
            surface = cairo_svg_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_svg_surface_restrict_to_version(surface, CAIRO_SVG_VERSION_1_1);
        } else if (ft==QCairoPaintDevice::cftSVG12) {
           //qDebug()<<  "Cairo-SVG12";
            surface = cairo_svg_surface_create (cpd->getFileName().toLocal8Bit().data(), s.width()/25.4*72.0, s.height()/25.4*72.0);
            cairo_svg_surface_restrict_to_version(surface, CAIRO_SVG_VERSION_1_2);
        }
    }
    if (surface) {
        if (cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS) {
            cr = cairo_create(surface);
            if (cr) {
                if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
                    qDebug()<<"QCairoPaintDevice error initializing CAIRO: "<<cairo_status_to_string(cairo_status(cr));
                    end();
                    return false;
                } else {
                    cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);
                }
            } else {
                qDebug()<<"QCairoPaintDevice error initializing CAIRO !!!";
                end();
                return false;
            }
        } else {
            qDebug()<<"QCairoPaintDevice error creating surface: "<<cairo_status_to_string(cairo_surface_status(surface));
            end();
            return false;
        }

    } else {
        qDebug()<<"UNKNOWN QCairoPaintDevice type !!!";
        return false;
    }
    return true;
}

bool QCairoPaintEngine::end()
{
    if (brushpattern) {
        releasePattern(brushpattern);
    }
    if (penpattern) {
        releasePattern(penpattern);
    }
    if (cr) {
        cairo_destroy(cr);
    }
    cr=NULL;
    if (surface) {
        cairo_surface_finish (surface);
        cairo_surface_destroy(surface);
    }
    surface=NULL;
    cdirtyflags=QPaintEngine::AllDirty;
    cpen=QPen();
    cbrush=QBrush();
    cfont=QFont();
    cmatrix=QMatrix();
    return true;
}

QPaintEngine::Type QCairoPaintEngine::type() const
{
    return QPaintEngine::User;
}

void QCairoPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
    drawImage(r, pm.toImage(), sr);
}

void QCairoPaintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    if (!exportText) {
        QPaintEngine::drawTextItem(p, textItem);
    } else {
        //cairo_matrix_t cm, cmBak;
        //cairo_get_matrix(cr, &cmBak);
        //cairo_translate(cr, p.x(), p.y());
        QFont oldF=cfont;
        cfont=textItem.font();
        updateFont();
        updatePen();

        cairo_move_to (cr, p.x(), p.y());
        cairo_show_text(cr, textItem.text().toUtf8().data());

        cfont=oldF;
        //cairo_set_matrix(cr, &cmBak);
    }
}

void QCairoPaintEngine::drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s)
{
    double w, h;
    cairo_surface_t *image;
    QImage img=pixmap.toImage().convertToFormat(QImage::Format_ARGB32);


    if (!img.isNull()) {
        cairo_format_t imgformat=CAIRO_FORMAT_ARGB32;

        updateMatrix();

        image = cairo_image_surface_create_for_data(img.bits(), imgformat, img.width(), img.height(), img.bytesPerLine());
        w = img.width();
        h = img.height();

        cairo_matrix_t cm;
        cairo_matrix_init_identity(&cm);
        cairo_matrix_translate (&cm, s.x(), s.y());
        cairo_matrix_invert(&cm);

        cairo_pattern_t* brush=cairo_pattern_create_for_surface(image);
        cairo_pattern_set_matrix(brush, &cm);
        cairo_pattern_set_extend(brush, CAIRO_EXTEND_REPEAT);

        cairo_rectangle(cr, r.x(), r.y(), r.width(), r.height());
        cairo_set_source(cr, brush);
        cairo_fill_preserve(cr);
        cairo_set_source_rgba(cr, 0,0,0,0);
        cairo_set_line_width(cr, 0.0);
        cairo_stroke(cr);
        //cairo_fill(cr);

        //cairo_set_source_surface (cr, image, 0, 0);
        //cairo_paint(cr);
        releasePattern(brush);
        updateMatrix();
    }
}

void QCairoPaintEngine::drawImage(const QRectF &r, const QImage &pm, const QRectF &sr, Qt::ImageConversionFlags /*flags*/)
{
   //qDebug()<<"drawImage r="<<r<<" sr="<<sr;
    double              w, h;
    cairo_surface_t *image;
    QImage img;
    QRect sri=sr.toRect();
    if (sri.isValid() && (sri.x()!=0 || sri.y()!=0 || sri.size()!=sri.size())) {
        img=pm.copy(sri).convertToFormat(QImage::Format_ARGB32);
    } else {
        img=pm.convertToFormat(QImage::Format_ARGB32);
    }


    if (!img.isNull()) {
        cairo_format_t imgformat=CAIRO_FORMAT_ARGB32;

        updateMatrix();

        image = cairo_image_surface_create_for_data(img.bits(), imgformat, img.width(), img.height(), img.bytesPerLine());
        w = img.width();
        h = img.height();

        cairo_matrix_t cm;
        cairo_matrix_init_identity(&cm);
        cairo_matrix_translate (&cm, r.x(), r.y());
        if (sr.isValid()) cairo_matrix_scale(&cm, r.width() / sr.width(), r.height() / sr.height());
        //cairo_matrix_translate (&cm, -sr.x(), -sr.y());
        cairo_matrix_invert(&cm);

        cairo_pattern_t* brush=cairo_pattern_create_for_surface(image);
        cairo_pattern_set_matrix(brush, &cm);
        cairo_pattern_set_extend(brush, CAIRO_EXTEND_NONE);

        cairo_rectangle(cr, r.x(), r.y(), r.width(), r.height());
        cairo_set_source(cr, brush);
        cairo_fill_preserve(cr);
        cairo_set_source_rgba(cr, 0,0,0,0);
        cairo_set_line_width(cr, 0.0);
        cairo_stroke(cr);
        //cairo_fill(cr);

        //cairo_set_source_surface (cr, image, 0, 0);
        //cairo_paint(cr);
        releasePattern(brush);
        updateMatrix();
    }
}

/*
void QCairoPaintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    cfont=textItem.font();
    cdirtyflags=cdirtyflags&(QPaintEngine::DirtyFont);
    updateFont();
    updateBrush();
    updatePen();
    //cairo_move_to (cr, 10.0, 135.0);
    cairo_show_text (cr, textItem.text().toLocal8Bit().data());
}*/



void QCairoPaintEngine::updateState(const QPaintEngineState &state)
{
    if (!cr || !surface) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::updateState]: no cairo or no surface!";
        return;
    }
    cdirtyflags=cdirtyflags|state.state();
    if ((state.state()&QPaintEngine::DirtyTransform)==QPaintEngine::DirtyTransform) {
        cmatrix=state.matrix();
        cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyTransform);
    }
    if ((state.state()&QPaintEngine::DirtyPen)==QPaintEngine::DirtyPen) {
        //qDebug()<<"updateState "<<state.state()<<state.pen();
        cpen=state.pen();
        cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyPen);
    }
    if ((state.state()&QPaintEngine::DirtyBrush)==QPaintEngine::DirtyBrush) {
        cbrush=state.brush();
        cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyBrush);
    }
    if ((state.state()&QPaintEngine::DirtyFont)==QPaintEngine::DirtyFont) {
        cfont=state.font();
        cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyFont);
    }
    if (((state.state()&QPaintEngine::DirtyClipRegion)==QPaintEngine::DirtyClipRegion)) {
        cclipregion=state.clipRegion();
        updateClip();
        cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyClipRegion);
    }
    if (((state.state()&QPaintEngine::DirtyClipPath)==QPaintEngine::DirtyClipPath)) {
        cclippath=state.clipPath();
        updateClip();
        cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyClipPath);
    }
    if (((state.state()&QPaintEngine::DirtyClipEnabled)==QPaintEngine::DirtyClipEnabled)) {
        cclipenabled=state.isClipEnabled();
        updateClip();
        cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyClipEnabled);
    }
    updateMatrix();
    /*updateBrush();
    updatePen();
    updateFont();*/
}


void QCairoPaintEngine::updateMatrix()
{
    //qDebug()<<" QCairoPaintEnginePrivate::updateMatrix() "<<cdirtyflags;
    //if (cdirtyflags&QPaintEngine::DirtyTransform==QPaintEngine::DirtyTransform) {
        cairo_matrix_t cm;
        cairo_matrix_init(&cm, cmatrix.m11(), cmatrix.m12(), cmatrix.m21(), cmatrix.m22(), cmatrix.dx(), cmatrix.dy());
        cairo_set_matrix(cr, &cm);
        //cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyTransform);
    //}
}

void QCairoPaintEngine::updatePen()
{
    //qDebug()<<"updatePen: "<<cpen;
    //if (cdirtyflags&QPaintEngine::DirtyPen==QPaintEngine::DirtyPen) {
        double penwidth=0.01;
        if (cpen.widthF()!=0) penwidth=cpen.widthF();
        cairo_set_line_width (cr, penwidth);


        cairo_line_cap_t cap = CAIRO_LINE_CAP_ROUND;
        switch (cpen.capStyle())
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

        cairo_line_join_t join = CAIRO_LINE_JOIN_ROUND;
        switch (cpen.joinStyle())
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

        cairo_set_miter_limit(cr, cpen.miterLimit() * 2.0);

        QVector<qreal> dashes = cpen.dashPattern();
        for (int i = 0; i < dashes.size(); i++)
          dashes[i] *= penwidth;
        cairo_set_dash(cr, dashes.data(), dashes.size(), 0);

        //qDebug()<<"set pen"<<cpen;
        //cairo_set_source_rgba (cr, cpen.color().redF(), cpen.color().greenF(), cpen.color().blueF(), cpen.color().alphaF());
        if (penpattern) {
            releasePattern(penpattern);
        }
        penpattern=cairo_pattern_create_rgba(cpen.color().redF(), cpen.color().greenF(), cpen.color().blueF(), cpen.color().alphaF());
        cairo_set_source(cr, penpattern);



    //}
}


void QCairoPaintEngine::updateBrush()
{
    //qDebug()<<" QCairoPaintEnginePrivate::updateBrush() "<<cdirtyflags;
   //if (cdirtyflags&QPaintEngine::DirtyBrush==QPaintEngine::DirtyBrush) {

        //qDebug()<<"updateBrush "<<cbrush;

        cairo_pattern_t *pat=NULL;
        switch (cbrush.style())
        {
            case Qt::NoBrush:
                break;
            default:
            case Qt::SolidPattern:
                pat = cairo_pattern_create_rgba(cbrush.color().redF(), cbrush.color().greenF(), cbrush.color().blueF(), cbrush.color().alphaF());
                break;
        }

        if (brushpattern) {
            releasePattern(brushpattern);
        }
        brushpattern=pat;
        cairo_set_source(cr, brushpattern);


        //cairo_set_source_rgba(cr, cbrush.color().redF(), cbrush.color().greenF(), cbrush.color().blueF(), cbrush.color().alphaF());
        //cairo_fill_preserve(cr);
        //cairo_set_source_rgba (cr, cpen.color().redF(), cpen.color().greenF(), cpen.color().blueF(), cpen.color().alphaF());

        //cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyBrush);

    //}
}

void QCairoPaintEngine::updateFont()
{
    //qDebug()<<" QCairoPaintEnginePrivate::updateFont() "<<cdirtyflags;
   //if (cdirtyflags&QPaintEngine::DirtyFont==QPaintEngine::DirtyFont) {

       cairo_font_slant_t sl=CAIRO_FONT_SLANT_NORMAL;
       if (cfont.italic()) sl=CAIRO_FONT_SLANT_ITALIC;
       cairo_font_weight_t bf=CAIRO_FONT_WEIGHT_NORMAL;
       if (cfont.bold()) bf=CAIRO_FONT_WEIGHT_BOLD;

       cairo_select_font_face (cr, cfont.family().toLocal8Bit().data(), sl, bf);
       //qDebug()<<"cairo_select_font_face: "<<cairo_status_to_string(cairo_status(cr));
       cairo_set_font_size (cr, cfont.pointSizeF());
       //qDebug()<<"cairo_set_font_size: "<<cairo_status_to_string(cairo_status(cr));

        //cdirtyflags=cdirtyflags&(~QPaintEngine::DirtyFont);

       //}
}

void QCairoPaintEngine::updateClip()
{
    cairo_reset_clip(cr);
    //qDebug()<<cclipenabled<<cclippath<<cclipregion<<(!cclipregion.boundingRect().size().isEmpty() || cclippath.elementCount()>0);
    if (cclipenabled && (!cclipregion.boundingRect().size().isEmpty() || cclippath.elementCount()>0)) {
        cairo_matrix_t cm, cmBak;
        cairo_get_matrix(cr, &cmBak);
        cairo_matrix_init_identity(&cm);
        cairo_set_matrix(cr, &cm);

        bool fill=false;

        QPainterPath path=cclippath;
        path.addRegion(cclipregion);
        updatePath(path, fill);
        cairo_clip(cr);

        cairo_set_matrix(cr, &cmBak);
    }
}

void QCairoPaintEngine::updatePath(const QPainterPath &path, bool& fill)
{
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

    fill=false;
    if (start != -1 && start != elmCount - 1
          && path.elementAt(start).x == path.elementAt(elmCount - 1).x
            && path.elementAt(start).y == path.elementAt(elmCount - 1).y){
        cairo_close_path(cr);
        //qDebug()<<"closing path";
        fill=true;
    }
}

void QCairoPaintEngine::drawLines(const QLine *lines, int lineCount)
{
    QVector<QLineF> l;
    for (int i=0; i<lineCount; i++) {
        l.append(QLineF(lines[i]));
    }
    drawLines(l.data(), l.size());
}

void QCairoPaintEngine::drawLines(const QLineF *lines, int lineCount)
{
    if (!cr || !surface) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawLines]: no cairo or no surface!";
        return;
    }
    if (cpen.style()==Qt::NoPen) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawLines]: no pen set!";
        return;
    }
    updatePen();
    //qDebug()<<"drawLines n="<<lineCount;
    cairo_new_path(cr);
    for (int i=0; i<lineCount; i++) {
        cairo_move_to(cr, lines[i].x1(), lines[i].y1());
        cairo_line_to(cr, lines[i].x2(), lines[i].y2());
    }
    //cairo_close_path(cr);
    updatePen();
    cairo_stroke(cr);
}



void QCairoPaintEngine::drawPath(const QPainterPath &path)
{
    if (!cr || !surface) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawPath]: no cairo or no surface!";
        return;
    }
    if (cbrush.style()==Qt::NoBrush && cpen.style()==Qt::NoPen) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawPath]: no pen and no brush set!";
        return;
    }
    //qDebug()<<"drawPath n="<<path;

    bool fill;
    updatePath(path, fill);
    /*cairo_new_path(cr);

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

    bool fill=false;
    if (start != -1 && start != elmCount - 1
          && path.elementAt(start).x == path.elementAt(elmCount - 1).x
            && path.elementAt(start).y == path.elementAt(elmCount - 1).y){
        cairo_close_path(cr);
        //qDebug()<<"closing path";
        fill=cbrush.style()!=Qt::NoBrush;
    }*/
    if (fill) fill=cbrush.style()!=Qt::NoBrush;


    if (fill) {
        switch (path.fillRule()) {
            case Qt::WindingFill:
                cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING);
                break;
            default:
            case Qt::OddEvenFill:
                cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
                break;
        }


        updateBrush();
        if (cpen.style()!=Qt::NoPen) cairo_fill_preserve(cr);
        else cairo_fill(cr);
    }
    if (cpen.style()!=Qt::NoPen) {
        updatePen();
        cairo_stroke(cr);
    } else {

    }


}

void QCairoPaintEngine::drawPoints(const QPointF *points, int pointCount)
{


    if (!cr || !surface) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawPoints]: no cairo or no surface!";
        return;
    }
    if (cpen.style()==Qt::NoPen) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawPoints]: no pen set!";
        return;
    }
    updatePen();
    for (int i=0; i<pointCount; i++) {
        cairo_new_path(cr);
        cairo_move_to (cr, points[i].x(), points[i].y());

        cairo_close_path (cr);
    }
    //cairo_close_path(cr);
    updatePen();
    cairo_stroke(cr);

}

void QCairoPaintEngine::drawPolygon(const QPointF *points, int pointCount, QPaintEngine::PolygonDrawMode mode)
{
    if (!cr || !surface) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawPolygon]: no cairo or no surface!";
        return;
    }
    /*if (cpen.style()==Qt::NoPen) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawPolygon]: no pen set!";
        return;
    }*/
    if (mode!=QPaintEngine::PolylineMode && cbrush.style()==Qt::NoBrush && cpen.style()==Qt::NoPen) {
        qDebug()<<"Cairo Error [QCairoPaintEngine::drawPolygon]: no pen and no brush set!";
        return;
    }
    //updatePen();

    //qDebug()<<"drawPolygon n="<<pointCount;
    if (pointCount>1) {
        cairo_new_path(cr);
        cairo_move_to(cr, points[0].x(), points[0].y());
        for (int i=1; i<pointCount; i++) {
            cairo_line_to(cr, points[i].x(), points[i].y());
        }
        if (points[0].x()==points[pointCount-1].x() && points[0].y()==points[pointCount-1].y()){
            cairo_close_path(cr);
        } else if (mode!=QPaintEngine::PolylineMode) {
            cairo_close_path(cr);
        }
        if (mode==QPaintEngine::PolylineMode) {
            updatePen();
            cairo_stroke(cr);
        } else {
            switch (mode) {
                case QPaintEngine::WindingMode:
                    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING);
                    break;
                default:
                case QPaintEngine::OddEvenMode:
                    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
                    break;
            }


            updateBrush();
            if (cpen.style()!=Qt::NoPen) {
                cairo_fill_preserve(cr);
                updatePen();
                cairo_stroke(cr);
            } else cairo_fill(cr);
        }
    }

}

void QCairoPaintEngine::drawPolygon(const QPoint *points, int pointCount, QPaintEngine::PolygonDrawMode mode)
{
    QVector<QPointF> l(pointCount);
    for (int i=0; i<pointCount; i++) {
        l[i]=QPointF(points[i]);
    }
    drawPolygon(l.data(), l.size(), mode);

}


