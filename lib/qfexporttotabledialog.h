/*
    Copyright (c) 2008-201f Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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


#ifndef QFExportToTableDialog_H
#define QFExportToTableDialog_H

#include "qfdialog.h"
#include "qfrdrtableinterface.h"
#include "qfrawdatarecord.h"
#include "lib_imexport.h"

namespace Ui {
    class QFExportToTableDialog;
}

class QFLIB_EXPORT QFExportToTableDialog : public QFDialog
{
        Q_OBJECT
        
    public:
        explicit QFExportToTableDialog(QWidget *parent = 0);
        ~QFExportToTableDialog();

        QFRDRTableInterface* getTable() const;
        QFRawDataRecord* getRDR() const;
        bool getNewTable(QString& name) const;
        bool getShowEditor() const;
        void setDescription(const QString& text);
        QString getPrefix();
        void addWidget(const QString& label, QWidget* wid);

    protected slots:
        void on_cmbRDR_refilled(bool full);
        void on_chkNewTable_toggled(bool checked);
    private:
        Ui::QFExportToTableDialog *ui;
};

#endif // QFExportToTableDialog_H
