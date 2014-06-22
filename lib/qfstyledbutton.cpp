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

#include "qfstyledbutton.h"
#include <QtGui>
#include "qftools.h"
#include <iostream>

QFStyledButton::QFStyledButton(const QIcon& icon, QWidget* parent):
    QAbstractButton(parent)
{
    setIcon(icon);
    resize(iconSize()+QSize(2,2));
    setCursor(Qt::ArrowCursor);
    setMouseTracking(true);
    m_actionmode=None;
    m_basepath="";
    m_buddy=NULL;
    hover=false;
    m_prependURL="";
    m_action=NULL;
    m_filter=tr("All Files (*.*)");
}

QFStyledButton::QFStyledButton(QAction *action, QWidget *parent):
     QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setMouseTracking(true);
    m_actionmode=None;
    m_basepath="";
    m_buddy=NULL;
    hover=false;
    m_prependURL="";
    m_action=NULL;
    m_filter=tr("All Files (*.*)");
    setAction(action);
}

QFStyledButton::QFStyledButton(ActionMode mode, QWidget* buddy, QWidget* parent):
    QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setMouseTracking(true);
    m_actionmode=None;
    m_basepath="";
    m_buddy=NULL;
    hover=false;
    setBuddyWithDefaultIcon(buddy, mode);
    resize(iconSize()+QSize(2,2));
    m_prependURL="";
    m_action=NULL;
    m_filter=tr("All Files (*.*)");
}

QFStyledButton::~QFStyledButton()
{
    //dtor
}

void QFStyledButton::setBasepath(QString path) {
    m_basepath=path;
}

void QFStyledButton::openBuddyContents() {
    if (!m_buddy) return;
    QString prop=m_buddy->metaObject()->userProperty().read(m_buddy).toString();

    emit clicked(prop);
    if (m_action) m_action->trigger();

    if (m_actionmode==OpenURL || m_actionmode==OpenFile) {
        if (!prop.isEmpty()) QDesktopServices::openUrl(QUrl(prop, QUrl::TolerantMode));
    } else if (m_actionmode==OpenPrependedURL) {
        if (!prop.isEmpty()) QDesktopServices::openUrl(QUrl(m_prependURL+prop, QUrl::TolerantMode));
    } else if (m_actionmode==ExecuteFile) {
        QString a=QDir::cleanPath (QDir((m_basepath.isEmpty())?QDir::currentPath():m_basepath).absoluteFilePath( prop));
        QDesktopServices::openUrl(QUrl("file:///"+a, QUrl::TolerantMode));
    } else if (m_actionmode==ClearLineEdit) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(m_buddy);
        if (edt) {
            edt->clear();
        }
    } else if (m_actionmode==SelectFromCompleter) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(m_buddy);
        if (edt) {
            QCompleter* comp=edt->completer();
            if (comp) {
                disconnect(comp, SIGNAL(activated(QString)), edt, SLOT(setText(QString)));
                connect(comp, SIGNAL(activated(QString)), edt, SLOT(setText(QString)));
                comp->setCompletionPrefix("");
                comp->setCompletionMode(QCompleter::PopupCompletion);
                comp->complete();
            }
        }
    } else if (m_actionmode==SelectFile) {
        static QString dir="";
        QString fileName = qfGetOpenFileName(this, tr("Select File ..."), dir, m_filter);
        if (!fileName.isEmpty()) {
            dir=QFileInfo(fileName).dir().absolutePath();
            m_buddy->metaObject()->userProperty().write(m_buddy, QDir((m_basepath.isEmpty())?QDir::currentPath():m_basepath).relativeFilePath(fileName));
            m_buddy->setFocus(Qt::MouseFocusReason);
        }
    } else if (m_actionmode==SelectNewFile) {
        static QString dir="";
        QString fileName = qfGetSaveFileName(this, tr("Select New File ..."), dir, m_filter);
        if (!fileName.isEmpty()) {
            dir=QFileInfo(fileName).dir().absolutePath();
            m_buddy->metaObject()->userProperty().write(m_buddy, QDir((m_basepath.isEmpty())?QDir::currentPath():m_basepath).relativeFilePath(fileName));
            m_buddy->setFocus(Qt::MouseFocusReason);
        }
    } else if (m_actionmode==SelectDirectory) {
        static QString dir="";
        QString fileName = qfGetExistingDirectory(this, tr("Select Directory ..."), dir);
        if (!fileName.isEmpty()) {
            m_buddy->metaObject()->userProperty().write(m_buddy, fileName);
            m_buddy->setFocus(Qt::MouseFocusReason);
        }
    }
}

void QFStyledButton::actionChanged() {
    if (m_action) {
        setIcon(m_action->icon());
        setToolTip(m_action->toolTip());
        setText(m_action->text());
        setShortcut(m_action->shortcut());
        setWhatsThis(m_action->whatsThis());
    }
}

