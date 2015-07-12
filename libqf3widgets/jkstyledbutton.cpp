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



#include "jkstyledbutton.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtGlobal>
#include <QtWidgets>
#else
#include <QtGui>
#endif



#include <iostream>

JKStyledButton::JKStyledButton(const QIcon& icon, QWidget* parent):
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
    m_initPath=QApplication::applicationDirPath();
}

JKStyledButton::JKStyledButton(QAction *action, QWidget *parent):
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
    m_initPath=QApplication::applicationDirPath();
    setAction(action);
}

JKStyledButton::JKStyledButton(ActionMode mode, QWidget* buddy, QWidget* parent):
    QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setMouseTracking(true);
    m_actionmode=None;
    m_basepath="";
    m_buddy=buddy;
    hover=false;
    setBuddyWithDefaultIcon(buddy, mode);
    resize(iconSize()+QSize(2,2));
    m_prependURL="";
    m_action=NULL;
    m_filter=tr("All Files (*.*)");
    m_initPath=QApplication::applicationDirPath();
}

JKStyledButton::~JKStyledButton()
{
    //dtor
}

void JKStyledButton::setBasepath(QString path) {
    m_basepath=path;
}

void JKStyledButton::openBuddyContents() {
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
    } else if (m_actionmode==SelectFile) {
        static QString dir=m_initPath;
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select File ..."), dir, m_filter);
        if (!fileName.isEmpty()) {
            dir=QFileInfo(fileName).dir().absolutePath();
            m_buddy->metaObject()->userProperty().write(m_buddy, QDir((m_basepath.isEmpty())?QDir::currentPath():m_basepath).relativeFilePath(fileName));
            m_buddy->setFocus(Qt::MouseFocusReason);
        }
    }
}

void JKStyledButton::actionChanged() {
    if (m_action) {
        setIcon(m_action->icon());
        setToolTip(m_action->toolTip());
        setText(m_action->text());
        setShortcut(m_action->shortcut());
        setWhatsThis(m_action->whatsThis());
    }
}

QWidget* JKStyledButton::buddy() const {
    return m_buddy;
};

void JKStyledButton::setBuddy(QWidget* b, ActionMode mode) {
    if (m_actionmode!=None) disconnect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
    m_actionmode=mode;
    m_buddy=b;
    if (m_actionmode!=None) connect(this, SIGNAL(clicked()), this, SLOT(openBuddyContents()));
}

void JKStyledButton::setBuddyWithDefaultIcon(QWidget* b, ActionMode mode) {
    setBuddy(b, mode);
    QIcon i;
    if ((mode==OpenURL)||(mode==OpenPrependedURL)) {
        i=QIcon(":/jkstyledbutton/network.png");
        i.addFile(":/jkstyledbutton/network_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/jkstyledbutton/network_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/jkstyledbutton/network_pressed.png", QSize(), QIcon::Active);
    } else if (mode==ExecuteFile) {
        i=QIcon(":/jkstyledbutton/execute.png");
        i.addFile(":/jkstyledbutton/execute_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/jkstyledbutton/execute_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/jkstyledbutton/execute_pressed.png", QSize(), QIcon::Active);
    } else if (mode==SelectFile) {
        i=QIcon(":/jkstyledbutton/selectfile.png");
        i.addFile(":/jkstyledbutton/selectfile_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/jkstyledbutton/selectfile_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/jkstyledbutton/selectfile_pressed.png", QSize(), QIcon::Active);
    } else if (mode==OpenFile) {
        i=QIcon(":/jkstyledbutton/openfile.png");
        i.addFile(":/jkstyledbutton/openfile_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/jkstyledbutton/openfile_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/jkstyledbutton/openfile_pressed.png", QSize(), QIcon::Active);
    } else if (mode==ClearLineEdit) {
        i=QIcon(":/jkstyledbutton/clear_edt.png");
        i.addFile(":/jkstyledbutton/clear_edt_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/jkstyledbutton/clear_edt_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/jkstyledbutton/clear_edt_pressed.png", QSize(), QIcon::Active);
    }


    setIcon(i);
}
void JKStyledButton::setNormalIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Normal);
    setIcon(i);
}

void JKStyledButton::setDisabledIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Disabled);
    setIcon(i);
}

void JKStyledButton::setHoverIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Active);
    setIcon(i);
}

void JKStyledButton::setPressedIcon(const QPixmap& pixmap) {
    QIcon i=icon();
    i.addPixmap(pixmap, QIcon::Selected);
    setIcon(i);
}

void JKStyledButton::paintEvent(QPaintEvent* /*e*/) {
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

QSize JKStyledButton::sizeHint()const {
    QSize s=iconSize()+QSize(2,2);
    //std::cout<<"sizeHint "<<s.width()<<", "<<s.height()<<"\n";
    return s;
}

QSize JKStyledButton::minimumSizeHint()const {
    QSize s=iconSize()+QSize(2,2);
    //std::cout<<"minimumSizeHint "<<s.width()<<", "<<s.height()<<"\n";
    return s; //icon().actualSize(QSize(16,16));
}

void JKStyledButton::enterEvent(QEvent* e) {
    hover=true;
    repaint();
    QAbstractButton::enterEvent(e);
}

void JKStyledButton::leaveEvent(QEvent* e) {
    hover=false;
    repaint();
    QAbstractButton::leaveEvent(e);
}

void JKStyledButton::setPrependURL(QString prepend) {
    m_prependURL=prepend;
}

void JKStyledButton::setAction(QAction *action) {
    if (m_action) disconnect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
    m_action=action;
    actionChanged();
    if (m_action) connect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
}

void JKStyledButton::setFilter(const QString &filter)
{
    m_filter=filter;
}

void JKStyledButton::setInitPath(const QString &initpath)
{
    m_initPath=initpath;
}
