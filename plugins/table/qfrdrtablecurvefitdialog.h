#ifndef QFRDRTABLECURVEFITDIALOG_H
#define QFRDRTABLECURVEFITDIALOG_H

#include <QDialog>
#include <QVector>
#include "qfrdrtable.h"
#include "qffitfunctionvalueinputtable.h"
#include "qffitfunctionvalueinputdelegate.h"
#include "qffitfunctionplottools.h"

namespace Ui {
    class QFRDRTableCurveFitDialog;
}

class QFRDRTableCurveFitDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableCurveFitDialog(QFRDRTable* table, int colX, int colY, int colW, QWidget *parent = 0, bool logX=false, bool logY=false, int resultColumn=-1, int addGraph=-1);
        ~QFRDRTableCurveFitDialog();
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
        Ui::QFRDRTableCurveFitDialog *ui;
        QFRDRTable* table;
        QVector<double> dataY, dataX, dataW, weights, residualsY, residualsYW;
        QList<QVariant> lastResults;
        QVector<double> lastResultD;
        QString resultComment;
        QString resultStat;
        int datapoints;
        QString fitresult;
        int colX,  colY,  colW;


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

#endif // QFRDRTABLECURVEFITDIALOG_H
