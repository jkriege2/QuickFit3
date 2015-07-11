#include "jkdoubleedit.h"
#include <QKeyEvent>
#include <QFontMetrics>
#include <iostream>
#include <cmath>
#include <QMenu>
#include <QFile>

JKDoubleEdit::JKDoubleEdit(QWidget* parent):
    QLineEdit(parent)
{
    JKDoubleEdit_BASIC_REGEXP=QString("(\\+|\\-)?\\d+("+QString(QLocale::system().decimalPoint())+"\\d+)?((e|E)(\\+|\\-)?\\d+)?");
    setMinimumHeight(22);

    m_Integer=false;
    m_checkMaximum=true;
    m_maximum=100;
    m_checkMinimum=true;
    m_minimum=-100;
    m_decimals=6;
    m_increment=true;
    m_current=1;

    m_btnUp=new QToolButton(this);
    m_btnUp->setText("+");
    if (QFile::exists(":/lib/qt/dblbtn_up.png")) {
        m_btnUp->setIcon(QIcon(":/lib/qt/dblbtn_up.png"));
        m_btnUp->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    m_btnDown=new QToolButton(this);
    m_btnDown->setText("-");
    if (QFile::exists(":/lib/qt/dblbtn_down.png")) {
        m_btnDown->setIcon(QIcon(":/lib/qt/dblbtn_down.png"));
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



    m_regexp=QRegExp(JKDoubleEdit_BASIC_REGEXP, Qt::CaseSensitive, QRegExp::RegExp);
    m_validator=new QRegExpValidator(m_regexp, this);
    setValidator(m_validator);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateWidget(const QString &)));
    m_background=palette().color(QPalette::Base);
    m_errorColor=QColor("darksalmon");
    m_noInt=QColor("peachpuff");
    setValue(0);
    setShowUpDown(true);
}

JKDoubleEdit::~JKDoubleEdit()
{
    //dtor
}

double JKDoubleEdit::value() const {
    bool ok=true;
    double val=text().toDouble(&ok);
    if (!ok) return 0;
    if (m_checkMaximum && (val>m_maximum)) { val=m_maximum; }
    if (m_checkMaximum && (val<m_minimum)) { val=m_minimum; }
    return val;
}

void JKDoubleEdit::setValue(double valueIn) {
    double value=valueIn;
    if (m_Integer) value=round(valueIn);
    if (m_checkMaximum && (value>m_maximum)) { value=m_maximum; }
    if (m_checkMaximum && (value<m_minimum)) { value=m_minimum; }
    if (m_Integer) value=round(valueIn);

    QString txt;
    txt=txt.sprintf(QString("%."+QString::number(m_decimals)+"g").toAscii().data(), value);
    txt.replace('.', QLocale::system().decimalPoint());
    txt.replace(',', QLocale::system().decimalPoint());
    if (txt!=text()) {
        setText(txt);
        if (!hasFocus()) setCursorPosition(0);
    }
}

void JKDoubleEdit::updateWidget(const QString & text) {
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

void JKDoubleEdit::focusOutEvent ( QFocusEvent * event ) {
    // on focus out, we always correct the input to a meaningful value and emit a signal
    setValue(value());
    emit valueChanged(value());
    emit focusOut(value());
    QLineEdit::focusOutEvent(event);
}

void JKDoubleEdit::keyPressEvent ( QKeyEvent * event )  {
    //std::cout<<"key="<<event->key()<<"   modifiers="<<event->modifiers()<<std::endl;
    if  ((event->modifiers()==Qt::NoModifier)&&((event->key()==44) || (event->key()==46))) { // convert '.' and ',' to the current locle's decimal point!
        QKeyEvent key(QEvent::KeyPress, 0, Qt::NoModifier, QString(QLocale::system().decimalPoint()));
        QLineEdit::keyPressEvent(&key);
        event->accept();
    } else if ((event->modifiers()==Qt::NoModifier)&&(event->key()==Qt::Key_Up)) {
        stepUp();
        event->accept();
    } else if ((event->modifiers()==Qt::NoModifier)&&(event->key()==Qt::Key_Down)) {
        stepDown();
        event->accept();
    } else {
        QLineEdit::keyPressEvent(event);
    }

    for (int i=0; i<m_keyEvents.size(); i++) {
        if ((event->key()==m_keyEvents[i].first)&&(event->modifiers()==m_keyEvents[i].second)) emit keyEventMatches(event->key(), event->modifiers());
    }
}

void JKDoubleEdit::setShowUpDown(bool showUpDown) {
    m_showUpDown=showUpDown;
    m_btnUp->setVisible(m_showUpDown);
    m_btnDown->setVisible(m_showUpDown);
}

void JKDoubleEdit::resizeEvent ( QResizeEvent * event ) {
    int btnWidth=ceil((double)event->size().height()*3.0/4.0);
    int btnHeight=ceil((double)qMax(22,event->size().height())/2.0);
    m_btnUp->move(width()-btnWidth, 0);
    m_btnUp->resize(btnWidth, btnHeight+1);
    m_btnDown->move(width()-btnWidth, btnHeight-1);
    m_btnDown->resize(btnWidth, height()-btnHeight);
}

void JKDoubleEdit::addContextmenuAction(QAction* menuAction) {
    m_contextmenuActions.append(menuAction);
}

void JKDoubleEdit::deleteContextmenuAction(QAction* menuAction) {
    m_contextmenuActions.removeAll(menuAction);
}

void JKDoubleEdit::clearContextmenuActions() {
    m_contextmenuActions.clear();
}

void JKDoubleEdit::contextMenuEvent(QContextMenuEvent* event) {
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

void JKDoubleEdit::addKeyEvent(int key, Qt::KeyboardModifiers modifiers) {
    m_keyEvents.append(qMakePair(key, modifiers));
}
