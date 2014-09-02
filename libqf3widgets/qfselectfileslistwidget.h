/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFSELECTFILESLISTWIDGET_H
#define QFSELECTFILESLISTWIDGET_H

#include <QWidget>
#include "libwid_imexport.h"
#include "qfwizard.h"

namespace Ui {
    class QFSelectFilesListWidget;
}

class QFWIDLIB_EXPORT QFSelectFilesListWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit QFSelectFilesListWidget(QWidget *parent = 0);
        ~QFSelectFilesListWidget();
        void setFilters(const QStringList& filters);
        void setSettingsIDs(const QString& lastDirID, const QString& lastFilterID);

        int filesCount() const;
        QStringList files() const;
        QStringList fileFilters() const;

    public slots:
        void addFiles();

    protected slots:
        void on_btnAdd_clicked();
        void on_btnRemove_clicked();
        void on_btnUp_clicked();
        void on_btnDown_clicked();
    private:
        Ui::QFSelectFilesListWidget *ui;
        QStringList filters;
        QString lastDirID;
        QString lastFilterID;
};


class QFWIDLIB_EXPORT QFSelectFilesWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFSelectFilesWizardPage(const QString &title, QWidget *parent = 0);
        QFSelectFilesListWidget* getSelFilesWidget() const {
            return sel;
        }

        void setFilters(const QStringList& filters);
        void setSettingsIDs(const QString& lastDirID, const QString& lastFilterID);

        int filesCount() const;
        QStringList files() const;
        QStringList fileFilters() const;
        void setAddOnStartup(bool add);
        void setFilesRequired(bool add);

        virtual void initializePage();
        virtual bool validatePage();
    signals:

    public slots:
    protected:
        QFSelectFilesListWidget *sel;
        bool m_addStartup;
        bool m_needsfiles;

};

#endif // QFSELECTFILESLISTWIDGET_H
