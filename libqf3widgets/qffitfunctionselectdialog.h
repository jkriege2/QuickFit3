/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFFITFUNCTIONSELECTDIALOG_H
#define QFFITFUNCTIONSELECTDIALOG_H

#include <QDialog>
#include "libwid_imexport.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

namespace Ui {
    class QFFitFunctionSelectDialog;
}

class QFWIDLIB_EXPORT QFFitFunctionSelectDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFFitFunctionSelectDialog(QWidget *parent = 0);
        ~QFFitFunctionSelectDialog();

        QString getSelected() const;
        void init(const QString& filter, const QString& current);
        void init(const QStringList& availableFF, const QString& current);

    protected slots:
        void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
        void on_edtFilter_textChanged( const QString & text );
    private:
        Ui::QFFitFunctionSelectDialog *ui;
        QStandardItemModel* model;
        QSortFilterProxyModel filterModel;
};

#endif // QFFITFUNCTIONSELECTDIALOG_H
