#ifndef QFRDRTABLE2DHISTOGRAMDIALOG_H
#define QFRDRTABLE2DHISTOGRAMDIALOG_H

#include <QDialog>
#include "qfproperties.h"
#include <QMap>
#include <QList>
#include <QStringList>
#include "qftablepluginmodel.h"

namespace Ui {
    class QFRDRTable2DHistogramDialog;
}

class QFRDRTable2DHistogramDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTable2DHistogramDialog(QWidget *parent = 0);
        ~QFRDRTable2DHistogramDialog();

        void setTable(QFTablePluginModel* table);
        void setSelectedColumn(int col, int colY);
        void init();


        QMap<int, QVector<double> > getHistograms() const;
        QMap<int, QVector<QVariant> > getHistograms2() const;
        QMap<int, QString> getHeaderNames() const;
        QMap<int, QMap<int, QVariant> > getExtraData() const;
    protected slots:
        void updateHistograms(bool estimateBinWidth=false);
        void on_edtRangeMax_valueChanged(double value);
        void on_edtRangeMin_valueChanged(double value);
        void on_edtBinWidth_valueChanged(double value);
        void on_spinBins_valueChanged(int value);
        void on_edtRangeMaxY_valueChanged(double value);
        void on_edtRangeMinY_valueChanged(double value);
        void on_edtBinWidthY_valueChanged(double value);
        void on_spinBinsY_valueChanged(int value);
        void on_cmbMethod_currentIndexChanged(int current);
        void on_btnAutoBinwidth_clicked();
        void on_btnAutoBandwidth_clicked();
        void on_btnAutoBins_clicked();
        void on_btnAutoBinwidthY_clicked();
        void on_btnAutoBandwidthY_clicked();
        void on_btnAutoBinsY_clicked();
        void calcAutoBinwidth();
        void calcAutoBandwidth();
        void calcAutoBins();
        void calcAutoBinwidthY();
        void calcAutoBandwidthY();
        void calcAutoBinsY();
    private:
        Ui::QFRDRTable2DHistogramDialog *ui;

        QMap<int, QVector<double> > histograms;
        QMap<int, QVector<QVariant> > histograms2;
        QMap<int, QString> headerNames;
        QMap<int, QMap<int, QVariant> > extraData;

        struct colStatistics {
            double min;
            double max;
            double q25;
            double q75;
            double mean;
            double median;
            double var;
            int values;
        };
        colStatistics stat;
        colStatistics statY;
        //QMap<int, colStatistics> columnStatistics;
        int selectedX;
        int selectedY;
        QFTablePluginModel* table;
};

#endif // QFRDRTABLE2DHISTOGRAMDIALOG_H
