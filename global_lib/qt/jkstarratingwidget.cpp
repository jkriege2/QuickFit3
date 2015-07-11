#include "jkstarratingwidget.h"
#include <QPainter>
#include <iostream>

JKStarRatingWidget::JKStarRatingWidget(QWidget* parent):
    QFrame(parent)
{
    starImage=    QPixmap(":/jkstarratingwidget/star.png");
    darkStarImage=QPixmap(":/jkstarratingwidget/darkstar.png");
    noStarImage=  QPixmap(":/jkstarratingwidget/nostar.png");
    m_rating=0;
    m_maximum=5;
    setWidgetSizes();
    setFocusPolicy(Qt::ClickFocus);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(1);
    setMidLineWidth(0);
}

JKStarRatingWidget::~JKStarRatingWidget()
{
    //dtor
}

int JKStarRatingWidget::rating() {
    return m_rating;
}

void JKStarRatingWidget::setRating(int value) {
    m_rating=value;
    if (value<0) m_rating=0;
    if (value>m_maximum) m_rating=m_maximum;
    setWidgetSizes();
    repaint();
}

int JKStarRatingWidget::maximum() {
    return m_maximum;
}

void JKStarRatingWidget::setMaximum(int value) {
    m_maximum=value;
    setWidgetSizes();
    repaint();
}

void JKStarRatingWidget::setWidgetSizes() {
    QSize s((starImage.width()+2)*m_maximum+noStarImage.width()+4+2*frameWidth(), qMax(starImage.height(), noStarImage.height())+4+2*frameWidth());
    setMaximumSize(s);
    setMinimumSize(s);
    resize(s);
}

void JKStarRatingWidget::paintEvent(QPaintEvent * event) {
    QFrame::paintEvent(event);
    QPainter painter(this);
    int startx=2+frameWidth();
    int starty=2+frameWidth();
    painter.drawPixmap(startx, starty,noStarImage);
    int x=noStarImage.width()+startx;
    //QPen p(palette().color(QPalette::Dark));
    //painter.setPen(p);
    //painter.drawRect(0,0,width()-1, height()-1);
    for (int i=1; i<=m_maximum; i++) {
        if (m_rating>=i) painter.drawPixmap(x+1,starty,starImage);
        else painter.drawPixmap(x+1,starty,darkStarImage);
        x=x+starImage.width()+2;
    }
    /*if (hasFocus()) {
        p.setStyle(Qt::DotLine);
        p.setColor(palette().color(QPalette::Light));
        painter.setPen(p);
        painter.drawRect(0,0,width()-1, height()-1);
    }*/
}

void JKStarRatingWidget::mouseReleaseEvent(QMouseEvent * event) {
    if (event->button()==Qt::LeftButton) {
        int n_rating=0;
        int startx=2+frameWidth();
        int starty=2+frameWidth();
        //std::cout<<"button(x="<<event->x()<<", "<<event->y()<<")\n";
        if (event->x()<=noStarImage.width()+starty) n_rating=0;
        else {
            int x=noStarImage.width()+starty;//+starImage.width()+1;
            n_rating=(event->x()-x)/(starImage.width()+2)+1;
        }
        event->accept();
        //std::cout<<"new rating="<<n_rating<<"\n";
        setRating(n_rating);
    }

}


void JKStarRatingWidget::setFrameStyle(int value) {
    QFrame::setFrameStyle(value);
    setWidgetSizes();
    repaint();
}

void JKStarRatingWidget::setFrameShape(Shape value) {
    QFrame::setFrameShape(value);
    setWidgetSizes();
    repaint();
}

void JKStarRatingWidget::setFrameShadow(Shadow value) {
    QFrame::setFrameShadow(value);
    setWidgetSizes();
    repaint();
}

void JKStarRatingWidget::setLineWidth(int value) {
    QFrame::setLineWidth(value);
    setWidgetSizes();
    repaint();
}

void JKStarRatingWidget::setMidLineWidth(int value) {
    QFrame::setMidLineWidth(value);
    setWidgetSizes();
    repaint();
}

void JKStarRatingWidget::setFrameRect(const QRect & value) {
    QFrame::setFrameRect(value);
    setWidgetSizes();
    repaint();
}



