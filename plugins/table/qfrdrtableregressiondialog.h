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
        explicit QFRDRTableRegressionDialog(QFRDRTable* table, int colX, int colY, int colW, QWidget *parent = 0, bool logX=false, bool logY=false, int resultColumn=-1, int addGraph=-1);
        ~QFRDRTableRegressionDialog();

    protected slots:
        void saveResults();
        void on_btnFit_clicked();
        void updateFitStatistics();
        void replotGraph();
        void showHelp();
        void methodChanged(int method);
    protected:
        void connectSignals(bool connectS=true);
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

#endif // QFRDRTableRegressionDialog_H