QWidget* QFStyledButton::buddy() const {
    return m_buddy;
};

void QFStyledButton::setBuddy(QWidget* b, ActionMode mode) {
    if (m_actionmode!=None) disconnect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
    m_actionmode=mode;
    m_buddy=b;
    if (m_actionmode!=None) connect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
}

void QFStyledButton::setBuddyWithDefaultIcon(QWidget* b, ActionMode mode) {
    setBuddy(b, mode);
    QIcon i;
    if ((mode==OpenURL)||(mode==OpenPrependedURL)) {
        i=QIcon(":/lib/qfstyledbutton/network.png");
        i.addFile(":/lib/qfstyledbutton/network_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/network_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/network_pressed.png", QSize(), QIcon::Active);
        setToolTip(tr("open weblink in line edit"));
    } else if (mode==ExecuteFile) {
        i=QIcon(":/lib/qfstyledbutton/execute.png");
        i.addFile(":/lib/qfstyledbutton/execute_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/execute_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/execute_pressed.png", QSize(), QIcon::Active);
        setToolTip(tr("execute file in line edit"));
    } else if (mode==SelectFile || mode==SelectNewFile || mode==SelectDirectory) {
        i=QIcon(":/lib/qfstyledbutton/selectfile.png");
        i.addFile(":/lib/qfstyledbutton/selectfile_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/selectfile_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/selectfile_pressed.png", QSize(), QIcon::Active);
        setToolTip(tr("select a filename for thw line edit"));
    } else if (mode==OpenFile) {
        i=QIcon(":/lib/qfstyledbutton/openfile.png");
        i.addFile(":/lib/qfstyledbutton/openfile_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/openfile_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/openfile_pressed.png", QSize(), QIcon::Active);
        setToolTip(tr("open file in line edit"));
    } else if (mode==ClearLineEdit) {
        i=QIcon(":/lib/qfstyledbutton/clear_edt.png");
        i.addFile(":/lib/qfstyledbutton/clear_edt_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/clear_edt_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/clear_edt_pressed.png", QSize(), QIcon::Active);
        setToolTip(tr("clear line edits contents"));
    } else if (mode==SelectFromCompleter) {
        i=QIcon(":/lib/qfstyledbutton/complete_edt.png");
        i.addFile(":/lib/qfstyledbutton/complete_edt_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/complete_edt_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/complete_edt_pressed.png", QSize(), QIcon::Active);
        setToolTip(tr("select text from autocompleter"));
    }

    setIcon(i);
}
void QFStyledButton::setNormalIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Normal);
    setIcon(i);
}

void QFStyledButton::setDisabledIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Disabled);
    setIcon(i);
}

void QFStyledButton::setHoverIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Active);
    setIcon(i);
}

void QFStyledButton::setPressedIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Selected);
    setIcon(i);
}

void QFStyledButton::paintEvent(QPaintEvent* e) {
    QSize s=size();
    //std::cout<<"paintEvent size="<<width()<<", "<<height()<<" hover="<<hover<<"\n";
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (!isEnabled()) {
        icon().paint(&painter, 0, 0, width(), height(), Qt::AlignCenter, QIcon::Disabled, (isChecked()?QIcon::On:QIcon::Off));
    } else {
        if (isDown()) icon().paint(&painter, 0, 0, width(), height(), Qt::AlignCenter, QIcon::Active, (isChecked()?QIcon::On:QIcon::Off));
        else {
            if (hover) icon().paint(&painter, 0, 0, width(), height(), Qt::AlignCenter, QIcon::Selected, (isChecked()?QIcon::On:QIcon::Off));
            else icon().paint(&painter, 0, 0, width(), height(), Qt::AlignCenter, QIcon::Normal, (isChecked()?QIcon::On:QIcon::Off));
        }
    }
}

QSize QFStyledButton::sizeHint()const {
    QSize s=iconSize()+QSize(2,2);
    //std::cout<<"sizeHint "<<s.width()<<", "<<s.height()<<"\n";
    return s;
}

QSize QFStyledButton::minimumSizeHint()const {
    QSize s=iconSize()+QSize(2,2);
    //std::cout<<"minimumSizeHint "<<s.width()<<", "<<s.height()<<"\n";
    return s; //icon().actualSize(QSize(16,16));
}

void QFStyledButton::enterEvent(QEvent* e) {
    hover=true;
    repaint();
    QAbstractButton::enterEvent(e);
}

void QFStyledButton::leaveEvent(QEvent* e) {
    hover=false;
    repaint();
    QAbstractButton::leaveEvent(e);
}

void QFStyledButton::setPrependURL(QString prepend) {
    m_prependURL=prepend;
}

void QFStyledButton::setFilter(const QString &filter)
{
    m_filter=filter;
}

void QFStyledButton::setAction(QAction *action) {
    if (m_action) disconnect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
    m_action=action;
    actionChanged();
    if (m_action) connect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
}
