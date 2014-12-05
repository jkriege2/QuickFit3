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
#include "qenhancedlineedit.h"
#include "qfstyledbutton.h"

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
        void setFilters(const QStringList& filters, const QStringList& filterids);
        void setSettingsIDs(const QString& lastDirID, const QString& lastFilterID);

        int filesCount() const;
        QStringList files() const;
        QStringList fileFilters() const;
        QStringList fileFilterIDs() const;
        QList<int> fileFilterNums() const;
        void setOnlyOneFormatAllowed(bool allowed);

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
        QStringList filterids;
        QString lastDirID;
        QString lastFilterID;
        bool onlyOneFormat;
};


class QFWIDLIB_EXPORT QFSelectFilesListWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFSelectFilesListWizardPage(const QString &title, QWidget *parent = 0);
        QFSelectFilesListWidget* getSelFilesWidget() const {
            return sel;
        }
        inline QFormLayout* getFormLayout() const {
            return formLay;
        }

        void addRow(const QString& label, QWidget* widget);
        void addRow(const QString& label, QLayout* layout);
        void setFilters(const QStringList& filters);
        void setFilters(const QStringList& filters, const QStringList& filterids);
        void setSettingsIDs(const QString& lastDirID, const QString& lastFilterID);

        int filesCount() const;
        QStringList files() const;
        QStringList fileFilters() const;
        QStringList fileFilterIDs() const;
        QList<int> fileFilterNums() const;
        void setAddOnStartup(bool add);
        void setFilesRequired(bool add);
        void setOnlyOneFormatAllowed(bool allowed);

        virtual void initializePage();
        virtual bool validatePage();
    signals:

    public slots:
    protected:
        QFSelectFilesListWidget *sel;
        bool m_addStartup;
        bool m_needsfiles;
        QFormLayout* formLay;
};



class QFWIDLIB_EXPORT QFSelectFilesWizardPage : public QFWizardPage
{
        Q_OBJECT
    public:
        explicit QFSelectFilesWizardPage(const QString &title, QWidget *parent = 0);

        inline QFormLayout* getFormLayout() const {
            return formLay;
        }

        void addRow(const QString& label, QWidget* widget);
        void addRow(const QString& label, QLayout* layout);

        void addFileSelection(const QString& label, const QStringList& filters, bool required=true);
        void addFileSelection(const QString& label, const QStringList& filters, const QStringList& filterids, bool required=true);
        void setSettingsIDs(const QString& lastDirID, const QString& lastFilterID);

        int filesCount() const;
        QStringList files() const;
        QStringList fileFilters() const;
        QStringList fileFilterIDs() const;
        QList<int> fileFilterNums() const;
        void setOnlyOneFormatAllowed(bool allowed);

        virtual void initializePage();
        virtual bool validatePage();
    signals:

    public slots:
    protected slots:
        void buttonClicked(QFStyledButton* btn);
    protected:
        struct FileData {
            QStringList filterids;
            QStringList filters;
            QEnhancedLineEdit* edit;
            QLabel* label;
            QFStyledButton* button;
            bool required;
            QString format;
        };

        QList<FileData> m_files;
        QString lastDirID;
        QString lastFilterID;

        QFormLayout* formLay;
        bool onlyOneFormat;
        QString lastFormat;
};

#endif // QFSELECTFILESLISTWIDGET_H
