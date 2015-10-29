/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFRDRTABLESORTDIALOG_H
#define QFRDRTABLESORTDIALOG_H

#include "qfdialog.h"

namespace Ui {
    class QFRDRTableSortDialog;
}

class QFRDRTableSortDialog : public QFDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableSortDialog(QWidget *parent = 0);
        ~QFRDRTableSortDialog();
        
        void setColumns(QList<QPair<int, QString> > cols);
        void setCurrentColumn(int col);
        int sortColumn() const;

        bool sortAscending() const;

    private:
        Ui::QFRDRTableSortDialog *ui;
};

#endif // QFRDRTABLESORTDIALOG_H
