#include "qvisiblehandlesplitter.h"

#include <QtGui>
#include <QtCore>

QVisibleGripSplitterHandle::QVisibleGripSplitterHandle(Qt::Orientation orientation, QSplitter * parent) :
    QSplitterHandle(orientation, parent)
{
    m_gripHeight=96;
    m_inside=false;
    m_lightup=true;
    m_drawGrip=true;
    setMouseTracking(true);
}


void QVisibleGripSplitterHandle::paintEvent(QPaintEvent *event)
{

    QBrush back=QBrush(palette().brush(QPalette::Light));
    QPen lines=palette().color(QPalette::Mid);
    if (m_lightup && m_inside) {
        QPainter painter(this);
        back= QBrush(palette().brush(QPalette::Highlight));
        painter.fillRect(event->rect(), back);
        back.setColor(back.color().lighter());
        lines.setColor(back.color().darker());
    } else {
        //painter.fillRect(event->rect(), QBrush(palette().brush(QPalette::Window)));
        QSplitterHandle::paintEvent(event);
    }
    QPainter painter(this);
    if (m_drawGrip) {
        if (orientation() == Qt::Horizontal) {// WIDGET | WIDGET | WIDGET
            QRect r=event->rect();
            QRect grip=QRect(r.left(), r.center().y()-m_gripHeight/2, r.width(), m_gripHeight);
            painter.fillRect(grip, back);
            painter.setPen(lines);
            for (int i=grip.top(); i<grip.bottom(); i+=2) {
                painter.drawLine(grip.left(), i, grip.right(), i);
            }
        } else {
            // WIDGET
            // ------
            // WIDGET
            // ------
            // WIDGET
            QRect r=event->rect();
            QRect grip=QRect(r.center().x()-m_gripHeight/2, r.top(), m_gripHeight, r.height());
            painter.fillRect(grip, back);
            painter.setPen(lines);
            for (int i=grip.left(); i<grip.right(); i+=2) {
                painter.drawLine(i, grip.top(), i, grip.bottom());
            }
        }
    }
}

void QVisibleGripSplitterHandle::leaveEvent(QEvent* event) {
    m_inside=false;
    repaint();
}

void QVisibleGripSplitterHandle::enterEvent(QEvent* event) {
    m_inside=true;
    repaint();
}







QVisibleHandleSplitter::QVisibleHandleSplitter(Qt::Orientation orientation, QWidget *parent) :
    QSplitter(orientation, parent)
{
    setChildrenCollapsible(false);
}

QVisibleHandleSplitter::QVisibleHandleSplitter(QWidget *parent) :
    QSplitter(parent)
{
}

QSplitterHandle* QVisibleHandleSplitter::createHandle() {
    return new QVisibleGripSplitterHandle(orientation(), this);
}

void QVisibleHandleSplitter::setDecorated(bool enabled) {
    for (int i=0;i<count(); i++) {
        QVisibleGripSplitterHandle* h=qobject_cast<QVisibleGripSplitterHandle*>(handle(i));
        if (h) h->setDecorated(enabled);
    }
}

void QVisibleHandleSplitter::setHoverEffect(bool enabled) {
    for (int i=0;i<count(); i++) {
        QVisibleGripSplitterHandle* h=qobject_cast<QVisibleGripSplitterHandle*>(handle(i));
        if (h) h->setHoverEffect(enabled);
    }
}
void QVisibleHandleSplitter::setGripWidth(int width) {
    for (int i=0;i<count(); i++) {
        QVisibleGripSplitterHandle* h=qobject_cast<QVisibleGripSplitterHandle*>(handle(i));
        if (h) h->setGripWidth(width);
    }
}
