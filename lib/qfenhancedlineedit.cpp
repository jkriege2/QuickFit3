/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

void QFEnhancedLineEdit::intInsertAction(QString name, QAction *act)
{
    if (!name.contains(";;")) {
        addAction(act);
        contextmenuActions.append(act);
    } else {
        QStringList sl=name.split(";;");
        QMenu* m=NULL;
        for (int i=0; i<submenus.size(); i++) {
            if (submenus[i]->title()==sl.first()) {
                m=submenus[i];
                break;
            }
        }
        QString n=sl.last();
        act->setText(n);
        if (m) {
            for (int i=1; i<sl.size()-1; i++) {
                QList<QAction*> al=m->actions();
                QMenu* mn=NULL;
                for (int j=0; j<al.size(); j++) {
                    if (al[j]->menu() && al[j]->menu()->title()==sl[i]) {
                        mn=al[j]->menu();
                        break;
                    }
                }
                if (!mn) {
                    mn=new QMenu(sl[i], this);
                    m->addAction(mn->menuAction());

                }
                m=mn;
            }
        } else {
            for (int i=0; i<sl.size()-1; i++) {
                QMenu* mn=new QMenu(sl[i], this);
                if (i==0) {
                    contextmenuActions.append(mn->menuAction());
                    addAction(mn->menuAction());
                    submenus.append(mn);
                }
                if (m) m->addMenu(mn);
                m=mn;
            }
        }
        if (m) {
            m->addAction(act);
            contextmenuActions.append(act);
        } else {
            addAction(act);
            contextmenuActions.append(act);
        }
        //addAction(act);
    }
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
    intInsertAction(name, act);
}


void QFEnhancedLineEdit::addContextMenuEntry(const QString &name, const QObject *receiver, const char *method) {
    addContextMenuEntry(QIcon(), name, receiver, method);
}

void QFEnhancedLineEdit::addContextMenuEntry(const QIcon &icon, const QString &name, const QObject *receiver, const char *method) {
    QAction* act=new QAction(icon, name, this);
    connect(act, SIGNAL(triggered()), receiver, method);
    intInsertAction(name, act);
}

void QFEnhancedLineEdit::addInsertContextMenuEntry(const QString &name, const QString &insert) {
    addInsertContextMenuEntry(QIcon(), name, insert);
}

void QFEnhancedLineEdit::addInsertContextMenuEntry(const QIcon &icon, const QString &name, const QString &insert) {
    QAction* act=new QAction(icon, name, this);
    act->setData(insert);
    connect(act, SIGNAL(triggered()), this, SLOT(insertActTriggered()));
    intInsertAction(name, act);
}

void QFEnhancedLineEdit::clearContextMenu() {
    foreach(QAction* act, contextmenuActions) {
        removeAction(act);
        act->disconnect();
        if (!act->menu()) delete act;
    }
    foreach(QMenu* act, submenus) {
        delete act;
    }
    contextmenuActions.clear();
    submenus.clear();
    /*QMapIterator<QString, QMenu*> i(submenus);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }*/
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
