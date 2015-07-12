/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QWWWTOOLBUTTON_H
#define QWWWTOOLBUTTON_H
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>
#include <QUrl>
#include <QString>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <iostream>
#include "libwid_imexport.h"




/** \brief specialized QToolButton which takes the contents of its buddy as webpage adress (URL) and opens
 *         it in the system's standard application
 *  \ingroup tools_qt
 */
class QFWIDLIB_EXPORT QWWWToolButton : public QToolButton {
        Q_OBJECT
    public:
        /** Default constructor */
        QWWWToolButton(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QWWWToolButton();

        inline QLineEdit* buddy() const { return m_buddy; };
        inline void setBuddy(QLineEdit* b) { m_buddy=b; }
    protected:
        QLineEdit* m_buddy;
    protected slots:
        void openBuddyContents();
    private:
};

/** \brief specialized QToolButton which allows to select a file and saves its path in the buddy
 *  \ingroup tools_qt
 */
class QFWIDLIB_EXPORT QFileSelectToolButton : public QToolButton {
        Q_OBJECT
    public:
        /** Default constructor */
        QFileSelectToolButton(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFileSelectToolButton();

        inline QLineEdit* buddy() const { return m_buddy; };
        inline void setBuddy(QLineEdit* b) { m_buddy=b; }
        inline void setBaseDir(QString& b) { baseDir=b; }
    protected:
        QLineEdit* m_buddy;
        QString baseDir;
    protected slots:
        void openBuddyContents();
    private:
};

/** \brief specialized QToolButton which allows to open the file referenced by the buddy edit
 *  \ingroup tools_qt
 */
class QFWIDLIB_EXPORT QFileExecuteToolButton : public QToolButton {
        Q_OBJECT
    public:
        /** Default constructor */
        QFileExecuteToolButton(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFileExecuteToolButton();

        inline QLineEdit* buddy() const { return m_buddy; };
        inline void setBuddy(QLineEdit* b) { m_buddy=b; }
        inline void setBaseDir(QString& b) { baseDir=b; }
    protected:
        QLineEdit* m_buddy;
        QString baseDir;
    protected slots:
        void openBuddyContents();
    private:
};

#endif // QWWWTOOLBUTTON_H
