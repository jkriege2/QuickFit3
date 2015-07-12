#include "numberedit.h"
#include <iostream>
#include <QMessageBox>
#include <QKeyEvent>

NumberEdit::NumberEdit(QWidget* parent):
    QLineEdit(parent)
{
    setMinimumHeight(23);
    resize(200,23);
    //validator=new QMyDoubleValidator(this);
    setRange(-10,10);
    setDecimals(2);
    setSingleStep(1.0);
    suffix="";
    //setValidator(validator);
    setValue(0);
    buttonUp=new QToolButton(this);
    buttonDown=new QToolButton(this);
    buttonUp->setIcon(QIcon(":/NumberEdit/btn_up.png"));
    buttonDown->setIcon(QIcon(":/NumberEdit/btn_down.png"));
    int bw=(int)floor((double)height()/2.0);
    buttonUp->resize(bw, bw);
    buttonUp->move(width()-bw, 0);
    buttonUp->setCursor(Qt::ArrowCursor);
    buttonUp->setFocusPolicy(Qt::NoFocus);
    QFont f=buttonUp->font();
    f.setPointSizeF(f.pointSizeF()*0.85);
    buttonUp->setFont(f);
    buttonDown->setFont(f);
    buttonDown->resize(bw, bw+1);
    buttonDown->move(width()-bw, bw-1);
    buttonDown->setCursor(Qt::ArrowCursor);
    buttonDown->setFocusPolicy(Qt::NoFocus);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(texttChanged(const QString&)));
    connect(this, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(cursorChanged(int,int)));
    connect(buttonUp, SIGNAL(clicked()), this, SLOT(stepUp()));
    connect(buttonDown, SIGNAL(clicked()), this, SLOT(stepDown()));
    pal=palette();
    palError=pal;
    palError.setColor(QPalette::Base, QColor("tomato"));
}

NumberEdit::~NumberEdit()
{
    //delete validator;
}

void NumberEdit::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    int bw=(int)floor((double)height()/2.0);
    buttonUp->resize(1.5*bw, bw);
    buttonUp->move(width()-1.5*bw-1, 1);
    buttonDown->resize(1.5*bw, bw);
    buttonDown->move(width()-1.5*bw-1, bw);
}

void NumberEdit::setViewError() {
    setPalette(palError);
}

void NumberEdit::setViewOK() {
    setPalette(pal);
}


// ALSO CARE FOR entf; backspace :::
void NumberEdit::keyPressEvent ( QKeyEvent * event ) {
    if (event->key()==Qt::Key_Backspace
       || event->key()==Qt::Key_Delete) QLineEdit::keyPressEvent(event);
    if (event->modifiers()!=Qt::ControlModifier && event->modifiers()!=Qt::AltModifier) { // do not react when Strg or Alt is pressed
        if (event->text()=="0"
            || event->text()=="1"
            || event->text()=="2"
            || event->text()=="3"
            || event->text()=="4"
            || event->text()=="5"
            || event->text()=="6"
            || event->text()=="7"
            || event->text()=="8"
            || event->text()=="9"
            || event->text()=="E"
            || event->text()=="e") {
            QLineEdit::keyPressEvent(event);
        }
        else if ((event->text()=="." || event->text()==",")/* &&
                 (!(text().contains(".") || text().contains(",")))*/)
        {
            QLocale c=QLocale::system();
            int key=event->key();
            QString text=event->text();
            if (c.decimalPoint()=='.') key=Qt::Key_Period;
            if (c.decimalPoint()==',') key=Qt::Key_Comma;
            QKeyEvent* newEvent=new QKeyEvent(event->type(), key, Qt::NoModifier, text);

            QLineEdit::keyPressEvent(newEvent);

            delete newEvent;
            }
        else if ((event->text()=="-") && (!text().contains("-")) && (min<0) && (cursorPosition()==0)) {
            QLineEdit::keyPressEvent(event);
            }
        else if (event->text().isEmpty()) {
            QLineEdit::keyPressEvent(event);
        }

    } else QLineEdit::keyPressEvent(event);

}


void NumberEdit::texttChanged(const QString& text) {
    QString in=text;
    if (!suffix.isEmpty()) {
        if (text.endsWith(suffix)) {
            in=in.remove(text.size()-suffix.size(), suffix.size());
        }
    }
    double d=extractVal(text);
    setViewOK();
    if ((checkMax) && (d>max)) { d=max; setViewError(); }//setValue(d);}
    if ((checkMin) && (d<min)) { d=min; setViewError(); }//setValue(d); }

    //std::cout<<d<<std::endl;
    //QMessageBox::information(this, "", QString("value is %1").arg(d));
    else emit valueChanged(d);
    if ((!suffix.isEmpty()) && (!text.contains(suffix))) {
        int cp=cursorPosition();
        setText(text+suffix);
        setCursorPosition(cp);
    }
}

double NumberEdit::value() {
    double d=extractVal(text());
    return d;
}

void NumberEdit::setValue(double value) {
    double v=value;
    if (v!=0 && v==extractVal(text())) return;
    if (checkMin) if (v<min) v=min;
    if (checkMax) if (v>max) v=max;
    QString t;
    t.setNum(v, 'f', decimals);
    setText(t+suffix);
    setViewOK();
}

void NumberEdit::stepUp() {
    double d=value();
    d+=singleStep;
    if (checkMax) if (d>max) d=max;
    if (checkMin) if (d<min) d=min;
    setValue(d);
    setViewOK();
}

void NumberEdit::stepDown() {
    double d=value();
    d-=singleStep;
    if (checkMin) if (d<min) d=min;
    if (checkMax) if (d>max) d=max;
    setValue(d);
    setViewOK();
}

void NumberEdit::setSuffix(QString s) {
    suffix=s;
    double num=value();
    //validator->setSuffix(suffix);
    setValue(num);
};

void NumberEdit::cursorChanged ( int old, int newp ) {
    //std::cout<<"old_pos="<<old<<"   new_pos="<<newp<<"   text().size()="<<text().size()<<"   suffix.size()="<<suffix.size()<<std::endl;
    if (newp>text().size()-suffix.size())
        setCursorPosition(text().size()-suffix.size());
}

double NumberEdit::extractVal(QString input) {
    bool ok;
    QString in=input;
    if (!suffix.isEmpty()) {
        if (in.endsWith(suffix)) {
            in=in.remove(input.size()-suffix.size(), suffix.size());
        }
    }
    double d=in.toDouble(&ok);
    while (!ok && in.size()>0) {
        in.remove(in.size()-1,1);
        d=input.toDouble(&ok);
    }
    return d;
}

