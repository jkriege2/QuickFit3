/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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



#include "qmodernprogresswidget.h"

#include <QPainter>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QDebug>
#include <iostream>

QModernProgressWidget::QModernProgressWidget(QWidget* parent):
    QWidget(parent)
{
    //m_pix=QImage(64,64,QImage::Format_ARGB32);
    m_rangeMin=0;
    m_rangeMax=100;
    m_value=50;
    m_outerRadius=0.9;
    m_innerRadius=0.55;
    m_spinInterval=100;
    m_items=15;
    m_doRepaint=true;
    m_spinItem=0;
    m_darkCircleBorder=true;
    m_displayPercent=false;
    m_suffix=QString("%");
    m_precision=0;
    m_percentageMode=QModernProgressWidget::Percent;

    m_backgroundColor=parent->palette().background().color().darker(150);//  QColor("lightgrey");
    m_startColor=m_backgroundColor;//QColor("lightgrey");;
    m_stopColor=QColor("darkslateblue");
    m_indicatorColor=QColor("darkslateblue");
    m_textColor=m_indicatorColor;
    m_nonBackgroundRange=0.75;
    m_spin=false;
    m_mode=QModernProgressWidget::Strokes;

    //timer.setInterval(m_spinInterval);
    //timer.stop();
    //connect(&timer, SIGNAL(timeout()), this, SLOT(doSpin()));

    resize(64,64);
    setSpin(false);
}

QModernProgressWidget::~QModernProgressWidget()
{
    //timer.stop();
    //disconnect(&timer);
}

void QModernProgressWidget::updateWidget() {
    m_doRepaint=true;
    update();
}

void QModernProgressWidget::setSpin(bool enabled) {
    if (m_spin!=enabled) {
        m_spin=enabled;
        if (enabled) {
            //timer.setInterval(m_spinInterval);
            //timer.start();
            m_spinItem=0;
            QTimer::singleShot(m_spinInterval, this, SLOT(doSpin()));
        } else {
            //timer.stop();
        }
    }
}

void QModernProgressWidget::doSpin() {
    /*int newVal=m_value+qMax(1,(m_rangeMax-m_rangeMin)/m_items);
    if (newVal>m_rangeMax) newVal=m_rangeMin+(newVal-m_rangeMax);*/
    m_spinItem++;
    if (m_spinItem>=m_items) m_spinItem=0;
    m_doRepaint=true;
    update();
    if (m_spin) QTimer::singleShot(m_spinInterval, this, SLOT(doSpin()));
}

void QModernProgressWidget::resizeEvent(QResizeEvent* event) {
    m_doRepaint=true;
    QWidget::resizeEvent(event);
}

QColor linColor(QColor col1, QColor col2, double frac) {
    QColor cColor=col1;
    cColor.setRedF(col1.redF()+frac*(col2.redF()-col1.redF()));
    cColor.setGreenF(col1.greenF()+frac*(col2.greenF()-col1.greenF()));
    cColor.setBlueF(col1.blueF()+frac*(col2.blueF()-col1.blueF()));
    return cColor;
}

void QModernProgressWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPixmap m_pix(width(), height());

    //if (m_doRepaint) {
        m_doRepaint=false;
        QPen pen;
        pen.setWidthF(1.0);

        double wid=qMin(height(), width());
        double displayFrac=(double)(m_value-m_rangeMin)/(double)(m_rangeMax-m_rangeMin);
        int currentItem=qMax(0, qMin(m_items, (int)round(displayFrac*(double)m_items)));
        int lastItem=round(m_nonBackgroundRange*(double)m_items);
        if (m_spin) currentItem=m_spinItem;
        if (!m_spin) lastItem=currentItem;
        //if (!m_spin) std::cout<<"m_value="<<m_value<<"   currentItem="<<currentItem<<std::endl;
        QPointF center=QPointF(width()/2.0, height()/2.0);

        double rIn=wid/2.0*m_innerRadius;
        double rOut=wid/2.0*m_outerRadius;
        if (rIn>rOut) qSwap(rIn, rOut);

        //                           2*M_PI
        double strokeWidth=qMax(1.0, 6.283 * rIn/(1.75*(double)m_items));
        double dAngle=360.0/(double)m_items;

        //m_pix.fill(this, 0, 0);
        m_pix.fill(Qt::transparent);

        QPainter painter(&m_pix);

        if (m_mode==QModernProgressWidget::Strokes) {
            QPen strokePen=pen;
            strokePen.setWidthF(strokeWidth);
            strokePen.setCapStyle(Qt::RoundCap);
            strokePen.setJoinStyle(Qt::RoundJoin);
            painter.save();
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.translate(center);
                if (m_spin) painter.rotate(-dAngle*(double)currentItem);
                else painter.rotate(-180);
                for (int i=0; i<m_items; i++) {
                    strokePen.setColor(m_backgroundColor);
                    //double frac=(double)i/(double)(m_items-1.0);

                    if (i<lastItem) {
                        const double r=m_startColor.redF();
                        const double g=m_startColor.greenF();
                        const double b=m_startColor.blueF();
                        const double re=m_stopColor.redF();
                        const double ge=m_stopColor.greenF();
                        const double be=m_stopColor.blueF();
                        double f=(double)i/(m_nonBackgroundRange*(double)m_items);
                        if (f>1) f=1.0;
                        if (f<0) f=0;
                        f=1.0-f;
                        QColor col;
                        col.setRgbF(r+f*(re-r), g+f*(ge-g), b+f*(be-b));
                        strokePen.setColor(col);
                    }
                    //if (fabs(frac-displayFrac)<=1.0/(double)m_items) strokePen.setColor(m_indicatorColor);
                    if (i==0) strokePen.setColor(m_indicatorColor);
                    painter.setPen(strokePen);
                    painter.drawLine(QLineF(0, rIn, 0, rOut));
                    painter.rotate(dAngle);
                }
            painter.restore();
        } else if (m_mode==QModernProgressWidget::Circles) {
            painter.save();
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.translate(center);
                if (m_spin) painter.rotate(-dAngle*(double)currentItem);
                else painter.rotate(-180);
                for (int i=0; i<m_items; i++) {
                    //double frac=(double)i/(double)(m_items-1.0);

                    QColor col=m_backgroundColor;

                    if (i<lastItem) {
                        const double r=m_startColor.redF();
                        const double g=m_startColor.greenF();
                        const double b=m_startColor.blueF();
                        const double re=m_stopColor.redF();
                        const double ge=m_stopColor.greenF();
                        const double be=m_stopColor.blueF();
                        double f=(double)i/(m_nonBackgroundRange*(double)m_items);
                        if (f>1) f=1.0;
                        if (f<0) f=0;
                        f=1.0-f;
                        col.setRgbF(r+f*(re-r), g+f*(ge-g), b+f*(be-b));
                    }
                    //if (fabs(frac-displayFrac)<=1.0/(double)m_items) strokePen.setColor(m_indicatorColor);
                    if (i==0) col=m_indicatorColor;
                    QBrush circleBrush(col);
                    QPen circlePen=QPen(col.darker(150));
                    painter.setBrush(circleBrush);
                    if (m_darkCircleBorder) {
                        painter.setPen(circlePen);
                    } else {
                        painter.setPen(QColor(Qt::transparent));
                    }
                    double d=(rOut-rIn)/2.0;
                    painter.drawEllipse(0, (rIn+rOut)/2.0, d, d);
                    painter.rotate(dAngle);
                }
            painter.restore();
        } else if (m_mode==QModernProgressWidget::GradientRing) {
            painter.save();
                QPainterPath path;
                path.setFillRule( Qt::OddEvenFill );
                path.addEllipse( QPointF(0,0), rOut, rOut );
                path.addEllipse( QPointF(0,0), rIn, rIn );

                painter.translate(center);
                if (m_spin) painter.rotate(-dAngle*(double)currentItem);
                else painter.rotate(-90);

                // Draw the ring background
                painter.setPen( Qt::NoPen );
                painter.setBrush( m_backgroundColor );
                painter.setRenderHint( QPainter::Antialiasing );
                painter.drawPath( path );

                // Draw the ring foreground
                QConicalGradient gradient( QPointF(0,0), 0.0 );
                if (m_spin) {
                    gradient.setColorAt( 0.0, m_backgroundColor );
                    gradient.setColorAt( qMax(0.0,(double)lastItem/(double)m_items-0.05), m_indicatorColor );
                    gradient.setColorAt( (double)lastItem/(double)m_items, m_backgroundColor );
                    painter.setBrush( gradient );
                } else {
                    double gfrac=qMax(0.0, qMin(1.0, 1.0-displayFrac));
                    //std::cout<<gfrac<<std::endl;
                    if ((fabs(gfrac)<=0.02)) {
                        painter.setBrush(QBrush(m_indicatorColor));
                    } else if (fabs(gfrac)>0.85) {
                        /*QColor cColor=m_indicatorColor;
                        cColor.setRedF(cColor.redF()+fabs(gfrac)/0.85*(m_backgroundColor.redF()-cColor.redF()));
                        cColor.setGreenF(cColor.greenF()+fabs(gfrac)/0.85*(m_backgroundColor.greenF()-cColor.greenF()));
                        cColor.setBlueF(cColor.blueF()+fabs(gfrac)/0.85*(m_backgroundColor.blueF()-cColor.blueF()));*/
                        QColor cColor=linColor(m_indicatorColor, m_backgroundColor, (gfrac-0.85)/0.15);
                        gradient.setColorAt( 0.05, m_backgroundColor );
                        gradient.setColorAt( 0.0, cColor );
                        gradient.setColorAt( 0.8, m_backgroundColor );
                        gradient.setColorAt( gfrac, cColor );
                        painter.setBrush( gradient );
                    } else if (fabs(gfrac)<0.15) {
                        /*QColor cColor=m_backgroundColor;
                        cColor.setRedF(m_indicatorColor.redF()-fabs(gfrac)/0.15*(m_indicatorColor.redF()-cColor.redF()));
                        cColor.setGreenF(m_indicatorColor.greenF()-fabs(gfrac)/0.15*(m_indicatorColor.greenF()-cColor.greenF()));
                        cColor.setBlueF(m_indicatorColor.blueF()-fabs(gfrac)/0.15*(m_indicatorColor.blueF()-cColor.blueF()));*/
                        QColor cColor=linColor(m_indicatorColor, m_backgroundColor, gfrac/0.15);
                        gradient.setColorAt( 0.0, m_indicatorColor );
                        gradient.setColorAt( gfrac/2.0, cColor );
                        gradient.setColorAt( gfrac, m_indicatorColor );
                        painter.setBrush( gradient );
                    } else {
                        gradient.setColorAt( 0.0, m_indicatorColor );
                        gradient.setColorAt( 0.05, m_backgroundColor );
                        gradient.setColorAt( qMax(0.0, gfrac-0.05), m_backgroundColor );
                        gradient.setColorAt( qMin(1.0, gfrac), m_indicatorColor );
                        painter.setBrush( gradient );
                    }
                }
                painter.drawPath( path );
            painter.restore();
        }

        if (m_displayPercent && ( (m_mode==QModernProgressWidget::Circles) || (m_mode==QModernProgressWidget::Strokes) || (m_mode==QModernProgressWidget::GradientRing) ) ) {
            painter.save();
            QFont f=font();
            QString s="";
            if (m_percentageMode==QModernProgressWidget::Percent) {
                s=QLocale::system().toString(100.0*(double)(m_value-m_rangeMin)/(double)(m_rangeMax-m_rangeMin), 'f', m_precision)+"%";
            } else {
                s=QLocale::system().toString(m_value, 'f', m_precision)+m_suffix;
            }
            QRect bound=QFontMetrics(f).boundingRect("100%");
            double r=sqrt( (bound.width()/2.0)*(bound.width()/2.0) + (bound.height()/2.0)*(bound.height()/2.0) );
            while ((f.pointSize()>5)&&(r>rIn)) {
                f.setPointSizeF(f.pointSizeF()-0.5);
                bound=QFontMetrics(f).boundingRect("100%");
                r=sqrt( (bound.width()/2.0)*(bound.width()/2.0) + (bound.height()/2.0)*(bound.height()/2.0) );
            }
            bound=QFontMetrics(f).boundingRect(s);
            painter.setFont(f);
            painter.setPen(m_textColor);
            painter.drawText(width()/2-bound.width()/2, height()/2+QFontMetrics(f).ascent()/2, s);
            painter.restore();
        }
    //}
    QPainter p(this);
    p.drawPixmap(QPoint(0,0), m_pix);
}














