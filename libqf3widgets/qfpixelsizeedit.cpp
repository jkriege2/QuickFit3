#include "qfpixelsizeedit.h"

QFPixelSizeEdit::QFPixelSizeEdit(QWidget *parent) : QWidget(parent)
{
    lay=new QHBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    spinWidth=new QDoubleSpinBox(this);
    spinWidth->setRange(0,100000);
    spinWidth->setSuffix(" nm");
    spinWidth->setValue(400);
    spinWidth->setDecimals(2);
    lay->addWidget(spinWidth);
    lay->addWidget(new QLabel("  X  ", this));
    spinHeight=new QDoubleSpinBox(this);
    spinHeight->setRange(0,100000);
    spinHeight->setSuffix(" nm");
    spinHeight->setValue(400);
    spinHeight->setDecimals(2);
    lay->addWidget(spinHeight);
    connect(spinHeight, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
    connect(spinWidth, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged()));
    setLayout(lay);
}

QFPixelSizeEdit::~QFPixelSizeEdit()
{

}

QSizeF QFPixelSizeEdit::getPixelSize() const
{
    return QSizeF(spinWidth->value(), spinHeight->value());
}

double QFPixelSizeEdit::getPixelWidth() const
{
    return getPixelSize().width();
}

double QFPixelSizeEdit::getPixelHeight() const
{
    return getPixelSize().height();
}

void QFPixelSizeEdit::addLayoutStretchAtEnd()
{
    lay->addStretch();
}

void QFPixelSizeEdit::addWidget(QWidget *widget)
{
    lay->addWidget(widget);
}

void QFPixelSizeEdit::setPixelSize(double width_and_height)
{
    setPixelSize(QSizeF(width_and_height, width_and_height));
}

void QFPixelSizeEdit::setPixelSize(QSizeF size)
{
    spinWidth->setValue(size.width());
    spinHeight->setValue(size.height());
}

void QFPixelSizeEdit::setPixelSize(double width, double height)
{
    setPixelSize(QSizeF(width, height));
}

void QFPixelSizeEdit::setUnits(const QString &unitname)
{
    spinWidth->setSuffix(unitname);
    spinHeight->setSuffix(unitname);
}

void QFPixelSizeEdit::setRange(double minWidth, double maxWidth)
{
    spinWidth->setRange(minWidth, maxWidth);
    spinHeight->setRange(minWidth, maxWidth);
}

void QFPixelSizeEdit::setDecimals(int dec)
{
    spinWidth->setDecimals(dec);
    spinHeight->setDecimals(dec);
}

void QFPixelSizeEdit::spinValueChanged()
{
    emit valueChanged(getPixelSize());
}

