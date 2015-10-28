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

#ifndef NEWTABLEDIALOG_H
#define NEWTABLEDIALOG_H

#include "qfdialog.h"
#include <QLabel>
namespace Ui {
    class NewTableDialog;
}

class NewTableDialog : public QFDialog
{
        Q_OBJECT

    public:
        explicit NewTableDialog(QWidget *parent = 0);
        virtual ~NewTableDialog();
        QString insertText() const;
    protected slots:
        void preview();

    private:
        QLabel* labPreview;
        Ui::NewTableDialog *ui;
};

#endif // NEWTABLEDIALOG_H
