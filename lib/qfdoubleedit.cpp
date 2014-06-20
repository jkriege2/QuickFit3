/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfdoubleedit.h"
#include <QKeyEvent>
#include <QFontMetrics>
#include <iostream>
#include <cmath>
#include <QMenu>
#include <QFile>
#include <QDebug>
#include <float.h>
QFDoubleEdit::QFDoubleEdit(QWidget* parent):
    QLineEdit(parent)
{
    QFDoubleEdit_BASIC_REGEXP=QString("(\\+|\\-)?\\d+("+QString(QLocale::system().decimalPoint())+"\\d+)?((e|E)(\\+|\\-)?\\d+)?");
    setMinimumHeight(22);

    m_logscale=false;
    m_logbase=10;
    m_Integer=false;
    m_checkMaximum=true;
    m_maximum=100;
    m_checkMinimum=true;
    m_minimum=-100;
    m_decimals=6;
    m_increment=1;
    m_current=1;
    m_updownKeyEnabled=true;

    m_btnUp=new QToolButton(this);
    m_btnUp->setText("+");
    if (QFile::exists(":/lib/btn_up.png")) {
        m_btnUp->setIcon(QIcon(":/lib/btn_up.png"));
        m_btnUp->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    m_btnDown=new QToolButton(this);
    m_btnDown->setText("-");
    if (QFile::exists(":/lib/btn_down.png")) {
        m_btnDown->setIcon(QIcon(":/lib/btn_down.png"));
        m_btnDown->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    QFont f=font();
    f.setPointSizeF(f.pointSizeF()*4.0/5.0);
    m_btnUp->setFont(f);
    m_btnDown->setFont(f);
    m_btnDown->setCursor(QCursor(Qt::ArrowCursor));
    m_btnUp->setCursor(QCursor(Qt::ArrowCursor));
    m_btnDown->setFocusPolicy(Qt::NoFocus);
    m_btnUp->setFocusPolicy(Qt::NoFocus);
    m_btnDown->setAutoRepeat(true);
    m_btnUp->setAutoRepeat(true);
    connect(m_btnUp, SIGNAL(clicked()), this, SLOT(stepUp()));
    connect(m_btnDown, SIGNAL(clicked()), this, SLOT(stepDown()));

    int btnWidth=ceil((double)height()*3.0/4.0);
    int btnHeight=qMax(22,height())/2;
    m_btnUp->move(width()-btnWidth, 0);
    m_btnUp->resize(btnWidth, btnHeight);
    m_btnDown->move(width()-btnWidth, btnHeight-1);
    m_btnDown->resize(btnWidth, height()-btnHeight);

    setMinimumHeight(22);



    m_regexp=QRegExp(QFDoubleEdit_BASIC_REGEXP, Qt::CaseSensitive, QRegExp::RegExp);
    m_validator=new QRegExpValidator(m_regexp, this);
    setValidator(m_validator);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateWidget(const QString &)));
    m_background=palette().color(QPalette::Base);
    m_errorColor=QColor("darksalmon");
    m_noInt=QColor("peachpuff");
    setValue(0);
    setShowUpDown(true);
}

QFDoubleEdit::~QFDoubleEdit()
{
    //dtor
}

double QFDoubleEdit::value() const {
    bool ok=true;
    double val=text().toDouble(&ok);
    if (!ok) return 0;
    if (m_checkMaximum && (val>m_maximum)) { val=m_maximum; }
    if (m_checkMaximum && (val<m_minimum)) { val=m_minimum; }
    if (m_logscale) {
        if (val<=0) {
            if (m_Integer) val=1;
            else val=DBL_MIN;
        }
    }
    return val;
}

void QFDoubleEdit::setValue(double valueIn) {
    double value=valueIn;
    if (m_Integer) value=round(valueIn);
    if (m_checkMaximum && (value>m_maximum)) { value=m_maximum; }
    if (m_checkMaximum && (value<m_minimum)) { value=m_minimum; }
    if (m_Integer) value=round(valueIn);
    if (m_logscale) {
        if (value<=0) {
            if (m_Integer) value=1;
            else value=DBL_MIN;
        }
    }

    QString txt;
    txt=txt.sprintf(QString("%."+QString::number(m_decimals)+"g").toAscii().data(), value);
    txt.replace('.', QLocale::system().decimalPoint());
    txt.replace(',', QLocale::system().decimalPoint());
    if (txt!=text()) {
        setText(txt);
        if (!hasFocus()) setCursorPosition(0);
    }
}

void QFDoubleEdit::setRange(double min, double max)
{
    setCheckMaximum(true);
    setCheckMinimum(true);
    m_minimum=min;
    m_maximum=max;
    setValue(value());
}

void QFDoubleEdit::stepUp()
{
    stepUp(1);
}

void QFDoubleEdit::stepDown()
{
    stepDown(1);
}

void QFDoubleEdit::stepUp(int steps)
{
    double newVal=value();
    if (m_logscale) {

        for (int j=0; j<steps; j++) {
            double decade=qMin(ceil(log(newVal)/log(m_logbase)), floor(log(newVal)/log(m_logbase)));
            double inc=pow(m_logbase, decade)/m_increment;
            //qDebug()<<newVal<<decade<<inc;
            newVal+=inc;
        }
    } else {
        double i=m_increment*double(steps);
        if (m_Integer) i=round(m_increment);
        newVal+=i;
    }
    setValue(newVal);
    emit valueChanged(value());
}

