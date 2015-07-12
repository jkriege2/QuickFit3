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

#ifndef QFFCSMSDEvaluationFitAllMSDDialog_H
#define QFFCSMSDEvaluationFitAllMSDDialog_H

#include <QDialog>
#include "qffcsmsdevaluation_item.h"
#include <QVector>
#include "qfdoubleedit.h"
#include <QCheckBox>
#include "doubledatacutsliders.h"
#include <QGroupBox>
namespace Ui {
    class QFFCSMSDEvaluationFitAllMSDDialog;
}

class QFFCSMSDEvaluationFitAllMSDDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSMSDEvaluationFitAllMSDDialog(QFFCSMSDEvaluationItem* evaluation, QWidget *parent = 0);
        ~QFFCSMSDEvaluationFitAllMSDDialog();

    protected slots:
        void performFit();
        void replotGraph();
        void showHelp();
    protected:
        void connectSignals(bool connectS=true);
    private:
        Ui::QFFCSMSDEvaluationFitAllMSDDialog *ui;
        QFFCSMSDEvaluationItem* evaluation;
        QVector<double> dist, distTau;
        int getRangeMin(const QVector<double> &distTau, DoubleDataCutSliders *sliders);
        int getRangeMax(const QVector<double> &distTau, DoubleDataCutSliders *sliders);
        QList<QGroupBox*> groupBoxs;
        QList<QCheckBox*> fixAlphas, fixDs;
        QList<QFDoubleEdit*> Ds, Ps, Alphas;
        QList<DoubleDataCutSliders*> Ranges;
};

#endif // QFFCSMSDEvaluationFitAllMSDDialog_H
