#include "qfenhancedlineedit.h"
#include <QtGui>

#include <iostream>

QFEnhancedLineEdit::QFEnhancedLineEdit(QWidget* parent):
    QLineEdit(parent)
{
    m_buttonDistance=1;
    m_historyposition=-1;
    setContextMenuPolicy(Qt::DefaultContextMenu);
    //setMinimumHeight(minimumHeight()+2);

}

QFEnhancedLineEdit::~QFEnhancedLineEdit()
{
    //dtor
}

void QFEnhancedLineEdit::focusOutEvent(QFocusEvent* event) {
    if (completer()) {
        completer()->model()->insertRow(completer()->model()->rowCount());
        completer()->model()->setData(completer()->model()->index(completer()->model()->rowCount()-1,0), this->text());
    }
    history.append(text());
    QLineEdit::focusOutEvent(event);
}


void QFEnhancedLineEdit::resizeEvent ( QResizeEvent * event ) {
    QLineEdit::resizeEvent(event);
    invalidateButtons();
}

void QFEnhancedLineEdit::contextMenuEvent(QContextMenuEvent *event) {
    QMenu* menu=createStandardContextMenu();

    QList<QAction*> acts=actions();
    if (acts.size()>0) {
        menu->addSeparator();
        for (int i=0; i<acts.size(); i++) {
            menu->addAction(acts[i]);
        }
    }
    menu->exec(event->globalPos());
    delete menu;
}

void QFEnhancedLineEdit::invalidateButtons() {
    QRect r = rect();
    QPalette pal = palette();
    QMargins m=textMargins();

    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
    setTextMargins(m.left(), m.top(), m.left()+getButtonsWidth(buttons.size()), m.bottom());

    QSize s=size();
    for (int i=0; i<buttons.size(); i++) {
        buttons[i]->resize(QSize(buttons[i]->minimumSizeHint().width(), qMax(buttons[i]->minimumSizeHint().height(), s.height())));
        //std::cout<<"resize("<<buttons[i]->minimumSizeHint().width()<<", "<<s.height()<<")\n";
    }
    moveButtons();
}

void QFEnhancedLineEdit::insertActTriggered() {
    QAction* act=qobject_cast<QAction*>(sender());
    if (act) {
        insert(act->data().toString());
    }
}

void QFEnhancedLineEdit::moveButtons() {
    QRect r = rect();
    QPalette pal = palette();
    QMargins m=textMargins();

    QStyleOptionFrameV2 panel;
    initStyleOption(&panel);
    r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

    int x=width();//r.right();
    for (int i=0; i<buttons.size(); i++) {
        QAbstractButton* b=buttons[i];
        b->move(x-b->width(), (double)(height()-b->height())/2.0);
        x=x-b->width()-m_buttonDistance;
    }
}

void QFEnhancedLineEdit::addButton(QAbstractButton* button) {
    if (!buttons.contains(button)) {
        QCursor c=button->cursor();
        buttons.append(button);
        button->setParent(this);
        button->setCursor(c);
        button->setFocusPolicy(Qt::NoFocus);

        invalidateButtons();
    }
}

QAbstractButton* QFEnhancedLineEdit::getButton(int i) {
    if (i<0) return NULL;
    if (i>=buttons.size()) return NULL;
    return buttons[i];
}

int QFEnhancedLineEdit::getButtonCount() {
    return buttons.size();
}

void QFEnhancedLineEdit::removeButton(int i) {
    if (i<0) return;
    if (i>=buttons.size()) return;
    buttons.removeAt(i);
    invalidateButtons();
}

void QFEnhancedLineEdit::removeButton(QAbstractButton* button) {
    buttons.removeAll(button);
    invalidateButtons();
}

int QFEnhancedLineEdit::getButtonsWidth(int i) {
    int w=0;
    for (int j=0; j<i; j++) {
        w=w+buttons[j]->width()+m_buttonDistance;
    }
    return w;
}

void QFEnhancedLineEdit::setButtonDistance(int d) {
    m_buttonDistance=d;
    invalidateButtons();
}

int QFEnhancedLineEdit::buttonDistance() {
    return m_buttonDistance;
}

void QFEnhancedLineEdit::addContextMenuEntry(const QString &name, QVariant data) {
    addContextMenuEntry(QIcon(), name, data);
}

void QFEnhancedLineEdit::addContextMenuEntry(const QIcon& icon, const QString &name, QVariant data) {
    QAction* act=new QAction(icon, name, this);
    act->setData(data);
    addAction(act);
    contextmenuActions.append(act);
}


void QFEnhancedLineEdit::addContextMenuEntry(const QString &name, const QObject *receiver, const char *method) {
    addContextMenuEntry(QIcon(), name, receiver, method);
}

void QFEnhancedLineEdit::addContextMenuEntry(const QIcon &icon, const QString &name, const QObject *receiver, const char *method) {
    QAction* act=new QAction(icon, name, this);
    connect(act, SIGNAL(triggered()), receiver, method);
    addAction(act);
    contextmenuActions.append(act);
}

void QFEnhancedLineEdit::addInsertContextMenuEntry(const QString &name, const QString &insert) {
    addInsertContextMenuEntry(QIcon(), name, insert);
}

void QFEnhancedLineEdit::addInsertContextMenuEntry(const QIcon &icon, const QString &name, const QString &insert) {
    QAction* act=new QAction(icon, name, this);
    act->setData(insert);
    connect(act, SIGNAL(triggered()), this, SLOT(insertActTriggered()));
    addAction(act);
    contextmenuActions.append(act);
}

void QFEnhancedLineEdit::clearContextMenu() {
    foreach(QAction* act, contextmenuActions) {
        removeAction(act);
        act->disconnect();
        delete act;
    }
}

bool QFEnhancedLineEdit::useHistory() const
{
    return m_useHistory;
}

void QFEnhancedLineEdit::setUseHistory(bool use)
{
    m_useHistory=use;
    m_historyposition=-1;
}

QStringList QFEnhancedLineEdit::getHistory() const
{
    return history;
}

void QFEnhancedLineEdit::clearHistory()
{
    history.clear();
    m_historyposition=-1;
}

void QFEnhancedLineEdit::addToHistory(const QString &item)
{
    history.append(item);
}

void QFEnhancedLineEdit::setHistory(const QStringList &history)
{
    this->history=history;
}


void QFEnhancedLineEdit::keyPressEvent(QKeyEvent *event) {
    //qDebug()<<event->key();
    if (m_useHistory) {
        if(event->key() == Qt::Key_Up){
            if (m_historyposition<history.size()+1) m_historyposition++;
            if (m_historyposition>=0 && m_historyposition<history.size()) {
                setText(history[history.size()-1-m_historyposition]);
                selectAll();
            }
            return ;
        } else if(event->key() == Qt::Key_Down){
            if (m_historyposition>-1) m_historyposition--;
            if (m_historyposition>=0 && m_historyposition<history.size()) {
                setText(history[history.size()-1-m_historyposition]);
                selectAll();
            }
            return ;
        }
    }
    if (event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return) {
        history.append(text());
    }
    // default handler for event
    QLineEdit::keyPressEvent(event);
}
