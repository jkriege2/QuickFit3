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

#ifndef SELECTRESOURCEIMAGE_H
#define SELECTRESOURCEIMAGE_H

#include <QDialog>
#include <QDir>
#include <QStandardItemModel>

namespace Ui {
    class SelectResourceImage;
}

class SelectResourceImage : public QDialog
{
        Q_OBJECT

    public:
        explicit SelectResourceImage(const QString& directory=QString(":/"), QWidget *parent = 0, const QString& pathPrefix=QString(""));
        explicit SelectResourceImage(const QStringList& directories, QWidget *parent = 0, const QStringList& pathPrefixes=QStringList(), const QStringList& baseNodeNames=QStringList());
        ~SelectResourceImage();

        QString getSelectFile() const;
    private:
        Ui::SelectResourceImage *ui;
        QStandardItemModel model;
        bool isMulti;
    protected:
        void listDirectory(QDir dir, QStandardItem *parentItem, const QString &pathPrefix=QString(""));
    protected slots:
        void updateView();
        void treeClicked(const QModelIndex& index);
        void listDoubleClicked(const QModelIndex& index);
};

#endif // SELECTRESOURCEIMAGE_H
