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

#ifndef QFENHANCEDLINEEDIT_H
#define QFENHANCEDLINEEDIT_H

#include <QAbstractButton>
#include <QLineEdit>
#include <QList>
#include "lib_imexport.h"
#include <QVariant>

/*! \brief enhanced QLineEdit which adds some features missing in the basic Qt implementation
    \ingroup qf3lib_widgets

    The new features are:
       - When this QLineEdit looses focus its contents is added to its completer (if any is defined)
       - it is possible to add QAbstractButton to the widget that will appear inside the line edit.
    .

    If you add context menu entries, it is possibly to easily build a tree structure of submenus, by using
    \c ';;' as sepearators. E.g. \c "Menu;;Submenu;;ActionName" will create a contextmenu entry with name
    \c ActionName in the menu \c Submenu, which is a submenu of \c Menu.
 */
class QFLIB_EXPORT QFEnhancedLineEdit : public QLineEdit {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEnhancedLineEdit(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEnhancedLineEdit();

        /** \brief add a new QAbstractButton to the edit. The button will appear left of the previously added button */
        void addButton(QAbstractButton* button);
        /** \brief return the i-th button */
        QAbstractButton* getButton(int i);
        /** \brief return the number of added buttons */
        int getButtonCount();
        /** \brief remove the given button */
        void removeButton(int i);
        /** \brief remove the given button */
        void removeButton(QAbstractButton* button);
        /** \brief set distance between two buttons in pixels */
        void setButtonDistance(int d);
        /** \brief return distance between two buttons */
        int buttonDistance();

        void addContextMenuEntry(const QString& name, QVariant data=QVariant());
        void addContextMenuEntry(const QIcon &icon, const QString& name, QVariant data=QVariant());
        void addContextMenuEntry(const QString& name, const QObject * receiver, const char * method);
        void addContextMenuEntry(const QIcon & icon, const QString& name, const QObject * receiver, const char * method);
        void addInsertContextMenuEntry(const QString& name, const QString& insert);
        void addInsertContextMenuEntry(const QIcon & icon, const QString& name, const QString& insert);
        void clearContextMenu();

        bool useHistory() const;
        void setUseHistory(bool use);
        QStringList getHistory() const;
        void clearHistory();
        void addToHistory(const QString& item);
        void setHistory(const QStringList& history);


    protected:
        virtual void focusOutEvent ( QFocusEvent * event ) ;
        virtual void resizeEvent ( QResizeEvent * event );
        virtual void contextMenuEvent ( QContextMenuEvent * event );
        virtual void keyPressEvent(QKeyEvent *event);
        /** \brief return the width of the buttons until the i-th button */
        int getButtonsWidth(int i);
        void moveButtons();
        void invalidateButtons();
        void intInsertAction(QString name, QAction* act);
    protected slots:
        void insertActTriggered();
    private:
        int m_buttonDistance;
        QList<QAbstractButton*> buttons;
        QList<QAction*> contextmenuActions;
        QList<QMenu*> submenus;

        int m_historyposition;
        QStringList history;
        bool m_useHistory;
};



#endif // QFENHANCEDLINEEDIT_H