QModernProgressDialog::QModernProgressDialog(QWidget* parent, Qt::WindowFlags f):
    QDialog(parent, f)
{
    createWidgets();
}

QModernProgressDialog::QModernProgressDialog(const QString & labelText, const QString & cancelButtonText, QWidget* parent, Qt::WindowFlags f):
    QDialog(parent, f)
{
    createWidgets();
    setLabelText(labelText);
    setCancelButtonText(cancelButtonText);
}


QModernProgressDialog::~QModernProgressDialog() {
}

void QModernProgressDialog::createWidgets() {
    setWindowTitle(tr("Progress"));
    setWindowIcon(QIcon());
    m_wasCancel=false;
    label=new QLabel(this);
    progress=new QModernProgressWidget(this);
    progress->setMinimumSize(QSize(48,48));
    cancel=new QPushButton(tr("Cancel"), this);
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));

    QVBoxLayout* vbl=new QVBoxLayout;
    setLayout(vbl);
    QHBoxLayout* hbl=new QHBoxLayout;
    hbl->addWidget(progress);
    hbl->addWidget(label);
    hbl->addStretch();
    vbl->addLayout(hbl);
    hbl=new QHBoxLayout;
    hbl->addStretch();
    hbl->addWidget(cancel);
    vbl->addLayout(hbl);
    connect(&timerDelay, SIGNAL(timeout()), this, SLOT(open()));

    QDesktopWidget desktopWidget;
    QRect desktopRect(desktopWidget.availableGeometry(desktopWidget.primaryScreen()));
    QRect widgetRect=rect();
    move(desktopRect.center() - widgetRect.center());
}



void QModernProgressDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    m_wasCancel=false;
    progress->setSpin(true);

}

void QModernProgressDialog::closeEvent(QCloseEvent* e) {
    timerDelay.stop();
    e->accept();
}

void QModernProgressDialog::openDelayed(int minimumDuration) {
    m_wasCancel=false;
    m_closedBeforeDelay=false;
    if (minimumDuration<=0) {
       open();
    } else {

       //QTimer::singleShot(minimumDuration, this, SLOT(open()));
       timerDelay.stop();
       timerDelay.setInterval(minimumDuration);
       timerDelay.setSingleShot(true);
       timerDelay.start();
    }
}

void QModernProgressDialog::cancelClicked() {
    m_wasCancel=true;
    emit canceled();
}

void QModernProgressDialog::setRange(double min, double max) {
    setMinimum(min);
    setMaximum(max);
}

void QModernProgressDialog::setMinimum(double min) {
    progress->setMinimum(min);
    progress->setDisplayPercent(true);
}

void QModernProgressDialog::setMaximum(double max) {
    progress->setMaximum(max);
    progress->setDisplayPercent(true);
}

void QModernProgressDialog::setMode(bool enabledSpin, bool enabledProgress) {
    progress->setSpin(enabledSpin);
    progress->setDisplayPercent(enabledProgress);
}

void QModernProgressDialog::setValue(double value) {
    progress->setValue(value);
    progress->setDisplayPercent(true);
}

