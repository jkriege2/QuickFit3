/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate: 2014-09-14 18:04:44 +0200 (So, 14 Sep 2014) $  (revision $Rev: 3464 $)

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
#include "qfenhancedplaintextedit.h"

#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QMessageBox>
#include <QPainter>
#include <QToolTip>

QFEnhancedPlainTextEdit::QFEnhancedPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    c=NULL;
    setContextMenuPolicy(Qt::DefaultContextMenu);
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorChanged()));
}

void QFEnhancedPlainTextEdit::addContextMenuEntry(const QString &name, QVariant data)
{
    addContextMenuEntry(QIcon(), name, data);
}

void QFEnhancedPlainTextEdit::addContextMenuEntry(const QIcon &icon, const QString &name, QVariant data)
{
    QAction* act=new QAction(icon, name, this);
    act->setData(data);
    intInsertAction(name, act);
}

void QFEnhancedPlainTextEdit::addContextMenuEntry(const QString &name, const QObject *receiver, const char *method)
{
    addContextMenuEntry(QIcon(), name, receiver, method);
}

void QFEnhancedPlainTextEdit::addContextMenuEntry(const QIcon &icon, const QString &name, const QObject *receiver, const char *method)
{
    QAction* act=new QAction(icon, name, this);
    connect(act, SIGNAL(triggered()), receiver, method);
    intInsertAction(name, act);
}

void QFEnhancedPlainTextEdit::addInsertContextMenuEntry(const QString &name, const QString &insert)
{
    addInsertContextMenuEntry(QIcon(), name, insert);
}

void QFEnhancedPlainTextEdit::addInsertContextMenuEntry(const QIcon &icon, const QString &name, const QString &insert)
{
    QAction* act=new QAction(icon, name, this);
    act->setData(insert);
    connect(act, SIGNAL(triggered()), this, SLOT(insertActTriggered()));
    intInsertAction(name, act);
}

void QFEnhancedPlainTextEdit::clearContextMenu()
{
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
}

QString QFEnhancedPlainTextEdit::text() const
{
    return toPlainText();
}

void QFEnhancedPlainTextEdit::setText(const QString &text)
{
    setPlainText(text);
}

void QFEnhancedPlainTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
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

void QFEnhancedPlainTextEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Help) {
        emit helpKeyPressed();
    } else {
        if (c && c->popup()->isVisible()) {
            // The following keys are forwarded by the completer to the widget
           switch (event->key()) {
               case Qt::Key_Enter:
               case Qt::Key_Return:
               case Qt::Key_Escape:
               case Qt::Key_Tab:
               case Qt::Key_Backtab:
                    event->ignore();
                    return; // completer widgets handles these keys!
               default:
                   break;
           }
        }
    }
    // default handler for event

    bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // dont process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(event);

    const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!c || (ctrlOrShift && event->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();
    /*QMessageBox::information(this, "", tr("completionPrefix=%1\nisShortcut=%2\nctrlOrShif=%3\nhasModifier=%4")
                                           .arg(completionPrefix)
                                           .arg(isShortcut)
                                           .arg(ctrlOrShift)
                                           .arg(hasModifier)
                                           );*/

    if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 2
                      || eow.contains(event->text().right(1)))) {
        c->popup()->hide();
        return;
    }
    /*QMessageBox::information(this, "", tr("c->completionPrefix=%1\ncompletionPrefix=%2")
                                           .arg(c->completionPrefix())
                                           .arg(completionPrefix)
                                           );*/
    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        /*QMessageBox::information(this, "", tr("c->completionModel()->rowCount()=%1")
                                           .arg(c->completionModel()->rowCount())
                                           );*/
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}

void QFEnhancedPlainTextEdit::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void QFEnhancedPlainTextEdit::intInsertAction(QString name, QAction *act)
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

void QFEnhancedPlainTextEdit::insertActTriggered()
{
    QAction* act=qobject_cast<QAction*>(sender());
    if (act) {
        textCursor().insertText(act->data().toString());
    }
}

void QFEnhancedPlainTextEdit::insertCompletion(const QString &completion1)
{
    if (c->widget() != this)
        return;
    QString completion=completion1;
    completion=completion.remove(QLatin1Char('\xB0'),  Qt::CaseInsensitive);
    QTextCursor tc = textCursor();
    int extra = completion.length() - c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));

    // handle special character '\xB0' in insertted text completion1
    unsigned char mcount=completion1.count(QLatin1Char('\xB0'), Qt::CaseInsensitive);
    int p1=completion1.indexOf(QLatin1Char('\xB0'), Qt::CaseInsensitive);
    int p2=completion1.indexOf(QLatin1Char('\xB0'), p1+1, Qt::CaseInsensitive);
    if (mcount==1) {
        tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, completion.size()-p1);
    } else if (mcount>1) {
        tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, completion.size()-p2+1);
        tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, p2-p1-1);
    }
    setTextCursor(tc);
}

QString QFEnhancedPlainTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void QFEnhancedPlainTextEdit::setCompleter(QCompleter *completer)
{
    if (c)
        QObject::disconnect(c, 0, this, 0);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}

QCompleter *QFEnhancedPlainTextEdit::completer() const
{
    return c;
}

void QFEnhancedPlainTextEdit::gotoLine(int line)
{
    QTextCursor c(textCursor());
    c.beginEditBlock();
    c.movePosition(QTextCursor::Start);
    if (line>1)
        c.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line-1);
    c.endEditBlock();
    setTextCursor(c);
}
