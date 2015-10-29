/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFERTRESULTCALCULATOR_H
#define QFERTRESULTCALCULATOR_H

#include "qfdialog.h"
#include "qfrawdatarecord.h"

namespace Ui {
class QFERTResultCalculator;
}

class QFERTResultCalculator : public QFDialog
{
    Q_OBJECT
    
public:
    explicit QFERTResultCalculator(QWidget *parent = 0);
    ~QFERTResultCalculator();

    QString getExpression() const;
    QFRawDataRecord* getRecA() const;
    QFRawDataRecord* getRecB() const;
    QString getEvalIDA() const;
    QString getResultIDA() const;
    QString getEvalIDB() const;
    QString getResultIDB() const;
    QString getNewResultName() const;
public slots:
    void eval();
protected slots:
    void checkBoxesChanged();
    void on_btnHelp_clicked();
    void on_cmbResultA_currentResultChanged(QString result, QString evalID);
    void on_cmbResultB_currentResultChanged(QString result, QString evalID);
private:
    Ui::QFERTResultCalculator *ui;
};

#endif // QFERTRESULTCALCULATOR_H
