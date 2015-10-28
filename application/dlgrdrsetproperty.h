/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef DLGRDRSETPROPERTY_H
#define DLGRDRSETPROPERTY_H

#include "qfdialog.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "programoptions.h"
#include "qftools.h"

namespace Ui {
class dlgRDRSetProperty;
}

class dlgRDRSetProperty : public QFDialog {
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
        void on_btnToggle_clicked();
        void on_btnDeselSel_clicked();
        void on_btnSelSel_clicked();
        void showHelp();
        void on_cmbWhich_currentIndexChanged(int idx);
    private:
        Ui::dlgRDRSetProperty *ui;
        QFProject* project;
        QList<QPointer<QFRawDataRecord> > rdrs;
};

#endif // DLGRDRSETPROPERTY_H
