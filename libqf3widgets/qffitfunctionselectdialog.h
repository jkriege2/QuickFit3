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


#ifndef QFFITFUNCTIONSELECTDIALOG_H
#define QFFITFUNCTIONSELECTDIALOG_H

#include "qfdialog.h"
#include "libwid_imexport.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "qfsimpletreemodel.h"
#include "qfpseudotreemodel.h"

namespace Ui {
    class QFFitFunctionSelectDialog;
}

class QFWIDLIB_EXPORT QFFitFunctionSelectDialog : public QFDialog
{
        Q_OBJECT

    public:
        explicit QFFitFunctionSelectDialog(QWidget *parent = 0);
        ~QFFitFunctionSelectDialog();

        QString getSelected() const;
        QString getCurrentItem() const;
        void init(const QString& filter, const QString& current);
        void init(const QStringList& availableFF, const QString& current);

    protected slots:
        void currentRowChanged(const QModelIndex &current, const QModelIndex &previous=QModelIndex());
        void currentRowAccepted(const QModelIndex &current, const QModelIndex &previous=QModelIndex());
        void on_edtFilter_textChanged( const QString & text );
    private:
        Ui::QFFitFunctionSelectDialog *ui;
        //QFPseudoTreeModel* model;
        QFSimpleTreeModel* model;
        QFSimpleTreeModelSortFilterProxyModel filterModel;
        //QFPseudoTreeModelSortFilterProxyModel filterModel;

        QString currentFunction;
};

#endif // QFFITFUNCTIONSELECTDIALOG_H
