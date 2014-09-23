#include <QApplication>
#include "qcairopaintdevice.h"
#include <QPainter>
#include <QImage>
#include <QDebug>
#include <cmath>

void paint(QPaintDevice* pd) {
    QPainter p;
    if (!p.begin(pd)) return;
    QPen pe(QColor("black"));
    pe.setWidthF(6.0);
    pe.setStyle(Qt::DashLine);
    p.setPen(pe);
    p.drawLine(QPointF(0,0),QPointF(100.0,100.0));
    pe.setColor("red");
    pe.setStyle(Qt::DotLine);
    p.setPen(pe);
    p.drawLine(QPointF(0,0),QPointF(50.0,25.0));
    pe.setColor("green");
    pe.setStyle(Qt::SolidLine);
    p.setPen(pe);
    p.drawLine(QPointF(0,0),QPointF(25.0,50.0));
    pe.setColor("blue");
    p.setPen(pe);
    p.drawText(50,10, QString("Hello World!"));
    p.drawLine(50,10,55,15);
    QFont f;
    QImage img(QString("qfe_plotterexportercairo.png"));
    pe.setWidthF(0.5);
    p.setPen(pe);
    p.drawImage(QRectF(50,25,50,50), img);
    //p.drawRect(QRectF(50,25,50,50));
    p.drawImage(QRectF(25,50,15,10), img);
    //p.drawRect(QRectF(25,50,15,10));
    p.drawImage(QRectF(225,50,100,100), img, QRect(5,5,12,12));
    p.drawRect(QRectF(125,50,15,10));
    pe.setWidthF(2);
    pe.setColor("red");
    p.setPen(pe);
    f.setFamily("Times New Roman");
    f.setPointSizeF(10);
    p.setFont(f);
    p.drawText(50,30, QString("Hello World!"));
    p.drawLine(50,30,55,35);

    f.setFamily("Arial");
    f.setPointSizeF(16);
    p.setFont(f);
    p.drawText(250,30, QString("Unicode-test: ")+QChar(0x2190)+QChar(0x2591)+QChar(0x2665)+QChar(0x039B)+QChar(0x03B6));
    p.drawLine(250,30,255,35);

    pe.setWidthF(1);
    p.setPen(pe);
    QBrush b(QColor("salmon"));
    p.fillRect(QRectF(100,10,50,50), b);
    p.drawRect(QRectF(100,10,50,50));
    QColor c=b.color();
    c.setAlphaF(0.5);
    b.setColor(c);
    pe.setWidthF(5);
    p.setPen(pe);
    p.setBrush(b);
    p.drawEllipse(QRectF(130,40,50,30));
    //p.drawRect(QRectF(130,40,50,30));
    QPolygonF poly;
    for (int phi=0; phi<60; phi++) {
        double p=double(phi)/20.0*M_PI;
        poly<<QPointF(50.0+cos(p)*35.0*double(phi)/60.0, 50.0-sin(p)*35.0*double(phi)/60.0);
    }

    p.setClipRect(0,0,200,50);

    pe.setWidthF(1);
    p.setPen(pe);
    p.drawPolygon(poly);
    pe.setColor("red");
    pe.setStyle(Qt::DashDotDotLine);
    p.setPen(pe);
    p.setClipping(false);
    p.drawPolyline(poly);


    p.end();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //qDebug()<<"create PDF:";
    {
        QCairoPaintDevice * cairoPDF=new QCairoPaintDevice(QSize(200,100), "test.pdf", QCairoPaintDevice::cftPDF);
        //qDebug()<<"  paint PDF";
        paint(cairoPDF);
        //qDebug()<<"  delete PDF";
        delete cairoPDF;
    }

    //qDebug()<<"create PDF+text:";
    {
        QCairoPaintDevice * cairoPDF=new QCairoPaintDevice(QSize(200,100), "testtxt.pdf", QCairoPaintDevice::cftPDF, true);
        //qDebug()<<"  paint PDF+text";
        paint(cairoPDF);
        //qDebug()<<"  delete PDF+text";
        delete cairoPDF;
    }

    //qDebug()<<"create SVG:";
    {
        QCairoPaintDevice * cairoPDF=new QCairoPaintDevice(QSize(200,100), "test.svg", QCairoPaintDevice::cftSVG);
        //qDebug()<<"  paint SVG";
        paint(cairoPDF);
        //qDebug()<<"  delete SVG";
        delete cairoPDF;
    }

    //qDebug()<<"create PS:";
    {
        QCairoPaintDevice * cairoPDF=new QCairoPaintDevice(QSize(200,100), "test.ps", QCairoPaintDevice::cftPS);
        //qDebug()<<"  paint PS";
        paint(cairoPDF);
        //qDebug()<<"  delete PS";
        delete cairoPDF;
    }

    //qDebug()<<"create EPS:";
    {
        QCairoPaintDevice * cairoPDF=new QCairoPaintDevice(QSize(200,100), "test.eps", QCairoPaintDevice::cftEPS);
        //qDebug()<<"  paint EPS";
        paint(cairoPDF);
        //qDebug()<<"  delete EPS";
        delete cairoPDF;
    }




    return 0;
}
