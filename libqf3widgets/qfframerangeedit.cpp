#include "qfframerangeedit.h"

QFFrameRangeEdit::QFFrameRangeEdit(QWidget *parent) : QWidget(parent)
{
    lay=new QHBoxLayout();
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(new QLabel("from:  ", this));

    chkFirst=new QCheckBox(tr("first "), this);
    chkFirst->setChecked(true);
    lay->addWidget(chkFirst);
    spinFirst=new QSpinBox(this);
    spinFirst->setRange(0,1000000);
    spinFirst->setValue(0);
    spinFirst->setEnabled(false);
    lay->addWidget(spinFirst);
    lay->addWidget(new QLabel("  to:  ", this));
    chkLast=new QCheckBox(tr("last "), this);
    chkLast->setChecked(true);
    lay->addWidget(chkLast);
    spinLast=new QSpinBox(this);
    spinLast->setRange(0,1000000);
    spinLast->setValue(100);
    spinLast->setEnabled(false);
    lay->addWidget(spinLast);
    connect(spinLast, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
    connect(spinFirst, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
    connect(chkLast, SIGNAL(toggled(bool)), this, SLOT(spinValueChanged()));
    connect(chkFirst, SIGNAL(toggled(bool)), this, SLOT(spinValueChanged()));
    connect(chkLast, SIGNAL(toggled(bool)), this, SLOT(checkChanged()));
    connect(chkFirst, SIGNAL(toggled(bool)), this, SLOT(checkChanged()));
    setLayout(lay);
}

QFFrameRangeEdit::~QFFrameRangeEdit()
{

}


int QFFrameRangeEdit::getFirst() const
{
    return spinFirst->value();
}

int QFFrameRangeEdit::getLast() const
{
    return spinLast->value();
}

void QFFrameRangeEdit::addLayoutStretchAtEnd()
{
    lay->addStretch();
}

void QFFrameRangeEdit::setFirstLastText(const QString &first, const QString &last)
{
    chkFirst->setText(first);
    chkLast->setText(last);
}

void QFFrameRangeEdit::setPixelSize(double width_and_height)
{
    setPixelSize(QSizeF(width_and_height, width_and_height));
}

void QFFrameRangeEdit::setPixelSize(QSizeF size)
{
    spinFirst->setValue(size.width());
    spinLast->setValue(size.height());
}

void QFFrameRangeEdit::setPixelSize(double width, double height)
{
    setPixelSize(QSizeF(width, height));
}

void QFFrameRangeEdit::setUnits(const QString &unitname)
{
    spinFirst->setSuffix(unitname);
    spinLast->setSuffix(unitname);
}

void QFFrameRangeEdit::setRange(int minRange, int maxRange)
{
    spinFirst->setRange(minRange, maxRange);
    spinLast->setRange(minRange, maxRange);
    if (chkFirst->isChecked()) spinFirst->setValue(0);
    if (chkLast->isChecked()) spinLast->setValue(maxRange);
    checkChanged();
}




void QFFrameRangeEdit::checkChanged()
{
    spinFirst->setEnabled(!chkFirst->isChecked());
    spinLast->setEnabled(!chkLast->isChecked());
    if (chkFirst->isChecked() && spinFirst->value()!=spinFirst->minimum()) {
        spinFirst->setValue(spinFirst->minimum());
    }
    if (chkLast->isChecked() && spinLast->value()!=spinLast->maximum()) {
        spinLast->setValue(spinLast->maximum());
    }
    spinValueChanged();
}



void QFFrameRangeEdit::spinValueChanged()
{
    int nv=qMax(spinFirst->value(), spinLast->value());
    if (spinLast->value()!=nv) spinLast->setValue(nv);
    emit valueChanged(getFirst(), getLast());

}
