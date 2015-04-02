/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFSTYLEDBUTTON_H
#define QFSTYLEDBUTTON_H

#include <QAbstractButton>
#include "lib_imexport.h"
#include <QIcon>
#include <QItemEditorCreator>

/*! \brief this is a specialized button which is not drawn as a standard button, but with
           user supplied  images for different states
    \ingroup qf3lib_widgets

    The user may supply these images:
      - normal: the default appearance of the button
      - disabled: appearance, when button is disabled
      - hover: when the mouse hover over the button
      - pressed: when the mouse button is pressed
    .
    You can also supply a single QIcon which is used to derive the above states.

    In addition this button allows to set a buddy QWidget. If it provides a user property,
    and one of the special function modes is set this user property is used to execute the
    specified function. Possible implemented functions are:
      - open an URL
      - execute/open a file (possibly using a specified basePath to decode relative file paths)
      - select a new file
    .
    You can reset this behaviour by calling \code setBuddy(NULL, QFStyledButton::None) \endcode .

 */
class QFLIB_EXPORT QFStyledButton : public QAbstractButton {
        Q_OBJECT
    public:
        enum ActionMode {
            None=0,
            OpenURL=1,
            ExecuteFile=2,
            SelectFile=3,
            OpenPrependedURL=4,
            ClearLineEdit=5,
            OpenFile=6,
            SelectFromCompleter=7,
            SelectDirectory=8,
            SelectNewFile=9
        };

        QFStyledButton(const QIcon& icon, QWidget* parent=NULL);
        QFStyledButton(QAction* action, QWidget* parent=NULL);
        /** \brief default constructor which loads one of the three predefined icon sets */
        QFStyledButton(ActionMode mode, QWidget* buddy, QWidget* parent=NULL);
        virtual ~QFStyledButton();

        /** \brief set the normal pixmap */
        void setNormalIcon(const QPixmap& pixmap);
        /** \brief set the normal pixmap */
        void setDisabledIcon(const QPixmap& pixmap);
        /** \brief set the normal pixmap */
        void setHoverIcon(const QPixmap& pixmap);
        /** \brief set the normal pixmap */
        void setPressedIcon(const QPixmap& pixmap);



        QWidget* buddy() const;
        /** \brief set buddy + action */
        void setBuddy(QWidget* b, ActionMode=ExecuteFile);
        void setBuddyWithDefaultIcon(QWidget* b, ActionMode=ExecuteFile);
        void setDefaultIcon(ActionMode mode =ExecuteFile);

        virtual QSize sizeHint()const;
        virtual QSize minimumSizeHint()const;

        void setBasepath(QString path);
        void setPrependURL(QString prepend);
        void setFilter(const QString& filter);

        void setAction(QAction* action);

    signals:
        void clicked(QString buddyContents);
        void clickedWithButton(QFStyledButton* button);
        void clickedWithBuddy(QWidget* buddy);
    protected:
        virtual void paintEvent(QPaintEvent* e);
        virtual void enterEvent(QEvent* e);
        virtual void leaveEvent(QEvent* e);
        QIcon image;
        QWidget* m_buddy;
        QString m_basepath;
        ActionMode m_actionmode;
        QString m_prependURL;
        QAction* m_action;
        QString m_filter;
        bool hover;
    protected slots:
        void openBuddyContents();
        void actionChanged();

    private:
};

#endif // QFSTYLEDBUTTON_H
