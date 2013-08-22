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
