/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef DLGPOLOLUMAESTROTESTER_H
#define DLGPOLOLUMAESTROTESTER_H

#include <QDialog>
#include "qfserialconnection.h"
#include "programoptions.h"
#include "qfbinaryserialprotocolhandler.h"

namespace Ui {
    class DlgPololuMaestroTester;
}

class DlgPololuMaestroTester : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgPololuMaestroTester(QWidget *parent = 0);
        ~DlgPololuMaestroTester();
        
    protected slots:
        void updateRead();
        void on_spinSet_valueChanged(int value);
        void on_btnConnect_clicked(bool checked);
        void on_cmbBaudrate_currentIndexChanged(int index);
    private:
        Ui::DlgPololuMaestroTester *ui;
        QFSerialConnection com;
        QFBinarySerialProtocolHandler* serial;
};

#endif // DLGPOLOLUMAESTROTESTER_H
