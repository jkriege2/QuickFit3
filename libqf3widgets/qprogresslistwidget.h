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


#ifndef QPROGRESSLISTWIDGET_H
#define QPROGRESSLISTWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QMap>
#include <QIcon>
#include <QList>
#include "libwid_imexport.h"
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include "qfdialog.h"
#include <QProgressBar>


/** \brief displays a list of items with an icon next to each which indicates the current task performed on the item (e.g. not started, in progress, success, failed, ...)
 *  \ingroup tools_qt
 */
class QFWIDLIB_EXPORT QProgressListWidget : public QWidget {
        Q_OBJECT
    public:
        enum {
            statusNone=0,
            statusNotStarted=1,
            statusDone=2,
            statusFailed=3,
            statusInProgress=4,
            statusUser=5
        };

        explicit QProgressListWidget(QWidget *parent = 0);

        void defineIcon(int status, QIcon icon);
        QIcon getIcon(int status) const;

        int getStatus(int item) const;
        QString getText(int item) const;
        void deleteItem(int item);
        int count() const;
        void addItem(const QString& text, int status=statusNotStarted);
        QSize getIconSize() const;
        void setIconSize(QSize size);
        bool isCentered() const;
        void setCentered(bool centered);
        void reset();

    signals:

    public slots:
        /** \brief searches for the first item with status statusInProgress and switches it's status to \a newStatusLast The next item with status  statusNotStarted is switched to statusInProgress */
        void nextItem(int newStatusLast=statusDone);
        /** \brief searched for the first item with status statusNotStarted and sets it to statusInProgress */
        void start();

        void setItemStatus(int item, int status);
        void setItemText(int item, const QString& text);

    protected:
        struct widgetstruct  {
            QLabel* labIcon;
            QLabel* labText;
        };

        bool m_centered;

        QStringList m_items;
        QList<int> m_status;
        QMap<int, QIcon> m_icons;
        QList<widgetstruct> m_widgets;
        QSize m_iconSize;
        QGridLayout* layout;

        void updateWidgets();
};


/** \brief a dialog with a QProgressListWidget and a cancel button (optional)
 *  \ingroup tools_qt
 */
class QFWIDLIB_EXPORT QProgressListDialog : public QFDialog {
        Q_OBJECT
    public:
        QProgressListDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
        QProgressListDialog(const QString & labelText, const QString & cancelButtonText, int minimum, int maximum, QWidget * parent = 0, Qt::WindowFlags f = 0);
        QProgressListWidget* getProgressList() const;

        void setHasCancelButton(bool hasCancel, bool cancelRejects=false);
        void setHasProgressBar(bool hasProg=true);
        bool wasCanceled() const;
        void defineIcon(int status, QIcon icon);
        void addItem(const QString& text, int status=QProgressListWidget::statusNotStarted);

    public slots:
        void cancel();
        /** \brief searches for the first item with status statusInProgress and switches it's status to \a newStatusLast The next item with status  statusNotStarted is switched to statusInProgress */
        void nextItem(int newStatusLast=QProgressListWidget::statusDone);
        /** \brief searched for the first item with status statusNotStarted and sets it to statusInProgress */
        void start();
        void reset();

        void setItemStatus(int item, int status);
        void setItemText(int item, const QString& text);
        void setRange(int min, int max);
        void setMinimum(int min);
        void setMaximum(int max);
        void setValue(int value);
        void setProgressText(const QString& text);
        void setLabelText(const QString& text);
    protected:
        QProgressListWidget* list;
        QPushButton* btnCancel;
        bool m_wasCanceled;
        bool m_cancelRejects;
        bool m_hasProgressBar;
        QLabel* labProgress;
        QProgressBar* progress;
        QFrame* progressLine;

        virtual void showEvent(QShowEvent* event);
        void createWidgets();
};


#endif // QPROGRESSLISTWIDGET_H

