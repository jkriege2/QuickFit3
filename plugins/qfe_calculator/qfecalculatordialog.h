/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFECALCULATORDIALOG_H
#define QFECALCULATORDIALOG_H

#include <QDialog>
#include "qfmathparser.h"
#include "qftools.h"
#include "qffunctionreferencetool.h"
#include "qfecalculatorplotdialog.h"

class QFECalculator;

namespace Ui {
class QFECalculatorDialog;
}

class QFECalculatorDialog : public QDialog {
        Q_OBJECT

    public:
        explicit QFECalculatorDialog(QFECalculator* calc, QWidget *parent = 0);
        ~QFECalculatorDialog();
        int getPrecision() const;
        QPlainTextEdit* getHistory() const;
        QFECalculatorPlotDialog* getPlot(int i=-1, int *index=NULL);
    protected slots:
        void showHelp();
        void showCache();
        void on_btnEvaluate_clicked();
        void on_btnClearHistory_clicked();
        void on_btnClearCache_clicked();
        void delayedStartSearch();
        void on_edtExpression_textChanged(QString text);
        void on_chkMultiline_toggled(bool enabled);        
    private:
        Ui::QFECalculatorDialog *ui;
        QFMathParser* parser;
        QFECalculator* calc;
        QFFunctionReferenceTool* functionRef;
        QStringList history;
        QMap<int, QPointer<QFECalculatorPlotDialog> > plots;
        void setupParser(QFMathParser* parser) const;
};

#endif // QFECALCULATORDIALOG_H
