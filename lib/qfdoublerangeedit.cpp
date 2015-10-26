#include "qfdoublerangeedit.h"

QFDoubleRangeEdit::QFDoubleRangeEdit(QWidget *parent) : QWidget(parent)
{
    lay=new QHBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    spinMin=new QFDoubleEdit(this);
    spinMin->setValue(0);
    spinMin->setRange(0,100);
    spinMin->setDecimals(8);
    spinMin->setCheckBounds(false, false);
    lay->addWidget(spinMin);
    lay->addWidget(new QLabel(" ... ", this));
    spinMax=new QFDoubleEdit(this);
    spinMax->setRange(0,100);
    spinMax->setValue(100);
    spinMax->setDecimals(8);
    spinMax->setCheckBounds(false, false);
    lay->addWidget(spinMax);
    lay->addWidget(labUnit=new QLabel(this));
    connect(spinMax, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
    connect(spinMin, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
    setLayout(lay);
}

QFDoubleRangeEdit::~QFDoubleRangeEdit()
{

}

double QFDoubleRangeEdit::getMin() const
{
    return spinMin->value();
}

double QFDoubleRangeEdit::getMax() const
{
    return spinMax->value();
}

void QFDoubleRangeEdit::addLayoutStretchAtEnd()
{
    lay->addStretch();
}

void QFDoubleRangeEdit::addWidget(QWidget *widget)
{
    lay->addWidget(widget);
}

void QFDoubleRangeEdit::setAbsRange(double min, double max)
{
    spinMin->setRange(min, max);
    spinMin->setCheckBounds(true, true);
    spinMax->setRange(min, max);
    spinMax->setCheckBounds(true, true);
    spinValueChanged();
}

void QFDoubleRangeEdit::setValues(double min, double max)
{
    spinMin->setValue(min);
    spinMax->setValue(max);
    spinValueChanged();
}

void QFDoubleRangeEdit::setNoAbsRange()
{
    spinMin->setCheckBounds(false, false);
    spinMax->setCheckBounds(false, false);
}

void QFDoubleRangeEdit::setSuffix(const QString &unitname)
{
    labUnit->setIndent(unitname.isEmpty()?0:5);
    labUnit->setText(unitname);
}

void QFDoubleRangeEdit::setDecimals(int dec)
{
    spinMin->setDecimals(dec);
    spinMax->setDecimals(dec);
}

void QFDoubleRangeEdit::spinValueChanged()
{
    if (spinMin->checkMinimum()) {
        disconnect(spinMax, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
        disconnect(spinMin, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
        double vmin=getMin();
        double vmax=getMax();
        if (vmin>vmax) {
            spinMin->setValue(vmax);
        }
        connect(spinMax, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
        connect(spinMin, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
    }
    emit valueChanged(getMin(), getMax());
}

