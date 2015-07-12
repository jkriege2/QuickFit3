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

#ifndef QFEACQUISITIONTESTDIALOG_H
#define QFEACQUISITIONTESTDIALOG_H
#include "qfpluginservices.h"
#include <QWidget>

namespace Ui {
    class QFEAcquisitiontestDialog;
}

class QFEAcquisitiontestDialog : public QWidget, public QFPluginLogService
{
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService)
    public:
        explicit QFEAcquisitiontestDialog(QWidget *parent = 0);
        ~QFEAcquisitiontestDialog();
        
        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);
    protected slots:
        void on_btnConnect_clicked(bool pressed);
    private:
        Ui::QFEAcquisitiontestDialog *ui;
};

#endif // QFEACQUISITIONTESTDIALOG_H
