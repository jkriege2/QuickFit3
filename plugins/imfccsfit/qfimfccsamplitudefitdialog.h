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

#ifndef QFImFCCSAmplitudeFitDialog_H
#define QFImFCCSAmplitudeFitDialog_H

#include <QWidget>
#include "qfimfccsfitevaluation_item.h"
#include "qfplotter.h"
#include "qmodernprogresswidget.h"
#include "qffitfunctionvalueinputtable.h"
#include "qffitfunctionvalueinputdelegate.h"

namespace Ui {
    class QFImFCCSAmplitudeFitDialog;
}

class QFImFCCSAmplitudeFitDialog : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSAmplitudeFitDialog(QWidget *parent = 0);
        ~QFImFCCSAmplitudeFitDialog();

        QFRawDataRecord* getACF0() const;
        QFRawDataRecord* getACF1() const;
        QFRawDataRecord* getCCF() const;

        bool calculateRelCCF(QFRawDataRecord *acf0, QFRawDataRecord *acf1, QFRawDataRecord *ccf, double **rel_out0, double **rel_error_out0, double **rel_out1, double **rel_error_out1, int&w, int& h, bool showErrorMessage=false) const;

    protected slots:
        void cmbACF_currentIndexChanged(int index);
        void cmbCCF_currentIndexChanged(int index);
        void replotImages();
        void addResult();
        void on_btnNextCCF_clicked();
        void on_btnStoreDataAll_clicked();
        void on_btnHelp_clicked();
        void on_cmbAmplitudeSource_currentIndexChanged(int index);
        void on_cmbModel_currentIndexChanged(int index);
    protected:
        void closeEvent(QCloseEvent * event);
    private:
        Ui::QFImFCCSAmplitudeFitDialog *ui;
        QFImFCCSMatchRDRFunctor* matchFunctor;
        JKQTPColumnMathImage* plt;

        QFFitFunctionValueInputTable* tableBrightness;
        QMap<QString, QFFitFunctionValueInputTableFitParamData> mapBrightness;
        QFFitFunctionValueInputTable* tableResults;
        QMap<QString, QFFitFunctionValueInputTableFitParamData> mapResults;

        bool nextCCF();
        bool prevCCF();

        bool isACF(const QFRawDataRecord* rec);
        bool isCCF(const QFRawDataRecord* rec);
};

#endif // QFImFCCSAmplitudeFitDialog_H
