#include "qfaction.h"



QFActionWithNoMenuRole::QFActionWithNoMenuRole(QObject *parent):
    QAction(parent)
{
    setMenuRole(QAction::NoRole);
}

QFActionWithNoMenuRole::QFActionWithNoMenuRole(const QString &text, QObject *parent):
    QAction(text, parent)
{
    setMenuRole(QAction::NoRole);
}

QFActionWithNoMenuRole::QFActionWithNoMenuRole(const QIcon &icon, const QString &text, QObject *parent):
    QAction(icon, text, parent)
{
    setMenuRole(QAction::NoRole);
}
