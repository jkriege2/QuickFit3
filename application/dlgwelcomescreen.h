/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-13 16:56:45 +0100 (Fr, 13 Feb 2015) $  (revision $Rev: 3827 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef DLGWELCOMESCREEN_H
#define DLGWELCOMESCREEN_H

#include <QDialog>

namespace Ui {
    class DlgWelcomeScreen;
}

class DlgWelcomeScreen : public QDialog
{
        Q_OBJECT

    public:
        explicit DlgWelcomeScreen(QWidget *parent = 0);
        ~DlgWelcomeScreen();

        enum Screens {
            scFirst,
            scRecent,
            scTutorials
        };

        void addAction(Screens s, QAction* act, int pixmapSize=16, bool closeOnClick=true);
        void addSeparator(Screens s);

        void finalizeLayout();

    protected slots:
        void on_chkShow_toggled(bool enabled);
        void on_btnClose_clicked();

    private:
        Ui::DlgWelcomeScreen *ui;
};

#endif // DLGWELCOMESCREEN_H
