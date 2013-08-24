#include "qfdoubleslider.h"
#include <cmath>

QFDoubleSlider::QFDoubleSlider(QWidget *parent) :
    QSlider(parent)
{
    d_min=0;
    d_max=100;
    m_value=0;
    m_steps=1000;
    setRange(0,m_steps);
    setValue(0);
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(dsliderMovedSlot(int)));
}

double QFDoubleSlider::dvalue() const
{
    return m_value;
}

void QFDoubleSlider::setDRange(double min, double max)
{
    d_min=min;
    d_max=max;
    setDValue(m_value);
}

void QFDoubleSlider::setDValue(double value)
{
    if (value==m_value) return;
    m_value=qBound(d_min, value, d_max);
    disconnect(this, SIGNAL(sliderMoved(int)), this, SLOT(dsliderMovedSlot(int)));
    setValue(round((m_value-d_min)/(d_max-d_min)*double(m_steps)));
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(dsliderMovedSlot(int)));
    emit dsliderMoved(m_value);
}

void QFDoubleSlider::setDMinimum(double min)
{
    setDRange(min, d_max);
}

void QFDoubleSlider::setDMaximum(double max)
{
    setDRange(d_min, max);
}

void QFDoubleSlider::dsliderMovedSlot(int pos)
{
    double rel=double(pos-minimum())/double(maximum()-minimum());
    m_value=d_min+rel*(d_max-d_min);
    emit dsliderMoved(m_value);
}

void QFDoubleSlider::sliderChange(QAbstractSlider::SliderChange change)
{
    QSlider::sliderChange(change);
}