void QFDoubleEdit::stepDown(int steps)
{
    double newVal=value();
    if (m_logscale) {

        for (int j=0; j<steps; j++) {
            double decade=qMin(ceil(log(newVal)/log(m_logbase)), floor(log(newVal)/log(m_logbase)));
            double inc=pow(m_logbase, decade)/m_increment;
            if (newVal-inc<=0.0 || newVal-inc<=m_minimum) inc=inc/m_logbase;
            //qDebug()<<newVal<<decade<<inc;
            newVal-=inc;
        }
    } else {
        double i=m_increment*double(steps);
        if (m_Integer) i=round(m_increment);
        newVal-=i;
    }
    setValue(newVal);
    emit valueChanged(value());
}

void QFDoubleEdit::updateWidget(const QString & text) {
    double oldvalue=value();
    /* if we have "1,", "1e", "1e-", "-" and are still typing, do not create a signal
       as we can not yet evaluate the input
    */
    if (text.size()>0) {
        if (!text[text.size()-1].isDigit()) return;
        if (text[text.size()-1]==QChar('0') && text.contains(QLocale::system().decimalPoint()) && (!(text.contains('e') || text.contains('E')))) return;
    }
    bool ok=true;
    double val=text.toDouble(&ok);
    if (!ok) return;
    double newVal=val;
    //palette().setColor(QPalette::Base, m_background);
    ok=true;
    if (m_checkMaximum && (val>m_maximum)) { ok=false; newVal=m_maximum; }
    if (m_checkMaximum && (val<m_minimum)) { ok=false; newVal=m_minimum; }
    if (!ok) {
        QPalette p=palette();
        p.setColor(QPalette::Base, m_errorColor);
        setPalette(p);
    } else {
        QPalette p=palette();
        if (m_Integer && (round(val)!=val)) p.setColor(QPalette::Base, m_noInt);
        else p.setColor(QPalette::Base, m_background);
        setPalette(p);

    }
    if (oldvalue!=value()) emit valueChanged(value());
}

void QFDoubleEdit::focusOutEvent ( QFocusEvent * event ) {
    // on focus out, we always correct the input to a meaningful value and emit a signal
    setValue(value());
    emit valueChanged(value());
    emit focusOut(value());
    QLineEdit::focusOutEvent(event);
}

void QFDoubleEdit::keyPressEvent ( QKeyEvent * event )  {
    //std::cout<<"key="<<event->key()<<"   modifiers="<<event->modifiers()<<std::endl;
    if  ((event->modifiers()==Qt::NoModifier)&&((event->key()==44) || (event->key()==46) || (event->key()==Qt::Key_Comma) || (event->key()==Qt::Key_Period))) { // convert '.' and ',' to the current locle's decimal point!
        QKeyEvent key(QEvent::KeyPress, 0, Qt::NoModifier, QString(QLocale::system().decimalPoint()));
        QLineEdit::keyPressEvent(&key);
        event->accept();
    } else if (m_updownKeyEnabled && (event->modifiers()==Qt::NoModifier)&&(event->key()==Qt::Key_Up)) {
        stepUp();
        event->accept();
    } else if (m_updownKeyEnabled && (event->modifiers()==Qt::NoModifier)&&(event->key()==Qt::Key_Down)) {
        stepDown();
        event->accept();
    } else if ((event->modifiers()==Qt::NoModifier)&&(event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter)) {
        emit enterPressed();
        QLineEdit::keyPressEvent(event);
    } else {
        QLineEdit::keyPressEvent(event);
    }

    for (int i=0; i<m_keyEvents.size(); i++) {
        if ((event->key()==m_keyEvents[i].first)&&(event->modifiers()==m_keyEvents[i].second)) emit keyEventMatches(event->key(), event->modifiers());
    }
}

void QFDoubleEdit::setShowUpDown(bool showUpDown) {
    m_showUpDown=showUpDown;
    m_btnUp->setVisible(m_showUpDown);
    m_btnDown->setVisible(m_showUpDown);
}

void QFDoubleEdit::setUpDownKeyEnabled(bool enabled)
{
    m_updownKeyEnabled=enabled;
}

void QFDoubleEdit::setLogScale(bool enabled, double increments)
{
    m_logscale=enabled;
    if (m_logscale) {
        m_increment=increments;
        setValue(value());
    }
}

void QFDoubleEdit::resizeEvent ( QResizeEvent * event ) {
    int btnWidth=ceil((double)event->size().height()*3.0/4.0);
    int btnHeight=ceil((double)qMax(22,event->size().height())/2.0);
    m_btnUp->move(width()-btnWidth, 0);
    m_btnUp->resize(btnWidth, btnHeight+1);
    m_btnDown->move(width()-btnWidth, btnHeight-1);
    m_btnDown->resize(btnWidth, height()-btnHeight);
}

void QFDoubleEdit::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    //qDebug()<<event->delta()<<event->orientation();

    if (event->orientation() == Qt::Vertical) {
        if (numSteps<0) stepDown(-numSteps);
            else stepUp(numSteps);
    }
    event->accept();
}

void QFDoubleEdit::addContextmenuAction(QAction* menuAction) {
    m_contextmenuActions.append(menuAction);
}

void QFDoubleEdit::deleteContextmenuAction(QAction* menuAction) {
    m_contextmenuActions.removeAll(menuAction);
}

void QFDoubleEdit::clearContextmenuActions() {
    m_contextmenuActions.clear();
}

void QFDoubleEdit::contextMenuEvent(QContextMenuEvent* event) {
    QMenu *menu = createStandardContextMenu();
    if (m_contextmenuActions.size()>0) {
        menu->addSeparator();
        for (int i=0; i<m_contextmenuActions.size(); i++) {
            menu->addAction(m_contextmenuActions[i]);
        }
    }
    menu->exec(event->globalPos());
    delete menu;
}

void QFDoubleEdit::addKeyEvent(int key, Qt::KeyboardModifiers modifiers) {
    m_keyEvents.append(qMakePair(key, modifiers));
}

bool QFDoubleEdit::logScale() const
{
    return m_logscale;
}
