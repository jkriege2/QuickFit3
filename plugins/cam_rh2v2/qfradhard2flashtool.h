/*
Copyright (c) 2008-2015 Jan Buchholz, German Cancer Research Center

    

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

#include "qfdialog.h"
#include "../base_classes/radhard2flash.h"
#include "programoptions.h"


namespace Ui {
    class QFRadhard2FlashtoolV2; // forward
}

class QFExtensionCameraRh2v2; // forward

/*! \brief Flash tool dialog for Radhard2
    \ingroup qf3ext_rh2v2

*/
class QFRadhard2FlashtoolV2 : public QFDialog{
        Q_OBJECT
    public:
        explicit QFRadhard2FlashtoolV2(QFExtensionCameraRh2v2 *ext, QWidget* parent = 0);
        ~QFRadhard2FlashtoolV2();

        void setBitfileMaster(QString bitfile);
        void setAutoBitfileMaster(QString bitfile);
        void setBitfileSlave(QString bitfile);
        void setAutoBitfileSlave(QString bitfile);
        void setAutoFlash(bool v);
        void setRetries(int v);
        void setRetryDelayMS(int milliseconds);

        int retries() const;
        int retryDelayMS() const;
        bool autoflash() const;
        QString bitfileMaster() const;
        QString bitfileSlave() const;
        QString autoBitfileMaster() const;
        QString autoBitfileSlave() const;
    protected slots:
        void on_btnLoad_clicked();
        void on_btnLoad1_clicked();
        void on_btnLoad2_clicked();
        void on_btnLoad3_clicked();
        void on_btnFlash_clicked();
    private:
        Ui::QFRadhard2FlashtoolV2 *ui;
        QFExtensionCameraRh2v2* radhard2extension;
};

#endif // QFRADHARD2FLASHTOOL_H
