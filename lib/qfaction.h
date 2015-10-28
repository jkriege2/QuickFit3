#ifndef QFACTION_H
#define QFACTION_H

#include <QAction>
#include "lib_imexport.h"

/** \brief a QAction-derivative that sets the action menu-role to QAction::NoRole by default */
class QFLIB_EXPORT QFActionWithNoMenuRole : public QAction
{
        Q_OBJECT
    public:
        explicit QFActionWithNoMenuRole(QObject* parent);
        QFActionWithNoMenuRole(const QString &text, QObject* parent);
        QFActionWithNoMenuRole(const QIcon &icon, const QString &text, QObject* parent);

};

#endif // QFACTION_H
