/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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


#ifndef QFSimpleCurveFitDialog_H
#define QFSimpleCurveFitDialog_H

#include <QDialog>
#include <QVector>
#include "qffitfunctionvalueinputtable.h"
#include "qffitfunctionvalueinputdelegate.h"
#include "qffitfunctionplottools.h"

namespace Ui {
    class QFSimpleCurveFitDialog;
}

class QFSimpleCurveFitDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFSimpleCurveFitDialog(const QVector<double>& dataX, const QVector<double>& dataY, const QVector<double>& dataW, QWidget *parent = 0, bool logX=false, bool logY=false);
        ~QFSimpleCurveFitDialog();
        QVector<double> getFitResults() const;
        QString getFitModelID() const;
    protected slots:
        void saveResults();
        void on_btnFit_clicked();
        void on_btnGuess_clicked();
        void replotGraph();
        void showHelp();
        void methodChanged(int method);
        void updateFitStatistics();
    protected:
        void connectSignals(bool connectS=true);

    private:
        Ui::QFSimpleCurveFitDialog *ui;
        QVector<double> dataY, dataX, dataW, weights, residualsY, residualsYW;
        QList<QVariant> lastResults;
        QVector<double> lastResultD;
        QString resultComment;
        QString resultStat;
        int datapoints;
        QString fitresult;


        QFFitFunctionValueInputTable* parameterTable;
        QMap<QString, QFFitFunctionValueInputTableFitParamData> paramMap;
        QStringList parameterNames, parameterLabels;

        int getRangeMin();
        int getRangeMax();
        double getParamValue(const QString& param, double defaultVal=0.0) const;
        double getParamError(const QString& param, double defaultVal=0.0) const;
        bool getParamFix(const QString& param, bool defaultVal=false) const;
        double getParamMin(const QString& param, double defaultVal=-DBL_MAX) const;
        double getParamMax(const QString& param, double defaultVal=DBL_MAX) const;

};

#endif // QFSimpleCurveFitDialog_H
