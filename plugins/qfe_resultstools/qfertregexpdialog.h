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

#ifndef QFERTREGEXPDIALOG_H
#define QFERTREGEXPDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
    class QFERTRegExpDialog;
}

class QFERTRegExpDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFERTRegExpDialog(QWidget *parent = 0);
        ~QFERTRegExpDialog();

    protected slots:
        void refresh();
        void on_btnHelp_clicked();

    private:
        Ui::QFERTRegExpDialog *ui;

        enum { MaxCaptures = 8 };
        QLabel *captureLabels[MaxCaptures];
        QLineEdit *captureEdits[MaxCaptures];
};

#endif // QFERTREGEXPDIALOG_H
