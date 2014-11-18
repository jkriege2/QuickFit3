#ifndef QFENHANCEDPLAINTEXTEDIT_H
#define QFENHANCEDPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QList>
#include "lib_imexport.h"
#include <QVariant>
#include <QIcon>
#include <QAction>
#include <QMenu>

/*! \brief enhanced QPlainTextEdit which adds some features missing in the basic Qt implementation
    \ingroup qf3lib_widgets


    If you add context menu entries, it is possibly to easily build a tree structure of submenus, by using
    \c ';;' as sepearators. E.g. \c "Menu;;Submenu;;ActionName" will create a contextmenu entry with name
    \c ActionName in the menu \c Submenu, which is a submenu of \c Menu.
 */
class QFLIB_EXPORT QFEnhancedPlainTextEdit : public QPlainTextEdit {
        Q_OBJECT
    public:
        explicit QFEnhancedPlainTextEdit(QWidget *parent = 0);

        void addContextMenuEntry(const QString& name, QVariant data=QVariant());
        void addContextMenuEntry(const QIcon &icon, const QString& name, QVariant data=QVariant());
        void addContextMenuEntry(const QString& name, const QObject * receiver, const char * method);
        void addContextMenuEntry(const QIcon & icon, const QString& name, const QObject * receiver, const char * method);
        void addInsertContextMenuEntry(const QString& name, const QString& insert);
        void addInsertContextMenuEntry(const QIcon & icon, const QString& name, const QString& insert);
        void clearContextMenu();
        QString text() const;
    signals:
        void helpKeyPressed();
    public slots:
        void setText(const QString& text);
    protected:
        virtual void contextMenuEvent ( QContextMenuEvent * event );
        virtual void keyPressEvent(QKeyEvent *event);
        void intInsertAction(QString name, QAction* act);
    protected slots:
        void insertActTriggered();

    private:
        QList<QAction*> contextmenuActions;
        QList<QMenu*> submenus;

};

#endif // QFENHANCEDPLAINTEXTEDIT_H
