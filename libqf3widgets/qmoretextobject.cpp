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



#include "qmoretextobject.h"
#include <QTextFormat>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtSvg>


QSizeF QSvgTextObject::intrinsicSize(QTextDocument* doc, int /*posInDocument*/, const QTextFormat &format) {
    QImage bufferedImage =  format.property(1).value<QImage>();
    QSize size = bufferedImage.size();
    double relSize=format.property(3).toDouble();
    if (relSize>0) {
        size *= (relSize*doc->textWidth() / double(size.width()));
        return QSizeF(size);
    } else {
        if (size.width() > doc->textWidth())
            size *= doc->textWidth() / (double) size.width();
        return QSizeF(size);
    }
}


void QSvgTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument* /*doc*/, int /*posInDocument*/, const QTextFormat &format) {
    QImage bufferedImage =  format.property(1).value<QImage>();

    painter->drawImage(rect, bufferedImage);
}

bool insertSVG(QTextCursor cursor, int SvgTextFormat, QString fileName, double relativeWidth) {
     QFile file(fileName);
     if (!file.open(QIODevice::ReadOnly)) {
         return false;
     }

     QByteArray svgData = file.readAll();

     QTextCharFormat svgCharFormat;
     svgCharFormat.setObjectType(SvgTextFormat);
     QSvgRenderer renderer(svgData);

     QImage svgBufferImage(renderer.defaultSize(), QImage::Format_ARGB32);
     QPainter painter(&svgBufferImage);
     renderer.render(&painter, svgBufferImage.rect());

     svgCharFormat.setProperty(1, svgBufferImage);
     svgCharFormat.setProperty(3, relativeWidth);

     cursor.insertText(QString(QChar::ObjectReplacementCharacter), svgCharFormat);
     return true;
}











QSizeF QPictureTextObject::intrinsicSize(QTextDocument* doc, int /*posInDocument*/, const QTextFormat &format) {
    QByteArray data = format.property(1).toByteArray();
    QSizeF gsize= format.property(2).toSizeF();
    double relSize=format.property(3).toDouble();
    QPicture pic;
    pic.setData(data.constData(), data.size());
    QSizeF size = gsize;//pic.boundingRect().size();
    //qDebug()<<"QPictureTextObject::intrinsicSize relSize="<<relSize<<"  textWidth="<<doc->textWidth()<<"  pic.width="<<pic.width()<<" pic.boundingRect="<<pic.boundingRect();

    if (relSize>0) {
        size *= (relSize*doc->textWidth() / double(size.width()));
        //qDebug()<<"    => "<<size;
        return QSizeF(size);
    } else {
        if (size.width() > doc->textWidth())
            size *= doc->textWidth() / (double) size.width();

        //qDebug()<<"    => "<<size;
        return QSizeF(size);
    }
}


void QPictureTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument* doc, int /*posInDocument*/, const QTextFormat &format) {
    QByteArray data = format.property(1).toByteArray();
    double relSize=format.property(3).toDouble();
    QPicture pic;
    pic.setData(data.constData(), data.size());
    QSizeF size = pic.boundingRect().size();
    //qDebug()<<"QPictureTextObject::drawObject relSize="<<relSize<<"  textWidth="<<doc->textWidth()<<"  rect="<<rect<<" pic.width="<<pic.width()<<" pic.boundingRect="<<pic.boundingRect()<<" paintDeviceWidth="<<painter->device()->width()<<" paintDeviceDPI="<<painter->device()->logicalDpiX()<<" picDPI0"<<pic.logicalDpiX();

    painter->save();
        painter->translate(rect.topLeft());
        painter->save();
            /*double scale=rect.width()/size.width();
            if (scale*size.height()>rect.height()) {
                scale=rect.height()/size.height();
            }*/
            double scale=double(doc->textWidth())*relSize/double(size.width());
            //qDebug()<<"QPictureTextObject::drawObject scale="<<scale;
            if (relSize<=0) {
                scale=rect.width()/size.width();

            }
            if (scale*size.height()>rect.height()) {
                scale=rect.height()/size.height();
            }
            //qDebug()<<"QPictureTextObject::drawObject scale="<<scale;
            //painter->translate(-1.0*pic.boundingRect().topLeft());
            scale=scale*double(pic.logicalDpiX())/double(painter->device()->logicalDpiX());
            painter->scale(scale, scale);
            painter->save();
                painter->drawPicture(0,0, pic);
                /*QColor col("red");
                col.setAlphaF(0.5);
                painter->fillRect(0,0,pic.width(), pic.height(), col);
                painter->setPen(QColor("darkred"));
                painter->drawRect(0,0,pic.width(),pic.height());*/
            painter->restore();
        painter->restore();
    painter->restore();

    /*painter->save();
    painter->setPen(QColor("blue"));
    painter->drawRect(rect);
    painter->restore();*/
}

bool insertQPicture(QTextCursor cursor, int QPictureTextFormat, const QPicture& picture, QSizeF siz, double relativeWidth) {


     QByteArray data(picture.data(), picture.size());

     QTextCharFormat qpictureCharFormat;
     qpictureCharFormat.setObjectType(QPictureTextFormat);

     qpictureCharFormat.setProperty(1, data);
     QSizeF size=siz;
     if (siz.isNull()) size=picture.boundingRect().size();
     qpictureCharFormat.setProperty(2, size);
     qpictureCharFormat.setProperty(3, relativeWidth);

     cursor.insertText(QString(QChar::ObjectReplacementCharacter), qpictureCharFormat);
     return true;
}

bool insertQPicture(QTextCursor cursor, int QPictureTextFormat, const QPicture& picture_in, QPointF translate, QSizeF siz, double relativeWidth) {
     QPicture picture;
     QPainter* p=new QPainter(&picture);
     p->drawPicture(translate, picture_in);
     p->end();
     delete p;

     QByteArray data(picture.data(), picture.size());

     QTextCharFormat qpictureCharFormat;
     qpictureCharFormat.setObjectType(QPictureTextFormat);

     qpictureCharFormat.setProperty(1, data);
     QSizeF size=siz;
     if (siz.isNull()) size=picture.boundingRect().size();
     qpictureCharFormat.setProperty(2, size);
     qpictureCharFormat.setProperty(3, relativeWidth);

     cursor.insertText(QString(QChar::ObjectReplacementCharacter), qpictureCharFormat);
     return true;
}
