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

#ifndef QFRDRTableRegressionDialog_H
#define QFRDRTableRegressionDialog_H

#include <QDialog>
#include <QVector>
#include "qfrdrtable.h"
#include "qffitfunctionvalueinputtable.h"
#include "qffitfunctionvalueinputdelegate.h"

namespace Ui {
    class QFRDRTableRegressionDialog;
}

class QFRDRTableRegressionDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableRegressionDialog(QFRDRTable* table, int colX, int colY, int colW, QFRDRTable::GraphDataSelection sel, QWidget *parent = 0, bool logX=false, bool logY=false, int resultColumn=-1, int addGraph=-1, int sourcegraph=-1);
        explicit QFRDRTableRegressionDialog(QFRDRTable* table, int plotid, int graphid, QWidget *parent = 0);
        ~QFRDRTableRegressionDialog();

    protected slots:
        void saveResults();
        void on_btnFit_clicked();
        void updateFitStatistics();
        void replotGraph();
        void showHelp();
        void methodChanged(int method);
        void fillFitParams(int method);
    protected:
        void connectSignals(bool connectS=true);
        void readDataFromTable();
    private:
        Ui::QFRDRTableRegressionDialog *ui;
        QFRDRTable* table;
        QVector<double> dataY, dataX, dataW, weights, residualsY, residualsYW;
        QList<QVariant> lastResults, lastResultErrors;
        QVector<double> lastResultD;
        QString resultComment, resultParams;
        QString resultStat;
        int datapoints;
        QString fitresult;
        int colX,  colY,  colW;

        QFRDRTable::GraphDataSelection selection;

        int overwriteGraph;
        int overwritePlot;
        int sourceGraph;

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



        void writeFitProperties(int pid, int gid, int saveToColumn);
        void readFitProperties(int pid, int gid, int *resultColumn=NULL, int *addGraph=NULL);
        void intInit(QFRDRTable *table, int colX, int colY, int colW, QWidget *parent, bool logX, bool logY, int resultColumn, int addGraph, bool propsAlreadySet=false);
};


#endif // QFRDRTableRegressionDialog_H
