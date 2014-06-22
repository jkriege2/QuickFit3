/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFFINDERRONEOUSCORRELATIONDLG_H
#define QFFINDERRONEOUSCORRELATIONDLG_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qfrdrrunselection.h"
#include "qfrdrfcsdatainterface.h"
#include "qftools.h"

namespace Ui {
    class QFFindErroneousCorrelationDlg;
}

class QFFindErroneousCorrelationDlg : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFindErroneousCorrelationDlg(QWidget *parent = 0);
        ~QFFindErroneousCorrelationDlg();
        
        void init(QFRDRRunSelectionsInterface* runselection, QFRDRFCSDataInterface* fcs);

        QList<int> getInitSelection() const;
        QList<int> getMaskedRuns() const;

        bool isPreview() const;
        bool doClearOldMask() const;
    signals:
        void setSelection(QList<int> selection, bool clearOld);
    private:
        Ui::QFFindErroneousCorrelationDlg *ui;

        QFRDRRunSelectionsInterface* runselection;
        QFRDRFCSDataInterface* fcs;
        QList<int> initSelection, mask;

    protected slots:
        void performCalc();
        void on_spinLagMin_valueChanged(int value);
        void on_spinLagMax_valueChanged(int value);
        void on_spinHighQuantile_valueChanged(double value);
        void on_spinLowQuantile_valueChanged(double value);
};

#endif // QFFINDERRONEOUSCORRELATIONDLG_H
