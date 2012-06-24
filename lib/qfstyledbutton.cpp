#include "qfstyledbutton.h"
#include <QtGui>

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
    } else if (m_actionmode==SelectFile) {
        static QString dir="";
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select File ..."), dir, tr("All Files (*.*)"));
        if (!fileName.isEmpty()) {
            dir=QFileInfo(fileName).dir().absolutePath();
            m_buddy->metaObject()->userProperty().write(m_buddy, QDir((m_basepath.isEmpty())?QDir::currentPath():m_basepath).relativeFilePath(fileName));
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
    } else if (mode==ExecuteFile) {
        i=QIcon(":/lib/qfstyledbutton/execute.png");
        i.addFile(":/lib/qfstyledbutton/execute_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/execute_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/execute_pressed.png", QSize(), QIcon::Active);
    } else if (mode==SelectFile) {
        i=QIcon(":/lib/qfstyledbutton/selectfile.png");
        i.addFile(":/lib/qfstyledbutton/selectfile_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/selectfile_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/selectfile_pressed.png", QSize(), QIcon::Active);
    } else if (mode==OpenFile) {
        i=QIcon(":/lib/qfstyledbutton/openfile.png");
        i.addFile(":/lib/qfstyledbutton/openfile_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/openfile_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/openfile_pressed.png", QSize(), QIcon::Active);
    } else if (mode==ClearLineEdit) {
        i=QIcon(":/lib/qfstyledbutton/clear_edt.png");
        i.addFile(":/lib/qfstyledbutton/clear_edt_disabled.png", QSize(), QIcon::Disabled);
        i.addFile(":/lib/qfstyledbutton/clear_edt_hover.png", QSize(), QIcon::Selected);
        i.addFile(":/lib/qfstyledbutton/clear_edt_pressed.png", QSize(), QIcon::Active);
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

void QFStyledButton::setAction(QAction *action) {
    if (m_action) disconnect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
    m_action=action;
    actionChanged();
    if (m_action) connect(m_action, SIGNAL(changed()), this, SLOT(actionChanged()));
}
