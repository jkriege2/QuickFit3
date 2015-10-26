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

#ifndef QFFitChi2LandscapeDialog_H
#define QFFitChi2LandscapeDialog_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qffitfunction.h"
#include "qffitresultsbyindexevaluation.h"
namespace Ui {
    class QFFitChi2LandscapeDialog;
}

class QFFitChi2LandscapeDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFitChi2LandscapeDialog(QFFitResultsByIndexEvaluation* item, QFRawDataRecord *rec, int index, QFFitFunction* fitfunction, QWidget *parent = 0);
        ~QFFitChi2LandscapeDialog();
    protected slots:
        void updateInputs();
        void on_btnPlot_clicked();
        
    private:
        Ui::QFFitChi2LandscapeDialog *ui;
        QFFitResultsByIndexEvaluation* item;
        QFRawDataRecord* rec;
        int index;
        QFFitFunction* fitfunction;
};

#endif // QFFitChi2LandscapeDialog_H
