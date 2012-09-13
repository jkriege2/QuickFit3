#ifndef QFRDRTABLEHISTOGRAMDIALOG_H
#define QFRDRTABLEHISTOGRAMDIALOG_H

#include <QDialog>
#include "qfproperties.h"
#include <QMap>
#include <QList>
#include <QStringList>

namespace Ui {
    class QFRDRTableHistogramDialog;
}

class QFRDRTableHistogramDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableHistogramDialog(QWidget *parent = 0);
        ~QFRDRTableHistogramDialog();

        void setColumnData(const QMap<int, QList<double> > &data);
        void readProperties(QFProperties* props);
        void setColumnTitles(const QStringList& titles);
        void init();


        QMap<int, QList<double> > getHistograms() const;
        QMap<int, QString> getHeaderNames() const;
    protected slots:
        void updateHistograms();
    private:
        Ui::QFRDRTableHistogramDialog *ui;
        QMap<int, QList<double> > data;
        QFProperties* props;
        QStringList columnTitles;
        QMap<int, QList<double> > histograms;
        QMap<int, QString> headerNames;
        struct colStatistics {
            double min;
            double max;
            double q25;
            double q75;
            double mean;
            double median;
            int values;
        };
        QMap<int, colStatistics> columnStatistics;
};

#endif // QFRDRTABLEHISTOGRAMDIALOG_H
