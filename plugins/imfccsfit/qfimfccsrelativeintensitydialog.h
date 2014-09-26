/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFImFCCSRelativeIntensityDialog_H
#define QFImFCCSRelativeIntensityDialog_H

#include <QWidget>
#include "qfimfccsfitevaluation_item.h"
#include "qfplotter.h"
#include "qmodernprogresswidget.h"

namespace Ui {
    class QFImFCCSRelativeIntensityDialog;
}

class QFImFCCSRelativeIntensityDialog : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSRelativeIntensityDialog(QWidget *parent = 0);
        ~QFImFCCSRelativeIntensityDialog();

        QFRawDataRecord* getCCF() const;

        static bool calculateRelIntensity(QFRawDataRecord* ccf, double** rel, int &w, int &h, bool showErrorMessage=false);

    protected slots:
        void cmbCCF_currentIndexChanged(int index);
        void replotImages();
        void addResult();
        void on_btnNextACF_clicked();
        void on_btnStoreDataAll_clicked();
        void on_btnHelp_clicked();
    private:
        Ui::QFImFCCSRelativeIntensityDialog *ui;
        QFImFCCSMatchRDRFunctor* matchFunctor;
        JKQTPColumnMathImage* plt;

        bool nextACF();

        bool isCCF(const QFRawDataRecord* rec);
};

#endif // QFImFCCSRelativeIntensityDialog_H
