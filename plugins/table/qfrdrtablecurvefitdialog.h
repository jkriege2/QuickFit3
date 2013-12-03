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
    explicit QFRDRTableCurveFitDialog(QFRDRTable* table, int graph, int plot, QWidget *parent = 0);
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

        void readDataFromTable();

    private:
        Ui::QFRDRTableCurveFitDialog *ui;
        QFRDRTable* table;
        QVector<double> dataY, dataX, dataW, weights, residualsY, residualsYW;
        QList<QVariant> lastResults, lastResultsErrors;
        QVector<double> lastResultD;
        QString resultComment;
        QString resultStat;
        QString resultPars;
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

        void writeFitProperties(int g, int p);
        void readFitProperties(int g, int p, int *resultColumn=NULL, int *addGraph=NULL);
        void intInit(QFRDRTable *table, int colX, int colY, int colW, QWidget *parent, bool logX, bool logY, int resultColumn, int addGraph);
};

#endif // QFRDRTABLECURVEFITDIALOG_H
