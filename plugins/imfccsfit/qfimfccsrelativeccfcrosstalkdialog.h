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

#ifndef QFImFCCSRelativeCCFCrosstalkDialog_H
#define QFImFCCSRelativeCCFCrosstalkDialog_H

#include <QWidget>
#include "qfimfccsfitevaluation_item.h"
#include "qfplotter.h"
#include "qmodernprogresswidget.h"

namespace Ui {
    class QFImFCCSRelativeCCFCrosstalkDialog;
}

class QFImFCCSRelativeCCFCrosstalkDialog : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSRelativeCCFCrosstalkDialog(QWidget *parent = 0);
        ~QFImFCCSRelativeCCFCrosstalkDialog();

        QFRawDataRecord* getACF0() const;
        QFRawDataRecord* getACF1() const;
        QFRawDataRecord* getCCF() const;

        static bool calculateRelCCF(QFRawDataRecord *acf0, QFRawDataRecord *acf1, QFRawDataRecord *ccf, double **rel_out0, double **rel_error_out0, double **rel_out1, double **rel_error_out1, double** acf0Amplitude, double **acf0UCAmplitude, double** acf1Amplitude, double **acf1UCAmplitude, double **ccfAmplitude, double **ccfUCAmplitude, int&w, int& h, int avgCount, double crosstalk, int crosstalkDir, bool showErrorMessage=false, int source=0, const QString& resultGroupACF0=QString(), const QString& resultGroupACF1=QString(), const QString& resultGroupCCF=QString(), const QString& param="fitparam_g0", bool subtractBackground=true);

    protected slots:
        void cmbACF_currentIndexChanged(int index);
        void cmbCCF_currentIndexChanged(int index);
        void replotImages();
        void addResult();
        void on_btnNextCCF_clicked();
        void on_btnStoreDataAll_clicked();
        void on_btnHelp_clicked();
        void on_cmbAmplitudeSource_currentIndexChanged(int index);
    protected:
        void closeEvent(QCloseEvent * event);
    private:
        Ui::QFImFCCSRelativeCCFCrosstalkDialog *ui;
        QFImFCCSMatchRDRFunctor* matchFunctor;
        JKQTPColumnMathImage* plt;

        bool nextCCF();
        bool prevCCF();

        bool isACF(const QFRawDataRecord* rec);
        bool isCCF(const QFRawDataRecord* rec);
};

#endif // QFImFCCSRelativeCCFCrosstalkDialog_H
