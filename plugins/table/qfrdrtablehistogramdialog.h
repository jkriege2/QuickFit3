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


#ifndef QFRDRTABLEHISTOGRAMDIALOG_H
#define QFRDRTABLEHISTOGRAMDIALOG_H

#include "qfdialog.h"
#include "qfproperties.h"
#include <QMap>
#include <QList>
#include <QStringList>
#include "qftablepluginmodel.h"

namespace Ui {
    class QFRDRTableHistogramDialog;
}

class QFRDRTableHistogramDialog : public QFDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableHistogramDialog(QWidget *parent = 0);
        ~QFRDRTableHistogramDialog();

        void setTable(QFTablePluginModel* table);
        void setSelectedColumn(int col);
        void init();


        QMap<int, QVector<double> > getHistograms() const;
        QMap<int, QString> getHeaderNames() const;
    protected slots:
        void updateHistograms(bool estimateBinWidth=false);
        void on_edtRangeMax_valueChanged(double value);
        void on_edtRangeMin_valueChanged(double value);
        void on_edtBinWidth_valueChanged(double value);
        void on_spinBins_valueChanged(int value);
        void on_cmbMethod_currentIndexChanged(int current);
        void on_btnAutoBinwidth_clicked();
        void on_btnAutoBandwidth_clicked();
        void on_btnAutoBins_clicked();
        void calcAutoBinwidth();
        void calcAutoBandwidth();
        void calcAutoBins();
    private:
        Ui::QFRDRTableHistogramDialog *ui;

        QMap<int, QVector<double> > histograms;
        QMap<int, QString> headerNames;
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
        //QMap<int, colStatistics> columnStatistics;
        int selected;
        QFTablePluginModel* table;
};

#endif // QFRDRTABLEHISTOGRAMDIALOG_H
