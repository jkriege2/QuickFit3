#include "qfenhancedplaintextedit.h"

QFEnhancedPlainTextEdit::QFEnhancedPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setContextMenuPolicy(Qt::DefaultContextMenu);
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
    }
    // default handler for event
    QPlainTextEdit::keyPressEvent(event);
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
