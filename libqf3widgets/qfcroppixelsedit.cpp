#include "qfcroppixelsedit.h"

QFCropPixelsEdit::QFCropPixelsEdit(QWidget *parent) : QWidget(parent)
{
    lay=new QHBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    spinX1=new QSpinBox(this);
    spinX2=new QSpinBox(this);
    spinY1=new QSpinBox(this);
    spinY2=new QSpinBox(this);

    lay->addWidget(new QLabel("x = ", this));
    lay->addWidget(spinX1);
    lay->addWidget(new QLabel(" ... ", this));
    lay->addWidget(spinX2);
    lay->addWidget(new QLabel("   y = ", this));
    lay->addWidget(spinY1);
    lay->addWidget(new QLabel(" ... ", this));
    lay->addWidget(spinY2);


    setImageSize(0,1);
    setFullImageSize();
    connectWidgets(true);
    setLayout(lay);

}

int QFCropPixelsEdit::getX1() const
{
    return spinX1->value();
}

int QFCropPixelsEdit::getX2() const
{
    return spinX2->value();
}

int QFCropPixelsEdit::getY1() const
{
    return spinY1->value();
}

int QFCropPixelsEdit::getY2() const
{
    return spinY2->value();
}

int QFCropPixelsEdit::getWIDTH() const
{
    return getX2()-getX1()+1;
}

int QFCropPixelsEdit::getHEIGHT() const
{
    return getY2()-getY1()+1;
}

void QFCropPixelsEdit::addLayoutStretchAtEnd()
{
    lay->addStretch();
}

void QFCropPixelsEdit::addWidget(QWidget *widget)
{
    lay->addWidget(widget);
}

void QFCropPixelsEdit::spinValueChanged()
{
    connectWidgets(false);
    if (spinX2->value()<=spinX1->value()) {
        spinX1->setValue(spinX2->value()+1);
    }
    if (spinY2->value()<=spinY1->value()) {
        spinY1->setValue(spinY2->value()+1);
    }
    connectWidgets(true);
    emit valueChanged(getX1(), getX2(), getY1(), getY2());
}

void QFCropPixelsEdit::setImageSize(int width, int height)
{
    spinX1->setRange(0, width-1);
    spinX2->setRange(0, width-1);
    spinY1->setRange(0, height-1);
    spinY2->setRange(0, height-1);
}

void QFCropPixelsEdit::setCropRange(int x1, int x2, int y1, int y2)
{
    connectWidgets(false);
    if (x1<=x2) {
        spinX1->setValue(x1);
        spinX2->setValue(x2);
    } else {
        spinX1->setValue(x2);
        spinX2->setValue(x1);
    }
    if (y1<=y2) {
        spinY1->setValue(y1);
        spinY2->setValue(y2);
    } else {
        spinY1->setValue(y2);
        spinY2->setValue(y1);
    }
    connectWidgets(true);
}

void QFCropPixelsEdit::setFullImageSize()
{
    setCropRange(0,spinX2->maximum(), 0, spinY2->maximum());
}

void QFCropPixelsEdit::connectWidgets(bool connected)
{
    if (connected){
        connect(spinX1, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
        connect(spinX2, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
        connect(spinY1, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
        connect(spinY2, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
    } else {
        disconnect(spinX1, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
        disconnect(spinX2, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
        disconnect(spinY1, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
        disconnect(spinY2, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
    }

}

