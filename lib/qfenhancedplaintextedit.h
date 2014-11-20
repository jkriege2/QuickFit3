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

#ifndef QFENHANCEDPLAINTEXTEDIT_H
#define QFENHANCEDPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QList>
#include "lib_imexport.h"
#include <QVariant>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QCompleter>
#include <QTextEdit>
#include <QTextBlock>
#include <QTextCursor>

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
        QString textUnderCursor() const;

        /** \brief returns true if any text is currently selected */
        inline bool hasSelection() { return textCursor().hasSelection(); };

        /** \brief returns the currently selected text */
        inline QString getSelection() { return textCursor().selectedText(); };

        /** \brief get the current line number */
        inline long getLineNumber() { return textCursor().blockNumber() + 1; };

        /** \brief get the current line number */
        inline long getColumnNumber() { return textCursor().positionInBlock() + 1; };


        /** \brief set the used QCompleter object */
        void setCompleter(QCompleter *completer);

        /** \brief return the currently used QCompleter object */
        QCompleter* completer() const;
    signals:
        void helpKeyPressed();

    public slots:
        /** \brief move the cursor to the specified line */
        void gotoLine(int line);
        void setText(const QString& text);
    protected:
        virtual void contextMenuEvent ( QContextMenuEvent * event );
        /** \brief this method implements the reaction on key presses and also displays the
         *         completer. In addition it handles special key press tasks (TAB to spaces ...)
         *
         * - If the completer is displayed, some of the special keys (TAB, RETURN ...) are beeing ignored
         * - in normal mode:
         *    - for TAB indentInc() is called
         *    - for Shift+TAB indentDec() is called
         *    - when RETURN is pressed, the indention is handled propperly:
         *       - the cursor in the new line is set to the same indention level, as the current
         *         line
         *       - if RETURN is pressed after '{' the indention is increased by \c tabwidth characters
         *       - if RETURN is pressed after '}' with only leading space characters, the indention is
         *         reduced to the indention of the matching '{'. Here the indention in the new line AND
         *         the indention of the line, conatining the '}', is set equally!
         *       .
         *    .
         * .
         */
        virtual void keyPressEvent(QKeyEvent *event);
        /** \brief called when the focus of the TextEdit changes */
        void focusInEvent(QFocusEvent *e);
        void intInsertAction(QString name, QAction* act);
    protected slots:
        void insertActTriggered();
    private slots:
        /** \brief called to insert the selected completion
         *
         * In the selction database file you can use the special character \c ° to specify a
         * position in the inserted text. if there is one \c ° the cursor will be set to this
         * position after insert. If there are two \c ° the text between them will be selected
         * after insert.
         */
        void insertCompletion(const QString &completion);
    private:
        QList<QAction*> contextmenuActions;
        QList<QMenu*> submenus;
        /** \brief points to the QCompleter class, used by this QTextEdit */
        QCompleter *c;


};

#endif // QFENHANCEDPLAINTEXTEDIT_H
