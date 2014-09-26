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

#ifndef QFIMFCCSFITCHI2LANDSCAPEDIALOG_H
#define QFIMFCCSFITCHI2LANDSCAPEDIALOG_H

#include <QDialog>
#include "qfimfccsfitchi2landscapedialog.h"
#include "qfimfccsfitevaluation_item.h"

namespace Ui {
    class QFImFCCSFitChi2LandscapeDialog;
}

class QFImFCCSFitChi2LandscapeDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFImFCCSFitChi2LandscapeDialog(QFImFCCSFitEvaluationItem* item, QWidget *parent = 0);
        ~QFImFCCSFitChi2LandscapeDialog();
    protected slots:
        void updateInputs();
        void on_btnPlot_clicked();
        
    private:
        Ui::QFImFCCSFitChi2LandscapeDialog *ui;
        QFImFCCSFitEvaluationItem* item;
};

#endif // QFIMFCCSFITCHI2LANDSCAPEDIALOG_H
