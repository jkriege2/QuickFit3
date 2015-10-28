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


#ifndef QFRDRTABLECURVEFITDIALOG_H
#define QFRDRTABLECURVEFITDIALOG_H

#include "qfdialog.h"
#include <QVector>
#include "qfrdrtable.h"
#include "qffitfunctionvalueinputtable.h"
#include "qffitfunctionvalueinputdelegate.h"
#include "qffitfunctionplottools.h"

namespace Ui {
    class QFRDRTableCurveFitDialog;
}

class QFRDRTableCurveFitDialog : public QFDialog
{
        Q_OBJECT
        
    public:
    explicit QFRDRTableCurveFitDialog(QFRDRTable* table, int colX, int colY, int colW, QFRDRTable::GraphDataSelection sel, QWidget *parent = 0, bool logX=false, bool logY=false, int resultColumn=-1, int addGraph=-1, int sourceGraph=-1);
    explicit QFRDRTableCurveFitDialog(QFRDRTable* table, int plotid, int graphid, QWidget *parent = 0);
        ~QFRDRTableCurveFitDialog();
    protected slots:
        void saveResults();
        void on_btnFit_clicked();
        void on_btnGuess_clicked();
        void on_btnConfigAlgorithm_clicked();
        void replotGraph();
        void showHelp();
        void methodChanged(int method);
        void updateFitStatistics();
        double getWeight(int i) const;
     protected:
        void connectSignals(bool connectS=true);

        void readDataFromTable();

    private:
        Ui::QFRDRTableCurveFitDialog *ui;
        QFRDRTable* table;
        QVector<double> dataY, dataX, dataW, residualsY, residualsYW;
        QList<QVariant> lastResults, lastResultsErrors;
        QVector<double> lastResultD;
        QString resultComment;
        QString resultCommentLong;

        QString resultStat;
        QString resultStatLong;
        QString resultPars;
        QString fitresultStats;
        int datapoints;
        QString fitresult;
        int colX,  colY,  colW;
        int overwriteGraph;
        int overwritePlot;
        int sourceGraph;

        QFRDRTable::GraphDataSelection selection;


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
        void readFitProperties(int pid, int p, int *resultColumn=NULL, int *addGraph=NULL);
        void intInit(QFRDRTable *table, int colX, int colY, int colW, QWidget *parent, bool logX, bool logY, int resultColumn, int addGraph, bool propsAlreadySet=false);
};

#endif // QFRDRTABLECURVEFITDIALOG_H
