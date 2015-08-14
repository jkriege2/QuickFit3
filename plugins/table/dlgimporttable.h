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

#ifndef DLGIMPORTTABLE_H
#define DLGIMPORTTABLE_H

#include <QDialog>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include "qftablepluginmodel.h"

namespace Ui {
    class DlgIMportTable;
}

class DlgIMportTable : public QDialog
{
        Q_OBJECT

    public:
        explicit DlgIMportTable(QWidget *parent = 0);
        explicit DlgIMportTable(bool pastAgainEnabled, QWidget *parent = 0);
        ~DlgIMportTable();

        bool importAll() const;
        bool importExpressions() const;
        void setModel(QFTablePluginModel* model);
        QItemSelectionModel* getSelection() const;
        int importMode() const;

        void doImport(QFTablePluginModel* model_out, int currentRow, int currentCol);

    protected slots:
        void on_radImportAll_toggled(bool checked);
        void selectionCHanged();
        void on_btnPasteAgain_clicked();
        void on_btnPivot_clicked();

    private:
        Ui::DlgIMportTable *ui;
        QFTablePluginModel* model;
};

#endif // DLGIMPORTTABLE_H
