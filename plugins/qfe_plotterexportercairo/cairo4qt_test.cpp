#include <QCoreApplication>
#include "cairo4qt/src/qpaintengine_cairo_p.h"
#include "cairo4qt/src/qcairopaintdevice.h"
#include <QPainter>
#include <QImage>

void paint(QPaintDevice* pd) {
    QPainter p;
    p.begin(pd);
    p.drawLine(QPointF(0,0),QPointF(100.0,100.0));
    p.setPen(Qt::red);
    p.drawLine(QPointF(0,0),QPointF(50.0,25.0));
    p.setPen(Qt::green);
    p.drawLine(QPointF(0,0),QPointF(25.0,50.0));
    p.setPen(Qt::blue);
    //p.drawText(50,10, QString("Hello World!"));
    QFont f;
    QImage img(QString("qfe_plotterexportercairo.png"));
    p.drawImage(QRectF(50,25,50,50), img);
    p.drawImage(QRectF(25,50,15,10), img);
    f.setFamily("Times new Roman");
    f.setPointSizeF(10);
    //p.setFont(f);
    //p.drawText(50,30, QString("Hello World!"));
    p.end();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    {
        QCairoPaintDevice * cairoPDF=new QCairoPaintDevice(QSize(200,100), "test.pdf", QCairoPaintDevice::cftPDF);
        paint(cairoPDF);
        delete cairoPDF;
    }

    {
        QCairoPaintDevice * cairoPDF2=new QCairoPaintDevice(QSize(100,100), "test2.pdf", QCairoPaintDevice::cftPDF);
        paint(cairoPDF2);
        delete cairoPDF2;
    }

    {
        QCairoPaintDevice * cairoSVG=new QCairoPaintDevice(QSize(100,100), "test.svg", QCairoPaintDevice::cftSVG);
        paint(cairoSVG);
        delete cairoSVG;
    }

    {
        QCairoPaintDevice * cairoPS=new QCairoPaintDevice(QSize(100,100), "test.ps", QCairoPaintDevice::cftPS2);
        paint(cairoPS);
        delete cairoPS;
    }

    {
        QCairoPaintDevice * cairoEPS=new QCairoPaintDevice(QSize(100,100), "test.eps", QCairoPaintDevice::cftEPS2);
        paint(cairoEPS);
        delete cairoEPS;
    }



    return 0;
}
