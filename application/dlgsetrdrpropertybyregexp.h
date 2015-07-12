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

#ifndef DLGSETRDRPROPERTYBYREGEXP_H
#define DLGSETRDRPROPERTYBYREGEXP_H

#include <QDialog>
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include <QRegExp>
#include <QList>
#include <QPointer>


namespace Ui {
    class DlgSetRDRPropertyByRegExp;
}

class DlgSetRDRPropertyByRegExp : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgSetRDRPropertyByRegExp(QWidget *parent = 0);
        ~DlgSetRDRPropertyByRegExp();
        void setProject(QFProject* project);

        QList<QPointer<QFRawDataRecord> > getSelectedRDRs() const;

        QString getInputString(QFRawDataRecord* rdr) const;
        QString getResult(QFRawDataRecord* rdr, bool *apply=NULL, QString *error=NULL) const;
        QString transformOutput(QFRawDataRecord *rdr, const QString& o) const;
        void applyResult(QFRawDataRecord* rdr);
    protected slots:
        void on_btnSelectNone_clicked();
        void on_btnSelectAll_clicked();
        void showHelp();
        void on_btnToggled_clicked();
        void on_btnDeselSel_clicked();
        void on_btnSelSel_clicked();
    public slots:
        void updateTest();
    private:
        Ui::DlgSetRDRPropertyByRegExp *ui;
        QFProject* project;
        QList<QPointer<QFRawDataRecord> > rdrs;
};

#endif // DLGSETRDRPROPERTYBYREGEXP_H
