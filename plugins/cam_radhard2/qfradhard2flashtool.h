/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef QFRADHARD2FLASHTOOL_H
#define QFRADHARD2FLASHTOOL_H

#include <QDialog>
#include "../base_classes/radhard2flash.h"
#include "programoptions.h"

namespace Ui {
    class QFRadhard2Flashtool; // forward
}

class QFExtensionCameraRadhard2; // forward

/*! \brief Flash tool dialog for Radhard2
    \ingroup qf3ext_radhard2

*/
class QFRadhard2Flashtool : public QDialog{
        Q_OBJECT
    public:
        explicit QFRadhard2Flashtool(QFExtensionCameraRadhard2 *ext, QWidget* parent = 0);
        ~QFRadhard2Flashtool();

        void setBitfile(QString bitfile);
        void setAutoBitfile(QString bitfile);
        void setAutoFlash(bool v);
        void setRetries(int v);
        void setRetryDelayMS(int milliseconds);

        int retryDelayMS() const;
        int retries() const;
        bool autoflash() const;
        QString bitfile() const;
        QString autoBitfile() const;
    protected slots:
        void on_btnLoad_clicked();
        void on_btnLoad1_clicked();
        void on_btnFlash_clicked();
    private:
        Ui::QFRadhard2Flashtool *ui;
        QFExtensionCameraRadhard2* radhard2extension;
};

#endif // QFRADHARD2FLASHTOOL_H
