/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "qenhancedlineedit.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif



#include <iostream>

QEnhancedLineEdit::QEnhancedLineEdit(QWidget* parent):
    QLineEdit(parent)
{
    m_buttonDistance=1;
    setContextMenuPolicy(Qt::DefaultContextMenu);
    //setMinimumHeight(minimumHeight()+2);

}

QEnhancedLineEdit::~QEnhancedLineEdit()
{
    //dtor
}

void QEnhancedLineEdit::focusOutEvent(QFocusEvent* event) {
    if (completer()) {
        completer()->model()->insertRow(completer()->model()->rowCount());
        completer()->model()->setData(completer()->model()->index(completer()->model()->rowCount()-1,0), this->text());
    }
    QLineEdit::focusOutEvent(event);
}


void QEnhancedLineEdit::resizeEvent ( QResizeEvent * event ) {
    QLineEdit::resizeEvent(event);
    invalidateButtons();
}

void QEnhancedLineEdit::contextMenuEvent(QContextMenuEvent *event) {
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

void QEnhancedLineEdit::invalidateButtons() {
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

void QEnhancedLineEdit::insertActTriggered() {
    QAction* act=qobject_cast<QAction*>(sender());
    if (act) {
        insert(act->data().toString());
    }
}

void QEnhancedLineEdit::moveButtons() {
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

void QEnhancedLineEdit::addButton(QAbstractButton* button) {
    if (!buttons.contains(button)) {
        QCursor c=button->cursor();
        buttons.append(button);
        button->setParent(this);
        button->setCursor(c);

        invalidateButtons();
    }
}

QAbstractButton* QEnhancedLineEdit::getButton(int i) {
    if (i<0) return NULL;
    if (i>=buttons.size()) return NULL;
    return buttons[i];
}

int QEnhancedLineEdit::getButtonCount() {
    return buttons.size();
}

void QEnhancedLineEdit::removeButton(int i) {
    if (i<0) return;
    if (i>=buttons.size()) return;
    buttons.removeAt(i);
    invalidateButtons();
}

void QEnhancedLineEdit::removeButton(QAbstractButton* button) {
    buttons.removeAll(button);
    invalidateButtons();
}

int QEnhancedLineEdit::getButtonsWidth(int i) {
    int w=0;
    for (int j=0; j<i; j++) {
        w=w+buttons[j]->width()+m_buttonDistance;
    }
    return w;
}

void QEnhancedLineEdit::setButtonDistance(int d) {
    m_buttonDistance=d;
    invalidateButtons();
}

int QEnhancedLineEdit::buttonDistance() {
    return m_buttonDistance;
}

void QEnhancedLineEdit::addContextMenuEntry(const QString &name, QVariant data) {
    addContextMenuEntry(QIcon(), name, data);
}

void QEnhancedLineEdit::addContextMenuEntry(const QIcon& icon, const QString &name, QVariant data) {
    QAction* act=new QAction(icon, name, this);
    act->setData(data);
    addAction(act);
    contextmenuActions.append(act);
}


void QEnhancedLineEdit::addContextMenuEntry(const QString &name, const QObject *receiver, const char *method) {
    addContextMenuEntry(QIcon(), name, receiver, method);
}

void QEnhancedLineEdit::addContextMenuEntry(const QIcon &icon, const QString &name, const QObject *receiver, const char *method) {
    QAction* act=new QAction(icon, name, this);
    connect(act, SIGNAL(triggered()), receiver, method);
    addAction(act);
    contextmenuActions.append(act);
}

void QEnhancedLineEdit::addInsertContextMenuEntry(const QString &name, const QString &insert) {
    addInsertContextMenuEntry(QIcon(), name, insert);
}

void QEnhancedLineEdit::addInsertContextMenuEntry(const QIcon &icon, const QString &name, const QString &insert) {
    QAction* act=new QAction(icon, name, this);
    act->setData(insert);
    connect(act, SIGNAL(triggered()), this, SLOT(insertActTriggered()));
    addAction(act);
    contextmenuActions.append(act);
}

void QEnhancedLineEdit::clearContextMenu() {
    foreach(QAction* act, contextmenuActions) {
        removeAction(act);
        act->disconnect();
        delete act;
    }
}

