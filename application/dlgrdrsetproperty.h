/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef DLGRDRSETPROPERTY_H
#define DLGRDRSETPROPERTY_H

#include <QDialog>
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "programoptions.h"
#include "qftools.h"

namespace Ui {
class dlgRDRSetProperty;
}

class dlgRDRSetProperty : public QDialog {
        Q_OBJECT

    public:
        explicit dlgRDRSetProperty(QWidget *parent = 0);
        ~dlgRDRSetProperty();

        void init(QFProject* project);
        bool doOverwrite() const;
        bool doCreateNew() const;
        int whichToSet() const;

        QList<QPointer<QFRawDataRecord> > getSelectedRDRs() const;
        QStringList getNewPropNames() const;
        QList<QVariant> getNewPropValues() const;

    protected slots:
        void on_btnSelectNone_clicked();
        void on_btnSelectAll_clicked();
        void showHelp();
        void on_cmbWhich_currentIndexChanged(int idx);
    private:
        Ui::dlgRDRSetProperty *ui;
        QFProject* project;
        QList<QPointer<QFRawDataRecord> > rdrs;
};

#endif // DLGRDRSETPROPERTY_H
